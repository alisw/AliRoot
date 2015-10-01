//-*- Mode: C++ -*-
// $Id: AliHLTTPCCalibManagerComponent.cxx $
/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: David Rohr, Jens Wiechula, C. Zampolli                *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file    AliHLTTPCCalibManagerComponent.cxx
    @author  David Rohr, Jens Wiechula, C. Zampolli, I. Vorobyev
    @brief   Component for Testing TPC Calibration inside HLT component
*/

#include "TMap.h"
#include "TSystem.h"
#include "TTimeStamp.h"
#include "TObjString.h"
#include "TH1F.h"
#include "TList.h"

#include <map>
#include <string>
#include "TString.h"
#include "TArrayI.h"

//#include "AliESDtrackCuts.h"
#include "AliESDEvent.h"
#include "AliHLTErrorGuard.h"
#include "AliHLTDataTypes.h"
#include "AliHLTTPCCalibManagerComponent.h"
#include "AliHLTITSClusterDataFormat.h"
#include "AliHLTAnalysisManager.h"
#include "AliHLTVEventInputHandler.h"
#include "AliTPCAnalysisTaskcalib.h"
#include "AliAnalysisDataContainer.h"
#include "TTree.h"
#include "TChain.h"
#include "AliFlatESDEvent.h"
#include "AliFlatESDFriend.h"
#include "AliVEvent.h"
#include "AliVfriendEvent.h"
#include "AliSysInfo.h"

#include "AliTPCcalibBase.h"

#include "AliTPCcalibAlign.h"
#include "AliTPCcalibLaser.h"
#include "AliTPCcalibCosmic.h"

#include "AliTPCcalibCalib.h"
#include "AliTPCcalibTimeGain.h"
#include "AliTPCcalibGainMult.h"
#include "AliTPCcalibTime.h"

#include "AliTPCcalibTracks.h"
#include "TPRegexp.h"

#include "AliTPCParam.h"
#include "AliMagF.h"
#include "TGeoGlobalMagField.h"
#include "AliTPCTransform.h"
#include "AliCDBEntry.h"
#include "AliTPCRecoParam.h"
#include "AliCDBManager.h"
#include "AliGRPObject.h"
#include "AliTPCcalibDB.h"
#include "AliTPCClusterParam.h"
#include "AliTPCcalibTracksCuts.h"

#include "TROOT.h"

using namespace std;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCCalibManagerComponent)

/*
 * ---------------------------------------------------------------------------------
 *                            Constructor / Destructor
 * ---------------------------------------------------------------------------------
 */

// #################################################################################
AliHLTTPCCalibManagerComponent::AliHLTTPCCalibManagerComponent() :
  AliHLTProcessor(),
  fUID(0),
  fAnalysisManager(NULL),
  fInputHandler(NULL),
  fTPCcalibConfigString("TPCCalib:CalibTimeDrift"),
  fAddTaskMacro("$ALICE_PHYSICS/PWGPP/CalibMacros/CPass0/AddTaskTPCCalib.C"),
  fWriteAnalysisToFile(kFALSE),
  fEnableDebug(kFALSE),
  fResetAfterPush(kTRUE)
{
  // an example component which implements the ALICE HLT processor
  // interface and does some analysis on the input raw data
  //
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
  //
  // NOTE: all helper classes should be instantiated in DoInit()
}

// #################################################################################
AliHLTTPCCalibManagerComponent::~AliHLTTPCCalibManagerComponent() {
  // see header file for class documentation
}

/*
 * ---------------------------------------------------------------------------------
 * Public functions to implement AliHLTComponent's interface.
 * These functions are required for the registration process
 * ---------------------------------------------------------------------------------
 */

// #################################################################################
const Char_t* AliHLTTPCCalibManagerComponent::GetComponentID() {
  // see header file for class documentation
  return "TPCCalibManagerComponent";
}

// #################################################################################
void AliHLTTPCCalibManagerComponent::GetInputDataTypes( vector<AliHLTComponentDataType>& list) {
  // see header file for class documentation
  list.push_back(kAliHLTDataTypeESDObject|kAliHLTDataOriginAny);
  list.push_back(kAliHLTDataTypeClusters|kAliHLTDataOriginITSSPD);
  list.push_back(kAliHLTDataTypeESDContent|kAliHLTDataOriginVZERO);
  list.push_back(kAliHLTDataTypeESDfriendObject|kAliHLTDataOriginAny);
  list.push_back(kAliHLTDataTypeFlatESD|kAliHLTDataOriginOut);
  list.push_back(kAliHLTDataTypeFlatESDFriend|kAliHLTDataOriginOut);
  list.push_back(kAliHLTDataTypeEOR|kAliHLTDataOriginAny);
}

// #################################################################################
AliHLTComponentDataType AliHLTTPCCalibManagerComponent::GetOutputDataType() {
  // see header file for class documentation
  return kAliHLTDataTypeTObject|kAliHLTDataOriginHLT;
}

// #################################################################################
void AliHLTTPCCalibManagerComponent::GetOutputDataSize( ULong_t& constBase, Double_t& inputMultiplier ) {
  // see header file for class documentation
  constBase = 100000;
  inputMultiplier = 0.5;
}

// #################################################################################
void AliHLTTPCCalibManagerComponent::GetOCDBObjectDescription( TMap* const targetMap) {
  // see header file for class documentation

  if (!targetMap) return;
  /*  targetMap->Add(new TObjString("HLT/ConfigGlobal/MultiplicityCorrelations"),
		 new TObjString("configuration object"));
  targetMap->Add(new TObjString("HLT/ConfigGlobal/MultiplicityCorrelationsCentrality"),
		 new TObjString("centrality configuration object"));
  */
  return;
}

// #################################################################################
AliHLTComponent* AliHLTTPCCalibManagerComponent::Spawn() {
  // see header file for class documentation
  return new AliHLTTPCCalibManagerComponent;
}

/*
 * ---------------------------------------------------------------------------------
 * Protected functions to implement AliHLTComponent's interface.
 * These functions provide initialization as well as the actual processing
 * capabilities of the component. 
 * ---------------------------------------------------------------------------------
 */

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::DoInit( Int_t /*argc*/, const Char_t** /*argv*/ ) {
  // see header file for class documentation
  HLTInfo("----> AliHLTTPCCalibManagerComponent::DoInit");

  //process arguments
  ProcessOptionString(GetComponentArgs());

  if (!fEnableDebug)
  {
    //shut up AliSysInfo globally, prevents syswatch.log files from being created
    //dont open any debug files
    AliSysInfo::SetDisabled(kTRUE);
    TTreeSRedirector::SetDisabled(kTRUE);
  }

  fAnalysisManager = new AliHLTAnalysisManager();
  fInputHandler    = new AliHLTVEventInputHandler("HLTinputHandler","HLT input handler");
  fAnalysisManager->SetInputEventHandler(fInputHandler);

  TString addTaskMacroArgs="\""+fTPCcalibConfigString+"\"";
  TString macro=fAddTaskMacro+"("+addTaskMacroArgs+")";
  HLTInfo("Executing: %s\n",macro.Data());
  gROOT->Macro(macro);

  fAnalysisManager->InitAnalysis();

  return 0;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::DoDeinit() {
  // see header file for class documentation

  if (fWriteAnalysisToFile && fAnalysisManager) 
    fAnalysisManager->WriteAnalysisToFile();
  delete fAnalysisManager;
  return 0;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::DoEvent(const AliHLTComponentEventData& evtData,
    AliHLTComponentTriggerData& /*trigData*/) {
  // see header file for class documentation

  HLTInfo("AliHLTTPCCalibManagerComponent::DoEvent\n");
  Int_t iResult=0;

  if( fUID == 0 ){
    TTimeStamp t;
    fUID = ( gSystem->GetPid() + t.GetNanoSec())*10 + evtData.fEventID;
  }

  if (!IsDataEvent())
  {
    //on EOR push unconditionally
    const AliHLTComponentBlockData* pBlock = 
      GetFirstInputBlock(kAliHLTDataTypeEOR|kAliHLTDataOriginAny);
    if (pBlock) 
    {
      PushAndReset(fAnalysisManager->GetOutputs()); 
    }
    return 0;
  }

  // -- Get ESD object
  // -------------------
  AliVEvent* vEvent=NULL;
  AliVfriendEvent* vFriend=NULL;
  iResult = ReadInput(vEvent,vFriend);

  if (!vEvent)

  if (vEvent) {HLTInfo("----> event %p has %d tracks: \n", vEvent, vEvent->GetNumberOfTracks());}
  if(vFriend) {HLTInfo("----> friend %p has %d tracks: \n", vFriend, vFriend->GetNumberOfTracks());}

  fInputHandler->InitTaskInputData(vEvent, vFriend, fAnalysisManager->GetTasks());
  fAnalysisManager->ExecAnalysis();
  fInputHandler->FinishEvent();

  //pushes once every n seconds if
  //configured with -pushback-period=n
  PushAndReset(fAnalysisManager->GetOutputs()); 

  return iResult;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::Reconfigure(const Char_t* cdbEntry, const Char_t* chainId) {
  // see header file for class documentation

  Int_t iResult=0;
  TString cdbPath;
  if (cdbEntry) {
    cdbPath=cdbEntry;
  } else {
    cdbPath="HLT/ConfigGlobal/";
    cdbPath+=GetComponentID();
  }

  AliInfoClass(Form("reconfigure '%s' from entry %s%s", chainId, cdbPath.Data(), cdbEntry?"":" (default)"));
  iResult=ConfigureFromCDBTObjString(cdbPath);

  return iResult;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::ReadPreprocessorValues(const Char_t* /*modules*/) {
  // see header file for class documentation
  ALIHLTERRORGUARD(5, "ReadPreProcessorValues not implemented for this component");
  return 0;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::PushAndReset(TObject* object)
{
  //push the data - the data might not get pushed, depending on the 
  //"-pushback-period=" argument which might delay the actual push. 
  //pushResult==0 if pushing delayed.
  //
  int pushResult = PushBack(object, kAliHLTDataTypeTObject|kAliHLTDataOriginHLT,fUID);
  if (pushResult > 0 && fResetAfterPush) {fAnalysisManager->ResetOutputData();}
  return 0;
}

// #################################################################################
Int_t AliHLTTPCCalibManagerComponent::ReadInput(AliVEvent*& vEvent, AliVfriendEvent*& vFriend)
{
  //read input, return code 0 for normal events, 1 for EOR

  Int_t iResult=0;
  for ( const TObject *iter = GetFirstInputObject(kAliHLTDataTypeESDObject); iter != NULL; iter = GetNextInputObject() ) {
    vEvent = dynamic_cast<AliESDEvent*>(const_cast<TObject*>( iter ) );
    if( !vEvent ){ 
      HLTWarning("Wrong ESDEvent object received");
      iResult = -1;
      continue;
    }
    vEvent->GetStdContent();
  }
  for ( const TObject *iter = GetFirstInputObject(kAliHLTDataTypeESDfriendObject); iter != NULL; iter = GetNextInputObject() ) {
    vFriend = dynamic_cast<AliESDfriend*>(const_cast<TObject*>( iter ) );
    if( !vFriend ){ 
      HLTWarning("Wrong ESDFriend object received");
      iResult = -1;
      continue;
    }
  }

  if (!vEvent){ 
    {
      const AliHLTComponentBlockData* pBlock=GetFirstInputBlock(kAliHLTDataTypeFlatESD|kAliHLTDataOriginOut);
      AliFlatESDEvent* tmpFlatEvent=reinterpret_cast<AliFlatESDEvent*>( pBlock->fPtr );
      if (tmpFlatEvent->GetSize()==pBlock->fSize ){
        tmpFlatEvent->Reinitialize();
      } else {
        tmpFlatEvent = NULL;
        iResult=-1;
        HLTWarning("data mismatch in block %s (0x%08x): size %d -> ignoring flatESD information",
            DataType2Text(pBlock->fDataType).c_str(), pBlock->fSpecification, pBlock->fSize);
      }
      vEvent = tmpFlatEvent;
    }

    if( vEvent ){
      const AliHLTComponentBlockData* pBlock=GetFirstInputBlock(kAliHLTDataTypeFlatESDFriend|kAliHLTDataOriginOut);
      AliFlatESDFriend* tmpFlatFriend = reinterpret_cast<AliFlatESDFriend*>( pBlock->fPtr );
      if (tmpFlatFriend->GetSize()==pBlock->fSize ){
        tmpFlatFriend->Reinitialize();
      } else {
        tmpFlatFriend = NULL;
        iResult=-1;
        HLTWarning("data mismatch in block %s (0x%08x): size %d -> ignoring flatESDfriend information", 
            DataType2Text(pBlock->fDataType).c_str(), pBlock->fSpecification, pBlock->fSize);
      }
      vFriend = tmpFlatFriend;
    }
  }
  return iResult;
}

// #################################################################################
int AliHLTTPCCalibManagerComponent::ProcessOption(TString option, TString value)
{
  //process option
  //to be implemented by the user
  if (option.Contains("TPCcalibConfigString")) 
  {
    fTPCcalibConfigString=value;
    HLTInfo("fTPCcalibConfigString=%s\n",fTPCcalibConfigString.Data());
  }
  else if (option.Contains("WriteAnalysisToFile")) 
  {
    fWriteAnalysisToFile=(value.Contains("0"))?kFALSE:kTRUE;
    HLTInfo("fWriteAnalysisToFile=%i\n",fWriteAnalysisToFile?1:0);
  }
  else if (option.Contains("AddTaskMacro"))
  {
    fAddTaskMacro=value;
    HLTInfo("fAddTaskMacro=%s\n",fAddTaskMacro.Data());
  }
  else if (option.Contains("ResetAfterPush"))
  {
    fResetAfterPush=(value.Contains("0")?kFALSE:kTRUE);
    HLTInfo("fResetAfterPush=%i\n",fResetAfterPush?1:0);
  }
  return 1; 
}

// #################################################################################
int AliHLTTPCCalibManagerComponent::ProcessOptionString(TString arguments)
{
  //process passed options
  HLTInfo("Argument string: %s\n", arguments.Data());
  stringMap* options = TokenizeOptionString(arguments);
  for (stringMap::iterator i=options->begin(); i!=options->end(); ++i)
  {
    HLTInfo("  %s : %s\n", i->first.data(), i->second.data());
    ProcessOption(i->first,i->second);
  }
  delete options; //tidy up

  return 1; 
}

// #################################################################################
AliHLTTPCCalibManagerComponent::stringMap* AliHLTTPCCalibManagerComponent::TokenizeOptionString(const TString str)
{
  //options have the form:
  // -option value
  // -option=value
  // -option
  // --option value
  // --option=value
  // --option
  // option=value
  // option value
  // (value can also be a string like 'some string')
  //
  // options can be separated by ' ' or ',' arbitrarily combined, e.g:
  //"-option option1=value1 --option2 value2, -option4=\'some string\'"
  
  //optionRE by construction contains a pure option name as 3rd submatch (without --,-, =)
  //valueRE does NOT match options
  TPRegexp optionRE("(?:(-{1,2})|((?='?[^,=]+=?)))"
                    "((?(2)(?:(?(?=')'(?:[^'\\\\]++|\\.)*+'|[^, =]+))(?==?))"
                    "(?(1)[^, =]+(?=[= ,$])))");
  TPRegexp valueRE("(?(?!(-{1,2}|[^, =]+=))"
                   "(?(?=')'(?:[^'\\\\]++|\\.)*+'"
                   "|[^, =]+))");

  stringMap* options = new stringMap;

  TArrayI pos;
  const TString mods="";
  Int_t start = 0;
  while (1) {
    Int_t prevStart=start;
    TString optionStr="";
    TString valueStr="";
    
    //check if we have a new option in this field
    Int_t nOption=optionRE.Match(str,mods,start,10,&pos);
    if (nOption>0)
    {
      optionStr = str(pos[6],pos[7]-pos[6]);
      optionStr=optionStr.Strip(TString::kBoth,'\'');
      start=pos[1]; //update the current character to the end of match
    }

    //check if the next field is a value
    Int_t nValue=valueRE.Match(str,mods,start,10,&pos);
    if (nValue>0)
    {
      valueStr = str(pos[0],pos[1]-pos[0]);
      valueStr=valueStr.Strip(TString::kBoth,'\'');
      start=pos[1]; //update the current character to the end of match
    }
    
    //skip empty entries
    if (nOption>0 || nValue>0)
    {
      (*options)[optionStr.Data()] = valueStr.Data();
    }
    
    if (start>=str.Length()-1 || start==prevStart ) break;
  }

  for (stringMap::iterator i=options->begin(); i!=options->end(); ++i)
  {
    Printf("%s : %s", i->first.data(), i->second.data());
  }
  return options;
}
