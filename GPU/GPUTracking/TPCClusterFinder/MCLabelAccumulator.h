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

/// \file MCLabelAccumulator.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_MC_LABEL_ACCUMULATOR_H
#define O2_GPU_MC_LABEL_ACCUMULATOR_H

#include "clusterFinderDefs.h"
#include "Array2D.h"
#include <bitset>
#include <vector>

namespace o2
{
class MCCompLabel;
namespace dataformats
{
class MCCompLabel;
template <typename T>
class ConstMCTruthContainerView;
using ConstMCLabelContainerView = o2::dataformats::ConstMCTruthContainerView<o2::MCCompLabel>;
} // namespace dataformats
} // namespace o2

namespace GPUCA_NAMESPACE::gpu
{

class GPUTPCClusterFinder;
struct GPUTPCClusterMCInterimArray;

class MCLabelAccumulator
{

 public:
  MCLabelAccumulator(GPUTPCClusterFinder&);

  void collect(const ChargePos&, tpccf::Charge);

  bool engaged() const { return mLabels != nullptr && mOutput != nullptr; }

  void commit(tpccf::Row, uint32_t, uint32_t);

 private:
  Array2D<const uint32_t> mIndexMap;
  const o2::dataformats::ConstMCLabelContainerView* mLabels = nullptr;
  GPUTPCClusterMCInterimArray* mOutput = nullptr;

  std::bitset<64> mMaybeHasLabel;
  std::vector<o2::MCCompLabel> mClusterLabels;
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
