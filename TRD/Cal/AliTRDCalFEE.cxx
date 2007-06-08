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
//  TRD calibration class for TRD FEE parameters                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTRDCalFEE.h"

ClassImp(AliTRDCalFEE)

//_____________________________________________________________________________
AliTRDCalFEE::AliTRDCalFEE()
  :TNamed()
  ,fNumberOfTimeBins(0)
  ,fTailCancelationTau1(0)
  ,fTailCancelationTau2(0)
  ,fTailCancelationAmp(0)
  ,fPedestal(0)
  ,fConfigID(0)
  ,fGainTableID(0)
{
  //
  // AliTRDCalFEE default constructor
  //

}

//_____________________________________________________________________________
AliTRDCalFEE::AliTRDCalFEE(const Text_t *name, const Text_t *title)
  :TNamed(name,title)
  ,fNumberOfTimeBins(0)
  ,fTailCancelationTau1(0)
  ,fTailCancelationTau2(0)
  ,fTailCancelationAmp(0)
  ,fPedestal(0)
  ,fConfigID(0)
  ,fGainTableID(0)
{
  //
  // AliTRDCalFEE constructor
  //

}
