#ifndef AliAnalysisTaskESDCheckCascade_cxx
#define AliAnalysisTaskESDCheckCascade_cxx

/*  See cxx source for full Copyright notice */

//-----------------------------------------------------------------
//                 AliAnalysisTaskESDCheckCascade class
//            (AliAnalysisTaskESDCheckCascade)
//            This task is for QAing the Cascades from the ESD
//              Origin:  AliAnalysisTaskESDCheckV0 by B.H. Nov2007, hippolyt@in2p3.fr
//            Adapted to Cascade : A.M Mar2008, antonin.maire@ires.in2p3.fr
//-----------------------------------------------------------------

class TList;
class TH1F;
class AliESDEvent;

#include "AliAnalysisTask.h"

class AliAnalysisTaskESDCheckCascade : public AliAnalysisTask {
 public:
  AliAnalysisTaskESDCheckCascade();
  AliAnalysisTaskESDCheckCascade(const char *name);
  virtual ~AliAnalysisTaskESDCheckCascade() {}
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);
  
 private:
  AliESDInputHandler *fesdH;		//! InputHandler object
  AliMCEvent  *fMCevent;		//!  MC event object
  AliESDEvent *fESD;			//! ESD object
  
 
   	
	

		TList	*fListHist_Cascade;		//! List of Cascade histograms
	
	// - General histos (filled for any event)
	TH1F	*fHistTrackMultiplicity;		//! Track multiplicity distribution
	TH1F	*fHistCascadeMultiplicity;		//! Cascade multiplicity distribution


	// - Vertex Positions
		// (Vtx coming from the full tracking)
	TH1F	*fHistPos_TrkgPrimaryVtx_x;  		//! Primary vertex position distribution in x 
	TH1F	*fHistPos_TrkgPrimaryVtx_y;		//! Primary vertex position distribution in y
	TH1F	*fHistPos_TrkgPrimaryVtx_z;  		//! Primary vertex position distribution in z
	TH1F	*fHist_TrkgPrimaryVtxRadius;		//! Primary vertex (3D) radius distribution 

		// (Vtx coming from the SPD only)
	TH1F	*fHistPos_SPDPrimaryVtx_x;      	//! Primary vertex position distribution in x 
	TH1F	*fHistPos_SPDPrimaryVtx_y;		//! Primary vertex position distribution in y
	TH1F	*fHistPos_SPDPrimaryVtx_z;      	//! Primary vertex position distribution in z
	TH1F	*fHist_SPDPrimaryVtxRadius;		//! Primary vertex (3D) radius distribution 
	
		// Correlation SPD Vtx / Full Tracking Vtx
	TH2F	*f2dHistTrkgPrimVtxVsSPDPrimVtx;	//!  Radius of Prim. Vtx from tracks Vs Radius of Prim. Vtx from SPD
	
	
	
	// - Typical histos for cascades
	TH1F	*fHistEffMassXi;      			//! reconstructed cascade effective mass
	TH1F	*fHistChi2Xi;         			//! chi2 value
	TH1F	*fHistDcaXiDaughters; 			//! dca between Xi's daughters
	TH1F 	*fHistDcaBachToPrimVertex;		//! Dca of the bachelor track to primary vertex
	TH1F	*fHistXiCosineOfPointingAngle;		//! Cosine of Xi pointing angle in a cascade
	TH1F	*fHistXiRadius;				//! (transverse) radius of the cascade vertex 
		
	TH1F	*fHistXiMom_transv;     		//! Xi transverse momentum 
	TH1F	*fHistXiMom_tot;     			//! Xi momentum norm
	
	TH1F	*fHistBachMom_transv;     		//! bachelor transverse momentum 
	TH1F	*fHistBachMom_tot;     			//! bachelor momentum norm
			
		
	// - Histos about ~ the "V0 part" of the cascade,  coming by inheritance from AliESDv0
	TH1F	*fHistMassLambdaAsCascDghter;		//! Test Invariant Mass of Lambda coming from Cascade
	TH1F	*fHistV0Chi2_Xi;			//! V0 chi2 distribution, for the V0 associated to a cascade
	TH1F	*fHistDcaV0Daughters_Xi;		//! Dca between V0 daughters, for the V0 associated to a cascade
	TH1F	*fHistDcaV0ToPrimVertex_Xi;		//! Dca of V0 to primary vertex, for the V0 associated to a cascade	
	TH1F	*fHistV0CosineOfPointingAngle_Xi;	//! Cosine of V0 pointing angle, for the V0 associated to a cascade
	TH1F	*fHistV0Radius_Xi;			//! V0 (transverse) distance distribution, for the V0 associated to a cascade

	TH1F	*fHistDcaPosToPrimVertex_Xi;		//! Dca of V0 positive daughter to primary vertex, for the V0 associated to a cascade
	TH1F	*fHistDcaNegToPrimVertex_Xi;		//! Dca of V0 negative daughter to primary vertex, for the V0 associated to a cascade
	

	// - Effective mass histos for cascades.
	TH1F	*fHistMassXiMinus;			//! reconstructed cascade effective mass, under Xi- hyp.
	TH1F	*fHistMassXiPlus;			//! reconstructed cascade effective mass, under Xi+ hyp.
	TH1F	*fHistMassOmegaMinus;			//! reconstructed cascade effective mass, under Omega- hyp.
	TH1F	*fHistMassOmegaPlus;			//! reconstructed cascade effective mass, under Omega+ hyp.
  
  

  AliAnalysisTaskESDCheckCascade(const AliAnalysisTaskESDCheckCascade&);            // not implemented
  AliAnalysisTaskESDCheckCascade& operator=(const AliAnalysisTaskESDCheckCascade&); // not implemented
  
  ClassDef(AliAnalysisTaskESDCheckCascade, 1);
};

#endif
