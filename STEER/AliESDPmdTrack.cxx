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

#include "AliESDPmdTrack.h"

// Event Data Summary Class
// For pmd tracks
// This is part of the reconstructed
// ESD events
// for the PMD detector

ClassImp(AliESDPmdTrack)

AliESDPmdTrack::AliESDPmdTrack (const AliESDPmdTrack& PMDTrack):TObject(PMDTrack)
{
  // Constructor
  fDet    = PMDTrack.fDet;
  fTheta  = PMDTrack.fTheta;
  fPhi    = PMDTrack.fPhi;
  fCluADC = PMDTrack.fCluADC;
  fCluPID = PMDTrack.fCluPID;
}
//--------------------------------------------------------------------------//
AliESDPmdTrack &AliESDPmdTrack::operator=(const AliESDPmdTrack& PMDTrack)
{
  // Copy constructor
  if(&PMDTrack == this) return *this;
  fDet    = PMDTrack.fDet;
  fTheta  = PMDTrack.fTheta;
  fPhi    = PMDTrack.fPhi;
  fCluADC = PMDTrack.fCluADC;
  fCluPID = PMDTrack.fCluPID;
  return *this;
}
