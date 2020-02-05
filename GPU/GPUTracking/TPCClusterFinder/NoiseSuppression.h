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

/// \file NoiseSuppression.h
/// \author Felix Weiglhofer

#ifndef O2_GPU_NOISE_SUPPRESSION_H
#define O2_GPU_NOISE_SUPPRESSION_H

#include "clusterFinderDefs.h"
#include "GPUTPCClusterFinderKernels.h"
#include "Array2D.h"
#include "PackedCharge.h"

namespace GPUCA_NAMESPACE
{
namespace gpu
{

class NoiseSuppression
{

 public:
  static GPUd() void noiseSuppressionImpl(int, int, int, int, GPUTPCClusterFinderKernels::GPUTPCSharedMemory&, const Array2D<PackedCharge>&, const Array2D<uchar>&, const deprecated::Digit*, const uint, uchar*);

  static GPUd() void updatePeaksImpl(int, int, int, int, GPUTPCClusterFinderKernels::GPUTPCSharedMemory&, const deprecated::Digit*, const uchar*, Array2D<uchar>&);

 private:
  static GPUd() void checkForMinima(float, float, PackedCharge, int, ulong*, ulong*);

  static GPUd() void findMinimaScratchPad(const PackedCharge*, const ushort, const int, int, const float, const float, ulong*, ulong*);

  static GPUd() void findPeaksScratchPad(const uchar*, const ushort, const int, int, ulong*);

  static GPUd() void findMinima(const Array2D<PackedCharge>&, const ChargePos&, const float, const float, ulong*, ulong*);

  static GPUd() ulong findPeaks(const Array2D<uchar>&, const ChargePos&);

  static GPUd() bool keepPeak(ulong, ulong);

  static GPUd() void findMinimaAndPeaksScratchpad(const Array2D<PackedCharge>&, const Array2D<uchar>&, float, const ChargePos&, ChargePos*, PackedCharge*, ulong*, ulong*, ulong*);
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
