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
// $MpId: AliMpVMotif.cxx,v 1.9 2006/05/24 13:58:41 ivana Exp $
// Category: motif
///
// Class AliMpVMotif
// -----------------
// Class that defines a motif with its unique ID
// and the motif type.
// Included in AliRoot: 2003/05/02
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#include "AliMpVMotif.h"
#include "AliMpMotifType.h"
#include "AliMpIntPair.h"
#include "AliMpConnection.h"

#include <Riostream.h>

#include <iomanip>

/// \cond CLASSIMP
ClassImp(AliMpVMotif)
/// \endcond

//_____________________________________________________________________________
AliMpVMotif::AliMpVMotif():
  fID(""),
  fMotifType(0)
{
/// Default constructor
}

//_____________________________________________________________________________
AliMpVMotif::AliMpVMotif(const TString &id, AliMpMotifType *motifType):
  fID(id),
  fMotifType(motifType)
{
  /// Standard constructor.              
  /// The dimension in a given direction is calculated by
  /// multiplying the total dimension by the number of pads

}

//_____________________________________________________________________________
AliMpVMotif::AliMpVMotif(const AliMpVMotif& right) 
  : TObject(right) 
{
  /// Protected copy constructor (not provided) 

  Fatal("AliMpVMotif", "Copy constructor not provided.");
}

//_____________________________________________________________________________
AliMpVMotif::~AliMpVMotif()
{
  /// Destructor
}

// operators

//_____________________________________________________________________________
AliMpVMotif& AliMpVMotif::operator=(const AliMpVMotif& right)
{
  /// Protected assignment operator (not provided)
  
  // check assignment to self
  if (this == &right) return *this;

  Fatal("operator =", "Assignment operator not provided.");
    
  return *this;  
}    

//_____________________________________________________________________________
AliMpConnection* 
AliMpVMotif::FindConnectionByLocalPos(const TVector2& localPos) const
{
  /// Return the local indices from the local
  /// (x,y) position

  AliMpIntPair padIndices=PadIndicesLocal(localPos);
  if (padIndices.GetFirst()>=0)
    return fMotifType->FindConnectionByLocalIndices(padIndices);
  else
    return 0;
}
//_____________________________________________________________________________
void AliMpVMotif::Print(Option_t *option) const
{
  /// Print the map of the motif. In each cel, the value
  /// printed depends of option, as the following:
  /// - option="N" the "name" of the pad is written
  /// - option="K" the Kapton connect. number attached to the pad is written
  /// - option="B" the Berg connect. number attached to the pad is written
  /// - option="X" the (X,Y) position, in cm, of the center of the pad is written
  /// otherwise the number of the pad is written
  ///
  /// NOTE : this method is really not optimized, in case 'N' or '',
  /// but the Print() this should not be very important in a Print() method

  if (option[0]=='X') {

    cout<<"(X,Y) mapping";
    cout<<" in the motif "<<fID<<endl;
    cout<<"-----------------------------------"<<endl;
    for (Int_t j=fMotifType->GetNofPadsY()-1;j>=0;j--){
      for (Int_t i=0;i<fMotifType->GetNofPadsX();i++){
	AliMpIntPair indices = AliMpIntPair(i,j);
	if (fMotifType->FindConnectionByLocalIndices(indices)){
	  TVector2 pos = PadPositionLocal(indices);
	  cout<<setw(11)<<Form("(%.1f,%.1f)",pos.X(),pos.Y());
	}
      }
      cout<<endl;
    }
  } else fMotifType->Print(option);
}


