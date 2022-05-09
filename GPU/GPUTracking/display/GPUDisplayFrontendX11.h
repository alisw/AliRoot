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

/// \file GPUDisplayFrontendX11.h
/// \author David Rohr

#ifndef GPUDISPLAYFRONTENDX11_H
#define GPUDISPLAYFRONTENDX11_H

#include "GPUDisplayFrontend.h"
#include <GL/glx.h>
#include <pthread.h>
#include <unistd.h>
#include <GL/glxext.h>

namespace GPUCA_NAMESPACE::gpu
{
class GPUDisplayFrontendX11 : public GPUDisplayFrontend
{
 public:
  GPUDisplayFrontendX11();
  ~GPUDisplayFrontendX11() override = default;

  int StartDisplay() override;
  void DisplayExit() override;
  void SwitchFullscreen(bool set) override;
  void ToggleMaximized(bool set) override;
  void SetVSync(bool enable) override;
  void OpenGLPrint(const char* s, float x, float y, float r, float g, float b, float a, bool fromBotton = true) override;
  void getSize(int& width, int& height) override;
  int getVulkanSurface(void* instance, void* surface) override;
  unsigned int getReqVulkanExtensions(const char**& p) override;

 private:
  int FrontendMain() override;
  int GetKey(int key);
  void GetKey(XEvent& event, int& keyOut, int& keyPressOut);

  pthread_mutex_t mSemLockExit = PTHREAD_MUTEX_INITIALIZER;
  volatile bool mDisplayRunning = false;

  GLuint mFontBase;

  Display* mDisplay = nullptr;
  Window mWindow;

  PFNGLXSWAPINTERVALEXTPROC mGlXSwapIntervalEXT = nullptr;
  bool vsync_supported = false;
};
} // namespace GPUCA_NAMESPACE::gpu

#endif
