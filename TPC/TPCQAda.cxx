/*
TPC DA for online calibration

Contact: Haavard.Helstrup@cern.ch, peter.christiansen@hep.lu.se
Link: 
Run Type: PHYSICS STANDALONE DAQ
DA Type: MON
Number of events needed: 500
Input Files: 
Output Files: tpcQA.root, to be exported to the DAQ FXS
fileId:   QA
Trigger types used: PHYSICS_EVENT

*/

/*

TPCQAda.cxx - algorithm for TPC RAW QA

10/06/2007  sylvain.chapeland@cern.ch :  first version - clean skeleton based on DAQ DA case1
06/12/2007  haavard.helstrup@cern.ch  :  created CE DA based on pulser code
09/06/2008  peter.christiansen@hep.lu.se and haavard.helstrup@cern.ch  :  created QA DA based on AliTPCdataQA code

contact: marian.ivanov@cern.ch, peter.christiansen@hep.lu.se


This process reads RAW data from the files provided as command line arguments
and save results in a file (named from RESULT_FILE define - see below).

*/

#define RESULT_FILE "tpcQA.root"
#define FILE_ID "QA"
#define MAPPING_FILE "tpcMapping.root"


#include <daqDA.h>
#include "event.h"
#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>

//
//Root includes
//
#include <TFile.h>
#include "TROOT.h"
#include "TPluginManager.h"
//
//AliRoot includes
//
#include "AliRawReader.h"
#include "AliRawReaderDate.h"
#include "AliTPCmapper.h"
#include "AliTPCRawStream.h"
#include "AliTPCROC.h"
#include "AliTPCCalROC.h"
#include "AliTPCCalPad.h"
#include "TTreeStream.h"
#include "AliLog.h"
#include "TSystem.h"

//
// TPC calibration algorithm includes
//
#include "AliTPCdataQA.h"

/* Main routine
      Arguments: list of DATE raw data files
*/
int main(int argc, char **argv) {

 gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
                                         "*",
                                         "TStreamerInfo",
                                         "RIO",
                                         "TStreamerInfo()");

  AliLog::SetClassDebugLevel("AliTPCRawStream",-5);
  AliLog::SetClassDebugLevel("AliRawReaderDate",-5);
  AliLog::SetClassDebugLevel("AliTPCAltroMapping",-5);
  AliLog::SetModuleDebugLevel("RAW",-5);

  int i,status;
  AliTPCmapper *mapping = 0;   // The TPC mapping
  // if  test setup get parameters from $DAQDA_TEST_DIR 
  
  if (!mapping){
    /* copy locally the mapping file from daq detector config db */
    status = daqDA_DB_getFile(MAPPING_FILE,"./tpcMapping.root");
    if (status) {
      printf("Failed to get mapping file (%s) from DAQdetDB, status=%d\n", MAPPING_FILE, status);
      printf("Continue anyway ... maybe it works?\n");              // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      return -1;   // temporarily uncommented for testing on pcald47 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }

    /* open the mapping file and retrieve mapping object */
    TFile *fileMapping = new TFile(MAPPING_FILE, "read");
    mapping = (AliTPCmapper*) fileMapping->Get("tpcMapping");
    delete fileMapping;
  }

  if (mapping == 0) {
    printf("Failed to get mapping object from %s.  ...\n", MAPPING_FILE);
    //return -1;
  } else {
    printf("Got mapping object from %s\n", MAPPING_FILE);
  }


  AliTPCdataQA calibQA;   // pedestal and noise calibration

  if (argc<2) {
    printf("Wrong number of arguments\n");
    return -1;
  }


  /* log start of process */
  printf("TPC QA DA started - %s\n",__FILE__);


  /* set time bin range */
  calibQA.SetRangeTime(0,1000);
  calibQA.SetAltroMapping(mapping->GetAltroMapping()); // Use altro mapping we got from daqDetDb

  /* declare monitoring program */
  status=monitorDeclareMp( __FILE__ );
  if (status!=0) {
    printf("monitorDeclareMp() failed : %s\n",monitorDecodeError(status));
    return -1;
  }


  /* loop over RAW data files */
  int nevents=0;
  for(i=1;i<argc;i++) {

    /* define data source : this is argument i */
    printf("Processing file %s\n", argv[i]);
    status=monitorSetDataSource( argv[i] );
    if (status!=0) {
      printf("monitorSetDataSource() failed : %s\n",monitorDecodeError(status));
      return -1;
    }

    /* read until EOF */
    while (true) {
      struct eventHeaderStruct *event;

      /* check shutdown condition */
      if (daqDA_checkShutdown()) {break;}

      /* get next event (blocking call until timeout) */
      status=monitorGetEventDynamic((void **)&event);
      if (status==MON_ERR_EOF) {
        printf ("End of File %d detected\n",i);
        break; /* end of monitoring file has been reached */
      }

      if (status!=0) {
        printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(status));
        break;
      }

      /* retry if got no event */
      if (event==NULL) {
        continue;
      }
      nevents++;

      //  Pulser calibration

      AliRawReader *rawReader = new AliRawReaderDate((void*)event);
      calibQA.ProcessEvent(rawReader);
      delete rawReader;

      /* free resources */
      free(event);
    }
  }

  calibQA.Analyse(); 
  printf ("%d events processed\n",nevents);

  TFile * fileTPC = new TFile (RESULT_FILE,"recreate");
  calibQA.Write("tpcCalibQA");
  delete fileTPC;
  printf("Wrote %s\n",RESULT_FILE);

  /* store the result file on FXS */

  status=daqDA_FES_storeFile(RESULT_FILE,FILE_ID);
  if (status) {
    status = -2;
  }

  return status;
}
