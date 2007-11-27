/**************************************************************************
 * Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Authors: Boris Polichtchouk & Per Thomas Hille for the ALICE           *
 * offline/HLT Project. Contributors are mentioned in the code where      *
 * appropriate.                                                           *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliHLTPHOSRcuCellEnergyDataStruct.h"
#include "AliHLTPHOSRcuHistogramProducer.h"
#include "AliHLTPHOSRcuHistogramProducerComponent.h"
#include "AliHLTPHOSRcuCellAccumulatedEnergyDataStruct.h"
#include "AliHLTPHOSSharedMemoryInterface.h"
#include "AliHLTPHOSValidCellDataStruct.h" 

AliHLTPHOSRcuHistogramProducerComponent gAliHLTPHOSRcuHistogramProducerComponent;
/*************************************************************************
* Class AliHLTPHOSRcuHistogramProducerComponent accumulating histograms  *
* with amplitudes per PHOS channel                                       *
* It is intended to run at the HLT farm                                  *
* and it fills the histograms with amplitudes per channel.               * 
* Usage example see in PHOS/macros/Shuttle/AliPHOSCalibHistoProducer.C   *
**************************************************************************/
AliHLTPHOSRcuHistogramProducerComponent:: AliHLTPHOSRcuHistogramProducerComponent() :
  AliHLTPHOSRcuProcessor(), fHistoWriteFrequency(100), fRcuHistoProducerPtr(0), fOutPtr(NULL)
{
  fShmPtr = new AliHLTPHOSSharedMemoryInterface();
 //Default constructor
} 


AliHLTPHOSRcuHistogramProducerComponent::~ AliHLTPHOSRcuHistogramProducerComponent()
{
  //Destructor
}


int 
AliHLTPHOSRcuHistogramProducerComponent::Deinit()
{
  //See html documentation of base class
  cout << "AliHLTPHOSRcuHistogramProducerComponent::Deinit()" << endl;
  fRcuHistoProducerPtr->WriteAllHistograms("recreate");
  return 0;
}


const char* 
AliHLTPHOSRcuHistogramProducerComponent::GetComponentID()
{
  //See html documentation of base class
  return "RcuHistogramProducer";
}


void
AliHLTPHOSRcuHistogramProducerComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  //See html documentation of base class
  const AliHLTComponentDataType* pType=fgkInputDataTypes;
  while (pType->fID!=0) 
    {
      list.push_back(*pType);
      pType++;
    }
}


AliHLTComponentDataType 
AliHLTPHOSRcuHistogramProducerComponent::GetOutputDataType()
{
  //See html documentation of base class  
  return AliHLTPHOSDefinitions::fgkCellEnergyDataType;
}


void
AliHLTPHOSRcuHistogramProducerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier )
{
  //See html documentation of base class
  constBase = 30;
  inputMultiplier = 1;
}



int  AliHLTPHOSRcuHistogramProducerComponent::DoEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
					      AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
					      AliHLTUInt32_t& size, vector<AliHLTComponentBlockData>& outputBlocks )
{
  //See html documentation of base class
  AliHLTPHOSValidCellDataStruct *currentChannel =0;
  unsigned long ndx       = 0;
  UInt_t offset           = 0; 
  UInt_t mysize           = 0;
  UInt_t tSize            = 0;
  const AliHLTComponentBlockData* iter = NULL;   
  AliHLTPHOSRcuCellEnergyDataStruct *cellDataPtr;
  AliHLTUInt8_t* outBPtr;

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      if(iter->fDataType != AliHLTPHOSDefinitions::fgkCellEnergyDataType)
	{
	  continue;
	}
      
      cellDataPtr = (AliHLTPHOSRcuCellEnergyDataStruct*)( iter->fPtr);
      fShmPtr->SetMemory(cellDataPtr);
      currentChannel = fShmPtr->NextChannel();

      while(currentChannel != 0)
	{
	  fRcuHistoProducerPtr->FillEnergy(currentChannel->fX, currentChannel->fZ, currentChannel->fGain, currentChannel->fEnergy);
	  fRcuHistoProducerPtr->FillLiveChannels(currentChannel->fData, fNTotalSamples, currentChannel->fX, currentChannel->fZ,currentChannel->fGain);
	  currentChannel = fShmPtr->NextChannel();
	}
    }

  outBPtr = outputPtr;
  fOutPtr =  (AliHLTPHOSRcuCellAccumulatedEnergyDataStruct*)outBPtr;
  const AliHLTPHOSRcuCellAccumulatedEnergyDataStruct  &innPtr = fRcuHistoProducerPtr->GetCellAccumulatedEnergies();
  fOutPtr->fModuleID = fModuleID;
  fOutPtr->fRcuX     = fRcuX;
  fOutPtr->fRcuZ     = fRcuZ;

  for(unsigned int x=0; x < N_XCOLUMNS_RCU; x ++)
    {
      for(unsigned int z=0; z < N_ZROWS_RCU; z ++)
	{
	  for(unsigned int gain =0;  gain < N_GAINS; gain ++)
	    {
	      fOutPtr->fAccumulatedEnergies[x][z][gain] = innPtr.fAccumulatedEnergies[x][z][gain];
	      fOutPtr->fHits[x][z][gain] = innPtr.fHits[x][z][gain];
	      fOutPtr->fDeadChannelMap[x][z][gain] = innPtr.fDeadChannelMap[x][z][gain];
	    }
	}
    }

  mysize += sizeof(AliHLTPHOSRcuCellAccumulatedEnergyDataStruct);
  AliHLTComponentBlockData bd;
  FillBlockData( bd );
  bd.fOffset = offset;
  bd.fSize = mysize;
  bd.fDataType = AliHLTPHOSDefinitions::fgkCellAccumulatedEnergyDataType;
  bd.fSpecification = 0xFFFFFFFF;
  outputBlocks.push_back( bd );
  tSize += mysize;
  outBPtr += mysize;

  if( tSize > size )
    {
      Logging( kHLTLogFatal, "HLT::AliHLTRcuHistogramProducerComponent::DoEvent", "Too much data",
	       "Data written over allowed buffer. Amount written: %lu, allowed amount: %lu."
	       , tSize, size );
      return EMSGSIZE;
    }

  fPhosEventCount++; 

  if( (fPhosEventCount%fHistoWriteFrequency == 0) &&  ( fPhosEventCount != 0))
    {
      fRcuHistoProducerPtr->WriteAllHistograms("recreate");
    }
  return 0;
}//end DoEvent


int
AliHLTPHOSRcuHistogramProducerComponent::DoInit( int argc, const char** argv )
{
  //See html documentation of base class
  fPrintInfo = kFALSE;
  int iResult=0;
  TString argument="";
  iResult = ScanArguments(argc, argv);
  if(fIsSetEquippmentID == kFALSE)
    {
      Logging( kHLTLogFatal, "HLT::AliHLTPHOSRcuHistogramProducerComponent::DoInt( int argc, const char** argv )", "Missing argument",
	       "The argument equippmentID is not set: set it with a component argumet like this: -equippmentID  <number>");
      iResult = -2; 
    }
  fRcuHistoProducerPtr = new AliHLTPHOSRcuHistogramProducer( fModuleID, fRcuX, fRcuZ);
  return iResult; 
  
}


AliHLTComponent*
AliHLTPHOSRcuHistogramProducerComponent::Spawn()
{
  //See html documentation of base class
  return new AliHLTPHOSRcuHistogramProducerComponent;
}


  
