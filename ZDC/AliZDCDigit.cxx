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

//_________________________________________________________________________
//
//
//   ZDC digit = ADC Channels for each PM 
//
//_________________________________________________________________________

#include "AliZDCDigit.h"


ClassImp(AliZDCDigit)

//____________________________________________________________________________
  AliZDCDigit::AliZDCDigit() 
{
  // Default constructor 
  
  fSector[0]   = 0;
  fSector[1]   = 0;
  fADCValue = 0;  
}

//____________________________________________________________________________
AliZDCDigit::AliZDCDigit(Int_t *Sector, Int_t ADCValue) 
{  
  // Constructor 
 
  Int_t i;
  for(i=0; i<2; i++) {
     fSector[i] = Sector[i];
  }
  fADCValue = ADCValue;  
}

//____________________________________________________________________________
AliZDCDigit::AliZDCDigit(const AliZDCDigit & digit):TObject(digit)
{
  // Copy constructor

  fSector[0]   = digit.fSector[0];           
  fSector[1]   = digit.fSector[1];           
  fADCValue = digit.fADCValue;             

}
