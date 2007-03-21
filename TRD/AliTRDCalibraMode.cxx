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

/////////////////////////////////////////////////////////////////////////////////
//                                                                             
// AliTRDCalibraMode                                                             
//                                                                             
// This class is for the modes of the TRD calibration.        
// The user has to choose with the functions SetNz and SetNrphi the precision of the calibration. 
//Begin_Html
/*
<br>
<CENTER>
<TABLE border=1>
<TR><TD><center>Nz</center></TD><TD><center> 0 </center></TD><TD><center> 1 </center></TD><TD><center> 2 </center></TD><TD><center> 3 </center></TD><TD><center> 4 </center></TD></TR>
<TR><TD><CENTER>group of row pads per detector</CENTER></TD><TD><CENTER>1</CENTER></TD><TD><CENTER>2</CENTER></TD><TD><CENTER>4</CENTER></TD><TD><CENTER>6(chamb2)<br> 8(others chambers)</CENTER></TD><TD><CENTER>12 (chamb2)<br> 16 (chamb0)</CENTER></TD></TR>
<TR><TD><CENTER>row pads per group</CENTER></TD><TD><CENTER>12 (chamb2)<br> 16 (chamb0)</CENTER></TD><TD><CENTER>6 (chamb2)<br> 8 (chamb0)</CENTER></TD><TD><CENTER>3 (chamb2)<br> 4 (chamb0)</CENTER></TD><TD><CENTER>2</CENTER></TD><TD><CENTER>1</CENTER></TD></TR>
<TR><TD><CENTER>~distance [cm]</CENTER></TD><TD><CENTER>106 (chamb2)<br> 130 (chamb0)</CENTER></TD><TD><CENTER>53 (chamb2)<br> 65 (chamb0)</CENTER></TD><TD><CENTER>26.5 (chamb2)<br> 32.5 (chamb0)</CENTER></TD><TD><CENTER>17 (chamb2)<br> 17 (chamb0)</CENTER></TD><TD><CENTER>9 (chamb2)<br> 9 (chamb0)</CENTER></TD></TR>
<CAPTION>In the z direction</CAPTION>
</TABLE>
</CENTER>
<CENTER>
<br>
<TABLE border=1>
<TR><TD><center>Nrphi</center></TD><TD><center> 0 </center></TD><TD><center> 1 </center></TD><TD><center> 2 </center></TD><TD><center> 3 </center></TD><TD><center> 4 </center></TD><TD><center> 5 </center></TD><TD><center> 6 </center></TD></TR>
<TR><TD><CENTER>group of col pads per detector</CENTER></TD><TD><CENTER>1</CENTER></TD><TD><CENTER>2</CENTER></TD><TD><CENTER>4</CENTER></TD><TD><CENTER>8</CENTER></TD><TD><CENTER>16</CENTER></TD><TD><center>36</center></TD><TD><center>144</center></TD></TR>
<TR><TD><CENTER>col pads per group</CENTER></TD><TD><CENTER>144</CENTER></TD><TD><CENTER>72</CENTER></TD><TD><CENTER>36</CENTER></TD><TD><CENTER>18</CENTER></TD><TD><CENTER>9</CENTER></TD><TD><center>4</center></TD><TD><center>1</center></TD></TR>
<TR><TD><CENTER>~distance [cm]</CENTER></TD><TD><CENTER>113.4</CENTER></TD><TD><CENTER>56.7</CENTER></TD><TD><CENTER>25.3</CENTER></TD><TD><CENTER>14.3</CENTER></TD><TD><CENTER>7.25</CENTER></TD><TD><center>3.2</center></TD><TD><center>0.8</center></TD></TR>
<CAPTION>In the rphi direction</CAPTION>
</TABLE>
</CENTER>
<br>
*/
//End_Html 
//
//                            
// Author:
//   R. Bailhache (R.Bailhache@gsi.de)
//                            
//////////////////////////////////////////////////////////////////////////////////////

#include "AliLog.h"

#include "AliTRDCalibraMode.h"
#include "AliTRDCommonParam.h"

ClassImp(AliTRDCalibraMode)

//______________________________________________________________________________________
AliTRDCalibraMode::AliTRDCalibraMode()
  :TObject()
{
  //
  // Default constructor
  //

  for (Int_t i = 0; i < 3; i++) {
    fNz[i]    = 0;
    fNrphi[i] = 0;
  }

  for (Int_t k = 0; k < 3; k++) {
    fDetChamb2[k] = 0;
    fDetChamb0[k] = 0;
  }

  for (Int_t i = 0; i < 3; i++) {
    fRowMin[i]    = -1;
    fRowMax[i]    = -1;
    fColMax[i]    = -1;
    fColMin[i]    = -1;
    fNnZ[i]       = -1;
    fNnRphi[i]    = -1;
    fNfragZ[i]    = -1;
    fNfragRphi[i] = -1;
    fXbins[i]     = -1;
  }
   
}

//______________________________________________________________________________________
AliTRDCalibraMode::AliTRDCalibraMode(const AliTRDCalibraMode &c)
  :TObject(c)
{
  //
  // Copy constructor
  //

}

//____________________________________________________________________________________
AliTRDCalibraMode::~AliTRDCalibraMode()
{
  //
  // AliTRDCalibraMode destructor
  //

}

//_____________________________________________________________________________
void AliTRDCalibraMode::SetNz(Int_t i, Short_t Nz)
{
  //
  // Set the mode of calibration group in the z direction for the parameter i
  // 

  if ((Nz >= 0) && 
      (Nz <  5)) {
    fNz[i] = Nz; 
  }
  else { 
    AliInfo("You have to choose between 0 and 4");
  }

}

//_____________________________________________________________________________
void AliTRDCalibraMode::SetNrphi(Int_t i, Short_t Nrphi)
{
  //
  // Set the mode of calibration group in the rphi direction for the parameter i
  //
 
  if ((Nrphi >= 0) && 
      (Nrphi <  7)) {
    fNrphi[i] = Nrphi; 
  }
  else {
    AliInfo("You have to choose between 0 and 6");
  }

}

//_______________________________________________________________________________________
void AliTRDCalibraMode::ModePadCalibration(Int_t iChamb, Int_t i)
{
  //
  // Definition of the calibration mode
  // from Nz and Nrphi, the number of row and col pads per calibration groups are setted
  //


  fNnZ[i]    = 0;
  fNnRphi[i] = 0;
  
  if ((fNz[i] == 0) && (iChamb == 2)) {
    fNnZ[i] = 12;
  }
  if ((fNz[i] == 0) && (iChamb != 2)) {
    fNnZ[i] = 16;
  }  
  if ((fNz[i] == 1) && (iChamb == 2)) {
    fNnZ[i] = 6;
  }
  if ((fNz[i] == 1) && (iChamb != 2)) {
    fNnZ[i] = 8;
  }
  if ((fNz[i] == 2) && (iChamb == 2)) {
    fNnZ[i] = 3;
  }
  if ((fNz[i] == 2) && (iChamb != 2)) {
    fNnZ[i] = 4;
  }
  if (fNz[i] == 3) {
    fNnZ[i] = 2;
  }
  if (fNz[i] == 4) {
    fNnZ[i] = 1;
  }
   
  if (fNrphi[i] == 0) {
    fNnRphi[i] = 144;
  }
  if (fNrphi[i] == 1) {
    fNnRphi[i] = 72;
  } 
  if (fNrphi[i] == 2) {
    fNnRphi[i] = 36;
  } 
  if (fNrphi[i] == 3) {
    fNnRphi[i] = 18;
  } 
  if (fNrphi[i] == 4) {
    fNnRphi[i] = 9;
  } 
  if (fNrphi[i] == 5) {
    fNnRphi[i] = 4;
  } 
  if (fNrphi[i] == 6) {
    fNnRphi[i] = 1;
  } 

}

//_____________________________________________________________________________________________
Bool_t AliTRDCalibraMode::ModePadFragmentation(Int_t iPlane,Int_t iChamb, Int_t iSect, Int_t i)
{
  //
  // Definition of the calibration mode
  // From the number of row and col pads per calibration groups the
  // number of calibration groups are setted
  //

  fNfragZ[i]    = 0;
  fNfragRphi[i] = 0;
  
  AliTRDCommonParam *parCom = AliTRDCommonParam::Instance();
  if (!parCom) {
    AliInfo("Could not get CommonParam Manager");
    return kFALSE;
  }

  // A little geometry:
  Int_t rowMax = parCom->GetRowMax(iPlane,iChamb,iSect);
  Int_t colMax = parCom->GetColMax(iPlane);
  
  // The fragmentation
  if (fNnZ[i]    != 0) {
    fNfragZ[i]    = (Int_t) rowMax / fNnZ[i];
  }

  if (fNnRphi[i] != 0) {
    fNfragRphi[i] = (Int_t) colMax / fNnRphi[i];
  }

  return kTRUE;

}

//_____________________________________________________________________________
void AliTRDCalibraMode::ReconstructionRowPadGroup(Int_t idect, Int_t i)
{
  //
  // For the calibration group idect in a detector calculate the
  // first and last row pad and col pad.
  // The pads in the interval will have the same calibrated coefficients
  //

  Int_t posc = -1;
  Int_t posr = -1;
  fRowMin[i] = -1;
  fRowMax[i] = -1;
  fColMin[i] = -1;
  fColMax[i] = -1;
  
  if (fNfragZ[i]    != 0) {
    posc = (Int_t) idect / fNfragZ[i];
  }
  if (fNfragRphi[i] != 0) {
    posr = (Int_t) idect % fNfragZ[i];
  }
  fRowMin[i] = posr     * fNnZ[i];
  fRowMax[i] = (posr+1) * fNnZ[i];
  fColMin[i] = posc     * fNnRphi[i];
  fColMax[i] = (posc+1) * fNnRphi[i];

}

//_____________________________________________________________________________
void AliTRDCalibraMode::CalculXBins(Int_t idect, Int_t i)
{
  //
  // For the detector idect calcul the first Xbins
  //

  fXbins[i] = 0;
  AliDebug(2, Form("detector: %d", idect));

  // In which sector?
  Int_t sector = GetSector(idect);
  fXbins[i] += sector*(6*fDetChamb2[i]+6*4*fDetChamb0[i]);
 
  // In which chamber?
  Int_t chamber = GetChamber(idect);
  Int_t kc      = 0;
  while (kc < chamber) {
    if (kc == 2) {
      fXbins[i] += 6 * fDetChamb2[i];
    }
    else {
      fXbins[i] += 6 * fDetChamb0[i];
    }
    kc ++;
  }
  
  // In which plane?
  Int_t plane = GetPlane(idect);
  if (chamber == 2) {
    fXbins[i] += plane*fDetChamb2[i];
  }
  else {
    fXbins[i] += plane*fDetChamb0[i];
  }
 
}

//_____________________________________________________________________________
void AliTRDCalibraMode::ResetMinMax(Int_t i)
{
  //
  // Reset fRowMin fRowMax fColMin fColMax [i]
  //

  fRowMin[i] = -1;
  fRowMax[i] = -1;
  fColMin[i] = -1;
  fColMax[i] = -1;
 
}

//_____________________________________________________________________________
void AliTRDCalibraMode::SetDetChamb0(Int_t i)
{
  //
  // Set the number of calibration group per detector != 2
  //

  fDetChamb0[i] = fNfragZ[i] * fNfragRphi[i];  
 
}

//_____________________________________________________________________________
void AliTRDCalibraMode::SetDetChamb2(Int_t i)
{
  //
  // Set the number of calibration group per detector == 2
  //

  fDetChamb2[i] = fNfragZ[i] * fNfragRphi[i];  
 
}

//_____________________________________________________________________________
Int_t AliTRDCalibraMode::GetPlane(Int_t d) const
{
  //
  // Reconstruct the plane number from the detector number
  //

  return ((Int_t) (d % 6));

}

//_____________________________________________________________________________
Int_t AliTRDCalibraMode::GetChamber(Int_t d) const
{
  //
  // Reconstruct the chamber number from the detector number
  //

  Int_t fgkNplan = 6;

  return ((Int_t) (d % 30) / fgkNplan);

}

//_____________________________________________________________________________
Int_t AliTRDCalibraMode::GetSector(Int_t d) const
{
  //
  // Reconstruct the sector number from the detector number
  //

  Int_t fg = 30;

  return ((Int_t) (d / fg));

}
