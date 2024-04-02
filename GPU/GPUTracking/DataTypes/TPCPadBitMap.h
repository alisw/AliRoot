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

/// \file TPCPadBitMap.h
/// \author Jens Wiechula

#ifndef O2_GPU_TPC_PAD_BITMAP_H
#define O2_GPU_TPC_PAD_BITMAP_H

#include "clusterFinderDefs.h"
#include "GPUCommonMath.h"

namespace o2::tpc
{
template <class T>
class CalDet;
} // namespace o2::tpc

namespace GPUCA_NAMESPACE::gpu
{

struct TPCPadBitMap {
 public:
#ifndef GPUCA_GPUCODE
  TPCPadBitMap();
  TPCPadBitMap(const o2::tpc::CalDet<bool>&);

  /// setting the stored values from CalDet
  void setFromMap(const o2::tpc::CalDet<bool>&);
#endif

  GPUdi() void set(int sector, tpccf::Row row, tpccf::Pad pad, bool c)
  {
    mBitMap[sector].set(globalPad(row, pad), c);
  }

  GPUdi() void set(int sector, unsigned short globalPad, bool c)
  {
    mBitMap[sector].set(globalPad, c);
  }

  GPUdi() bool isSet(int sector, tpccf::Row row, tpccf::Pad pad) const
  {
    return mBitMap[sector].get(globalPad(row, pad));
  }

  GPUdi() bool isSet(int sector, unsigned short globalPad) const
  {
    return mBitMap[sector].get(globalPad);
  }

  GPUdi() unsigned short globalPad(tpccf::Row row, tpccf::Pad pad) const
  {
    return mPadOffsetPerRow[row] + pad;
  }

 private:
  /// Flat dead channel map, one value per pad in sector
  class SectorBitMap
  {
   public:
    using T = unsigned int;
    static constexpr int NWORDS = (TPC_PADS_IN_SECTOR + sizeof(T) * 8 - 1) / sizeof(T);
    GPUdi() SectorBitMap()
    {
      reset();
    }

    GPUdi() void set(unsigned short globalPad, bool c)
    {
      const auto word = globalPad / (sizeof(T) * 8);
      const auto pos = globalPad % (sizeof(T) * 8);
      const auto mask = T(1) << pos;
      mDeadChannelMap[word] = (mDeadChannelMap[word] & ~mask) | (T(c) << pos);
    }

    GPUdi() bool get(unsigned short globalPad) const
    {
      const auto word = globalPad / (sizeof(T) * 8);
      const auto pos = globalPad % (sizeof(T) * 8);
      const auto mask = T(1) << pos;
      return (mDeadChannelMap[word] & mask) == mask;
    }

    GPUd() void reset()
    {
      for (int iword = 0; iword < NWORDS; ++iword) {
        mDeadChannelMap[iword] = 0;
      }
    }

   private:
    T mDeadChannelMap[NWORDS];
  };

  unsigned short mPadOffsetPerRow[GPUCA_ROW_COUNT];
  SectorBitMap mBitMap[GPUCA_NSLICES];
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
