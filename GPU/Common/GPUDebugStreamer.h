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

/// \file GPUDebugStreamer.h

#ifndef GPUDEBUGSTREAMER_H
#define GPUDEBUGSTREAMER_H

#include "GPUCommonDef.h"
#if defined(GPUCA_HAVE_O2HEADERS) && !defined(GPUCA_GPUCODE) && !defined(GPUCA_STANDALONE) && defined(DEBUG_STREAMER)
#include "CommonUtils/DebugStreamer.h"
#endif

#endif
