#ifndef ALITPCCALPAD_H
#define ALITPCCALPAD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TPC calibration class for parameters which are saved per pad                 //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"

class AliTPCCalROC;
class AliTPCCalDet;
class TObjArray;
class TGraph;

class AliTPCCalPad : public TNamed {
 public:
  enum { kNsec = 72 };
  AliTPCCalPad();
  AliTPCCalPad(const Text_t* name, const Text_t* title);
  AliTPCCalPad(const AliTPCCalPad &c);   
  AliTPCCalPad(TObjArray *arrayROC);
  virtual ~AliTPCCalPad();
  AliTPCCalPad &operator=(const AliTPCCalPad &c);
  virtual void     Copy(TObject &c) const;
  AliTPCCalROC *GetCalROC(Int_t sector) const { return fROC[sector]; };  
  //
  //
  TGraph       *MakeGraph(Int_t type=0, Float_t ratio=0.7);
 protected:
  AliTPCCalROC *fROC[kNsec];                    //  Array of ROC objects which contain the values per pad
  ClassDef(AliTPCCalPad,1)                      //  TPC calibration class for parameters which are saved per pad
};

#endif
