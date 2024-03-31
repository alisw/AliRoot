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

/// \file GPUTPCSectorDebugSortKernels.h
/// \author David Rohr

#ifndef GPUTPCSECTORDEBUGSORTKERNELS_H
#define GPUTPCSECTORDEBUGSORTKERNELS_H

#include "GPUTPCDef.h"
#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

namespace GPUCA_NAMESPACE::gpu
{
class GPUTPCTracker;

class GPUTPCSectorDebugSortKernels : public GPUKernelTemplate
{
 public:
  enum K { defaultKernel = 0,
           hitData = 0,
           startHits = 1,
           sliceTracks = 2 };
  GPUhdi() CONSTEXPR static GPUDataTypes::RecoStep GetRecoStep() { return GPUDataTypes::RecoStep::TPCSliceTracking; }
  typedef GPUTPCTracker processorType;
  GPUhdi() static processorType* Processor(GPUConstantMem& processors) { return processors.tpcTrackers; }

  template <int iKernel = defaultKernel>
  GPUd() static void Thread(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& tracker);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif // GPUTPCSECTORDEBUGSORTKERNELS_H
