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

/// \file GPUROOTCartesianFwd.h
/// \author David Rohr

#ifndef GPUROOTCARTESIANFWD_H
#define GPUROOTCARTESIANFWD_H

// Standalone forward declarations for Cartesian2D / Cartesian3D / Point2D / Point3D etc.
// To be used on GPU where ROOT is not available.

#include "GPUCommonDef.h"

namespace ROOT
{
namespace Math
{
template <class T, unsigned int D1, unsigned int D2, class R>
class SMatrix;
template <class T, unsigned int D>
class MatRepSym;
template <class T, unsigned int D1, unsigned int D2>
class MatRepStd;
template <class CoordSystem, class Tag>
class PositionVector2D;
template <class CoordSystem, class Tag>
class PositionVector3D;
template <class CoordSystem, class Tag>
class DisplacementVector2D;
template <class CoordSystem, class Tag>
class DisplacementVector3D;
template <class T>
class Cartesian2D;
template <class T>
class Cartesian3D;
class DefaultCoordinateSystemTag;
} // namespace Math
} // namespace ROOT

namespace o2
{
namespace math_utils
{

namespace detail
{
template <typename T, int I>
struct GPUPoint2D;
template <typename T, int I>
struct GPUPoint3D;
} // namespace detail

#if (!defined(GPUCA_STANDALONE) || !defined(DGPUCA_NO_ROOT)) && !defined(GPUCA_GPUCODE) && !defined(GPUCOMMONRTYPES_H_ACTIVE)
template <typename T>
using Point2D = ROOT::Math::PositionVector2D<ROOT::Math::Cartesian2D<T>, ROOT::Math::DefaultCoordinateSystemTag>;
template <typename T>
using Vector2D = ROOT::Math::DisplacementVector2D<ROOT::Math::Cartesian2D<T>, ROOT::Math::DefaultCoordinateSystemTag>;
template <typename T>
using Point3D = ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<T>, ROOT::Math::DefaultCoordinateSystemTag>;
template <typename T>
using Vector3D = ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<T>, ROOT::Math::DefaultCoordinateSystemTag>;
#else
template <typename T>
using Point2D = detail::GPUPoint2D<T, 0>;
template <typename T>
using Vector2D = detail::GPUPoint2D<T, 1>;
template <typename T>
using Point3D = detail::GPUPoint3D<T, 0>;
template <typename T>
using Vector3D = detail::GPUPoint3D<T, 1>;
#endif

} // namespace math_utils
} // namespace o2

#endif
