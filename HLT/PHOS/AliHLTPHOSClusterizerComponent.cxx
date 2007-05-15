/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Øystein Djuvsland <oysteind@ift.uib.no>                       *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/


#include "AliHLTPHOSClusterizerComponent.h"
#include "AliHLTPHOSClusterizer.h"
#include "AliHLTPHOSPhysicsDefinitions.h"
#include "AliHLTPHOSDefinitions.h"
#include "AliHLTPHOSRecPointDataStruct.h"
#include "AliHLTPHOSClusterDataStruct.h"
#include "AliHLTPHOSRecPointListDataStruct.h"

using namespace std;


const AliHLTComponentDataType AliHLTPHOSClusterizerComponent::fgkInputDataTypes[]={kAliHLTVoidDataType,{0,"",""}};

AliHLTPHOSClusterizerComponent gAliHLTPHOSClusterizerComponent;

AliHLTPHOSClusterizerComponent::AliHLTPHOSClusterizerComponent():AliHLTProcessor(), fClusterizerPtr(0), fOutPtr(0), 
								 fRecPointStructArrayPtr(0), fRecPointListPtr(0)
{
  //Constructor

}

AliHLTPHOSClusterizerComponent::~AliHLTPHOSClusterizerComponent()
{
  //Destructor

  if(fClusterizerPtr)
    {
      delete fClusterizerPtr;
      fClusterizerPtr = 0;
    }
  
  if(fRecPointListPtr)
    {
      delete fRecPointListPtr;
      fRecPointListPtr = 0;
    }

  if(fRecPointStructArrayPtr)
    {
      delete fRecPointStructArrayPtr;
      fRecPointStructArrayPtr = 0;
    }

}

AliHLTPHOSClusterizerComponent::AliHLTPHOSClusterizerComponent(const AliHLTPHOSClusterizerComponent &):AliHLTProcessor(), 
												       fClusterizerPtr(0), 
												       fOutPtr(0),
												       fRecPointStructArrayPtr(0),
												       fRecPointListPtr(0)
{
  //Copy constructor, not implemented
}

Int_t
AliHLTPHOSClusterizerComponent::Deinit()
{
  //Deinitialization

  if(fClusterizerPtr)
    {
      delete fClusterizerPtr;
      fClusterizerPtr = 0;
    }
  
  if(fRecPointListPtr)
    {
      delete fRecPointListPtr;
      fRecPointListPtr = 0;
    }

  if(fRecPointStructArrayPtr)
    {
      delete fRecPointStructArrayPtr;
      fRecPointStructArrayPtr = 0;
    }

  return 0;
}

Int_t
AliHLTPHOSClusterizerComponent::DoDeinit()
{
  //Do deinitialization
  Logging(kHLTLogInfo, "HLT", "PHOS", ",AliHLTPHOSClusterizerComponent DoDeinit");

  return 0;
}


const Char_t* 
AliHLTPHOSClusterizerComponent::GetComponentID()
{
  return "AliHltPhosClusterizer";
}

void
AliHLTPHOSClusterizerComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  //Get datatypes for input
  const AliHLTComponentDataType* pType=fgkInputDataTypes;
  while (pType->fID!=0) {
    list.push_back(*pType);
    pType++;
  }
}

AliHLTComponentDataType 
AliHLTPHOSClusterizerComponent::GetOutputDataType()
{
  return AliHLTPHOSPhysicsDefinitions::fgkAliHLTClusterDataType;
}

void
AliHLTPHOSClusterizerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier )

{
  constBase = 30;
  inputMultiplier = 1;
}

Int_t 
AliHLTPHOSClusterizerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& size,
					std::vector<AliHLTComponentBlockData>& outputBlocks)
{
  //Do event
   
  UInt_t tSize            = 0;
  UInt_t offset           = 0; 
  UInt_t mysize           = 0;
  Int_t nRecPoints        = 0;
  Int_t index             = 0;

  AliHLTUInt8_t* outBPtr;
  outBPtr = outputPtr;
  const AliHLTComponentBlockData* iter = 0; 
  unsigned long ndx; 

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      
      if(iter->fDataType != AliHLTPHOSDefinitions::fgkCellEnergyDataType)
	{
	  cout << "Warning: data type is not fgkCellEnergyDataType " << endl;
	  continue;
	}
      index = fClusterizerPtr->BuildCellEnergyArray( reinterpret_cast<AliHLTPHOSRcuCellEnergyDataStruct*>(iter->fPtr),
						     fRecPointListPtr);
      
    }
  
  nRecPoints = fClusterizerPtr->CreateRecPointStructArray(fRecPointStructArrayPtr, fRecPointListPtr, index);
  
  for(Int_t i = 0; i < nRecPoints; i++)
    {
      mysize = 0;
      offset = tSize;
      
      fOutPtr =  (AliHLTPHOSClusterDataStruct*)outBPtr;
      fClusterizerPtr->CalculateCenterOfGravity(&fRecPointStructArrayPtr[i]);
      fClusterizerPtr->ClusterizeStruct(&fRecPointStructArrayPtr[i], fOutPtr);

      mysize += sizeof(AliHLTPHOSClusterDataStruct);
      
      AliHLTComponentBlockData bd;
      FillBlockData( bd );
      bd.fOffset = offset;
      bd.fSize = mysize;
      bd.fDataType = AliHLTPHOSPhysicsDefinitions::fgkAliHLTClusterDataType;
      bd.fSpecification = 0xFFFFFFFF;
      outputBlocks.push_back( bd );
       
      tSize += mysize;
      outBPtr += mysize;
      
      if( tSize > size )
	{
	  Logging( kHLTLogFatal, "HLT::AliHLTPHOSClusterizerComponent::DoEvent", "Too much data",
		   "Data written over allowed buffer. Amount written: %lu, allowed amount: %lu."
		   , tSize, size );
	  return EMSGSIZE;
	}
    }

  size = tSize;
  fClusterizerPtr->ResetCellEnergyArray();

  return 0;

}

Int_t
AliHLTPHOSClusterizerComponent::DoInit(Int_t argc, const Char_t** argv )
{
  //Do initialization
  
  fClusterizerPtr = new AliHLTPHOSClusterizer();
  fClusterizerPtr->SetThreshold(atof(argv[0]));
  fClusterizerPtr->SetClusterThreshold(atof(argv[1]));
  fClusterizerPtr->SetHighGainFactor(atof(argv[2]));
  fClusterizerPtr->SetLowGainFactor(atof(argv[3]));
  fClusterizerPtr->SetArraySize(atoi(argv[4]));
  fClusterizerPtr->ResetCellEnergyArray();
  fRecPointListPtr = new AliHLTPHOSRecPointListDataStruct[N_ROWS_MOD*N_COLUMNS_MOD];
  fRecPointStructArrayPtr = new AliHLTPHOSRecPointDataStruct[1000];

  if (argc==0 && argv==NULL) {
    // this is currently just to get rid of the warning "unused parameter"
  }
  
  return 0;
}

AliHLTComponent*
AliHLTPHOSClusterizerComponent::Spawn()
{
  //Spawn a new AliHLTPHOSClusterizerComponent, for HLT framework
  return new AliHLTPHOSClusterizerComponent();
}
