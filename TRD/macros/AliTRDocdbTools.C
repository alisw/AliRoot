#include <AliCDBEntry.h>
#include <AliCDBManager.h>
#include <AliCDBStorage.h>
#include <AliTRDCalDCS.h>
#include <AliTRDCalDCSv2.h>
#include <AliTRDCalDCSFEE.h>
#include <AliTRDCalDCSFEEv2.h>

#include <TH1.h>
#include <TAlien.h>
#include <TGridResult.h>

#include <iostream>

using namespace std;


Int_t year=2018;

void UseAlienCDB(Int_t runNr);
void UseCvmfsCDB(Int_t year);
void UseLocalCDB(TString path);

AliTRDCalDCSv2* get_caldcs(int run, int soreor=1);
bool check_run(int run);
void runinfo(int run);


void AliTRDocdbTools()
{
  cout << endl
       << "LOADING AliTRDocdbTools.C..." << endl
       << endl
       << "This is a collection of utilities to interact with the "
       << "TRD part of the OCDB." << endl
       << endl;

  
  //UseCvmfsCDB(2018);
  UseAlienCDB(288908);
}

void UseAlienCDB(Int_t runNr)
{
  cout << "I : Accessing grid storage for run number " << runNr << endl;
  cout << "I : Get CDBManager instance." << endl;
  AliCDBManager *man = AliCDBManager::Instance();
  cout << "I : SetDefaultStorage." << endl;
  man->SetDefaultStorageFromRun(runNr);
}

void UseCvmfsCDB(Int_t year)
{
  UseLocalCDB(Form("/cvmfs/alice-ocdb.cern.ch/calibration/data/%4d/OCDB",
		   year));
}

void UseLocalCDB(TString path)
{
  cout << "I : Accessing local CDB storage at " << path << endl;
  AliCDBManager *man = AliCDBManager::Instance();
  man->SetDefaultStorage("local://"+path);
}



void list_runs(Int_t year)
{
  const char* dirpattern = 
    Form("/cvmfs/alice-ocdb.cern.ch/calibration/data/%d/OCDB/TRD/Calib/DCS",
	 year);

  void* dir = gSystem->OpenDirectory(gSystem->ExpandPathName(dirpattern));

  const char* entry;
  while((entry = (char*)gSystem->GetDirEntry(dir))) {
    int firstrun=0, lastrun=0,v=0,s=0;
    sscanf(entry,"Run%d_%d_v%d_s%d.root",&firstrun,&lastrun,&v,&s);
    
    if (firstrun==0) continue;
    
    if ( lastrun != 999999999 ) {
      // this entry looks like a fix for a broken config -> skip
      continue;
    }
    
    runinfo(firstrun);
  }

}

void list_runs_alien(Int_t year, int minrun=0, int maxrun=999999999)
{
  AliLog::SetGlobalLogLevel(AliLog::kWarning);
  TGridResult* res=gGrid->Ls(Form("/alice/data/%d/OCDB/TRD/Calib/DCS", year));
  
  for (int i=0; i<res->GetEntries(); i++) {
    
    int firstrun=0, lastrun=0,v=0,s=0;
    sscanf(res->GetFileName(i), "Run%d_%d_v%d_s%d.root",
	   &firstrun, &lastrun, &v, &s);
    
    // skip default entry, not associated with a run
    if (firstrun==0) continue;

    // skip broken config, or already present patches
    if ( lastrun != 999999999 ) continue;

    // skip runs outside requested range
    if (firstrun < minrun) continue;
    if (firstrun > maxrun) continue;
    
    runinfo(firstrun);
  }

}



void check_runs_alien(Int_t year, int minrun=0, int maxrun=999999999)
{
  AliLog::SetGlobalLogLevel(AliLog::kWarning);
  TGridResult* res=gGrid->Ls(Form("/alice/data/%d/OCDB/TRD/Calib/DCS", year));
  
  for (int i=0; i<res->GetEntries(); i++) {
    
    int firstrun=0, lastrun=0,v=0,s=0;
    sscanf(res->GetFileName(i), "Run%d_%d_v%d_s%d.root",
	   &firstrun, &lastrun, &v, &s);
    
    // skip default entry, not associated with a run
    if (firstrun==0) continue;

    // skip broken config, or already present patches
    if ( lastrun != 999999999 ) continue;

    // skip runs outside requested range
    if (firstrun < minrun) continue;
    if (firstrun > maxrun) continue;
    
    check_run(firstrun);
  }

}

void check_runs(Int_t* runs)
{
  AliLog::SetGlobalLogLevel(AliLog::kWarning);

  for (Int_t i=0; runs[i]>0; i++) {
    check_run(runs[i]);
  }
}


AliTRDCalDCSv2* get_caldcs(int run, int soreor=1)
{
  
  AliCDBManager* man = AliCDBManager::Instance();
  man->SetRun(run);
  AliCDBEntry *entry = man->Get("TRD/Calib/DCS");
  //  AliCDBEntry *entry = AliCDBManager::Instance()->Get("TRD/Calib/DCS",run);
  if (entry == NULL) {
    cout << endl << "ERROR: Unable to get the AliTRDCalDCS object"
	 << "from the OCDB for run number " << run << "." << endl;
    return NULL;
  }

  TObjArray* objarr = (TObjArray*)entry->GetObject();
  
  return (AliTRDCalDCSv2*) objarr->At(soreor);
}

bool check_run(int run)
{

  // Get OCDB entry
  AliCDBManager* man = AliCDBManager::Instance();
  man->SetRun(run);
  AliCDBEntry *entry = man->Get("TRD/Calib/DCS");
  TObjArray *objArrayCDB = (TObjArray*)entry->GetObject();

  // error counter
  int nerr = 0;

  TString soreor[] = {"SOR", "EOR"};
  
  for (int i=0; i<2; i++) {
    AliTRDCalDCSv2 *caldcs  = (AliTRDCalDCSv2 *)objArrayCDB->At(i);

    if ( caldcs->GetGlobalConfigName() == "mixed" ||
	 caldcs->GetGlobalConfigVersion() == "mixed" ) {

      cout << "run " << run << (i?" SOR: ":" EOR: ")
	   << "ERROR: mixed configuration" << endl;

      nerr++;
    }
  }

  if (nerr == 0) {
    cout << "run " << run << ": ok" << endl;
  }
  
  return (nerr > 0);
}


void runinfo(int run)
{
  AliTRDCalDCSv2* c = get_caldcs(run);

  c->Print();

  if (0) {
    for(Int_t i=0; i<540; i++) {
      AliTRDCalDCSFEEv2 *fee = c->GetCalDCSFEEObj(i);
      if(fee == NULL) continue;
      if(fee->GetStatusBit() != 0) continue;
      
      cout << fee->GetNumberOfTimeBins() << " "
	   << fee->GetConfigTag() << " "
	   << fee->GetSingleHitThres() << " "
	   << fee->GetThreePadClustThres() << " "
	   << fee->GetSelectiveNoZS() << " "
	   << fee->GetTCFilterWeight() << " "
	   << fee->GetTCFilterShortDecPar() << " "
	   << fee->GetTCFilterLongDecPar() << " "
	   << fee->GetFastStatNoise() << " "
	//<< fee->GetConfigVersion() << " "
	//<< fee->GetConfigName() << " "
	   << fee->GetFilterType() << " "
	   << fee->GetReadoutParam() << " "
	   << fee->GetTestPattern() << " "
	   << fee->GetTrackletMode() << " "
	   << fee->GetTrackletDef() << " "
	   << fee->GetTriggerSetup() << " "
	   << fee->GetAddOptions() << endl;
      

    }
  }
  
}



void fix_mixed_config(Int_t run, TString jira)
{

  // figure out who is to blame for the patch
  TString responsible = "";
  
  if (gSystem->GetUserInfo()->fRealName == "Tom Dietel" ||
      gSystem->GetUserInfo()->fRealName == "Thomas Dietel" ) {

    responsible = "Tom.Dietel@cern.ch";

  } else {
    cerr << "Cannot determine responsible user. "
	 << "Please update AliTRDocdbTools.C" << endl;
    return;
  }
  
  // Get OCDB entry
  AliCDBManager* man = AliCDBManager::Instance();
  man->SetRun(run);
  AliCDBEntry *entry = man->Get("TRD/Calib/DCS");
  TObjArray *objArrayCDB = (TObjArray*)entry->GetObject();
  AliTRDCalDCSv2 *caldcsSOR  = (AliTRDCalDCSv2 *)objArrayCDB->At(0);
  AliTRDCalDCSv2 *caldcsEOR  = (AliTRDCalDCSv2 *)objArrayCDB->At(1);
   
  // Fix mixed configuration
  if(caldcsSOR->GetGlobalConfigName().Contains("mixed") ||
     caldcsEOR->GetGlobalConfigName().Contains("mixed") ){
    
    caldcsSOR->ForceOverwritePluralityConfig();
    caldcsEOR->ForceOverwritePluralityConfig();

    AliCDBId id("TRD/Calib/DCS",run,run);

    // Modify the metadata
    AliCDBMetaData *md = entry->GetMetaData();
    md->SetResponsible(responsible);
    md->SetComment((TString)md->GetComment()
		   + " Fix for mixed config Jira " + jira);

    
    // Store locally
    AliCDBStorage *locStore = man->GetStorage("local://OCDB");
    locStore->Put(objArrayCDB,id,md);
  } // mixed
}

void fix_mixed_config(Int_t* runs, TString jira)
{
  for (Int_t i=0; runs[i]>0; i++) {
    fix_mixed_config(runs[i],jira);
  }
}


//// List all runs in a given year. 
//void list_runs(Int_t year) {
//
//  // Run ranges will have to be updated every year, refer to file
//  // OCDBFoldervsRunRange.xml in CVMVS OCDB for reference.
//
//  switch (year) {
//  case 2017: list_runs(267258, 282900); break;
//  case 2018: list_runs(282901, 999999); break;
//
//  default:
//    cout << "Unknown year '" << year
//	 << "', please update AliTRDocdbTools.C" << endl;
//    break;
//  }
//}
//
