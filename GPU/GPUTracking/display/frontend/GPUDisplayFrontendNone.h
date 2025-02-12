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

/// \file GPUDisplayFrontendNone.h
/// \author David Rohr

#ifndef GPUDISPLAYFRONTENDNONE_H
#define GPUDISPLAYFRONTENDNONE_H

#include "GPUDisplayFrontend.h"

namespace GPUCA_NAMESPACE::gpu
{
class GPUDisplayFrontendNone : public GPUDisplayFrontend
{
  GPUDisplayFrontendNone() = default;
  ~GPUDisplayFrontendNone() override = default;

  int32_t StartDisplay() override { return 1; }
  void DisplayExit() override {}
  void SwitchFullscreen(bool set) override {}
  void ToggleMaximized(bool set) override {}
  void SetVSync(bool enable) override {}
  void OpenGLPrint(const char* s, float x, float y, float r, float g, float b, float a, bool fromBotton = true) override {}
};
} // namespace GPUCA_NAMESPACE::gpu

#endif
