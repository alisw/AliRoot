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

/// \file GPUDisplayGUI.h
/// \author David Rohr

#ifndef GPUDISPLAYGUI_H
#define GPUDISPLAYGUI_H

#define QT_BEGIN_NAMESPACE \
  namespace o2::gpu::qtgui \
  {
#define QT_END_NAMESPACE }

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class GPUDisplayGUI;
}
QT_END_NAMESPACE

namespace o2::gpu
{
class GPUDisplayGUIWrapper;
class GPUDisplayGUI : public QMainWindow
{
  Q_OBJECT

 public:
  GPUDisplayGUI(QWidget* parent = nullptr);
  ~GPUDisplayGUI();
  void setWrapper(GPUDisplayGUIWrapper* w) { mWrapper = w; }

 private slots:
  void UpdateTimer();

 private:
  Ui::GPUDisplayGUI* ui;
  GPUDisplayGUIWrapper* mWrapper;
};
} // namespace o2::gpu

#endif // GPUDISPLAYGUI_H
