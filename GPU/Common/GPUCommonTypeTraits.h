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

/// \file GPUCommonTypeTraits.h
/// \author David Rohr

#ifndef GPUCOMMONTYPETRAITS_H
#define GPUCOMMONTYPETRAITS_H

#if !defined(GPUCA_GPUCODE_DEVICE) || defined(__CUDACC__) || defined(__HIPCC__)
#ifndef GPUCA_GPUCODE_GENRTC
#include <type_traits>
#endif
#elif !defined(__OPENCL__) || defined(__OPENCLCPP__)
// We just reimplement some type traits in std for the GPU
namespace std
{
template <bool B, class T, class F>
struct conditional {
  typedef T type;
};
template <class T, class F>
struct conditional<false, T, F> {
  typedef F type;
};
template <bool B, class T, class F>
using contitional_t = typename conditional<B, T, F>::type;
template <class T, class U>
struct is_same {
  static constexpr bool value = false;
};
template <class T>
struct is_same<T, T> {
  static constexpr bool value = true;
};
template <class T, class U>
static constexpr bool is_same_v = is_same<T, U>::value;
template <bool B, class T = void>
struct enable_if {
};
template <class T>
struct enable_if<true, T> {
  typedef T type;
};
template <class T>
struct remove_cv {
  typedef T type;
};
template <class T>
struct remove_cv<const T> {
  typedef T type;
};
template <class T>
struct remove_cv<volatile T> {
  typedef T type;
};
template <class T>
struct remove_cv<const volatile T> {
  typedef T type;
};
template <class T>
struct remove_const {
  typedef T type;
};
template <class T>
struct remove_const<const T> {
  typedef T type;
};
template <class T>
struct remove_volatile {
  typedef T type;
};
template <class T>
struct remove_volatile<volatile T> {
  typedef T type;
};
template <class T>
struct is_pointer_t {
  static constexpr bool value = false;
};
template <class T>
struct is_pointer_t<T*> {
  static constexpr bool value = true;
};
template <class T>
struct is_pointer : is_pointer_t<typename std::remove_cv<T>::type> {
};
} // namespace std
#endif

#endif
