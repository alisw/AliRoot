#ifndef ALIPMDCLUSTER_H
#define ALIPMDCLUSTER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
//-----------------------------------------------------//
//                                                     //
//  Date   : August 05 2003                            //
//                                                     //
//  Store cluster informations for PMD                 //
//                                                     //
//-----------------------------------------------------//

#include "Rtypes.h"
#include "TObject.h"
class TClonesArray;

class AliPMDcluster : public TObject
{
 public:
  AliPMDcluster();
  AliPMDcluster(Int_t idet, Int_t ismn, Float_t *clusdata);
  AliPMDcluster(AliPMDcluster *pmdcluster) {*this = *pmdcluster;}
  AliPMDcluster (const AliPMDcluster &pmdcluster);  // copy constructor
  AliPMDcluster &operator=(const AliPMDcluster &pmdcluster); // assignment op
  
  virtual ~AliPMDcluster();

  Int_t   GetDetector() const;
  Int_t   GetSMN() const;
  Float_t GetClusX() const;
  Float_t GetClusY() const;
  Float_t GetClusADC() const;
  Float_t GetClusCells() const;
  Float_t GetClusRadius() const;

 protected:

  Int_t   fDet;          // Detector No (0:PRE, 1:CPV)
  Int_t   fSMN;          // Serial Module No.
  Float_t fClusData[5];  // Array containing cluster information
  /*
    fDet         : Det (0:PRE, 1:CPV), fSMN         : SerialModuleNo
    fClusData[0] : Cluster x         , fClusData[1] : Cluster y
    fClusData[2] : Cluster adc       , fClusData[3] : Cluster Cells
    fClusData[4] : Cluster radius
  */
  
  ClassDef(AliPMDcluster,2) // Keep Cluster information
};

#endif
