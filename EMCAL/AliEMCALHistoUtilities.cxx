/**************************************************************************
 * Copyright(c) 1998-2002, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id$ */

//_________________________________________________________________________
// This is a set of histogram
// utilities for the EMCAL
// to make some common
// functions easier
//
//*-- Authors: J.L. Klay (LLNL) & Aleksei Pavlinov (WSU) 

#include "AliEMCALHistoUtilities.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <TROOT.h>
#include <TPad.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TChain.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRegexp.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <Gtypes.h> // color, line style and so on

#include "AliESDCaloCluster.h"
#include "AliEMCALRecPoint.h"
#include "AliRunLoader.h"

using namespace std;

ClassImp(AliEMCALHistoUtilities)

AliEMCALHistoUtilities::AliEMCALHistoUtilities(const char *name, const char *tit) : TNamed(name,tit)
{
  // constructor
}

AliEMCALHistoUtilities::~AliEMCALHistoUtilities()
{
	//destructor
}  

TList* AliEMCALHistoUtilities::MoveHistsToList(const char* name, Bool_t putToBrowser)
{
  // Move HIST to list
  gROOT->cd();
  TIter nextHist(gDirectory->GetList());
  TList *list = new TList;
  list->SetName(name);
  TObject *objHist;
  while((objHist=nextHist())){
    if (!objHist->InheritsFrom("TH1")) continue;
    ((TH1*)objHist)->SetDirectory(0); // Remove from gROOT
    list->Add(objHist);
    gDirectory->GetList()->Remove(objHist);
  }
  if(putToBrowser) gROOT->GetListOfBrowsables()->Add((TObject*)list);
  return list;
}

void AliEMCALHistoUtilities::FillH1(TList *l, Int_t ind, Double_t x, Double_t w)
{
  //fill 1d histogram
  static TH1* hid=0;
  if(l == 0) return;
  if(ind>=0 && ind < l->GetSize()){
    hid = (TH1*)l->At(ind);
    hid->Fill(x,w);
  }
}

void AliEMCALHistoUtilities::FillH2(TList *l, Int_t ind, Double_t x, Double_t y, Double_t w)
{
  //fill 2d histogram
  static TH2* hid=0;
  if(l == 0) return;
  if(ind>=0 && ind < l->GetSize()){
    hid = (TH2*)l->At(ind);
    hid->Fill(x,y,w);
  }
}

int AliEMCALHistoUtilities::SaveListOfHists(TList *mylist,const char* name,Bool_t kSingleKey,const char* opt)
{
  //write histograms to file
  printf(" Name of out file |%s|\n", name); 
  int save = 0;
  if(mylist && mylist->GetSize() && strlen(name)){
    TString nf(name); 
    if(nf.Contains(".root") == kFALSE) nf += ".root";
    TFile file(nf.Data(),opt);
    TIter nextHist(mylist);
    TObject* objHist=0;
    int nh=0;
    if(kSingleKey) {
       file.cd();
       mylist->Write(mylist->GetName(),TObject::kSingleKey);
       mylist->ls();
       save = 1;
    } else {
      while((objHist=nextHist())) { // loop over list 
        if(objHist->InheritsFrom("TH1")) {
          TH1* hid = (TH1*)objHist;
          file.cd();
          hid->Write();
          nh++;
          printf("Save hist. %s \n",hid ->GetName());
        }
      }
      printf("%i hists. save to file -> %s\n", nh,file.GetName());
      if(nh>0) save = 1;
    }
    file.Close();
  } else {
    printf("AliEMCALHistoUtilities::SaveListOfHists : N O  S A V I N G \n");
  }
  return save;
}

void AliEMCALHistoUtilities::AddToNameAndTitle(TH1 *h, const char *name, const char *title)
{
  if(h==0) return;
  if(name  && strlen(name))  h->SetName(Form("%s%s",h->GetName(),name));
  if(title && strlen(title)) h->SetTitle(Form("%s%s",h->GetTitle(),title));
}

void AliEMCALHistoUtilities::AddToNameAndTitleToList(TList *l, const char *name, const char *title)
{
  if(l==0) return;
  if(name || title) {
    for(int i=0; i<l->GetSize(); i++) {
      TObject *o = l->At(i);
      if(o->InheritsFrom("TH1")) {
        TH1 *h = (TH1*)o;
        AddToNameAndTitle(h, name, title);
      }
    }
  }
}

void AliEMCALHistoUtilities::ResetListOfHists(TList *l)
{
  if(l==0) return;

  for(int i=0; i<l->GetSize(); i++) {
    TH1F* h = (TH1F*)l->At(i);
    h->Reset(); 
  }
}

TLatex *AliEMCALHistoUtilities::Lat(const char *text, Float_t x,Float_t y, Int_t align, Float_t tsize, short tcolor)
{ 
  double y1=y;
  TLatex *latex = new TLatex;
  latex->SetTextAlign(align);
  latex->SetTextSize(tsize);
  latex->SetTextColor(tcolor);
  latex->DrawLatex(x, y1, text);
  printf("<I> AliEMCALHistoUtilities::lat() -> %s gPad->GetLogy() %i\n", 
  text, gPad->GetLogy());
  return latex;
}

TGraph *AliEMCALHistoUtilities::DrawGraph(Int_t n, Double_t *x, Double_t *y, Int_t markerColor, 
Int_t markerStyle, const char* opt, const char* tit, const char* xTit,const char* yTit, Int_t ifun,  
const char *optFit, const char *fun)
{
  /* Drawing options 
  chopt='L' :  A simple polyline between every points is drawn
  chopt='F' :  A fill area is drawn ('CF' draw a smooth fill area)
  chopt='A' :  Axis are drawn around the graph
  chopt='C' :  A smooth Curve is drawn
  chopt='*' :  A Star is plotted at each point
  chopt='P' :  Idem with the current marker
  chopt='B' :  A Bar chart is drawn at each point
  chopt='1' :  ylow=rwymin
  chopt='X+' : The X-axis is drawn on the top side of the plot.
  chopt='Y+' : The Y-axis is drawn on the right side of the plot.

    Fitting options
   The list of fit options is given in parameter option.
      option = "W"  Set all errors to 1
             = "U" Use a User specified fitting algorithm (via SetFCN)
             = "Q" Quiet mode (minimum printing)
             = "V" Verbose mode (default is between Q and V)
             = "B" Use this option when you want to fix one or more parameters
                   and the fitting function is like "gaus","expo","poln","landau".
             = "R" Use the Range specified in the function range
             = "N" Do not store the graphics function, do not draw
             = "0" Do not plot the result of the fit. By default the fitted function
                   is drawn unless the option"N" above is specified.
             = "+" Add this new fitted function to the list of fitted functions
                   (by default, any previous function is deleted)
             = "C" In case of linear fitting, not calculate the chisquare
                    (saves time)
             = "F" If fitting a polN, switch to minuit fitter
             = "ROB" In case of linear fitting, compute the LTS regression
                     coefficients (robust(resistant) regression), using
                     the default fraction of good points
               "ROB=0.x" - compute the LTS regression coefficients, using
                           0.x as a fraction of good points

  */
  printf("AliEMCALHistoUtilities::drawGraph started \n");
  printf("Drawing opt |%s| : Fitting opt |%s|\n", opt, optFit);

    TGraph *gr = new TGraph(n, x, y);
    gr->SetMarkerColor(markerColor);
    gr->SetLineColor(markerColor);
    gr->SetMarkerStyle(markerStyle);
    gr->SetTitle(tit);
    gr->Draw(opt);

    TString ctmp(opt);
    if(ctmp.Contains("A")) {
       gr->GetHistogram()->SetXTitle(xTit);
       gr->GetHistogram()->SetYTitle(yTit);
    }
    ctmp = optFit; 
    if(ifun>=0) {
      TString sf("pol"); sf += ifun;
      gr->Fit(sf.Data(),optFit);
      printf("\n ** Fit by Polynomial of degree %i : %s **\n", ifun, sf.Data());
    } else if(!ctmp.Contains("no",TString::kIgnoreCase)){
      printf("\n ** ifun %i : %s **\n", ifun, fun);
      gr->Fit(fun, optFit);
    }

    return gr;
}

TGraphErrors *AliEMCALHistoUtilities::DrawGraphErrors(const Int_t n,Double_t *x,Double_t *y,Double_t *ex, 
Double_t *ey, Int_t markerColor,  Int_t markerStyle, const char* opt, const char* tit, 
const char* xTit,char* yTit, Int_t ifun, const char *optFit, const char *fun)
{
  printf("AliEMCALHistoUtilities::drawGraphErrors started \n");
  printf("Drawing opt |%s| : ifun %i: Fitting opt |%s|, fun |%s|\n", 
	 opt, ifun, optFit, fun);

  TGraphErrors *gr = new TGraphErrors(n, x,y,ex,ey);
  gr->SetMarkerColor(markerColor);
  gr->SetLineColor(markerColor);
  gr->SetMarkerStyle(markerStyle);
  if(tit&&strlen(tit)>0) gr->SetTitle(tit);

  TString ctmp(opt);
  if(ctmp.Contains("A")) {
     gr->GetHistogram()->SetXTitle(xTit);
     gr->GetHistogram()->SetYTitle(yTit);
  }
  if(ifun>0) {
    if(ifun != 999) {
      TString sf("pol"); sf += ifun;
      gr->Fit(sf.Data(),optFit);
      printf("\n ** Fit by Polynomial of degree %i : %s **\n", ifun, sf.Data());
    } else {
      gr->Fit(fun, optFit);
      printf("\n ** Fit by %s **\n", fun);
    }
  } else {
    if(strlen(optFit)) {
      printf("\n ** ifun %i : %s **\n", ifun, fun);
      gr->Fit(fun, optFit);
    }
  }

  gr->Draw(opt);

  return gr;
}

TF1* AliEMCALHistoUtilities::GetResolutionFunction(const char *opt, TString &latexName)
{
  TString OPT(opt);
  OPT.ToUpper();
  TF1 *fres=0;
  if      (OPT.Contains("FRES1")) {
    fres = new TF1("fres","[0]+[1]/sqrt(x)", 0.0, 101.);
    latexName = "#frac{#sigma_{E}}{E} = A+#frac{B}{#sqrt{E}}";
  } else if(OPT.Contains("FRES2")) { 
    fres = new TF1("fres","sqrt([0]*[0]+[1]*[1]/x)", 0.0, 101.);
    latexName = "#sqrt{A^{2}+#frac{B^{2}}{E}}";
  }
  if(fres) {
    fres->SetParName(0,"A");
    fres->SetParName(1,"B");

    fres->SetParameter(0, 2.0);
    fres->SetParameter(1, 6.6);
    fres->SetLineWidth(2);
    fres->SetLineColor(kRed);
  }
  return fres;
}

void AliEMCALHistoUtilities::InitChain(TChain *chain, const char* nameListOfFiles, Int_t nFileMax)
{
  // Read name of files from text file with nameListOfFiles and added to the chain
  if(chain==0 || nameListOfFiles==0) return;
 
  ifstream in;
  in.open(nameListOfFiles);
  if (!in) {
    cout << "Input file "<<nameListOfFiles<<" cannot be opened.\n";
    return;
  }

  Int_t nfiles = 0; // number of files in chain
  char nf[200];     // name of current file
  while (!in.getline(nf,200).eof()) {
    if(!in.good()) break;
    chain->Add(nf);
    nfiles++;
    cout<<nfiles<<" "<<nf<<endl;
    if(nFileMax && nfiles>=nFileMax) break;
  }
  cout << " \n ********** <I> Accepted file "<< nfiles << "********* \n"<<endl;
  //  chainEsd->Print();
  //  chain->Lookup();
}

AliRunLoader* AliEMCALHistoUtilities::InitKinematics(const Int_t nev, const char* galiceName)
{
  static AliRunLoader *RL = 0;
  if(RL == 0 || nev%1000==0) {
    if(RL)  {
      RL->UnloadgAlice();
      delete RL;
    }
    RL = AliRunLoader::Open(galiceName,AliConfig::GetDefaultEventFolderName(),"read");
    RL->LoadgAlice(); // obligatory
  }
  if(RL) {
    RL->GetEvent(nev%1000);
    RL->LoadKinematics();
    /* Get what you need after that
      RL->LoadHits();
      AliStack *stack=RL->Stack();
      AliESDCaloCluster * clus = esd->GetCaloCluster(n);
      Int_t label = clus->GetLabel(); // what is this 
      TParticle *primary=stack->Particle(label); 
    */
  }
  return RL;
}

Double_t AliEMCALHistoUtilities::GetMomentum(const char* nameListOfFiles)
{
  // Get momentum value from string  - like /....100GEV/.. 
  Double_t p=-1; // negative if undefined 
  TString st(nameListOfFiles);
  if(st.Length()>=5) {
    st.ToUpper();
    Ssiz_t ind1 = st.Index("GEV"), ind2=0;
    if(ind1>0) {
      for(Int_t i=2; i<=10; i++) {
        ind2 = st.Index("/",ind1-i);
        if(ind2>0 && ind2<ind1) break;
      }
      TString mom  = st(ind2+1, ind1-ind2-1);
      if(mom.IsFloat()) p = mom.Atof();
      printf(" dir |%s| : mom |%s| : p %f : ind2,1 %i->%i\n", st.Data(), mom.Data(), p, ind2, ind1);
    }
  }
  return p;
}

int AliEMCALHistoUtilities::ParseString(const TString &topt, TObjArray &Opt)
{ 
  // Moved from AliEMCALGeometry
  // Feb 06, 2006
  Ssiz_t begin, index, end, end2;
  begin = index = end = end2 = 0;
  TRegexp separator("[^ ;,\\t\\s/]+");
  while ( (begin < topt.Length()) && (index != kNPOS) ) {
    // loop over given options
    index = topt.Index(separator,&end,begin);
    if (index >= 0 && end >= 1) {
      TString substring(topt(index,end));
      Opt.Add(new TObjString(substring.Data()));
    }
    begin += end+1;
  }
  return Opt.GetEntries();
}

// Analysis utilites
Bool_t AliEMCALHistoUtilities::GetLorentzVectorFromESDCluster(TLorentzVector &v, const AliESDCaloCluster* cl)
{
  // May 8, 2007
  static Double_t e=0.0;
  static Float_t pos[3];
  static TVector3 gpos;
  if(cl==0) return kFALSE;
  
  e = Double_t(cl->E());
  if(e<=0.0) {
    printf(" negative cluster energy : %f \n", e);
    return kFALSE;
  }
  cl->GetPosition(pos);
  gpos.SetXYZ(Double_t(pos[0]), Double_t(pos[1]), Double_t(pos[2]));
  gpos.SetMag(e);
  v.SetVectM(gpos, 0.0);

  return kTRUE;
}

Bool_t AliEMCALHistoUtilities::GetLorentzVectorFromRecPoint(TLorentzVector &v, const AliEMCALRecPoint  *rp)
{
  // Jun 20, 2007
  static Double_t e=0.0;
  static TVector3 gpos;
  if(rp==0) return kFALSE;
  
  e = Double_t(rp->GetPointEnergy());
  if(e<=0.0) {
    printf(" negative rec.point energy : %f \n", e);
    return kFALSE;
  }
  rp->GetGlobalPosition(gpos);
  gpos.SetMag(e);
  v.SetVectM(gpos, 0.0);

  return kTRUE;
}
//
//// Drawing:
//
void AliEMCALHistoUtilities::DrawHist(TH1* hid,int lineWidth,int lineColor,const char* opt, int lineStyle)
{
  TString OPT;
  OPT = opt;
  if(!hid) return;
  printf(" Hist. %s : option |%s| \n", hid->GetName(), opt);
  if(hid && hid->GetEntries()>=1.) {
    if(lineWidth) hid->SetLineWidth(lineWidth);
    if(lineColor) hid->SetLineColor(lineColor);
    if(lineStyle) hid->SetLineStyle(lineStyle);
    if(OPT.Contains("stat",TString::kIgnoreCase)) hid->SetStats(kTRUE);
    hid->Draw(opt);
  } else {
    if   (strcmp(opt,"empty")==0) hid->Draw();
    else printf(" has fewer entries %i or hid is zero\n", (int) hid->GetEntries());
  }
}

//
//// Fitting:
//
TF1* AliEMCALHistoUtilities::Gausi(const char *addName,double xmi,double xma,double N,double mean,double sig,double width)
{ // Fit by gaus where first parameter is the number of events under ga
  TString name("gi");
  name += addName;
  TF1 *F = new TF1(name.Data(), Gi, xmi, xma, 4); 
  F->SetParNames("INTEGRAL","MEAN","SIGMA","WIDTH");

  F->SetParameter(0,N);
  F->SetParameter(1,mean);
  F->SetParameter(2,sig);

  F->FixParameter(3,width); // width of histogramm bin
  return F;
}

TF1* AliEMCALHistoUtilities::Gausi(const char *addName, double xmi, double xma, TH1 *h) 
{
  // You can change parameters after that if you don't like this assumption
  if(h) return Gausi(addName, xmi, xma, h->Integral(), h->GetMean(),h->GetRMS(), h->GetBinWidth(1));
  else  return 0; 
}

TF1* AliEMCALHistoUtilities::GausiPol2(const char *addName,double xmi,double xma, TF1 *g, TF1* bg)
{ // Fit by gaus where first parameter is the number of events under ga
  TString name("giPol2");
  name += addName;
  TF1 *F = new TF1(name.Data(), GiPol2, xmi, xma, 7); 
  F->SetParNames("INTEGRAL","MEAN","SIGMA","WIDTH","a0","a1","a2");

  if(g) {
    for(int i=0; i<4; i++) F->SetParameter(i, g->GetParameter(i));
    F->FixParameter(3,g->GetParameter(3));
  }

  if(bg) {
    for(int i=4; i<7; i++) F->SetParameter(i, bg->GetParameter(i+4));
  }
  F->SetLineColor(kRed);
  return F;
}

Double_t AliEMCALHistoUtilities::Gi(Double_t *x, Double_t *par)
{ 
  // First parameter is integral (number events under gaus)
  // Forth parameter (par[3]) is width of histogram bin 
  // gaus(0) is a substitute for [0]*exp(-0.5*((x-[1])/[2])**2)

  static Double_t C = TMath::Sqrt(2.*TMath::Pi()), y=0.0, f=0.0; // sqrt(2.*pi)

  y  = (x[0]-par[1])/par[2];
  f  = par[0]*par[3]/(par[2]*C) * TMath::Exp(-0.5*y*y);

  return f;
}

Double_t AliEMCALHistoUtilities::GiPol2(Double_t *x, Double_t *par)
{ 
  // First parameter is integral (number events under gaus)
  // Forth parameter (par[3]) is width of histogram bin 
  // gaus(0) is a substitute for [0]*exp(-0.5*((x-[1])/[2])**2)
  // + pol2 -> 7 parameters
  static Double_t C = TMath::Sqrt(2.*TMath::Pi()), y=0.0, f=0.0; // sqrt(2.*pi)

  y  = (x[0]-par[1])/par[2];
  f  = par[0]*par[3]/(par[2]*C) * TMath::Exp(-0.5*y*y);

  f += par[4] + par[5]*x[0] + par[6]*x[0]*x[0];

  return f;
}

// Calibration stuff
Double_t AliEMCALHistoUtilities::GetCorrectionCoefficientForGamma_1(const Double_t eRec)
{
  // Correction to rec.energy - Jul 15, 2007
  // E(gamma) = corr * E(eRec);
  /* corr = p0*(eRec-7.5)+p1*(eRec-7.5)*(eRec-7.5)+p2; 0.0<eRec<10.0
   1  p0           6.07157e-05   1.15179e-04  -0.00000e+00   1.20997e-03
   2  p1           1.50019e-04   3.13566e-05  -0.00000e+00   1.22531e-02
   3  p2           9.99019e-01   4.08251e-04  -0.00000e+00   1.40606e-03

     corr = p3 + p4*eRec + p5*eRec*eRec
   1  p3           9.97135e-01   5.31970e-04   1.37962e-09   1.68120e-08
   2  p4           3.15740e-04   2.53371e-05   1.11475e-11   1.74192e-04
   3  p5          -1.35383e-06   2.19495e-07  -5.82864e-13   4.52277e-02
  */
  static Double_t p0=6.07157e-05, p1=1.50019e-04, p2=9.99019e-01;
  static Double_t p3=9.97135e-01, p4=3.15740e-04, p5=-1.35383e-06;
  static Double_t corr=1.0;
  if(eRec>=0.0 && eRec <=10.0) {
    corr = p0*(eRec-7.5) + p1*(eRec-7.5)*(eRec-7.5) + p2;
  } else if(eRec>10.0 && eRec <=105.0) {
    corr = p3 + p4*eRec + p5*eRec*eRec;
  }
  //printf(" eRec %f | corr %f \n", eRec, corr);
  return corr;
}

Double_t AliEMCALHistoUtilities::GetCorrectedEnergyForGamma_1(const Double_t eRec)
{
  return GetCorrectionCoefficientForGamma_1(eRec) * eRec;
}
