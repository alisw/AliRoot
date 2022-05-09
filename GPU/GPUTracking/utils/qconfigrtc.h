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

/// \file qconfigrtc.h
/// \author David Rohr

#ifndef QCONFIG_RTC_H
#define QCONFIG_RTC_H

#include "qconfig.h"
#include "qconfig_helpers.h"

#ifndef qon_mxstr
#define qon_mstr(a) #a
#define qon_mxstr(a) qon_mstr(a)
#endif
#ifndef qon_mxcat
#define qon_mcat(a, b) a##b
#define qon_mxcat(a, b) qon_mcat(a, b)
#endif

template <class T>
static std::string qConfigPrintRtc(const T& tSrc, bool useConstexpr)
{
#if defined(__cplusplus) && __cplusplus >= 201703L
  std::stringstream out;
#define QCONFIG_PRINT_RTC
#include "qconfig.h"
#undef QCONFIG_PRINT_RTC
  return out.str();
#else
  throw std::runtime_error("not supported");
#endif
}

#define QCONFIG_CONVERT_RTC
#include "qconfig.h"
#undef QCONFIG_CONVERT_RTC

#endif
