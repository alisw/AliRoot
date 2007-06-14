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

//-------------------------------------------------------------------------
//                        Implemenation Class AliESDTZERO
//   This is a class that summarizes the TZERO data for the ESD   
//   Origin: Christian Klein-Boesing, CERN, Christian.Klein-Boesing@cern.ch 
//-------------------------------------------------------------------------



#include "AliESDTZERO.h"


ClassImp(AliESDTZERO)

//______________________________________________________________________________
AliESDTZERO::AliESDTZERO() :
  TObject(),
  fT0zVertex(0),
  fT0timeStart(0)   
{
  for(int i = 0;i<24;i++)fT0time[i] = fT0amplitude[i] = 0;
}

AliESDTZERO::AliESDTZERO(const AliESDTZERO &tzero ) :
  TObject(tzero),
  fT0zVertex(tzero.fT0zVertex),
  fT0timeStart(tzero.fT0timeStart)   
{
  // copy constuctor
  for(int i = 0;i<24;i++){
    fT0time[i] = tzero.fT0time[i]; 
    fT0amplitude[i] = tzero.fT0amplitude[i];
  }
}

AliESDTZERO& AliESDTZERO::operator=(const AliESDTZERO& tzero){
  // assigmnent operator
  if(this!=&tzero) {
    TObject::operator=(tzero);
    fT0zVertex = tzero.fT0zVertex;
    fT0timeStart = tzero.fT0timeStart;   
    for(int i = 0;i<24;i++){
      fT0time[i] = tzero.fT0time[i]; 
      fT0amplitude[i] = tzero.fT0amplitude[i];
    }
  } 
  return *this;
}

//______________________________________________________________________________
void AliESDTZERO::Reset()
{
  // reset contents
  fT0zVertex = 0;  
  fT0timeStart = 0;
  for(int i = 0;i<24;i++)fT0time[i] = fT0amplitude[i] = 0;
}

//______________________________________________________________________________
void AliESDTZERO::Print(const Option_t *) const
{
  // does noting fornow
}
