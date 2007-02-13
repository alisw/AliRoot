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
Revision 1.27  2007/01/30 17:52:42  jgrosseo
adding monalisa monitoring

Revision 1.26  2007/01/23 19:20:03  acolla
Removed old ldif files, added TOF, MCH ldif files. Added some options in
AliShuttleConfig::Print. Added in Ali Shuttle: SetShuttleTempDir and
SetShuttleLogDir

Revision 1.25  2007/01/15 19:13:52  acolla
Moved some AliInfo to AliDebug in SendMail function

Revision 1.21  2006/12/07 08:51:26  jgrosseo
update (alberto):
table, db names in ldap configuration
added GRP preprocessor
DCS data can also be retrieved by data point

Revision 1.20  2006/11/16 16:16:48  jgrosseo
introducing strict run ordering flag
removed giving preprocessor name to preprocessor, they have to know their name themselves ;-)

Revision 1.19  2006/11/06 14:23:04  jgrosseo
major update (Alberto)
o) reading of run parameters from the logbook
o) online offline naming conversion
o) standalone DCSclient package

Revision 1.18  2006/10/20 15:22:59  jgrosseo
o) Adding time out to the execution of the preprocessors: The Shuttle forks and the parent process monitors the child
o) Merging Collect, CollectAll, CollectNew function
o) Removing implementation of empty copy constructors (declaration still there!)

Revision 1.17  2006/10/05 16:20:55  jgrosseo
adapting to new CDB classes

Revision 1.16  2006/10/05 15:46:26  jgrosseo
applying to the new interface

Revision 1.15  2006/10/02 16:38:39  jgrosseo
update (alberto):
fixed memory leaks
storing of objects that failed to be stored to the grid before
interfacing of shuttle status table in daq system

Revision 1.14  2006/08/29 09:16:05  jgrosseo
small update

Revision 1.13  2006/08/15 10:50:00  jgrosseo
effc++ corrections (alberto)

Revision 1.12  2006/08/08 14:19:29  jgrosseo
Update to shuttle classes (Alberto)

- Possibility to set the full object's path in the Preprocessor's and
Shuttle's  Store functions
- Possibility to extend the object's run validity in the same classes
("startValidity" and "validityInfinite" parameters)
- Implementation of the StoreReferenceData function to store reference
data in a dedicated CDB storage.

Revision 1.11  2006/07/21 07:37:20  jgrosseo
last run is stored after each run

Revision 1.10  2006/07/20 09:54:40  jgrosseo
introducing status management: The processing per subdetector is divided into several steps,
after each step the status is stored on disk. If the system crashes in any of the steps the Shuttle
can keep track of the number of failures and skips further processing after a certain threshold is
exceeded. These thresholds can be configured in LDAP.

Revision 1.9  2006/07/19 10:09:55  jgrosseo
new configuration, accesst to DAQ FES (Alberto)

Revision 1.8  2006/07/11 12:44:36  jgrosseo
adding parameters for extended validity range of data produced by preprocessor

Revision 1.7  2006/07/10 14:37:09  jgrosseo
small fix + todo comment

Revision 1.6  2006/07/10 13:01:41  jgrosseo
enhanced storing of last sucessfully processed run (alberto)

Revision 1.5  2006/07/04 14:59:57  jgrosseo
revision of AliDCSValue: Removed wrapper classes, reduced storage size per value by factor 2

Revision 1.4  2006/06/12 09:11:16  jgrosseo
coding conventions (Alberto)

Revision 1.3  2006/06/06 14:26:40  jgrosseo
o) removed files that were moved to STEER
o) shuttle updated to follow the new interface (Alberto)

Revision 1.2  2006/03/07 07:52:34  hristov
New version (B.Yordanov)

Revision 1.6  2005/11/19 17:19:14  byordano
RetrieveDATEEntries and RetrieveConditionsData added

Revision 1.5  2005/11/19 11:09:27  byordano
AliShuttle declaration added

Revision 1.4  2005/11/17 17:47:34  byordano
TList changed to TObjArray

Revision 1.3  2005/11/17 14:43:23  byordano
import to local CVS

Revision 1.1.1.1  2005/10/28 07:33:58  hristov
Initial import as subdirectory in AliRoot

Revision 1.2  2005/09/13 08:41:15  byordano
default startTime endTime added

Revision 1.4  2005/08/30 09:13:02  byordano
some docs added

Revision 1.3  2005/08/29 21:15:47  byordano
some docs added

*/

//
// This class is the main manager for AliShuttle. 
// It organizes the data retrieval from DCS and call the 
// interface methods of AliPreprocessor.
// For every detector in AliShuttleConfgi (see AliShuttleConfig),
// data for its set of aliases is retrieved. If there is registered
// AliPreprocessor for this detector then it will be used
// accroding to the schema (see AliPreprocessor).
// If there isn't registered AliPreprocessor than the retrieved
// data is stored automatically to the undelying AliCDBStorage.
// For detSpec is used the alias name.
//

#include "AliShuttle.h"

#include "AliCDBManager.h"
#include "AliCDBStorage.h"
#include "AliCDBId.h"
#include "AliCDBRunRange.h"
#include "AliCDBPath.h"
#include "AliCDBEntry.h"
#include "AliShuttleConfig.h"
#include "DCSClient/AliDCSClient.h"
#include "AliLog.h"
#include "AliPreprocessor.h"
#include "AliShuttleStatus.h"
#include "AliShuttleLogbookEntry.h"

#include <TSystem.h>
#include <TObject.h>
#include <TString.h>
#include <TTimeStamp.h>
#include <TObjString.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TMutex.h>

#include <TMonaLisaWriter.h>

#include <fstream>

#include <sys/types.h>
#include <sys/wait.h>

ClassImp(AliShuttle)

Bool_t AliShuttle::fgkProcessDCS(kTRUE);

//______________________________________________________________________________________________
AliShuttle::AliShuttle(const AliShuttleConfig* config,
		UInt_t timeout, Int_t retries):
fConfig(config),
fTimeout(timeout), fRetries(retries),
fPreprocessorMap(),
fLogbookEntry(0),
fCurrentDetector(),
fStatusEntry(0),
fGridError(kFALSE),
fMonitoringMutex(0),
fLastActionTime(0),
fLastAction(),
fMonaLisa(0)
{
	//
	// config: AliShuttleConfig used
	// timeout: timeout used for AliDCSClient connection
	// retries: the number of retries in case of connection error.
	//

	if (!fConfig->IsValid()) AliFatal("********** !!!!! Invalid configuration !!!!! **********");
	for(int iSys=0;iSys<4;iSys++) {
		fServer[iSys]=0;
		if (iSys < 3)
			fFXSlist[iSys].SetOwner(kTRUE);
	}
	fPreprocessorMap.SetOwner(kTRUE);

	for (UInt_t iDet=0; iDet<NDetectors(); iDet++)
		fFirstUnprocessed[iDet] = kFALSE;

	fMonitoringMutex = new TMutex();
}

//______________________________________________________________________________________________
AliShuttle::~AliShuttle()
{
// destructor

	fPreprocessorMap.DeleteAll();
	for(int iSys=0;iSys<4;iSys++)
		if(fServer[iSys]) {
			fServer[iSys]->Close();
			delete fServer[iSys];
		        fServer[iSys] = 0;
		}

	if (fStatusEntry){
		delete fStatusEntry;
		fStatusEntry = 0;
	}
	
	if (fMonitoringMutex) 
	{
		delete fMonitoringMutex;
		fMonitoringMutex = 0;
	}
}

//______________________________________________________________________________________________
void AliShuttle::RegisterPreprocessor(AliPreprocessor* preprocessor)
{
	//
	// Registers new AliPreprocessor.
	// It uses GetName() for indentificator of the pre processor.
	// The pre processor is registered it there isn't any other
	// with the same identificator (GetName()).
	//

	const char* detName = preprocessor->GetName();
	if(GetDetPos(detName) < 0)
		AliFatal(Form("********** !!!!! Invalid detector name: %s !!!!! **********", detName));

	if (fPreprocessorMap.GetValue(detName)) {
		AliWarning(Form("AliPreprocessor %s is already registered!", detName));
		return;
	}

	fPreprocessorMap.Add(new TObjString(detName), preprocessor);
}
//______________________________________________________________________________________________
UInt_t AliShuttle::Store(const AliCDBPath& path, TObject* object,
		AliCDBMetaData* metaData, Int_t validityStart, Bool_t validityInfinite)
{
  // Stores a CDB object in the storage for offline reconstruction. Objects that are not needed for
  // offline reconstruction, but should be stored anyway (e.g. for debugging) should NOT be stored
  // using this function. Use StoreReferenceData instead!
  // It calls WriteToCDB function which perform actual storage

	return WriteToCDB(fgkMainCDB, fgkLocalCDB, path, object,
				metaData, validityStart, validityInfinite);

}

//______________________________________________________________________________________________
UInt_t AliShuttle::StoreReferenceData(const AliCDBPath& path, TObject* object, AliCDBMetaData* metaData)
{
  // Stores a CDB object in the storage for reference data. This objects will not be available during
  // offline reconstrunction. Use this function for reference data only!
  // It calls WriteToCDB function which perform actual storage

	return WriteToCDB(fgkMainRefStorage, fgkLocalRefStorage, path, object, metaData);

}

//______________________________________________________________________________________________
UInt_t AliShuttle::WriteToCDB(const char* mainUri, const char* localUri,
			const AliCDBPath& path, TObject* object, AliCDBMetaData* metaData,
			Int_t validityStart, Bool_t validityInfinite)
{
  // write object into the CDB. Parameters are passed by Store and StoreReferenceData functions.
  // The parameters are:
  //   1) Uri of the main storage (Grid)
  //   2) Uri of the backup storage (Local)
  //   3) the object's path.
  //   4) the object to be stored
  //   5) the metaData to be associated with the object
  //   6) the validity start run number w.r.t. the current run,
  //      if the data is valid only for this run leave the default 0
  //   7) specifies if the calibration data is valid for infinity (this means until updated),
  //      typical for calibration runs, the default is kFALSE
  //
  // returns 0 if fail
  // 	     1 if stored in main (Grid) storage
  // 	     2 if stored in backup (Local) storage

	const char* cdbType = (mainUri == fgkMainCDB) ? "CDB" : "Reference";

	Int_t firstRun = GetCurrentRun() - validityStart;
  	if(firstRun < 0) {
		AliError("First valid run happens to be less than 0! Setting it to 0.");
		firstRun=0;
  	}

	Int_t lastRun = -1;
	if(validityInfinite) {
		lastRun = AliCDBRunRange::Infinity();
	} else {
		lastRun = GetCurrentRun();
	}

	AliCDBId id(path, firstRun, lastRun, -1, -1);

	if(! dynamic_cast<TObjString*> (metaData->GetProperty("RunUsed(TObjString)"))){
		TObjString runUsed = Form("%d", GetCurrentRun());
		metaData->SetProperty("RunUsed(TObjString)", runUsed.Clone());
	}

	UInt_t result = 0;

	if (!(AliCDBManager::Instance()->GetStorage(mainUri))) {
		AliError(Form("WriteToCDB - Cannot activate main %s storage", cdbType));
	} else {
		result = (UInt_t) AliCDBManager::Instance()->GetStorage(mainUri)
					->Put(object, id, metaData);
	}

	if(!result) {

		Log(fCurrentDetector,
			Form("WriteToCDB - Problem with main %s storage. Putting <%s> into backup storage",
				cdbType, path.GetPath().Data()));

		// Set Grid version to current run number, to ease retrieval later
		id.SetVersion(GetCurrentRun());

		result = AliCDBManager::Instance()->GetStorage(localUri)
					->Put(object, id, metaData);

		if(result) {
			result = 2;
      			fGridError = kTRUE;
		}else{
			Log(fCurrentDetector, "WriteToCDB - Can't store data!");
		}
	}

	return result;

}

//______________________________________________________________________________________________
AliShuttleStatus* AliShuttle::ReadShuttleStatus()
{
// Reads the AliShuttleStatus from the CDB

	if (fStatusEntry){
		delete fStatusEntry;
		fStatusEntry = 0;
	}

	fStatusEntry = AliCDBManager::Instance()->GetStorage(GetLocalCDB())
		->Get(Form("/SHUTTLE/STATUS/%s", fCurrentDetector.Data()), GetCurrentRun());

	if (!fStatusEntry) return 0;
	fStatusEntry->SetOwner(1);

	AliShuttleStatus* status = dynamic_cast<AliShuttleStatus*> (fStatusEntry->GetObject());
	if (!status) {
		AliError("Invalid object stored to CDB!");
		return 0;
	}

	return status;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::WriteShuttleStatus(AliShuttleStatus* status)
{
// writes the status for one subdetector

	if (fStatusEntry){
		delete fStatusEntry;
		fStatusEntry = 0;
	}

	Int_t run = GetCurrentRun();

	AliCDBId id(AliCDBPath("SHUTTLE", "STATUS", fCurrentDetector), run, run);

	fStatusEntry = new AliCDBEntry(status, id, new AliCDBMetaData);
	fStatusEntry->SetOwner(1);

	UInt_t result = AliCDBManager::Instance()->GetStorage(fgkLocalCDB)->Put(fStatusEntry);

	if (!result) {
		AliError(Form("WriteShuttleStatus for %s, run %d failed", fCurrentDetector.Data(), run));
		return kFALSE;
	}
	
	SendMLInfo();

	return kTRUE;
}

//______________________________________________________________________________________________
void AliShuttle::UpdateShuttleStatus(AliShuttleStatus::Status newStatus, Bool_t increaseCount)
{
  // changes the AliShuttleStatus for the given detector and run to the given status

	if (!fStatusEntry){
		AliError("UNEXPECTED: fStatusEntry empty");
		return;
	}

	AliShuttleStatus* status = dynamic_cast<AliShuttleStatus*> (fStatusEntry->GetObject());

	if (!status){
		AliError("UNEXPECTED: status could not be read from current CDB entry");
		return;
	}

	TString actionStr = Form("UpdateShuttleStatus - %s: Changing state from %s to %s",
				fCurrentDetector.Data(),
				status->GetStatusName(),
				status->GetStatusName(newStatus));
	Log("SHUTTLE", actionStr);
	SetLastAction(actionStr);

	status->SetStatus(newStatus);
	if (increaseCount) status->IncreaseCount();

	AliCDBManager::Instance()->GetStorage(fgkLocalCDB)->Put(fStatusEntry);

	SendMLInfo();
}

//______________________________________________________________________________________________
void AliShuttle::SendMLInfo()
{
	//
	// sends ML information about the current status of the current detector being processed
	//
	
	AliShuttleStatus* status = dynamic_cast<AliShuttleStatus*> (fStatusEntry->GetObject());
	
	if (!status){
		AliError("UNEXPECTED: status could not be read from current CDB entry");
		return;
	}
	
	TMonaLisaText  mlStatus(Form("%s_status", fCurrentDetector.Data()), status->GetStatusName());
	TMonaLisaValue mlRetryCount(Form("%s_count", fCurrentDetector.Data()), status->GetCount());

	TList mlList;
	mlList.Add(&mlStatus);
	mlList.Add(&mlRetryCount);

	fMonaLisa->SendParameters(&mlList);
}

//______________________________________________________________________________________________
Bool_t AliShuttle::ContinueProcessing()
{
// this function reads the AliShuttleStatus information from CDB and
// checks if the processing should be continued
// if yes it returns kTRUE and updates the AliShuttleStatus with nextStatus

	if (!fConfig->HostProcessDetector(fCurrentDetector)) return kFALSE;

	AliPreprocessor* aPreprocessor =
		dynamic_cast<AliPreprocessor*> (fPreprocessorMap.GetValue(fCurrentDetector));
	if (!aPreprocessor)
	{
		AliInfo(Form("%s: no preprocessor registered", fCurrentDetector.Data()));
		return kFALSE;
	}

	AliShuttleLogbookEntry::Status entryStatus =
		fLogbookEntry->GetDetectorStatus(fCurrentDetector);

	if(entryStatus != AliShuttleLogbookEntry::kUnprocessed) {
		AliInfo(Form("ContinueProcessing - %s is %s",
				fCurrentDetector.Data(),
				fLogbookEntry->GetDetectorStatusName(entryStatus)));
		return kFALSE;
	}

	// if we get here, according to Shuttle logbook subdetector is in UNPROCESSED state

	// check if current run is first unprocessed run for current detector
	if (fConfig->StrictRunOrder(fCurrentDetector) &&
		!fFirstUnprocessed[GetDetPos(fCurrentDetector)])
	{
		Log("SHUTTLE", Form("ContinueProcessing - %s requires strict run ordering but this is not the first unprocessed run!"));
		return kFALSE;
	}

	AliShuttleStatus* status = ReadShuttleStatus();
	if (!status) {
		// first time
		Log("SHUTTLE", Form("ContinueProcessing - %s: Processing first time",
				fCurrentDetector.Data()));
		status = new AliShuttleStatus(AliShuttleStatus::kStarted);
		return WriteShuttleStatus(status);
	}

	// The following two cases shouldn't happen if Shuttle Logbook was correctly updated.
	// If it happens it may mean Logbook updating failed... let's do it now!
	if (status->GetStatus() == AliShuttleStatus::kDone ||
	    status->GetStatus() == AliShuttleStatus::kFailed){
		Log("SHUTTLE", Form("ContinueProcessing - %s is already %s. Updating Shuttle Logbook",
					fCurrentDetector.Data(),
					status->GetStatusName(status->GetStatus())));
		UpdateShuttleLogbook(fCurrentDetector.Data(),
					status->GetStatusName(status->GetStatus()));
		return kFALSE;
	}

	if (status->GetStatus() == AliShuttleStatus::kStoreFailed) {
		Log("SHUTTLE",
			Form("ContinueProcessing - %s: Grid storage of one or more objects failed. Trying again now",
				fCurrentDetector.Data()));
		if(TryToStoreAgain()){
			Log(fCurrentDetector.Data(), "ContinueProcessing - All objects successfully stored into OCDB");
			UpdateShuttleStatus(AliShuttleStatus::kDone);
			UpdateShuttleLogbook(fCurrentDetector.Data(), "DONE");
		} else {
			Log("SHUTTLE",
				Form("ContinueProcessing - %s: Grid storage failed again",
					fCurrentDetector.Data()));
			// trigger ML information manually because we do not had a status change
			SendMLInfo();
		}
		return kFALSE;
	}

	// if we get here, there is a restart
	Bool_t cont = kFALSE;

	// abort conditions
	if (status->GetCount() >= fConfig->GetMaxRetries()) {
		Log("SHUTTLE", Form("ContinueProcessing - %s failed %d times in status %s - "
				"Updating Shuttle Logbook", fCurrentDetector.Data(),
				status->GetCount(), status->GetStatusName()));
		UpdateShuttleLogbook(fCurrentDetector.Data(), "FAILED");
		UpdateShuttleStatus(AliShuttleStatus::kFailed);
	} else {
		Log("SHUTTLE", Form("ContinueProcessing - %s: restarting. "
				"Aborted before with %s. Retry number %d.", fCurrentDetector.Data(),
				status->GetStatusName(), status->GetCount()));
		UpdateShuttleStatus(AliShuttleStatus::kStarted, kTRUE);
		cont = kTRUE;
	}

	// Send mail to detector expert!
	AliInfo(Form("Sending mail to %s expert...", fCurrentDetector.Data()));
	if (!SendMail())
		Log("SHUTTLE", Form("ContinueProcessing - Could not send mail to %s expert",
				fCurrentDetector.Data()));

	return cont;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::Process(AliShuttleLogbookEntry* entry)
{
	//
	// Makes data retrieval for all detectors in the configuration.
	// entry: Shuttle logbook entry, contains run paramenters and status of detectors
	// (Unprocessed, Inactive, Failed or Done).
	// Returns kFALSE in case of error occured and kTRUE otherwise
	//

	if(!entry) return kFALSE;

	fLogbookEntry = entry;

	if (fLogbookEntry->IsDone())
	{
		Log("SHUTTLE","Process - Shuttle is already DONE. Updating logbook");
		UpdateShuttleLogbook("shuttle_done");
		fLogbookEntry = 0;
		return kTRUE;
	}

	// create ML instance that monitors this run
	fMonaLisa = new TMonaLisaWriter(Form("%d", GetCurrentRun()), "SHUTTLE", "aliendb1.cern.ch");
	// disable monitoring of other parameters that come e.g. from TFile
	gMonitoringWriter = 0;

	AliInfo(Form("\n\n \t\t\t^*^*^*^*^*^*^*^*^*^*^*^* run %d: START ^*^*^*^*^*^*^*^*^*^*^*^* \n",
					GetCurrentRun()));


	// Send the information to ML
	TMonaLisaText  mlStatus("SHUTTLE_status", "Processing");

	TList mlList;
	mlList.Add(&mlStatus);

	fMonaLisa->SendParameters(&mlList);
			
	fLogbookEntry->Print("all");

	// Initialization
	Bool_t hasError = kFALSE;

	AliCDBStorage *mainCDBSto = AliCDBManager::Instance()->GetStorage(fgkMainCDB);
	if(mainCDBSto) mainCDBSto->QueryCDB(GetCurrentRun());
	AliCDBStorage *mainRefSto = AliCDBManager::Instance()->GetStorage(fgkMainRefStorage);
	if(mainRefSto) mainRefSto->QueryCDB(GetCurrentRun());

	// Loop on detectors in the configuration
	TIter iter(fConfig->GetDetectors());
	TObjString* aDetector = 0;

	while ((aDetector = (TObjString*) iter.Next()))
	{
		fCurrentDetector = aDetector->String();

		if (ContinueProcessing() == kFALSE) continue;

		AliInfo(Form("\n\n \t\t\t****** run %d - %s: START  ******",
						GetCurrentRun(), aDetector->GetName()));

		for(Int_t iSys=0;iSys<3;iSys++) fFXSCalled[iSys]=kFALSE;

		Log(fCurrentDetector.Data(), "Starting processing");

		Int_t pid = fork();

		if (pid < 0)
		{
			Log("SHUTTLE", "ERROR: Forking failed");
		}
		else if (pid > 0)
		{
			// parent
			AliInfo(Form("In parent process of %d - %s: Starting monitoring",
							GetCurrentRun(), aDetector->GetName()));

			Long_t begin = time(0);

			int status; // to be used with waitpid, on purpose an int (not Int_t)!
			while (waitpid(pid, &status, WNOHANG) == 0)
			{
				Long_t expiredTime = time(0) - begin;

				if (expiredTime > fConfig->GetPPTimeOut())
				{
					Log("SHUTTLE", Form("Process time out. Run time: %d seconds. Killing...",
								expiredTime));

					kill(pid, 9);

					hasError = kTRUE;

					gSystem->Sleep(1000);
				}
				else
				{
					if (expiredTime % 60 == 0)
					Log("SHUTTLE", Form("Checked process. Run time: %d seconds.",
								expiredTime));
					gSystem->Sleep(1000);
				}
			}

			AliInfo(Form("In parent process of %d - %s: Client has terminated.",
								GetCurrentRun(), aDetector->GetName()));

			if (WIFEXITED(status))
			{
				Int_t returnCode = WEXITSTATUS(status);

				Log("SHUTTLE", Form("The return code is %d", returnCode));

				if (returnCode != 0)
				hasError = kTRUE;
			}
		}
		else if (pid == 0)
		{
			// client
			AliInfo(Form("In client process of %d - %s", GetCurrentRun(), aDetector->GetName()));

			UInt_t result = ProcessCurrentDetector();

			Int_t returnCode = 0; // will be set to 1 in case of an error

			if (!result)
			{
				returnCode = 1;
				AliInfo(Form("\n \t\t\t****** run %d - %s: PREPROCESSOR ERROR ****** \n\n",
							GetCurrentRun(), aDetector->GetName()));
			}
			else if (result == 2)
			{
				AliInfo(Form("\n \t\t\t****** run %d - %s: STORAGE ERROR ****** \n\n",
							GetCurrentRun(), aDetector->GetName()));
			} else
			{
				AliInfo(Form("\n \t\t\t****** run %d - %s: DONE ****** \n\n",
							GetCurrentRun(), aDetector->GetName()));
			}

			if (result > 0)
			{
				// Process successful: Update time_processed field in FXS logbooks!
				if (UpdateTable() == kFALSE) returnCode = 1;
			}

			for (UInt_t iSys=0; iSys<3; iSys++)
			{
				if (fFXSCalled[iSys]) fFXSlist[iSys].Clear();
			}

			AliInfo(Form("Client process of %d - %s is exiting now with %d.",
							GetCurrentRun(), aDetector->GetName(), returnCode));

			// the client exits here
			gSystem->Exit(returnCode);

			AliError("We should never get here!!!");
		}
	}

	AliInfo(Form("\n\n \t\t\t^*^*^*^*^*^*^*^*^*^*^*^* run %d: FINISH ^*^*^*^*^*^*^*^*^*^*^*^* \n",
							GetCurrentRun()));

	//check if shuttle is done for this run, if so update logbook
	TObjArray checkEntryArray;
	checkEntryArray.SetOwner(1);
	TString whereClause = Form("where run=%d", GetCurrentRun());
	if (!QueryShuttleLogbook(whereClause.Data(), checkEntryArray) || checkEntryArray.GetEntries() == 0) {
		Log("SHUTTLE", Form("Process - Warning: Cannot check status of run %d on Shuttle logbook!",
						GetCurrentRun()));
		return hasError == kFALSE;
	}

	AliShuttleLogbookEntry* checkEntry = dynamic_cast<AliShuttleLogbookEntry*>
						(checkEntryArray.At(0));

	if (checkEntry)
	{
		if (checkEntry->IsDone())
		{
			Log("SHUTTLE","Process - Shuttle is DONE. Updating logbook");
			UpdateShuttleLogbook("shuttle_done");
		}
		else
		{
			for (UInt_t iDet=0; iDet<NDetectors(); iDet++)
			{
				if (checkEntry->GetDetectorStatus(iDet) == AliShuttleLogbookEntry::kUnprocessed)
				{
					AliDebug(2, Form("Run %d: setting %s as \"not first time unprocessed\"",
							checkEntry->GetRun(), GetDetName(iDet)));
					fFirstUnprocessed[iDet] = kFALSE;
				}
			}
		}
	}

	// remove ML instance
	delete fMonaLisa;
	fMonaLisa = 0;

	fLogbookEntry = 0;

	return hasError == kFALSE;
}

//______________________________________________________________________________________________
UInt_t AliShuttle::ProcessCurrentDetector()
{
	//
        // Makes data retrieval just for a specific detector (fCurrentDetector).
	// Threre should be a configuration for this detector.

	AliInfo(Form("Retrieving values for %s, run %d", fCurrentDetector.Data(), GetCurrentRun()));

	UpdateShuttleStatus(AliShuttleStatus::kDCSStarted);

	TMap dcsMap;
	dcsMap.SetOwner(1);

	Bool_t aDCSError = kFALSE;
	fGridError = kFALSE;

	// TODO Test only... I've added a flag that allows to
	// exclude DCS archive DB query
	if (!fgkProcessDCS)
	{
		AliInfo("Skipping DCS processing!");
		aDCSError = kFALSE;
	} else {
		TString host(fConfig->GetDCSHost(fCurrentDetector));
		Int_t port = fConfig->GetDCSPort(fCurrentDetector);

		// Retrieval of Aliases
		TObjString* anAlias = 0;
		Int_t iAlias = 1;
		Int_t nTotAliases= ((TMap*)fConfig->GetDCSAliases(fCurrentDetector))->GetEntries();
		TIter iterAliases(fConfig->GetDCSAliases(fCurrentDetector));
		while ((anAlias = (TObjString*) iterAliases.Next()))
		{
			TObjArray *valueSet = new TObjArray();
			valueSet->SetOwner(1);

			if (((iAlias-1) % 500) == 0 || iAlias == nTotAliases)
				AliInfo(Form("Querying DCS archive: alias %s (%d of %d)",
						anAlias->GetName(), iAlias++, nTotAliases));
			aDCSError = (GetValueSet(host, port, anAlias->String(), valueSet, kAlias) == 0);

			if(!aDCSError)
			{
				dcsMap.Add(anAlias->Clone(), valueSet);
			} else {
				Log(fCurrentDetector,
					Form("ProcessCurrentDetector - Error while retrieving alias %s",
						anAlias->GetName()));
				UpdateShuttleStatus(AliShuttleStatus::kDCSError);
				dcsMap.DeleteAll();
				return 0;
			}
		}

		// Retrieval of Data Points
		TObjString* aDP = 0;
		Int_t iDP = 0;
		Int_t nTotDPs= ((TMap*)fConfig->GetDCSDataPoints(fCurrentDetector))->GetEntries();
		TIter iterDP(fConfig->GetDCSDataPoints(fCurrentDetector));
		while ((aDP = (TObjString*) iterDP.Next()))
		{
			TObjArray *valueSet = new TObjArray();
			valueSet->SetOwner(1);
			if (((iDP-1) % 500) == 0 || iDP == nTotDPs)
				AliInfo(Form("Querying DCS archive: DP %s (%d of %d)",
						aDP->GetName(), iDP++, nTotDPs));
			aDCSError = (GetValueSet(host, port, aDP->String(), valueSet, kDP) == 0);

			if(!aDCSError)
			{
				dcsMap.Add(aDP->Clone(), valueSet);
			} else {
				Log(fCurrentDetector,
					Form("ProcessCurrentDetector - Error while retrieving data point %s",
						aDP->GetName()));
				UpdateShuttleStatus(AliShuttleStatus::kDCSError);
				dcsMap.DeleteAll();
				return 0;
			}
		}
	}

	// DCS Archive DB processing successful. Call Preprocessor!
	UpdateShuttleStatus(AliShuttleStatus::kPPStarted);

	AliPreprocessor* aPreprocessor =
		dynamic_cast<AliPreprocessor*> (fPreprocessorMap.GetValue(fCurrentDetector));

	aPreprocessor->Initialize(GetCurrentRun(), GetCurrentStartTime(), GetCurrentEndTime());
	UInt_t aPPResult = aPreprocessor->Process(&dcsMap);

	UInt_t returnValue = 0;
	if (aPPResult == 0) { // Preprocessor error
		UpdateShuttleStatus(AliShuttleStatus::kPPError);
		returnValue = 0;
	} else if (fGridError == kFALSE) { // process and Grid storage ok!
    		UpdateShuttleStatus(AliShuttleStatus::kDone);
		UpdateShuttleLogbook(fCurrentDetector, "DONE");
		Log(fCurrentDetector.Data(),
			"ProcessCurrentDetector - Preprocessor and Grid storage ended successfully");
		returnValue = 1;
        } else { // Grid storage error (process ok, but object put in local storage)
     		UpdateShuttleStatus(AliShuttleStatus::kStoreFailed);
		returnValue = 2;
	}

	dcsMap.DeleteAll();

	return returnValue;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::QueryShuttleLogbook(const char* whereClause,
		TObjArray& entries)
{
// Query DAQ's Shuttle logbook and fills detector status object.
// Call QueryRunParameters to query DAQ logbook for run parameters.

	entries.SetOwner(1);

	// check connection, in case connect
	if(!Connect(3)) return kFALSE;

	TString sqlQuery;
	sqlQuery = Form("select * from logbook_shuttle %s order by run", whereClause);

	TSQLResult* aResult = fServer[3]->Query(sqlQuery);
	if (!aResult) {
		AliError(Form("Can't execute query <%s>!", sqlQuery.Data()));
		return kFALSE;
	}

	AliDebug(2,Form("Query = %s", sqlQuery.Data()));

	if(aResult->GetRowCount() == 0) {
//		if(sqlQuery.EndsWith("where shuttle_done=0 order by run")){
//			Log("SHUTTLE", "QueryShuttleLogbook - All runs in Shuttle Logbook are already DONE");
//			delete aResult;
//			return kTRUE;
//		} else {
			AliInfo("No entries in Shuttle Logbook match request");
			delete aResult;
			return kTRUE;
//		}
	}

	// TODO Check field count!
	const UInt_t nCols = 22;
	if (aResult->GetFieldCount() != (Int_t) nCols) {
		AliError("Invalid SQL result field number!");
		delete aResult;
		return kFALSE;
	}

	TSQLRow* aRow;
	while ((aRow = aResult->Next())) {
		TString runString(aRow->GetField(0), aRow->GetFieldLength(0));
		Int_t run = runString.Atoi();

		AliShuttleLogbookEntry *entry = QueryRunParameters(run);
		if (!entry)
			continue;

		// loop on detectors
		for(UInt_t ii = 0; ii < nCols; ii++)
			entry->SetDetectorStatus(aResult->GetFieldName(ii), aRow->GetField(ii));

		entries.AddLast(entry);
		delete aRow;
	}

//	if(sqlQuery.EndsWith("where shuttle_done=0 order by run"))
//		Log("SHUTTLE", Form("QueryShuttleLogbook - Found %d unprocessed runs in Shuttle Logbook",
//							entries.GetEntriesFast()));
	delete aResult;
	return kTRUE;
}

//______________________________________________________________________________________________
AliShuttleLogbookEntry* AliShuttle::QueryRunParameters(Int_t run)
{
	//
	// Retrieve run parameters written in the DAQ logbook and sets them into AliShuttleLogbookEntry object
	//

	// check connection, in case connect
	if (!Connect(3))
		return 0;

	TString sqlQuery;
	sqlQuery.Form("select * from %s where run=%d", fConfig->GetDAQlbTable(), run);

	TSQLResult* aResult = fServer[3]->Query(sqlQuery);
	if (!aResult) {
		AliError(Form("Can't execute query <%s>!", sqlQuery.Data()));
		return 0;
	}

	if (aResult->GetRowCount() == 0) {
		Log("SHUTTLE", Form("QueryRunParameters - No entry in DAQ Logbook for run %d. Skipping", run));
		delete aResult;
		return 0;
	}

	if (aResult->GetRowCount() > 1) {
		AliError(Form("More than one entry in DAQ Logbook for run %d. Skipping", run));
		delete aResult;
		return 0;
	}

	TSQLRow* aRow = aResult->Next();
	if (!aRow)
	{
		AliError(Form("Could not retrieve row for run %d. Skipping", run));
		delete aResult;
		return 0;
	}

	AliShuttleLogbookEntry* entry = new AliShuttleLogbookEntry(run);

	for (Int_t ii = 0; ii < aResult->GetFieldCount(); ii++)
		entry->SetRunParameter(aResult->GetFieldName(ii), aRow->GetField(ii));

	UInt_t startTime = entry->GetStartTime();
	UInt_t endTime = entry->GetEndTime();

	if (!startTime || !endTime || startTime > endTime) {
		Log("SHUTTLE",
			Form("QueryRunParameters - Invalid parameters for Run %d: startTime = %d, endTime = %d",
				run, startTime, endTime));
		delete entry;
		delete aRow;
		delete aResult;
		return 0;
	}

	delete aRow;
	delete aResult;

	return entry;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::TryToStoreAgain()
{
  // Called in case the detector failed to store the object in Grid OCDB
  // It tries to store the object again, if it does not find more recent and overlapping objects
  // Calls underlying TryToStoreAgain(const char*) function twice, for OCDB and Reference storage.

	AliInfo("Trying to store OCDB data again...");
	Bool_t resultCDB = TryToStoreAgain(fgkMainCDB);

	AliInfo("Trying to store reference data again...");
	Bool_t resultRef = TryToStoreAgain(fgkMainRefStorage);

	return resultCDB && resultRef;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::TryToStoreAgain(TString& gridURI)
{
  // Called by TryToStoreAgain(), performs actual storage retry

	TObjArray* gridIds=0;

	Bool_t result = kTRUE;

	const char* type = 0;
	TString backupURI;
	if(gridURI == fgkMainCDB) {
		type = "OCDB";
		backupURI = fgkLocalCDB;
	} else if(gridURI == fgkMainRefStorage) {
		type = "reference";
		backupURI = fgkLocalRefStorage;
	} else {
		AliError(Form("Invalid storage URI: %s", gridURI.Data()));
		return kFALSE;
	}

	AliCDBManager* man = AliCDBManager::Instance();

	AliCDBStorage *gridSto = man->GetStorage(gridURI);
	if(!gridSto) {
		Log(fCurrentDetector.Data(),
			Form("TryToStoreAgain - cannot activate main %s storage", type));
		return kFALSE;
	}

	gridIds = gridSto->GetQueryCDBList();

	// get objects previously stored in local CDB
	AliCDBStorage *backupSto = man->GetStorage(backupURI);
	AliCDBPath aPath(GetOfflineDetName(fCurrentDetector.Data()),"*","*");
	// Local objects were stored with current run as Grid version!
	TList* localEntries = backupSto->GetAll(aPath.GetPath(), GetCurrentRun(), GetCurrentRun());
	localEntries->SetOwner(1);

	// loop on local stored objects
	TIter localIter(localEntries);
	AliCDBEntry *aLocEntry = 0;
	while((aLocEntry = dynamic_cast<AliCDBEntry*> (localIter.Next()))){
		aLocEntry->SetOwner(1);
		AliCDBId aLocId = aLocEntry->GetId();
		aLocEntry->SetVersion(-1);
		aLocEntry->SetSubVersion(-1);

		// loop on Grid valid Id's
		Bool_t store = kTRUE;
		TIter gridIter(gridIds);
		AliCDBId* aGridId = 0;
		while((aGridId = dynamic_cast<AliCDBId*> (gridIter.Next()))){
			// If local object is valid up to infinity we store it only if it is
			// the first unprocessed run!
			if (aLocId.GetLastRun() == AliCDBRunRange::Infinity())
			{
				if (!fFirstUnprocessed[GetDetPos(fCurrentDetector)])
				{
					Log(fCurrentDetector.Data(),
						("TryToStoreAgain - This object has validity infinite but "
						 "there are previous unprocessed runs!"));
					continue;
				} else {
					break;
				}
			}
			if(aGridId->GetPath() != aLocId.GetPath()) continue;
			// skip all objects valid up to infinity
			if(aGridId->GetLastRun() == AliCDBRunRange::Infinity()) continue;
			// if we get here, it means there's already some more recent object stored on Grid!
			store = kFALSE;
			break;
		}

		if(!store){
			Log(fCurrentDetector.Data(),
				Form("TryToStoreAgain - A more recent object already exists in %s storage: <%s>",
					type, aGridId->ToString().Data()));
			// removing local filename...
			// TODO maybe it's better not to remove it, it was not copied to the Grid!
			TString filename;
			backupSto->IdToFilename(aLocId, filename);
			AliInfo(Form("Removing local file %s", filename.Data()));
			gSystem->Exec(Form("rm %s",filename.Data()));
			continue;
		}

		// If we get here, the file can be stored!
		Bool_t storeOk = gridSto->Put(aLocEntry);
		if(storeOk){
			Log(fCurrentDetector.Data(),
				Form("TryToStoreAgain - Object <%s> successfully put into %s storage",
					aLocId.ToString().Data(), type));

			// removing local filename...
			TString filename;
			backupSto->IdToFilename(aLocId, filename);
			AliInfo(Form("Removing local file %s", filename.Data()));
			gSystem->Exec(Form("rm %s", filename.Data()));
			continue;
		} else	{
			Log(fCurrentDetector.Data(),
				Form("TryToStoreAgain - Grid %s storage of object <%s> failed again",
					type, aLocId.ToString().Data()));
			result = kFALSE;
		}
	}
	localEntries->Clear();

	return result;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::GetValueSet(const char* host, Int_t port, const char* entry,
				TObjArray* valueSet, DCSType type)
{
// Retrieve all "entry" data points from the DCS server
// host, port: TSocket connection parameters
// entry: name of the alias or data point
// valueSet: array of retrieved AliDCSValue's
// type: kAlias or kDP

	AliDCSClient client(host, port, fTimeout, fRetries);
	if (!client.IsConnected())
	{
		return kFALSE;
	}

	Int_t result=0;

	if (type == kAlias)
	{
		result = client.GetAliasValues(entry,
			GetCurrentStartTime(), GetCurrentEndTime(), valueSet);
	} else
	if (type == kDP)
	{
		result = client.GetDPValues(entry,
			GetCurrentStartTime(), GetCurrentEndTime(), valueSet);
	}

	if (result < 0)
	{
		Log(fCurrentDetector.Data(), Form("GetValueSet - Can't get '%s'! Reason: %s",
			entry, AliDCSClient::GetErrorString(result)));

		if (result == AliDCSClient::fgkServerError)
		{
			Log(fCurrentDetector.Data(), Form("GetValueSet - Server error: %s",
				client.GetServerError().Data()));
		}

		return kFALSE;
	}

	return kTRUE;
}

//______________________________________________________________________________________________
const char* AliShuttle::GetFile(Int_t system, const char* detector,
		const char* id, const char* source)
{
// Get calibration file from file exchange servers
// First queris the FXS database for the file name, using the run, detector, id and source info
// then calls RetrieveFile(filename) for actual copy to local disk
// run: current run being processed (given by Logbook entry fLogbookEntry)
// detector: the Preprocessor name
// id: provided as a parameter by the Preprocessor
// source: provided by the Preprocessor through GetFileSources function

	// check connection, in case connect
	if (!Connect(system))
	{
		Log(detector, Form("GetFile - Couldn't connect to %s FXS database", GetSystemName(system)));
		return 0;
	}

	// Query preparation
	TString sqlQueryStart;
	TString whereClause;
	TString sourceName(source);
	Int_t nFields = 0;
	if (system == kDAQ)
	{
		sqlQueryStart = Form("select filePath,size from %s where", fConfig->GetFXSdbTable(system));
		whereClause = Form("run=%d and detector=\"%s\" and fileId=\"%s\" and DAQsource=\"%s\"",
				GetCurrentRun(), detector, id, source);
		nFields = 2;

	}
	else if (system == kDCS)
	{
		sqlQueryStart = Form("select filePath,size from %s where", fConfig->GetFXSdbTable(system));
		whereClause = Form("run=%d and detector=\"%s\" and fileId=\"%s\"",
				GetCurrentRun(), detector, id);
		nFields = 2;
		sourceName="none";
	}
	else if (system == kHLT)
	{
		sqlQueryStart = Form("select filePath,fileSize,fileChecksum from %s where",
										fConfig->GetFXSdbTable(system));
		whereClause = Form("run=%d and detector=\"%s\" and fileId=\"%s\" and DDLnumbers=\"%s\"",
				GetCurrentRun(), detector, id, source);
		nFields = 3;
	}

	TString sqlQuery = Form("%s %s", sqlQueryStart.Data(), whereClause.Data());

	AliDebug(2, Form("SQL query: \n%s",sqlQuery.Data()));

	// Query execution
	TSQLResult* aResult = 0;
	aResult = dynamic_cast<TSQLResult*> (fServer[system]->Query(sqlQuery));
	if (!aResult) {
		Log(detector, Form("GetFileName - Can't execute SQL query to %s database for: id = %s, source = %s",
				GetSystemName(system), id, sourceName.Data()));
		return 0;
	}

	if(aResult->GetRowCount() == 0)
	{
		Log(detector,
			Form("GetFileName - No entry in %s FXS db for: id = %s, source = %s",
				GetSystemName(system), id, sourceName.Data()));
		delete aResult;
		return 0;
	}

	if (aResult->GetRowCount() > 1) {
		Log(detector,
			Form("GetFileName - More than one entry in %s FXS db for: id = %s, source = %s",
				GetSystemName(system), id, sourceName.Data()));
		delete aResult;
		return 0;
	}

	if (aResult->GetFieldCount() != nFields) {
		Log(detector,
			Form("GetFileName - Wrong field count in %s FXS db for: id = %s, source = %s",
				GetSystemName(system), id, sourceName.Data()));
		delete aResult;
		return 0;
	}

	TSQLRow* aRow = dynamic_cast<TSQLRow*> (aResult->Next());

	if (!aRow){
		Log(detector, Form("GetFileName - Empty set result in %s FXS db from query: id = %s, source = %s",
				GetSystemName(system), id, sourceName.Data()));
		delete aResult;
		return 0;
	}

	TString filePath(aRow->GetField(0), aRow->GetFieldLength(0));
	TString fileSize(aRow->GetField(1), aRow->GetFieldLength(1));
	TString fileMd5Sum;
	if(system == kHLT) fileMd5Sum = aRow->GetField(2);

	delete aResult;
	delete aRow;

	AliDebug(2, Form("filePath = %s",filePath.Data()));

	// retrieved file is renamed to make it unique
	TString localFileName = Form("%s_%s_%d_%s_%s.shuttle",
					GetSystemName(system), detector, GetCurrentRun(), id, sourceName.Data());


	// file retrieval from FXS
	UInt_t nRetries = 0;
	UInt_t maxRetries = 3;
	Bool_t result = kFALSE;

	// copy!! if successful TSystem::Exec returns 0
	while(nRetries++ < maxRetries) {
		AliDebug(2, Form("Trying to copy file. Retry # %d", nRetries));
		result = RetrieveFile(system, filePath.Data(), localFileName.Data());
		if(!result)
		{
			Log(detector, Form("GetFileName - Copy of file %s from %s FXS failed",
					filePath.Data(), GetSystemName(system)));
			continue;
		} else {
			AliInfo(Form("File %s copied from %s FXS into %s/%s",
						filePath.Data(), GetSystemName(system),
						GetShuttleTempDir(), localFileName.Data()));
		}

		if (system == kHLT)
		{
			// compare md5sum of local file with the one stored in the FXS DB
			Int_t md5Comp = gSystem->Exec(Form("md5sum %s/%s |grep %s 2>&1 > /dev/null",
						GetShuttleTempDir(), localFileName.Data(), fileMd5Sum.Data()));

			if (md5Comp != 0)
			{
				Log(detector, Form("GetFileName - md5sum of file %s does not match with local copy!",
							filePath.Data()));
				result = kFALSE;
				continue;
			}
		}
		if (result) break;
	}

	if(!result) return 0;

	fFXSCalled[system]=kTRUE;
	TObjString *fileParams = new TObjString(Form("%s#!?!#%s", id, sourceName.Data()));
	fFXSlist[system].Add(fileParams);

	static TString fullLocalFileName;
	fullLocalFileName = TString::Format("%s/%s", GetShuttleTempDir(), localFileName.Data());

	AliInfo(Form("fullLocalFileName = %s", fullLocalFileName.Data()));

	return fullLocalFileName.Data();

}

//______________________________________________________________________________________________
Bool_t AliShuttle::RetrieveFile(UInt_t system, const char* fxsFileName, const char* localFileName)
{
// Copies file from FXS to local Shuttle machine

	// check temp directory: trying to cd to temp; if it does not exist, create it
	AliDebug(2, Form("Copy file %s from %s FXS into %s/%s",
			GetSystemName(system), fxsFileName, GetShuttleTempDir(), localFileName));

	void* dir = gSystem->OpenDirectory(GetShuttleTempDir());
	if (dir == NULL) {
		if (gSystem->mkdir(GetShuttleTempDir(), kTRUE)) {
			AliError(Form("Can't open directory <%s>", GetShuttleTempDir()));
			return kFALSE;
		}

	} else {
		gSystem->FreeDirectory(dir);
	}

	TString baseFXSFolder;
	if (system == kDAQ)
	{
		baseFXSFolder = "FES/";
	}
	else if (system == kDCS)
	{
		baseFXSFolder = "";
	}
	else if (system == kHLT)
	{
		baseFXSFolder = "~/";
	}


	TString command = Form("scp -oPort=%d -2 %s@%s:%s%s %s/%s",
		fConfig->GetFXSPort(system),
		fConfig->GetFXSUser(system),
		fConfig->GetFXSHost(system),
		baseFXSFolder.Data(),
		fxsFileName,
		GetShuttleTempDir(),
		localFileName);

	AliDebug(2, Form("%s",command.Data()));

	Bool_t result = (gSystem->Exec(command.Data()) == 0);

	return result;
}

//______________________________________________________________________________________________
TList* AliShuttle::GetFileSources(Int_t system, const char* detector, const char* id)
{
// Get sources producing the condition file Id from file exchange servers

	if (system == kDCS)
	{
		AliError("DCS system has only one source of data!");
		return NULL;

	}

	// check connection, in case connect
	if (!Connect(system))
	{
		Log(detector, Form("GetFile - Couldn't connect to %s FXS database", GetSystemName(system)));
		return NULL;
	}

	TString sourceName = 0;
	if (system == kDAQ)
	{
		sourceName = "DAQsource";
	} else if (system == kHLT)
	{
		sourceName = "DDLnumbers";
	}

	TString sqlQueryStart = Form("select %s from %s where", sourceName.Data(), fConfig->GetFXSdbTable(kDAQ));
	TString whereClause = Form("run=%d and detector=\"%s\" and fileId=\"%s\"",
				GetCurrentRun(), detector, id);
	TString sqlQuery = Form("%s %s", sqlQueryStart.Data(), whereClause.Data());

	AliDebug(2, Form("SQL query: \n%s",sqlQuery.Data()));

	// Query execution
	TSQLResult* aResult;
	aResult = fServer[system]->Query(sqlQuery);
	if (!aResult) {
		Log(detector, Form("GetFileSources - Can't execute SQL query to %s database for id: %s",
				GetSystemName(system), id));
		return 0;
	}

	if (aResult->GetRowCount() == 0)
	{
		Log(detector,
			Form("GetFileSources - No entry in %s FXS table for id: %s", GetSystemName(system), id));
		delete aResult;
		return 0;
	}

	TSQLRow* aRow;
	TList *list = new TList();
	list->SetOwner(1);

	while ((aRow = aResult->Next()))
	{

		TString source(aRow->GetField(0), aRow->GetFieldLength(0));
		AliDebug(2, Form("%s = %s", sourceName.Data(), source.Data()));
		list->Add(new TObjString(source));
		delete aRow;
	}

	delete aResult;

	return list;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::Connect(Int_t system)
{
// Connect to MySQL Server of the system's FXS MySQL databases
// DAQ Logbook, Shuttle Logbook and DAQ FXS db are on the same host

	// check connection: if already connected return
	if(fServer[system] && fServer[system]->IsConnected()) return kTRUE;

	TString dbHost, dbUser, dbPass, dbName;

	if (system < 3) // FXS db servers
	{
		dbHost = Form("mysql://%s:%d", fConfig->GetFXSdbHost(system), fConfig->GetFXSdbPort(system));
		dbUser = fConfig->GetFXSdbUser(system);
		dbPass = fConfig->GetFXSdbPass(system);
		dbName =   fConfig->GetFXSdbName(system);
	} else { // Run & Shuttle logbook servers
	// TODO Will the Shuttle logbook server be the same as the Run logbook server ???
		dbHost = Form("mysql://%s:%d", fConfig->GetDAQlbHost(), fConfig->GetDAQlbPort());
		dbUser = fConfig->GetDAQlbUser();
		dbPass = fConfig->GetDAQlbPass();
		dbName =   fConfig->GetDAQlbDB();
	}

	fServer[system] = TSQLServer::Connect(dbHost.Data(), dbUser.Data(), dbPass.Data());
	if (!fServer[system] || !fServer[system]->IsConnected()) {
		if(system < 3)
		{
		AliError(Form("Can't establish connection to FXS database for %s",
					AliShuttleInterface::GetSystemName(system)));
		} else {
		AliError("Can't establish connection to Run logbook.");
		}
		if(fServer[system]) delete fServer[system];
		return kFALSE;
	}

	// Get tables
	TSQLResult* aResult=0;
	switch(system){
		case kDAQ:
			aResult = fServer[kDAQ]->GetTables(dbName.Data());
			break;
		case kDCS:
			aResult = fServer[kDCS]->GetTables(dbName.Data());
			break;
		case kHLT:
			aResult = fServer[kHLT]->GetTables(dbName.Data());
			break;
		default:
			aResult = fServer[3]->GetTables(dbName.Data());
			break;
	}

	delete aResult;
	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::UpdateTable()
{
// Update FXS table filling time_processed field in all rows corresponding to current run and detector

	Bool_t result = kTRUE;

	for (UInt_t system=0; system<3; system++)
	{
		if(!fFXSCalled[system]) continue;

		// check connection, in case connect
		if (!Connect(system))
		{
			Log(fCurrentDetector, Form("UpdateTable - Couldn't connect to %s FXS database", GetSystemName(system)));
			result = kFALSE;
			continue;
		}

		TTimeStamp now; // now

		// Loop on FXS list entries
		TIter iter(&fFXSlist[system]);
		TObjString *aFXSentry=0;
		while ((aFXSentry = dynamic_cast<TObjString*> (iter.Next())))
		{
			TString aFXSentrystr = aFXSentry->String();
			TObjArray *aFXSarray = aFXSentrystr.Tokenize("#!?!#");
			if (!aFXSarray || aFXSarray->GetEntries() != 2 )
			{
				Log(fCurrentDetector, Form("UpdateTable - error updating %s FXS entry. Check string: <%s>",
					GetSystemName(system), aFXSentrystr.Data()));
				if(aFXSarray) delete aFXSarray;
				result = kFALSE;
				continue;
			}
			const char* fileId = ((TObjString*) aFXSarray->At(0))->GetName();
			const char* source = ((TObjString*) aFXSarray->At(1))->GetName();

			TString whereClause;
			if (system == kDAQ)
			{
				whereClause = Form("where run=%d and detector=\"%s\" and fileId=\"%s\" and DAQsource=\"%s\";",
							GetCurrentRun(), fCurrentDetector.Data(), fileId, source);
			}
			else if (system == kDCS)
			{
				whereClause = Form("where run=%d and detector=\"%s\" and fileId=\"%s\";",
							GetCurrentRun(), fCurrentDetector.Data(), fileId);
			}
			else if (system == kHLT)
			{
				whereClause = Form("where run=%d and detector=\"%s\" and fileId=\"%s\" and DDLnumbers=\"%s\";",
							GetCurrentRun(), fCurrentDetector.Data(), fileId, source);
			}

			delete aFXSarray;

			TString sqlQuery = Form("update %s set time_processed=%d %s", fConfig->GetFXSdbTable(system),
								now.GetSec(), whereClause.Data());

			AliDebug(2, Form("SQL query: \n%s",sqlQuery.Data()));

			// Query execution
			TSQLResult* aResult;
			aResult = dynamic_cast<TSQLResult*> (fServer[system]->Query(sqlQuery));
			if (!aResult)
			{
				Log(fCurrentDetector, Form("UpdateTable - %s db: can't execute SQL query <%s>",
								GetSystemName(system), sqlQuery.Data()));
				result = kFALSE;
				continue;
			}
			delete aResult;
		}
	}

	return result;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::UpdateShuttleLogbook(const char* detector, const char* status)
{
	//
	// Update Shuttle logbook filling detector or shuttle_done column
	// ex. of usage: UpdateShuttleLogbook("PHOS", "DONE") or UpdateShuttleLogbook("shuttle_done")
	//

	// check connection, in case connect
	if(!Connect(3)){
		Log("SHUTTLE", "UpdateShuttleLogbook - Couldn't connect to DAQ Logbook.");
		return kFALSE;
	}

	TString detName(detector);
	TString setClause;
	if(detName == "shuttle_done")
	{
		setClause = "set shuttle_done=1";

		// Send the information to ML
		TMonaLisaText  mlStatus("SHUTTLE_status", "Done");

		TList mlList;
		mlList.Add(&mlStatus);

		fMonaLisa->SendParameters(&mlList);
	} else {
		TString statusStr(status);
		if(statusStr.Contains("done", TString::kIgnoreCase) ||
		   statusStr.Contains("failed", TString::kIgnoreCase)){
			setClause = Form("set %s=\"%s\"", detector, status);
		} else {
			Log("SHUTTLE",
				Form("UpdateShuttleLogbook - Invalid status <%s> for detector %s",
					status, detector));
			return kFALSE;
		}
	}

	TString whereClause = Form("where run=%d", GetCurrentRun());

	TString sqlQuery = Form("update logbook_shuttle %s %s",
					setClause.Data(), whereClause.Data());

	AliDebug(2, Form("SQL query: \n%s",sqlQuery.Data()));

	// Query execution
	TSQLResult* aResult;
	aResult = dynamic_cast<TSQLResult*> (fServer[3]->Query(sqlQuery));
	if (!aResult) {
		Log("SHUTTLE", Form("UpdateShuttleLogbook - Can't execute query <%s>", sqlQuery.Data()));
		return kFALSE;
	}
	delete aResult;

	return kTRUE;
}

//______________________________________________________________________________________________
Int_t AliShuttle::GetCurrentRun() const
{
// Get current run from logbook entry

	return fLogbookEntry ? fLogbookEntry->GetRun() : -1;
}

//______________________________________________________________________________________________
UInt_t AliShuttle::GetCurrentStartTime() const
{
// get current start time

	return fLogbookEntry ? fLogbookEntry->GetStartTime() : 0;
}

//______________________________________________________________________________________________
UInt_t AliShuttle::GetCurrentEndTime() const
{
// get current end time from logbook entry

	return fLogbookEntry ? fLogbookEntry->GetEndTime() : 0;
}

//______________________________________________________________________________________________
void AliShuttle::Log(const char* detector, const char* message)
{
// Fill log string with a message

	void* dir = gSystem->OpenDirectory(GetShuttleLogDir());
	if (dir == NULL) {
		if (gSystem->mkdir(GetShuttleLogDir(), kTRUE)) {
			AliError(Form("Can't open directory <%s>", GetShuttleLogDir()));
			return;
		}

	} else {
		gSystem->FreeDirectory(dir);
	}

	TString toLog = Form("%s (%d): %s - ", TTimeStamp(time(0)).AsString("s"), getpid(), detector);
	if (GetCurrentRun() >= 0) 
		toLog += Form("run %d - ", GetCurrentRun());
	toLog += Form("%s", message);

  	AliInfo(toLog.Data());

  	TString fileName;
	if (GetCurrentRun() >= 0) 
		fileName.Form("%s/%s_%d.log", GetShuttleLogDir(), detector, GetCurrentRun());
	else
		fileName.Form("%s/%s.log", GetShuttleLogDir(), detector);
	
  	gSystem->ExpandPathName(fileName);

  	ofstream logFile;
  	logFile.open(fileName, ofstream::out | ofstream::app);

  	if (!logFile.is_open()) {
    		AliError(Form("Could not open file %s", fileName.Data()));
    		return;
  	}

  	logFile << toLog.Data() << "\n";

  	logFile.close();
}

//______________________________________________________________________________________________
Bool_t AliShuttle::Collect(Int_t run)
{
//
// Collects conditions data for all UNPROCESSED run written to DAQ LogBook in case of run = -1 (default)
// If a dedicated run is given this run is processed
//
// In operational mode, this is the Shuttle function triggered by the EOR signal.
//

	if (run == -1)
		Log("SHUTTLE","Collect - Shuttle called. Collecting conditions data for unprocessed runs");
	else
		Log("SHUTTLE", Form("Collect - Shuttle called. Collecting conditions data for run %d", run));

	SetLastAction("Starting");

	TString whereClause("where shuttle_done=0");
	if (run != -1)
		whereClause += Form(" and run=%d", run);

	TObjArray shuttleLogbookEntries;
	if (!QueryShuttleLogbook(whereClause, shuttleLogbookEntries))
	{
		Log("SHUTTLE", "Collect - Can't retrieve entries from Shuttle logbook");
		return kFALSE;
	}

	if (shuttleLogbookEntries.GetEntries() == 0)
	{
		if (run == -1)
			Log("SHUTTLE","Collect - Found no UNPROCESSED runs in Shuttle logbook");
		else
			Log("SHUTTLE", Form("Collect - Run %d is already DONE "
						"or it does not exist in Shuttle logbook", run));
		return kTRUE;
	}

	for (UInt_t iDet=0; iDet<NDetectors(); iDet++)
		fFirstUnprocessed[iDet] = kTRUE;

	if (run != -1)
	{
		// query Shuttle logbook for earlier runs, check if some detectors are unprocessed,
		// flag them into fFirstUnprocessed array
		TString whereClause(Form("where shuttle_done=0 and run < %d", run));
		TObjArray tmpLogbookEntries;
		if (!QueryShuttleLogbook(whereClause, tmpLogbookEntries))
		{
			Log("SHUTTLE", "Collect - Can't retrieve entries from Shuttle logbook");
			return kFALSE;
		}

		TIter iter(&tmpLogbookEntries);
		AliShuttleLogbookEntry* anEntry = 0;
		while ((anEntry = dynamic_cast<AliShuttleLogbookEntry*> (iter.Next())))
		{
			for (UInt_t iDet=0; iDet<NDetectors(); iDet++)
			{
				if (anEntry->GetDetectorStatus(iDet) == AliShuttleLogbookEntry::kUnprocessed)
				{
					AliDebug(2, Form("Run %d: setting %s as \"not first time unprocessed\"",
							anEntry->GetRun(), GetDetName(iDet)));
					fFirstUnprocessed[iDet] = kFALSE;
				}
			}

		}

	}

	if (!RetrieveConditionsData(shuttleLogbookEntries))
	{
		Log("SHUTTLE", "Collect - Process of at least one run failed");
		return kFALSE;
	}

	Log("SHUTTLE", "Collect - Requested run(s) successfully processed");
	return kTRUE;
}

//______________________________________________________________________________________________
Bool_t AliShuttle::RetrieveConditionsData(const TObjArray& dateEntries)
{
// Retrieve conditions data for all runs that aren't processed yet

	Bool_t hasError = kFALSE;

	TIter iter(&dateEntries);
	AliShuttleLogbookEntry* anEntry;

	while ((anEntry = (AliShuttleLogbookEntry*) iter.Next())){
		if (!Process(anEntry)){
			hasError = kTRUE;
		}

		// clean SHUTTLE temp directory
		TString command = Form("rm -f %s/*.shuttle", GetShuttleTempDir());
		gSystem->Exec(command.Data());
	}

	return hasError == kFALSE;
}

//______________________________________________________________________________________________
ULong_t AliShuttle::GetTimeOfLastAction() const
{
	ULong_t tmp;

	fMonitoringMutex->Lock();

	tmp = fLastActionTime;

	fMonitoringMutex->UnLock();

	return tmp;
}

//______________________________________________________________________________________________
const TString AliShuttle::GetLastAction() const
{
	// returns a string description of the last action

	TString tmp;

	fMonitoringMutex->Lock();
	
	tmp = fLastAction;
	
	fMonitoringMutex->UnLock();

	return tmp;
}

//______________________________________________________________________________________________
void AliShuttle::SetLastAction(const char* action)
{
	// updates the monitoring variables

	fMonitoringMutex->Lock();

	fLastAction = action;
	fLastActionTime = time(0);
	
	fMonitoringMutex->UnLock();
}

//______________________________________________________________________________________________
const char* AliShuttle::GetRunParameter(const char* param)
{
// returns run parameter read from DAQ logbook

	if(!fLogbookEntry) {
		AliError("No logbook entry!");
		return 0;
	}

	return fLogbookEntry->GetRunParameter(param);
}

//______________________________________________________________________________________________
Bool_t AliShuttle::SendMail()
{
// sends a mail to the subdetector expert in case of preprocessor error

	void* dir = gSystem->OpenDirectory(GetShuttleLogDir());
	if (dir == NULL)
	{
		if (gSystem->mkdir(GetShuttleLogDir(), kTRUE))
		{
			AliError(Form("Can't open directory <%s>", GetShuttleLogDir()));
			return kFALSE;
		}

	} else {
		gSystem->FreeDirectory(dir);
	}

  	TString bodyFileName;
  	bodyFileName.Form("%s/mail.body", GetShuttleLogDir());
  	gSystem->ExpandPathName(bodyFileName);

  	ofstream mailBody;
  	mailBody.open(bodyFileName, ofstream::out);

  	if (!mailBody.is_open())
	{
    		AliError(Form("Could not open mail body file %s", bodyFileName.Data()));
    		return kFALSE;
  	}

	TString to="";
	TIter iterExperts(fConfig->GetResponsibles(fCurrentDetector));
	TObjString *anExpert=0;
	while ((anExpert = (TObjString*) iterExperts.Next()))
	{
		to += Form("%s,", anExpert->GetName());
	}
	to.Remove(to.Length()-1);
	AliDebug(2, Form("to: %s",to.Data()));

	// TODO this will be removed...
	if (to.Contains("not_yet_set")) {
		AliInfo("List of detector responsibles not yet set!");
		return kFALSE;
	}

	TString cc="alberto.colla@cern.ch";

	TString subject = Form("%s Shuttle preprocessor error in run %d !",
				fCurrentDetector.Data(), GetCurrentRun());
	AliDebug(2, Form("subject: %s", subject.Data()));

	TString body = Form("Dear %s expert(s), \n\n", fCurrentDetector.Data());
	body += Form("SHUTTLE just detected that your preprocessor "
			"exited with ERROR state in run %d!!\n\n", GetCurrentRun());
	body += Form("Please check %s status on the web page asap!\n\n", fCurrentDetector.Data());
	body += Form("The last 10 lines of %s log file are following:\n\n");

	AliDebug(2, Form("Body begin: %s", body.Data()));

	mailBody << body.Data();
  	mailBody.close();
  	mailBody.open(bodyFileName, ofstream::out | ofstream::app);

	TString logFileName = Form("%s/%s_%d.log", GetShuttleLogDir(), fCurrentDetector.Data(), GetCurrentRun());
	TString tailCommand = Form("tail -n 10 %s >> %s", logFileName.Data(), bodyFileName.Data());
	if (gSystem->Exec(tailCommand.Data()))
	{
		mailBody << Form("%s log file not found ...\n\n", fCurrentDetector.Data());
	}

	TString endBody = Form("------------------------------------------------------\n\n");
	endBody += Form("In case of problems please contact the SHUTTLE core team.\n\n");
	endBody += "Please do not answer this message directly, it is automatically generated.\n\n";
	endBody += "Sincerely yours,\n\n \t\t\tthe SHUTTLE\n";

	AliDebug(2, Form("Body end: %s", endBody.Data()));

	mailBody << endBody.Data();

  	mailBody.close();

	// send mail!
	TString mailCommand = Form("mail -s \"%s\" -c %s %s < %s",
						subject.Data(),
						cc.Data(),
						to.Data(),
						bodyFileName.Data());
	AliDebug(2, Form("mail command: %s", mailCommand.Data()));

	Bool_t result = gSystem->Exec(mailCommand.Data());

	return result == 0;
}
