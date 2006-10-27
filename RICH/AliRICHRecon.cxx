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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AliRICHRecon                                                         //
//                                                                      //
// RICH class to perfom pattern recognition based on Hough transfrom    //
// for single chamber                                                   //
//////////////////////////////////////////////////////////////////////////

#include "AliRICHRecon.h"  //class header
#include "AliRICHCluster.h" //CkovAngle()
#include <AliLog.h>        //AliInfo()
#include <TMath.h>         //many 
#include <TRotation.h>      //
#include <TH1D.h>          //HoughResponse()
#include <TClonesArray.h>  //CkovAngle()

const Double_t AliRICHRecon::fkRadThick=1.5;
const Double_t AliRICHRecon::fkWinThick=0.5;
const Double_t AliRICHRecon::fkGapThick=8.0;
const Double_t AliRICHRecon::fkRadIdx  =1.292;
const Double_t AliRICHRecon::fkWinIdx  =1.5787;
const Double_t AliRICHRecon::fkGapIdx  =1.0005;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliRICHRecon::AliRICHRecon():TTask("RichRec","RichPat"),
  fPhotCnt(-1),
  fCkovSigma2(0),
  fIsWEIGHT(kFALSE),
  fDTheta(0.001),
  fWindowWidth(0.045),
  fTrkDir(TVector3(0,0,1)),fTrkPos(TVector2(30,40))  
{
// main ctor
  for (Int_t i=0; i<3000; i++) {
    fPhotFlag[i] =  0;
    fPhotCkov[i] = -1;
    fPhotPhi [i] = -1;
    fPhotWei [i] =  0;
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::CkovAngle(TClonesArray *pCluLst,Int_t &iNaccepted)
{
// Pattern recognition method based on Hough transform
// Arguments: pCluLst  - list of clusters for this chamber   
//   Returns:          - track ckov angle, [rad], 
  
  if(pCluLst->GetEntries()>200) fIsWEIGHT = kTRUE; // offset to take into account bkg in reconstruction
  else                          fIsWEIGHT = kFALSE;

  // Photon Flag:  Flag = 0 initial set; Flag = 1 good candidate (charge compatible with photon); Flag = 2 photon used for the ring;

  fPhotCnt=0;                                                      
  for (Int_t iClu=0; iClu<pCluLst->GetEntriesFast();iClu++){//clusters loop
    AliRICHCluster *pClu=(AliRICHCluster*)pCluLst->UncheckedAt(iClu);                       //get pointer to current cluster    
    if(pClu->Q()>100) continue;                                                             //avoid MIP clusters from bkg
    
    fPhotCkov[fPhotCnt]=FindPhotCkov(pClu->X(),pClu->Y());                                  //find ckov angle for this  photon candidate
    fPhotCnt++;         //increment counter of photon candidates
  }//clusters loop

  iNaccepted=FlagPhot(HoughResponse()); //flag photons according to individual theta ckov with respect to most probable track theta ckov
  if(iNaccepted<1) return -11; 
  else             return FindRingCkov(pCluLst->GetEntries());  //find best Theta ckov for ring i.e. track
}//ThetaCerenkov()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::FindPhotCkov(Double_t cluX,Double_t cluY)
{
// Finds Cerenkov angle  for this photon candidate
// Arguments: cluX,cluY - position of cadidate's cluster  
//   Returns: Cerenkov angle 

  TVector2 pos(cluX,cluY); Double_t cluR=(pos-fTrkPos).Mod();  Double_t phi=FindPhotPhi(cluX,cluY);      
  Printf("new dist %f phi %f",cluR,phi);
  Double_t ckov1=0,ckov2=0.75;
  const Double_t kTol=0.05; 
  Int_t iIterCnt = 0;
  while(1){
    if(iIterCnt>=50) return -1;
    Double_t ckov=0.5*(ckov1+ckov2);
    Double_t dist=cluR-TracePhoton(ckov,phi,pos); iIterCnt++; //get distance between trial point and cluster position
    Printf("New: phi %f ckov %f dist %f",phi,ckov,dist);
    if     (dist> kTol) ckov1=ckov;                           //cluster @ larger ckov 
    else if(dist<-kTol) ckov2=ckov;                           //cluster @ smaller ckov
    else                return ckov;                          //precision achived         
  }
}//FindPhotTheta()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::FindPhotPhi(Double_t cluX,Double_t cluY)
{
// Finds phi angle og photon candidate by considering the cluster's position  of this candudate w.r.t track position
  
  Double_t emiss=0; 
  return fPhotPhi[fPhotCnt]=TMath::ATan2(cluY-fTrkPos.Y()-emiss*TMath::Tan(fTrkDir.Theta())*TMath::Sin(fTrkDir.Phi()),
                                         cluX-fTrkPos.X()-emiss*TMath::Tan(fTrkDir.Theta())*TMath::Cos(fTrkDir.Phi()));
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::FindRingArea(Double_t ckovAng)const
{
// Find area inside the cerenkov ring which lays inside PCs
// Arguments: ckovThe - cernkov    
//   Returns: area of the ring in cm^2 for given theta ckov
   
  
  TVector2 pos1,pos2;
  
  const Int_t kN=100;
  Double_t area=0;
  for(Int_t i=0;i<kN;i++){
    TracePhoton(ckovAng,Double_t(TMath::TwoPi()*i    /kN),pos1);//trace this photon 
    TracePhoton(ckovAng,Double_t(TMath::TwoPi()*(i+1)/kN),pos2);//trace this photon 
    area+=(pos1-fTrkPos)*(pos2-fTrkPos);
      
  }
  return area;
}//RingArea()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::TracePhoton(Double_t ckovThe,Double_t ckovPhi,TVector2 &pos)const
{
// Trace a single Ckov photon from emission point somewhere in radiator up to photocathode taking into account ref indexes of materials it travereses
// Arguments: ckovThe,ckovPhi- photon ckov angles, [rad]  (warning: not photon theta and phi)     
//   Returns: distance between photon point on PC and track projection  
  TRotation mtheta;   mtheta.RotateY(fTrkDir.Theta());
  TRotation mphi;       mphi.RotateZ(fTrkDir.Phi());  
  TRotation mrot=mphi*mtheta;
  
  TVector3  posCkov(fTrkPos.X(),fTrkPos.Y(),-0.5*fkRadThick-fkWinThick-fkGapThick);   //RAD: photon position is track position @ middle of RAD 
  TVector3  dirCkov;   dirCkov.SetMagThetaPhi(1,ckovThe,ckovPhi);                     //initially photon is directed according to requested ckov angle
                                               dirCkov=mrot*dirCkov;                  //now we know photon direction in LORS
                       dirCkov.SetPhi(ckovPhi);   
  if(dirCkov.Theta() > TMath::ASin(1./fkRadIdx)) return -999;//total refraction on WIN-GAP boundary
  
  Propagate(dirCkov,posCkov,-fkWinThick-fkGapThick); //go to RAD-WIN boundary  remeber that z=0 is PC plane
  Refract  (dirCkov,         fkRadIdx,fkWinIdx    ); //RAD-WIN refraction
  Propagate(dirCkov,posCkov,-fkGapThick           ); //go to WIN-GAP boundary
  Refract  (dirCkov,         fkWinIdx,fkGapIdx    ); //WIN-GAP refraction
  Propagate(dirCkov,posCkov,0                     ); //go to PC
  
  pos.Set(posCkov.X(),posCkov.Y());
  return (pos-fTrkPos).Mod();
}//TracePhoton()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliRICHRecon::Propagate(const TVector3 &dir,TVector3 &pos,Double_t z)const
{
// Finds an intersection point between a line and XY plane shifted along Z.
// Arguments:  dir,pos   - vector along the line and any point of the line
//             z         - z coordinate of plain 
//   Returns:  none
//   On exit:  pos is the position if this intesection if any
  static TVector3 nrm(0,0,1); 
         TVector3 pnt(0,0,z);
  
  TVector3 diff=pnt-pos;
  Double_t sint=(nrm*diff)/(nrm*dir);
  pos+=sint*dir;
}//Propagate()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliRICHRecon::Refract(TVector3 &dir,Double_t n1,Double_t n2)const
{
// Refract direction vector according to Snell law
// Arguments: 
//            n1 - ref idx of first substance
//            n2 - ref idx of second substance
//   Returns: none
//   On exit: dir is new direction
  Double_t sinref=(n1/n2)*TMath::Sin(dir.Theta());
  if(sinref>1.)    dir.SetXYZ(-999,-999,-999);
  else             dir.SetTheta(TMath::ASin(sinref));
}//Refract()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::HoughResponse()
{
//
//
//       
  Double_t kThetaMax=0.75;
  Int_t nChannels = (Int_t)(kThetaMax/fDTheta+0.5);
  TH1D *phots   = new TH1D("Rphot"  ,"phots"         ,nChannels,0,kThetaMax);
  TH1D *photsw  = new TH1D("RphotWeighted" ,"photsw" ,nChannels,0,kThetaMax);
  TH1D *resultw = new TH1D("resultw","resultw"       ,nChannels,0,kThetaMax);
  Int_t nBin = (Int_t)(kThetaMax/fDTheta);
  Int_t nCorrBand = (Int_t)(fWindowWidth/(2*fDTheta));
  
  for (Int_t i=0; i< fPhotCnt; i++){//photon cadidates loop
    Double_t angle = fPhotCkov[i];  if(angle<0||angle>kThetaMax) continue;
    phots->Fill(angle);
    Int_t bin = (Int_t)(0.5+angle/(fDTheta));
    Double_t weight=1.;
    if(fIsWEIGHT){
      Double_t lowerlimit = ((Double_t)bin)*fDTheta - 0.5*fDTheta;  Double_t upperlimit = ((Double_t)bin)*fDTheta + 0.5*fDTheta;   
      Double_t diffArea = FindRingArea(upperlimit)-FindRingArea(lowerlimit);
      if(diffArea>0) weight = 1./diffArea;
    }
    photsw->Fill(angle,weight);
    fPhotWei[i]=weight;
  }//photon candidates loop 
   
  for (Int_t i=1; i<=nBin;i++){
    Int_t bin1= i-nCorrBand;
    Int_t bin2= i+nCorrBand;
    if(bin1<1) bin1=1;
    if(bin2>nBin)bin2=nBin;
    Double_t sumPhots=phots->Integral(bin1,bin2);
    if(sumPhots<3) continue;                            // if less then 3 photons don't trust to this ring
    Double_t sumPhotsw=photsw->Integral(bin1,bin2);
    resultw->Fill((Double_t)((i+0.5)*fDTheta),sumPhotsw);
  } 
// evaluate the "BEST" theta ckov as the maximum value of histogramm
  Double_t *pVec = resultw->GetArray();
  Int_t locMax = TMath::LocMax(nBin,pVec);
  phots->Delete();photsw->Delete();resultw->Delete(); // Reset and delete objects
  
  return (Double_t)(locMax*fDTheta+0.5*fDTheta); //final most probable track theta ckov   
}//HoughResponse()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::FindRingCkov(Int_t)
{
// Loops on all Ckov candidates and estimates the best Theta Ckov for a ring formed by those candidates. Also estimates an error for that Theat Ckov
// collecting errors for all single Ckov candidates thetas. (Assuming they are independent)  
// Arguments: iNclus- total number of clusters in chamber for background estimation
//    Return: best estimation of track Theta ckov

  Double_t wei = 0.;
  Double_t weightThetaCerenkov = 0.;

  Double_t ckovMin=9999.,ckovMax=0.;
  Double_t sigma2 = 0;   //to collect error squared for this ring
  
  for(Int_t i=0;i<fPhotCnt;i++){//candidates loop
    if(fPhotFlag[i] == 2){
      if(fPhotCkov[i]<ckovMin) ckovMin=fPhotCkov[i];  //find max and min Theta ckov from all candidates within probable window
      if(fPhotCkov[i]>ckovMax) ckovMax=fPhotCkov[i]; 
      weightThetaCerenkov += fPhotCkov[i]*fPhotWei[i];   wei += fPhotWei[i];                 //collect weight as sum of all candidate weghts   
      
     //Double_t phiref=(GetPhiPoint()-GetTrackPhi());
       if(fPhotCkov[i]<=0) continue;//?????????????????Flag photos = 2 may imply CkovEta = 0?????????????? 
                                     
      sigma2 += 1./Sigma2(fPhotCkov[i],fPhotPhi[i],fTrkDir.Theta(),fTrkDir.Phi());
    }
  }//candidates loop
  
  if(sigma2>0) fCkovSigma2=1./sigma2;
  else         fCkovSigma2=1e10;  
  

  if(wei != 0.) weightThetaCerenkov /= wei; else weightThetaCerenkov = 0.;  
  return weightThetaCerenkov;
}//FindCkovRing()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliRICHRecon::FlagPhot(Double_t ckov)
{
// Flag photon candidates if their individual ckov angle is inside the window around ckov angle returned by  HoughResponse()
// Arguments: ckov- value of most probable ckov angle for track as returned by HoughResponse()
//   Returns: number of photon candidates happened to be inside the window

  
  Int_t steps = (Int_t)((ckov )/ fDTheta); //how many times we need to have fDTheta to fill the distance between 0  and thetaCkovHough

  Double_t tmin = (Double_t)(steps - 1)*fDTheta;
  Double_t tmax = (Double_t)(steps)*fDTheta;
  Double_t tavg = 0.5*(tmin+tmax);

  tmin = tavg - 0.5*fWindowWidth;  tmax = tavg + 0.5*fWindowWidth;

  Int_t iInsideCnt = 0; //count photons which Theta ckov inside the window
  for(Int_t i=0;i<fPhotCnt;i++){//photon candidates loop
    if(fPhotCkov[i] >= tmin && fPhotCkov[i] <= tmax)	{ 
      fPhotFlag[i]=2;	  
      iInsideCnt++;
    }
  }
  return iInsideCnt;
}//FlagPhotons()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::Sigma2(Double_t ckovTh, Double_t ckovPh, Double_t trkTh, Double_t trkPh)const
{
// Analithical calculation of total error (as a sum of localization, geometrical and chromatic errors) on Cerenkov angle for a given Cerenkov photon 
// created by a given MIP. Fromulae according to CERN-EP-2000-058 
// Arguments: Cerenkov and azimuthal angles for Cerenkov photon, [radians]
//            dip and azimuthal angles for MIP taken at the entrance to radiator, [radians]        
//            MIP beta
//   Returns: absolute error on Cerenkov angle, [radians]    
  
  TVector3 v(-999,-999,-999);
  Double_t trkBeta = 1./(TMath::Cos(ckovTh)*fkRadIdx);

  v.SetX(SigLoc (ckovTh,ckovPh,trkTh,trkPh,trkBeta));
  v.SetY(SigGeom(ckovTh,ckovPh,trkTh,trkPh,trkBeta));
  v.SetZ(SigCrom(ckovTh,ckovPh,trkTh,trkPh,trkBeta));

  return v.Mag2();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::SigLoc(Double_t thetaC, Double_t phiC, Double_t thetaM, Double_t phiM, Double_t betaM)const
{
// Analithical calculation of localization error (due to finite segmentation of PC) on Cerenkov angle for a given Cerenkov photon 
// created by a given MIP. Fromulae according to CERN-EP-2000-058 
// Arguments: Cerenkov and azimuthal angles for Cerenkov photon, [radians]
//            dip and azimuthal angles for MIP taken at the entrance to radiator, [radians]        
//            MIP beta
//   Returns: absolute error on Cerenkov angle, [radians]    
  Double_t phiDelta = phiC - phiM;

  Double_t alpha =TMath::Cos(thetaM)-TMath::Tan(thetaC)*TMath::Cos(phiDelta)*TMath::Sin(thetaM);
  Double_t k = 1.-fkRadIdx*fkRadIdx+alpha*alpha/(betaM*betaM);
  if (k<0) return 1e10;

  Double_t mu =TMath::Sin(thetaM)*TMath::Sin(phiM)+TMath::Tan(thetaC)*(TMath::Cos(thetaM)*TMath::Cos(phiDelta)*TMath::Sin(phiM)+TMath::Sin(phiDelta)*TMath::Cos(phiM));
  Double_t e  =TMath::Sin(thetaM)*TMath::Cos(phiM)+TMath::Tan(thetaC)*(TMath::Cos(thetaM)*TMath::Cos(phiDelta)*TMath::Cos(phiM)-TMath::Sin(phiDelta)*TMath::Sin(phiM));

  Double_t kk = betaM*TMath::Sqrt(k)/(8*alpha);
  Double_t dtdxc = kk*(k*(TMath::Cos(phiDelta)*TMath::Cos(phiM)-TMath::Cos(thetaM)*TMath::Sin(phiDelta)*TMath::Sin(phiM))-(alpha*mu/(betaM*betaM))*TMath::Sin(thetaM)*TMath::Sin(phiDelta));
  Double_t dtdyc = kk*(k*(TMath::Cos(phiDelta)*TMath::Sin(phiM)+TMath::Cos(thetaM)*TMath::Sin(phiDelta)*TMath::Cos(phiM))+(alpha* e/(betaM*betaM))*TMath::Sin(thetaM)*TMath::Sin(phiDelta));

  return  TMath::Sqrt(0.2*0.2*dtdxc*dtdxc + 0.25*0.25*dtdyc*dtdyc);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::SigCrom(Double_t thetaC, Double_t phiC, Double_t thetaM, Double_t phiM, Double_t betaM)const
{
// Analithical calculation of chromatic error (due to lack of knowledge of Cerenkov photon energy) on Cerenkov angle for a given Cerenkov photon 
// created by a given MIP. Fromulae according to CERN-EP-2000-058 
// Arguments: Cerenkov and azimuthal angles for Cerenkov photon, [radians]
//            dip and azimuthal angles for MIP taken at the entrance to radiator, [radians]        
//            MIP beta
//   Returns: absolute error on Cerenkov angle, [radians]    
  Double_t phiDelta = phiC - phiM;
  Double_t alpha =TMath::Cos(thetaM)-TMath::Tan(thetaC)*TMath::Cos(phiDelta)*TMath::Sin(thetaM);

  Double_t dtdn = TMath::Cos(thetaM)*fkRadIdx*betaM*betaM/(alpha*TMath::Tan(thetaC));
            
  Double_t f = 0.00928*(7.75-5.635)/TMath::Sqrt(12.);

  return f*dtdn;
}//SigCrom()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Double_t AliRICHRecon::SigGeom(Double_t thetaC, Double_t phiC, Double_t thetaM, Double_t phiM, Double_t betaM)const
{
// Analithical calculation of geometric error (due to lack of knowledge of creation point in radiator) on Cerenkov angle for a given Cerenkov photon 
// created by a given MIP. Formulae according to CERN-EP-2000-058 
// Arguments: Cerenkov and azimuthal angles for Cerenkov photon, [radians]
//            dip and azimuthal angles for MIP taken at the entrance to radiator, [radians]        
//            MIP beta
//   Returns: absolute error on Cerenkov angle, [radians]    

  Double_t phiDelta = phiC - phiM;
  Double_t alpha =TMath::Cos(thetaM)-TMath::Tan(thetaC)*TMath::Cos(phiDelta)*TMath::Sin(thetaM);

  Double_t k = 1.-fkRadIdx*fkRadIdx+alpha*alpha/(betaM*betaM);
  if (k<0) return 1e10;

  Double_t eTr = 0.5*1.5*betaM*TMath::Sqrt(k)/(8*alpha);
  Double_t lambda = 1.-TMath::Sin(thetaM)*TMath::Sin(thetaM)*TMath::Sin(phiC)*TMath::Sin(phiC);

  Double_t c = 1./(1.+ eTr*k/(alpha*alpha*TMath::Cos(thetaC)*TMath::Cos(thetaC)));
  Double_t i = betaM*TMath::Tan(thetaC)*lambda*TMath::Power(k,1.5);
  Double_t ii = 1.+eTr*betaM*i;

  Double_t err = c * (i/(alpha*alpha*8) +  ii*(1.-lambda) / ( alpha*alpha*8*betaM*(1.+eTr)) );
  Double_t trErr = 1.5/(TMath::Sqrt(12.)*TMath::Cos(thetaM));

  return trErr*err;
}//SigGeom()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
