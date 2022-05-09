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

/// \file GPUReconstructionCUDIncludes.h
/// \author David Rohr

#ifndef O2_GPU_GPURECONSTRUCTIONCUDAINCLUDES_H
#define O2_GPU_GPURECONSTRUCTIONCUDAINCLUDES_H

#include <cstdint>
#include <cuda_runtime.h>
#include <cuda.h>
#include <cooperative_groups.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <cub/cub.cuh>
#include <cub/block/block_scan.cuh>
#include <thrust/sort.h>
#include <thrust/execution_policy.h>
#include <thrust/device_ptr.h>
#pragma GCC diagnostic pop
#include <sm_20_atomic_functions.h>
#include <cuda_fp16.h>
#include <type_traits>
#include <string>

#endif
