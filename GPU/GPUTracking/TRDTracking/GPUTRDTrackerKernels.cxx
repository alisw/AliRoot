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

/// \file GPUTRDTrackerKernels.cxx
/// \author David Rohr

#include "GPUTRDTrackerKernels.h"
#include "GPUTRDGeometry.h"
#include "GPUConstantMem.h"
#include "GPUCommonTypeTraits.h"
#if defined(WITH_OPENMP) && !defined(GPUCA_GPUCODE)
#include "GPUReconstruction.h"
#endif

using namespace GPUCA_NAMESPACE::gpu;

template <int I, class T>
GPUdii() void GPUTRDTrackerKernels::Thread(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, T* externalInstance)
{
  auto* trdTracker = &processors.getTRDTracker<I>();
#ifndef GPUCA_GPUCODE_DEVICE
#if defined(__cplusplus) && __cplusplus >= 201703L
  if constexpr (std::is_same_v<decltype(trdTracker), decltype(externalInstance)>)
#endif
  {
    if (externalInstance) {
      trdTracker = externalInstance;
    }
  }
#endif
  GPUCA_OPENMP(parallel for if(!trdTracker->GetRec().GetProcessingSettings().ompKernels) num_threads(trdTracker->GetRec().GetProcessingSettings().ompThreads))
  for (int i = get_global_id(0); i < trdTracker->NTracks(); i += get_global_size(0)) {
    trdTracker->DoTrackingThread(i, get_global_id(0));
  }
}

template GPUd() void GPUTRDTrackerKernels::Thread<0>(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, GPUTRDTrackerGPU* externalInstance);
#ifdef GPUCA_HAVE_O2HEADERS
template GPUd() void GPUTRDTrackerKernels::Thread<1>(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& processors, GPUTRDTracker* externalInstance);
#endif // GPUCA_HAVE_O2HEADERS
