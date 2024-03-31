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

/// \file GPUTPCDecompressionKernels.h
/// \author Gabriele Cimador

#ifndef GPUTPCDECOMPRESSIONKERNELS_H
#define GPUTPCDECOMPRESSIONKERNELS_H

#include "GPUGeneralKernels.h"
#include "GPUO2DataTypes.h"
#include "GPUParam.h"
#include "GPUConstantMem.h"

#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/CompressedClusters.h"
#else
namespace o2::tpc
{
struct CompressedClusters {
};
} // namespace o2::tpc
#endif

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCDecompressionKernels : public GPUKernelTemplate
{
 public:
  GPUhdi() constexpr static GPUDataTypes::RecoStep GetRecoStep() { return GPUDataTypes::RecoStep::TPCDecompression; }

  enum K : int {
    step0attached = 0,
    step1unattached = 1,
  };

  template <int iKernel = defaultKernel>
  GPUd() static void Thread(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors);
  GPUd() static void decompressTrack(o2::tpc::CompressedClusters& cmprClusters, const GPUParam& param, const unsigned int maxTime, const unsigned int trackIndex, unsigned int& clusterOffset, GPUTPCDecompression& decompressor);
  GPUdi() static o2::tpc::ClusterNative decompressTrackStore(const o2::tpc::CompressedClusters& cmprClusters, const unsigned int clusterOffset, unsigned int slice, unsigned int row, unsigned int pad, unsigned int time, GPUTPCDecompression& decompressor);
  GPUdi() static void decompressHits(const o2::tpc::CompressedClusters& cmprClusters, const unsigned int start, const unsigned int end, o2::tpc::ClusterNative* clusterNativeBuffer);

  GPUd() static unsigned int computeLinearTmpBufferIndex(unsigned int slice, unsigned int row, unsigned int maxClustersPerBuffer)
  {
    return slice * (GPUCA_ROW_COUNT * maxClustersPerBuffer) + row * maxClustersPerBuffer;
  }

  template <typename T>
  GPUdi() static void decompressorMemcpyBasic(T* dst, const T* src, unsigned int size);
};

class GPUTPCDecompressionUtilKernels : public GPUKernelTemplate
{
 public:
  enum K : int {
    sortPerSectorRow = 0
  };

  template <int iKernel = defaultKernel>
  GPUd() static void Thread(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& GPUrestrict() processors);
};

} // namespace GPUCA_NAMESPACE::gpu
#endif // GPUTPCDECOMPRESSIONKERNELS_H
