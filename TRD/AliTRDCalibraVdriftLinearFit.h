#ifndef ALITRDCALIBRAVDRIFTLINEARFIT_H
#define ALITRDCALIBRAVDRIFTLINEARFIT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for online calibration                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class TObjArray;
class TH2F;
class TTreeSRedirector;

class AliTRDCalibraVdriftLinearFit : public TObject {

public:

  AliTRDCalibraVdriftLinearFit();
  AliTRDCalibraVdriftLinearFit(const AliTRDCalibraVdriftLinearFit &ped);
  virtual ~AliTRDCalibraVdriftLinearFit();

  AliTRDCalibraVdriftLinearFit& operator = (const  AliTRDCalibraVdriftLinearFit &source);

  void            Update(Int_t detector, Float_t tnp, Float_t pars1);
  void            FillPEArray();
  void            Add(AliTRDCalibraVdriftLinearFit *ped);
  TH2F            *GetLinearFitterHisto(Int_t detector, Bool_t force=kFALSE);
  Bool_t          GetParam(Int_t detector, TVectorD *param);
  Bool_t          GetError(Int_t detector, TVectorD *error);

  TObjArray       *GetPArray()                   { return &fLinearFitterPArray;       };
  TObjArray       *GetEArray()                   { return &fLinearFitterEArray;       };

private:
   
  Int_t           fVersion;                 // Version of the object

  TObjArray       fLinearFitterHistoArray;  // TObjArray of histo2D for debugging Linear Fitters
  TObjArray       fLinearFitterPArray;      // Array of result parameters from linear fitters for the detectors
  TObjArray       fLinearFitterEArray;      // Array of result errors from linear fitters for the detectors

public:

  ClassDef(AliTRDCalibraVdriftLinearFit,1)  // Online Vdrift calibration

};



#endif

