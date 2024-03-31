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

/// \file GPUReconstructionCUDDef.h
/// \author David Rohr

#ifndef O2_GPU_GPURECONSTRUCTIONCUDADEF_H
#define O2_GPU_GPURECONSTRUCTIONCUDADEF_H

#ifndef __HIPCC__ // CUDA
#define GPUCA_UNROLL(optCu, optHi) GPUCA_M_UNROLL_##optCu
#define GPUdic(optCu, optHi) GPUCA_GPUdic_select_##optCu()
#else
#define GPUCA_UNROLL(optCu, optHi) GPUCA_M_UNROLL_##optHi
#define GPUdic(optCu, optHi) GPUCA_GPUdic_select_##optHi()
#endif

#include "GPUDef.h"

#ifndef GPUCA_NO_CONSTANT_MEMORY
#define GPUCA_CONSMEM_PTR
#define GPUCA_CONSMEM_CALL
#define GPUCA_CONSMEM (gGPUConstantMemBuffer.v)
#else
#define GPUCA_CONSMEM_PTR const GPUConstantMem *gGPUConstantMemBuffer,
#define GPUCA_CONSMEM_CALL me->mDeviceConstantMem,
#define GPUCA_CONSMEM ((GPUConstantMem&)(*gGPUConstantMemBuffer))
#endif
#define GPUCA_KRNL_BACKEND_CLASS GPUReconstructionCUDABackend

#endif
