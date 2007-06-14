// -*- mode: C++ -*- 
#ifndef ALIESDRUN_H
#define ALIESDRUN_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                     Implementation Class AliESDRun
//   Run by run data
//   for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------

#include <TObject.h>

class AliESDVertex;

class AliESDRun: public TObject {
public:

  AliESDRun();
  AliESDRun(const AliESDRun& esd);
  AliESDRun& operator=(const AliESDRun& esd);


  Int_t   GetRunNumber() const {return fRunNumber;}
  void    SetRunNumber(Int_t n) {fRunNumber=n;}
  void    SetMagneticField(Float_t mf){fMagneticField = mf;}
  Float_t GetMagneticField() const {return fMagneticField;}
  Int_t   GetPeriodNumber() const {return fPeriodNumber;}
  void    SetPeriodNumber(Int_t n) {fPeriodNumber=n;}
  void    Reset();
  void    Print(const Option_t *opt=0) const;
  void SetDiamond(const AliESDVertex *vertex);


  Float_t GetDiamondX() const {return fDiamondXY[0];}
  Float_t GetDiamondY() const {return fDiamondXY[1];}
  Float_t GetSigma2DiamondX() const {return fDiamondCovXY[0];}
  Float_t GetSigma2DiamondY() const {return fDiamondCovXY[2];}
  void GetDiamondCovXY(Float_t cov[3]) const {
    for(Int_t i=0;i<3;i++) cov[i]=fDiamondCovXY[i]; return;
  }
private:
  Int_t        fRunNumber;       // Run Number
  Int_t        fPeriodNumber;    // PeriodNumber
  Int_t        fRecoVersion;     // Version of reconstruction 
  Float_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF
  Float_t      fDiamondXY[2];    // Interaction diamond (x,y) in RUN
  Float_t      fDiamondCovXY[3]; // Interaction diamond covariance (x,y) in RUN

  ClassDef(AliESDRun,1)
};

#endif 
