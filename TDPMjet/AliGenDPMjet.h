#ifndef ALIGENDPMJET_H
#define ALIGENDPMJET_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// Generator using DPMJET as an external generator
// The main DPMJET options are accessable for the user through this interface.

#include "AliGenMC.h"
#include <TString.h>
#include <TArrayI.h>

class TDPMjet;
class TArrayI;
class TParticle;
class TClonesArray;
class TGraph;

class AliGenDPMjet : public AliGenMC
{

 public:
    AliGenDPMjet();
    AliGenDPMjet(Int_t npart);
    AliGenDPMjet(const AliGenDPMjet &Dpmjet);
    virtual ~AliGenDPMjet(); 
    virtual void    Generate();
    virtual void    Init();

    virtual void    SetBeamEnergy(Float_t energy=5400.) {fBeamEn=energy;}
    virtual void    SetEnergyCMS(Float_t energy=5400.)   {fEnergyCMS=energy;}
    virtual void    SetProjectile(Int_t a=208, Int_t z=82)
			{fAProjectile = a; fZProjectile = z;}    
    virtual void    SetTarget(Int_t a=208, Int_t z=82)
			{fATarget = a; fZTarget = z;}    
    virtual void    SetImpactParameterRange(Float_t bmin=0., Float_t bmax=16.)
			{fMinImpactParam=bmin; fMaxImpactParam=bmax;}
    virtual void    SetCentral(Int_t icentr=-2) {fICentr = icentr;}
    virtual void    KeepFullEvent();
    virtual void    SetDecaysOff(Int_t flag=1)        {fDecaysOff  = flag;}
    virtual void    SetFlavor(Int_t flag=0)           {fFlavor     = flag;}
    virtual void    SetEvaluate(Int_t flag=0)         {fEvaluate   = flag;}
    virtual void    SetSelectAll(Int_t flag=0)        {fSelectAll  = flag;}
    virtual void    SetSpectators(Int_t spects=1)     {fSpectators = spects;}
    virtual void    SetBoostLHC(Int_t flag=0)         {fLHC        = flag;}
	    
    virtual Float_t GetEnergyCMS() {return fEnergyCMS;}
    virtual void    GetProjectile(Int_t& a, Int_t& z)
			{a = fAProjectile; z = fZProjectile;}    
    virtual void    GetTarget(Int_t& a, Int_t& z)
			{a = fATarget; z = fZTarget;}    
    virtual void    GetImpactParameterRange(Float_t& bmin, Float_t& bmax)
			{bmin = fMinImpactParam; bmax = fMaxImpactParam;}
    virtual Int_t   GetSpectators()        {return fSpectators;}
    virtual Int_t   GetFlavor()            {return fFlavor;}

    // Temporeaneo!?!
    virtual void    SetGenImpPar(Float_t bValue) {fGenImpPar=bValue;}
    virtual Float_t GetGenImpPar() {return fGenImpPar;}
    
    /*virtual void EvaluateCrossSections();
    virtual void Boost();
    virtual TGraph* CrossSection()     {return fDsigmaDb;}
    virtual TGraph* BinaryCollisions() {return fDnDb;}
    */

    AliGenDPMjet &  operator=(const AliGenDPMjet & rhs);

 protected:
    Bool_t SelectFlavor(Int_t pid);
    void   MakeHeader();

 protected:

    Int_t         fAProjectile;    // Projectile A
    Int_t         fZProjectile;    // Projectile Z
    Int_t         fATarget;        // Target A
    Int_t         fZTarget;        // Target Z
    Float_t       fBeamEn; 	   // beam energy
    Float_t       fEnergyCMS;      // Centre of mass energy
    Float_t       fMinImpactParam; // minimum impact parameter
    Float_t       fMaxImpactParam; // maximum impact parameter	
    Int_t	  fICentr;	   // Flag to force central production
    Float_t	  fCrossSec;	   // Fraction of x-section
    Int_t         fKeep;           // Flag to keep full event information
    Int_t         fDecaysOff;      // Flag to turn off decays of pi0, K_s, D, Lambda, sigma
    Int_t         fEvaluate;       // Evaluate total and partial cross-sections
    Int_t         fSelectAll;      // Flag to write the full event
    Int_t         fFlavor;         // Selected particle flavor 4: charm+beauty 5: beauty
    Int_t         fTrials;         // Number of trials
    TArrayI       fParentSelect;   // Parent particles to be selected 
    TArrayI       fChildSelect;    // Decay products to be selected
    Float_t       fXsection;       // Cross-section
    Int_t         fSpectators;     // put spectators on stack
    Int_t 	  fSpecn;	   // Num. of spectator neutrons
    Int_t 	  fSpecp;	   // Num. of spectator protons
    TDPMjet      *fDPMjet;         // DPMjet
    TGraph*       fDsigmaDb;       // dSigma/db for the system
    TGraph*       fDnDb;           // dNBinaryCollisions/db
    TClonesArray *fParticles;      // Particle List
    Int_t         fNoGammas;       // Don't write gammas if flag "on"
    Int_t         fLHC;            // Assume LHC as lab frame
    // Temporaneo!
    Float_t	  fGenImpPar;	   // GeneratedImpactParameter
    
 private:
    // adjust the weight from kinematic cuts
    void   AdjustWeights();
    // check seleted daughters
    Bool_t DaughtersSelection(TParticle* iparticle);
    // check if stable
    Bool_t Stable(TParticle*  particle);
    
    ClassDef(AliGenDPMjet,1) // AliGenerator interface to DPMJET
};
#endif





