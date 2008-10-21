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


/* $Id: $ */

/*
  Checks the quality assurance. 
  By comparing with reference data
  Y. Schutz CERN July 2007
*/

// --- ROOT system ---
#include <TClass.h>
#include <TH1F.h> 
#include <TH1I.h> 
#include <TIterator.h> 
#include <TKey.h> 
#include <TFile.h> 
#include <TNtupleD.h>

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliQA.h"
#include "AliQAChecker.h"
#include "AliCorrQAChecker.h"

ClassImp(AliCorrQAChecker)

//__________________________________________________________________
Double_t AliCorrQAChecker::Check(AliQA::ALITASK_t index, TNtupleD * nData) 
{
 // check the QA of correlated data stored in a ntuple
  if ( index != AliQA::kRAW ) {
    AliWarning("Checker not implemented") ; 
    return 1.0 ; 
  }
  Double_t test = 0.0  ;
//	if (!fRefSubDir) {
//		test = 1. ; // no reference data
//	} else {
    if ( ! nData ) {
      AliError(Form("nRawCorr not found in %s", fDataSubDir->GetName())) ; 
    } else {
      TObjArray * bList = nData->GetListOfBranches() ; 
      for (Int_t b = 0 ; b < bList->GetEntries() ; b++) {
        AliInfo(Form("Ntuple parameter name %d : %s", b, bList->At(b)->GetName())) ;  
      }
    }
 // }
  return test ; 
}

