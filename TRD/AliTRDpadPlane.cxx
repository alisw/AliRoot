/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Describes a pad plane of a TRD ROC                                       //
//                                                                           //
//  Contains the information on pad postions, pad dimensions,                //
//  tilting angle, etc.                                                      //
//  It also provides methods to identify the current pad number from         //
//  global coordinates.                                                      //
//  The numbering and coordinates should follow the official convention      //
//  (see David Emschermanns note on TRD convention                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TMath.h>

#include "AliTRDpadPlane.h"
#include "AliTRDgeometry.h"

ClassImp(AliTRDpadPlane)

//_____________________________________________________________________________
AliTRDpadPlane::AliTRDpadPlane()
  :TObject()
  ,fPla(0)
  ,fCha(0)
  ,fLength(0)
  ,fWidth(0)
  ,fLengthRim(0)
  ,fWidthRim(0)
  ,fLengthOPad(0)
  ,fWidthOPad(0)
  ,fLengthIPad(0)
  ,fWidthIPad(0)
  ,fRowSpacing(0)
  ,fColSpacing(0)
  ,fNrows(0)
  ,fNcols(0)
  ,fTiltingAngle(0)
  ,fTiltingTan(0)
  ,fPadRow(0)
  ,fPadCol(0)
  ,fPadRowSMOffset(0)
{
  //
  // Default constructor
  //

}

//_____________________________________________________________________________
AliTRDpadPlane::AliTRDpadPlane(const AliTRDpadPlane &p)
  :TObject(p)
  ,fPla(p.fPla)
  ,fCha(p.fCha)
  ,fLength(p.fLength)
  ,fWidth(p.fWidth)
  ,fLengthRim(p.fLengthRim)
  ,fWidthRim(p.fLengthRim)
  ,fLengthOPad(p.fLengthOPad)
  ,fWidthOPad(p.fWidthOPad)
  ,fLengthIPad(p.fLengthIPad)
  ,fWidthIPad(p.fWidthIPad)
  ,fRowSpacing(p.fRowSpacing)
  ,fColSpacing(p.fColSpacing)
  ,fNrows(p.fNrows)
  ,fNcols(p.fNcols)
  ,fTiltingAngle(p.fTiltingAngle)
  ,fTiltingTan(p.fTiltingTan)
  ,fPadRow(0)
  ,fPadCol(0)
  ,fPadRowSMOffset(p.fPadRowSMOffset)
{
  //
  // AliTRDpadPlane copy constructor
  //

  Int_t iBin = 0;

  if (((AliTRDpadPlane &) p).fPadRow) {
    delete [] ((AliTRDpadPlane &) p).fPadRow;
  }
  ((AliTRDpadPlane &) p).fPadRow = new Double_t[fNrows];
  for (iBin = 0; iBin < fNrows; iBin++) {
    ((AliTRDpadPlane &) p).fPadRow[iBin] = fPadRow[iBin];
  }                                                                             

  if (((AliTRDpadPlane &) p).fPadCol) {
    delete [] ((AliTRDpadPlane &) p).fPadCol;
  }
  ((AliTRDpadPlane &) p).fPadCol = new Double_t[fNrows];
  for (iBin = 0; iBin < fNrows; iBin++) {
    ((AliTRDpadPlane &) p).fPadCol[iBin] = fPadCol[iBin];
  }                                                                             

}

//_____________________________________________________________________________
AliTRDpadPlane::~AliTRDpadPlane()
{
  //
  // AliTRDpadPlane destructor
  //

  if (fPadRow) {
    delete [] fPadRow;
    fPadRow = 0;
  }

  if (fPadCol) {
    delete [] fPadCol;
    fPadCol = 0;
  }

}

//_____________________________________________________________________________
AliTRDpadPlane &AliTRDpadPlane::operator=(const AliTRDpadPlane &p)
{
  //
  // Assignment operator
  //

  if (this != &p) {
    ((AliTRDpadPlane &) p).Copy(*this);
  }

  return *this;

}

//_____________________________________________________________________________
void AliTRDpadPlane::Copy(TObject &p) const
{
  //
  // Copy function
  //

  Int_t iBin = 0;

  ((AliTRDpadPlane &) p).fPla            = fPla;
  ((AliTRDpadPlane &) p).fCha            = fCha;

  ((AliTRDpadPlane &) p).fLength         = fLength;
  ((AliTRDpadPlane &) p).fWidth          = fWidth;
  ((AliTRDpadPlane &) p).fLengthRim      = fLengthRim;
  ((AliTRDpadPlane &) p).fWidthRim       = fWidthRim;
  ((AliTRDpadPlane &) p).fLengthOPad     = fLengthOPad;
  ((AliTRDpadPlane &) p).fWidthOPad      = fWidthOPad;
  ((AliTRDpadPlane &) p).fLengthIPad     = fLengthIPad;
  ((AliTRDpadPlane &) p).fWidthIPad      = fWidthIPad;

  ((AliTRDpadPlane &) p).fRowSpacing     = fRowSpacing;
  ((AliTRDpadPlane &) p).fColSpacing     = fColSpacing;

  ((AliTRDpadPlane &) p).fNrows          = fNrows;
  ((AliTRDpadPlane &) p).fNcols          = fNcols;

  ((AliTRDpadPlane &) p).fTiltingAngle   = fTiltingAngle;
  ((AliTRDpadPlane &) p).fTiltingTan     = fTiltingTan;

  ((AliTRDpadPlane &) p).fPadRowSMOffset = fPadRowSMOffset;

  if (((AliTRDpadPlane &) p).fPadRow) {
    delete [] ((AliTRDpadPlane &) p).fPadRow;
  }
  ((AliTRDpadPlane &) p).fPadRow = new Double_t[fNrows];
  for (iBin = 0; iBin < fNrows; iBin++) {
    ((AliTRDpadPlane &) p).fPadRow[iBin] = fPadRow[iBin];
  }                                                                             

  if (((AliTRDpadPlane &) p).fPadCol) {
    delete [] ((AliTRDpadPlane &) p).fPadCol;
  }
  ((AliTRDpadPlane &) p).fPadCol = new Double_t[fNrows];
  for (iBin = 0; iBin < fNrows; iBin++) {
    ((AliTRDpadPlane &) p).fPadCol[iBin] = fPadCol[iBin];
  }                                                                             

  TObject::Copy(p);

}

//_____________________________________________________________________________
Int_t AliTRDpadPlane::GetPadRowNumber(Double_t z) const
{
  //
  // Finds the pad row number for a given z-position in local supermodule system
  //

  Int_t row    = 0;
  Int_t nabove = 0;
  Int_t nbelow = 0;
  Int_t middle = 0;

  if ((z > GetRow0()  ) || 
      (z < GetRowEnd())) {

    row = -1;

  }
  else {

    nabove = fNrows + 1;
    nbelow = 0;
    while (nabove - nbelow > 1) {
      middle = (nabove + nbelow) / 2;
      if (z == (fPadRow[middle-1] + fPadRowSMOffset)) {
        row    = middle;
      }
      if (z  > (fPadRow[middle-1] + fPadRowSMOffset)) {
        nabove = middle;
      }
      else {
        nbelow = middle;
      }
    }
    row = nbelow - 1;

  }

  return row;

}

//_____________________________________________________________________________
Int_t AliTRDpadPlane::GetPadRowNumberROC(Double_t z) const
{
  //
  // Finds the pad row number for a given z-position in local ROC system
  //

  Int_t row    = 0;
  Int_t nabove = 0;
  Int_t nbelow = 0;
  Int_t middle = 0;

  if ((z > GetRow0ROC()  ) || 
      (z < GetRowEndROC())) {

    row = -1;

  }
  else {

    nabove = fNrows + 1;
    nbelow = 0;
    while (nabove - nbelow > 1) {
      middle = (nabove + nbelow) / 2;
      if (z == fPadRow[middle-1]) {
        row    = middle;
      }
      if (z  > fPadRow[middle-1]) {
        nabove = middle;
      }
      else {
        nbelow = middle;
      }
    }
    row = nbelow - 1;

  }

  return row;

}

//_____________________________________________________________________________
Int_t AliTRDpadPlane::GetPadColNumber(Double_t rphi) const
{
  //
  // Finds the pad column number for a given global rphi-position
  //

  Int_t col    = 0;
  Int_t nabove = 0;
  Int_t nbelow = 0;
  Int_t middle = 0;

  if ((rphi > GetCol0()  ) || 
      (rphi < GetColEnd())) {

    col = -1;

  }
  else {

    nabove = fNcols + 1;
    nbelow = 0;
    while (nabove - nbelow > 1) {
      middle = (nabove + nbelow) / 2;
      if (rphi == fPadCol[middle-1]) {
        col    = middle;
      }
      if (rphi  > fPadCol[middle-1]) {
        nabove = middle;
      }
      else {
        nbelow = middle;
      }
    }
    col = nbelow - 1;

  }

  return col;

}
