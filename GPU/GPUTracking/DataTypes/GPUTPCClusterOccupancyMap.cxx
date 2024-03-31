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

/// \file GPUTPCClusterOccupancyMap.cxx
/// \author David Rohr

#include "GPUTPCClusterOccupancyMap.h"
#include "GPUParam.h"

using namespace GPUCA_NAMESPACE::gpu;

GPUd() unsigned int GPUTPCClusterOccupancyMapBin::getNBins(const GPUParam& param)
{
  unsigned int maxTimeBin = param.par.continuousTracking ? param.par.continuousMaxTimeBin : TPC_MAX_TIME_BIN_TRIGGERED;
  return (maxTimeBin + param.rec.tpc.occupancyMapTimeBins) / param.rec.tpc.occupancyMapTimeBins; // Not -1, since maxTimeBin is allowed
}

GPUd() unsigned int GPUTPCClusterOccupancyMapBin::getTotalSize(const GPUParam& param)
{
  return getNBins(param) * sizeof(GPUTPCClusterOccupancyMapBin);
}
