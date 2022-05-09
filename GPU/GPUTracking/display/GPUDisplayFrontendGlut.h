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

/// \file GPUDisplayFrontendGlut.h
/// \author David Rohr

#ifndef GPUDISPLAYFRONTENDGLUT_H
#define GPUDISPLAYFRONTENDGLUT_H

#include "GPUDisplayFrontend.h"
#include <pthread.h>

namespace GPUCA_NAMESPACE::gpu
{
class GPUDisplayFrontendGlut : public GPUDisplayFrontend
{
 public:
  GPUDisplayFrontendGlut();
  ~GPUDisplayFrontendGlut() override = default;

  int StartDisplay() override;
  void DisplayExit() override;
  void SwitchFullscreen(bool set) override;
  void ToggleMaximized(bool set) override;
  void SetVSync(bool enable) override;
  void OpenGLPrint(const char* s, float x, float y, float r, float g, float b, float a, bool fromBotton = true) override;

 private:
  int FrontendMain() override;

  static void displayFunc();
  static void glutLoopFunc();
  static void keyboardUpFunc(unsigned char key, int x, int y);
  static void keyboardDownFunc(unsigned char key, int x, int y);
  static void specialUpFunc(int key, int x, int y);
  static void specialDownFunc(int key, int x, int y);
  static void mouseMoveFunc(int x, int y);
  static void mMouseWheelFunc(int button, int dir, int x, int y);
  static void mouseFunc(int button, int state, int x, int y);
  static void ResizeSceneWrapper(int width, int height);
  static int GetKey(int key);
  static void GetKey(int keyin, int& keyOut, int& keyPressOut, bool special);

  volatile bool mGlutRunning = false;
  pthread_mutex_t mSemLockExit = PTHREAD_MUTEX_INITIALIZER;

  int mWidth = INIT_WIDTH;
  int mHeight = INIT_HEIGHT;
  bool mFullScreen = false;
};
} // namespace GPUCA_NAMESPACE::gpu

#endif
