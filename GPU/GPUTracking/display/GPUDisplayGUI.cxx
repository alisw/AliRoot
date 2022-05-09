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

/// \file GPUDisplayGUI.cxx
/// \author David Rohr

#include "GPUDisplayGUI.h"
#include "GPUDisplayGUIWrapper.h"
#include "./ui_GPUDisplayGUI.h"

using namespace o2::gpu;

GPUDisplayGUI::GPUDisplayGUI(QWidget* parent)
  : QMainWindow(parent), ui(new Ui::GPUDisplayGUI)
{
  ui->setupUi(this);
}

GPUDisplayGUI::~GPUDisplayGUI()
{
  delete ui;
}

void GPUDisplayGUI::UpdateTimer()
{
  mWrapper->UpdateTimer();
}
