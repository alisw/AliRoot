//------------------------------------------------------------------------------
// Implementation of AliComparisonEff class. It keeps information from 
// comparison of reconstructed and MC particle tracks. In addtion, 
// it keeps selection cuts used during comparison. The comparison 
// information is stored in the ROOT histograms. Analysis of these 
// histograms can be done by using Analyse() class function. The result of 
// the analysis (histograms) are stored in the output picture_eff.root file.
//  
// Author: J.Otwinowski 04/02/2008 
//------------------------------------------------------------------------------

/*
  // after running analysis, read the file, and get component
  gSystem->Load("libPWG1.so");
  TFile f("Output.root");
  AliComparisonEff * comp = (AliComparisonEff*)f.Get("AliComparisonEff");


  // analyse comparison data (output stored in pictures_eff.root)
  comp->Analyse();
 
  // paramtetrisation of the TPC track length (for information only)
  TF1 fl("fl","((min(250./(abs(x+0.000001)),250)-90))",0,2);  // length function
  TF1 fl2("fl2","[0]/((min(250./(abs(x+0.000001)),250)-90))^[1]",0,2);
  fl2.SetParameter(1,1);
  fl2.SetParameter(0,1);
*/


#include <iostream>

#include "TFile.h"
#include "TCint.h"
#include "TH3F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TGraph2D.h"
#include "TCanvas.h"
#include "TGraph.h"
// 
#include "AliESDEvent.h"  
#include "AliESD.h"
#include "AliESDfriend.h"
#include "AliESDfriendTrack.h"
#include "AliRecInfoCuts.h" 
#include "AliMCInfoCuts.h" 
#include "AliLog.h" 
//
#include "AliMathBase.h"
#include "AliTreeDraw.h" 
#include "AliMagFMaps.h" 
#include "AliESDVertex.h" 
#include "AliExternalTrackParam.h" 
#include "AliTracker.h" 

#include "AliMCInfo.h" 
#include "AliESDRecInfo.h" 
#include "AliComparisonEff.h" 

using namespace std;


ClassImp(AliComparisonEff)

//_____________________________________________________________________________
AliComparisonEff::AliComparisonEff():
  TNamed("AliComparisonEff","AliComparisonEff"),

  // histograms
 
  fMCPt(0),
  fMCRecPt(0),
  fMCRecPrimPt(0),
  fMCRecSecPt(0),

  fEffTPCPt(0),
  fEffTPCPtMC(0),
  fEffTPCPtF(0),
  //
  fEffTPCPt_P(0),
  fEffTPCPtMC_P(0),
  fEffTPCPtF_P(0),
  //
  fEffTPCPt_Pi(0),
  fEffTPCPtMC_Pi(0),
  fEffTPCPtF_Pi(0),
  //
  fEffTPCPt_K(0),
  fEffTPCPtMC_K(0),
  fEffTPCPtF_K(0),
 
  fEffTPCTan(0),
  fEffTPCTanMC(0),
  fEffTPCTanF(0),
  //
  fEffTPCPtTan(0),
  fEffTPCPtTanMC(0),
  fEffTPCPtTanF(0),

  // Cuts 
  fCutsRC(0), 
  fCutsMC(0),

  fVertex(0)
{
  // init vertex
  fVertex = new AliESDVertex();
  fVertex->SetXv(0.0); fVertex->SetYv(0.0); fVertex->SetZv(0.0); 

  for(Int_t i=0; i<4; ++i)
  {
    fTPCPtDCASigmaIdeal[i]=0;
    fTPCPtDCASigmaFull[i]=0;
    fTPCPtDCASigmaDay0[i]=0;

    fTPCPtDCAXY[i]=0;
    fTPCPtDCAZ[i]=0;

	fTPCPtDCASigmaIdealPid[i]=0;
	fTPCPtDCASigmaFullPid[i]=0;
	fTPCPtDCASigmaDay0Pid[i]=0;

	fTPCPtDCAXYPid[i]=0;   
	fTPCPtDCAZPid[i]=0; 
  }
  InitHisto();
  InitCuts();
}

//_____________________________________________________________________________
AliComparisonEff::~AliComparisonEff(){

  // 
  if(fMCPt)  delete  fEffTPCPt; fEffTPCPt=0;
  if(fMCRecPt)  delete  fMCRecPt; fMCRecPt=0;
  if(fMCRecPrimPt)  delete  fMCRecPrimPt; fMCRecPrimPt=0;
  if(fMCRecSecPt)  delete  fMCRecSecPt; fMCRecSecPt=0;

  // 
  if(fEffTPCPt)   delete  fEffTPCPt;   fEffTPCPt=0;
  if(fEffTPCPtMC) delete  fEffTPCPtMC; fEffTPCPtMC=0;
  if(fEffTPCPtF)  delete  fEffTPCPtF;  fEffTPCPtF=0;

  // 
  if(fEffTPCPt_P)   delete  fEffTPCPt_P;   fEffTPCPt_P=0;
  if(fEffTPCPtMC_P) delete  fEffTPCPtMC_P; fEffTPCPtMC_P=0;
  if(fEffTPCPtF_P)  delete  fEffTPCPtF_P;  fEffTPCPtF_P=0;

  // 
  if(fEffTPCPt_Pi)   delete  fEffTPCPt_Pi;   fEffTPCPt_Pi=0;
  if(fEffTPCPtMC_Pi) delete  fEffTPCPtMC_Pi; fEffTPCPtMC_Pi=0;
  if(fEffTPCPtF_Pi)  delete  fEffTPCPtF_Pi;  fEffTPCPtF_Pi=0;

  // 
  if(fEffTPCPt_K)   delete  fEffTPCPt_K;   fEffTPCPt_K=0;
  if(fEffTPCPtMC_K) delete  fEffTPCPtMC_K; fEffTPCPtMC_K=0;
  if(fEffTPCPtF_K)  delete  fEffTPCPtF_K;  fEffTPCPtF_K=0;

  // 
  if(fEffTPCTan)   delete  fEffTPCTan;   fEffTPCTan=0;
  if(fEffTPCTanMC) delete  fEffTPCTanMC; fEffTPCTanMC=0;
  if(fEffTPCTanF)  delete  fEffTPCTanF;  fEffTPCTanF=0;

  //
  if(fEffTPCPtTan)   delete  fEffTPCPtTan;   fEffTPCPtTan=0;
  if(fEffTPCPtTanMC) delete  fEffTPCPtTanMC; fEffTPCPtTanMC=0;
  if(fEffTPCPtTanF)  delete  fEffTPCPtTanF;  fEffTPCPtTanF=0;

  for(Int_t i=0; i<4; ++i)
  {
    if(fTPCPtDCASigmaIdeal[i]) delete  fTPCPtDCASigmaIdeal[i];  fTPCPtDCASigmaIdeal[i]=0;
    if(fTPCPtDCASigmaFull[i]) delete  fTPCPtDCASigmaFull[i];  fTPCPtDCASigmaFull[i]=0;
    if(fTPCPtDCASigmaDay0[i]) delete  fTPCPtDCASigmaDay0[i];  fTPCPtDCASigmaDay0[i]=0;

    if(fTPCPtDCAXY[i]) delete  fTPCPtDCAXY[i];  fTPCPtDCAXY[i]=0;
    if(fTPCPtDCAZ[i]) delete  fTPCPtDCAZ[i];  fTPCPtDCAZ[i]=0;

 	if(fTPCPtDCASigmaIdealPid[i]) delete  fTPCPtDCASigmaIdealPid[i];  fTPCPtDCASigmaIdealPid[i]=0;
	if(fTPCPtDCASigmaFullPid[i]) delete  fTPCPtDCASigmaFullPid[i];  fTPCPtDCASigmaFullPid[i]=0;
	if(fTPCPtDCASigmaDay0Pid[i]) delete  fTPCPtDCASigmaDay0Pid[i];  fTPCPtDCASigmaDay0Pid[i]=0;

	if(fTPCPtDCAXYPid[i]) delete  fTPCPtDCAXYPid[i]; fTPCPtDCAXYPid[i]=0;
	if(fTPCPtDCAZPid[i]) delete   fTPCPtDCAZPid[i];  fTPCPtDCAZPid[i]=0;
  }
}

//_____________________________________________________________________________
void AliComparisonEff::InitHisto(){

  // Init histograms
  //
  fMCPt = new TH1F("fMCPt","fMCPt",50,0.1,3);           
  fMCPt->SetXTitle("p_{t}");
  fMCPt->SetYTitle("yield");

  fMCRecPt = new TH1F("fMCRecPt","fMCRecPt",50,0.1,3);           
  fMCRecPt->SetXTitle("p_{t}");
  fMCRecPt->SetYTitle("yield");

  fMCRecPrimPt = new TH1F("fMCRecPrimPt","fMCRecPrimPt",50,0.1,3);           
  fMCRecPrimPt->SetXTitle("p_{t}");
  fMCRecPrimPt->SetYTitle("yield");

  fMCRecSecPt = new TH1F("fMCRecSecPt","fMCRecSecPt",50,0.1,3);           
  fMCRecSecPt->SetXTitle("p_{t}");
  fMCRecSecPt->SetYTitle("yield");

  // Efficiency as function of pt
  fEffTPCPt = new TProfile("Eff_pt","Eff_Pt",50,0.1,3);           
  fEffTPCPt->SetXTitle("p_{t}");
  fEffTPCPt->SetYTitle("TPC Efficiency");

  fEffTPCPtMC = new TProfile("MC_Eff_pt","MC_Eff_Pt",50,0.1,3);   
  fEffTPCPtMC->SetXTitle("p_{t}");
  fEffTPCPtMC->SetYTitle("MC TPC Efficiency");

  fEffTPCPtF = new TProfile("F_Eff_pt","F_Eff_Pt",50,0.1,3);     
  fEffTPCPtF->SetXTitle("p_{t}");
  fEffTPCPtF->SetYTitle("TPC Findable Efficiency");

  // Efficiency as function of pt protons
  fEffTPCPt_P = new TProfile("Eff_pt_P","Eff_Pt_P",50,0.1,3);           
  fEffTPCPt_P->SetXTitle("p_{t}");
  fEffTPCPt_P->SetYTitle("TPC Efficiency");

  fEffTPCPtMC_P = new TProfile("MC_Eff_pt_P","MC_Eff_Pt_P",50,0.1,3);   
  fEffTPCPtMC_P->SetXTitle("p_{t}");
  fEffTPCPtMC_P->SetYTitle("MC TPC Efficiency");

  fEffTPCPtF_P = new TProfile("F_Eff_pt_P","F_Eff_Pt_P",50,0.1,3);     
  fEffTPCPtF_P->SetXTitle("p_{t}");
  fEffTPCPtF_P->SetYTitle("TPC Findable Efficiency");

  // Efficiency as function of pt pions
  fEffTPCPt_Pi = new TProfile("Eff_pt_Pi","Eff_Pit_Pi",50,0.1,3);           
  fEffTPCPt_Pi->SetXTitle("p_{t}");
  fEffTPCPt_Pi->SetYTitle("TPC Efficiency");

  fEffTPCPtMC_Pi = new TProfile("MC_Eff_pt_Pi","MC_Eff_Pit_Pi",50,0.1,3);   
  fEffTPCPtMC_Pi->SetXTitle("p_{t}");
  fEffTPCPtMC_Pi->SetYTitle("MC TPC Efficiency");

  fEffTPCPtF_Pi = new TProfile("F_Eff_pt_Pi","F_Eff_Pit_Pi",50,0.1,3);     
  fEffTPCPtF_Pi->SetXTitle("p_{t}");
  fEffTPCPtF_Pi->SetYTitle("TPC Findable Efficiency");

  // Efficiency as function of pt kaons
  fEffTPCPt_K = new TProfile("Eff_pt_K","Eff_Kt_K",50,0.1,3);           
  fEffTPCPt_K->SetXTitle("p_{t}");
  fEffTPCPt_K->SetYTitle("TPC Efficiency");

  fEffTPCPtMC_K = new TProfile("MC_Eff_pt_K","MC_Eff_Kt_K",50,0.1,3);   
  fEffTPCPtMC_K->SetXTitle("p_{t}");
  fEffTPCPtMC_K->SetYTitle("MC TPC Efficiency");

  fEffTPCPtF_K = new TProfile("F_Eff_pt_K","F_Eff_Kt_K",50,0.1,3);     
  fEffTPCPtF_K->SetXTitle("p_{t}");
  fEffTPCPtF_K->SetYTitle("TPC Findable Efficiency");

  // Efficiency as function of tan(theta) 
  fEffTPCTan = new TProfile("Eff_tan","Eff_tan",50,-2.5,2.5);         
  fEffTPCTan->SetXTitle("tan(#theta)");
  fEffTPCTan->SetYTitle("TPC Efficiency");

  fEffTPCTanMC = new TProfile("MC_Eff_tan","MC_Eff_tan",50,-2.5,2.5); 
  fEffTPCTanMC->SetXTitle("tan(#theta)");
  fEffTPCTanMC->SetYTitle("MC TPC Efficiency");

  fEffTPCTanF = new TProfile("F_Eff_tan","F_Eff_tan",50,-2.5,2.5);   
  fEffTPCPtF->SetXTitle("tan(#theta)");
  fEffTPCPtF->SetYTitle("TPC Findable Efficiency");

  // Efficiency as function of pt and tan(theta) 
  fEffTPCPtTan = new TProfile2D("Eff_pt_tan","Eff_Pt_tan",10,0.1,3,20,-2.,2.);
  fEffTPCPtTan->SetXTitle("tan(#theta)");
  fEffTPCPtTan->SetYTitle("p_{t}");

  fEffTPCPtTanMC = new TProfile2D("MC_Eff_pt_tan_MC","MC Eff Pt",10,0.1,3,20,-2.,2.);
  fEffTPCPtTanMC->SetXTitle("tan(#theta)");
  fEffTPCPtTanMC->SetYTitle("p_{t}");

  fEffTPCPtTanF = new TProfile2D("MC_Eff_pt_tan_F","MC Eff Pt",10,0.1,3,20,-2.,2.);
  fEffTPCPtTanF->SetXTitle("tan(#theta)");
  fEffTPCPtTanF->SetYTitle("p_{t}");

  char name[256];
  for(Int_t i=0; i<4; ++i)
  {
    sprintf(name, "fTPCPtDCASigmaIdeal_%d",i);
    fTPCPtDCASigmaIdeal[i] = new TH2F(name,name,50,0.1,3,100,0,100);

    sprintf(name, "fTPCPtDCASigmaFull_%d",i);
    fTPCPtDCASigmaFull[i] = new TH2F(name,name,50,0.1,3,100,0,100);

    sprintf(name, "fTPCPtDCASigmaDay0_%d",i);
    fTPCPtDCASigmaDay0[i] = new TH2F(name,name,50,0.1,3,100,0,100);

    sprintf(name, "fTPCPtDCAXY_%d",i);
    fTPCPtDCAXY[i]= new TH2F(name,name,50,0.1,3,100,0,100);
    sprintf(name, "fTPCPtDCAZ_%d",i);
    fTPCPtDCAZ[i]= new TH2F(name,name,50,0.1,3,100,0,100);

    sprintf(name, "fTPCPtDCASigmaIdealPid_%d",i);
   	fTPCPtDCASigmaIdealPid[i] = new TH3F(name,name,50,0.1,3,100,0,100,5,0,5);

    sprintf(name, "fTPCPtDCASigmaFullPid_%d",i);
	fTPCPtDCASigmaFullPid[i]= new TH3F(name,name,50,0.1,3,100,0,100,5,0,5);

    sprintf(name, "fTPCPtDCASigmaDay0Pid_%d",i);
	fTPCPtDCASigmaDay0Pid[i]= new TH3F(name,name,50,0.1,3,100,0,100,5,0,5);

    sprintf(name, "fTPCPtDCAXYPid_%d",i);
	fTPCPtDCAXYPid[i]= new TH3F(name,name,50,0.1,3,100,0,100,5,0,5);

    sprintf(name, "fTPCPtDCAZPid_%d",i);
	fTPCPtDCAZPid[i]= new TH3F(name,name,50,0.1,3,100,0,100,5,0,5);
  }
}

//_____________________________________________________________________________
void AliComparisonEff::InitCuts()
{

  if(!fCutsMC) 
    AliDebug(AliLog::kError, "ERROR: Cannot find AliMCInfoCuts object");
  if(!fCutsRC) 
    AliDebug(AliLog::kError, "ERROR: Cannot find AliRecInfoCuts object");
}

//_____________________________________________________________________________
void AliComparisonEff::Process(AliMCInfo* infoMC, AliESDRecInfo *infoRC)
{
  // Fill efficiency comparison information
  
  AliExternalTrackParam *track = 0;
  Double_t kRadius    = 3.0;      // beam pipe radius
  Double_t kMaxStep   = 5.0;      // max step
  Double_t field      = 0.4;      // mag. field 
  Double_t kMaxD      = 123456.0; // max distance

  // systematics
  const Double_t kSigma2Full_xy  = 0.25; // ExB full systematics  [cm]
  const Double_t kSigma2Full_z  =  5.0;  // [cm] 

  const Double_t kSigma2Day0_xy  = 0.02; //  ExB  [cm]
  const Double_t kSigma2Day0_z  =  0.1; //  [cm] goofie  

  //  
  Double_t	DCASigmaIdeal=0;
  Double_t  DCASigmaFull=0;
  Double_t  DCASigmaDay0=0;

  Double_t dca[2], cov[3]; // dca_xy, dca_z, sigma_xy, sigma_xy_z, sigma_z
  const Double_t* dv;

  // distance to Prim. vertex 
  dv = infoMC->GetVDist(); 

  Float_t mcpt = infoMC->GetParticle().Pt();
  Float_t tantheta = TMath::Tan(infoMC->GetParticle().Theta()-TMath::Pi()*0.5);
  //Bool_t isPrim = infoMC->GetParticle().R()<fCutsMC->GetMaxR() && TMath::Abs(infoMC->GetParticle().Vz())<fCutsMC->GetMaxVz();
  Bool_t isPrim = TMath::Sqrt(dv[0]*dv[0] + dv[1]*dv[1])<fCutsMC->GetMaxR() && TMath::Abs(dv[2])<fCutsMC->GetMaxVz();
 
  // calculate and set prim. vertex
  fVertex->SetXv( infoMC->GetParticle().Vx() - dv[0] );
  fVertex->SetYv( infoMC->GetParticle().Vy() - dv[1] );
  fVertex->SetZv( infoMC->GetParticle().Vz() - dv[2] );
  
  // Check selection cuts
  if (fCutsMC->IsPdgParticle(TMath::Abs(infoMC->GetParticle().GetPdgCode())) == kFALSE) return; 

  // transform Pdg to Pid
  Double_t pid = -1;
  if( TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetEP() ) pid = 0; 
  if( TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetMuP() ) pid = 1; 
  if( TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetPiP() ) pid = 2; 
  if( TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetKP() ) pid = 3; 
  if( TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetProt() ) pid = 4; 

  //cout << "dv[0] " << dv[0] << " dv[1] " << dv[1]  <<  " dv[2] " << dv[2] << endl; 
  //cout << "v[0] " << fVertex->GetXv()  << " v[1] " << fVertex->GetYv()  <<  " v[2] " << fVertex->GetZv()<< endl; 
  if (TMath::Abs(tantheta)<fCutsRC->GetMaxAbsTanTheta())
  {
  
    // MC pt
    fMCPt->Fill(mcpt); 


    if (infoRC->GetESDtrack()->GetTPCInnerParam()) 
    {
      if ((track = new AliExternalTrackParam(*infoRC->GetESDtrack()->GetTPCInnerParam())) != 0 )
      {
        AliTracker::PropagateTrackTo(track,kRadius,infoMC->GetMass(),kMaxStep,kTRUE);
        track->PropagateToDCA(fVertex,field,kMaxD,dca,cov);

		//
		cov[2] = track->GetCovariance()[2];

	    // Eff = infoRC->GetStatus(1)==3 && isPrim / isPrim;
        // Pt vs ( dca[0]^2/cov[0]^2 + dca[1]^2/cov[2]^2 ) 
        // Pt vs ( dca[0]^2/(cov[0]^2 + kSigma2Full_xy)  + dca[1]^2/( cov[2]^2 + kSigma2Full_z ) 
        // Pt vs ( dca[0]^2/(cov[0]^2 + kSigma2_xy)  + dca[1]^2/( cov[2]^2 + kSigma2_z ) 

         if(cov[0]>0.0 && cov[2]>0.0)
		 {
			 DCASigmaIdeal = TMath::Power(dca[0],2)/cov[0] 
									  + TMath::Power(dca[1],2)/cov[2]; 

			 DCASigmaFull = TMath::Power(dca[0],2)/(cov[0]+kSigma2Full_xy) 
									 + TMath::Power(dca[1],2)/(cov[2]+kSigma2Full_z); 

			 DCASigmaDay0 = TMath::Power(dca[0],2)/(cov[0]+kSigma2Day0_xy) 
									 + TMath::Power(dca[1],2)/(cov[2]+kSigma2Day0_z); 

              //cout << "dca[0] " << dca[0]  << " dca[1] " << dca[1]  << endl; 
              //cout << "cov[0] " << cov[0]  << " cov[2] " << cov[2]  << endl; 
              //cout << "DCASigmaIdeal " << DCASigmaIdeal  << " DCASigmaFull " << DCASigmaFull  << " DCASigmaDay0 "  <<DCASigmaDay0 <<  endl; 
            //cout << " -------------------------------------------------------- "<<  endl; 
		 }

         if(infoRC->GetStatus(1)==3) fMCRecPt->Fill(mcpt); 
         if(infoRC->GetStatus(1)==3 && isPrim) fMCRecPrimPt->Fill(mcpt); 
         if(infoRC->GetStatus(1)==3 && !isPrim) fMCRecSecPt->Fill(mcpt); 

         if(isPrim)
		 {
           fTPCPtDCASigmaIdeal[0]->Fill(mcpt,DCASigmaIdeal);
           fTPCPtDCASigmaFull[0]->Fill(mcpt,DCASigmaFull);
           fTPCPtDCASigmaDay0[0]->Fill(mcpt,DCASigmaDay0);

           fTPCPtDCAXY[0]->Fill(mcpt,dca[0]);
           fTPCPtDCAZ[0]->Fill(mcpt,dca[1]);

           fTPCPtDCASigmaIdealPid[0]->Fill(mcpt,DCASigmaIdeal,pid);
           fTPCPtDCASigmaFullPid[0]->Fill(mcpt,DCASigmaFull,pid);
           fTPCPtDCASigmaDay0Pid[0]->Fill(mcpt,DCASigmaDay0,pid);

           fTPCPtDCAXYPid[0]->Fill(mcpt,dca[0],pid);
           fTPCPtDCAZPid[0]->Fill(mcpt,dca[1],pid);
           		   
		   if(infoRC->GetStatus(1)==3)
		   {
             fTPCPtDCASigmaIdeal[1]->Fill(mcpt,DCASigmaIdeal);
             fTPCPtDCASigmaFull[1]->Fill(mcpt,DCASigmaFull);
             fTPCPtDCASigmaDay0[1]->Fill(mcpt,DCASigmaDay0);

             fTPCPtDCAXY[1]->Fill(mcpt,dca[0]);
             fTPCPtDCAZ[1]->Fill(mcpt,dca[1]);

             fTPCPtDCASigmaIdealPid[1]->Fill(mcpt,DCASigmaIdeal,pid);
             fTPCPtDCASigmaFullPid[1]->Fill(mcpt,DCASigmaFull,pid);
             fTPCPtDCASigmaDay0Pid[1]->Fill(mcpt,DCASigmaDay0,pid);

             fTPCPtDCAXYPid[1]->Fill(mcpt,dca[0],pid);
             fTPCPtDCAZPid[1]->Fill(mcpt,dca[1],pid);
           }
		 }

        // Cont = infoRC->GetStatus(1)==3 && !isPrim / infoRC->GetStatus(1)==3   
        // Pt vs ( dz[0]^2/cov[0]^2 + dz[1]^2/cov[2]^2 ) 
        // Pt vs ( dz[0]^2/(cov[0]^2 + kSigma2Full_xy)  + dz[1]^2/( cov[2]^2 + kSigma2Full_z ) 
        // Pt vs ( dz[0]^2/(cov[0]^2 + kSigma2_xy)  + dz[1]^2/( cov[2]^2 + kSigma2_z ) 

		 if(infoRC->GetStatus(1)==3) 
		 {
		   fTPCPtDCASigmaIdeal[2]->Fill(mcpt,DCASigmaIdeal);
           fTPCPtDCASigmaFull[2]->Fill(mcpt,DCASigmaFull);
           fTPCPtDCASigmaDay0[2]->Fill(mcpt,DCASigmaDay0);

           fTPCPtDCAXY[2]->Fill(mcpt,dca[0]);
           fTPCPtDCAZ[2]->Fill(mcpt,dca[1]);

           fTPCPtDCASigmaIdealPid[2]->Fill(mcpt,DCASigmaIdeal,pid);
           fTPCPtDCASigmaFullPid[2]->Fill(mcpt,DCASigmaFull,pid);
           fTPCPtDCASigmaDay0Pid[2]->Fill(mcpt,DCASigmaDay0,pid);

           fTPCPtDCAXYPid[2]->Fill(mcpt,dca[0],pid);
           fTPCPtDCAZPid[2]->Fill(mcpt,dca[1],pid);
 
		   if(isPrim==0)
		   {
             fTPCPtDCASigmaIdeal[3]->Fill(mcpt,DCASigmaIdeal);
             fTPCPtDCASigmaFull[3]->Fill(mcpt,DCASigmaFull);
             fTPCPtDCASigmaDay0[3]->Fill(mcpt,DCASigmaDay0);

             fTPCPtDCAXY[3]->Fill(mcpt,dca[0]);
             fTPCPtDCAZ[3]->Fill(mcpt,dca[1]);

             fTPCPtDCASigmaIdealPid[3]->Fill(mcpt,DCASigmaIdeal,pid);
             fTPCPtDCASigmaFullPid[3]->Fill(mcpt,DCASigmaFull,pid);
             fTPCPtDCASigmaDay0Pid[3]->Fill(mcpt,DCASigmaDay0,pid);

             fTPCPtDCAXYPid[3]->Fill(mcpt,dca[0],pid);
             fTPCPtDCAZPid[3]->Fill(mcpt,dca[1],pid);
           }
	     }
	   delete track;
       }
     } 
	 else 
	 {
       if(isPrim)
	   {
	     fTPCPtDCASigmaIdeal[0]->Fill(mcpt,0.0);
         fTPCPtDCASigmaFull[0]->Fill(mcpt,0.0);
         fTPCPtDCASigmaDay0[0]->Fill(mcpt,0.0);

         fTPCPtDCAXY[0]->Fill(mcpt,0.0);
         fTPCPtDCAZ[0]->Fill(mcpt,0.0);

         fTPCPtDCASigmaIdealPid[0]->Fill(mcpt,0.0,pid);
         fTPCPtDCASigmaFullPid[0]->Fill(mcpt,0.0,pid);
         fTPCPtDCASigmaDay0Pid[0]->Fill(mcpt,0.0,pid);

         fTPCPtDCAXYPid[0]->Fill(mcpt,0.0,pid);
         fTPCPtDCAZPid[0]->Fill(mcpt,0.0,pid);
	   }
     }
  }

  // only primary particles
  if (!isPrim) return;

  // pt
  if (TMath::Abs(tantheta)<fCutsRC->GetMaxAbsTanTheta()){

    fEffTPCPt->Fill(mcpt, infoRC->GetStatus(1)==3);
    fEffTPCPtMC->Fill(mcpt, infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits());
    if (infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()){
      fEffTPCPtF->Fill(mcpt, infoRC->GetStatus(1)==3);
    }

    // protons
    if(TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetProt()) { 
	   fEffTPCPt_P->Fill(mcpt, infoRC->GetStatus(1)==3);
       fEffTPCPtMC_P->Fill(mcpt, infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits());

       if(infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()) {
         fEffTPCPtF_P->Fill(mcpt, infoRC->GetStatus(1)==3);
       }
	}

    // pions
    if(TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetPiP()) {
	  fEffTPCPt_Pi->Fill(mcpt, infoRC->GetStatus(1)==3);
      fEffTPCPtMC_Pi->Fill(mcpt, infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits());

       if(infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()) {
         fEffTPCPtF_Pi->Fill(mcpt, infoRC->GetStatus(1)==3);
       }
	}

	// kaons
    if(TMath::Abs(infoMC->GetParticle().GetPdgCode())==fCutsMC->GetKP()) {
	  fEffTPCPt_K->Fill(mcpt, infoRC->GetStatus(1)==3);
      fEffTPCPtMC_K->Fill(mcpt, infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits());

       if(infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()) {
         fEffTPCPtF_K->Fill(mcpt, infoRC->GetStatus(1)==3);
       }
	}
  }

  // theta
  if (TMath::Abs(mcpt)>fCutsRC->GetPtMin()){
    fEffTPCTan->Fill(tantheta, infoRC->GetStatus(1)==3);
    fEffTPCTanMC->Fill(tantheta, infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits());
    if (infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()){
      fEffTPCTanF->Fill(tantheta, infoRC->GetStatus(1)==3);
    }
  }

  // pt-theta
  fEffTPCPtTan->Fill(mcpt,tantheta,infoRC->GetStatus(1)==3);
  fEffTPCPtTanMC->Fill(mcpt,tantheta,infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()); 
  if (infoMC->GetRowsWithDigits()>fCutsMC->GetMinRowsWithDigits()){
    fEffTPCPtTanF->Fill(mcpt,tantheta,infoRC->GetStatus(1)==3); 
  }
}

//_____________________________________________________________________________
void AliComparisonEff::Exec(AliMCInfo* infoMC, AliESDRecInfo *infoRC)
{
  // Process comparison information
  Process(infoMC,infoRC);
}

//_____________________________________________________________________________
Long64_t AliComparisonEff::Merge(TCollection* list) 
{
  // Merge list of objects (needed by PROOF)

  if (!list)
  return 0;

  if (list->IsEmpty())
  return 1;

  TIterator* iter = list->MakeIterator();
  TObject* obj = 0;

  // collection of generated histograms

  Int_t count=0;
  while((obj = iter->Next()) != 0) 
  {
    AliComparisonEff* entry = dynamic_cast<AliComparisonEff*>(obj);
    if (entry == 0) 
      continue; 
  
    fMCPt->Add(entry->fMCPt);
    fMCRecPt->Add(entry->fMCRecPt);
    fMCRecPrimPt->Add(entry->fMCRecPrimPt);
    fMCRecSecPt->Add(entry->fMCRecSecPt);

    fEffTPCPt->Add(entry->fEffTPCPt);
	fEffTPCPtMC->Add(entry->fEffTPCPtMC);
	fEffTPCPtF->Add(entry->fEffTPCPtF);

    fEffTPCPt_P->Add(entry->fEffTPCPt_P);
	fEffTPCPtMC_P->Add(entry->fEffTPCPtMC_P);
	fEffTPCPtF_P->Add(entry->fEffTPCPtF_P);

    fEffTPCPt_Pi->Add(entry->fEffTPCPt_Pi);
	fEffTPCPtMC_Pi->Add(entry->fEffTPCPtMC_Pi);
	fEffTPCPtF_Pi->Add(entry->fEffTPCPtF_Pi);

    fEffTPCPt_K->Add(entry->fEffTPCPt_K);
	fEffTPCPtMC_K->Add(entry->fEffTPCPtMC_K);
	fEffTPCPtF_K->Add(entry->fEffTPCPtF_K);

	fEffTPCTan->Add(entry->fEffTPCTan);
	fEffTPCTanMC->Add(entry->fEffTPCTanMC);
	fEffTPCTanF->Add(entry->fEffTPCTanF);
	  
	fEffTPCPtTan->Add(entry->fEffTPCPtTan);
	fEffTPCPtTanMC->Add(entry->fEffTPCPtTanMC);
	fEffTPCPtTanF->Add(entry->fEffTPCPtTanF);
    
    for(Int_t i=0; i<4; ++i)
    {
      fTPCPtDCASigmaIdeal[i]->Add(entry->fTPCPtDCASigmaIdeal[i]);
      fTPCPtDCASigmaFull[i]->Add(entry->fTPCPtDCASigmaFull[i]);
      fTPCPtDCASigmaDay0[i]->Add(entry->fTPCPtDCASigmaDay0[i]);

      fTPCPtDCAXY[i]->Add(entry->fTPCPtDCAXY[i]);
      fTPCPtDCAZ[i]->Add(entry->fTPCPtDCAXY[i]);

      fTPCPtDCASigmaIdealPid[i]->Add(entry->fTPCPtDCASigmaIdealPid[i]);
      fTPCPtDCASigmaFullPid[i]->Add(entry->fTPCPtDCASigmaFullPid[i]);
      fTPCPtDCASigmaDay0Pid[i]->Add(entry->fTPCPtDCASigmaDay0Pid[i]);

      fTPCPtDCAXYPid[i]->Add(entry->fTPCPtDCAXYPid[i]);
      fTPCPtDCAZPid[i]->Add(entry->fTPCPtDCAXYPid[i]);
    }

  count++;
  }

return count;
}
 
//_____________________________________________________________________________
void AliComparisonEff::Analyse() 
{
  // Analyse output histograms
  
  AliComparisonEff * comp=this;

  // calculate efficiency and contamination (4 sigma) 

  TH1 *h_sigmaidealpid[20];
  TH1 *h_sigmafullpid[20];
  TH1 *h_sigmaday0pid[20];

  //TH1 *h_sigmaday0pidclone[20];

  TH1 *h_sigmaidealpidtot[4];
  TH1 *h_sigmafullpidtot[4];
  TH1 *h_sigmaday0pidtot[4];

  //TH1 *h_sigmaday0pidtotclone[4];

  char name[256];
  char name1[256];
  Int_t idx;

  for(Int_t i=0; i<4; ++i)
  {
     //total
     comp->fTPCPtDCASigmaIdealPid[i]->GetYaxis()->SetRange(1,4);
     sprintf(name,"h_sigmaidealpidtot_%d",i);
     h_sigmaidealpidtot[i] = comp->fTPCPtDCASigmaIdealPid[i]->Project3D();
     h_sigmaidealpidtot[i]->SetName(name);

     comp->fTPCPtDCASigmaFullPid[i]->GetYaxis()->SetRange(1,4);
     sprintf(name,"h_sigmafullpidtot_%d",i);
     h_sigmafullpidtot[i] = comp->fTPCPtDCASigmaFullPid[i]->Project3D();
     h_sigmafullpidtot[i]->SetName(name);

     comp->fTPCPtDCASigmaDay0Pid[i]->GetYaxis()->SetRange(1,4);
     sprintf(name,"h_sigmaday0pidtot_%d",i);
     h_sigmaday0pidtot[i] = comp->fTPCPtDCASigmaDay0Pid[i]->Project3D();
     h_sigmaday0pidtot[i]->SetName(name);

     // pid wise
     for(Int_t j=0; j<5; ++j)
	 {
       idx = i*5 + j;

       comp->fTPCPtDCASigmaIdealPid[i]->GetYaxis()->SetRange(1,4);
       comp->fTPCPtDCASigmaIdealPid[i]->GetZaxis()->SetRange(j+1,j+1);

       sprintf(name,"h_sigmaidealpid_%d",idx);
       h_sigmaidealpid[idx] = comp->fTPCPtDCASigmaIdealPid[i]->Project3D();
       h_sigmaidealpid[idx]->SetName(name);
	   

       comp->fTPCPtDCASigmaFullPid[i]->GetYaxis()->SetRange(1,4);
       comp->fTPCPtDCASigmaFullPid[i]->GetZaxis()->SetRange(j+1,j+1);

       sprintf(name,"h_sigmafullpid_%d",idx);
       h_sigmafullpid[idx] = comp->fTPCPtDCASigmaFullPid[i]->Project3D();
       h_sigmafullpid[idx]->SetName(name);
       
       comp->fTPCPtDCASigmaDay0Pid[i]->GetYaxis()->SetRange(1,4);
       comp->fTPCPtDCASigmaDay0Pid[i]->GetZaxis()->SetRange(j+1,j+1);

       sprintf(name,"h_sigmaday0pid_%d",idx);
       h_sigmaday0pid[idx] = comp->fTPCPtDCASigmaDay0Pid[i]->Project3D();
       h_sigmaday0pid[idx]->SetName(name);

	} 
  }

  // calculate efficiency and contamination (all pids)
  h_sigmaidealpidtot[0]->Sumw2();
  h_sigmaidealpidtot[1]->Divide(h_sigmaidealpidtot[0]);
  h_sigmaidealpidtot[2]->Sumw2();
  h_sigmaidealpidtot[3]->Divide(h_sigmaidealpidtot[2]);

  h_sigmafullpidtot[0]->Sumw2();
  h_sigmafullpidtot[1]->Divide(h_sigmafullpidtot[0]);
  h_sigmafullpidtot[2]->Sumw2();
  h_sigmafullpidtot[3]->Divide(h_sigmafullpidtot[2]);

  h_sigmaday0pidtot[0]->Sumw2();
  h_sigmaday0pidtot[1]->Divide(h_sigmaday0pidtot[0]);
  h_sigmaday0pidtot[2]->Sumw2();
  h_sigmaday0pidtot[3]->Divide(h_sigmaday0pidtot[2]);

  // calculate efficiency pid wise
  for(Int_t idx = 0; idx<5; idx++)
  {
    h_sigmaidealpid[idx]->Sumw2();
    h_sigmaidealpid[idx+5]->Divide(h_sigmaidealpid[idx]);

    h_sigmafullpid[idx]->Sumw2();
    h_sigmafullpid[idx+5]->Divide(h_sigmafullpid[idx]);

    h_sigmaday0pid[idx]->Sumw2();
    h_sigmaday0pid[idx+5]->Divide(h_sigmaday0pid[idx]);
  }

  // calculate cont. pid wise
  for(Int_t idx = 0; idx<5; idx++)
  {
    h_sigmaidealpid[idx+15]->Divide(h_sigmaidealpidtot[2]);
    h_sigmafullpid[idx+15]->Divide(h_sigmafullpidtot[2]);
    h_sigmaday0pid[idx+15]->Divide(h_sigmaday0pidtot[2]);
  }

  // write results
  TFile *fp = new TFile("pictures_eff.root","recreate");
  fp->cd();

  TCanvas * c = new TCanvas("Efficiency","Track efficiency");
  c->cd();

  fMCPt->Write();
  fMCRecPt->Write();
  fMCRecPrimPt->Write();
  fMCRecSecPt->Write();

  for(int i = 0; i<4;i++)  
  {
   comp->fTPCPtDCASigmaIdealPid[i]->GetYaxis()->SetRange();
   comp->fTPCPtDCASigmaIdealPid[i]->GetZaxis()->SetRange();
   comp->fTPCPtDCASigmaFullPid[i]->GetYaxis()->SetRange();
   comp->fTPCPtDCASigmaFullPid[i]->GetZaxis()->SetRange();
   comp->fTPCPtDCASigmaDay0Pid[i]->GetYaxis()->SetRange();
   comp->fTPCPtDCASigmaDay0Pid[i]->GetZaxis()->SetRange();

    comp->fTPCPtDCASigmaIdealPid[i]->Write();
    comp->fTPCPtDCASigmaFullPid[i]->Write();
    comp->fTPCPtDCASigmaDay0Pid[i]->Write();
  }
  //
  comp->fEffTPCTanF->SetXTitle("Tan(#theta)");
  comp->fEffTPCTanF->SetYTitle("eff_{findable}");
  comp->fEffTPCTanF->Write("EffTanFindable");
  //
  comp->fEffTPCTan->SetXTitle("Tan(#theta)");
  comp->fEffTPCTan->SetYTitle("eff_{all}");
  comp->fEffTPCTan->Write("EffTanAll");

  h_sigmaidealpidtot[1]->Write("Eff_SigmaIdeal");
  h_sigmaidealpidtot[3]->Write("Cont_SigmaIdeal");

  h_sigmafullpidtot[1]->Write("Eff_SigmaFull");
  h_sigmafullpidtot[3]->Write("Cont_SigmaFull");

  h_sigmaday0pidtot[1]->Write("Eff_SigmaDay0");
  h_sigmaday0pidtot[3]->Write("Cont_SigmaDay0");

  for(Int_t idx = 0; idx<5; idx++)
  {
    sprintf(name,"Eff_SigmaIdeal_%d",idx);
    sprintf(name1,"Cont_SigmaIdeal_%d",idx);

    h_sigmaidealpid[idx+5]->Write(name);
    h_sigmaidealpid[idx+15]->Write(name1);

    sprintf(name,"Eff_SigmaFull_%d",idx);
    sprintf(name1,"Cont_SigmaFull_%d",idx);

    h_sigmafullpid[idx+5]->Write(name);
    h_sigmafullpid[idx+15]->Write(name1);

    sprintf(name,"Eff_SigmaDay0_%d",idx);
    sprintf(name1,"Cont_SigmaDay0_%d",idx);

    h_sigmaday0pid[idx+5]->Write(name);
    h_sigmaday0pid[idx+15]->Write(name1);
  }

  //
  fp->Close();
}
