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

/// \file GPUDisplayInterface.h
/// \author David Rohr

#ifndef GPUDISPLAYINTERFACE_H
#define GPUDISPLAYINTERFACE_H

#include "GPUSettings.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class GPUChainTracking;
class GPUQA;
struct GPUParam;
class GPUDisplayInterface
{
 public:
  GPUDisplayInterface(const GPUDisplayInterface&) = delete;
  virtual ~GPUDisplayInterface();
  virtual int StartDisplay() = 0;
  virtual void ShowNextEvent(const GPUTrackingInOutPointers* ptrs = nullptr) = 0;
  virtual void WaitForNextEvent() = 0;
  virtual void SetCollisionFirstCluster(unsigned int collision, int slice, int cluster) = 0;
  static GPUDisplayInterface* getDisplay(GPUDisplayFrontendInterface* frontend, GPUChainTracking* chain, GPUQA* qa, const GPUParam* param = nullptr, const GPUCalibObjectsConst* calib = nullptr, const GPUSettingsDisplay* config = nullptr);

 protected:
  GPUDisplayInterface();
};

class GPUDisplayFrontendInterface
{
 public:
  virtual ~GPUDisplayFrontendInterface();
  static GPUDisplayFrontendInterface* getFrontend(const char* type);
  virtual void DisplayExit() = 0;
  virtual bool EnableSendKey() = 0;
  virtual int getDisplayControl() const = 0;
  virtual int getSendKey() const = 0;
  virtual int getNeedUpdate() const = 0;
  virtual void setDisplayControl(int v) = 0;
  virtual void setSendKey(int v) = 0;
  virtual void setNeedUpdate(int v) = 0;
  virtual const char* frontendName() const = 0;

 protected:
  GPUDisplayFrontendInterface();
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif // GPUDISPLAYINTERFACE_H
