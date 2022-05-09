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

/// \file GPUChainTrackingRefit.cxx
/// \author David Rohr

#include "GPUChainTracking.h"
#include "GPULogging.h"
#include "GPUO2DataTypes.h"

using namespace GPUCA_NAMESPACE::gpu;

int GPUChainTracking::RunRefit()
{
#ifdef GPUCA_HAVE_O2HEADERS
  bool doGPU = GetRecoStepsGPU() & RecoStep::Refit;
  GPUTrackingRefitProcessor& Refit = processors()->trackingRefit;
  GPUTrackingRefitProcessor& RefitShadow = doGPU ? processorsShadow()->trackingRefit : Refit;

  const auto& threadContext = GetThreadContext();
  (void)threadContext;
  SetupGPUProcessor(&Refit, false);
  RefitShadow.SetPtrsFromGPUConstantMem(processorsShadow(), doGPU ? &processorsDevice()->param : nullptr);
  RefitShadow.SetPropagator(doGPU ? processorsShadow()->calibObjects.o2Propagator : GetO2Propagator());
  RefitShadow.mPTracks = (doGPU ? processorsShadow() : processors())->tpcMerger.OutputTracks();
  WriteToConstantMemory(RecoStep::Refit, (char*)&processors()->trackingRefit - (char*)processors(), &RefitShadow, sizeof(RefitShadow), 0);
  //TransferMemoryResourcesToGPU(RecoStep::Refit, &Refit, 0);
  if (param().rec.trackingRefitGPUModel) {
    runKernel<GPUTrackingRefitKernel, GPUTrackingRefitKernel::mode0asGPU>(GetGrid(mIOPtrs.nMergedTracks, 0), krnlRunRangeNone);
  } else {
    runKernel<GPUTrackingRefitKernel, GPUTrackingRefitKernel::mode1asTrackParCov>(GetGrid(mIOPtrs.nMergedTracks, 0), krnlRunRangeNone);
  }
  //TransferMemoryResourcesToHost(RecoStep::Refit, &Refit, 0);
  SynchronizeStream(0);
#endif
  return 0;
}
