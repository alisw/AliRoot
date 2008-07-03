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

///////////////////////////////////////////////////////////////////////////////
///                                                                          //
/// class for VZERO reconstruction                                           //
///                                                                          //
///////////////////////////////////////////////////////////////////////////////

#include "AliRunLoader.h"
#include "AliRawReader.h"
#include "AliVZEROReconstructor.h"
#include "AliVZERORawStream.h"
#include "AliESDEvent.h"
#include "AliVZEROTriggerMask.h"

ClassImp(AliVZEROReconstructor)

//_____________________________________________________________________________
AliVZEROReconstructor:: AliVZEROReconstructor(): AliReconstructor(),
   fESDVZERO(0x0),
   fESD(0x0),
   fCalibData(GetCalibData())
{
  // Default constructor  
  // Get calibration data
  
  // fCalibData = GetCalibData(); 
}


//_____________________________________________________________________________
AliVZEROReconstructor& AliVZEROReconstructor::operator = 
  (const AliVZEROReconstructor& /*reconstructor*/)
{
// assignment operator

  Fatal("operator =", "assignment operator not implemented");
  return *this;
}

//_____________________________________________________________________________
AliVZEROReconstructor::~AliVZEROReconstructor()
{
// destructor
   delete fESDVZERO; 
   
}

//_____________________________________________________________________________
void AliVZEROReconstructor::Init()
{
// initializer

  fESDVZERO  = new AliESDVZERO;
}

//______________________________________________________________________
void AliVZEROReconstructor::ConvertDigits(AliRawReader* rawReader, TTree* digitsTree) const
{
// converts to digits

  if (!digitsTree) {
    AliError("No digits tree!");
    return;
  }

  TClonesArray* digitsArray = new TClonesArray("AliVZEROdigit");
  digitsTree->Branch("VZERODigit", &digitsArray);

  rawReader->Reset();
  AliVZERORawStream rawStream(rawReader);
  if (rawStream.Next()) {
    for(Int_t iChannel = 0; iChannel < 64; iChannel++) {
    Int_t adc = rawStream.GetADC(iChannel);  
    Int_t time = rawStream.GetTime(iChannel);
    new ((*digitsArray)[digitsArray->GetEntriesFast()])
      AliVZEROdigit(iChannel,adc,time);
    }
  }

  digitsTree->Fill();
}

//______________________________________________________________________
void AliVZEROReconstructor::FillESD(TTree* digitsTree, TTree* /*clustersTree*/,
				    AliESDEvent* esd) const
{
// fills multiplicities to the ESD

  if (!digitsTree) {
    AliError("No digits tree!");
    return;
  }

  TClonesArray* digitsArray = NULL;
  TBranch* digitBranch = digitsTree->GetBranch("VZERODigit");
  digitBranch->SetAddress(&digitsArray);

  const Float_t mip0=110.0;
  Short_t Multiplicity[64];
  Float_t mult[64];  
  Short_t   adc[64]; 
  Short_t   time[64]; 
  Float_t mip[64];
  for (Int_t i=0; i<64; i++){
       adc[i] = 0;
       mip[i] = mip0;
       mult[i]= 0.0;
  }
     
  // loop over VZERO entries to get multiplicity
  Int_t nEntries = (Int_t)digitsTree->GetEntries();
  for (Int_t e=0; e<nEntries; e++) {
    digitsTree->GetEvent(e);

    Int_t nDigits = digitsArray->GetEntriesFast();
    
    for (Int_t d=0; d<nDigits; d++) {    
      AliVZEROdigit* digit = (AliVZEROdigit*)digitsArray->At(d);      
      Int_t  pmNumber      = digit->PMNumber();  
      adc[pmNumber] = (Short_t) digit->ADC(); 
      time[pmNumber] = (Short_t) digit->Time();
      // cut of ADC at MIP/2
      if  (adc[pmNumber] > (mip[pmNumber]/2)) 
	mult[pmNumber] += float(adc[pmNumber])/mip[pmNumber];
    } // end of loop over digits
  } // end of loop over events in digits tree
  
  for (Int_t j=0; j<64; j++) Multiplicity[j] = short(mult[j]+0.5);       
  fESDVZERO->SetMultiplicity(Multiplicity);
  fESDVZERO->SetADC(adc);
  fESDVZERO->SetTime(time);

  // now get the trigger mask

  AliVZEROTriggerMask *TriggerMask = new AliVZEROTriggerMask();
  TriggerMask->SetAdcThreshold(mip0/2.0);
  TriggerMask->SetTimeWindowWidthBBA(50);
  TriggerMask->SetTimeWindowWidthBGA(20);
  TriggerMask->SetTimeWindowWidthBBC(50);
  TriggerMask->SetTimeWindowWidthBGC(20);
  TriggerMask->FillMasks(digitsTree,digitsArray);

  fESDVZERO->SetBBtriggerV0A(TriggerMask->GetBBtriggerV0A());
  fESDVZERO->SetBGtriggerV0A(TriggerMask->GetBGtriggerV0A());
  fESDVZERO->SetBBtriggerV0C(TriggerMask->GetBBtriggerV0C());
  fESDVZERO->SetBGtriggerV0C(TriggerMask->GetBGtriggerV0C());
  
  if (esd) { 
    AliDebug(1, Form("Writing VZERO data to ESD tree"));
    esd->SetVZEROData(fESDVZERO);
  }
}

//_____________________________________________________________________________
AliCDBStorage* AliVZEROReconstructor::SetStorage(const char *uri) 
{
// Sets the storage  

  Bool_t deleteManager = kFALSE;
  
  AliCDBManager *manager = AliCDBManager::Instance();
  AliCDBStorage *defstorage = manager->GetDefaultStorage();
  
  if(!defstorage || !(defstorage->Contains("VZERO"))){ 
     AliWarning("No default storage set or default storage doesn't contain VZERO!");
     manager->SetDefaultStorage(uri);
     deleteManager = kTRUE;
  }
 
  AliCDBStorage *storage = manager->GetDefaultStorage();

  if(deleteManager){
    AliCDBManager::Instance()->UnsetDefaultStorage();
    defstorage = 0;   // the storage is killed by AliCDBManager::Instance()->Destroy()
  }

  return storage; 
}

//_____________________________________________________________________________
AliVZEROCalibData* AliVZEROReconstructor::GetCalibData() const
{
  // Gets calibration object for VZERO set

  AliCDBManager *man = AliCDBManager::Instance();

  AliCDBEntry *entry=0;

  entry = man->Get("VZERO/Calib/Data");

//   if(!entry){
//     AliWarning("Load of calibration data from default storage failed!");
//     AliWarning("Calibration data will be loaded from local storage ($ALICE_ROOT)");
//     Int_t runNumber = man->GetRun();
//     entry = man->GetStorage("local://$ALICE_ROOT")
//       ->Get("VZERO/Calib/Data",runNumber);
// 	
//   }

  // Retrieval of data in directory VZERO/Calib/Data:

  AliVZEROCalibData *calibdata = 0;

  if (entry) calibdata = (AliVZEROCalibData*) entry->GetObject();
  if (!calibdata)  AliFatal("No calibration data from calibration database !");

  return calibdata;
}
