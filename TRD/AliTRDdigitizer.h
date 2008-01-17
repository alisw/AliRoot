#ifndef ALITRDDIGITIZER_H
#define ALITRDDIGITIZER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Produces digits from the hits information                             //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "AliDigitizer.h"

class TFile;
class TF1;

class AliRunDigitizer;
class AliRunLoader;

class AliTRD;
class AliTRDdigitsManager;
class AliTRDgeometry;
class AliTRDfeeParam;
class AliTRDdataArrayF;
class AliTRDsegmentArray;

class AliTRDdigitizer : public AliDigitizer {

 public:

  AliTRDdigitizer();
  AliTRDdigitizer(const Text_t *name, const Text_t *title);
  AliTRDdigitizer(AliRunDigitizer *manager, const Text_t *name, const Text_t *title);
  AliTRDdigitizer(AliRunDigitizer *manager);
  AliTRDdigitizer(const AliTRDdigitizer &d);
  virtual             ~AliTRDdigitizer();
  AliTRDdigitizer     &operator=(const AliTRDdigitizer &d);

  virtual void         Copy(TObject &d) const;
          Bool_t       InitDetector();
          void         InitOutput(Int_t iEvent);
  virtual void         Exec(Option_t *option = 0);  

  virtual Bool_t       Open(const Char_t *file, Int_t nEvent = 0);
  virtual Bool_t       Open(AliRunLoader *runLoader, Int_t nEvent = 0);
  virtual Bool_t       MakeBranch(TTree *tree) const;
  virtual Bool_t       WriteDigits() const;

  virtual void         AddSDigitsManager(AliTRDdigitsManager *manager);
  virtual void         DeleteSDigitsManager();

  virtual Bool_t       MakeDigits();

          Bool_t       SortHits(Float_t **hits, Int_t *nhit);
          Bool_t       ConvertHits(Int_t det, Float_t *hits, Int_t nhit, AliTRDdataArrayF *signals);
          Bool_t       ConvertSignals(Int_t det, AliTRDdataArrayF *signals);

          Bool_t       SDigits2Digits();
          Bool_t       MergeSDigits();
          Bool_t       ConvertSDigits();

          Bool_t       Signal2ADC(Int_t det, AliTRDdataArrayF *signals);
          Bool_t       Signal2SDigits(Int_t det, AliTRDdataArrayF *signals);
          Bool_t       CopyDictionary(Int_t det);
          void         CompressOutputArrays(Int_t det);

          void         SetCompress(Int_t c = 1)             { fCompress        = c;   }
          void         SetSDigits(Int_t v = 1)              { fSDigits         = v;   }
          void         SetEvent(Int_t v = 0)                { fEvent           = v;   }
          void         SetManager(AliTRDdigitsManager *man) { fDigitsManager   = man; }
          void         SetGeometry(AliTRDgeometry *geo)     { fGeo             = geo; }
          void         SetMergeSignalOnly(Bool_t m = kTRUE) { fMergeSignalOnly = m;   }

  AliTRDdigitsManager *Digits() const                       { return fDigitsManager;  }

          Bool_t       GetCompress() const                  { return fCompress;       }
          Bool_t       GetSDigits() const                   { return fSDigits;        }
          Float_t      GetDiffusionT(Float_t vdrift);
          Float_t      GetDiffusionL(Float_t vdrift);
          Float_t      GetLorentzFactor(Float_t vdrift);

          Double_t     TimeStruct(Float_t vdrift, Double_t time, Double_t z);
          Int_t        Diffusion(Float_t vdrift, Double_t absdriftlength
                               , Double_t &lRow, Double_t &lCol, Double_t &lTime);
          Int_t        ExB(Float_t vdrift, Double_t driftlength, Double_t &lRow);
  
 protected:

  virtual Bool_t       Init();
          void         SampleTimeStruct(Float_t vdrift);
          void         RecalcDiffusion(Float_t vdrift);

  AliRunLoader        *fRunLoader;          //! Local pointer
  AliTRDdigitsManager *fDigitsManager;      //! Manager for the output digits
  AliTRDdigitsManager *fSDigitsManager;     //! Manager for the summed input s-digits
  TList               *fSDigitsManagerList; //! List of managers of input s-digits
  AliTRD              *fTRD;                //! TRD detector class
  AliTRDgeometry      *fGeo;                //! TRD geometry
  AliTRDfeeParam      *fFee;                //  Fee Parameters

          Int_t        fEvent;              //! Event number
          Int_t       *fMasks;              //! Masks for the merging
          Bool_t       fCompress;           //  Switch to keep only compressed data in memory
          Bool_t       fSDigits;            //  Switch for the summable digits
          Bool_t       fMergeSignalOnly;    //  Merge only detectors that contain a signal

          Float_t      fDiffLastVdrift;     //  The structures are valid for fLastVdrift (caching)
          Float_t      fDiffusionT;         //  Transverse drift coefficient
          Float_t      fDiffusionL;         //  Longitudinal drift coefficient
          Float_t      fOmegaTau;           //  Tangens of the Lorentz angle
          Float_t      fLorentzFactor;      //  Factor due to Lorentz force

          Float_t      fTimeLastVdrift;     //  The structures are valid for fLastVdrift (caching)
          Float_t     *fTimeStruct1;        //! Time Structure of Drift Cells
          Float_t     *fTimeStruct2;        //! Time Structure of Drift Cells
          Float_t      fVDlo;               //  Lower drift velocity, for interpolation
          Float_t      fVDhi;               //  Higher drift velocity, for interpolation

  ClassDef(AliTRDdigitizer,16)              //  Produces TRD-Digits

};
#endif
