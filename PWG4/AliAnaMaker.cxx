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

/* History of cvs commits:
 *
 * $Log$
 *
 *
 */

//_________________________________________________________________________
// Base class for particle (gamma, hadron) identification and correlation analysis
// It is called by the task class AliAnalysisGammaTask and it connects the input 
// (ESD/AOD/MonteCarlo)
// got with AliCaloTrackReader (produces TClonesArrays of TParticles), with the 
// analysis classes that derive from AliAnaBaseClass
//
//*-- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system ---

	      //#include <TParticle.h>
	      //#include <TH2.h>

//---- AliRoot system ---- 
#include "AliAnaBaseClass.h" 
#include "AliAnaMaker.h" 
#include "AliCaloTrackReader.h" 
// #include "AliAODCaloCluster.h"
// #include "AliAODTrack.h"
// #include "AliAODEvent.h"
#include "Riostream.h"
#include "AliLog.h"

ClassImp(AliAnaMaker)


//____________________________________________________________________________
  AliAnaMaker::AliAnaMaker() : 
    TObject(),
    fOutputContainer(new TList ), fAnalysisContainer(new TList ),
    fMakeHisto(0), fMakeAOD(0), fAnaDebug(0), 
    fReader(0x0), fAODBranch(0x0), fAODBranchName("")
{
  //Default Ctor
  if(fAnaDebug > 1 ) printf("*** Analysis Maker  Constructor *** \n");
  
  //Initialize parameters, pointers and histograms
  if(!fReader)
    fReader = new AliCaloTrackReader();
  
  InitParameters();
}

//____________________________________________________________________________
AliAnaMaker::AliAnaMaker(const AliAnaMaker & g) :   
  TObject(),
  fOutputContainer(g. fOutputContainer), fAnalysisContainer(g.fAnalysisContainer), 
  fMakeHisto(g.fMakeHisto), fMakeAOD(fMakeAOD), fAnaDebug(g. fAnaDebug),
  fReader(g.fReader), fAODBranch(g.fAODBranch), 
  fAODBranchName(g.fAODBranchName)
{
  // cpy ctor
  
}

//_________________________________________________________________________
AliAnaMaker & AliAnaMaker::operator = (const AliAnaMaker & source)
{
  // assignment operator

  if(this == &source)return *this;
  ((TObject *)this)->operator=(source);

  fOutputContainer = source.fOutputContainer ;
  fAnalysisContainer = source.fAnalysisContainer ;
  fAnaDebug = source.fAnaDebug;
  fMakeHisto = source.fMakeHisto;
  fMakeAOD = source.fMakeAOD;

  fAODBranchName = source.fAODBranchName;
  fAODBranch = source.fAODBranch;
 
  fReader = source.fReader ;

  return *this;

}

//____________________________________________________________________________
AliAnaMaker::~AliAnaMaker() 
{
  // Remove all pointers.

  // Protection added in case of NULL pointers (MG)
  if (fOutputContainer) {
     fOutputContainer->Clear();
     delete fOutputContainer ;
  }   

  if (fAnalysisContainer) {
     fAnalysisContainer->Clear();
     delete fAnalysisContainer ;
  }   

  if (fReader) delete fReader ;

  if (fAODBranch) {
    fAODBranch->Clear();
    delete fAODBranch ;
  }

}

//________________________________________________________________________
void AliAnaMaker::Init()
{  
  //Init container histograms and other common variables

  if(fMakeHisto){// Analysis with histograms as output on
    
    //Fill container with appropriate histograms
    
    if(!fAnalysisContainer || fAnalysisContainer->GetEntries()==0)
      AliFatal("Analysis job list not initailized");

    for(Int_t iana = 0; iana <  fAnalysisContainer->GetEntries(); iana++){
      TList * templist =  ((AliAnaBaseClass *) fAnalysisContainer->At(iana)) -> GetCreateOutputObjects(); 

      for(Int_t i = 0; i < templist->GetEntries(); i++)
	fOutputContainer->Add(templist->At(i)) ;
    
    }
  }// Analysis with histograms as output on
}

//____________________________________________________________________________
void AliAnaMaker::InitParameters()
{

  //Init data members
  fMakeHisto = kTRUE;
  fMakeAOD = kTRUE; 
  fAnaDebug = 0; // No debugging info displayed by default
  fAODBranchName = "Photons" ;

}

//__________________________________________________________________
void AliAnaMaker::Print(const Option_t * opt) const
{

  //Print some relevant parameters set for the analysis
  if(! opt)
    return;

  printf("***** Print: %s %s ******\n", GetName(), GetTitle() ) ;
  printf("Debug level     =     %d\n", fAnaDebug) ;
  printf("New AOD branch  =     %s\n", fAODBranchName.Data()) ;
  printf("Produce Histo   =     %d\n", fMakeHisto) ;
  printf("Produce AOD     =     %d\n", fMakeAOD) ;

} 


//____________________________________________________________________________
Bool_t AliAnaMaker::ProcessEvent(Int_t iEntry){
  //Process analysis for this event
  
  if(fMakeHisto && !fOutputContainer)
    AliFatal("Histograms not initialized");
  
  if(fAnaDebug >= 0 ) printf("***  Event %d   ***  \n",iEntry);

  //Each event needs an empty branch
  fAODBranch->Delete();
  
  //Tell the reader to fill the data in the 3 detector lists
  fReader->FillInputEvent();

  //Loop on analysis algorithms
  Int_t nana = fAnalysisContainer->GetEntries() ;
  for(Int_t iana = 0; iana <  nana; iana++){
    
    AliAnaBaseClass * ana =  ((AliAnaBaseClass *) fAnalysisContainer->At(iana)) ; 
    
    //Set reader and aod branch for each analysis
    ana->SetReader(fReader);
    ana->SetAODBranch(fAODBranch);
    
    //Make analysis, create aods in aod branch or AODCaloClusters
    if(fMakeAOD) ana->MakeAnalysisFillAOD()  ;
    //Make further analysis with aod branch and fill histograms
    if(fMakeHisto) ana->MakeAnalysisFillHistograms()  ;
    
  }
  
  fReader->ResetLists();
  
  return kTRUE ;
  
}
