
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

/* History of cvs commits:
 *
 * $Log$
 * Revision 1.4  2007/10/29 13:48:42  gustavo
 * Corrected coding violations
 *
 * Revision 1.2  2007/08/17 12:40:04  schutz
 * New analysis classes by Gustavo Conesa
 *
 * Revision 1.1.2.1  2007/07/26 10:32:09  schutz
 * new analysis classes in the the new analysis framework
 *
 *
 */

//_________________________________________________________________________
// Class for reading data (Kinematics and ESDs) in order to do prompt gamma correlations
//  Class created from old AliPHOSGammaJet 
//  (see AliRoot versions previous Release 4-09)
//
//*-- Author: Gustavo Conesa (LNF-INFN) 
//////////////////////////////////////////////////////////////////////////////


// --- ROOT system ---
#include <TFormula.h>
#include <TParticle.h>
 
//---- ANALYSIS system ----
#include "AliGammaMCDataReader.h" 
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliESDCaloCluster.h"
#include "AliStack.h"
#include "AliLog.h"

ClassImp(AliGammaMCDataReader)

//____________________________________________________________________________
  AliGammaMCDataReader::AliGammaMCDataReader() : 
    AliGammaReader()
{
  //Default Ctor
  
  //Initialize parameters
  fDataType=kMCData;
  
}

//____________________________________________________________________________
AliGammaMCDataReader::AliGammaMCDataReader(const AliGammaMCDataReader & g) :   
  AliGammaReader(g)
{
  // cpy ctor
}

//_________________________________________________________________________
AliGammaMCDataReader & AliGammaMCDataReader::operator = (const AliGammaMCDataReader & source)
{
  // assignment operator
  
  if(&source == this) return *this;
  

  return *this;
  
}

//____________________________________________________________________________
void AliGammaMCDataReader::CreateParticleList(TObject * data, TObject * kine,
					      TClonesArray * plCTS, 
					      TClonesArray * plEMCAL,  
					      TClonesArray * plPHOS,   
					      TClonesArray * plPrimCTS, 
					      TClonesArray * plPrimEMCAL, 
					      TClonesArray * plPrimPHOS){
  
  //Create a list of particles from the ESD. These particles have been measured 
  //by the Central Tracking system (TPC+ITS+...), PHOS and EMCAL 
  //Also create particle list with mothers.

  AliESDEvent* esd = (AliESDEvent*) data;
  AliStack* stack = (AliStack*) kine;
  
  Int_t npar  = 0 ;
  Double_t *pid = new Double_t[AliPID::kSPECIESN];  
  AliDebug(3,"Fill particle lists");
  
  //Get vertex for momentum calculation  
  Double_t v[3] ; //vertex ;
  esd->GetVertex()->GetXYZ(v) ; 
  
  //########### CALORIMETERS ##############  
  Int_t nCaloCluster = esd->GetNumberOfCaloClusters() ;  
  Int_t indexPH = plPHOS->GetEntries() ;
  Int_t indexEM = plEMCAL->GetEntries() ;
  
  for (npar =  0; npar <  nCaloCluster; npar++) {//////////////CaloCluster loop
    AliESDCaloCluster * clus = esd->GetCaloCluster(npar) ; // retrieve cluster from esd
    Int_t type = clus->GetClusterType();
    
    //########### PHOS ##############
    if(fSwitchOnPHOS && type ==  AliESDCaloCluster::kPHOSCluster){
      AliDebug(4,Form("PHOS clusters: E %f, match %d", clus->E(),clus->GetTrackMatched()));
      
      if(clus->GetTrackMatched()==-1){
	TLorentzVector momentum ;
	clus->GetMomentum(momentum, v);      
	Double_t phi = momentum.Phi();
	if(phi<0) phi+=TMath::TwoPi() ;
	if(momentum.Pt() > fNeutralPtCut &&  TMath::Abs(momentum.Eta()) < fPHOSEtaCut &&
	   phi > fPhiPHOSCut[0] && phi < fPhiPHOSCut[1] ) {
	  
	  pid=clus->GetPid();	
	  Int_t pdg = 22;
	  
	  if(IsPHOSPIDOn()){
	    AliDebug(5,Form("E %1.2f; PID: ph %0.2f, pi0 %0.2f, el %0.2f, conv el %0.2f,pi %0.2f, k %0.2f, p %0.2f, k0 %0.2f, n %0.2f, mu %0.2f ",
			    momentum.E(),pid[AliPID::kPhoton],pid[AliPID::kPi0],pid[AliPID::kElectron],pid[AliPID::kEleCon],pid[AliPID::kPion],
			    pid[AliPID::kKaon],pid[AliPID::kProton], pid[AliPID::kKaon0],pid[AliPID::kNeutron], pid[AliPID::kMuon]));
	    
	    Float_t wPhoton =  fPHOSPhotonWeight;
	    Float_t wPi0 =  fPHOSPi0Weight;
	    
	    if(fPHOSWeightFormula){
	      wPhoton = fPHOSPhotonWeightFormula->Eval(momentum.E()) ;
	      wPi0 =    fPHOSPi0WeightFormula->Eval(momentum.E());
	    }
	    
	    if(pid[AliPID::kPhoton] > wPhoton) 
	      pdg = kPhoton ;
	    else if(pid[AliPID::kPi0] > wPi0) 
	      pdg = kPi0 ; 
	    else if(pid[AliPID::kElectron] > fPHOSElectronWeight)  
	      pdg = kElectron ;
	    else if(pid[AliPID::kEleCon] > fPHOSElectronWeight) 
	      pdg = kEleCon ;
	    else if(pid[AliPID::kPion]+pid[AliPID::kKaon]+pid[AliPID::kProton] > fPHOSChargeWeight) 
	      pdg = kChargedHadron ;  
	    else if(pid[AliPID::kKaon0]+pid[AliPID::kNeutron] > fPHOSNeutralWeight) 
	      pdg = kNeutralHadron ; 
	    
	    else if(pid[AliPID::kElectron]+pid[AliPID::kEleCon]+pid[AliPID::kPion]+pid[AliPID::kKaon]+pid[AliPID::kProton]  >  
		    pid[AliPID::kPhoton] + pid[AliPID::kPi0]+pid[AliPID::kKaon0]+pid[AliPID::kNeutron]) 
	      pdg = kChargedUnknown  ; 
	    else 
	      pdg = kNeutralUnknown ; 
	    //neutral cluster, unidentifed.
	  }
	  
	  if(pdg != kElectron && pdg != kEleCon && pdg !=kChargedHadron && pdg !=kChargedUnknown ){//keep only neutral particles in the array

	    TParticle * particle = new TParticle(pdg, 1, -1, -1, -1, -1, momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E(), v[0], v[1], v[2], 0);	    
	    new((*plPHOS)[indexPH])   TParticle(*particle) ;
	    AliDebug(4,Form("PHOS added: pdg %d, pt %f, phi %f, eta %f", pdg, particle->Pt(),particle->Phi(),particle->Eta()));
 
	    //###############
	    //Check kinematics
	    //###############
	    TParticle * pmother = new TParticle();
	    Int_t label = clus->GetLabel();
	    if(label < stack->GetNprimary())
	      pmother = GetMotherParticle(label,stack, "PHOS",momentum);
	    else 
	      AliInfo(Form("PHOS: Bad label %d, too large, NPrimaries %d",label,stack->GetNprimary()));
	    new((*plPrimPHOS)[indexPH])   TParticle(*pmother) ;
	    
	    indexPH++;
	  }
	  else AliDebug(4,Form("PHOS charged cluster NOT added: pdg %d, pt %f, phi %f, eta %f\n", 
			       pdg, momentum.Pt(),momentum.Phi(),momentum.Eta()));	
	  
	}//pt, eta, phi cut
	else 	AliDebug(4,"Particle not added");
      }//track-match?
    }//PHOS cluster

    //################ EMCAL ##############
    else if(fSwitchOnEMCAL &&  type ==  AliESDCaloCluster::kEMCALClusterv1){
      AliDebug(4,Form("EMCAL clusters: E %f, match %d", clus->E(),clus->GetTrackMatched()));
      
      if(clus->GetTrackMatched()==-1 ){
	TLorentzVector momentum ;
	clus->GetMomentum(momentum, v); 
	Double_t phi = momentum.Phi();
	if(phi<0) phi+=TMath::TwoPi() ;
	if(momentum.Pt() > fNeutralPtCut &&  TMath::Abs(momentum.Eta()) < fEMCALEtaCut &&
	   phi > fPhiEMCALCut[0] && phi < fPhiEMCALCut[1] ) {
	  
	  pid=clus->GetPid();	
	  Int_t pdg = 22;
	  
	  if(IsEMCALPIDOn()){
	    AliDebug(5,Form("E %1.2f; PID: ph %0.2f, pi0 %0.2f, el %0.2f, conv el %0.2f,pi %0.2f, k %0.2f, p %0.2f, k0 %0.2f, n %0.2f, mu %0.2f ",
			    momentum.E(),pid[AliPID::kPhoton],pid[AliPID::kPi0],pid[AliPID::kElectron],pid[AliPID::kEleCon],pid[AliPID::kPion],
			    pid[AliPID::kKaon],pid[AliPID::kProton], pid[AliPID::kKaon0],pid[AliPID::kNeutron], pid[AliPID::kMuon]));
	    
	    if(pid[AliPID::kPhoton] > fEMCALPhotonWeight) 
	      pdg = kPhoton ;
	    else if(pid[AliPID::kPi0] > fEMCALPi0Weight) 
	      pdg = kPi0 ; 
	    else if(pid[AliPID::kElectron] > fEMCALElectronWeight)  
	      pdg = kElectron ;
	    else if(pid[AliPID::kEleCon] > fEMCALElectronWeight) 
	      pdg = kEleCon ;
	    else if(pid[AliPID::kPion]+pid[AliPID::kKaon]+pid[AliPID::kProton] > fEMCALChargeWeight) 
	      pdg = kChargedHadron ;  
	    else if(pid[AliPID::kKaon0]+pid[AliPID::kNeutron] > fEMCALNeutralWeight) 
	      pdg = kNeutralHadron ; 
	    else if(pid[AliPID::kElectron]+pid[AliPID::kEleCon]+pid[AliPID::kPion]+pid[AliPID::kKaon]+pid[AliPID::kProton]  >  
		    pid[AliPID::kPhoton] + pid[AliPID::kPi0]+pid[AliPID::kKaon0]+pid[AliPID::kNeutron]) 
	      pdg = kChargedUnknown ; 
	    else 
	      pdg = kNeutralUnknown ;
	  }
	  
	  if(pdg != kElectron && pdg != kEleCon && pdg !=kChargedHadron && pdg !=kChargedUnknown){//keep only neutral particles in the array

	    TParticle * particle = new TParticle(pdg, 1, -1, -1, -1, -1, momentum.Px(), momentum.Py(), momentum.Pz(), momentum.E(), v[0], v[1], v[2], 0);
	    new((*plEMCAL)[indexEM])   TParticle(*particle) ;
	    AliDebug(4,Form("EMCAL cluster added: pdg %f, pt %f, phi %f, eta %f", pdg, particle->Pt(),particle->Phi(),particle->Eta()));
	    
	    //###############
	    //Check kinematics
	    //###############
	    TParticle * pmother = new TParticle();
	    Int_t label = clus->GetLabel();
	    if(label < stack->GetNprimary())
	      pmother = GetMotherParticle(label,stack, "EMCAL",momentum);
	    else 
	      AliInfo(Form("EMCAL: Bad label %d, too large, NPrimaries %d",label,stack->GetNprimary()));	    
	    new((*plPrimEMCAL)[indexEM])   TParticle(*pmother) ;

	    indexEM++;
	  }
	  else AliDebug(4,Form("EMCAL charged cluster NOT added: pdg %d, pt %f, phi %f, eta %f", 
			       pdg, momentum.Pt(),momentum.Phi(),momentum.Eta()));
	  
	}//pt, phi, eta cut
	else 	AliDebug(4,"Particle not added");
      }//track-matched
    }//EMCAL cluster

  }//cluster loop

 

  //########### CTS (TPC+ITS) #####################
  Int_t nTracks   = esd->GetNumberOfTracks() ;
  Int_t indexCh  = plCTS->GetEntries() ;

  if(fSwitchOnCTS){
    AliDebug(3,Form("Number of tracks %d",nTracks));
  
    for (npar =  0; npar <  nTracks; npar++) {////////////// track loop
      AliESDtrack * track = esd->GetTrack(npar) ; // retrieve track from esd
      
      //We want tracks fitted in the detectors:
      ULong_t status=AliESDtrack::kTPCrefit;
      status|=AliESDtrack::kITSrefit;
    
      //We want tracks whose PID bit is set:
      //     ULong_t status =AliESDtrack::kITSpid;
      //     status|=AliESDtrack::kTPCpid;
      
      if ( (track->GetStatus() & status) == status) {//Check if the bits we want are set
	// Do something with the tracks which were successfully
	// re-fitted 
	Double_t en = 0; //track ->GetTPCsignal() ;
	Double_t mom[3];
	track->GetPxPyPz(mom) ;
	Double_t px = mom[0];
	Double_t py = mom[1];
	Double_t pz = mom[2]; //Check with TPC people if this is correct.
	Int_t pdg = 11; //Give any charged PDG code, in this case electron.
	//I just want to tag the particle as charged
	
	//Check kinematics
	Int_t label = TMath::Abs(track->GetLabel());
	TParticle * pmother = new TParticle();
	pmother = stack->Particle(label);
	
	TParticle * particle = new TParticle(pdg, 1, -1, -1, -1, -1, 
					     px, py, pz, en, v[0], v[1], v[2], 0);
	
	if(particle->Pt() > fChargedPtCut && TMath::Abs(particle->Eta())<fCTSEtaCut){
	  new((*plCTS)[indexCh])       TParticle(*particle) ;   
	  new((*plPrimCTS)[indexCh])       TParticle(*pmother) ;
	  indexCh++;    
	  
	}//kinematic selection
      }//select track from refit
    }//track loop    
  }//CTS

  AliDebug(3,Form("Particle lists filled, tracks  %d , clusters: EMCAL %d, PHOS %d", indexCh,indexEM,indexPH));

}

TParticle * AliGammaMCDataReader:: GetMotherParticle(Int_t label, AliStack *stack, TString calo,  TLorentzVector momentum)
{
  //Gets the primary particle and do some checks:
  //Check if primary is inside calorimeter and look the mother outsie
  //Check if mother is a decay photon, in which case check if decay was overlapped
  
  Float_t minangle = 0;
  Float_t ipdist = 0;
  TParticle * pmother = new TParticle();

  if(calo == "PHOS"){
    ipdist= fPHOSIPDistance;
    minangle = fPHOSMinAngle; 
  }
  else if (calo == "EMCAL"){
    ipdist = fEMCALIPDistance;
    minangle = fEMCALMinAngle;
  }


  if(label>=0){
    pmother = stack->Particle(label);
    Int_t mostatus = pmother->GetStatusCode();
    Int_t mopdg    = TMath::Abs(pmother->GetPdgCode());
    
    //Check if mother is a conversion inside the calorimeter
    //In such case, return the mother before the calorimeter.
    //First approximation.
    Float_t vy = TMath::Abs(pmother->Vy()) ;

    if( mostatus == 0 && vy >= ipdist){

      //cout<<"Calo: "<<calo<<" vy "<<vy<<" E clus "<<momentum.E()<<" Emother "<<pmother->Energy()<<" "
      //  <<pmother->GetName()<<endl;

      while( vy >= ipdist){//inside calorimeter
	AliDebug(4,Form("Conversion inside calorimeter, mother vertex %0.2f, IP distance %0.2f", vy, ipdist));
	pmother =  stack->Particle(pmother->GetMother(0));
	vy = TMath::Abs(pmother->Vy()) ;
	//cout<<" label "<<label<<" Mother: "<<pmother->GetName()<<" E "<<pmother->Energy()<<" Status "<<pmother->GetStatusCode()<<"  and vertex "<<vy<<endl;
	mostatus = pmother->GetStatusCode();
	mopdg    = TMath::Abs(pmother->GetPdgCode());
      }//while vertex is inside calorimeter
      //cout<<"Calo: "<<calo<<" final vy "<<vy<<" E clus "<<momentum.E()<<" Emother "<<pmother->Energy()<<" "
      //  <<pmother->GetName()<<endl;
    }//check status and vertex

    AliDebug(4,Form("%s, ESD E %2.2f, PID %d,  mother: E %2.2f, label %d, status %d,  vertex %3.2f, mother 2 %d, grandmother %d \n",
		    calo.Data(),momentum.E(),pmother->Energy(), label, pmother->GetPdgCode(),
		    pmother->GetStatusCode(), vy, pmother->GetMother(0), stack->GetPrimary(label)));
    
    //Check Decay photons
    if(mopdg == 22){
      
      //his mother was a pi0?
      TParticle * pmotherpi0 =  stack->Particle(pmother->GetMother(0));
      if( pmotherpi0->GetPdgCode() == 111){

	AliDebug(4,Form(" %s: E cluster %2.2f, E gamma %2.2f, Mother Pi0, E %0.2f, status %d, daughters %d\n",
			calo.Data(), momentum.E(),pmother->Energy(),pmotherpi0->Energy(), 
			pmotherpi0->GetStatusCode(), pmotherpi0->GetNDaughters()));
	
	if(pmotherpi0->GetNDaughters() == 1) mostatus = 2 ; //signal that this photon can only be decay, not overlapped.
	else if(pmotherpi0->GetNDaughters() == 2){
	  
	  TParticle * pd1 = stack->Particle(pmotherpi0->GetDaughter(0));
	  TParticle * pd2 = stack->Particle(pmotherpi0->GetDaughter(1));
	  //if(pmotherpi0->Energy()> 10 ){
// 	  cout<<"Two "<<calo<<" daugthers, pi0 label "<<pmother->GetMother(0)<<" E :"<<pmotherpi0->Energy()<<endl;
// 	  cout<<" 1) label "<<pmotherpi0->GetDaughter(0)<<" pdg "<<pd1->GetPdgCode()<<" E  "<<pd1->Energy()
// 	      <<" phi "<<pd1->Phi()*TMath::RadToDeg()<<" eta "<<pd1->Eta()
// 	      <<" mother label "<<pd1->GetMother(0)<<" n daug "<<pd1->GetNDaughters() <<endl;
// 	    cout<<" 2) label "<<pmotherpi0->GetDaughter(1)<<" pdg "<<pd2->GetPdgCode()<<" E  "<<pd2->Energy()
// 		<<" phi "<<pd2->Phi()*TMath::RadToDeg()<<" eta "<<pd2->Eta()<<" mother label "
// 		<<pd2->GetMother(0)<<" n daug "<<pd2->GetNDaughters() <<endl;
	    //}
	  if(pd1->GetPdgCode() == 22 && pd2->GetPdgCode() == 22){
	    TLorentzVector lv1 , lv2 ;
	    pd1->Momentum(lv1);
	    pd2->Momentum(lv2);
	    Double_t angle = lv1.Angle(lv2.Vect());
// 	    if(pmotherpi0->Energy()> 10 )
// 	      cout<<"angle*ipdist "<<angle*ipdist<<" mindist "<< minangle <<endl;
	    if (angle < minangle){
	      //There is overlapping, pass the pi0 as mother
	      
	      AliDebug(4,Form(">>>> %s cluster with E %2.2f is a overlapped pi0, E %2.2f, angle %2.4f < anglemin %2.4f\n",
			      calo.Data(), momentum.E(), pmotherpi0->Energy(), angle, minangle));	    
	     
	      pmother = pmotherpi0 ;
	      
	    }
	    else mostatus = 2 ; // gamma decay not overlapped
	  }// daughters are photons
	  else mostatus = 2; // daughters are e-gamma or e-e, no overlapped, or charged cluster
	}//2 daughters
	else AliDebug(4,Form("pi0 has %d daughters",pmotherpi0->GetNDaughters()));
      }//pi0 decay photon?
    }//photon 

    pmother->SetStatusCode(mostatus); // if status = 2, decay photon.

  }//label >=0
  else AliInfo(Form("Negative Kinematic label of PHOS cluster:  %d",label));

  return pmother ;

}
