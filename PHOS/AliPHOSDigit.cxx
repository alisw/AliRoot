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
//  PHOS digit: Id
//              energy
//              3 identifiers for the primary particle(s) at the origine of the digit
//  The digits are made in FinishEvent() by summing all the hits in a single PHOS crystal or PPSD gas cell
//
//*-- Author: Laurent Aphecetche & Yves Schutz (SUBATECH) & Dmitri Peressounko (RRC KI & SUBATECH)


// --- ROOT system ---

#include "TMath.h"

// --- Standard library ---

// --- AliRoot header files ---

#include "AliPHOSDigit.h"




ClassImp(AliPHOSDigit)

//____________________________________________________________________________
  AliPHOSDigit::AliPHOSDigit() 
{
  // default ctor 

  fIndexInList = -1 ; 
  fNprimary    = 0 ;  
  fPrimary = 0;
}

//____________________________________________________________________________
AliPHOSDigit::AliPHOSDigit(Int_t primary, Int_t id, Int_t digEnergy, Float_t time, Int_t index) 
{  
  // ctor with all data 

  fAmp         = digEnergy ;
  fTime        = time ;
  fId          = id ;
  fIndexInList = index ; 
  if( primary != -1){
    fNprimary    = 1 ; 
    fPrimary = new Int_t[fNprimary] ;
    fPrimary[0]  = primary ;
  }
  else{  //If the contribution of this primary smaller than fDigitThreshold (AliPHOSv1)
    fNprimary = 0 ; 
    fPrimary  = 0 ;
  }
}

//____________________________________________________________________________
AliPHOSDigit::AliPHOSDigit(const AliPHOSDigit & digit) : AliDigitNew(digit)
{
  // copy ctor

  fNprimary = digit.fNprimary ;  
  if(fNprimary){
    fPrimary = new Int_t[fNprimary] ;
    for (Int_t i = 0; i < fNprimary ; i++)
       fPrimary[i]  = digit.fPrimary[i] ;
  }
  else
    fPrimary = 0 ;
  fAmp         = digit.fAmp ;
  fTime        = digit.fTime ;
  fId          = digit.fId;
  fIndexInList = digit.fIndexInList ; 
}

//____________________________________________________________________________
AliPHOSDigit::~AliPHOSDigit() 
{
  // Delete array of primiries if any
  if(fPrimary)
    delete [] fPrimary ;
}

//____________________________________________________________________________
Int_t AliPHOSDigit::Compare(const TObject * obj) const
{
  // Compares two digits with respect to its Id
  // to sort according increasing Id

  Int_t rv ;

  AliPHOSDigit * digit = (AliPHOSDigit *)obj ; 

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
Int_t AliPHOSDigit::GetPrimary(Int_t index) const
{
  // retrieves the primary particle number given its index in the list 
  Int_t rv = -1 ;
  if ( index <= fNprimary && index > 0){
    rv = fPrimary[index-1] ;
  } 

  return rv ; 
  
}
//____________________________________________________________________________
void AliPHOSDigit::Print(Option_t *) const
{
  printf("PHOS digit: Amp=%d, Id=%d, Time=%f, NPrim=%d ",fAmp,fId,fTime,fNprimary);
  for(Int_t index = 0; index <fNprimary; index ++ )
    printf(" %d ",fPrimary[index]); 
  printf("\n") ;
}
//____________________________________________________________________________
void AliPHOSDigit::ShiftPrimary(Int_t shift)
{
  //shifts primary number to BIG offset, to separate primary in different TreeK
  for(Int_t index = 0; index <fNprimary; index ++ ){
    fPrimary[index]+= shift ;
  } 
}
//____________________________________________________________________________
Bool_t AliPHOSDigit::operator==(AliPHOSDigit const & digit) const 
{
  // Two digits are equal if they have the same Id
  
  if ( fId == digit.fId ) 
    return kTRUE ;
  else 
    return kFALSE ;
}
 
//____________________________________________________________________________
AliPHOSDigit& AliPHOSDigit::operator+(AliPHOSDigit const & digit) 
{

  // Adds the amplitude of digits and completes the list of primary particles
  if(digit.fNprimary>0){
     Int_t *tmp = new Int_t[fNprimary+digit.fNprimary] ;
     if(fAmp < digit.fAmp){//most energetic primary in second digit => first primaries in list from second digit
        for (Int_t index = 0 ; index < digit.fNprimary ; index++)
           tmp[index]=(digit.fPrimary)[index] ;
        for (Int_t index = 0 ; index < fNprimary ; index++)
           tmp[index+digit.fNprimary]=fPrimary[index] ;
     }
     else{ //add new primaries to the end
        for (Int_t index = 0 ; index < fNprimary ; index++)
           tmp[index]=fPrimary[index] ;
        for (Int_t index = 0 ; index < digit.fNprimary ; index++)
           tmp[index+fNprimary]=(digit.fPrimary)[index] ;
     }
     if(fPrimary)
       delete []fPrimary ;
     fPrimary = tmp ;
   }
   fNprimary+=digit.fNprimary ;
   fAmp += digit.fAmp ;
   if(fTime > digit.fTime)
      fTime = digit.fTime ;
   return *this ;
}
//____________________________________________________________________________
AliPHOSDigit& AliPHOSDigit::operator*(Float_t factor) 
{
  // Multiplies the amplitude by a factor
  
  Float_t tempo = static_cast<Float_t>(fAmp) ; 
  tempo *= factor ; 
  fAmp = static_cast<Int_t>(TMath::Ceil(tempo)) ; 
  return *this ;
}

//____________________________________________________________________________
ostream& operator << ( ostream& out , const AliPHOSDigit & digit)
{
  // Prints the data of the digit
  
//   out << "ID " << digit.fId << " Energy = " << digit.fAmp << " Time = " << digit.fTime << endl ; 
//   Int_t i ;
//   for(i=0;i<digit.fNprimary;i++)
//     out << "Primary " << i+1 << " = " << digit.fPrimary[i] << endl ;
//   out << "Position in list = " << digit.fIndexInList << endl ; 
  digit.Warning("operator <<", "Implement differently") ; 
  return out ;
}


