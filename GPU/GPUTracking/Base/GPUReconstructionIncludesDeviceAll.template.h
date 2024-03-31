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

/// \file GPUReconstructionIncludesDevice.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONINCLUDESDEVICE_H
#define GPURECONSTRUCTIONINCLUDESDEVICE_H

#include "GPUDef.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{
}
} // namespace GPUCA_NAMESPACE
using namespace GPUCA_NAMESPACE::gpu;

#if !defined(GPUCA_OPENCL1) && (!defined(GPUCA_ALIROOT_LIB) || !defined(GPUCA_GPUCODE))
#define GPUCA_KRNL_NOOCL1
#endif

// clang-format off
$<JOIN:$<LIST:TRANSFORM,$<LIST:TRANSFORM,$<LIST:REMOVE_DUPLICATES,$<TARGET_PROPERTY:O2_GPU_KERNELS,O2_GPU_KERNEL_FILES>>,APPEND,">,PREPEND,#include ">,
>
// clang-format on

#endif // GPURECONSTRUCTIONINCLUDESDEVICE_H
