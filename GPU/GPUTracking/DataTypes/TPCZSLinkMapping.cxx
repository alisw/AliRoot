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

/// \file TPCZSLinkMapping.cxx
/// \author Felix Weiglhofer

#include "TPCZSLinkMapping.h"
#include "TPCBase/Mapper.h"

#include <algorithm>
#include <cassert>

using namespace GPUCA_NAMESPACE::gpu;

TPCZSLinkMapping::TPCZSLinkMapping(o2::tpc::Mapper& mapper)
{
#ifdef GPUCA_TPC_GEOMETRY_O2
  const auto& fecToGlobalPad = mapper.getMapFECIDGlobalPad();
  assert(fecToGlobalPad.size() == TPC_FEC_IDS_IN_SECTOR);

  const auto& globalPadToPadPos = mapper.getMapGlobalPadToPadPos();
  assert(globalPadToPadPos.size() == TPC_PADS_IN_SECTOR);

  for (size_t i = 0; i < TPC_FEC_IDS_IN_SECTOR; i++) {
    FECIDToPadPos[i] = globalPadToPadPos[fecToGlobalPad[i]];
  }
#endif
}
