#ifndef ALI_STACK_H
#define ALI_STACK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TObject.h>
#include <TArrayI.h>
#include <TStopwatch.h>

#include "AliMCProcess.h"

class TObjArray;
class TClonesArray;
class TParticle;
class AliHeader;
class TFile;
class TTree;



class AliStack : public TObject
{
  public:
    // creators, destructors
    AliStack(Int_t size);
    AliStack();
    virtual ~AliStack();

    // methods
    void  MakeTree(Int_t event, const char *file);
    void  BeginEvent(Int_t event);
    void  FinishRun();
    Bool_t GetEvent(Int_t nevent);
    void  SetTrack(Int_t done, Int_t parent, Int_t pdg, 
  	           Float_t *pmom, Float_t *vpos, Float_t *polar, 
                   Float_t tof, AliMCProcess mech, Int_t &ntr,
                   Float_t weight = 1, Int_t is = 0);
    void  SetTrack(Int_t done, Int_t parent, Int_t pdg,
  	           Double_t px, Double_t py, Double_t pz, Double_t e,
  		   Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		   Double_t polx, Double_t poly, Double_t polz,
		   AliMCProcess mech, Int_t &ntr, Float_t weight = 1,
		   Int_t is = 0);
    void  GetNextTrack(Int_t &mtrack, Int_t &ipart, Float_t *pmom,
		   Float_t &e, Float_t *vpos, Float_t *polar, Float_t &tof);
    void  PurifyKine();
    void  FinishEvent();
    void  FlagTrack(Int_t track);
    void  KeepTrack(Int_t itrack); 
    void  Reset(Int_t size = 0);
    void  DumpPart(Int_t i) const;
    void  DumpPStack ();
    void  DumpLoadedStack () const;

    // set methods
    void  SetNtrack(Int_t ntrack);
    void  SetCurrentTrack(Int_t track);                           
    void  SetHighWaterMark(Int_t hgwmk);    
    // get methods
    Int_t       GetNtrack() const;
    Int_t       GetNprimary() const;
    Int_t       CurrentTrack() const;
    TObjArray*  Particles() const;
    TParticle*  Particle(Int_t id);
    Int_t       GetPrimary(Int_t id);
    TTree*      TreeK() const {return fTreeK;}
    
  protected:
    // methods
    void  CleanParents();
    void  ResetArrays(Int_t size);
    TParticle* GetNextParticle();
    
  private:
    // data members
    TClonesArray  *fParticles;         //! Pointer to list of particles
    TObjArray     *fParticleMap;       //! Map of particles in the supporting TClonesArray
    TArrayI        fParticleFileMap;   //  Map for particle ids 
    TParticle     *fParticleBuffer;    //! Pointer to current particle for writing
    TTree         *fTreeK;             //! Particle stack  
    Int_t          fNtrack;            //  Number of tracks
    Int_t          fNprimary;          //  Number of primaries
    Int_t          fCurrent;           //! Last track returned from the stack
    Int_t          fCurrentPrimary;    //! Last primary track returned from the stack
    Int_t          fHgwmk;             //! Last track purified
    Int_t          fLoadPoint;         //! Next free position in the particle buffer
    TStopwatch     fTimer;             //! Timer object
    ClassDef(AliStack,2) //Particles stack
};

// inline

inline void  AliStack::SetNtrack(Int_t ntrack)
{ fNtrack = ntrack; }

inline void  AliStack::SetCurrentTrack(Int_t track)
{ fCurrent = track; }

inline Int_t AliStack::GetNtrack() const
{ return fNtrack; }

inline Int_t AliStack::GetNprimary() const
{ return fNprimary; }

inline Int_t AliStack::CurrentTrack() const 
{ return fCurrent; }

inline TObjArray* AliStack::Particles() const
{ return fParticleMap; }

#endif //ALI_STACK_H
