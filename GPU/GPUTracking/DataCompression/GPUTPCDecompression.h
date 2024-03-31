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

/// \file GPUTPCDecompression.h
/// \author Gabriele Cimador

#ifndef GPUTPCDECOMPRESSION_H
#define GPUTPCDECOMPRESSION_H

#include "GPUDef.h"
#include "GPUProcessor.h"
#include "GPUCommonMath.h"
#include "GPUParam.h"
#include "GPUO2DataTypes.h"

#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/CompressedClusters.h"
#else
namespace o2::tpc
{
struct CompressedClustersPtrs {
};
struct CompressedClusters {
};
struct CompressedClustersFlat {
};
} // namespace o2::tpc
#endif

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCDecompression : public GPUProcessor
{
  friend class GPUTPCDecompressionKernels;
  friend class GPUTPCDecompressionUtilKernels;
  friend class GPUChainTracking;

 public:
#ifndef GPUCA_GPUCODE
  void InitializeProcessor();
  void RegisterMemoryAllocation();
  void SetMaxData(const GPUTrackingInOutPointers& io);

  void* SetPointersInputGPU(void* mem);
  void* SetPointersTmpNativeBuffersGPU(void* mem);
  void* SetPointersTmpNativeBuffersOutput(void* mem);
  void* SetPointersTmpNativeBuffersInput(void* mem);

#endif

 protected:
  constexpr static unsigned int NSLICES = GPUCA_NSLICES;
  o2::tpc::CompressedClusters mInputGPU;

  unsigned int mMaxNativeClustersPerBuffer;
  unsigned int* mNativeClustersIndex;
  unsigned int* mUnattachedClustersOffsets;
  o2::tpc::ClusterNative* mTmpNativeClusters;
  o2::tpc::ClusterNative* mNativeClustersBuffer;

  template <class T>
  void SetPointersCompressedClusters(void*& mem, T& c, unsigned int nClA, unsigned int nTr, unsigned int nClU, bool reducedClA);

  short mMemoryResInputGPU = -1;
  short mResourceTmpIndexes = -1;
  short mResourceTmpClustersOffsets = -1;
};
} // namespace GPUCA_NAMESPACE::gpu
#endif // GPUTPCDECOMPRESSION_H
