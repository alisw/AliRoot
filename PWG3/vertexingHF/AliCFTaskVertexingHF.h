#ifndef ALICFTASKVERTEXINGHF_H
#define ALICFTASKVERTEXINGHF_H
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
//			D. Caffarri, Univ & INFN Padova caffarri@pd.infn.it
// Base class for HF Unfolding - agrelli@uu.nl
//-----------------------------------------------------------------------


#include "AliAnalysisTaskSE.h"
#include "AliCFVertexingHF2Prong.h"
#include "AliCFVertexingHF.h"

class TH1I;
class TParticle ;
class TFile ;
class TClonesArray ;
class AliCFManager;
class AliAODRecoDecay;
class AliAODRecoDecayHF2Prong;
class AliAODMCParticle;
class THnSparse;
class AliRDHFCutsD0toKpi;
class AliCFVertexingHF2Prong;

class AliCFTaskVertexingHF: public AliAnalysisTaskSE {
public:
	
	enum {
	        kStepGeneratedLimAcc = 0,
		kStepGenerated       = 1,
		kStepAcceptance      = 2,
		kStepVertex          = 3,
		kStepRefit           = 4,
		kStepReconstructed   = 5,
		kStepRecoAcceptance  = 6,
		kStepRecoITSClusters = 7,
		kStepRecoPPR         = 8,
		kStepRecoPID         = 9
	};
	
	AliCFTaskVertexingHF();
	AliCFTaskVertexingHF(const Char_t* name, AliRDHFCutsD0toKpi* cuts);
	AliCFTaskVertexingHF& operator= (const AliCFTaskVertexingHF& c);
	AliCFTaskVertexingHF(const AliCFTaskVertexingHF& c);
 	virtual ~AliCFTaskVertexingHF();
	
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

	//	Setters for teh config macro
	void   SetFillFromGenerated(Bool_t flag) {fFillFromGenerated = flag;}
	Bool_t GetFillFromGenerated() const {return fFillFromGenerated;}


	//void   SetDselection(UShort_t originDselection);
	 
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
	Int_t fCountRecoPID;          //Reco PID step 
	Int_t fEvents;                //  n. of events
	Bool_t fFillFromGenerated;    //  flag to indicate whether data container should be filled with generated values also for reconstructed particles
	UShort_t fOriginDselection;      // flag to select D0 origins. 0 Only from charm 1 only from beauty 2 both from charm and beauty
	Bool_t fAcceptanceUnf;        //  flag for unfolding before or after cuts.
	AliRDHFCutsD0toKpi* fCuts;    // cuts

	
	ClassDef(AliCFTaskVertexingHF,1); // class for HF corrections as a function of many variables
};

#endif
