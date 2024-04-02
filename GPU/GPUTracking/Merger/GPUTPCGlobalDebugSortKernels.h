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

/// \file GPUTPCGlobalDebugSortKernels.h
/// \author David Rohr

#ifndef GPUTPCGLOBALDEBUGSORTKERNELS_H
#define GPUTPCGLOBALDEBUGSORTKERNELS_H

#include "GPUTPCDef.h"
#include "GPUGeneralKernels.h"
#include "GPUConstantMem.h"

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCGMMerger;
class GPUTPCGlobalDebugSortKernels : public GPUKernelTemplate
{
 public:
  enum K { defaultKernel = 0,
           clearIds = 0,
           sectorTracks = 1,
           globalTracks1 = 2,
           globalTracks2 = 3,
           borderTracks = 4 };
  GPUhdi() CONSTEXPR static GPUDataTypes::RecoStep GetRecoStep() { return GPUDataTypes::RecoStep::TPCMerging; }
  typedef GPUTPCGMMerger processorType;
  GPUhdi() static processorType* Processor(GPUConstantMem& processors) { return &processors.tpcMerger; }

  template <int iKernel = defaultKernel>
  GPUd() static void Thread(int nBlocks, int nThreads, int iBlock, int iThread, GPUsharedref() GPUSharedMemory& smem, processorType& tracker, char parameter);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif // GPUTPCGLOBALDEBUGSORTKERNELS_H
