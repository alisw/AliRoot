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

/// \file GPUTriggerOutputs.h
/// \author David Rohr

#ifndef GPUTRIGGEROUTPUTS_H
#define GPUTRIGGEROUTPUTS_H

#include "GPUCommonDef.h"
#include <unordered_set>
#include <array>
#ifdef GPUCA_HAVE_O2HEADERS
#include "DataFormatsTPC/ZeroSuppression.h"
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{

struct GPUTriggerOutputs {
#ifdef GPUCA_HAVE_O2HEADERS
  struct hasher {
    size_t operator()(const o2::tpc::TriggerInfoDLBZS& key) const
    {
      std::array<unsigned int, sizeof(key) / sizeof(unsigned int)> tmp;
      memcpy((void*)tmp.data(), (const void*)&key, sizeof(key));
      std::hash<unsigned int> std_hasher;
      size_t result = 0;
      for (size_t i = 0; i < tmp.size(); ++i) {
        result ^= std_hasher(tmp[i]);
      }
      return result;
    }
  };

  struct equal {
    bool operator()(const o2::tpc::TriggerInfoDLBZS& lhs, const o2::tpc::TriggerInfoDLBZS& rhs) const
    {
      return memcmp((const void*)&lhs, (const void*)&rhs, sizeof(lhs)) == 0;
    }
  };

  std::unordered_set<o2::tpc::TriggerInfoDLBZS, hasher, equal> triggers;
  static_assert(sizeof(o2::tpc::TriggerInfoDLBZS) % sizeof(unsigned int) == 0);
#endif
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
