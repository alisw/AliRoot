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

/// \file qconfig_helpers.h
/// \author David Rohr

#ifndef QCONFIG_HELPERS_H
#define QCONFIG_HELPERS_H

#include <string>
#include <sstream>

#define qon_mcat(a, b) a##b
#define qon_mxcat(a, b) qon_mcat(a, b)
#define qon_mcat3(a, b, c) a##b##c
#define qon_mxcat3(a, b, c) qon_mcat3(a, b, c)
#define qon_mstr(a) #a
#define qon_mxstr(a) qon_mstr(a)

namespace qConfig
{
template <class T>
inline std::string print_type(T val)
{
  std::ostringstream s;
  s << val;
  return s.str();
};
template <>
inline std::string print_type<char>(char val)
{
  return std::to_string(val);
};
template <>
inline std::string print_type<unsigned char>(unsigned char val)
{
  return std::to_string(val);
};
template <>
inline std::string print_type<bool>(bool val)
{
  return val ? "true" : "false";
};
} // namespace qConfig

#endif
