#ifndef ALIANALYSISTASKUE_H
#define ALIANALYSISTASKUE_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
#include "AliAnalysisTaskSE.h"

class AliESDEvent;
class AliAODEvent;
class TH1F;
class TH2F;
class TH1I;
class TVector3;
       
class  AliAnalysisTaskUE : public AliAnalysisTask
{
 public:
                     AliAnalysisTaskUE(const char* name="AliAnalysisTaskUE");
  virtual           ~AliAnalysisTaskUE() {;}
  
  // Implementation of interface methods
  virtual     void   ConnectInputData(Option_t *);
  virtual     void   CreateOutputObjects();
  virtual     void   Exec(Option_t *option);
  virtual     void   Terminate(Option_t *);
  
  //  Setters
  virtual     void   SetDebugLevel(Int_t level)      { fDebug = level; }
              void   SetPtRangeInHist(Int_t bin, Double_t min, Double_t max) { 
                                          fBinsPtInHist = bin; 
                                          fMinJetPtInHist = min; 
                                          fMaxJetPtInHist = max; 
                                       }
  
              void   SetAnaTopology(Int_t val)    { fAnaType = val;    }          
              void   SetRegionType(Int_t val)     { fRegionType = val; }
              void   SetConeRadius(Double_t val)  { fConeRadius = val; }
              void   SetPtSumOrdering(Int_t val)  { fOrdering = val;   }
      // Jet cuts    
              void   SetJet1EtaCut(Double_t val)      { fJet1EtaCut = val; }
              void   SetJet2DeltaPhiCut(Double_t val) { fJet2DeltaPhiCut = val; }
              void   SetJet2RatioPtCut(Double_t val)  { fJet2RatioPtCut = val; }
              void   SetJet3PtCut(Double_t val)       { fJet3PtCut = val; }
      // track cuts
              void   SetTrackPtCut(Double_t val)  { fTrackPtCut = val; }
              void   SetTrackEtaCut(Double_t val) { fTrackEtaCut = val; }

private:
                     AliAnalysisTaskUE(const  AliAnalysisTaskUE &det);
AliAnalysisTaskUE&   operator=(const  AliAnalysisTaskUE &det);

              void   AnalyseUE();
             Int_t   IsTrackInsideRegion(TVector3 *jetVect, TVector3 *partVect);
              void   CreateHistos();
              void   FillSumPtRegion( Double_t leadingE, Double_t ptMax, Double_t ptMin );
              void   FillAvePartPtRegion( Double_t leadingE, Double_t ptMax, Double_t ptMin );
              void   FillMultRegion( Double_t leadingE, Double_t nTrackPtmax, Double_t nTrackPtmin, Double_t ptMin );
    
       
             Int_t   fDebug;           //  Debug flag
       AliAODEvent*  fAOD;             //! AOD Event 
             TList*  fListOfHistos;    //  Output list of histograms
      
      // Config     
             Int_t   fBinsPtInHist;     //  # bins for Pt histos range
          Double_t   fMinJetPtInHist;   //  min Jet Pt value for histo range
          Double_t   fMaxJetPtInHist;   //  max Jet Pt value for histo range
      
      // Cuts 
             Int_t   fAnaType;          // Analysis type on jet topology: 
                                        //     1=inclusive  (default) 
                                        //     2=back to back inclusive
                                        //     3=back to back exclusive
                                        //     4=gama jet (back to back) ???
                                        //  Minimum bias
                                        //     31 = Semi jet (charged leading particle jets)
                                        //     32 = Random jetcone  ?
                                        //     33 = Swiss chees   ?

             Int_t   fRegionType;       // 1 = transverse regions (default)
                                        // 2 = cone regions   
                                        
             Double_t   fConeRadius;    // if selected Cone-like region type set Radius (=0.7 default)
                                        
             Int_t   fOrdering;         //  Pt and multiplicity summation ordering:
                                        //     1=CDF-like -independent sorting according quantity to be scored: Double sorting- (default)
                                        //       if Pt summation will be scored take Pt minimum between both zones and 
                                        //          fill Pt Max. and Min. histog. accordingly
                                        //       if Multiplicity summation will be scored take Mult. minimum between both zones and 
                                        //          fill Mult Max and Min histog. accordingly
                                        //     2=Marchesini-like (Only Pt sorting: Single sorting)
                                        //          sort only according Pt summation scored, find minimum between both zones and
                                        //          fill Pt and Multiplicity Max and Min summation histog. following only this criterium
                                        //     3=User Selection sorting (NOTE: USER must implement it within cxx)
                                        
      // Jet cuts    
          Double_t   fJet1EtaCut;        // |jet1 eta| < fJet1EtaCut   (fAnaType = 1,2,3)
          Double_t   fJet2DeltaPhiCut;   // |Jet1.Phi - Jet2.Phi| < fJet2DeltaPhiCut (fAnaType = 2,3)
          Double_t   fJet2RatioPtCut;    // Jet2.Pt/Jet1Pt > fJet2RatioPtCut  (fAnaType = 2,3)
          Double_t   fJet3PtCut;         // Jet3.Pt < fJet3PtCut  (fAnaType = 3)
      // track cuts
          Double_t   fTrackPtCut;        // Pt cut of tracks in the regions
          Double_t   fTrackEtaCut;       // Eta cut on tracks in the regions (fRegionType=1)

      // Histograms    ( are owner by fListOfHistos TList )
              TH1F*  fhNJets;                  //!
              TH1F*  fhEleadingPt;             //!
              
              TH1F*  fhMinRegPtDist;
              TH1F*  fhRegionMultMin;
              TH1F*  fhMinRegAvePt; 
              TH1F*  fhMinRegSumPt;            
              TH1F*  fhMinRegMaxPtPart;
              TH1F*  fhMinRegSumPtvsMult;
              
              TH1F*  fhdNdEta_PhiDist;         //!
              TH2F*  fhFullRegPartPtDistVsEt;  //!
              TH2F*  fhTransRegPartPtDistVsEt; //!
              
              TH1F*  fhRegionSumPtMaxVsEt;     //!
              TH1I*  fhRegionMultMax;          //!
              TH1F*  fhRegionMultMaxVsEt;      //!
              TH1F*  fhRegionSumPtMinVsEt;     //!
              TH1F*  fhRegionMultMinVsEt;      //!
              TH1F*  fhRegionAveSumPtVsEt;     //!
              TH1F*  fhRegionDiffSumPtVsEt;    //!
                     
              TH1F*  fhRegionAvePartPtMaxVsEt; //!
              TH1F*  fhRegionAvePartPtMinVsEt; //!
              TH1F*  fhRegionMaxPartPtMaxVsEt; //!
              
      //        TH2F*  fhValidRegion; //! test to be canceled
         
    ClassDef( AliAnalysisTaskUE, 1) // Analysis task for Underlying Event analysis
};
 
#endif
