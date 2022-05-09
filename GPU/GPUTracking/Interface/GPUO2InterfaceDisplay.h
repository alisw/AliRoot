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

/// \file GPUO2InterfaceDisplay.h
/// \author David Rohr

#ifndef GPUO2INTERFACEDisplay_H
#define GPUO2INTERFACEDisplay_H

// Some defines denoting that we are compiling for O2
#ifndef GPUCA_HAVE_O2HEADERS
#define GPUCA_HAVE_O2HEADERS
#endif
#ifndef GPUCA_TPC_GEOMETRY_O2
#define GPUCA_TPC_GEOMETRY_O2
#endif
#ifndef GPUCA_O2_INTERFACE
#define GPUCA_O2_INTERFACE
#endif

#include <memory>
#include <vector>

namespace o2::gpu
{
class GPUDisplayInterface;
class GPUQA;
struct GPUParam;
struct GPUTrackingInOutPointers;
struct GPUO2InterfaceConfiguration;
class GPUDisplayFrontendInterface;
class GPUO2InterfaceDisplay
{
 public:
  GPUO2InterfaceDisplay(const GPUO2InterfaceConfiguration* config = nullptr);
  ~GPUO2InterfaceDisplay();

  int startDisplay();
  int show(const GPUTrackingInOutPointers* ptrs);
  int endDisplay();

 private:
  std::unique_ptr<GPUDisplayInterface> mDisplay;
  std::unique_ptr<GPUQA> mQA;
  std::unique_ptr<GPUParam> mParam;
  std::unique_ptr<GPUDisplayFrontendInterface> mFrontend;
  std::unique_ptr<GPUO2InterfaceConfiguration> mConfig;
};
} // namespace o2::gpu

#endif
