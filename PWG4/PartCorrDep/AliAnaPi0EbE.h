#ifndef ALIANAPI0EBE_H
#define ALIANAPI0EBE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id: AliAnaPi0EbE.h 27413 2008-07-18 13:28:12Z gconesab $ */

//_________________________________________________________________________
//
// Class for the analysis of high pT pi0 event by event
// Pi0 identified by one of the following:
//  -Invariant mass of 2 cluster in calorimeter
//  -Shower shape analysis in calorimeter
//  -Invariant mass of one cluster in calorimeter and one photon reconstructed in TPC (in near future)
//
//-- Author: Gustavo Conesa (INFN-LNF)  &  Raphaelle Ichou (SUBATECH)
//_________________________________________________________________________


// --- ROOT system ---
class TH2F ; 
class TList ;

// --- ANALYSIS system ---
#include "AliAnaPartCorrBaseClass.h"
class AliAODPWG4ParticleCorrelation ;

class AliAnaPi0EbE : public AliAnaPartCorrBaseClass {

 public: 
  
  AliAnaPi0EbE() ; // default ctor
  AliAnaPi0EbE(const AliAnaPi0EbE & g) ; // cpy ctor
  AliAnaPi0EbE & operator = (const AliAnaPi0EbE & g) ;//cpy assignment
  virtual ~AliAnaPi0EbE() ; //virtual dtor
  
  enum anaTypes {kIMCalo, kSSCalo, kIMCaloTracks};
  
  TList *  GetCreateOutputObjects();
  
  void Init();
  void InitParameters();
  
  void MakeAnalysisFillAOD()  ;
  void MakeAnalysisFillHistograms() ; 
  
  void MakeInvMassInCalorimeter() ;
  void MakeInvMassInCalorimeterAndCTS() ;
  void MakeShowerShapeIdentification() ;
  
  void Print(const Option_t * opt)const;
  
  anaTypes GetAnalysisType()   const {return fAnaType ; }
  void SetAnalysisType(anaTypes ana)    {fAnaType = ana ; }
  
  TString GetInputAODGammaConvName()   const {return fInputAODGammaConvName ; }
  void SetInputAODGammaConvName(TString name)    {fInputAODGammaConvName = name ; }	
  
  //Only for pi0 SS identification case
  void SetCalorimeter(TString det)    {fCalorimeter = det ; }
  
  void SetMinDistanceToBadChannel(Float_t m1, Float_t m2, Float_t m3) {
    fMinDist = m1;
    fMinDist2 = m2;
    fMinDist3 = m3;
  }
  
 private:
  
  anaTypes fAnaType; //Select analysis type
  
  //Only for pi0 SS identification case, kSSCalo
  TString fCalorimeter ; // Calorimeter where the gamma is searched;
  Float_t fMinDist ;     // Minimal distance to bad channel to accept cluster
  Float_t fMinDist2;     // Cuts on Minimal distance to study acceptance evaluation
  Float_t fMinDist3;     // One more cut on distance used for acceptance-efficiency study
  
  //Only for combination of calorimeter and conversion photons, kIMCaloTracks
  TClonesArray * fInputAODGammaConv; //AOD array with conversion photons reconstructed in CTS
  TString fInputAODGammaConvName;    //Name of AOD branch with conversion photons
  
  //Histograms
  TH1F * fhPtPi0   ;         //! Number of identified  pi0
  TH2F * fhPhiPi0  ;         //! Phi of identified  pi0
  TH2F * fhEtaPi0  ;         //! eta of identified  pi0  
  
  //MC
  TH1F * fhPtMCNoPi0;   //! Number of identified pi0, not coming from pi0
  TH2F * fhPhiMCNoPi0;  //! Phi of identified pi0, not coming from pi0
  TH2F * fhEtaMCNoPi0;  //! eta of identified  pi0, not coming from pi0
  TH1F * fhPtMCPi0;     //! Number of identified pi0, coming from pi0
  TH2F * fhPhiMCPi0;    //! Phi of identified pi0, coming from pi0
  TH2F * fhEtaMCPi0;    //! eta of identified pi0, coming from pi0
  
  ClassDef(AliAnaPi0EbE,1)
    } ;


#endif //ALIANAPI0EBE_H



