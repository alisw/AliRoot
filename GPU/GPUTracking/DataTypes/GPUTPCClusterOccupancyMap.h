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

/// \file GPUTPCClusterOccupancyMap.h
/// \author David Rohr

#ifndef GPUTPCCLUSTEROCCUPANCYMAP_H
#define GPUTPCCLUSTEROCCUPANCYMAP_H

#include "GPUCommonDef.h"
#include "GPUDefConstantsAndSettings.h"

namespace GPUCA_NAMESPACE::gpu
{
struct GPUParam;
struct GPUTPCClusterOccupancyMapBin {
  unsigned short bin[GPUCA_NSLICES][GPUCA_ROW_COUNT];

  GPUd() static unsigned int getNBins(const GPUParam& param);
  GPUd() static unsigned int getTotalSize(const GPUParam& param);
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
