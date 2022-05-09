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

/// \file GPUDisplayLoader.cxx
/// \author David Rohr

#include "GPUDisplay.h"
#include "GPUDisplayFrontend.h"
#include "GPUDisplayInterface.h"

#include <tuple>
#include <stdexcept>

using namespace GPUCA_NAMESPACE::gpu;

extern "C" void* GPUTrackingDisplayLoader(const char*, void*);

template <class T, typename... Args>
static inline T* createHelper(Args... args)
{
  return new T(args...);
}

void* GPUTrackingDisplayLoader(const char* type, void* args)
{
  if (strcmp(type, "display") == 0) {
    auto x = (std::tuple<GPUDisplayFrontend*, GPUChainTracking*, GPUQA*, const GPUParam*, const GPUCalibObjectsConst*, const GPUSettingsDisplay*>*)args;
    return std::apply([](auto&&... y) { return createHelper<GPUDisplay>(y...); }, *x);
  } else if (strcmp(type, "frontend") == 0) {
    auto x = (std::tuple<const char*>*)args;
    return std::apply([](auto&&... y) { return GPUDisplayFrontend::getFrontend(y...); }, *x);
  } else {
    throw std::runtime_error("Invalid display obejct type specified");
  }
  return nullptr;
}
