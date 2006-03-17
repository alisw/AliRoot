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

// $Id$

#include "AliMUONCalibParam2F.h"

#include "AliLog.h"
#include "Riostream.h"
#include "TMath.h"
#include "TString.h"

///
/// Implementation of AliMUONVCalibParam for pair of floats.
///
/// Conceptually, this class is the equivalent of a vector or float pairs,
/// but it is implemented using bare Float_t[] array.
///

ClassImp(AliMUONCalibParam2F)

//_____________________________________________________________________________
AliMUONCalibParam2F::AliMUONCalibParam2F() 
: AliMUONVCalibParam(),
  fSize(0),
  fN(0),
  fValues(0x0)
{
  //
  // Default ctor.
  // 
}

//_____________________________________________________________________________
AliMUONCalibParam2F::AliMUONCalibParam2F(Int_t theSize, Int_t fillWithValue) 
: AliMUONVCalibParam(),
  fSize(theSize),
  fN(fSize*Dimension())
{
  //
  // Normal ctor, where theSize specifies the number of channels handled
  // by this object, and fillWithValue the default value assigned to each
  // channel.
  //
  if ( fN > 0 )
  {
    fValues = new Float_t[fN];
    memset(fValues,fillWithValue,fN*sizeof(Float_t));
  }
}


//_____________________________________________________________________________
AliMUONCalibParam2F::AliMUONCalibParam2F(const AliMUONCalibParam2F& other) 
: AliMUONVCalibParam(),
fSize(0),
fN(0),
fValues(0x0)
{
  other.CopyTo(*this);
}

//_____________________________________________________________________________
AliMUONCalibParam2F&
AliMUONCalibParam2F::operator=(const AliMUONCalibParam2F& other) 
{
  other.CopyTo(*this);
  return *this;
}

//_____________________________________________________________________________
AliMUONCalibParam2F::~AliMUONCalibParam2F()
{
  //
  // dtor
  //
  delete[] fValues;
}

//_____________________________________________________________________________
void
AliMUONCalibParam2F::CopyTo(AliMUONCalibParam2F& destination) const
{
  //
  // Copy *this to destination
  //
  delete[] destination.fValues;
  destination.fN = fN;
  destination.fSize = fSize;

  if ( fN > 0 )
  {
    destination.fValues = new Float_t[fN];
    for ( Int_t i = 0; i < fN; ++i )
    {
      destination.fValues[i] = fValues[i];
    }
  }
}

//_____________________________________________________________________________
Int_t
AliMUONCalibParam2F::Index(Int_t i, Int_t j) const
{
  //
  // Compute the 1D index of the internal storage from the pair (i,j)
  // Returns -1 if the (i,j) pair is invalid
  //
  if ( i >= 0 && i < Size() && j >= 0 && j < Dimension() )
  {
    return i + Size()*j;
  }
  return -1;
}

//_____________________________________________________________________________
void
AliMUONCalibParam2F::Print(Option_t* opt) const
{
  //
  // Output this object to stdout.
  // If opt=="full", then all channels are printed, otherwise
  // only the general characteristics are printed.
  //
  TString sopt(opt);
  sopt.ToUpper();
  cout << "AliMUONCalibParam2F - Size=" << Size()
    << " Dimension=" << Dimension()
    << endl;
  if ( sopt.Contains("FULL") )
  {
    for ( Int_t i = 0; i < Size(); ++i )
    {
      cout << setw(6) << ValueAsFloat(i,0) << "," << ValueAsFloat(i,1) << endl;
    }
  }
}

//_____________________________________________________________________________
void
AliMUONCalibParam2F::SetValueAsFloat(Int_t i, Int_t j, Float_t value)
{
  //
  // Set one value as a float, after checking that the indices are correct.
  //
  Int_t ix = Index(i,j);
  
  if ( ix < 0 )
  {
    AliError(Form("Invalid (i,j)=(%d,%d) max allowed is (%d,%d)",
                  i,j,Size()-1,Dimension()-1));
  }
  else
  {
    fValues[ix]=value;
  }
}

//_____________________________________________________________________________
void
AliMUONCalibParam2F::SetValueAsInt(Int_t i, Int_t j, Int_t value)
{
  //
  // Set one value as an int.
  //
  SetValueAsFloat(i,j,static_cast<Float_t>(value));
}

//_____________________________________________________________________________
Float_t
AliMUONCalibParam2F::ValueAsFloat(Int_t i, Int_t j) const
{
  //
  // Return the value as a float (which it is), after checking indices.
  //
  Int_t ix = Index(i,j);
  
  if ( ix < 0 )
  {
    AliError(Form("Invalid (i,j)=(%d,%d) max allowed is (%d,%d)",
                  i,j,Size()-1,Dimension()-1));
    return 0.0;
  }
  else
  {
    return fValues[ix];
  }
}

//_____________________________________________________________________________
Int_t
AliMUONCalibParam2F::ValueAsInt(Int_t i, Int_t j) const
{
  //
  // Return the value as an int, by rounding the internal float value.
  //
  Float_t v = ValueAsFloat(i,j);
  return TMath::Nint(v);
}
