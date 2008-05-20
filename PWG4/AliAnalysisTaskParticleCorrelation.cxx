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

// root
#include <TROOT.h>
#include <TSystem.h>
#include <TInterpreter.h>
#include <TChain.h>
#include <TFile.h>
#include <Riostream.h>

// analysis
#include "AliAnalysisTaskParticleCorrelation.h"
#include "AliAnalysisManager.h"
#include "AliESDInputHandler.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliAnaMaker.h"
#include "AliCaloTrackReader.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliAODHandler.h"
#include "AliStack.h"
#include "AliLog.h"

ClassImp(AliAnalysisTaskParticleCorrelation)

////////////////////////////////////////////////////////////////////////

  AliAnalysisTaskParticleCorrelation::AliAnalysisTaskParticleCorrelation():
    AliAnalysisTaskSE(),
    fAna(0x0),
    fOutputContainer(0x0),
    fAODBranch(0x0),
    fConfigName(0)
{
  // Default constructor
}

//_____________________________________________________
AliAnalysisTaskParticleCorrelation::AliAnalysisTaskParticleCorrelation(const char* name):
    AliAnalysisTaskSE(name),
    fAna(0x0),
    fOutputContainer(0x0),
    fAODBranch(0x0),
    fConfigName("ConfigAnalysis")
{
  // Default constructor

  DefineOutput(1, TList::Class());

}

//_____________________________________________________
AliAnalysisTaskParticleCorrelation::~AliAnalysisTaskParticleCorrelation() 
{
  // Remove all pointers
 
  if(fOutputContainer){
    fOutputContainer->Clear() ; 
    delete fOutputContainer ;
  }

}

//_____________________________________________________
void AliAnalysisTaskParticleCorrelation::UserCreateOutputObjects()
{
  // Create the output container
  if (fDebug > 1) printf("AnalysisTaskParticleCorrelation::CreateOutputData() \n");

  //AODs
  fAODBranch = new TClonesArray("AliAODParticleCorrelation", 0);
  fAODBranch->SetName(fAna->GetAODBranchName());
  AddAODBranch("TClonesArray", &fAODBranch);
  fAna->SetAODBranch(fAODBranch);

  //Histograms container
  OpenFile(1);
  fOutputContainer = fAna->GetOutputContainer();
  
}

//_____________________________________________________
void AliAnalysisTaskParticleCorrelation::Init()
{
  // Initialization
  if (fDebug > 1) printf("AnalysisTaskParticleCorrelation::Init() \n");
  
  // Call configuration file

  if(fConfigName == ""){
    fConfigName="ConfigAnalysis";
  }
 
  AliInfo(Form("### Configuration file is %s.C ###", fConfigName.Data()));
  gROOT->LoadMacro(fConfigName+".C");
  fAna = (AliAnaMaker*) gInterpreter->ProcessLine("ConfigAnalysis()");
  
  if(!fAna)
    AliFatal("Analysis pointer not initialized, abort analysis!");
  
  // Initialise analysis
  fAna->Init();
  
  AliDebug(1,"End");
  
}


//_____________________________________________________
void AliAnalysisTaskParticleCorrelation::UserExec(Option_t */*option*/)
{
  // Execute analysis for current event
  //
  if (fDebug > 1) printf("AnalysisTaskParticleCorrelation::Exec() \n");

  //Get the type of data, check if type is correct
  Int_t  datatype = fAna->GetReader()->GetDataType();
  if(datatype != AliCaloTrackReader::kESD && datatype != AliCaloTrackReader::kAOD &&
     datatype != AliCaloTrackReader::kMC){
    AliFatal("Wrong type of data");
    return ;
  }
  
  fAna->GetReader()->SetInputEvent(InputEvent(), AODEvent(), MCEvent());

  //Process event
  fAna->ProcessEvent((Int_t) Entry());
  
  PostData(1, fOutputContainer);
  
}

//_____________________________________________________
void AliAnalysisTaskParticleCorrelation::Terminate(Option_t */*option*/)
{
  // Terminate analysis
  //
  AliDebug(1,"Do nothing in Terminate");
  //fAna->Terminate();
}

