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

/* $Id: AliTRDCalDCSFEEv2.cxx 18952 2007-06-08 11:36:12Z cblume $ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for TRD DCS FEE configuration parameters           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

// fStatusBit:
// 0: no errors for that ROC
// 1: ROC sent invalid or corrupted data. 
// 2: ROC was not in state CONFIGURED or STANDBY_INIT (most probably it was in STANDBY)
// 3: No new data received from that ROC.
// 4: DCS id from XML attributes <DCS> and <ack> and the one calculated from SM, S, L do not match
// 5: ROC has not responded at all, most probably it was off.

#include "AliTRDCalDCSFEEv2.h"

ClassImp(AliTRDCalDCSFEEv2)
  
//_____________________________________________________________________________
AliTRDCalDCSFEEv2::AliTRDCalDCSFEEv2()
  :TObject()
  ,fStatusBit(0)
  ,fSM(-1)
  ,fStack(-1)
  ,fLayer(-1)
  ,fGainTableRocSerial(0)
  ,fDCSID(-1)
  ,fNumberOfTimeBins(-1)
  ,fConfigTag(-1)
  ,fSingleHitThres(-1)
  ,fThrPdClsThres(-1)
  ,fSelNoZS(-1)
  ,fTCFilterWeight(-1)
  ,fTCFilterShortDecPar(-1)
  ,fTCFilterLongDecPar(-1)
  ,fFastStatNoise(-1)
  ,fGainTableRocType("")
  ,fFilterType("")
  ,fReadoutParam("")
  ,fTestPattern("")
  ,fTrackletMode("")
  ,fTrackletDef("")
  ,fTriggerSetup("")
  ,fAddOptions("") 
  ,fConfigName("")
  ,fConfigVersion("")
  ,fGainTableName("")
  ,fGainTableDesc("")
{
  //
  // AliTRDCalDCSFEEv2 default constructor
  //
  for(Int_t i=0; i<(Int_t)fgkROB; i++) {
    for(Int_t j=0; j<(Int_t)fgkMCM; j++) {
      fRStateGSM[i][j]  = -1;
      fRStateNI[i][j]   = -1;
      fRStateEV[i][j]   = -1;
      fRStatePTRG[i][j] = -1;
      fGainTableAdcdac[i][j] = -1;
      for(Int_t k=0; k<(Int_t)fgkADC; k++) {
	fGainTableFgfn[i][j][k] = -1;
	fGainTableFgan[i][j][k] = -1;
      }
    }
  }
}


//_____________________________________________________________________________
AliTRDCalDCSFEEv2::AliTRDCalDCSFEEv2(const AliTRDCalDCSFEEv2 &c)
  :TObject(c)
  ,fStatusBit(c.fStatusBit)
  ,fSM(c.fSM)
  ,fStack(c.fStack)
  ,fLayer(c.fLayer)
  ,fGainTableRocSerial(c.fGainTableRocSerial)
  ,fDCSID(c.fDCSID)
  ,fNumberOfTimeBins(c.fNumberOfTimeBins)
  ,fConfigTag(c.fConfigTag)
  ,fSingleHitThres(c.fSingleHitThres)
  ,fThrPdClsThres(c.fThrPdClsThres)
  ,fSelNoZS(c.fSelNoZS)
  ,fTCFilterWeight(c.fTCFilterWeight)
  ,fTCFilterShortDecPar(c.fTCFilterShortDecPar)
  ,fTCFilterLongDecPar(c.fTCFilterLongDecPar)
  ,fFastStatNoise(c.fFastStatNoise)
  ,fGainTableRocType(c.fGainTableRocType)
  ,fFilterType(c.fFilterType)
  ,fReadoutParam(c.fReadoutParam)
  ,fTestPattern(c.fTestPattern)
  ,fTrackletMode(c.fTrackletMode)
  ,fTrackletDef(c.fTrackletDef)
  ,fTriggerSetup(c.fTriggerSetup)
  ,fAddOptions(c.fAddOptions) 
  ,fConfigName(c.fConfigName)
  ,fConfigVersion(c.fConfigVersion)
  ,fGainTableName(c.fGainTableName)
  ,fGainTableDesc(c.fGainTableDesc)
{
  //
  // AliTRDCalDCSFEEv2 copy constructor
  //
  for(Int_t i=0; i<(Int_t)fgkROB; i++) {
    for(Int_t j=0; j<(Int_t)fgkMCM; j++) {
      fRStateGSM[i][j]  = c.fRStateGSM[i][j];
      fRStateNI[i][j]   = c.fRStateNI[i][j];
      fRStateEV[i][j]   = c.fRStateEV[i][j];
      fRStatePTRG[i][j] = c.fRStatePTRG[i][j];
      fGainTableAdcdac[i][j] = c.fGainTableAdcdac[i][j];
      for(Int_t k=0; k<(Int_t)fgkADC; k++) {
	fGainTableFgfn[i][j][k] = c.fGainTableFgfn[i][j][k];
	fGainTableFgan[i][j][k] = c.fGainTableFgan[i][j][k];
      }
    }
  }
}


//_____________________________________________________________________________
AliTRDCalDCSFEEv2 &AliTRDCalDCSFEEv2::operator=(const AliTRDCalDCSFEEv2 &c)
{
  //
  // Assignment operator
  //
  if (&c == this) return *this;

  new (this) AliTRDCalDCSFEEv2(c);
  return *this;
}

//_____________________________________________________________________________
TString AliTRDCalDCSFEEv2::GetRstateString(Int_t r, Int_t m)
{

  TString gsmstate = "UNKNOWN  ";
  switch (fRStateGSM[r][m]) {
  case  0: gsmstate = "low_power"; break;
  case  1: gsmstate = "test     "; break;
  case  3: gsmstate = "wait_pre "; break;
  case  7: gsmstate = "preproc  "; break;
  case  8: gsmstate = "zero_sp  "; break;
  case  9: gsmstate = "full_rd  "; break;
  case 11: gsmstate = "clear_st "; break;
  case 12: gsmstate = "wait_l1  "; break;
  case 14: gsmstate = "tr_send  "; break;
  case 15: gsmstate = "tr_proc  "; break;
  }

  TString nistate = "UNKNOWN ";
  switch (fRStateNI[r][m]) {
  case  0: nistate = "clear   "; break;
  case  1: nistate = "idle    "; break;
  case  2: nistate = "idle_tr "; break;
  case  3: nistate = "idle_rr "; break;
  case  4: nistate = "idle_net"; break;
  case  5: nistate = "tri0l   "; break;
  case  6: nistate = "tri0h   "; break;
  case  7: nistate = "tri1l   "; break;
  case  8: nistate = "tri1h   "; break;
  case  9: nistate = "tri2l   "; break;
  case 10: nistate = "tri2h   "; break;
  case 11: nistate = "tri3l   "; break;
  case 12: nistate = "tri3h   "; break;
  case 13: nistate = "trp0    "; break;
  case 14: nistate = "trp1    "; break;
  case 15: nistate = "trp2    "; break;
  case 16: nistate = "trp3    "; break;
  case 17: nistate = "rrf0    "; break;
  case 18: nistate = "rrf1    "; break;
  case 19: nistate = "rrf2    "; break;
  case 20: nistate = "rrf3    "; break;
  case 21: nistate = "rrp0    "; break;
  case 22: nistate = "rrp1    "; break;
  case 23: nistate = "rrp2    "; break;
  case 24: nistate = "rrp3    "; break;
  case 25: nistate = "finished"; break;
  case 26: nistate = "tpl     "; break;
  case 27: nistate = "tph     "; break;
  }
  
  return ( gsmstate + " (" + nistate + ")"
	   + Form( "%8d events   %8d pretriggers",
		   fRStateEV[r][m], fRStatePTRG[r][m]) );
  
}
