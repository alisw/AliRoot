#ifndef ALITPCCLUSTERKR_H
#define ALITPCCLUSTERKR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTPCclusterKr.h,v 1.8 2008/01/22 16:07:15 matyja Exp $ */

//-------------------------------------------------------
//                    TPC Kr Cluster Class
//
//   Origin: Adam Matyja, INP PAN, adam.matyja@ifj.edu.pl
//-------------------------------------------------------

#include "AliCluster.h"
#include "TObjArray.h"
#include "AliTPCvtpr.h"


//_____________________________________________________________________________
class AliTPCclusterKr: public AliCluster{
public:
  AliTPCclusterKr();
  AliTPCclusterKr(const AliTPCclusterKr & param);//copy constructor
  AliTPCclusterKr &operator = (const AliTPCclusterKr & param); 
  virtual ~AliTPCclusterKr();

  virtual void SetCenter();//set center of the cluster weighted by charge

  virtual void SetMax(AliTPCvtpr q){fMax=q;}//set values of max. in cluster
  virtual void SetADCcluster(Int_t q){fADCcluster=q;}
  virtual void SetSec(Short_t q){fSec=q;}
  virtual void SetNPads(Short_t q){fNPads=q;}
  virtual void SetNRows(Short_t q){fNRows=q;}
  virtual void SetSize(){fSize=fCluster->GetEntriesFast();}
  virtual void SetCenterX(Double_t q){fCenterX=q;}
  virtual void SetCenterY(Double_t q){fCenterY=q;}
  virtual void SetCenterT(Double_t q){fCenterT=q;}
  //void AddDigitToCluster(AliTPCvtpr *q){fCluster.push_back(q);}
  virtual void AddDigitToCluster(AliTPCvtpr *q){
    fCluster->AddLast(q);
    fCluster->Compress();
  }

  AliTPCvtpr GetMax() const {return fMax;}
  Int_t GetADCcluster() const {return  fADCcluster;}
  Short_t GetSec() const {return fSec;}
  Short_t GetNPads() const {return fNPads;}
  Short_t GetNRows() const {return fNRows;}
  Short_t GetSize() const {return fSize;}
  Double_t GetCenterX() const {return fCenterX;}
  Double_t GetCenterY() const {return fCenterY;}
  Double_t GetCenterT() const {return fCenterT;}
  AliTPCvtpr *GetDigitFromCluster(Int_t i) const {return (AliTPCvtpr*)fCluster->At(i);}

private:
  AliTPCvtpr fMax;//max (ADC,timebin,pad,row) in cluster
  Int_t fADCcluster; //ADC of cluster
  Short_t fSec;  //sector of the cluster
  Short_t fNPads; //number of pads in cluster
  Short_t fNRows; //number of rows in cluster
  Short_t fSize; //size of vector
  Double_t fCenterX;// X coordinate of the cluster center in cm
  Double_t fCenterY;// Y coordinate of the cluster center in cm
  Double_t fCenterT;// time coordinate of the cluster center in timebins
  //std::vector< AliTPCvtpr*> fCluster;//cluster contents(adc,nt,np,nr)
  TObjArray *fCluster;//cluster contents(adc,nt,np,nr)


  ClassDef(AliTPCclusterKr,4)  // Time Projection Chamber Kr clusters
};


#endif


