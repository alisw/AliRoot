
/**************************************************************************
 *  Authors : Iouri Belikov, Antonin Maire
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

//-----------------------------------------------------------------
//                 AliAnalysisTaskGlobalQA class
// This task is for running the GlobalQA over already existing ESDs
//          Origin:  I.Belikov, Iouri.Belikov@cern.ch, June 2009
//-----------------------------------------------------------------

#include <TPDGCode.h>

#include "TChain.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"

#include "AliAnalysisManager.h"

#include "AliESDEvent.h"
#include "AliESDv0.h"
#include "AliESDInputHandler.h"

#include "AliAnalysisTaskGlobalQA.h"

//
// Run the GlobalQA analysis over already existing ESDs
// Origin: Iouri.Belikov@cern.ch
//

ClassImp(AliAnalysisTaskGlobalQA)

//________________________________________________________________________
AliAnalysisTaskGlobalQA::AliAnalysisTaskGlobalQA() : 
 AliAnalysisTaskSE("GlobalQA"), 
 fArrayQA(0)
{
  // Default Constructor

  DefineOutput(1, TObjArray::Class());
}

//________________________________________________________________________
void AliAnalysisTaskGlobalQA::UserCreateOutputObjects()
{
  // Create the histograms
  // Called once

  fArrayQA = new TObjArray(20);


  {// Cluster related QA
  const Char_t *name[]={
    "Fraction of the assigned clusters in ITS",
    "Fraction of the assigned clusters in TPC",
    "Fraction of the assigned clusters in TRD"
  };
  Add2ESDsList(new TH1F(name[0],name[0],100,0.,2.),kClr0);
  Add2ESDsList(new TH1F(name[1],name[1],100,0.,2.),kClr1);
  Add2ESDsList(new TH1F(name[2],name[2],100,0.,2.),kClr2);
  }

  {// Track related QA
  const Char_t *name[]={
    "Track azimuthal distribution (rad)",                   // kTrk0
    "Track pseudo-rapidity distribution",                   // kTrk1
    "TPC: track momentum distribution (GeV)",               // kTrk2
    "TPC-ITS matched: track momentum distribution (GeV)",   // kTrk3
    "TPC-TOF matched: track momentum distribution (GeV)",   // kTrk4
    "TPC-ITS track-matching probability",                   // kTrk5
    "TPC-TOF track-matching probability"                    // kTrk6
  };
  Add2ESDsList(new TH1F(name[0],name[0],100, 0.,TMath::TwoPi()),kTrk0);
  Add2ESDsList(new TH1F(name[1],name[1],100,-2.00,2.00),kTrk1);
  Add2ESDsList(new TH1F(name[2],name[2],50,  0.20,5.00),kTrk2);
  Add2ESDsList(new TH1F(name[3],name[3],50,  0.20,5.00),kTrk3);
  Add2ESDsList(new TH1F(name[4],name[4],50,  0.20,5.00),kTrk4);
  Add2ESDsList(new TH1F(name[5],name[5],50,  0.20,5.00),kTrk5);
  Add2ESDsList(new TH1F(name[6],name[6],50,  0.20,5.00),kTrk6);
  }

  {// V0 related QA
  const Char_t *name[]={
    "On-the-fly K0s mass (GeV)",
    "Offline K0s mass (GeV)",
    "On-the-fly Lambda0 + Lambda0Bar mass (GeV)",
    "Offline Lambda0 + Lambda0Bar mass (GeV)"
  };
  Add2ESDsList(new TH1F(name[0],name[0],50,  0.4477,0.5477),kK0on);
  Add2ESDsList(new TH1F(name[1],name[1],50,  0.4477,0.5477),kK0off);
  Add2ESDsList(new TH1F(name[2],name[2],50,  1.0657,1.1657),kL0on);
  Add2ESDsList(new TH1F(name[3],name[3],50,  1.0657,1.1657),kL0off);
  }

  {// PID related QA
  const Char_t *name[]={
    "ITS: dEdx (A.U.) for particles with momentum 0.4 - 0.5 (GeV)",
    "TPC: dEdx (A.U.) for particles with momentum 0.4 - 0.5 (GeV)",
    "TOF: tracking - measured (ps)",
    "TPC: dEdx (A.U.) vs momentum (GeV)"
  };
  Add2ESDsList(new TH1F(name[0],name[0],50,0.00,200.),kPid0);
  Add2ESDsList(new TH1F(name[1],name[1],50,0.00,100.),kPid1);
  Add2ESDsList(new TH1F(name[2],name[2],50,-3500.,3500.),kPid2);
  Add2ESDsList(new TH2F(name[3],name[3],1500,0.05,15.,700,0.,700.),kPid3);
  }

}

//________________________________________________________________________
void AliAnalysisTaskGlobalQA::UserExec(Option_t *) 
{
  // Main loop
  // Called for each event
  
  const AliESDEvent *esd=(const AliESDEvent *)InputEvent();

  if (!esd) {
    Printf("ERROR: ESD is not available");
    return;
  }

  for (Int_t iTracks = 0; iTracks < esd->GetNumberOfTracks(); iTracks++) {
      AliESDtrack* track = esd->GetTrack(iTracks);
      if (!track) {
         Printf("ERROR: Could not receive track %d", iTracks);
         continue;
      }


    // Cluster related QA
    if (track->IsOn(AliESDtrack::kITSrefit)) {
      Int_t n=track->GetITSclusters(0);
      GetESDsData(kClr0)->Fill(Float_t(n)/6.); //6 is the number of ITS layers
    }

    if (track->IsOn(AliESDtrack::kTPCrefit)) {
      Int_t n =track->GetTPCNcls();
      Int_t nf=track->GetTPCNclsF();      // number of crossed TPC pad rows
      if (nf>0) {
        Double_t val = n*1.0/nf; 
        GetESDsData(kClr1)->Fill(val); 
      }
    }

    if (track->IsOn(AliESDtrack::kTRDrefit)) {
      Int_t n=track->GetTRDclusters(0);
      GetESDsData(kClr2)->Fill(Float_t(n)/(6*24));//(6*24) is the number of TRD time bins
    }

    Double_t p=track->GetP();

    // Track related QA
    if (track->IsOn(AliESDtrack::kTPCrefit)) {
      Float_t dz[2]; 
      track->GetDZ(0.,0.,0.,esd->GetMagneticField(),dz); 
      if ((TMath::Abs(dz[0])<3.) && (TMath::Abs(dz[1])<3.)) { // beam pipe
        Double_t phi=track->Phi();
	GetESDsData(kTrk0)->Fill(phi);
	Double_t y=track->Eta();
	GetESDsData(kTrk1)->Fill(y);

        if (TMath::Abs(y)<0.9) {
	   GetESDsData(kTrk2)->Fill(p);
	   if (track->IsOn(AliESDtrack::kITSrefit)) GetESDsData(kTrk3)->Fill(p);
	  //if (track->IsOn(AliESDtrack::kTOFout)) GetESDsData(kTrk4)->Fill(p);
	   if (track->GetTOFsignal()>0) GetESDsData(kTrk4)->Fill(p);
	}
      }
    }

    // PID related QA
    if ((p>0.4) && (p<0.5)) {
      if (track->IsOn(AliESDtrack::kITSpid)) {
	Double_t dedx=track->GetITSsignal();
        GetESDsData(kPid0)->Fill(dedx);
      }
      if (track->IsOn(AliESDtrack::kTPCpid)) {
	Double_t dedx=track->GetTPCsignal();
        GetESDsData(kPid1)->Fill(dedx);
      }
    }
    if (p>1.0) {
      if (track->IsOn(AliESDtrack::kTOFpid)) {
        Double_t times[10];
        track->GetIntegratedTimes(times);
        Double_t tof=track->GetTOFsignal();
        GetESDsData(kPid2)->Fill(times[2]-tof);
      }
    }
    const AliExternalTrackParam *par=track->GetInnerParam();
    if (par) {
      Double_t pp=par->GetP();
      Double_t dedx=track->GetTPCsignal();
      TH2F *h = dynamic_cast<TH2F*>(GetESDsData(kPid3));
      h->Fill(pp,dedx);
    }
  }

  // V0 related QA
  Int_t nV0=esd->GetNumberOfV0s();
  for (Int_t i=0; i<nV0; i++) {
    Double_t mass;
    AliESDv0 v0(*esd->GetV0(i));

    v0.ChangeMassHypothesis(kK0Short);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kK0on)->Fill(mass);
    else
       GetESDsData(kK0off)->Fill(mass);

    v0.ChangeMassHypothesis(kLambda0);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kL0on)->Fill(mass);
    else
       GetESDsData(kL0off)->Fill(mass);

    v0.ChangeMassHypothesis(kLambda0Bar);
    mass=v0.GetEffMass();
    if (v0.GetOnFlyStatus())
       GetESDsData(kL0on)->Fill(mass);
    else
       GetESDsData(kL0off)->Fill(mass);
  }

  // Post output data.
  PostData(1, fArrayQA);
}      

//________________________________________________________________________
void AliAnalysisTaskGlobalQA::Terminate(Option_t *) 
{
  // Draw the results on the screen
  // Called once at the end of the query

  fArrayQA=(TObjArray*)GetOutputData(1);

  TH1 *tpc=GetESDsData(kTrk2); tpc->Sumw2();
  TH1 *its=GetESDsData(kTrk3); its->Sumw2();
  TH1 *tof=GetESDsData(kTrk4); tof->Sumw2();
  GetESDsData(kTrk5)->Divide(its,tpc,1,1.,"b");
  GetESDsData(kTrk6)->Divide(tof,tpc,1,1.,"b");

  TH1 *hTPCdEdxMIP = GetESDsData(kPid1);
  if (!hTPCdEdxMIP) {
    Printf("ERROR: hTPCdEdxMIP not available");
    return;
  }

  TH2 *hTPCdEdxVsP = dynamic_cast<TH2*>(GetESDsData(kPid3));
  if (!hTPCdEdxVsP) {
    Printf("ERROR: hTPCdEdxVsP not available");
    return;
  }

  TCanvas *c2=new TCanvas("c2","",320,32,530,590);

  TPad *p6=new TPad("p6","",0.,0.,1.,.5); p6->Draw(); p6->cd(); 
  p6->SetFillColor(42); p6->SetFrameFillColor(10);
  hTPCdEdxMIP->SetFillColor(2); hTPCdEdxMIP->SetFillStyle(3005);
  if (hTPCdEdxMIP->GetEntries()<333) 
      hTPCdEdxMIP->DrawCopy("E"); 
  else 
      hTPCdEdxMIP->Fit("gaus"); 
  c2->cd();

  TPad *p7=new TPad("p7","",0.,0.5,1.,1.); p7->Draw(); p7->cd(); p7->SetLogx();
  p7->SetFillColor(42); p7->SetFrameFillColor(10);
  hTPCdEdxVsP->DrawCopy();

}
