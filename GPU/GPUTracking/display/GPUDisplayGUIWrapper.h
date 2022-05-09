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

/// \file GPUDisplayGUIWrapper.h
/// \author David Rohr

#ifndef GPUDISPLAYGUIWRAPPER_H
#define GPUDISPLAYGUIWRAPPER_H

#include "GPUCommonDef.h"
#include <memory>

namespace GPUCA_NAMESPACE::gpu
{
struct GPUDisplayGUIWrapperObjects;

class GPUDisplayGUIWrapper
{
 public:
  GPUDisplayGUIWrapper();
  ~GPUDisplayGUIWrapper();
  bool isRunning() const;
  void UpdateTimer();

  int start();
  int stop();
  int focus();

 private:
  std::unique_ptr<GPUDisplayGUIWrapperObjects> mO;

  void guiThread();
};
} // namespace GPUCA_NAMESPACE::gpu
#endif // GPUDISPLAYGUIWRAPPER_H
