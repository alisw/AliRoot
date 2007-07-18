/*

TPCda_pedestal.cxx - calibration algorithm for TPC pedestal runs

10/06/2007  sylvain.chapeland@cern.ch :  first version - clean skeleton based on DAQ DA case1

contact: marian.ivanov@cern.ch


This process reads RAW data from the files provided as command line arguments
and save results in a file (named from RESULT_FILE define - see below).

*/

#define RESULT_FILE "tpcPedestal.root"


extern "C" {
#include <daqDA.h>
}
#include "event.h"
#include "monitor.h"
#include <stdio.h>
#include <stdlib.h>

//
//Root includes
//
#include <TFile.h>

//
//AliRoot includes
//
#include "AliRawReader.h"
#include "AliRawReaderDate.h"
#include "AliTPCRawStream.h"
#include "AliTPCROC.h"
#include "AliTPCCalROC.h"
#include "AliTPCCalPad.h"
#include "AliMathBase.h"
#include "TTreeStream.h"

//
// TPC calibration algorithm includes
//
#include "AliTPCCalibPedestal.h"




/* Main routine
      Arguments: list of DATE raw data files
*/
int main(int argc, char **argv) {

  int i,status;
  AliTPCCalibPedestal calibPedestal;   // pedestal and noise calibration

  if (argc<2) {
    printf("Wrong number of arguments\n");
    return -1;
  }


  /* log start of process */
  printf("TPC DA started - %s\n",__FILE__);


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
    for(;;) {
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

      //  Pedestal calibration
      AliRawReader *rawReader = new AliRawReaderDate((void*)event);
      calibPedestal.ProcessEvent(rawReader);
      delete rawReader;

      /* free resources */
      free(event);
    }
  }

  calibPedestal.Analyse(); 
  printf ("%d events processed\n",nevents);

  TFile * fileTPC = new TFile (RESULT_FILE,"recreate");
  calibPedestal.Write("calibPedestal");
  delete fileTPC;
  printf("Wrote %s\n",RESULT_FILE);

  return status;
}
