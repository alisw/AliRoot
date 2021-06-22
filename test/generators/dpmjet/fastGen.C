#if !defined(__CINT__) || defined(__MAKECINT__)
#include <Riostream.h>
#include <TStopwatch.h>
#include <TRandom.h>
// #include <TDatabasePDG.h>
#include <TTree.h>

#include "AliGenerator.h"
// #include "AliPDG.h"
#include "AliRunLoader.h"
#include "AliRun.h"
#include "AliStack.h"
#include "AliHeader.h"
#include "AliGenDPMjet.h"

using std::cout;
using std::endl;
#endif

void fastGen(Int_t nev = 1, const char* filename = "galice.root")
{
  // If we want to run with Root only, we need the line below
  new AliRun("gAlice","The ALICE Off-line Simulation Framework");

  // AliPDG::AddParticlesToPdgDataBase();
  // TDatabasePDG::Instance();
 
  //=======================================================================
  // Set Random Number seed
  gRandom->SetSeed(0); // Set 0 to use the current time
  cout<<"Seed for random number generation= "<<gRandom->GetSeed()<<endl; 

  //=======================================================================
  // Prepare the simulation environment
  
  // Create run loader
  AliRunLoader* rl = AliRunLoader::Open(filename,"FASTRUN","recreate");
  
  rl->SetCompressionLevel(2);
  rl->SetNumberOfEventsPerFile(nev);
  rl->LoadKinematics("RECREATE");
  rl->MakeTree("E");
  gAlice->SetRunLoader(rl);
  
  // Create stack
  rl->MakeStack();
  AliStack* stack = rl->Stack();
  
  // Create header
  AliHeader* header = rl->GetHeader();
  
  // Create and Initialize Generator

  AliGenDPMjet *gener = new AliGenDPMjet(-1);
  gener->SetEnergyCMS(5500.);
  gener->SetProjectile("Pb",208,82);
  gener->SetTarget("Pb",208,82);
  gener->SetMomentumRange(0,999999);
  gener->SetPhiRange(0., 360.);
  gener->SetThetaRange(0.,180.);
  gener->SetProcess(kDpmMb); // Correct multiplicity, wrong Pt
  gener->SetStack(stack);
  gener->Init();

  // Go to galice.root
  rl->CdGAFile();

  //=======================================================================
  // Main event loop
  TStopwatch timer;
  timer.Start();
  for (Int_t iev = 0; iev < nev; iev++) { // Event Loop
    
    cout <<"Event number "<< iev << endl;
    
    // Initialize event
    header->Reset(0,iev);
    rl->SetEventNumber(iev);
    stack->Reset();
    rl->MakeTree("K");
    stack->ConnectTree(rl->TreeK());
    
    //---------------------------------------------------------------------
    // Generate event

    gener->Generate();
      
    // Finish event
    header->SetNprimary(stack->GetNprimary());
    header->SetNtrack(stack->GetNtrack());  
    
    // I/O
    stack->FinishEvent();
    header->SetStack(stack);
    rl->TreeE()->Fill();
    rl->WriteKinematics("OVERWRITE");
    
  } // End of event loop
  timer.Stop();
  timer.Print();
  
  // Termination

  // Generator
  gener->FinishRun();
  // Write file
  rl->WriteHeader("OVERWRITE");
  gener->Write();
  rl->Write();
}
