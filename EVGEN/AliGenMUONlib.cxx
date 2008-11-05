/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

// Library class for particle pt and y distributions used for 
// muon spectrometer simulations.
// To be used with AliGenParam.
// The following particle typed can be simulated:
// pi, K, phi, omega, eta, J/Psi, Upsilon, charm and beauty mesons. 
//
// andreas.morsch@cern.ch
//

#include "TMath.h"
#include "TRandom.h"

#include "AliGenMUONlib.h"

ClassImp(AliGenMUONlib)
//
//  Pions
Double_t AliGenMUONlib::PtPion(Double_t *px, Double_t* /*dummy*/)
{
//
//     PT-PARAMETERIZATION CDF, PRL 61(88) 1819
//     POWER LAW FOR PT > 500 MEV
//     MT SCALING BELOW (T=160 MEV)
//
  const Double_t kp0 = 1.3;
  const Double_t kxn = 8.28;
  const Double_t kxlim=0.5;
  const Double_t kt=0.160;
  const Double_t kxmpi=0.139;
  const Double_t kb=1.;
  Double_t y, y1, xmpi2, ynorm, a;
  Double_t x=*px;
  //
  y1=TMath::Power(kp0/(kp0+kxlim),kxn);
  xmpi2=kxmpi*kxmpi;
  ynorm=kb*(TMath::Exp(-sqrt(kxlim*kxlim+xmpi2)/kt));
  a=ynorm/y1;
  if (x > kxlim)
    y=a*TMath::Power(kp0/(kp0+x),kxn);
  else
    y=kb*TMath::Exp(-sqrt(x*x+xmpi2)/kt);
  return y*x;
}
//
// y-distribution
//
Double_t AliGenMUONlib::YPion( Double_t *py, Double_t */*dummy*/)
{
// Pion y
  Double_t y=TMath::Abs(*py);
/*
  const Double_t ka    = 7000.;
  const Double_t kdy   = 4.;
  Double_t ex = y*y/(2*kdy*kdy);
  return ka*TMath::Exp(-ex);
*/
  return 1.16526e+04+y*-3.79886e+03+y*y*4.31130e+02;
  
}
//                 particle composition
//
Int_t AliGenMUONlib::IpPion(TRandom *ran)
{
// Pion composition 
    if (ran->Rndm() < 0.5) {
	return  211;
    } else {
	return -211;
    }
}

//____________________________________________________________
//
// Mt-scaling

Double_t AliGenMUONlib::PtScal(Double_t pt, Int_t np)
{
  //    SCALING EN MASSE PAR RAPPORT A PTPI
  //    MASS PI,K,ETA,RHO,OMEGA,ETA',PHI
  const Double_t khm[10] = {.13957,.493,.5488,.769,.7826,.958,1.02,0,0,0};
  //     VALUE MESON/PI AT 5 GEV
  const Double_t kfmax[10]={1.,0.3,0.55,1.0,1.0,1.0,1.0,0,0,0};
  np--;
  Double_t f5=TMath::Power(((sqrt(100.018215)+2.)/(sqrt(100.+khm[np]*khm[np])+2.0)),12.3);
  Double_t fmax2=f5/kfmax[np];
  // PIONS
  Double_t ptpion=100.*PtPion(&pt, (Double_t*) 0);
  Double_t fmtscal=TMath::Power(((sqrt(pt*pt+0.018215)+2.)/
				 (sqrt(pt*pt+khm[np]*khm[np])+2.0)),12.3)/ fmax2;
  return fmtscal*ptpion;
}
//
// kaon
//
//                pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtKaon( Double_t *px, Double_t */*dummy*/)
{
// Kaon pT
  return PtScal(*px,2);
}

// y-distribution
//____________________________________________________________
Double_t AliGenMUONlib::YKaon( Double_t *py, Double_t */*dummy*/)
{
// Kaon y
  Double_t y=TMath::Abs(*py);
/*
  const Double_t ka    = 1000.;
  const Double_t kdy   = 4.;
  //
  Double_t ex = y*y/(2*kdy*kdy);
  return ka*TMath::Exp(-ex);
*/

  return 1.16526e+04+y*-3.79886e+03+y*y*4.31130e+02;
}

//                 particle composition
//
Int_t AliGenMUONlib::IpKaon(TRandom *ran)
{
// Kaon composition
    if (ran->Rndm() < 0.5) {
	return  321;
    } else {
	return -321;
    }
}

//                    J/Psi 
//
//
//                pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtJpsi( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
  const Double_t kpt0 = 4.;
  const Double_t kxn  = 3.6;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiCDFscaled( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
//
// PbPb 5.5 TeV
// scaled from CDF data at 2 TeV
// see S.Grigoryan, PWG3 Meeting, 27th Oct 2008

  const Double_t kpt0 = 5.100;
  const Double_t kxn  = 4.102;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiCDFscaledold( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
//
// PbPb 5.5 TeV
// scaled from CDF "old" data at 2 TeV

  const Double_t kpt0 = 4.703;
  const Double_t kxn  = 3.826;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiCDFscaledPP( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
//
// pp 14 TeV
// scaled from CDF data at 2 TeV

  const Double_t kpt0 = 5.630;
  const Double_t kxn  = 4.071;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiCDFscaledPPold( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
//
// pp 14 TeV
// scaled from CDF "old" data at 2 TeV

  const Double_t kpt0 = 5.355;
  const Double_t kxn  = 3.821;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiCDFscaledPP10( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT
//
// pp 10 TeV
// scaled from CDF data at 2 TeV

  const Double_t kpt0 = 5.334;
  const Double_t kxn  = 4.071;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtJpsiFlat( Double_t */*px*/, Double_t */*dummy*/ )
{
  return 1.;
}

Double_t AliGenMUONlib::PtJpsiPbPb( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT spectrum
//
// R. Vogt 2002
// PbPb 5.5 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Float_t x = px[0];
    Float_t c[8] = {
	-2.13098e+00, 9.46552e+00, -5.06799e+00, 1.27260e+00, 
	-1.83806e-01, 1.55853e-02, -7.23241e-04, 1.42105e-05
    };
    
    Double_t y;
    if (x < 10.) {
	Int_t j;
	y = c[j = 7];
	while (j > 0) y  = y * x +c[--j];
	y = x * TMath::Exp(y);
    } else {
	y = 0.;
    }
    return y;
}

Double_t AliGenMUONlib::PtJpsiBPbPb( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT spectrum
// B -> J/Psi X
    Double_t x0 =   4.0384;
    Double_t  n =   3.0288;
    
    Double_t x = px[0];
    Double_t y = x / TMath::Power((1. + (x/x0)*(x/x0)), n);
    
    return y;
}


Double_t AliGenMUONlib::PtJpsiPP( Double_t *px, Double_t */*dummy*/)
{
// J/Psi pT spectrum
//
// R. Vogt 2002
// pp 14 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Float_t x = px[0];
    Float_t c[4] = {8.47471e+00, -1.93567e+00, 1.50271e-01, -5.51212e-03};
 
    Double_t y;
    if (x < 10.) {
	Int_t j;
	y = c[j = 3];
	while (j > 0) y  = y * x +c[--j];
	y = x * TMath::Exp(y);
    } else {
	y = 0.;
    }
    return y;
}

//
//               y-distribution
//____________________________________________________________
Double_t AliGenMUONlib::YJpsi(Double_t *py, Double_t */*dummy*/)
{
// J/psi y
  const Double_t ky0 = 4.;
  const Double_t kb=1.;
  Double_t yj;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yj=kb;
  else
    yj=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yj;
}

Double_t AliGenMUONlib::YJpsiFlat( Double_t */*py*/, Double_t */*dummy*/ )
{
  return 1.;
}


Double_t AliGenMUONlib::YJpsiPbPb( Double_t *px, Double_t */*dummy*/)
{

//
// J/Psi y
//
//
// R. Vogt 2002
// PbPb 5.5 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Double_t c[5] = {-6.03425e+02, 4.98257e+02, -1.38794e+02, 1.62209e+01, -6.85955e-01};
    Double_t x = TMath::Abs(px[0]);
    Double_t y;
    
    if (x < 4.) {
	y = 31.754;
    } else if (x < 6) {
	Int_t j;
	y = c[j = 4];
	while (j > 0) y  = y * x + c[--j];
    } else {
	y =0.;
    }
    
    return y;
}

Double_t AliGenMUONlib::YJpsiCDFscaled( Double_t *px, Double_t* dummy)
{
    // J/Psi y 
    return AliGenMUONlib::YJpsiPbPb(px, dummy);
}

Double_t AliGenMUONlib::YJpsiCDFscaledold( Double_t *px, Double_t* dummy)
{
    // J/Psi y 
    return AliGenMUONlib::YJpsiPbPb(px, dummy);
}

Double_t AliGenMUONlib::YJpsiCDFscaledPP( Double_t *px, Double_t* dummy)
{
    // J/Psi y 
    return AliGenMUONlib::YJpsiPP(px, dummy);
}

Double_t AliGenMUONlib::YJpsiCDFscaledPPold( Double_t *px, Double_t* dummy)
{
    // J/Psi y 
    return AliGenMUONlib::YJpsiPP(px, dummy);
}

Double_t AliGenMUONlib::YJpsiCDFscaledPP10( Double_t *px, Double_t */*dummy*/)
{

//
// J/Psi y
//
// pp 10 TeV
// scaled from YJpsiPP(14 TeV) using 10 TeV / 14 TeV ratio of y-spectra in LO pQCD. 
// see S.Grigoryan, PWG3 Meeting, 27th Oct 2008
//

    Double_t c[5] = {2.46681e+01, 8.91486e+01, -3.21227e+01, 3.63075e+00, -1.32047e-01};

    Double_t x = TMath::Abs(px[0]);
    Double_t y;

    if (x < 3.2) {
        y = 98.523 - 1.3664 * x * x;
    } else if (x < 7.5) {
        Int_t j;
        y = c[j = 4];
        while (j > 0) y  = y * x + c[--j];
    } else {
        y =0.;
    }

    if(y<0) y=0;

    return y;
}

Double_t AliGenMUONlib::YJpsiPP( Double_t *px, Double_t */*dummy*/)
{

//
// J/Psi y
//
//
// R. Vogt 2002
// pp 14  TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//

    Double_t c[5] = {1.38532e+00, 1.00596e+02, -3.46378e+01, 3.94172e+00, -1.48319e-01};
    Double_t x = TMath::Abs(px[0]);
    Double_t y;
    
    if (x < 2.5) {
	y = 96.455 - 0.8483 * x * x;
    } else if (x < 7.9) {
	Int_t j;
	y = c[j = 4];
	while (j > 0) y  = y * x + c[--j];
    } else {
	y =0.;
    }
    
    return y;
}

Double_t AliGenMUONlib::YJpsiBPbPb( Double_t *px, Double_t */*dummy*/)
{

//
// J/Psi from B->J/Psi X
//
//
    

    Double_t c[7] = {7.37025e-02, 0., -2.94487e-03, 0., 6.07953e-06, 0., 5.39219e-07};
    
    Double_t x = TMath::Abs(px[0]);
    Double_t y;
    
    if (x > 6.) {
	y = 0.;
    } else {
	Int_t j;
	y = c[j = 6];
	while (j > 0) y  = y * x + c[--j];
    } 
    
    return y;
}



//                 particle composition
//
Int_t AliGenMUONlib::IpJpsi(TRandom *)
{
// J/Psi composition
    return 443;
}
Int_t AliGenMUONlib::IpPsiP(TRandom *)
{
// Psi prime composition
    return 100443;
}
Int_t AliGenMUONlib::IpJpsiFamily(TRandom *)
{
// J/Psi composition
  Int_t ip;
  Float_t r = gRandom->Rndm();
  if (r < 0.98) {
    ip = 443;
  } else {
    ip = 100443;
  }
  return ip;
}



//                      Upsilon
//
//
//                  pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtUpsilon( Double_t *px, Double_t */*dummy*/ )
{
// Upsilon pT
  const Double_t kpt0 = 5.3;
  const Double_t kxn  = 2.5;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtUpsilonCDFscaled( Double_t *px, Double_t */*dummy*/ )
{
// Upsilon pT
  const Double_t kpt0 = 7.753;
  const Double_t kxn  = 3.042;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtUpsilonCDFscaledPP( Double_t *px, Double_t */*dummy*/ )
{
// Upsilon pT
//
// pp 14 TeV
//
// scaled from CDF data at 2 TeV

  const Double_t kpt0 = 8.610;
  const Double_t kxn  = 3.051;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtUpsilonFlat( Double_t */*px*/, Double_t */*dummy*/ )
{
  return 1.;
}

Double_t AliGenMUONlib::PtUpsilonPbPb( Double_t *px, Double_t */*dummy*/)
{

//
// Upsilon pT
//
//
// R. Vogt 2002
// PbPb 5.5 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Float_t x = px[0];
    Double_t c[8] = {
	-1.03488e+01, 1.28065e+01, -6.60500e+00, 1.66140e+00,       
	-2.34293e-01, 1.86925e-02, -7.80708e-04, 1.30610e-05
    };
    Double_t y;
    if (x < 10.) {
	Int_t j;
	y = c[j = 7];
	while (j > 0) y  = y * x +c[--j];
	y = x * TMath::Exp(y);
    } else {
	y = 0.;
    }
    return y;
}

Double_t AliGenMUONlib::PtUpsilonPP( Double_t *px, Double_t */*dummy*/)
{

//
// Upsilon pT
//
//
// R. Vogt 2002
// pp 14 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Float_t x = px[0];
    Double_t c[8] = {-7.93955e+00, 1.06306e+01, -5.21392e+00, 1.19703e+00,   
		     -1.45718e-01, 8.95151e-03, -2.04806e-04, -1.13053e-06};
    
    Double_t y;
    if (x < 10.) {
	Int_t j;
	y = c[j = 7];
	while (j > 0) y  = y * x +c[--j];
	y = x * TMath::Exp(y);
    } else {
	y = 0.;
    }
    return y;
}

//
//                    y-distribution
//
//____________________________________________________________
Double_t AliGenMUONlib::YUpsilon(Double_t *py, Double_t */*dummy*/)
{
// Upsilon y
  const Double_t ky0 = 3.;
  const Double_t kb=1.;
  Double_t yu;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yu=kb;
  else
    yu=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yu;
}


Double_t AliGenMUONlib::YUpsilonPbPb( Double_t *px, Double_t */*dummy*/)
{

//
// Upsilon y
//
//
// R. Vogt 2002
// PbPb 5.5 TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//

    Double_t c[7] = {3.40036e-01, -3.98882e-07, -4.48398e-03, 8.46411e-08, -6.10854e-04,
		     -2.99753e-09, 1.28895e-05};
        
    Double_t x = px[0];
    if (TMath::Abs(x) > 5.55) return 0.;
    Int_t j;
    Double_t y = c[j = 6];
    while (j > 0) y  = y * x +c[--j];
    return y;
}

Double_t AliGenMUONlib::YUpsilonCDFscaled( Double_t *px, Double_t *dummy)
{
    // Upsilon y
    return AliGenMUONlib::YUpsilonPbPb(px, dummy);
    
}

Double_t AliGenMUONlib::YUpsilonCDFscaledPP( Double_t *px, Double_t *dummy)
{
    // Upsilon y
    return AliGenMUONlib::YUpsilonPP(px, dummy);
    
}

Double_t AliGenMUONlib::YUpsilonFlat( Double_t */*px*/, Double_t */*dummy*/)
{
    // Upsilon y
    return 1.;
    
}

Double_t AliGenMUONlib::YUpsilonPP( Double_t *px, Double_t */*dummy*/)
{

//
// Upsilon y
//
//
// R. Vogt 2002
// p p  14. TeV
// MRST HO
// mc = 1.4 GeV, pt-kick 1 GeV
//
    Double_t c[7] = {8.91936e-01, -6.46645e-07, -1.52774e-02, 4.28677e-08, -7.01517e-04, 
		     -6.20539e-10, 1.29943e-05};
                
    Double_t x = px[0];
    if (TMath::Abs(x) > 6.2) return 0.;
    Int_t j;
    Double_t y = c[j = 6];
    while (j > 0) y  = y * x +c[--j];
    return y;
}

//                 particle composition
//
Int_t AliGenMUONlib::IpUpsilon(TRandom *)
{
// y composition
    return 553;
}
Int_t AliGenMUONlib::IpUpsilonP(TRandom *)
{
// y composition
    return 100553;
}
Int_t AliGenMUONlib::IpUpsilonPP(TRandom *)
{
// y composition
    return 200553;
}
Int_t AliGenMUONlib::IpUpsilonFamily(TRandom *)
{
// y composition
  Int_t ip;
  Float_t r = gRandom->Rndm();
  
  if (r < 0.712) {
    ip = 553;
  } else if (r < 0.896) {
    ip = 100553;
  } else {
    ip = 200553;
  }
  return ip;
}


//
//                        Phi
//
//
//    pt-distribution (by scaling of pion distribution)
//____________________________________________________________
Double_t AliGenMUONlib::PtPhi( Double_t *px, Double_t */*dummy*/)
{
// Phi pT
  return PtScal(*px,7);
}
//    y-distribution
Double_t AliGenMUONlib::YPhi( Double_t *px, Double_t */*dummy*/)
{
// Phi y
    Double_t *dum=0;
    return YJpsi(px,dum);
}
//                 particle composition
//
Int_t AliGenMUONlib::IpPhi(TRandom *)
{
// Phi composition
    return 333;
}

//
//                        omega
//
//
//    pt-distribution (by scaling of pion distribution)
//____________________________________________________________
Double_t AliGenMUONlib::PtOmega( Double_t *px, Double_t */*dummy*/)
{
// Omega pT
  return PtScal(*px,5);
}
//    y-distribution
Double_t AliGenMUONlib::YOmega( Double_t *px, Double_t */*dummy*/)
{
// Omega y
    Double_t *dum=0;
    return YJpsi(px,dum);
}
//                 particle composition
//
Int_t AliGenMUONlib::IpOmega(TRandom *)
{
// Omega composition
    return 223;
}


//
//                        Eta
//
//
//    pt-distribution (by scaling of pion distribution)
//____________________________________________________________
Double_t AliGenMUONlib::PtEta( Double_t *px, Double_t */*dummy*/)
{
// Eta pT
  return PtScal(*px,3);
}
//    y-distribution
Double_t AliGenMUONlib::YEta( Double_t *px, Double_t */*dummy*/)
{
// Eta y
    Double_t *dum=0;
    return YJpsi(px,dum);
}
//                 particle composition
//
Int_t AliGenMUONlib::IpEta(TRandom *)
{
// Eta composition
    return 221;
}

//
//                        Charm
//
//
//                    pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtCharm( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
  const Double_t kpt0 = 2.25;
  const Double_t kxn  = 3.17;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtCharmCentral( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
  const Double_t kpt0 = 2.12;
  const Double_t kxn  = 2.78;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtCharmF0M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// FiMjSkPP define theoretical uncertainties around F0M0S0PP as follows:
// PtCharmFiMjSkPP = PtCharmF0M0S0PP * (dN(i,j,k)/dpt / dN(0,0,0)/dpt)_MNR
//       i=0,1,2;  j=0,1,2;  k=0,1,...,6
// dN(i,j,k)/dpt - spectra obtained by A.Dainese (hep-ph/0601164, p.88; 
// http://www-zeus.desy.de/~corradi/benchmarks) from NLO pQCD (MNR)
// calculations for the following inputs: 
// Peterson fragmentation function (F) with \epsilon_c = 0.02, 0.002 & 0.11 
// for i=0,1 & 2 respectively; quark mass (M) of 1.5, 1.3 & 1.7 GeV 
// for j=0,1 & 2 respectively; 
// factorisation \mu_F = a*mt and renormalisation \mu_R = b*mt scales (S) 
// with a/b = 1/1, 1/0.5, 0.5/1, 0.5/0.5, 1/2, 2/1 & 2/2 
// for k = 0, 1, 2, 3, 4, 5 & 6 respectively; CTEQ6.1 PDF set 
// (PDF uncertainty not considered since is small, see hep-ph/0601164, p.89).
// June 2008, Smbat.Grigoryan@cern.ch

// Charm pT
// Pythia6.214 (kCharmppMNRwmi, PDF = CTEQ5L, quark mass = 1.2 GeV, PtHard > 2.76 GeV/c)
// for pp collisions at 14 TeV with one c-cbar pair per event.
// Corresponding NLO total cross section is 5.68 mb


  const Double_t kpt0 = 2.2930;
  const Double_t kxn  = 3.1196;
  Double_t c[3]={-5.2180e-01,1.8753e-01,2.8669e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF1M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 6.06 mb
  const Double_t kpt0 = 2.8669;
  const Double_t kxn  = 3.1044;
  Double_t c[3]={-4.6714e-01,1.5005e-01,4.5003e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF2M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 6.06 mb
  const Double_t kpt0 = 1.8361;
  const Double_t kxn  = 3.2966;
  Double_t c[3]={-6.1550e-01,2.6498e-01,1.0728e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M1S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 7.69 mb
  const Double_t kpt0 = 2.1280;
  const Double_t kxn  = 3.1397;
  Double_t c[3]={-5.4021e-01,2.0944e-01,2.5211e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M2S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 4.81 mb
  const Double_t kpt0 = 2.4579;
  const Double_t kxn  = 3.1095;
  Double_t c[3]={-5.1497e-01,1.7532e-01,3.2429e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S1PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 14.09 mb
  const Double_t kpt0 = 2.1272;
  const Double_t kxn  = 3.1904;
  Double_t c[3]={-4.6088e-01,2.1918e-01,2.3055e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S2PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 1.52 mb
  const Double_t kpt0 = 2.8159;
  const Double_t kxn  = 3.0857;
  Double_t c[3]={-6.4691e-01,2.0289e-01,2.4922e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S3PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 3.67 mb
  const Double_t kpt0 = 2.7297;
  const Double_t kxn  = 3.3019;
  Double_t c[3]={-6.2216e-01,1.9031e-01,1.5341e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S4PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 3.38 mb
  const Double_t kpt0 = 2.3894;
  const Double_t kxn  = 3.1075;
  Double_t c[3]={-4.9742e-01,1.7032e-01,2.5994e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S5PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 10.37 mb
  const Double_t kpt0 = 2.0187;
  const Double_t kxn  = 3.3011;
  Double_t c[3]={-3.9869e-01,2.9248e-01,1.1763e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}
Double_t AliGenMUONlib::PtCharmF0M0S6PP( Double_t *px, Double_t */*dummy*/)
{
// Charm pT
// Corresponding NLO total cross section is 7.22 mb
  const Double_t kpt0 = 2.1089;
  const Double_t kxn  = 3.1848;
  Double_t c[3]={-4.6275e-01,1.8114e-01,2.1363e-02};
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn)*(1.+c[0]*x+c[1]*x*x)/(1.+c[2]*x*x);
}

//                  y-distribution
Double_t AliGenMUONlib::YCharm( Double_t *px, Double_t */*dummy*/)
{
// Charm y :: Carrer & Dainese : ALICE-INT-2003-019 v.3 (hep-ph/0311225) 
// Pythia tuned to reproduce the distribution given by the HVQMNR program based on NLO calculations (pQCD)
// shadowing + kt broadening 

    Double_t x=px[0];
    Double_t c[2]={-2.42985e-03,-2.31001e-04};
    Double_t y=1+(c[0]*TMath::Power(x,2))+(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>8) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// FiMjSkPP define theoretical uncertainties around F0M0S0PP as follows:
// YCharmFiMjSkPP = YCharmF0M0S0PP * (dN(i,j,k)/dy / dN(0,0,0)/dy)_MNR
//       i=0,1,2;  j=0,1,2;  k=0,1,...,6
// dN(i,j,k)/dy - spectra obtained by A.Dainese (hep-ph/0601164, p.88; 
// http://www-zeus.desy.de/~corradi/benchmarks) from NLO pQCD (MNR) 
// calculations for the following inputs: 
// Peterson fragmentation function (F) with \epsilon_c = 0.02, 0.002 & 0.11 
// for i=0,1 & 2 respectively; quark mass (M) of 1.5, 1.3 & 1.7 GeV 
// for j=0,1 & 2 respectively; 
// factorisation \mu_F = a*mt and renormalisation \mu_R = b*mt scales (S) 
// with a/b = 1/1,1/0.5, 0.5/1, 0.5/0.5, 1/2, 2/1 & 2/2 for 
// k = 0, 1, 2, 3, 4, 5 & 6 respectively; CTEQ6.1 PDF set
// (PDF uncertainty not considered since is small, see hep-ph/0601164, p.89).
// June 2008, Smbat.Grigoryan@cern.ch

// Charm y
// Pythia6.214 (kCharmppMNRwmi, PDF = CTEQ5L, quark mass = 1.2 GeV, PtHard > 2.76 GeV/c)
// for pp collisions at 14 TeV with one c-cbar pair per event.
// Corresponding NLO total cross section is 5.68 mb

    Double_t x=px[0];
    Double_t c[2]={7.0909e-03,6.1967e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF1M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 6.06 mb
    Double_t x=px[0];
    Double_t c[2]={6.9707e-03,6.0971e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF2M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 6.06 mb
    Double_t x=px[0];
    Double_t c[2]={7.1687e-03,6.5303e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M1S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 7.69 mb
    Double_t x=px[0];
    Double_t c[2]={5.9090e-03,7.1854e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M2S0PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 4.81 mb
    Double_t x=px[0];
    Double_t c[2]={8.0882e-03,5.5872e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S1PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 14.09 mb
    Double_t x=px[0];
    Double_t c[2]={7.2520e-03,6.2691e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S2PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 1.52 mb
    Double_t x=px[0];
    Double_t c[2]={1.1040e-04,1.4498e-04};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S3PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 3.67 mb
    Double_t x=px[0];
    Double_t c[2]={-3.1328e-03,1.8270e-04};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S4PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 3.38 mb
    Double_t x=px[0];
    Double_t c[2]={7.0865e-03,6.2532e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S5PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 10.37 mb
    Double_t x=px[0];
    Double_t c[2]={7.7070e-03,5.3533e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}
Double_t AliGenMUONlib::YCharmF0M0S6PP( Double_t *px, Double_t */*dummy*/)
{
// Charm y
// Corresponding NLO total cross section is 7.22 mb
    Double_t x=px[0];
    Double_t c[2]={7.9195e-03,5.3823e-05};
    Double_t y=1-(c[0]*TMath::Power(x,2))-(c[1]*TMath::Power(x,4));
    Double_t ycharm;
    
    if (TMath::Abs(x)>9) {
      ycharm=0.;
    }
    else {
      ycharm=TMath::Power(y,3);
    }
    
    return ycharm;
}


Int_t AliGenMUONlib::IpCharm(TRandom *ran)
{  
// Charm composition
    Float_t random;
    Int_t ip;
//    411,421,431,4122
    random = ran->Rndm();
//  Taux de production Carrer & Dainese : ALICE-INT-2003-019 v.3  
//  >>>>> cf. tab 4 p 11
  
    if (random < 0.30) {                       
        ip=421;
    } else if (random < 0.60) {
        ip=-421;
    } else if (random < 0.70) {
        ip=411;
    } else if (random < 0.80) {
        ip=-411;
    } else if (random < 0.86) {
        ip=431;
    } else if (random < 0.92) {
        ip=-431;	
    } else if (random < 0.96) {
        ip=4122;
    } else {
        ip=-4122;
    }
    
    return ip;
}

//
//                        Beauty
//
//
//                    pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtBeauty( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
  const Double_t kpt0 = 6.53;
  const Double_t kxn  = 3.59;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

Double_t AliGenMUONlib::PtBeautyCentral( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
  const Double_t kpt0 = 6.14;
  const Double_t kxn  = 2.93;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// FiMjSkPP define theoretical uncertainties around F0M0S0PP as follows:
// PtBeautyFiMjSkPP = PtBeautyF0M0S0PP * (dN(i,j,k)/dpt / dN(0,0,0)/dpt)_MNR
//       i=0,1,2;  j=0,1,2;  k=0,1,...,6
// dN(i,j,k)/dpt - spectra obtained by A.Dainese (hep-ph/0601164, p.88; 
// http://www-zeus.desy.de/~corradi/benchmarks) from NLO pQCD (MNR) 
// calculations for the following inputs: 
// Peterson fragmentation function (F) with \epsilon_b = 0.001, 0.0002 & 0.004 
// for i=0,1 & 2 respectively; quark mass (M) of 4.75, 4.5 & 5.0 GeV 
// for j=0,1 & 2 respectively; 
// factorisation \mu_F = a*mt and renormalisation \mu_R = b*mt scales (S) 
// with a/b = 1/1, 1/0.5, 0.5/1, 0.5/0.5, 1/2, 2/1 & 2/2 for 
// k = 0, 1, 2, 3, 4, 5 & 6 respectively; CTEQ6.1 PDF set
// (PDF uncertainty not considered since is small, see hep-ph/0601164, p.89).
// June 2008, Smbat.Grigoryan@cern.ch

// Beauty pT
// Pythia6.214 (kBeautyppMNRwmi, PDF = CTEQ5L, quark mass = 4.75 GeV, PtHard > 2.76 GeV/c)
// for pp collisions at 14 TeV with one b-bbar pair per event.
// Corresponding NLO total cross section is 0.494 mb

  const Double_t kpt0 = 8.0575;
  const Double_t kxn  = 3.1921;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF1M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.445 mb
  const Double_t kpt0 = 8.6239;
  const Double_t kxn  = 3.2911;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF2M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.445 mb
  const Double_t kpt0 = 7.3367;
  const Double_t kxn  = 3.0692;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M1S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.518 mb
  const Double_t kpt0 = 7.6409;
  const Double_t kxn  = 3.1364;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M2S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.384 mb
  const Double_t kpt0 = 8.4948;
  const Double_t kxn  = 3.2546;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S1PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.648 mb
  const Double_t kpt0 = 7.6631;
  const Double_t kxn  = 3.1621;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S2PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.294 mb
  const Double_t kpt0 = 8.7245;
  const Double_t kxn  = 3.2213;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S3PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.475 mb
  const Double_t kpt0 = 8.5296;
  const Double_t kxn  = 3.2187;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S4PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.324 mb
  const Double_t kpt0 = 7.9440;
  const Double_t kxn  = 3.1614;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S5PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.536 mb
  const Double_t kpt0 = 8.2408;
  const Double_t kxn  = 3.3029;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtBeautyF0M0S6PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty pT
// Corresponding NLO total cross section is 0.420 mb
  const Double_t kpt0 = 7.8041;
  const Double_t kxn  = 3.2094;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

//                     y-distribution
Double_t AliGenMUONlib::YBeauty( Double_t *px, Double_t */*dummy*/)
{
// Beauty y :: Carrer & Dainese : ALICE-INT-2003-019 v.3 (hep-ph/0311225) 
// Pythia tuned to reproduce the distribution given by the HVQMNR program based on NLO calculations (pQCD)
// shadowing + kt broadening 

    Double_t x=px[0];
    Double_t c[2]={-1.27590e-02,-2.42731e-04};
    Double_t y=1+c[0]*TMath::Power(x,2)+c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// FiMjSkPP define theoretical uncertainties around F0M0S0PP as follows:
// YBeautyFiMjSkPP = YBeautyF0M0S0PP * (dN(i,j,k)/dy / dN(0,0,0)/dy)_MNR
//       i=0,1,2;  j=0,1,2;  k=0,1,...,6
// dN(i,j,k)/dy - spectra obtained by A.Dainese (hep-ph/0601164, p.88; 
// http://www-zeus.desy.de/~corradi/benchmarks) from NLO pQCD (MNR) 
// calculations for the following inputs: 
// Peterson fragmentation function (F) with \epsilon_b = 0.001, 0.0002 & 0.004 
// for i=0,1 & 2 respectively; quark mass (M) of 4.75, 4.5 & 5.0 GeV 
// for j=0,1 & 2 respectively; 
// factorisation \mu_F = a*mt and renormalisation \mu_R = b*mt scales (S) 
// with a/b = 1/1, 1/0.5, 0.5/1, 0.5/0.5, 1/2, 2/1 & 2/2 
// for k = 0, 1, 2, 3, 4, 5 & 6 respectively; CTEQ6.1 PDF set 
// (PDF uncertainty not considered since is small, see hep-ph/0601164, p.89).
// June 2008, Smbat.Grigoryan@cern.ch

// Beauty y
// Pythia6.214 (kBeautyppMNRwmi, PDF = CTEQ5L, quark mass = 4.75 GeV, PtHard > 2.76 GeV/c)
// for pp collisions at 14 TeV with one b-bbar pair per event.
// Corresponding NLO total cross section is 0.494 mb


    Double_t x=px[0];
    Double_t c[2]={1.2350e-02,9.2667e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF1M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.445 mb
    Double_t x=px[0];
    Double_t c[2]={1.2292e-02,9.1847e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF2M0S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.445 mb
    Double_t x=px[0];
    Double_t c[2]={1.2436e-02,9.3709e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M1S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.518 mb
    Double_t x=px[0];
    Double_t c[2]={1.1714e-02,1.0068e-04};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M2S0PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.384 mb
    Double_t x=px[0];
    Double_t c[2]={1.2944e-02,8.5500e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S1PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.648 mb
    Double_t x=px[0];
    Double_t c[2]={1.2455e-02,9.2713e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S2PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.294 mb
    Double_t x=px[0];
    Double_t c[2]={1.0897e-02,1.1878e-04};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S3PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.475 mb
    Double_t x=px[0];
    Double_t c[2]={1.0912e-02,1.1858e-04};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S4PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.324 mb
    Double_t x=px[0];
    Double_t c[2]={1.2378e-02,9.2490e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S5PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.536 mb
    Double_t x=px[0];
    Double_t c[2]={1.2886e-02,8.2912e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}
Double_t AliGenMUONlib::YBeautyF0M0S6PP( Double_t *px, Double_t */*dummy*/)
{
// Beauty y
// Corresponding NLO total cross section is 0.420 mb
    Double_t x=px[0];
    Double_t c[2]={1.3106e-02,8.0115e-05};
    Double_t y=1-c[0]*TMath::Power(x,2)-c[1]*TMath::Power(x,4);
    Double_t ybeauty;
    
    if (TMath::Abs(x)>7.6) {
      ybeauty=0.;
    }
    else {
      ybeauty=TMath::Power(y,3);
    }
    
    return ybeauty;
}

Int_t AliGenMUONlib::IpBeauty(TRandom *ran)
{  
// Beauty Composition
    Float_t random;
    Int_t ip;
    random = ran->Rndm(); 
    
//  Taux de production Carrer & Dainese : ALICE-INT-2003-019 v.3  
//  >>>>> cf. tab 4 p 11
    
 if (random < 0.20) {                       
        ip=511;
    } else if (random < 0.40) {
        ip=-511;
    } else if (random < 0.605) {
        ip=521;
    } else if (random < 0.81) {
        ip=-521;
    } else if (random < 0.87) {
        ip=531;
    } else if (random < 0.93) {
        ip=-531;	
    } else if (random < 0.965) {
        ip=5122;
    } else {
        ip=-5122;
    }
    
 return ip;
}


typedef Double_t (*GenFunc) (Double_t*,  Double_t*);
GenFunc AliGenMUONlib::GetPt(Int_t param,  const char* tname) const
{
// Return pointer to pT parameterisation
    TString sname = TString(tname);
    GenFunc func;
    switch (param) 
    {
    case kPhi:
	func=PtPhi;
	break;
    case kOmega:
	func=PtOmega;
	break;
    case kEta:
	func=PtEta;
	break;
    case kJpsiFamily:
    case kPsiP:
    case kJpsi:
	if (sname == "Vogt" || sname == "Vogt PbPb") {
	    func=PtJpsiPbPb;
	} else if (sname == "Vogt pp") {
	    func=PtJpsiPP;
	} else if (sname == "CDF scaled") {
	    func=PtJpsiCDFscaled;
	} else if (sname == "CDF pp") {
	    func=PtJpsiCDFscaledPP;
	} else if (sname == "CDF pp 10") {
	    func=PtJpsiCDFscaledPP10;
	} else if (sname == "CDF scaled old") {
	    func=PtJpsiCDFscaledold;
	} else if (sname == "CDF pp old") {
	    func=PtJpsiCDFscaledPPold;
	} else if (sname == "Flat") {
	    func=PtJpsiFlat;
	} else {
	    func=PtJpsi;
	}
	break;
    case kJpsiFromB:
	func = PtJpsiBPbPb;
	break;
    case kUpsilonFamily:
    case kUpsilonP:
    case kUpsilonPP:
    case kUpsilon:
	if (sname == "Vogt" || sname == "Vogt PbPb") {
	    func=PtUpsilonPbPb;
	} else if (sname == "Vogt pp") {
	    func=PtUpsilonPP;
	} else if (sname == "CDF scaled") {
	    func=PtUpsilonCDFscaled;
	} else if (sname == "CDF pp") {
	    func=PtUpsilonCDFscaledPP;
	} else if (sname == "Flat") {
	    func=PtUpsilonFlat;
	} else {
	    func=PtUpsilon;
	}
	break;  
    case kCharm:
	if (sname == "F0M0S0 pp") {
	    func=PtCharmF0M0S0PP;
	} else if (sname == "F1M0S0 pp") {
	    func=PtCharmF1M0S0PP;
	} else if (sname == "F2M0S0 pp") {
	    func=PtCharmF2M0S0PP;
	} else if (sname == "F0M1S0 pp") {
	    func=PtCharmF0M1S0PP;
	} else if (sname == "F0M2S0 pp") {
	    func=PtCharmF0M2S0PP;
	} else if (sname == "F0M0S1 pp") {
	    func=PtCharmF0M0S1PP;
	} else if (sname == "F0M0S2 pp") {
	    func=PtCharmF0M0S2PP;
	} else if (sname == "F0M0S3 pp") {
	    func=PtCharmF0M0S3PP;
	} else if (sname == "F0M0S4 pp") {
	    func=PtCharmF0M0S4PP;
	} else if (sname == "F0M0S5 pp") {
	    func=PtCharmF0M0S5PP;
	} else if (sname == "F0M0S6 pp") {
	    func=PtCharmF0M0S6PP;
	} else if (sname == "central") {
	    func=PtCharmCentral;
	} else {
	    func=PtCharm;
	}
	break;
    case kBeauty:
	if (sname == "F0M0S0 pp") {
	    func=PtBeautyF0M0S0PP;
	} else if (sname == "F1M0S0 pp") {
	    func=PtBeautyF1M0S0PP;
	} else if (sname == "F2M0S0 pp") {
	    func=PtBeautyF2M0S0PP;
	} else if (sname == "F0M1S0 pp") {
	    func=PtBeautyF0M1S0PP;
	} else if (sname == "F0M2S0 pp") {
	    func=PtBeautyF0M2S0PP;
	} else if (sname == "F0M0S1 pp") {
	    func=PtBeautyF0M0S1PP;
	} else if (sname == "F0M0S2 pp") {
	    func=PtBeautyF0M0S2PP;
	} else if (sname == "F0M0S3 pp") {
	    func=PtBeautyF0M0S3PP;
	} else if (sname == "F0M0S4 pp") {
	    func=PtBeautyF0M0S4PP;
	} else if (sname == "F0M0S5 pp") {
	    func=PtBeautyF0M0S5PP;
	} else if (sname == "F0M0S6 pp") {
	    func=PtBeautyF0M0S6PP;
	} else if (sname == "central") {
	    func=PtBeautyCentral;
	} else {
	    func=PtBeauty;
	}
	break;
    case kPion:
	func=PtPion;
	break;
    case kKaon:
	func=PtKaon;
	break;
    case kChic0:
	func=PtChic0;
	break;
    case kChic1:
	func=PtChic1;
	break;
    case kChic2:
	func=PtChic2;
	break;
    case kChic:
	func=PtChic;
	break;
    default:
        func=0;
        printf("<AliGenMUONlib::GetPt> unknown parametrisation\n");
    }
    return func;
}

GenFunc AliGenMUONlib::GetY(Int_t param, const char* tname) const
{
  //    
  // Return pointer to y- parameterisation
  //
    TString sname = TString(tname);
    GenFunc func;
    switch (param) 
    {
    case kPhi:
	func=YPhi;
	break;
    case kEta:
	func=YEta;
	break;
    case kOmega:
	func=YOmega;
	break;
    case kJpsiFamily:
    case kPsiP:
    case kJpsi:
	if (sname == "Vogt" || sname == "Vogt PbPb") {
	    func=YJpsiPbPb;
	} else if (sname == "Vogt pp"){
	    func=YJpsiPP;
	} else if (sname == "CDF scaled") {
	    func=YJpsiCDFscaled;
	} else if (sname == "CDF pp") {
	    func=YJpsiCDFscaledPP;
	} else if (sname == "CDF pp 10") {
	    func=YJpsiCDFscaledPP10;
	} else if (sname == "CDF scaled old") {
	    func=YJpsiCDFscaledold;
	} else if (sname == "CDF pp old") {
	    func=YJpsiCDFscaledPPold;
	} else if (sname == "Flat") {
	    func=YJpsiFlat;
	} else {
	    func=YJpsi;
	}
	break;
    case kJpsiFromB:
	func = YJpsiBPbPb;
	break;
    case kUpsilonFamily:
    case kUpsilonP:
    case kUpsilonPP:
    case kUpsilon:
	if (sname == "Vogt" || sname == "Vogt PbPb") {
	    func=YUpsilonPbPb;
	} else if (sname == "Vogt pp") {
	    func = YUpsilonPP;
	} else if (sname == "CDF scaled") {
	    func=YUpsilonCDFscaled;
	} else if (sname == "CDF pp") {
	    func=YUpsilonCDFscaledPP;
	} else if (sname == "Flat") {
	    func=YUpsilonFlat;
	} else {
	    func=YUpsilon;
	}
	break;
    case kCharm:
	if (sname == "F0M0S0 pp") {
	    func=YCharmF0M0S0PP;
	} else if (sname == "F1M0S0 pp") {
	    func=YCharmF1M0S0PP;
	} else if (sname == "F2M0S0 pp") {
	    func=YCharmF2M0S0PP;
	} else if (sname == "F0M1S0 pp") {
	    func=YCharmF0M1S0PP;
	} else if (sname == "F0M2S0 pp") {
	    func=YCharmF0M2S0PP;
	} else if (sname == "F0M0S1 pp") {
	    func=YCharmF0M0S1PP;
	} else if (sname == "F0M0S2 pp") {
	    func=YCharmF0M0S2PP;
	} else if (sname == "F0M0S3 pp") {
	    func=YCharmF0M0S3PP;
	} else if (sname == "F0M0S4 pp") {
	    func=YCharmF0M0S4PP;
	} else if (sname == "F0M0S5 pp") {
	    func=YCharmF0M0S5PP;
	} else if (sname == "F0M0S6 pp") {
	    func=YCharmF0M0S6PP;
	} else {
	    func=YCharm;
	}
	break;
    case kBeauty:
	if (sname == "F0M0S0 pp") {
	    func=YBeautyF0M0S0PP;
	} else if (sname == "F1M0S0 pp") {
	    func=YBeautyF1M0S0PP;
	} else if (sname == "F2M0S0 pp") {
	    func=YBeautyF2M0S0PP;
	} else if (sname == "F0M1S0 pp") {
	    func=YBeautyF0M1S0PP;
	} else if (sname == "F0M2S0 pp") {
	    func=YBeautyF0M2S0PP;
	} else if (sname == "F0M0S1 pp") {
	    func=YBeautyF0M0S1PP;
	} else if (sname == "F0M0S2 pp") {
	    func=YBeautyF0M0S2PP;
	} else if (sname == "F0M0S3 pp") {
	    func=YBeautyF0M0S3PP;
	} else if (sname == "F0M0S4 pp") {
	    func=YBeautyF0M0S4PP;
	} else if (sname == "F0M0S5 pp") {
	    func=YBeautyF0M0S5PP;
	} else if (sname == "F0M0S6 pp") {
	    func=YBeautyF0M0S6PP;
	} else {
	    func=YBeauty;
	}
	break;
    case kPion:
	func=YPion;
	break;
    case kKaon:
	func=YKaon;
	break;
    case kChic0:
	func=YChic0;
	break;
    case kChic1:
	func=YChic1;
	break;
    case kChic2:
	func=YChic2;
	break;
    case kChic:
	func=YChic;
	break;
    default:
        func=0;
        printf("<AliGenMUONlib::GetY> unknown parametrisation\n");
    }
    return func;
}

//
//                    Chi
//
//
//                pt-distribution
//____________________________________________________________
Double_t AliGenMUONlib::PtChic0( Double_t *px, Double_t */*dummy*/)
{
// Chi_c1 pT
  const Double_t kpt0 = 4.;
  const Double_t kxn  = 3.6;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtChic1( Double_t *px, Double_t */*dummy*/)
{
// Chi_c1 pT
  const Double_t kpt0 = 4.;
  const Double_t kxn  = 3.6;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtChic2( Double_t *px, Double_t */*dummy*/)
{
// Chi_c2 pT
  const Double_t kpt0 = 4.;
  const Double_t kxn  = 3.6;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}
Double_t AliGenMUONlib::PtChic( Double_t *px, Double_t */*dummy*/)
{
// Chi_c family pT
  const Double_t kpt0 = 4.;
  const Double_t kxn  = 3.6;
  Double_t x=*px;
  //
  Double_t pass1 = 1.+(x/kpt0)*(x/kpt0);
  return x/TMath::Power(pass1,kxn);
}

//
//               y-distribution
//____________________________________________________________
Double_t AliGenMUONlib::YChic0(Double_t *py, Double_t */*dummy*/)
{
// Chi-1c y
  const Double_t ky0 = 4.;
 const Double_t kb=1.;
  Double_t yj;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yj=kb;
  else
    yj=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yj;
}

Double_t AliGenMUONlib::YChic1(Double_t *py, Double_t */*dummy*/)
{
// Chi-1c y
  const Double_t ky0 = 4.;
  const Double_t kb=1.;
  Double_t yj;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yj=kb;
  else
    yj=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yj;
}

Double_t AliGenMUONlib::YChic2(Double_t *py, Double_t */*dummy*/)
{
// Chi-2c y
  const Double_t ky0 = 4.;
  const Double_t kb=1.;
  Double_t yj;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yj=kb;
  else
    yj=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yj;
}

Double_t AliGenMUONlib::YChic(Double_t *py, Double_t */*dummy*/)
{
// Chi_c family y
  const Double_t ky0 = 4.;
  const Double_t kb=1.;
  Double_t yj;
  Double_t y=TMath::Abs(*py);
  //
  if (y < ky0)
    yj=kb;
  else
    yj=kb*TMath::Exp(-(y-ky0)*(y-ky0)/2);
  return yj;
}

//                 particle composition
//
Int_t AliGenMUONlib::IpChic0(TRandom *)
{
// Chi composition
    return 10441;
}
//
Int_t AliGenMUONlib::IpChic1(TRandom *)
{
// Chi composition
    return 20443;
}
Int_t AliGenMUONlib::IpChic2(TRandom *)
{
// Chi_c2 prime composition
    return 445;
}
Int_t AliGenMUONlib::IpChic(TRandom *)
{
// Chi composition
  Int_t ip;
  Float_t r = gRandom->Rndm();
  if (r < 0.001) {
    ip = 10441;
  } else if( r < 0.377 ) {
    ip = 20443;
  } else {
    ip = 445;
  }
  return ip;
}


//_____________________________________________________________

typedef Int_t (*GenFuncIp) (TRandom *);
GenFuncIp AliGenMUONlib::GetIp(Int_t param,  const char* /*tname*/) const
{
// Return pointer to particle type parameterisation
    GenFuncIp func;
    switch (param) 
    {
    case kPhi:
	func=IpPhi;
	break;
    case kEta:
	func=IpEta;
	break;
    case kOmega:
	func=IpOmega;
	break;
    case kJpsiFamily:
      	func=IpJpsiFamily;
	break;
    case kPsiP:
      	func=IpPsiP;
	break;
    case kJpsi:
    case kJpsiFromB:
	func=IpJpsi;
	break;
    case kUpsilon:
	func=IpUpsilon;
	break;
    case kUpsilonFamily:
      func=IpUpsilonFamily;
      break;
    case kUpsilonP:
	func=IpUpsilonP;
	break;
    case kUpsilonPP:
	func=IpUpsilonPP;
	break;
    case kCharm:
	func=IpCharm;
	break;
    case kBeauty:
	func=IpBeauty;
	break;
    case kPion:
	func=IpPion;
	break;
    case kKaon:
	func=IpKaon;
	break;
    case kChic0:
	func=IpChic0;
	break;
    case kChic1:
	func=IpChic1;
	break;
    case kChic2:
	func=IpChic2;
	break;
    case kChic:
        func=IpChic;
        break;
    default:
        func=0;
        printf("<AliGenMUONlib::GetIp> unknown parametrisation\n");
    }
    return func;
}



Float_t AliGenMUONlib::Interpolate(Float_t x, Float_t* y, Float_t x0, 
				   Float_t dx,
				   Int_t n, Int_t no)
{
//
// Neville's alorithm for interpolation
//
// x:  x-value
// y:  Input array
// x0: minimum x 
// dx: step size
//  n: number of data points
// no: order of polynom 
//
    Float_t*  c = new Float_t[n];
    Float_t*  d = new Float_t[n];
    Int_t m, i;
    for (i = 0; i < n; i++) {
	c[i] = y[i];
	d[i] = y[i];
    }
    
    Int_t   ns  = int((x - x0)/dx);
    
    Float_t y1  = y[ns];
    ns--;    
    for (m = 0; m < no; m++) {
	for (i = 0; i < n-m; i++) {	
	    Float_t ho = x0 + Float_t(i) * dx - x;
	    Float_t hp = x0 + Float_t(i+m+1) * dx - x;
	    Float_t w  = c[i+1] - d[i];
	    Float_t den = ho-hp;
	    den = w/den;
	    d[i] = hp * den;
	    c[i] = ho * den;
	}
	Float_t dy;
	
	if (2*ns < (n-m-1)) {
	    dy  = c[ns+1];
	} else {
	    dy  = d[ns--];
	}
	y1 += dy;}
    delete[] c;
    delete[] d;

    return y1;
}


