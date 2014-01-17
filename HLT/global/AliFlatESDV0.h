#ifndef ALIFLATESDV0_H
#define ALIFLATESDV0_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 * Primary Authors : Sergey Gorbunov, Jochen Thaeder, Chiara Zampolli     */

/**
 * >> Flat structure representing a ESD vertex <<
 */

#include "Rtypes.h"

struct AliFlatESDV0
{
  Int_t fNegTrackID;
  Int_t fPosTrackID;
};

typedef struct AliFlatESDV0 AliFlatESDV0;

#endif
