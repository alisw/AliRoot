#ifndef ALIGENISAJET_H
#define ALIGENISAJET_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

// Implementation of the interface for TIsajet
// Author: 


#include "TString.h" 

#include "AliGenerator.h"
#include "AliGenMC.h"
#include <AliRndm.h>
#include <TString.h>
#include <TArrayI.h>

class TIsajet;
class TArrayI;
class TParticle;
class TClonesArray;


class AliGenIsajet : public AliGenMC { 

 public:
  AliGenIsajet();
  AliGenIsajet(Int_t npart);
  virtual ~AliGenIsajet();

  // 
  
  
  virtual void Init();
  virtual void Generate();

 protected:
  TIsajet * fIsajet;
  Float_t     fKineBias;       // Bias from kinematic selection
  Int_t       fTrials;         // Number of trials
 private:
  
  AliGenIsajet(const AliGenIsajet &Isajet); // Not Implemented
  AliGenIsajet &  operator=(const AliGenIsajet & rhs); // Not Implemented
      
  ClassDef(AliGenIsajet,1) // Interface class for AliIsajet
    
};
#endif
