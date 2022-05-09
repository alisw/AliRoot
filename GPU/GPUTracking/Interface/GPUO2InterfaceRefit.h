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

/// \file GPUO2InterfaceRefit.h
/// \author David Rohr

#ifndef GPUO2INTERFACEREFIT_H
#define GPUO2INTERFACEREFIT_H

// Some defines denoting that we are compiling for O2
#ifndef GPUCA_HAVE_O2HEADERS
#define GPUCA_HAVE_O2HEADERS
#endif
#ifndef GPUCA_TPC_GEOMETRY_O2
#define GPUCA_TPC_GEOMETRY_O2
#endif
#ifndef GPUCA_O2_INTERFACE
#define GPUCA_O2_INTERFACE
#endif

#include <memory>
#include <vector>
#include <gsl/span>

namespace o2::base
{
template <typename value_T>
class PropagatorImpl;
using Propagator = PropagatorImpl<float>;
} // namespace o2::base
namespace o2::dataformats
{
template <typename FirstEntry, typename NElem>
class RangeReference;
}
namespace o2::tpc
{
using TPCClRefElem = uint32_t;
using TrackTPCClusRef = o2::dataformats::RangeReference<uint32_t, uint16_t>;
class TrackTPC;
struct ClusterNativeAccess;
} // namespace o2::tpc
namespace o2::track
{
template <typename value_T>
class TrackParametrizationWithError;
using TrackParCovF = TrackParametrizationWithError<float>;
using TrackParCov = TrackParCovF;
} // namespace o2::track

namespace o2::gpu
{
class GPUParam;
class GPUTrackingRefit;
class TPCFastTransform;
class GPUO2InterfaceRefit
{
 public:
  // Must initialize with:
  // - In any case: Cluster Native access structure (cl), TPC Fast Transformation instance (trans), solenoid field (bz), TPC Track hit references (trackRef)
  // - Either the shared cluster map (sharedmap) or the vector of tpc tracks (trks) to build the shared cluster map internally
  // - o2::base::Propagator (p) in case RefitTrackAsTrackParCov is to be used

  GPUO2InterfaceRefit(const o2::tpc::ClusterNativeAccess* cl, const TPCFastTransform* trans, float bz, const o2::tpc::TPCClRefElem* trackRef, const unsigned char* sharedmap = nullptr, const std::vector<o2::tpc::TrackTPC>* trks = nullptr, o2::base::Propagator* p = nullptr);
  ~GPUO2InterfaceRefit();

  int RefitTrackAsGPU(o2::tpc::TrackTPC& trk, bool outward = false, bool resetCov = false);
  int RefitTrackAsTrackParCov(o2::tpc::TrackTPC& trk, bool outward = false, bool resetCov = false);
  int RefitTrackAsGPU(o2::track::TrackParCov& trk, const o2::tpc::TrackTPCClusRef& clusRef, float time0, float* chi2 = nullptr, bool outward = false, bool resetCov = false);
  int RefitTrackAsTrackParCov(o2::track::TrackParCov& trk, const o2::tpc::TrackTPCClusRef& clusRef, float time0, float* chi2 = nullptr, bool outward = false, bool resetCov = false);
  void setGPUTrackFitInProjections(bool v = true);
  void setTrackReferenceX(float v);
  void setIgnoreErrorsAtTrackEnds(bool v);

  static void fillSharedClustersMap(const o2::tpc::ClusterNativeAccess* cl, const gsl::span<const o2::tpc::TrackTPC> trks, const o2::tpc::TPCClRefElem* trackRef, unsigned char* shmap);

 private:
  std::unique_ptr<GPUTrackingRefit> mRefit;
  std::unique_ptr<GPUParam> mParam;
  std::vector<unsigned char> mSharedMap;
};
} // namespace o2::gpu

#endif
