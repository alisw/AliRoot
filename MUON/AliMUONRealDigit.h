#ifndef ALIMUONREALDIGIT_H
#define ALIMUONREALDIGIT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup base
/// \class AliMUONRealDigit
/// \brief Implementation of AliMUONVDigit for real (i.e. not simulated) digits
/// 
// author Laurent Aphecetche

#ifndef ALIMUONVDIGIT_H
#  include "AliMUONVDigit.h"
#endif

class AliMUONRealDigit : public AliMUONVDigit
{
public:
  AliMUONRealDigit();
  AliMUONRealDigit(Int_t detElemId, Int_t manuId, Int_t manuChannel, Int_t cathode);
  virtual ~AliMUONRealDigit();
  
  /// Return the detection element this digit belongs to
  virtual Int_t DetElemId() const { return AliMUONVDigit::DetElemId(GetUniqueID()); }
  virtual Int_t PadX() const;
  virtual Int_t PadY() const;
  /// Return the cathode this digit belongs to
  virtual Int_t Cathode() const { return AliMUONVDigit::Cathode(GetUniqueID()); }
  
  /// Charge (should be non zero if calibrated)
  virtual Float_t Charge() const { return fCharge; }
  
  /// ADC value (it is the real raw adc value, not pedestal subtracted)
  virtual Int_t ADC() const { return fADC; }
  
  /// Return the manu chip this digit belongs to
  virtual Int_t ManuId() const  { return AliMUONVDigit::ManuId(GetUniqueID()); }
  /// Return the manu channel this digits is connected to
  virtual Int_t ManuChannel() const  { return AliMUONVDigit::ManuChannel(GetUniqueID()); }
  
  /// Whether this digit's charge has saturated the electronics
  virtual Bool_t IsSaturated() const { return TestBit(kSaturated); }
  /// Set the saturation status
  virtual void Saturated(Bool_t saturated=kTRUE) { SetBit(kSaturated,saturated); }
  
  /// We have no idea whether a real digit is noise only or not ;-)
  virtual Bool_t IsNoiseOnly() const { return kFALSE; }
  
  /// Again, this is for simulation only
  virtual Bool_t IsEfficiencyApplied() const { return kFALSE; }
  
  /// Whether this digit is calibrated or not
  virtual Bool_t IsCalibrated() const { return TestBit(kCalibrated); }
  /// Set the calibration status
  virtual void Calibrated(Bool_t value) { SetBit(kCalibrated,value); }
  
  /// Whether this digit is part of a cluster or something else
  virtual Bool_t IsUsed() const { return TestBit(kUsed); }
  /// Set the used status
  virtual void Used(Bool_t value) { SetBit(kUsed,value); }
  
  /// The status map (i.e. the status of the neighbours) of this digit
  virtual UInt_t StatusMap() const { return fStatusMap; }
  /// Set the status map value
  virtual void SetStatusMap(UInt_t statusMap) { fStatusMap = statusMap; }
  
  /// Whether this digit is real or virtual (a clustering detail...)
  virtual Bool_t IsVirtual() const { return TestBit(kVirtual); }
  /// Set the virtual status
  virtual void SetVirtual(Bool_t value) { SetBit(kVirtual,value); }

  /// Set the ADC value (should be between 0 and 4095)
  virtual void SetADC(Int_t adc) { fADC = adc; }
  virtual void SetPadXY(Int_t padx, Int_t pady);
  /// Set the charge
  virtual void SetCharge(Float_t q) { fCharge=q; }
  
  virtual Bool_t MergeWith(const AliMUONVDigit& other);
  
  /// No, this digit is not a Monte-Carlo one, sorry.
  virtual Bool_t HasMCInformation() const { return kFALSE; }
  
private:
  Float_t fCharge; ///< Charge on pad  
  UInt_t fPadXY; ///< Pad number along x and Y (packed)
  Int_t fADC; ///< Raw ADC value
  UInt_t fStatusMap; ///< Neighbouring pad status (whether ped, gains, hv were ok or not)
  
  enum EStatusBit 
  {
    kSaturated = BIT(20),
    kUsed = BIT(21),
    kCalibrated = BIT(22),
    kVirtual = BIT(23)
  };
  
  ClassDef(AliMUONRealDigit,1) // Implementation of AliMUONVDigit
};

#endif
