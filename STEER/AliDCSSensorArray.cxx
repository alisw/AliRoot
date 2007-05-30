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


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Calibration class for DCS sensors                                        //
//  Authors: Marian Ivanov and Haavard Helstrup                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliDCSSensorArray.h"

ClassImp(AliDCSSensorArray)

const Int_t  kMinGraph = 10;       // minimum #points of graph to be fitted
const Int_t  kMinPoints = 10;      // minimum number of points per knot in fit
const Int_t  kIter = 10;           // number of iterations for spline fit
const Double_t  kMaxDelta = 0.00;  // precision parameter for spline fit
const Int_t  kFitReq = 2;          // fit requirement, 2 = continuous 2nd derivative
const Double_t kSecInHour = 3600.; // seconds in one hour

//_____________________________________________________________________________
AliDCSSensorArray::AliDCSSensorArray():TNamed(), 
  fStartTime (2000,1,1,0,0,0),
  fEndTime   (2000,1,1,0,0,0),
  fSensors(0)
{
  //
  // AliDCSSensorArray default constructor
  //

}
//_____________________________________________________________________________
AliDCSSensorArray::AliDCSSensorArray(Int_t prevRun, const char* dbEntry) : 
  TNamed(),
  fStartTime (2000,1,1,0,0,0),
  fEndTime   (2000,1,1,0,0,0),
  fSensors(0)
{
  //
  // Read positions etc. from data base entry for previous run
  // Delete all fits and graphs
  //
   AliDCSSensorArray *temp=0;
   AliCDBEntry* entry = 
       AliCDBManager::Instance()->Get(dbEntry,prevRun); 
   if (entry){
     entry->SetOwner(kTRUE);
     temp = (AliDCSSensorArray*)entry->GetObject();
   } 

   TGraph *gr;
   AliSplineFit* fit;

// Delete previous values
   
   Int_t nsensors = temp->fSensors->GetEntries();
   for ( Int_t isensor=0; isensor<nsensors; isensor++) {
     AliDCSSensor *entry = (AliDCSSensor*)temp->fSensors->At(isensor);
     gr = entry->GetGraph();
     if ( gr != 0 ) {
       delete gr;
       gr = 0;
     }
     fit = entry->GetFit();
     if ( fit != 0 ) {
       delete fit;
       fit = 0;
     }
   }    
   
   new (this) AliDCSSensorArray(*temp);
   delete temp;
}



//_____________________________________________________________________________
AliDCSSensorArray::AliDCSSensorArray(const AliDCSSensorArray &c):TNamed(c),
  fStartTime (c.fStartTime),
  fEndTime   (c.fEndTime),
  fSensors(0)

{
  //
  // AliDCSSensorArray copy constructor
  //

  ((AliDCSSensorArray &) c).Copy(*this);

}

///_____________________________________________________________________________
AliDCSSensorArray::~AliDCSSensorArray()
{
  //
  // AliDCSSensorArray destructor
  //
  fSensors->Delete();
  delete fSensors;

}

//_____________________________________________________________________________
AliDCSSensorArray &AliDCSSensorArray::operator=(const AliDCSSensorArray &c)
{
  //
  // Assignment operator
  //

  if (this != &c) ((AliDCSSensorArray &) c).Copy(*this);
  return *this;

}

//_____________________________________________________________________________
void AliDCSSensorArray::Copy(TObject &c) const
{
  //
  // Copy function
  //

  TObject::Copy(c);
}
//_____________________________________________________________________________
void AliDCSSensorArray::SetGraph(TMap *map, const char *amandaString) 
{
  // 
  // Read graphs from DCS maps 
  //
  char dname[100];
  Int_t nsensors = fSensors->GetEntries();
  for ( Int_t isensor=0; isensor<nsensors; isensor++) {
    AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(isensor);
    Int_t dcsSensor=entry->GetIdDCS();
    sprintf(dname,amandaString,dcsSensor);
    TGraph *gr = (TGraph*)map->GetValue(dname);
    entry->SetGraph(gr);
  } 
}  
//_____________________________________________________________________________
void AliDCSSensorArray::MakeSplineFit(TMap *map, const char *amandaString,
                                                               Bool_t keepMap) 
{
  // 
  // Make spline fits from DCS maps 
  //
  char dname[100];
  Int_t nsensors = fSensors->GetEntries();
  for ( Int_t isensor=0; isensor<nsensors; isensor++) {
    AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(isensor);
    Int_t dcsSensor=entry->GetIdDCS();
    sprintf(dname,amandaString,dcsSensor);
    TGraph *gr = (TGraph*)map->GetValue(dname);
    if (gr->GetN() < kMinGraph ) continue;    
    AliSplineFit *fit = new AliSplineFit();
    fit->InitKnots(gr,kMinPoints,kIter,kMaxDelta);
    fit->SplineFit(kFitReq);
    entry->SetStartTime(fStartTime);
    entry->SetEndTime(fEndTime);
    fit->Cleanup();
    entry->SetFit(fit);
    if (keepMap) entry->SetGraph(gr);
  } 
}  

//_____________________________________________________________________________
Double_t AliDCSSensorArray::GetValue(UInt_t timeSec, Int_t sensor) 
{
  // 
  // Return sensor value at time timeSec (obtained from fitted function)
  //  timeSec = time in seconds from start of run
  //
  AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(sensor);
  return entry->GetValue(timeSec);
}
    

//_____________________________________________________________________________
TMap* AliDCSSensorArray::ExtractDCS(TMap *dcsMap, const char *amandaString) 
{
 //
 // Extract temperature graphs from DCS maps
 //
 TMap *values = new TMap;
 TObjArray * valueSet;
 Int_t nsensors = fSensors->GetEntries();
 for ( Int_t isensor=0; isensor<nsensors; isensor++) {
   AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(isensor);
   Int_t dcsSensor=entry->GetIdDCS();
   TString DPname = Form (amandaString,dcsSensor);
   TPair *pair = (TPair*)dcsMap->FindObject(DPname.Data());
   valueSet = (TObjArray*)pair->Value();
   TGraph *graph = MakeGraph(valueSet);
   values->Add(new TObjString(DPname.Data()),graph);
 }
 return values;
}

//_____________________________________________________________________________
TGraph* AliDCSSensorArray::MakeGraph(TObjArray* valueSet){
  //
  // Make graph of temperature values read from DCS map
  //   (spline fit parameters will subsequently be obtained from this graph) 
  //
  Int_t nentries = valueSet->GetEntriesFast(); 
  Float_t *x = new Float_t[nentries];
  Float_t *y = new Float_t[nentries];
  Int_t time0=0;
  Int_t out=0;
  Int_t skipped=0;
  Float_t value;  
  for (Int_t i=0; i<nentries; i++){
    AliDCSValue * val = (AliDCSValue *)valueSet->At(i);
    if (!val) continue;
    if (time0==0){
      time0=val->GetTimeStamp();
    }
    value = val->GetFloat();
    if (TMath::Abs(value)>100) continue;   // refuse values exceeding 100
    if ( out>0 && skipped<10 && TMath::Abs(value-y[out-1])>5) {
      skipped++;                               // refuse temperatures changing 
      continue;                                // by > 5 degrees in one time step
    }                                          
    skipped=0;					      
    if (val->GetTimeStamp()-time0>1000000) continue;
    x[out] = (val->GetTimeStamp()-time0)/kSecInHour; // give times in fractions of hours 
    y[out] = val->GetFloat();
    out++;
    
  }
  TGraph * graph = new TGraph(out,x,y);
  delete [] x;
  delete [] y;
  return graph;
}
  
//_____________________________________________________________________________
AliDCSSensor* AliDCSSensorArray::GetSensor(Int_t IdDCS) 
{
 //
 //  Return sensor information for sensor specified by IdDCS
 //
 Int_t nsensors = fSensors->GetEntries();
 for (Int_t isensor=0; isensor<nsensors; isensor++) {
   AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(isensor);
   if (entry->GetIdDCS() == IdDCS) return entry;
 }
 return 0;
}
//_____________________________________________________________________________
AliDCSSensor* AliDCSSensorArray::GetSensor(Double_t x, Double_t y, Double_t z) 
{
 //
 //  Return sensor closest to given position
 //
 Int_t nsensors = fSensors->GetEntries();
 Double_t dist2min=1e99;
 Double_t xs,ys,zs,dist2;
 Int_t ind=-1;
 for (Int_t isensor=0; isensor<nsensors; isensor++) {
   AliDCSSensor *entry = (AliDCSSensor*)fSensors->At(isensor);
   xs = entry->GetX();
   ys = entry->GetY();
   zs = entry->GetZ();
   dist2 = (x-xs)*(x-xs) + (y-ys)*(y-ys) + (z-zs)*(z-zs);
   if (dist2 < dist2min) {
      ind=isensor;
      dist2min = dist2;
   } 
 }
 if ( ind >= 0 ) {
    return (AliDCSSensor*)fSensors->At(ind);
 } else {
    return 0;
 }
}

AliDCSSensor* AliDCSSensorArray::GetSensorNum(Int_t ind) 
{
 //
 //  Return sensor given by array index
 //
 return (AliDCSSensor*)fSensors->At(ind);
}

Int_t AliDCSSensorArray::GetFirstIdDCS() const
{
 //
 //  Return DCS Id of first sensor
 //
 if ( fSensors != 0 ) {
    return ((AliDCSSensor*)fSensors->At(0))->GetIdDCS();
 } else {
    return 0;
 }
}

Int_t AliDCSSensorArray::GetLastIdDCS() const 
{
 //
 //  Return DCS Id of last sensor
 //
 if ( fSensors != 0 ) {
    Int_t last = fSensors->GetEntries();
    return ((AliDCSSensor*)fSensors->At(last-1))->GetIdDCS();
 } else {
    return 0;
 }
}
