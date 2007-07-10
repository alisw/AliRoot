// @(#) $Id$
// Original: AliHLTConfMapPoint.cxx,v 1.10 2005/06/23 17:46:55 hristov 

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Anders Vestbo, maintained by                          *
 *                  Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTTPCConfMapPoint.cxx
    @author Anders Vestbo, maintained by Matthias Richter
    @date   
    @brief  Hit class for conformal mapper
*/

#include "AliHLTTPCLogging.h"
#include "AliHLTTPCConfMapPoint.h"
#include "AliHLTTPCSpacePointData.h"
#include "AliHLTTPCVertex.h"
#include "AliHLTTPCConfMapTrack.h"

/**
<pre>
//_____________________________________________________________
// AliHLTTPCConfMapPoint
//
// Hit class for conformal mapper
</pre>
*/

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTTPCConfMapPoint);

Bool_t AliHLTTPCConfMapPoint::fgDontMap=kFALSE;

AliHLTTPCConfMapPoint::AliHLTTPCConfMapPoint()
  :
  fHitNumber(-1),
  fTrackNumber(0),
  fNextHitNumber(0),
  fUsed(0),
  fPadrow(0),
  fSector(0),
  fx(0),
  fy(0),
  fz(0),
  fxerr(0),
  fyerr(0),
  fzerr(0),
  fWxy(0),
  fWz(0),
  fs(0),
  fXt(0),
  fYt(0),
  fZt(0),
  fXterr(0),
  fYterr(0),
  fZterr(0),
  fXprime(0),
  fYprime(0),
  fXprimeerr(0),
  fYprimeerr(0),
  fXv(0),
  fYv(0),
  fZv(0),
  fXverr(0),
  fYverr(0),
  fZverr(0),
  fPhi(0),
  fEta(0),
  fNextVolumeHit(0),
  fNextRowHit(0),
  fNextTrackHit(0),
  fPhiIndex(0),
  fEtaIndex(0),
  fXYChi2(0),
  fSZChi2(0)
{
  //Constructor
  
  SetUsage(false);
  SetIntPoint(0., 0., 0., 0., 0., 0.);
  SetShiftedCoord();
  SetMCTrackID(0,0,0);
}

AliHLTTPCConfMapPoint::AliHLTTPCConfMapPoint(const AliHLTTPCConfMapPoint&)
  :
  fHitNumber(-1),
  fTrackNumber(0),
  fNextHitNumber(0),
  fUsed(0),
  fPadrow(0),
  fSector(0),
  fx(0),
  fy(0),
  fz(0),
  fxerr(0),
  fyerr(0),
  fzerr(0),
  fWxy(0),
  fWz(0),
  fs(0),
  fXt(0),
  fYt(0),
  fZt(0),
  fXterr(0),
  fYterr(0),
  fZterr(0),
  fXprime(0),
  fYprime(0),
  fXprimeerr(0),
  fYprimeerr(0),
  fXv(0),
  fYv(0),
  fZv(0),
  fXverr(0),
  fYverr(0),
  fZverr(0),
  fPhi(0),
  fEta(0),
  fNextVolumeHit(0),
  fNextRowHit(0),
  fNextTrackHit(0),
  fPhiIndex(0),
  fEtaIndex(0),
  fXYChi2(0),
  fSZChi2(0)
{
  // dummy copy constructor
}

AliHLTTPCConfMapPoint& AliHLTTPCConfMapPoint::operator=(const AliHLTTPCConfMapPoint&)
{ 
  // dummy assignment operator
  return *this;
}

AliHLTTPCConfMapPoint::~AliHLTTPCConfMapPoint()
{
  // Destructor.
}

Bool_t AliHLTTPCConfMapPoint::ReadHits(AliHLTTPCSpacePointData* hits )
{
  //read the hits
  SetHitNumber(hits->fID);
  SetPadRow(hits->fPadRow);
  Int_t slice = (hits->fID>>25) & 0x7f;
  SetSector(slice);
  SetX(hits->fX);
  SetY(hits->fY);
  SetZ(hits->fZ);
  SetXerr(sqrt(hits->fSigmaY2));
  SetYerr(sqrt(hits->fSigmaY2));
  SetZerr(sqrt(hits->fSigmaZ2));
  return kTRUE;
}

void AliHLTTPCConfMapPoint::Reset()
{
  //Reset this point.
  SetUsage(kFALSE);
  SetS(0);
  fNextRowHit = 0;
  fNextVolumeHit=0;
  fNextTrackHit=0;
}

void AliHLTTPCConfMapPoint::Setup(AliHLTTPCVertex *vertex)
{
  //Setup. Sets the vertex, conformal coordinates, 
  //and phi and eta of each hit.
  
  SetIntPoint(vertex->GetX(),    vertex->GetY(),    vertex->GetZ(),
              vertex->GetXErr(), vertex->GetYErr(), vertex->GetZErr());
  SetShiftedCoord();
  SetConfCoord();
  // The angles are set properly if they are set after 
  // the interaction point and the shifted coordinates
  SetAngles();
  //SetDist(0., 0.);
  return;
}

void AliHLTTPCConfMapPoint::SetIntPoint(Double_t inx, Double_t iny, Double_t inz,
			            Double_t inxerr, Double_t inyerr, Double_t inzerr)
{
  // Defines a new interaction point. This point is needed to calculate
  // the conformal coordinates.

  SetXt(inx);
  SetYt(iny);
  SetZt(inz);
  SetXterr(inxerr);
  SetYterr(inyerr);
  SetZterr(inzerr);

  return;
}

void AliHLTTPCConfMapPoint::SetAllCoord(const AliHLTTPCConfMapPoint *precedinghit)
{
  // Sets the interaction point, the shifted coordinates, and the conformal mapping coordinates.
  // These values are calculated from the interaction point of the given cluster which should be a
  // already found cluster on the same track.

  if (this == precedinghit) {
    SetIntPoint(precedinghit->GetX(),    precedinghit->GetY(),    precedinghit->GetZ(),
                precedinghit->GetXerr(), precedinghit->GetYerr(), precedinghit->GetZerr());
  }

  else {
    SetIntPoint(precedinghit->GetXt(),    precedinghit->GetYt(),    precedinghit->GetZt(),
                precedinghit->GetXterr(), precedinghit->GetYterr(), precedinghit->GetZterr());
  }

  SetShiftedCoord();
  SetConfCoord();

  return;
}

void AliHLTTPCConfMapPoint::SetShiftedCoord()
{
  // Sets the coordinates with resepct to the given vertex point

  SetXv(GetX() - fXt);
  SetYv(GetY() - fYt);
  SetZv(GetZ() - fZt);
  /*
  SetXverr(TMath::Sqrt(GetXerr()*GetXerr() + fXterr*fXterr));
  SetYverr(TMath::Sqrt(GetYerr()*GetYerr() + fYterr*fYterr));
  SetZverr(TMath::Sqrt(GetZerr()*GetZerr() + fZterr*fZterr));
  */
  return;
}

void AliHLTTPCConfMapPoint::SetConfCoord()
{
  // Calculates the conformal coordinates of one cluster.
  // If the option "vertex_constraint" applies the interaction point is
  // assumed to be at (0, 0, 0). Otherwise the function will use the
  // interaction point specified by fXt and fYt.

  if(fgDontMap){
    fXprime = fx;
    fYprime = fy;
    fWxy = 0;
    fs = 0; //track trajectory
    fWz = 0;
    return;
  }

  Double_t r2;
  Double_t xyErrorScale = 1;
  Double_t szErrorScale = 1;

  if ((r2 = fXv*fXv + fYv*fYv)) 
    {
      fXprime =  fXv / r2;
      fYprime = -fYv / r2;
      
      //set weights:
      fWxy = r2*r2 / ((xyErrorScale*xyErrorScale)*((fxerr*fxerr)+(fyerr*fyerr)));
      fs = 0; //track trajectory
      fWz = (Double_t)(1./(szErrorScale*fzerr*fzerr));
    } else {
    fXprime    = 0.;
    fYprime    = 0.;
    fXprimeerr = 0.;
    fYprimeerr = 0.;
    fWxy = 0;
    fWz = 0;
    fs = 0;
  }

  return;
}

void AliHLTTPCConfMapPoint::SetAngles()
{
  // Calculates the angle phi and the pseudorapidity eta for each cluster.
  /*
  Double_t r = TMath::Sqrt(x*x + y*y);

  fPhi = TMath::ATan2(y,x);
  if(fPhi<0) fPhi = fPhi + 2*TMath::Pi();
  fEta = 3.*z/(TMath::Abs(z)+2.*r);
  return;
  */
  //  Double_t r3dim = TMath::Sqrt(fXv*fXv + fYv*fYv + fZv*fZv);
  Double_t r3dim = sqrt(fXv*fXv + fYv*fYv + fZv*fZv);
  //Double_t r2dim = TMath::Sqrt(fXv*fXv + fYv*fYv);

  /*if (r2dim == 0.) {
  // If r2dim == 0 the pseudorapidity eta cannot be calculated (division by zero)!
  // This can only happen if the point is lying on the z-axis and this should never be possible.
    cerr << "The pseudorapidity eta cannot be calculated (division by zero)! Set to 1.e-10." << endl;
    r2dim = 1.e-10;
  }

  if (fXv == 0.) {
    fPhi = (fYv > 0.) ? TMath::Pi() / 2. : - TMath::Pi() / 2.;
  }

  else {
    fPhi = (fXv > 0.) ? TMath::ASin(fYv/r2dim) : TMath::Pi() - TMath::ASin(fYv/r2dim);
  }

  if (fPhi < 0.) {
    fPhi += 2. * TMath::Pi();
  }
  */
  //fPhi = TMath::ATan2(y,x);
  fPhi = atan2(fy,fx);
  //if(fPhi<0) fPhi = fPhi + 2*TMath::Pi();
  
  //fEta = 0.5 * TMath::Log((r3dim + fZv)/(r3dim - fZv));
  fEta = 0.5 * log((r3dim + fZv)/(r3dim - fZv));
  return;
}

/*
AliHLTTPCConfMapTrack *AliHLTTPCConfMapPoint::GetTrack(TClonesArray *tracks) const
{
  // Returns the pointer to the track to which this hit belongs.
  
  return (AliHLTTPCConfMapTrack*)tracks->At(this->GetTrackNumber());
}
*/
