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

/// \file GPUROOTDumpCore.cxx
/// \author David Rohr

#include "GPUROOTDumpCore.h"

#if (!defined(GPUCA_STANDALONE) || defined(GPUCA_BUILD_QA)) && !defined(GPUCA_GPUCODE)
#include <atomic>
#include <memory>
#include <TFile.h>

using namespace GPUCA_NAMESPACE::gpu;

std::weak_ptr<GPUROOTDumpCore> GPUROOTDumpCore::sInstance;

GPUROOTDumpCore::GPUROOTDumpCore(GPUROOTDumpCore::GPUROOTDumpCorePrivate)
{
}

GPUROOTDumpCore::~GPUROOTDumpCore()
{
  if (mFile) {
    for (unsigned int i = 0; i < mBranches.size(); i++) {
      mBranches[i]->write();
    }
    mFile->Close();
  }
}

std::shared_ptr<GPUROOTDumpCore> GPUROOTDumpCore::getAndCreate()
{
  static std::atomic_flag lock = ATOMIC_FLAG_INIT;
  while (lock.test_and_set(std::memory_order_acquire)) {
  }
  std::shared_ptr<GPUROOTDumpCore> retVal = sInstance.lock();
  if (!retVal) {
    retVal = std::make_shared<GPUROOTDumpCore>(GPUROOTDumpCorePrivate());
    sInstance = retVal;
  }
  lock.clear(std::memory_order_release);
  return retVal;
}

GPUROOTDumpBase::GPUROOTDumpBase()
{
  std::shared_ptr<GPUROOTDumpCore> p = GPUROOTDumpCore::get().lock();
  if (!p) {
    throw std::runtime_error("No instance of GPUROOTDumpCore exists");
  }
  p->mBranches.emplace_back(this);
  if (!p->mFile) {
    std::remove("gpudebug.root");
    p->mFile.reset(new TFile("gpudebug.root", "recreate"));
  }
  p->mFile->cd();
}

#endif
