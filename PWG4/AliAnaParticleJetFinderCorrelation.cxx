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
/* $Id: AliAnaParticleJetFinderCorrelation.cxx 22232 2007-11-17 16:39:49Z gustavo $ */

//_________________________________________________________________________
// Class for the analysis of particle (direct gamma) -jet (jet found with finder) correlations
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include "TH2F.h"
#include "Riostream.h"

//---- AliRoot system ----
#include "AliCaloTrackReader.h"
#include "AliAODJet.h"
#include "AliAnaParticleJetFinderCorrelation.h" 
#include "AliLog.h"
  
  ClassImp(AliAnaParticleJetFinderCorrelation)
  

//____________________________________________________________________________
  AliAnaParticleJetFinderCorrelation::AliAnaParticleJetFinderCorrelation() : 
	AliAnaPartCorrBaseClass(),  
    fDeltaPhiMaxCut(0.), fDeltaPhiMinCut(0.), fRatioMaxCut(0.),  fRatioMinCut(0.), 
	fConeSize(0), fPtThresholdInCone(0),fUseJetRefTracks(0), fMakeCorrelationInHistoMaker(0),
	fhDeltaEta(0), fhDeltaPhi(0), fhDeltaPt(0), fhPtRatio(0), fhPt(0),
	fhFFz(0),fhFFxi(0),fhFFpt(0),fhNTracksInCone(0)
{
	//Default Ctor

	//Initialize parameters
	InitParameters();
}

//____________________________________________________________________________
AliAnaParticleJetFinderCorrelation::AliAnaParticleJetFinderCorrelation(const AliAnaParticleJetFinderCorrelation & pjf) :   
	AliAnaPartCorrBaseClass(pjf),  
	fDeltaPhiMaxCut(pjf.fDeltaPhiMaxCut), fDeltaPhiMinCut(pjf.fDeltaPhiMinCut), 
	fRatioMaxCut(pjf.fRatioMaxCut), fRatioMinCut(pjf.fRatioMinCut), 
	fConeSize(pjf.fConeSize), fPtThresholdInCone(pjf.fPtThresholdInCone),   
	fUseJetRefTracks(pjf.fUseJetRefTracks), fMakeCorrelationInHistoMaker(pjf.fMakeCorrelationInHistoMaker),
	fhDeltaEta(pjf.fhDeltaEta), fhDeltaPhi(pjf.fhDeltaPhi), 
	fhDeltaPt(pjf.fhDeltaPt), fhPtRatio(pjf.fhPtRatio), fhPt(pjf.fhPt), 
	fhFFz(pjf.fhFFz),fhFFxi(pjf.fhFFxi),fhFFpt(pjf.fhFFpt),
	fhNTracksInCone(pjf.fhNTracksInCone)
{
	// cpy ctor

}

//_________________________________________________________________________
AliAnaParticleJetFinderCorrelation & AliAnaParticleJetFinderCorrelation::operator = (const AliAnaParticleJetFinderCorrelation & pjf)
{
	// assignment operator

	if(this == &pjf)return *this;
	((AliAnaPartCorrBaseClass *)this)->operator=(pjf);
  
	fDeltaPhiMaxCut    = pjf.fDeltaPhiMaxCut ; 
	fDeltaPhiMinCut    = pjf.fDeltaPhiMinCut ; 
	fRatioMaxCut       = pjf.fRatioMaxCut ;
	fRatioMinCut       = pjf.fRatioMinCut ; 
	fConeSize          = pjf.fConeSize ; 
	fPtThresholdInCone = pjf.fPtThresholdInCone ;
	fUseJetRefTracks   = pjf.fUseJetRefTracks ;
	fMakeCorrelationInHistoMaker = pjf.fMakeCorrelationInHistoMaker ;  
	  
	//Histograms
	fhDeltaEta = pjf.fhDeltaEta;
	fhDeltaPhi = pjf.fhDeltaPhi;
	fhDeltaPt  = pjf.fhDeltaPt;
	fhPtRatio  = pjf.fhPtRatio;
	fhPt       = pjf.fhPt;
  
	fhFFz   = pjf.fhFFz;
	fhFFxi  = pjf.fhFFxi;
	fhFFpt  = pjf.fhFFpt;
	fhNTracksInCone = pjf.fhNTracksInCone;  
  
  return *this;

}

//____________________________________________________________________________
//AliAnaParticleJetFinderCorrelation::~AliAnaParticleJetFinderCorrelation() 
//{
//   // Remove all pointers except analysis output pointers.
// 
//}


//________________________________________________________________________
TList *  AliAnaParticleJetFinderCorrelation::GetCreateOutputObjects()
{  
  // Create histograms to be saved in output file and 
  // store them in fOutputContainer

  if(GetDebug()>1) printf("Init histograms \n");

  TList * outputContainer = new TList() ; 
  outputContainer->SetName("ParticleJetFinderHistos") ; 

  fhDeltaPhi  = new TH2F("DeltaPhi","#phi_{jet} - #phi_{trigger} vs p_{T trigger}",120,0,120,200,-4,4); 
  fhDeltaPhi->SetYTitle("#Delta #phi");
  fhDeltaPhi->SetXTitle("p_{T trigger} (GeV/c)");
  outputContainer->Add(fhDeltaPhi);

  fhDeltaEta  = new TH2F("DeltaEta","#eta_{jet} - #eta_{trigger} vs p_{T trigger}",120,0,120,200,-5,5); 
  fhDeltaEta->SetYTitle("#Delta #eta");
  fhDeltaEta->SetXTitle("p_{T trigger} (GeV/c)");
  outputContainer->Add(fhDeltaEta);

  fhDeltaPt  = new TH2F("DeltaPt","#p_{T trigger} - #p_{T jet} vs p_{T trigger}",120,0,120,200,-100,100); 
  fhDeltaPt->SetYTitle("#Delta #p_{T}");
  fhDeltaPt->SetXTitle("p_{T trigger} (GeV/c)"); 
  outputContainer->Add(fhDeltaPt);

  fhPtRatio  = new TH2F("PtRatio","#p_{T jet} / #p_{T trigger} vs p_{T trigger}",120,0,120,200,0,2.); 
  fhPtRatio->SetYTitle("ratio");
  fhPtRatio->SetXTitle("p_{T trigger} (GeV/c)");
  outputContainer->Add(fhPtRatio);
  
  fhPt  = new TH2F("Pt","#p_{T jet} vs p_{T trigger}",120,0,120,120,0,120); 
  fhPt->SetYTitle("#p_{T jet}(GeV/c)");
  fhPt->SetXTitle("p_{T trigger} (GeV/c)");
  outputContainer->Add(fhPt);
  
  fhFFz  = new TH2F("FFz","z = p_{T i charged}/p_{T trigger} vs p_{T trigger}", 120,0.,120.,200,0.,2);  
  fhFFz->SetYTitle("z");
  fhFFz->SetXTitle("p_{T trigger}");
  outputContainer->Add(fhFFz) ;

  fhFFxi  = new TH2F("FFxi","#xi = ln(p_{T trigger}/p_{T i charged}) vs p_{T trigger}", 120,0.,120.,100,0.,10.); 
  fhFFxi->SetYTitle("#xi");
  fhFFxi->SetXTitle("p_{T trigger}");
  outputContainer->Add(fhFFxi) ;
  
  fhFFpt  = new TH2F("FFpt","#xi = p_{T i charged}) vs p_{T trigger}", 120,0.,120.,200,0.,50.); 
  fhFFpt->SetYTitle("p_{T charged hadron}");
  fhFFpt->SetXTitle("p_{T trigger}");
  outputContainer->Add(fhFFpt) ;

  fhNTracksInCone  = new TH2F("NTracksInCone","#xi = p_{T i charged}) vs p_{T trigger}", 120,0.,120.,200,0.,50.); 
  fhNTracksInCone->SetYTitle("p_{T charged hadron}");
  fhNTracksInCone->SetXTitle("p_{T trigger}");
  outputContainer->Add(fhNTracksInCone) ;
  	
  return outputContainer;
}

 //____________________________________________________________________________
void AliAnaParticleJetFinderCorrelation::InitParameters()
{
 
  //Initialize the parameters of the analysis.

  fDeltaPhiMinCut    = 1.5 ;
  fDeltaPhiMaxCut    = 4.5 ; 
  fRatioMaxCut       = 1.2 ; 
  fRatioMinCut       = 0.3 ; 
  fConeSize          = 0.3 ;
  fPtThresholdInCone = 0.5 ;
  fUseJetRefTracks   = kFALSE ;
  fMakeCorrelationInHistoMaker = kFALSE ;
  }

//__________________________________________________________________
Int_t  AliAnaParticleJetFinderCorrelation::SelectJet(AliAODParticleCorrelation * particle)
{
	//Returns the index of the jet that is opposite to the particle
	
	Int_t njets = (GetReader()->GetAOD())->GetNJets() ;	
	cout<<">> Loop on Jets"<<endl;
	AliAODJet * jet = new AliAODJet ;
	Int_t index = -1;
	for(Int_t ijet = 0; ijet < njets ; ijet++){
		jet = (GetReader()->GetAOD())->GetJet(ijet) ;	  
		Float_t dphi  = TMath::Abs(particle->Phi()-jet->Phi());
		Float_t ratio = jet->Pt()/particle->Pt();
		if(GetDebug() > 3)
			printf("Jet %d, Ratio pT %2.3f, Delta phi %2.3f\n",ijet,ratio,dphi);	  
		Float_t dphiprev= 10000;
		if((dphi > fDeltaPhiMinCut) && (dphi<fDeltaPhiMaxCut) &&
			(ratio > fRatioMinCut) && (ratio < fRatioMaxCut)  &&
			(TMath::Abs(dphi-3.14159) < TMath::Abs(dphiprev-3.14159))){//In case there is more than one jet select the most opposite.
			dphiprev = dphi;
			index = ijet ;	
		}//Selection cuts
	}//AOD Jet loop

	return index ;

}

//__________________________________________________________________
void  AliAnaParticleJetFinderCorrelation::MakeAnalysisFillAOD() 
{  
	//Particle-Jet Correlation Analysis, fill AODs
	
	Int_t ntrig =  GetAODBranch()->GetEntriesFast() ;  
    if(GetDebug() > -3){
    printf("Begin jet finder  correlation analysis, fill AODs \n");
    printf("In particle branch aod entries %d\n", ntrig);
    printf("In jet      branch aod entries %d\n", (GetReader()->GetAOD())->GetNJets());
	}
	
  	//Loop on stored AOD particles, trigger
	for(Int_t iaod = 0; iaod < ntrig ; iaod++){
		AliAODParticleCorrelation* particle =  dynamic_cast<AliAODParticleCorrelation*> (GetAODBranch()->At(iaod));
		//cout<<">> Test reference "<<particle->GetRefJet()<<endl;
			//Correlate with jets
		Int_t ijet = SelectJet(particle);
		if(ijet > -1){
			if(GetDebug() > 2) printf ("Jet with index %d selected \n",ijet);
			AliAODJet *jet = (GetReader()->GetAOD())->GetJet(ijet);
			particle->SetRefJet(jet);	
		}
	}		  
	
	if(GetDebug() >1)printf("End of jet leading cone analysis, fill AODs \n");
} 

//__________________________________________________________________
void  AliAnaParticleJetFinderCorrelation::MakeAnalysisFillHistograms() 
{
	//Particle-Jet Correlation Analysis, fill histograms
	Int_t ntrig   =  GetAODBranch()->GetEntriesFast() ;    
	if(GetDebug() > 1){
		printf("Begin jet finder  correlation analysis, fill histograms \n");
		printf("In particle branch aod entries %d\n", ntrig);
		printf("In jet      branch aod entries %d\n", (GetReader()->GetAOD())->GetNJets());
	}
	
	//Loop on stored AOD particles, trigger
	for(Int_t iaod = 0; iaod < ntrig ; iaod++){
		AliAODParticleCorrelation* particle =  dynamic_cast<AliAODParticleCorrelation*> (GetAODBranch()->At(iaod));
		//cout<<">> Test reference "<<particle->GetRefJet()<<endl;
		
		//Recover the jet correlated, found previously.
		AliAODJet	* jet = particle->GetJet();
		//If correlation not made before, do it now.
		if(fMakeCorrelationInHistoMaker){
			//Correlate with jets
			Int_t ijet = SelectJet(particle);
			if(ijet > -1){
				if(GetDebug() > 2) printf ("Jet with index %d selected \n",ijet);
				jet = (GetReader()->GetAOD())->GetJet(ijet);
				particle->SetRefJet(jet);	
			}
		}
		
		if (!jet) continue ;
	
		//Fill Histograms
	  
		Double_t ptTrig = particle->Pt();
		Double_t ptJet = jet->Pt();
		Double_t phiTrig = particle->Phi();
		Double_t phiJet = jet->Phi();
		Double_t etaTrig = particle->Eta();
		Double_t etaJet = jet->Eta();
		//printf("pT trigger %2.3f, pT jet %2.3f, Delta phi %2.3f, Delta eta %2.3f, Delta pT %2.3f, ratio %2.3f \n",
		//	ptTrig,ptJet, phiJet-phiTrig, etaJet-etaTrig, ptTrig-ptJet, ptJet/ptTrig);
		fhDeltaPt ->Fill(ptTrig, ptTrig-ptJet);
		fhDeltaPhi->Fill(ptTrig, phiJet-phiTrig);
		fhDeltaEta->Fill(ptTrig, etaJet-etaTrig);
		fhPtRatio ->Fill(ptTrig, ptJet/ptTrig);
		fhPt      ->Fill(ptTrig, ptJet);

		//Fragmentation function
		Float_t	 rad = 0, pt = 0, eta = 0, phi = 0;
		Int_t	 npartcone = 0;
		TVector3 p3;
		AliAODTrack* track = new AliAODTrack ;
	
		Int_t ntracks =  0;
		if(!fUseJetRefTracks)
			ntracks =GetAODCTS()->GetEntriesFast();
		else //If you want to use jet tracks from JETAN
			ntracks =  (jet->GetRefTracks())->GetEntriesFast();
	
		for(Int_t ipr = 0;ipr < ntracks ; ipr ++ ){
			if(!fUseJetRefTracks)
				track = dynamic_cast<AliAODTrack *>(GetAODCTS()->At(ipr)) ; 
			else //If you want to use jet tracks from JETAN
				track = dynamic_cast<AliAODTrack *>((jet->GetRefTracks())->At(ipr));
					
			p3.SetXYZ(track->Px(),track->Py(),track->Pz());
			pt    = p3.Pt();
			eta  = p3.Eta();
			phi  = p3.Phi() ;
			if(phi < 0) phi+=TMath::TwoPi();
		
			//Check if there is any particle inside cone with pt larger than  fPtThreshold
			rad = TMath::Sqrt((eta-etaJet)*(eta-etaJet)+ (phi-phiJet)*(phi-phiJet));
			if(rad < fConeSize  && pt > fPtThresholdInCone){	
				//printf("charged in jet cone pt %f, phi %f, eta %f, R %f \n",pt,phi,eta,rad);
				npartcone++;
				fhFFz ->Fill(ptTrig, pt/ptTrig);
				fhFFxi->Fill(ptTrig, TMath::Log(ptTrig/pt));
				fhFFpt->Fill(ptTrig, pt);
			}
		}//Tracks
		fhNTracksInCone->Fill(ptTrig, npartcone);
	}//AOD trigger particle loop
	if(GetDebug() >1) printf("End of jet leading cone analysis, fill histograms \n");
} 


//__________________________________________________________________
void AliAnaParticleJetFinderCorrelation::Print(const Option_t * opt) const
{

	//Print some relevant parameters set for the analysis
	if(! opt)
		return;
  
	printf("Phi trigger-jet        <     %3.2f\n", fDeltaPhiMaxCut) ; 
	printf("Phi trigger-jet        >     %3.2f\n", fDeltaPhiMinCut) ;
	printf("pT Ratio trigger/jet   <     %3.2f\n", fRatioMaxCut) ; 
	printf("pT Ratio trigger/jet   >     %3.2f\n", fRatioMinCut) ;
	printf("fConeSize              =     %3.2f\n", fConeSize) ; 
	printf("fPtThresholdInCone     =     %3.2f\n", fPtThresholdInCone) ;
	printf("fUseJetRefTracks	   =     %d\n",    fUseJetRefTracks) ;
	printf("fMakeCorrelationInHistoMaker	   =     %d\n",    fMakeCorrelationInHistoMaker) ;	
} 

