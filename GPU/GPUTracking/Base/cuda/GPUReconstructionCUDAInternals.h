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

/// \file GPUReconstructionCUDAInternals.h
/// \author David Rohr

// All CUDA-header related stuff goes here, so we can run CING over GPUReconstructionCUDA

#ifndef GPURECONSTRUCTIONCUDAINTERNALS_H
#define GPURECONSTRUCTIONCUDAINTERNALS_H

#include <cuda.h>
#include "GPULogging.h"
#include <vector>
#include <memory>
#include <string>

namespace GPUCA_NAMESPACE
{
namespace gpu
{
#define GPUFailedMsg(x) GPUFailedMsgA(x, __FILE__, __LINE__)
#define GPUFailedMsgI(x) GPUFailedMsgAI(x, __FILE__, __LINE__)

struct GPUReconstructionCUDAInternals {
  std::vector<std::unique_ptr<CUmodule>> kernelModules;     // module for RTC compilation
  std::vector<std::unique_ptr<CUfunction>> kernelFunctions; // vector of ptrs to RTC kernels
  std::vector<std::string> kernelNames;                     // names of kernels
  cudaStream_t Streams[GPUCA_MAX_STREAMS];                  // Pointer to array of CUDA Streams

  static void getArgPtrs(const void** pArgs) {}
  template <typename T, typename... Args>
  static void getArgPtrs(const void** pArgs, const T& arg, const Args&... args)
  {
    *pArgs = &arg;
    getArgPtrs(pArgs + 1, args...);
  }
};

class GPUDebugTiming
{
 public:
  GPUDebugTiming(bool d, gpu_reconstruction_kernels::deviceEvent* t, cudaStream_t* s, const gpu_reconstruction_kernels::krnlSetupTime& x, GPUReconstructionCUDABackend* r) : mDeviceTimers(t), mStreams(s), mXYZ(x), mRec(r), mDo(d)
  {
    if (mDo) {
      if (mDeviceTimers) {
        mRec->GPUFailedMsg(cudaEventRecord(mDeviceTimers[0].get<cudaEvent_t>(), mStreams[mXYZ.x.stream]));
      } else {
        mTimer.ResetStart();
      }
    }
  }
  ~GPUDebugTiming()
  {
    if (mDo && mXYZ.t == 0.) {
      if (mDeviceTimers) {
        mRec->GPUFailedMsg(cudaEventRecord(mDeviceTimers[1].get<cudaEvent_t>(), mStreams[mXYZ.x.stream]));
        mRec->GPUFailedMsg(cudaEventSynchronize(mDeviceTimers[1].get<cudaEvent_t>()));
        float v;
        mRec->GPUFailedMsg(cudaEventElapsedTime(&v, mDeviceTimers[0].get<cudaEvent_t>(), mDeviceTimers[1].get<cudaEvent_t>()));
        mXYZ.t = v * 1.e-3f;
      } else {
        mRec->GPUFailedMsg(cudaStreamSynchronize(mStreams[mXYZ.x.stream]));
        mXYZ.t = mTimer.GetCurrentElapsedTime();
      }
    }
  }

 private:
  gpu_reconstruction_kernels::deviceEvent* mDeviceTimers;
  cudaStream_t* mStreams;
  const gpu_reconstruction_kernels::krnlSetupTime& mXYZ;
  GPUReconstructionCUDABackend* mRec;
  HighResTimer mTimer;
  bool mDo;
};

static_assert(std::is_convertible<cudaEvent_t, void*>::value, "CUDA event type incompatible to deviceEvent");

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
