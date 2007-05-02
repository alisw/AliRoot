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
//---------------------------------------------------------------------

#include <Riostream.h>
#include <TFile.h>
#include "AliJetFinder.h"
#include "AliJet.h"
#include "AliJetReader.h"
#include "AliJetReaderHeader.h"
#include "AliJetControlPlots.h"
#include "AliLeading.h"

ClassImp(AliJetFinder)

AliJetFinder::AliJetFinder():
    fTreeJ(0),
    fPlotMode(kFALSE),
    fJets(0),
    fGenJets(0),
    fLeading(0),
    fReader(0x0),
    fHeader(0x0),
    fPlots(0x0),
    fOut(0x0)
    
{
  // Constructor
  fJets    = new AliJet();
  fGenJets = new AliJet();
  fLeading = new AliLeading();
}

////////////////////////////////////////////////////////////////////////

AliJetFinder::~AliJetFinder()
{
  // destructor
  // here reset and delete jets
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
    fOut = new TFile("jets.root","recreate");
    fOut->cd();
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
  fOut->cd();
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
      fReader->FillMomentumArray(i);
      fLeading->FindLeading(fReader);
      fReader->GetGenJets(fGenJets);

      if (option == 0) { // TPC with fMomentumArray
	  if(debug > 1) 
	      printf("In FindJetsTPC() routine: find jets with fMomentumArray !!!\n");
	  FindJetsTPC();
      } else {
	   if(debug > 1) printf("In FindJets() routine: find jets with fUnitArray !!!\n");
	   FindJets();
      }
      if (fOut) {
	  fOut->cd();
	  fTreeJ->Fill();
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

void AliJetFinder::ConnectTree(TTree* tree, TObject* data)
{
    // Connect the input file
    fReader->ConnectTree(tree, data);
}

void AliJetFinder::WriteHeaders()
{
    // Write the Headers
    if (fOut) {
	fOut->cd();
	WriteRHeaderToFile();
	WriteJHeaderToFile();
    }
}


Bool_t AliJetFinder::ProcessEvent(Long64_t entry)
{
//
// Process one event
//
    Int_t debug  = fReader->GetReaderHeader()->GetDebug();
    if (debug > 0) printf("<<<<< Processing Event %5d >>>>> \n", (Int_t) entry);
    Bool_t ok = fReader->FillMomentumArray(entry);
    if (!ok) return kFALSE;
    fLeading->FindLeading(fReader);
    FindJets();
    if (fOut) {
	fOut->cd();
	fTreeJ->Fill();
    }
    
    if (fPlots) fPlots->FillHistos(fJets);
    fLeading->Reset();
    fGenJets->ClearJets();
    Reset();  
    return kTRUE;
}

void AliJetFinder::FinishRun()
{
    // Finish a run
    if (fPlots) {
	fPlots->Normalize();
	fPlots->PlotHistos();
    }
    
    if (fOut) {
	 fOut->cd();
	 fTreeJ->Write();
	 if (fPlots) {
	     fPlots->Write();
	 }
	 fOut->Close();
    }
}

