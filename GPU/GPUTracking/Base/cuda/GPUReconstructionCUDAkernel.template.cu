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

/// \file GPUReconstructionCUDAkernel.cu
/// \author David Rohr

#define GPUCA_GPUCODE_COMPILEKERNELS
#include "GPUReconstructionCUDAIncludes.h"
#include "GPUReconstructionCUDADef.h"
#define GPUCA_KRNL_REG(args) __launch_bounds__(GPUCA_M_MAX2_3(GPUCA_M_STRIP(args)))
#define GPUCA_KRNL(...) GPUCA_KRNL_WRAP(GPUCA_KRNL_LOAD_, __VA_ARGS__)
#define GPUCA_KRNL_LOAD_single(...) GPUCA_KRNLGPU_SINGLE(__VA_ARGS__);
#define GPUCA_KRNL_LOAD_multi(...) GPUCA_KRNLGPU_MULTI(__VA_ARGS__);
#include "GPUReconstructionKernelMacros.h"

// clang-format off
@O2_GPU_KERNEL_TEMPLATE_FILES@
// clang-format on

extern "C" {
// clang-format off
@O2_GPU_KERNEL_TEMPLATE_REPLACE@
// clang-format on
}
