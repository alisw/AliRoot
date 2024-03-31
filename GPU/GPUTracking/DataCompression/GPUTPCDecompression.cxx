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

/// \file GPUTPCDecompression.cxx
/// \author Gabriele Cimador

#include "GPUTPCDecompression.h"
#include "GPUTPCCompression.h"
#include "GPUReconstruction.h"
#include "GPUO2DataTypes.h"
#include "GPUMemorySizeScalers.h"
#include "GPULogging.h"

using namespace GPUCA_NAMESPACE::gpu;

void GPUTPCDecompression::InitializeProcessor() {}

void* GPUTPCDecompression::SetPointersInputGPU(void* mem)
{
  SetPointersCompressedClusters(mem, mInputGPU, mInputGPU.nAttachedClusters, mInputGPU.nTracks, mInputGPU.nUnattachedClusters, true);
  return mem;
}

template <class T>
void GPUTPCDecompression::SetPointersCompressedClusters(void*& mem, T& c, unsigned int nClA, unsigned int nTr, unsigned int nClU, bool reducedClA)
{
  computePointerWithAlignment(mem, c.qTotU, nClU); // Do not reorder, qTotU ist used as first address in GPUChainTracking::RunTPCCompression
  computePointerWithAlignment(mem, c.qMaxU, nClU);
  computePointerWithAlignment(mem, c.flagsU, nClU);
  computePointerWithAlignment(mem, c.padDiffU, nClU);
  computePointerWithAlignment(mem, c.timeDiffU, nClU);
  computePointerWithAlignment(mem, c.sigmaPadU, nClU);
  computePointerWithAlignment(mem, c.sigmaTimeU, nClU);
  computePointerWithAlignment(mem, c.nSliceRowClusters, GPUCA_ROW_COUNT * NSLICES);

  unsigned int nClAreduced = reducedClA ? nClA - nTr : nClA;

  if (!(mRec->GetParam().rec.tpc.compressionTypeMask & GPUSettings::CompressionTrackModel)) {
    return; // Track model disabled, do not allocate memory
  }
  computePointerWithAlignment(mem, c.qTotA, nClA);
  computePointerWithAlignment(mem, c.qMaxA, nClA);
  computePointerWithAlignment(mem, c.flagsA, nClA);
  computePointerWithAlignment(mem, c.rowDiffA, nClAreduced);
  computePointerWithAlignment(mem, c.sliceLegDiffA, nClAreduced);
  computePointerWithAlignment(mem, c.padResA, nClAreduced);
  computePointerWithAlignment(mem, c.timeResA, nClAreduced);
  computePointerWithAlignment(mem, c.sigmaPadA, nClA);
  computePointerWithAlignment(mem, c.sigmaTimeA, nClA);

  computePointerWithAlignment(mem, c.qPtA, nTr);
  computePointerWithAlignment(mem, c.rowA, nTr);
  computePointerWithAlignment(mem, c.sliceA, nTr);
  computePointerWithAlignment(mem, c.timeA, nTr);
  computePointerWithAlignment(mem, c.padA, nTr);

  computePointerWithAlignment(mem, c.nTrackClusters, nTr);
}

void* GPUTPCDecompression::SetPointersTmpNativeBuffersGPU(void* mem)
{
  computePointerWithAlignment(mem, mTmpNativeClusters, NSLICES * GPUCA_ROW_COUNT * mMaxNativeClustersPerBuffer);
  return mem;
}

void* GPUTPCDecompression::SetPointersTmpNativeBuffersOutput(void* mem)
{
  computePointerWithAlignment(mem, mNativeClustersIndex, NSLICES * GPUCA_ROW_COUNT);
  return mem;
}

void* GPUTPCDecompression::SetPointersTmpNativeBuffersInput(void* mem)
{
  computePointerWithAlignment(mem, mUnattachedClustersOffsets, NSLICES * GPUCA_ROW_COUNT);
  return mem;
}

void GPUTPCDecompression::RegisterMemoryAllocation()
{
  AllocateAndInitializeLate();
  mMemoryResInputGPU = mRec->RegisterMemoryAllocation(this, &GPUTPCDecompression::SetPointersInputGPU, GPUMemoryResource::MEMORY_INPUT_FLAG | GPUMemoryResource::MEMORY_GPU | GPUMemoryResource::MEMORY_EXTERNAL | GPUMemoryResource::MEMORY_SCRATCH, "TPCDecompressionInput");
  mRec->RegisterMemoryAllocation(this, &GPUTPCDecompression::SetPointersTmpNativeBuffersGPU, GPUMemoryResource::MEMORY_SCRATCH, "TPCDecompressionTmpBuffersGPU");
  mResourceTmpIndexes = mRec->RegisterMemoryAllocation(this, &GPUTPCDecompression::SetPointersTmpNativeBuffersOutput, GPUMemoryResource::MEMORY_OUTPUT | GPUMemoryResource::MEMORY_SCRATCH, "TPCDecompressionTmpBuffersOutput");
  mResourceTmpClustersOffsets = mRec->RegisterMemoryAllocation(this, &GPUTPCDecompression::SetPointersTmpNativeBuffersInput, GPUMemoryResource::MEMORY_INPUT | GPUMemoryResource::MEMORY_SCRATCH, "TPCDecompressionTmpBuffersInput");
}

void GPUTPCDecompression::SetMaxData(const GPUTrackingInOutPointers& io)
{
  mMaxNativeClustersPerBuffer = mRec->GetProcessingSettings().tpcMaxAttachedClustersPerSectorRow;
}
