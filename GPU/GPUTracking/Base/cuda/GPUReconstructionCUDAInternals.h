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

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUReconstructionCUDAInternals {
  CUcontext CudaContext;                                 // CUDA context
  std::vector<std::unique_ptr<CUmodule>> rtcModules;     // module for RTC compilation
  std::vector<std::unique_ptr<CUfunction>> rtcFunctions; // vector of ptrs to RTC kernels
  unsigned int cudaContextObtained = 0;                  // If multiple instances of GPUThreadContextCUDA are obtained, we count them and return the context only after all are destroyed
  cudaStream_t Streams[GPUCA_MAX_STREAMS];               // Pointer to array of CUDA Streams

  template <bool multi, class T, int I = 0>
  static int getRTCkernelNum(int k = -1);
};

#define GPUFailedMsg(x) GPUFailedMsgA(x, __FILE__, __LINE__)
#define GPUFailedMsgI(x) GPUFailedMsgAI(x, __FILE__, __LINE__)

static int GPUFailedMsgAI(const long long int error, const char* file, int line)
{
  // Check for CUDA Error and in the case of an error display the corresponding error string
  if (error == cudaSuccess) {
    return (0);
  }
  GPUError("CUDA Error: %lld / %s (%s:%d)", error, cudaGetErrorString((cudaError_t)error), file, line);
  return 1;
}

static void GPUFailedMsgA(const long long int error, const char* file, int line)
{
  if (GPUFailedMsgAI(error, file, line)) {
    throw std::runtime_error("CUDA Failure");
  }
}

static_assert(std::is_convertible<cudaEvent_t, void*>::value, "CUDA event type incompatible to deviceEvent");

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
