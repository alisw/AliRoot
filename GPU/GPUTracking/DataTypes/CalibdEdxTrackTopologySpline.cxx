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

/// \file  dEdxCalibrationSplines.cxx
/// \brief Definition of dEdxCalibrationSplines class
///
/// \author  Matthias Kleiner <matthias.kleiner@cern.ch>

#include "CalibdEdxTrackTopologySpline.h"

#if !defined(GPUCA_STANDALONE)
#include "TFile.h"
#endif

using namespace GPUCA_NAMESPACE::gpu;
using namespace o2::tpc;

#if !defined(GPUCA_STANDALONE)
CalibdEdxTrackTopologySpline::CalibdEdxTrackTopologySpline(const char* dEdxSplinesFile, const char* name)
  : FlatObject()
{
  TFile dEdxFile(dEdxSplinesFile);
  setFromFile(dEdxFile, name);
}
#endif

CalibdEdxTrackTopologySpline::CalibdEdxTrackTopologySpline(const CalibdEdxTrackTopologySpline& obj)
  : FlatObject()
{
  /// Copy constructor
  this->cloneFromObject(obj, nullptr);
}

CalibdEdxTrackTopologySpline& CalibdEdxTrackTopologySpline::operator=(const CalibdEdxTrackTopologySpline& obj)
{
  /// Assignment operator
  this->cloneFromObject(obj, nullptr);
  return *this;
}

void CalibdEdxTrackTopologySpline::recreate(const int nKnots[])
{
  /// Default constructor
  FlatObject::startConstruction();

  int buffSize = 0;
  int offsets1[FSplines];
  int offsets2[FSplines];
  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqMax[i].recreate(nKnots);
    buffSize = alignSize(buffSize, mCalibSplinesqMax[i].getBufferAlignmentBytes());
    offsets1[i] = buffSize;
    buffSize += mCalibSplinesqMax[i].getFlatBufferSize();
  }
  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqTot[i].recreate(nKnots);
    buffSize = alignSize(buffSize, mCalibSplinesqTot[i].getBufferAlignmentBytes());
    offsets2[i] = buffSize;
    buffSize += mCalibSplinesqTot[i].getFlatBufferSize();
  }

  FlatObject::finishConstruction(buffSize);

  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqMax[i].moveBufferTo(mFlatBufferPtr + offsets1[i]);
  }
  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqTot[i].moveBufferTo(mFlatBufferPtr + offsets2[i]);
  }
}

void CalibdEdxTrackTopologySpline::cloneFromObject(const CalibdEdxTrackTopologySpline& obj, char* newFlatBufferPtr)
{
  /// See FlatObject for description

  const char* oldFlatBufferPtr = obj.mFlatBufferPtr;
  FlatObject::cloneFromObject(obj, newFlatBufferPtr);

  for (unsigned int i = 0; i < FSplines; i++) {
    char* buffer = FlatObject::relocatePointer(oldFlatBufferPtr, mFlatBufferPtr, obj.mCalibSplinesqMax[i].getFlatBufferPtr());
    mCalibSplinesqMax[i].cloneFromObject(obj.mCalibSplinesqMax[i], buffer);
  }

  for (unsigned int i = 0; i < FSplines; i++) {
    char* buffer = FlatObject::relocatePointer(oldFlatBufferPtr, mFlatBufferPtr, obj.mCalibSplinesqTot[i].getFlatBufferPtr());
    mCalibSplinesqTot[i].cloneFromObject(obj.mCalibSplinesqTot[i], buffer);
  }
  mMaxTanTheta = obj.mMaxTanTheta;
  mMaxSinPhi = obj.mMaxSinPhi;

  for (unsigned int i = 0; i < FSplines; ++i) {
    mScalingFactorsqTot[i] = obj.mScalingFactorsqTot[i];
    mScalingFactorsqMax[i] = obj.mScalingFactorsqMax[i];
  }
}

void CalibdEdxTrackTopologySpline::moveBufferTo(char* newFlatBufferPtr)
{
  /// See FlatObject for description
  char* oldFlatBufferPtr = mFlatBufferPtr;
  FlatObject::moveBufferTo(newFlatBufferPtr);
  char* currFlatBufferPtr = mFlatBufferPtr;
  mFlatBufferPtr = oldFlatBufferPtr;
  setActualBufferAddress(currFlatBufferPtr);
}

void CalibdEdxTrackTopologySpline::destroy()
{
  /// See FlatObject for description
  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqMax[i].destroy();
    mCalibSplinesqTot[i].destroy();
  }
  FlatObject::destroy();
}

void CalibdEdxTrackTopologySpline::setActualBufferAddress(char* actualFlatBufferPtr)
{
  /// See FlatObject for description

  FlatObject::setActualBufferAddress(actualFlatBufferPtr);
  int offset = 0;
  for (unsigned int i = 0; i < FSplines; i++) {
    offset = alignSize(offset, mCalibSplinesqMax[i].getBufferAlignmentBytes());
    mCalibSplinesqMax[i].setActualBufferAddress(mFlatBufferPtr + offset);
    offset += mCalibSplinesqMax[i].getFlatBufferSize();
  }
  for (unsigned int i = 0; i < FSplines; i++) {
    offset = alignSize(offset, mCalibSplinesqTot[i].getBufferAlignmentBytes());
    mCalibSplinesqTot[i].setActualBufferAddress(mFlatBufferPtr + offset);
    offset += mCalibSplinesqTot[i].getFlatBufferSize();
  }
}

void CalibdEdxTrackTopologySpline::setFutureBufferAddress(char* futureFlatBufferPtr)
{
  /// See FlatObject for description

  for (unsigned int i = 0; i < FSplines; i++) {
    char* buffer = relocatePointer(mFlatBufferPtr, futureFlatBufferPtr, mCalibSplinesqMax[i].getFlatBufferPtr());
    mCalibSplinesqMax[i].setFutureBufferAddress(buffer);
  }
  for (unsigned int i = 0; i < FSplines; i++) {
    char* buffer = relocatePointer(mFlatBufferPtr, futureFlatBufferPtr, mCalibSplinesqTot[i].getFlatBufferPtr());
    mCalibSplinesqTot[i].setFutureBufferAddress(buffer);
  }
  FlatObject::setFutureBufferAddress(futureFlatBufferPtr);
}

#if !defined(GPUCA_STANDALONE)

CalibdEdxTrackTopologySpline* CalibdEdxTrackTopologySpline::readFromFile(
  TFile& inpf, const char* name)
{
  /// read a class object from the file
  return FlatObject::readFromFile<CalibdEdxTrackTopologySpline>(inpf, name);
}

void CalibdEdxTrackTopologySpline::setFromFile(TFile& inpf, const char* name)
{
  LOGP(info, "Warnings when reading from file can be ignored");
  o2::tpc::CalibdEdxTrackTopologySpline* cont = readFromFile(inpf, name);
  *this = *cont;
  delete cont;
  LOGP(info, "CalibdEdxTrackTopologySpline sucessfully loaded from file");
}

int CalibdEdxTrackTopologySpline::writeToFile(TFile& outf, const char* name)
{
  /// write a class object to the file
  LOGP(info, "Warnings when writting to file can be ignored");
  return FlatObject::writeToFile(*this, outf, name);
}

void CalibdEdxTrackTopologySpline::setDefaultSplines()
{
  FlatObject::startConstruction();

  int buffSize = 0;
  int offsets1[FSplines];
  int offsets2[FSplines];

  auto defaultF = [&](const double x[], double f[]) {
    f[0] = 1.f;
  };
  double xMin[FDimX]{};
  double xMax[FDimX]{};

  for (int iDimX = 0; iDimX < FDimX; ++iDimX) {
    xMin[iDimX] = 0;
    xMax[iDimX] = 1;
  }

  for (unsigned int ireg = 0; ireg < FSplines; ++ireg) {
    SplineType splineTmpqMax;
    splineTmpqMax.approximateFunction(xMin, xMax, defaultF);
    mCalibSplinesqMax[ireg] = splineTmpqMax;
    buffSize = alignSize(buffSize, mCalibSplinesqMax[ireg].getBufferAlignmentBytes());
    offsets1[ireg] = buffSize;
    buffSize += mCalibSplinesqMax[ireg].getFlatBufferSize();
  }

  for (unsigned int ireg = 0; ireg < FSplines; ++ireg) {
    SplineType splineTmpqTot;
    splineTmpqTot.approximateFunction(xMin, xMax, defaultF);
    mCalibSplinesqTot[ireg] = splineTmpqTot;
    buffSize = alignSize(buffSize, mCalibSplinesqTot[ireg].getBufferAlignmentBytes());
    offsets2[ireg] = buffSize;
    buffSize += mCalibSplinesqTot[ireg].getFlatBufferSize();
  }

  FlatObject::finishConstruction(buffSize);

  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqMax[i].moveBufferTo(mFlatBufferPtr + offsets1[i]);
  }
  for (unsigned int i = 0; i < FSplines; i++) {
    mCalibSplinesqTot[i].moveBufferTo(mFlatBufferPtr + offsets2[i]);
  }
}

inline void CalibdEdxTrackTopologySpline::setRangesFromFile(TFile& inpf)
{
  std::vector<float>* tanThetaMax = nullptr;
  std::vector<float>* sinPhiMax = nullptr;
  inpf.GetObject("tanThetaMax", tanThetaMax);
  inpf.GetObject("sinPhiMax", sinPhiMax);
  if (tanThetaMax) {
    mMaxTanTheta = (*tanThetaMax).front();
    delete tanThetaMax;
  }
  if (sinPhiMax) {
    mMaxSinPhi = (*sinPhiMax).front();
    delete sinPhiMax;
  }
}

std::string CalibdEdxTrackTopologySpline::getSplineName(const int region, const ChargeType charge)
{
  const std::string typeName[2] = {"qMax", "qTot"};
  const std::string polname = fmt::format("spline_{}_region{}", typeName[charge], region).data();
  return polname;
}

#endif
