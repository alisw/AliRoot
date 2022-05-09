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

/// \file qGetLdBinarySymbols.h
/// \author David Rohr

#ifndef QGETLDBINARYSYMBOLS_H
#define QGETLDBINARYSYMBOLS_H

#define QGET_LD_BINARY_SYMBOLS(filename)        \
  extern "C" char _binary_##filename##_start[]; \
  extern "C" char _binary_##filename##_end[];   \
  static size_t _binary_##filename##_len = _binary_##filename##_end - _binary_##filename##_start;

#endif // QGETLDBINARYSYMBOLS_H
