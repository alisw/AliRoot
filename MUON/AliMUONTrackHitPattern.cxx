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


//-----------------------------------------------------------------------------
/// \class AliMUONTrackHitPattern
///
/// This class propagates tracks to trigger chambers 
/// searching for fired strips.
///
/// To each track, a hit pattern for trigger chambers is set.
/// The hit pattern is a UShort_t with 8 bits used:
///
///            1  1  0  1    1  1  0  1
///           |           |            |
///            ----------- ------------
/// chamber:  11 12 13 14 | 11 12 13 14
/// cathode:    bending   | non-bending
///
/// The main method is:
/// * GetHitPattern
///
///  \author Diego Stocco
//-----------------------------------------------------------------------------


#include "AliMUONTrackHitPattern.h"

#include "AliMUONConstants.h"
#include "AliMUONVDigit.h"
#include "AliMUONDigitMaker.h"
#include "AliMUONDigitStoreV1.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONLocalTrigger.h"
#include "AliMUONLocalTriggerBoard.h"
#include "AliMUONTrack.h"
#include "AliMUONTrackExtrap.h"
#include "AliMUONTrackParam.h"
#include "AliMUONVTrackStore.h"
#include "AliMUONVTriggerStore.h"
#include "AliMpPad.h"
#include "AliMpSegmentation.h"
#include "AliMpVSegmentation.h"

#include "AliMagF.h"
#include "AliLog.h"
#include "AliTracker.h"

#include <Riostream.h>
#include <TArrayS.h>
#include <TClonesArray.h>
#include <TMath.h>
#include <TMatrixD.h>

/// \cond CLASSIMP
ClassImp(AliMUONTrackHitPattern) // Class implementation in ROOT context
/// \endcond


//______________________________________________________________________________
AliMUONTrackHitPattern::AliMUONTrackHitPattern(const AliMUONGeometryTransformer& transformer,
                                               const AliMUONDigitMaker& digitMaker)
    : TObject(),
      fTransformer(transformer),
      fDigitMaker(digitMaker)
{
    /// Default constructor

    // Set magnetic field
    const AliMagF* kField = AliTracker::GetFieldMap();
    if (!kField) AliFatal("No field available");
    AliMUONTrackExtrap::SetField(kField);
}


//______________________________________________________________________________
AliMUONTrackHitPattern::~AliMUONTrackHitPattern(void)
{
  /// Destructor
}


//______________________________________________________________________________
void AliMUONTrackHitPattern::GetHitPattern(AliMUONVTrackStore& trackStore,
                                           const AliMUONVTriggerStore& triggerStore) const
{
    //
    /// Main method:
    /// Loops on reco tracks, extrapolates them to trigger chambers
    /// and searches for matching digits
    //
    
    const Int_t kMask[2][4]= {{0x80, 0x40, 0x20, 0x10},
                              {0x08, 0x04, 0x02, 0x01}};
    Bool_t isMatch[2];

    UShort_t pattern=0;
    
    AliMUONDigitStoreV1 digitStore;
    
    TriggerDigits(triggerStore,digitStore);
    
    AliMUONTrack* muonTrack;
    TIter next(trackStore.CreateIterator());
    
    while ( ( muonTrack = static_cast<AliMUONTrack*>(next()) ) )
    {
      pattern = 0;
      AliMUONTrackParam trackParam = (*(static_cast<AliMUONTrackParam*> 
        (muonTrack->GetTrackParamAtHit()->Last())));
      
      for(Int_t ch=0; ch<4; ++ch)
      {
        AliMUONTrackExtrap::ExtrapToZCov(&trackParam, AliMUONConstants::DefaultChamberZ(10+ch));
        FindPadMatchingTrack(digitStore,trackParam, isMatch, ch);
        for(Int_t cath=0; cath<2; ++cath)
        {
          if(isMatch[cath]) pattern |= kMask[cath][ch];
	    }
      }
      muonTrack->SetHitsPatternInTrigCh(pattern);
    }
}


//______________________________________________________________________________
void 
AliMUONTrackHitPattern::FindPadMatchingTrack(AliMUONVDigitStore& digitStore,
                                             const AliMUONTrackParam& trackParam,
                                             Bool_t isMatch[2], Int_t /*iChamber*/) const
{
    //
    /// Given track position, searches for matching digits.
    //

    Float_t minMatchDist[2];

    for(Int_t cath=0; cath<2; ++cath)
    {
      isMatch[cath]=kFALSE;
      minMatchDist[cath]=9999.;
    }

    TIter next(digitStore.CreateIterator());
    AliMUONVDigit* mDigit;
    
    while ( ( mDigit = static_cast<AliMUONVDigit*>(next()) ) )
    {
      Int_t currDetElemId = mDigit->DetElemId();
      Int_t cathode = mDigit->Cathode();
      Int_t ix = mDigit->PadX();
      Int_t iy = mDigit->PadY();
      Float_t xpad, ypad, zpad;
      const AliMpVSegmentation* seg = AliMpSegmentation::Instance()
        ->GetMpSegmentation(currDetElemId,AliMp::GetCathodType(cathode));
      
      AliMpPad pad = seg->PadByIndices(AliMpIntPair(ix,iy),kTRUE);
      Float_t xlocal1 = pad.Position().X();
      Float_t ylocal1 = pad.Position().Y();
      Float_t dpx = pad.Dimensions().X();
      Float_t dpy = pad.Dimensions().Y();
      fTransformer.Local2Global(currDetElemId, xlocal1, ylocal1, 0, xpad, ypad, zpad);
      Float_t matchDist = MinDistanceFromPad(xpad, ypad, zpad, dpx, dpy, trackParam);
      if(matchDist>minMatchDist[cathode])continue;
      isMatch[cathode] = kTRUE;
      minMatchDist[cathode] = matchDist;
    }
}


//______________________________________________________________________________
Float_t 
AliMUONTrackHitPattern::MinDistanceFromPad(Float_t xPad, Float_t yPad, Float_t zPad,
                                           Float_t dpx, Float_t dpy, 
                                           const AliMUONTrackParam& trackParam) const
{
    //
    /// Decides if the digit belongs to the track.
    //
    Float_t xTrackAtPad = trackParam.GetNonBendingCoor();
    Float_t yTrackAtPad = trackParam.GetBendingCoor();

    Float_t sigmaX, sigmaY, sigmaMS;
    GetPosUncertainty(trackParam, zPad, sigmaX, sigmaY, sigmaMS);

    Float_t maxDistX = 3.*(sigmaX + sigmaMS); // in cm
    Float_t maxDistY = 3.*(sigmaY + sigmaMS); // in cm

    Float_t deltaX = TMath::Abs(xPad-xTrackAtPad)-dpx;
    Float_t deltaY = TMath::Abs(yPad-yTrackAtPad)-dpy;

    Float_t matchDist = 99999.;
    if(deltaX<=maxDistX && deltaY<=maxDistY) matchDist = TMath::Max(deltaX, deltaY);
    return matchDist;
}


//______________________________________________________________________________
void 
AliMUONTrackHitPattern::GetPosUncertainty(const AliMUONTrackParam& trackParam,
                                          Float_t zChamber, 
                                          Float_t &sigmaX, Float_t &sigmaY, 
                                          Float_t &sigmaMS) const
{
    //
    /// Returns uncertainties on extrapolated position.
    /// Takes into account Branson plane corrections in the iron wall.
    //

    const Float_t kAlpha = 0.1123; // GeV/c
    
    // Find a better way to get such parameters ???
    const Float_t kZFilterIn = 1471.; // From STRUCT/SHILConst2.h
    const Float_t kZFilterOut = kZFilterIn + 120.; // From STRUCT/SHILConst2.h
    
    const Float_t kZBranson = - (kZFilterIn + (kZFilterOut - kZFilterIn)*2./3. ); // - sign because distance are positive
    Float_t zDistFromWall = TMath::Abs(zChamber - kZBranson);
    Float_t zDistFromLastTrackCh = TMath::Abs(zChamber - AliMUONConstants::DefaultChamberZ(9));

    const TMatrixD& kCovParam = trackParam.GetCovariances();
    
    sigmaX = kCovParam(0,0);
    sigmaY = kCovParam(2,2);

    // If covariance matrix is not extrapolated, use "reasonable" errors
    // (To be removed as soon as covariance matrix is correctly propagated).
    if (sigmaX==0.)sigmaX = 0.003 * zDistFromLastTrackCh;
    if (sigmaY==0.)sigmaY = 0.004 * zDistFromLastTrackCh;

    Float_t p = trackParam.P();
    Float_t thetaMS = kAlpha/p;
    sigmaMS = zDistFromWall * TMath::Tan(thetaMS);
}


//______________________________________________________________________________
Bool_t 
AliMUONTrackHitPattern::TriggerDigits(const AliMUONVTriggerStore& triggerStore,
                                      AliMUONVDigitStore& digitStore) const
{
  //
  /// make (S)Digit for trigger
  //
  
  digitStore.Clear();
  
  AliMUONLocalTrigger* locTrg;
  TIter next(triggerStore.CreateLocalIterator());
  
  while ( ( locTrg = static_cast<AliMUONLocalTrigger*>(next()) ) ) 
  {
    if (locTrg->IsNull()) continue;
   
    TArrayS xyPattern[2];
    locTrg->GetXPattern(xyPattern[0]);
    locTrg->GetYPattern(xyPattern[1]);

    // do we need this ? (Ch.F.)
//     for(Int_t cath=0; cath<2; ++cath)
//     {
//       for(Int_t ch=0; ch<4; ++ch)
//       {
//         if(xyPattern[cath][ch]==0) continue;
//       }
//     }
    
    Int_t nBoard = locTrg->LoCircuit();
    fDigitMaker.TriggerDigits(nBoard, xyPattern, digitStore);
  }
  return kTRUE;
}
