#ifndef ALIEMCALTrigger_H
#define ALIEMCALTrigger_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id $ */
/* $Log $ */
//___________________________________________________________
//  Class for trigger analysis.
//  Digits are grouped in TRU's (Trigger Units). A TRU consist of 384 cells 
//  ordered fNTRUPhi x fNTRUZ. The algorithm searches all possible 
//  4x4 crystal combinations per each TRU, adding the digits amplitude and 
//  finding the maximum. Maximums are transformed in adc time samples. 
//  Each time bin is compared to the trigger threshold until it is larger 
//  and then, triggers are set. Thresholds need to be fixed. 
//  Last 2 modules are half size in Phi, I considered that the number 
//  of TRU is maintained for the last modules but final decision has not 
//  been taken. If different, then this must to be changed. 
//  Usage:
//
//  //Inside the event loop
//  AliEMCALTrigger *tr = new AliEMCALTrigger();//Init Trigger
//  tr->SetL0Threshold(100);
//  tr->SetL1JetLowPtThreshold(1000);
//  tr->SetL1JetMediumPtThreshold(10000);
//  tr->SetL1JetHighPtThreshold(20000);
//  tr->Trigger(); //Execute Trigger
//  tr->Print(""); //Print results
//
//*-- Author: Gustavo Conesa & Yves Schutz (IFIC, SUBATECH, CERN)
     
// --- ROOT system ---

class TClonesArray ;
#include "TMatrixD.h"

// --- AliRoot header files ---
#include "AliTriggerDetector.h"

class AliEMCALGeometry ;

class AliEMCALTrigger : public AliTriggerDetector {
  
 public:   

  AliEMCALTrigger() ; //  ctor
  AliEMCALTrigger(const AliEMCALTrigger & trig) ; // cpy ctor
  virtual ~AliEMCALTrigger() {}; //virtual dtor
  virtual void    CreateInputs(); //Define trigger inputs for Central Trigger Processor
  void            Print(const Option_t * opt ="") const ;  
  virtual void    Trigger();  //Make EMCAL trigger

  //Getters
  Float_t  Get2x2MaxAmplitude()  const {return f2x2MaxAmp ; }
  Float_t  Get4x4MaxAmplitude()  const {return f4x4MaxAmp ; }
  Int_t    Get2x2CellPhi()       const {return f2x2CellPhi ; }
  Int_t    Get4x4CellPhi()       const {return f4x4CellPhi ; }
  Int_t    Get2x2CellEta()       const {return f2x2CellEta ; }
  Int_t    Get4x4CellEta()       const {return f4x4CellEta ; }
  Int_t    Get2x2SuperModule()   const {return f2x2SM ; }
  Int_t    Get4x4SuperModule()   const {return f4x4SM ; }

  Int_t *  GetADCValuesLowGainMax2x2Sum()  {return fADCValuesLow2x2; }
  Int_t *  GetADCValuesHighGainMax2x2Sum() {return fADCValuesHigh2x2; }
  Int_t *  GetADCValuesLowGainMax4x4Sum()  {return fADCValuesLow4x4; }
  Int_t *  GetADCValuesHighGainMax4x4Sum() {return fADCValuesHigh4x4; }

  Float_t  GetL0Threshold() const           {return fL0Threshold ; } 
  Float_t  GetL1JetLowPtThreshold() const   {return fL1JetLowPtThreshold ; }
  Float_t  GetL1JetMediumPtThreshold()const {return fL1JetMediumPtThreshold ; }
  Float_t  GetL1JetHighPtThreshold() const  {return fL1JetHighPtThreshold ; }
  
  Bool_t   IsSimulation() const {return fSimulation ; }
  
  //Setters
  void     SetDigitsList(TClonesArray * digits)          
   {fDigitsList  = digits ; }

  void     SetL0Threshold(Int_t amp)     
    {fL0Threshold            = amp; }
  void     SetL1JetLowPtThreshold(Int_t amp) 
    {fL1JetLowPtThreshold    = amp; } 
  void     SetL1JetMediumPtThreshold(Int_t amp) 
    {fL1JetMediumPtThreshold = amp; } 
  void     SetL1JetHighPtThreshold(Int_t amp)
    {fL1JetHighPtThreshold   = amp; }

  void SetSimulation(Bool_t sim )          {fSimulation = sim ; }

 private:
 
  void MakeSlidingCell(const TClonesArray * amptrus, const TClonesArray * timeRtrus,const Int_t supermod, TMatrixD *ampmax2, TMatrixD *ampmax4, AliEMCALGeometry * geom) ; 
  

  void SetTriggers(const Int_t iSM, const TMatrixD *ampmax2, const TMatrixD *ampmax4, AliEMCALGeometry *geom) ;
    
 private: 

  Float_t f2x2MaxAmp ;        //! Maximum 2x2 added amplitude (not overlapped) 
  Int_t   f2x2CellPhi ;       //! upper right cell, row(phi)   
  Int_t   f2x2CellEta ;       //! and column(eta)  
  Int_t   f2x2SM ;            //! Super Module where maximum is found
  Float_t f4x4MaxAmp ;        //! Maximum 4x4 added amplitude (overlapped)
  Int_t   f4x4CellPhi ;       //! upper right cell, row(phi)   
  Int_t   f4x4CellEta ;       //! and column(eta)
  Int_t   f4x4SM ;            //! Super Module where maximum is found

  Int_t*   fADCValuesHigh4x4 ; //! Sampled ADC high gain values for the 4x4 crystals amplitude sum
  Int_t*   fADCValuesLow4x4  ; //! " low gain  " 
  Int_t*   fADCValuesHigh2x2 ; //! " high gain " 2x2 "
  Int_t*   fADCValuesLow2x2  ; //! " low gaing " "

  TClonesArray* fDigitsList ;  //Array of digits 

  Float_t fL0Threshold ;            //! L0 trigger energy threshold
  Float_t fL1JetLowPtThreshold ;    //! L1 Low pT trigger energy threshold
  Float_t fL1JetMediumPtThreshold ; //! L1 Medium pT trigger energy threshold
  Float_t fL1JetHighPtThreshold ;   //! L1 High pT trigger energy threshold

  Bool_t  fSimulation ;           //! Flag to do the trigger during simulation or reconstruction

  ClassDef(AliEMCALTrigger,1)
} ;
    
    
#endif //ALIEMCALTrigger_H
    
