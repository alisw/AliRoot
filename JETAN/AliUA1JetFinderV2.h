#ifndef ALIUA1JETFINDERV2_H
#define ALIUA1JETFINDERV2_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


//---------------------------------------------------------------------
// UA1 Cone Algorithm Finder V1
// manages the search for jets
// Author: Rafael.Diaz.Valdes@cern.ch
// (version in c++)
//---------------------------------------------------------------------

#include "AliJetFinder.h"

class AliUA1JetHeaderV1;
class TH2F;
class TChain;

class AliUA1JetFinderV2 : public AliJetFinder
{
 public:

  AliUA1JetFinderV2();
  ~AliUA1JetFinderV2();

  // others
  void FindJetsC();
  void FindJets();
  void RunAlgoritmC(Float_t EtbgTotal, Double_t dEtTotal, Int_t& nJets,
		   Float_t* etJet,Float_t* etaJet, Float_t* phiJet,
		   Float_t* etallJet, Int_t* ncellsJet);

  void RunAlgoritm(Int_t nIn, Float_t* etCell, Float_t* etaCell, Float_t* phiCell, 
		   Int_t* flagCell, const Float_t* etCell2, const Float_t* etaCell2, const Float_t* phiCell2, 
		   const Int_t* flagCell2, Float_t etbgTotal, Double_t dEtTotal, 
		   Int_t& nJets, Float_t* etJet,Float_t* etaJet, Float_t* phiJet,
		   Float_t* etallJet, Int_t* ncellsJet);
   
  void SubtractBackgC(const Int_t& nIn, const Int_t&nJ, Float_t&EtbgTotalN,
                      const Float_t* ptT, const Float_t* etaT, const Float_t* phiT,
                      Float_t* etJet, const Float_t* etaJet, const Float_t* phiJet,
                      Float_t* etsigJet,Int_t* multJet, Int_t* injet);

  void SubtractBackg(const Int_t& nIn, const Int_t&nJ, Float_t&EtbgTotalN, const Float_t* ptT, const Int_t* vectT, 
		     const Float_t* etaT, const Float_t* phiT, const Float_t* cFlagT, const Float_t* cFlag2T, 
		     const Float_t* sFlagT, Float_t* etJet, const Float_t* etaJet, const Float_t* phiJet, 
		     Float_t* etsigJet, Int_t* multJet, Int_t* injet);

  void SubtractBackgCone(const Int_t& nIn, const Int_t&nJ,Float_t& EtbgTotalN,
                      Float_t* ptT, Float_t* etaT, const Float_t* phiT, const Float_t* cFlagT, const Float_t* sFlagT,
                      Float_t* etJet, const Float_t* etaJet, const Float_t* phiJet,
                      Float_t* etsigJet, Int_t* multJet, Int_t* injet);

  void SubtractBackgRatio(const Int_t& nIn, const Int_t&nJ,Float_t& EtbgTotalN,
                      Float_t* ptT, Float_t* etaT, const Float_t* phiT, const Float_t* cFlagT, const Float_t* sFlagT,
                      Float_t* etJet, const Float_t* etaJet, const Float_t* phiJet,
                      Float_t* etsigJet, Int_t* multJet, Int_t* injet);

  void SubtractBackgStat(const Int_t& nIn, const Int_t&nJ,Float_t&EtbgTotalN,
			 const Float_t* ptT, const Float_t* etaT, const Float_t* phiT, const Float_t* cFlagT, 
			 const Float_t* sFlagT, Float_t* etJet, const Float_t* etaJet, const Float_t* phiJet,
                      Float_t* etsigJet, Int_t* multJet, Int_t* injet);
  void Reset();
  void InitTask(TChain* tree);
  void WriteJHeaderToFile();
 protected:
  AliUA1JetFinderV2(const AliUA1JetFinderV2& rJetF1);
  AliUA1JetFinderV2& operator = (const AliUA1JetFinderV2& rhsf);
  TH2F           * fLego;           // Lego Histo
  Int_t fDebug;                     // Debug option
  Int_t fOpt;                       // Detector option (charged only or charged+neutral)

  ClassDef(AliUA1JetFinderV2,1)
};

#endif
