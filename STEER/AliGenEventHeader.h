#ifndef ALIGENEVENTHEADER_H
#define ALIGENEVENTHEADER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TNamed.h>
#include <TArrayF.h>

// Event header base class for generator. 
// Stores as a minimum the date, run number, event number, number of particles produced  
// and the impact parameter
// Author: andreas.morsch@cern.ch

class AliGenEventHeader : public TNamed
{
 public:

  AliGenEventHeader(const char* name);
  AliGenEventHeader();
  virtual ~AliGenEventHeader() {}
  // Getters
  virtual Int_t           NProduced()         {return fNProduced;}
  virtual Float_t         ImpactParameter()   {return fImpactParameter;}
  virtual void            PrimaryVertex(TArrayF &o) const;
  
  // Setters
  virtual void   SetNProduced(Int_t nprod)         {fNProduced=nprod;}
  virtual void   SetImpactParameter(Float_t b)     {fImpactParameter=b;}
  virtual void   SetPrimaryVertex(const TArrayF &o);
  
protected:
  Int_t     fNProduced;                 // Number stable or undecayed particles
  Float_t   fImpactParameter;           // Impact Parameter
  TArrayF   fVertex;                    // Primary Vertex Position
  ClassDef(AliGenEventHeader,1)         // Event header for primary event
};

#endif
