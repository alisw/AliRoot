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

/// \file GPUO2InterfaceDisplay.cxx
/// \author David Rohr

#include "GPUParam.h"
#include "display/GPUDisplayInterface.h"
#include "GPUQA.h"
#include "GPUO2InterfaceConfiguration.h"
#include "GPUO2InterfaceDisplay.h"
#include <unistd.h>

using namespace o2::gpu;
using namespace o2::tpc;

GPUO2InterfaceDisplay::GPUO2InterfaceDisplay(const GPUO2InterfaceConfiguration* config)
{
  mConfig.reset(new GPUO2InterfaceConfiguration(*config));
  mFrontend.reset(GPUDisplayFrontendInterface::getFrontend(mConfig->configDisplay.displayFrontend.c_str()));
  mConfig->configProcessing.eventDisplay = mFrontend.get();
  mConfig->configDisplay.showTPCTracksFromO2Format = true;
  mParam.reset(new GPUParam);
  mParam->SetDefaults(&config->configGRP, &config->configReconstruction, &config->configProcessing, nullptr);
  mParam->par.earlyTpcTransform = 0;
  if (mConfig->configProcessing.runMC) {
    mQA.reset(new GPUQA(nullptr, &config->configQA, mParam.get()));
    mQA->InitO2MCData();
  }
  mDisplay.reset(GPUDisplayInterface::getDisplay(mFrontend.get(), nullptr, mQA.get(), mParam.get(), &mConfig->configCalib, &mConfig->configDisplay));
}

GPUO2InterfaceDisplay::~GPUO2InterfaceDisplay() = default;

int32_t GPUO2InterfaceDisplay::startDisplay()
{
  int32_t retVal = mDisplay->StartDisplay();
  if (retVal) {
    return retVal;
  }
  mDisplay->WaitForNextEvent();
  return 0;
}

int32_t GPUO2InterfaceDisplay::show(const GPUTrackingInOutPointers* ptrs)
{
  std::unique_ptr<GPUTrackingInOutPointers> tmpPtr;
  if (mConfig->configProcessing.runMC) {
    tmpPtr = std::make_unique<GPUTrackingInOutPointers>(*ptrs);
    mQA->InitO2MCData(tmpPtr.get());
    ptrs = tmpPtr.get();
  }
  mDisplay->ShowNextEvent(ptrs);
  do {
    usleep(10000);
  } while (mFrontend->getDisplayControl() == 0);
  if (mFrontend->getDisplayControl() == 2) {
    return 1;
  }
  mFrontend->setDisplayControl(0);
  mDisplay->WaitForNextEvent();
  return 0;
}

int32_t GPUO2InterfaceDisplay::endDisplay()
{
  mFrontend->DisplayExit();
  return 0;
}

void GPUO2InterfaceDisplay::UpdateCalib(const GPUCalibObjectsConst* calib)
{
  mDisplay->UpdateCalib(calib);
}

void GPUO2InterfaceDisplay::UpdateGRP(const GPUSettingsGRP* grp)
{
  mConfig->configGRP = *grp;
  mParam->UpdateSettings(&mConfig->configGRP);
  mDisplay->UpdateParam(mParam.get());
  if (mConfig->configProcessing.runMC) {
    mQA->UpdateParam(mParam.get());
  }
}
