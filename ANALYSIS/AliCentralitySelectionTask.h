#ifndef ALICENTRALITYSELECTIONTASK_H
#define ALICENTRALITYSELECTIONTASK_H

/* Copyright(c) 1998-2008, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//*****************************************************
//   Class AliCentralitySelectionTask
//   author: Alberica Toia
//*****************************************************

#include "AliAnalysisTaskSE.h"

class TFile;
class TH1F;
class TH2F;
class TList;
class TString;

class AliESDEvent;
class AliESDtrackCuts;

class AliCentralitySelectionTask : public AliAnalysisTaskSE {

 public:

  AliCentralitySelectionTask();
  AliCentralitySelectionTask(const char *name);
  AliCentralitySelectionTask& operator= (const AliCentralitySelectionTask& ana);
  AliCentralitySelectionTask(const AliCentralitySelectionTask& c);
  virtual ~AliCentralitySelectionTask();

  // Implementation of interface methods
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t *option);
  virtual void Terminate(Option_t *option);

  virtual void  SetDebugLevel(Int_t level) {fDebug = level;}
  void SetInput(const char* input)         {fAnalysisInput = input;}
  void SetMCInput()                        {fIsMCInput = kTRUE;}
  
  void ReadCentralityHistos(TString filename);
  void ReadCentralityHistos2(TString filename);
  
  Float_t GetCorrV0(const AliESDEvent* esd, float &v0CorrResc, int run);
  Float_t GetCorrSPD2(Float_t spd2raw,Float_t zv);

 private:

  Int_t SetupRun(AliESDEvent* esd);

  Int_t    fDebug;	   	// Debug flag
  TString  fAnalysisInput; 	// "ESD", "AOD"
  Bool_t   fIsMCInput;          // true when input is MC
  TFile   *fFile;               // file that holds the centrality vs multiplicity 1d
  TFile   *fFile2;              // file that holds the centrality vs multiplicity 2d  
  Int_t    fCurrentRun;         // current run number
  Int_t    fRunNo;              // reference run number

  AliESDtrackCuts* fTrackCuts;             //! optional track cuts

  Float_t  fCentV0M;            // percentile centrality from V0
  Float_t  fCentFMD;            // percentile centrality from FMD
  Float_t  fCentTRK;            // percentile centrality from tracks
  Float_t  fCentTKL;            // percentile centrality from tracklets
  Float_t  fCentCL0;            // percentile centrality from clusters in layer 0
  Float_t  fCentCL1;            // percentile centrality from clusters in layer 0
  Float_t  fCentV0MvsFMD;       // percentile centrality from V0 vs FMD
  Float_t  fCentTKLvsV0M;       // percentile centrality from tracklets vs V0
  Float_t  fCentZEMvsZDC;       // percentile centrality from ZEM vs ZDC

  TH1F    *fHtempV0M;           // histogram with centrality vs multiplicity using V0
  TH1F    *fHtempFMD;           // histogram with centrality vs multiplicity using FMD
  TH1F    *fHtempTRK;           // histogram with centrality vs multiplicity using tracks
  TH1F    *fHtempTKL;           // histogram with centrality vs multiplicity using tracklets
  TH1F    *fHtempCL0;           // histogram with centrality vs multiplicity using clusters in layer 0
  TH1F    *fHtempCL1;           // histogram with centrality vs multiplicity using clusters in layer 0
  TH1F    *fHtempV0MvsFMD;           // histogram with centrality vs multiplicity using V0 vs FMD   
  TH1F    *fHtempTKLvsV0M;           // histogram with centrality vs multiplicity using tracklets vs V0
  TH1F    *fHtempZEMvsZDC;           // histogram with centrality vs multiplicity using ZEM vs ZDC 

  TList   *fOutputList; // output list
  
  TH1F *fHOutCentV0M     ;    //control histogram for centrality
  TH1F *fHOutCentFMD     ;    //control histogram for centrality
  TH1F *fHOutCentTRK     ;    //control histogram for centrality
  TH1F *fHOutCentTKL     ;    //control histogram for centrality
  TH1F *fHOutCentCL0     ;    //control histogram for centrality
  TH1F *fHOutCentCL1     ;    //control histogram for centrality
  TH1F *fHOutCentV0MvsFMD;    //control histogram for centrality
  TH1F *fHOutCentTKLvsV0M;    //control histogram for centrality
  TH1F *fHOutCentZEMvsZDC;    //control histogram for centrality

  TH1F *fHOutMultV0M ;        //control histogram for multiplicity
  TH1F *fHOutMultFMD ;        //control histogram for multiplicity
  TH1F *fHOutMultTRK ;        //control histogram for multiplicity
  TH1F *fHOutMultTKL ;        //control histogram for multiplicity
  TH1F *fHOutMultCL0 ;        //control histogram for multiplicity
  TH1F *fHOutMultCL1 ;        //control histogram for multiplicity

  TH2F *fHOutMultV0MvsZDC;    //control histogram for multiplicity
  TH2F *fHOutMultZEMvsZDC;    //control histogram for multiplicity
  TH2F *fHOutMultV0MvsCL1;    //control histogram for multiplicity
  TH2F *fHOutMultV0MvsTRK;    //control histogram for multiplicity
  TH2F *fHOutMultTRKvsCL1;    //control histogram for multiplicity

  ClassDef(AliCentralitySelectionTask,1); 

};

#endif

