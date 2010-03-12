/*************************************************************************
* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
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

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  Dielectron Pair class. Internally it makes use of AliKFParticle.     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////


#include "AliDielectronPair.h"
#include "AliVTrack.h"

ClassImp(AliDielectronPair)

AliDielectronPair::AliDielectronPair() :
  fOpeningAngle(-1),
  fType(-1),
  fLabel(-1),
  fPair(),
  fRefD1(),
  fRefD2()
{
  //
  // Default Constructor
  //
  
}

//______________________________________________
AliDielectronPair::AliDielectronPair(AliVTrack * const particle1, Int_t pid1,
                                     AliVTrack * const particle2, Int_t pid2, Char_t type) :
  fOpeningAngle(-1),
  fType(type),
  fLabel(-1),
  fPair(),
  fRefD1(),
  fRefD2()
{
  //
  // Constructor with tracks
  //
  SetTracks(particle1, pid1, particle2, pid2);
}

//______________________________________________
AliDielectronPair::~AliDielectronPair()
{
  //
  // Default Destructor
  //
  
}

//______________________________________________
void AliDielectronPair::SetTracks(AliVTrack * const particle1, Int_t pid1,
                                  AliVTrack * const particle2, Int_t pid2)
{
  //
  // Set the tracks to the pair KF particle
  //
  fPair.Initialize();
  
  AliKFParticle kf1(*particle1,pid1);
  AliKFParticle kf2(*particle2,pid2);
    
  fPair.AddDaughter(kf1);
  fPair.AddDaughter(kf2);
  
  if (particle1->Pt()>particle2->Pt()){
    fRefD1 = particle1;
    fRefD2 = particle2;
  } else {
    fRefD1 = particle2;
    fRefD2 = particle1;
  }

  fOpeningAngle=kf1.GetAngle(kf2);
}

