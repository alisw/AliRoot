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

//_________________________________________________________________________
//  EMCAL digit: Id
//              energy
//              3 identifiers for the primary particle(s) at the origine of the digit
//  The digits are made in FinishEvent() by summing all the hits in a single EMCAL crystal or PPSD gas cell
//  It would be nice to replace the 3 identifiers by an array, but, because digits are kept in a TClonesQArray,
//   it is not possible to stream such an array... (beyond my understqnding!)
//
//*-- Author: Laurent Aphecetche & Yves Schutz (SUBATECH)


// --- ROOT system ---

// --- Standard library ---

#include <iostream.h>

// --- AliRoot header files ---

#include "AliEMCALDigit.h"


ClassImp(AliEMCALDigit)

//____________________________________________________________________________
  AliEMCALDigit::AliEMCALDigit() 
{
  // default ctor 

  fIndexInList = -1 ; 
  fNprimary    = 0 ;  
  fNMaxPrimary = 5 ; 
  fNiparent     = 0 ;
  fNMaxiparent = fNMaxPrimary*10;
}

//____________________________________________________________________________
AliEMCALDigit::AliEMCALDigit(Int_t primary, Int_t iparent, Int_t id, Int_t DigEnergy, Int_t index) 
{  
  // ctor with all data 

  fNMaxPrimary = 5 ; 
  fNMaxiparent = fNMaxPrimary*10;
  fAmp         = DigEnergy ;
  fId          = id ;
  fIndexInList = index ; 
  if( primary != -1){
    fNprimary    = 1 ; 
    fPrimary[0]  = primary ;
    fNiparent   = 1 ;
    fIparent[0] = iparent ;    

  }
  else{  //If the contribution of this primary smaller than fDigitThreshold (AliEMCALv1)
    fNprimary    = 0 ; 
    fPrimary[0]  = -1 ;
    fNiparent   = 0 ;
    fIparent[0] = -1 ;    

  }
  Int_t i ;
  for ( i = 1; i < fNMaxPrimary ; i++)
    fPrimary[i]  = -1 ; 

  for ( Int_t j =1; j< fNMaxiparent ; j++)
    fIparent[j] = -1 ;  
}

//____________________________________________________________________________
AliEMCALDigit::AliEMCALDigit(const AliEMCALDigit & digit) 
{
  // copy ctor
  

  fNMaxPrimary = digit.fNMaxPrimary ;  
  fNMaxiparent = digit.fNMaxiparent ;
  Int_t i ;
  for ( i = 0; i < fNMaxPrimary ; i++)
  fPrimary[i]  = digit.fPrimary[i] ;
  Int_t j ;
  for (j = 0; j< fNMaxiparent ; j++)
  fIparent[j]  = digit.fIparent[j] ;
  fAmp         = digit.fAmp ;
  fId          = digit.fId;
  fIndexInList = digit.fIndexInList ; 
  fNprimary    = digit.fNprimary ;
  fNiparent    = digit.fNiparent ;
}

//____________________________________________________________________________
AliEMCALDigit::~AliEMCALDigit() 
{
  // Delete array of primiries if any
  
}

//____________________________________________________________________________
Int_t AliEMCALDigit::Compare(const TObject * obj) const
{
  // Compares two digits with respect to its Id
  // to sort according increasing Id

  Int_t rv ;

  AliEMCALDigit * digit = (AliEMCALDigit *)obj ; 

  Int_t iddiff = fId - digit->GetId() ; 

  if ( iddiff > 0 ) 
    rv = 1 ;
  else if ( iddiff < 0 )
    rv = -1 ; 
  else
    rv = 0 ;
  
  return rv ; 

}

//____________________________________________________________________________
Int_t AliEMCALDigit::GetPrimary(Int_t index) const
{
  // retrieves the primary particle number given its index in the list 
  Int_t rv = -1 ;
  if ( index <= fNprimary ){
    rv = fPrimary[index-1] ;
  } 

  return rv ; 
  
}

//____________________________________________________________________________
Int_t AliEMCALDigit::GetIparent(Int_t index) const
{
  // retrieves the primary particle number given its index in the list 
  Int_t rv = -1 ;
  if ( index <= fNiparent ){
    rv = fIparent[index-1] ;
  } 

  return rv ; 
  
}


//____________________________________________________________________________
void AliEMCALDigit::ShiftPrimary(Int_t shift){
  //shifts primary nimber to BIG offset, to separate primary in different TreeK
  Int_t index  ;
  for(index = 0; index <fNprimary; index++ ){
    fPrimary[index] = fPrimary[index]+ shift * 10000000   ;}
  for(index =0; index <fNiparent; index++){
    fIparent[index] = fIparent[index] + shift * 10000000 ;}
}
//____________________________________________________________________________
Bool_t AliEMCALDigit::operator==(AliEMCALDigit const & digit) const 
{
  // Two digits are equal if they have the same Id
  
  if ( fId == digit.fId ) 
    return kTRUE ;
  else 
    return kFALSE ;
}
 
//____________________________________________________________________________
AliEMCALDigit& AliEMCALDigit::operator+(AliEMCALDigit const & digit) 
{
  // Adds the amplitude of digits and completes the list of primary particles
  // if amplitude is larger than 
  
  fAmp += digit.fAmp ;
  
  Int_t max1 = fNprimary ; 
  Int_t max2 = fNiparent ;  
  Int_t index ; 
  for (index = 0 ; index < digit.fNprimary ; index++){
    Bool_t deja = kTRUE ;
    Int_t old ;
    for ( old = 0 ; (old < max1) && deja; old++) { //already have this primary?
      if(fPrimary[old] == (digit.fPrimary)[index])
	deja = kFALSE;
    }
    if(deja){
      fPrimary[fNprimary] = (digit.fPrimary)[index] ; 
      fNprimary++ ;
      if(fNprimary>fNMaxPrimary) {
	cout << "AliEMCALDigit >> Increase NMaxPrimary "<< endl ;
	return *this ;
      }
    }
  }
  
  for (index = 0 ; index < digit.fNiparent ; index++){
    Bool_t dejavu = kTRUE ;
    Int_t old ;
    for ( old = 0 ; (old < max2) && dejavu; old++) { //already have this primary?
      if(fIparent[old] == (digit.fIparent)[index])
	dejavu = kFALSE;
    }
    if(dejavu){
      fIparent[fNiparent] = (digit.fIparent)[index] ; 
      fNiparent++ ;
      if(fNiparent>fNMaxiparent) {
	cout << "AliEMCALDigit >> Increase NMaxiparent "<< endl ;
	return *this ;
      }
    }
  }
  
  return *this ;
}

//____________________________________________________________________________
ostream& operator << ( ostream& out , const AliEMCALDigit & digit)
{
  // Prints the data of the digit
  
  out << "ID " << digit.fId << " Energy = " << digit.fAmp << endl ; 
  Int_t i,j ;
  for(i=0;i<digit.fNprimary;i++)
    out << "Primary " << i+1 << " = " << digit.fPrimary[i] << endl ;
   
  for(j=0;j<digit.fNiparent;j++)
    out << "Iparent " << j+1 << " = " << digit.fIparent[j] << endl ;
 out << "Position in list = " << digit.fIndexInList << endl ; 
  return out ;
}


