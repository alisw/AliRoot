#ifndef ALIPMDRECHIT_H
#define ALIPMDRECHIT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
//-----------------------------------------------------//
//                                                     //
//  Date   : February 26 2006                          //
//                                                     //
//  Store cellhits associated to a cluster             //
//                                                     //
//-----------------------------------------------------//
// Author - B.K. Nandi
//
#include "TObject.h"
class TClonesArray;

class AliPMDrechit : public TObject
{
 public:
  AliPMDrechit();
  AliPMDrechit(Int_t cellx, Int_t celly);
  AliPMDrechit(AliPMDrechit *pmdrechit);
  AliPMDrechit (const AliPMDrechit &alipmdrechit);  // copy constructor
  AliPMDrechit &operator=(const AliPMDrechit &alipmdrechit); // assignment op

  virtual ~AliPMDrechit();

  Int_t   GetCellX() const;
  Int_t   GetCellY() const;
  
 protected:
  Int_t   fXcell;         // x-position of the cell
  Int_t   fYcell;         // y-position of the cell
  
  ClassDef(AliPMDrechit,2) // To keep cell hit information
};

#endif
