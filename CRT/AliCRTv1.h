#ifndef ALICRTV1_H
#define ALICRTV1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
////////////////////////////////////////////////
//  Manager class for detector: CRTv1         //
////////////////////////////////////////////////

#include "AliCRTv0.h"

class AliCRTv1 : public AliCRTv0 {
public:
  AliCRTv1();
  AliCRTv1(const char *name, const char *title);
  AliCRTv1(const AliCRTv1& crt);
  virtual ~AliCRTv1();

  AliCRTv1&       operator=(const AliCRTv1& crt);
  virtual TString Version() { return TString("v1"); }
  virtual Int_t   IsVersion() const { return 1; }

  virtual void    AddHit(Int_t track, Int_t *vol, Float_t *hits);
  virtual void    FinishEvent();
  virtual void    ResetHits();
  virtual void    ResetDigits();

  virtual void    CreateMaterials();
  virtual void    CreateGeometry();
  virtual void    Init();
  virtual void    DrawDetector();
  virtual void    StepManager();

protected:
  virtual void    CreateMolasse();
  virtual void    CreateShafts();

private: 
  ClassDef(AliCRTv1, 1)  //Class for CRT, version 1, Shafts outside of AliHALL
};

#endif // ALICRTV1_H
