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

/// \file GPUO2InterfaceConfigurableParam.cxx
/// \author David Rohr

#include "GPUO2InterfaceConfigurableParam.h"
#include "GPUO2InterfaceConfiguration.h"
#include "GPUDataTypes.h"

using namespace o2::gpu;
#define BeginNamespace(name)
#define EndNamespace()
#define AddOption(name, type, default, optname, optnameshort, help, ...)
#define AddOptionRTC(...) AddOption(__VA_ARGS__)
#define AddVariable(name, type, default)
#define AddVariableRTC(...) AddVariable(__VA_ARGS__)
#define AddOptionSet(name, type, value, optname, optnameshort, help, ...)
#define AddOptionVec(name, type, optname, optnameshort, help, ...)
#define AddOptionArray(name, type, count, default, optname, optnameshort, help, ...)
#define AddSubConfig(name, instance)
#define BeginSubConfig(name, instance, parent, preoptname, preoptnameshort, descr, o2prefix) O2ParamImpl(GPUCA_M_CAT(GPUConfigurableParam, name))
#define BeginHiddenConfig(...)
#define EndConfig()
#define AddCustomCPP(...)
#define AddHelp(...)
#define AddShortcut(...)
#include "GPUSettingsList.h"
#undef BeginNamespace
#undef EndNamespace
#undef AddOption
#undef AddOptionRTC
#undef AddVariable
#undef AddVariableRTC
#undef AddOptionSet
#undef AddOptionVec
#undef AddOptionArray
#undef AddSubConfig
#undef BeginSubConfig
#undef BeginHiddenConfig
#undef EndConfig
#undef AddCustomCPP
#undef AddHelp
#undef AddShortcut

GPUSettingsO2 GPUO2InterfaceConfiguration::ReadConfigurableParam_internal()
{
#define BeginNamespace(name)
#define EndNamespace()
#define AddOption(name, type, default, optname, optnameshort, help, ...) dst.name = src.name;
#define AddOptionRTC(...) AddOption(__VA_ARGS__)
#define AddVariable(name, type, default)
#define AddVariableRTC(...) AddVariable(__VA_ARGS__)
#define AddOptionSet(name, type, value, optname, optnameshort, help, ...)
#define AddOptionVec(name, type, optname, optnameshort, help, ...)
#define AddOptionArray(name, type, count, default, optname, optnameshort, help, ...) \
  for (int i = 0; i < count; i++) {                                                  \
    dst.name[i] = src.name[i];                                                       \
  }
#define AddSubConfig(name, instance) dst.instance = instance;
#define BeginSubConfig(name, instance, parent, preoptname, preoptnameshort, descr, o2prefix) \
  name instance;                                                                             \
  {                                                                                          \
    auto& src = GPUCA_M_CAT(GPUConfigurableParam, name)::Instance();                         \
    name& dst = instance;
#define BeginHiddenConfig(name, instance) {
#define EndConfig() }
#define AddCustomCPP(...)
#define AddHelp(...)
#define AddShortcut(...)
#include "GPUSettingsList.h"
#undef BeginNamespace
#undef EndNamespace
#undef AddOption
#undef AddOptionRTC
#undef AddVariable
#undef AddVariableRTC
#undef AddOptionSet
#undef AddOptionVec
#undef AddOptionArray
#undef AddSubConfig
#undef BeginSubConfig
#undef BeginHiddenConfig
#undef EndConfig
#undef AddCustomCPP
#undef AddHelp
#undef AddShortcut

  configProcessing = proc;
  configReconstruction = rec;
  configDisplay = display;
  configQA = QA;
  if (global.continuousMaxTimeBin) {
    configGRP.continuousMaxTimeBin = global.continuousMaxTimeBin;
  }
  if (global.solenoidBz > -1e6f) {
    configGRP.solenoidBz = global.solenoidBz;
  }
  if (global.constBz) {
    configGRP.constBz = global.constBz;
  }
  if (global.gpuDisplayfilterMacro != "") {
    configDisplay.filterMacros.emplace_back(global.gpuDisplayfilterMacro);
  }
  if (configReconstruction.tpc.trackReferenceX == 1000.f) {
    configReconstruction.tpc.trackReferenceX = 83.f;
  }
  configDeviceBackend.deviceType = GPUDataTypes::GetDeviceType(global.deviceType.c_str());
  configDeviceBackend.forceDeviceType = global.forceDeviceType;
  return global;
}

#include "utils/qconfig_helpers.h"

namespace
{
GPUSettingsStandalone configStandalone;
std::vector<std::function<void()>> qprint_global;
#define QCONFIG_PRINT
#include "utils/qconfig.h"
#undef QCONFIG_PRINT
} // namepsace

void GPUO2InterfaceConfiguration::PrintParam_internal()
{
  qConfigPrint(configProcessing, "proc.");
  qConfigPrint(configReconstruction, "rec.");
}
