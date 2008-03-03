#ifndef ALIITSRESIDUALSANALYSIS_H
#define ALIITSRESIDUALSANALYSIS_H

/* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


//*************************************************************************
// AliITSResidualAnalysis: class which deals with the analysis of the     *
// tracks residuals in the ITS                                            *
// More comments will come with the development of the interfaces and     *
// functionalities of the class.                                          *
//*************************************************************************

#include "AliAlignmentTracks.h"

class TObject;
class TCanvas;
class TH1F;
class TH2F;
class TArrayI;

class AliTrackPointArray;
class AliTrackResiduals;
class AliAlignObj;

class AliITSResidualsAnalysis : public AliAlignmentTracks {
  

 public:
    
  AliITSResidualsAnalysis();  
  AliITSResidualsAnalysis(const TString aliTrackPoints,const TString geom);
  AliITSResidualsAnalysis(const TArrayI *volIDs);
   AliITSResidualsAnalysis(TArrayI *volIDs,AliTrackPointArray **tracksClustArray,AliTrackPointArray **tracksFitPointsArray);
  //AliITSResidualsAnalysis(const AliITSResidualsAnalysis &res);
  ~AliITSResidualsAnalysis();


  //virtual ~AliITSResidualsAnalysis(){}
  // void AddArray(AliTrackPointArray *trackClust,AliTrackPoinArray *trackFitPoints);



  void ListVolUsed(TTree *pointsTree,TArrayI ***arrayIndex,Int_t **lastIndex);
  void ListVolUsed(){if(!fIsIndexBuilt)return;ListVolUsed(fPointsTree,fArrayIndex,fLastIndex);};

  void  InitHistograms(const TArrayI *volIDs);
  void FillResHists(AliTrackPointArray *trackClust,AliTrackPointArray *trackFitPoints) const;
  void  DrawHists() const;
  Bool_t AnalyzeHists(Int_t minNpoints) const;
  void FillVolumeCorrelationHists(Int_t ivol,Int_t volIDalignable,Int_t volIDpoint,Bool_t used) const;
  Bool_t SetBinning(const TArrayI *volids,Float_t *phiBin,Float_t *zBin);
  Float_t** CheckSingleLayer(const TArrayI *volids);
  TArrayI* GetSingleLayerVolids(Int_t layer) const;
  Int_t GetBinPhiZ(const Int_t volID,Int_t *binz) const;
  void GetTrackDirClusterCov(AliTrackPoint *point,Double_t &phi,Double_t &lambda,Double_t &lambda2,Double_t &alpha,Double_t &xovery,Double_t &zovery) const;
  void CalculateResiduals(const TArrayI *volids,const TArrayI *volidsfit,AliGeomManager::ELayerID layerRangeMin,AliGeomManager::ELayerID layerRangeMax,Int_t iterations=1,Bool_t draw=kTRUE);
  void ProcessPoints(TString minimizer="fast",Int_t fit=0,AliGeomManager::ELayerID iLayerToAlign=(AliGeomManager::ELayerID)1,AliGeomManager::ELayerID iLayerToExclude=(AliGeomManager::ELayerID)1,TString misalignmentFile="");
  void ExtractResiduals(Int_t layer=1,Int_t minEnt=10,TString filename="ResidualsAnalysisTree.root") const;
  Int_t PlotResiduals(Int_t layer=1,TString filename="resPlot_MA_layer") const;
  TString GetFileNameTrackPoints() const { return fAliTrackPoints; };
  TString GetFileNameGeometry() const { return fGeom; };
  void SetFileNameTrackPoints(TString name) { fAliTrackPoints=name; };
  void SetFileNameGeometry(TString name) { fGeom=name; };


  enum ModulePhiZ{
    kPhiSPD1=20,
    kZSPD1=4,
    kPhiSPD2=40,
    kZSPD2=4,
    kPhiSDD1=14,
    kZSDD1=6,
    kPhiSDD2=22,
    kZSDD2=8,
    kPhiSSD1=34,
    kZSSD1=22,
    kPhiSSD2=38,
    kZSSD2=25
  };


 protected:

  AliITSResidualsAnalysis(const AliITSResidualsAnalysis& obj);
  AliITSResidualsAnalysis& operator = (const AliITSResidualsAnalysis& obj);
    
  Int_t fnHist;    // number of histogram = number of alignable volumes considered
  Int_t fnPhi;     // coordinate of the volume (Phi)
  Int_t fnZ;       // coordinate of the volume (Z)
  Int_t **fvolidsToBin;         // array with volID and bin
  Int_t *fLastVolVolid;         // Last Vol Volid
  Double_t ***fCoordToBinTable; // array with coordinate and bin
  TH1F **fVolResHistRPHI;       //  Histogram of Residuals along Global rphi 
  TH1F **fResHistZ;             //  Histogram of Residuals along Global Z 
  TH1F **fPullHistRPHI;         //  Misc Histogram ...
  TH1F **fPullHistZ;            //  Misc Histogram ...
  TH1F **fTrackDirPhi;          //  Misc Histogram ...
  TH1F **fTrackDirLambda;       //  Misc Histogram ...
  TH1F **fTrackDirLambda2;      //  Misc Histogram ...
  TH1F **fTrackDirAlpha;        //  Misc Histogram ...
  TH1F *fTrackDirPhiAll;        //  Misc Histogram ...
  TH1F *fTrackDirLambdaAll;     //  Misc Histogram ...
  TH1F *fTrackDirLambda2All;    //  Misc Histogram ...
  TH1F *fTrackDirAlphaAll;      //  Misc Histogram ...
  TH2F **fTrackDir;             //  Misc Histogram ...
  TH2F *fTrackDirAll;           //  Misc Histogram ...
  TH2F *fTrackDir2All;          //  Misc Histogram ...
  TH2F *fTrackDirXZAll;         //  Misc Histogram ...
  TH1F **fResHistGlob;          //  Histogram of Residuals
  TH2F **fhistCorrVol;          //  Misc Histogram ...
  TH2F *fVolNTracks;       // Histogram to see how many tracks pass through a given module 
  TH2F *fhEmpty;           // Histogram to get/set the binning
  TH2F *fhistVolNptsUsed;  // Histogram with the numer of points used per volume
  TH2F *fhistVolUsed;      // Histogram with the numer of volumes udes
  TH2F *fSigmaVolZ;        // Histogram with the sigma vs volume Z
  Bool_t fsingleLayer;     // Tag for single layer
  Bool_t fWriteHist;       // Tag for histograms
  TArrayI *fpTrackVolIDs;  // array that gives the volID for the i^(th) entry 
  TArrayI **fVolVolids;    // array with the volumes to analyze
  TArrayI **fVolUsed;      // array with the volumes to use
  Bool_t fRealignObjFileIsOpen; // indicator- if the file fRealignObjFilename is opened
  TClonesArray *fClonesArray;   // pointer to the TClonesArray with the final AliAlignObjParams
  TString fAliTrackPoints;      // Filename with the AliTrackPoints
  TString fGeom;                // Filename with the Geometry

  ClassDef(AliITSResidualsAnalysis,1) // Residuals analysis for the ITS
    
    };
    
#endif
    
