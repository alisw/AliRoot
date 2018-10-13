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

/* $Id: AliTRDCalDCSv2.cxx 18952 2007-06-08 11:36:12Z cblume $ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class v2 for TRD DCS parameters                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTRDCalDCSv2.h"
#include "AliTRDCalDCSFEEv2.h"
#include "AliTRDCalDCSGTU.h"
#include "AliLog.h"

#include <iostream>

#include <TH1.h>

ClassImp(AliTRDCalDCSv2)
  
//_____________________________________________________________________________
AliTRDCalDCSv2::AliTRDCalDCSv2()
  :TNamed()
  ,fGNumberOfTimeBins(-1)
  ,fGConfigTag(-1)
  ,fGSingleHitThres(-1)
  ,fGThreePadClustThres(-1)
  ,fGSelNoZS(-1)
  ,fGTCFilterWeight(-1)
  ,fGTCFilterShortDecPar(-1)
  ,fGTCFilterLongDecPar(-1)
  ,fGFastStatNoise(-1)
  ,fGConfigVersion(0)
  ,fGConfigName(0)
  ,fGFilterType(0)
  ,fGReadoutParam(0)
  ,fGTestPattern(0)
  ,fGTrackletMode(0)
  ,fGTrackletDef(0)
  ,fGTriggerSetup(0)
  ,fGAddOptions(0)
  ,fRunType("")
  ,fStartTime(0)
  ,fEndTime(0)
  ,fFEEArr(new TObjArray(540))
  ,fPTRArr(new TObjArray(6))
  ,fGTUObj(new AliTRDCalDCSGTU())
{
  //
  // AliTRDCalDCSv2 default constructor
  //
}

//_____________________________________________________________________________
AliTRDCalDCSv2::AliTRDCalDCSv2(const Text_t *name, const Text_t *title)
  :TNamed(name,title)
  ,fGNumberOfTimeBins(-1)
  ,fGConfigTag(-1)
  ,fGSingleHitThres(-1)
  ,fGThreePadClustThres(-1)
  ,fGSelNoZS(-1)
  ,fGTCFilterWeight(-1)
  ,fGTCFilterShortDecPar(-1)
  ,fGTCFilterLongDecPar(-1)
  ,fGFastStatNoise(-1)
  ,fGConfigVersion(0)
  ,fGConfigName(0)
  ,fGFilterType(0)
  ,fGReadoutParam(0)
  ,fGTestPattern(0)
  ,fGTrackletMode(0)
  ,fGTrackletDef(0)
  ,fGTriggerSetup(0)
  ,fGAddOptions(0)
  ,fRunType("")
  ,fStartTime(0)
  ,fEndTime(0)
  ,fFEEArr(new TObjArray(540))
  ,fPTRArr(new TObjArray(6))
  ,fGTUObj(new AliTRDCalDCSGTU())
{
  //
  // AliTRDCalDCSv2 constructor
  //
}

//_____________________________________________________________________________
AliTRDCalDCSv2::AliTRDCalDCSv2(const AliTRDCalDCSv2 &cd)
  :TNamed(cd)
  ,fGNumberOfTimeBins(-1)
  ,fGConfigTag(-1)
  ,fGSingleHitThres(-1)
  ,fGThreePadClustThres(-1)
  ,fGSelNoZS(-1)
  ,fGTCFilterWeight(-1)
  ,fGTCFilterShortDecPar(-1)
  ,fGTCFilterLongDecPar(-1)
  ,fGFastStatNoise(-1)
  ,fGConfigVersion(0)
  ,fGConfigName(0)
  ,fGFilterType(0)
  ,fGReadoutParam(0)
  ,fGTestPattern(0)
  ,fGTrackletMode(0)
  ,fGTrackletDef(0)
  ,fGTriggerSetup(0)
  ,fGAddOptions(0)
  ,fRunType("")
  ,fStartTime(0)
  ,fEndTime(0)
  ,fFEEArr(new TObjArray(540))
  ,fPTRArr(new TObjArray(6))
  ,fGTUObj(new AliTRDCalDCSGTU())
{
  //
  // AliTRDCalDCSv2 copy constructor
  //
}

//_____________________________________________________________________________
AliTRDCalDCSv2 &AliTRDCalDCSv2::operator=(const AliTRDCalDCSv2 &cd)
{
  //
  // Assignment operator
  //
  if (&cd == this) return *this;

  new (this) AliTRDCalDCSv2(cd);
  return *this;
}

//_____________________________________________________________________________
void AliTRDCalDCSv2::Print(Option_t *option) const
{
  AliInfo(Form("Run duration: %d - %d (%d sec)",
	       fStartTime, fEndTime, fEndTime-fStartTime));
  

  // ------------------------------------------------------------------------
  // display FEE configuration for all ROCs
  for(Int_t i=0; i<540; i++) {
    AliTRDCalDCSFEEv2 *iDCSFEEObj = GetCalDCSFEEObj(i);
    if(iDCSFEEObj == NULL) continue;

    if(iDCSFEEObj->GetStatusBit() != 0) {
      AliWarning(Form("status bit of DCS FEE object [%d] is set", i));
      continue;
    }
    
    AliInfo(Form("%02d_%d_%d [%03d]: config = %s : %s",
		 i/30, (i%30)/6, i%6, i,
		 iDCSFEEObj->GetConfigName().Data(),
		 iDCSFEEObj->GetConfigVersion().Data()
		 ));
  }

  // ------------------------------------------------------------------------
  // display summary of FEE configurations

  TH1F* h = CreatePluralityHistogram(&AliTRDCalDCSFEEv2::GetConfigNameVersion);
  for (Int_t i=1; i<=h->GetNbinsX(); i++) {
    AliInfoStream() << std::setw(4) << h->GetBinContent(i) << "   "
		    << h->GetXaxis()->GetBinLabel(i) << "\n";
  }
  delete h;
  
}

//_____________________________________________________________________________
void AliTRDCalDCSv2::PrintGlobalConfig(Option_t *option) const
{
  AliInfoStream()
    << "fGNumberOfTimeBins    - " << fGNumberOfTimeBins << "\n"
    << "fGConfigTag           - " << fGConfigTag << "\n"
    << "fGSingleHitThres      - " << fGSingleHitThres << "\n"
    << "fGThreePadClustThres  - " << fGThreePadClustThres << "\n"
    << "fGSelNoZS             - " << fGSelNoZS << "\n"
    << "fGTCFilterWeight      - " << fGTCFilterWeight << "\n"
    << "fGTCFilterShortDecPar - " << fGTCFilterShortDecPar << "\n"
    << "fGTCFilterLongDecPar  - " << fGTCFilterLongDecPar << "\n"
    << "fGFastStatNoise       - " << fGFastStatNoise << "\n"
    << "fGConfigVersion       - " << fGConfigVersion << "\n"
    << "fGConfigName          - " << fGConfigName << "\n"
    << "fGFilterType          - " << fGFilterType << "\n"
    << "fGReadoutParam        - " << fGReadoutParam << "\n"
    << "fGTestPattern         - " << fGTestPattern << "\n"
    << "fGTrackletMode        - " << fGTrackletMode << "\n"
    << "fGTrackletDef         - " << fGTrackletDef << "\n"
    << "fGTriggerSetup        - " << fGTriggerSetup << "\n"
    << "fGAddOptions          - " << fGAddOptions << "\n";
}

//____________________________________________________________________________
TH1F* AliTRDCalDCSv2::CreatePluralityHistogram(TString (AliTRDCalDCSFEEv2::*fct)() const) const
{
  // Create a histogram with alphanumernic labels from a function in
  // the AliTRDCalDCSFEEv2 configuration class. This function can be
  // used to find the frequency of configuration values in
  // AliTRDCalDCSFEEv2.

  // Code based on an example on alphanumeric labels in the ROOT docs, see:
  // https://root.cern.ch/doc/master/hlabels1_8C.html

  // create the histogram and enable automatic extension 
  TH1F *h = new TH1F("h","test",3,0.0,3.0);
  h->SetStats(0); // disable statistics display
  h->SetCanExtend(TH1::kAllAxes); // 
  
  // loop over all 540 chambers
  for(Int_t i=0; i<540; i++) {
    AliTRDCalDCSFEEv2 *iDCSFEEObj = GetCalDCSFEEObj(i);
    if(iDCSFEEObj == NULL) {
      h->Fill("+++ NOT PRESENT +++",1);
      continue;
    }
    
    if(iDCSFEEObj->GetStatusBit() != 0) {
      h->Fill("+++ STATUS BIT SET +++",1);
      continue;
    }

    h->Fill( (iDCSFEEObj->*fct)(),1);
  }

  // compress and reorder bins
  h->LabelsDeflate();
  h->LabelsOption(">");

  return h;
  
}

//____________________________________________________________________________
AliTRDCalDCSFEEv2* AliTRDCalDCSv2::FindPluralityChamber(TString (AliTRDCalDCSFEEv2::*fct)() const) const
{

  // find the plurality name
  TH1F* h = CreatePluralityHistogram(fct);
  TString plurality = h->GetXaxis()->GetBinLabel(1);
  delete h;

  // loop over all chambers to find one with plurality name
  for(Int_t i=0; i<540; i++) {
    AliTRDCalDCSFEEv2 *iDCSFEEObj = GetCalDCSFEEObj(i);
    if(iDCSFEEObj == NULL) {
      h->Fill("+++ NOT PRESENT +++",1);
      continue;
    }
    
    if(iDCSFEEObj->GetStatusBit() != 0) {
      h->Fill("+++ STATUS BIT SET +++",1);
      continue;
    }

    if ( (iDCSFEEObj->*fct)() == plurality ) {
      return iDCSFEEObj;
    }
  }

  // we should never get here
  AliFatal("No chamber with plurality configuration found!");
  return NULL;
}

//____________________________________________________________________________
void AliTRDCalDCSv2::ForceOverwritePluralityConfig()
{
  AliTRDCalDCSFEEv2* plcfg;
  plcfg = FindPluralityChamber(&AliTRDCalDCSFEEv2::GetConfigNameVersion);

  fGNumberOfTimeBins    = plcfg->GetNumberOfTimeBins();
  fGConfigTag           = plcfg->GetConfigTag();
  fGSingleHitThres      = plcfg->GetSingleHitThres();
  fGThreePadClustThres  = plcfg->GetThreePadClustThres();
  fGSelNoZS             = plcfg->GetSelectiveNoZS();
  fGTCFilterWeight      = plcfg->GetTCFilterWeight();
  fGTCFilterShortDecPar = plcfg->GetTCFilterShortDecPar();
  fGTCFilterLongDecPar  = plcfg->GetTCFilterLongDecPar();
  fGFastStatNoise       = plcfg->GetFastStatNoise();
  fGConfigVersion       = plcfg->GetConfigVersion();
  fGConfigName          = plcfg->GetConfigName();
  fGFilterType          = plcfg->GetFilterType();
  fGReadoutParam        = plcfg->GetReadoutParam();
  fGTestPattern         = plcfg->GetTestPattern();
  fGTrackletMode        = plcfg->GetTrackletMode();
  fGTrackletDef         = plcfg->GetTrackletDef();
  fGTriggerSetup        = plcfg->GetTriggerSetup();
  fGAddOptions          = plcfg->GetAddOptions();
}


//_____________________________________________________________________________
void AliTRDCalDCSv2::EvaluateGlobalParameters()
{
  //
  // Do an evaluation of all global parameters
  //

  for(Int_t i=0; i<540; i++) {
    AliTRDCalDCSFEEv2 *iDCSFEEObj;
    iDCSFEEObj = GetCalDCSFEEObj(i);
    if(iDCSFEEObj != NULL) {
      if(iDCSFEEObj->GetStatusBit() == 0) {
	// first, set the parameters of the first good ROC as global
	fGNumberOfTimeBins    = iDCSFEEObj->GetNumberOfTimeBins();
	fGConfigTag           = iDCSFEEObj->GetConfigTag();
	fGSingleHitThres      = iDCSFEEObj->GetSingleHitThres();
	fGThreePadClustThres  = iDCSFEEObj->GetThreePadClustThres();
	fGSelNoZS             = iDCSFEEObj->GetSelectiveNoZS();
	fGTCFilterWeight      = iDCSFEEObj->GetTCFilterWeight();
	fGTCFilterShortDecPar = iDCSFEEObj->GetTCFilterShortDecPar();
	fGTCFilterLongDecPar  = iDCSFEEObj->GetTCFilterLongDecPar();
	fGFastStatNoise       = iDCSFEEObj->GetFastStatNoise();
	fGConfigVersion       = iDCSFEEObj->GetConfigVersion();
	fGConfigName          = iDCSFEEObj->GetConfigName();
	fGFilterType          = iDCSFEEObj->GetFilterType();
	fGReadoutParam        = iDCSFEEObj->GetReadoutParam();
	fGTestPattern         = iDCSFEEObj->GetTestPattern();
	fGTrackletMode        = iDCSFEEObj->GetTrackletMode();
	fGTrackletDef         = iDCSFEEObj->GetTrackletDef();
	fGTriggerSetup        = iDCSFEEObj->GetTriggerSetup();
	fGAddOptions          = iDCSFEEObj->GetAddOptions();
	break;
      }
    }
  }

  for(Int_t i=0; i<540; i++) {
    AliTRDCalDCSFEEv2 *iDCSFEEObj;
    iDCSFEEObj = GetCalDCSFEEObj(i);
    if(iDCSFEEObj != NULL) {
      if(iDCSFEEObj->GetStatusBit() == 0) {
	// second, if any of the other good chambers differ, set the global value to -1/""
	if(fGNumberOfTimeBins    != iDCSFEEObj->GetNumberOfTimeBins())    fGNumberOfTimeBins    = -2;
	if(fGConfigTag           != iDCSFEEObj->GetConfigTag())           fGConfigTag           = -2;
	if(fGSingleHitThres      != iDCSFEEObj->GetSingleHitThres())      fGSingleHitThres      = -2;
	if(fGThreePadClustThres  != iDCSFEEObj->GetThreePadClustThres())  fGThreePadClustThres  = -2;
	if(fGSelNoZS             != iDCSFEEObj->GetSelectiveNoZS())       fGSelNoZS             = -2;
	if(fGTCFilterWeight      != iDCSFEEObj->GetTCFilterWeight())      fGTCFilterWeight      = -2;
	if(fGTCFilterShortDecPar != iDCSFEEObj->GetTCFilterShortDecPar()) fGTCFilterShortDecPar = -2;
	if(fGTCFilterLongDecPar  != iDCSFEEObj->GetTCFilterLongDecPar())  fGTCFilterLongDecPar  = -2;
	if(fGFastStatNoise       != iDCSFEEObj->GetFastStatNoise())       fGFastStatNoise       = -2;
	if(fGConfigVersion       != iDCSFEEObj->GetConfigVersion())       fGConfigVersion       = "mixed";
	if(fGConfigName          != iDCSFEEObj->GetConfigName())          fGConfigName          = "mixed";
	if(fGFilterType          != iDCSFEEObj->GetFilterType())          fGFilterType          = "mixed";
	if(fGReadoutParam        != iDCSFEEObj->GetReadoutParam())        fGReadoutParam        = "mixed";
	if(fGTestPattern         != iDCSFEEObj->GetTestPattern())         fGTestPattern         = "mixed";
	if(fGTrackletMode        != iDCSFEEObj->GetTrackletMode())        fGTrackletMode        = "mixed";
	if(fGTrackletDef         != iDCSFEEObj->GetTrackletDef())         fGTrackletDef         = "mixed";
	if(fGTriggerSetup        != iDCSFEEObj->GetTriggerSetup())        fGTriggerSetup        = "mixed";
	if(fGAddOptions          != iDCSFEEObj->GetAddOptions())          fGAddOptions          = "mixed";
      }
    }
  }
}


