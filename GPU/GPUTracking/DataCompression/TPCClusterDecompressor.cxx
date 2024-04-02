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

/// \file TPCClusterDecompressor.cxx
/// \author David Rohr

#include "TPCClusterDecompressor.h"
#include "GPUO2DataTypes.h"
#include "GPUParam.h"
#include "GPUTPCCompressionTrackModel.h"
#include "GPULogging.h"
#include <algorithm>
#include <cstring>
#include <atomic>
#include "TPCClusterDecompressor.inc"

using namespace GPUCA_NAMESPACE::gpu;
using namespace o2::tpc;

int TPCClusterDecompressor::decompress(const CompressedClustersFlat* clustersCompressed, o2::tpc::ClusterNativeAccess& clustersNative, std::function<o2::tpc::ClusterNative*(size_t)> allocator, const GPUParam& param)
{
  CompressedClusters c;
  const CompressedClusters* p;
  if (clustersCompressed->ptrForward) {
    p = clustersCompressed->ptrForward;
  } else {
    c = *clustersCompressed;
    p = &c;
  }
  return decompress(p, clustersNative, allocator, param);
}

int TPCClusterDecompressor::decompress(const CompressedClusters* clustersCompressed, o2::tpc::ClusterNativeAccess& clustersNative, std::function<o2::tpc::ClusterNative*(size_t)> allocator, const GPUParam& param)
{
  if (clustersCompressed->nTracks && clustersCompressed->solenoidBz != -1e6f && clustersCompressed->solenoidBz != param.bzkG) {
    throw std::runtime_error("Configured solenoid Bz does not match value used for track model encoding");
  }
  if (clustersCompressed->nTracks && clustersCompressed->maxTimeBin != -1e6 && clustersCompressed->maxTimeBin != param.par.continuousMaxTimeBin) {
    throw std::runtime_error("Configured max time bin does not match value used for track model encoding");
  }
  std::vector<ClusterNative> clusters[NSLICES][GPUCA_ROW_COUNT];
  std::atomic_flag locks[NSLICES][GPUCA_ROW_COUNT];
  for (unsigned int i = 0; i < NSLICES * GPUCA_ROW_COUNT; i++) {
    (&locks[0][0])[i].clear();
  }
  unsigned int offset = 0, lasti = 0;
  const unsigned int maxTime = param.par.continuousMaxTimeBin > 0 ? ((param.par.continuousMaxTimeBin + 1) * ClusterNative::scaleTimePacked - 1) : TPC_MAX_TIME_BIN_TRIGGERED;
  GPUCA_OPENMP(parallel for firstprivate(offset, lasti))
  for (unsigned int i = 0; i < clustersCompressed->nTracks; i++) {
    if (i < lasti) {
      offset = lasti = 0; // dynamic OMP scheduling, need to reinitialize offset
    }
    while (lasti < i) {
      offset += clustersCompressed->nTrackClusters[lasti++];
    }
    lasti++;
    decompressTrack(clustersCompressed, param, maxTime, i, offset, clusters, locks);
  }
  size_t nTotalClusters = clustersCompressed->nAttachedClusters + clustersCompressed->nUnattachedClusters;
  ClusterNative* clusterBuffer = allocator(nTotalClusters);
  unsigned int offsets[NSLICES][GPUCA_ROW_COUNT];
  offset = 0;
  unsigned int decodedAttachedClusters = 0;
  for (unsigned int i = 0; i < NSLICES; i++) {
    for (unsigned int j = 0; j < GPUCA_ROW_COUNT; j++) {
      clustersNative.nClusters[i][j] = clusters[i][j].size() + ((i * GPUCA_ROW_COUNT + j >= clustersCompressed->nSliceRows) ? 0 : clustersCompressed->nSliceRowClusters[i * GPUCA_ROW_COUNT + j]);
      offsets[i][j] = offset;
      offset += (i * GPUCA_ROW_COUNT + j >= clustersCompressed->nSliceRows) ? 0 : clustersCompressed->nSliceRowClusters[i * GPUCA_ROW_COUNT + j];
      decodedAttachedClusters += clusters[i][j].size();
    }
  }
  if (decodedAttachedClusters != clustersCompressed->nAttachedClusters) {
    GPUWarning("%u / %u clusters failed track model decoding (%f %%)", clustersCompressed->nAttachedClusters - decodedAttachedClusters, clustersCompressed->nAttachedClusters, 100.f * (float)(clustersCompressed->nAttachedClusters - decodedAttachedClusters) / (float)clustersCompressed->nAttachedClusters);
  }
  clustersNative.clustersLinear = clusterBuffer;
  clustersNative.setOffsetPtrs();
  GPUCA_OPENMP(parallel for)
  for (unsigned int i = 0; i < NSLICES; i++) {
    for (unsigned int j = 0; j < GPUCA_ROW_COUNT; j++) {
      ClusterNative* buffer = &clusterBuffer[clustersNative.clusterOffset[i][j]];
      if (clusters[i][j].size()) {
        memcpy((void*)buffer, (const void*)clusters[i][j].data(), clusters[i][j].size() * sizeof(clusterBuffer[0]));
      }
      ClusterNative* clout = buffer + clusters[i][j].size();
      unsigned int end = offsets[i][j] + ((i * GPUCA_ROW_COUNT + j >= clustersCompressed->nSliceRows) ? 0 : clustersCompressed->nSliceRowClusters[i * GPUCA_ROW_COUNT + j]);
      decompressHits(clustersCompressed, offsets[i][j], end, clout);
      if (param.rec.tpc.clustersShiftTimebins != 0.f) {
        for (unsigned int k = 0; k < clustersNative.nClusters[i][j]; k++) {
          auto& cl = buffer[k];
          float t = cl.getTime() + param.rec.tpc.clustersShiftTimebins;
          if (t < 0) {
            t = 0;
          }
          if (param.par.continuousMaxTimeBin > 0 && t > param.par.continuousMaxTimeBin) {
            t = param.par.continuousMaxTimeBin;
          }
          cl.setTime(t);
        }
      }
      std::sort(buffer, buffer + clustersNative.nClusters[i][j]);
    }
  }

  return 0;
}
