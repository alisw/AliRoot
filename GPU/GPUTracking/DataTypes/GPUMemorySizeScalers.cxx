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

/// \file GPUMemorySizeScalers.cxx
/// \author David Rohr

#include "GPUMemorySizeScalers.h"
#include "GPULogging.h"

using namespace GPUCA_NAMESPACE::gpu;

void GPUMemorySizeScalers::rescaleMaxMem(size_t newAvailableMemory)
{
  GPUMemorySizeScalers tmp;
  double scaleFactor = (double)newAvailableMemory / tmp.availableMemory;
  if (scaleFactor != 1.) {
    GPUInfo("Rescaling buffer size limits from %lu to %lu bytes of memory (factor %f)", tmp.availableMemory, newAvailableMemory, scaleFactor);
  }
  tpcMaxPeaks = (double)tmp.tpcMaxPeaks * scaleFactor;
  tpcMaxClusters = (double)tmp.tpcMaxClusters * scaleFactor;
  tpcMaxStartHits = (double)tmp.tpcMaxStartHits * scaleFactor;
  tpcMaxRowStartHits = (double)tmp.tpcMaxRowStartHits * scaleFactor;
  tpcMaxTracklets = (double)tmp.tpcMaxTracklets * scaleFactor;
  tpcMaxTrackletHits = (double)tmp.tpcMaxTrackletHits * scaleFactor;
  tpcMaxSectorTracks = (double)tmp.tpcMaxSectorTracks * scaleFactor;
  tpcMaxSectorTrackHits = (double)tmp.tpcMaxSectorTrackHits * scaleFactor;
  tpcMaxMergedTracks = (double)tmp.tpcMaxMergedTracks * scaleFactor;
  tpcMaxMergedTrackHits = (double)tmp.tpcMaxMergedTrackHits * scaleFactor;
  availableMemory = newAvailableMemory;
}
