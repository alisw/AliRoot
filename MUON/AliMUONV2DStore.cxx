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

// $Id$

#include "AliMUONV2DStore.h"

/// \class AliMUONV2DStore
/// Defines an interface equivalent to a 2D array of TObject, indexed
/// by a pair of integers (somehow a matrix, 
/// except that indices are not necessarily sequential).
/// 
/// It's extremely simple and hopefully allow many implementations.
/// It also makes the object ownership self-evident.
///
/// \author Laurent Aphecetche

/// \cond CLASSIMP
ClassImp(AliMUONV2DStore)
/// \endcond

#include "AliMpIntPair.h"
#include "AliMUONObjectPair.h"
#include "AliMpHelper.h"
#include "AliMUONVDataIterator.h"
#include "Riostream.h"
#include "TMap.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"

//_____________________________________________________________________________
AliMUONV2DStore::AliMUONV2DStore()
{
/// Default constructor
}

//_____________________________________________________________________________
AliMUONV2DStore::~AliMUONV2DStore()
{
/// Destructor
}

//_____________________________________________________________________________
void
AliMUONV2DStore::Print(Option_t* opt) const
{
  /// Printout
  /// opt is used to filter which (i,j) couple you want to see
  /// e.g opt="I=12;J=1;opt=Full" to see complete values, but only for the 
  /// (12,1) pair.
  /// Warning : decoding of opt format is not really bullet-proof (yet?)
  
  AliMUONVDataIterator* it = this->Iterator();
  
  AliMUONObjectPair* pair;
  
  TMap* m = AliMpHelper::Decode(opt);
  
  TString si;  
  Bool_t selectI = AliMpHelper::Decode(*m,"i",si);
  TString sj;
  Bool_t selectJ = AliMpHelper::Decode(*m,"j",sj);
  TString sopt;
  AliMpHelper::Decode(*m,"opt",sopt);
  
  m->DeleteAll();
  delete m;
  
  while ( ( pair = static_cast<AliMUONObjectPair*>(it->Next() ) ) )
  {
    AliMpIntPair* ip = static_cast<AliMpIntPair*>(pair->First());
    Int_t i = ip->GetFirst();
    Int_t j = ip->GetSecond();
    if ( selectI && i != si.Atoi() ) continue;
    if ( selectJ && j != sj.Atoi() ) continue;
    cout << Form("[%d,%d]",i,j) << endl;
    TObject* o = pair->Second();
    if (o) 
    {
      o->Print(sopt.Data());
    }
  }
  
  delete it;
}


