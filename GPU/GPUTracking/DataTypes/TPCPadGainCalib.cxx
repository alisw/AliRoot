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

/// \file TPCPadGainCalib.cxx
/// \author Felix Weiglhofer

#include "TPCPadGainCalib.h"

#include "GPUTPCGeometry.h"
#include "DataFormatsTPC/Constants.h"
#include "TPCBase/CalDet.h"

using namespace GPUCA_NAMESPACE::gpu;

TPCPadGainCalib::TPCPadGainCalib()
{
  GPUTPCGeometry geo{};
  int offset = 0;
  for (int r = 0; r < GPUCA_ROW_COUNT; r++) {
    mPadOffsetPerRow[r] = offset;
    offset += geo.NPads(r);
  }
}

TPCPadGainCalib::TPCPadGainCalib(const o2::tpc::CalDet<float>& gainMap) : TPCPadGainCalib()
{
  setFromMap(gainMap);
}

TPCPadGainCalib::TPCPadGainCalib(const o2::tpc::CalDet<float>& gainMap, const float minValue, const float maxValue, const bool inv) : TPCPadGainCalib()
{
  setMinCorrectionFactor(minValue);
  setMaxCorrectionFactor(maxValue);
  setFromMap(gainMap, inv);
}

void TPCPadGainCalib::setFromMap(const o2::tpc::CalDet<float>& gainMap, const bool inv)
{
  for (int sector = 0; sector < o2::tpc::constants::MAXSECTOR; sector++) {
    for (int p = 0; p < TPC_PADS_IN_SECTOR; p++) {
      const float gainVal = gainMap.getValue(sector, p);
      inv ? mGainCorrection[sector].set(p, (gainVal > 1.e-5f) ? 1.f / gainVal : 1.f) : mGainCorrection[sector].set(p, gainVal);
    }
  }
}
