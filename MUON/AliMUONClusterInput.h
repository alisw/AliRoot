#ifndef ALIMUONCLUSTERINPUT_H
#define ALIMUONCLUSTERINPUT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id $*/
// Revision of includes 07/05/2004

#include <TObject.h>
#include <TClonesArray.h> // needed for inline function Digit

class TMinuit;

class AliMUONDigit;
class AliMUONRawCluster;
class AliSegmentation;
class AliMUONMathieson;

class AliMUONClusterInput : public TObject 
{
 public:
    virtual ~AliMUONClusterInput();
    static AliMUONClusterInput* Instance();
//  Configuration
    void SetDigits(Int_t chamber, TClonesArray* dig1, TClonesArray* dig2);
    void SetDigits(Int_t chamber, TClonesArray* dig);
    void SetCluster(AliMUONRawCluster* cluster);
// Access functions
    Int_t Chamber() const {return fChamber;}
    AliMUONDigit* Digit(Int_t cath, Int_t i) const {return (AliMUONDigit*) (fDigits[cath]->UncheckedAt(i));}
    TClonesArray* Digits(Int_t cath) const {return fDigits[cath];}
    Int_t NDigits(Int_t cath) const {return fNDigits[cath];}
    AliSegmentation* Segmentation(Int_t cath) const {return fSegmentation[cath];}
    AliMUONMathieson* Mathieson() const {return fgMathieson;}    
    Float_t ChargeCorrel() const {return fChargeCorrel;}    

// Fitting    
    TMinuit*      Fitter() const {return fgMinuit;}
// Current cluster information    
    Float_t       TotalCharge(Int_t cath) const {return fChargeTot[cath];}
    Float_t       Charge(Int_t dig, Int_t cath) const {return fCharge[dig][cath];}
    Int_t         Ix(Int_t dig, Int_t cath) const {return fix[dig][cath];}
    Int_t         Iy(Int_t dig, Int_t cath) const {return fiy[dig][cath];}
    Int_t         Nmul(Int_t cath) const {return fNmul[cath];}
//  Helpers for Fit     
    Float_t DiscrChargeS1(Int_t i,Double_t *par);
    Float_t DiscrChargeCombiS1(Int_t i,Double_t *par, Int_t cath);
    Float_t DiscrChargeS2(Int_t i,Double_t *par);
    Float_t DiscrChargeCombiS2(Int_t i,Double_t *par, Int_t cath);
// 
 protected:
    AliMUONClusterInput();
    AliMUONClusterInput(const AliMUONClusterInput& clusterInput);
    AliMUONClusterInput & operator = (const AliMUONClusterInput& rhs);
 private:
    static AliMUONClusterInput* fgClusterInput; // ! singleton instance
    static AliMUONMathieson*    fgMathieson;     // ! Mathieson

    // Digits
    TClonesArray*        fDigits[2];       // ! Array of pointers to digits
    Int_t                fNDigits[2];      // ! Number of digits
    AliSegmentation*     fSegmentation[2]; // ! Segmentation per cathode
    Int_t                fNseg;            // ! number of cathode planes
    Int_t                fChamber;         // ! Current chamber number
    
    // Current cluster
    AliMUONRawCluster*   fCluster;         // ! current cluster
    Int_t                fNmul[2];         // ! current cluster multiplicity
    // Digits contribuing to current cluster
    Int_t                fix[500][2];      // ! List of x-positions for current cluster
    Int_t                fiy[500][2];      // ! List of y-positions for current cluster
    Float_t              fCharge[500][2];  // ! List of charges for current cluster
    Int_t                fChargeTot[2];    // ! Total charge
    Float_t              fQtot[2];         // ! Total charge
    Float_t              fZ;               // ! Current z-position
    Float_t              fChargeCorrel;    // ! charge correlation 

    // Fitter
    static TMinuit*      fgMinuit;          // ! Fitter
    ClassDef(AliMUONClusterInput, 0)        // Global data service for hit reconstruction
};
#endif

