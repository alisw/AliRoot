// $Id$
//-*- Mode: C++ -*-
//* This file is property of and copyright by the ALICE HLT Project *
//* ALICE Experiment at CERN, All rights reserved.                  *
//* See cxx source for full Copyright notice                        *

#ifndef ALIANALYSISTASKHLT_H
#define ALIANALYSISTASKHLT_H


/** @file AliAnalysisTaskHLT.h
    @author Kalliopi Kanaki, Hege Erdal
    @date   
    @brief An analysis task to compare the offline and HLT esd trees
*/

// forward declarations
class TH1F;
class TH2F;
class TList;
//class AliESDEvent;
//class AliESDtrack;
//class AliESDRun;
class TObjArray;

#include "AliAnalysisTaskSE.h"

class AliAnalysisTaskHLT : public AliAnalysisTaskSE {
 
  public: 


  /*
   * ---------------------------------------------------------------------------------
   *                            Constructor / Destructor
   * ---------------------------------------------------------------------------------
   */

    AliAnalysisTaskHLT();
    AliAnalysisTaskHLT(const char *name);
    virtual ~AliAnalysisTaskHLT() {}

  /*
   * ---------------------------------------------------------------------------------
   *                                    Methods
   * ---------------------------------------------------------------------------------
   */

    virtual void  UserCreateOutputObjects();
    virtual void  UserExec(Option_t *option);
    virtual void  Terminate(Option_t *);
    virtual void  NotifyRun();

 private:

    /** copy constructor */
    AliAnalysisTaskHLT(const AliAnalysisTaskHLT&); 
    /** assignment operator */
    AliAnalysisTaskHLT& operator=(const AliAnalysisTaskHLT&); 


    /*
     * ---------------------------------------------------------------------------------
     *                            Setup Methods - private
     * ---------------------------------------------------------------------------------
     */
    
    void SetupESDtrackCuts();  


 
  
    /*
     * ---------------------------------------------------------------------------------
     *                             Members - private
     * ---------------------------------------------------------------------------------
     */


    static const Int_t fNcontrArray[];   //! Array to hold the different number of contributors
    static const Int_t fNcontr;          //! Array size

 
    //----------------------------------------------------------------------------------



    AliESDtrackCuts *fESDOfftrackCuts;   //! ESD cuts  
    AliESDtrackCuts *fESDHLTtrackCuts;   //! ESD cuts - on HLT object 
    
    TList *fOutputList; // list of output histograms

    TH1F *fHistTrigger, *fHistHLTTrigger; //! trigger counters 

    TH1F  *fChargeOff;         //! Charge distribution      
    TH1F  *fMomentumOff;       //! momentum	
    TH1F  *fMomentumOffTpc;         //! pseudorapidity for kTPCin
    TH1F  *fMomentumOffTpcIts;      //! pseudorapidity for kTPCin && kITSin
    TH1F  *fDCArOff;           //! track DCAr to beam line	 
    TH1F  *fDCAzOff;           //! track DCAz to beam line	 
    TH1F  *fNclusterOff;       //! #clusters per track
    TH1F  *fNclusterOffwCut;   //! #clusters per track w cuts
    TH1F  *fdEdxOff;           //! TPC signal (offline)
    TH2F  *fdEdxVSPOff;        //! dE/dx vs. momentum 
    TH1F  *fPhiOff;            //! azimuthal angle distribution
    TH1F  *fThetaOff;          //! polar angle distribution
    TH1F  *fMultOff;           //! track multiplicity of the event
    TH2F  *fXYvertexOff;       //! XY primary vertex distribution
    TH1F  *fXvertexOff;        //! X primary vertex distribution
    TH1F  *fYvertexOff;        //! Y primary vertex distribution
    TH1F  *fZvertexOff;        //! Z primary vertex distribution
    TH1F  *fEtaOff;            //! pseudorapidity
    TH1F  *fEtaMomentumcutOff;      //! pseudorapidity with DCA cut
    TH2F  *fNclusVSphiOff;     //! clusters per track vs. azimuthal angle 
    TH2F  *fNclusVSthetaOff;   //! clusters per track vs. polar angle 
    TH1F  *fStatusOff;         //! Status counters 
    TH1F  *fStatusOff_Ocl;     //! Status counters for TPCNcl=0
    TH1F  *fEventSpecieOff;    //! Event Specie Offline
    
    TH1F  *fChargeHLT;         //! Charge distribution 
    TH1F  *fMomentumHLT;       //! momentum	
    TH1F  *fMomentumHLTTpc;    //! pseudorapidity for kTPCin
    TH1F  *fMomentumHLTTpcIts; //! pseudorapidity for kTPCin && kITSin 
    TH1F  *fDCArHLT;	       //! track DCAr to beam line	 
    TH1F  *fDCAzHLT;	       //! track DCAz to beam line	 
    TH1F  *fDCArHLTSG;	       //! track DCAr to beam line as calculated in the HLT reco	 
    TH1F  *fDCAzHLTSG;	       //! track DCAz to beam line as calculated in the HLT reco	 
    TH1F  *fNclusterHLT;       //! #clusters per track
    TH1F  *fNclusterHLTwCut;   //! #clusters per track with cuts
    TH1F  *fdEdxHLT;	       //! TPC signal (offline)
    TH2F  *fdEdxVSPHLT;        //! dE/dx vs. momentum 
    TH1F  *fPhiHLT;	       //! azimuthal angle distribution
    TH1F  *fThetaHLT;          //! polar angle distribution
    TH1F  *fMultHLT;	       //! track multiplicity of the event   
    TH2F  *fXYvertexHLT;       //! XY primary vertex distribution
    TH1F  *fXvertexHLT;        //! X primary vertex distribution
    TH1F  *fYvertexHLT;        //! Y primary vertex distribution
    TH1F  *fZvertexHLT;        //! Z primary vertex distribution
    TH1F  *fEtaHLT;	       //! pseudorapidity
    TH1F  *fEtaMomentumcutHLT;      //! pseudorapidity with DCA cut
    TH2F  *fNclusVSphiHLT;     //! clusters per track vs. azimuthal angle 
    TH2F  *fNclusVSthetaHLT;   //! clusters per track vs. polar angle 
    TH1F  *fStatusHLT;         //! Status counters 
    TH1F  *fStatusHLT_Ocl;     //! Status counters for TPCNcl=0
    TH1F  *fEventSpecieHLT;    //! Event Specie HLT

    
    TObjArray *fTrgClsArray; //! array of trigger classes

 
  
//     TH1F *fDCAOff_trig;      //! track DCA to beam line for triggered events
//     TH1F *fNclusterOff_trig; //! #clusters per track for triggered events
//     
//     TH1F *fDCAHLT_trig;     
//     TH1F *fNclusterHLT_trig;
   
   
//     TH1F *fHistOfflTrkDCANoTrigNclsCut1; //! with cut on #clusters>=60
//     TH1F *fHistOfflTrkDCANoTrigNclsCut2; //! with cut on #clusters<60
//     
//     TH1F *fHistOfflResPtInv; //! resolution on 1/pt for offline tracks
//     TH1F *fHistOnlResPtInv; //! resoltion on 1/pt for online tracks
// 
//     TH1F *fHistOffldZ;  //! resolution on z 
//     TH1F *fHistOnldZ;   //! resolution on z 
// 
//     TH1F *fHistOffldX; //! resolution on r 
//     TH1F *fHistOnldX;  //! resolution on r 
//     
//     TH1F *fHistOfflPhi;  //! resolution on azimuthal angle 
//     TH1F *fHistOnlPhi;  //! resolution on azimuthal angle 
// 
//     TH1F *fHistOfflTheta; //! resolution on polar angle 
//     TH1F *fHistOnlTheta; //! resolution on polar angle 
// 
//     TH2F *fHistOnlDZ;  //! online trigger tracks distance to beam and Z to IP
//     TH2F *fHistOfflDZ; //! offline tracks distance to beam and Z to IP
//     TH2F *fHistOfflDZTrig; //!
//     TH2F *fHistOfflDZNoTrig; //!


//     static const Float_t fgkPhiMin[5];
//     static const Float_t fgkPhiMax[5];
//     static const Float_t fgkEtaMin;
//     static const Float_t fgkEtaMax;
//     static const Float_t fgkNormX[5];
//     static const Float_t fgkNormY[5];
//     static const Float_t fgkInitPosX[5];
//     static const Float_t fgkInitPosY[5];

    ClassDef(AliAnalysisTaskHLT, 2);
};

#endif
