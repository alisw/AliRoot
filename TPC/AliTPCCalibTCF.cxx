/**************************************************************************
 * Copyright(c) 2007-08, ALICE Experiment at CERN, All rights reserved. *
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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class for Evaluation and Validation of the ALTRO Tail Cancelation Filter  //
// (TCF) parameters out of TPC Raw data                                      //
//                                                                           //
// Author: Stefan Rossegger                                                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTPCCalibTCF.h"

#include <TObject.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TKey.h>
#include <TStyle.h>
#include <TMinuit.h>
#include <TH1F.h>

#include <TMath.h>
#include <TNtuple.h>
#include <TEntryList.h>

#include "AliRawReaderRoot.h"
#include "AliTPCRawStream.h"
#include "AliTPCROC.h"

#include "AliTPCAltroEmulator.h"

ClassImp(AliTPCCalibTCF)
  
AliTPCCalibTCF::AliTPCCalibTCF() :
  TNamed(),
  fGateWidth(100),
  fSample(900),
  fPulseLength(500),
  fLowPulseLim(30),
  fUpPulseLim(1000),
  fRMSLim(4.)
{
  //
  //  AliTPCCalibTCF standard constructor
  //
}
  
//_____________________________________________________________________________
AliTPCCalibTCF::AliTPCCalibTCF(Int_t gateWidth, Int_t sample, Int_t pulseLength, Int_t lowPulseLim, Int_t upPulseLim, Double_t rmsLim) : 
  TNamed(),
  fGateWidth(gateWidth),
  fSample(sample),
  fPulseLength(pulseLength),
  fLowPulseLim(lowPulseLim),
  fUpPulseLim(upPulseLim),
  fRMSLim(rmsLim)
{
  //
  //  AliTPCCalibTCF constructor with specific (non-standard) thresholds
  //
}
  
//_____________________________________________________________________________
AliTPCCalibTCF::AliTPCCalibTCF(const AliTPCCalibTCF &tcf) : 
  TNamed(tcf),
  fGateWidth(tcf.fGateWidth),
  fSample(tcf.fSample),
  fPulseLength(tcf.fPulseLength),
  fLowPulseLim(tcf.fLowPulseLim),
  fUpPulseLim(tcf.fUpPulseLim),
  fRMSLim(tcf.fRMSLim)
{
  //
  //  AliTPCCalibTCF copy constructor
  //
}


//_____________________________________________________________________________
AliTPCCalibTCF& AliTPCCalibTCF::operator = (const AliTPCCalibTCF &source)
{
  //
  // AliTPCCalibTCF assignment operator
  //
 
  if (&source == this) return *this;
  new (this) AliTPCCalibTCF(source);

  return *this;

}

//_____________________________________________________________________________
AliTPCCalibTCF::~AliTPCCalibTCF()
{
  //
  // AliTPCCalibTCF destructor
  //
}

//_____________________________________________________________________________
void AliTPCCalibTCF::ProcessRawFile(const char *nameRawFile, const char *nameFileOut) {
  //
  // Loops over all events within one RawData file and collects proper pulses 
  // (according to given tresholds) per pad
  // Histograms per pad are stored in 'nameFileOut'
  //
  
  AliRawReader *rawReader = new AliRawReaderRoot(nameRawFile);
  rawReader->Reset();

  while ( rawReader->NextEvent() ){ // loop
    printf("Reading next event ...");
    AliTPCRawStream rawStream(rawReader);
    rawReader->Select("TPC");
    ProcessRawEvent(&rawStream, nameFileOut);
  }

  rawReader->~AliRawReader();
  
}


//_____________________________________________________________________________
void AliTPCCalibTCF::ProcessRawEvent(AliTPCRawStream *rawStream, const char *nameFileOut) {
  //
  // Extracts proper pulses (according the given tresholds) within one event
  // and accumulates them into one histogram per pad. All histograms are
  // saved in the file 'nameFileOut'. 
  // The first bins of the histograms contain the following information:
  //   bin 1: Number of accumulated pulses
  //   bin 2;3;4: Sector; Row; Pad; 
  // 

  Int_t sector = rawStream->GetSector();
  Int_t row    = rawStream->GetRow();
  Int_t prevTime = 999999;
  Int_t prevPad = 999999;

  TFile fileOut(nameFileOut,"UPDATE");
  fileOut.cd();  
  
  TH1I *tempHis = new TH1I("tempHis","tempHis",fSample+fGateWidth,fGateWidth,fSample+fGateWidth);
  TH1I *tempRMSHis = new TH1I("tempRMSHis","tempRMSHis",2000,0,2000);

  while (rawStream->Next()) {
    
    // in case of a new row, get sector and row number
    if (rawStream->IsNewRow()){ 
      sector = rawStream->GetSector();
      row    = rawStream->GetRow();
    }

    Int_t pad = rawStream->GetPad();
    Int_t time = rawStream->GetTime();
    Int_t signal = rawStream->GetSignal();

    if (!rawStream->IsNewPad()) { // Reading signal from one Pad 
      if (time>prevTime) {
        printf("Wrong time: %d %d\n",rawStream->GetTime(),prevTime);
        rawStream->Dump();
      } else {
	// still the same pad, save signal to temporary histogram
	if (time<=fSample+fGateWidth && time>fGateWidth) {
	  tempHis->SetBinContent(time,signal);
	}
      }      
    } else { 
      // complete pulse found and stored into tempHis, now calculation 
      // of it's properties and comparison to given thresholds
   
      Int_t max = (Int_t)tempHis->GetMaximum(FLT_MAX);
      Int_t maxpos =  tempHis->GetMaximumBin();
      
      Int_t first = (Int_t)TMath::Max(maxpos-10, 0);
      Int_t last  = TMath::Min((Int_t)maxpos+fPulseLength-10, fSample);
      
      // simple baseline substraction ? better one needed ? (pedestalsubstr.?)
      // and RMS calculation with timebins before the pulse and at the end of
      // the signal 
      for (Int_t ipos = 0; ipos<6; ipos++) {
	// before the pulse
	tempRMSHis->Fill(tempHis->GetBinContent(first+ipos));
	// at the end to get rid of pulses with serious baseline fluctuations
	tempRMSHis->Fill(tempHis->GetBinContent(last-ipos)); 
      }
      Double_t baseline = tempRMSHis->GetMean();
      Double_t rms = tempRMSHis->GetRMS();
      tempRMSHis->Reset();

      Double_t lowLim = fLowPulseLim+baseline;
      Double_t upLim = fUpPulseLim+baseline;

      // Decision if found pulse is a proper one according to given tresholds
      if (max>lowLim && max<upLim && !((last-first)<fPulseLength) && rms<fRMSLim){
	char hname[100];
	sprintf(hname,"sec%drow%dpad%d",sector,row,prevPad);
	
	TH1F *his = (TH1F*)fileOut.Get(hname);
	
	if (!his ) { // new entry (pulse in new pad found)
	  
	  his = new TH1F(hname,hname, fPulseLength+4, 0, fPulseLength+4);
	  his->SetBinContent(1,1);       //  pulse counter (1st pulse)
	  his->SetBinContent(2,sector);  //  sector
	  his->SetBinContent(3,row);     //  row
	  his->SetBinContent(4,prevPad); //  pad	  
       
	  for (Int_t ipos=0; ipos<last-first; ipos++){
	    Int_t signal = (Int_t)(tempHis->GetBinContent(ipos+first)-baseline);
	    his->SetBinContent(ipos+5,signal);
	  }
	  his->Write(hname);
	  printf("new  %s: Signal %d at bin %d \n", hname, max-(Int_t)baseline, maxpos+fGateWidth);
	
	} else {  // adding pulse to existing histogram (pad already found)
	
	  his->AddBinContent(1,1); //  pulse counter for each pad
	  for (Int_t ipos=0; ipos<last-first; ipos++){
	    Int_t signal= (Int_t)(tempHis->GetBinContent(ipos+first)-baseline);
	    his->AddBinContent(ipos+5,signal);
	  }
	  printf("adding ...  %s: Signal %d at bin %d \n", hname, max-(Int_t)baseline, maxpos+fGateWidth);
	  his->Write(hname,kOverwrite);
	}	
      }
      tempHis->Reset();
    }
    prevTime = time;
    prevPad = pad;
  }

  tempHis->~TH1I();
  tempRMSHis->~TH1I();
  printf("Finished to read event ... \n");
  fileOut.Close();
}

//____________________________________________________________________________
void AliTPCCalibTCF::MergeHistoPerSector(const char *nameFileIn) {
  //
  // Merges all histograms within one sector, calculates the TCF parameters
  // of the 'histogram-per-sector' and stores (histo and parameters) into 
  // seperated files ...
  //
  // note: first 4 timebins of a histogram hold specific informations
  //       about number of collected pulses, sector, row and pad
  //
  // 'nameFileIn':  root file produced with Process function which holds
  //                one histogram per pad (sum of signals of proper pulses)
  // 'Sec+nameFileIn': root file with one histogram per sector
  //                   (information of row and pad are set to -1)
  //

  TFile fileIn(nameFileIn,"READ");
  TH1F *hisPad = 0;
  TKey *key = 0;
  TIter next( fileIn.GetListOfKeys() );

  char nameFileOut[100];
  sprintf(nameFileOut,"Sec-%s",nameFileIn);

  TFile fileOut(nameFileOut,"RECREATE");
  fileOut.cd();
  
  Int_t nHist = fileIn.GetNkeys();
  Int_t iHist = 0; // histogram counter for merge-status print
  
  while ( (key=(TKey*)next()) ) {

    iHist++;

    hisPad = (TH1F*)fileIn.Get(key->GetName()); // copy object to memory
    Int_t pulseLength = hisPad->GetNbinsX() -4; 
    // -4 because first four timebins contain pad specific informations
    Int_t npulse = (Int_t)hisPad->GetBinContent(1);
    Int_t sector = (Int_t)hisPad->GetBinContent(2);
  
    char hname[100];
    sprintf(hname,"sector%d",sector);
    TH1F *his = (TH1F*)fileOut.Get(hname);
    
    if (!his ) { // new histogram (new sector)
      his = new TH1F(hname,hname, pulseLength+4, 0, pulseLength+4);
      his->SetBinContent(1,npulse); // pulse counter
      his->SetBinContent(2,sector); // set sector info 
      his->SetBinContent(3,-1); // set to dummy value 
      his->SetBinContent(4,-1); // set to dummy value
      for (Int_t ipos=0; ipos<pulseLength; ipos++){
	his->SetBinContent(ipos+5,hisPad->GetBinContent(ipos+5));
      }
      his->Write(hname);
      printf("found  %s ...\n", hname);
    } else { // add to existing histogram for sector
      his->AddBinContent(1,npulse); // pulse counter      
      for (Int_t ipos=0; ipos<pulseLength; ipos++){
	his->AddBinContent(ipos+5,hisPad->GetBinContent(ipos+5));
      }
      his->Write(hname,kOverwrite);
    }

    if (iHist%500==0) {
      printf("merging status: \t %d pads out of %d \n",iHist, nHist);
    }
  }
  printf("merging done ...\n");
  fileIn.Close();
  fileOut.Close();

  // calculate TCF parameters on averaged pulse per Sector
  AnalyzeRootFile(nameFileOut);


}


//____________________________________________________________________________
void AliTPCCalibTCF::AnalyzeRootFile(const char *nameFileIn, Int_t minNumPulse) {
  //
  // This function takes a prepeared root file (accumulated histograms: output
  // of process function) and performs an analysis (fit and equalization) in 
  // order to get the TCF parameters. These are stored in an TNtuple along with 
  // the pad and creation infos. The tuple is written to the output file 
  // "TCFparam+nameFileIn"
  // To reduce the analysis time, the minimum number of accumulated pulses within 
  // one histogram 'minNumPulse' (to perform the analysis on) can be set
  //

  TFile fileIn(nameFileIn,"READ");
  TH1F *hisIn;
  TKey *key;
  TIter next( fileIn.GetListOfKeys() );

  char nameFileOut[100];
  sprintf(nameFileOut,"TCFparam-%s",nameFileIn);
  
  TFile fileOut(nameFileOut,"RECREATE");
  fileOut.cd();

  TNtuple *paramTuple = new TNtuple("TCFparam","TCFparameter","sec:row:pad:npulse:Z0:Z1:Z2:P0:P1:P2");
  
  Int_t nHist = fileIn.GetNkeys(); 
  Int_t iHist = 0;  // counter for print of analysis-status
  
  while ((key = (TKey *) next())) { // loop over histograms
  
    printf("Analyze histogramm %d out of %d\n",++iHist,nHist);
    hisIn = (TH1F*)fileIn.Get(key->GetName()); // copy object to memory

    Int_t numPulse = (Int_t)hisIn->GetBinContent(1); 
    if ( numPulse >= minNumPulse ) {
    
      Double_t* coefP = new Double_t[3];
      Double_t* coefZ = new Double_t[3];
      for(Int_t i = 0; i < 3; i++){
	coefP[i] = 0;
	coefZ[i] = 0;
      }
      // perform the analysis on the given histogram 
      Int_t fitOk = AnalyzePulse(hisIn, coefZ, coefP);    
      if (fitOk) { // Add found parameters to file 
	Int_t sector = (Int_t)hisIn->GetBinContent(2);
	Int_t row = (Int_t)hisIn->GetBinContent(3);
	Int_t pad = (Int_t)hisIn->GetBinContent(4);
	paramTuple->Fill(sector,row,pad,numPulse,coefZ[0],coefZ[1],coefZ[2],coefP[0],coefP[1],coefP[2]);
      }
      coefP->~Double_t();
      coefZ->~Double_t();
    }

  }

  fileIn.Close();
  paramTuple->Write();
  fileOut.Close();

}


//____________________________________________________________________________
Int_t AliTPCCalibTCF::AnalyzePulse(TH1F *hisIn, Double_t *coefZ, Double_t *coefP) {
  //
  // Performs the analysis on one specific pulse (histogram) by means of fitting
  // the pulse and equalization of the pulseheight. The found TCF parameters 
  // are stored in the arrays coefZ and coefP
  //

  Int_t pulseLength = hisIn->GetNbinsX() -4; 
  // -1 because the first four timebins usually contain pad specific informations
  Int_t npulse = (Int_t)hisIn->GetBinContent(1);
  Int_t sector = (Int_t)hisIn->GetBinContent(2);
  Int_t row = (Int_t)hisIn->GetBinContent(3);
  Int_t pad = (Int_t)hisIn->GetBinContent(4);
  
  // write pulseinformation to TNtuple and normalize to 100 ADC (because of 
  // given upper and lower fit parameter limits) in order to pass the pulse
  // to TMinuit

  TNtuple *dataTuple = new TNtuple("ntupleFit","Pulse","timebin:sigNorm:error");  
  Double_t error  = 0.05;
  Double_t max = hisIn->GetMaximum(FLT_MAX);
  for (Int_t ipos=0; ipos<pulseLength; ipos++) {
    Double_t errorz=error;
    if (ipos>100) { errorz = error*100; } // very simple weight: FIXME in case
    Double_t signal = hisIn->GetBinContent(ipos+5);
    Double_t signalNorm = signal/max*100; //pulseheight normaliz. to 100ADC
    dataTuple->Fill(ipos, signalNorm, errorz);
  }
   
  // Call fit function (TMinuit) to get the first 2 PZ Values for the 
  // Tail Cancelation Filter
  Int_t fitOk = FitPulse(dataTuple, coefZ, coefP);
 
  if (fitOk) {
    // calculates the 3rd set (remaining 2 PZ values) in order to restore the
    // original height of the pulse
    Equalization(dataTuple, coefZ, coefP);
      
    printf("Calculated TCF parameters for: \n");
    printf("Sector %d | Row %d | Pad %d |", sector, row, pad);
    printf(" Npulses: %d \n", npulse);
    for(Int_t i = 0; i < 3; i++){
      printf("P[%d] = %f     Z[%d] = %f \n",i,coefP[i],i,coefZ[i]);
      if (i==2) { printf("\n"); }
    }
    dataTuple->~TNtuple();
    return 1;
  } else { // fit did not converge
    Error("FindFit", "TCF fit not converged - pulse abandoned ");
    printf("in Sector %d | Row %d | Pad %d |", sector, row, pad);
    printf(" Npulses: %d \n\n", npulse);
    coefP[2] = 0; coefZ[2] = 0;
    dataTuple->~TNtuple();
    return 0;
  }
  
}



//____________________________________________________________________________
void AliTPCCalibTCF::TestTCFonRootFile(const char *nameFileIn, const char *nameFileTCF, Int_t plotFlag, Int_t lowKey, Int_t upKey)
{
  //
  // Performs quality parameters evaluation of the calculated TCF parameters in 
  // the file 'nameFileTCF' for every (accumulated) histogram within the 
  // prepeared root file 'nameFileIn'. 
  // The found quality parameters are stored in an TNtuple which will be saved
  // in a Root file 'Quality-*'. 
  // If the parameter for the given pulse (given pad) was not found, the pulse 
  // is rejected.
  //

  TFile fileIn(nameFileIn,"READ");

  Double_t* coefP = new Double_t[3];
  Double_t* coefZ = new Double_t[3];
  for(Int_t i = 0; i < 3; i++){
    coefP[i] = 0;
    coefZ[i] = 0;
  }

  char nameFileOut[100];
  sprintf(nameFileOut,"Quality_%s_AT_%s",nameFileTCF, nameFileIn);
  TFile fileOut(nameFileOut,"RECREATE");

  TNtuple *qualityTuple = new TNtuple("TCFquality","TCF quality Values","sec:row:pad:npulse:heightDev:areaRed:widthRed:undershot:maxUndershot");
 
  TH1F *hisIn;
  TKey *key;
  TIter next( fileIn.GetListOfKeys() );

  Int_t nHist = fileIn.GetNkeys();
  Int_t iHist = 0;
  
  for(Int_t i=0;i<lowKey-1;i++){++iHist; key = (TKey *) next();}
  while ((key = (TKey *) next())) { // loop over saved histograms
    
    //  loading pulse to memory;
    printf("validating pulse %d out of %d\n",++iHist,nHist);
    hisIn = (TH1F*)fileIn.Get(key->GetName()); 

    // find the correct TCF parameter according to the his infos (first 4 bins)
    Int_t nPulse = FindCorTCFparam(hisIn, nameFileTCF, coefZ, coefP); 
    if (nPulse) {  // doing the TCF quality analysis 
      Double_t *quVal = GetQualityOfTCF(hisIn,coefZ,coefP, plotFlag);
      Int_t sector = (Int_t)hisIn->GetBinContent(2);
      Int_t row = (Int_t)hisIn->GetBinContent(3);
      Int_t pad = (Int_t)hisIn->GetBinContent(4);      
      qualityTuple->Fill(sector,row,pad,nPulse,quVal[0],quVal[1],quVal[2],quVal[3],quVal[4],quVal[5]);
      quVal->~Double_t();
    }
    
    if (iHist>=upKey) {break;}
    
  }

  fileOut.cd();
  qualityTuple->Write();

  coefP->~Double_t();
  coefZ->~Double_t();

  fileOut.Close();
  fileIn.Close();

}



//_____________________________________________________________________________
void AliTPCCalibTCF::TestTCFonRawFile(const char *nameRawFile, const char *nameFileOut, const char *nameFileTCF, Int_t plotFlag) {
  //
  // Performs quality parameters evaluation of the calculated TCF parameters in 
  // the file 'nameFileTCF' for every proper pulse (according to given thresholds)
  // within the RAW file 'nameRawFile'. 
  // The found quality parameters are stored in a TNtuple which will be saved
  // in the Root file 'nameFileOut'. If the parameter for the given pulse 
  // (given pad) was not found, the pulse is rejected.
  //

  //
  // Reads a RAW data file, extracts Pulses (according the given tresholds)
  // and test the found TCF parameters on them ...
  // 
  
  AliRawReader *rawReader = new AliRawReaderRoot(nameRawFile);
  rawReader->Reset();
        
  Double_t* coefP = new Double_t[3];
  Double_t* coefZ = new Double_t[3];
  for(Int_t i = 0; i < 3; i++){
    coefP[i] = 0;
    coefZ[i] = 0;
  }

  while ( rawReader->NextEvent() ){

    printf("Reading next event...");
    AliTPCRawStream rawStream(rawReader);
    rawReader->Select("TPC");

    Int_t sector = rawStream.GetSector();
    Int_t row    = rawStream.GetRow();
    Int_t prevTime = 999999;
    Int_t prevPad = 999999;
    
    TH1I *tempHis = new TH1I("tempHis","tempHis",fSample+fGateWidth,fGateWidth,fSample+fGateWidth);
    TH1I *tempRMSHis = new TH1I("tempRMSHis","tempRMSHis",2000,0,2000);
    
    TFile fileOut(nameFileOut,"UPDATE"); // Quality Parameters storage
    TNtuple *qualityTuple = (TNtuple*)fileOut.Get("TCFquality");
    if (!qualityTuple) { // no entry in file
      qualityTuple = new TNtuple("TCFquality","TCF quality Values","sec:row:pad:npulse:heightDev:areaRed:widthRed:undershot:maxUndershot:pulseRMS");
    }

    while (rawStream.Next()) {
    
      if (rawStream.IsNewRow()){
	sector = rawStream.GetSector();
	row    = rawStream.GetRow();
      }
      
      Int_t pad = rawStream.GetPad();
      Int_t time = rawStream.GetTime();
      Int_t signal = rawStream.GetSignal();
      
      if (!rawStream.IsNewPad()) { // Reading signal from one Pad 
	if (time>prevTime) {
	  printf("Wrong time: %d %d\n",rawStream.GetTime(),prevTime);
	  rawStream.Dump();
	} else {
	  if (time<=fSample+fGateWidth && time>fGateWidth) {
	    tempHis->SetBinContent(time,signal);
	  }
	}      
      } else { // Decision for saving pulse according to treshold settings
   
	Int_t max = (Int_t)tempHis->GetMaximum(FLT_MAX);
	Int_t maxpos =  tempHis->GetMaximumBin();
	
	Int_t first = (Int_t)TMath::Max(maxpos-10, 0);
	Int_t last  = TMath::Min((Int_t)maxpos+fPulseLength-10, fSample);
	

	// simple baseline substraction ? better one needed ? (pedestalsubstr.?)
	// and RMS calculation with timebins before the pulse and at the end of
	// the signal
	for (Int_t ipos = 0; ipos<6; ipos++) {
	  // before the pulse
	  tempRMSHis->Fill(tempHis->GetBinContent(first+ipos));
	  // at the end to get rid of pulses with serious baseline fluctuations
	  tempRMSHis->Fill(tempHis->GetBinContent(last-ipos));
	}
	Double_t baseline = tempRMSHis->GetMean();
	Double_t rms = tempRMSHis->GetRMS();
	tempRMSHis->Reset();
	
	Double_t lowLim = fLowPulseLim+baseline;
	Double_t upLim = fUpPulseLim+baseline;
	
	// Decision if found pulse is a proper one according to given tresholds
	if (max>lowLim && max<upLim && !((last-first)<fPulseLength) && rms<fRMSLim){
	  // note:
	  // assuming that lowLim is higher than the pedestal value!
	  char hname[100];
	  sprintf(hname,"sec%drow%dpad%d",sector,row,prevPad);
	  TH1F *his = new TH1F(hname,hname, fPulseLength+4, 0, fPulseLength+4);
	  his->SetBinContent(1,1); //  pulse counter (1st pulse)
	  his->SetBinContent(2,sector); //  sector
	  his->SetBinContent(3,row);    //  row
	  his->SetBinContent(4,prevPad);    //  pad
	  for (Int_t ipos=0; ipos<last-first; ipos++){
	   Int_t signal = (Int_t)(tempHis->GetBinContent(ipos+first)-baseline);
	   his->SetBinContent(ipos+5,signal);
	  }
	    
	  printf("Pulse found in %s: ADC %d at bin %d \n", hname, max, maxpos+fGateWidth);

	  // find the correct TCF parameter according to the his infos 
	  // (first 4 bins)
	  Int_t nPulse = FindCorTCFparam(his, nameFileTCF, coefZ, coefP);

	  if (nPulse) {  // Parameters found - doing the TCF quality analysis
	    Double_t *quVal = GetQualityOfTCF(his,coefZ,coefP, plotFlag);
	    qualityTuple->Fill(sector,row,pad,nPulse,quVal[0],quVal[1],quVal[2],quVal[3],quVal[4],quVal[5]);
	    quVal->~Double_t();
	  }
	  his->~TH1F();
	}
	tempHis->Reset();
      }
      prevTime = time;
      prevPad = pad;
    }   

    tempHis->~TH1I();
    tempRMSHis->~TH1I();

    printf("Finished to read event - close output file ... \n");
   
    fileOut.cd();
    qualityTuple->Write("TCFquality",kOverwrite);
    fileOut.Close();



  } // event loop


  coefP->~Double_t();
  coefZ->~Double_t();

  rawReader->~AliRawReader();
  
}


//____________________________________________________________________________
TNtuple *AliTPCCalibTCF::PlotOccupSummary(const char *nameFile, Int_t nPulseMin) {
  //
  // Plots the number of summed pulses per pad above a given minimum at the 
  // pad position
  // 'nameFile': root-file created with the Process function
  //

  TFile *file = new TFile(nameFile,"READ");

  TH1F *his;
  TKey *key;
  TIter next( file->GetListOfKeys() );

  TNtuple *ntuple = new TNtuple("ntuple","ntuple","x:y:z:npulse");

  Int_t nPads = 0;
  while ((key = (TKey *) next())) { // loop over histograms within the file

    his = (TH1F*)file->Get(key->GetName()); // copy object to memory

    Int_t npulse = (Int_t)his->GetBinContent(1);
    Int_t sec = (Int_t)his->GetBinContent(2);
    Int_t row = (Int_t)his->GetBinContent(3);
    Int_t pad = (Int_t)his->GetBinContent(4);

    if (row==-1 & pad==-1) { // summed pulses per sector
      row = 40; pad = 40;    // set to approx middle row for better plot
    }

    Float_t *pos = new Float_t[3];
    // find x,y,z position of the pad
    AliTPCROC::Instance()->GetPositionGlobal(sec,row,pad,pos); 
    if (npulse>=nPulseMin) { 
      ntuple->Fill(pos[0],pos[1],pos[2],npulse);
      printf("%d collected pulses in sector %d row %d pad %d\n",npulse,sec,row,pad);
    }
    pos->~Float_t();
    nPads++;
  }
 
  TCanvas *c1 = new TCanvas("TCanvas","Number of pulses found",1000,500);
  c1->Divide(2,1);
  char cSel[100];
  gStyle->SetPalette(1);
  gStyle->SetLabelOffset(-0.03,"Z");

  if (nPads<72) { // pulse per pad
    ntuple->SetMarkerStyle(8);
    ntuple->SetMarkerSize(4);
  } else {        // pulse per sector
    ntuple->SetMarkerStyle(7);
  }

  c1->cd(1);
  sprintf(cSel,"z>0&&npulse>=%d",nPulseMin);
  ntuple->Draw("y:x:npulse",cSel,"colz");
  gPad->SetTitle("A side");

  c1->cd(2);
  sprintf(cSel,"z<0&&npulse>%d",nPulseMin);
  ntuple->Draw("y:x:npulse",cSel,"colz");
  gPad->SetTitle("C side");

  file->Close();
  return ntuple;

}

//____________________________________________________________________________
void AliTPCCalibTCF::PlotQualitySummary(const char *nameFileQuality, const char *plotSpec, const char *cut, const char *pOpt)
{
  // 
  // This function is an easy interface to load the QualityTuple (produced with
  // the function 'TestOn%File' and plots them according to the plot specifications
  // 'plotSpec' e.g. "widthRed:maxUndershot"
  // One may also set cut and plot options ("cut","pOpt") 
  //
  // The stored quality parameters are ...
  //   sec:row:pad:npulse: ... usual pad info
  //   heightDev ... height deviation in percent
  //   areaRed ... area reduction in percent
  //   widthRed ... width reduction in percent
  //   undershot ... mean undershot after the pulse in ADC
  //   maxUndershot ... maximum of the undershot after the pulse in ADC
  //   pulseRMS ... RMS of the pulse used to calculate the Quality parameters in ADC
  //

  TFile file(nameFileQuality,"READ");
  TNtuple *qualityTuple = (TNtuple*)file.Get("TCFquality");
  gStyle->SetPalette(1);
  qualityTuple->Draw(plotSpec,cut,pOpt);
  
}

//____________________________________________________________________________
void AliTPCCalibTCF::DumpTCFparamToFile(const char *nameFileTCF,const char *nameFileOut)
{
  //
  // Writes the TCF parameters from file 'nameFileTCF' to a output file
  //

  // Note: currently just TCF parameters per Sector or TCF parameters for pad 
  //       which were analyzed. There is no method included so far to export
  //       parameters for not analyzed pad, which means there are eventually
  //       missing TCF parameters
  //   TODO: carefull! Fill up missing pads with averaged (sector) values?


  // open file with TCF parameters
  TFile fileTCF(nameFileTCF,"READ");
  TNtuple *paramTuple = (TNtuple*)fileTCF.Get("TCFparam");
  
  // open output txt file ...
  FILE *output;
  output=fopen(nameFileOut,"w");      // open outfile.

  // Header line
  Int_t sectorWise =  paramTuple->GetEntries("row==-1&&pad==-1");
  if (sectorWise) {
    fprintf(output,"sector \t  Z0 \t\t Z1 \t\t Z2 \t\t P0 \t\t P1 \t\t P2\n"); 
  } else {
    fprintf(output,"sector \t row \t pad  \t Z0 \t\t Z1 \t\t Z2 \t\t P0 \t\t P1 \t\t P2\n");  
  }
  
  for (Int_t i=0; i<paramTuple->GetEntries(); i++) {
    paramTuple->GetEntry(i);
    Float_t *p = paramTuple->GetArgs();
    
    // _______________________________________________________________
    // to Tuple to txt file - unsorted printout
    
    for (Int_t i=0; i<10; i++){
      if (sectorWise) {
	if (i<1)  fprintf(output,"%3.0f \t ",p[i]);    // sector info
	if (i>3)  fprintf(output,"%1.4f \t ",p[i]);    // TCF param
      } else {
	if (i<3)  fprintf(output,"%3.0f \t ",p[i]);    // pad info
	if (i>3)  fprintf(output,"%1.4f \t ",p[i]);    // TCF param
      }  	    
    }
    fprintf(output,"\n");
  }

  // close output txt file
  fprintf(output,"\n");
  fclose(output);
  
  fileTCF.Close();


}
  


//_____________________________________________________________________________
Int_t AliTPCCalibTCF::FitPulse(TNtuple *dataTuple, Double_t *coefZ, Double_t *coefP) {
  //
  // function to fit one pulse and to calculate the according pole-zero parameters
  //
 
  // initialize TMinuit with a maximum of 8 params
  TMinuit *gMinuit = new TMinuit(8);
  gMinuit->mncler();                    // Reset Minuit's list of paramters
  gMinuit->SetPrintLevel(-1);           // No Printout
  gMinuit->SetFCN(AliTPCCalibTCF::FitFcn); // To set the address of the 
                                           // minimization function  
  gMinuit->SetObjectFit(dataTuple);
  
  Double_t arglist[10];
  Int_t ierflg = 0;
  
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
  
  // Set standard starting values and step sizes for each parameter
  // upper and lower limit (in a reasonable range) are set to improve 
  // the stability of TMinuit
  static Double_t vstart[8] = {125, 4.0, 0.3, 0.5, 5.5, 100,    1, 2.24};
  static Double_t step[8]   = {0.1, 0.1,  0.1, 0.1, 0.1, 0.1,  0.1,  0.1};
  static Double_t min[8]    = {100,  3.,  0.1, 0.2,  3.,  60.,  0.,  2.0};
  static Double_t max[8]    = {200, 20.,   5.,  3., 30., 300., 20., 2.5};
  
  gMinuit->mnparm(0, "A1", vstart[0], step[0], min[0], max[0], ierflg);
  gMinuit->mnparm(1, "A2", vstart[1], step[1], min[1], max[1], ierflg);
  gMinuit->mnparm(2, "A3", vstart[2], step[2], min[2], max[2], ierflg);
  gMinuit->mnparm(3, "T1", vstart[3], step[3], min[3], max[3], ierflg);
  gMinuit->mnparm(4, "T2", vstart[4], step[4], min[4], max[4], ierflg);
  gMinuit->mnparm(5, "T3", vstart[5], step[5], min[5], max[5], ierflg);
  gMinuit->mnparm(6, "T0", vstart[6], step[6], min[6], max[6], ierflg);
  gMinuit->mnparm(7, "TTP", vstart[7], step[7], min[7], max[7],ierflg);
  gMinuit->FixParameter(7); // 2.24 ... out of pulserRun Fit (->IRF)

  // Now ready for minimization step
  arglist[0] = 2000;   // max num of iterations
  arglist[1] = 0.1;    // tolerance

  gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);
  
  Double_t p1 = 0.0 ;
  gMinuit->mnexcm("SET NOW", &p1 , 0, ierflg) ;  // No Warnings
  
  if (ierflg == 4) { // Fit failed
    for (Int_t i=0;i<3;i++) { 
      coefP[i] = 0; 
      coefZ[i] = 0; 
    }
    gMinuit->~TMinuit();
    return 0;
  } else { // Fit successfull

    // Extract parameters from TMinuit
    Double_t *fitParam = new Double_t[6];
    for (Int_t i=0;i<6;i++) {
      Double_t err = 0;
      Double_t val = 0;
      gMinuit->GetParameter(i,val,err);
      fitParam[i] = val;
    } 
    
    // calculates the first 2 sets (4 PZ values) out of the fitted parameters
    Double_t *valuePZ = ExtractPZValues(fitParam);
   
    // TCF coefficients which are used for the equalisation step (stage)
    // ZERO/POLE Filter
    coefZ[0] = TMath::Exp(-1/valuePZ[2]);
    coefZ[1] = TMath::Exp(-1/valuePZ[3]);
    coefP[0] = TMath::Exp(-1/valuePZ[0]);
    coefP[1] = TMath::Exp(-1/valuePZ[1]);
   
    fitParam->~Double_t();
    valuePZ->~Double_t();
    gMinuit->~TMinuit();

    return 1;

  }

}


//____________________________________________________________________________
void AliTPCCalibTCF::FitFcn(Int_t &/*nPar*/, Double_t */*grad*/, Double_t &f, Double_t *par, Int_t /*iflag*/)
{
  //
  // Minimization function needed for TMinuit with FitFunction included 
  // Fit function: Sum of three convolution terms (IRF conv. with Exp.)
  //

  // Get Data ...
  TNtuple *dataTuple = (TNtuple *) gMinuit->GetObjectFit();

  //calculate chisquare
  Double_t chisq = 0;
  Double_t delta = 0;
  for (Int_t i=0; i<dataTuple->GetEntries(); i++) { // loop over data points
    dataTuple->GetEntry(i);
    Float_t *p = dataTuple->GetArgs();
    Double_t t = p[0];
    Double_t signal = p[1];   // Normalized signal
    Double_t error = p[2]; 

    // definition and evaluation if the IonTail specific fit function
    Double_t sigFit = 0;
    
    Double_t ttp = par[7];   // signal shaper raising time
    t=t-par[6];              // time adjustment
    
    if (t<0) {
      sigFit = 0;
    } else {
      Double_t f1 = 1/TMath::Power((4-ttp/par[3]),5)*(24*ttp*TMath::Exp(4)*(TMath::Exp(-t/par[3]) - TMath::Exp(-4*t/ttp) * ( 1+t*(4-ttp/par[3])/ttp+TMath::Power(t*(4-ttp/par[3])/ttp,2)/2 + TMath::Power(t*(4-ttp/par[3])/ttp,3)/6 + TMath::Power(t*(4-ttp/par[3])/ttp,4)/24)));
      
      Double_t f2 = 1/TMath::Power((4-ttp/par[4]),5)*(24*ttp*TMath::Exp(4)*(TMath::Exp(-t/par[4]) - TMath::Exp(-4*t/ttp) * ( 1+t*(4-ttp/par[4])/ttp+TMath::Power(t*(4-ttp/par[4])/ttp,2)/2 + TMath::Power(t*(4-ttp/par[4])/ttp,3)/6 + TMath::Power(t*(4-ttp/par[4])/ttp,4)/24)));
      
      Double_t f3 = 1/TMath::Power((4-ttp/par[5]),5)*(24*ttp*TMath::Exp(4)*(TMath::Exp(-t/par[5]) - TMath::Exp(-4*t/ttp) * ( 1+t*(4-ttp/par[5])/ttp+TMath::Power(t*(4-ttp/par[5])/ttp,2)/2 + TMath::Power(t*(4-ttp/par[5])/ttp,3)/6 + TMath::Power(t*(4-ttp/par[5])/ttp,4)/24)));
      
      sigFit = par[0]*f1 + par[1]*f2 +par[2]*f3;
    }

    // chisqu calculation
    delta  = (signal-sigFit)/error;
    chisq += delta*delta;
  }

  f = chisq;

}



//____________________________________________________________________________
Double_t* AliTPCCalibTCF::ExtractPZValues(Double_t *param) {
  //
  // Calculation of Pole and Zero values out of fit parameters
  //

  Double_t vA1, vA2, vA3, vTT1, vTT2, vTT3, vTa, vTb;
  vA1 = 0;  vA2 = 0;  vA3 = 0;
  vTT1 = 0; vTT2 = 0; vTT3 = 0;
  vTa = 0; vTb = 0;
  
  // nasty method of sorting the fit parameters to avoid wrong mapping
  // to the different stages of the TCF filter
  // (e.g. first 2 fit parameters represent the electron signal itself!)

  if (param[3]==param[4]) {param[3]=param[3]+0.0001;}
  if (param[5]==param[4]) {param[5]=param[5]+0.0001;}
  
  if ((param[5]>param[4])&(param[5]>param[3])) {
    if (param[4]>=param[3]) {
      vA1 = param[0];  vA2 = param[1];  vA3 = param[2];
      vTT1 = param[3]; vTT2 = param[4]; vTT3 = param[5];
    } else {
      vA1 = param[1];  vA2 = param[0];  vA3 = param[2];
      vTT1 = param[4]; vTT2 = param[3]; vTT3 = param[5];
    }
  } else if ((param[4]>param[5])&(param[4]>param[3])) {
    if (param[5]>=param[3]) {
      vA1 = param[0];  vA2 = param[2];  vA3 = param[1];
      vTT1 = param[3]; vTT2 = param[5]; vTT3 = param[4];
    } else {
      vA1 = param[2];  vA2 = param[0];  vA3 = param[1];
      vTT1 = param[5]; vTT2 = param[3]; vTT3 = param[4];
    }
  } else if ((param[3]>param[4])&(param[3]>param[5])) {
    if (param[5]>=param[4]) {
      vA1 = param[1];  vA2 = param[2];  vA3 = param[0];
      vTT1 = param[4]; vTT2 = param[5]; vTT3 = param[3];
    } else {
      vA1 = param[2];  vA2 = param[1];  vA3 = param[0];
      vTT1 = param[5]; vTT2 = param[4]; vTT3 = param[3];
    }    
  }
  

  // Transformation of fit parameters into PZ values (needed by TCF) 
  Double_t beq = (vA1/vTT2+vA1/vTT3+vA2/vTT1+vA2/vTT3+vA3/vTT1+vA3/vTT2)/(vA1+vA2+vA3);
  Double_t ceq = (vA1/(vTT2*vTT3)+vA2/(vTT1*vTT3)+vA3/(vTT1*vTT2))/(vA1+vA2+vA3);
  
  Double_t  s1 = -beq/2-sqrt((beq*beq-4*ceq)/4);
  Double_t  s2 = -beq/2+sqrt((beq*beq-4*ceq)/4);
  
  if (vTT2<vTT3) {// not necessary but avoids significant undershots in first PZ 
    vTa = -1/s1;
    vTb = -1/s2;
  }else{ 
    vTa = -1/s2;
    vTb = -1/s1;
  }
    
  Double_t *valuePZ = new Double_t[4];
  valuePZ[0]=vTa;
  valuePZ[1]=vTb;
  valuePZ[2]=vTT2;
  valuePZ[3]=vTT3;
      
  return valuePZ;
  
}


//____________________________________________________________________________
void AliTPCCalibTCF::Equalization(TNtuple *dataTuple, Double_t *coefZ, Double_t *coefP) {
  //
  // calculates the 3rd set of TCF parameters (remaining 2 PZ values) in 
  // order to restore the original pulse height and adds them to the passed arrays
  //

  Double_t *s0 = new Double_t[1000]; // original pulse
  Double_t *s1 = new Double_t[1000]; // pulse after 1st PZ filter
  Double_t *s2 = new Double_t[1000]; // pulse after 2nd PZ filter

  const Int_t kPulseLength = dataTuple->GetEntries();
  
  for (Int_t ipos=0; ipos<kPulseLength; ipos++) {
    dataTuple->GetEntry(ipos);
    Float_t *p = dataTuple->GetArgs();
    s0[ipos] = p[1]; 
  }
  
  // non-discret implementation of the first two TCF stages (recursive formula)
  // discrete Altro emulator is not used because of accuracy!
  s1[0] = s0[0]; // 1st PZ filter
  for(Int_t ipos = 1; ipos < kPulseLength ; ipos++){
    s1[ipos] = s0[ipos] + coefP[0]*s1[ipos-1] - coefZ[0]*s0[ipos-1];
  }
  s2[0] = s1[0]; // 2nd PZ filter
  for(Int_t ipos = 1; ipos < kPulseLength ; ipos++){
    s2[ipos] = s1[ipos] + coefP[1]*s2[ipos-1] - coefZ[1]*s1[ipos-1];
  }
  
  // find maximum amplitude and position of original pulse and pulse after 
  // the first two stages of the TCF 
  Int_t s0pos = 0, s2pos = 0; 
  Double_t s0ampl = s0[0], s2ampl = s2[0]; // start values
  for(Int_t ipos = 1; ipos < kPulseLength; ipos++){
    if (s0[ipos] > s0ampl){
      s0ampl = s0[ipos]; 
      s0pos = ipos;      // should be pos 11 ... check?
    }
    if (s2[ipos] > s2ampl){
      s2ampl = s2[ipos];
      s2pos = ipos;
    }    
  }
  // calculation of 3rd set ...
  if(s0ampl > s2ampl){
    coefZ[2] = 0;
    coefP[2] = (s0ampl - s2ampl)/s0[s0pos-1];
  } else if (s0ampl < s2ampl) {
    coefP[2] = 0;
    coefZ[2] = (s2ampl - s0ampl)/s0[s0pos-1];
  } else { // same height ? will most likely not happen ?
    coefP[2] = 0;
    coefZ[2] = 0;
  }

  s0->~Double_t();
  s1->~Double_t();
  s2->~Double_t();
   
}



//____________________________________________________________________________
Int_t AliTPCCalibTCF::FindCorTCFparam(TH1F *hisIn, const char *nameFileTCF, Double_t *coefZ, Double_t *coefP) {
  //
  // This function searches for the correct TCF parameters to the given
  // histogram 'hisIn' within the file 'nameFileTCF' 
  // If no parameters for this pad (padinfo within the histogram!) where found
  // the function returns 0

  //  Int_t numPulse = (Int_t)hisIn->GetBinContent(1); // number of pulses
  Int_t sector = (Int_t)hisIn->GetBinContent(2);
  Int_t row = (Int_t)hisIn->GetBinContent(3);
  Int_t pad = (Int_t)hisIn->GetBinContent(4);
  Int_t nPulse = 0; 

  //-- searching for calculated TCF parameters for this pad/sector
  TFile fileTCF(nameFileTCF,"READ");
  TNtuple *paramTuple = (TNtuple*)fileTCF.Get("TCFparam");

  // create selection criteria to find the correct TCF params
  char sel[100];   
  if ( paramTuple->GetEntries("row==-1&&pad==-1") ) { 
    // parameters per SECTOR
    sprintf(sel,"sec==%d&&row==-1&&pad==-1",sector);
  } else {            
    // parameters per PAD
    sprintf(sel,"sec==%d&&row==%d&&pad==%d",sector,row,pad);
  }

  // list should contain just ONE entry! ... otherwise there is a mistake!
  Long64_t entry = paramTuple->Draw(">>list",sel,"entrylist");
  TEntryList *list = (TEntryList*)gDirectory->Get("list");
  
  if (entry) { // TCF set was found for this pad
    Long64_t pos = list->GetEntry(0);
    paramTuple->GetEntry(pos);   // get specific TCF parameters       
    Float_t *p = paramTuple->GetArgs();
    // check ...
    if(sector==p[0]) {printf("sector ok ... "); }          
    if(row==p[1]) {printf("row ok ... "); }          
    if(pad==p[2]) {printf("pad ok ... \n"); }          
    
    // number of averaged pulses used to produce TCF params
    nPulse = (Int_t)p[3]; 
    // TCF parameters
    coefZ[0] = p[4];  coefP[0] = p[7];
    coefZ[1] = p[5];  coefP[1] = p[8];
    coefZ[2] = p[6];  coefP[2] = p[9];
      
  } else { // no specific TCF parameters found for this pad 
    
    printf("no specific TCF paramaters found for pad in ...\n");
    printf("in Sector %d | Row %d | Pad %d |\n", sector, row, pad);
    nPulse = 0;
    coefZ[0] = 0;  coefP[0] = 0;
    coefZ[1] = 0;  coefP[1] = 0;
    coefZ[2] = 0;  coefP[2] = 0;

  }

  fileTCF.Close();

  return nPulse; // number of averaged pulses for producing the TCF params
  
}


//____________________________________________________________________________
Double_t *AliTPCCalibTCF::GetQualityOfTCF(TH1F *hisIn, Double_t *coefZ, Double_t *coefP, Int_t plotFlag) {
  //
  // This function evaluates the quality parameters of the given TCF parameters
  // tested on the passed pulse (hisIn)
  // The quality parameters are stored in an array. They are ...
  //    height deviation [ADC]
  //    area reduction [percent]
  //    width reduction [percent]
  //    mean undershot [ADC]
  //    maximum of undershot after pulse [ADC]
  //    Pulse RMS [ADC]

  // perform ALTRO emulator
  TNtuple *pulseTuple = ApplyTCFilter(hisIn, coefZ, coefP, plotFlag); 

  printf("calculate quality val. for pulse in ... ");
  printf(" Sector %d | Row %d | Pad %d |\n", (Int_t)hisIn->GetBinContent(2),  (Int_t)hisIn->GetBinContent(3), (Int_t)hisIn->GetBinContent(4));
  
  // Reasonable limit for the calculation of the quality values
  Int_t binLimit = 80; 
  
  // ============== Variable preparation

  // -- height difference in percent of orginal pulse
  Double_t maxSig = pulseTuple->GetMaximum("sig");
  Double_t maxSigTCF = pulseTuple->GetMaximum("sigAfterTCF");      
  // -- area reduction (above zero!)
  Double_t area = 0;
  Double_t areaTCF = 0;    
  // -- width reduction at certain ADC treshold
  // TODO: set treshold at ZS treshold? (3 sigmas of noise?)
  Int_t threshold = 3; // treshold in percent
  Int_t threshADC = (Int_t)(maxSig/100*threshold);  
  Int_t startOfPulse = 0;   Int_t startOfPulseTCF = 0;
  Int_t posOfStart = 0;     Int_t posOfStartTCF = 0;
  Int_t widthFound = 0;     Int_t widthFoundTCF = 0;
  Int_t width = 0;          Int_t widthTCF = 0;
  // -- Calcluation of Undershot (mean of negavive signal after the first 
  // undershot)
  Double_t undershotTCF = 0;  
  Double_t undershotStart = 0;
  // -- Calcluation of Undershot (Sum of negative signal after the pulse)
  Double_t maxUndershot = 0;


  // === loop over timebins to calculate quality parameters
  for (Int_t i=0; i<binLimit; i++) {
   
    // Read signal values
    pulseTuple->GetEntry(i); 
    Float_t *p = pulseTuple->GetArgs();
    Double_t sig = p[1]; 
    Double_t sigTCF = p[2];

    // calculation of area (above zero)
    if (sig>0) {area += sig; }
    if (sigTCF>0) {areaTCF += sigTCF; }
    

    // Search for width at certain ADC treshold 
    // -- original signal
    if (widthFound == 0) {
      if( (sig > threshADC) && (startOfPulse == 0) ){
	startOfPulse = 1;
	posOfStart = i;
      }
      if( (sig < threshADC) && (startOfPulse == 1) ){
	widthFound = 1;
	width = i - posOfStart + 1;	
      }
    }
    // -- signal after TCF
    if (widthFoundTCF == 0) {
      if( (sigTCF > threshADC) && (startOfPulseTCF == 0) ){
	startOfPulseTCF = 1;
	posOfStartTCF = i;
      }
      if( (sigTCF < threshADC) && (startOfPulseTCF == 1) ){
	widthFoundTCF = 1;
	widthTCF = i -posOfStartTCF + 1;
      }
      
    }
      
    // finds undershot start
    if  ( (widthFoundTCF==1) && (sigTCF<0) ) {
      undershotStart = 1;
    }

    // Calculation of undershot sum (after pulse)
    if ( widthFoundTCF==1 ) {
      undershotTCF += sigTCF; 
    }

    // Search for maximal undershot (is equal to minimum after the pulse)
    if ( (undershotStart==1)&&(i<(posOfStartTCF+widthTCF+20)) ) {
      if (maxUndershot>sigTCF) { maxUndershot = sigTCF; }
    }

  }  

  // ==  Calculation of Quality parameters

  // -- height difference in ADC
  Double_t heightDev = maxSigTCF-maxSig; 

  // Area reduction of the pulse in percent
  Double_t areaReduct = 100-areaTCF/area*100; 

  // Width reduction in percent
  Double_t widthReduct = 0;
  if ((widthFound==1)&&(widthFoundTCF==1)) { // in case of not too big IonTail 
    widthReduct = 100-(Double_t)widthTCF/(Double_t)width*100; 
    if (widthReduct<0) { widthReduct = 0;}  
  }

  // Undershot - mean of neg.signals after pulse
  Double_t length = 1;
  if (binLimit-widthTCF-posOfStartTCF) { length = (binLimit-widthTCF-posOfStartTCF);}
  Double_t undershot = undershotTCF/length; 


  // calculation of pulse RMS with timebins before and at the end of the pulse
  TH1I *tempRMSHis = new TH1I("tempRMSHis","tempRMSHis",100,-50,50);
  for (Int_t ipos = 0; ipos<6; ipos++) {
    // before the pulse
    tempRMSHis->Fill(hisIn->GetBinContent(ipos+5));
    // at the end 
    tempRMSHis->Fill(hisIn->GetBinContent(hisIn->GetNbinsX()-ipos));
  }
  Double_t pulseRMS = tempRMSHis->GetRMS();
  tempRMSHis->~TH1I();
  
  if (plotFlag) {
    // == Output 
    printf("height deviation [ADC]:\t\t\t %3.1f\n", heightDev);
    printf("area reduction [percent]:\t\t %3.1f\n", areaReduct);
    printf("width reduction [percent]:\t\t %3.1f\n", widthReduct);
    printf("mean undershot [ADC]:\t\t\t %3.1f\n", undershot);
    printf("maximum of undershot after pulse [ADC]: %3.1f\n", maxUndershot);
    printf("RMS of the original pulse [ADC]: \t %3.2f\n\n", pulseRMS);

  }

  Double_t *qualityParam = new Double_t[6];
  qualityParam[0] = heightDev;
  qualityParam[1] = areaReduct;
  qualityParam[2] = widthReduct;
  qualityParam[3] = undershot;
  qualityParam[4] = maxUndershot;
  qualityParam[5] = pulseRMS;

  pulseTuple->~TNtuple();

  return qualityParam;
}


//____________________________________________________________________________
TNtuple *AliTPCCalibTCF::ApplyTCFilter(TH1F *hisIn, Double_t *coefZ, Double_t *coefP, Int_t plotFlag) {
  //
  // Applies the given TCF parameters on the given pulse via the ALTRO emulator 
  // class (discret values) and stores both pulses into a returned TNtuple
  //

  Int_t nbins = hisIn->GetNbinsX() -4; 
  // -1 because the first four timebins usually contain pad specific informations  
  Int_t nPulse = (Int_t)hisIn->GetBinContent(1); // Number of summed pulses
  Int_t sector = (Int_t)hisIn->GetBinContent(2);
  Int_t row = (Int_t)hisIn->GetBinContent(3);
  Int_t pad = (Int_t)hisIn->GetBinContent(4);
 
  // redirect histogram values to arrays (discrete for altro emulator)
  Double_t *signalIn = new Double_t[nbins];
  Double_t *signalOut = new Double_t[nbins];
  short *signalInD = new short[nbins]; 
  short *signalOutD = new short[nbins];
  for (Int_t ipos=0;ipos<nbins;ipos++) {
    Double_t signal = hisIn->GetBinContent(ipos+5); // summed signal
    signalIn[ipos]=signal/nPulse;                 // mean signal
    signalInD[ipos]=(short)(TMath::Nint(signalIn[ipos])); //discrete mean signal 
    signalOutD[ipos]=signalInD[ipos];    // will be overwritten by AltroEmulator    
  }

  // transform TCF parameters into ALTRO readable format (Integer)
  Int_t* valP = new Int_t[3];
  Int_t* valZ = new Int_t[3];
  for (Int_t i=0; i<3; i++) {
    valP[i] = (Int_t)(coefP[i]*(TMath::Power(2,16)-1));
    valZ[i] = (Int_t)(coefZ[i]*(TMath::Power(2,16)-1));
  }
    
  // discret ALTRO EMULATOR ____________________________
  AliTPCAltroEmulator *altro = new AliTPCAltroEmulator(nbins, signalOutD);
  altro->ConfigAltro(0,1,0,0,0,0); // perform just the TailCancelation
  altro->ConfigTailCancellationFilter(valP[0],valP[1],valP[2],valZ[0],valZ[1],valZ[2]);
  altro->RunEmulation();
  delete altro;
  
  // non-discret implementation of the (recursive formula)
  // discrete Altro emulator is not used because of accuracy!
  Double_t *s1 = new Double_t[1000]; // pulse after 1st PZ filter
  Double_t *s2 = new Double_t[1000]; // pulse after 2nd PZ filter
  s1[0] = signalIn[0]; // 1st PZ filter
  for(Int_t ipos = 1; ipos<nbins; ipos++){
    s1[ipos] = signalIn[ipos] + coefP[0]*s1[ipos-1] - coefZ[0]*signalIn[ipos-1];
  }
  s2[0] = s1[0]; // 2nd PZ filter
  for(Int_t ipos = 1; ipos<nbins; ipos++){
    s2[ipos] = s1[ipos] + coefP[1]*s2[ipos-1] - coefZ[1]*s1[ipos-1];
  }
  signalOut[0] = s2[0]; // 3rd PZ filter
  for(Int_t ipos = 1; ipos<nbins; ipos++){
    signalOut[ipos] = s2[ipos] + coefP[2]*signalOut[ipos-1] - coefZ[2]*s2[ipos-1];
  }
  s1->~Double_t();
  s2->~Double_t();

  // writing pulses to tuple
  TNtuple *pulseTuple = new TNtuple("ntupleTCF","PulseTCF","timebin:sig:sigAfterTCF:sigND:sigNDAfterTCF");
  for (Int_t ipos=0;ipos<nbins;ipos++) {
    pulseTuple->Fill(ipos,signalInD[ipos],signalOutD[ipos],signalIn[ipos],signalOut[ipos]);
  }

  if (plotFlag) {
    char hname[100];
    sprintf(hname,"sec%drow%dpad%d",sector,row,pad);
    new TCanvas(hname,hname,600,400);
    //just plotting non-discret pulses | they look pretties in case of mean sig ;-)
    pulseTuple->Draw("sigND:timebin","","L");
    // pulseTuple->Draw("sig:timebin","","Lsame");
    pulseTuple->SetLineColor(3);
    pulseTuple->Draw("sigNDAfterTCF:timebin","","Lsame");
    // pulseTuple->Draw("sigAfterTCF:timebin","","Lsame");
  }
  
  valP->~Int_t();
  valZ->~Int_t();

  signalIn->~Double_t();
  signalOut->~Double_t();
  delete signalIn;
  delete signalOut;

  return pulseTuple;

}




//____________________________________________________________________________
void AliTPCCalibTCF::PrintPulseThresholds() {
  //
  // Prints the pulse threshold settings
  //

  printf("   %4.0d [ADC] ... expected Gate fluctuation length \n", fGateWidth);
  printf("   %4.0d [ADC] ... expected usefull signal length \n",  fSample);
  printf("   %4.0d [ADC] ... needed pulselength for TC characterisation \n", fPulseLength);
  printf("   %4.0d [ADC] ... lower pulse height limit \n", fLowPulseLim);
  printf("   %4.0d [ADC] ... upper pulse height limit \n", fUpPulseLim);
  printf("   %4.1f [ADC] ... maximal pulse RMS \n", fRMSLim);

} 
