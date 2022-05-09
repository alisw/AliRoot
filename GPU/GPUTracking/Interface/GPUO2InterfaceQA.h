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

/// \file GPUO2InterfaceQA.h
/// \author David Rohr

#ifndef GPUO2INTERFACEQA_H
#define GPUO2INTERFACEQA_H

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

class TH1F;
class TH1D;
class TH2F;
class TObjArray;

namespace o2
{
class MCCompLabel;
namespace tpc
{
class TrackTPC;
struct ClusterNativeAccess;
} // namespace tpc
} // namespace o2

namespace o2::gpu
{
class GPUQA;
struct GPUParam;
struct GPUO2InterfaceConfiguration;
class GPUO2InterfaceQA
{
 public:
  GPUO2InterfaceQA(const GPUO2InterfaceConfiguration* config = nullptr);
  ~GPUO2InterfaceQA();

  int initializeForProcessing(int tasks); // only needed for processing, not for postprocessing

  void runQA(const std::vector<o2::tpc::TrackTPC>* tracksExternal, const std::vector<o2::MCCompLabel>* tracksExtMC, const o2::tpc::ClusterNativeAccess* clNative);
  int postprocess(TObjArray& out);

  // Input might be modified, so we assume non-const. If it is const, a copy should be created before.
  int postprocessExternal(std::vector<TH1F>& in1, std::vector<TH2F>& in2, std::vector<TH1D>& in3, TObjArray& out, int tasks);

  void getHists(const std::vector<TH1F>*& h1, const std::vector<TH2F>*& h2, const std::vector<TH1D>*& h3);
  void resetHists();
  void cleanup();

 private:
  std::unique_ptr<GPUQA> mQA;
  std::unique_ptr<GPUParam> mParam;
};
} // namespace o2::gpu

#endif
