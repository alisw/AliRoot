/**************************************************************************
 * Copyright(c) 2006, ALICE Experiment at CERN, All rights reserved.      *
 *                                                                        *
 * Author: Per Thomas Hille for the ALICE HLT Project.                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

#include "AliHLTPHOSRawAnalyzerPeakFinderComponent.h"
#include "AliHLTPHOSRawAnalyzerPeakFinder.h"

//ClassImp(AliHLTPHOSRawAnalyzerPeakFinderComponent) 
AliHLTPHOSRawAnalyzerPeakFinderComponent gAliHLTPHOSRawAnalyzerPeakFinderComponent;

AliHLTPHOSRawAnalyzerPeakFinderComponent::AliHLTPHOSRawAnalyzerPeakFinderComponent():AliHLTPHOSRawAnalyzerComponent()
{
  analyzerPtr = new AliHLTPHOSRawAnalyzerPeakFinder();
} 

AliHLTPHOSRawAnalyzerPeakFinderComponent::~AliHLTPHOSRawAnalyzerPeakFinderComponent()
{

}



AliHLTPHOSRawAnalyzerPeakFinderComponent::AliHLTPHOSRawAnalyzerPeakFinderComponent(const AliHLTPHOSRawAnalyzerPeakFinderComponent & ) : AliHLTPHOSRawAnalyzerComponent()
{

}


const char* 
AliHLTPHOSRawAnalyzerPeakFinderComponent::GetComponentID()
{
  return "PhosRawPeakFinder";
}


AliHLTComponent*
AliHLTPHOSRawAnalyzerPeakFinderComponent::Spawn()
{
  return new AliHLTPHOSRawAnalyzerPeakFinderComponent;
}
