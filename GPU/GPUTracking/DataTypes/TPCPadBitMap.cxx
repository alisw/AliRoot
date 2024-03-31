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

/// \file TPCPadBitMap.cxx
/// \author Jens Wiechula

#include "TPCPadBitMap.h"

#include "GPUTPCGeometry.h"
#include "DataFormatsTPC/Constants.h"

using namespace GPUCA_NAMESPACE::gpu;

TPCPadBitMap::TPCPadBitMap()
{
  GPUTPCGeometry geo{};
  int offset = 0;
  for (int r = 0; r < GPUCA_ROW_COUNT; r++) {
    mPadOffsetPerRow[r] = offset;
    offset += geo.NPads(r);
  }
}

#ifndef GPUCA_STANDALONE
#include "TPCBase/CalDet.h"

TPCPadBitMap::TPCPadBitMap(const o2::tpc::CalDet<bool>& map) : TPCPadBitMap()
{
  setFromMap(map);
}

void TPCPadBitMap::setFromMap(const o2::tpc::CalDet<bool>& map)
{
  for (int sector = 0; sector < o2::tpc::constants::MAXSECTOR; sector++) {
    for (int p = 0; p < TPC_PADS_IN_SECTOR; p++) {
      const auto val = map.getValue(sector, p);
      mBitMap[sector].set(p, val);
    }
  }
}
#endif
