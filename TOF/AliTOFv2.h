//_________________________________________________________________________
// Implementation version v2 of TOF Manager class
// HOLES FOR PHOS AND HMPID (RICH) DETECTOR
//   
//*-- 
//*-- Authors: Pierella, Seganti, Vicinanza (Bologna and Salerno University)

#ifndef ALITOFv2_H
#define ALITOFv2_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */
 
#include "AliTOF.h"
 
 
class AliTOFv2 : public AliTOF {
 
public:
  AliTOFv2();
  AliTOFv2(const char *name, const char *title);
  virtual ~AliTOFv2(void) ;
  virtual void   BuildGeometry();
  virtual void   CreateGeometry();
  virtual void   CreateMaterials();
  virtual void   Init();
  virtual Int_t  IsVersion() const {return 2;}
  virtual void   TOFpc(Float_t xtof,Float_t ytof,Float_t zlenC,Float_t zlenB,
                       Float_t zlenA,Float_t ztof0);
  virtual void   StepManager();
  virtual void   DrawModule() const;

private:
  Int_t fIdFTOA; // FTOA volume identifier (outer plate A)
  Int_t fIdFTOB; // FTOB volume identifier (outer plate B)
  Int_t fIdFTOC; // FTOC volume identifier (outer plate C)
  Int_t fIdFLTA; // FLTA volume identifier (inner plate A)
  Int_t fIdFLTB; // FLTB volume identifier (inner plate B)
  Int_t fIdFLTC; // FLTC volume identifier (inner plate C)
  
   ClassDef(AliTOFv2,1)  //Time Of Flight version 2
};
 
#endif /* ALITOFv2_H  */
