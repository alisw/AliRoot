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

/// \file GPUCommonLogger.h
/// \author David Rohr

#ifndef GPUCOMMONFAIRLOGGER_H
#define GPUCOMMONFAIRLOGGER_H

#include "GPUCommonDef.h"

#if defined(GPUCA_GPUCODE_DEVICE)
namespace o2::gpu::detail
{
struct DummyLogger {
  template <typename... Args>
  GPUd() DummyLogger& operator<<(Args... args)
  {
    return *this;
  }
};
} // namespace o2::gpu::detail
#endif

#if defined(__OPENCL__) || (defined(GPUCA_GPUCODE_DEVICE) && !defined(GPUCA_GPU_DEBUG_PRINT))
#define LOG(...) o2::gpu::detail::DummyLogger()
#define LOGF(...)
#define LOGP(...)

#elif defined(GPUCA_GPUCODE_DEVICE)
#define LOG(...) o2::gpu::detail::DummyLogger()
// #define LOG(...) static_assert(false, "LOG(...) << ... unsupported in GPU code");
#define LOGF(type, string, ...)         \
  {                                     \
    printf(string "\n", ##__VA_ARGS__); \
  }
#define LOGP(...)
// #define LOGP(...) static_assert(false, "LOGP(...) unsupported in GPU code");

#elif defined(GPUCA_STANDALONE) || defined(GPUCA_ALIROOT_LIB) || (!defined(__cplusplus) || __cplusplus < 201703L)
#include <iostream>
#include <cstdio>
#define LOG(type) std::cout
#define LOGF(type, string, ...)         \
  {                                     \
    printf(string "\n", ##__VA_ARGS__); \
  }
#define LOGP(type, string, ...) \
  {                             \
    printf("%s\n", string);     \
  }

#else
#include <Framework/Logger.h>

#endif

#endif
