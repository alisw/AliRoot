//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUTPCTrackletSelector.cxx
/// \author Sergey Gorbunov, Ivan Kisel, David Rohr

#include "GPUTPCTrackletSelector.h"
#include "GPUTPCTrack.h"
#include "GPUTPCTracker.h"
#include "GPUTPCTrackParam.h"
#include "GPUTPCTracklet.h"
#include "GPUCommonMath.h"

using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUd() void GPUTPCTrackletSelector::Thread<0>(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() MEM_LOCAL(GPUTPCSharedMemory) & s, processorType& tracker)
{
  // select best tracklets and kill clones

  if (iThread == 0) {
    s.mNTracklets = *tracker.NTracklets();
    s.mNThreadsTotal = nThreads * nBlocks;
    s.mItr0 = nThreads * iBlock;
  }
  GPUbarrier();

  GPUTPCHitId trackHits[GPUCA_ROW_COUNT - GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE];

  for (int itr = s.mItr0 + iThread; itr < s.mNTracklets; itr += s.mNThreadsTotal) {
    while (tracker.Tracklets()[itr].NHits() == 0) {
      itr += s.mNThreadsTotal;
      if (itr >= s.mNTracklets) {
        return;
      }
    }

    GPUglobalref() MEM_GLOBAL(GPUTPCTracklet)& tracklet = tracker.Tracklets()[itr];
    const int kMaxRowGap = 4;
    const float kMaxShared = .1f;

    int firstRow = tracklet.FirstRow();
    int lastRow = tracklet.LastRow();

    const int w = tracklet.HitWeight();

    int irow = firstRow;

    int gap = 0;
    int nShared = 0;
    int nHits = 0;
    const int minHits = tracker.Param().rec.MinNTrackClusters == -1 ? GPUCA_TRACKLET_SELECTOR_MIN_HITS(tracklet.Param().QPt()) : tracker.Param().rec.MinNTrackClusters;

    for (irow = firstRow; irow <= lastRow && lastRow - irow + nHits >= minHits; irow++) {
      gap++;
#ifdef GPUCA_EXTERN_ROW_HITS
      calink ih = tracker.TrackletRowHits()[irow * s.mNTracklets + itr];
#else
      calink ih = tracklet.RowHit(irow);
#endif // GPUCA_EXTERN_ROW_HITS
      if (ih != CALINK_INVAL) {
        GPUglobalref() const MEM_GLOBAL(GPUTPCRow)& row = tracker.Row(irow);
        bool own = (tracker.HitWeight(row, ih) <= w);
        bool sharedOK = ((nShared < nHits * kMaxShared));
        if (own || sharedOK) { // SG!!!
          gap = 0;
#if GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
          if (nHits < GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE) {
            s.mHits[iThread][nHits].Set(irow, ih);
          } else
#endif // GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
          {
            trackHits[nHits - GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE].Set(irow, ih);
          }
          nHits++;
          if (!own) {
            nShared++;
          }
        }
      }

      if (gap > kMaxRowGap || irow == lastRow) { // store
        if (nHits >= minHits) {
          unsigned int itrout = CAMath::AtomicAdd(tracker.NTracks(), 1);
          if (itrout + 1 >= tracker.NMaxTracks()) {
            tracker.CommonMemory()->kernelError = GPUCA_ERROR_TRACK_OVERFLOW;
            CAMath::AtomicExch(tracker.NTracks(), 0);
            return;
          }
          unsigned int nFirstTrackHit = CAMath::AtomicAdd(tracker.NTrackHits(), nHits);
          if ((nFirstTrackHit + nHits) >= tracker.NMaxTrackHits()) {
            tracker.CommonMemory()->kernelError = GPUCA_ERROR_TRACK_HIT_OVERFLOW;
            CAMath::AtomicExch(tracker.NTrackHits(), tracker.NMaxTrackHits());
            CAMath::AtomicExch(tracker.NTracks(), 0);
            return;
          }
          tracker.Tracks()[itrout].SetAlive(1);
          tracker.Tracks()[itrout].SetLocalTrackId(itrout);
          tracker.Tracks()[itrout].SetParam(tracklet.Param());
          tracker.Tracks()[itrout].SetFirstHitID(nFirstTrackHit);
          tracker.Tracks()[itrout].SetNHits(nHits);
          for (int jh = 0; jh < nHits; jh++) {
#if GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
            if (jh < GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE) {
              tracker.TrackHits()[nFirstTrackHit + jh] = s.mHits[iThread][jh];
            } else
#endif // GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE != 0
            {
              tracker.TrackHits()[nFirstTrackHit + jh] = trackHits[jh - GPUCA_TRACKLET_SELECTOR_HITS_REG_SIZE];
            }
          }
        }
        nHits = 0;
        gap = 0;
        nShared = 0;
      }
    }
  }
}
