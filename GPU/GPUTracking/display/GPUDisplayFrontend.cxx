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

/// \file GPUDisplayFrontend.cxx
/// \author David Rohr

#include "GPUDisplayFrontend.h"
#include "GPUDisplay.h"

#ifdef _WIN32
#include "GPUDisplayFrontendWindows.h"
#elif defined(GPUCA_BUILD_EVENT_DISPLAY_X11)
#include "GPUDisplayFrontendX11.h"
#endif
#ifdef GPUCA_BUILD_EVENT_DISPLAY_GLFW
#include "GPUDisplayFrontendGlfw.h"
#endif
#ifdef GPUCA_BUILD_EVENT_DISPLAY_GLUT
#include "GPUDisplayFrontendGlut.h"
#endif
#ifdef GPUCA_BUILD_EVENT_DISPLAY_WAYLAND
#include "GPUDisplayFrontendWayland.h"
#endif

#ifdef GPUCA_BUILD_EVENT_DISPLAY_QT
#include "GPUDisplayGUIWrapper.h"
#else
namespace GPUCA_NAMESPACE::gpu
{
class GPUDisplayGUIWrapper
{
};
} // namespace GPUCA_NAMESPACE::gpu
#endif

using namespace GPUCA_NAMESPACE::gpu;

GPUDisplayFrontend::~GPUDisplayFrontend() = default;

void* GPUDisplayFrontend::FrontendThreadWrapper(void* ptr)
{
  GPUDisplayFrontend* me = reinterpret_cast<GPUDisplayFrontend*>(ptr);
  int retVal = me->FrontendMain();
  if (retVal == -1) {
    me->InitDisplay(true);
  }
  return ((void*)(size_t)retVal);
}

void GPUDisplayFrontend::HandleSendKey()
{
  if (mSendKey) {
    mDisplay->HandleSendKey(mSendKey);
    mSendKey = 0;
  }
}

void GPUDisplayFrontend::HandleKey(unsigned char key) { mDisplay->HandleKey(key); }
int GPUDisplayFrontend::DrawGLScene() { return mDisplay->DrawGLScene(); }
void GPUDisplayFrontend::ResizeScene(int width, int height)
{
  mDisplayHeight = height;
  mDisplayWidth = width;
  mDisplay->ResizeScene(width, height);
}
int GPUDisplayFrontend::InitDisplay(bool initFailure) { return mDisplay->InitDisplay(initFailure); }
void GPUDisplayFrontend::ExitDisplay()
{
  mDisplay->ExitDisplay();
  stopGUI();
  mGUI.reset(nullptr);
}
bool GPUDisplayFrontend::EnableSendKey() { return true; }

void GPUDisplayFrontend::stopGUI()
{
#ifdef GPUCA_BUILD_EVENT_DISPLAY_QT
  if (mGUI) {
    mGUI->stop();
  }
#endif
}

int GPUDisplayFrontend::startGUI()
{
  int retVal = 1;
#ifdef GPUCA_BUILD_EVENT_DISPLAY_QT
  if (!mGUI) {
    mGUI.reset(new GPUDisplayGUIWrapper);
  }
  if (!mGUI->isRunning()) {
    mGUI->start();
  } else {
    mGUI->focus();
  }
#endif
  return retVal;
}

GPUDisplayFrontend* GPUDisplayFrontend::getFrontend(const char* type)
{
#if !defined(GPUCA_STANDALONE) && defined(GPUCA_BUILD_EVENT_DISPLAY_GLFW)
  if (strcmp(type, "glfw") == 0 || strcmp(type, "auto") == 0) {
    return new GPUDisplayFrontendGlfw;
  } else
#endif
#ifdef _WIN32
  if (strcmp(type, "windows") == 0 || strcmp(type, "auto") == 0) {
    return new GPUDisplayFrontendWindows;
  } else
#elif defined(GPUCA_BUILD_EVENT_DISPLAY_X11)
  if (strcmp(type, "x11") == 0 || strcmp(type, "auto") == 0) {
    return new GPUDisplayFrontendX11;
  } else
#endif
#if defined(GPUCA_STANDALONE) && defined(GPUCA_BUILD_EVENT_DISPLAY_GLFW)
  if (strcmp(type, "glfw") == 0 || strcmp(type, "auto") == 0) {
    return new GPUDisplayFrontendGlfw;
  } else
#endif
#ifdef GPUCA_BUILD_EVENT_DISPLAY_WAYLAND
  if (strcmp(type, "wayland") == 0 || (strcmp(type, "auto") == 0 && getenv("XDG_SESSION_TYPE") && strcmp(getenv("XDG_SESSION_TYPE"), "wayland") == 0)) {
    return new GPUDisplayFrontendWayland;
  } else
#endif
#ifdef GPUCA_BUILD_EVENT_DISPLAY_GLUT
  if (strcmp(type, "glut") == 0 || strcmp(type, "auto") == 0) {
    return new GPUDisplayFrontendGlut;
  } else
#endif
  {
    GPUError("Requested frontend not available");
  }
  return nullptr;
}

GPUDisplayBackend* GPUDisplayFrontend::backend()
{
  return mDisplay->backend();
}

int& GPUDisplayFrontend::drawTextFontSize()
{
  return mDisplay->drawTextFontSize();
}
