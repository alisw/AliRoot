#ifndef ALIHLTPHOSVALIDCELLDATASTRUCT_H
#define ALIHLTPHOSVALIDCELLDATASTRUCT_H 

/***************************************************************************
 * Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved.       *
 *                                                                         *
 * Author: Per Thomas Hille <perthi@fys.uio.no> for the ALICE HLT Project. *
 * Contributors are mentioned in the code where appropriate.               *
 *                                                                         *
 * Permission to use, copy, modify and distribute this software and its    *
 * documentation strictly for non-commercial purposes is hereby granted    *
 * without fee, provided that the above copyright notice appears in all    *
 * copies and that both the copyright notice and this permission notice    *
 * appear in the supporting documentation. The authors make no claims      *
 * about the suitability of this software for any purpose. It is           *
 * provided "as is" without express or implied warranty.                   *
 **************************************************************************/

#include "AliHLTDataTypes.h"
#include "Rtypes.h"

struct AliHLTPHOSValidCellDataStruct
{
  //  AliHLTUInt16_t fRow;
  //  AliHLTUInt16_t fCol;

  //  AliHLTUInt16_t fZ;
  // AliHLTUInt16_t fX;

  //  AliHLTUInt16_t fGain;

  //  int fZ;
  //  int fX;

  //  Int_t fZ;
  //  Int_t fX;
  AliHLTUInt8_t fZ;
  AliHLTUInt8_t fX;

  //  AliHLTUInt16_t fGain;
  ///  int fGain;
  //  Int_t fGain;
  AliHLTUInt8_t fGain;

  Float_t fEnergy;
  Float_t fTime;
};


#endif
