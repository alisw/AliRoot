// @(#) $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTAgentSample.cxx
    @author Matthias Richter
    @date   
    @brief  Agent of the libAliHLTSample library
*/

#include <cassert>
#include "AliHLTAgentSample.h"
#include "AliHLTConfiguration.h"
#include "TSystem.h"

// header files of library components
#include "AliHLTDummyComponent.h"
#include "AliHLTSampleComponent1.h"
#include "AliHLTSampleComponent2.h"

// header file of the module preprocessor
#include "AliHLTSamplePreprocessor.h"

/** global instance for agent registration */
AliHLTAgentSample gAliHLTAgentSample;

const char* AliHLTAgentSample::fgkAliHLTAgentSampleData="/tmp/testdata";
const char* AliHLTAgentSample::fgkAliHLTAgentSampleOut="/tmp/hltout";

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTAgentSample)

AliHLTAgentSample::AliHLTAgentSample()
  :
  AliHLTModuleAgent("Sample")
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt
}

AliHLTAgentSample::~AliHLTAgentSample()
{
  // see header file for class documentation

  // delete the test data
  ofstream dump(fgkAliHLTAgentSampleData, ios::in);
  if (dump.good()) {
    TString arg("rm -f ");
    arg+=fgkAliHLTAgentSampleData;
    gSystem->Exec(arg.Data());
  }
}

int AliHLTAgentSample::CreateConfigurations(AliHLTConfigurationHandler* handler,
					    AliRawReader* /*rawReader*/,
					    AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation

  // create some test data
  ofstream dump(fgkAliHLTAgentSampleData, (ios::openmode)0);
  dump << "This is just some test data for the ALICE HLT analysis example";
  dump << "---- not copied" << endl;
  dump.close();

  if (handler) {
    // the publisher configuration for the test data
    TString arg("-datatype DUMMYDAT TEST -datafile ");
    arg+=fgkAliHLTAgentSampleData;
    HLTDebug(arg.Data());
    handler->CreateConfiguration("sample-fp1"  , "FilePublisher", NULL , arg.Data());

    // the configuration for the dummy component
    handler->CreateConfiguration("sample-cp"   , "Dummy"        , "sample-fp1", "output_percentage 80");

    // the writer configuration
    arg="-datafile "; arg+=fgkAliHLTAgentSampleOut;
    handler->CreateConfiguration("sample-sink1", "FileWriter"   , "sample-cp" , arg.Data());

    // sample offline source
    handler->CreateConfiguration("sample-offsrc", "AliLoaderPublisher"   , NULL , "-loader TPCLoader -tree digits -verbose");

    // sample offline sink
    handler->CreateConfiguration("sample-offsnk", "SampleOfflineDataSink"   , "sample-offsrc" , NULL);
  }
  return 0;
}

const char* AliHLTAgentSample::GetReconstructionChains(AliRawReader* /*rawReader*/,
						       AliRunLoader* /*runloader*/) const
{
  // see header file for class documentation
  return "sample-sink1 sample-offsnk";
}

const char* AliHLTAgentSample::GetRequiredComponentLibraries() const
{
  // see header file for class documentation
  return "libAliHLTUtil.so libAliHLTSample.so";
}

int AliHLTAgentSample::RegisterComponents(AliHLTComponentHandler* pHandler) const
{
  // see header file for class documentation
  assert(pHandler);
  if (!pHandler) return -EINVAL;
  pHandler->AddComponent(new AliHLTDummyComponent);
  pHandler->AddComponent(new AliHLTSampleComponent1);
  pHandler->AddComponent(new AliHLTSampleComponent2);

  return 0;
}

AliHLTModulePreprocessor* AliHLTAgentSample::GetPreprocessor()
{
  // see header file for class documentation
  return new AliHLTSamplePreprocessor;
}
