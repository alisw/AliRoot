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

/* $Id$ */
#include <Riostream.h> 
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <TList.h>

#include "AliAnalysisTaskJets.h"
#include "AliAnalysisManager.h"
#include "AliJetFinder.h"
#include "AliJetHeader.h"
#include "AliJetReader.h"
#include "AliJetReaderHeader.h"
#include "AliJetHistos.h"
#include "AliESDEvent.h"
#include "AliESD.h"
#include "AliAODEvent.h"
#include "AliAODJetEventBackground.h"
#include "AliAODHandler.h"
#include "AliMCEventHandler.h"
#include "AliESDInputHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"


ClassImp(AliAnalysisTaskJets)

////////////////////////////////////////////////////////////////////////

AliAnalysisTaskJets::AliAnalysisTaskJets():
  AliAnalysisTaskSE(),
  fConfigFile("ConfigJetAnalysis.C"),
  fNonStdBranch(""), 
  fNonStdFile(""),
  fJetFinder(0x0),
  fHistos(0x0),
  fAODExtension(0x0),
  fListOfHistos(0x0),
  fChain(0x0),
  fOpt(0),
  fReadAODFromOutput(0)
{
  // Default constructor
}

AliAnalysisTaskJets::AliAnalysisTaskJets(const char* name):
  AliAnalysisTaskSE(name),
  fConfigFile("ConfigJetAnalysis.C"),
  fNonStdBranch(""),
  fNonStdFile(""),
  fJetFinder(0x0),
  fHistos(0x0),
  fAODExtension(0x0),
  fListOfHistos(0x0),
  fChain(0x0),
  fOpt(0),
  fReadAODFromOutput(0)
{
  // Default constructor
  DefineOutput(1, TList::Class());
}

AliAnalysisTaskJets::AliAnalysisTaskJets(const char* name, TChain* chain):
  AliAnalysisTaskSE(name),
  fConfigFile("ConfigJetAnalysis.C"),
  fNonStdBranch(""),
  fNonStdFile(""),
  fJetFinder(0x0),
  fHistos(0x0),
  fAODExtension(0x0),
  fListOfHistos(0x0),
  fChain(chain),
  fOpt(0),
  fReadAODFromOutput(0)
{
  // Default constructor
  DefineOutput(1, TList::Class());
}

//----------------------------------------------------------------
void AliAnalysisTaskJets::UserCreateOutputObjects()
{
  // Create the output container
  //
  if (fDebug > 1) printf("AnalysisTaskJets::CreateOutPutData() \n");

  if(fNonStdBranch.Length()==0)
    {
      //  Connec default AOD to jet finder
      // create a new branch for the background
      if(!AODEvent()->FindListObject(AliAODJetEventBackground::StdBranchName())){
	AliAODJetEventBackground* evBkg = new AliAODJetEventBackground();
	evBkg->SetName(AliAODJetEventBackground::StdBranchName());
	AddAODBranch("AliAODJetEventBackground",&evBkg);
      }
      fJetFinder->ConnectAOD(AODEvent());
    }
  else
    {
      // Create a new branch for jets...
      // how is this reset? -> cleared in the UserExec....
      // Can this be handled by the framework?
      // here we can also have the case that the brnaches are written to a separate file

      TClonesArray *tca = new TClonesArray("AliAODJet", 0);
      tca->SetName(fNonStdBranch.Data());
      AddAODBranch("TClonesArray",&tca,fNonStdFile.Data());
      if(!AODEvent()->FindListObject(Form("%s_%s",AliAODJetEventBackground::StdBranchName(),fNonStdBranch.Data()))){
	AliAODJetEventBackground* evBkg = new AliAODJetEventBackground();
	evBkg->SetName(Form("%s_%s",AliAODJetEventBackground::StdBranchName(),fNonStdBranch.Data()));
	AddAODBranch("AliAODJetEventBackground",&evBkg,fNonStdFile.Data());
      }

      if(fNonStdFile.Length()!=0){
	// 
	// case that we have an AOD extension we need to fetch the jets from the extended output
	// we identifay the extension aod event by looking for the branchname
	AliAODHandler *aodH = dynamic_cast<AliAODHandler*>(AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler());
	TObjArray* extArray = aodH->GetExtensions();
	if (extArray) {
	  TIter next(extArray);
	  while ((fAODExtension=(AliAODExtension*)next())){
	    TObject *obj = fAODExtension->GetAOD()->FindListObject(fNonStdBranch.Data());
	    if(fDebug>10){
	      Printf("%s:%d Dumping..",(char*)__FILE__,__LINE__);
	      fAODExtension->GetAOD()->Dump();
	    }
	    if(obj){
	      if(fDebug>1)Printf("AODExtension found for %s",fNonStdBranch.Data());
	      break;
	    }
	    fAODExtension = 0;
	  }
	}
	if(fAODExtension)fJetFinder->ConnectAODNonStd(fAODExtension->GetAOD(), fNonStdBranch.Data()); 
      }
      else{
	fJetFinder->ConnectAODNonStd(AODEvent(), fNonStdBranch.Data()); 
      }
    }


  // Histograms
  OpenFile(1);
  fListOfHistos = new TList();
  fHistos       = new AliJetHistos();
  fHistos->AddHistosToList(fListOfHistos);
  
  // Add the JetFinderInformation to the Outputlist
  AliJetHeader *fH = fJetFinder->GetHeader();
  
  // Compose a characteristic output name
  // with the name of the output branch
  if(fH) {
    if(fNonStdBranch.Length()==0) {
      fH->SetName("AliJetHeader_jets");
    }
    else {
      fH->SetName(Form("AliJetHeader_%s",fNonStdBranch.Data()));
    }
  }
  if(!fAODExtension)OutputTree()->GetUserInfo()->Add(fH);
  else fAODExtension->GetTree()->GetUserInfo()->Add(fH);
}

//----------------------------------------------------------------
void AliAnalysisTaskJets::Init()
{
  // Initialization
  if (fDebug > 1) printf("AnalysisTaskJets::Init() \n");
  
  // Call configuration file
  if (fConfigFile.Length()) {
    gROOT->LoadMacro(fConfigFile);
    fJetFinder = (AliJetFinder*) gInterpreter->ProcessLine("ConfigJetAnalysis()");
  }
  AliJetReaderHeader *header = (AliJetReaderHeader*)fJetFinder->GetReader()->GetReaderHeader();
  fOpt = header->GetDetector();

  // Initialise Jet Analysis
  if(fOpt == 0) fJetFinder->Init();
  else fJetFinder->InitTask(fChain); // V2
}


//----------------------------------------------------------------
void AliAnalysisTaskJets::UserExec(Option_t */*option*/)
{
  // Execute analysis for current event
  //
  // Fill control histos
  TClonesArray* jarray = 0;
  AliAODJetEventBackground*  evBkg;

  if(fNonStdBranch.Length()==0) {
    jarray = AODEvent()->GetJets();
    evBkg = (AliAODJetEventBackground*)(AODEvent()->FindListObject(AliAODJetEventBackground::StdBranchName()));
    evBkg->Reset();
  }
  else {
    jarray = (TClonesArray*)(AODEvent()->FindListObject(fNonStdBranch.Data()));
    if(!jarray)jarray = (TClonesArray*)(fAODExtension->GetAOD()->FindListObject(fNonStdBranch.Data()));
    jarray->Delete();    // this is our responsibility, clear before filling again
    evBkg = (AliAODJetEventBackground*)(AODEvent()->FindListObject(Form("%s_%s",AliAODJetEventBackground::StdBranchName(),fNonStdBranch.Data())));
    if(!evBkg)  evBkg = (AliAODJetEventBackground*)(fAODExtension->GetAOD()->FindListObject(Form("%s_%s",AliAODJetEventBackground::StdBranchName(),fNonStdBranch.Data())));
    evBkg->Reset();
  }

  if (dynamic_cast<AliAODEvent*>(InputEvent()) !=  0 && !fReadAODFromOutput) {
// AOD is input event..........................................V                                       
      fJetFinder->GetReader()->SetInputEvent(InputEvent(), InputEvent(), MCEvent());
  } else {
// AOD is read from output ....................................V      
      fJetFinder->GetReader()->SetInputEvent(InputEvent(), AODEvent(), MCEvent());
  }
  
  

  if(fOpt==0) fJetFinder->ProcessEvent();
  else fJetFinder->ProcessEvent2();    // V2
 
  // Fill control histos
  fHistos->FillHistos(jarray);

  // Post the data
  PostData(1, fListOfHistos);
  return;
}


//*************************************************************

void AliAnalysisTaskJets::Terminate(Option_t */*option*/)
{
// Terminate analysis
//
    if (fDebug > 1) printf("AnalysisJets: Terminate() \n");
//    if (fJetFinder) fJetFinder->FinishRun();
}

