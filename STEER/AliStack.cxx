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

/*
$Log$
Revision 1.5  2001/05/30 12:18:46  hristov
Loop variables declared once

Revision 1.4  2001/05/25 07:25:20  hristov
AliStack destructor corrected (I.Hrivnacova)

Revision 1.3  2001/05/22 14:33:16  hristov
Minor changes

Revision 1.2  2001/05/17 05:49:39  fca
Reset pointers to daughters

Revision 1.1  2001/05/16 14:57:22  alibrary
New files for folders and Stack

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Particles stack class
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
 
#include <TObjArray.h>
#include <TParticle.h>
#include <TTree.h>
#include <TFile.h>
#include <TFolder.h>
#include <TROOT.h>

#include "AliStack.h"
#include "AliRun.h"
#include "AliModule.h"
#include "AliHit.h"
//#include "ETrackBits.h"

ClassImp(AliStack)

//_____________________________________________________________________________
AliStack::AliStack(Int_t size)
{
  //
  //  Constructor
  //
  
  // Create the particles arrays 
  fParticles      = new TClonesArray("TParticle",1000);
  fParticleMap    = new TObjArray(size);
  fParticleBuffer = 0;
  fNtrack         =  0;
  fNprimary      =  0;
  fCurrent        = -1;
  fCurrentPrimary = -1;
  fTreeK          =  0;
}


//_____________________________________________________________________________
AliStack::AliStack()
{
  //
  //  Default constructor
  //
  
  // Create the particles arrays 
  fParticles      = new TClonesArray("TParticle",1000);
  fParticleMap    = new TObjArray(10000);
  fParticleBuffer = 0;
  fNtrack         =  0;
  fCurrent        = -1;
  fNprimary      =  0;
  fCurrentPrimary = -1;
  fTreeK          =  0;
}


//_____________________________________________________________________________
AliStack::~AliStack()
{
  //
  // Destructor
  //
  
  delete fParticleBuffer;
  if (fParticles) {
    fParticles->Delete();
    delete fParticles;
  }
  delete fParticleMap;
  delete fTreeK;
}

//
// public methods
//

//_____________________________________________________________________________
void AliStack::SetTrack(Int_t done, Int_t parent, Int_t pdg, Float_t *pmom,
		      Float_t *vpos, Float_t *polar, Float_t tof,
		      AliMCProcess mech, Int_t &ntr, Float_t weight)
{ 
  //
  // Load a track on the stack
  //
  // done     0 if the track has to be transported
  //          1 if not
  // parent   identifier of the parent track. -1 for a primary
  // pdg    particle code
  // pmom     momentum GeV/c
  // vpos     position 
  // polar    polarisation 
  // tof      time of flight in seconds
  // mecha    production mechanism
  // ntr      on output the number of the track stored
  //

  Float_t mass;
  const Int_t kfirstdaughter=-1;
  const Int_t klastdaughter=-1;
  const Int_t kS=0;
  //  const Float_t tlife=0;
  
  //
  // Here we get the static mass
  // For MC is ok, but a more sophisticated method could be necessary
  // if the calculated mass is required
  // also, this method is potentially dangerous if the mass
  // used in the MC is not the same of the PDG database
  //
  mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();
  Float_t e=TMath::Sqrt(mass*mass+pmom[0]*pmom[0]+
			pmom[1]*pmom[1]+pmom[2]*pmom[2]);
  
//  printf("Loading  mass %f ene %f No %d ip %d parent %d done %d pos %f %f %f mom %f %f %f kS %d m \n",
//  mass,e,fNtrack,pdg,parent,done,vpos[0],vpos[1],vpos[2],pmom[0],pmom[1],pmom[2],kS);

  TClonesArray &particles = *fParticles;
  TParticle* particle
   = new(particles[fLoadPoint++]) 
     TParticle(pdg, kS, parent, -1, kfirstdaughter, klastdaughter,
               pmom[0], pmom[1], pmom[2], e, vpos[0], vpos[1], vpos[2], tof);
  particle->SetPolarisation(TVector3(polar[0],polar[1],polar[2]));
  particle->SetWeight(weight);
  particle->SetUniqueID(mech);
  if(!done) particle->SetBit(kDoneBit);
  
  
  //  Declare that the daughter information is valid
  particle->SetBit(kDaughtersBit);
  //  Add the particle to the stack
  fParticleMap->AddAtAndExpand(particle, fNtrack);

  if(parent>=0) {
    particle = (TParticle*) fParticleMap->At(parent);
    particle->SetLastDaughter(fNtrack);
    if(particle->GetFirstDaughter()<0) particle->SetFirstDaughter(fNtrack);
  } 
  else { 
    //
    // This is a primary track. Set high water mark for this event
    fHgwmk = fNtrack;
    //
    // Set also number if primary tracks
    fNprimary = fHgwmk+1;
    fCurrentPrimary++;
  }
  ntr = fNtrack++;
}

//_____________________________________________________________________________
void AliStack::SetTrack(Int_t done, Int_t parent, Int_t pdg,
  	              Double_t px, Double_t py, Double_t pz, Double_t e,
  		      Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		      Double_t polx, Double_t poly, Double_t polz,
		      AliMCProcess mech, Int_t &ntr, Float_t weight)
{ 
  //
  // Load a track on the stack
  //
  // done        0 if the track has to be transported
  //             1 if not
  // parent      identifier of the parent track. -1 for a primary
  // pdg         particle code
  // kS          generation status code
  // px, py, pz  momentum GeV/c
  // vx, vy, vz  position 
  // polar       polarisation 
  // tof         time of flight in seconds
  // mech        production mechanism
  // ntr         on output the number of the track stored
  //    
  // New method interface: 
  // arguments were changed to be in correspondence with TParticle
  // constructor.
  // Note: the energy is not calculated from the static mass but
  // it is passed by argument e.


  const Int_t kS=0;
  const Int_t kFirstDaughter=-1;
  const Int_t kLastDaughter=-1;
  
  TClonesArray &particles = *fParticles;
  TParticle* particle
    = new(particles[fLoadPoint++]) 
      TParticle(pdg, kS, parent, -1, kFirstDaughter, kLastDaughter,
		px, py, pz, e, vx, vy, vz, tof);
   
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(mech);

  if(!done) particle->SetBit(kDoneBit);

  //  Declare that the daughter information is valid
  particle->SetBit(kDaughtersBit);
  //  Add the particle to the stack
  fParticleMap->AddAtAndExpand(particle, fNtrack);//CHECK!!

  if(parent>=0) {
    particle = (TParticle*) fParticleMap->At(parent);
    particle->SetLastDaughter(fNtrack);
    if(particle->GetFirstDaughter()<0) particle->SetFirstDaughter(fNtrack);
  } 
  else { 
    //
    // This is a primary track. Set high water mark for this event
    fHgwmk = fNtrack;
    //
    // Set also number if primary tracks
    fNprimary = fHgwmk+1;
    fCurrentPrimary++;
  }
  ntr = fNtrack++;
}

//_____________________________________________________________________________
void AliStack::GetNextTrack(Int_t &mtrack, Int_t &ipart, Float_t *pmom,
			  Float_t &e, Float_t *vpos, Float_t *polar,
			  Float_t &tof)
{
  //
  // Return next track from stack of particles
  //
  

  TParticle* track = GetNextParticle();
//  cout << "GetNextTrack():" << fCurrent << fNprimary << endl;

  if(track) {
    mtrack=fCurrent;
    ipart=track->GetPdgCode();
    pmom[0]=track->Px();
    pmom[1]=track->Py(); 
    pmom[2]=track->Pz();
    e      =track->Energy();
    vpos[0]=track->Vx();
    vpos[1]=track->Vy();
    vpos[2]=track->Vz();
    TVector3 pol;
    track->GetPolarisation(pol);
    polar[0]=pol.X();
    polar[1]=pol.Y();
    polar[2]=pol.Z();
    tof=track->T();
    track->SetBit(kDoneBit);
    //cout << "Filled params" << endl;
  }
  else 
    mtrack=-1;

  //
  // stop and start timer when we start a primary track
  Int_t nprimaries = fNprimary;
  if (fCurrent >= nprimaries) return;
  if (fCurrent < nprimaries-1) { 
    fTimer.Stop();
    track=(TParticle*) fParticleMap->At(fCurrent+1);
    //    track->SetProcessTime(fTimer.CpuTime());
  }
  fTimer.Start();
}


//_____________________________________________________________________________
void AliStack::PurifyKine()
{
  //
  // Compress kinematic tree keeping only flagged particles
  // and renaming the particle id's in all the hits
  //

  TObjArray &particles = *fParticleMap;
  int nkeep=fHgwmk+1, parent, i;
  TParticle *part, *father;
  TArrayI map(particles.GetLast()+1);

  // Save in Header total number of tracks before compression

  // If no tracks generated return now
  if(fHgwmk+1 == fNtrack) return;

  // First pass, invalid Daughter information
  for(i=0; i<fNtrack; i++) {
    // Preset map, to be removed later
    if(i<=fHgwmk) map[i]=i ; 
    else {
      map[i] = -99;
      if((part=(TParticle*) particles.At(i))) { 
          part->ResetBit(kDaughtersBit);
          part->SetFirstDaughter(-1);
          part->SetLastDaughter(-1);
      }
    }
  }
  // Invalid daughter information for the parent of the first particle
  // generated. This may or may not be the current primary according to
  // whether decays have been recorded among the primaries
  part = (TParticle *)particles.At(fHgwmk+1);
  particles.At(part->GetFirstMother())->ResetBit(kDaughtersBit);
  // Second pass, build map between old and new numbering
  for(i=fHgwmk+1; i<fNtrack; i++) {
    if(particles.At(i)->TestBit(kKeepBit)) {
      
      // This particle has to be kept
      map[i]=nkeep;
      // If old and new are different, have to move the pointer
      if(i!=nkeep) particles[nkeep]=particles.At(i);
      part = (TParticle*) particles.At(nkeep);
      
      // as the parent is always *before*, it must be already
      // in place. This is what we are checking anyway!
      if((parent=part->GetFirstMother())>fHgwmk) 
	if(map[parent]==-99) Fatal("PurifyKine","map[%d] = -99!\n",parent);
	else part->SetFirstMother(map[parent]);

      nkeep++;
    }
  }
  
  // Fix daughters information
  for (i=fHgwmk+1; i<nkeep; i++) {
    part = (TParticle *)particles.At(i);
    parent = part->GetFirstMother();
    if(parent>=0) {
      father = (TParticle *)particles.At(parent);
      if(father->TestBit(kDaughtersBit)) {
      
	if(i<father->GetFirstDaughter()) father->SetFirstDaughter(i);
	if(i>father->GetLastDaughter())  father->SetLastDaughter(i);
      } else {
	// Initialise daughters info for first pass
	father->SetFirstDaughter(i);
	father->SetLastDaughter(i);
	father->SetBit(kDaughtersBit);
      }
    }
  }
  
  // Now loop on all registered hit lists
  TList* hitLists = gAlice->GetHitLists();
  TIter next(hitLists);
  TCollection *hitList;
  while((hitList = (TCollection*)next())) {
    TIter nexthit(hitList);
    AliHit *hit;
    while((hit = (AliHit*)nexthit())) {
      hit->SetTrack(map[hit->GetTrack()]);
    }
  }

  // 
  // This for detectors which have a special mapping mechanism
  // for hits, such as TPC and TRD
  //

   TObjArray* modules = gAlice->Modules();
   TIter nextmod(modules);
   AliModule *detector;
   while((detector = (AliModule*)nextmod())) {
     detector->RemapTrackHitIDs(map.GetArray());
   }
  
   // Now the output bit, from fHgwmk to nkeep we write everything and we erase
   if(nkeep>fParticleFileMap.GetSize()) fParticleFileMap.Set(Int_t (nkeep*1.5));

   for (i=fHgwmk+1; i<nkeep; ++i) {
     fParticleBuffer = (TParticle*) particles.At(i);
     fParticleFileMap[i]=(Int_t) fTreeK->GetEntries();
     fTreeK->Fill();
     particles[i]=fParticleBuffer=0;
   }

   for (i=nkeep; i<fNtrack; ++i) particles[i]=0;

   Int_t toshrink = fNtrack-fHgwmk-1;
   fLoadPoint-=toshrink;
   for(i=fLoadPoint; i<fLoadPoint+toshrink; ++i) fParticles->RemoveAt(i);

   fNtrack=nkeep;
   fHgwmk=nkeep-1;
   //   delete [] map;
}

//_____________________________________________________________________________
void AliStack::FinishEvent()
{
  //
  // Write out the kinematics that was not yet filled
  //
  
  // Update event header


  if (!fTreeK) {
//    Fatal("FinishEvent", "No kinematics tree is defined.");
//    Don't panic this is a probably a lego run
      return;
      
  }  
  
  CleanParents();
  if(fTreeK->GetEntries() ==0) {
    // set the fParticleFileMap size for the first time
    fParticleFileMap.Set(fHgwmk+1);
  }

  Bool_t allFilled = kFALSE;
  TObject *part;
  for(Int_t i=0; i<fHgwmk+1; ++i) 
    if((part=fParticleMap->At(i))) {
      fParticleBuffer = (TParticle*) part;
      fParticleFileMap[i]= (Int_t) fTreeK->GetEntries();
      fTreeK->Fill();
      (*fParticleMap)[i]=fParticleBuffer=0;      
      
      // When all primaries were filled no particle!=0
      // should be left => to be removed later.
      if (allFilled) printf("Why != 0 part # %d?\n",i);
    }
    else {
      // // printf("Why = 0 part # %d?\n",i); => We know.
      // break;
         // we don't break now in order to be sure there is no
	 // particle !=0 left.
	 // To be removed later and replaced with break.
      if(!allFilled) allFilled = kTRUE;
    } 
  //cout << "Nof particles: " << fNtrack << endl;
  //Reset();   
} 

//_____________________________________________________________________________
void AliStack::FlagTrack(Int_t track)
{
  //
  // Flags a track and all its family tree to be kept
  //
  
  TParticle *particle;

  Int_t curr=track;
  while(1) {
    particle=(TParticle*)fParticleMap->At(curr);
    
    // If the particle is flagged the three from here upward is saved already
    if(particle->TestBit(kKeepBit)) return;
    
    // Save this particle
    particle->SetBit(kKeepBit);
    
    // Move to father if any
    if((curr=particle->GetFirstMother())==-1) return;
  }
}
 
//_____________________________________________________________________________
void AliStack::KeepTrack(Int_t track)
{ 
  //
  // Flags a track to be kept
  //
  
  fParticleMap->At(track)->SetBit(kKeepBit);
}

//_____________________________________________________________________________
void  AliStack::Reset(Int_t size) 
{
  //
  // Resets stack
  //

  fNtrack=0;
  fNprimary=0;
  fHgwmk=0;
  fLoadPoint=0;
  fCurrent = -1;
  ResetArrays(size);
}

//_____________________________________________________________________________
void  AliStack::ResetArrays(Int_t size) 
{
  //
  // Resets stack arrays
  //

  fParticles->Clear();
  fParticleMap->Clear();
  if (size>0) fParticleMap->Expand(size);  
}

//_____________________________________________________________________________
void AliStack::SetHighWaterMark(Int_t nt)
{
  //
  // Set high water mark for last track in event
  //
  
  fHgwmk = fNtrack-1;
  fCurrentPrimary=fHgwmk;
  
  // Set also number of primary tracks
  fNprimary = fHgwmk+1;
  fNtrack   = fHgwmk+1;      
}

//_____________________________________________________________________________
TParticle* AliStack::Particle(Int_t i)
{
  //
  // Return particle with specified ID
  
  if(!(*fParticleMap)[i]) {
    Int_t nentries = fParticles->GetEntries();
    // algorithmic way of getting entry index
    // (primary particles are filled after secondaries)
    Int_t entry;
    if (i<fNprimary)
	entry = i+fNtrack-fNprimary;
    else 
	entry = i-fNprimary;
    // check whether algorithmic way and 
    // and the fParticleFileMap[i] give the same;
    // give the fatal error if not
    if (entry != fParticleFileMap[i]) {
      Fatal("Particle",
        "!! The algorithmic way and map are different: !!\n entry: %d map: %d",
	entry, fParticleFileMap[i]); 
    } 
      
    fTreeK->GetEntry(entry);
    new ((*fParticles)[nentries]) TParticle(*fParticleBuffer);
    fParticleMap->AddAt((*fParticles)[nentries],i);
  }
  return (TParticle *) (*fParticleMap)[i];
}

//_____________________________________________________________________________
Int_t AliStack::GetPrimary(Int_t id) const
{
  //
  // Return number of primary that has generated track
  //
  
  int current, parent;
  TParticle *part;
  //
  parent=id;
  while (1) {
    current=parent;
    part = (TParticle *)fParticleMap->At(current);
    parent=part->GetFirstMother();
    if(parent<0) return current;
  }
}
 
//_____________________________________________________________________________
void AliStack::DumpPart (Int_t i) const
{
  //
  // Dumps particle i in the stack
  //
  
  ((TParticle*) (*fParticleMap)[i])->Print();
}

//_____________________________________________________________________________
void AliStack::DumpPStack ()
{
  //
  // Dumps the particle stack
  //

  Int_t i;

  printf(
	 "\n\n=======================================================================\n");
  for (i=0;i<fNtrack;i++) 
    {
      TParticle* particle = Particle(i);
      if (particle) {
        printf("-> %d ",i); particle->Print();
        printf("--------------------------------------------------------------\n");
      }
      else 
        Warning("DumpPStack", "No particle with id %d.", i); 
    }	 

  printf(
	 "\n=======================================================================\n\n");
  
  // print  particle file map
  printf("\nParticle file map: \n");
  for (i=0; i<fNtrack; i++) 
      printf("   %d th entry: %d \n",i,fParticleFileMap[i]);
}


//_____________________________________________________________________________
void AliStack::DumpLoadedStack() const
{
  //
  // Dumps the particle in the stack
  // that are loaded in memory.
  //

  TObjArray &particles = *fParticleMap;
  printf(
	 "\n\n=======================================================================\n");
  for (Int_t i=0;i<fNtrack;i++) 
    {
      TParticle* particle = (TParticle*) particles[i];
      if (particle) {
        printf("-> %d ",i); particle->Print();
        printf("--------------------------------------------------------------\n");
      }
      else { 	
        printf("-> %d  Particle not loaded.\n",i);
        printf("--------------------------------------------------------------\n");
      }	
    }
  printf(
	 "\n=======================================================================\n\n");
}

//
// protected methods
//

//_____________________________________________________________________________
void AliStack::CleanParents()
{
  //
  // Clean particles stack
  // Set parent/daughter relations
  //
  
  TObjArray &particles = *fParticleMap;
  TParticle *part;
  int i;
  for(i=0; i<fHgwmk+1; i++) {
    part = (TParticle *)particles.At(i);
    if(part) if(!part->TestBit(kDaughtersBit)) {
      part->SetFirstDaughter(-1);
      part->SetLastDaughter(-1);
    }
  }
}

//_____________________________________________________________________________
TParticle* AliStack::GetNextParticle()
{
  //
  // Return next particle from stack of particles
  //
  
  TParticle* particle = 0;
  
  // search secondaries
  //for(Int_t i=fNtrack-1; i>=0; i--) {
  for(Int_t i=fNtrack-1; i>fHgwmk; i--) {
      particle = (TParticle*) fParticleMap->At(i);
      if ((particle) && (!particle->TestBit(kDoneBit))) {
	  fCurrent=i;    
	  //cout << "GetNextParticle() - secondary " 
	  // << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
	  return particle;
      }   
  }    

  // take next primary if all secondaries were done
  while (fCurrentPrimary>=0) {
      fCurrent = fCurrentPrimary;    
      particle = (TParticle*) fParticleMap->At(fCurrentPrimary--);
      if ((particle) && (!particle->TestBit(kDoneBit))) {
	  //cout << "GetNextParticle() - primary " 
	  //   << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
	  return particle;
      } 
  }
  
  // nothing to be tracked
  fCurrent = -1;
    //cout << "GetNextParticle() - none  " 
    //   << fNtrack << " " << fHgwmk << " " << fCurrent << endl;
  return particle;  
}

//__________________________________________________________________________________________
void AliStack::MakeTree(Int_t event, const char *file)
{
//
//  Make Kine tree and creates branch for writing particles
//  
  TBranch *branch=0;
  // Make Kinematics Tree
  char hname[30];
  //    printf("\n MakeTree called %d", event);
  if (!fTreeK) {
    sprintf(hname,"TreeK%d",event);
    fTreeK = new TTree(hname,"Kinematics");
    //  Create a branch for particles
    branch = fTreeK->Branch("Particles", "TParticle", &fParticleBuffer, 4000, 1);          
    fTreeK->Write(0,TObject::kOverwrite);
  }
}

//_____________________________________________________________________________
void AliStack::BeginEvent(Int_t event)
{
// start a new event
//
//
    fNprimary = 0;
    fNtrack   = 0;
    
    char hname[30];
    if(fTreeK) {
	fTreeK->Reset();
	sprintf(hname,"TreeK%d",event);
	fTreeK->SetName(hname);
    }
}

//_____________________________________________________________________________
void AliStack::FinishRun()
{
// Clean TreeK information
    if (fTreeK) {
	delete fTreeK; fTreeK = 0;
    }
}

//_____________________________________________________________________________
void AliStack::GetEvent(Int_t event)
{
//
// Get new event from TreeK
    // Reset/Create the particle stack
    if (fTreeK) delete fTreeK;
    
    // Get Kine Tree from file
    char treeName[20];
    sprintf(treeName,"TreeK%d",event);
    fTreeK = (TTree*)gDirectory->Get(treeName);

    if (fTreeK) fTreeK->SetBranchAddress("Particles", &fParticleBuffer);

  else    
      Error("GetEvent","cannot find Kine Tree for event:%d\n",event);
//
//    printf("\n primaries %d", fNprimary);
//    printf("\n tracks    %d", fNtrack);    
//
    Int_t size = (Int_t)fTreeK->GetEntries();
    ResetArrays(size);
}
