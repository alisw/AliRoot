#ifndef ALICRTV0_H
#define ALICRTV0_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
////////////////////////////////////////////////
//  Manager class for detector: CRTv0         //
////////////////////////////////////////////////

#include "AliCRT.h"

class AliCRTv0 : public AliCRT {
public:
  AliCRTv0();
  AliCRTv0(const char *name, const char *title);
  AliCRTv0(const AliCRTv0& crt);
  virtual ~AliCRTv0();

  AliCRTv0& operator=(const AliCRTv0& crt);

  //  virtual TString Version();
  //  virtual Int_t   IsVersion() const;

  virtual void    CreateGeometry();
  virtual void    BuildGeometry();
  virtual void    DrawDetector();

protected:
  virtual void CreateMolasse() {}
  virtual void CreateShafts() {}

private: 
  ClassDef(AliCRTv0,1) // Cosmic Ray Trigger (ACORDE).
};

// inline TString AliCRTv0::Version()
// { return TString("v0"); }

// inline Int_t AliCRTv0::IsVersion() const
// { return 0; }
#endif // ALICRTV0_H
