// @(#) $Id$
// Original: AliHLTConfMapTrack.cxx,v 1.16 2005/06/14 10:55:21 cvetan 

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

/** @file   AliHLTTPCConfMapTrack.cxx
    @author Anders Vestbo, maintained by Matthias Richter
    @date   
    @brief  Track class for conformal mapper
*/

#include "AliHLTTPCRootTypes.h"
#include "AliHLTTPCLogging.h"
#include "AliHLTTPCVertex.h"
#include "AliHLTTPCConfMapPoint.h"
#include "AliHLTTPCConfMapFit.h"
#include "AliHLTTPCConfMapTrack.h"
#include "AliHLTTPCTransform.h"
//#include "AliHLTTPC.h"

#if __GNUC__ >= 3
using namespace std;
#endif

ClassImp(AliHLTTPCConfMapTrack)


AliHLTTPCConfMapTrack::AliHLTTPCConfMapTrack()
  :
  fCurrentHit(0),
  fLastHit(0),
  fFirstHit(0),
  fs11Xy(0),
  fs12Xy(0),
  fs22Xy(0),
  fg1Xy(0),
  fg2Xy(0),
  fs11Sz(0),
  fs12Sz(0),
  fs22Sz(0),
  fg1Sz(0),
  fg2Sz(0),
  fddXy(0),
  fa1Xy(0),
  fa2Xy(0),
  fddSz(0),
  fa1Sz(0),
  fa2Sz(0)
{
  //Constructor
  fChiSq[0] = 0.;
  fChiSq[1] = 0.;
}

AliHLTTPCConfMapTrack::AliHLTTPCConfMapTrack(const AliHLTTPCConfMapTrack&)
  :
  fCurrentHit(0),
  fLastHit(0),
  fFirstHit(0),
  fs11Xy(0),
  fs12Xy(0),
  fs22Xy(0),
  fg1Xy(0),
  fg2Xy(0),
  fs11Sz(0),
  fs12Sz(0),
  fs22Sz(0),
  fg1Sz(0),
  fg2Sz(0),
  fddXy(0),
  fa1Xy(0),
  fa2Xy(0),
  fddSz(0),
  fa1Sz(0),
  fa2Sz(0)
{
  // dummy copy constructor
  //HLTFatal("copy constructor untested");
}

AliHLTTPCConfMapTrack& AliHLTTPCConfMapTrack::operator=(const AliHLTTPCConfMapTrack&)
{ 
  // dummy assignment operator
  //HLTFatal("assignment operator untested");
  return *this;
}

AliHLTTPCConfMapTrack::~AliHLTTPCConfMapTrack()
{
  //deconstructor
}

void AliHLTTPCConfMapTrack::DeleteCandidate()
{
  //Deletes this track by resetting all its parameters. Does not delete
  //the object itself.

  AliHLTTPCConfMapPoint *curHit = (AliHLTTPCConfMapPoint*)fFirstHit;
  AliHLTTPCConfMapPoint *nextHit;
  
  while(curHit != 0)
    {
      nextHit = (AliHLTTPCConfMapPoint*)curHit->GetNextTrackHit();
      curHit->SetNextTrackHit(0);
      curHit = nextHit;
    }
  
  UInt_t *hit_numbers = GetHitNumbers();
  for(Int_t i=0; i<GetNHits(); i++)
    {
      //fHitNumbers[i] = 0;
      hit_numbers[i]=0;
    }
    
  SetRadius(0.);
  SetCenterX(0.);
  SetCenterY(0.);
  
  ComesFromMainVertex(false);

  SetNHits(0);
  SetCharge(0);
  fChiSq[0] = 0.;
  fChiSq[1] = 0.;
}


void AliHLTTPCConfMapTrack::SetProperties(Bool_t usage)
{
  //Set the hits to this track to 'usage'
  for(StartLoop(); LoopDone(); GetNextHit())
    {
      AliHLTTPCConfMapPoint *p = (AliHLTTPCConfMapPoint*)fCurrentHit;
      p->SetUsage(usage);
    }
  return;
}

void AliHLTTPCConfMapTrack::Reset()
{
  //Resets the fit parameters of this track.

  //xy-plane
  fs11Xy   = 0;
  fs12Xy   = 0;
  fs22Xy   = 0;
  fg1Xy    = 0;
  fg2Xy    = 0;
  fChiSq[0]  = 0.;
    
  //sz-plane
  fs11Sz = 0;
  fs12Sz = 0;
  fs22Sz = 0;
  fg1Sz  = 0;
  fg2Sz  = 0;
  fChiSq[1] = 0; 
  SetLength(0);
  SetNHits(0);
}

void AliHLTTPCConfMapTrack::UpdateParam(AliHLTTPCConfMapPoint *thisHit)
{
  //Function to update fit parameters of track
  //Also, it updates the hit pointers.
  

  //Increment the number of hits assigned to this track:

  //fNHits++;
  Int_t nhits = GetNHits();
  nhits++;
  SetNHits(nhits); //SetNHits(nhits++);

  //Set the hit pointers:
  //if(fNHits == 1)
  if(GetNHits()==1)  
    fFirstHit = thisHit;
  else
    ((AliHLTTPCConfMapPoint*)fLastHit)->SetNextTrackHit(thisHit);
  fLastHit = thisHit;

  
  fs11Xy = fs11Xy + thisHit->GetXYWeight() ;
  fs12Xy = fs12Xy + thisHit->GetXYWeight() * thisHit->GetXprime() ;
  fs22Xy = fs22Xy + thisHit->GetXYWeight() * pow((thisHit->GetXprime()),2) ;
  fg1Xy  = fg1Xy  + thisHit->GetXYWeight() * thisHit->GetYprime() ;
  fg2Xy  = fg2Xy  + thisHit->GetXYWeight() * thisHit->GetXprime() * thisHit->GetYprime() ;
    
  fddXy  = fs11Xy * fs22Xy - pow((fs12Xy),2) ;
  if ( fddXy != 0 ) 
    {
      fa1Xy  = ( fg1Xy * fs22Xy - fg2Xy * fs12Xy ) / fddXy ;
      fa2Xy  = ( fg2Xy * fs11Xy - fg1Xy * fs12Xy ) / fddXy ;
    }

  //     Now in the sz plane
  fs11Sz = fs11Sz + thisHit->GetZWeight() ;
  fs12Sz = fs12Sz + thisHit->GetZWeight() * thisHit->GetS() ;
  fs22Sz = fs22Sz + thisHit->GetZWeight() * thisHit->GetS() * thisHit->GetS() ;
  fg1Sz  = fg1Sz  + thisHit->GetZWeight() * thisHit->GetZ() ;
  fg2Sz  = fg2Sz  + thisHit->GetZWeight() * thisHit->GetS() * thisHit->GetZ() ;
  
        
  fddSz  = fs11Sz * fs22Sz -  fs12Sz * fs12Sz ;
  if ( fddSz != 0 ) {
    fa1Sz  = ( fg1Sz * fs22Sz - fg2Sz * fs12Sz ) / fddSz ;
    fa2Sz  = ( fg2Sz * fs11Sz - fg1Sz * fs12Sz ) / fddSz ;
  }
}


void AliHLTTPCConfMapTrack::Fill(AliHLTTPCVertex *vertex,Double_t max_Dca)
{
    //Fill track variables with or without fit.
    
    // for straight line fit
    if (AliHLTTPCTransform::GetBFieldValue() == 0.0 ){

	SetRadius(0.);	
	SetPt(0.);
	
	//go for fit of straight line in real space
	AliHLTTPCConfMapFit *fit = new AliHLTTPCConfMapFit(this,vertex);
    /* Matthias 13.12.2006
     * the global variable AliHLTTPCS::fgDoVertexFit has never been used so far
     * and has always been kTRUE.
     * In order to remove the AliHLTTPC class (which is the old steering class for
     * HLT (TPC) tracking) from the compilation, this function can not be activated
     * again. We have to think about a more elegant way to specify the parameters
     * anyway. 
	ComesFromMainVertex(AliHLTTPC::DoVertexFit());
     */
	ComesFromMainVertex(kTRUE);
	fit->FitStraightLine();
	    
	AliHLTTPCConfMapPoint *fHit = (AliHLTTPCConfMapPoint*)fFirstHit;
	SetLastPoint(fHit->GetX(),fHit->GetY(),fHit->GetZ());
	
	UpdateToFirstPoint();
	
	delete fit;
	
    }
    // for helix fit
    else { 
      //fRadius = sqrt(fa2Xy*fa2Xy+1)/(2*fabs(fa1Xy));
      Double_t radius = sqrt(fa2Xy*fa2Xy+1)/(2*fabs(fa1Xy));
      SetRadius(radius);
      
      //fPt = (Double_t)(AliHLTTPCTransform::GetBFieldValue() * fRadius);
      Double_t pt = (Double_t)(AliHLTTPCTransform::GetBFieldValue() * GetRadius());
      SetPt(pt);
      
      if(GetPt() > max_Dca) //go for fit of helix in real space
	{
	  AliHLTTPCConfMapFit *fit = new AliHLTTPCConfMapFit(this,vertex);
    /* Matthias 13.12.2006
     * the global variable AliHLTTPCS::fgDoVertexFit has never been used so far
     * and has always been kTRUE.
     * In order to remove the AliHLTTPC class (which is the old steering class for
     * HLT (TPC) tracking) from the compilation, this function can not be activated
     * again. We have to think about a more elegant way to specify the parameters
     * anyway. 
	    ComesFromMainVertex(AliHLTTPC::DoVertexFit());
     */
	  ComesFromMainVertex(kTRUE);
	  fit->FitHelix();
	  
	  //AliHLTTPCConfMapPoint *lHit = (AliHLTTPCConfMapPoint*)fLastHit;
	  AliHLTTPCConfMapPoint *fHit = (AliHLTTPCConfMapPoint*)fFirstHit;
	  SetLastPoint(fHit->GetX(),fHit->GetY(),fHit->GetZ());
	  
	  UpdateToFirstPoint();
	  
	  delete fit;
	}
      else if(GetPt() == 0) 
	{
	  LOG(AliHLTTPCLog::kError,"AliHLTTPCConfMapTrack::Fill","Tracks")<<AliHLTTPCLog::kDec<<
	    "Found track with Pt=0!!!"<<ENDLOG;
	}
      else
	{
	  LOG(AliHLTTPCLog::kError,"AliHLTTPCConfMapTrack::Fill","Tracks")<<AliHLTTPCLog::kDec<<
	    "Track with pt<max_Dca :"<<GetPt()<<ENDLOG;
	}
    }
}

Int_t AliHLTTPCConfMapTrack::GetMCLabel()
{
  //For evaluation study.
  //Returns the MCtrackID of the belonging clusters.
  //If MCLabel < 0, means that track is fake.

  return 0;
  /*
  Int_t num_of_clusters = GetNumberOfPoints();
  S *s=new S[num_of_clusters];
  Int_t i;
  for (i=0; i<num_of_clusters; i++) s[i].lab=s[i].max=0;
  
  Int_t lab=123456789;
  for (i=0; i<num_of_clusters; i++) {
    AliHLTTPCConfMapPoint *c=(AliHLTTPCConfMapPoint*)fPoints->UncheckedAt(i);
    lab=fabs(c->fMCTrackID[0]);
    Int_t j;
    for (j=0; j<num_of_clusters; j++)
      if (s[j].lab==lab || s[j].max==0) break;
    s[j].lab=lab;
    s[j].max++;
  }
  
  Int_t max=0;
  for (i=0; i<num_of_clusters; i++) 
    if (s[i].max>max) {max=s[i].max; lab=s[i].lab;}
    
  delete[] s;
  
  for (i=0; i<num_of_clusters; i++) {
    AliHLTTPCConfMapPoint *c=(AliHLTTPCConfMapPoint*)fPoints->UncheckedAt(i);
    if (fabs(c->fMCTrackID[1]) == lab ||
	fabs(c->fMCTrackID[2]) == lab ) max++;
  }
  
  //check if more than 10% of the clusters are incorrectly assigned (fake track):
  
  if (1.-Float_t(max)/num_of_clusters > 0.10) 
    {
      return -lab;
    }
  Int_t tail=Int_t(0.08*174);
  if (num_of_clusters < tail) return lab;
  
  max=0;
  for (i=1; i<=tail; i++) {
    AliHLTTPCConfMapPoint *c = (AliHLTTPCConfMapPoint*)fPoints->UncheckedAt(num_of_clusters-i);
    if (lab == fabs(c->fMCTrackID[0]) ||
	lab == fabs(c->fMCTrackID[1]) ||
	lab == fabs(c->fMCTrackID[2])) max++;
  }
  if (max < Int_t(0.5*tail)) 
    {
      //printf("Wrong innermost clusters\n");
      return -lab;
    }
  return lab;
  */
}

