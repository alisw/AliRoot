#ifndef ALICFHEAVYFLAVOURTASKMULTIVARMULTISTEP_H
#define ALICFHEAVYFLAVOURTASKMULTIVARMULTISTEP_H
/**************************************************************************
 * Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

//-----------------------------------------------------------------------
// Class for HF corrections as a function of many variables and step 
// Author : C. Zampolli, CERN
// Base class for HF Unfolding - agrelli@uu.nl
//-----------------------------------------------------------------------


#include "AliAnalysisTaskSE.h"

class TH1I;
class TParticle ;
class TFile ;
class TClonesArray ;
class AliCFManager;
class AliAODRecoDecay;
class AliAODRecoDecayHF2Prong;
class AliAODMCParticle;
class THnSparse;

class AliCFHeavyFlavourTaskMultiVarMultiStep : public AliAnalysisTaskSE {
  public:

  enum {
    kStepGenerated       = 0,
    kStepAcceptance      = 1,
    kStepVertex          = 2,
    kStepRefit           = 3,
    kStepReconstructed   = 4,
    kStepRecoAcceptance  = 5,
    kStepRecoITSClusters = 6,
    kStepRecoPPR         = 7
  };

  AliCFHeavyFlavourTaskMultiVarMultiStep();
  AliCFHeavyFlavourTaskMultiVarMultiStep(const Char_t* name);
  AliCFHeavyFlavourTaskMultiVarMultiStep& operator= (const AliCFHeavyFlavourTaskMultiVarMultiStep& c);
  AliCFHeavyFlavourTaskMultiVarMultiStep(const AliCFHeavyFlavourTaskMultiVarMultiStep& c);
  virtual ~AliCFHeavyFlavourTaskMultiVarMultiStep();

  // ANALYSIS FRAMEWORK STUFF to loop on data and fill output objects
  void     UserCreateOutputObjects();
  void     UserExec(Option_t *option);
  void     Terminate(Option_t *);

 // UNFOLDING
  void     SetCorrelationMatrix(THnSparse* h) {fCorrelation=h;}
  void     SetAcceptanceUnf(Bool_t AcceptanceUnf) {fAcceptanceUnf = AcceptanceUnf;}
  Bool_t   GetAcceptanceUnf() const {return fAcceptanceUnf;}

  
  // CORRECTION FRAMEWORK RELATED FUNCTIONS
  void           SetCFManager(AliCFManager* io) {fCFManager = io;}   // global correction manager
  AliCFManager * GetCFManager()                 {return fCFManager;} // get corr manager

  void     SetPDG(Int_t code) {fPDG = code; }     // defines the PDG code of searched HF
  Double_t CosThetaStar(AliAODMCParticle* mcPart, AliAODMCParticle* mcPartDaughter0, AliAODMCParticle* mcPartDaughter1) const;  // returns cos(ThetaStar) of the D0 decay
  Double_t CT(AliAODMCParticle* mcPart, AliAODMCParticle* mcPartDaughter0, AliAODMCParticle* mcPartDaughter1) const;            // returns cT of the D0 decay
  void     SetFillFromGenerated(Bool_t flag) {fFillFromGenerated = flag;}
  Bool_t   GetFillFromGenerated() const {return fFillFromGenerated;}
  Bool_t   GetGeneratedValuesFromMCParticle(AliAODMCParticle* mcPart, TClonesArray* mcArray, Double_t* vectorMC) const;
  void     SetMinITSClusters(Int_t minITSClusters) {fMinITSClusters = minITSClusters;}
  Int_t    GetMinITSClusters() const {return fMinITSClusters;}
  Int_t    CheckOrigin(AliAODMCParticle* mcPart, TClonesArray* mcArray) const;

  void SetKeepD0fromB(Bool_t keepD0fromB){fKeepD0fromB=keepD0fromB;}

 protected:
  Int_t           fPDG;         //  PDG code of searched V0's
  AliCFManager   *fCFManager;   //  pointer to the CF manager
  TH1I *fHistEventsProcessed;   //! simple histo for monitoring the number of events processed
  THnSparse* fCorrelation;      //  response matrix for unfolding
  Int_t fCountMC;               //  MC particle found
  Int_t fCountAcc;              //  MC particle found that satisfy acceptance cuts
  Int_t fCountVertex;       //  Reco particle found that satisfy vertex constrained
  Int_t fCountRefit;        //  Reco particle found that satisfy kTPCrefit and kITSrefit
  Int_t fCountReco;             //  Reco particle found that satisfy cuts
  Int_t fCountRecoAcc;          //  Reco particle found that satisfy cuts in requested acceptance
  Int_t fCountRecoITSClusters;  //  Reco particle found that satisfy cuts in n. of ITS clusters
  Int_t fCountRecoPPR;          //  Reco particle found that satisfy cuts in PPR
  Int_t fEvents;                //  n. of events
  Bool_t fFillFromGenerated;    //  flag to indicate whether data container should be filled 
                                //  with generated values also for reconstructed particles
  Int_t fMinITSClusters;        //  min n. of ITS clusters for RecoDecay
  Bool_t fAcceptanceUnf;        //  flag for unfolding before or after cuts.

  Bool_t fKeepD0fromB;          // flag to consider also D0 coming from B
  
  ClassDef(AliCFHeavyFlavourTaskMultiVarMultiStep,3); // class for HF corrections as a function of many variables
};

#endif
