/**************************************************************************
 * Copyright(c) 2004, ALICE Experiment at CERN, All rights reserved. *
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
/** @file    AliFMDPreprocessor.cxx
    @author  Hans Hjersing Dalsgaard <canute@nbi.dk>
    @date    Mon Mar 27 12:39:09 2006
    @brief   Shuttle "preprocessor" for the FMD
*/
//___________________________________________________________________
//
// The class processes data points from DCS (via Amanada), and DAQ DA
// files (via FXS) to make calibration data for the FMD. 
//
// Data points: 
//    *  Nothing yet. 
//
// DAQ FXS file:
//    * pedestals - a (ASCII) Comma Separated Values files with the
//                  fields 
//                       rcu	 DDL number 
//                       board   FEC board number 
//                       chip    ALTRO chip number on FEC
//                       channel ALTRO channel number
//                       strip   VA1 strip number
//                       sample  Sample number
//                       ped     Mean of ADC spectra
//                       noise   Spread of ADC spectra
//                       mu      Mean of Gaussian fit to ADC spectra
//                       sigma   Variance of Gaussian fit to ADC spectra
//                       chi2    Chi^2 per degrees of freedom of fit
//    * Gains     - a (ASCII) Comma Separated Values files with the
//                  fields 
//                       rcu	 DDL number 
//                       board   FEC board number 
//                       chip    ALTRO chip number on FEC
//                       channel ALTRO channel number
//                       strip   VA1 strip number
//                       gain    Slope of gain
//                       error   Error on gain
//                       chi2    Chi^2 per degrees of freedom of fit
//                  
// See also 
//
//   http://aliceinfo.cern.ch/Offline/Activities/Shuttle.html
//
// Latest changes by Christian Holm Christensen
//

// #include <iostream>

#include <fstream>
#include "AliFMDPreprocessor.h"
#include "AliFMDCalibPedestal.h"
#include "AliFMDCalibGain.h"
#include "AliFMDCalibStripRange.h"
#include "AliFMDCalibSampleRate.h"
#include "AliFMDParameters.h"
#include "AliCDBMetaData.h"
#include "AliCDBManager.h"
// #include "AliDCSValue.h"
#include "AliLog.h"
#include <TTimeStamp.h>
// #include <TFile.h>
#include <TObjString.h>
#include <TString.h>
#include <TNamed.h>


ClassImp(AliFMDPreprocessor)
#if 0 // Do not remove - here to make Emacs happy
;
#endif 


//____________________________________________________
AliFMDPreprocessor::AliFMDPreprocessor(AliShuttleInterface* shuttle)
  : AliPreprocessor("FMD", shuttle)
{
  AddRunType("PHYSICS");
  AddRunType("STANDALONE");
  AddRunType("PEDESTAL");
  AddRunType("GAIN");
}


//____________________________________________________
Bool_t AliFMDPreprocessor::GetAndCheckFileSources(TList*&     list,
						  Int_t       system, 
						  const char* id) 
{
  // Convinience function 
  // Parameters: 
  //   list     On return, list of files. 
  //   system   Alice system (DAQ, DCS, ...)
  //   id       File id
  // Return:
  //   kTRUE on success. 
  list = GetFileSources(system, id);
  if (!list) { 
    TString sys;
    switch (system) { 
    case kDAQ: sys = "DAQ";     break;
    case kDCS: sys = "DCS";     break;
    default:   sys = "unknown"; break;
    }
    Log(Form("Failed to get file sources for %s/%s", sys.Data(), system));
    return kFALSE;
  }
  return kTRUE;
}

//____________________________________________________
AliCDBEntry* 
AliFMDPreprocessor::GetFromCDB(const char* second, const char* third)
{
  return GetFromOCDB(second, third);
}


//____________________________________________________
UInt_t AliFMDPreprocessor::Process(TMap* /* dcsAliasMap */)
{
  // Main member function. 
  // Parameters: 
  //    dcsAliassMap   Map of DCS data point aliases.
  // Return 
  //    0 on success, >0 otherwise 
  Bool_t resultPed   = kTRUE;
  Bool_t resultGain  = kTRUE;
  Bool_t resultRange = kTRUE;
  Bool_t resultRate  = kTRUE;
  Bool_t resultZero  = kTRUE;

  // Do we need this ?
  // if(!dcsAliasMap) return 1;
  // 
  // Invoking the cdb manager and the FMD parameters class
  // AliCDBManager* cdb   = AliCDBManager::Instance();
  // cdb->SetDefaultStorage("local://$ALICE_ROOT");
  // cdb->SetRun(0);
  AliFMDParameters* pars = AliFMDParameters::Instance();
  pars->Init(this, false, AliFMDParameters::kAltroMap);

  // This is if the SOR contains Fee parameters, and we run a DA to
  // extract these parameters.   The same code could work if we get
  // the information from DCS via the FXS 
  TList* files = 0;
  AliFMDCalibSampleRate*      calibRate  = 0;
  AliFMDCalibStripRange*      calibRange = 0;
  AliFMDCalibZeroSuppression* calibZero  = 0;
  // Disabled for now. 
#if 0
  if (GetAndCheckFileSources(files, kDAQ,pars->GetConditionsShuttleID()))
    GetInfoCalibration(files, calibRate, calibRange, calibZero);
  resultRate  = (!calibRate  ? kFALSE : kTRUE);
  resultRange = (!calibRange ? kFALSE : kTRUE);
  resultZero  = (!calibZero  ? kFALSE : kTRUE);
#endif

  // Gt the run type 
  TString runType(GetRunType()); 

  //Creating calibration objects
  AliFMDCalibPedestal* calibPed  = 0;
  AliFMDCalibGain*     calibGain = 0;
  if (runType.Contains("PEDESTAL", TString::kIgnoreCase)) { 
    if (GetAndCheckFileSources(files, kDAQ, pars->GetPedestalShuttleID())) {
      if(files->GetSize())
	calibPed = GetPedestalCalibration(files);
    }
    resultPed = (calibPed ? kTRUE : kFALSE);
  }
  if (runType.Contains("GAIN", TString::kIgnoreCase)) {
    if (GetAndCheckFileSources(files, kDAQ, pars->GetGainShuttleID())) {
      if(files->GetSize())
	calibGain = GetGainCalibration(files);
    }
    resultGain = (calibGain ? kTRUE : kFALSE);
  }
  
  //Storing Calibration objects  
  AliCDBMetaData metaData;
  metaData.SetBeamPeriod(0);
  metaData.SetResponsible("Hans H. Dalsgaard");
  metaData.SetComment("Preprocessor stores pedestals and gains for the FMD.");
  
  if(calibPed)  { 
    resultPed  = Store("Calib","Pedestal", calibPed, &metaData, 0, kTRUE);
    delete calibPed;
  }
  if(calibGain) { 
    resultGain = Store("Calib","PulseGain", calibGain, &metaData, 0, kTRUE);
    delete calibGain;
  }
  if(calibRange) { 
    resultRange = Store("Calib","StripRange", calibRange, &metaData, 0, kTRUE);
    delete calibRange;
  }
  if(calibRate) { 
    resultRate = Store("Calib","SampleRate", calibRate, &metaData, 0, kTRUE);
    delete calibRate;
  }
  if(calibZero) { 
    resultZero = Store("Calib","ZeroSuppression", calibZero,&metaData,0,kTRUE);
    delete calibZero;
  }

#if 0
  // Disabled until we implement GetInfoCalibration properly
  Bool_t success = (resultPed && resultGain  && resultRange && 
		    resultRate  && resultZero);
#endif
  Bool_t success = resultPed && resultGain;
  Log(Form("FMD preprocessor was %s", (success ? "successful" : "failed")));
  return (success ? 0 : 1);
}

//____________________________________________________________________
Bool_t
AliFMDPreprocessor::GetInfoCalibration(TList* files, 
				       AliFMDCalibSampleRate*&      s,
				       AliFMDCalibStripRange*&      r, 
				       AliFMDCalibZeroSuppression*& z)
{
  // Get info calibrations. 
  // Parameters:
  //     files List of files. 
  //     s     On return, newly allocated object 
  //     r     On return, newly allocated object 
  //     z     On return, newly allocated object 
  // Return: 
  //     kTRUE on success
  if (!files) return kTRUE; // Should really be false
  if (files->GetEntries() <= 0) return kTRUE;
  
  s = new AliFMDCalibSampleRate();
  r = new AliFMDCalibStripRange();
  z = new AliFMDCalibZeroSuppression();
  
  AliFMDParameters*    pars     = AliFMDParameters::Instance();
  TIter                iter(files);
  TObjString*          fileSource;

  while((fileSource = dynamic_cast<TObjString*>(iter.Next()))) {
    const Char_t* filename = GetFile(kDAQ, pars->GetConditionsShuttleID(), fileSource->GetName());
    std::ifstream in(filename);
    if(!in) {
      Log(Form("File %s not found!", filename));
      continue;
    }
  }
  return kTRUE;
}

  
//____________________________________________________________________
AliFMDCalibPedestal* 
AliFMDPreprocessor::GetPedestalCalibration(TList* pedFiles)
{
  // Read DAQ DA produced CSV files of pedestals, and return a
  // calibration object. 
  // Parameters:
  //   pedFiles     List of pedestal files 
  // Return 
  //   A pointer to a newly allocated AliFMDCalibPedestal object, or
  //   null in case of errors. 
  if(!pedFiles) return 0;

  AliFMDCalibPedestal* calibPed = new AliFMDCalibPedestal();
  AliFMDParameters*    pars     = AliFMDParameters::Instance();
  TIter                iter(pedFiles);
  TObjString*          fileSource;
  
  while((fileSource = dynamic_cast<TObjString*>(iter.Next()))) {
    const Char_t* filename = GetFile(kDAQ, pars->GetPedestalShuttleID(), fileSource->GetName());
    std::ifstream in(filename);
    if(!in) {
      Log(Form("File %s not found!", filename));
      continue;
    }

    // Get header (how long is it ?)
    TString header;
    header.ReadLine(in);
    header.ToLower();
    if(!header.Contains("pedestal")) {
      Log("File header is not from pedestal!");
      continue;
    }
    Log("File contains data from pedestals");
    
    // Read columns line
    int lineno = 2;
    header.ReadLine(in);
    
    // Loop until EOF
    while(in.peek()!=EOF) {
      if(in.bad()) { 
	Log(Form("Bad read at line %d in %s", lineno, filename));
	break;
      }
      UInt_t ddl=2, board, chip, channel, strip, sample, tb;
      Float_t ped, noise, mu, sigma, chi2ndf;
      Char_t c[11];
	  
      in >> ddl      >> c[0] 
	 >> board    >> c[1]
	 >> chip     >> c[2]
	 >> channel  >> c[3]
	 >> strip    >> c[4]
	 >> sample   >> c[5]
	 >> tb       >> c[6]
	 >> ped      >> c[7]
	 >> noise    >> c[8]
	 >> mu       >> c[9]
	 >> sigma    >> c[10]
	 >> chi2ndf;
      lineno++;
      
      // Ignore trailing garbage 
      if (strip > 127) continue;
      
      //Setting DDL to comply with the FMD in DAQ
      UInt_t FmdDDLBase = 3072; 
      ddl = ddl - FmdDDLBase;
      //Setting the pedestals via the hardware address
      UShort_t det, sec, str;
      Char_t ring;
      
      pars->Hardware2Detector(ddl,board,chip,channel,det,ring,sec,str);
      strip += str;
     
      calibPed->Set(det,ring,sec,strip,ped,noise);
     
    }
  }
  return calibPed;
}	

//____________________________________________________________________
AliFMDCalibGain* 
AliFMDPreprocessor::GetGainCalibration(TList* gainFiles)
{
  // Read DAQ DA produced CSV files of pedestals, and return a
  // calibration object. 
  // Parameters:
  //   pedFiles     List of pedestal files 
  // Return 
  //   A pointer to a newly allocated AliFMDCalibPedestal object, or
  //   null in case of errors. 
  if(!gainFiles) return 0;
  
  AliFMDCalibGain*  calibGain  = new AliFMDCalibGain();
  AliFMDParameters* pars       = AliFMDParameters::Instance();
  TIter             iter(gainFiles);
  TObjString*       fileSource;
  while((fileSource = dynamic_cast<TObjString *>(iter.Next()))) {
    const Char_t* filename = GetFile(kDAQ, pars->GetGainShuttleID(), fileSource->GetName());
    std::ifstream in(filename);
    if(!in) {
      Log(Form("File %s not found!", filename));
      continue;
    }

    //Get header (how long is it ?)
    TString header;
    header.ReadLine(in);
    header.ToLower();
    if(!header.Contains("gain")) {
      Log("File header is not from gain!");
      continue;
    }
    Log("File contains data from pulse gain");

    // Read column headers
    header.ReadLine(in);

    int lineno  = 2;
    // Read until EOF 
    while(in.peek()!=EOF) {
      if(in.bad()) { 
	Log(Form("Bad read at line %d in %s", lineno, filename));
	break;
      }
      UInt_t ddl=2, board, chip, channel, strip;
      Float_t gain,error,  chi2ndf;
      Char_t c[7];
	      
      in >> ddl      >> c[0] 
	 >> board    >> c[1]
	 >> chip     >> c[2]
	 >> channel  >> c[3]
	 >> strip    >> c[4]
	 >> gain     >> c[5]
	 >> error    >> c[6]
	 >> chi2ndf;
      lineno++;
      // Ignore trailing garbage
      if(strip > 127) continue;
      
      //Setting DDL to comply with the FMD in DAQ
      UInt_t FmdDDLBase = 3072; 
      ddl = ddl - FmdDDLBase;
      //Setting the pedestals via the hardware address
      UShort_t det, sec, str;
      Char_t ring;
      pars->Hardware2Detector(ddl,board,chip,channel,det,ring,sec,str);

      strip += str;
      calibGain->Set(det,ring,sec,strip,gain);
    }
  }
  return calibGain;
}

//____________________________________________________________________
//
// EOF
//
