/**************************************************************************
 * Author: Paraskevi Ganoti, University of Athens (pganoti@phys.uoa.gr)   *
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
 
//----------------------------------------------------------------------------------------------------------------
//                        class AliResonanceKink
//        Example of an analysis task for reconstructing resonances having at least one kaon-kink in their decay 
//        products. It provides basic plots as well as plots helping to calculate the corrections.
//        Usage: To analyse a resonance having a kaon in its decay products, one has to modify the integer 
//        variables resonancePDG, fdaughter1pdg and fdaughter2pdg accordingly as well as daughter1pdgMass  and daughter2pdgMass.
//        Also, depending on the analysis mode (ESD or MC), fAnalysisType in the constructor must also be changed 
//-----------------------------------------------------------------------------------------------------------------

#include "TChain.h"
#include "TTree.h"
#include "TH2D.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TParticlePDG.h"
#include "TF1.h"
#include "TList.h"
#include "TString.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliResonanceKink.h"
#include "AliESDkink.h"
#include "AliStack.h"
#include "AliESDtrack.h"
#include "AliESDEvent.h"
#include "AliExternalTrackParam.h"

ClassImp(AliResonanceKink)

//________________________________________________________________________
AliResonanceKink::AliResonanceKink() 
  : TObject(), fDebug(0), fListOfHistos(0), fOpeningAngle(0), fInvariantMass(0), fInvMassTrue(0), fPhiBothKinks(0), fRecPt(0), fRecEta(0), fRecEtaPt(0), fSimPt(0), fSimEta(0), fSimEtaPt(0), fSimPtKink(0), fSimEtaKink(0),  fSimEtaPtKink(0), 
  fhdr(0), fhdz(0), f1(0), f2(0), fAnalysisType(), fvtxz(0), fNbins(0), fLowX(0), fHighX(0), fdaughter1pdg(0), fdaughter2pdg(0), fresonancePDGcode(0), fMaxNSigmaToVertex(0), fMinPtTrackCut(0), fMaxDCAxy(0), fMaxDCAzaxis(0), 
fMinTPCclusters(0),fMaxChi2PerTPCcluster(0), fMaxCov0(0), fMaxCov2(0), fMaxCov5(0) , fMaxCov9(0), fMaxCov14(0) //, fTPCrefitFlag(kFALSE)

{
  // Constructor
}

//________________________________________________________________________
AliResonanceKink::AliResonanceKink(Int_t nbins, Float_t nlowx, Float_t nhighx, Int_t daughter1, Int_t daughter2, Int_t resonancePDG) 
  : TObject(), fDebug(0), fListOfHistos(0), fOpeningAngle(0), fInvariantMass(0), fInvMassTrue(0), fPhiBothKinks(0), fRecPt(0), fRecEta(0), fRecEtaPt(0), fSimPt(0), fSimEta(0), fSimEtaPt(0), fSimPtKink(0), fSimEtaKink(0),  fSimEtaPtKink(0), 
  fhdr(0), fhdz(0), f1(0), f2(0), fAnalysisType(), fvtxz(0), fNbins(nbins), fLowX(nlowx), fHighX(nhighx), fdaughter1pdg(daughter1), fdaughter2pdg(daughter2), fresonancePDGcode(resonancePDG), fMaxNSigmaToVertex(0), fMinPtTrackCut(0), 
fMaxDCAxy(0), fMaxDCAzaxis(0), fMinTPCclusters(0), fMaxChi2PerTPCcluster(0), fMaxCov0(0), fMaxCov2(0), fMaxCov5(0), fMaxCov9(0), fMaxCov14(0) //, fTPCrefitFlag(kFALSE)

{
   // Constructor
  
   fOpeningAngle=new TH1D("fOpeningAngle"," ", 100,-1.0,1.0);

   fInvariantMass=new TH1D("fInvariantMass"," ",fNbins,fLowX,fHighX);
   fInvMassTrue=new TH1D("fInvMassTrue"," ",fNbins,fLowX,fHighX);
   fPhiBothKinks=new TH1D("fPhiBothKinks"," ",fNbins,fLowX,fHighX);  // Applicable for phi(1020)

   fRecPt=new TH1D("fRecPt"," ", 50,0.0,5.0);
   fRecEta=new TH1D("fRecEta"," ", 44,-1.1,1.1);
   fRecEtaPt=new TH2D("fRecEtaPt"," ", 50,0.0,5.0, 44,-1.1,1.1); 
   fSimPt=new TH1D("fSimPt"," ", 50,0.0,5.0);
   fSimEta=new TH1D("fSimEta"," ", 44,-1.1,1.1); 
   fSimEtaPt=new TH2D("fSimEtaPt"," ", 50,0.0,5.0, 44,-1.1,1.1);
   fSimPtKink=new TH1D("fSimPtKink"," ", 50,0.0,5.0);
   fSimEtaKink=new TH1D("fSimEtaKink"," ", 44,-1.1,1.1);
   fSimEtaPtKink=new TH2D("fSimEtaPtKink"," ", 50,0.0,5.0, 44,-1.1,1.1);                
   fhdr=new TH1D("fhdr"," ", 100,0.0,5.0);  
   fhdz=new TH1D("fhdz"," ", 100,0.0,5.0);
   
   f1=new TF1("f1","((atan([0]*[1]*(1.0/(sqrt((x^2)*(1.0-([1]^2))-([0]^2)*([1]^2))))))*180.)/[2]",1.1,10.0);
   f1->SetParameter(0,0.493677);
   f1->SetParameter(1,0.9127037);
   f1->SetParameter(2,TMath::Pi());

   f2=new TF1("f2","((atan([0]*[1]*(1.0/(sqrt((x^2)*(1.0-([1]^2))-([0]^2)*([1]^2))))))*180.)/[2]",0.1,10.0);
   f2->SetParameter(0,0.13957018);
   f2->SetParameter(1,0.2731374);
   f2->SetParameter(2,TMath::Pi());
   
   fvtxz=new TH1D("fvtxz"," ", 100,-20.0,20.0);
   
}

//________________________________________________________________________
AliResonanceKink:: ~AliResonanceKink()
{
 //  Destructor
 if(fOpeningAngle) delete fOpeningAngle;
 if(fInvariantMass) delete fInvariantMass;
 if(fInvMassTrue) delete fInvMassTrue;
 if(fPhiBothKinks) delete fPhiBothKinks;
 if(fRecPt) delete fRecPt;
 if(fRecEta) delete fRecEta;
 if(fRecEtaPt) delete fRecEtaPt;
 if(fSimPt) delete fSimPt;
 if(fSimEta) delete fSimEta;
 if(fSimEtaPt) delete fSimEtaPt;
 if(fSimPtKink) delete fSimPtKink;
 if(fSimEtaKink) delete fSimEtaKink;
 if(fSimEtaPtKink) delete fSimEtaPtKink;
 if(fhdr) delete fhdr;
 if(fhdz) delete fhdz;   
 if(fvtxz) delete fvtxz;         
}
//________________________________________________________________________
TList* AliResonanceKink::GetHistogramList()
{
  // Adding histograms to the list
  fListOfHistos=new TList();
 
  fListOfHistos->Add(fOpeningAngle);
  fListOfHistos->Add(fInvariantMass);
  fListOfHistos->Add(fInvMassTrue);
  fListOfHistos->Add(fPhiBothKinks);
  fListOfHistos->Add(fRecPt);    
  fListOfHistos->Add(fRecEta);   
  fListOfHistos->Add(fRecEtaPt);    
  fListOfHistos->Add(fSimPt);    
  fListOfHistos->Add(fSimEta);   
  fListOfHistos->Add(fSimEtaPt);     
  fListOfHistos->Add(fSimPtKink);    
  fListOfHistos->Add(fSimEtaKink);   
  fListOfHistos->Add(fSimEtaPtKink);                                                           
  fListOfHistos->Add(fhdr);
  fListOfHistos->Add(fhdz);
  fListOfHistos->Add(fvtxz);
   
  return fListOfHistos;
}

//________________________________________________________________________
void AliResonanceKink::InitOutputHistograms(Int_t nbins, Float_t nlowx, Float_t nhighx)
{
  //  Initialisation of the output histograms
  fNbins=nbins; 
  fLowX=nlowx; 
  fHighX=nhighx;
    
  fOpeningAngle=new TH1D("fOpeningAngle"," ", 100,-1.0,1.0);

  fInvariantMass=new TH1D("fInvariantMass"," ",fNbins,fLowX,fHighX);
  fInvMassTrue=new TH1D("fInvMassTrue"," ",fNbins,fLowX,fHighX);
  fPhiBothKinks=new TH1D("fPhiBothKinks"," ",fNbins,fLowX,fHighX);  // Applicable for phi(1020)

  fRecPt=new TH1D("fRecPt"," ", 50,0.0,5.0);
  fRecEta=new TH1D("fRecEta"," ", 44,-1.1,1.1);
  fRecEtaPt=new TH2D("fRecEtaPt"," ", 50,0.0,5.0, 44,-1.1,1.1); 
  fSimPt=new TH1D("fSimPt"," ", 50,0.0,5.0);
  fSimEta=new TH1D("fSimEta"," ", 44,-1.1,1.1); 
  fSimEtaPt=new TH2D("fSimEtaPt"," ", 50,0.0,5.0, 44,-1.1,1.1);
  fSimPtKink=new TH1D("fSimPtKink"," ", 50,0.0,5.0);
  fSimEtaKink=new TH1D("fSimEtaKink"," ", 44,-1.1,1.1);
  fSimEtaPtKink=new TH2D("fSimEtaPtKink"," ", 50,0.0,5.0, 44,-1.1,1.1);                
  fhdr=new TH1D("fhdr"," ", 100,0.0,5.0);  
  fhdz=new TH1D("fhdz"," ", 100,0.0,5.0);
   
  f1=new TF1("f1","((atan([0]*[1]*(1.0/(sqrt((x^2)*(1.0-([1]^2))-([0]^2)*([1]^2))))))*180.)/[2]",1.1,10.0);
  f1->SetParameter(0,0.493677);
  f1->SetParameter(1,0.9127037);
  f1->SetParameter(2,TMath::Pi());

  f2=new TF1("f2","((atan([0]*[1]*(1.0/(sqrt((x^2)*(1.0-([1]^2))-([0]^2)*([1]^2))))))*180.)/[2]",0.1,10.0);
  f2->SetParameter(0,0.13957018);
  f2->SetParameter(1,0.2731374);
  f2->SetParameter(2,TMath::Pi());
   
  fvtxz=new TH1D("fvtxz"," ", 100,-20.0,20.0);
}
  
//________________________________________________________________________
void AliResonanceKink::Analyse(AliESDEvent* esd, AliMCEvent* mcEvent) 
{
  // Main loop
  // Called for each event
  Int_t resonancePDGcode, antiresonancePDGcode;
  
  if (fdaughter1pdg==kdaughterKaon)  {
    resonancePDGcode=fresonancePDGcode;
    antiresonancePDGcode=-fresonancePDGcode;
  }
  if (fdaughter1pdg!=kdaughterKaon)  {
    resonancePDGcode=-fresonancePDGcode;
    antiresonancePDGcode=fresonancePDGcode;
  }  
  if (fdaughter1pdg==fdaughter2pdg)  {
    resonancePDGcode=fresonancePDGcode;
    antiresonancePDGcode=fresonancePDGcode;
  }  

   Double_t daughter1pdgMass=TDatabasePDG::Instance()->GetParticle(fdaughter1pdg)->Mass();
   Double_t daughter2pdgMass=TDatabasePDG::Instance()->GetParticle(fdaughter2pdg)->Mass();
   
  if (!esd) {
    Printf("ERROR: fESD not available");
    return;
  }  

    if (!mcEvent) {
    Printf("ERROR: mcEvent not available");
    return;
  }  

  AliStack* stack=mcEvent->Stack();

  if(fAnalysisType == "MC") {
  for (Int_t iMc = 0; iMc < stack->GetNprimary(); ++iMc)
  {
    TParticle* particle = stack->Particle(iMc);

    if (!particle)
    {
      if (fDebug > 0) Printf("UNEXPECTED: particle with label %d not found in stack (mc loop)", iMc);
      continue;
    }

     if(TMath::Abs(particle->GetPdgCode())==fresonancePDGcode) {
       Int_t firstD=particle->GetFirstDaughter();
       Int_t lastD=particle->GetLastDaughter();
       TParticle *daughterParticle1=stack->Particle(firstD);
       TParticle *daughterParticle2=stack->Particle(lastD);
       
       TParticle* kaonFirstDaughter;
       Int_t mcProcessKaonFirstDaughter = -999;
       
       for(Int_t ia=0; ia<daughterParticle1->GetNDaughters(); ia++){
        if ((daughterParticle1->GetFirstDaughter()+ia)!=-1) {
	  kaonFirstDaughter=stack->Particle(daughterParticle1->GetFirstDaughter()+ia);
          mcProcessKaonFirstDaughter=kaonFirstDaughter->GetUniqueID();
        }
       }       
 
       if((daughterParticle1->Pt()>0.25)&&(daughterParticle2->Pt()>0.25)&&(TMath::Abs(daughterParticle1->Eta())<1.1)&&            (TMath::Abs(daughterParticle2->Eta())<1.1)&&(TMath::Abs(particle->Eta())<1.1)) {
         fSimEta->Fill(particle->Eta());
	 fSimPt->Fill(particle->Pt());
	 fSimEtaPt->Fill(particle->Pt(), particle->Eta());
	 if(mcProcessKaonFirstDaughter==4) {
	   fSimPtKink->Fill(particle->Pt());
	   fSimEtaKink->Fill(particle->Eta());
	   fSimEtaPtKink->Fill(particle->Pt(), particle->Eta());
	 }
       }
     }
  } 
  
  } // end fAnalysisType==MC
  else 
  
  if(fAnalysisType == "ESD") {
  const AliESDVertex* vertex = GetEventVertex(esd);
  if(!vertex) return;
  Double_t vtx[3];
  vertex->GetXYZ(vtx);
  fvtxz->Fill(vtx[2]);
  Double_t ptrackpos[3], ptrackneg[3];
  
  TLorentzVector p4pos, anp4pos;
  TLorentzVector p4neg, anp4neg;
  TLorentzVector p4comb, anp4comb;
  
  for (Int_t iTracks = 0; iTracks < esd->GetNumberOfTracks(); iTracks++) {
    AliESDtrack* trackpos = esd->GetTrack(iTracks);
    if (!trackpos) {
      if (fDebug > 0) Printf("ERROR: Could not receive track %d", iTracks);
      continue;
    }
    if (trackpos->GetSign() < 0) continue;
    
    AliExternalTrackParam *tpcTrackpos = (AliExternalTrackParam *)trackpos->GetTPCInnerParam();
    if(!tpcTrackpos) continue;
    ptrackpos[0]=tpcTrackpos->Px();
    ptrackpos[1]=tpcTrackpos->Py();   
    ptrackpos[2]=tpcTrackpos->Pz();  
    
    Bool_t firstLevelAcceptPosTrack=IsAcceptedForKink(esd, vertex, trackpos);
    if(firstLevelAcceptPosTrack==kFALSE) continue;
    
    TVector3 posTrackMom(ptrackpos[0],ptrackpos[1],ptrackpos[2]);
  	
    TParticle * partpos = stack->Particle(TMath::Abs(trackpos->GetLabel()));
    if (!partpos) continue;
    Int_t pdgpos = partpos->GetPdgCode();
    Int_t mumlabelpos=partpos->GetFirstMother();
    mumlabelpos = TMath::Abs(mumlabelpos);
    TParticle * mumpos=stack->Particle(mumlabelpos);
    if (!mumpos) continue;
    Int_t mumpdgpos = mumpos->GetPdgCode();
    
    Int_t indexKinkPos=trackpos->GetKinkIndex(0);
    Bool_t posKaonKinkFlag=0;
    if(indexKinkPos<0) posKaonKinkFlag=IsKink(esd, indexKinkPos, posTrackMom);
    
    if(posKaonKinkFlag==1) anp4pos.SetVectM(posTrackMom,daughter1pdgMass);
    
    if(indexKinkPos==0) {

    Bool_t secondLevelAcceptPosTrack=IsAcceptedForTrack(esd, vertex, trackpos);
    if(secondLevelAcceptPosTrack==kFALSE) continue;

      p4pos.SetVectM(posTrackMom, daughter2pdgMass);
    
    }
	
      for (Int_t j=0; j<esd->GetNumberOfTracks(); j++) {
        if(iTracks==j) continue;
        AliESDtrack* trackneg=esd->GetTrack(j);
        if (trackneg->GetSign() > 0) continue;
	
        AliExternalTrackParam *tpcTrackneg = (AliExternalTrackParam *)trackneg->GetTPCInnerParam();
        if(!tpcTrackneg) continue;
        ptrackneg[0]=tpcTrackneg->Px();
        ptrackneg[1]=tpcTrackneg->Py();   
        ptrackneg[2]=tpcTrackneg->Pz();  
    
        Bool_t firstLevelAcceptNegTrack=IsAcceptedForKink(esd, vertex, trackneg);
        if(firstLevelAcceptNegTrack==kFALSE) continue;	

        TVector3 negTrackMom(ptrackneg[0],ptrackneg[1],ptrackneg[2]);
	
        TParticle * partneg = stack->Particle(TMath::Abs(trackneg->GetLabel()));
        if (!partneg) continue;
        Int_t pdgneg = partneg->GetPdgCode();
	Int_t mumlabelneg=partneg->GetFirstMother();
        mumlabelneg = TMath::Abs(mumlabelneg);
        TParticle * mumneg=stack->Particle(mumlabelneg);
        if (!mumneg) continue;
        Int_t mumpdgneg = mumneg->GetPdgCode();
	
	Int_t indexKinkNeg=trackneg->GetKinkIndex(0);
	Bool_t negKaonKinkFlag=0;
	if(indexKinkNeg<0) negKaonKinkFlag=IsKink(esd, indexKinkNeg, negTrackMom);
	
	if(negKaonKinkFlag==1) p4neg.SetVectM(negTrackMom,daughter1pdgMass);
	
	if(indexKinkNeg==0)  {
 
	   Bool_t secondLevelAcceptNegTrack=IsAcceptedForTrack(esd, vertex, trackneg);
           if(secondLevelAcceptNegTrack==kFALSE) continue;  
	  
	  anp4neg.SetVectM(negTrackMom, daughter2pdgMass);
	
        }
	
	Double_t openingAngle=(ptrackpos[0]*ptrackneg[0]+ptrackpos[1]*ptrackneg[1]+ptrackpos[2]*ptrackneg[2])/(posTrackMom.Mag()*negTrackMom.Mag());

	if((posKaonKinkFlag==1)&&(negKaonKinkFlag==1)) {
	 p4comb=anp4pos;
	 p4comb+=p4neg;
	 if(openingAngle>0.6) fPhiBothKinks->Fill(p4comb.M());
	}
		
	if(negKaonKinkFlag==1) {
	  p4comb=p4pos;
          p4comb+=p4neg;
	  fInvariantMass->Fill(p4comb.M());
	  if ((mumpdgpos==(antiresonancePDGcode))&&(mumpdgneg==(antiresonancePDGcode))&&(mumlabelpos==mumlabelneg)
          &&(pdgpos==fdaughter2pdg)&&(pdgneg==(-fdaughter1pdg))&&(TMath::Abs(trackpos->GetLabel())>=0)&&(TMath::Abs(trackneg->GetLabel())>=0)&&(mumlabelpos>=0)&&(mumlabelneg>=0)) {
            fOpeningAngle->Fill(openingAngle);
            fInvMassTrue->Fill(p4comb.M());
	    if((TMath::Abs(p4pos.Vect().Eta())<1.1)&&(TMath::Abs(p4neg.Vect().Eta())<1.1)&&(p4comb.Vect().Eta()<1.1)) {
	      fRecPt->Fill(p4comb.Vect().Pt());
	      fRecEta->Fill(p4comb.Vect().Eta());
	      fRecEtaPt->Fill(p4comb.Vect().Perp(),p4comb.Vect().Eta());

	    }

           }
	  
	}
	
	if(posKaonKinkFlag==1) {
          anp4comb=anp4pos;
          anp4comb+=anp4neg;  
	  fInvariantMass->Fill(anp4comb.M());
	  if ((mumpdgpos==resonancePDGcode)&&(mumpdgneg==resonancePDGcode)&&(mumlabelpos==mumlabelneg)
          &&(pdgpos==fdaughter1pdg)&&(pdgneg==(-fdaughter2pdg))&&(TMath::Abs(trackpos->GetLabel())>=0)&&(TMath::Abs(trackneg->GetLabel())>=0)&&(mumlabelpos>=0)  &&(mumlabelneg>=0)) {
            fOpeningAngle->Fill(openingAngle);
            fInvMassTrue->Fill(p4comb.M());
            if((TMath::Abs(anp4neg.Vect().Eta())<1.1)&&(TMath::Abs(anp4pos.Vect().Eta())<1.1)&&(anp4comb.Vect().Eta()<1.1)) {	
	     fRecPt->Fill(anp4comb.Vect().Pt());
	     fRecEta->Fill(anp4comb.Vect().Eta());
	     fRecEtaPt->Fill(anp4comb.Vect().Pt(), anp4comb.Vect().Eta());
	   }

         }

	}
	 
      } //inner track loop 

  } //outer track loop 
  
  } // end fAnalysisType == ESD
  
}      

//____________________________________________________________________//
Float_t AliResonanceKink::GetSigmaToVertex(AliESDtrack* esdTrack) const {
  // Calculates the number of sigma to the vertex.
  
  Float_t b[2];
  Float_t bRes[2];
  Float_t bCov[3];

    esdTrack->GetImpactParametersTPC(b,bCov);
  
  if (bCov[0]<=0 || bCov[2]<=0) {
    bCov[0]=0; bCov[2]=0;
  }
  
  bRes[0] = TMath::Sqrt(bCov[0]);
  bRes[1] = TMath::Sqrt(bCov[2]);
  
  if (bRes[0] == 0 || bRes[1] ==0) return -1;
  
  Float_t d = TMath::Sqrt(TMath::Power(b[0]/bRes[0],2) + TMath::Power(b[1]/bRes[1],2));
  
  if (TMath::Exp(-d * d / 2) < 1e-10) return 1000;
  
  d = TMath::ErfInverse(1 - TMath::Exp(-d * d / 2)) * TMath::Sqrt(2);
  
  return d;
}

//________________________________________________________________________
const AliESDVertex* AliResonanceKink::GetEventVertex(const AliESDEvent* esd) const
{
  // Get the vertex 
  
  const AliESDVertex* vertex = esd->GetPrimaryVertex();

  if((vertex->GetStatus()==kTRUE)&&(vertex->GetNContributors()>2)) return vertex;
  else
  { 
     vertex = esd->GetPrimaryVertexSPD();
     if((vertex->GetStatus()==kTRUE)&&(vertex->GetNContributors()>2)) return vertex;
     else
     return 0;
  }
}

//________________________________________________________________________

 Bool_t AliResonanceKink::IsAcceptedForKink(AliESDEvent *localesd,
            const AliESDVertex *localvertex, AliESDtrack* localtrack) {
   // Apply the selections for each kink

  Double_t gPt = 0.0, gPx = 0.0, gPy = 0.0, gPz = 0.0;
  Double_t dca[2] = {0.0,0.0}, cov[3] = {0.0,0.0,0.0};  //The impact parameters and their covariance.
  Double_t dca3D = 0.0;
  
  AliExternalTrackParam *tpcTrack = (AliExternalTrackParam *)localtrack->GetTPCInnerParam();
  if(!tpcTrack) {
    gPt = 0.0; gPx = 0.0; gPy = 0.0; gPz = 0.0;
    dca[0] = -100.; dca[1] = -100.; dca3D = -100.;
    cov[0] = -100.; cov[1] = -100.; cov[2] = -100.;
  }
  else {
    gPt = tpcTrack->Pt();
    gPx = tpcTrack->Px();
    gPy = tpcTrack->Py();
    gPz = tpcTrack->Pz();
    tpcTrack->PropagateToDCA(localvertex,
    	       localesd->GetMagneticField(),100.,dca,cov);
  }
  
  if(GetSigmaToVertex(localtrack) > fMaxNSigmaToVertex) {
      if (fDebug > 1) Printf("IsAcceptedKink: Track rejected because it has a %lf sigmas to vertex TPC (max. requested: %lf)",   GetSigmaToVertex(localtrack),fMaxNSigmaToVertex);
      return kFALSE;
  }
  
  if(TMath::Abs(dca[0]) > fMaxDCAxy) {
      if (fDebug > 1) Printf("IsAcceptedKink: Track rejected because it has a value of dca(xy) (TPC) of %lf (max. requested: %lf)", TMath::Abs(dca[0]), fMaxDCAxy);
      return kFALSE;
  }
    
  if(TMath::Abs(dca[1]) > fMaxDCAzaxis) {
      if (fDebug > 1) Printf("IsAcceptedKink: Track rejected because it has a value of dca(z) of %lf (max. requested: %lf)", TMath::Abs(dca[1]), fMaxDCAzaxis);
      return kFALSE;
  }
  
  if(gPt < fMinPtTrackCut) {
      if (fDebug > 1) Printf("IsAcceptedKink: Track rejected because it has a min value of pt of %lf (min. requested: %lf)", gPt, fMinPtTrackCut);
      return kFALSE;
  } 
  
  return kTRUE;
}

//________________________________________________________________________
Bool_t AliResonanceKink::IsAcceptedForTrack(AliESDEvent *localesd,                                                                                                                                          const AliESDVertex *localvertex, AliESDtrack *localtrack) {
   // Apply the selections for each track

  Double_t gPt = 0.0, gPx = 0.0, gPy = 0.0, gPz = 0.0;
  Double_t dca[2] = {0.0,0.0}, cov[3] = {0.0,0.0,0.0};  //The impact parameters and their covariance.
  Double_t dca3D = 0.0;
  
  AliExternalTrackParam *tpcTrack = (AliExternalTrackParam *)localtrack->GetTPCInnerParam();
  if(!tpcTrack) {
    gPt = 0.0; gPx = 0.0; gPy = 0.0; gPz = 0.0;
    dca[0] = -100.; dca[1] = -100.; dca3D = -100.;
    cov[0] = -100.; cov[1] = -100.; cov[2] = -100.;
  }
  else {
    gPt = tpcTrack->Pt();
    gPx = tpcTrack->Px();
    gPy = tpcTrack->Py();
    gPz = tpcTrack->Pz();
    tpcTrack->PropagateToDCA(localvertex,
    	       localesd->GetMagneticField(),100.,dca,cov);
  }
  
  Int_t fcls[200];
  Int_t nClustersTPC=localtrack->GetTPCclusters(fcls);
  Float_t chi2perTPCcluster=-1.0;
  if(nClustersTPC!=0) chi2perTPCcluster=(localtrack->GetTPCchi2())/Float_t(nClustersTPC);
  
  Double_t extCov[15];
  localtrack->GetExternalCovariance(extCov);
  
  if((localtrack->GetStatus() & AliESDtrack::kTPCrefit) == 0) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because of no refited in TPC");
      return kFALSE;
  } 

  if(nClustersTPC < fMinTPCclusters) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of nclusters (TPC) of %ld (min. requested: %ld)", nClustersTPC, fMinTPCclusters);
      return kFALSE;
  } 
  
  if(chi2perTPCcluster > fMaxChi2PerTPCcluster) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of chi2perTPCcluster of %lf (max. requested: %lf)", chi2perTPCcluster, fMaxChi2PerTPCcluster);
      return kFALSE;
  } 

  if(extCov[0] > fMaxCov0) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of cov[0] of %lf (max. requested: %lf)", cov[0], fMaxCov0);
      return kFALSE;
  }
  
  if(extCov[2] > fMaxCov2) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of cov[2] of %lf (max. requested: %lf)", cov[2], fMaxCov2);
      return kFALSE;
  }
    
  if(extCov[5] > fMaxCov5) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of cov[5] of %lf (max. requested: %lf)", cov[5], fMaxCov5);
      return kFALSE;
  }  
  
  if(extCov[9] > fMaxCov9) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of cov[9] of %lf (max. requested: %lf)", cov[9], fMaxCov9);
      return kFALSE;
  }  
  
  if(extCov[14] > fMaxCov14) {
      if (fDebug > 1) Printf("IsAccepted: Track rejected because it has a value of cov[14] of %lf (max. requested: %lf)", cov[14], fMaxCov14);
      return kFALSE;
  } 
 
  return kTRUE;

}

//________________________________________________________________________
Bool_t AliResonanceKink::IsKink(AliESDEvent *localesd, Int_t kinkIndex, TVector3 trackMom) 
{
   // Test some kinematical criteria for each kink

	 AliESDkink *kink=localesd->GetKink(TMath::Abs(kinkIndex)-1);
	 const TVector3 motherMfromKink(kink->GetMotherP());
	 const TVector3 daughterMKink(kink->GetDaughterP());
	 Float_t qt=kink->GetQt();

	 Double_t maxDecAngKmu=f1->Eval(motherMfromKink.Mag(),0.,0.,0.);
	 Double_t maxDecAngpimu=f2->Eval(motherMfromKink.Mag(),0.,0.,0.);

         Float_t kinkAngle=TMath::RadToDeg()*kink->GetAngle(2);
	 
	 Float_t energyDaughterMu=TMath::Sqrt(daughterMKink.Mag()*daughterMKink.Mag()+0.105658*0.105658);
	 Float_t p1XM= motherMfromKink.Px();
         Float_t p1YM= motherMfromKink.Py();
         Float_t p1ZM= motherMfromKink.Pz();
         Float_t p2XM= daughterMKink.Px();
         Float_t p2YM= daughterMKink.Py();
         Float_t p2ZM= daughterMKink.Pz();
         Float_t p3Daughter=TMath::Sqrt(((p1XM-p2XM)*(p1XM-p2XM))+((p1YM-p2YM)*(p1YM-p2YM))+((p1ZM-p2ZM)*(p1ZM-p2ZM)));
         Double_t invariantMassKmu= TMath::Sqrt((energyDaughterMu+p3Daughter)*(energyDaughterMu+p3Daughter)-motherMfromKink.Mag()*motherMfromKink.Mag());

         if((kinkAngle>maxDecAngpimu)&&(qt>0.05)&&(qt<0.25)&&((kink->GetR()>110.)&&(kink->GetR()<230.))&&(TMath::Abs(trackMom.Eta())<1.1)&&(invariantMassKmu<0.6)) {

           if(trackMom.Mag()<=1.1) {
		return kTRUE;
           }
	   else 
	   if (kinkAngle<maxDecAngKmu) {
		return kTRUE;
	   }
	 }
	 return kFALSE;
}
