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

/** @file   AliHLTTPCDigitDumpComponent.cxx
    @author Matthias Richter
    @date   
    @brief  Special file writer converting TPC digit input to ASCII. */

// see header file for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

#include <cassert>
#include "AliHLTTPCDigitDumpComponent.h"
#include "AliHLTTPCTransform.h"
#include "AliHLTTPCDigitReader.h"
#include "AliHLTTPCDigitReaderUnpacked.h"
#include "AliHLTTPCDigitReaderPacked.h"
#include "AliHLTTPCDigitReaderRaw.h"
#include "AliHLTTPCDefinitions.h"

#define DefaultRawreaderMode 0

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCDigitDumpComponent)

AliHLTTPCDigitDumpComponent::AliHLTTPCDigitDumpComponent()
  :
  AliHLTFileWriter(),
  fRawreaderMode(DefaultRawreaderMode),
  fDigitReaderType(kDigitReaderRaw),
  fRcuTrailerSize(2),
  fUnsorted(false)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCDigitDumpComponent::~AliHLTTPCDigitDumpComponent()
{
  // see header file for class documentation
}

const char* AliHLTTPCDigitDumpComponent::GetComponentID()
{
  // see header file for class documentation
  return "TPCDigitDump";
}

void AliHLTTPCDigitDumpComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  // see header file for class documentation
  list.clear();
  list.push_back(kAliHLTAnyDataType);
}

AliHLTComponent* AliHLTTPCDigitDumpComponent::Spawn()
{
  // see header file for class documentation
  return new AliHLTTPCDigitDumpComponent;
}

int AliHLTTPCDigitDumpComponent::InitWriter()
{
  // see header file for class documentation
  return 0;
}

int AliHLTTPCDigitDumpComponent::ScanArgument(int argc, const char** argv)
{
  // see header file for class documentation
  int iResult=0;
  TString argument="";
  bool bMissingParam=0;
  int i=0;
  do {
    if (i>=argc || (argument=argv[i]).IsNull()) continue;

    // -rawreadermode
    if (argument.CompareTo("-rawreadermode")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      int mode=AliHLTTPCDigitReaderRaw::DecodeMode(argv[i]);
      if (mode<0) {
	HLTError("invalid rawreadermode specifier '%s'", argv[i]);
	iResult=-EINVAL;
      } else {
	fRawreaderMode=static_cast<unsigned>(mode);
      }
      break;
    }

    // -digitreader
    if (argument.CompareTo("-digitreader")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString param=argv[i];
      if (param.CompareTo("unpacked", TString::kIgnoreCase)==0) {
	fDigitReaderType=kDigitReaderUnpacked;
      } else if (param.CompareTo("packed", TString::kIgnoreCase)==0) {
	fDigitReaderType=kDigitReaderPacked;
      } else if (param.CompareTo("raw", TString::kIgnoreCase)==0) {
	fDigitReaderType=kDigitReaderRaw;
      } else {
	HLTError("unknown digit reader type %s", param.Data());
	iResult=-EINVAL;
      }

      if (fDigitReaderType!=kDigitReaderRaw && fRawreaderMode!=DefaultRawreaderMode && iResult>=0) {
	HLTWarning("the selected digit reader does not support the option \'-rawreadermode\'");
      }

      break;
    }

    // -rcutrailersize
    if (argument.CompareTo("-rcutrailersize")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      char *endptr=NULL;
      fRcuTrailerSize=strtoul(argv[i], &endptr, 0);
      if (/*endptr ||*/ fRcuTrailerSize<1) {
	HLTError("invalid parameter '%s', %s", argv[i], endptr==NULL?"number >= 1 expected":"can not convert string to number");
	iResult=-EINVAL;
      }
      break;
    }

    // -unsorted
    if (argument.CompareTo("-unsorted")==0) {
      fUnsorted=true;
      break;
    }

    // -sorted
    if (argument.CompareTo("-sorted")==0) {
      fUnsorted=false;
      break;
    }
  } while (0); // just use the do/while here to have the option of breaking

  if (bMissingParam) iResult=-EPROTO;
  else if (iResult>=0) iResult=i;

  return iResult;
}

int AliHLTTPCDigitDumpComponent::CloseWriter()
{
  // see header file for class documentation
  return 0;
}

int AliHLTTPCDigitDumpComponent::DumpEvent( const AliHLTComponentEventData& evtData,
					    const AliHLTComponentBlockData* blocks, 
					    AliHLTComponentTriggerData& /*trigData*/ )
{
  // see header file for class documentation
  int iResult=0;
  int iPrintedSlice=-1;
  int iPrintedPart=-1;
  int blockno=0;
  const AliHLTComponentBlockData* pDesc=NULL;

  for (pDesc=GetFirstInputBlock(kAliHLTAnyDataType); pDesc!=NULL; pDesc=GetNextInputBlock(), blockno++) {
    HLTDebug("event %Lu block %d: %s 0x%08x size %d", evtData.fEventID, blockno, DataType2Text(pDesc->fDataType).c_str(), pDesc->fSpecification, pDesc->fSize);

    if (fDigitReaderType==kDigitReaderUnpacked && pDesc->fDataType!=AliHLTTPCDefinitions::fgkUnpackedRawDataType) continue;
    else if (fDigitReaderType!=kDigitReaderUnpacked && pDesc->fDataType!=(kAliHLTDataTypeDDLRaw|kAliHLTDataOriginTPC)) continue;

    TString filename;
    iResult=BuildFileName(evtData.fEventID, blockno, pDesc->fDataType, pDesc->fSpecification, filename);
    ios::openmode filemode=(ios::openmode)0;
    if (fCurrentFileName.CompareTo(filename)==0) {
      // append to the file
      filemode=ios::app;
    } else {
      // store the file for the next block
      fCurrentFileName=filename;
    }
    if (iResult>=0) {
      ofstream dump(filename.Data(), filemode);
      if (dump.good()) {
	int part=AliHLTTPCDefinitions::GetMinPatchNr(*pDesc);
	assert(part==AliHLTTPCDefinitions::GetMaxPatchNr(*pDesc));
	int slice=AliHLTTPCDefinitions::GetMinSliceNr(*pDesc);
	assert(slice==AliHLTTPCDefinitions::GetMaxSliceNr(*pDesc));
	int firstRow=AliHLTTPCTransform::GetFirstRow(part);
	int lastRow=AliHLTTPCTransform::GetLastRow(part);
	AliHLTTPCDigitReader* pReader=NULL;
	switch (fDigitReaderType) {
	case kDigitReaderUnpacked:
	  HLTInfo("create DigitReaderUnpacked");
	  pReader=new AliHLTTPCDigitReaderUnpacked; 
	  break;
	case kDigitReaderPacked:
	  HLTInfo("create DigitReaderPacked");
	  pReader=new AliHLTTPCDigitReaderPacked; 
	  if (pReader && fRcuTrailerSize==1) {
	    pReader->SetOldRCUFormat(true);
	  }
	  break;
	case kDigitReaderRaw:
	  HLTInfo("create DigitReaderRaw");
	  pReader=new AliHLTTPCDigitReaderRaw(fRawreaderMode);
	  break;
	}
	if (!pReader) {
	  HLTError("can not create digit reader of type %d", fDigitReaderType);
	  iResult=-EFAULT;
	  break;
	}
	pReader->SetUnsorted(fUnsorted);
	iResult=pReader->InitBlock(pDesc->fPtr,pDesc->fSize,firstRow,lastRow,part,slice);

	int iPrintedRow=-1;
	int iPrintedPad=-1;
	int iLastTime=-1;
	while (pReader->Next()) {
	  if ((iPrintedSlice!=-1 && iLastTime!=pReader->GetTime()+1 && iLastTime!=pReader->GetTime()-1) ||
	      (iPrintedPad!=-1 && iPrintedPad!=pReader->GetPad()) ||
	      (iPrintedRow!=-1 && iPrintedRow!=pReader->GetRow())) {
	    dump << endl;
	  }
	  if (iPrintedSlice!=slice || iPrintedPart!=part) {
	    iPrintedSlice=slice;
	    iPrintedPart=part;
	    dump << "====================================================================" << endl;
	    dump << "    Slice: " << iPrintedSlice << "   Partition: " << iPrintedPart << endl;
	    iPrintedRow=-1;
	  }
	  if (iPrintedRow!=pReader->GetRow()) {
	    iPrintedRow=pReader->GetRow();
	    dump << "--------------------------------------------------------------------" << endl;
	    dump << "Row: " << iPrintedRow << endl;
	    iPrintedPad=-1;
	  }
	  if (iPrintedPad!=pReader->GetPad()) {
	    iPrintedPad=pReader->GetPad();
	    dump << "Row: " << iPrintedRow << "  Pad: " << iPrintedPad << "  HW address: " << pReader->GetAltroBlockHWaddr() << endl;
	    iLastTime=-1;
	  }
	  if (iLastTime!=pReader->GetTime()+1 && iLastTime!=pReader->GetTime()-1 ) {
	    dump << "                     Time " << pReader->GetTime() << ":  ";
	  }
	  iLastTime=pReader->GetTime();
	  dump << "  " << pReader->GetSignal();
	}
	dump << endl << endl;
	delete pReader;
	pReader=NULL;
      } else {
	HLTError("can not open file %s for writing", filename.Data());
	iResult=-EBADF;
      }
      dump.close();
    }
  }
  return iResult;
}
