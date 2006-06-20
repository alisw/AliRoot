/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


///////////////////////////////////////////////////////////////////////////////
///
/// Class Payload
///
/// Decodes rawdata from buffer and stores in TClonesArray.
/// 
/// First version implement for Tracker
///
///////////////////////////////////////////////////////////////////////////////

#include "AliMUONPayloadTracker.h"

#include "AliRawReader.h"
#include "AliRawDataHeader.h"
#include "AliLog.h"

#include "AliMUONDspHeader.h"
#include "AliMUONBlockHeader.h"
#include "AliMUONBusStruct.h"
#include "AliMUONDDLTracker.h"

ClassImp(AliMUONPayloadTracker)

AliMUONPayloadTracker::AliMUONPayloadTracker()
  : TObject(),
    fMaxBlock(2),
    fMaxDsp(5),
    fMaxBus(5)
{
  //
  // create an object to decode MUON payload
  //

  fDDLTracker      = new AliMUONDDLTracker();
  fBusStruct       = new AliMUONBusStruct();
  fBlockHeader     = new AliMUONBlockHeader();
  fDspHeader       = new AliMUONDspHeader();
}

//_________________________________________________________________
AliMUONPayloadTracker::AliMUONPayloadTracker(const AliMUONPayloadTracker& stream) :
  TObject(stream)
{ 
  //
  // copy ctor
  //
  AliFatal("copy constructor not implemented");
}

//______________________________________________________________________
AliMUONPayloadTracker& AliMUONPayloadTracker::operator = (const AliMUONPayloadTracker& 
					      /* stream */)
{
  // 
  // assignment operator
  //
  AliFatal("assignment operator not implemented");
  return *this;
}


//___________________________________
AliMUONPayloadTracker::~AliMUONPayloadTracker()
{
  //
  // clean up
  //
  delete fDDLTracker;
  delete fBusStruct;
  delete fBlockHeader;
  delete fDspHeader;
}

//______________________________________________________
Bool_t AliMUONPayloadTracker::Decode(UInt_t* buffer, Int_t totalDDLSize)
{

  // Each DDL is made with 2 Blocks each of which consists of 5 DSP's at most 
  // and each of DSP has at most 5 buspatches.
  // The different structures, Block (CRT), DSP (FRT) and Buspatch,
  // are identified by a key word 0xFC0000FC, 0xF000000F and 0xB000000B respectively.
  // (fBusPatchManager no more needed !)


  //Read Header Size of DDL,Block,DSP and BusPatch
  static Int_t kBlockHeaderSize    = fBlockHeader->GetHeaderLength();
  static Int_t kDspHeaderSize      = fDspHeader->GetHeaderLength();
  static Int_t kBusPatchHeaderSize = fBusStruct->GetHeaderLength();

  // size structures
  Int_t totalBlockSize;
  Int_t totalDspSize;
  Int_t totalBusPatchSize;
  Int_t dataSize; 
  Int_t bufSize;

  // indexes
  Int_t indexBlk;
  Int_t indexDsp;
  Int_t indexBusPatch;
  Int_t index = 0;
  Int_t iBlock = 0;

  // CROCUS CRT
  while (buffer[index] == fBlockHeader->GetDefaultDataKey()) {

    if (iBlock > fMaxBlock) break;
     
    // copy within padding words
    memcpy(fBlockHeader->GetHeader(),&buffer[index], (kBlockHeaderSize)*4);

    totalBlockSize = fBlockHeader->GetTotalLength();

    indexBlk = index;
    index += kBlockHeaderSize;

    // copy in TClonesArray
    fDDLTracker->AddBlkHeader(*fBlockHeader);

    // Crocus FRT
    Int_t iDsp = 0;
    while (buffer[index] == fDspHeader->GetDefaultDataKey()) {

      if (iDsp > fMaxDsp) break; // if ever...
     		
      memcpy(fDspHeader->GetHeader(),&buffer[index], kDspHeaderSize*4);

      totalDspSize = fDspHeader->GetTotalLength();
      indexDsp = index;
      index += kDspHeaderSize;

//       if (fDspHeader->GetPaddingWord() != fDspHeader->GetDefaultPaddingWord()) {
// 	// copy the field of Padding word into ErrorWord field
// 	fDspHeader->SetErrorWord(fDspHeader->GetPaddingWord());
// 	index--;
//       }

      // copy in TClonesArray
      fDDLTracker->AddDspHeader(*fDspHeader, iBlock);

      // buspatch structure
      Int_t iBusPatch = 0;
      while (buffer[index] == fBusStruct->GetDefaultDataKey()) {

	if (iBusPatch > fMaxBus) break; // if ever
	
	//copy buffer into header structure
	memcpy(fBusStruct->GetHeader(), &buffer[index], kBusPatchHeaderSize*4);

	totalBusPatchSize = fBusStruct->GetTotalLength();
	indexBusPatch     = index;
		
	//Check Buspatch header, not empty events
	if(totalBusPatchSize > kBusPatchHeaderSize) {    

	  index   += kBusPatchHeaderSize;
	  dataSize = fBusStruct->GetLength();
	  bufSize  = fBusStruct->GetBufSize();

	  if(dataSize > 0) { // check data present
	    if (dataSize > bufSize) // check buffer size
	      fBusStruct->SetAlloc(dataSize);
	 
	    //copy buffer into data structure
	    memcpy(fBusStruct->GetData(), &buffer[index], dataSize*4);
	    fBusStruct->SetBlockId(iBlock); // could be usefull in future applications ?
	    fBusStruct->SetDspId(iDsp);

	    // copy in TClonesArray
	    fDDLTracker->AddBusPatch(*fBusStruct, iBlock, iDsp);

	  } // dataSize test

	} // testing buspatch

	index = indexBusPatch + totalBusPatchSize;
	if (index >= totalDDLSize) {// check the end of DDL
	  index = totalDDLSize - 1; // point to the last element of buffer
	  break;
	}
	iBusPatch++;
      }  // buspatch loop

      // skipping additionnal word if padding
      if (fDspHeader->GetPaddingWord() == 1) {
	if (buffer[index++] != fDspHeader->GetDefaultPaddingWord())
	  AliWarning(Form("Error in padding word for iBlock %d, iDsp %d, iBus %d\n", 
			  iBlock, iDsp, iBusPatch));
      }

      index = indexDsp + totalDspSize;
      if (index >= totalDDLSize) {
	index = totalDDLSize - 1;
	break;
      }
      iDsp++;
    }  // dsp loop

    index = indexBlk + totalBlockSize;
    if (index >= totalDDLSize) {
      index = totalDDLSize - 1;
      break;
    }
    iBlock++;
  }  // block loop

  return kTRUE;
}

//______________________________________________________
void AliMUONPayloadTracker::ResetDDL()
{
  // reseting TClonesArray
  // after each DDL
  //
  fDDLTracker->GetBlkHeaderArray()->Delete();
}

//______________________________________________________
void AliMUONPayloadTracker::SetMaxBlock(Int_t blk) 
{
  // set regional card number
  if (blk > 2) blk = 2;
  fMaxBlock = blk;
}

Bool_t AliMUONPayloadTracker::CheckDataParity()
{
  // parity check
  // taken from MuTrkBusPatch.cxx (sotfware test for CROCUS)
  // A. Baldisseri

  Int_t  parity, bit;
  UInt_t data;
  
  Int_t dataSize = fBusStruct->GetLength();
  for (int idata = 0; idata < dataSize; idata++) {

    data  = fBusStruct->GetData(idata);
    // Compute the parity for each data word
    parity = data & 0x1;

    for (Int_t i = 1; i <= 30; i++) {
      bit = ((data >> i) & 0x1);
      parity = (parity || bit) && (!(parity && bit));
    } 

    // Check
    if (parity != fBusStruct->GetParity(idata)) {
      AliWarning(Form("Parity error in word %d for manuId %d and channel %d\n", 
		      idata, fBusStruct->GetManuId(idata), fBusStruct->GetChannelId(idata)));
      return kFALSE;
		     
    }
  }
  return kTRUE;
}
