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

/// \file AliHLT3DTrackParam.h
/// \author Sergey Gorbunov

#ifndef ALIHLT3DTRACKPARAM_H
#define ALIHLT3DTRACKPARAM_H

#include "TObject.h"

/**
 * @class AliHLT3DTrackParam
 */
class AliHLT3DTrackParam : public TObject
{
 public:
  //*
  //*  INITIALIZATION
  //*

  //* Constructor

  AliHLT3DTrackParam() : fChi2(0), fNDF(0), fSignQ(0) {}

  //* Destructor (empty)

  virtual ~AliHLT3DTrackParam() {}

  //*
  //*  ACCESSORS
  //*

  //* Simple accessors

  double GetX() const { return mParam[0]; }
  double GetY() const { return mParam[1]; }
  double GetZ() const { return mParam[2]; }
  double GetPx() const { return mParam[3]; }
  double GetPy() const { return mParam[4]; }
  double GetPz() const { return mParam[5]; }
  double GetChi2() const { return fChi2; }
  int32_t GetNDF() const { return fNDF; }
  int32_t GetCharge() const { return fSignQ; }

  double GetParameter(int32_t i) const { return mParam[i]; }
  double GetCovariance(int32_t i) const { return fCov[i]; }
  double GetCovariance(int32_t i, int32_t j) const { return fCov[(j <= i) ? i * (i + 1) / 2 + j : j * (j + 1) / 2 + i]; }

  //*
  //* Accessors
  //*

  const double* Param() const { return mParam; }
  const double* Cov() const { return fCov; }
  double X() const { return mParam[0]; }
  double Y() const { return mParam[1]; }
  double Z() const { return mParam[2]; }
  double Px() const { return mParam[3]; }
  double Py() const { return mParam[4]; }
  double Pz() const { return mParam[5]; }
  double Chi2() const { return fChi2; }
  int32_t NDF() const { return fNDF; }
  int32_t Charge() const { return fSignQ; }

  //* Accessors with calculations( &value, &estimated sigma )
  //* error flag returned (0 means no error during calculations)

  //*
  //*  MODIFIERS
  //*

  void SetParam(int32_t i, double v) { mParam[i] = v; }
  void SetCov(int32_t i, double v) { fCov[i] = v; }
  void SetX(double v) { mParam[0] = v; }
  void SetY(double v) { mParam[1] = v; }
  void SetZ(double v) { mParam[2] = v; }
  void SetPx(double v) { mParam[3] = v; }
  void SetPy(double v) { mParam[4] = v; }
  void SetPz(double v) { mParam[5] = v; }
  void SetChi2(double v) { fChi2 = v; }
  void SetNDF(int32_t v) { fNDF = v; }
  void SetCharge(int32_t v) { fSignQ = v; }

  //*
  //*  UTILITIES
  //*

  //* Transport utilities

  double GetDStoPoint(double Bz, const double xyz[3], const double* T0 = 0) const;

  void TransportToDS(double Bz, double DS, double* T0 = 0);

  void TransportToPoint(double Bz, const double xyz[3], double* T0 = 0) { TransportToDS(Bz, GetDStoPoint(Bz, xyz, T0), T0); }

  void TransportToPoint(double Bz, double x, double y, double z, const double* T0 = 0)
  {
    double xyz[3] = {x, y, z};
    TransportToPoint(Bz, xyz, T0);
  }

  //* Fit utilities

  void InitializeCovarianceMatrix();

  void GetGlueMatrix(const double p[3], double G[6], const double* T0 = 0) const;

  void Filter(const double m[3], const double V[6], const double G[6]);

  //* Other utilities

  void SetDirection(double Direction[3]);

  void RotateCoordinateSystem(double alpha);

  void Get5Parameters(double alpha, double T[6], double C[15]) const;

 protected:
  double mParam[6]; // Parameters ( x, y, z, px, py, pz ): 3-position and 3-momentum
  double fCov[21];  // Covariance matrix
  double fChi2;     // Chi^2
  int32_t fNDF;     // Number of Degrees of Freedom
  int32_t fSignQ;   // Charge

  ClassDef(AliHLT3DTrackParam, 1);
};

#endif
