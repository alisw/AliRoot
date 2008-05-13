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
 
//-------------------------------------------------------------------------
//                      Class AliRsnDaughterCut
//                     -------------------------
//           Implementation of track cuts for analysis.
//           These cuts must be added to the AliRsnAnalysis
//           object in order to make cut on single particles
//           or on pairs of particles.
// 
// author: A. Pulvirenti             (email: alberto.pulvirenti@ct.infn.it)
//-------------------------------------------------------------------------

#include <Riostream.h>

#include "AliRsnDaughter.h"
#include "AliRsnDaughterCut.h"

ClassImp(AliRsnDaughterCut)

//--------------------------------------------------------------------------------------------------------
Bool_t AliRsnDaughterCut::Pass(AliRsnDaughter *track) const
{
// 
// Virtual method for cut passing.
// This function checks that passed argument is not NULL.
// 
	if (!track) return kFALSE;
	return kTRUE;
}
//--------------------------------------------------------------------------------------------------------
Bool_t AliRsnDaughterCutPt::Pass(AliRsnDaughter *track) const
{
// 
// Cut on single track momentum.
//
	if (!AliRsnDaughterCut::Pass(track)) return kFALSE;
	if (track->Pt() < fPtMin) return kFALSE;
	if (track->Pt() > fPtMax) return kFALSE;
	
	return kTRUE;
}
//--------------------------------------------------------------------------------------------------------
Bool_t AliRsnDaughterCutVt::Pass(AliRsnDaughter *track) const
{
//
// Cut on impact parameter
//
    return (track->Vt() <= fVtMax);
}
