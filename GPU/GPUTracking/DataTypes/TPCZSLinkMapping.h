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

/// \file TPCZSLinkMapping.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_TPC_ZS_LINK_MAPPING_H
#define O2_GPU_TPC_ZS_LINK_MAPPING_H

#include "clusterFinderDefs.h"
#ifdef GPUCA_TPC_GEOMETRY_O2
#include "TPCBase/PadPos.h"
#endif

namespace o2::tpc
{
class Mapper;
}

namespace GPUCA_NAMESPACE::gpu
{

struct TPCZSLinkMapping {
#ifndef GPUCA_GPUCODE
  TPCZSLinkMapping() = default;
  TPCZSLinkMapping(o2::tpc::Mapper& mapper);
#endif
#ifdef GPUCA_TPC_GEOMETRY_O2
  o2::tpc::PadPos FECIDToPadPos[TPC_FEC_IDS_IN_SECTOR];
#endif
};

} // namespace GPUCA_NAMESPACE::gpu

#endif
