/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id$   */

//  *************************************************************
//  Checks the quality assurance 
//  by comparing with reference data
//  contained in a DB
//  -------------------------------------------------------------
//  W. Ferrarese + P. Cerello Feb 2008
//  INFN Torino
//  SSD QA part: P. Christakoglou - NIKHEF/UU

// --- ROOT system ---
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliITS.h"
#include "AliITSmodule.h"
#include "AliITShit.h"
#include "AliITSdigit.h"
#include "AliITSpListItem.h"
#include "AliRun.h"
#include "AliITSQADataMakerSim.h"
#include "AliITSQASSDDataMakerSim.h"
#include "AliLog.h"
#include "AliQA.h"
#include "AliQAChecker.h"
#include "AliRawReader.h"

ClassImp(AliITSQASSDDataMakerSim)

//____________________________________________________________________________ 
AliITSQASSDDataMakerSim::AliITSQASSDDataMakerSim(AliITSQADataMakerSim *aliITSQADataMakerSim) :
TObject(),
fAliITSQADataMakerSim(aliITSQADataMakerSim),
fSSDhTask(0),
fGenOffsetH(0),
fGenOffsetS(0), 
fGenOffsetD(0) 
{
  //ctor used to discriminate OnLine-Offline analysis   
}

//____________________________________________________________________________ 
AliITSQASSDDataMakerSim::AliITSQASSDDataMakerSim(const AliITSQASSDDataMakerSim& qadm) :
TObject(),
fAliITSQADataMakerSim(qadm.fAliITSQADataMakerSim),
fSSDhTask(qadm.fSSDhTask),
fGenOffsetH(qadm.fGenOffsetH), 
fGenOffsetS(qadm.fGenOffsetS), 
fGenOffsetD(qadm.fGenOffsetD) 
{
  //copy ctor 
  fAliITSQADataMakerSim->SetName((const char*)qadm.fAliITSQADataMakerSim->GetName()) ; 
  fAliITSQADataMakerSim->SetTitle((const char*)qadm.fAliITSQADataMakerSim->GetTitle());
  }

//__________________________________________________________________
AliITSQASSDDataMakerSim& AliITSQASSDDataMakerSim::operator = (const AliITSQASSDDataMakerSim& qac ) {
  // Equal operator.
  this->~AliITSQASSDDataMakerSim();
  new(this) AliITSQASSDDataMakerSim(qac);
  return *this;
}

//____________________________________________________________________________ 
void AliITSQASSDDataMakerSim::StartOfDetectorCycle() {
  //Detector specific actions at start of cycle
  AliDebug(1,"AliITSQADM::Start of SSD Cycle\n");
}

//____________________________________________________________________________ 
void AliITSQASSDDataMakerSim::EndOfDetectorCycle(AliQA::TASKINDEX_t task, TObjArray* list) {
  // launch the QA checking
  AliDebug(1,"AliITSDM instantiates checker with Run(AliQA::kITS, task, list)\n"); 
  
  AliQAChecker::Instance()->Run( AliQA::kITS , task, list);
}

//____________________________________________________________________________ 
void AliITSQASSDDataMakerSim::InitDigits() { 
  // Initialization for DIGIT data - SSD -
  fGenOffsetD = (fAliITSQADataMakerSim->fDigitsQAList)->GetEntries();

  // custom code here
  TH1F *fHistSSDModule = new TH1F("fHistSSDDigitsModule",
				  ";SSD Module Number;N_{DIGITS}",
				  1698,499.5,2197.5);  
  fAliITSQADataMakerSim->Add2DigitsList(fHistSSDModule,
					fGenOffsetD + 0);
  fSSDhTask += 1;
  TH2F *fHistSSDModuleStrip = new TH2F("fHistSSDDigitsModuleStrip",
				       ";N_{Strip};N_{Module}",
				       1540,0,1540,1698,499.5,2197.5);  
  fAliITSQADataMakerSim->Add2DigitsList(fHistSSDModuleStrip,
					fGenOffsetD + 1);
  fSSDhTask += 1;

  AliDebug(1,Form("%d SSD Digits histograms booked\n",fSSDhTask));

}

//____________________________________________________________________________
void AliITSQASSDDataMakerSim::MakeDigits(TTree *digits) { 
  // Fill QA for DIGIT - SSD -
  AliITS *fITS  = (AliITS*)gAlice->GetModule("ITS");
  fITS->SetTreeAddress();
  TClonesArray *iSSDdigits  = fITS->DigitsAddress(2);
  for(Int_t iModule = 500; iModule < 2198; iModule++) {
    iSSDdigits->Clear();
    digits->GetEvent(iModule);    
    Int_t ndigits = iSSDdigits->GetEntries();
    fAliITSQADataMakerSim->GetDigitsData(fGenOffsetD + 0)->Fill(iModule,ndigits);
    if(ndigits != 0)
      AliDebug(1,Form("Module: %d - Digits: %d",iModule,ndigits));
 
    for (Int_t iDigit = 0; iDigit < ndigits; iDigit++) {
      AliITSdigit *dig = (AliITSdigit*)iSSDdigits->UncheckedAt(iDigit);
      Int_t fStripNumber = (dig->GetCoord1() == 0) ? dig->GetCoord2() : dig->GetCoord2() + fgkNumberOfPSideStrips;
      ((TH2F *)fAliITSQADataMakerSim->GetDigitsData(fGenOffsetD + 1))->Fill(fStripNumber,iModule,dig->GetSignal());
    }//digit loop
  }//module loop
}

//____________________________________________________________________________ 
void AliITSQASSDDataMakerSim::InitSDigits() { 
  // Initialization for SDIGIT data - SSD -
  fGenOffsetS = (fAliITSQADataMakerSim->fSDigitsQAList)->GetEntries();

  // custom code here
  TH1F *fHistSSDModule = new TH1F("fHistSSDSDigitsModule",
				  ";SSD Module Number;N_{SDIGITS}",
				  1698,499.5,2197.5);  
  fAliITSQADataMakerSim->Add2SDigitsList(fHistSSDModule,
					 fGenOffsetS + 0);
  fSSDhTask += 1;  

  AliDebug(1,Form("%d SSD SDigits histograms booked\n",fSSDhTask));
}

//____________________________________________________________________________
void AliITSQASSDDataMakerSim::MakeSDigits(TTree *sdigits) { 
  // Fill QA for SDIGIT - SSD -
  static TClonesArray iSSDEmpty("AliITSpListItem",10000);
  iSSDEmpty.Clear();
  TClonesArray *iSSDsdigits = &iSSDEmpty;

  AliInfo(Form("Trying to access the sdigits histogram: %d\n",fGenOffsetS));

  TBranch *brchSDigits = sdigits->GetBranch("ITS");
  brchSDigits->SetAddress(&iSSDsdigits);
  for(Int_t iModule = 500; iModule < 2198; iModule++) {
    iSSDsdigits->Clear();
    sdigits->GetEvent(iModule);    
    Int_t ndigits = iSSDsdigits->GetEntries();
    fAliITSQADataMakerSim->GetSDigitsData(fGenOffsetS + 0)->Fill(iModule,ndigits);
    if(ndigits != 0)
      AliDebug(1,Form("Module: %d - Digits: %d",iModule,ndigits));

    for (Int_t iDigit = 0; iDigit < ndigits; iDigit++) {
      AliITSpListItem *dig=(AliITSpListItem*)iSSDsdigits->At(iDigit);
      dig=0;
    }//digit loop
  }//module loop
}

//____________________________________________________________________________ 
void AliITSQASSDDataMakerSim::InitHits() { 
  // Initialization for HITS data - SSD -
  fGenOffsetH = (fAliITSQADataMakerSim->fHitsQAList)->GetEntries();

  // custom code here
  TH1F *fHistSSDModule = new TH1F("fHistSSDHitsModule",
				  ";SDD Module Number;N_{HITS}",
				  1698,499.5,2197.5); 
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDModule,
				      fGenOffsetH + 0);
  fSSDhTask += 1;
  TH1F *fHistSSDGlobalX = new TH1F("fHistSSDHitsGlobalX",
				   ";x [cm];Entries",
				   1000,-50.,50.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDGlobalX,
				      fGenOffsetH + 1);
  fSSDhTask += 1;
  TH1F *fHistSSDGlobalY = new TH1F("fHistSSDHitsGlobalY",
				   ";y [cm];Entries",
				   1000,-50.,50.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDGlobalY,
				      fGenOffsetH + 2);
  fSSDhTask += 1;
  TH1F *fHistSSDGlobalZ = new TH1F("fHistSSDHitsGlobalZ",
				   ";z [cm];Entries",
				   1000,-60.,60.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDGlobalZ,
				      fGenOffsetH + 3);
  fSSDhTask += 1;
  TH1F *fHistSSDLocalX = new TH1F("fHistSSDHitsLocalX",
				  ";x [cm];Entries",
				  1000,-4.,4.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDLocalX,
				      fGenOffsetH + 4);
  fSSDhTask += 1;
  TH1F *fHistSSDLocalY = new TH1F("fHistSSDHitsLocalY",
				  ";y [cm];Entries",
				  1000,-0.1,0.1);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDLocalY,
				      fGenOffsetH + 5);
  fSSDhTask += 1;
  TH1F *fHistSSDLocalZ = new TH1F("fHistSSDHitsLocalZ",
				  ";z [cm];Entries",
				  1000,-4.,4.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDLocalZ,
				      fGenOffsetH + 6);
  fSSDhTask += 1;
  TH1F *fHistSSDIonization = new TH1F("fHistSSDHitsIonization",
				      ";log(dE/dx) [KeV];N_{Hits}",
				      100,-7,-2);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDIonization,
				      fGenOffsetH + 7 );
  fSSDhTask += 1;
  TH2F *fHistSSDGlobalXY = new TH2F("fHistSSDHitsGlobalXY",
				    ";x [cm];y [cm]",
				    1000,-50.,50.,
				    1000,-50.,50.);
  fAliITSQADataMakerSim->Add2HitsList(fHistSSDGlobalXY,
				      fGenOffsetH + 8 );
  fSSDhTask += 1;
 
  AliDebug(1,Form("%d SSD Hits histograms booked\n",fSSDhTask));
}


//____________________________________________________________________________
void AliITSQASSDDataMakerSim::MakeHits(TTree *hits) { 
  // Fill QA for HITS - SSD -
  AliITS *fITS  = (AliITS*)gAlice->GetModule("ITS");
  fITS->SetTreeAddress();
  Int_t nmodules;
  fITS->InitModules(-1,nmodules);
  fITS->FillModules(hits,0);
  for(Int_t iModule = 500; iModule < 2198; iModule++) {
    AliITSmodule *module = fITS->GetModule(iModule);
    TObjArray *arrHits = module->GetHits();
    Int_t nhits = arrHits->GetEntriesFast();
    if(nhits != 0)
      AliDebug(1,Form("Module: %d - Hits: %d",iModule,nhits));
    for (Int_t iHit = 0; iHit < nhits; iHit++) {
      AliITShit *hit = (AliITShit*) arrHits->At(iHit);
      
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 0)->Fill(iModule);
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 1)->Fill(hit->GetXG());
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 2)->Fill(hit->GetYG());
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 3)->Fill(hit->GetZG());
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 4)->Fill(hit->GetXL());
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 5)->Fill(hit->GetYL());
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 6)->Fill(hit->GetZL());
      if(hit->GetIonization())
	fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 7)->Fill(TMath::Log10(hit->GetIonization()));
      fAliITSQADataMakerSim->GetHitsData(fGenOffsetH + 8)->Fill(hit->GetXG(),hit->GetYG());
    }//hit loop
  }//module loop  
}
