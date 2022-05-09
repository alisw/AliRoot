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

/// \file CUDAThrustHelpers.h
/// \author David Rohr

#ifndef GPU_CUDATHRUSTHELPERS_H
#define GPU_CUDATHRUSTHELPERS_H

#include "GPULogging.h"
#include <vector>
#include <memory>

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class ThrustVolatileAsyncAllocator
{
 public:
  typedef char value_type;

  ThrustVolatileAsyncAllocator(GPUReconstruction* r) : mRec(r) {}
  char* allocate(std::ptrdiff_t n) { return (char*)mRec->AllocateVolatileDeviceMemory(n); }

  void deallocate(char* ptr, size_t) {}

 private:
  GPUReconstruction* mRec;
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

// Override synchronize call at end of thrust algorithm running on stream, just don't run cudaStreamSynchronize
namespace thrust
{
namespace cuda_cub
{

typedef thrust::cuda_cub::execution_policy<typeof(thrust::cuda::par(*(GPUCA_NAMESPACE::gpu::ThrustVolatileAsyncAllocator*)nullptr).on(*(cudaStream_t*)nullptr))> thrustStreamPolicy;
template <>
__host__ __device__ inline cudaError_t synchronize<thrustStreamPolicy>(thrustStreamPolicy& policy)
{
#ifndef GPUCA_GPUCODE_DEVICE
  // Do not synchronize!
  return cudaSuccess;
#else
  return synchronize_stream(derived_cast(policy));
#endif
}

} // namespace cuda_cub
} // namespace thrust

#endif
