#ifndef STARTVERTEX_H
#define STARTVERTEX_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */



#include <AliESD.h>

//___________________________________________
class AliSTARTvertex   : public TObject {


////////////////////////////////////////////////////////////////////////
 private:
  Int_t fZposition;        // Z position of vertex (mm)
 AliRunLoader* fRunLoader;
 public:
    AliSTARTvertex() {}
    AliSTARTvertex(Int_t *);
    void Reconstruct(AliESD *pESD);
    Int_t GetVertex();
    virtual ~AliSTARTvertex() {}
    void Set(Int_t);

    ClassDef(AliSTARTvertex,1)  //Reconstructive vertex (Header) object 
};
inline Int_t AliSTARTvertex::GetVertex(){return fZposition;}
inline void AliSTARTvertex::Set(Int_t Z_position)
  {fZposition=Z_position;}
#endif
