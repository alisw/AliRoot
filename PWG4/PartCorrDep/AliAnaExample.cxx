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

//_________________________________________________________________________
// Example class on how to read AODCaloClusters, ESDCaloCells and AODTracks and how 
// fill AODs with PWG4PartCorr analysis frame
// Select the type of detector information that you want to analyze, CTS (tracking), PHOS or EMCAL
// Select the PID custer type of the calorimeters
// Set min momentum of the cluster/tracks
// Fill few histograms
//
//-- Author: Gustavo Conesa (INFN-LNF)
//_________________________________________________________________________


// --- ROOT system ---
//#include "Riostream.h"
#include "TClonesArray.h"
#include "TParticle.h"

//---- AliRoot system ----
#include "AliAnaExample.h"
#include "AliCaloTrackReader.h"
#include "AliLog.h"
#include "AliAODPWG4Particle.h"
#include "AliESDCaloCells.h"
#include "AliStack.h"
#include "AliCaloPID.h"

ClassImp(AliAnaExample)
  
//____________________________________________________________________________
  AliAnaExample::AliAnaExample() : 
    AliAnaPartCorrBaseClass(),fPdg(0),  fDetector(""), fhPt(0),fhPhi(0),fhEta(0),  fh2Pt(0),fh2Phi(0),fh2Eta(0),
    fhNCells(0), fhAmplitude(0)
{
  //Default Ctor

  //Initialize parameters
  InitParameters();
}

//____________________________________________________________________________
AliAnaExample::AliAnaExample(const AliAnaExample & ex) :   
  AliAnaPartCorrBaseClass(ex), fPdg(ex.fPdg), fDetector(ex.fDetector), fhPt(ex.fhPt),  fhPhi(ex.fhPhi),fhEta(ex.fhEta), 
  fh2Pt(ex.fh2Pt),  fh2Phi(ex.fh2Phi),fh2Eta(ex.fh2Eta), fhNCells(ex.fhNCells), fhAmplitude(ex.fhAmplitude)
{
  // cpy ctor
  
}

//_________________________________________________________________________
AliAnaExample & AliAnaExample::operator = (const AliAnaExample & ex)
{
  // assignment operator

  if(this == &ex)return *this;
  ((AliAnaPartCorrBaseClass *)this)->operator=(ex);
 
  fPdg = ex.fPdg;
  fDetector = ex.fDetector;
  fhPt = ex.fhPt;
  fhPhi = ex.fhPhi;
  fhEta = ex.fhEta;
  fh2Pt = ex.fh2Pt;
  fh2Phi = ex.fh2Phi;
  fh2Eta = ex.fh2Eta;
  fhNCells = ex.fhNCells;
  fhAmplitude = ex.fhAmplitude;

  return *this;

}

// //____________________________________________________________________________
// AliAnaExample::~AliAnaExample() 
// {
//   // Remove all pointers except analysis output pointers.

//   ;
// }


//________________________________________________________________________
TList *  AliAnaExample::GetCreateOutputObjects()
{  
	// Create histograms to be saved in output file and 
	// store them in fOutputContainer
	
	AliDebug(1,"Init parton histograms");
	
	TList * outputContainer = new TList() ; 
	outputContainer->SetName("ExampleHistos") ; 
	
	Int_t nptbins  = GetHistoNPtBins();
	Int_t nphibins = GetHistoNPhiBins();
	Int_t netabins = GetHistoNEtaBins();
	Float_t ptmax  = GetHistoPtMax();
	Float_t phimax = GetHistoPhiMax();
	Float_t etamax = GetHistoEtaMax();
	Float_t ptmin  = GetHistoPtMin();
	Float_t phimin = GetHistoPhiMin();
	Float_t etamin = GetHistoEtaMin();	
	
	fhPt  = new TH1F ("hPt","p_T distribution", nptbins,ptmin,ptmax); 
	fhPt->SetXTitle("p_{T} (GeV/c)");
	outputContainer->Add(fhPt);
	
	fhPhi  = new TH1F ("hPhi","#phi distribution",nphibins,phimin,phimax); 
	fhPhi->SetXTitle("#phi (rad)");
	outputContainer->Add(fhPhi);
	
	fhEta  = new TH1F ("hEta","#eta distribution",netabins,etamin,etamax); 
	fhEta->SetXTitle("#eta ");
	outputContainer->Add(fhEta);
	
	if(GetReader()->GetDataType()!= AliCaloTrackReader::kMC) {
		
		//Calo cells
		fhNCells  = new TH1F ("hNCells","# cells per event", 100,0,1000); 
		fhNCells->SetXTitle("n cells");
		outputContainer->Add(fhNCells);
		
		fhAmplitude  = new TH1F ("hAmplitude","#eta distribution", 100,0,1000); 
		fhAmplitude->SetXTitle("Amplitude ");
		outputContainer->Add(fhAmplitude);
	} 
	
	if(IsDataMC()){
		fh2Pt  = new TH2F ("h2Pt","p_T distribution, reconstructed vs generated", nptbins,ptmin,ptmax,nptbins,ptmin,ptmax); 
		fh2Pt->SetXTitle("p_{T,rec} (GeV/c)");
		fh2Pt->SetYTitle("p_{T,gen} (GeV/c)");
		outputContainer->Add(fh2Pt);
		
		fh2Phi  = new TH2F ("h2Phi","#phi distribution, reconstructed vs generated", nphibins,phimin,phimax, nphibins,phimin,phimax); 
		fh2Phi->SetXTitle("#phi_{rec} (rad)");
		fh2Phi->SetYTitle("#phi_{gen} (rad)");
		outputContainer->Add(fh2Phi);
		
		fh2Eta  = new TH2F ("h2Eta","#eta distribution, reconstructed vs generated", netabins,etamin,etamax,netabins,etamin,etamax); 
		fh2Eta->SetXTitle("#eta_{rec} ");
		fh2Eta->SetYTitle("#eta_{gen} ");
		outputContainer->Add(fh2Eta);
		
	}
	return outputContainer;
}

 //__________________________________________________
void AliAnaExample::InitParameters()
{ 
  //Initialize the parameters of the analysis.
  SetOutputAODClassName("AliAODPWG4Particle");
  SetOutputAODName("example");
  fPdg = 22; //Keep photons
  fDetector = "PHOS";
  
}

//__________________________________________________________________
void AliAnaExample::Print(const Option_t * opt) const
{
  //Print some relevant parameters set for the analysis
  if(! opt)
    return;
  
  printf("Min Pt = %3.2f\n", GetMinPt());
  printf("Max Pt = %3.2f\n", GetMaxPt());
  printf("Select clusters with pdg %d \n",fPdg);
  printf("Select detector %s \n",fDetector.Data());
  
} 

//__________________________________________________________________
void  AliAnaExample::MakeAnalysisFillAOD() 
{
	//Do analysis and fill aods
	
	//Some prints
	if(GetDebug() > 0){
		if(fDetector == "EMCAL" && GetAODEMCAL())printf("Example : in EMCAL aod entries %d\n", GetAODEMCAL()->GetEntriesFast());
		if(fDetector == "CTS" && GetAODCTS())printf("Example : in CTS aod entries %d\n", GetAODCTS()->GetEntriesFast());
		if(fDetector == "PHOS" && GetAODPHOS())printf("Example : in PHOS aod entries %d\n", GetAODPHOS()->GetEntriesFast());
	}
	
	//Get List with tracks or clusters  
	TClonesArray * partList = new TClonesArray;
	if(fDetector == "CTS") partList = GetAODCTS();
	else if(fDetector == "EMCAL") partList = GetAODEMCAL();
	else if(fDetector == "PHOS") partList = GetAODPHOS();
	
	if(!partList || partList->GetEntriesFast() == 0) return ;
	
	//Fill AODCaloClusters and AODParticle with PHOS/EMCAL aods
	if(fDetector == "EMCAL" || fDetector == "PHOS"){
		
		//WORK WITH CALOCLUSTERS 
		if(GetReader()->GetDataType()!= AliCaloTrackReader::kMC){ 
			ConnectAODCaloClusters(); //Do Only when filling AODCaloClusters 
			if(GetDebug() > 0) printf("Example: in calo clusters aod entries %d\n", GetAODCaloClusters()->GetEntriesFast());
		}
		//Get vertex for photon momentum calculation
		Double_t v[3] ; //vertex ;
		GetReader()->GetVertex(v);
		
		TLorentzVector mom ;
		for(Int_t i = 0; i < partList->GetEntriesFast(); i++){
			
			AliAODCaloCluster * calo =  (AliAODCaloCluster*) (partList->At(i));
			
			//Fill AODCaloClusters  
			if(GetReader()->GetDataType()!= AliCaloTrackReader::kMC) 
				AddAODCaloCluster(AliAODCaloCluster(*(calo)));
			
			//Fill AODParticle after some selection
			calo->GetMomentum(mom,v);
			Int_t pdg = fPdg;
			
			if(IsCaloPIDOn()){
				Double_t pid[13];
				calo->GetPID(pid);
				pdg = GetCaloPID()->GetPdg(fDetector,pid,mom.E());
				//pdg = GetCaloPID()->GetPdg(fDetector,mom,
				//		  calo->GetM02(), calo->GetM02(),
				//		  calo->GetDispersion(), 0, 0); 
			}
			
			//Acceptance selection   
			Bool_t in = kTRUE;
			if(IsFidutialCutOn())
				in =  GetFidutialCut()->IsInFidutialCut(mom,fDetector) ;
			
			if(GetDebug() > 1) printf("cluster pt %2.2f, phi %2.2f, pdg %d, in fidutial cut %d \n",mom.Pt(), mom.Phi(), pdg, in);
			
			//Select cluster if momentum, pdg and acceptance are good
			if(mom.Pt() > GetMinPt() && pdg ==fPdg && in) {
				AliAODPWG4Particle ph = AliAODPWG4Particle(mom);
				//AddAODParticleCorrelation(AliAODPWG4ParticleCorrelation(mom));
				ph.SetLabel(calo->GetLabel(0));
				ph.SetPdg(pdg);
				ph.SetDetector(fDetector);
				AddAODParticle(ph);
			}//selection
		}//loop
		
		if(GetReader()->GetDataType()!= AliCaloTrackReader::kMC) {
			//WORK WITH ESDCALOCELLS
			//Don't connect in the same analysis PHOS and EMCAL cells.
			
			AliESDCaloCells * esdCell = new AliESDCaloCells ;
			if(fDetector == "PHOS") {
				ConnectAODPHOSCells(); //Do Only when filling AODCaloCells
				esdCell = (AliESDCaloCells *) GetPHOSCells();
			}
			else  {
				ConnectAODEMCALCells(); //Do Only when filling AODCaloCells
				esdCell = (AliESDCaloCells *) GetEMCALCells();
			}
			
			if(!esdCell) AliFatal("No CELLS available for analysis");
			
			//Some prints
			if(GetDebug() > 0 && esdCell )
				printf("Example : in ESD %s cell entries %d\n", fDetector.Data(), esdCell->GetNumberOfCells());    
			
			//Fill AODCells in file
			Int_t ncells = esdCell->GetNumberOfCells() ;
			GetAODCaloCells()->CreateContainer(ncells);
			
			GetAODCaloCells()->SetType((AliAODCaloCells::AODCells_t) esdCell->GetType());
			
			for (Int_t iCell = 0; iCell < ncells; iCell++) {      
				if(GetDebug() > 2)  printf("cell : amp %f, absId %d,  time %f\n", esdCell->GetAmplitude(iCell), esdCell->GetCellNumber(iCell), esdCell->GetTime(iCell));
				
				GetAODCaloCells()->SetCell(iCell,esdCell->GetCellNumber(iCell),esdCell->GetAmplitude(iCell));
			}
			GetAODCaloCells()->Sort();
		} 
	}//cluster-cell analysis
	else if(fDetector == "CTS"){ //Track analysis
		//Fill AODParticle with CTS aods
		TVector3 p3;
		for(Int_t i = 0; i < GetAODCTS()->GetEntriesFast(); i++){
			
			AliAODTrack * track =  (AliAODTrack*) (GetAODCTS()->At(i));
			
			//Fill AODParticle after some selection       
			Double_t mom[3] = {track->Px(),track->Py(),track->Pz()};
			p3.SetXYZ(mom[0],mom[1],mom[2]);
			
			//Acceptance selection
			Bool_t in =  GetFidutialCut()->IsInFidutialCut(mom,"CTS") ;
			if(GetDebug() > 1) printf("track pt %2.2f, phi %2.2f, in fidutial cut %d\n",p3.Pt(), p3.Phi(), in);
			if(p3.Pt() > GetMinPt() && in) {
				AliAODPWG4Particle tr = AliAODPWG4Particle(mom[0],mom[1],mom[2],0);
				tr.SetDetector("CTS");
				AddAODParticle(tr);
			}//selection
		}//loop
	}//CTS analysis
	
	if(GetDebug() > 0) {
		if(fDetector!="CTS" && GetReader()->GetDataType()!= AliCaloTrackReader::kMC) 
			printf("Example: final aod calocluster entries %d\n", GetAODCaloClusters()->GetEntriesFast());
		printf("Example: final aod branch entries %d\n", GetOutputAODBranch()->GetEntriesFast());  
		if(fDetector!="CTS" && GetReader()->GetDataType()!= AliCaloTrackReader::kMC) 
			printf("Example: final aod cell  entries %d\n", GetAODCaloCells()->GetNumberOfCells());
	}
} 

//__________________________________________________________________
void  AliAnaExample::MakeAnalysisFillHistograms() 
{
	//Do analysis and fill histograms
	
	//Loop on stored AODParticles
	Int_t naod = GetOutputAODBranch()->GetEntriesFast();
	if(GetDebug() > 0) printf("histo aod branch entries %d\n", naod);
	for(Int_t iaod = 0; iaod < naod ; iaod++){
		AliAODPWG4Particle* ph =  (AliAODPWG4Particle*) (GetOutputAODBranch()->At(iaod));
		
		fhPt->Fill(ph->Pt());
		fhPhi->Fill(ph->Phi());
		fhEta->Fill(ph->Eta());
		
		if(IsDataMC()){
			//Play with the MC stack if available
			AliStack * stack =  GetMCStack() ;
			
			if(ph->GetLabel() < 0 || !stack) {
				printf("*** bad label or no stack ***:  label %d \n", ph->GetLabel());
				continue;
			}
			
			if(ph->GetLabel() >=  stack->GetNtrack()) {
				printf("*** large label ***:  label %d, n tracks %d \n", ph->GetLabel(), stack->GetNtrack());
				continue ;
			}
			
			TParticle * mom = GetMCStack()->Particle(ph->GetLabel());
			
			fh2Pt->Fill(ph->Pt(), mom->Pt());
			fh2Phi->Fill(ph->Phi(), mom->Phi());
			fh2Eta->Fill(ph->Eta(), mom->Eta());
		}//Work with stack also
	}// aod branch loop
	
	// CaloCells histograms
	if(GetReader()->GetDataType()!= AliCaloTrackReader::kMC) {
		if(GetAODCaloCells()){
			
			Int_t ncells = GetAODCaloCells()->GetNumberOfCells();
			fhNCells->Fill(ncells) ;
			
			for (Int_t iCell = 0; iCell < ncells; iCell++) {      
				if(GetDebug() > 2)  printf("cell : amp %f, absId %d \n", GetAODCaloCells()->GetAmplitude(iCell), GetAODCaloCells()->GetCellNumber(iCell));
				fhAmplitude->Fill(GetAODCaloCells()->GetAmplitude(iCell));
			}
		}//calo cells container exist
	}
}
