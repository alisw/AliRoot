// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#include "AliL3Logging.h"
#include "AliL3HoughBaseTransformer.h"
#include "AliL3MemHandler.h"
#include "AliL3DigitData.h"
#include "AliL3Histogram.h"


//_____________________________________________________________
// AliL3HoughBaseTransformer
//
// The base class for implementations of Hough Transform on ALICE TPC data.
//
// This is an abstract class, and is only meant to provide the interface
// to the different implementations.

ClassImp(AliL3HoughBaseTransformer)

AliL3HoughBaseTransformer::AliL3HoughBaseTransformer()
{
  //Default constructor
  fDigitRowData = 0;

  fSlice = 0;
  fPatch = 0;
  fNEtaSegments =0;
  fEtaMin = 0;
  fEtaMax = 0;
  fLowerThreshold = 0;
  fUpperThreshold = 1023;
}

AliL3HoughBaseTransformer::AliL3HoughBaseTransformer(Int_t slice,Int_t patch,Int_t n_eta_segments)
{
  fDigitRowData = 0;

  fSlice = 0;
  fPatch = 0;
  fNEtaSegments =0;
  fEtaMin = 0;
  fEtaMax = 0;
  fLowerThreshold = 3;
  fUpperThreshold = 1023;

  Init(slice,patch,n_eta_segments);
}

AliL3HoughBaseTransformer::~AliL3HoughBaseTransformer()
{
}

void AliL3HoughBaseTransformer::Init(Int_t slice,Int_t patch,Int_t n_eta_segments,Int_t n_seqs)
{
  fSlice = slice;
  fPatch = patch;
  fNEtaSegments = n_eta_segments;
  fEtaMin = 0;
  fEtaMax = fSlice < 18 ? 1. : -1.;
}
