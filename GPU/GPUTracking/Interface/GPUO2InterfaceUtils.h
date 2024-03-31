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

/// \file GPUO2Interface.h
/// \author David Rohr

#ifndef GPUO2INTERFACEUTILS_H
#define GPUO2INTERFACEUTILS_H

#include <functional>
#include <memory>

namespace o2
{
struct InteractionRecord;
namespace raw
{
class RawFileWriter;
} // namespace raw
namespace tpc
{
class CalibdEdxContainer;
class Digit;
template <class T>
class CalDet;
} // namespace tpc
} // namespace o2

namespace o2::gpu
{
struct GPUParam;
struct GPUO2InterfaceConfiguration;
struct TPCPadGainCalib;
class GPUO2InterfaceUtils
{
 public:
  static std::unique_ptr<TPCPadGainCalib> getPadGainCalibDefault();
  static std::unique_ptr<TPCPadGainCalib> getPadGainCalib(const o2::tpc::CalDet<float>& in);
  static std::unique_ptr<o2::tpc::CalibdEdxContainer> getCalibdEdxContainerDefault();
  template <class S>
  static void RunZSEncoder(const S& in, std::unique_ptr<unsigned long long int[]>* outBuffer, unsigned int* outSizes, o2::raw::RawFileWriter* raw, const o2::InteractionRecord* ir, int version, bool verify, float threshold = 0.f, bool padding = false, std::function<void(std::vector<o2::tpc::Digit>&)> digitsFilter = nullptr);
  template <class S>
  static void RunZSEncoder(const S& in, std::unique_ptr<unsigned long long int[]>* outBuffer, unsigned int* outSizes, o2::raw::RawFileWriter* raw, const o2::InteractionRecord* ir, GPUO2InterfaceConfiguration& config, int version, bool verify, bool padding = false, std::function<void(std::vector<o2::tpc::Digit>&)> digitsFilter = nullptr);
  template <class T>
  static float getNominalGPUBz(T& src)
  {
    return (5.00668f / 30000.f) * src.getL3Current();
  }

  class GPUReconstructionZSDecoder
  {
   public:
    void DecodePage(std::vector<o2::tpc::Digit>& outputBuffer, const void* page, unsigned int tfFirstOrbit, const GPUParam* param, unsigned int triggerBC = 0);

   private:
    std::vector<std::function<void(std::vector<o2::tpc::Digit>&, const void*, unsigned int, unsigned int)>> mDecoders;
  };
};

} // namespace o2::gpu

#endif
