// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTOfflineDataSource.cxx
    @author Matthias Richter
    @date   
    @brief  AliRoot data source component base class.
*/

#include "AliHLTOfflineDataSource.h"

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTOfflineDataSource)

AliHLTOfflineDataSource::AliHLTOfflineDataSource()
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
  Register(this);
}

AliHLTOfflineDataSource::AliHLTOfflineDataSource(const AliHLTOfflineDataSource&)
  :
  AliHLTDataSource(),
  AliHLTOfflineInterface()
{
  // see header file for class documentation
  HLTFatal("copy constructor untested");
}

AliHLTOfflineDataSource& AliHLTOfflineDataSource::operator=(const AliHLTOfflineDataSource&)
{ 
  // see header file for class documentation
  HLTFatal("assignment operator untested");
  return *this;
}

AliHLTOfflineDataSource::~AliHLTOfflineDataSource()
{
  // see header file for class documentation
  Unregister(this);
}
