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

/// \file GPUTPCCFMCLabelFlattener.cxx
/// \author Felix Weiglhofer

#include "GPUTPCCFMCLabelFlattener.h"

#if !defined(GPUCA_GPUCODE)
#include "GPUHostDataTypes.h"
#endif

using namespace GPUCA_NAMESPACE::gpu;
using namespace GPUCA_NAMESPACE::gpu::tpccf;

#if !defined(GPUCA_GPUCODE)
void GPUTPCCFMCLabelFlattener::setGlobalOffsetsAndAllocate(
  GPUTPCClusterFinder& cls,
  GPUTPCLinearLabels& labels)
{
  uint headerOffset = labels.header.size();
  uint dataOffset = labels.data.size();

  cls.mPlabelsHeaderGlobalOffset = headerOffset;
  cls.mPlabelsDataGlobalOffset = dataOffset;

  for (Row row = 0; row < GPUCA_ROW_COUNT; row++) {
    headerOffset += cls.mPclusterInRow[row];
    dataOffset += cls.mPlabelsInRow[row];
  }

  labels.header.resize(headerOffset);
  labels.data.resize(dataOffset);
}
#endif

template <>
GPUd() void GPUTPCCFMCLabelFlattener::Thread<GPUTPCCFMCLabelFlattener::setRowOffsets>(int nBlocks, int nThreads, int iBlock, int iThread, GPUSharedMemory&, processorType& clusterer)
{
#if !defined(GPUCA_GPUCODE)
  Row row = get_global_id(0);

  uint clusterInRow = clusterer.mPclusterInRow[row];
  uint labelCount = 0;

  for (uint i = 0; i < clusterInRow; i++) {
    auto& interim = clusterer.mPlabelsByRow[row].data[i];
    labelCount += interim.labels.size();
  }

  clusterer.mPlabelsInRow[row] = labelCount;
#endif
}

template <>
GPUd() void GPUTPCCFMCLabelFlattener::Thread<GPUTPCCFMCLabelFlattener::flatten>(int nBlocks, int nThreads, int iBlock, int iThread, GPUSharedMemory&, processorType& clusterer, GPUTPCLinearLabels* out)
{
#if !defined(GPUCA_GPUCODE)
  uint row = get_global_id(0);

  uint headerOffset = clusterer.mPlabelsHeaderGlobalOffset;
  uint dataOffset = clusterer.mPlabelsDataGlobalOffset;
  for (uint r = 0; r < row; r++) {
    headerOffset += clusterer.mPclusterInRow[r];
    dataOffset += clusterer.mPlabelsInRow[r];
  }

  auto* labels = clusterer.mPlabelsByRow[row].data.data();
  for (uint c = 0; c < clusterer.mPclusterInRow[row]; c++) {
    GPUTPCClusterMCInterim& interim = labels[c];
    assert(dataOffset + interim.labels.size() <= out->data.size());
    out->header[headerOffset] = dataOffset;
    std::copy(interim.labels.cbegin(), interim.labels.cend(), out->data.begin() + dataOffset);

    headerOffset++;
    dataOffset += interim.labels.size();
    interim = {}; // ensure interim labels are destroyed to prevent memleak
  }
#endif
}
