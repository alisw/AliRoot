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

/// \file GPUTPCGMMergerDump.cxx
/// \author David Rohr

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "GPUTPCSliceOutTrack.h"
#include "GPUTPCTracker.h"
#include "GPUTPCClusterData.h"
#include "GPUTPCTrackParam.h"
#include "GPUTPCGMMerger.h"
#include "GPUO2DataTypes.h"
#include "GPUCommonMath.h"
#include "GPUTPCTrackParam.h"
#include "GPUTPCSliceOutput.h"
#include "GPUTPCGMMergedTrack.h"
#include "GPUParam.h"
#include "GPUTPCGMTrackParam.h"
#include "GPUTPCGMSliceTrack.h"
#include "GPUTPCGMBorderTrack.h"

using namespace GPUCA_NAMESPACE::gpu;

void GPUTPCGMMerger::DumpSliceTracks(std::ostream& out)
{
  out << "\nTPC Merger Slice Tracks\n";
  for (int iSlice = 0; iSlice < NSLICES; iSlice++) {
    out << "Slice Track Info Index" << mSliceTrackInfoIndex[iSlice] << " / " << mSliceTrackInfoIndex[NSLICES + iSlice] << "\n";
    for (int iGlobal = 0; iGlobal < 2; iGlobal++) {
      out << "  Track type " << iGlobal << "\n";
      for (int j = mSliceTrackInfoIndex[iSlice + NSLICES * iGlobal]; j < mSliceTrackInfoIndex[iSlice + NSLICES * iGlobal + 1]; j++) {
        const auto& trk = mSliceTrackInfos[j];
        out << "    Track " << j << ": X " << trk.X() << " A " << trk.Alpha() << " Y " << trk.Y() << " Z " << trk.Z() << " Phi " << trk.SinPhi() << " Tgl " << trk.DzDs() << " QPt " << trk.QPt() << "\n";
      }
    }
  }
}

void GPUTPCGMMerger::DumpMergedWithinSlices(std::ostream& out)
{
  out << "\nTPC Merger Merge Within Slices\n";
  for (int iSlice = 0; iSlice < NSLICES; iSlice++) {
    for (int j = mSliceTrackInfoIndex[iSlice]; j < mSliceTrackInfoIndex[iSlice + 1]; j++) {
      const auto& trk = mSliceTrackInfos[j];
      if (trk.NextSegmentNeighbour()) {
        out << "  Track " << j << ": Neighbour " << trk.PrevSegmentNeighbour() << " / " << trk.NextSegmentNeighbour() << "\n";
      }
    }
  }
}

void GPUTPCGMMerger::DumpMergedBetweenSlices(std::ostream& out)
{
  out << "\nTPC Merger Merge Within Slices\n";
  for (int iSlice = 0; iSlice < NSLICES; iSlice++) {
    for (int j = mSliceTrackInfoIndex[iSlice]; j < mSliceTrackInfoIndex[iSlice + 1]; j++) {
      const auto& trk = mSliceTrackInfos[j];
      if (trk.NextNeighbour() || trk.PrevNeighbour()) {
        out << "  Track " << j << ": Neighbour " << trk.PrevNeighbour() << " / " << trk.NextNeighbour() << "\n";
      }
      if (trk.PrevNeighbour() == -1 && trk.PrevSegmentNeighbour() == -1) {
        PrintMergeGraph(&trk, out);
      }
    }
  }
}

void GPUTPCGMMerger::DumpCollected(std::ostream& out)
{
  out << "\nTPC Merger Collected Tracks\n";
  for (int i = 0; i < mNOutputTracks; i++) {
    const auto& trk = mOutputTracks[i];
    const auto& p = trk.GetParam();
    out << "  Track " << i << ": Loop " << trk.Looper() << " Alpha " << trk.GetAlpha() << " X " << p.GetX() << " Y " << p.GetY() << " Z " << p.GetZ() << " SPhi " << p.GetSinPhi() << " Tgl " << p.GetDzDs() << " QPt " << p.GetQPt() << " NCl " << trk.NClusters() << "\n";
  }
}

void GPUTPCGMMerger::DumpMergeCE(std::ostream& out)
{
  out << "\nTPC Merger Merge CE\n";
  for (int i = 0; i < mNOutputTracks; i++) {
    const auto& trk = mOutputTracks[i];
    if (trk.CCE()) {
      out << "  Track " << i << ": CCE\n";
    }
  }
}

void GPUTPCGMMerger::DumpFitPrepare(std::ostream& out)
{
  out << "\nTPC Merger Refit Prepare\n";
  out << "  Sort\n";
  for (int i = 0; i < mNOutputTracks; i++) {
    out << "    " << i << ": " << mTrackOrder[i] << "\n";
  }
  out << "  Clusters\n";
  for (int i = 0; i < mNOutputTrackClusters; i++) {
    out << "    Cluster state " << i << ": " << (int)mClusters[i].state << "\n";
  }
  for (int i = 0; i < mMaxID; i++) {
    out << "    Cluster attachment " << i << ": " << (mClusterAttachment[i] & attachTrackMask) << " / " << (mClusterAttachment[i] & attachFlagMask) << "\n";
  }
}

void GPUTPCGMMerger::DumpRefit(std::ostream& out)
{
  out << "\nTPC Merger Refit\n";
  for (int i = 0; i < mNOutputTracks; i++) {
    const auto& trk = mOutputTracks[i];
    const auto& p = trk.GetParam();
    out << "  Track " << i << ": OK " << trk.OK() << " Alpha " << trk.GetAlpha() << " X " << p.GetX() << " Y " << p.GetY() << " Z " << p.GetZ() << " SPhi " << p.GetSinPhi() << " Tgl " << p.GetDzDs() << " QPt " << p.GetQPt() << " NCl " << trk.NClusters() << " / " << trk.NClustersFitted() << "\n";
  }
}

void GPUTPCGMMerger::DumpFinal(std::ostream& out)
{
  out << "\nTPC Merger Finalized\n";
  for (int i = 0; i < mNOutputTrackClusters; i++) {
    out << "    Cluster state " << i << ": " << (int)mClusters[i].state << "\n";
  }
  for (int i = 0; i < mMaxID; i++) {
    out << "    Cluster attachment " << i << ": " << (mClusterAttachment[i] & attachTrackMask) << " / " << (mClusterAttachment[i] & attachFlagMask) << "\n";
  }
}
