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

/* $Id$ */

/*
 example: fill pedestal with gausschen noise
 AliTRDCalibPadStatus ped;
 ped.TestEvent(numberofevent);
 // Method without histo
 //////////////////////////
 ped.Analyse();
 //Create the histo of the AliTRDCalROC
 TH2F * histo2dm = ped.GetCalRocMean(0,kFALSE)->MakeHisto2D();
 histo2dm->Scale(10.0);
 TH1F * histo1dm = ped.GetCalRocMean(0,kFALSE)->MakeHisto1D();
 histo1dm->Scale(10.0);
 TH2F * histo2ds = ped.GetCalRocSquares(0,kFALSE)->MakeHisto2D();
 histo2ds->Scale(10.0);
 TH1F * histo1ds = ped.GetCalRocSquares(0,kFALSE)->MakeHisto1D();
 histo1ds->Scale(10.0)
 //Draw output;
 TCanvas* c1 = new TCanvas;
 c1->Divide(2,2);
 c1->cd(1);
 histo2dm->Draw("colz");
 c1->cd(2);
 histo1dm->Draw();
 c1->cd(3);
 histo2ds->Draw("colz");
 c1->cd(4);
 histo1ds->Draw();
// Method with histo
/////////////////////////
ped.AnalyseHisto();
//Take the histo
TH1F * histo = ped.GetHisto(31);
histo->SetEntries(1);
histo->Draw();

*/

//Root includes
#include <TObjArray.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TString.h>
#include <TMath.h>
#include <TF1.h>
#include <TRandom.h>
#include <TDirectory.h>
#include <TFile.h>
#include <AliMathBase.h>
#include "TTreeStream.h"

//AliRoot includes
#include "AliRawReader.h"
#include "AliRawReaderRoot.h"
#include "AliRawReaderDate.h"

#include "AliTRDRawStream.h"
#include "AliTRDarrayF.h"
#include "AliTRDgeometry.h"
#include "./Cal/AliTRDCalROC.h"
#include "./Cal/AliTRDCalPadStatus.h"
#include "./Cal/AliTRDCalSingleChamberStatus.h"

#ifdef ALI_DATE
#include "event.h"
#endif

//header file
#include "AliTRDCalibPadStatus.h"

ClassImp(AliTRDCalibPadStatus) /*FOLD00*/

//_____________________________________________________________________
AliTRDCalibPadStatus::AliTRDCalibPadStatus() : /*FOLD00*/
  TObject(),
  fGeo(0),
  fAdcMin(0),
  fAdcMax(20),
  fDetector(-1),
  fNumberOfTimeBins(0),
  fCalArrayEntries(540),
  fCalArrayMean(540),
  fCalArraySquares(540),
  fCalRocArrayMean(540),
  fCalRocArrayRMS(540),
  fHistoArray(540),
  fCalEntries(0x0),
  fCalMean(0x0),
  fCalSquares(0x0)
{
    //
    // default constructor
    //

  fGeo = new AliTRDgeometry();

}

//_____________________________________________________________________
AliTRDCalibPadStatus::AliTRDCalibPadStatus(const AliTRDCalibPadStatus &ped) : /*FOLD00*/
  TObject(ped),
  fGeo(0),
  fAdcMin(ped.GetAdcMin()),
  fAdcMax(ped.GetAdcMax()),
  fDetector(ped.fDetector),
  fNumberOfTimeBins(ped.fNumberOfTimeBins),
  fCalArrayEntries(540),
  fCalArrayMean(540),
  fCalArraySquares(540),
  fCalRocArrayMean(540),
  fCalRocArrayRMS(540),
  fHistoArray(540),
  fCalEntries(0x0),
  fCalMean(0x0),
  fCalSquares(0x0)
{
    //
    // copy constructor
    //
    for (Int_t idet = 0; idet < 540; idet++){
	const AliTRDarrayF *calEntries  = (AliTRDarrayF*)ped.fCalArrayEntries.UncheckedAt(idet);
	const AliTRDarrayF *calMean     = (AliTRDarrayF*)ped.fCalArrayMean.UncheckedAt(idet);
	const AliTRDarrayF *calSquares  = (AliTRDarrayF*)ped.fCalArraySquares.UncheckedAt(idet);
	const AliTRDCalROC *calRocMean  = (AliTRDCalROC*)ped.fCalRocArrayMean.UncheckedAt(idet);
	const AliTRDCalROC *calRocRMS   = (AliTRDCalROC*)ped.fCalRocArrayRMS.UncheckedAt(idet);
	const TH2F         *hped        = (TH2F*)ped.fHistoArray.UncheckedAt(idet);
    
	if ( calEntries != 0x0 ) fCalArrayEntries.AddAt(new AliTRDarrayF(*calEntries), idet);
	if ( calMean != 0x0 )    fCalArrayMean.AddAt(new AliTRDarrayF(*calMean), idet);
	if ( calSquares != 0x0 ) fCalArraySquares.AddAt(new AliTRDarrayF(*calSquares), idet);
	if ( calRocMean != 0x0 ) fCalRocArrayMean.AddAt(new AliTRDCalROC(*calRocMean), idet);
	if ( calRocRMS != 0x0 )  fCalRocArrayRMS.AddAt(new AliTRDCalROC(*calRocRMS), idet);

	if ( hped != 0x0 ){
	  TH2F *hNew = new TH2F(*hped);
	  hNew->SetDirectory(0);
	  fHistoArray.AddAt(hNew,idet);
	}
	
    }

    if (fGeo) {
      delete fGeo;
    }
    fGeo = new AliTRDgeometry();

}

//_____________________________________________________________________
AliTRDCalibPadStatus& AliTRDCalibPadStatus::operator = (const  AliTRDCalibPadStatus &source)
{
  //
  // assignment operator
  //
  if (&source == this) return *this;
  new (this) AliTRDCalibPadStatus(source);

  return *this;
}

//_____________________________________________________________________
AliTRDCalibPadStatus::~AliTRDCalibPadStatus() /*FOLD00*/
{
  //
  // destructor
  //

  if (fGeo) {
    delete fGeo;
  }
  
}

//_____________________________________________________________________
Int_t AliTRDCalibPadStatus::Update(const Int_t icdet, /*FOLD00*/
				   const Int_t icRow,
				   const Int_t icCol,
				   const Int_t csignal,
				   const Int_t crowMax)
{
    //
    // Signal filling methode 
    //
    if ( (csignal>fAdcMax) || (csignal<fAdcMin)   ) return 0;

    if(fDetector != icdet){
      fCalEntries = ((AliTRDarrayF *)GetCalEntries(icdet,kTRUE));
      fCalMean    = ((AliTRDarrayF *)GetCalMean(icdet,kTRUE));
      fCalSquares = ((AliTRDarrayF *)GetCalSquares(icdet,kTRUE));
    }

    Float_t entries  = fCalEntries->At(icRow+icCol*crowMax);
    Float_t mean     = fCalMean->At(icRow+icCol*crowMax);
    Float_t squares  = fCalSquares->At(icRow+icCol*crowMax);
    
    Float_t entriesn = entries+1.0;
    fCalEntries->AddAt(entriesn,(icRow+icCol*crowMax));
    Float_t meann = (mean*entries+((Float_t)(csignal+0.5)))/entriesn;
    fCalMean->AddAt(meann,icRow+icCol*crowMax);
    Float_t squaresn = ((squares*entries)+(((Float_t)(csignal+0.5))*((Float_t)(csignal+0.5))))/entriesn;
    fCalSquares->AddAt(squaresn,icRow+icCol*crowMax);

    //printf("icdet %d, icRow %d, icCol %d, csignal %d, crowMax %d\n",icdet,icRow,icCol,csignal,crowMax);
    //printf("entries %f, mean %f, squares %f\n",entriesn,meann,squaresn);

    fDetector = icdet;
    
    return 0;
}
//_____________________________________________________________________
Int_t AliTRDCalibPadStatus::UpdateHisto(const Int_t icdet, /*FOLD00*/
				       const Int_t icRow,
				       const Int_t icCol,
				       const Int_t csignal,
				       const Int_t crowMax)
{
    //
    // Signal filling methode 
    //
  Int_t nbchannel = icRow+icCol*crowMax;
  
  // fast filling methode.
  // Attention: the entry counter of the histogram is not increased
  //            this means that e.g. the colz draw option gives an empty plot
  Int_t bin = 0;
  if ( !(((Int_t)csignal>fAdcMax ) || ((Int_t)csignal<fAdcMin)) )
    bin = (nbchannel+1)*(fAdcMax-fAdcMin+2)+((Int_t)csignal-fAdcMin+1);
  
  GetHisto(icdet,kTRUE)->GetArray()[bin]++;
  
  return 0;
}
//_____________________________________________________________________
Bool_t AliTRDCalibPadStatus::ProcessEvent(AliTRDRawStream *rawStream, Bool_t nocheck)
{
  //
  // Event Processing loop - AliTRDRawStream
  //


  Bool_t withInput = kFALSE;

  if(!nocheck) {
    while (rawStream->Next()) {
      Int_t rawversion = rawStream->GetRawVersion();                     //  current raw version
      if(rawversion != 2) return kFALSE;
      Int_t idetector  = rawStream->GetDet();                            //  current detector
      Int_t iRow       = rawStream->GetRow();                            //  current row
      Int_t iRowMax    = rawStream->GetMaxRow();                         //  current rowmax
      Int_t iCol       = rawStream->GetCol();                            //  current col
      Int_t iTimeBin   = rawStream->GetTimeBin();                        //  current time bin
      Int_t *signal    = rawStream->GetSignals();                        //  current ADC signal
      Int_t nbtimebin  = rawStream->GetNumberOfTimeBins();               //  number of time bins read from data

      if((fDetector != -1) && (nbtimebin != fNumberOfTimeBins)) return kFALSE;
      fNumberOfTimeBins = nbtimebin;
      
      Int_t fin        = TMath::Min(nbtimebin,(iTimeBin+3));
      Int_t n          = 0;
      
      for(Int_t k = iTimeBin; k < fin; k++){
	if(signal[n]>0) UpdateHisto(idetector,iRow,iCol,signal[n],iRowMax);
	n++;
      }
      
      withInput = kTRUE;
    }
  }
  else {
    while (rawStream->Next()) {
      Int_t idetector  = rawStream->GetDet();                            //  current detector
      Int_t iRow       = rawStream->GetRow();                            //  current row
      Int_t iRowMax    = rawStream->GetMaxRow();                         //  current rowmax
      Int_t iCol       = rawStream->GetCol();                            //  current col
      Int_t iTimeBin   = rawStream->GetTimeBin();                        //  current time bin
      Int_t *signal    = rawStream->GetSignals();                        //  current ADC signal
      Int_t nbtimebin = rawStream->GetNumberOfTimeBins();               //  number of time bins read from data
      
      Int_t fin        = TMath::Min(nbtimebin,(iTimeBin+3));
      Int_t n          = 0;
      
      for(Int_t k = iTimeBin; k < fin; k++){
	if(signal[n]>0) UpdateHisto(idetector,iRow,iCol,signal[n],iRowMax);
	n++;
      }
      
      withInput = kTRUE;
    }
  }
  
  return withInput;
}
//_____________________________________________________________________
Bool_t AliTRDCalibPadStatus::ProcessEvent(AliRawReader *rawReader, Bool_t nocheck)
{
  //
  //  Event processing loop - AliRawReader
  //


  AliTRDRawStream rawStream(rawReader);

  rawReader->Select("TRD");

  return ProcessEvent(&rawStream, nocheck);
}
//_________________________________________________________________________
Bool_t AliTRDCalibPadStatus::ProcessEvent(
#ifdef ALI_DATE
					  eventHeaderStruct *event,
					  Bool_t nocheck
#else
					  eventHeaderStruct* /*event*/,
					  Bool_t /*nocheck*/
	    
#endif 
					  )
{
  //
  //  process date event
  //
#ifdef ALI_DATE
    AliRawReader *rawReader = new AliRawReaderDate((void*)event);
    Bool_t result=ProcessEvent(rawReader, nocheck);
    delete rawReader;
    return result;
#else
    Fatal("AliTRDCalibPadStatus", "this class was compiled without DATE");
    return 0;
#endif

}
//_____________________________________________________________________
Bool_t AliTRDCalibPadStatus::TestEvent(Int_t nevent) /*FOLD00*/
{
  //
  //  Test event loop
  // fill one oroc and one iroc with random gaus
  //

  gRandom->SetSeed(0);

    for (Int_t ism=0; ism<18; ism++){
       	for (Int_t ich=0; ich < 5; ich++){
	    for (Int_t ipl=0; ipl < 6; ipl++){
	      for(Int_t irow = 0; irow < fGeo->GetRowMax(ipl,ich,ism); irow++){
		for(Int_t icol = 0; icol < fGeo->GetColMax(ipl); icol++){
		  for (Int_t iTimeBin=0; iTimeBin<(30*nevent); iTimeBin++){
		    Int_t signal=(Int_t)(ich+8+gRandom->Gaus(0,1.2));
		    if ( signal>0 )Update((ipl+ich*6+ism*6*5),irow,icol,signal,fGeo->GetRowMax(ipl,ich,ism));
		  }
		}
	      }
	    }
	}
    }
    return kTRUE;
}
//_____________________________________________________________________
Bool_t AliTRDCalibPadStatus::TestEventHisto(Int_t nevent) /*FOLD00*/
{
  //
  //  Test event loop
  // fill one oroc and one iroc with random gaus
  //

  gRandom->SetSeed(0);

    for (Int_t ism=0; ism<18; ism++){
       	for (Int_t ich=0; ich < 5; ich++){
	    for (Int_t ipl=0; ipl < 6; ipl++){
	      for(Int_t irow = 0; irow < fGeo->GetRowMax(ipl,ich,ism); irow++){
		for(Int_t icol = 0; icol < fGeo->GetColMax(ipl); icol++){
		  for (Int_t iTimeBin=0; iTimeBin<(30*nevent); iTimeBin++){
		    Int_t signal=(Int_t)(ich+8+gRandom->Gaus(0,1.2));
		    if ( signal>0 )UpdateHisto((ipl+ich*6+ism*6*5),irow,icol,signal,fGeo->GetRowMax(ipl,ich,ism));
		  }
		}
	      }
	    }
	}
    }
    return kTRUE;
}
//_____________________________________________________________________
TH2F* AliTRDCalibPadStatus::GetHisto(Int_t det, TObjArray *arr, /*FOLD00*/
				  Int_t nbinsY, Float_t ymin, Float_t ymax,
				  Char_t *type, Bool_t force)
{
    //
    // return pointer to histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(det) )
	return (TH2F*)arr->UncheckedAt(det);

    // if we are forced and histogram doesn't yes exist create it
    Char_t name[255], title[255];

    sprintf(name,"hCalib%s%.2d",type,det);
    sprintf(title,"%s calibration histogram detector %.2d;ADC channel;Channel (pad)",type,det);

    Int_t nbchannels = fGeo->GetRowMax(GetPlane(det),GetChamber(det),GetSector(det))
                     * fGeo->GetColMax(GetPlane(det));

    // new histogram with calib information. One value for each pad!
    TH2F* hist = new TH2F(name,title,
			  nbinsY, ymin, ymax,
			  nbchannels,0,nbchannels
			  );
    hist->SetDirectory(0);
    arr->AddAt(hist,det);
    return hist;
}
//_____________________________________________________________________
TH2F* AliTRDCalibPadStatus::GetHisto(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to histogram
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fHistoArray;
    return GetHisto(det, arr, fAdcMax-fAdcMin, fAdcMin, fAdcMax, "Pedestal", force);
}
//_____________________________________________________________________
AliTRDarrayF* AliTRDCalibPadStatus::GetCal(Int_t det, TObjArray* arr, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to ROC Calibration
    // if force is true create a new AliTRDarrayF if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(det) )
	return (AliTRDarrayF*)arr->UncheckedAt(det);

    // if we are forced and histogram doesn't yes exist create it
    AliTRDarrayF *croc = new AliTRDarrayF();
    Int_t nbpad = fGeo->GetRowMax(GetPlane(det),GetChamber(det),GetSector(det))
                * fGeo->GetColMax(GetPlane(det));

    // new AliTRDCalROC. One value for each pad!
    croc->Expand(nbpad);
    for(Int_t k = 0; k < nbpad; k++){
      croc->AddAt(0.0,k);
    }
    arr->AddAt(croc,det);
    return croc;
}
//_____________________________________________________________________
AliTRDCalROC* AliTRDCalibPadStatus::GetCalRoc(Int_t det, TObjArray* arr, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to ROC Calibration
    // if force is true create a new AliTRDCalROC if it doesn't exist allready
    //
    if ( !force || arr->UncheckedAt(det) )
	return (AliTRDCalROC*)arr->UncheckedAt(det);

    // if we are forced and histogram doesn't yes exist create it

    // new AliTRDCalROC. One value for each pad!
    AliTRDCalROC *croc = new AliTRDCalROC(GetPlane(det),GetChamber(det));
    arr->AddAt(croc,det);
    return croc;
}
//_____________________________________________________________________
AliTRDarrayF* AliTRDCalibPadStatus::GetCalEntries(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalArrayEntries;
    return GetCal(det, arr, force);
}
//_____________________________________________________________________
AliTRDarrayF* AliTRDCalibPadStatus::GetCalMean(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalArrayMean;
    return GetCal(det, arr, force);
}
//_____________________________________________________________________
AliTRDarrayF* AliTRDCalibPadStatus::GetCalSquares(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalArraySquares;
    return GetCal(det, arr, force);
}
//_____________________________________________________________________
AliTRDCalROC* AliTRDCalibPadStatus::GetCalRocMean(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayMean;
    return GetCalRoc(det, arr, force);
}
//_____________________________________________________________________
AliTRDCalROC* AliTRDCalibPadStatus::GetCalRocRMS(Int_t det, Bool_t force) /*FOLD00*/
{
    //
    // return pointer to Carge ROC Calibration
    // if force is true create a new histogram if it doesn't exist allready
    //
    TObjArray *arr = &fCalRocArrayRMS;
    return GetCalRoc(det, arr, force);
}
//_________________________________________________________________________
void AliTRDCalibPadStatus::Analyse() /*FOLD00*/
{
  //
  // Calcul the rms properly
  // 

  for(Int_t idet = 0; idet < 540; idet++){

    // Take the stuff
    fCalEntries                 = ((AliTRDarrayF *)GetCalEntries(idet));
    fCalMean                    = ((AliTRDarrayF *)GetCalMean(idet));
    fCalSquares                 = ((AliTRDarrayF *)GetCalSquares(idet));

    if(!fCalEntries) continue;

    AliTRDCalROC *calRocMean    = ((AliTRDCalROC *)GetCalRocMean(idet,kTRUE));
    AliTRDCalROC *calRocRMS     = ((AliTRDCalROC *)GetCalRocRMS(idet,kTRUE));

    // range channels
    Int_t channels = calRocMean->GetNchannels();
    
    for(Int_t ichannels = 0 ; ichannels < channels; ichannels++){
      
      Float_t entries  = fCalEntries->At(ichannels);
      Float_t mean     = fCalMean->At(ichannels);
      Float_t squares  = fCalSquares->At(ichannels);

      Float_t rms = 0.0;
      if(entries > 0){
	Double_t rm = TMath::Abs(squares-(mean*mean));
	rms = TMath::Sqrt(rm);
	calRocRMS->SetValue(ichannels,rms/10.0);
	calRocMean->SetValue(ichannels,mean/10.0);
      }
    }
  }
}
//_____________________________________________________________________
void AliTRDCalibPadStatus::AnalyseHisto() /*FOLD00*/
{
    //
    //  Calculate calibration constants
    //

    Int_t nbinsAdc = fAdcMax-fAdcMin;

    TVectorD param(3);
    TMatrixD dummy(3,3);

    Float_t *array_hP=0;


    for (Int_t idet=0; idet<540; idet++){
	TH2F *hP = GetHisto(idet);
        if ( !hP ) {
	  continue;
	}

	AliTRDCalROC *rocMean     = GetCalRocMean(idet,kTRUE);
	AliTRDCalROC *rocRMS      = GetCalRocRMS(idet,kTRUE);

	array_hP = hP->GetArray();
        Int_t nChannels = rocMean->GetNchannels();

	for (Int_t iChannel=0; iChannel<nChannels; iChannel++){
            Int_t offset = (nbinsAdc+2)*(iChannel+1)+1;
	    Double_t ret = AliMathBase::FitGaus(array_hP+offset,nbinsAdc,fAdcMin,fAdcMax,&param,&dummy);
            // if the fitting failed set noise and pedestal to 0
	    if ((ret == -4) || (ret == -1)) {
		param[1]=0.0;
		param[2]=0.0;
	    }
	    if((param[1]/10.0) > 65534.0) param[1] = 0.0;
	    if((param[2]/10.0) > 65534.0) param[2] = 0.0;
	    rocMean->SetValue(iChannel,param[1]/10.0);
            rocRMS->SetValue(iChannel,param[2]/10.0);
	}
    }
   
}
//_______________________________________________________________________________________
AliTRDCalPadStatus* AliTRDCalibPadStatus::CreateCalPadStatus()
{
  //
  //
  //

  AliTRDCalPadStatus* obj = new AliTRDCalPadStatus("padstatus", "padstatus");
  
  for (Int_t idet=0; idet<540; ++idet)
    {
      AliTRDCalSingleChamberStatus *calROC = obj->GetCalROC(idet);

      //Take the stuff
      fCalEntries = ((AliTRDarrayF *)GetCalEntries(idet));
      AliTRDCalROC *calRocMean    = ((AliTRDCalROC *)GetCalRocMean(idet));
      AliTRDCalROC *calRocRMS     = ((AliTRDCalROC *)GetCalRocRMS(idet));

      if ( !calRocMean ) {
	for(Int_t k = 0; k < calROC->GetNchannels(); k++){
	  calROC->SetStatus(k,AliTRDCalPadStatus::kMasked);
	}
	//printf("no fCalRocMean for %d\n",idet);
	continue;
      }

      //Range
      Int_t channels = calROC->GetNchannels();
          

      //Mean 
      Float_t meanentries = 0.0;
      if(!fCalEntries){
	if(GetHisto(idet)){
	  meanentries = GetHisto(idet)->GetEntries()/(channels);
	}
      }
      else meanentries       = TMath::Mean(channels,((TArrayF *)fCalEntries)->GetArray());
      //Double_t meanmean      = calRocMean->GetMean()*10.0;
      //Double_t meansquares   = calRocRMS->GetMean()*10.0;


      for(Int_t ich = 0; ich < channels; ich++){
	
	Float_t entries = 0.0;
	if(!fCalEntries){
	  if(GetHisto(idet)){
	    for(Int_t bin = 0; bin < (fAdcMax-fAdcMin); bin++){
	      entries += GetHisto(idet)->GetArray()[(ich+1)*(fAdcMax-fAdcMin+2)+(bin+1)];
	    }
	  }
	}
	else entries     = fCalEntries->At(ich);
	//Float_t mean     = calRocMean->GetValue(ich)*10.0;
	Float_t rms      = calRocRMS->GetValue(ich)*10.0;
	if(ich > 1720) printf("rms %f\n",rms);
		
	//if((entries < 0.3*meanentries) || (TMath::Abs(rms-meansquares) > ((Float_t)(fAdcMax-fAdcMin)/2.0)) || (rms == 0.0)) calROC->SetStatus(ich, AliTRDCalPadStatus::kMasked);
	if(rms <= 0.01) calROC->SetStatus(ich, AliTRDCalPadStatus::kMasked);
	}
    }
  
  return obj;
  
}
//_____________________________________________________________________
void AliTRDCalibPadStatus::DumpToFile(const Char_t *filename, const Char_t *dir, Bool_t append) /*FOLD00*/
{
    //
    //  Write class to file
    //

    TString sDir(dir);
    TString option;

    if ( append )
	option = "update";
    else
        option = "recreate";

    TDirectory *backup = gDirectory;
    TFile f(filename,option.Data());
    f.cd();
    if ( !sDir.IsNull() ){
	f.mkdir(sDir.Data());
	f.cd(sDir);
    }
    this->Write();
    f.Close();

    if ( backup ) backup->cd();
}//_____________________________________________________________________________
Int_t AliTRDCalibPadStatus::GetPlane(Int_t d) const
{
  //
  // Reconstruct the plane number from the detector number
  //

  return ((Int_t) (d % 6));

}

//_____________________________________________________________________________
Int_t AliTRDCalibPadStatus::GetChamber(Int_t d) const
{
  //
  // Reconstruct the chamber number from the detector number
  //

  return ((Int_t) (d % 30) / 6);

}
//_____________________________________________________________________________
Int_t AliTRDCalibPadStatus::GetSector(Int_t d) const
{
  //
  // Reconstruct the sector number from the detector number
  //

  return ((Int_t) (d / 30));

}


