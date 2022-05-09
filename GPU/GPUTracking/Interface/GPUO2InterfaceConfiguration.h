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

/// \file GPUO2InterfaceConfiguration.h
/// \author David Rohr

#ifndef GPUO2INTERFACECONFIGURATION_H
#define GPUO2INTERFACECONFIGURATION_H

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
#include <array>
#include <vector>
#include <functional>
#include <gsl/gsl>
#include "GPUSettings.h"
#include "GPUDataTypes.h"
#include "GPUHostDataTypes.h"
#include "GPUOutputControl.h"
#include "DataFormatsTPC/Constants.h"

class TH1F;
class TH1D;
class TH2F;

namespace o2
{
namespace tpc
{
class TrackTPC;
class Digit;
} // namespace tpc
namespace gpu
{
class TPCFastTransform;
class GPUReconstruction;
struct GPUSettingsO2;

struct GPUInterfaceQAOutputs {
  const std::vector<TH1F>* hist1 = nullptr;
  const std::vector<TH2F>* hist2 = nullptr;
  const std::vector<TH1D>* hist3 = nullptr;
};

struct GPUInterfaceOutputs : public GPUTrackingOutputs {
  GPUInterfaceQAOutputs qa;
};

// Full configuration structure with all available settings of GPU...
struct GPUO2InterfaceConfiguration {
  GPUO2InterfaceConfiguration() = default;
  ~GPUO2InterfaceConfiguration() = default;
  GPUO2InterfaceConfiguration(const GPUO2InterfaceConfiguration&) = default;

  // Settings for the Interface class
  struct GPUInterfaceSettings {
    int dumpEvents = 0;
    bool outputToExternalBuffers = false;
    // These constants affect GPU memory allocation only and do not limit the CPU processing
    unsigned long maxTPCZS = 8192ul * 1024 * 1024;
    unsigned int maxTPCHits = 1024 * 1024 * 1024;
    unsigned int maxTRDTracklets = 128 * 1024;
    unsigned int maxITSTracks = 96 * 1024;
  };

  GPUSettingsDeviceBackend configDeviceBackend;
  GPUSettingsProcessing configProcessing;
  GPUSettingsGRP configGRP;
  GPUSettingsRec configReconstruction;
  GPUSettingsDisplay configDisplay;
  GPUSettingsQA configQA;
  GPUInterfaceSettings configInterface;
  GPURecoStepConfiguration configWorkflow;
  GPUCalibObjectsConst configCalib;

  GPUSettingsO2 ReadConfigurableParam();
  void PrintParam();

 private:
  friend class GPUReconstruction;
  GPUSettingsO2 ReadConfigurableParam_internal();
  void PrintParam_internal();
};

} // namespace gpu
} // namespace o2

#endif
