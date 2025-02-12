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

/// \file GPUTrackingLinkDef_Standalone.h
/// \author David Rohr

#ifdef __CLING__

#include "GPUTrackingLinkDef_O2.h"
#include "GPUTrackingLinkDef_O2_DataTypes.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class o2::tpc::ClusterNative + ;
#pragma link C++ class o2::tpc::TrackTPC + ;
#pragma link C++ class o2::track::TrackParametrization < float> + ;
#pragma link C++ class o2::track::TrackParametrizationWithError < float> + ;
#pragma link C++ class o2::dataformats::RangeReference < uint32_t, uint16_t> + ;
#pragma link C++ class o2::tpc::dEdxInfo + ;
#pragma link C++ class o2::track::PID + ;

#endif
