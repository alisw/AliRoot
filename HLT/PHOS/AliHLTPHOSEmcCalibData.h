//-*- Mode: C++ -*-
// $Id$


#ifndef ALIHLTPHOSEMCCALIBDATA_H
#define ALIHLTPHOSEMCCALIBDATA_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////
//  class for EMC calibration                 //
////////////////////////////////////////////////


#include "TNamed.h"
#include "AliHLTPHOSConstants.h"


using namespace PhosHLTConst;

class AliHLTPHOSEmcCalibData: public TNamed {

 public:
  AliHLTPHOSEmcCalibData();
  AliHLTPHOSEmcCalibData(const char* name);
  AliHLTPHOSEmcCalibData(const AliHLTPHOSEmcCalibData &calibda);
  AliHLTPHOSEmcCalibData& operator= (const AliHLTPHOSEmcCalibData &calibda);
  virtual ~AliHLTPHOSEmcCalibData();
  void Reset();
  virtual void Print(Option_t *option = "") const; 
  Float_t GetADCchannelEnergy(Int_t module, Int_t column, Int_t row, Int_t gain) const;
  Float_t GetADCpedestalEmcMeasured(Int_t module, Int_t column, Int_t row, Int_t gain) const;
  void SetADCchannelEnergy(Int_t module, Int_t column, Int_t row, Int_t gain, Float_t value);
  void SetADCpedestalEmcMeasured(Int_t module, Int_t column, Int_t row, Int_t gain, Float_t value);
  void MakeADCpedestalCorrectionTable(); 
 protected:
  Float_t  fADCchannelEnergy[N_MODULES][N_XCOLUMNS_MOD][N_ZROWS_MOD][N_GAINS] ;  /**<width of one EMC ADC channel in GeV*/
  Float_t  fADCpedestalEmcMeasured[N_MODULES][N_XCOLUMNS_MOD][N_ZROWS_MOD][N_GAINS] ; /**<value of the EMC ADC pedestal measured from calibration run*/
  Int_t    fADCpedestalAltroReg[N_MODULES][N_XCOLUMNS_MOD][N_ZROWS_MOD][N_GAINS] ; /**<value of the EMC ADC pedestal subtraction values stored in the ALTRO registers*/
  Float_t  fADCpedestalCorrectionTable[N_MODULES][N_XCOLUMNS_MOD][N_ZROWS_MOD][N_GAINS] ; /**<value of the EMC ADC pedestal values to be subtracted form the decoed cahnnel data (= fADCpedestalEmcMeasured - fADCpedestalAltroReg)*/

  ClassDef(AliHLTPHOSEmcCalibData,1)    // PHOS EMC calibration data
};

#endif
