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



//---
//  Produces the data needed to calculate the quality assurance. 
//  All data must be mergeable objects.

//  Authors:
//
//  Luciano Diaz Gonzalez <luciano.diaz@nucleares.unam.mx> (ICN-UNAM)
//  Mario Rodriguez Cahuantzi <mrodrigu@mail.cern.ch> (FCFM-BUAP)
//  Arturo Fernandez Tellez <afernan@mail.cern.ch (FCFM-BUAP)
//
//  Created: June 13th 2008
//---

// --- ROOT system ---
#include <TClonesArray.h>
#include <TFile.h> 
#include <TH1F.h> 
#include <TH2F.h>
#include <TDirectory.h>
// --- Standard library ---

// --- AliRoot header files ---
#include "AliESDEvent.h"
#include "AliLog.h"
#include "AliACORDEdigit.h" 
#include "AliACORDEhit.h"
#include "AliACORDEQADataMakerSim.h"
#include "AliQAChecker.h"
#include "AliACORDERawReader.h"
ClassImp(AliACORDEQADataMakerSim)
           
//____________________________________________________________________________ 
AliACORDEQADataMakerSim::AliACORDEQADataMakerSim():AliQADataMakerSim(AliQA::GetDetName(AliQA::kACORDE), "ACORDE Quality Assurance Data Maker")
{
}
//____________________________________________________________________________ 
AliACORDEQADataMakerSim::AliACORDEQADataMakerSim(const AliACORDEQADataMakerSim& qadm) :
  AliQADataMakerSim() 
{
  SetName((const char*)qadm.GetName()) ; 
  SetTitle((const char*)qadm.GetTitle()); 
}
//__________________________________________________________________
AliACORDEQADataMakerSim& AliACORDEQADataMakerSim::operator = (const AliACORDEQADataMakerSim& qadm )
{
  // Equal operator.
  this->~AliACORDEQADataMakerSim();
  new(this) AliACORDEQADataMakerSim(qadm);
  return *this;
}
//____________________________________________________________________________
void AliACORDEQADataMakerSim::EndOfDetectorCycle(AliQA::TASKINDEX_t task, TObjArray * list)
{
  //Detector specific actions at end of cycle
  // do the QA checking
   printf("ACORDE---->Detector specific actions at END of cycle\n................\n");

  AliQAChecker::Instance()->Run(AliQA::kACORDE, task, list) ;
}
//____________________________________________________________________________
void AliACORDEQADataMakerSim::StartOfDetectorCycle()
{
  //Detector specific actions at start of cycle
  printf("ACORDE---->Detector specific actions at START of cycle\n................\n");
}
//____________________________________________________________________________ 
void AliACORDEQADataMakerSim::InitHits()
{
  // create Hits histograms in Hits subdir
	
	TH1F *   fHitsACORDE;
	fHitsACORDE = new TH1F("hACORDEBitPattern","Distribution of fired modules",60,0,60);
	Add2HitsList(fHitsACORDE,0);
}
//____________________________________________________________________________ 
void AliACORDEQADataMakerSim::InitDigits()
{
  // create Digits histograms in Digits subdir

   TH1F *    fhDigitsModule;
   TString   modulename;
   modulename = "hDigitsModule";
   fhDigitsModule = new TH1F(modulename.Data(),"hDigitsModuleSingle",60,0,60);
   Add2DigitsList( fhDigitsModule,0);

}
//____________________________________________________________________________

void AliACORDEQADataMakerSim::MakeHits(TTree *hitTree)
{
  // Here we fill the QA histos for Hits declared above

	TClonesArray * hits = new TClonesArray("AliACORDEhit",1000);
	TBranch * branch = hitTree->GetBranch("ACORDE");
	if (!branch)
	{
		AliWarning("ACORDE branch in Hit Tree not found");
	}else
	{
		if (branch)
		{
			branch->SetAddress(&hits);
		}else
		{
			AliError("Branch ACORDE hit not found");
			exit(111);
		}
		Int_t ntracks = (Int_t)hitTree->GetEntries();
		if (ntracks<=0) return;
		for(Int_t track=0;track<ntracks;track++)
		{
			branch->GetEntry(track);
			Int_t nhits = hits->GetEntriesFast();
			for(Int_t ihit=1;ihit<=nhits;ihit++)
			{
				AliACORDEhit *AcoHit = (AliACORDEhit*) hits->UncheckedAt(ihit);
				if(!AcoHit)
				{
					AliError("The unchecked hit doesn't exist");
					break;
				}
				GetHitsData(0)->Fill(AcoHit->GetModule()-1);
			}
		}
	}

}
//____________________________________________________________________________
void AliACORDEQADataMakerSim::MakeDigits( TTree *digitsTree)
{
  //fills QA histos for Digits


        TClonesArray * digits = new TClonesArray("AliACORDEdigit",1000);
        TBranch * branch = digitsTree->GetBranch("ACORDEdigit");
        if (!branch)
        {
                AliWarning("ACORDE branch in Digits Tree not found");
        }else
        {
                if (branch)
                {
                        branch->SetAddress(&digits);
                }else
                {
                        AliError("Branch ACORDE digit not found");
                        exit(111);
                }
                Int_t ntracks = (Int_t)digitsTree->GetEntries();
                if (ntracks<=0) return;
                printf("Entries in DigitsTree:%d\n",ntracks);
                for(Int_t track=0;track<ntracks;track++)
                {
                        branch->GetEntry(track);
                        Int_t ndigits = digits->GetEntriesFast();
                        for(Int_t idigit=0;idigit<ndigits;idigit++)
                        {
                                AliACORDEdigit *AcoDigit = (AliACORDEdigit*) digits->UncheckedAt(idigit);
                                if (!AcoDigit)
                                {
                                        AliError("The unchecked digit doesn't exist");
                                        break;
                                }
                                GetDigitsData(0)->Fill(AcoDigit->GetModule()-1);
                        }
                }


        }

}
