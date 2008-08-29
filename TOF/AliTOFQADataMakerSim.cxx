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

///////////////////////////////////////////////////////////////////////
//                                                                   //
//  Produces the data needed to calculate the TOF quality assurance. //
//  QA objects are 1 & 2 Dimensional histograms.                     //
//  author: S.Arcelli                                                //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <TClonesArray.h>
//#include <TFile.h> 
//#include <TH1I.h> 
#include <TH1F.h> 
#include <TH2F.h> 
#include <TTree.h>
#include <TMath.h>

#include "AliLog.h"

#include "AliTOFdigit.h"
#include "AliTOFSDigit.h"
#include "AliTOFhitT0.h"
#include "AliTOFQADataMakerSim.h"
#include "AliQAChecker.h"
#include "AliTOFGeometry.h"


ClassImp(AliTOFQADataMakerSim)
           
//____________________________________________________________________________ 
  AliTOFQADataMakerSim::AliTOFQADataMakerSim() : 
  AliQADataMakerSim(AliQA::GetDetName(AliQA::kTOF), "TOF Quality Assurance Data Maker")
{
  //
  // ctor
  //
}

//____________________________________________________________________________ 
AliTOFQADataMakerSim::AliTOFQADataMakerSim(const AliTOFQADataMakerSim& qadm) :
  AliQADataMakerSim()
{
  //
  //copy ctor 
  //
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}

//__________________________________________________________________
AliTOFQADataMakerSim& AliTOFQADataMakerSim::operator = (const AliTOFQADataMakerSim& qadm )
{
  //
  // assignment operator.
  //
  this->~AliTOFQADataMakerSim();
  new(this) AliTOFQADataMakerSim(qadm);
  return *this;
}
 
//____________________________________________________________________________ 
void AliTOFQADataMakerSim::InitHits()
{
  //
  // create Hits histograms in Hits subdir
  //

  Bool_t expert = kFALSE;

  TH1F * h0 = new TH1F("hTOFHits",    "Number of TOF Hits ",301, -1.02, 5.) ; 
  h0->Sumw2() ;
  Add2HitsList(h0, 0, expert) ;

  TH1F * h1  = new TH1F("hTOFHitsTime", "Hits Time Spectrum in TOF (ns)", 2000, 0., 200) ; 
  h1->Sumw2() ;
  Add2HitsList(h1, 1, expert) ;

  TH1F * h2  = new TH1F("hTOFHitsLength", "Length Spectrum in TOF (cm)", 500, 0., 500) ; 
  h2->Sumw2() ;
  Add2HitsList(h2, 2, expert) ;

  TH2F * h3  = new TH2F("hTOFHitsClusMap","Hits vs TOF eta-phi",183, -0.5, 182.5,865,-0.5,864.5) ; 
  h3->Sumw2() ;
  Add2HitsList(h3, 3, expert) ;
}

//____________________________________________________________________________ 
void AliTOFQADataMakerSim::InitDigits()
{
  //
  // create Digits histograms in Digits subdir
  //

  Bool_t expert = kFALSE;

  TH1F * h0 = new TH1F("hTOFDigits",    "Number of TOF Digits ",301, -1.02, 5.) ;   h0->Sumw2() ;
  Add2DigitsList(h0, 0, expert) ;

  TH1F * h1  = new TH1F("hTOFDigitsTime", "Digits Time Spectrum in TOF (ns)", 2000, 0., 200) ; 
  h1->Sumw2() ;
  Add2DigitsList(h1, 1, expert) ;

  TH1F * h2  = new TH1F("hTOFDigitsToT", "Digits ToT Spectrum in TOF (ns)", 500, 0., 50) ; 
  h2->Sumw2() ;
  Add2DigitsList(h2, 2, expert) ;

  TH2F * h3  = new TH2F("hTOFDigitsClusMap","Digits vs TOF eta-phi",183, -0.5, 182.5,865,-0.5,864.5) ; 
  h3->Sumw2() ;
  Add2DigitsList(h3, 3, expert) ;

}

//____________________________________________________________________________ 
void AliTOFQADataMakerSim::InitSDigits()
{
  //
  // create SDigits histograms in SDigits subdir
  //

  Bool_t expert = kFALSE;

  TH1F * h0 = new TH1F("hTOFSDigits",    "Number of TOF SDigits ",301, -1.02, 5.) ;   h0->Sumw2() ;
  Add2SDigitsList(h0, 0, expert) ;

  TH1F * h1  = new TH1F("hTOFSDigitsTime", "SDigits Time Spectrum in TOF (ns)", 2000, 0., 200) ; 
  h1->Sumw2() ;
  Add2SDigitsList(h1, 1, expert) ;

  TH2F * h2  = new TH2F("hTOFSDigitsClusMap","SDigits vs TOF eta-phi",183, -0.5, 182.5,865,-0.5,864.5) ; 
  h2->Sumw2() ;
  Add2SDigitsList(h2, 2, expert) ;

}

//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeHits(TClonesArray * hits)
{
  //
  //make QA data from Hits
  //

  Int_t in[5];
  Int_t out[5];

  Int_t nentries=hits->GetEntriesFast();
  if(nentries<=0) {
    GetHitsData(0)->Fill(-1.) ; 
  } else{
    GetHitsData(0)->Fill(TMath::Log10(nentries)) ; 
  }
  TIter next(hits) ; 
  AliTOFhitT0 * hit ; 
  while ( (hit = dynamic_cast<AliTOFhitT0 *>(next())) ) {

    GetHitsData(1)->Fill( hit->GetTof()*1.E9) ;//in ns
    GetHitsData(2)->Fill( hit->GetLen()) ;//in cm
  
    in[0] = hit->GetSector();
    in[1] = hit->GetPlate();
    in[2]= hit->GetStrip();
    in[3]= hit->GetPadx();
    in[4]= hit->GetPadz();
    GetMapIndeces(in,out);
    GetHitsData(3)->Fill( out[0],out[1]) ;//hit map
  }

}


//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeHits(TTree * hitTree)
{
  //
  // make QA data from Hit Tree
  //
  if(!hitTree){
    AliError("can't get the tree with TOF hits !");
    return;
  }	

  TBranch * branch = hitTree->GetBranch("TOF") ;

  if (!branch ) {
    AliError("TOF branch in Hit Tree not found") ; 
    return;
  }

  static TClonesArray statichits("AliTOFhitT0", 1000);
  statichits.Clear();
  TClonesArray *hits = &statichits;
  static TClonesArray staticdummy("AliTOFhitT0", 1000);
  staticdummy.Clear();
  TClonesArray *dummy = &staticdummy;
  branch->SetAddress(&dummy);
  Int_t index = 0 ;  
  for (Int_t ientry = 0 ; ientry < branch->GetEntries() ; ientry++) {
    branch->GetEntry(ientry) ; 
    for (Int_t ihit = 0 ; ihit < dummy->GetEntries() ; ihit++) {
      AliTOFhitT0 * hit = dynamic_cast<AliTOFhitT0 *> (dummy->At(ihit)) ; 
      new((*hits)[index]) AliTOFhitT0(*hit) ; 
      index++ ;
    } 
  } 	

  MakeHits(hits) ; 

}

//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeDigits(TClonesArray * digits)
{
  //
  // makes data from Digits
  //
  Double_t tdc2ns=AliTOFGeometry::TdcBinWidth()*1E-3;
  Double_t tot2ns=AliTOFGeometry::ToTBinWidth()*1E-3;
  Int_t in[5];
  Int_t out[5];

  Int_t nentries=digits->GetEntriesFast();
  if(nentries<=0){
    GetDigitsData(0)->Fill(-1.) ; 
  }else{
    GetDigitsData(0)->Fill(TMath::Log10(nentries)) ; 
  } 

  TIter next(digits) ; 
  AliTOFdigit * digit ; 
  while ( (digit = dynamic_cast<AliTOFdigit *>(next())) ) {
    
    GetDigitsData(1)->Fill( digit->GetTdc()*tdc2ns) ;//in ns
    GetDigitsData(2)->Fill( digit->GetToT()*tot2ns) ;//in ns

    in[0] = digit->GetSector();
    in[1] = digit->GetPlate();
    in[2] = digit->GetStrip();
    in[3] = digit->GetPadx();
    in[4]= digit->GetPadz();
    GetMapIndeces(in,out);
    GetDigitsData(3)->Fill( out[0],out[1]) ;//digit map
  }

}


//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeDigits(TTree * digitTree)
{
  //
  // makes data from Digit Tree
  //
  TClonesArray * digits = new TClonesArray("AliTOFdigit", 1000) ; 
  
  TBranch * branch = digitTree->GetBranch("TOF") ;
  if ( ! branch ) {
    AliError("TOF branch in Digit Tree not found") ; 
    return;
  }
  branch->SetAddress(&digits) ;
  branch->GetEntry(0) ; 
  MakeDigits(digits) ; 
}

//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeSDigits(TClonesArray * sdigits)
{
  //
  // makes data from SDigits
  //

  Double_t tdc2ns=AliTOFGeometry::TdcBinWidth()*1E-3;
  Int_t in[5];
  Int_t out[5];

  Int_t nentries=sdigits->GetEntriesFast();
  if(nentries<=0){
    GetSDigitsData(0)->Fill(-1.) ; 
  }else{
    GetSDigitsData(0)->Fill(TMath::Log10(nentries)) ; 
  } 

  TIter next(sdigits) ; 
  AliTOFSDigit * sdigit ; 
  while ( (sdigit = dynamic_cast<AliTOFSDigit *>(next())) ) {
    
    for(Int_t i=0;i<sdigit->GetNDigits();i++){
      GetSDigitsData(1)->Fill( sdigit->GetTdc(i)*tdc2ns) ;//in ns
    }

    in[0] = sdigit->GetSector();
    in[1] = sdigit->GetPlate();
    in[2] = sdigit->GetStrip();
    in[3] = sdigit->GetPadx();
    in[4]= sdigit->GetPadz();
    GetMapIndeces(in,out);
    GetSDigitsData(2)->Fill( out[0],out[1]) ;//sdigit map
  }
}

//____________________________________________________________________________
void AliTOFQADataMakerSim::MakeSDigits(TTree * sdigitTree)
{
  //
  // makes data from SDigit Tree
  //
  TClonesArray * sdigits = new TClonesArray("AliTOFSDigit", 1000) ; 
  
  TBranch * branch = sdigitTree->GetBranch("TOF") ;
  if ( ! branch ) {
    AliError("TOF branch in SDigit Tree not found") ; 
    return;
  }
  branch->SetAddress(&sdigits) ;
  branch->GetEntry(0) ; 
  MakeSDigits(sdigits) ; 
}

//____________________________________________________________________________ 
void AliTOFQADataMakerSim::StartOfDetectorCycle()
{
  //
  //Detector specific actions at start of cycle
  //to be implemented  
}

//____________________________________________________________________________ 
void AliTOFQADataMakerSim::EndOfDetectorCycle(AliQA::TASKINDEX_t task, TObjArray * list)
{
  //Detector specific actions at end of cycle
  // do the QA checking

  AliQAChecker::Instance()->Run(AliQA::kTOF, task, list) ;  
}
//____________________________________________________________________________
void AliTOFQADataMakerSim::GetMapIndeces(Int_t* in , Int_t* out)
{
  //
  //return appropriate indeces for the theta-phi map
  //

  Int_t npadX = AliTOFGeometry::NpadX();
  Int_t npadZ = AliTOFGeometry::NpadZ();
  Int_t nStripA = AliTOFGeometry::NStripA();
  Int_t nStripB = AliTOFGeometry::NStripB();
  Int_t nStripC = AliTOFGeometry::NStripC();

  Int_t isector = in[0];
  Int_t iplate = in[1];
  Int_t istrip = in[2];
  Int_t ipadX = in[3]; 
  Int_t ipadZ = in[4]; 
  
  Int_t stripOffset = 0;
  switch (iplate) {
  case 0:
    stripOffset = 0;
      break;
  case 1:
    stripOffset = nStripC;
    break;
  case 2:
    stripOffset = nStripC+nStripB;
    break;
  case 3:
    stripOffset = nStripC+nStripB+nStripA;
    break;
  case 4:
    stripOffset = nStripC+nStripB+nStripA+nStripB;
    break;
  default:
    AliError(Form("Wrong plate number in TOF (%d) !",iplate));
    break;
  };
  Int_t zindex=npadZ*(istrip+stripOffset)+(ipadZ+1);
  Int_t phiindex=npadX*isector+ipadX+1;
  out[0]=zindex;  
  out[1]=phiindex;  
  
}
