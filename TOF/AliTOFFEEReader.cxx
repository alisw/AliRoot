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
***************************************************************************/

/* 
 * author: Roberto Preghenella (R+), Roberto.Preghenella@bo.infn.it
 */

//////////////////////////////////////////////////////////////////////
//                                                                  //
//                                                                  //
//   This class provides the TOF FEE reader.                        //
//                                                                  //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#include "AliTOFFEEReader.h"
#include "AliTOFFEEConfig.h"
#include "AliTOFRawStream.h"
#include "AliLog.h"
#include <fstream>

ClassImp(AliTOFFEEReader)

//_______________________________________________________________

AliTOFFEEReader::AliTOFFEEReader() :
  TObject(),
  fFEEConfig(new AliTOFFEEConfig()),
  fChannelEnabled()
{
  /* 
   * 
   * default constructor 
   *
   */

}

//_______________________________________________________________

AliTOFFEEReader::AliTOFFEEReader(const AliTOFFEEReader &source) :
  TObject(source),
  fFEEConfig(new AliTOFFEEConfig())
{
  /* 
   * 
   * copy constructor 
   *
   */

  memcpy(fFEEConfig, source.fFEEConfig, sizeof(AliTOFFEEConfig));
}

//_______________________________________________________________

AliTOFFEEReader &
AliTOFFEEReader::operator=(const AliTOFFEEReader &source)
{
  /* 
   * 
   * operator = 
   * 
   */

  TObject::operator=(source);
  memcpy(fFEEConfig, source.fFEEConfig, sizeof(AliTOFFEEConfig));
  return *this;
}

//_______________________________________________________________

AliTOFFEEReader::~AliTOFFEEReader()
{
  /* 
   *
   * default destructor 
   *
   */

  delete fFEEConfig;
}

//_______________________________________________________________

void
AliTOFFEEReader::ResetChannelEnabledArray()
{
  /*
   *
   * reset channel enabled array
   *
   */

  for (Int_t iIndex = 0; iIndex < GetNumberOfIndexes(); iIndex++)
    fChannelEnabled[iIndex] = kFALSE;
}

//_______________________________________________________________

void
AliTOFFEEReader::LoadFEEConfig(const Char_t *FileName)
{
  /*
   *
   * load FEE config
   *
   */

  std::ifstream is;
  is.open(FileName, std::ios::binary);
  is.read((Char_t *)fFEEConfig, sizeof(AliTOFFEEConfig));
  is.close();
}

//_______________________________________________________________

Int_t
AliTOFFEEReader::ParseFEEConfig()
{
  /* 
   *
   * parse FEE config
   *
   * loops over all FEE channels, checks whether they are enabled
   * and sets channel enabled 
   *
   */

  AliInfo("parsing TOF FEE config")

  AliTOFRawStream rawStream;
  Int_t nEnabled = 0;
  Int_t volume[5], index;
  Int_t temp;

  ResetChannelEnabledArray();

  /* loop over all FEE channels */
  for (Int_t iDDL = 0; iDDL < GetNumberOfDDLs(); iDDL++)
    for (Int_t iTRM = 0; iTRM < GetNumberOfTRMs(); iTRM++)
      for (Int_t iChain = 0; iChain < GetNumberOfChains(); iChain++)
	for (Int_t iTDC = 0; iTDC < GetNumberOfTDCs(); iTDC++)
	  for (Int_t iChannel = 0; iChannel < GetNumberOfChannels(); iChannel++)
	    /* check whether FEE channel is enabled */
	    if (IsChannelEnabled(iDDL, iTRM + 3, iChain, iTDC, iChannel)) {
	      /* convert FEE channel indexes into detector indexes */
	      rawStream.EquipmentId2VolumeId(iDDL, iTRM + 3, iChain, iTDC, iChannel, volume);
	      /* swap padx and padz to fit AliTOFGeometry::GetIndex behaviour */
	      temp = volume[4]; volume[4] = volume[3]; volume[3] = temp; 
	      /* convert detector indexes into calibration index */
	      index = AliTOFGeometry::GetIndex(volume);
	      /* check calibration index */
	      if (index != -1 && index < GetNumberOfIndexes()) {
		/* set calibration channel enabled */
		fChannelEnabled[index] = kTRUE;
		nEnabled++;
	      }
	    }
  return nEnabled;
}

//_______________________________________________________________

Bool_t 
AliTOFFEEReader::IsChannelEnabled(Int_t iDDL, Int_t iTRM, Int_t iChain, Int_t iTDC, Int_t iChannel)
{
  /*
   *
   * is channel enabled
   *
   * checks whether a FEE channel is enabled using the
   * TOF FEE config object.
   *
   */

  AliTOFFEEConfig *feeConfig;
  AliTOFCrateConfig *crateConfig;
  AliTOFTRMConfig *trmConfig;
  Int_t maskPB, maskTDC;
  
  /* get and check fee config */
  if (!(feeConfig = GetFEEConfig()))
    return kFALSE;
  
  /* get and check crate config */
  if (!(crateConfig = feeConfig->GetCrateConfig(iDDL)))
    return kFALSE;
  
  /* get and check TRM config */
  if (!(trmConfig = crateConfig->GetTRMConfig(iTRM - 3)))
    return kFALSE;

  /* check DRM enabled */
  if (!crateConfig->IsDRMEnabled())
    return kFALSE;

  /* check TRM enabled */
  if (!crateConfig->IsTRMEnabled(iTRM - 3))
    return kFALSE;

  /* switch chain */
  switch (iChain) {
    /* chain A */
  case 0:
    /* check chain enabled */
    if (trmConfig->GetChainAFlag() != 1)
      return kFALSE;
    /* switch TDC */
    switch (iTDC) {
    case 0: case 1: case 2:
      maskPB = trmConfig->GetMaskPB0();
      break;
    case 3: case 4: case 5:
      maskPB = trmConfig->GetMaskPB1();
      break;
    case 6: case 7: case 8:
      maskPB = trmConfig->GetMaskPB2();
      break;
    case 9: case 10: case 11:
      maskPB = trmConfig->GetMaskPB3();
      break;
    case 12: case 13: case 14:
      maskPB = trmConfig->GetMaskPB4();
      break;
    default:
      return kFALSE;
      break;  
    } /* switch TDC */
    break; /* chain A */
    /* chain B */
  case 1:
    /* check chain enabled */
    if (trmConfig->GetChainBFlag() != 1)
      return kFALSE;
    /* switch TDC */
    switch (iTDC) {
    case 0: case 1: case 2:
      maskPB = trmConfig->GetMaskPB5();
      break;
    case 3: case 4: case 5:
      maskPB = trmConfig->GetMaskPB6();
      break;
    case 6: case 7: case 8:
      maskPB = trmConfig->GetMaskPB7();
      break;
    case 9: case 10: case 11:
      maskPB = trmConfig->GetMaskPB8();
      break;
    case 12: case 13: case 14:
      maskPB = trmConfig->GetMaskPB9();
      break;
    default:
      return kFALSE;
      break;  
    } /* switch TDC */
    break; /* chain B */
  default:
    return kFALSE;
    break;
  } /* switch chain */

  /* check channel enabled */
  maskTDC = (maskPB & (0xFF << ((iTDC % 3) * 8))) >> ((iTDC % 3) * 8);
  if (maskTDC & (0x1 << iChannel))
    return kTRUE;
  else
    return kFALSE;
  
}


void
AliTOFFEEReader::DumpFEEConfig()
{
  /*
   * 
   * dump FEE config
   *
   */

  AliTOFFEEConfig *feeConfig = GetFEEConfig();
  AliTOFCrateConfig *crateConfig;
  AliTOFDRMConfig *drmConfig;
  AliTOFLTMConfig *ltmConfig;
  AliTOFTRMConfig *trmConfig;

  AliInfo("-------------------------------------");
  AliInfo("dumping TOF FEE config");
  AliInfo("-------------------------------------");
  AliInfo(Form("version: %d", feeConfig->GetVersion()));
  AliInfo(Form("dump time: %d", feeConfig->GetDumpTime()));
  AliInfo(Form("run number: %d", feeConfig->GetRunNumber()));
  AliInfo(Form("run type: %d", feeConfig->GetRunType()));
  AliInfo("-------------------------------------");
  
  /* loop over crates */
  for (Int_t iCrate = 0; iCrate < AliTOFFEEConfig::GetNumberOfCrates(); iCrate++) {
    crateConfig = feeConfig->GetCrateConfig(iCrate);
    
    /* check crate config */
    if (!crateConfig)
      continue;
    
    /* check DRM enabled */
    if (!crateConfig->IsDRMEnabled())
    continue;

    AliInfo(Form("crate id: %02d", iCrate));

    /* dump DRM config */
    drmConfig = crateConfig->GetDRMConfig();
    AliInfo(Form("DRM is enabled: drmId=%d, slotMask=%03x", drmConfig->GetDRMId(), drmConfig->GetSlotMask()));

    /* dump LTM config if enabled */
    if (crateConfig->IsLTMEnabled()) {
      ltmConfig = crateConfig->GetLTMConfig();
      AliInfo(Form("LTM is enabled: threshold=%d", ltmConfig->GetThreshold()));
    }
    
    /* dump CPDM config if enabled */
    if (crateConfig->IsCPDMEnabled()) {
      AliInfo(Form("CPDM is enabled"));
    }
    
    /* loop over TRMs */
    for (Int_t iTRM = 0; iTRM < AliTOFCrateConfig::GetNumberOfTRMs(); iTRM++) {

      trmConfig = crateConfig->GetTRMConfig(iTRM);

      /* check TRM config */
      if (!trmConfig)
	continue;
      
      /* check TRM enabled */
      if (!crateConfig->IsTRMEnabled(iTRM))
	continue;

      /* dump TRM config */
      AliInfo(Form("TRM%02d is enabled: matchWin=%d, latWin=%d, packFlag=%d", iTRM + 3, trmConfig->GetMatchingWindow(), trmConfig->GetLatencyWindow(), trmConfig->GetPackingFlag()));
      
      /* check TRM chain A flag */
      if (trmConfig->GetChainAFlag() == 1) {
	AliInfo(Form("TRM%02d chainA is enabled: PB0=%06X, PB1=%06X, PB2=%06X, PB3=%06X, PB4=%06X", iTRM + 3, trmConfig->GetMaskPB0(), trmConfig->GetMaskPB1(), trmConfig->GetMaskPB2(), trmConfig->GetMaskPB3(), trmConfig->GetMaskPB4()));
      }

      /* check TRM chain B flag */
      if (trmConfig->GetChainBFlag() == 1) {
	AliInfo(Form("TRM%02d chainB is enabled: PB5=%06X, PB6=%06X, PB7=%06X, PB8=%06X, PB9=%06X", iTRM + 3, trmConfig->GetMaskPB5(), trmConfig->GetMaskPB6(), trmConfig->GetMaskPB7(), trmConfig->GetMaskPB8(), trmConfig->GetMaskPB9()));
      }
      

      
    } /* loop over TRMs */
    AliInfo("-------------------------------------");
  } /* loop over crates */
}

