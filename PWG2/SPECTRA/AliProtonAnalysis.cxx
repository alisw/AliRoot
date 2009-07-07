/**************************************************************************
 * Author: Panos Christakoglou.                                           *
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

/* $Id$ */

//-----------------------------------------------------------------
//                 AliProtonAnalysis class
//   This is the class to deal with the proton analysis
//   Origin: Panos Christakoglou | Panos.Christakoglou@cern.ch
//-----------------------------------------------------------------
#include <Riostream.h>
#include <TFile.h>
#include <TSystem.h>
#include <TF1.h>
#include <TH2D.h>
#include <TH1D.h>
#include <TH1I.h>
#include <TParticle.h>

#include <AliExternalTrackParam.h>
#include <AliAODEvent.h>
#include <AliESDEvent.h>
//#include <AliLog.h>
#include <AliPID.h>
#include <AliStack.h>
#include <AliCFContainer.h>
#include <AliCFEffGrid.h>
#include <AliCFDataGrid.h>
//#include <AliESDVertex.h>
class AliLog;
class AliESDVertex;

#include "AliProtonAnalysis.h"
#include "AliProtonAnalysisBase.h"

ClassImp(AliProtonAnalysis)

//____________________________________________________________________//
AliProtonAnalysis::AliProtonAnalysis() : 
  TObject(), fProtonAnalysisBase(0),
  fNBinsY(0), fMinY(0), fMaxY(0),
  fNBinsPt(0), fMinPt(0), fMaxPt(0),
  fProtonContainer(0), fAntiProtonContainer(0),
  fHistEvents(0), fHistYPtProtons(0), fHistYPtAntiProtons(0),
  fEffGridListProtons(0), fCorrectionListProtons2D(0), 
  fEfficiencyListProtons1D(0), fCorrectionListProtons1D(0),
  fEffGridListAntiProtons(0), fCorrectionListAntiProtons2D(0), 
  fEfficiencyListAntiProtons1D(0), fCorrectionListAntiProtons1D(0),
  fCorrectProtons(0), fCorrectAntiProtons(0) {
  //Default constructor
}

//____________________________________________________________________//
AliProtonAnalysis::AliProtonAnalysis(Int_t nbinsY, 
				     Float_t fLowY, Float_t fHighY,
				     Int_t nbinsPt, 
				     Float_t fLowPt, Float_t fHighPt) : 
  TObject(), fProtonAnalysisBase(0),
  fNBinsY(nbinsY), fMinY(fLowY), fMaxY(fHighY),
  fNBinsPt(nbinsPt), fMinPt(fLowPt), fMaxPt(fHighPt),
  fProtonContainer(0), fAntiProtonContainer(0),
  fHistEvents(0), fHistYPtProtons(0), fHistYPtAntiProtons(0),
  fEffGridListProtons(0), fCorrectionListProtons2D(0), 
  fEfficiencyListProtons1D(0), fCorrectionListProtons1D(0),
  fEffGridListAntiProtons(0), fCorrectionListAntiProtons2D(0), 
  fEfficiencyListAntiProtons1D(0), fCorrectionListAntiProtons1D(0),
  fCorrectProtons(0), fCorrectAntiProtons(0){
  //Default constructor
  fHistEvents = new TH1I("fHistEvents","Analyzed events",1,0,1);

  fHistYPtProtons = new TH2D("fHistYPtProtons","Protons",
			     fNBinsY,fMinY,fMaxY,
			     fNBinsPt,fMinPt,fMaxPt);
  fHistYPtProtons->SetStats(kTRUE);
  fHistYPtProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtProtons->GetXaxis()->SetTitle("y");
  fHistYPtProtons->GetXaxis()->SetTitleColor(1);

  fHistYPtAntiProtons = new TH2D("fHistYPtAntiProtons","Antiprotons",
				 fNBinsY,fMinY,fMaxY,
				 fNBinsPt,fMinPt,fMaxPt);
  fHistYPtAntiProtons->SetStats(kTRUE);
  fHistYPtAntiProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtAntiProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtAntiProtons->GetXaxis()->SetTitle("y");
  fHistYPtAntiProtons->GetXaxis()->SetTitleColor(1);

  //setting up the containers
  Int_t iBin[2];
  iBin[0] = nbinsY;
  iBin[1] = nbinsPt;
  Double_t *binLimY = new Double_t[nbinsY+1];
  Double_t *binLimPt = new Double_t[nbinsPt+1];
  //values for bin lower bounds
  for(Int_t i = 0; i <= nbinsY; i++) 
    binLimY[i]=(Double_t)fLowY  + (fHighY - fLowY)  /nbinsY*(Double_t)i;
  for(Int_t i = 0; i <= nbinsPt; i++) 
    binLimPt[i]=(Double_t)fLowPt  + (fHighPt - fLowPt)  /nbinsPt*(Double_t)i;

  fProtonContainer = new AliCFContainer("containerProtons",
					"container for protons",
					1,2,iBin);
  fProtonContainer->SetBinLimits(0,binLimY); //rapidity or eta
  fProtonContainer->SetBinLimits(1,binLimPt); //pT
  fAntiProtonContainer = new AliCFContainer("containerAntiProtons",
					    "container for antiprotons",
					    1,2,iBin);
  fAntiProtonContainer->SetBinLimits(0,binLimY); //rapidity or eta
  fAntiProtonContainer->SetBinLimits(1,binLimPt); //pT
} 

//____________________________________________________________________//
AliProtonAnalysis::AliProtonAnalysis(Int_t nbinsY, Double_t *gY,
				     Int_t nbinsPt,Double_t *gPt) : 
  TObject(), fProtonAnalysisBase(0),
  fNBinsY(nbinsY), fMinY(gY[0]), fMaxY(gY[nbinsY]),
  fNBinsPt(nbinsPt), fMinPt(gPt[0]), fMaxPt(gPt[nbinsPt]),
  fProtonContainer(0), fAntiProtonContainer(0),
  fHistEvents(0), fHistYPtProtons(0), fHistYPtAntiProtons(0),
  fEffGridListProtons(0), fCorrectionListProtons2D(0), 
  fEfficiencyListProtons1D(0), fCorrectionListProtons1D(0),
  fEffGridListAntiProtons(0), fCorrectionListAntiProtons2D(0), 
  fEfficiencyListAntiProtons1D(0), fCorrectionListAntiProtons1D(0),
  fCorrectProtons(0), fCorrectAntiProtons(0){
  //Default constructor
  fHistEvents = new TH1I("fHistEvents","Analyzed events",1,0,1);

  fHistYPtProtons = new TH2D("fHistYPtProtons","Protons",
			     fNBinsY,gY,fNBinsPt,gPt);
  fHistYPtProtons->SetStats(kTRUE);
  fHistYPtProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtProtons->GetXaxis()->SetTitle("y");
  fHistYPtProtons->GetXaxis()->SetTitleColor(1);

  fHistYPtAntiProtons = new TH2D("fHistYPtAntiProtons","Antiprotons",
				 fNBinsY,gY,fNBinsPt,gPt);
  fHistYPtAntiProtons->SetStats(kTRUE);
  fHistYPtAntiProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtAntiProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtAntiProtons->GetXaxis()->SetTitle("y");
  fHistYPtAntiProtons->GetXaxis()->SetTitleColor(1);

  //setting up the containers
  Int_t iBin[2];
  iBin[0] = nbinsY;
  iBin[1] = nbinsPt;
  fProtonContainer = new AliCFContainer("containerProtons",
					"container for protons",
					1,2,iBin);
  fProtonContainer->SetBinLimits(0,gY); //rapidity or eta
  fProtonContainer->SetBinLimits(1,gPt); //pT
  fAntiProtonContainer = new AliCFContainer("containerAntiProtons",
					    "container for antiprotons",
					    1,2,iBin);
  fAntiProtonContainer->SetBinLimits(0,gY); //rapidity or eta
  fAntiProtonContainer->SetBinLimits(1,gPt); //pT
} 

//____________________________________________________________________//
AliProtonAnalysis::~AliProtonAnalysis() {
  //Default destructor
  if(fProtonAnalysisBase) delete fProtonAnalysisBase;

  if(fHistEvents) delete fHistEvents;
  if(fHistYPtProtons) delete fHistYPtProtons;
  if(fHistYPtAntiProtons) delete fHistYPtAntiProtons;
  if(fProtonContainer) delete fProtonContainer;
  if(fAntiProtonContainer) delete fAntiProtonContainer;

  if(fEffGridListProtons) delete fEffGridListProtons;
  if(fCorrectionListProtons2D) delete fCorrectionListProtons2D;
  if(fEfficiencyListProtons1D) delete fEfficiencyListProtons1D;
  if(fCorrectionListProtons1D) delete fCorrectionListProtons1D;
  if(fEffGridListAntiProtons) delete fEffGridListAntiProtons;
  if(fCorrectionListAntiProtons2D) delete fCorrectionListAntiProtons2D;
  if(fEfficiencyListAntiProtons1D) delete fEfficiencyListAntiProtons1D;
  if(fCorrectionListAntiProtons1D) delete fCorrectionListAntiProtons1D;
  if(fCorrectProtons) delete fCorrectProtons;
  if(fCorrectAntiProtons) delete fCorrectAntiProtons;
}

//____________________________________________________________________//
void AliProtonAnalysis::InitAnalysisHistograms(Int_t nbinsY, 
					       Float_t fLowY, Float_t fHighY, 
					       Int_t nbinsPt, 
					       Float_t fLowPt, Float_t fHighPt) {
  //Initializes the histograms
  fNBinsY = nbinsY;
  fMinY = fLowY;
  fMaxY = fHighY;
  fNBinsPt = nbinsPt;
  fMinPt = fLowPt;
  fMaxPt = fHighPt;

  fHistEvents = new TH1I("fHistEvents","Analyzed events",1,0,1);

  fHistYPtProtons = new TH2D("fHistYPtProtons","Protons",
			     fNBinsY,fMinY,fMaxY,
			     fNBinsPt,fMinPt,fMaxPt);
  fHistYPtProtons->SetStats(kTRUE);
  fHistYPtProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtProtons->GetXaxis()->SetTitle("y");
  fHistYPtProtons->GetXaxis()->SetTitleColor(1);

  fHistYPtAntiProtons = new TH2D("fHistYPtAntiProtons","Antiprotons",
				 fNBinsY,fMinY,fMaxY,
				 fNBinsPt,fMinPt,fMaxPt);
  fHistYPtAntiProtons->SetStats(kTRUE);
  fHistYPtAntiProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtAntiProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtAntiProtons->GetXaxis()->SetTitle("y");
  fHistYPtAntiProtons->GetXaxis()->SetTitleColor(1);

  //setting up the containers
  Int_t iBin[2];
  iBin[0] = nbinsY;
  iBin[1] = nbinsPt;
  Double_t *binLimY = new Double_t[nbinsY+1];
  Double_t *binLimPt = new Double_t[nbinsPt+1];
  //values for bin lower bounds
  for(Int_t i = 0; i <= nbinsY; i++) 
    binLimY[i]=(Double_t)fLowY  + (fHighY - fLowY)  /nbinsY*(Double_t)i;
  for(Int_t i = 0; i <= nbinsPt; i++) 
    binLimPt[i]=(Double_t)fLowPt  + (fHighPt - fLowPt)  /nbinsPt*(Double_t)i;

  fProtonContainer = new AliCFContainer("containerProtons",
					"container for protons",
					1,2,iBin);
  fProtonContainer->SetBinLimits(0,binLimY); //rapidity
  fProtonContainer->SetBinLimits(1,binLimPt); //pT
  fAntiProtonContainer = new AliCFContainer("containerAntiProtons",
					    "container for antiprotons",
					    1,2,iBin);
  fAntiProtonContainer->SetBinLimits(0,binLimY); //rapidity
  fAntiProtonContainer->SetBinLimits(1,binLimPt); //pT
}

//____________________________________________________________________//
void AliProtonAnalysis::InitAnalysisHistograms(Int_t nbinsY, Double_t *gY, 
					       Int_t nbinsPt, Double_t *gPt) {
  //Initializes the histograms using asymmetric values - global tracking
  fNBinsY = nbinsY;
  fMinY = gY[0];
  fMaxY = gY[nbinsY];
  fNBinsPt = nbinsPt;
  fMinPt = gPt[0];
  fMaxPt = gPt[nbinsPt];

  fHistEvents = new TH1I("fHistEvents","Analyzed events",1,0,1);

  fHistYPtProtons = new TH2D("fHistYPtProtons","Protons",
			     fNBinsY,gY,fNBinsPt,gPt);
  fHistYPtProtons->SetStats(kTRUE);
  fHistYPtProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtProtons->GetXaxis()->SetTitle("y");
  fHistYPtProtons->GetXaxis()->SetTitleColor(1);

  fHistYPtAntiProtons = new TH2D("fHistYPtAntiProtons","Antiprotons",
				 fNBinsY,gY,fNBinsPt,gPt);
  fHistYPtAntiProtons->SetStats(kTRUE);
  fHistYPtAntiProtons->GetYaxis()->SetTitle("P_{T} [GeV/c]");
  if(fProtonAnalysisBase->GetEtaMode())
    fHistYPtAntiProtons->GetXaxis()->SetTitle("#eta");
  else
    fHistYPtAntiProtons->GetXaxis()->SetTitle("y");
  fHistYPtAntiProtons->GetXaxis()->SetTitleColor(1);

  //setting up the containers
  Int_t iBin[2];
  iBin[0] = nbinsY;
  iBin[1] = nbinsPt;

  fProtonContainer = new AliCFContainer("containerProtons",
					"container for protons",
					1,2,iBin);
  fProtonContainer->SetBinLimits(0,gY); //rapidity
  fProtonContainer->SetBinLimits(1,gPt); //pT
  fAntiProtonContainer = new AliCFContainer("containerAntiProtons",
					    "container for antiprotons",
					    1,2,iBin);
  fAntiProtonContainer->SetBinLimits(0,gY); //rapidity
  fAntiProtonContainer->SetBinLimits(1,gPt); //pT
}

//____________________________________________________________________//
Bool_t AliProtonAnalysis::ReadFromFile(const char* filename) {
  //Read the containers from the existing file
  Bool_t status = kTRUE;

  TFile *file = TFile::Open(filename);
  if(!file) {
    cout<<"Could not find the input file "<<filename<<endl;
    status = kFALSE;
  }

  TList *list = (TList *)file->Get("outputList");
  if(list) {
    cout<<"Retrieving objects from the list "<<list->GetName()<<"..."<<endl; 
    fHistYPtProtons = (TH2D *)list->At(0);
    fHistYPtAntiProtons = (TH2D *)list->At(1);
    fHistEvents = (TH1I *)list->At(2);
    fProtonContainer = (AliCFContainer *)list->At(3);
    fAntiProtonContainer = (AliCFContainer *)list->At(4);
  }
  else if(!list) {
    cout<<"Retrieving objects from the file... "<<endl;
    fHistYPtProtons = (TH2D *)file->Get("fHistYPtProtons");
    fHistYPtAntiProtons = (TH2D *)file->Get("fHistYPtAntiProtons");
    fHistEvents = (TH1I *)file->Get("fHistEvents");
    fProtonContainer = (AliCFContainer *)file->Get("containerProtons");
    fAntiProtonContainer = (AliCFContainer *)file->Get("containerAntiProtons");
  }
  if((!fHistYPtProtons)||(!fHistYPtAntiProtons)||(!fHistEvents)
     ||(!fProtonContainer)||(!fAntiProtonContainer)) {
    cout<<"Input containers were not found!!!"<<endl;
    status = kFALSE;
  }
  else {
    //fHistYPtProtons = fProtonContainer->ShowProjection(0,1,0);
    //fHistYPtAntiProtons = fAntiProtonContainer->ShowProjection(0,1,0);
    fHistYPtProtons->Sumw2();
    fHistYPtAntiProtons->Sumw2();
  }

  return status;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetProtonYHistogram() {
  //Get the y histogram for protons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();

  TH1D *fYProtons = (TH1D *)fHistYPtProtons->ProjectionX("fYProtons",0,fHistYPtProtons->GetYaxis()->GetNbins(),"");
  //TH1D *fYProtons = fProtonContainer->ShowProjection(0,0); //variable-step
   
  fYProtons->SetStats(kFALSE);
  fYProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dy)");
  fYProtons->SetTitle("dN/dy protons");
  fYProtons->SetMarkerStyle(kFullCircle);
  fYProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
  fYProtons->Scale(1./nAnalyzedEvents);
  
  return fYProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetAntiProtonYHistogram() {
  //Get the y histogram for antiprotons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();
  
  TH1D *fYAntiProtons = (TH1D *)fHistYPtAntiProtons->ProjectionX("fYAntiProtons",0,fHistYPtAntiProtons->GetYaxis()->GetNbins(),"");
  //TH1D *fYAntiProtons = fAntiProtonContainer->ShowProjection(0,0);//variable-step 
 
  fYAntiProtons->SetStats(kFALSE);
  fYAntiProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dy)");
  fYAntiProtons->SetTitle("dN/dy antiprotons");
  fYAntiProtons->SetMarkerStyle(kFullCircle);
  fYAntiProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fYAntiProtons->Scale(1./nAnalyzedEvents);

  return fYAntiProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetProtonPtHistogram() {
  //Get the Pt histogram for protons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();
  
  TH1D *fPtProtons = (TH1D *)fHistYPtProtons->ProjectionY("fPtProtons",0,fHistYPtProtons->GetXaxis()->GetNbins(),""); 
  //TH1D *fPtProtons = fProtonContainer->ShowProjection(1,0); //variable-step

  fPtProtons->SetStats(kFALSE);
  fPtProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dP_{T})");
  fPtProtons->SetTitle("dN/dPt protons");
  fPtProtons->SetMarkerStyle(kFullCircle);
  fPtProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fPtProtons->Scale(1./nAnalyzedEvents);

  return fPtProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetAntiProtonPtHistogram() {
  //Get the Pt histogram for antiprotons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();
  
  TH1D *fPtAntiProtons = (TH1D *)fHistYPtAntiProtons->ProjectionY("fPtAntiProtons",0,fHistYPtProtons->GetXaxis()->GetNbins(),""); 
  //TH1D *fPtAntiProtons = fAntiProtonContainer->ShowProjection(1,0); //variable-step

  fPtAntiProtons->SetStats(kFALSE);
  fPtAntiProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dP_{T})");
  fPtAntiProtons->SetTitle("dN/dPt antiprotons");
  fPtAntiProtons->SetMarkerStyle(kFullCircle);
  fPtAntiProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fPtAntiProtons->Scale(1./nAnalyzedEvents);

  return fPtAntiProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetProtonCorrectedYHistogram() {
  //Get the corrected y histogram for protons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();

  TH1D *fYProtons = fCorrectProtons->Project(0); //0: rapidity
   
  fYProtons->SetStats(kFALSE);
  fYProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dy)");
  fYProtons->GetXaxis()->SetTitle("y");
  fYProtons->SetTitle("dN/dy protons");
  fYProtons->SetMarkerStyle(kFullCircle);
  fYProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fYProtons->Scale(1./nAnalyzedEvents);
  
  return fYProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetAntiProtonCorrectedYHistogram() {
  //Get the corrected y histogram for antiprotons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();

  TH1D *fYAntiProtons = fCorrectAntiProtons->Project(0); //0: rapidity
   
  fYAntiProtons->SetStats(kFALSE);
  fYAntiProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dy)");
  fYAntiProtons->GetXaxis()->SetTitle("y");
  fYAntiProtons->SetTitle("dN/dy protons");
  fYAntiProtons->SetMarkerStyle(kFullCircle);
  fYAntiProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fYAntiProtons->Scale(1./nAnalyzedEvents);
  
  return fYAntiProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetProtonCorrectedPtHistogram() {
  //Get the corrected Pt histogram for protons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();

  TH1D *fPtProtons = fCorrectProtons->Project(0); //0: rapidity
   
  fPtProtons->SetStats(kFALSE);
  fPtProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dP_{T})");
  fPtProtons->GetXaxis()->SetTitle("P_{T} [GeV/c]");
  fPtProtons->SetTitle("dN/dPt protons");
  fPtProtons->SetMarkerStyle(kFullCircle);
  fPtProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fPtProtons->Scale(1./nAnalyzedEvents);
  
  return fPtProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetAntiProtonCorrectedPtHistogram() {
  //Get the corrected Pt histogram for antiprotons
  Int_t nAnalyzedEvents = GetNumberOfAnalyzedEvents();

  TH1D *fPtAntiProtons = fCorrectAntiProtons->Project(0); //0: rapidity
   
  fPtAntiProtons->SetStats(kFALSE);
  fPtAntiProtons->GetYaxis()->SetTitle("(1/N_{events})(dN/dP_{T})");
  fPtAntiProtons->GetXaxis()->SetTitle("P_{T} [GeV/c]");
  fPtAntiProtons->SetTitle("dN/dPt antiprotons");
  fPtAntiProtons->SetMarkerStyle(kFullCircle);
  fPtAntiProtons->SetMarkerColor(4);
  if(nAnalyzedEvents > 0)
    fPtAntiProtons->Scale(1./nAnalyzedEvents);
  
  return fPtAntiProtons;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetYRatioHistogram() {
  //Returns the rapidity dependence of the ratio (uncorrected)
  TH1D *fYProtons = GetProtonYHistogram();
  TH1D *fYAntiProtons = GetAntiProtonYHistogram();
  
  TH1D *hRatioY = new TH1D("hRatioY","",fYProtons->GetNbinsX(),fYProtons->GetXaxis()->GetXmin(),fYProtons->GetXaxis()->GetXmax());
  hRatioY->Divide(fYAntiProtons,fYProtons,1.0,1.0);
  hRatioY->SetMarkerStyle(kFullCircle);
  hRatioY->SetMarkerColor(4);
  hRatioY->GetYaxis()->SetTitle("#bar{p}/p");
  hRatioY->GetYaxis()->SetTitleOffset(1.4);
  hRatioY->GetXaxis()->SetTitle("y");
  hRatioY->GetXaxis()->SetTitleColor(1);
  hRatioY->SetStats(kFALSE);

  return hRatioY;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetYRatioCorrectedHistogram(TH2D *gCorrectionProtons, 
						     TH2D *gCorrectionAntiProtons) {
  //Returns the rapidity dependence of the ratio (corrected)
  fHistYPtProtons->Multiply(gCorrectionProtons);
  TH1D *fYProtons = GetProtonYHistogram();
  fHistYPtAntiProtons->Multiply(gCorrectionAntiProtons);
  TH1D *fYAntiProtons = GetAntiProtonYHistogram();
  
  TH1D *hRatioY = new TH1D("hRatioY","",fYProtons->GetNbinsX(),fYProtons->GetXaxis()->GetXmin(),fYProtons->GetXaxis()->GetXmax());
  hRatioY->Divide(fYAntiProtons,fYProtons,1.0,1.0);
  hRatioY->SetMarkerStyle(kFullCircle);
  hRatioY->SetMarkerColor(4);
  hRatioY->GetYaxis()->SetTitle("#bar{p}/p");
  hRatioY->GetYaxis()->SetTitleOffset(1.4);
  hRatioY->GetXaxis()->SetTitle("y");
  hRatioY->GetXaxis()->SetTitleColor(1);
  hRatioY->SetStats(kFALSE);

  return hRatioY;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetPtRatioHistogram() {
  //Returns the pT dependence of the ratio (uncorrected)
  TH1D *fPtProtons = GetProtonPtHistogram();
  TH1D *fPtAntiProtons = GetAntiProtonPtHistogram();
  
  TH1D *hRatioPt = new TH1D("hRatioPt","",fPtProtons->GetNbinsX(),fPtProtons->GetXaxis()->GetXmin(),fPtProtons->GetXaxis()->GetXmax());
  hRatioPt->Divide(fPtAntiProtons,fPtProtons,1.0,1.0);
  hRatioPt->SetMarkerStyle(kFullCircle);
  hRatioPt->SetMarkerColor(4);
  hRatioPt->GetYaxis()->SetTitle("#bar{p}/p");
  hRatioPt->GetYaxis()->SetTitleOffset(1.4);
  hRatioPt->GetXaxis()->SetTitle("P_{T} [GeV/c]");
  hRatioPt->GetXaxis()->SetTitleColor(1);
  hRatioPt->SetStats(kFALSE);

  return hRatioPt;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetPtRatioCorrectedHistogram(TH2D *gCorrectionProtons, 
						      TH2D *gCorrectionAntiProtons) {
  //Returns the Pt dependence of the ratio (corrected)
  fHistYPtProtons->Multiply(gCorrectionProtons);
  TH1D *fPtProtons = GetProtonPtHistogram();
  fHistYPtAntiProtons->Multiply(gCorrectionAntiProtons);
  TH1D *fPtAntiProtons = GetAntiProtonPtHistogram();
  
  TH1D *hRatioPt = new TH1D("hRatioPt","",fPtProtons->GetNbinsX(),fPtProtons->GetXaxis()->GetXmin(),fPtProtons->GetXaxis()->GetXmax());
  hRatioPt->Divide(fPtAntiProtons,fPtProtons,1.0,1.0);
  hRatioPt->SetMarkerStyle(kFullCircle);
  hRatioPt->SetMarkerColor(4);
  hRatioPt->GetYaxis()->SetTitle("#bar{p}/p");
  hRatioPt->GetYaxis()->SetTitleOffset(1.4);
  hRatioPt->GetXaxis()->SetTitle("y");
  hRatioPt->GetXaxis()->SetTitleColor(1);
  hRatioPt->SetStats(kFALSE);

  return hRatioPt;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetYAsymmetryHistogram() {
  //Returns the rapidity dependence of the asymmetry (uncorrected)
  TH1D *fYProtons = GetProtonYHistogram();
  TH1D *fYAntiProtons = GetAntiProtonYHistogram();
  
  TH1D *hsum = new TH1D("hsumY","",fYProtons->GetNbinsX(),fYProtons->GetXaxis()->GetXmin(),fYProtons->GetXaxis()->GetXmax());
  hsum->Add(fYProtons,fYAntiProtons,1.0,1.0);

  TH1D *hdiff = new TH1D("hdiffY","",fYProtons->GetNbinsX(),fYProtons->GetXaxis()->GetXmin(),fYProtons->GetXaxis()->GetXmax());
  hdiff->Add(fYProtons,fYAntiProtons,1.0,-1.0);

  TH1D *hAsymmetryY = new TH1D("hAsymmetryY","",fYProtons->GetNbinsX(),fYProtons->GetXaxis()->GetXmin(),fYProtons->GetXaxis()->GetXmax());
  hAsymmetryY->Divide(hdiff,hsum,2.0,1.);
  hAsymmetryY->SetMarkerStyle(kFullCircle);
  hAsymmetryY->SetMarkerColor(4);
  hAsymmetryY->GetYaxis()->SetTitle("A_{p}");
  hAsymmetryY->GetYaxis()->SetTitleOffset(1.4);
  hAsymmetryY->GetXaxis()->SetTitle("y");
  hAsymmetryY->GetXaxis()->SetTitleColor(1);
  hAsymmetryY->SetStats(kFALSE);

  return hAsymmetryY;
}

//____________________________________________________________________//
TH1D *AliProtonAnalysis::GetPtAsymmetryHistogram() {
  //Returns the pT dependence of the asymmetry (uncorrected)
  TH1D *fPtProtons = GetProtonPtHistogram();
  TH1D *fPtAntiProtons = GetAntiProtonPtHistogram();
  
  TH1D *hsum = new TH1D("hsumPt","",fPtProtons->GetNbinsX(),fPtProtons->GetXaxis()->GetXmin(),fPtProtons->GetXaxis()->GetXmax());
  hsum->Add(fPtProtons,fPtAntiProtons,1.0,1.0);

  TH1D *hdiff = new TH1D("hdiffPt","",fPtProtons->GetNbinsX(),fPtProtons->GetXaxis()->GetXmin(),fPtProtons->GetXaxis()->GetXmax());
  hdiff->Add(fPtProtons,fPtAntiProtons,1.0,-1.0);

  TH1D *hAsymmetryPt = new TH1D("hAsymmetryPt","",fPtProtons->GetNbinsX(),fPtProtons->GetXaxis()->GetXmin(),fPtProtons->GetXaxis()->GetXmax());
  hAsymmetryPt->Divide(hdiff,hsum,2.0,1.);
  hAsymmetryPt->SetMarkerStyle(kFullCircle);
  hAsymmetryPt->SetMarkerColor(4);
  hAsymmetryPt->GetYaxis()->SetTitle("A_{p}");
  hAsymmetryPt->GetYaxis()->SetTitleOffset(1.4);
  hAsymmetryPt->GetXaxis()->SetTitle("P_{T} [GeV/c]");
  hAsymmetryPt->GetXaxis()->SetTitleColor(1);
  hAsymmetryPt->SetStats(kFALSE);

  return hAsymmetryPt;
}

//____________________________________________________________________//
void AliProtonAnalysis::Analyze(AliESDEvent* esd,
				const AliESDVertex *vertex) {
  //Main analysis part - ESD
  Int_t nTracks = 0;
  Int_t nIdentifiedProtons = 0, nIdentifiedAntiProtons = 0;
  Int_t nSurvivedProtons = 0, nSurvivedAntiProtons = 0;

  fHistEvents->Fill(0); //number of analyzed events
  Double_t containerInput[2] ;
  Double_t gPt = 0.0, gP = 0.0;
  nTracks = esd->GetNumberOfTracks();
  for(Int_t iTracks = 0; iTracks < nTracks; iTracks++) {
    AliESDtrack* track = esd->GetTrack(iTracks);
    AliESDtrack trackTPC;

    //in case it's a TPC only track relate it to the proper vertex
    /*if(fProtonAnalysisBase->GetAnalysisMode()==AliProtonAnalysisBase::kTPC) {
      Float_t p[2],cov[3];
      track->GetImpactParametersTPC(p,cov);
      if (p[0]==0 && p[1]==0)  
	track->RelateToVertexTPC(((AliESDEvent*)esd)->GetPrimaryVertexTPC(),esd->GetMagneticField(),kVeryBig);
      if (!track->FillTPCOnlyTrack(trackTPC)) {
	continue;
      }
      track = &trackTPC ;
      }*/

    if((fProtonAnalysisBase->GetAnalysisMode()==AliProtonAnalysisBase::kTPC)||(fProtonAnalysisBase->GetAnalysisMode()==AliProtonAnalysisBase::kHybrid)) {
      AliExternalTrackParam *tpcTrack = (AliExternalTrackParam *)track->GetTPCInnerParam();
      if(!tpcTrack) continue;
      gPt = tpcTrack->Pt();
      gP = tpcTrack->P();
      
      if(fProtonAnalysisBase->IsProton(track)) {
	  if(tpcTrack->Charge() > 0) {
	    nIdentifiedProtons += 1;
	    if(!fProtonAnalysisBase->IsAccepted(esd,vertex,track)) continue;//track cuts
	    if(!fProtonAnalysisBase->IsInPhaseSpace(track)) continue; //track outside the analyzed y-Pt
	    nSurvivedProtons += 1;
	    if(fProtonAnalysisBase->GetEtaMode()) {
	      fHistYPtProtons->Fill(tpcTrack->Eta(),
				    gPt);
	      containerInput[0] = tpcTrack->Eta();
	    }
	    else {
	      fHistYPtProtons->Fill(fProtonAnalysisBase->Rapidity(tpcTrack->Px(),
								  tpcTrack->Py(),
								  tpcTrack->Pz()),
				    gPt);
	      //fill the container
	      containerInput[0] = fProtonAnalysisBase->Rapidity(tpcTrack->Px(),
								tpcTrack->Py(),
								tpcTrack->Pz());
	    }
	    containerInput[1] = gPt;
	    fProtonContainer->Fill(containerInput,0);   
	  }//protons
	  else if(tpcTrack->Charge() < 0) {
	    nIdentifiedAntiProtons += 1;
	    if(!fProtonAnalysisBase->IsAccepted(esd,vertex,track)) continue;//track cuts
	    if(!fProtonAnalysisBase->IsInPhaseSpace(track)) continue; //track outside the analyzed y-Pt
	    nSurvivedAntiProtons += 1;
	    if(fProtonAnalysisBase->GetEtaMode()) {
	      fHistYPtAntiProtons->Fill(tpcTrack->Eta(),
					gPt);
	      containerInput[0] = tpcTrack->Eta();
	    }
	    else {
	      fHistYPtAntiProtons->Fill(fProtonAnalysisBase->Rapidity(tpcTrack->Px(),
								      tpcTrack->Py(),
								      tpcTrack->Pz()),
					gPt);
	      //fill the container
	      containerInput[0] = fProtonAnalysisBase->Rapidity(tpcTrack->Px(),
								tpcTrack->Py(),
								tpcTrack->Pz());
	    }
	    containerInput[1] = gPt;
	    fAntiProtonContainer->Fill(containerInput,0);
	  }//antiprotons   
      }//proton check
    }//TPC only tracks
    else if(fProtonAnalysisBase->GetAnalysisMode() == AliProtonAnalysisBase::kGlobal) {
      gPt = track->Pt();
      gP = track->P();
      
      if(fProtonAnalysisBase->IsProton(track)) {
	if(track->Charge() > 0) {
	  nIdentifiedProtons += 1;
	  if(!fProtonAnalysisBase->IsAccepted(esd,vertex,track)) continue;//track cuts
	  if(!fProtonAnalysisBase->IsInPhaseSpace(track)) continue; //track outside the analyzed y-Pt
	  nSurvivedProtons += 1;
	  if(fProtonAnalysisBase->GetEtaMode()) {
	    fHistYPtProtons->Fill(track->Eta(),
				  gPt);
	    containerInput[0] = track->Eta();
	  }
	  else {
	    fHistYPtProtons->Fill(fProtonAnalysisBase->Rapidity(track->Px(),
								track->Py(),
								track->Pz()),
				  gPt);
	    //fill the container
	    containerInput[0] = fProtonAnalysisBase->Rapidity(track->Px(),
							      track->Py(),
							      track->Pz());
	  }
	  containerInput[1] = gPt;
	  fProtonContainer->Fill(containerInput,0);   
	}//protons
	else if(track->Charge() < 0) {
	  nIdentifiedAntiProtons += 1;
	  if(!fProtonAnalysisBase->IsAccepted(esd,vertex,track)) continue;//track cuts
	  if(!fProtonAnalysisBase->IsInPhaseSpace(track)) continue; //track outside the analyzed y-Pt
	  nSurvivedAntiProtons += 1;
	  if(fProtonAnalysisBase->GetEtaMode()) {
	    fHistYPtAntiProtons->Fill(track->Eta(),
				      gPt);
	    containerInput[0] = track->Eta();
	  }
	  else {
	    fHistYPtAntiProtons->Fill(fProtonAnalysisBase->Rapidity(track->Px(),
								    track->Py(),
								    track->Pz()),
				      gPt);
	    //fill the container
	    containerInput[0] = fProtonAnalysisBase->Rapidity(track->Px(),
							      track->Py(),
							      track->Pz());
	  }
	  containerInput[1] = gPt;
	  fAntiProtonContainer->Fill(containerInput,0);   
	}//antiprotons
      }//proton check 
    }//combined tracking
  }//track loop 
  
  if(fProtonAnalysisBase->GetDebugMode())
    Printf("Initial number of tracks: %d | Identified (anti)protons: %d - %d | Survived (anti)protons: %d - %d",nTracks,nIdentifiedProtons,nIdentifiedAntiProtons,nSurvivedProtons,nSurvivedAntiProtons);
}

//____________________________________________________________________//
void AliProtonAnalysis::Analyze(AliAODEvent* const fAOD) {
  //Main analysis part - AOD
  fHistEvents->Fill(0); //number of analyzed events
  Int_t nTracks = fAOD->GetNumberOfTracks();
  for(Int_t iTracks = 0; iTracks < nTracks; iTracks++) {
    AliAODTrack* track = fAOD->GetTrack(iTracks);
    Double_t gPt = track->Pt();
    Double_t gP = track->P();
    
    //pid
    Double_t probability[10];
    track->GetPID(probability);
    Double_t rcc = 0.0;
    for(Int_t i = 0; i < AliPID::kSPECIESN; i++) rcc += probability[i]*fProtonAnalysisBase->GetParticleFraction(i,gP);
    if(rcc == 0.0) continue;
    Double_t w[10];
    for(Int_t i = 0; i < AliPID::kSPECIESN; i++) w[i] = probability[i]*fProtonAnalysisBase->GetParticleFraction(i,gP)/rcc;
    Long64_t fParticleType = TMath::LocMax(AliPID::kSPECIESN,w);
    if(fParticleType == 4) {
      if(track->Charge() > 0) 
	fHistYPtProtons->Fill(track->Y(fParticleType),gPt);
      else if(track->Charge() < 0) 
	fHistYPtAntiProtons->Fill(track->Y(fParticleType),gPt);
    }//proton check
  }//track loop 
}

//____________________________________________________________________//
void AliProtonAnalysis::Analyze(AliStack* const stack, 
				Bool_t iInclusive) {
  //Main analysis part - MC
  fHistEvents->Fill(0); //number of analyzed events

  Int_t nParticles = 0;
  //inclusive protons - 
  if(iInclusive) nParticles = stack->GetNtrack();
  else nParticles = stack->GetNprimary();

  for(Int_t i = 0; i < nParticles; i++) {
    TParticle *particle = stack->Particle(i);
    if(!particle) continue;

    //in case of inclusive protons reject the secondaries from hadronic inter.
    if(particle->GetUniqueID() == 13) continue;

    if(TMath::Abs(particle->Eta()) > 1.0) continue;
    if((particle->Pt() > fMaxPt)||(particle->Pt() < fMinPt)) continue;
    if((fProtonAnalysisBase->Rapidity(particle->Px(),particle->Py(),particle->Pz()) > fMaxY)||(fProtonAnalysisBase->Rapidity(particle->Px(),particle->Py(),particle->Pz()) < fMinY)) continue;

    Int_t pdgcode = particle->GetPdgCode();
    if(pdgcode == 2212) fHistYPtProtons->Fill(fProtonAnalysisBase->Rapidity(particle->Px(),
									    particle->Py(),
									    particle->Pz()),
					      particle->Pt());
    if(pdgcode == -2212) fHistYPtAntiProtons->Fill(fProtonAnalysisBase->Rapidity(particle->Px(),
										 particle->Py(),
										 particle->Pz()),
						   particle->Pt());
  }//particle loop
}

//____________________________________________________________________//
Bool_t AliProtonAnalysis::PrintMean(TH1 *hist, Double_t edge) {
  //calculates the mean value of the ratio/asymmetry within \pm edge
  Double_t sum = 0.0;
  Int_t nentries = 0;
  //calculate the mean
  for(Int_t i = 0; i < hist->GetXaxis()->GetNbins(); i++) {
    Double_t x = hist->GetBinCenter(i+1);
    Double_t y = hist->GetBinContent(i+1);
    if(TMath::Abs(x) < edge) {
      sum += y;
      nentries += 1;
    }
  }
  Double_t mean = 0.0;
  if(nentries != 0)
    mean = sum/nentries;

  //calculate the error
  for(Int_t i = 0; i < hist->GetXaxis()->GetNbins(); i++) {
    Double_t x = hist->GetBinCenter(i+1);
    Double_t y = hist->GetBinContent(i+1);
    if(TMath::Abs(x) < edge) {
      sum += TMath::Power((mean - y),2);
      nentries += 1;
    }
  }

  Double_t error = 0.0;
  if(nentries != 0)
    error =  TMath::Sqrt(sum)/nentries;

  cout<<"========================================="<<endl;
  cout<<"Input distribution: "<<hist->GetName()<<endl;
  cout<<"Interval used: -"<<edge<<" -> "<<edge<<endl;
  cout<<"Mean value :"<<mean<<endl;
  cout<<"Error: "<<error<<endl;
  cout<<"========================================="<<endl;

  return 0;
}

//____________________________________________________________________//
Bool_t AliProtonAnalysis::PrintYields(TH1 *hist, Double_t edge) {
  //calculates the (anti)proton yields within the \pm edge
  Double_t sum = 0.0, sumerror = 0.0;
  Double_t error = 0.0;
  for(Int_t i = 0; i < hist->GetXaxis()->GetNbins(); i++) {
    Double_t x = hist->GetBinCenter(i+1);
    Double_t y = hist->GetBinContent(i+1);
    if(TMath::Abs(x) < edge) {
      sum += y;
      sumerror += TMath::Power(hist->GetBinError(i+1),2); 
    }
  }

  error = TMath::Sqrt(sumerror);

  cout<<"========================================="<<endl;
  cout<<"Input distribution: "<<hist->GetName()<<endl;
  cout<<"Interval used: -"<<edge<<" -> "<<edge<<endl;
  cout<<"Yields :"<<sum<<endl;
  cout<<"Error: "<<error<<endl;
  cout<<"========================================="<<endl;

  return 0;
}

//____________________________________________________________________//
void AliProtonAnalysis::Correct(Int_t step) {
  //Applies the correction maps to the initial containers
  fCorrectProtons = new AliCFDataGrid("correctProtons",
				      "corrected data",
				      *fProtonContainer);
  fCorrectProtons->SetMeasured(0);
  fCorrectProtons->ApplyEffCorrection(*(AliCFEffGrid *)fEffGridListProtons->At(step));

  fCorrectAntiProtons = new AliCFDataGrid("correctAntiProtons",
					  "corrected data",
					  *fAntiProtonContainer);
  fCorrectAntiProtons->SetMeasured(0);
  fCorrectAntiProtons->ApplyEffCorrection(*(AliCFEffGrid *)fEffGridListAntiProtons->At(step));
}

//____________________________________________________________________//
Bool_t AliProtonAnalysis::ReadCorrectionContainer(const char* filename) {
  // Reads the outout of the correction framework task
  // Creates the correction maps
  // Puts the results in the different TList objects
  Bool_t status = kTRUE;

  TFile *file = TFile::Open(filename);
  if(!file) {
    cout<<"Could not find the input CORRFW file "<<filename<<endl;
    status = kFALSE;
  }

  //________________________________________//
  //Protons
  fEffGridListProtons = new TList();
  fCorrectionListProtons2D = new TList(); 
  fEfficiencyListProtons1D = new TList(); 
  fCorrectionListProtons1D = new TList();
  
  AliCFContainer *corrfwContainerProtons = (AliCFContainer*) (file->Get("containerProtons"));
  if(!corrfwContainerProtons) {
    cout<<"CORRFW container for protons not found!"<<endl;
    status = kFALSE;
  }
  
  Int_t nSteps = corrfwContainerProtons->GetNStep();
  TH2D *gYPt[4];
  //currently the GRID is formed by the y-pT parameters
  //Add Vz as a next step
  Int_t iRap = 0, iPt = 1;
  AliCFEffGrid *effProtonsStep0Step1 = new AliCFEffGrid("eff10",
					 "effProtonsStep0Step1",
					 *corrfwContainerProtons);
  effProtonsStep0Step1->CalculateEfficiency(1,0); //eff= step1/step0
  fEffGridListProtons->Add(effProtonsStep0Step1);
  gYPt[0] = effProtonsStep0Step1->Project(iRap,iPt);
  fCorrectionListProtons2D->Add(gYPt[0]);
  
  AliCFEffGrid *effProtonsStep0Step2 = new AliCFEffGrid("eff20",
					 "effProtonsStep0Step2",
					 *corrfwContainerProtons);
  effProtonsStep0Step2->CalculateEfficiency(2,0); //eff= step2/step0
  fEffGridListProtons->Add(effProtonsStep0Step2);
  gYPt[1] = effProtonsStep0Step2->Project(iRap,iPt);
  fCorrectionListProtons2D->Add(gYPt[1]);

  AliCFEffGrid *effProtonsStep0Step3 = new AliCFEffGrid("eff30",
					 "effProtonsStep0Step3",
					 *corrfwContainerProtons);
  effProtonsStep0Step3->CalculateEfficiency(3,0); //eff= step1/step0
  fEffGridListProtons->Add(effProtonsStep0Step3);
  gYPt[2] = effProtonsStep0Step3->Project(iRap,iPt);
  fCorrectionListProtons2D->Add(gYPt[2]);

  TH1D *gEfficiency[2][3]; //efficiency as a function of pT and of y (raws-[2])
  TH1D *gCorrection[2][3]; //efficiency as a function of pT and of y (raws-[2])
  TString gTitle;
  //Get the projection of the efficiency maps
  for(Int_t iParameter = 0; iParameter < 2; iParameter++) {
    gEfficiency[iParameter][0] = effProtonsStep0Step1->Project(iParameter);
    gTitle = "ProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step1"; 
    gEfficiency[iParameter][0]->SetName(gTitle.Data());
    fEfficiencyListProtons1D->Add(gEfficiency[iParameter][0]);  
    gTitle = "ProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step1"; 
    gCorrection[iParameter][0] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][0]->GetNbinsX(),
					  gEfficiency[iParameter][0]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][0]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][0]->GetNbinsX(); iBin++)
      gCorrection[iParameter][0]->SetBinContent(iBin,1.0);

    gEfficiency[iParameter][1] = effProtonsStep0Step2->Project(iParameter);
    gTitle = "ProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step2"; 
    gEfficiency[iParameter][1]->SetName(gTitle.Data());
    fEfficiencyListProtons1D->Add(gEfficiency[iParameter][1]);  
    gTitle = "ProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step2"; 
    gCorrection[iParameter][1] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][1]->GetNbinsX(),
					  gEfficiency[iParameter][1]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][1]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][1]->GetNbinsX(); iBin++)
      gCorrection[iParameter][1]->SetBinContent(iBin,1.0);

    gEfficiency[iParameter][2] = effProtonsStep0Step3->Project(iParameter);
    gTitle = "ProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step3"; 
    gEfficiency[iParameter][2]->SetName(gTitle.Data());
    fEfficiencyListProtons1D->Add(gEfficiency[iParameter][2]);  
    gTitle = "ProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step3"; 
    gCorrection[iParameter][2] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][2]->GetNbinsX(),
					  gEfficiency[iParameter][2]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][2]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][2]->GetNbinsX(); iBin++)
      gCorrection[iParameter][2]->SetBinContent(iBin,1.0);
  }//parameter loop
  //Calculate the 1D correction parameters as a function of y and pT
  for(Int_t iParameter = 0; iParameter < 2; iParameter++) { 
    for(Int_t iStep = 1; iStep < nSteps; iStep++) { 
      gCorrection[iParameter][iStep-1]->Divide(gEfficiency[iParameter][iStep-1]);
      fCorrectionListProtons1D->Add(gCorrection[iParameter][iStep-1]);  
    }
  }

  //________________________________________//
  //AntiProtons
  fEffGridListAntiProtons = new TList();
  fCorrectionListAntiProtons2D = new TList(); 
  fEfficiencyListAntiProtons1D = new TList(); 
  fCorrectionListAntiProtons1D = new TList();
  
  AliCFContainer *corrfwContainerAntiProtons = (AliCFContainer*) (file->Get("containerAntiProtons"));
  if(!corrfwContainerAntiProtons) {
    cout<<"CORRFW container for antiprotons not found!"<<endl;
    status = kFALSE;
  }
  
  nSteps = corrfwContainerAntiProtons->GetNStep();
  //currently the GRID is formed by the y-pT parameters
  //Add Vz as a next step
  AliCFEffGrid *effAntiProtonsStep0Step1 = new AliCFEffGrid("eff10",
					 "effAntiProtonsStep0Step1",
					 *corrfwContainerAntiProtons);
  effAntiProtonsStep0Step1->CalculateEfficiency(1,0); //eff= step1/step0
  fEffGridListAntiProtons->Add(effAntiProtonsStep0Step1);
  gYPt[0] = effAntiProtonsStep0Step1->Project(iRap,iPt);
  fCorrectionListAntiProtons2D->Add(gYPt[0]);
  
  AliCFEffGrid *effAntiProtonsStep0Step2 = new AliCFEffGrid("eff20",
					 "effAntiProtonsStep0Step2",
					 *corrfwContainerAntiProtons);
  effAntiProtonsStep0Step2->CalculateEfficiency(2,0); //eff= step2/step0
  fEffGridListAntiProtons->Add(effAntiProtonsStep0Step2);
  gYPt[1] = effAntiProtonsStep0Step2->Project(iRap,iPt);
  fCorrectionListAntiProtons2D->Add(gYPt[1]);

  AliCFEffGrid *effAntiProtonsStep0Step3 = new AliCFEffGrid("eff30",
					 "effAntiProtonsStep0Step3",
					 *corrfwContainerAntiProtons);
  effAntiProtonsStep0Step3->CalculateEfficiency(3,0); //eff= step1/step0
  fEffGridListAntiProtons->Add(effAntiProtonsStep0Step3);
  gYPt[2] = effAntiProtonsStep0Step3->Project(iRap,iPt);
  fCorrectionListAntiProtons2D->Add(gYPt[2]);

  //Get the projection of the efficiency maps
  for(Int_t iParameter = 0; iParameter < 2; iParameter++) {
    gEfficiency[iParameter][0] = effAntiProtonsStep0Step1->Project(iParameter);
    gTitle = "AntiProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step1"; 
    gEfficiency[iParameter][0]->SetName(gTitle.Data());
    fEfficiencyListAntiProtons1D->Add(gEfficiency[iParameter][0]);  
    gTitle = "AntiProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step1"; 
    gCorrection[iParameter][0] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][0]->GetNbinsX(),
					  gEfficiency[iParameter][0]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][0]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][0]->GetNbinsX(); iBin++)
      gCorrection[iParameter][0]->SetBinContent(iBin,1.0);

    gEfficiency[iParameter][1] = effAntiProtonsStep0Step2->Project(iParameter);
    gTitle = "AntiProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step2"; 
    gEfficiency[iParameter][1]->SetName(gTitle.Data());
    fEfficiencyListAntiProtons1D->Add(gEfficiency[iParameter][1]);  
    gTitle = "AntiProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step2"; 
    gCorrection[iParameter][1] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][1]->GetNbinsX(),
					  gEfficiency[iParameter][1]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][1]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][1]->GetNbinsX(); iBin++)
      gCorrection[iParameter][1]->SetBinContent(iBin,1.0);

    gEfficiency[iParameter][2] = effAntiProtonsStep0Step3->Project(iParameter);
    gTitle = "AntiProtonsEfficiency_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step3"; 
    gEfficiency[iParameter][2]->SetName(gTitle.Data());
    fEfficiencyListAntiProtons1D->Add(gEfficiency[iParameter][2]);  
    gTitle = "AntiProtonsCorrection_Parameter"; gTitle += iParameter+1;
    gTitle += "_Step0_Step3"; 
    gCorrection[iParameter][2] = new TH1D(gTitle.Data(),
					  gTitle.Data(),
					  gEfficiency[iParameter][2]->GetNbinsX(),
					  gEfficiency[iParameter][2]->GetXaxis()->GetXmin(),
					  gEfficiency[iParameter][2]->GetXaxis()->GetXmax());
    //initialisation of the correction
    for(Int_t iBin = 1; iBin <= gEfficiency[iParameter][2]->GetNbinsX(); iBin++)
      gCorrection[iParameter][2]->SetBinContent(iBin,1.0);
  }//parameter loop
  //Calculate the 1D correction parameters as a function of y and pT
  for(Int_t iParameter = 0; iParameter < 2; iParameter++) { 
    for(Int_t iStep = 1; iStep < nSteps; iStep++) { 
      gCorrection[iParameter][iStep-1]->Divide(gEfficiency[iParameter][iStep-1]);
      fCorrectionListAntiProtons1D->Add(gCorrection[iParameter][iStep-1]);  
    }
  }

  return status;
}
 








