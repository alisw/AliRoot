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

/// \file GPUO2InterfaceQA.cxx
/// \author David Rohr

#include "GPUParam.h"
#include "GPUQA.h"
#include "GPUO2InterfaceConfiguration.h"
#include "GPUO2InterfaceQA.h"
#include "TGraphAsymmErrors.h"

using namespace o2::gpu;
using namespace o2::tpc;

GPUO2InterfaceQA::GPUO2InterfaceQA(const GPUO2InterfaceConfiguration* config)
{
  mParam.reset(new GPUParam);
  mParam->SetDefaults(&config->configGRP, &config->configReconstruction, &config->configProcessing, nullptr);
  mQA.reset(new GPUQA(nullptr, &config->configQA, mParam.get()));
}

GPUO2InterfaceQA::~GPUO2InterfaceQA() = default;

int GPUO2InterfaceQA::initializeForProcessing(int tasks)
{
  return mQA->InitQA(tasks);
}

void GPUO2InterfaceQA::runQA(const std::vector<o2::tpc::TrackTPC>* tracksExternal, const std::vector<o2::MCCompLabel>* tracksExtMC, const o2::tpc::ClusterNativeAccess* clNative)
{
  mQA->RunQA(false, tracksExternal, tracksExtMC, clNative);
}
int GPUO2InterfaceQA::postprocess(TObjArray& out)
{
  return mQA->DrawQAHistograms(&out);
}

int GPUO2InterfaceQA::postprocessExternal(std::vector<TH1F>& in1, std::vector<TH2F>& in2, std::vector<TH1D>& in3, std::vector<TGraphAsymmErrors>& in4, TObjArray& out, int tasks)
{
  if (mQA->loadHistograms(in1, in2, in3, in4, tasks)) {
    return 1;
  }
  return mQA->DrawQAHistograms(&out);
}

void GPUO2InterfaceQA::cleanup()
{
  mQA->DrawQAHistogramsCleanup();
}

void GPUO2InterfaceQA::getHists(const std::vector<TH1F>*& h1, const std::vector<TH2F>*& h2, const std::vector<TH1D>*& h3, const std::vector<TGraphAsymmErrors>*& h4)
{
  h1 = &mQA->getHistograms1D();
  h2 = &mQA->getHistograms2D();
  h3 = &mQA->getHistograms1Dd();
  h4 = &mQA->getGraphs();
}

void GPUO2InterfaceQA::resetHists()
{
  mQA->resetHists();
}

void GPUO2InterfaceQA::updateGRP(GPUSettingsGRP* grp)
{
  mParam->UpdateSettings(grp, nullptr);
  mQA->UpdateParam(mParam.get());
}
