#ifndef ALIRUN_H
#define ALIRUN_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TMCProcess.h>
#include <TStopwatch.h>
#include <TError.h>

class TGeometry;
class TParticle;
class TRandom;
class TTree;

#include "AliRunLoader.h"
class AliDisplay;
class AliGenEventHeader;
class AliGenerator;
class AliHeader;
class AliLego;
class AliLegoGenerator;
class AliMC;
class AliMagF;
class AliStack;

enum {kKeepBit=1, kDaughtersBit=2, kDoneBit=4};

class AliRun : public TNamed {
public:
   // Creators - distructors
   AliRun();
   AliRun(const char *name, const char *title);
   AliRun(const AliRun &arun);
   virtual ~AliRun();

   AliRun& operator = (const AliRun &arun) 
     {arun.Copy(*this); return (*this);}
   virtual  void  Build();
   virtual  void  BuildSimpleGeometry();
   virtual  void  CleanDetectors();
   TObjArray     *Detectors() const {return fModules;}
   TObjArray     *Modules() const {return fModules;}
   AliDisplay    *Display() const { return fDisplay;}
   virtual AliMagF *Field() const {return fField;}
   virtual  void  FinishRun();
   void           AddModule(AliModule* mod);
   Int_t          GetEvNumber() const;
   Int_t          GetRunNumber() const {return fRun;}
   void           SetRunNumber(Int_t run) {fRun=run;}
   void           SetEventNrInRun(Int_t event) {fEventNrInRun=event;}
   Int_t          GetEventNrInRun() const {return fEventNrInRun;}
   Int_t          GetEventsPerRun() const {return fEventsPerRun;}
   Int_t          GetNdets() const {return fNdets;}
   Int_t          GetDebug() const {return fDebug;}
   AliModule     *GetModule(const char *name) const;
   AliDetector   *GetDetector(const char *name) const;
   Int_t          GetModuleID(const char *name) const;
   virtual  const char *GetBaseFile() const 
    {return fBaseFileName.Data();}
   virtual  Int_t GetEvent(Int_t event);
   virtual  void  SetEvent(Int_t event) {fEvent=event;}
   virtual  void  SetConfigFunction(const char * config="Config();");
   virtual  const char *GetConfigFunction() const 
    {return fConfigFunction.Data();}
   TGeometry     *GetGeometry();
   virtual  void  SetGenEventHeader(AliGenEventHeader* header);
   AliMC*         GetMCApp() const {return fMCApp;}
   virtual  void  Hits2Digits(const char *detector=0); 
   virtual  void  Hits2SDigits(const char *detector=0)   {Tree2Tree("S",detector);}
   virtual  void  SDigits2Digits(const char *detector=0) {Tree2Tree("D",detector);}
   virtual  void  Digits2Reco(const char *detector=0)    {Tree2Tree("R",detector);}
   virtual  void  InitMC(const char *setup="Config.C");
   virtual  void  Init(const char *setup="Config.C") {InitMC(setup);}
   Bool_t         IsFolder() const {return kTRUE;}
   virtual AliLego* Lego() const {return fLego;}

   virtual  void  ResetDigits();
   virtual  void  ResetSDigits();
   virtual  void  ResetPoints();
   virtual  void  SetBaseFile(const char *filename="galice.root");
   virtual  void  RunMC(Int_t nevent=1, const char *setup="Config.C");
   virtual  void  Run(Int_t nevent=1, const char *setup="Config.C") {RunMC(nevent,setup);}
   virtual  void  RunLego(const char *setup="Config.C",Int_t nc1=60,Float_t c1min=2,Float_t c1max=178,
                          Int_t nc2=60,Float_t c2min=0,Float_t c2max=360,Float_t rmin=0,
                          Float_t rmax=430,Float_t zmax=10000, AliLegoGenerator* gener=NULL);
   virtual  Bool_t IsLegoRun() const {return (fLego!=0);}
   virtual  void  RunReco(const char *detector=0, Int_t first = 0, Int_t last = 0);
   virtual  void  SetDebug(Int_t level=0) {fDebug = level;}
   virtual  void  SetDisplay(AliDisplay *display) {fDisplay = display;}
   virtual  void  SetField(Int_t type=2, Int_t version=1, Float_t scale=1, Float_t maxField=10, char*filename="$(ALICE_ROOT)/data/field01.dat");
   virtual  void  SetField(AliMagF* magField);
   virtual  TDatabasePDG* PDGDB() const {return fPDGDB;}
   
   virtual  void Field(const Double_t* x, Double_t* b) const;
   
   // Delegations
   virtual  void  ResetHits();
   virtual  AliGenerator* Generator() const;

   //
   // End of MC Application

   TTree         *TreeE() {return (fRunLoader)?fRunLoader->TreeE():0x0;}
   TTree         *TreeK() {return (fRunLoader)?fRunLoader->TreeK():0x0;}
   AliStack      *Stack() {return (fRunLoader)?fRunLoader->Stack():0x0;}
   AliHeader*     GetHeader() {return (fRunLoader)?fRunLoader->GetHeader():0x0;}

   TTree         *TreeD() {MayNotUse("TreeD"); return 0x0;}
   TTree         *TreeS() {MayNotUse("TreeS"); return 0x0;}
   TTree         *TreeR() {MayNotUse("TreeR"); return 0x0;}

   
   void SetRunLoader(AliRunLoader* rloader);
   AliRunLoader* GetRunLoader() const {return fRunLoader;}
//   void SetEventFolderName(const char* eventfoldername);
  virtual  void Announce() const;
   
  virtual  void  InitLoaders(); //prepares run (i.e. creates getters)
  static void Deprecated(TObject *obj, const char *method,
			 const char *replacement) {
    if (obj)
      ::Warning(Form("%s::%s", obj->ClassName(), method),
		"method is depricated\nPlease use: %s", replacement);
    else
      ::Warning(method, "method is depricated\nPlease use: %s", replacement);
  }
protected:
  virtual  void  Tree2Tree(Option_t *option, const char *detector=0);
  Int_t          fRun;               //! Current run number
  Int_t          fEvent;             //! Current event number (from 1)
  Int_t          fEventNrInRun;      //! Current unique event number in run
  Int_t          fEventsPerRun;      //  Number of events per run
  Int_t          fDebug;             //  Debug flag
  TObjArray     *fModules;           //  List of Detectors
  TGeometry     *fGeometry;          //  Pointer to geometry
  AliMC         *fMCApp;             //  Pointer to virtual MC Application
  AliDisplay    *fDisplay;           //! Pointer to event display
  TStopwatch     fTimer;             //  Timer object
  AliMagF       *fField;             //  Magnetic Field Map
  TVirtualMC    *fMC;                //! Pointer to MonteCarlo object
  Int_t          fNdets;             //  Number of detectors
  Bool_t         fInitDone;          //! True when initialisation done
  AliLego       *fLego;              //! Pointer to aliLego object if it exists
  TDatabasePDG  *fPDGDB;             //  Particle factory object
  TString        fConfigFunction;    //  Configuration file to be executed
  TRandom       *fRandom;            //  Pointer to the random number generator
  TString        fBaseFileName;      //  Name of the base root file

  AliRunLoader  *fRunLoader;         //!run getter - written as a separate object
private:
  void Copy(TObject &arun) const;

  ClassDef(AliRun,8)      //Supervisor class for all Alice detectors
};
 
R__EXTERN  AliRun *gAlice;
 
#endif
