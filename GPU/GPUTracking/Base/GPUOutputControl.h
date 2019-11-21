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

/// \file GPUOutputControl.h
/// \author David Rohr

#ifndef GPUOUTPUTCONTROL_H
#define GPUOUTPUTCONTROL_H

#include "GPUCommonDef.h"
#ifndef GPUCA_GPUCODE
#include <cstddef>
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUOutputControl {
  enum OutputTypeStruct { AllocateInternal = 0,
                          UseExternalBuffer = 1,
                          ControlledExternal = 2 };
#ifndef GPUCA_GPUCODE_DEVICE
  GPUOutputControl() = default;
#endif

  char* OutputPtr = nullptr;                      // Pointer to Output Space
  volatile size_t Offset = 0;                     // Offset to write into output pointer
  size_t OutputMaxSize = 0;                       // Max Size of Output Data if Pointer to output space is given
  OutputTypeStruct OutputType = AllocateInternal; // How to perform the output
  char EndOfSpace = 0;                            // end of space flag
};
} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
