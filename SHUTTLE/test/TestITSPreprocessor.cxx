/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
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

//
// Example of a Shuttle Preprocessor
//

#include "TestITSPreprocessor.h"

#include "AliCDBMetaData.h"
#include "AliDCSValue.h"
#include "AliLog.h"

#include <TTimeStamp.h>

ClassImp(TestITSPreprocessor)

//________________________________________________________________________________________
TestITSPreprocessor::TestITSPreprocessor():
	AliPreprocessor("ITS",0)
{
// default constructor - Don't use this!

}

//________________________________________________________________________________________
TestITSPreprocessor::TestITSPreprocessor(const char* detector, AliShuttleInterface* shuttle):
	AliPreprocessor(detector,shuttle)
{
// constructor - shuttle must be instantiated!

}

//________________________________________________________________________________________
void TestITSPreprocessor::Initialize(Int_t run, UInt_t startTime,
	UInt_t endTime) 
{
// Initialize preprocessor

	AliInfo(Form("\n\tRun %d \n\tStartTime %s \n\tEndTime %s", run, 
		TTimeStamp(startTime).AsString(),
		TTimeStamp(endTime).AsString()));
}

//________________________________________________________________________________________
UInt_t TestITSPreprocessor::Process(TMap* valueMap)
{
// process data retrieved by the Shuttle

	AliInfo(Form("You're in AliITSPreprocessor::Process!"));

	TIter iter(valueMap);
	TPair* aPair;
	while ((aPair = (TPair*) iter.Next())) {
		aPair->Print();
	}
	AliCDBMetaData metaData;
	metaData.SetComment("This is a test!");

	return Store("Calib", "ITSData", valueMap, &metaData);
}

