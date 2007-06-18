//////////////////////////////////////////////////////////////////////////////
// Detector Algorithm for analysis of SDD test pulse runs.                  //
//                                                                          //
// Produces ASCII and ROOT output files with:                               //
// 1. anode quality bit                                                     //
// 1. Baseline values                                                       //
// 2. Raw noise                                                             //
// 3. Common mode coefficients                                              //
// 4. Noise corrected for common mode                                       //
// 5. Gain                                                                  //
// Files are written to FXS                                                 //
//                                                                          //
// Author: F. Prino (prino@to.infn.it)                                      //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////



extern "C" {
#include "daqDA.h"
}

#include "event.h"
#include "monitor.h"

#include <stdio.h>
#include <stdlib.h>

// ROOT includes
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TSystem.h>

// AliRoot includes
#include "AliRawReaderDate.h"
#include "AliITSOnlineSDDTP.h"
#include "AliITSRawStreamSDD.h"
/* Main routine
      Arguments: list of DATE raw data files
*/
int main(int argc, char **argv) {

  int status = 0;


  /* log start of process */
  printf("ITS SDD TP algorithm program started\n");  


  /* check that we got some arguments = list of files */
  if (argc<2) {
    printf("Wrong number of arguments\n");
    return -1;
  }


  Int_t maxNEvents=10; // maximum number of events to be analyzed
  const Int_t nSDDmodules=12;  // temp for test raw data
  AliITSOnlineSDDTP **tpan=new AliITSOnlineSDDTP*[2*nSDDmodules];
  TH2F **histo=new TH2F*[2*nSDDmodules];

  Char_t hisnam[20];
  for(Int_t imod=0; imod<nSDDmodules;imod++){
    for(Int_t isid=0;isid<2;isid++){
      Int_t index=2*imod+isid;
      tpan[index]=new AliITSOnlineSDDTP(imod,isid,411.);
      sprintf(hisnam,"his%03ds%d",imod,isid);
      histo[index]=new TH2F(hisnam,"",256,-0.5,255.5,256,-0.5,255.5);
    }
  }
  
  /* report progress */
  daqDA_progressReport(10);
  Int_t iev=0;
  /* read the data files */
  int n;
  for (n=1;n<argc;n++) {
   
    status=monitorSetDataSource( argv[n] );
    if (status!=0) {
      printf("monitorSetDataSource() failed : %s\n",monitorDecodeError(status));
      return -1;
    }

    /* report progress */
    /* in this example, indexed on the number of files */
    // Progress report inside the event loop as well?
    daqDA_progressReport(10+80*n/argc);

    /* read the file */
    for(;;) {
      struct eventHeaderStruct *event;
      eventTypeType eventT;

      /* get next event */
      status=monitorGetEventDynamic((void **)&event);
      if (status==MON_ERR_EOF) break; /* end of monitoring file has been reached */
      if (status!=0) {
	printf("monitorGetEventDynamic() failed : %s\n",monitorDecodeError(status));
	return -1;
      }

      /* retry if got no event */
      if (event==NULL) {
	break;
      }

      iev++; 
      if(iev>maxNEvents) break;
      
      /* use event - here, just write event id to result file */
      eventT=event->eventType;
      switch (event->eventType){
	
	/* START OF RUN */
      case START_OF_RUN:
	break;
	
	/* END OF RUN */
      case END_OF_RUN:
	break;
	
	//      case PHYSICS_EVENT:  // comment this line for test raw data
	//	break;               // comment this line for test raw data

      case CALIBRATION_EVENT:
	break;  // uncomment this line for test raw data
      case PHYSICS_EVENT: // uncomment this line for test raw data
	printf(" event number = %i \n",iev);
	AliRawReader *rawReader = new AliRawReaderDate((void*)event);
	rawReader->RequireHeader(kFALSE);
	rawReader->SelectEquipment(17,101,101);  // temp for test raw data

	Int_t evtyp=0;
	while(rawReader->ReadHeader()){
	  const UInt_t *subev = rawReader->GetSubEventAttributes();
	  if(subev[0]==0 && subev[1]==0 && subev[2]==0) evtyp=1; 
	}

	rawReader->Reset();
	for(Int_t imod=0; imod<nSDDmodules;imod++){
	  for(Int_t isid=0; isid<2;isid++){
	    Int_t index=2*imod+isid;
	    histo[index]->Reset();
	  }
	}
	AliITSRawStreamSDD s(rawReader);
	rawReader->SelectEquipment(17,101,101);  // temp for test raw data
	
	while(s.Next()){
	  Int_t iddl=rawReader->GetDDLID();
	  iddl=0; // temporary for test raw data
	  Int_t isddmod=s.GetModuleNumber(iddl,s.GetCarlosId()); 
	  isddmod-=240;  // to have SDD modules from 0 to 259
	  isddmod=s.GetCarlosId(); // temporary for test raw data
	  if(isddmod<nSDDmodules){ 
	    Int_t index=2*isddmod+s.GetChannel(); 
	    histo[index]->Fill(s.GetCoord2(),s.GetCoord1(),s.GetSignal());
	  }
	}
	delete rawReader;
	for(Int_t imod=0; imod<nSDDmodules;imod++){
	  for(Int_t isid=0; isid<2;isid++){
	    Int_t index=2*imod+isid;
	    tpan[index]->AddEvent(histo[index]);    
	  }
	}
	
	/* free resources */
	free(event);
      }
    }
    
  }
    
  TFile *fh=new TFile("SDDgainHistos.root","RECREATE");
  Char_t filnam[100],command[120];
  for(Int_t imod=0; imod<nSDDmodules;imod++){
    for(Int_t isid=0; isid<2;isid++){
      Int_t index=2*imod+isid;
      tpan[index]->ValidateAnodes();
      tpan[index]->WriteToASCII();
      tpan[index]->WriteToROOT(fh);
      sprintf(filnam,"SDDbase_mod%03d_sid%d.data",imod,isid);
      sprintf(command,"tar -rf SDDbase_LDC1.tar %s",filnam);
      gSystem->Exec(command);
    }  
  }
  fh->Close();

  /* write report */
  printf("Run #%s, received %d calibration events\n",getenv("DATE_RUN_NUMBER"),iev);

  /* report progress */
  daqDA_progressReport(90);



  fh->Close();



  status=daqDA_FES_storeFile("./SDDbase_LDC1.tar","SDD_Calib");

  /* report progress */
  daqDA_progressReport(100);



  return status;
}
