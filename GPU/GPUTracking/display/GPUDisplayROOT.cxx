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

/// \file GPUDisplayROOT.cxx
/// \author David Rohr

#include "GPUDisplay.h"
using namespace GPUCA_NAMESPACE::gpu;

#ifndef GPUCA_NO_ROOT
#include "Rtypes.h" // Include ROOT header first, to use ROOT and disable replacements
#include "TROOT.h"
#include "TSystem.h"
#include "TMethodCall.h"

int GPUDisplay::buildTrackFilter()
{
  if (!mCfgH.trackFilter) {
    return 0;
  }
  if (mUpdateTrackFilter) {
    std::string name = "displayTrackFilter/";
    name += mConfig.filterMacros[mCfgH.trackFilter - 1];
    gROOT->Reset();
    if (gROOT->LoadMacro(name.c_str())) {
      GPUError("Error loading trackFilter macro %s", name.c_str());
      return 1;
    }
  }
  TMethodCall call;
  call.InitWithPrototype("gpuDisplayTrackFilter", "std::vector<bool>*, const o2::gpu::GPUTrackingInOutPointers*, const o2::gpu::GPUConstantMem*");
  const void* args[3];
  std::vector<bool>* arg0 = &mTrackFilter;
  args[0] = &arg0;
  args[1] = &mIOPtrs;
  const GPUConstantMem* arg2 = mChain ? mChain->GetProcessors() : nullptr;
  args[2] = &arg2;

  call.Execute(nullptr, args, sizeof(args) / sizeof(args[0]), nullptr);
  return 0;
}

#else

int GPUDisplay::buildTrackFilter()
{
}

#endif
