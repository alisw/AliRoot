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

/*
$Log$
Revision 1.12  2007/04/27 07:06:48  jgrosseo
GetFileSources returns empty list in case of no files, but successful query
No mails sent in testmode

Revision 1.11  2007/04/04 10:33:36  jgrosseo
1) Storing of files to the Grid is now done _after_ your preprocessors succeeded. This is transparent, which means that you can still use the same functions (Store, StoreReferenceData) to store files to the Grid. However, the Shuttle first stores them locally and transfers them after the preprocessor finished. The return code of these two functions has changed from UInt_t to Bool_t which gives you the success of the storing.
In case of an error with the Grid, the Shuttle will retry the storing later, the preprocessor does not need to be run again.

2) The meaning of the return code of the preprocessor has changed. 0 is now success and any other value means failure. This value is stored in the log and you can use it to keep details about the error condition.

3) New function StoreReferenceFile to _directly_ store a file (without opening it) to the reference storage.

4) The memory usage of the preprocessor is monitored. If it exceeds 2 GB it is terminated.

5) New function AliPreprocessor::ProcessDCS(). If you do not need to have DCS data in all cases, you can skip the processing by implemting this function and returning kFALSE under certain conditions. E.g. if there is a certain run type.
If you always need DCS data (like before), you do not need to implement it.

6) The run type has been added to the monitoring page

Revision 1.10  2007/02/28 10:41:01  acolla
Run type field added in SHUTTLE framework. Run type is read from "run type" logbook and retrieved by
AliPreprocessor::GetRunType() function.
Added some ldap definition files.

Revision 1.8  2007/02/13 11:22:25  acolla
Shuttle getters and setters of main/local OCDB/Reference storages, temp and log
folders moved to AliShuttleInterface

Revision 1.6  2006/11/06 14:22:47  jgrosseo
major update (Alberto)
o) reading of run parameters from the logbook
o) online offline naming conversion
o) standalone DCSclient package

Revision 1.5  2006/10/02 12:58:52  jgrosseo
Small interface change in StoreReferenceData

Revision 1.4  2006/08/08 14:19:07  jgrosseo
Update to shuttle classes (Alberto)

- Possibility to set the full object's path in the Preprocessor's and
Shuttle's  Store functions
- Possibility to extend the object's run validity in the same classes
("startValidity" and "validityInfinite" parameters)
- Implementation of the StoreReferenceData function to store reference
data in a dedicated CDB storage.

Revision 1.3  2006/07/11 12:44:32  jgrosseo
adding parameters for extended validity range of data produced by preprocessor

Revision 1.2  2006/06/06 14:20:05  jgrosseo
o) updated test preprocessor (alberto)
o) added comments to example macro
o) test shuttle implements new interface

Revision 1.2  2006/03/07 07:52:34  hristov
New version (B.Yordanov)

Revision 1.3  2005/11/17 17:47:34  byordano
TList changed to TObjArray

Revision 1.2  2005/11/17 14:43:22  byordano
import to local CVS

Revision 1.1.1.1  2005/10/28 07:33:58  hristov
Initial import as subdirectory in AliRoot

Revision 1.1.1.1  2005/09/12 22:11:40  byordano
SHUTTLE package

Revision 1.2  2005/08/29 21:15:47  byordano
some docs added

*/

//
// test implementation of the AliShuttleInterface, to be used for local tests of preprocessors
//
// reads files from the local disk
// stores to local CDB
// logs to the screen
//

#include "AliTestShuttle.h"
#include "AliLog.h"

#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBMetaData.h"
#include "AliCDBPath.h"
#include "AliCDBId.h"
#include "AliPreprocessor.h"

#include <TMap.h>
#include <TList.h>
#include <TObjString.h>
#include <TSystem.h>

ClassImp(AliTestShuttle)

//______________________________________________________________________________________________
AliTestShuttle::AliTestShuttle(Int_t run, UInt_t startTime, UInt_t endTime) :
  fRun(run),
  fStartTime(startTime),
  fEndTime(endTime),
  fInputFiles(0),
  fRunParameters(0),
  fRunType(),
  fPreprocessors(0),
  fDcsAliasMap(0)
{
  // constructor

  fInputFiles = new TMap;
  fRunParameters = new TMap;
  fPreprocessors = new TObjArray;

  fInputFiles->SetOwner(1);
  fRunParameters->SetOwner(1);
  fPreprocessors->SetOwner(1);
}

//______________________________________________________________________________________________
AliTestShuttle::~AliTestShuttle()
{
  // destructor

  delete fInputFiles;
  fInputFiles = 0;

  delete fRunParameters;
  fRunParameters = 0;

  delete fPreprocessors;
  fPreprocessors = 0;

  delete fDcsAliasMap;
  fDcsAliasMap = 0;
}

//______________________________________________________________________________________________
Bool_t AliTestShuttle::Store(const AliCDBPath& path, TObject* object, AliCDBMetaData* metaData,
				Int_t validityStart, Bool_t validityInfinite)
{
  // Stores the CDB object
  // This function should be called at the end of the preprocessor cycle
  //
  // This implementation just stores it on the local disk, the full AliShuttle
  // puts it to the Grid FileCatalog

  Int_t startRun = fRun - validityStart;
  if(startRun < 0) {
	AliError("First valid run happens to be less than 0! Setting it to 0...");
	startRun=0;
  }

  Int_t endRun = -1;
  if(validityInfinite) {
	endRun = AliCDBRunRange::Infinity();
  } else {
	endRun = fRun;
  }

  AliCDBId id(path, startRun, endRun);

  return AliCDBManager::Instance()->GetStorage(fgkMainCDB)->Put(object, id, metaData);
}

//______________________________________________________________________________________________
Bool_t AliTestShuttle::StoreReferenceData(const AliCDBPath& path, TObject* object, AliCDBMetaData* metaData)
{
  // Stores the object as reference data
  // This function should be called at the end of the preprocessor cycle
  //
  // This implementation just stores it on the local disk, the full AliShuttle
  // puts it to the Grid FileCatalog

  AliCDBId id(path, fRun, fRun);

  return AliCDBManager::Instance()->GetStorage(fgkMainRefStorage)->Put(object, id, metaData);
}

//______________________________________________________________________________________________
Bool_t AliTestShuttle::StoreReferenceFile(const char* detector, const char* localFile, const char* gridFileName)
{
	//
	// Stores reference file directly (without opening it). 
	//
	// This implementation just stores it on the local disk, the full AliShuttle 
	// puts it to the Grid FileCatalog
	
	AliCDBManager* man = AliCDBManager::Instance();
	AliCDBStorage* sto = man->GetStorage(fgkMainRefStorage);
	
	TString localBaseFolder = sto->GetBaseFolder();
	
	TString targetDir;
	targetDir.Form("%s/%s", localBaseFolder.Data(), detector);
	
	TString target;
	target.Form("%s/%d_%s", targetDir.Data(), fRun, gridFileName);
	
	Int_t result = gSystem->GetPathInfo(targetDir, 0, (Long64_t*) 0, 0, 0);
	if (result)
	{
		result = gSystem->mkdir(targetDir, kTRUE);
		if (result != 0)
		{
			Log("SHUTTLE", Form("StoreReferenceFile - Error creating base directory %s", targetDir.Data()));
			return kFALSE;
		}
	}
		
	result = gSystem->CopyFile(localFile, target);

	if (result == 0)
	{
		Log("SHUTTLE", Form("StoreReferenceFile - Stored file %s locally to %s", localFile, target.Data()));
		return kTRUE;
	}
	else
	{
		Log("SHUTTLE", Form("StoreReferenceFile - Storing file %s locally to %s failed with %d", localFile, target.Data(), result));
		return kFALSE;
	}		
}

//______________________________________________________________________________________________
const char* AliTestShuttle::GetFile(Int_t system, const char* detector, const char* id, const char* source)
{
  // This function retrieves a file from the given system (kDAQ, kDCS, kHLT) with the given file id
  // and from the given source in the system.
  // The function returnes the path to the local file.
  //
  // test implementation of GetFile
  // takes files from the local disks, files are passen in a TMap in the constructor

  TString key;
  key.Form("%s-%s-%s", fkSystemNames[system], detector, id);
  TPair* sourceListPair = dynamic_cast<TPair*> (fInputFiles->FindObject(key.Data()));
  TMap* sourceList = 0;
  if (sourceListPair)
    sourceList = dynamic_cast<TMap*> (sourceListPair->Value());
  if (!sourceList)
  {
    AliError(Form("Could not find any file in %s with id %s (%s)", fkSystemNames[system], id, key.Data()));
    return 0;
  }

  TObjString* fileName = 0;
  TPair* fileNamePair = dynamic_cast<TPair*> (sourceList->FindObject(source));
  if (fileNamePair)
  	fileName = dynamic_cast<TObjString*> (fileNamePair->Value());
  if (!fileName)
  {
    AliError(Form("Could not find files from source %s in %s with id %s",
			source, fkSystemNames[system], id));
    return 0;
  }

  return fileName->GetString().Data();
}

//______________________________________________________________________________________________
TList* AliTestShuttle::GetFileSources(Int_t system, const char* detector, const char* id)
{
  // Returns a list of sources in a given system that saved a file with the given id
  //
  // test implementation of GetFileSources
  // takes files from the local disks, files are passen in a TMap in the constructor

  TString key;
  if (id)
    key.Form("%s-%s-%s", fkSystemNames[system], detector, id);
  else
    key.Form("%s-%s", fkSystemNames[system], detector);
  
  TList* list = new TList;
  
  TIterator* iter = fInputFiles->MakeIterator();
  TObject* obj = 0;
  while ((obj = iter->Next()))
  {
	TObjString* objStr = dynamic_cast<TObjString*> (obj);
	if (objStr)
	{
		Bool_t found = kFALSE;
 		if (id)
		{
			found = (objStr->String().CompareTo(key) == 0);
		}
		else
			found = objStr->String().BeginsWith(key);
		
		if (found)
		{
			TPair* sourceListPair = dynamic_cast<TPair*> (fInputFiles->FindObject(objStr->String().Data()));
			TMap* sourceList = dynamic_cast<TMap*> (sourceListPair->Value());
	
			TIterator* iter2 = sourceList->GetTable()->MakeIterator();
			TObject* obj2 = 0;
			while ((obj2 = iter2->Next()))
			{
				TPair* pair = dynamic_cast<TPair*> (obj2);
				if (pair)
				{
					if (!list->FindObject(pair->Key()))
						list->Add(new TObjString(pair->Key()->GetName()));
				}
			}
			
			delete iter2;
		}
	}
  }
  
  if (list->GetEntries() == 0)
    AliInfo(Form("Could not find any file in %s with id %s (%s)", fkSystemNames[system], id, key.Data()));
  
  return list;
}

//______________________________________________________________________________________________
TList* AliTestShuttle::GetFileIDs(Int_t system, const char* detector, const char* source)
{
  // Returns a list of ids in a given system that saved a file with the given source
  //
  // test implementation of GetFileSources
  // takes files from the local disks, files are passen in a TMap in the constructor


  TString key;
  key.Form("%s-%s", fkSystemNames[system], detector);
  
  TList* list = new TList;
  
  TIterator* iter = fInputFiles->MakeIterator();
  TObject* obj = 0;
  while ((obj = iter->Next()))
  {
	TObjString* objStr = dynamic_cast<TObjString*> (obj);
	if (objStr)
	{
		if (objStr->String().BeginsWith(key))
		{
			Bool_t found = kFALSE;
		
			TPair* sourceListPair = dynamic_cast<TPair*> (fInputFiles->FindObject(objStr->String().Data()));
			TMap* sourceList = dynamic_cast<TMap*> (sourceListPair->Value());
	
			TIterator* iter2 = sourceList->GetTable()->MakeIterator();
			TObject* obj2 = 0;
			while ((obj2 = iter2->Next()))
			{
				TPair* pair = dynamic_cast<TPair*> (obj2);
				if (pair)
				{
					if (strcmp(pair->Key()->GetName(), source) == 0)
						found = kTRUE;
				}
			}
			
			delete iter2;
			
			if (found)
			{
				TObjArray* tokens = objStr->String().Tokenize("-");
				if (tokens->GetEntries() == 3)
				{
					TObjString* id = dynamic_cast<TObjString*> (tokens->At(2));
					if (id && !list->FindObject(id->String()))
						list->Add(new TObjString(id->String()));
				}
				
				delete tokens;
	
			}
		}
	}
  }
  
  if (list->GetEntries() == 0)
    AliInfo(Form("Could not find any file in %s with source %s (%s)", fkSystemNames[system], source, key.Data()));
  
  return list;
}

//______________________________________________________________________________________________
void AliTestShuttle::Log(const char* detector, const char* message)
{
  // test implementation of Log
  // just prints to the screen

  AliInfo(Form("%s: %s", detector, message));
}

//______________________________________________________________________________________________
void AliTestShuttle::AddInputFile(Int_t system, const char* detector, const char* id, const char* source, const char* fileName)
{
  //
  // This function adds a file to the list of input files
  // the list is stored in fInputFiles 
  // fInputFiles: TMap (key -> value)
  //    <system>-<detector>-<id> -> TMap (key -> value)
  //                                <source> -> <filename>
  //  
  
  TString key;
  key.Form("%s-%s-%s", fkSystemNames[system], detector, id);
  TPair* sourceListPair = dynamic_cast<TPair*> (fInputFiles->FindObject(key.Data()));
  TMap* sourceList = 0;
  if (sourceListPair)
    sourceList = dynamic_cast<TMap*> (sourceListPair->Value());
  if (!sourceList)
  {
    sourceList = new TMap;
    fInputFiles->Add(new TObjString(key), sourceList);
  }

  sourceList->Add(new TObjString(source), new TObjString(fileName));
}

//______________________________________________________________________________________________
Bool_t AliTestShuttle::AddInputCDBEntry(AliCDBEntry* entry)
{
  // This function adds an object in the OCDB to be later retrieved with GetFromOCDB

	AliCDBStorage *sto = AliCDBManager::Instance()->GetStorage(fgkMainCDB);
	if (!sto)
	{
		Log("SHUTTLE", "GetFromOCDB - Cannot activate main OCDB for query!");
		return 0;
	}

	return sto->Put(entry);
}

//______________________________________________________________________________________________
AliCDBEntry* AliTestShuttle::GetFromOCDB(const char* detector, const AliCDBPath& path)
{
// returns obiect from OCDB valid for current run

	AliCDBStorage *sto = AliCDBManager::Instance()->GetStorage(fgkMainCDB);
	if (!sto)
	{
		Log("SHUTTLE", "GetFromOCDB - Cannot activate main OCDB for query!");
		return 0;
	}

	return (AliCDBEntry*) sto->Get(path, fRun);
}

//______________________________________________________________________________________________
void AliTestShuttle::Process()
{
  // This function tests all preprocessors that are registered to it
  // All preprocessors get the same dcs alias map and have access to the same list of files.

  for (Int_t i=0; i<fPreprocessors->GetEntries(); ++i)
  {
    AliPreprocessor* preprocessor = dynamic_cast<AliPreprocessor*> (fPreprocessors->At(i));
    if (preprocessor)
    {
      preprocessor->Initialize(fRun, fStartTime, fEndTime);
      preprocessor->Process(fDcsAliasMap);
    }
  }
}

//______________________________________________________________________________________________
void AliTestShuttle::RegisterPreprocessor(AliPreprocessor* preprocessor)
{
  // registers a preprocessor

	const char* detName = preprocessor->GetName();
	if(strcmp("DET", detName) != 0) {
		if(GetDetPos(detName) < 0)
			AliFatal(Form("********** !!!!! Invalid detector name: %s !!!!! **********", detName));
	}

  	fPreprocessors->Add(preprocessor);
}

//______________________________________________________________________________________________
void AliTestShuttle::AddInputRunParameter(const char* key, const char* value){
// set a run parameter (in reality it will be read from the DAQ logbook)

	TObjString* keyObj = new TObjString(key);
	if (fRunParameters->Contains(key)) {
		AliWarning(Form("Parameter %s already existing and it will be replaced.", key));
		delete fRunParameters->Remove(keyObj);

	}
	fRunParameters->Add(keyObj, new TObjString(value));
	AliDebug(2, Form("Number of parameters: %d", fRunParameters->
	GetEntries()));
}

//______________________________________________________________________________________________
const char* AliTestShuttle::GetRunType()
{
	//
	// get a run parameter
	//

	return fRunType;
}

//______________________________________________________________________________________________
const char* AliTestShuttle::GetRunParameter(const char* key){
// get a run parameter

	TObjString* value = dynamic_cast<TObjString*> (fRunParameters->GetValue(key));
	if(!value) {
		AliError(Form("No such parameter: %s", key));
		return 0;
	}
	return value->GetName();
}

//______________________________________________________________________________________________
void AliTestShuttle::SetShuttleTempDir(const char* tmpDir)
{
// sets Shuttle temp directory

	fgkShuttleTempDir = gSystem->ExpandPathName(tmpDir);
}

//______________________________________________________________________________________________
void AliTestShuttle::SetShuttleLogDir(const char* logDir)
{
// sets Shuttle log directory

	fgkShuttleLogDir = gSystem->ExpandPathName(logDir);
}

