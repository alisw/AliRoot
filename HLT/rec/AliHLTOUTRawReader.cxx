// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTOUTRawReader.cxx
    @author Matthias Richter
    @date   
    @brief  HLTOUT data wrapper for AliRawReader.                         */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include "AliHLTOUTRawReader.h"
#include "AliHLTHOMERLibManager.h"
#include "AliRawReader.h"
#include "AliHLTHOMERReader.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTOUTRawReader)

AliHLTOUTRawReader::AliHLTOUTRawReader(AliRawReader* pRawreader)
  :
  AliHLTOUTHomerBuffer(NULL, 0),
  fpRawreader(pRawreader),
  fpCurrent(NULL)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

const int AliHLTOUTRawReader::fgkIdShift=16;

AliHLTOUTRawReader::~AliHLTOUTRawReader()
{
  // see header file for class documentation
  if (fpManager) {
    if (fpCurrent) fpManager->DeleteReader(fpCurrent);
    fpCurrent=NULL;
  }
}

int AliHLTOUTRawReader::GenerateIndex()
{
  // see header file for class documentation
  // step through all HLT ddls, create HOMER readers and
  // scan data block
  int iResult=0;
  if (fpRawreader && fpManager) {
    fpRawreader->Reset();
    // there was a bug in AliDAQ returning the wrong equipment id
    // for the HLT links. It has been fixed in the trunk on Feb 5th 2008
    // and from v4-10-Release (Rev-02). For the moment we select directly
    // to support older AliRoot versions
    //fpRawreader->Select("HLT");
    fpRawreader->SelectEquipment(0,7680, 7689);
    UChar_t* pSrc=NULL;
    while (fpRawreader->ReadNextData(pSrc) && pSrc!=NULL && iResult>=0) {
      AliHLTUInt32_t id=(fpRawreader->GetEquipmentId());
      unsigned int size=fpRawreader->GetDataSize();

      AliHLTHOMERReader* pReader=OpenReader(pSrc, size);

      // we use the equipment id to identify the different homer blocks 
      id<<=fgkIdShift;
      if (pReader) {
	iResult=ScanReader(pReader, id);
	fpManager->DeleteReader(pReader);
      }
    }
  } else {
    iResult=-ENODEV;
  }
  return iResult;
}

int AliHLTOUTRawReader::GetDataBuffer(AliHLTUInt32_t index, const AliHLTUInt8_t* &pBuffer, 
				      AliHLTUInt32_t& size)
{
  // see header file for class documentation
  int iResult=0;
  pBuffer=NULL;
  size=0;
  if (fpManager) {
    Int_t id = Int_t(index>>fgkIdShift);
    AliHLTUInt32_t blockNo=index&((0x1<<fgkIdShift)-1);

    // block from the same ddl requested?
    if (fpCurrent && fpRawreader->GetEquipmentId()!=id) {
      fpManager->DeleteReader(fpCurrent);
      fpCurrent=NULL;
    }

    // open ddl for equipment id and create HOMER reader
    if (!fpCurrent) {
      fpRawreader->Reset();
      fpRawreader->SelectEquipment(-1, id, id);
      UChar_t* pSrc=NULL;
      if (fpRawreader->ReadNextData(pSrc) && pSrc!=NULL) {
	int srcSize=fpRawreader->GetDataSize();
	fpCurrent=OpenReader(pSrc, srcSize);
	if (fpCurrent && fpCurrent->ReadNextEvent()!=0) {
	  iResult=-ENODATA;
	}
      } else {
	iResult=-ENOSYS;
      }
    }

    // get data
    if (fpCurrent) {
      AliHLTMonitoringReader* pReader=fpCurrent;
      if ((pBuffer=static_cast<const AliHLTUInt8_t*>(pReader->GetBlockData(blockNo)))!=NULL) {
	size=pReader->GetBlockDataLength(blockNo);
      } else {
	iResult=-ENOENT;
      }
    }
  } else {
    iResult=-ENODEV;
  }
  return iResult;
}

AliHLTHOMERReader* AliHLTOUTRawReader::OpenReader(UChar_t* pSrc, unsigned int size)
{
  // see header file for class documentation
  unsigned int offset=sizeof(AliHLTOUTEventHeader);
  const AliRawDataHeader* pCDH=fpRawreader->GetDataHeader();
  AliHLTUInt32_t id=(fpRawreader->GetEquipmentId());
  AliHLTUInt32_t statusFlags=pCDH->GetStatus();
  AliHLTOUTEventHeader* pHLTHeader=reinterpret_cast<AliHLTOUTEventHeader*>(pSrc);

  // consistency check for the block size
  if (pHLTHeader->fLength!=size) {
    HLTWarning("can not treat HLT data block %d: size missmatch, header %d, but buffer is %d", id, pHLTHeader->fLength, size);
    return NULL;
  }

  // determine the offset of the homer block
  // the HLT header is mandatory, HLT decision and HLT
  // payload are optional. HLT decision is always before HLT
  // payload if existent.
  if (statusFlags&(0x1<<kCDHFlagsHLTDecision)) {
    // the block contains HLT decision data, this is just
    // skipped here
    AliHLTUInt32_t* pDecisionLen=reinterpret_cast<AliHLTUInt32_t*>(pSrc+offset);
    if ((*pDecisionLen)*sizeof(AliHLTUInt32_t)+offset<size) {
      // the first 32bit word specifies the number of 32bit words in the
      // decision block -> +1 for this length word
      offset+=((*pDecisionLen)+1)*sizeof(AliHLTUInt32_t);
    } else {
      HLTWarning("size missmatch: HLT decision block bigger than total block length, skipping ...");
      return NULL;
    }
  }

  // check if there is payload
  if (!(statusFlags&(0x1<<kCDHFlagsHLTPayload))) return NULL;

  // continue if there is no data left in the buffer
  if (offset>=size) {
    HLTWarning("no HLT payload available, but bit is set, skipping ...");
    return NULL;
  }

  // check for the HOME descriptor type id
  AliHLTUInt64_t* pHomerDesc=reinterpret_cast<AliHLTUInt64_t*>(pSrc+offset);
  if (*(pHomerDesc+kID_64b_Offset) != HOMER_BLOCK_DESCRIPTOR_TYPEID && 
      Swap(*(pHomerDesc+kID_64b_Offset)) != HOMER_BLOCK_DESCRIPTOR_TYPEID) {
    HLTWarning("format error: can not find HOMER block descriptor typid, skipping this data block");
    return NULL;
  }

  return fpManager->OpenReaderBuffer(pSrc+offset, size-offset);
}
