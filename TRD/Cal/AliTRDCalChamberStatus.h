#ifndef ALITRDCalChamberStatus_H
#define ALITRDCalChamberStatus_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for the status of a readout chamber                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "TNamed.h"

class AliTRDCalChamberStatus : public TNamed {

 public:

  enum { kNdet = 540, kNstacks = 90, kNcham = 5, kNsect = 18 };
  enum { kInstalled = 1, kMasked = 2, kHalfChamberSideAMasked = 3, kHalfChamberSideBMasked = 4};
  
  AliTRDCalChamberStatus();
  AliTRDCalChamberStatus(const Text_t* name, const Text_t* title);

  Char_t GetStatus(Int_t det) const          { return fStatus[det];   };
  void   SetStatus(Int_t det, Char_t status) { fStatus[det] = status; };

  Bool_t IsInstalled(Int_t sm) const         { return (GetStatus(sm) & kInstalled) ? kTRUE : kFALSE; }
  Bool_t IsMasked(Int_t sm) const            { return (GetStatus(sm) & kMasked)    ? kTRUE : kFALSE; }
  Bool_t IsHalfChamberSideAMasked(Int_t sm) const     { return (GetStatus(sm) & kHalfChamberSideAMasked)    ? kTRUE : kFALSE; }
  Bool_t IsHalfChamberSideBMasked(Int_t sm) const     { return (GetStatus(sm) & kHalfChamberSideBMasked)    ? kTRUE : kFALSE; }

 protected:

  Char_t fStatus[kNdet];                    //  Status byte

  ClassDef(AliTRDCalChamberStatus,1)        //  Defines the status of a single readout chamber

};

#endif


