// @(#) $Id$

// Author: Uli Frankenfeld <mailto:franken@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group 

#include "AliHLTStandardIncludes.h"

#include "AliHLTLogging.h"
#include "AliHLTGlobalMerger.h"
#include "AliHLTTrack.h"
#include "AliHLTTransform.h"
#include "AliHLTTrackArray.h"

/** \class AliHLTGlobalMerger
<pre>
//_____________________________________________________________
// AliHLTGlobalMerger
//
// The L3 Slice merger
//
</pre>
*/

#if __GNUC__ >= 3
using namespace std;
#endif

ClassImp(AliHLTGlobalMerger)

AliHLTGlobalMerger::AliHLTGlobalMerger()
{
  //Default constructor. Use Setup to specify and setup the necessary parameters and arrays.
  Is2Global(kTRUE);
  SetParameter(0,0,0,0,0);
  fNSlices=0;
  fFirst=0;
  fLast=0;
}


AliHLTGlobalMerger::~AliHLTGlobalMerger()
{
  //Destructor
}

void AliHLTGlobalMerger::Setup(Int_t first,Int_t last)
{
  //Used to setup the arrays and everything
  
  fNSlices = last-first+1;
  fFirst = first;
  fLast = last;
  InitMerger(last-first+1);
}

void AliHLTGlobalMerger::InitSlice(Int_t slice)
{
  // 
  // Select Sector The following FillTracks call will 
  // fill this Sector
  //
  fSlice = slice;
  fCurrentTracks = fSlice - fFirst; 
}

Double_t AliHLTGlobalMerger::CheckTracks(AliHLTTrack *innertrack,AliHLTTrack *outertrack,Int_t slice)
{
  //Compare the tracks by propagating the outermost track to the last and first point plane
  //of the innermost track. This plane is defined by the padrow plane where these points
  //are.
  
  if(innertrack->GetCharge()!=outertrack->GetCharge()) return -1;
  
  Float_t angle = 0;//perpendicular to the padrowplane (in local system)
  AliHLTTransform::Local2GlobalAngle(&angle,slice);
  Double_t dx[2],dy[2],dz[2];
  Double_t diff =-1;
  AliHLTTrack *tracks[2];
  tracks[0] = innertrack;
  tracks[1] = outertrack;
  SortGlobalTracks(tracks,2);
  innertrack = tracks[0]; 
  outertrack = tracks[1];
  
  Float_t point[3];
  
  point[0]=innertrack->GetLastPointX();
  point[1]=innertrack->GetLastPointY();
  point[2]=innertrack->GetLastPointZ();
  AliHLTTransform::Global2LocHLT(point,slice);
  
  outertrack->CalculateReferencePoint(angle,point[0]);//local x = global distance to padrowplane
  if(!outertrack->IsPoint()) return diff;
  dx[0] = fabs(outertrack->GetPointX()-innertrack->GetLastPointX());
  dy[0] = fabs(outertrack->GetPointY()-innertrack->GetLastPointY());
  dz[0] = fabs(outertrack->GetPointZ()-innertrack->GetLastPointZ());
  
  point[0]=innertrack->GetFirstPointX();
  point[1]=innertrack->GetFirstPointY();
  point[2]=innertrack->GetFirstPointZ();
  AliHLTTransform::Global2LocHLT(point,slice);
  
  outertrack->CalculateReferencePoint(angle,point[0]);//local x = global distance to padrowplane
  if(!outertrack->IsPoint()) return diff;
  dx[1] = fabs(outertrack->GetPointX()-innertrack->GetFirstPointX());
  dy[1] = fabs(outertrack->GetPointY()-innertrack->GetFirstPointY());
  dz[1] = fabs(outertrack->GetPointZ()-innertrack->GetFirstPointZ());
  
  diff=0;//This was a tough bug to find....
  for(Int_t i=0; i<2; i++)
    diff += sqrt(dx[i]*dx[i] + dy[i]*dy[i] + dz[i]*dz[i]);
  return diff;
}

void AliHLTGlobalMerger::SlowMerge(Char_t *path)
{
  //Tuning of parameters. This matches _all_ tracks between two neighbouring
  //slices, and merges the ones which are closest in space. The difference
  //is written to a ntuppel, which can be used as a input for the SetParameters
  //when using normal Merge function.
  
  
  void* ntuple=GetNtuple();
  AliHLTTrack *track[2];
  AliHLTTrackArray *tout = GetOutTracks();
  if(fNSlices<2)
    {
      LOG(AliHLTLog::kWarning,"AliHLTGlobalMerger::SlowMerge","Slice Number")
	<<"Need more than one Slice!"<<ENDLOG;
      return;
    }
  
  for(Int_t i=0; i<fNSlices; i++)
    {
      //if(fNSlices!=18 && i+1 == fNSlices) continue; 
      Int_t slice = fFirst + i;
      AliHLTTrackArray *ttt0=GetInTracks(i);
      Int_t slice2 = i+1;
      //if(slice2==fNSlices) slice2 =0; 
      
      //Make sure slices are on the same side of the TPC
      if(slice2 == 18) slice2=0;
      else if(slice2 == 36) slice2=18;
      AliHLTTrackArray *ttt1=GetInTracks(slice2);
      //10 degrees -> the border of the slices in local coordinates
      Float_t angle = AliHLTTransform::Pi()/18; 
      AliHLTTransform::Local2GlobalAngle(&angle,slice);
      
      //In the two following cases, the angle is 2*pi, so set it back to 0 in order for
      //the calculation of crossing points to be correct.
      if(slice==17 || slice==35) 
	angle=0;
      if(i==0)
	ttt0->QSort();
      ttt1->QSort();
      for(Int_t s0=0;s0<ttt0->GetNTracks();s0++)
	{
	  AliHLTTrack *track0=ttt0->GetCheckedTrack(s0);
	  if(!track0) continue;
	  track0->CalculateHelix();
	  track0->CalculateEdgePoint(angle);
	  //      if(track0->IsPoint()) AddTrack(tout,track0);
	}
      for(Int_t s1=0;s1<ttt1->GetNTracks();s1++)
	{
	  AliHLTTrack *track1=ttt1->GetCheckedTrack(s1);
	  if(!track1) continue;
	  track1->CalculateHelix();
	  track1->CalculateEdgePoint(angle);
	  //      if(track1->IsPoint())  AddTrack(tout,track1); 
	}
      Bool_t merge = kTRUE;
      while(merge)
	{
	  Int_t min0=-1,min1=-1;
	  Double_t min=5;
	  Int_t n0=ttt0->GetNTracks(),n1=ttt1->GetNTracks();
	  for(Int_t s0=0;s0<n0;s0++)
	    {
	      AliHLTTrack *track0=ttt0->GetCheckedTrack(s0);
	      if(!track0) continue;
	      if(!track0->IsPoint()) continue;
	      for(Int_t s1=0;s1<n1;s1++)
		{
		  AliHLTTrack *track1=ttt1->GetCheckedTrack(s1);
		  if(!track1) continue;
		  if(!track1->IsPoint()) continue;
		  
		  //Double_t diff = TrackDiff(track0,track1,angle);
		  Double_t diff = CheckTracks(track0,track1,slice);
		  //PrintDiff(track0,track1);
		  if(diff>=0&&diff<min)
		    {
		      min=diff;
		      min0=s0;
		      min1=s1;
		    }
		}
	    }
	  if(min0>=0&&min1>=0)
	    {
	      AliHLTTrack *track0=ttt0->GetTrack(min0);
	      AliHLTTrack *track1=ttt1->GetTrack(min1);
	      track[0] = track0;
	      track[1] = track1;
	      SortGlobalTracks(track,2);
	      track1->CalculateEdgePoint((angle+AliHLTTransform::Pi()/9));
	      if(track1->IsPoint())//Check if the track will cross the boundary of yet another slice.
		MultiMerge(ttt1,track,2);
	      else
		MultiMerge(tout,track,2); 
	      track0->CalculateReferencePoint(angle);
	      track1->CalculateReferencePoint(angle);
	      //PrintDiff(track0,track1);
	      FillNtuple(ntuple,track0,track1);
	      ttt0->Remove(min0);
	      ttt1->Remove(min1);
	      
	    }
	  else merge = kFALSE;
	}
      ttt0->Compress();
      ttt1->Compress();
      LOG(AliHLTLog::kInformational,"AliHLTGlobalMerger::SlowMerge","Result")
	<<AliHLTLog::kDec<<"Merged Tracks: "<<tout->GetNTracks()<<" at:"
	<<angle<<ENDLOG;
    }
  Char_t fname[1024];
  sprintf(fname,"%s/merge_parameters.root",path);
  WriteNtuple(fname,ntuple);
}

void AliHLTGlobalMerger::Merge()
{
  //Normal merging procedure. Matches tracks which are within limits
  //set by SetParameters. Parameters can be tuned by SlowMerge.
  
  AliHLTTrack *track[2];
  AliHLTTrackArray *tout = GetOutTracks();
  if(fNSlices<2)
    {
      LOG(AliHLTLog::kWarning,"AliHLTGlobalMerger::Merge","Slice Number")
	<<"Need more than one Slice!"<<ENDLOG;
      return;
    }
  for(Int_t i=0; i<fNSlices; i++)
    {
      //if(fNSlices!=18 && i+1 == fNSlices) continue; 
      Int_t slice = fFirst + i;
      AliHLTTrackArray *ttt0=GetInTracks(i);
      Int_t slice2 = i+1;
      //if(slice2==fNSlices) slice2 =0;
      
      //Make sure slices are on the same side of the TPC
      if(slice2 == 18) slice2=0;
      else if(slice2 == 36) slice2=18;
      AliHLTTrackArray *ttt1=GetInTracks(slice2);
      //10 degrees -> the border of the slices in local coordinates
      Float_t angle = AliHLTTransform::Pi()/18; 
      AliHLTTransform::Local2GlobalAngle(&angle,slice);
      
      //In the two following cases, the angle is 2*pi, so set it back to 0 in order for
      //the calculation of crossing points to be correct.
      if(slice==17 || slice==35)
	angle=0;
      if(i==0)
	ttt0->QSort();
      ttt1->QSort();
      Bool_t *ismatched0  = new Bool_t[ttt0->GetNTracks()];
      Bool_t *ismatched1  = new Bool_t[ttt1->GetNTracks()];
      Int_t n0=0,n1=0;
      for(Int_t s0=0;s0<ttt0->GetNTracks();s0++)
	{
	  ismatched0[s0]=kFALSE;
	  AliHLTTrack *track0=ttt0->GetCheckedTrack(s0);
	  if(!track0) continue;
	  track0->CalculateHelix();
	  track0->CalculateEdgePoint(angle);
	  if(track0->IsPoint()) 
	    {
	      n0++;
	      track0->CalculateReferencePoint(angle);
	    }
	}
      for(Int_t s1=0;s1<ttt1->GetNTracks();s1++)
	{
	  ismatched1[s1]=kFALSE;
	  AliHLTTrack *track1=ttt1->GetCheckedTrack(s1);
	  if(!track1) continue;
	  track1->CalculateHelix();
	  track1->CalculateEdgePoint(angle);
	  if(track1->IsPoint()) 
	    {
	      n1++;
	      track1->CalculateReferencePoint(angle);
	    }
	}
      for(Int_t s0=0;s0<ttt0->GetNTracks();s0++)
	{
	  if(ismatched0[s0]) continue;
	  AliHLTTrack *track0=ttt0->GetCheckedTrack(s0);
	  if(!track0) continue;
	  if(!track0->IsPoint()) continue;
	  for(Int_t s1=0;s1<ttt1->GetNTracks();s1++)
	    {
	      if(ismatched1[s1]) continue;
	      AliHLTTrack *track1=ttt1->GetCheckedTrack(s1);
	      if(!track1) continue;
	      if(!track1->IsPoint()) continue;
	      if(IsRTrack(track0,track1))
		{
		  track[0] = track0;
		  track[1] = track1;
		  SortGlobalTracks(track,2);
		  Double_t r0 = pow(track[0]->GetLastPointX(),2)+
		    pow(track[0]->GetLastPointY(),2);
		  Double_t r1 = pow(track[1]->GetFirstPointX(),2)+
		    pow(track[1]->GetFirstPointY(),2);
		  if(r0<r1)
		    {
		      MultiMerge(tout,track,2); 
		      ismatched0[s0]=kTRUE;
		      ismatched1[s1]=kTRUE;
		      ttt0->Remove(s0);
		      ttt1->Remove(s1);
		      break;
		      /*
			The track is merged, so we will _not_ look for more matches.
			Because there could easily be more matches, if a track is being
			split within the sector....
		      */
		    }
		}
	    }
	}
      LOG(AliHLTLog::kInformational,"AliHLTGlobalMerger::Merge","Result")
	<<AliHLTLog::kDec<<"slice0: "<<n0<<" slice1: "<<n1
	<<" Merged Tracks: "<<tout->GetNTracks()<<ENDLOG;
      delete [] ismatched0;
      delete [] ismatched1;
    }
}
