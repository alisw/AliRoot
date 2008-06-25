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


// --- ROOT system ---
#include <TH1F.h> 
#include <TIterator.h> 
#include <TString.h> 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliLog.h"
#include "AliZDCQAChecker.h"

ClassImp(AliZDCQAChecker)

//__________________________________________________________________
AliZDCQAChecker& AliZDCQAChecker::operator = (const AliZDCQAChecker& qac )
{
  // Equal operator.
  this->~AliZDCQAChecker();
  new(this) AliZDCQAChecker(qac);
  return *this;
}


//____________________________________________________________________________
const Double_t AliZDCQAChecker::Check(AliQA::ALITASK_t index, TObjArray * list) 
{
  // Checks the QA histograms on the input list: 
  //
  Double_t test=0.;
  Int_t count=0, ntests=0; 
  const char* taskName = AliQA::GetAliTaskName(index);
  
  // -------------------------------------------------------------------
  if(index == AliQA::kSIM){
  
    if(list->GetEntries()==0){  
      AliWarning("\tAliZDCQAChecker->The list to be checked is empty!");
      test = 1.; // nothing to check
      return test;
    }
    printf("\n\tAliZDCQAChecker-> checking QA histograms for task %s\n\n",taskName);
    
    TIter next(list); 
    TH1 * hdata;	  
    Double_t meanX=0., meanY=0.;
    Double_t meanZNA=0., rmsZNA=0., meanZNC=0.;
    Double_t meanZPA=0., rmsZPA=0., meanZPC=0.;
    Double_t pmCZNA=0., pmCZNC=0., pmCZPA=0., pmCZPC=0.;
    Double_t pmQZNA=0., pmQZNC=0., pmQZPA=0., pmQZPC=0.;
    Float_t  res=0.;
    Int_t    digInd=0, testgood=0;
    //
    count = ntests = 0; 
    //
    while((hdata = dynamic_cast<TH1 *>(next()))){
      if(hdata){ 
        //printf("\tAliZDCQAChecker-> checking histo %s",hdata->GetName());
	// Check HITS histos
        if(!(strncmp(hdata->GetName(),"hZNCh",5))){ 
          // hits histos
          meanX = hdata->GetMean(1);
          meanY = hdata->GetMean(2);
          // check if the spot is centered
          if((TMath::Abs(meanX)<0.2) && (TMath::Abs(meanY)<0.2)) res=1.;
          else res=0.5;
          test += res;
          ntests++;
          // 
          //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
        }
        // Check DIGITS histos
        else{
          // [1] check response of ZNC vs. ZNA
          if(digInd==0 || digInd==1){
             if(digInd==0){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNA = hdata->GetMean();
        	 rmsZNA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNC = hdata->GetMean();
               }
               else testgood=0;
               // check if the response m.v. of ZNA and ZNC are equal (@ 1sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZNA-meanZNC)<rmsZNA) res=1.;
        	 else res=.5;
        	 testgood=0;
        	 test += res;
        	 ntests++;
          	 // 
          	 //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
               }
               else res=0.;
             }
          }
          // [2] check response of ZPC vs. ZPA
          else if(digInd==2 || digInd==3){
             if(digInd==2){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPA = hdata->GetMean();
        	 rmsZPA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPC = hdata->GetMean();
               }
               // check if the response m.v. of ZPA and ZPC are equal (@ 3sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZPA-meanZPC)<(3.*rmsZPA)) res=1.;
        	 else res=.5;
        	 test += res;
        	 ntests++;
        	 testgood=0;
          	 // 
          	 //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
               }
               else res=0.;
             }
          }
          // [2] check PMC responses vs. summed PMQ responses
          else if(digInd>3 && digInd<12){
            if(digInd==4) pmQZNC = hdata->GetMean();
            else if(digInd==5) pmQZNA = hdata->GetMean();
            else if(digInd==6) pmQZPC = hdata->GetMean();
            else if(digInd==7) pmQZPA = hdata->GetMean();
            else if(digInd==8){
              pmCZNC = hdata->GetMean();
              if(TMath::Abs(pmQZNC-pmCZNC)<(0.1*(pmQZNC+pmCZNC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
              // 
              //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
            }
            else if(digInd==9){
              pmCZNA = hdata->GetMean();
              if(TMath::Abs(pmQZNA-pmCZNA)<(0.1*(pmQZNA+pmCZNA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
              // 
              //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
            }
            else if(digInd==10){
              pmCZPC = hdata->GetMean();
              if(TMath::Abs(pmQZPC-pmCZPC)<(0.1*(pmQZPC+pmCZPC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
              // 
              //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
            }
            else if(digInd==11){
              pmCZPA = hdata->GetMean();
              if(TMath::Abs(pmQZPA-pmCZPA)<(0.1*(pmQZPA+pmCZPA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
              // 
              //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
            }
          }
          //
          digInd++;
        }
        //
        count++;
      }
      else{
        AliError("AliZDCQAChecker-> No histos!!!\n");
      }
    }
    if(ntests!=0) test = test/ntests;
    printf("\n\tAliZDCQAChecker-> QA check result = %1.2f\n",test);
  }
  
  // -------------------------------------------------------------------
  else if(index == AliQA::kRAW){
  
    if(list->GetEntries()==0){  
      AliWarning("\tAliZDCQAChecker->The list to be checked is empty!");
      test = 1.; // nothing to check
      return test;
    }
    printf("\n\tAliZDCQAChecker-> checking QA histograms for task %s\n\n",taskName);
    
    TIter next(list); 
    TH1 * hdata;
    Double_t meanZNA=0., rmsZNA=0., meanZNC=0.;
    Double_t meanZPA=0., rmsZPA=0., meanZPC=0.;
    Double_t pmCZNA=0., pmCZNC=0., pmCZPA=0., pmCZPC=0.;
    Double_t pmQZNA=0., pmQZNC=0., pmQZPA=0., pmQZPC=0.;
    Float_t  res=0.;
    Int_t    rawInd=0, testgood=0;
    count = ntests = 0; 
    //
    while((hdata = dynamic_cast<TH1 *>(next()))){
      if(hdata){
          // [1] check response of ZNC vs. ZNA
          if(rawInd==0 || rawInd==1){
             if(rawInd==0){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNA = hdata->GetMean();
        	 rmsZNA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNC = hdata->GetMean();
               }
               else testgood=0;
               // check if the response m.v. of ZNA and ZNC are equal (@ 1sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZNA-meanZNC)<rmsZNA) res=1.;
        	 else res=.5;
        	 test += res;
        	 ntests++;
        	 testgood=0;
               }
               else res=0.;
             }
          }
          // [2] check response of ZPC vs. ZPA
          else if(rawInd==2 || rawInd==3){
             if(rawInd==2){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPA = hdata->GetMean();
        	 rmsZPA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPC = hdata->GetMean();
               }
               // check if the response m.v. of ZPA and ZPC are equal (@ 3sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZPA-meanZPC)<(3.*rmsZPA)) res=1.;
        	 else res=.5;
        	 test += res;
        	 ntests++;
        	 testgood=0;
               }
               else res=0.;
             }
          }
          // [2] check PMC responses vs. summed PMQ responses
          else if(rawInd>3 && rawInd<12){
            if(rawInd==4) pmQZNC = hdata->GetMean();
            else if(rawInd==5) pmQZNA = hdata->GetMean();
            else if(rawInd==6) pmQZPC = hdata->GetMean();
            else if(rawInd==7) pmQZPA = hdata->GetMean();
            else if(rawInd==8){
              pmCZNC = hdata->GetMean();
              if(TMath::Abs(pmQZNC-pmCZNC)<(0.1*(pmQZNC+pmCZNC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(rawInd==9){
              pmCZNA = hdata->GetMean();
              if(TMath::Abs(pmQZNA-pmCZNA)<(0.1*(pmQZNA+pmCZNA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(rawInd==10){
              pmCZPC = hdata->GetMean();
              if(TMath::Abs(pmQZPC-pmCZPC)<(0.1*(pmQZPC+pmCZPC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(rawInd==11){
              pmCZPA = hdata->GetMean();
              if(TMath::Abs(pmQZPA-pmCZPA)<(0.1*(pmQZPA+pmCZPA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
          }
          //
          rawInd++;
          count++;
      }
      // 
      //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
      else{
        AliError("\t AliZDCQAChecker->No histos!!!\n");
      }
    }
    if(ntests!=0) test = test/ntests;
    printf("\n\tAliZDCQAChecker-> QA check result = %1.2f\n",test);
  }

  // -------------------------------------------------------------------
  else if(index == AliQA::kESD){
  
    if(list->GetEntries()==0){  
      AliWarning("\tAliZDCQAChecker->The list to be checked is empty!");
      test = 1.; // nothing to check
      return test;
    }
    printf("\n\tAliZDCQAChecker-> checking QA histograms for task %s\n\n",taskName);
    
    TIter next(list); 
    TH1 * hdata;
    Double_t meanX=0., meanY=0.;
    Double_t meanZNA=0., rmsZNA=0., meanZNC=0.;
    Double_t meanZPA=0., rmsZPA=0., meanZPC=0.;
    Double_t eneCZNA=0., eneCZNC=0., eneCZPA=0., eneCZPC=0.;
    Double_t eneQZNA=0., eneQZNC=0., eneQZPA=0., eneQZPC=0.;
    Float_t  res=0.;
    Int_t    esdInd=0, testgood=0;
    //
    count = ntests = 0; 
    //
    while((hdata = dynamic_cast<TH1 *>(next()))){
      if(hdata){ 
        //printf("\tAliZDCQAChecker-> checking histo %s",hdata->GetName());
        if(esdInd<2){
          // hits histos
          meanX = hdata->GetMean(1);
          meanY = hdata->GetMean(2);
          // check if the spot is centered
          if((TMath::Abs(meanX)<0.2) && (TMath::Abs(meanY)<0.2)) res=1.;
          else res=0.5;
          test += res;
          ntests++;
        }
        //
        else{
          // [1] check response of ZNC vs. ZNA
          if(esdInd==0 || esdInd==1){
             if(esdInd==0){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNA = hdata->GetMean();
        	 rmsZNA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZNC = hdata->GetMean();
               }
               else testgood=0;
               // check if the response m.v. of ZNA and ZNC are equal (@ 1sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZNA-meanZNC)<rmsZNA) res=1.;
        	 else res=.5;
        	 testgood=0;
        	 test += res;
        	 ntests++;
               }
               else res=0.;
             }
          }
          // [2] check response of ZPC vs. ZPA
          else if(esdInd==2 || esdInd==3){
             if(esdInd==2){
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPA = hdata->GetMean();
        	 rmsZPA = hdata->GetRMS();
               }
             }
             else{
               if(hdata->GetEntries() != 0.){
        	 testgood=1;
        	 meanZPC = hdata->GetMean();
               }
               // check if the response m.v. of ZPA and ZPC are equal (@ 3sigma level)
               if(testgood==1){
        	 if(TMath::Abs(meanZPA-meanZPC)<(3.*rmsZPA)) res=1.;
        	 else res=.5;
        	 test += res;
        	 ntests++;
        	 testgood=0;
               }
               else res=0.;
             }
          }
          // [2] check eneC responses vs. summed eneQ responses
          else if(esdInd>3 && esdInd<12){
            if(esdInd==4) eneQZNC = hdata->GetMean();
            else if(esdInd==5) eneQZNA = hdata->GetMean();
            else if(esdInd==6) eneQZPC = hdata->GetMean();
            else if(esdInd==7) eneQZPA = hdata->GetMean();
            else if(esdInd==8){
              eneCZNC = hdata->GetMean();
              if(TMath::Abs(eneQZNC-eneCZNC)<(0.1*(eneQZNC+eneCZNC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(esdInd==9){
              eneCZNA = hdata->GetMean();
              if(TMath::Abs(eneQZNA-eneCZNA)<(0.1*(eneQZNA+eneCZNA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(esdInd==10){
              eneCZPC = hdata->GetMean();
              if(TMath::Abs(eneQZPC-eneCZPC)<(0.1*(eneQZPC+eneCZPC)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
            else if(esdInd==11){
              eneCZPA = hdata->GetMean();
              if(TMath::Abs(eneQZPA-eneCZPA)<(0.1*(eneQZPA+eneCZPA)/2)) res=1.;
              else res=0.5;
              test += res;
              ntests++;
            }
          }
          //
          esdInd++;
        }
        // 
        //printf("\t %d performed tests, results %1.2f\n",ntests,test/ntests);
        //
        count++;
      }
      else{
        AliError("AliZDCQAChecker-> No histos!!!\n");
      }
    }
    if(ntests!=0) test = test/ntests;
    printf("\n\tAliZDCQAChecker-> QA check result = %1.2f\n\n",test);
  }
  else{
    AliWarning(Form("\n\t No ZDC QA for %s task\n",taskName)); 
    return 1.;
  }
  
  AliInfo(Form("Test Result = %f", test)); 
  return test; 
}  
