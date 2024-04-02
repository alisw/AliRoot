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

/// \file GPUChainITS.cxx
/// \author David Rohr

#include "GPUChainITS.h"
#include "DataFormatsITS/TrackITS.h"
#include "ITStracking/ExternalAllocator.h"
#include "GPUReconstructionIncludesITS.h"
#include <algorithm>

using namespace GPUCA_NAMESPACE::gpu;

namespace o2::its
{
class GPUFrameworkExternalAllocator final : public o2::its::ExternalAllocator
{
 public:
  void* allocate(size_t size) override
  {
    return mFWReco->AllocateUnmanagedMemory(size, GPUMemoryResource::MEMORY_GPU);
  }

  void setReconstructionFramework(o2::gpu::GPUReconstruction* fwr) { mFWReco = fwr; }

 private:
  o2::gpu::GPUReconstruction* mFWReco;
};
} // namespace o2::its

GPUChainITS::~GPUChainITS()
{
  mITSTrackerTraits.reset();
  mITSVertexerTraits.reset();
}

GPUChainITS::GPUChainITS(GPUReconstruction* rec, unsigned int maxTracks) : GPUChain(rec), mMaxTracks(maxTracks) {}

void GPUChainITS::RegisterPermanentMemoryAndProcessors() { mRec->RegisterGPUProcessor(&processors()->itsFitter, GetRecoStepsGPU() & RecoStep::ITSTracking); }

void GPUChainITS::RegisterGPUProcessors()
{
  if (GetRecoStepsGPU() & RecoStep::ITSTracking) {
    mRec->RegisterGPUDeviceProcessor(&processorsShadow()->itsFitter, &processors()->itsFitter);
  }
}

void GPUChainITS::MemorySize(size_t& gpuMem, size_t& pageLockedHostMem)
{
  gpuMem = mMaxTracks * sizeof(GPUITSTrack) + GPUCA_MEMALIGN;
  pageLockedHostMem = gpuMem;
}

int GPUChainITS::Init() { return 0; }

o2::its::TrackerTraits* GPUChainITS::GetITSTrackerTraits()
{
  if (mITSTrackerTraits == nullptr) {
    mRec->GetITSTraits(&mITSTrackerTraits, nullptr, nullptr);
  }
  return mITSTrackerTraits.get();
}

o2::its::VertexerTraits* GPUChainITS::GetITSVertexerTraits()
{
  if (mITSVertexerTraits == nullptr) {
    mRec->GetITSTraits(nullptr, &mITSVertexerTraits, nullptr);
  }
  return mITSVertexerTraits.get();
}

o2::its::TimeFrame* GPUChainITS::GetITSTimeframe()
{
  if (mITSTimeFrame == nullptr) {
    mRec->GetITSTraits(nullptr, nullptr, &mITSTimeFrame);
  }
#if !defined(GPUCA_STANDALONE)
  if (mITSTimeFrame->mIsGPU) {
    auto doFWExtAlloc = [this](size_t size) -> void* { return rec()->AllocateUnmanagedMemory(size, GPUMemoryResource::MEMORY_GPU); };

    mFrameworkAllocator.reset(new o2::its::GPUFrameworkExternalAllocator);
    mFrameworkAllocator->setReconstructionFramework(rec());
    mITSTimeFrame->setExternalAllocator(mFrameworkAllocator.get());
    LOGP(info, "GPUChainITS is giving me allocator: {}", (void*)mFrameworkAllocator.get());
  }
#endif
  return mITSTimeFrame.get();
}

int GPUChainITS::PrepareEvent() { return 0; }

int GPUChainITS::Finalize() { return 0; }

int GPUChainITS::RunChain() { return 0; }
