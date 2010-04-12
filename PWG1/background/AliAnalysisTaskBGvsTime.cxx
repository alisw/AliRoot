// AliAnalysisTaskBGvsTime

// This task computes various histos as a function of time, using a
// time stamp made from orbit, bx number and period. It is used for
// studies of BG vs time and of luminosity.

// The histograms are booked dinamically when they are called the
// first time. This simplifies the handling of different trigger
// classes (you don't need to know them in advance). The merging is
// then complicated, but it is handled via the external class
// AliHistoListWrapper.

// Different sets of cuts can be used, and the binning of vs time
// histos can be defined dynamically (see setters)

// Can also process MC, in which case the efficiency x acceptance for
// tracks used in the rate (luminosity) histos is also computed, and
// put in a histo in which each bin corresponds to a given rate histo
// (eta < 0.8 and pt > 0.5 or pt > 1.0 at the time I'm writing this).

#include "AliAnalysisTaskBGvsTime.h"
#include "AliESDInputHandler.h"
#include "TString.h"
#include "AliVParticle.h"
#include "AliESDInputHandlerRP.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TBranch.h"
//#include "AliITSRecPoint.h"
#include "AliMultiplicity.h"
#include "AliLog.h"
#include "AliAnalysisManager.h"
#include "AliBackgroundSelection.h"
#include <iostream>
#include "TFile.h"
#include "TCanvas.h"
#include "AliHistoListWrapper.h"
#include "AliTriggerAnalysis.h"
#include "TMath.h"
#include "AliPhysicsSelection.h"
#include "AliBackgroundSelection.h"
#include "AliESDtrackCuts.h"
#include "AliStack.h"
#include "AliMCEvent.h"
#include "TDatime.h"

using namespace std;

ClassImp(AliAnalysisTaskBGvsTime)



AliAnalysisTaskBGvsTime::AliAnalysisTaskBGvsTime()
  : AliAnalysisTaskSE("TaskBGvsTime"),
    fESD(0),fListHisto(0),fListWrapper(0),fStartTime(0),fEndTime(0),
    fNMultBins(0), fMultBins(0),fFirstTimeStamp(0), fLastTimeStamp(0), fBinWidth(10), fNoCuts(0), fPhysicsSelection(0), fUsePhysicsSelection(0),
    fUseZeroBin(0), fIsMC(0), fSkipV0(0), fSkipZeroBin(0), fUseBunchInt(0), fHistoTimeStampVsUTC(0), fHistoTimeStampDiffUTC(0)

{
  // constructor

  DefineOutput(1, AliHistoListWrapper::Class());
  DefineOutput(2, AliPhysicsSelection::Class());
  //  DefineOutput(2, TH1I::Class());

}
AliAnalysisTaskBGvsTime::AliAnalysisTaskBGvsTime(const char * name)
  : AliAnalysisTaskSE(name),fESD (0),fListHisto(0),fListWrapper(0),fStartTime(0),fEndTime(0),
    fNMultBins(0),fMultBins(0),fFirstTimeStamp(0), fLastTimeStamp(0), fBinWidth(10), fNoCuts(0), fPhysicsSelection(0), fUsePhysicsSelection(0),
    fUseZeroBin(0), fIsMC(0), fSkipV0(0), fSkipZeroBin(0), fUseBunchInt(0), fHistoTimeStampVsUTC(0), fHistoTimeStampDiffUTC(0)
{
  //
  // Standard constructur which should be used
  //

  DefineOutput(1, AliHistoListWrapper::Class());
  DefineOutput(2, AliPhysicsSelection::Class());
  //  DefineOutput(2, TH1I::Class());

}

AliAnalysisTaskBGvsTime::AliAnalysisTaskBGvsTime(const AliAnalysisTaskBGvsTime& obj) : 
  AliAnalysisTaskSE(obj) ,fESD (0),fListHisto(0),fListWrapper(0),fStartTime(0),fEndTime(0),
  fNMultBins(0),fMultBins(0),fFirstTimeStamp(0), fLastTimeStamp(0), fBinWidth(10), fNoCuts(0), fPhysicsSelection(0), fUsePhysicsSelection(0),
  fUseZeroBin(0), fIsMC(0), fSkipV0(0), fSkipZeroBin(0), fUseBunchInt(0), fHistoTimeStampVsUTC(0), fHistoTimeStampDiffUTC(0)
{
  //copy ctor
  fESD = obj.fESD ;
  fListHisto= obj.fListHisto;
  fListWrapper= obj.fListWrapper;
  fStartTime= obj.fStartTime;
  fEndTime= obj.fEndTime;
  
  fNMultBins= obj.fNMultBins;
  fMultBins= obj.fMultBins;
  fFirstTimeStamp= obj.fFirstTimeStamp;
  fLastTimeStamp= obj.fLastTimeStamp;
  fBinWidth= obj.fBinWidth;
  fNoCuts= obj.fNoCuts;
  fPhysicsSelection= obj.fPhysicsSelection;
  fUsePhysicsSelection= obj.fUsePhysicsSelection;
  fUseZeroBin= obj.fUseZeroBin;
  fIsMC= obj.fIsMC;
  fSkipV0= obj.fSkipV0;
  fSkipZeroBin= obj.fSkipZeroBin;
  fUseBunchInt= obj.fUseBunchInt;
  fHistoTimeStampVsUTC = obj.fHistoTimeStampVsUTC;
  fHistoTimeStampDiffUTC = obj.fHistoTimeStampDiffUTC;

}

AliAnalysisTaskBGvsTime::~AliAnalysisTaskBGvsTime(){

  // destructor
  if(fListWrapper) {
    delete fListWrapper;
    fListWrapper = 0;
  }
  if(fMultBins) {
    delete fMultBins;
    fMultBins = 0;
  }
  if(fPhysicsSelection) {
    delete fPhysicsSelection;
    fPhysicsSelection = 0;
  }
  // Histos should not be destroyed: fListWrapper is owner!
}
void AliAnalysisTaskBGvsTime::UserCreateOutputObjects()
{
  // Called once
  fListWrapper = new AliHistoListWrapper("histoListWrapper","histoListWrapper");
  fListHisto = fListWrapper->GetList();
  Float_t lenght =  fEndTime - fStartTime;
  Int_t nBins = TMath::FloorNint(lenght/fBinWidth)+1;
  Int_t end   = nBins*fBinWidth;
  // cover 20 days in UTC. I'll set offset by hand below.  
  Int_t lenghtUTC = 20*3600*24;
  Int_t nBinsUTC = TMath::FloorNint(Double_t(lenghtUTC)/fBinWidth)+1;
  Int_t endUTC   = nBinsUTC*fBinWidth;  
  if (!fIsMC) {
    Bool_t oldStatus = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);
    fHistoTimeStampVsUTC = new TH2F ("fHistoTimeStampVsUTC", "fHistoTimeStampVsUTC", nBins, -0.5, end-0.5,  nBinsUTC, -0.5, endUTC-0.5);
    fListHisto->Add(fHistoTimeStampVsUTC);
    fHistoTimeStampDiffUTC = new TH1F("fHistoTimeStampDiffUTC", "fHistoTimeStampDiffUTC", nBinsUTC, -0.5, endUTC-0.5);
    fListHisto->Add(fHistoTimeStampDiffUTC);
    TH1::AddDirectory(oldStatus);
  }
  
  fPhysicsSelection = new AliPhysicsSelection();
  if(fIsMC) fPhysicsSelection->SetAnalyzeMC();
  fPhysicsSelection->SetBin0Callback(this->GetName());

  fPhysicsSelection->SetSkipTriggerClassSelection();// 

  AliBackgroundSelection * bg = new AliBackgroundSelection();
  bg->SetDeltaPhiCut(10);
  fPhysicsSelection->AddBackgroundIdentification(bg);

  if (fSkipV0) {
    fPhysicsSelection->SetSkipV0();
  }

}


void AliAnalysisTaskBGvsTime::UserExec(Option_t *)
{

  /* PostData(0) is taken care of by AliAnalysisTaskSE */
  PostData(1,fListWrapper);
  PostData(2,fPhysicsSelection);
  
  const float etaCut = 0.8;


  fESD = dynamic_cast<AliESDEvent*>(fInputEvent);
  if (strcmp(fESD->ClassName(),"AliESDEvent")) {
    AliFatal("Not processing ESDs");
  }

  if (fUseBunchInt) {
    fPhysicsSelection->SetComputeBG();
  }

  // If it is MC: fill histo of generated events for efficiency calculations
  if (fIsMC) {
    Bool_t atLeastPt1MC  = kFALSE; 
    Bool_t atLeastPt05MC = kFALSE; 

    if (!fMCEvent) {
      AliError("No MC info found");
    } else {
      
      //loop on the MC event
      for (Int_t ipart=0; ipart<fMCEvent->GetNumberOfTracks(); ipart++) { 
	AliMCParticle *mcPart  = (AliMCParticle*)fMCEvent->GetTrack(ipart);
	
	// We don't care about neutrals and non-physical primaries
	if(mcPart->Charge() == 0) continue;
	if(!fMCEvent->Stack()->IsPhysicalPrimary(ipart)) continue;
	
	// Kinematic cuts:
	if (TMath::Abs(mcPart->Eta()) < etaCut) {
	  if (mcPart->Pt() > 0.5) atLeastPt1MC  = kTRUE; 
	  if (mcPart->Pt() > 1.0) atLeastPt05MC = kTRUE; 
	}
	if (atLeastPt1MC && atLeastPt05MC) break; // no need to look for other tracks
      }
      if (atLeastPt1MC ) GetEfficiencyHisto(1)->Fill(kEffPt05);    
      if (atLeastPt05MC) GetEfficiencyHisto(1)->Fill(kEffPt1 );     
    }
  }
  



  // CUTS
  const AliMultiplicity* mult = fESD->GetMultiplicity();
  if (!mult){
    AliFatal("No multiplicity object"); // TODO: Should this be fatal?
  }
  // get number of SPD clusters
  Int_t spdClusters = 0;
  for(Int_t ilayer = 0; ilayer < 2; ilayer++){
    spdClusters += mult->GetNumberOfITSClusters(ilayer);
  }

  //  Bool_t isZeroBin = kFALSE;
  Bool_t isZeroBin = IsEventInBinZero();
  if(fUseZeroBin  && !isZeroBin) return;
  if(fSkipZeroBin && isZeroBin ) return;

  Bool_t physelDecision = fPhysicsSelection->IsCollisionCandidate(fESD);

  if (fUsePhysicsSelection) {
    if(!physelDecision) return;
  }
  else if (!fNoCuts) {if (spdClusters < 2) return;}// At least 2 clusters 
  //  else {AliInfo("No Cuts");}  
  
  

  // get time stamp
  Long64_t timeStampBX = 0;
  timeStampBX = fESD->GetBunchCrossNumber();
  timeStampBX += (Long64_t) 3564 * (fESD->GetOrbitNumber() + fESD->GetPeriodNumber() * 16777215);
  timeStampBX = (Long64_t) (25e-9 * timeStampBX);
  if (fFirstTimeStamp == 0) {
    fFirstTimeStamp = timeStampBX;   
    fLastTimeStamp  = timeStampBX;   
  } 
  if (timeStampBX < fFirstTimeStamp) {
    AliError("Time stamp not monothonic!");
    fFirstTimeStamp = timeStampBX;
  }
  if (timeStampBX > fLastTimeStamp) {
    fLastTimeStamp = timeStampBX;
  }
  timeStampBX -= fStartTime;

  Long64_t timeStamp = timeStampBX;

  Long64_t timeStampGDC = fESD->GetTimeStamp();  
  static TDatime timeOffsetDT(2009,12,5,0,0,0);
  static Long64_t timeOffset = timeOffsetDT.Convert();
  
  if (!fIsMC) {
    fHistoTimeStampVsUTC->Fill(timeStampBX,timeStampGDC-timeOffset);
    fHistoTimeStampDiffUTC->Fill(timeStampGDC-timeOffset-timeStampBX);
  }
  // Get V0 flags
  static AliTriggerAnalysis * triggerAnalysis = new AliTriggerAnalysis();
  Bool_t v0A   = triggerAnalysis->IsOfflineTriggerFired(fESD, AliTriggerAnalysis::kV0A);
  Bool_t v0C   = triggerAnalysis->IsOfflineTriggerFired(fESD, AliTriggerAnalysis::kV0C);
  Bool_t v0ABG = triggerAnalysis->IsOfflineTriggerFired(fESD, AliTriggerAnalysis::kV0ABG);
  Bool_t v0CBG = triggerAnalysis->IsOfflineTriggerFired(fESD, AliTriggerAnalysis::kV0CBG);
  Bool_t isInV0 = !v0ABG && !v0CBG && ((v0A && !v0C) || (v0C && !v0A)); // try to select beam gas in CINT1A/C events: require one v0 interaction (but not the other) and not BG hit in v0. This should select events produced in between the 2 v0s and boosted forward.


  // loop over trigger classes in the event
  TObjArray * tokens = 0;
  if(fIsMC) {
    // in case of montecarlo I override the trigger class to CINT1B for latter compatibility
    tokens = new TObjArray;
    tokens->SetOwner();
    tokens->Add(new TObjString("CINT1B-ABCE-NOPF-ALL")); 
  }
  else {  
    TString trgClasses = fESD->GetFiredTriggerClasses();
    tokens = trgClasses.Tokenize(" ");
  }
  TIter iter(tokens);
    
  while(TObjString * tok = (TObjString*) iter.Next()){
    // clean up trigger name
    TString trg = tok->GetString();
    trg.Strip(TString::kTrailing, ' ');
    trg.Strip(TString::kLeading, ' ');
    // print selected events in !CIN1B trigs:
    //    if(!trg.Contains("CINT1B")) 
//       Printf("File: %s, IEV: %d, TRG: %s, Orbit: 0x%x, Period: %d, BC: %d\n",
// 				       ((TTree*) GetInputData(0))->GetCurrentFile()->GetName(), fESD->GetEventNumberInFile(), 
// 				       trg.Data(),
// 				       fESD->GetOrbitNumber(),fESD->GetPeriodNumber(),fESD->GetBunchCrossNumber());



    // Fill histos
    GetVsTimeHistoAll(trg.Data())->Fill(timeStamp);
    GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"ALL")->Fill(timeStamp);
    GetVsTimeHisto(trg.Data(),-1,         fESD->GetBunchCrossNumber(),"ALL")->Fill(timeStamp);
    GetVsTimeHisto(trg.Data(),spdClusters,-1,                         "ALL")->Fill(timeStamp);
    GetVsTimeHisto(trg.Data(),-1         ,-1,                         "ALL")->Fill(timeStamp);
    if (isInV0)     {
      GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"inV0")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),-1         ,fESD->GetBunchCrossNumber(),"inV0")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),-1         ,-1,                         "inV0")->Fill(timeStamp);
    }      

    // In order to compute mean multiplicity of spd clusters in a time
    // bin, we integrate the multiplicity in that bin, and then we
    // divide for the number of events in that bin in terminate.
    
    // Is the error computed correctly if we fill with a weight ? Looping to be sure...
    for (Int_t iclus = 0; iclus < spdClusters; iclus ++)  { 
      GetVsTimeHisto((TString("hMultSPDvsTime_")+trg).Data())->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),-1,         fESD->GetBunchCrossNumber(),"ALL","MultSPDvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"ALL","MultSPDvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),spdClusters,-1,                         "ALL","MultSPDvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),-1,-1,                                  "ALL","MultSPDvsTime")->Fill(timeStamp);
      if (isInV0)     {
	GetVsTimeHisto(trg.Data(),-1,         fESD->GetBunchCrossNumber(),"inV0","MultSPDvsTime")->Fill(timeStamp);
	GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"inV0","MultSPDvsTime")->Fill(timeStamp);
	GetVsTimeHisto(trg.Data(),-1,-1,                                  "inV0","MultSPDvsTime")->Fill(timeStamp);
      }

    }

    // Keep cluster distribution for reference
    GetDistributionHisto(trg.Data(),kDistSPDMult)->Fill(spdClusters);
    if(isInV0) GetDistributionHisto(trg.Data(),kDistSPDMult,"_inV0")->Fill(spdClusters);
    

    // TPC track multiplicity vs time

    // Selection by andrea.dainese@pd.infn.it
    // 15.03.2010
    

    // Primary vertex
    Bool_t badVertex = kFALSE;
    const AliESDVertex *vertex = fESD->GetPrimaryVertexTracks();
    if(vertex->GetNContributors()<1) {
      // SPD vertex
      vertex = fESD->GetPrimaryVertexSPD();
      if(vertex->GetNContributors()<1) {
	badVertex = kTRUE;
      }
    }
    
    // Fill vertex distribution 
    if ( ((vertex->IsFromVertexerZ() && vertex->GetDispersion()<=0.02) || !vertex->IsFromVertexerZ()) && !badVertex ) {
      GetDistributionHisto(trg.Data(),kDistVertex)->Fill(vertex->GetZ());	
      if (vertex->IsFromVertexerZ()) GetDistributionHisto(trg.Data(),kDistVertexZ) ->Fill(vertex->GetZ());	
      else                           GetDistributionHisto(trg.Data(),kDistVertex3D)->Fill(vertex->GetZ());	
	
      if(isInV0) GetDistributionHisto(trg.Data(),kDistVertex,"_inV0")->Fill(vertex->GetZ());
    }

    // apply a cut |zVertex| < CUT, if needed
    
    // Track cuts (except d0 cut)
    //------- TPC track selection --------
    // Selection by andrea.dainese@pd.infn.it
    // 15.03.2010

    Int_t    minclsTPC=70;
    Double_t maxchi2perTPCcl=4.;
    static AliESDtrackCuts* esdtrackCutsITSTPC = new AliESDtrackCuts("esdtrackCutsITSTPC");
    esdtrackCutsITSTPC->SetRequireITSRefit(kTRUE);
    esdtrackCutsITSTPC->SetClusterRequirementITS(AliESDtrackCuts::kSPD,
						 AliESDtrackCuts::kAny);
    esdtrackCutsITSTPC->SetAcceptKinkDaughters(kFALSE);
    esdtrackCutsITSTPC->SetMinNClustersTPC(minclsTPC);
    esdtrackCutsITSTPC->SetMaxChi2PerClusterTPC(maxchi2perTPCcl);
    //esdtrackCutsITSTPC->SetEtaRange(-0.8,0.8); // normally, |eta|<0.8


    // loop on tracks
    Int_t ntracks = fESD->GetNumberOfTracks();
    if(badVertex) ntracks = -1; // skip loop if the vertex is bad
    
    // flags for luminosity histos
    Bool_t atLeastPt1  = kFALSE;
    Bool_t atLeastPt05 = kFALSE; 
   
    for (Int_t iTrack = 0; iTrack<ntracks; iTrack++) {    
      AliESDtrack * track = dynamic_cast<AliESDtrack*>(fESD->GetTrack(iTrack));
      // for each track
      
      // track quality cuts
      if(!esdtrackCutsITSTPC->AcceptTrack(track)) continue;
      
      // bring it to the primary vertex and compute impact parameters
      if(!track->RelateToVertex(vertex,fESD->GetMagneticField(),kVeryBig)) continue; // this is already done in AliReconstruction...
      
      // track-to-vertex cut (see below)
      if(!SelectOnImpPar(track)) continue;
      
      // Fill histos (TPC Multiplicity vs TIME)
      GetVsTimeHisto(trg.Data(),-1,         fESD->GetBunchCrossNumber(),"ALL","MultTPCvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"ALL","MultTPCvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),spdClusters,-1,                         "ALL","MultTPCvsTime")->Fill(timeStamp);
      GetVsTimeHisto(trg.Data(),-1,-1,                                  "ALL","MultTPCvsTime")->Fill(timeStamp);
      if ((v0A || v0C) && !(v0ABG || v0CBG))     {
	GetVsTimeHisto(trg.Data(),-1,         fESD->GetBunchCrossNumber(),"inV0","MultTPCvsTime")->Fill(timeStamp);
	GetVsTimeHisto(trg.Data(),spdClusters,fESD->GetBunchCrossNumber(),"inV0","MultTPCvsTime")->Fill(timeStamp);
	GetVsTimeHisto(trg.Data(),-1,-1,                                  "inV0","MultTPCvsTime")->Fill(timeStamp);
      }
      
      // Fill pt and DCA distribution
      // pt
      GetDistributionHisto(trg.Data(),kDistPt)->Fill(track->Pt());
      if(isInV0) GetDistributionHisto(trg.Data(),kDistPt, "_inV0")->Fill(track->Pt());
      // dca (on the xy plane)
      Float_t xy_dca, z_dca;
      track->GetImpactParameters(xy_dca,z_dca);
      GetDistributionHisto(trg.Data(),kDistDCATPC)->Fill(xy_dca);
      if(isInV0) GetDistributionHisto(trg.Data(),kDistDCATPC, "_inV0")->Fill(track->Pt());

      // has the event at least one track satisfying the required conditions? (used for rate/luminosity)
      if (TMath::Abs(track->Eta()) < etaCut) {
	if (track->Pt() > 0.5) {
	  atLeastPt05 = kTRUE;
	}
	if (track->Pt() > 1.0) {
	  atLeastPt1  = kTRUE;
	}
      }
    }


    // Fill histos for luminosity: rate of events with at least one
    // track in the pseudo rapidity region |eta| < 0.8 and pt > 0.5 or
    // 1 GeV
    if (atLeastPt05) {
      GetVsTimeHisto(GetVsTimeHistoForLuminosityName(trg.Data(), 0.5))->Fill(timeStamp);
      if(fIsMC) GetEfficiencyHisto(0)->Fill(kEffPt05);
    }
    if (atLeastPt1)  {
      
      if(GetVsTimeHisto(GetVsTimeHistoForLuminosityName(trg.Data(), 1.0))->Fill(timeStamp) < 0) {
	AliWarning(Form("Timestamp out of range %lld", timeStamp));
      };
      if(fIsMC) GetEfficiencyHisto(0)->Fill(kEffPt1);
    }
    
  }

}

void   AliAnalysisTaskBGvsTime::Terminate(Option_t *){

  // normalize and rescale histos
  AliInfo("Normalizing multiplicity histo") ;
  
  fListWrapper = dynamic_cast<AliHistoListWrapper*> (GetOutputData(1));
  if (!fListWrapper){
    AliError("Cannot get list wrapper");
  }
  fListHisto = fListWrapper->GetList();
  fHistoTimeStampDiffUTC = (TH1F*) fListHisto->FindObject("fHistoTimeStampDiffUTC");
  fHistoTimeStampVsUTC   = (TH2F*) fListHisto->FindObject("fHistoTimeStampVsUTC");
  
  // Divide rate histos for the number of events
  TIterator * iter = fListHisto->MakeIterator();
  TH1 * h = 0;
  while ((h = (TH1*) iter->Next())) {
    if((TString(h->GetName())).Contains("hMultSPDvsTime_") || 
       (TString(h->GetName())).Contains("hMultTPCvsTime_") 
       ){
	 
      AliInfo(Form("Normalizing %s",h->GetName()));
      //      continue;
      TString histoname = h->GetName();
      histoname.ReplaceAll("MultSPDvsTime","");
      histoname.ReplaceAll("MultTPCvsTime","");

      TH1* hev = (TH1*)fListHisto->FindObject(histoname.Data());
      if(!hev) {
	AliError(Form(" -> Cannot find events histo %s",histoname.Data()));
	continue;
      }
      AliInfo (Form(" with histo %s",hev->GetName()));
      // Errors on ev num should be ignored in the division
      Int_t nbin  = h->GetNbinsX();
      for(Int_t ibin = 1; ibin <=nbin; ibin++) {
	if(hev->GetBinContent(ibin) != 0) {
	  h->SetBinContent(ibin, h->GetBinContent(ibin)/ hev->GetBinContent(ibin));
	  h->SetBinError  (ibin, h->GetBinError(ibin)  / hev->GetBinContent(ibin));
	}
      }
    }
    if((TString(h->GetName())).Contains("hRate")){
      h->Scale(1.,"width"); // divide for bin width to obtain a rate
    }
  }

  // Compute Efficiency:
  if(fIsMC) {
    TH1F* hgen =(TH1F*) fListHisto->FindObject("hEffGen");
    TH1F* hrec =(TH1F*) fListHisto->FindObject("hEffRec");
    if (!hgen || !hrec) {
      AliError("Cannot find eff histos");
    }
    else {
      TH1F* heff = (TH1F*) fListHisto->FindObject("hEffRatio");
      if (heff) {
	AliWarning("hEffRatio already in output list?");
      }
      else {
	heff = (TH1F*) hgen->Clone("hEffRatio");
      }
      heff->Reset();
      heff->Divide(hrec,hgen,1,1,"B");
      fListHisto->Add(heff);
    }
  }
  

  AliInfo(Form("Time interval: %lld -- %lld",fFirstTimeStamp,fLastTimeStamp)); 

  AliInfo("Saving physics selection histos");
  fPhysicsSelection = dynamic_cast<AliPhysicsSelection*> (GetOutputData(2));
  
  TFile* fout = new TFile("event_stat.root", "RECREATE");
  
  if (fPhysicsSelection)
      {
	fPhysicsSelection->Print();
	fPhysicsSelection->SaveHistograms("physics_selection");
      }
  
  fout->Write();
  fout->Close();

}

TH1F * AliAnalysisTaskBGvsTime::BookVsTimeHisto(const char * name, const char * title){

  // Book istograms vs time

  AliInfo(Form("Booking histo %s",name));

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  //  static Int_t nBins =  fEndTime - fStartTime + 1;
  // Compute bin width and range.
  // if time interval is not a multiple of bin width drop the last bin

  Float_t lenght =  fEndTime - fStartTime;
  Int_t nBins = TMath::FloorNint(lenght/fBinWidth)+1;
  Float_t * bins = new Float_t[nBins+1];
  for(Int_t ibin = 0; ibin <= nBins; ibin++){
    Float_t edge = ibin*fBinWidth - 0.5;
    bins[ibin] = edge < (lenght - 0.5) ? edge  : (lenght-0.5);
  }
  
  //  Int_t end   = nBins*fBinWidth;

  //  TH1F * h = new TH1F(name,title, nBins,  - 0.5, fEndTime - fStartTime + 0.5);
  TH1F * h = new TH1F(name,title, nBins, bins);
  fListHisto->Add(h);
  h->SetXTitle("orbit");
  h->Sumw2();

  TH1::AddDirectory(oldStatus);
  
  return h;
}

TH1F * AliAnalysisTaskBGvsTime::GetVsTimeHisto(const char * name) {

  // Returns vs time histo. If not existing, creates it.
  
  TH1F * h = (TH1F*) fListHisto->FindObject(name);
  if(!h) h = BookVsTimeHisto(name,name);
  return h;

}

TH1F * AliAnalysisTaskBGvsTime::GetEfficiencyHisto(Bool_t Gen_NotRec) {

  // Return efficiency histo. If not existing, creates it.
  // 1 bin per category

  // Probability of reconstructing one event with at least one
  // particle in a given category
  // This is only created for MC.

  // If the first argument is true, returns the histo at generation
  // level, otherwise returns the histo at rec level. Fill this histo
  // with elements of the enum kEffPt1, kEffPt05, ...

  const char * name = Gen_NotRec ? "hEffGen" : "hEffRec";

  TH1F * h = (TH1F*) fListHisto->FindObject(name);
  if(!h) {
    Bool_t oldStatus = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);
    h = new TH1F (name,name, kNEff, -0.5, kNEff-0.5);
    h->GetXaxis()->SetBinLabel(h->FindBin(kEffPt1) , "pt > 1.0 GeV");
    h->GetXaxis()->SetBinLabel(h->FindBin(kEffPt05), "pt > 0.5 GeV");
    h->Sumw2();
    fListHisto->Add(h);    
    TH1::AddDirectory(oldStatus);
  }

  return h;

}

const char * AliAnalysisTaskBGvsTime::GetVsTimeHistoForLuminosityName(const char * trigger_class, Float_t ptmin) {

  // Compose name of rate histos

  static TString name;
  name = "hRate_";
  name += trigger_class;

  name += Form ("_etamin_0.8_ptmin_%2.2f",ptmin);

  return name.Data();
}


const char * AliAnalysisTaskBGvsTime::GetVsTimeHistoName(const char * trigger_class, Int_t nclusters, Int_t bx, const char * v0flag, const char *prefix){
  
  // compose name of vs time histos w/ different cuts
  
  static TString name;
  name = "h";
  name = name +prefix+"_";
  name += trigger_class;

  if (nclusters >= 0) { 
    if(fMultBins){
      // Add multiplicity label
      Int_t selected_bin = -1;
      for(Int_t ibin = 0; ibin < (fNMultBins-1); ibin++){
	if (nclusters < fMultBins[ibin+1]) {
	  selected_bin = ibin;
	  break;
	}
      }
      if(selected_bin < 0) {
	AliError(Form("Cannot get mult bin - %d", nclusters));
	selected_bin = (fNMultBins-1);
      }
      name += Form("_SPDCls_%d-%d",fMultBins[selected_bin],fMultBins[selected_bin+1]);
      //    AliInfo(Form("Mult bin: %d, %d, %s", nclusters, selected_bin, name.Data()));        
    }
  } else {
    name += "_SPDCls_ALL";
  }
  if(bx < 0) name = name +"_BXALL_"+v0flag; 
  else       name = name +"_BX"+long(bx)+"_"+v0flag;
  return name.Data();
}
const char * AliAnalysisTaskBGvsTime::GetVsTimeHistoNameAll(const char * trigger_class) {

  // Compose default name of vstime histos in a given trigger class

  static TString name;
  name = "h_";
  name += trigger_class;
  return name.Data();

}

TH1F * AliAnalysisTaskBGvsTime::GetDistributionHisto(const char * trigger_class, Int_t dist, const char * suffix) {

  // Returns distributions histos. If not existing, creates it.

  // Possible distributions:
  // - SPD cluster
  // - TPC tracks multiplicity
  // - TPC tracks pt
  // - Vz

  TString name  ;
  TString title ;
  TString xtitle;
  Int_t   nbin =0;
  Float_t   min  =0;
  Float_t   max  =0;


  if (dist == kDistSPDMult) {
    name  = "hMultSPD_";
    title = "SPD Cluster Multiplicity (";
    xtitle = "SPD Clusters";
    nbin  = 50;
    min   = 0;
    max   = 100;
  } else if (dist == kDistTPCMult) {
    name  = "hMultTPC_";
    title = "TPC Tracks Multiplicity (";
    xtitle = "TPC Tracks";
    nbin  = 25;
    min   = 0;
    max   = 50;
  } else if (dist == kDistPt) {
    name  = "hPt_";
    title = "p_{T} distribution (";
    xtitle = "p_{T} (GeV)";
    nbin  = 50;
    min   = 0;
    max   = 10;
  } else if (dist == kDistVertex || dist == kDistVertexZ || dist == kDistVertex3D ) {
    nbin  = 120;
    min   = -30;
    max   = 30;
    name  = "hVz_";
    title = "V_{z} distribution";
    if      (dist == kDistVertexZ)  {
      title += " - Vertexer Z  (";
      name+= "Z_";
    }
    else if (dist == kDistVertex3D) {
      title += " - Vertexer 3D (";    
      name+= "3D_";
    }
    else    title += " (";
    xtitle = "V_{z} (cm)";
  } else if (dist == kDistDCATPC) {
    nbin  = 50;
    min   = -0.5;
    max   = +0.5;
    name  = "hDCA_";
    title = "TPC DCA distribution (";    
  } else {
    AliError(Form("Distribution type not supported: %d",dist));
    return 0;
  } 


  name += trigger_class;
  if (suffix) name += suffix;
  title = title+name+")";

  TH1F* h = (TH1F*) fListHisto->FindObject(name.Data());

  if(!h) h = BookDistributionHisto(name.Data(), title.Data(), xtitle.Data(), nbin, min, max);
  return h;

}

TH1F * AliAnalysisTaskBGvsTime::BookDistributionHisto(const char * name, const char * title, const char * xtitle, Int_t nbin, Float_t min, Float_t max) {

  // Book distributions histos

  AliInfo(Form("Booking histo %s",name));

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  TH1F * h = new TH1F (name,title,nbin,min,max);
  h->Sumw2();
  h->SetXTitle(xtitle);
  //  h->SetYTitle("N");
  fListHisto->Add(h);
  TH1::AddDirectory(oldStatus);

  return h;

}


Bool_t AliAnalysisTaskBGvsTime::IsEventInBinZero() {

  // Returns true if an event is to be assigned to the zero bin

  Bool_t isZeroBin = kTRUE;
  const AliESDEvent* esd=fESD;
  const AliMultiplicity* mult = esd->GetMultiplicity();
  if (!mult){
    Printf("AliAnalysisTaskBGvsTime::IsBinZero: Can't get mult object");
    return kFALSE;
  }
  Int_t ntracklet = mult->GetNumberOfTracklets();
  const AliESDVertex * vtxESD = esd->GetPrimaryVertexSPD();
  if(vtxESD) {
    // If there is a vertex from vertexer z with delta phi > 0.02 we
    // don't consider it rec (we keep the event in bin0). If quality
    // is good eneough we check the number of tracklets
    // if the vertex is more than 15 cm away, this is autamatically bin0
    if( TMath::Abs(vtxESD->GetZ()) <= 15 ) {
      if (vtxESD->IsFromVertexerZ()) {
	if (vtxESD->GetDispersion()<=0.02 ) {
	  if(ntracklet>0) isZeroBin = kFALSE;
	}
      } else if(ntracklet>0) isZeroBin = kFALSE; // if the event is not from Vz we chek the n of tracklets
    } 
  }
  return isZeroBin;

}

Bool_t AliAnalysisTaskBGvsTime::SelectOnImpPar(AliESDtrack* t) {
  // from andrea dainese
  // cut on transverse impact parameter
  Float_t d0z0[2],covd0z0[3];
  t->GetImpactParameters(d0z0,covd0z0);
  Float_t sigma= 0.0050+0.0060/TMath::Power(t->Pt(),0.9);
  Float_t d0max = 7.*sigma;
  if(TMath::Abs(d0z0[0]) < d0max) return kTRUE;
  return kFALSE;
}




