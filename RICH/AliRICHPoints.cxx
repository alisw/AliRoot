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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  This class contains the points for the ALICE event display               //
//                                                                           //
//Begin_Html
/*
<img src="gif/AliRICHPointsClass.gif">
*/
//End_Html
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#include <TPad.h>
#include <TTree.h>
#include <TView.h>
#include <TMath.h>
#include <TPolyMarker3D.h>
#include <TMarker3DBox.h>

#include "AliRICH.h"
#include <TParticle.h>
#include "AliRICHDisplay.h"
#include "AliRICHPoints.h"
#include <AliRun.h>
#include "AliRICHSDigit.h"
#include <AliMC.h>

const Int_t kMaxNipx=400, kMaxNipy=800;
 
ClassImp(AliRICHPoints)

//_____________________________________________________________________________
AliRICHPoints::AliRICHPoints()
{
  //
  // Default constructor
  //
  fHitIndex = 0;
  fTrackIndex = 0;
  fDigitIndex = 0;
  fMarker[0] = fMarker[1] = fMarker[2]=0;
}

//_____________________________________________________________________________
AliRICHPoints::AliRICHPoints(Int_t npoints)
  :AliPoints(npoints)
{
  //
  // Standard constructor
  //
  fHitIndex = 0;
  fTrackIndex = 0;
  fDigitIndex = 0;
  fMarker[0] = fMarker[1] = fMarker[2]=0;
}
	 
//_____________________________________________________________________________
AliRICHPoints::~AliRICHPoints()
{
  //
  // Default destructor
  //
  fHitIndex = 0;
  fTrackIndex = 0;
  fDigitIndex = 0;
}

//_____________________________________________________________________________
void AliRICHPoints::DumpHit()
{
  //
  //   Dump hit corresponding to this point
  //
  AliRICHhit *hit = GetHit();
  if (hit) hit->Dump();
}

//_____________________________________________________________________________
void AliRICHPoints::DumpDigit()
{
  //
  //   Dump digit corresponding to this point
  //
  AliRICHdigit *digit = GetDigit();
  if (digit) digit->Dump();
}

//_____________________________________________________________________________
void AliRICHPoints::InspectHit()
{
  //
  //   Inspect hit corresponding to this point
  //
  AliRICHhit *hit = GetHit();
  if (hit) hit->Inspect();
}

//_____________________________________________________________________________
void AliRICHPoints::InspectDigit()
{
  //
  //   Inspect digit corresponding to this point
  //
  AliRICHdigit *digit = GetDigit();
  if (digit) digit->Inspect();
}

//_____________________________________________________________________________
Int_t AliRICHPoints::GetTrackIndex()
{
  //
  //   Dump digit corresponding to this point
  //
  printf("GetTrackIndex - fTrackIndex %d \n",fTrackIndex);
  this->Inspect();
  return fTrackIndex;
}
//_____________________________________________________________________________
TParticle *AliRICHPoints::GetParticle() const
{
  //
  //   Returns pointer to particle index in AliRun::fParticles
  //
  if (fIndex < 0 || fIndex >= gAlice->GetMCApp()->GetNtrack()) return 0;
  return gAlice->GetMCApp()->Particle(fIndex);
}

//_____________________________________________________________________________
AliRICHhit *AliRICHPoints::GetHit() const
{
  //
  //   Returns pointer to hit index in AliRun::fParticles
  //
  AliRICH *pRICH  = (AliRICH*)gAlice->GetDetector("RICH");
  pRICH->TreeH()->GetEvent(fTrackIndex);
  TClonesArray *pRICHhits  = pRICH->Hits();
  Int_t nhits = pRICHhits->GetEntriesFast();
  if (fHitIndex < 0 || fHitIndex >= nhits) return 0;
  return (AliRICHhit*)pRICHhits->UncheckedAt(fHitIndex);
}

//_____________________________________________________________________________
AliRICHdigit *AliRICHPoints::GetDigit() const
{
  //
  //   Returns pointer to digit index in AliRun::fParticles
  //

  AliRICHDisplay *display=(AliRICHDisplay*)gAlice->Display();
  Int_t chamber=display->GetChamber();
  Int_t cathode=display->GetCathode();
   
  AliRICH *pRICH  = (AliRICH*)gAlice->GetDetector("RICH");
  TClonesArray *pRICHdigits  = pRICH->Digits(chamber);
  gAlice->TreeD()->GetEvent(cathode);
  Int_t ndigits = pRICHdigits->GetEntriesFast();
  if (fDigitIndex < 0 || fDigitIndex >= ndigits) return 0;
  return (AliRICHdigit*)pRICHdigits->UncheckedAt(fDigitIndex);
}
//----------------------------------------------------------------------------
void AliRICHPoints::ShowRing(Int_t highlight) {

//
// Highlights all pads generated by the same mother particle

   
  AliRICH *pRICH  = (AliRICH*)gAlice->GetDetector("RICH");
  AliRICHChamber*       iChamber;
  AliSegmentation*      segmentation;

      
  AliRICHPoints *points = 0;
  TMarker3DBox  *marker = 0;
    
  AliRICHhit *mHit = GetHit();

  printf("Hit %d on chamber: %d\n",fHitIndex, mHit->Chamber());

  TClonesArray *digits  = pRICH->Digits(mHit->Chamber());
  iChamber = &(pRICH->Chamber(mHit->Chamber() - 1));
  segmentation=iChamber->GetSegmentationModel();

  Float_t dpx  = segmentation->Dpx();
  Float_t dpy  = segmentation->Dpy();

  int ndigits=digits->GetEntriesFast();
  
  printf("Show Ring called with %d digits\n",ndigits);
  
  for (int digit=0;digit<ndigits;digit++) {
    AliRICHdigit *mdig = (AliRICHdigit*)digits->UncheckedAt(digit);
    points = new AliRICHPoints(1);
    
     //printf("Particle %d belongs to ring %d \n", fTrackIndex, mdig->fTracks[1]);

    if (!points) continue;
    if (fTrackIndex == mdig->T(0)) {


      Int_t charge=(Int_t)mdig->Q();
      Int_t index=Int_t(TMath::Log(charge)/(TMath::Log(kadc_satm)/22));
      Int_t color=701+index;
      if (color>722) color=722;
      points->SetMarkerColor(color);
      points->SetMarkerStyle(21);
      points->SetMarkerSize(.5);
      Float_t xpad, ypad, zpad;
      segmentation->GetPadC(mdig->X(), mdig->Y(),xpad, ypad, zpad);
      Float_t vectorLoc[3]={xpad,6.276,ypad};
      Float_t  vectorGlob[3];
      points->SetParticle(-1);
      points->SetHitIndex(-1);
      points->SetTrackIndex(-1);
      points->SetDigitIndex(digit);
      iChamber->LocaltoGlobal(vectorLoc,vectorGlob);
      points->SetPoint(0,vectorGlob[0],vectorGlob[1],vectorGlob[2]);
      
      segmentation->GetPadC(mdig->X(), mdig->Y(), xpad, ypad, zpad);
      Float_t theta = iChamber->GetRotMatrix()->GetTheta();
      Float_t phi   = iChamber->GetRotMatrix()->GetPhi();	   
      marker=new TMarker3DBox(vectorGlob[0],vectorGlob[1],vectorGlob[2],
			      dpy/2,0,dpx/2,theta,phi);
      marker->SetLineColor(highlight);
      marker->SetFillStyle(1001);
      marker->SetFillColor(color);
      marker->SetRefObject((TObject*)points);
      points->Set3DMarker(0, marker);
      
      points->Draw("same");
      for (Int_t im=0;im<3;im++) {
	TMarker3DBox *marker=points->GetMarker(im);
	if (marker)
	  marker->Draw();
      }
      TParticle *p = gAlice->GetMCApp()->Particle(fIndex);
      printf("\nTrack index %d\n",fTrackIndex);
      printf("Particle ID %d\n",p->GetPdgCode());
      printf("Parent %d\n",p->GetFirstMother());
      printf("First child %d\n",p->GetFirstDaughter());
      printf("Px,Py,Pz %f %f %f\n",p->Px(),p->Py(),p->Pz());
    }
  }
}

//_____________________________________________________________________________
const Text_t *AliRICHPoints::GetName() const
{
  //
  // Return name of the Geant3 particle corresponding to this point
  //
  TParticle *particle = GetParticle();
  if (!particle) return "Particle";
  return particle->GetName();
}

//_____________________________________________________________________________
Text_t *AliRICHPoints::GetObjectInfo(Int_t, Int_t)
{
  //
  //   Redefines TObject::GetObjectInfo.
  //   Displays the info (particle,etc
  //   corresponding to cursor position px,py
  //
  static char info[64];
  sprintf(info,"%s %d",GetName(),fIndex);
  return info;
}



