#ifndef ALIL3TRANSFORM_H
#define ALIL3TRANSFORM_H

#include "AliL3RootTypes.h"

class AliL3Transform {

 private:
  
  static Double_t fBField;
  static Int_t fNTimeBins;
  static Int_t fNRowLow;
  static Int_t fNRowUp;
  static Int_t fNSectorLow;
  static Int_t fNSectorUp;
  static Double_t fPadPitchWidthLow;
  static Double_t fPadPitchWidthUp;
  static Double_t fZWidth;
  static Double_t fZSigma;
  static Int_t fNSector;
  static Int_t fNSlice;
  static Int_t fNRow;
  static Double_t fPi;
  static Double_t fNRotShift;
  static Double_t fZLength;
  static Double_t fZOffset;
  static Int_t fNPads[176]; //fill this following Init
  static Double_t fX[176];  //fill this following Init
  static Int_t fVersion; //flags which version one is using
  static Int_t fRows[6][2];
  static Int_t fNRows[6];
  static Int_t fNPatches;
  
  static Double_t fDiffT; //Transversal diffusion constant
  static Double_t fDiffL; //Longitudinal diffusion constant
  static Double_t fAnodeWireSpacing; 
  static Double_t fInnerPadLength;
  static Double_t fOuterPadLength;
  static Double_t fInnerPRFSigma;
  static Double_t fOuterPRFSigma;
  static Double_t fTimeSigma; //Minimal longitudinal width

 public:
  
  static void Init(const Char_t* path); //new init for all AliRoot versions
  
  static void SetBField(Double_t f) {fBField = f;}
  static Int_t GetFirstRow(Int_t patch) {return fRows[patch][0];}
  static Int_t GetLastRow(Int_t patch) {return fRows[patch][1];}
  static Int_t GetNRows(Int_t patch) {return fNRows[patch];}
  static Int_t GetNPatches() {return fNPatches;}
  static Double_t GetBField() {return fBField;}
  static Double_t Pi() {return fPi;}
  static Int_t GetVersion(){return fVersion;}
  static Double_t GetPadPitchWidthLow() {return fPadPitchWidthLow;}
  static Double_t GetPadPitchWidthUp() {return fPadPitchWidthUp;}
  static Double_t GetPadPitchWidth(Int_t patch) {return patch < 2 ? fPadPitchWidthLow : fPadPitchWidthUp;}  
  static Double_t GetZWidth() {return fZWidth;}
  static Double_t GetZLength() {return fZLength;}
  static Double_t GetZOffset() {return fZOffset;}
  static Double_t GetDiffT() {return fDiffT;}
  static Double_t GetDiffL() {return fDiffL;}
  static Double_t GetAnodeWireSpacing() {return fAnodeWireSpacing;}
  static Double_t GetPadLength(Int_t patch) {return patch < 2 ? fInnerPadLength : fOuterPadLength;}
  static Double_t GetPRFSigma(Int_t patch) {return patch < 2 ? fInnerPRFSigma : fOuterPRFSigma;}
  static Double_t GetTimeSigma() {return fTimeSigma;}
  static Int_t GetNSectorLow() {return fNSectorLow;}
  static Int_t GetNSectorUp() {return fNSectorUp;}
  
  static Bool_t Slice2Sector(Int_t slice, Int_t slicerow, Int_t &sector, Int_t &row);
  static Bool_t Sector2Slice(Int_t &slice, Int_t sector);
  static Bool_t Sector2Slice(Int_t &slice, Int_t &slicerow, Int_t sector, Int_t row);

  static Int_t GetNPads(Int_t row){return (row<fNRow)?fNPads[row]:0;}
  static Int_t GetNTimeBins(){return fNTimeBins;}
  static Double_t Row2X(Int_t slicerow);
  static Double_t GetMaxY(Int_t slicerow);
  static Double_t GetEta(Float_t *xyz);
  static Double_t GetEta(Int_t row, Int_t pad, Int_t time);
  static Double_t GetPhi(Float_t *xyz);

  static void XYZtoRPhiEta(Float_t *rpe, Float_t *xyz);
  static void Local2Global(Float_t *xyz, Int_t slice);
  static void Local2GlobalAngle(Float_t *angle, Int_t slice);
  static void Global2LocalAngle(Float_t *angle, Int_t slice);

  static void Raw2Local(Float_t *xyz, Int_t sector, Int_t row, Float_t pad, Float_t time);
  static void Local2Global(Float_t *xyz, Int_t sector, Int_t row);
  static void Global2Local(Float_t *xyz, Int_t sector, Bool_t isSlice=kFALSE);
  static void Raw2Global(Float_t *xyz, Int_t sector, Int_t row, Float_t pad, Float_t time);
  static void Local2Raw(Float_t *xyz, Int_t sector, Int_t row);
  static void Global2Raw(Float_t *xyz, Int_t sector, Int_t row);
  
  ClassDef(AliL3Transform,1) //Transformation class for ALICE TPC
};
#endif





