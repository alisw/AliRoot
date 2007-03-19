#ifndef ALIMUONCLUSTERINPUT_H
#define ALIMUONCLUSTERINPUT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id $*/
// Revision of includes 07/04/2006

/// \ingroup rec
/// \class AliMUONClusterInput
/// \brief Global data service for hit reconstruction

#include <TObject.h>
#include <TClonesArray.h> // needed for inline function Digit

class AliMUONDigit;
class AliMUONRawCluster;
class AliMUONMathieson;
class AliMUONGeometryTransformer;
class AliMUONSegmentation;
class AliMUONGeometrySegmentation;

class TMinuit;

class AliMUONClusterInput : public TObject 
{
 public:
    virtual ~AliMUONClusterInput();
    static AliMUONClusterInput* Instance();
//  Configuration
    void SetDigits(Int_t chamber, Int_t idDE, TClonesArray* dig1, TClonesArray* dig2);
    void SetDigits(Int_t chamber, Int_t idDE, TClonesArray* dig);
    void SetCluster(AliMUONRawCluster* cluster);
// Access functions
    /// Return the current chamber number
    Int_t Chamber() const {return fChamber;}
    /// Return i-th digit for given cath
    AliMUONDigit* Digit(Int_t cath, Int_t i) const {return (AliMUONDigit*) (fDigits[cath]->UncheckedAt(i));}
    /// Return the array of digits for given cathod 
    TClonesArray* Digits(Int_t cath) const {return fDigits[cath];}
    /// Return number of digits for given cathod 
    Int_t NDigits(Int_t cath) const {return fNDigits[cath];}
    /// Return geometry segmentation for given cathod 
    AliMUONGeometrySegmentation* Segmentation2(Int_t cath) const {return fSegmentation2[cath];}

    /// Return Mathieson    
    AliMUONMathieson* Mathieson() const {return fgMathieson;}    
    /// Return charge correlation
    Float_t ChargeCorrel() const {return fChargeCorrel;}    
    /// Return detection elt id
    Int_t DetElemId() const {return fDetElemId;}

// Fitting    
    /// Return the fitter
    TMinuit*      Fitter() const {return fgMinuit;}
// Current cluster information    
    /// Return the total charge for given cathod 
    Float_t       TotalCharge(Int_t cath) const {return fChargeTot[cath];}
    /// Return the charge for the given cluster and cathod
    Float_t       Charge(Int_t dig, Int_t cath) const {return fCharge[dig][cath];}
    /// Return the x-position for the given cluster and cathod
    Int_t         Ix(Int_t dig, Int_t cath) const {return fix[dig][cath];}
    /// Return the y-position for the given cluster and cathod
    Int_t         Iy(Int_t dig, Int_t cath) const {return fiy[dig][cath];}
    /// Return the cluster multiplicity for given cathod 
    Int_t         Nmul(Int_t cath) const {return fNmul[cath];}

//  Helpers for Fit     
    Float_t DiscrChargeS1(Int_t i,Double_t *par);
    Float_t DiscrChargeCombiS1(Int_t i,Double_t *par, Int_t cath);
    Float_t DiscrChargeS2(Int_t i,Double_t *par);
    Float_t DiscrChargeCombiS2(Int_t i,Double_t *par, Int_t cath);
// 
 private:
    /// Not implemented
    AliMUONClusterInput();
    /// Not implemented
    AliMUONClusterInput(const AliMUONClusterInput& clusterInput);
    /// Not implemented
    AliMUONClusterInput & operator = (const AliMUONClusterInput& rhs);

    static AliMUONClusterInput*  fgClusterInput;  //!< singleton instance
    static AliMUONMathieson*     fgMathieson;     //!< Mathieson
    static AliMUONGeometryTransformer*  fgTransformer;  //!< Geometry transformer
    static AliMUONSegmentation*  fgSegmentation;  //!< Segmentation

    // Digits
    TClonesArray*        fDigits[2];       //!< Array of pointers to digits
    Int_t                fNDigits[2];      //!< Number of digits
    AliMUONGeometrySegmentation*     fSegmentation2[2]; //!< Geometry Segmentation per cathode

    Int_t                fNseg;            //!< number of cathode planes
    Int_t                fChamber;         //!< Current chamber number
    
    // Current cluster
    AliMUONRawCluster*   fCluster;         //!< current cluster
    Int_t                fNmul[2];         //!< current cluster multiplicity
    // Digits contribuing to current cluster
    Int_t                fix[500][2];      //!< List of x-positions for current cluster
    Int_t                fiy[500][2];      //!< List of y-positions for current cluster
    Float_t              fCharge[500][2];  //!< List of charges for current cluster
    Int_t                fChargeTot[2];    //!< Total charge
    Float_t              fQtot[2];         //!< Total charge
    Float_t              fZ;               //!< Current z-position
    Float_t              fChargeCorrel;    //!< charge correlation 
    Int_t                fDetElemId;       //!< detection elt id    

    // Fitter
    static TMinuit*      fgMinuit;          //!< Fitter
    ClassDef(AliMUONClusterInput, 0)        // Global data service for hit reconstruction
};
#endif

