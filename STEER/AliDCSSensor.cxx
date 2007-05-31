/**************************************************************************
 * Copyright(c) 2006-07, ALICE Experiment at CERN, All rights reserved. *
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


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Class describing TPC temperature sensors (including pointers to graphs/fits//
// Authors: Marian Ivanov, Haavard Helstrup and Martin Siska                  //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


// Running instructions:
/*
  TClonesArray * arr = AliDCSSensor::ReadList("TempSensor.txt");
  TFile f("TempSensors.root","RECREATE");
  TTree * tree = new TTree("TempSensor", "TempSensor");
  tree->Branch("Temp",&arr);
  tree->Fill();
  tree->Write();
  
 */
//


#include "AliDCSSensor.h"
ClassImp(AliDCSSensor)

const Double_t kSmall = -9e99;     // invalid small value
const Double_t kLarge = 9e99;      // invalid large value
const Double_t kSecInHour = 3600.; // seconds in one hour



AliDCSSensor::AliDCSSensor():
  fId(),
  fIdDCS(0),
  fStartTime(0),
  fEndTime(0),
  fGraph(0),
  fFit(0),
  fX(0),
  fY(0),
  fZ(0)
{
  //
  //  Standard constructor
  //
}

AliDCSSensor::AliDCSSensor(const AliDCSSensor& source) :
   TNamed(source),
   fId(source.fId),
   fIdDCS(source.fIdDCS),
   fStartTime(source.fStartTime),
   fEndTime(source.fEndTime),
   fGraph(source.fGraph),
   fFit(source.fFit),
   fX(source.fX),
   fY(source.fY),
   fZ(source.fZ)
//
//  Copy constructor
//
{ }

AliDCSSensor& AliDCSSensor::operator=(const AliDCSSensor& source){
//
// assignment operator
//
  if (&source == this) return *this;
  new (this) AliDCSSensor(source);
  
  return *this;  
}

//_____________________________________________________________________________
Double_t AliDCSSensor::GetValue(UInt_t timeSec) 
{
 //
 // Get temperature value for actual sensor
 //  timeSec given as offset from start-of-run measured in seconds
 //
 Double_t timeHrs = timeSec/kSecInHour;
 if (fFit) {
  return  fFit->Eval(timeHrs,0);
 } else { 
  return kSmall;
 }
}
//_____________________________________________________________________________
Double_t AliDCSSensor::GetValue(TTimeStamp time) 
{
 // Get temperature value for actual sensor
 //  time given as absolute TTimeStamp
 //
 Double_t timeHrs = (time.GetSec() - fStartTime)/kSecInHour;
 if (fFit) {
  return  fFit->Eval(timeHrs,0);
 } else { 
  return kSmall;
 }
}

//_____________________________________________________________________________

Double_t AliDCSSensor::Eval(const TTimeStamp& time) const
{
  // 
  // Return temperature at given time
  //  If time < start of map  return kSmall
  //  If time > end of map    return kLarge
  
  UInt_t timeSec = time.GetSec();
  UInt_t diff = timeSec-fStartTime;
  
  if ( timeSec < fStartTime ) return kSmall;
  if ( timeSec > fEndTime ) return kLarge;
 
  Double_t timeHour = diff/kSecInHour;
  if ( fFit ) {
     return fFit->Eval(timeHour); 
  } else {
     return kSmall;
  }
}

TGraph* AliDCSSensor::MakeGraph(Int_t nPoints) const
{
  //
  // Make graph from start time to end time of DCS values 
  //

  UInt_t stepTime = (fEndTime-fStartTime)/nPoints;
  
  if ( !fFit ) return 0;

  Double_t *x = new Double_t[nPoints+1];
  Double_t *y = new Double_t[nPoints+1];
  for (Int_t ip=0; ip<nPoints; ip++) {
    x[ip] = fStartTime+ip*stepTime;
    y[ip] = fFit->Eval(ip*stepTime/kSecInHour);
  }
  
  TGraph *graph = new TGraph(nPoints,x,y);
  delete [] x;
  delete [] y;
  
  graph->GetXaxis()->SetTimeDisplay(1);
  graph->GetXaxis()->SetLabelOffset(0.02);
  graph->GetXaxis()->SetTimeFormat("#splitline{%d/%m}{%H:%M}");
  
  return graph;
}


