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

// This class handles the mapping of the Altro channels in the PHOS/EMCAL
// The mapping is read from an external mapping files
// Author: C.Cheshkov

/// Exported from PHOS to be used also by EMCAL
/// November 2006 Gustavo Conesa Balbastre

#include "AliCaloAltroMapping.h"
#include "AliLog.h"
#include <Riostream.h>
//#include <stdlib.h>


ClassImp(AliCaloAltroMapping)

//_____________________________________________________________________________
AliCaloAltroMapping::AliCaloAltroMapping():
  AliAltroMapping(),
  fMinRow(0),
  fMaxRow(0),
  fMinCol(0),
  fMaxCol(0),
  fMapping(NULL),
  fInvMappingLow(NULL),
  fInvMappingHigh(NULL)
{
  // Default constructor
}

//_____________________________________________________________________________
AliCaloAltroMapping::AliCaloAltroMapping(const char *mappingFile):
  AliAltroMapping(mappingFile),
  fMinRow(0),
  fMaxRow(0),
  fMinCol(0),
  fMaxCol(0),
  fMapping(NULL),
  fInvMappingLow(NULL),
  fInvMappingHigh(NULL)
{
  // Constructor
  ReadMapping();
  CloseMappingFile();
}

//_____________________________________________________________________________
AliCaloAltroMapping::~AliCaloAltroMapping()
{
  // destructor
  DeleteMappingArrays();
}

//_____________________________________________________________________________
Bool_t AliCaloAltroMapping::ReadMapping()
{
  // Initalizes the ALTRO mapping from a file
  // Look at the Calo module for the format of
  // the mapping file
  if (!fIn) {
    AliFatal("Mapping file has not been opened !");
    return kFALSE;
  }

  fMinRow = 0x7fffffff;
  fMaxRow = 0;
  fMinCol = 0x7fffffff;
  fMaxCol = 0;
  fMappingSize = 3*(fMaxHWAddress+1);
  fMapping = new Short_t[fMappingSize];
  for (Int_t i = 0; i <= fMaxHWAddress; i++) {
    fMapping[3*i] = fMapping[3*i+1] = fMapping[3*i+2] = -1;
  }
 
  for(Int_t i = 0; i < fNumberOfChannels ; i++) { // 1792 = 2*896 channels connected to each RCU
    Int_t hwAddress;
    if (!(*fIn >> hwAddress)) {
      AliFatal("Syntax of the mapping file is wrong !");
      return kFALSE;
    }
    if (hwAddress > fMaxHWAddress) {
      AliFatal(Form("Hardware (ALTRO) adress (%d) outside the range (0 -> %d) !",hwAddress,fMaxHWAddress));
      return kFALSE;
    }
    Int_t row,col,gain;
    if (!(*fIn >> row >> col >> gain)) {
      AliFatal("Syntax of the mapping file is wrong !");
      return kFALSE;
    }

    if (gain < 0 || gain > 1) {
      AliFatal(Form("Wrong gain value found (%d)! Should be 0 or 1 !",gain));
      return kFALSE;
    }
 
    fMapping[3*hwAddress] = row;
    fMapping[3*hwAddress+1] = col;
    fMapping[3*hwAddress+2] = gain;

    if (row > fMaxRow) fMaxRow = row;
    if (row < fMinRow) fMinRow = row;
    if (col > fMaxCol) fMaxCol = col;
    if (col < fMinCol) fMinCol = col;

  }

  Int_t nRows = fMaxRow - fMinRow + 1;
  Int_t nCols = fMaxCol - fMinCol + 1;
  fInvMappingSize = nRows*nCols;


  fInvMappingLow  = new Short_t[fInvMappingSize];
  fInvMappingHigh = new Short_t[fInvMappingSize];
  for (Int_t i = 0; i < nRows; i++) {
    for (Int_t j = 0; j < nCols; j++) {
      fInvMappingLow[nCols*i+j]  = -1;
      fInvMappingHigh[nCols*i+j] = -1;
    }
  }

  for(Int_t i = 0; i <= fMaxHWAddress; i++) {
    Int_t row = fMapping[3*i];
    Int_t col = fMapping[3*i+1];
    Int_t gain = fMapping[3*i+2];
    if(row != -1 && col != -1) {
      if (gain == 0)
	fInvMappingLow[nCols*(row-fMinRow)+(col-fMinCol)] = i;
      if (gain == 1)
	fInvMappingHigh[nCols*(row-fMinRow)+(col-fMinCol)] = i;
    }
  }

  return kTRUE;
}

//_____________________________________________________________________________
Int_t AliCaloAltroMapping::GetHWAddress(Int_t row, Int_t col, Int_t gain) const
{
  // Get the content of the mapping array
  // return -1 in case there is no hardware
  // adress defined for these row-column-gain
  if (!fInvMappingLow || !fInvMappingHigh) {
    AliWarning("Mapping array was not initalized correctly !");
    return -1;
  }
  if (row < fMinRow || row > fMaxRow) {
    AliWarning(Form("Index of row (%d) outside the range (%d -> %d) !",row,fMinRow,fMaxRow));
    return -1;
  }
  if (col < fMinCol || col > fMaxCol) {
    AliWarning(Form("Index of column (%d) outside the range (0 -> %d) !",col,fMinCol,fMaxCol));
    return -1;
  }
  if (gain < 0 || gain > 1) {
    AliWarning(Form("Invalid gain (%d)! Should be 0 or 1 !",gain));
    return -1;
  }
  Int_t hwAddress = -1;
  if (gain == 0)
    hwAddress = fInvMappingLow[(fMaxCol - fMinCol + 1)*(row-fMinRow)+(col-fMinCol)];
  if (gain == 1)
    hwAddress = fInvMappingHigh[(fMaxCol - fMinCol + 1)*(row-fMinRow)+(col-fMinCol)];

  if (hwAddress == -1)
    AliWarning(Form("Hardware (ALTRO) adress is not defined for these row (%d), column (%d) and gain (%d) !",row,col,gain));

  return hwAddress;
}

//_____________________________________________________________________________
Int_t AliCaloAltroMapping::GetPadRow(Int_t hwAddress) const
{
  // Return the row index
  // Note the difference w.r.t to the base class notation
  if (!fMapping) {
    AliWarning("Mapping array was not initalized correctly !");
    return -1;
  }
  if (hwAddress > fMaxHWAddress) {
    AliWarning(Form("Hardware (ALTRO) adress (%d) outside the range (0 -> %d) !",hwAddress,fMaxHWAddress));
    return -1;
  }
  Int_t row = fMapping[3*hwAddress];
  if (row == -1)
    AliWarning(Form("Hardware (ALTRO) adress (%d) is not defined !",hwAddress));

  return row;
}

//_____________________________________________________________________________
Int_t AliCaloAltroMapping::GetPad(Int_t hwAddress) const
{
  // Return the column index
  // Note the difference w.r.t to the base class notation
  if (!fMapping) {
    AliWarning("Mapping array was not initalized correctly !");
    return -1;
  }
  if (hwAddress > fMaxHWAddress) {
    AliWarning(Form("Hardware (ALTRO) adress (%d) outside the range (0 -> %d) !",hwAddress,fMaxHWAddress));
    return -1;
  }
  Int_t col = fMapping[3*hwAddress+1];
  if (col == -1)
    AliWarning(Form("Hardware (ALTRO) adress (%d) is not defined !",hwAddress));

  return col;
}

//_____________________________________________________________________________
Int_t AliCaloAltroMapping::GetSector(Int_t hwAddress) const
{
  // Return the gain factor (0/1)
  // Note the difference w.r.t to the base class notation
  if (!fMapping) {
    AliWarning("Mapping array was not initalized correctly !");
    return -1;
  }
  if (hwAddress > fMaxHWAddress) {
    AliWarning(Form("Hardware (ALTRO) adress (%d) outside the range (0 -> %d) !",hwAddress,fMaxHWAddress));
    return -1;
  }
  Int_t gain = fMapping[3*hwAddress+2];
  if (gain == -1)
    AliWarning(Form("Hardware (ALTRO) adress (%d) is not defined !",hwAddress));

  return gain;
}

//_____________________________________________________________________________
void AliCaloAltroMapping::DeleteMappingArrays()
{
  // Deletes the arrays which have been
  // allocated during the reading of the
  // mapping file
  if (fMapping) delete [] fMapping;

  if (fInvMappingLow) delete [] fInvMappingLow;

  if (fInvMappingHigh) delete [] fInvMappingHigh;
}
