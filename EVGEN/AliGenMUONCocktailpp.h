#ifndef ALIGENMUONCOCKTAILPP_H
#define ALIGENMUONCOCKTAILPP_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "AliGenCocktail.h"
#include "AliDecayer.h"

class AliGenCocktailEntry;

class AliGenMUONCocktailpp : public AliGenCocktail
{
 public:

    AliGenMUONCocktailpp();
    virtual ~AliGenMUONCocktailpp();    
    virtual void Init();
    virtual void CreateCocktail();
    virtual void Generate();    
    Int_t   GetNSucceded()         const {return fNSucceded;}    
    Int_t   GetNGenerated()        const {return fNGenerated;}
    Int_t   GetMuonMultiplicity()  const {return fMuonMultiplicity;}
    Float_t GetMuonPtCut()         const {return fMuonPtCut;}
    Float_t GetMuonPCut()          const {return fMuonPCut;}    
    Float_t GetMuonThetaMin()      const {return fMuonThetaMinCut;}
    Float_t GetMuonThetaMax()      const {return fMuonThetaMaxCut;}	    
    Float_t GetMuonOriginCut()     const {return fMuonOriginCut;}	    
    Float_t GetDecayModeResonance()const {return fDecayModeResonance;}
    Float_t GetDecayModePythia()   const {return fDecayModePythia;}
    
    void    SetMuonMultiplicity(Int_t MuonMultiplicity) { fMuonMultiplicity = MuonMultiplicity;}
    void    SetMuonPtCut(Float_t PtCut) { fMuonPtCut = PtCut;}
    void    SetMuonPCut(Float_t PCut) { fMuonPCut = PCut;}    
    void    SetMuonOriginCut(Float_t originCut) { fMuonOriginCut = originCut;}
    void    SetMuonThetaRange(Float_t ThetaMin, Float_t ThetaMax){
	fMuonThetaMinCut=ThetaMin;
	fMuonThetaMaxCut=ThetaMax; }    
    void    SetDecayer(AliDecayer* decayer){fDecayer = decayer;}
    void    SetDecayModeResonance(Decay_t decay){ fDecayModeResonance = decay;}
    void    SetDecayModePythia(Decay_t decay){ fDecayModePythia = decay;}
    void    SetResPolarization(Double_t JpsiPol, Double_t PsiPPol, Double_t UpsPol, 
    				Double_t UpsPPol, Double_t UpsPPPol, char *PolFrame);

    
    void    SetCMSEnergy(Int_t einc)      { fCMSEnergy = einc; }
    void    SetSigmaReaction(Double_t sig)      { fSigmaReaction = sig; }    
    void    SetSigmaJPsi(Double_t sig)      { fSigmaJPsi = sig; }
    void    SetSigmaChic1(Double_t sig)      { fSigmaChic1 = sig; }
    void    SetSigmaChic2(Double_t sig)      { fSigmaChic2 = sig; }
    void    SetSigmaPsiP(Double_t sig)      { fSigmaPsiP = sig; }
    void    SetSigmaUpsilon(Double_t sig)   { fSigmaUpsilon = sig; }
    void    SetSigmaUpsilonP(Double_t sig)  { fSigmaUpsilonP = sig; }
    void    SetSigmaUpsilonPP(Double_t sig) { fSigmaUpsilonPP = sig; }
    void    SetSigmaCCbar(Double_t sig)     { fSigmaCCbar = sig; }
    void    SetSigmaBBbar(Double_t sig)     { fSigmaBBbar = sig; }

    void    SetSigmaSilent() { fSigmaSilent = kTRUE; }
 protected:

    //
 private:
    AliGenMUONCocktailpp(const AliGenMUONCocktailpp &cocktail); 
    AliGenMUONCocktailpp & operator=(const AliGenMUONCocktailpp &cocktail); 
    AliDecayer* fDecayer;
    Decay_t fDecayModeResonance; //decay mode in which resonances are forced to decay, default: kAll
    Decay_t fDecayModePythia; //decay mode in which particles in Pythia are forced to decay, default: kAll
    Int_t   fMuonMultiplicity; // Muon multiplicity for the primordial trigger
    Float_t fMuonPtCut;// Transverse momentum cut for muons
    Float_t fMuonPCut;// Momentum cut for muons    
    Float_t fMuonThetaMinCut;// Minimum theta cut for muons
    Float_t fMuonThetaMaxCut; // Maximum theta cut for muons
    Float_t fMuonOriginCut; //use only muons whose "part->Vz()" value is larger than fMuonOrigin
    Int_t   fNSucceded;// Number of Succes in the (di)-muon generation in the acceptance
    Int_t   fNGenerated;// Number of generated cocktails
    Double_t fJpsiPol, fChic1Pol, fChic2Pol, fPsiPPol, fUpsPol, fUpsPPol, fUpsPPPol;//Resonances polarization parameters
    Int_t    fPolFrame;//Resonances polarization frame (Collins-Soper / Helicity)
    Int_t fCMSEnergy; // CMS beam energy
    Double_t fSigmaReaction; //  cross-section pp
    Double_t fSigmaJPsi;      // cross-section JPsi resonance
    Double_t fSigmaChic1;      // cross-section Chic1 resonance
    Double_t fSigmaChic2;      // cross-section Chic2 resonance    
    Double_t fSigmaPsiP;      // cross-section Psi-prime resonance
    Double_t fSigmaUpsilon;   // cross-section Upsilon resonance
    Double_t fSigmaUpsilonP;  // cross-section Upsilon-prime resonance
    Double_t fSigmaUpsilonPP; // cross-section Upsilon-double-prime resonance
    Double_t fSigmaCCbar;     // cross-section correlated charm
    Double_t fSigmaBBbar;     // cross-section correlated beauty
    Bool_t   fSigmaSilent;    // hide values of cross-sections in output

    ClassDef(AliGenMUONCocktailpp,4)  //  cocktail for physics in the Alice
};

#endif



