//  **************************************************************************
//  * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
//  *                                                                        *
//  * Author: The ALICE Off-line Project.                                    *
//  * Contributors are mentioned in the code where appropriate.              *
//  *                                                                        *
//  * Permission to use, copy, modify and distribute this software and its   *
//  * documentation strictly for non-commercial purposes is hereby granted   *
//  * without fee, provided that the above copyright notice appears in all   *
//  * copies and that both the copyright notice and this permission notice   *
//  * appear in the supporting documentation. The authors make no claims     *
//  * about the suitability of this software for any purpose. It is          *
//  * provided "as is" without express or implied warranty.                  *
//  **************************************************************************

#include "AliHMPIDHit.h"  //class header
#include "AliHMPIDDigit.h"
#include <TPDGCode.h>    
#include <TMarker.h>
 
ClassImp(AliHMPIDHit)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDHit::Draw(Option_t*)
{
  Int_t iMark;
  switch(Pid()){
    case 50000050:   iMark=4;  break;
    case 50000051:   iMark=27; break;
    default:         iMark=26; break;
  }    
  TMarker *pMark=new TMarker(LorsX(),LorsY(),iMark); pMark->SetMarkerColor(kRed); pMark->Draw();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void AliHMPIDHit::Print(Option_t*)const
{
//Print hit
  char *sPart=Form("pid=%i",Pid());
  switch(Pid()){
    case kProton:      sPart="p+  ";break;
    case kProtonBar:   sPart="p-  ";break;
    case kKPlus:       sPart="K+  ";break;
    case kKMinus:      sPart="K-  ";break;
    case kPiPlus:      sPart="Pi+ ";break;
    case kPiMinus:     sPart="Pi- ";break;
    case kMuonPlus:    sPart="Mu+ ";break;
    case kMuonMinus:   sPart="Mu- ";break;
    case kElectron:    sPart="e-  ";break;
    case kPositron:    sPart="e+  ";break;
    case 50000050:     sPart="ckov";break;
    case 50000051:     sPart="feed";break;
  }

  Printf("HIT:(%7.3f,%7.3f) Q=%8.3f ch=%i                   TID= %5i, MARS=(%7.2f,%7.2f,%7.2f) %s  %s",
              LorsX(),LorsY(), Q(), Ch(),                  Tid(),         X(),  Y(),  Z(),   sPart, 
                        (AliHMPIDDigit::IsInDead(LorsX(),LorsY()))? "IN DEAD ZONE":"");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
