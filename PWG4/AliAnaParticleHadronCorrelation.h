#ifndef AliAnaParticleHadronCorrelation_H
#define AliAnaParticleHadronCorrelation_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id:  $ */

/* History of cvs commits:
 *
 * $Log$
 *
 *
 */

//_________________________________________________________________________
// Class that contains the algorithm for the analysis of particle - hadron correlations
// Particle (for example direct gamma) must be found in a previous analysis 
//-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---
class TH2F;

// --- Analysis system ---
#include "AliAnaBaseClass.h"

class AliAnaParticleHadronCorrelation : public AliAnaBaseClass {

public: 
  
  AliAnaParticleHadronCorrelation() ; // default ctor
  AliAnaParticleHadronCorrelation(const AliAnaParticleHadronCorrelation & ph) ; // cpy ctor
  AliAnaParticleHadronCorrelation & operator = (const AliAnaParticleHadronCorrelation & ph) ;//cpy assignment
  virtual ~AliAnaParticleHadronCorrelation() {;} //virtual dtor

  TList * GetCreateOutputObjects();

  Double_t GetDeltaPhiMaxCut() const {return fDeltaPhiMaxCut ; }
  Double_t GetDeltaPhiMinCut() const {return fDeltaPhiMinCut ; }
  void SetDeltaPhiCutRange(Double_t phimin, Double_t phimax)
  {fDeltaPhiMaxCut =phimax;  fDeltaPhiMinCut =phimin;}

  void InitParameters();

  void Print(const Option_t * opt) const;
 
  void MakeChargedCorrelation(AliAODParticleCorrelation * aodParticle,TSeqCollection* pl, const Bool_t bFillHisto) ;
  void MakeNeutralCorrelation(AliAODParticleCorrelation * aodParticle,TSeqCollection* pl, const Bool_t bFillHisto)  ;

  void MakeAnalysisFillAOD()  ;
  
  void MakeAnalysisFillHistograms() ; 

  Bool_t SelectCluster(AliAODCaloCluster * calo, Double_t *vertex, TLorentzVector & mom, Int_t & pdg);

  private:
  
  Double_t   fDeltaPhiMaxCut ;      // Minimum Delta Phi Gamma-Hadron
  Double_t   fDeltaPhiMinCut ;      //  Maximum Delta Phi Gamma-Hadron

  //Histograms
  TH2F * fhPhiCharged  ; //! Phi distribution of selected charged particles
  TH2F * fhPhiNeutral   ;  //! Phi distribution of selected neutral particles
  TH2F * fhEtaCharged  ; //! Eta distribution of selected charged particles
  TH2F * fhEtaNeutral   ; //! Eta distribution of selected neutral particles
  TH2F * fhDeltaPhiCharged  ;  //! Difference of charged particle phi and trigger particle  phi as function of  trigger particle pT
  TH2F * fhDeltaPhiNeutral   ;  //! Difference of neutral particle phi and trigger particle  phi as function of  trigger particle pT
  TH2F * fhDeltaEtaCharged  ;  //! Difference of charged particle eta and trigger particle  eta as function of  trigger particle pT
  TH2F * fhDeltaEtaNeutral  ;  //! Difference of neutral particle eta and trigger particle  eta as function of  trigger particle pT
  TH2F * fhDeltaPhiChargedPt  ;  //! Difference of charged particle phi and trigger particle  phi as function of charged particle pT
  TH2F * fhDeltaPhiNeutralPt  ;  //! Difference of neutral particle phi and trigger particle  phi as function of neutral particle particle pT

  TH2F * fhPtImbalanceNeutral  ; //! Trigger particle - neutral hadron momentum imbalance histogram 
  TH2F * fhPtImbalanceCharged  ; //! Trigger particle -charged hadron momentim imbalance histogram
  
  ClassDef(AliAnaParticleHadronCorrelation,1)
} ;
 

#endif //AliAnaParticleHadronCorrelation_H



