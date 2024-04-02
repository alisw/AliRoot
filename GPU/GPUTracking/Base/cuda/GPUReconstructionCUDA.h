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

/// \file GPUReconstructionCUDA.h
/// \author David Rohr

#ifndef GPURECONSTRUCTIONCUDA_H
#define GPURECONSTRUCTIONCUDA_H

#include "GPUReconstructionDeviceBase.h"
#include <vector>
#include <string>

#ifdef _WIN32
extern "C" __declspec(dllexport) GPUCA_NAMESPACE::gpu::GPUReconstruction* GPUReconstruction_Create_CUDA(const GPUCA_NAMESPACE::gpu::GPUSettingsDeviceBackend& cfg);
#else
extern "C" GPUCA_NAMESPACE::gpu::GPUReconstruction* GPUReconstruction_Create_CUDA(const GPUCA_NAMESPACE::gpu::GPUSettingsDeviceBackend& cfg);
#endif

namespace GPUCA_NAMESPACE
{
namespace gpu
{
struct GPUReconstructionCUDAInternals;

class GPUReconstructionCUDABackend : public GPUReconstructionDeviceBase
{
 public:
  ~GPUReconstructionCUDABackend() override;
  static int GPUFailedMsgAI(const long long int error, const char* file, int line);
  void GPUFailedMsgA(const long long int error, const char* file, int line);

 protected:
  GPUReconstructionCUDABackend(const GPUSettingsDeviceBackend& cfg);

  void PrintKernelOccupancies() override;

  template <class T, int I = 0, typename... Args>
  int runKernelBackend(const krnlSetupArgs<T, I, Args...>& args);
  template <class T, int I = 0, typename... Args>
  void runKernelBackendInternal(const krnlSetupTime& _xyz, const Args&... args);
  template <class T, int I = 0>
  gpu_reconstruction_kernels::krnlProperties getKernelPropertiesBackend();
  template <class T, int I>
  class backendInternal;

  template <bool multi, class T, int I = 0>
  static int getRTCkernelNum(int k = -1);

  void getRTCKernelCalls(std::vector<std::string>& kernels);

  GPUReconstructionCUDAInternals* mInternals;
};

class GPUReconstructionCUDA : public GPUReconstructionKernels<GPUReconstructionCUDABackend>
{
 public:
  ~GPUReconstructionCUDA() override;
  GPUReconstructionCUDA(const GPUSettingsDeviceBackend& cfg);

 protected:
  int InitDevice_Runtime() override;
  int ExitDevice_Runtime() override;
  void UpdateAutomaticProcessingSettings() override;

  std::unique_ptr<GPUThreadContext> GetThreadContext() override;
  void SynchronizeGPU() override;
  int GPUDebug(const char* state = "UNKNOWN", int stream = -1, bool force = false) override;
  void SynchronizeStream(int stream) override;
  void SynchronizeEvents(deviceEvent* evList, int nEvents = 1) override;
  void StreamWaitForEvents(int stream, deviceEvent* evList, int nEvents = 1) override;
  bool IsEventDone(deviceEvent* evList, int nEvents = 1) override;
  int registerMemoryForGPU_internal(const void* ptr, size_t size) override;
  int unregisterMemoryForGPU_internal(const void* ptr) override;

  size_t WriteToConstantMemory(size_t offset, const void* src, size_t size, int stream = -1, deviceEvent* ev = nullptr) override;
  size_t TransferMemoryInternal(GPUMemoryResource* res, int stream, deviceEvent* ev, deviceEvent* evList, int nEvents, bool toGPU, const void* src, void* dst) override;
  size_t GPUMemCpy(void* dst, const void* src, size_t size, int stream, int toGPU, deviceEvent* ev = nullptr, deviceEvent* evList = nullptr, int nEvents = 1) override;
  void ReleaseEvent(deviceEvent ev) override;
  void RecordMarker(deviceEvent ev, int stream) override;

  void GetITSTraits(std::unique_ptr<o2::its::TrackerTraits>* trackerTraits, std::unique_ptr<o2::its::VertexerTraits>* vertexerTraits, std::unique_ptr<o2::its::TimeFrame>* timeFrame) override;

#ifndef __HIPCC__ // CUDA
  bool CanQueryMaxMemory() override { return true; }
  int PrepareTextures() override;
  void startGPUProfiling() override;
  void endGPUProfiling() override;
#else // HIP
  void* getGPUPointer(void* ptr) override;
#endif

 private:
  int genRTC(std::string& filename, unsigned int& nCompile);
  void genAndLoadRTC();
  void loadKernelModules(bool perKernel, bool perSingleMulti = true);
  const char *mRtcSrcExtension = ".src", *mRtcBinExtension = ".o";
};

} // namespace gpu
} // namespace GPUCA_NAMESPACE

#endif
