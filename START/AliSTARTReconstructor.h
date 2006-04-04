#ifndef ALISTARTRECONSTRUCTOR_H
#define ALISTARTRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliReconstructor.h"
#include "AliSTARTdigit.h"
class AliRunLoader;

class AliSTARTReconstructor: public AliReconstructor {
public:
  AliSTARTReconstructor(): AliReconstructor() {};
  virtual ~AliSTARTReconstructor() {};
  virtual  void   ConvertDigits( AliRawReader* rawReader, TTree* fdigits) const;
  virtual  void   Reconstruct(TTree* fdigits, TTree * frecpoints) const ;
 
  virtual  void   Reconstruct(AliRunLoader* , AliRawReader*   ) const {};
  virtual  void   Reconstruct(AliRawReader* ) const {};
  virtual  void   Reconstruct(AliRawReader* , TTree*) const {};
  virtual  void   Reconstruct(AliRunLoader* ) const {};
  
  virtual void         FillESD(AliRunLoader* runLoader, AliESD* esd) const;
  virtual void         FillESD(AliRunLoader* , AliRawReader*, AliESD* ) const  {};
  virtual void         FillESD(  AliRawReader*,  TTree*, AliESD* ) const  {};
  virtual void         FillESD( TTree*,  TTree*, AliESD* ) const  {};
  virtual Bool_t       HasLocalReconstruction() const {return kTRUE;};
  virtual Bool_t       HasDigitConversion() const {return kTRUE;};
 public:
 
  AliSTARTdigit *fdigits   ; // digits
  Float_t fZposition; // vertex position

  ClassDef(AliSTARTReconstructor, 0)   // class for the START reconstruction


};

#endif
