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

/// \file GPUTPCStartHitsSorter.cxx
/// \author David Rohr

#include "GPUTPCStartHitsSorter.h"
#include "GPUTPCTracker.h"

#include "GPUTPCHit.h"
#include "GPUCommonMath.h"
#include "GPUDefMacros.h"

using namespace GPUCA_NAMESPACE::gpu;
template <>
GPUdii() void GPUTPCStartHitsSorter::Thread<0>(int32_t nBlocks, int32_t nThreads, int32_t iBlock, int32_t iThread, GPUsharedref() GPUSharedMemory& s, processorType& GPUrestrict() tracker)
{
  // Sorts the Start Hits by Row Index
  if (iThread == 0) {
    const int32_t tmpNRows = GPUCA_ROW_COUNT - 6;
    const int32_t nRows = iBlock == (nBlocks - 1) ? (tmpNRows - (tmpNRows / nBlocks) * (nBlocks - 1)) : (tmpNRows / nBlocks);
    const int32_t nStartRow = (tmpNRows / nBlocks) * iBlock + 1;
    int32_t startOffset2 = 0;
    GPUCA_UNROLL(, U())
    for (int32_t ir = 1; ir < GPUCA_ROW_COUNT - 5; ir++) {
      if (ir < nStartRow) {
        startOffset2 += tracker.mRowStartHitCountOffset[ir];
      }
    }
    s.mStartOffset = startOffset2;
    s.mNRows = nRows;
    s.mStartRow = nStartRow;
  }
  GPUbarrier();

  int32_t startOffset = s.mStartOffset;
#ifdef __HIPCC__ // TODO: Fixme
  for (int32_t ir = -1; ++ir < s.mNRows;) {
#else
  for (int32_t ir = 0; ir < s.mNRows; ir++) {
#endif
    GPUglobalref() GPUTPCHitId* const GPUrestrict() startHits = tracker.mTrackletStartHits;
    GPUglobalref() GPUTPCHitId* const GPUrestrict() tmpStartHits = tracker.mTrackletTmpStartHits + (s.mStartRow + ir) * tracker.mNMaxRowStartHits;

    const int32_t tmpLen = tracker.mRowStartHitCountOffset[ir + s.mStartRow]; // Length of hits in row stored by StartHitsFinder
    for (int32_t j = iThread; j < tmpLen; j += nThreads) {
      startHits[startOffset + j] = tmpStartHits[j];
    }
    startOffset += tmpLen;
  }
}
