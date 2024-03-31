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

/// \file GPUReconstructionCUDAExternalProvider.cu
/// \author David Rohr

#include "GPUReconstructionCUDADef.h"
#include "GPUReconstructionCUDAIncludes.h"

#include "GPUReconstructionCUDA.h"
#include "GPUReconstructionCUDAInternals.h"
#include "CUDAThrustHelpers.h"

#include <stdexcept>

using namespace GPUCA_NAMESPACE::gpu;

#include "GPUConstantMem.h"

// Files needed for O2 propagator
#include "MatLayerCylSet.cxx"
#include "MatLayerCyl.cxx"
#include "Ray.cxx"
#include "TrackParametrization.cxx"
#include "TrackParametrizationWithError.cxx"
#include "Propagator.cxx"
#include "TrackLTIntegral.cxx"

#ifndef GPUCA_NO_CONSTANT_MEMORY
static GPUReconstructionDeviceBase::deviceConstantMemRegistration registerConstSymbol([]() {
  void* retVal = nullptr;
  if (cudaGetSymbolAddress(&retVal, gGPUConstantMemBuffer) != cudaSuccess) {
    throw std::runtime_error("Could not obtain GPU constant memory symbol");
  }
  return retVal;
});
#endif
