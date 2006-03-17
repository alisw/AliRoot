/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpConstants.h,v 1.9 2006/03/13 12:08:51 ivana Exp $

/// \ingroup basic
/// \class AliMpConstants
/// \brief Globally used constants definition.
///
/// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_CONSTANTS_H
#define ALI_MP_CONSTANTS_H

#include <TObject.h>
#include "AliMpPlaneType.h"

class TVector2;

class AliMpConstants : public TObject
{
 public:
  AliMpConstants();
  virtual ~AliMpConstants();

  // static compare methods
  static Bool_t  IsEqual(Double_t length1, Double_t length2);
  static Bool_t  IsEqual(const TVector2& v1, const TVector2& v2);

  // static get methods
  static Double_t LengthTolerance();
  static Double_t LengthStep();
  static Int_t    StartPadIndex();
  static Int_t    NCh();
  static Int_t    ManuMask(AliMpPlaneType planeType);
  
 private:
  // unused derived functions
  virtual Bool_t  IsEqual(const TObject*) const { return true; }
 
  // static data members
  static const Double_t  fgkLengthTolerance;// the length precision for tests
  static const Double_t  fgkLengthStep;     // the step in length used to move from
                                            // a geometric border inside (pad, motif)
  static const Int_t     fgkStartPadIndex;  // global pad indices start value
  static const Int_t     fgkNCh;            // number of chambers
  static const Int_t     fgkNonBendingManuMask; // bit to set to indicate a manu located in non-bending plane
  
  ClassDef(AliMpConstants,3) //Class for globally used constants definition
};

// inline functions

inline Double_t AliMpConstants::LengthTolerance() {return fgkLengthTolerance;}
inline Double_t AliMpConstants::LengthStep()      {return fgkLengthStep;}
inline Int_t    AliMpConstants::StartPadIndex()   {return fgkStartPadIndex;}
inline Int_t    AliMpConstants::NCh()             {return fgkNCh;}

#endif //ALI_MP_CONSTANTS_H

