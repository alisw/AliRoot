#ifndef ALIEMCALV2_H
#define ALIEMCALV2_H
/* Copyright(c) 1998-2004, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                      
         */
/* $Id$ */

//_________________________________________________________________________
// Implementation version v2 of EMCAL Manager class for Shish-Kebab case 
//*--                  
//*-- Author:  Aleksei Pavlinov

// --- ROOT system ---
class TClonesArray;
class TLorentzVector;
class TFile;
class TH1F;

class TBrowser;
class AliEMCALGeometry;

// --- AliRoot header files ---
#include "AliEMCALv1.h"

// for TRD2 case
//#include "TGeant3.h"

class AliEMCALv2 : public AliEMCALv1 {
  
public:

  AliEMCALv2(void) ; 
  AliEMCALv2(const char *name, const char *title="") ;
  // cpy ctor: no implementation yet
  // requested by the Coding Convention
  AliEMCALv2(const AliEMCALv1 & emcal):AliEMCALv1(emcal) {
    Fatal("cpy ctor", "not implemented") ;  }
  virtual ~AliEMCALv2(void) ;
  virtual void  AddHit( Int_t shunt, Int_t primary, Int_t track, Int_t iparent, Float_t ienergy,
			Int_t id, Float_t *hits, Float_t *p);

  virtual void StepManager(void) ;
  virtual void FinishEvent();

  // Gives the version number 
  virtual Int_t  IsVersion(void) const {return 2;}
  virtual const TString Version(void)const {return TString("v2");}
  //  virtual void RemapTrackHitIDs(Int_t *map);
  //virtual void FinishPrimary();
  // virtual void SetTimeCut(Float_t tc){ fTimeCut = tc;}
  // virtual Float_t GetTimeCut(){return fTimeCut;}
  // assignement operator requested by coding convention but not needed  
  AliEMCALv2 & operator = (const AliEMCALv1 & /*rvalue*/){
    Fatal("operator =", "not implemented") ;  
    return *this;}
  // 23-mar-05
  Double_t GetDepositEnergy(int print=1); // *MENU*
  // 30-aug-04
  virtual void Browse(TBrowser* b);
  // drawing
  void DrawCalorimeterCut(const char *name="SMOD", int axis=3, double dcut=1.); // *MENU*
  void DrawSuperModuleCut(const char *name="EMOD", int axis=2, double dcut=0.03, int fill = 6);//  *MENU*
  void DrawTowerCut(const char *name="SCMY", int axis=2, double dcut=0., int fill=1, char *optShad="on");   //  *MENU*
  void DrawAlicWithHits(int mode=1);                            // *MENU*
  void SetVolumeAttributes(const char *name="SCM0",const int seen=1, int color=1, int fill=1); // *MENU*
  void TestIndexTransition(int pri=0, int idmax=0); // *MENU*

  AliEMCALGeometry* fGeometry; //!
  TH1F*             fHDe;      //!
  TH1F*             fHNhits;      //!

  ClassDef(AliEMCALv2,1)    //Implementation of EMCAL manager class to produce hits in a Shish-Kebab
    
};

#endif // AliEMCALV2_H
