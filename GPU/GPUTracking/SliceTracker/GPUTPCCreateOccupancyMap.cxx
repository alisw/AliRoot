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

/// \file GPUTPCCreateOccupancyMap.cxx
/// \author David Rohr

#include "GPUTPCCreateOccupancyMap.h"
#include "GPUTPCClusterOccupancyMap.h"

using namespace GPUCA_NAMESPACE::gpu;

template <>
GPUdii() void GPUTPCCreateOccupancyMap::Thread<GPUTPCCreateOccupancyMap::fill>(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUTPCClusterOccupancyMapBin* GPUrestrict() map)
{
  const GPUTrackingInOutPointers& GPUrestrict() ioPtrs = processors.ioPtrs;
  const o2::tpc::ClusterNativeAccess* GPUrestrict() clusters = ioPtrs.clustersNative;
  GPUParam& GPUrestrict() param = processors.param;
  const int iSliceRow = iBlock * nThreads + iThread;
  if (iSliceRow >= GPUCA_ROW_COUNT * GPUCA_NSLICES) {
    return;
  }
  const unsigned int iSlice = iSliceRow / GPUCA_ROW_COUNT;
  const unsigned int iRow = iSliceRow % GPUCA_ROW_COUNT;
  for (unsigned int i = 0; i < clusters->nClusters[iSlice][iRow]; i++) {
    const unsigned int bin = clusters->clusters[iSlice][iRow][i].getTime() / param.rec.tpc.occupancyMapTimeBins;
    map[bin].bin[iSlice][iRow]++;
  }
}

template <>
GPUdii() void GPUTPCCreateOccupancyMap::Thread<GPUTPCCreateOccupancyMap::fold>(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors, GPUTPCClusterOccupancyMapBin* GPUrestrict() map, unsigned int* GPUrestrict() output)
{
  GPUParam& GPUrestrict() param = processors.param;
  const unsigned int bin = iBlock * nThreads + iThread;
  if (bin >= GPUTPCClusterOccupancyMapBin::getNBins(param)) {
    return;
  }
  int binmin = CAMath::Max<int>(0, bin - param.rec.tpc.occupancyMapTimeBinsAverage);
  int binmax = CAMath::Min<int>(GPUTPCClusterOccupancyMapBin::getNBins(param), bin + param.rec.tpc.occupancyMapTimeBinsAverage + 1);
  unsigned int sum = 0;
  for (int i = binmin; i < binmax; i++) {
    for (int iSliceRow = 0; iSliceRow < GPUCA_NSLICES * GPUCA_ROW_COUNT; iSliceRow++) {
      sum += (&map[i].bin[0][0])[iSliceRow];
    }
  }
  sum /= binmax - binmin;
  output[bin] = sum;
}
