#ifndef ALITPCCALIBPEDESTAL_H
#define ALITPCCALIBPEDESTAL_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */



#include <TObject.h>
#include <TObjArray.h>

class TArrayF;
class TH2F;
class TTreeSRedirector;
class AliTPCROC;
class AliTPCCalROC;
class AliTPCRawStream;
class AliRawReader;
class AliTPCAltroMapping;

struct eventHeaderStruct;

class AliTPCCalibPedestal : public TObject {

public:
  AliTPCCalibPedestal();
  AliTPCCalibPedestal(const AliTPCCalibPedestal &ped);
  virtual ~AliTPCCalibPedestal();

  AliTPCCalibPedestal& operator = (const  AliTPCCalibPedestal &source);

  Bool_t ProcessEvent(AliTPCRawStream *rawStream);
  Bool_t ProcessEvent(AliRawReader    *rawReader);
  Bool_t ProcessEvent(eventHeaderStruct   *event);

  Int_t  Update(const Int_t isector, const Int_t iRow, const Int_t iPad,
	        const Int_t iTimeBin, const Float_t signal);
  void   Analyse();
  //
  AliTPCAltroMapping **GetAltroMapping() { return fMapping; };
  void  SetAltroMapping(AliTPCAltroMapping **mapp) { fMapping = mapp; };
  //
  AliTPCCalROC* GetCalRocPedestal (Int_t sector, Bool_t force=kFALSE);  // get calibration object - sector
  AliTPCCalROC* GetCalRocRMS(Int_t sector, Bool_t force=kFALSE);        // get calibration object - sector
  const TObjArray* GetCalPadPedestal (){return &fCalRocArrayPedestal;}  // get calibration object
  const TObjArray* GetCalPadRMS(){return &fCalRocArrayRMS;}             // get calibration object
  
  TH2F* GetHistoPedestal  (Int_t sector, Bool_t force=kFALSE);          // get refernce histogram
  void  DumpToFile(const Char_t *filename, const Char_t *dir="", const Bool_t append=kFALSE);
  //
  void  SetTimeAnalysis(Bool_t time = kTRUE);                  // Use ONLY in TPCPEDESTALda on LDC for one sector!
  void  AnalyseTime(Int_t nevents);                            // Makes sense only in TPCPEDESTALda on LDC!
  TArrayF **GetTimePedestals()  const { return fTimeSignal; }  // Get array with time dependent pedestals (for one sector!)
  //
  Int_t GetFirstTimeBin() const { return fFirstTimeBin; }
  Int_t GetLastTimeBin()  const { return fLastTimeBin;  }
  Int_t GetAdcMin()       const { return fAdcMin;       }
  Int_t GetAdcMax()       const { return fAdcMax;       }

  void  SetRangeTime(Int_t tMin, Int_t tMax){ fFirstTimeBin=tMin; fLastTimeBin=tMax; }  // Set time bin range that is used for the pedestal calibration
  void  SetRangeAdc (Int_t aMin, Int_t aMax){ fAdcMin=aMin; fAdcMax=aMax; }  // Set adc range for the pedestal calibration

  void  SetOldRCUformat(Bool_t format=kTRUE) { fOldRCUformat = format; }

  void  Merge(AliTPCCalibPedestal *ped);

  Bool_t TestEvent();  // Test the fast approach to fill histogram - used for test purposes

private:

  Int_t fFirstTimeBin;              //  First Time bin needed for analysis
  Int_t fLastTimeBin;               //  Last Time bin needed for analysis

  Int_t fAdcMin;                    //  min adc channel of pedestal value
  Int_t fAdcMax;                    //  max adc channel of pedestal value

  Bool_t  fOldRCUformat;            //! Should we use the old RCU format for data reading
  Bool_t  fTimeAnalysis;            //! Should we use the time dependent analysis? ONLY ON LDC!

  AliTPCROC *fROC;                  //! ROC information
  AliTPCAltroMapping **fMapping;    //! Altro Mapping

  TObjArray fCalRocArrayPedestal;   //  Array of AliTPCCalROC class for Time0 calibration
  TObjArray fCalRocArrayRMS;        //  Array of AliTPCCalROC class for signal width calibration

  TObjArray fHistoPedestalArray;    //  Calibration histograms for Pedestal distribution

  TArrayF **fTimeSignal;            //! Arrays which hold time dependent signals

  TH2F* GetHisto(Int_t sector, TObjArray *arr,
		 Int_t nbinsY, Float_t ymin, Float_t ymax,
		 Char_t *type, Bool_t force);

  AliTPCCalROC* GetCalRoc(Int_t sector, TObjArray* arr, Bool_t force);

public:
  ClassDef(AliTPCCalibPedestal, 3)  // Implementation of the TPC pedestal and noise calibration
};



#endif

