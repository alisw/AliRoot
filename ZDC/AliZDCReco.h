#ifndef ALIZDCRECO_H
#define ALIZDCRECO_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//  Classe for ZDC RecPoints                  //
////////////////////////////////////////////////

#include "TObject.h"

class AliZDCReco : public TObject {

public:
  AliZDCReco() {}
  AliZDCReco(Float_t ezn, Float_t ezp, Float_t ezdc, Float_t ezem, Int_t detspn, 
             Int_t detspp, Int_t trspn, Int_t trspp, Int_t trsp, Int_t part, Float_t b);
  AliZDCReco(AliZDCReco* oldreco) {*this=*oldreco;}
  virtual ~AliZDCReco() {}

  // Getters 
  virtual Float_t GetZNenergy()      {return fZNenergy;}
  virtual Float_t GetZPenergy()      {return fZPenergy;}
  virtual Float_t GetZDCenergy()     {return fZDCenergy;}
  virtual Float_t GetZEMenergy()     {return fZEMenergy;}
  virtual Int_t   GetNDetSpecN()     {return fNDetSpecN;}
  virtual Int_t   GetNDetSpecP()     {return fNDetSpecP;}
  virtual Int_t   GetNTrueSpecN()    {return fNTrueSpecN;}
  virtual Int_t   GetNTrueSpecP()    {return fNTrueSpecP;}
  virtual Int_t   GetNTrueSpec()     {return fNTrueSpec;}
  virtual Int_t   GetNPart()         {return fNPart;}
  virtual Float_t GetImpPar()        {return fImpPar;}

  // Data members
  Float_t fZNenergy;	// Energy detected in neutron ZDC
  Float_t fZPenergy;	// Energy detected in proton ZDC
  Float_t fZDCenergy;	// Total hadronic energy detcted in ZDCs
  Float_t fZEMenergy;	// Energy detected in EM ZDC
  Int_t	  fNDetSpecN;	// Number of spectator neutrons detected
  Int_t	  fNDetSpecP;	// Number of spectator protons detected
  Int_t	  fNTrueSpecN;  // Estimate of the number of spectator neutrons generated
  Int_t	  fNTrueSpecP;  // Estimate of the number of spectator protons generated
  Int_t	  fNTrueSpec ;  // Estimate of the total number of spectators
  Int_t	  fNPart;	// Estimate of the number of participants for 1 nucleus
  Float_t fImpPar;	// Estimate of the impact parameter

  // Print method
  virtual void Print(Option_t *) const {
     printf("	---   Reconstruction -> EZN = %f TeV, EZP = %f TeV, EZDC = %f TeV,"
            " EZEM = %f GeV \n 		NDetSpecN = %d, NDetSpecP = %d, Nspecn = %d,"
	    " Nspecp = %d, Npart = %d, b = %f fm.\n ", 
	    fZNenergy,fZPenergy,fZDCenergy,fZEMenergy,fNDetSpecN,fNDetSpecP,
	    fNTrueSpecN,fNTrueSpecP,fNPart,fImpPar);
  }

  ClassDef(AliZDCReco,1)  // RecPoints for the Zero Degree Calorimeters
};
 
#endif
