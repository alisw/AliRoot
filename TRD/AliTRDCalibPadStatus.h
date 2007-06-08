#ifndef ALITRDCALIBPADSTATUS_H
#define ALITRDCALIBPADSTATUS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  TRD calibration class for online calibration                             //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

class TObjArray;
class TH2F;
class TTreeSRedirector;
class AliTRDCalROC;
class AliTRDCalPadStatus;
class AliTRDRawStream;
class AliTRDarrayF;
class AliRawReader;
struct eventHeaderStruct;

class AliTRDCalibPadStatus : public TObject {

public:

  AliTRDCalibPadStatus();
  AliTRDCalibPadStatus(const AliTRDCalibPadStatus &ped);
  virtual ~AliTRDCalibPadStatus();

  AliTRDCalibPadStatus& operator = (const  AliTRDCalibPadStatus &source);

  Bool_t ProcessEvent(AliTRDRawStream *rawStream, Bool_t nocheck = kFALSE);
  Bool_t ProcessEvent(AliRawReader    *rawReader, Bool_t nocheck = kFALSE);
  Bool_t ProcessEvent(eventHeaderStruct   *event, Bool_t nocheck = kFALSE);

  Int_t Update(const Int_t idet, const Int_t iRow, const Int_t iCol,
	       const Int_t signal, const Int_t rowMax);
  Int_t UpdateHisto(const Int_t idet, const Int_t iRow, const Int_t iCol,
		    const Int_t signal, const Int_t crowMax);
  void Analyse();
  void AnalyseHisto();
  AliTRDCalPadStatus *CreateCalPadStatus();
  //
  AliTRDarrayF* GetCalEntries(Int_t det, Bool_t force=kFALSE);    // get calibration object
  AliTRDarrayF* GetCalMean(Int_t det, Bool_t force=kFALSE);       // get calibration object
  AliTRDarrayF* GetCalSquares(Int_t det, Bool_t force=kFALSE);    // get calibration object
  AliTRDCalROC* GetCalRocMean(Int_t det, Bool_t force=kFALSE);    // get calibration object
  AliTRDCalROC* GetCalRocRMS(Int_t det, Bool_t force=kFALSE);     // get calibration object

  TH2F* GetHisto  (Int_t det, Bool_t force=kFALSE);              // get refernce histogram
  
  void  DumpToFile(const Char_t *filename, const Char_t *dir="", const Bool_t append=kFALSE);
  //
  Int_t   GetAdcMin()       const { return fAdcMin;       }
  Int_t   GetAdcMax()       const { return fAdcMax;       }

  void    SetRangeAdc (Int_t aMin, Int_t aMax){ fAdcMin=aMin; fAdcMax=aMax; }  // Set adc range 


  Bool_t TestEvent(Int_t nevent);  //test the fast approach to fill array  - used for test purposes
  Bool_t TestEventHisto(Int_t nevent);  //test the fast approach to fill histograms  

private:

  Int_t fAdcMin;                    //  min adc channel of pedestal value
  Int_t fAdcMax;                    //  max adc channel of pedestal value
  Int_t fDetector;                  //  Current detector
  Int_t fNumberOfTimeBins;          //  Current number of time bins
     
  TObjArray fCalArrayEntries;       //  Array of AliTRDarrayF class calibration
  TObjArray fCalArrayMean;          //  Array of AliTRDarrayF class calibration
  TObjArray fCalArraySquares;       //  Array of AliTRDarrayF class calibration
  TObjArray fCalRocArrayMean;       //  Array of AliTRDCalROC class for signal width calibration
  TObjArray fCalRocArrayRMS;        //  Array of AliTRDCalROC class for mean width calibration

  TObjArray fHistoArray;            //  Array of histos for mean width calibration
  
  AliTRDarrayF *fCalEntries;        //  Current AliTRDArrayF entries
  AliTRDarrayF *fCalMean;           //  Current AliTRDArrayF Mean
  AliTRDarrayF *fCalSquares;        //  Current AliTRDArrayF Squares

  AliTRDarrayF* GetCal(Int_t det, TObjArray* arr, Bool_t force);
  AliTRDCalROC* GetCalRoc(Int_t det, TObjArray* arr, Bool_t force);
 
  TH2F* GetHisto(Int_t det, TObjArray *arr,
		 Int_t nbinsY, Float_t ymin, Float_t ymax,
		 Char_t *type, Bool_t force);

  // Some basic geometry function
  virtual Int_t    GetPlane(Int_t d) const;
  virtual Int_t    GetChamber(Int_t d) const;
  virtual Int_t    GetSector(Int_t d) const;

public:

  ClassDef(AliTRDCalibPadStatus,1)

};



#endif

