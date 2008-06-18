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
/* $Id */

#include "Riostream.h"
#include "AliFlowEventSimpleMaker.h"
#include "AliFlowEventSimple.h"
#include "AliFlowTrackSimple.h"
#include "TTree.h"
#include "TParticle.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"

// AliFlowEventSimpleMaker:
// Class to fill the AliFlowEventSimple
// with AliFlowTrackSimple objects
// Has fill methods for TTree, AliMCEvent, AliESDEvent and AliAODEvent
// author: N. van der Kolk (kolk@nikhef.nl)



ClassImp(AliFlowEventSimpleMaker)
//----------------------------------------------------------------------- 
AliFlowEventSimpleMaker::AliFlowEventSimpleMaker()
{
  //constructor
}

//-----------------------------------------------------------------------   
AliFlowEventSimpleMaker::~AliFlowEventSimpleMaker()
{
  //destructor
}

//-----------------------------------------------------------------------   
AliFlowEventSimple* AliFlowEventSimpleMaker::FillTracks(TTree* anInput)
{
  //fills the event from a TTree of kinematic.root files
  Bool_t  bDoubleLoop = kFALSE; 

  Int_t iNumberOfInputTracks = anInput->GetEntries() ;
  //cerr<<"iNumberOfInputTracks = "<<iNumberOfInputTracks<<endl;
  TParticle* pParticle = new TParticle();
  anInput->SetBranchAddress("Particles",&pParticle);  
  //  AliFlowEventSimple* pEvent = new AliFlowEventSimple(iNumberOfInputTracks);
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(10);
  //cerr<<pEvent<<" pEvent "<<endl;
  
  Int_t iN = iNumberOfInputTracks;
  //  Int_t iN = 576; //multiplicity for chi=1.5
  //  Int_t iN = 256; //multiplicity for chi=1
  //  Int_t iN = 164; //multiplicity for chi=0.8
  Int_t iGoodTracks = 0;
  Int_t itrkN = 0;
  Int_t iSelParticlesDiff = 0;
  Int_t iSelParticlesInt = 0;
  
  if (bDoubleLoop)
    {                   //double loop
      while (iGoodTracks < iN*2 && itrkN < iNumberOfInputTracks) 
	{
	  anInput->GetEntry(itrkN);   //get input particle
	  //cut on tracks
	  if(TMath::Abs(pParticle->Eta()) < 0.9) 
	    {
	      //	      Int_t fLoop = floor(2.*fParticle->Pt())+2;
	      //	      for(Int_t d=0;d<fLoop;d++) 
	      for(Int_t d=0;d<2;d++) 
		{
		  if(
		     TMath::Abs(pParticle->GetPdgCode()) == 211
		     //	      TMath::Abs(pParticle->GetPdgCode()) == 211 ||
		     //	      TMath::Abs(pParticle->GetPdgCode()) == 321 ||
		     //	      TMath::Abs(pParticle->GetPdgCode()) == 2212
		     )
		    {
		      AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
		      pTrack->SetPt(pParticle->Pt() );
		      pTrack->SetEta(pParticle->Eta() );
		      pTrack->SetPhi(pParticle->Phi() );
		      pTrack->SetForIntegratedFlow(kTRUE);
		      pTrack->SetForDifferentialFlow(kTRUE);

		      if (pTrack->UseForIntegratedFlow())
			{ iSelParticlesInt++; }
		      if (pTrack->UseForDifferentialFlow())
			{ iSelParticlesDiff++; }
		      iGoodTracks++;
		      pEvent->TrackCollection()->Add(pTrack);
		    }
			/*
		  else if(
			  TMath::Abs(pParticle->GetPdgCode()) == 2212
			  )
		    {
		      AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
		      pTrack->SetPt(pParticle->Pt() );
		      pTrack->SetEta(pParticle->Eta() );
		      pTrack->SetPhi(pParticle->Phi() );
		      pTrack->SetForIntegratedFlow(kFALSE);
		      pTrack->SetForDifferentialFlow(kTRUE);

		      if (pTrack->UseForIntegratedFlow())
			{ iSelParticlesInt++; }
		      if (pTrack->UseForDifferentialFlow())
			{ iSelParticlesDiff++; }
		      iGoodTracks++;
		      pEvent->TrackCollection()->Add(pTrack);     
		    }
			*/
		}
	    }
	  itrkN++; 
	}
    }

  else {                                  //normal loop
    while (iGoodTracks < iN && itrkN < iNumberOfInputTracks) {
      anInput->GetEntry(itrkN);   //get input particle
      //cut on tracks
      if (TMath::Abs(pParticle->Eta()) < 0.2)
	{
	  if(
	     TMath::Abs(pParticle->GetPdgCode()) == 211
	     //	      TMath::Abs(pParticle->GetPdgCode()) == 211 ||
	     //	      TMath::Abs(pParticle->GetPdgCode()) == 321 ||
	     //	      TMath::Abs(pParticle->GetPdgCode()) == 2212
	     )
	    {
	      AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	      pTrack->SetPt(pParticle->Pt() );
	      pTrack->SetEta(pParticle->Eta() );
	      pTrack->SetPhi(pParticle->Phi() );
	      pTrack->SetForIntegratedFlow(kTRUE);
	      pTrack->SetForDifferentialFlow(kTRUE);

	      if (pTrack->UseForIntegratedFlow())
		{ iSelParticlesInt++; }
	      if (pTrack->UseForDifferentialFlow())
		{ iSelParticlesDiff++; }
	      iGoodTracks++;
	      pEvent->TrackCollection()->Add(pTrack) ;  	     
	    }
	  /*	  else if(
		  TMath::Abs(pParticle->GetPdgCode()) == 211
		  )
	    {
	      AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	      pTrack->SetPt(pParticle->Pt() );
	      pTrack->SetEta(pParticle->Eta() );
	      pTrack->SetPhi(pParticle->Phi() );
	      pTrack->SetForIntegratedFlow(kFALSE);
	      pTrack->SetForDifferentialFlow(kTRUE);

	      if (pTrack->UseForIntegratedFlow())
		{ iSelParticlesInt++; }
	      if (pTrack->UseForDifferentialFlow())
		{ iSelParticlesDiff++; }
	      iGoodTracks++;
	      pEvent->TrackCollection()->Add(pTrack);  	     
	    }
	  */
	}
      
      itrkN++; 
    }
  }

  pEvent-> SetEventNSelTracksIntFlow(iSelParticlesInt);  
  pEvent->SetNumberOfTracks(iGoodTracks);
  cout<<" iGoodTracks = "<<iGoodTracks<<endl;
  cout << "  iSelectedTracksInt = " << iSelParticlesInt << endl;  
  return pEvent;
  
}

//-----------------------------------------------------------------------   
AliFlowEventSimple* AliFlowEventSimpleMaker::FillTracks(AliMCEvent* anInput)
{
  //Fills the event from the MC kinematic information
  
  Int_t iNumberOfInputTracks = anInput->GetNumberOfTracks() ;
  cerr<<"anInput->GetNumberOfTracks() = "<<iNumberOfInputTracks<<endl;
 
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(10);
    
  //Int_t iN = 256; //multiplicity for chi=1
  Int_t iN = iNumberOfInputTracks;
  Int_t iGoodTracks = 0;
  Int_t itrkN = 0;
  Int_t iSelParticlesDiff = 0;
  Int_t iSelParticlesInt = 0;

   
  //normal loop
  while (iGoodTracks < iN && itrkN < iNumberOfInputTracks) {
    AliMCParticle* pParticle = anInput->GetTrack(itrkN);   //get input particle
    //cut on tracks
    if (TMath::Abs(pParticle->Eta()) < 0.2)
      {
	if(
	   TMath::Abs(pParticle->Particle()->GetPdgCode()) == 211
	   //	      TMath::Abs(pParticle->Particle()->GetPdgCode()) == 211 ||
	   //	      TMath::Abs(pParticle->Particle()->GetPdgCode()) == 321 ||
	   //	      TMath::Abs(pParticle->Particle()->GetPdgCode()) == 2212
	   )
	  {
	    AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	    pTrack->SetPt(pParticle->Pt() );
	    pTrack->SetEta(pParticle->Eta() );
	    pTrack->SetPhi(pParticle->Phi() );
	    pTrack->SetForIntegratedFlow(kTRUE);
	    pTrack->SetForDifferentialFlow(kTRUE);

	    if (pTrack->UseForIntegratedFlow())
	      { iSelParticlesInt++; }
	    if (pTrack->UseForDifferentialFlow())
	      { iSelParticlesDiff++; }
	    iGoodTracks++;
	    pEvent->TrackCollection()->Add(pTrack) ;  	     
	  }
	  /*	  else if(
		  TMath::Abs(pParticle->Particle()->GetPdgCode()) == 211
		  )
	    {
	      AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	      pTrack->SetPt(pParticle->Pt() );
	      pTrack->SetEta(pParticle->Eta() );
	      pTrack->SetPhi(pParticle->Phi() );
	      pTrack->SetForIntegratedFlow(kFALSE);
	      pTrack->SetForDifferentialFlow(kTRUE);

	      if (pTrack->UseForIntegratedFlow())
		{ iSelParticlesInt++; }
	      if (pTrack->UseForDifferentialFlow())
		{ iSelParticlesDiff++; }
	      iGoodTracks++;
	      pEvent->TrackCollection()->Add(pTrack);  	     
	    }
	  */
      }
      
    itrkN++; 
  }
  
  pEvent-> SetEventNSelTracksIntFlow(iSelParticlesInt);  
  pEvent->SetNumberOfTracks(iGoodTracks);
  cout<<" iGoodTracks = "<<iGoodTracks<<endl;
  cout << "  iSelectedTracksInt = " << iSelParticlesInt << endl;  
  return pEvent;

}

//-----------------------------------------------------------------------   
AliFlowEventSimple* AliFlowEventSimpleMaker::FillTracks(AliESDEvent* anInput)
{
  //Fills the event from the ESD
  
  Int_t iNumberOfInputTracks = anInput->GetNumberOfTracks() ;
  cerr<<"anInput->GetNumberOfTracks() = "<<iNumberOfInputTracks<<endl;
  
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(10);
    
  //Int_t iN = 256; //multiplicity for chi=1
  Int_t iN = iNumberOfInputTracks;
  Int_t iGoodTracks = 0;
  Int_t itrkN = 0;
  Int_t iSelParticlesDiff = 0;
  Int_t iSelParticlesInt = 0;


  //normal loop
  while (iGoodTracks < iN && itrkN < iNumberOfInputTracks) {
    AliESDtrack* pParticle = anInput->GetTrack(itrkN);   //get input particle
    //cut on tracks
    if (TMath::Abs(pParticle->Eta()) < 0.2)
      {
	AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	pTrack->SetPt(pParticle->Pt() );
	pTrack->SetEta(pParticle->Eta() );
	pTrack->SetPhi(pParticle->Phi() );
	pTrack->SetForIntegratedFlow(kTRUE);
	pTrack->SetForDifferentialFlow(kTRUE);

	if (pTrack->UseForIntegratedFlow())
	  { iSelParticlesInt++; }
	if (pTrack->UseForDifferentialFlow())
	  { iSelParticlesDiff++; }
	iGoodTracks++;
	pEvent->TrackCollection()->Add(pTrack) ;  	     
      }
      
    itrkN++; 
  }
  
  pEvent-> SetEventNSelTracksIntFlow(iSelParticlesInt);  
  pEvent->SetNumberOfTracks(iGoodTracks);
  cout<<" iGoodTracks = "<<iGoodTracks<<endl;
  cout << "  iSelectedTracksInt = " << iSelParticlesInt << endl;  
  return pEvent;


}



//-----------------------------------------------------------------------   
AliFlowEventSimple* AliFlowEventSimpleMaker::FillTracks(AliAODEvent* anInput)
{
  //Fills the event from the AOD
  
  Int_t iNumberOfInputTracks = anInput->GetNumberOfTracks() ;
  cerr<<"anInput->GetNumberOfTracks() = "<<iNumberOfInputTracks<<endl;
  
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(10);
    
  //Int_t iN = 256; //multiplicity for chi=1
  Int_t iN = iNumberOfInputTracks;
  Int_t iGoodTracks = 0;
  Int_t itrkN = 0;
  Int_t iSelParticlesDiff = 0;
  Int_t iSelParticlesInt = 0;

  
  //normal loop
  while (iGoodTracks < iN && itrkN < iNumberOfInputTracks) {
    AliAODTrack* pParticle = anInput->GetTrack(itrkN);   //get input particle
    //cut on tracks
    if (TMath::Abs(pParticle->Eta()) < 0.2)
      {
	AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	pTrack->SetPt(pParticle->Pt() );
	pTrack->SetEta(pParticle->Eta() );
	pTrack->SetPhi(pParticle->Phi() );
	pTrack->SetForIntegratedFlow(kTRUE);
	pTrack->SetForDifferentialFlow(kTRUE);

	if (pTrack->UseForIntegratedFlow())
	  { iSelParticlesInt++; }
	if (pTrack->UseForDifferentialFlow())
	  { iSelParticlesDiff++; }
	iGoodTracks++;
	pEvent->TrackCollection()->Add(pTrack) ;  	     
      }
      
    itrkN++; 
  }
  
  pEvent-> SetEventNSelTracksIntFlow(iSelParticlesInt);  
  pEvent->SetNumberOfTracks(iGoodTracks);
  cout<<" iGoodTracks = "<<iGoodTracks<<endl;
  cout << "  iSelectedTracksInt = " << iSelParticlesInt << endl;  
  return pEvent;
  
}
//-----------------------------------------------------------------------   
AliFlowEventSimple*  AliFlowEventSimpleMaker::FillTracks(AliESDEvent* anInput, AliMCEvent* anInputMc, Int_t anOption)
{
  //fills the event with tracks from the ESD and kinematics from the MC info via the track label

  if (!(anOption ==0 || anOption ==1)) {
    cout<<"WRONG OPTION IN AliFlowEventSimpleMaker::FillTracks(AliESDEvent* anInput, AliMCEvent* anInputMc, Int_t anOption)"<<endl;
    exit(1);
  }

  Int_t iNumberOfInputTracks = anInput->GetNumberOfTracks() ;
  cerr<<"anInput->GetNumberOfTracks() = "<<iNumberOfInputTracks<<endl;
  
  AliFlowEventSimple* pEvent = new AliFlowEventSimple(10);
    
  //Int_t iN = 256; //multiplicity for chi=1
  Int_t iN = iNumberOfInputTracks;
  Int_t iGoodTracks = 0;
  Int_t itrkN = 0;
  Int_t iSelParticlesDiff = 0;
  Int_t iSelParticlesInt = 0;

  //normal loop
  while (iGoodTracks < iN && itrkN < iNumberOfInputTracks) {
    AliESDtrack* pParticle = anInput->GetTrack(itrkN);   //get input particle
    //get Label
    Int_t iLabel = pParticle->GetLabel();
    //match to mc particle
    AliMCParticle* pMcParticle = anInputMc->GetTrack(TMath::Abs(iLabel));
    
    //check
    if (TMath::Abs(pParticle->GetLabel())!=pMcParticle->Label()) cout<<"pParticle->GetLabel()!=pMcParticle->Label() "<<pParticle->GetLabel()<<"  "<<pMcParticle->Label()<<endl;
    
    //cut on tracks
    if (TMath::Abs(pParticle->Eta()) < 0.2)
      {
	if(
	   TMath::Abs(pMcParticle->Particle()->GetPdgCode()) == 211 //pions
	   //	      TMath::Abs(pMcParticle->Particle()->GetPdgCode()) == 211 ||
	   //	      TMath::Abs(pMcParticle->Particle()->GetPdgCode()) == 321 ||
	   //	      TMath::Abs(pMcParticle->Particle()->GetPdgCode()) == 2212
	   )
	  {
	    AliFlowTrackSimple* pTrack = new AliFlowTrackSimple();
	    if(anOption == 0) { //take the PID from the MC & the kinematics from the ESD
	      pTrack->SetPt(pParticle->Pt() );
	      pTrack->SetEta(pParticle->Eta() );
	      pTrack->SetPhi(pParticle->Phi() );
	      pTrack->SetForIntegratedFlow(kTRUE);
	      pTrack->SetForDifferentialFlow(kTRUE);
	    }
	    else if (anOption == 1) { //take the PID and kinematics from the MC
	      pTrack->SetPt(pMcParticle->Pt() );
	      pTrack->SetEta(pMcParticle->Eta() );
	      pTrack->SetPhi(pMcParticle->Phi() );
	      pTrack->SetForIntegratedFlow(kTRUE);
	      pTrack->SetForDifferentialFlow(kTRUE);
	    }
	    else { cout<<"Not a valid option"<<endl; }
	    if (pTrack->UseForIntegratedFlow())
	      { iSelParticlesInt++; }
	    if (pTrack->UseForDifferentialFlow())
	      { iSelParticlesDiff++; }
	    iGoodTracks++;
	    pEvent->TrackCollection()->Add(pTrack) ;  	     
	  }
      }
    itrkN++; 
  }
  
  pEvent-> SetEventNSelTracksIntFlow(iSelParticlesInt);  
  pEvent->SetNumberOfTracks(iGoodTracks);
  cout<<" iGoodTracks = "<<iGoodTracks<<endl;
  cout << "  iSelectedTracksInt = " << iSelParticlesInt << endl;  
  return pEvent;


}



/*
$Log$
*/ 

