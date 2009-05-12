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

//---------------------------------------------------------------------
// Jet finder base class
// manages the search for jets 
// Authors: jgcn@mda.cinvestav.mx
//          andreas.morsch@cern.ch
//          magali.estienne@subatech.in2p3.fr
//---------------------------------------------------------------------

#include <Riostream.h>
#include <TFile.h>
#include <TClonesArray.h>
#include <TProcessID.h>

#include "AliJetFinder.h"
#include "AliJet.h"
#include "AliAODJet.h"
#include "AliJetReader.h"
#include "AliJetReaderHeader.h"
#include "AliJetControlPlots.h"
#include "AliLeading.h"
#include "AliAODEvent.h"
#include "AliJetUnitArray.h"

ClassImp(AliJetFinder)

AliJetFinder::AliJetFinder():
    fTreeJ(0),
    fPlotMode(kFALSE),
    fJets(0),
    fGenJets(0),
    fLeading(0),
    fReader(0x0),
    fHeader(0x0),
    fAODjets(0x0),
    fNAODjets(0),
    fPlots(0x0),
    fOut(0x0)
    
{
  //
  // Constructor
  //

  fJets    = new AliJet();
  fGenJets = new AliJet();
  fLeading = new AliLeading();
  fAODjets = 0;
}

////////////////////////////////////////////////////////////////////////
AliJetFinder::~AliJetFinder()
{
  //
  // Destructor
  //

  // Reset and delete jets
  fJets->ClearJets();
  delete fJets;
  fGenJets->ClearJets();
  delete fGenJets;
  // close file
  if (fOut) {
    fOut->Close();
    fOut->Delete();
  }
  delete fOut;
  // reset and delete control plots
  if (fPlots) delete fPlots;
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::SetOutputFile(const char */*name*/)
{
  //  opens output file 
  //  fOut = new TFile(name,"recreate");
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::PrintJets()
{
  // Print jet information
  cout << " Jets found with jet algorithm:" << endl;
  fJets->PrintJets();
  cout << " Jets found by pythia:" << endl;
  fGenJets->PrintJets();
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::SetPlotMode(Bool_t b)
{
  // Sets the plotting mode
  fPlotMode=b;
  if (b && !fPlots) fPlots = new AliJetControlPlots(); 
}

////////////////////////////////////////////////////////////////////////
TTree* AliJetFinder::MakeTreeJ(char* name)
{
    // Create the tree for reconstructed jets
    fTreeJ = new TTree(name, "AliJet");
    fTreeJ->Branch("FoundJet",   &fJets,   1000);
    fTreeJ->Branch("GenJet",     &fGenJets,1000);
    fTreeJ->Branch("LeadingPart",&fLeading,1000);
    return fTreeJ;
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::WriteRHeaderToFile()
{
  // write reader header
    AliJetReaderHeader *rh = fReader->GetReaderHeader();
    rh->Write();
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::Run()
{
  // Do some initialization
  Init();
  // connect files
  fReader->OpenInputFiles();

  // write headers
  WriteHeaders();
  // loop over events
  Int_t nFirst, nLast, option, debug, arrayInitialised; 
  nFirst = fReader->GetReaderHeader()->GetFirstEvent();
  nLast  = fReader->GetReaderHeader()->GetLastEvent();

  option = fReader->GetReaderHeader()->GetDetector();
  debug  = fReader->GetReaderHeader()->GetDebug();
  arrayInitialised = fReader->GetArrayInitialised();

  // loop over events
  for (Int_t i=nFirst;i<nLast;i++) {
      fReader->FillMomentumArray();
      fLeading->FindLeading(fReader);
      fReader->GetGenJets(fGenJets);

      if (option == 0) { // TPC with fMomentumArray
	  if(debug > 1) 
	      printf("In FindJetsC() routine: find jets with fMomentumArray !!!\n");
	  FindJetsC();
      } else {
	if(debug > 1) printf("In FindJets() routine: find jets with fUnitArray !!!\n");
	FindJets();
      }
      if (fOut) {
	  fOut->cd();
      }
      
      if (fPlots) fPlots->FillHistos(fJets);
      fLeading->Reset();
      fGenJets->ClearJets();
      Reset();
  } 

  // write out
  if (fPlots) {
      fPlots->Normalize();
      fPlots->PlotHistos();
  }
  if (fOut) {
      fOut->cd();
      fPlots->Write();
      fOut->Close();
  }
}


//
// The following methods have been added to allow for event steering from the outside
//

////////////////////////////////////////////////////////////////////////
void AliJetFinder::ConnectTree(TTree* tree, TObject* data)
{
    // Connect the input file
    fReader->ConnectTree(tree, data);
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::WriteHeaders()
{
    // Write the Headers
    TFile* f = new TFile("jets_local.root", "recreate");
    WriteRHeaderToFile();
    WriteJHeaderToFile();
    f->Close();
}

////////////////////////////////////////////////////////////////////////
Bool_t AliJetFinder::ProcessEvent()
{
  //
  // Process one event
  // Charged only jets
  //

  Bool_t ok = fReader->FillMomentumArray();
  if (!ok) return kFALSE;
  
  // Leading particles
  fLeading->FindLeading(fReader);
  // Jets
  FindJets(); // V1
  //  FindJetsC(); // V2
  
  if (fPlots) fPlots->FillHistos(fJets);
  fLeading->Reset();
  fGenJets->ClearJets();
  Reset();  
  return kTRUE;
}

////////////////////////////////////////////////////////////////////////
Bool_t AliJetFinder::ProcessEvent2()
{
  //
  // Process one event
  // Charged only or charged+neutral jets
  //

  TRefArray* ref = new TRefArray();
  Bool_t procid = kFALSE;
  Bool_t ok = fReader->ExecTasks(procid,ref);

  // Delete reference pointer  
  if (!ok) {delete ref; return kFALSE;}
  
  // Leading particles
  fLeading->FindLeading(fReader);
  // Jets
  FindJets();
  
  Int_t         nEntRef    = ref->GetEntries();
  vector<Float_t> vtmp;

  for(Int_t i=0; i<nEntRef; i++)
    { 
      // Reset the UnitArray content which were referenced
      ((AliJetUnitArray*)ref->At(i))->SetUnitTrackID(0);
      ((AliJetUnitArray*)ref->At(i))->SetUnitEnergy(0.);
      ((AliJetUnitArray*)ref->At(i))->SetUnitCutFlag(kPtSmaller);
      ((AliJetUnitArray*)ref->At(i))->SetUnitCutFlag2(kPtSmaller);
      ((AliJetUnitArray*)ref->At(i))->SetUnitPxPyPz(kTRUE,vtmp);
      ((AliJetUnitArray*)ref->At(i))->SetUnitSignalFlag(kBad);
      ((AliJetUnitArray*)ref->At(i))->SetUnitDetectorFlag(kTpc);
      ((AliJetUnitArray*)ref->At(i))->SetUnitFlag(kOutJet);

      // Reset process ID
      AliJetUnitArray* uA = (AliJetUnitArray*)ref->At(i);
      uA->ResetBit(kIsReferenced);
      uA->SetUniqueID(0);     
    }

  // Delete the reference pointer
  ref->Delete();
  delete ref;

  if (fPlots) fPlots->FillHistos(fJets);
  fLeading->Reset();
  fGenJets->ClearJets();
  Reset();

  return kTRUE;
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::FinishRun()
{
    // Finish a run
    if (fPlots) {
	fPlots->Normalize();
	fPlots->PlotHistos();
    }
    
    if (fOut) {
	 fOut->cd();
	 if (fPlots) {
	     fPlots->Write();
	 }
	 fOut->Close();
    }
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::AddJet(AliAODJet p)
{
// Add new jet to the list
  new ((*fAODjets)[fNAODjets++]) AliAODJet(p);
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::ConnectAOD(AliAODEvent* aod)
{
// Connect to the AOD
    fAODjets = aod->GetJets();
}

////////////////////////////////////////////////////////////////////////
void AliJetFinder::ConnectAODNonStd(AliAODEvent* aod,const char *bname)
{

  fAODjets = dynamic_cast<TClonesArray*>(aod->FindListObject(bname));
  // how is this is reset? Cleared?
}

