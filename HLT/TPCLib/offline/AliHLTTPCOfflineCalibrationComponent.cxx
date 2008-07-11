// $Id$

//**************************************************************************
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//*                                                                        *
//* Primary Authors: Jacek Otwinowski <J.Otwinowski@gsi.de>                *
//*                                                                        *
//* Permission to use, copy, modify and distribute this software and its   *
//* documentation strictly for non-commercial purposes is hereby granted   *
//* without fee, provided that the above copyright notice appears in all   *
//* copies and that both the copyright notice and this permission notice   *
//* appear in the supporting documentation. The authors make no claims     *
//* about the suitability of this software for any purpose. It is          *
//* provided "as is" without express or implied warranty.                  *
//**************************************************************************

/** @file   AliHLTTPCOfflineCalibrationComponent.cxx
    @author Jacek Otwinowski <J.Otwinowski@gsi.de>
    @date   
    @brief  TPC calibration component
*/

#include "AliHLTTPCOfflineCalibrationComponent.h"
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "AliTPCcalibAlign.h"
#include "AliTPCcalibTracksGain.h"
#include "AliTPCcalibTracks.h"
#include "AliESDEvent.h"
#include "AliESDfriend.h"
#include "AliCDBManager.h"
#include "AliTPCcalibDB.h"
#include "AliTPCClusterParam.h"
#include "AliTPCcalibTracksCuts.h"
#include "AliTPCseed.h"
#include "AliTPCcalibTracksCuts.h"
#include "AliTPCClusterParam.h"
#include "AliHLTTPCDefinitions.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCOfflineCalibrationComponent)

AliHLTTPCOfflineCalibrationComponent::AliHLTTPCOfflineCalibrationComponent() : AliHLTCalibrationProcessor(),
fEnableAnalysis(kTRUE),
fClustParam(0),
fTrackCuts(0),
fTPCcalibAlign(0),
fTPCcalibTracksGain(0),
fTPCcalibTracks(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTTPCOfflineCalibrationComponent::~AliHLTTPCOfflineCalibrationComponent()
{
  // see header file for class documentation
}

const char* AliHLTTPCOfflineCalibrationComponent::GetComponentID()
{
  // see header file for class documentation
  return "TPCOfflineCalibration";
}

void AliHLTTPCOfflineCalibrationComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  // get input data type
  list.clear();
  list.push_back(kAliHLTDataTypeESDObject|kAliHLTDataOriginTPC/*AliHLTTPCDefinitions::fgkOfflineClustersDataType*/);
}

AliHLTComponentDataType AliHLTTPCOfflineCalibrationComponent::GetOutputDataType()
{
  // return ouput data type
  return kAliHLTMultipleDataType;
}
 

int AliHLTTPCOfflineCalibrationComponent::GetOutputDataTypes(AliHLTComponentDataTypeList& tgtList)
{
  // create output data type
  tgtList.clear();
  tgtList.push_back(AliHLTTPCDefinitions::fgkOfflineCalibAlignDataType);
  tgtList.push_back(AliHLTTPCDefinitions::fgkOfflineCalibTracksDataType);
  tgtList.push_back(AliHLTTPCDefinitions::fgkOfflineCalibTracksGainDataType);

  return tgtList.size(); 
}

void AliHLTTPCOfflineCalibrationComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  // get output data size
  constBase = 30000000;
  inputMultiplier = 1;
}

AliHLTComponent* AliHLTTPCOfflineCalibrationComponent::Spawn()
{
  // create instance of the component
  return new AliHLTTPCOfflineCalibrationComponent;
}

int AliHLTTPCOfflineCalibrationComponent::InitCalibration()
{
  // init configuration 
  
  HLTInfo("init calibration component");
  int iResult=0;

  //
  // initialisation
  //

  // Init parameters and cuts
  fClustParam = AliTPCcalibDB::Instance()->GetClusterParam();
  fTrackCuts = new AliTPCcalibTracksCuts(20, 0.4, 0.5, 0.13, 0.018);

  // Init calibration componenets
  fTPCcalibAlign = new AliTPCcalibAlign("TPCcalibAlign","TPCcalibAlign");
  fTPCcalibTracksGain = new AliTPCcalibTracksGain("TPCcalibTracksGain","TPCcalibTracksGain",fTrackCuts);
  fTPCcalibTracks = new AliTPCcalibTracks("TPCcalibTracks","TPCcalibTracks",fClustParam,fTrackCuts);

  if (!fTrackCuts || !fClustParam ||  !fTPCcalibAlign || !fTPCcalibTracksGain || !fTPCcalibTracks) {
    HLTError("failed creating internal objects");
    iResult=-ENOMEM;
  }

  return iResult;
}

Int_t AliHLTTPCOfflineCalibrationComponent::DeinitCalibration()
{
  // deinit configuration
  if(fClustParam) delete fClustParam; fClustParam = 0; 
  if(fTrackCuts) delete fTrackCuts; fTrackCuts = 0; 

  if(fTPCcalibAlign) delete fTPCcalibAlign; fTPCcalibAlign = 0; 
  if(fTPCcalibTracksGain) delete fTPCcalibTracksGain; fTPCcalibTracksGain = 0; 
  if(fTPCcalibTracks) delete fTPCcalibTracks; fTPCcalibTracks = 0; 

  return 0;
}

Int_t AliHLTTPCOfflineCalibrationComponent::ScanArgument(Int_t argc, const char** argv)
{
  int iResult = 0;
 
  TString argument="";
  TString configuration=""; 
  int bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }

  if (iResult>=0 && !configuration.IsNull()) {
    iResult=Configure(configuration.Data());
  } else {
    iResult=Reconfigure(NULL, NULL);
  }

  return iResult;
}

int AliHLTTPCOfflineCalibrationComponent::ProcessCalibration(const AliHLTComponentEventData& /*evtData*/, AliHLTComponentTriggerData& /*trigData*/)
{
  // calibration function
  HLTInfo("ProcessCalibration processing data");

  int iResult=0;
  AliESDEvent *pESD =0;
  AliESDfriend* pESDfriend=0;
  int slice, patch;
  
  // calculate specification
  const AliHLTComponentBlockData* pBlock=GetFirstInputBlock(kAliHLTDataTypeESDObject|kAliHLTDataOriginTPC); 
  if(!pBlock) {
     HLTError("Cannot get first data block 0x%08x ",pBlock);
     iResult=-ENOMEM; return iResult;
  }
  int minSlice=AliHLTTPCDefinitions::GetMinSliceNr(pBlock->fSpecification);
  int maxSlice=AliHLTTPCDefinitions::GetMaxSliceNr(pBlock->fSpecification);
  int minPatch=AliHLTTPCDefinitions::GetMinPatchNr(pBlock->fSpecification);
  int maxPatch=AliHLTTPCDefinitions::GetMaxPatchNr(pBlock->fSpecification);  
 
  if (fTPCcalibAlign && fTPCcalibTracksGain && fTPCcalibTracks) 
  {
    // loop over input data blocks: ESD events
    for (TObject *pObj = (TObject *)GetFirstInputObject(kAliHLTDataTypeESDObject|kAliHLTDataOriginTPC,"ESD",0);
	 pObj !=0 && iResult>=0;
	 pObj = (TObject *)GetNextInputObject(0)) {
      pESD = dynamic_cast<AliESDEvent*>(pObj);
      if (!pESD) continue;

      // get standard ESD content
      pESD->GetStdContent();

      HLTInfo("load %d esd tracks from block %s 0x%08x", pESD->GetNumberOfTracks(), DataType2Text(GetDataType(pObj)).c_str(), GetSpecification(pObj));

      slice=AliHLTTPCDefinitions::GetMinSliceNr(GetSpecification(pObj));
      patch=AliHLTTPCDefinitions::GetMinPatchNr(GetSpecification(pObj));

      if(slice < minSlice) minSlice=slice;
      if(slice > maxSlice) maxSlice=slice;
      if(patch < minPatch) minPatch=patch;
      if(patch > maxPatch) maxPatch=patch;

      // get ESD fiends
      pESDfriend=static_cast<AliESDfriend*>(pESD->FindListObject("AliESDfriend"));
      if (!pESDfriend) {
         HLTError("Cannot load ESD friends  0x%08x", pESDfriend);
         iResult=-ENOMEM; 
	 return iResult;
      }
      HLTInfo("load %d esd friend tracks from 0x%08x", pESDfriend->GetNumberOfTracks(), pESDfriend);

      // get TPC seeds 
      Int_t n=pESD->GetNumberOfTracks();
      for (Int_t i=0;i<n;++i) {
         AliESDfriendTrack *friendTrack=pESDfriend->GetTrack(i);

	 if(!friendTrack) continue;
         HLTInfo("Process calibration on friend track 0x%08x", friendTrack);

         TObject *calibObject=0;
         AliTPCseed *seed=0;
         for (Int_t j=0;(calibObject=friendTrack->GetCalibObject(j));++j) {
            if ((seed=dynamic_cast<AliTPCseed*>(calibObject))!=0) break;
	 }
         if (seed) {
            HLTInfo("Process calibration on seed 0x%08x", seed);
            fTPCcalibAlign->Process(seed);
            fTPCcalibTracksGain->Process(seed);
            fTPCcalibTracks->Process(seed);
	 }
      }
      // calculate specification from the specification of input data blocks
        AliHLTUInt32_t iSpecification = AliHLTTPCDefinitions::EncodeDataSpecification(minSlice, maxSlice, minPatch, maxPatch);

	// send data
  	PushBack((TObject*)fTPCcalibAlign,AliHLTTPCDefinitions::fgkOfflineCalibAlignDataType,iSpecification);
  	PushBack((TObject*)fTPCcalibTracksGain,AliHLTTPCDefinitions::fgkOfflineCalibTracksGainDataType,iSpecification);
  	PushBack((TObject*)fTPCcalibTracks,AliHLTTPCDefinitions::fgkOfflineCalibTracksDataType,iSpecification);

    }// end loop over input objects
    
  } else {
    HLTError("component not initialized");
    iResult=-ENOMEM;
  }

  return iResult;
}

Int_t AliHLTTPCOfflineCalibrationComponent::ShipDataToFXS( const AliHLTComponentEventData& /*evtData*/,
	                                               AliHLTComponentTriggerData& /*trigData*/ ) {
// see header file for class documentation
   if( fEnableAnalysis )	 
   {
      fTPCcalibAlign->Analyze();
      fTPCcalibTracksGain->Analyze();
      fTPCcalibTracks->Analyze();
   }
   PushToFXS((TObject*)fTPCcalibAlign, "TPC", "TPCcalibAlign") ;
   PushToFXS((TObject*)fTPCcalibTracksGain, "TPC", "TPCcalibTracksGain") ;
   PushToFXS((TObject*)fTPCcalibTracks, "TPC", "TPCcalibTracks") ;

return 0;
}

int AliHLTTPCOfflineCalibrationComponent::Configure(const char* arguments)
{
  // see header file for class documentation
  int iResult=0;
  if (!arguments) return iResult;

  TString allArgs=arguments;
  TString argument;
  int bMissingParam=0;

  TObjArray* pTokens=allArgs.Tokenize(" ");
  if (pTokens) {
    for (int i=0; i<pTokens->GetEntries() && iResult>=0; i++) {
      argument=((TObjString*)pTokens->At(i))->GetString();
      if (argument.IsNull()) continue;

      if (argument.CompareTo("-something")==0) {
	if ((bMissingParam=(++i>=pTokens->GetEntries()))) break;

      } else {
	HLTError("unknown argument %s", argument.Data());
	iResult=-EINVAL;
	break;
      }
    }
    delete pTokens;
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTTPCOfflineCalibrationComponent::Reconfigure(const char* /*cdbEntry*/, const char* /*chainId*/)
{
  // see header file for class documentation
  int iResult=0;
  return iResult;
}
