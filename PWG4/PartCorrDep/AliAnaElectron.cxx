/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes hereby granted      *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
/* $Id: $ */

//_________________________________________________________________________
//
// Class for the electron identification.
// Clusters from EMCAL matched to tracks
// and kept in the AOD. Few histograms produced.
//
// -- Author: J.L. Klay (Cal Poly), M. Heinz (Yale)
//////////////////////////////////////////////////////////////////////////////
  
// --- ROOT system --- 
#include <TH2F.h>
#include <TParticle.h>
#include <TNtuple.h>
#include <TClonesArray.h>
#include <TObjString.h>
//#include <Riostream.h>

// --- Analysis system --- 
#include "AliAnaElectron.h" 
#include "AliCaloTrackReader.h"
#include "AliMCAnalysisUtils.h"
#include "AliAODCaloCluster.h"
#include "AliFidutialCut.h"
#include "AliAODTrack.h"
#include "AliAODPid.h"
#include "AliCaloPID.h"
#include "AliAODMCParticle.h"
#include "AliStack.h"
#include "AliExternalTrackParam.h"
#include "AliESDv0.h"
#include "AliESDtrack.h"
#include "AliAODJet.h"
#include "AliAODEvent.h"

ClassImp(AliAnaElectron)
  
//____________________________________________________________________________
AliAnaElectron::AliAnaElectron() 
: AliAnaPartCorrBaseClass(),fCalorimeter(""),
  fpOverEmin(0.),fpOverEmax(0.),fResidualCut(0.),
  fDrCut(0.),fPairDcaCut(0.),fDecayLenCut(0.),fImpactCut(0.),
  fAssocPtCut(0.),fMassCut(0.),fSdcaCut(0.),fITSCut(0),
  fNTagTrkCut(0),fIPSigCut(0.),
  fWriteNtuple(kFALSE),
  //matching checks
  fEleNtuple(0),
  fh1pOverE(0),fh1dR(0),fh2EledEdx(0),fh2MatchdEdx(0),fh2dEtadPhi(0),
  fh2dEtadPhiMatched(0),fh2dEtadPhiUnmatched(0),
  fh2TrackPVsClusterE(0),fh2TrackPtVsClusterE(0),fh2TrackPhiVsClusterPhi(0),fh2TrackEtaVsClusterEta(0),
  //Photonic electron checks
  fh1OpeningAngle(0),fh1MinvPhoton(0),
  //reco
  fhPtElectron(0),fhPhiElectron(0),fhEtaElectron(0),
  fhPtNPE(0),fhPhiNPE(0),fhEtaNPE(0),
  fhPtPE(0),fhPhiPE(0),fhEtaPE(0),
  fhPtConversion(0),fhPhiConversion(0),fhEtaConversion(0),
  fhPtBottom(0),fhPhiBottom(0),fhEtaBottom(0),
  fhPtCharm(0),fhPhiCharm(0),fhEtaCharm(0),
  fhPtCFromB(0),fhPhiCFromB(0),fhEtaCFromB(0),
  fhPtDalitz(0),fhPhiDalitz(0),fhEtaDalitz(0),
  fhPtWDecay(0),fhPhiWDecay(0),fhEtaWDecay(0),
  fhPtZDecay(0),fhPhiZDecay(0),fhEtaZDecay(0),
  fhPtAll(0),fhPhiAll(0),fhEtaAll(0),
  fhPtUnknown(0),fhPhiUnknown(0),fhEtaUnknown(0),
  fhPtMisidentified(0),fhPhiMisidentified(0),fhEtaMisidentified(0),
  fhPtHadron(0),fhPtEleTrkDet(0),
  //event QA
  fhImpactXY(0),fhRefMult(0),fhRefMult2(0),
  //B-tagging
  fhDVMBtagCut1(0),fhDVMBtagCut2(0),fhDVMBtagCut3(0),fhDVMBtagQA1(0),fhDVMBtagQA2(0),fhDVMBtagQA3(0),
  fhDVMBtagQA4(0),fhDVMBtagQA5(0),fhIPSigBtagQA1(0),fhIPSigBtagQA2(0),
  //B-jets
  fhJetType(0),fhBJetXsiFF(0),fhBJetPtFF(0),fhBJetEtaPhi(0),fhNonBJetXsiFF(0),fhNonBJetPtFF(0),fhNonBJetEtaPhi(0),
  //MC
  fMCEleNtuple(0),fhPtMCHadron(0),fhPtMCBottom(0),fhPtMCCharm(0),fhPtMCCFromB(0),fhPtMCConversion(0),
  fhPtMCDalitz(0),fhPtMCWDecay(0),fhPtMCZDecay(0),fhPtMCUnknown(0)
{
  //default ctor
  
  //Initialize parameters
  InitParameters();

}

//____________________________________________________________________________
AliAnaElectron::AliAnaElectron(const AliAnaElectron & g) 
  : AliAnaPartCorrBaseClass(g),fCalorimeter(g.fCalorimeter),
    fpOverEmin(g.fpOverEmin),fpOverEmax(g.fpOverEmax),fResidualCut(g.fResidualCut),
    fDrCut(g.fDrCut),fPairDcaCut(g.fPairDcaCut),fDecayLenCut(g.fDecayLenCut),fImpactCut(g.fImpactCut),
    fAssocPtCut(g.fAssocPtCut),fMassCut(g.fMassCut),fSdcaCut(g.fSdcaCut),fITSCut(g.fITSCut),
    fNTagTrkCut(g.fNTagTrkCut),fIPSigCut(g.fIPSigCut),
    fWriteNtuple(g.fWriteNtuple),
    //matching checks
    fEleNtuple(g.fEleNtuple),
    fh1pOverE(g.fh1pOverE),fh1dR(g.fh1dR),
    fh2EledEdx(g.fh2EledEdx),fh2MatchdEdx(g.fh2MatchdEdx),fh2dEtadPhi(g.fh2dEtadPhi),
    fh2dEtadPhiMatched(g.fh2dEtadPhiMatched),fh2dEtadPhiUnmatched(g.fh2dEtadPhiUnmatched),
    fh2TrackPVsClusterE(g.fh2TrackPVsClusterE),fh2TrackPtVsClusterE(g.fh2TrackPtVsClusterE),
    fh2TrackPhiVsClusterPhi(g.fh2TrackPhiVsClusterPhi),fh2TrackEtaVsClusterEta(g.fh2TrackEtaVsClusterEta),
    //Photonic electron checks
    fh1OpeningAngle(g.fh1OpeningAngle),fh1MinvPhoton(g.fh1MinvPhoton),
    //reco
    fhPtElectron(g.fhPtElectron),fhPhiElectron(g.fhPhiElectron),fhEtaElectron(g.fhEtaElectron),
    fhPtNPE(g.fhPtNPE),fhPhiNPE(g.fhPhiNPE),fhEtaNPE(g.fhEtaNPE),
    fhPtPE(g.fhPtPE),fhPhiPE(g.fhPhiPE),fhEtaPE(g.fhEtaPE),
    fhPtConversion(g.fhPtConversion),fhPhiConversion(g.fhPhiConversion),fhEtaConversion(g.fhEtaConversion),
    fhPtBottom(g.fhPtBottom),fhPhiBottom(g.fhPhiBottom),fhEtaBottom(g.fhEtaBottom),
    fhPtCharm(g.fhPtCharm),fhPhiCharm(g.fhPhiCharm),fhEtaCharm(g.fhEtaCharm),
    fhPtCFromB(g.fhPtCFromB),fhPhiCFromB(g.fhPhiCFromB),fhEtaCFromB(g.fhEtaCFromB),
    fhPtDalitz(g.fhPtDalitz),fhPhiDalitz(g.fhPhiDalitz),fhEtaDalitz(g.fhEtaDalitz),
    fhPtWDecay(g.fhPtWDecay),fhPhiWDecay(g.fhPhiWDecay),fhEtaWDecay(g.fhEtaWDecay),
    fhPtZDecay(g.fhPtZDecay),fhPhiZDecay(g.fhPhiZDecay),fhEtaZDecay(g.fhEtaZDecay),
    fhPtAll(g.fhPtAll),fhPhiAll(g.fhPhiAll),fhEtaAll(g.fhEtaAll),
    fhPtUnknown(g.fhPtUnknown),fhPhiUnknown(g.fhPhiUnknown),fhEtaUnknown(g.fhEtaUnknown),
    fhPtMisidentified(g.fhPtMisidentified),fhPhiMisidentified(g.fhPhiMisidentified),fhEtaMisidentified(g.fhEtaMisidentified),
    fhPtHadron(g.fhPtHadron),fhPtEleTrkDet(g.fhPtEleTrkDet),
    //event QA
    fhImpactXY(g.fhImpactXY),fhRefMult(g.fhRefMult),fhRefMult2(g.fhRefMult2),
    //B-tagging
    fhDVMBtagCut1(g.fhDVMBtagCut1),fhDVMBtagCut2(g.fhDVMBtagCut2),fhDVMBtagCut3(g.fhDVMBtagCut3),
    fhDVMBtagQA1(g.fhDVMBtagQA1),fhDVMBtagQA2(g.fhDVMBtagQA2),fhDVMBtagQA3(g.fhDVMBtagQA3),
    fhDVMBtagQA4(g.fhDVMBtagQA4),fhDVMBtagQA5(g.fhDVMBtagQA5),fhIPSigBtagQA1(g.fhIPSigBtagQA1),
    fhIPSigBtagQA2(g.fhIPSigBtagQA2),
    //B-jets
    fhJetType(g.fhJetType),fhBJetXsiFF(g.fhBJetXsiFF),fhBJetPtFF(g.fhBJetPtFF),fhBJetEtaPhi(g.fhBJetEtaPhi),
    fhNonBJetXsiFF(g.fhNonBJetXsiFF),fhNonBJetPtFF(g.fhNonBJetPtFF),fhNonBJetEtaPhi(g.fhNonBJetEtaPhi),
    //MC
    fMCEleNtuple(g.fMCEleNtuple),fhPtMCHadron(g.fhPtMCHadron),fhPtMCBottom(g.fhPtMCBottom),
    fhPtMCCharm(g.fhPtMCCharm),fhPtMCCFromB(g.fhPtMCCFromB),fhPtMCConversion(g.fhPtMCConversion),
    fhPtMCDalitz(g.fhPtMCDalitz),fhPtMCWDecay(g.fhPtMCWDecay),
    fhPtMCZDecay(g.fhPtMCZDecay),fhPtMCUnknown(g.fhPtMCUnknown)
{
  // cpy ctor
  
}

//_________________________________________________________________________
AliAnaElectron & AliAnaElectron::operator = (const AliAnaElectron & g)
{
  // assignment operator
  
  if(&g == this) return *this;
  fCalorimeter = g.fCalorimeter;
  fpOverEmin = g.fpOverEmin;
  fpOverEmax = g.fpOverEmax;
  fResidualCut = g.fResidualCut;
  fDrCut = g.fDrCut;
  fPairDcaCut = g.fPairDcaCut;
  fDecayLenCut = g.fDecayLenCut;
  fImpactCut = g.fImpactCut;
  fAssocPtCut = g.fAssocPtCut;
  fMassCut = g.fMassCut;
  fSdcaCut = g.fSdcaCut;
  fITSCut = g.fITSCut;
  fNTagTrkCut = g.fNTagTrkCut;
  fIPSigCut = g.fIPSigCut;
  fWriteNtuple = g.fWriteNtuple;
  fEleNtuple = g.fEleNtuple;
  fh1pOverE = g.fh1pOverE;
  fh1dR = g.fh1dR;
  fh2EledEdx = g.fh2EledEdx;
  fh2MatchdEdx = g.fh2MatchdEdx;
  fh2dEtadPhi = g.fh2dEtadPhi;
  fh2dEtadPhiMatched = g.fh2dEtadPhiMatched;
  fh2dEtadPhiUnmatched = g.fh2dEtadPhiUnmatched;
  fh2TrackPVsClusterE = g.fh2TrackPVsClusterE;
  fh2TrackPtVsClusterE = g.fh2TrackPtVsClusterE;
  fh2TrackPhiVsClusterPhi = g.fh2TrackPhiVsClusterPhi;
  fh2TrackEtaVsClusterEta = g.fh2TrackEtaVsClusterEta;   
  fh1OpeningAngle = g.fh1OpeningAngle;
  fh1MinvPhoton = g.fh1MinvPhoton;
  fhPtElectron = g.fhPtElectron;
  fhPhiElectron = g.fhPhiElectron;
  fhEtaElectron = g.fhEtaElectron;
  fhPtNPE = g.fhPtNPE;
  fhPhiNPE = g.fhPhiNPE;
  fhEtaNPE = g.fhEtaNPE;
  fhPtPE = g.fhPtPE;
  fhPhiPE = g.fhPhiPE;
  fhEtaPE = g.fhEtaPE;
  fhPtConversion = g.fhPtConversion;
  fhPhiConversion = g.fhPhiConversion;
  fhEtaConversion = g.fhEtaConversion;
  fhPtBottom = g.fhPtBottom;
  fhPhiBottom = g.fhPhiBottom;
  fhEtaBottom = g.fhEtaBottom;
  fhPtCharm = g.fhPtCharm;
  fhPhiCharm = g.fhPhiCharm;
  fhEtaCharm = g.fhEtaCharm;
  fhPtCFromB = g.fhPtCFromB;
  fhPhiCFromB = g.fhPhiCFromB;
  fhEtaCFromB = g.fhEtaCFromB;
  fhPtDalitz = g.fhPtDalitz;
  fhPhiDalitz = g.fhPhiDalitz;
  fhEtaDalitz = g.fhEtaDalitz;
  fhPtWDecay = g.fhPtWDecay;
  fhPhiWDecay = g.fhPhiWDecay;
  fhEtaWDecay = g.fhEtaWDecay;
  fhPtZDecay = g.fhPtZDecay;
  fhPhiZDecay = g.fhPhiZDecay;
  fhEtaZDecay = g.fhEtaZDecay;
  fhPtAll = g.fhPtAll;
  fhPhiAll = g.fhPhiAll;
  fhEtaAll = g.fhEtaAll;
  fhPtUnknown = g.fhPtUnknown;
  fhPhiUnknown = g.fhPhiUnknown;
  fhEtaUnknown = g.fhEtaUnknown;
  fhPtMisidentified = g.fhPtMisidentified;
  fhPhiMisidentified = g.fhPhiMisidentified;
  fhEtaMisidentified = g.fhEtaMisidentified;

  fhPtHadron = g.fhPtHadron;
  fhPtEleTrkDet = g.fhPtEleTrkDet;

  //event QA
  fhImpactXY = g.fhImpactXY;
  fhRefMult = g.fhRefMult;
  fhRefMult2 = g.fhRefMult2;

  //B-tagging
  fhDVMBtagCut1 = g.fhDVMBtagCut1;
  fhDVMBtagCut2 = g.fhDVMBtagCut2;
  fhDVMBtagCut3 = g.fhDVMBtagCut3;
  fhDVMBtagQA1 = g.fhDVMBtagQA1;
  fhDVMBtagQA2 = g.fhDVMBtagQA2;
  fhDVMBtagQA3 = g.fhDVMBtagQA3;
  fhDVMBtagQA4 = g.fhDVMBtagQA4;
  fhDVMBtagQA5 = g.fhDVMBtagQA5;
  fhIPSigBtagQA1 = g.fhIPSigBtagQA1;
  fhIPSigBtagQA2 = g.fhIPSigBtagQA2;

  fhJetType = g.fhJetType;
  fhBJetXsiFF = g.fhBJetXsiFF;
  fhBJetPtFF = g.fhBJetPtFF;
  fhBJetEtaPhi = g.fhBJetEtaPhi;
  fhNonBJetXsiFF = g.fhNonBJetXsiFF;
  fhNonBJetPtFF = g.fhNonBJetPtFF;
  fhNonBJetEtaPhi = g.fhNonBJetEtaPhi;

  fMCEleNtuple = g.fMCEleNtuple;
  fhPtMCHadron = g.fhPtMCHadron;
  fhPtMCBottom = g.fhPtMCBottom;
  fhPtMCCharm = g.fhPtMCCharm;
  fhPtMCCFromB = g.fhPtMCCFromB;
  fhPtMCConversion = g.fhPtMCConversion;
  fhPtMCDalitz = g.fhPtMCDalitz;
  fhPtMCWDecay = g.fhPtMCWDecay;
  fhPtMCZDecay = g.fhPtMCZDecay;
  fhPtMCUnknown = g.fhPtMCUnknown;

  return *this;
  
}

//____________________________________________________________________________
AliAnaElectron::~AliAnaElectron() 
{
  //dtor

}


//________________________________________________________________________
TList *  AliAnaElectron::GetCreateOutputObjects()
{  
  // Create histograms to be saved in output file and 
  // store them in outputContainer
  TList * outputContainer = new TList() ; 
  outputContainer->SetName("ElectronHistos") ; 
  
  //created ele ntuple for further analysis
  if(fWriteNtuple) {
      fEleNtuple = new TNtuple("EleNtuple","Electron Ntuple","tmctag:cmctag:pt:phi:eta:p:E:deta:dphi:nCells:dEdx:pidProb:impXY:impZ");
    outputContainer->Add(fEleNtuple) ;
  }

  Int_t nptbins  = GetHistoNPtBins();
  Int_t nphibins = GetHistoNPhiBins();
  Int_t netabins = GetHistoNEtaBins();
  Float_t ptmax  = GetHistoPtMax();
  Float_t phimax = GetHistoPhiMax();
  Float_t etamax = GetHistoEtaMax();
  Float_t ptmin  = GetHistoPtMin();
  Float_t phimin = GetHistoPhiMin();
  Float_t etamin = GetHistoEtaMin();	

  fh1pOverE = new TH1F("h1pOverE","EMCAL-TRACK matches p/E",100,0.,10.);
  fh1dR = new TH1F("h1dR","EMCAL-TRACK matches dR",300, 0.,TMath::Pi());
  fh2EledEdx = new TH2F("h2EledEdx","dE/dx vs. p for electrons",200,0.,50.,200,0.,400.);
  fh2MatchdEdx = new TH2F("h2MatchdEdx","dE/dx vs. p for all matches",200,0.,50.,200,0.,400.);
  fh2dEtadPhi = new TH2F("h2dEtadPhi","#Delta#eta vs. #Delta#phi for all track-cluster pairs",200,0.,1.4,300,0.,TMath::Pi());
  fh2dEtadPhiMatched = new TH2F("h2dEtadPhiMatched","#Delta#eta vs. #Delta#phi for matched track-cluster pairs",200,0.,1.4,300,0.,TMath::Pi());
  fh2dEtadPhiUnmatched = new TH2F("h2dEtadPhiUnmatched","#Delta#eta vs. #Delta#phi for unmatched track-cluster pairs",200,0.,1.4,300,0.,TMath::Pi());

  fh2TrackPVsClusterE = new TH2F("h2TrackPVsClusterE","h2TrackPVsClusterE",nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
  fh2TrackPtVsClusterE = new TH2F("h2TrackPtVsClusterE","h2TrackPtVsClusterE",nptbins,ptmin,ptmax,nptbins,ptmin,ptmax);
  fh2TrackPhiVsClusterPhi = new TH2F("h2TrackPhiVsClusterPhi","h2TrackPhiVsClusterPhi",nphibins,phimin,phimax,nphibins,phimin,phimax);
  fh2TrackEtaVsClusterEta = new TH2F("h2TrackEtaVsClusterEta","h2TrackEtaVsClusterEta",netabins,etamin,etamax,netabins,etamin,etamax);

  outputContainer->Add(fh1pOverE) ; 
  outputContainer->Add(fh1dR) ; 
  outputContainer->Add(fh2EledEdx) ;
  outputContainer->Add(fh2MatchdEdx) ;
  outputContainer->Add(fh2dEtadPhi) ;
  outputContainer->Add(fh2dEtadPhiMatched) ;
  outputContainer->Add(fh2dEtadPhiUnmatched) ;
  outputContainer->Add(fh2TrackPVsClusterE) ;
  outputContainer->Add(fh2TrackPtVsClusterE) ;
  outputContainer->Add(fh2TrackPhiVsClusterPhi) ;
  outputContainer->Add(fh2TrackEtaVsClusterEta) ;
  
  //photonic electron checks
  fh1OpeningAngle = new TH1F("hOpeningAngle","Opening angle between e+e- pairs",100,0.,TMath::Pi());
  fh1MinvPhoton = new TH1F("hMinvPhoton","Invariant mass of e+e- pairs",200,0.,2.);

  outputContainer->Add(fh1OpeningAngle);
  outputContainer->Add(fh1MinvPhoton);

  fhPtElectron = new TH1F("hPtElectron","Electron pT",nptbins,ptmin,ptmax);
  fhPhiElectron = new TH2F("hPhiElectron","Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
  fhEtaElectron = new TH2F("hEtaElectron","Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
  fhPtNPE = new TH1F("hPtNPE","Non-photonic Electron pT",nptbins,ptmin,ptmax);
  fhPhiNPE = new TH2F("hPhiNPE","Non-photonic Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
  fhEtaNPE = new TH2F("hEtaNPE","Non-photonic Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
  fhPtPE = new TH1F("hPtPE","Photonic Electron pT",nptbins,ptmin,ptmax);
  fhPhiPE = new TH2F("hPhiPE","Photonic Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
  fhEtaPE = new TH2F("hEtaPE","Photonic Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);

  outputContainer->Add(fhPtElectron) ; 
  outputContainer->Add(fhPhiElectron) ; 
  outputContainer->Add(fhEtaElectron) ;
  outputContainer->Add(fhPtNPE) ; 
  outputContainer->Add(fhPhiNPE) ; 
  outputContainer->Add(fhEtaNPE) ;
  outputContainer->Add(fhPtPE) ; 
  outputContainer->Add(fhPhiPE) ; 
  outputContainer->Add(fhEtaPE) ;

  fhPtHadron = new TH1F("hPtHadron","Charged hadrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
  fhPtEleTrkDet = new TH1F("hPtEleTrkDet","Electrons identified by tracking detectors w/in EMCAL acceptance",nptbins,ptmin,ptmax);

  outputContainer->Add(fhPtHadron);
  outputContainer->Add(fhPtEleTrkDet);

  //event QA
  fhImpactXY = new TH1F("hImpactXY","Impact parameter for all tracks",200,-10,10.);
  fhRefMult = new TH1F("hRefMult" ,"refmult QA: " ,100,0,200);
  fhRefMult2  = new TH1F("hRefMult2" ,"refmult2 QA: " ,100,0,200);

  outputContainer->Add(fhImpactXY);
  outputContainer->Add(fhRefMult);
  outputContainer->Add(fhRefMult2);

  //B-tagging
  fhDVMBtagCut1 = new TH2F("hdvmbtag_cut1","DVM B-tag result cut1", 10,0,10 ,nptbins,ptmin,ptmax);
  fhDVMBtagCut2 = new TH2F("hdvmbtag_cut2","DVM B-tag result cut2", 10,0,10 ,nptbins,ptmin,ptmax);
  fhDVMBtagCut3 = new TH2F("hdvmbtag_cut3","DVM B-tag result cut3", 10,0,10 ,nptbins,ptmin,ptmax);
  fhDVMBtagQA1  = new TH2F("hdvmbtag_qa1" ,"DVM B-tag QA: pairDCA vs length", 100,0,0.2 ,100,0,1.0);
  fhDVMBtagQA2  = new TH2F("hdvmbtag_qa2" ,"DVM B-tag QA: signDCA vs mass"  , 200,-0.5,0.5 ,100,0,10);
  fhDVMBtagQA3  = new TH1F("hdvmbtag_qa3" ,"DVM B-tag QA: ITS-Hits electron" ,7,0,7);
  fhDVMBtagQA4  = new TH1F("hdvmbtag_qa4" ,"DVM B-tag QA: IP d electron" ,200,-3,3);
  fhDVMBtagQA5  = new TH1F("hdvmbtag_qa5" ,"DVM B-tag QA: IP z electron" ,200,-3,3);
  fhIPSigBtagQA1  = new TH1F("hipsigbtag_qa1" ,"IPSig B-tag QA: # tag tracks", 20,0,20);
  fhIPSigBtagQA2  = new TH1F("hipsigbtag_qa2" ,"IPSig B-tag QA: IP significance", 200,-10.,10.);

  outputContainer->Add(fhDVMBtagCut1) ;
  outputContainer->Add(fhDVMBtagCut2) ;
  outputContainer->Add(fhDVMBtagCut3) ;
  outputContainer->Add(fhDVMBtagQA1) ;
  outputContainer->Add(fhDVMBtagQA2) ;
  outputContainer->Add(fhDVMBtagQA3) ;
  outputContainer->Add(fhDVMBtagQA4) ;
  outputContainer->Add(fhDVMBtagQA5) ;
  outputContainer->Add(fhIPSigBtagQA1) ;
  outputContainer->Add(fhIPSigBtagQA2) ;

  fhJetType = new TH2F("hJetType","# jets passing each tag method vs jet pt",10,0,10,300,0.,300.);
  fhBJetXsiFF = new TH2F("hBJetXsiFF","B-jet #Xsi Frag. Fn.",100,0.,10.,300,0.,300.);
  fhBJetPtFF = new TH2F("hBJetPtFF","B-jet p_{T} Frag. Fn.",nptbins,ptmin,ptmax,300,0.,300.);
  fhBJetEtaPhi = new TH2F("hBJetEtaPhi","B-jet eta-phi distribution",netabins,etamin,etamax,nphibins,phimin,phimax);
  fhNonBJetXsiFF = new TH2F("hNonBJetXsiFF","Non B-jet #Xsi Frag. Fn.",100,0.,10.,300,0.,300.);
  fhNonBJetPtFF = new TH2F("hNonBJetPtFF","Non B-jet p_{T} Frag. Fn.",nptbins,ptmin,ptmax,300,0.,300.);
  fhNonBJetEtaPhi = new TH2F("hNonBJetEtaPhi","Non B-jet eta-phi distribution",netabins,etamin,etamax,nphibins,phimin,phimax);

  outputContainer->Add(fhJetType);
  outputContainer->Add(fhBJetXsiFF);
  outputContainer->Add(fhBJetPtFF);
  outputContainer->Add(fhBJetEtaPhi);
  outputContainer->Add(fhNonBJetXsiFF);
  outputContainer->Add(fhNonBJetPtFF);
  outputContainer->Add(fhNonBJetEtaPhi);

  if(IsDataMC()){
    
    fhPtConversion = new TH1F("hPtConversion","Conversion electron pT",nptbins,ptmin,ptmax);
    fhPhiConversion = new TH2F("hPhiConversion","Conversion Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaConversion = new TH2F("hEtaConversion","Conversion Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtBottom = new TH1F("hPtBottom","Bottom electron pT",nptbins,ptmin,ptmax);
    fhPhiBottom = new TH2F("hPhiBottom","Bottom Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaBottom = new TH2F("hEtaBottom","Bottom Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtCharm = new TH1F("hPtCharm","Charm electron pT",nptbins,ptmin,ptmax);
    fhPhiCharm = new TH2F("hPhiCharm","Charm Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaCharm = new TH2F("hEtaCharm","Charm Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtCFromB = new TH1F("hPtCFromB","Charm from Bottom electron pT",nptbins,ptmin,ptmax);
    fhPhiCFromB = new TH2F("hPhiCFromB","Charm from Bottom Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaCFromB = new TH2F("hEtaCFromB","Charm from Bottom Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtDalitz = new TH1F("hPtDalitz","Dalitz electron pT",nptbins,ptmin,ptmax);
    fhPhiDalitz = new TH2F("hPhiDalitz","Dalitz Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaDalitz = new TH2F("hEtaDalitz","Dalitz Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtWDecay = new TH1F("hPtWDecay","W-boson Electron pT",nptbins,ptmin,ptmax);
    fhPhiWDecay = new TH2F("hPhiWDecay","W-boson electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaWDecay = new TH2F("hEtaWDecay","W-boson Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtZDecay = new TH1F("hPtZDecay","Z-boson electron pT",nptbins,ptmin,ptmax);
    fhPhiZDecay = new TH2F("hPhiZDecay","Z-boson Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaZDecay = new TH2F("hEtaZDecay","Z-boson Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtAll = new TH1F("hPtAll","All electron pT",nptbins,ptmin,ptmax);
    fhPhiAll = new TH2F("hPhiAll","All Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaAll = new TH2F("hEtaAll","All Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtUnknown = new TH1F("hPtUnknown","Unknown electron pT",nptbins,ptmin,ptmax);
    fhPhiUnknown = new TH2F("hPhiUnknown","Unknown Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaUnknown = new TH2F("hEtaUnknown","Unknown Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);
    fhPtMisidentified = new TH1F("hPtMisidentified","Misidentified electron pT",nptbins,ptmin,ptmax);
    fhPhiMisidentified = new TH2F("hPhiMisidentified","Misidentified Electron phi vs pT",nptbins,ptmin,ptmax,nphibins,phimin,phimax);
    fhEtaMisidentified = new TH2F("hEtaMisidentified","Misidentified Electron eta vs. eta",nptbins,ptmin,ptmax,netabins,etamin,etamax);

    outputContainer->Add(fhPtConversion);
    outputContainer->Add(fhPhiConversion);
    outputContainer->Add(fhEtaConversion);
    outputContainer->Add(fhPtBottom);
    outputContainer->Add(fhPhiBottom);
    outputContainer->Add(fhEtaBottom);
    outputContainer->Add(fhPtCharm);
    outputContainer->Add(fhPhiCharm);
    outputContainer->Add(fhEtaCharm);
    outputContainer->Add(fhPtCFromB);
    outputContainer->Add(fhPhiCFromB);
    outputContainer->Add(fhEtaCFromB);
    outputContainer->Add(fhPtDalitz);
    outputContainer->Add(fhPhiDalitz);
    outputContainer->Add(fhEtaDalitz);
    outputContainer->Add(fhPtWDecay);
    outputContainer->Add(fhPhiWDecay);
    outputContainer->Add(fhEtaWDecay);
    outputContainer->Add(fhPtZDecay);
    outputContainer->Add(fhPhiZDecay);
    outputContainer->Add(fhEtaZDecay);
    outputContainer->Add(fhPtAll);
    outputContainer->Add(fhPhiAll);
    outputContainer->Add(fhEtaAll);
    outputContainer->Add(fhPtUnknown);
    outputContainer->Add(fhPhiUnknown);
    outputContainer->Add(fhEtaUnknown);
    outputContainer->Add(fhPtMisidentified);
    outputContainer->Add(fhPhiMisidentified);
    outputContainer->Add(fhEtaMisidentified);
    
    //created ele ntuple for further analysis
    if(fWriteNtuple) {
      fMCEleNtuple = new TNtuple("MCEleNtuple","MC Electron Ntuple","mctag:pt:phi:eta:x:y:z");
      outputContainer->Add(fMCEleNtuple) ;
    }

    fhPtMCHadron = new TH1F("hPtMCHadron","MC Charged hadrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCBottom = new TH1F("hPtMCBottom","MC Bottom electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCCharm = new TH1F("hPtMCCharm","MC Charm electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCCFromB = new TH1F("hPtMCCFromB","MC Charm from Bottom electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCConversion = new TH1F("hPtMCConversion","MC Conversion electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCDalitz  = new TH1F("hPtMCDalitz","MC Dalitz electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCWDecay = new TH1F("hPtMCWDecay","MC W Decay electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCZDecay = new TH1F("hPtMCZDecay","MC Z Decay electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);
    fhPtMCUnknown = new TH1F("hPtMCUnknown","MC Unknown electrons w/in EMCAL acceptance",nptbins,ptmin,ptmax);

    outputContainer->Add(fhPtMCHadron);
    outputContainer->Add(fhPtMCBottom);
    outputContainer->Add(fhPtMCCharm);
    outputContainer->Add(fhPtMCCFromB);
    outputContainer->Add(fhPtMCConversion);
    outputContainer->Add(fhPtMCDalitz);
    outputContainer->Add(fhPtMCWDecay);
    outputContainer->Add(fhPtMCZDecay);
    outputContainer->Add(fhPtMCUnknown);

  }//Histos with MC
  
  //Save parameters used for analysis
  TString parList ; //this will be list of parameters used for this analysis.
  char onePar[500] ;
  
  sprintf(onePar,"--- AliAnaElectron ---\n") ;
  parList+=onePar ;	
  sprintf(onePar,"fCalorimeter: %s\n",fCalorimeter.Data()) ;
  parList+=onePar ;  
  sprintf(onePar,"fpOverEmin: %f\n",fpOverEmin) ;
  parList+=onePar ;  
  sprintf(onePar,"fpOverEmax: %f\n",fpOverEmax) ;
  parList+=onePar ;  
  sprintf(onePar,"fResidualCut: %f\n",fResidualCut) ;
  parList+=onePar ;  
  sprintf(onePar,"---DVM Btagging\n");
  parList+=onePar ;
  sprintf(onePar,"max IP-cut (e,h): %f\n",fImpactCut);
  parList+=onePar ;
  sprintf(onePar,"min ITS-hits: %d\n",fITSCut);
  parList+=onePar ;
  sprintf(onePar,"max dR (e,h): %f\n",fDrCut);
  parList+=onePar ;
  sprintf(onePar,"max pairDCA: %f\n",fPairDcaCut);
  parList+=onePar ;
  sprintf(onePar,"max decaylength: %f\n",fDecayLenCut);
  parList+=onePar ;
  sprintf(onePar,"min Associated Pt: %f\n",fAssocPtCut);
  parList+=onePar ;
  sprintf(onePar,"---IPSig Btagging\n");
  parList+=onePar ;
  sprintf(onePar,"min tag track: %d\n",fNTagTrkCut);
  parList+=onePar ;
  sprintf(onePar,"min IP significance: %f\n",fIPSigCut);
  parList+=onePar ;

  //Get parameters set in base class.
  parList += GetBaseParametersList() ;
  
  //Get parameters set in FidutialCut class (not available yet)
  //parlist += GetFidCut()->GetFidCutParametersList() 
  
  TObjString *oString= new TObjString(parList) ;
  outputContainer->Add(oString);
  
  return outputContainer ;
  
}

//____________________________________________________________________________
void AliAnaElectron::Init()
{

  //do some initialization
  if(fCalorimeter == "PHOS") {
    printf("AliAnaElectron::Init() - !!STOP: You want to use PHOS in analysis but this is not (yet) supported!!\n!!Check the configuration file!!\n");
    fCalorimeter = "EMCAL";
  }
  if(fCalorimeter == "EMCAL" && !GetReader()->IsEMCALSwitchedOn()){
    printf("AliAnaElectron::Init() - !!STOP: You want to use EMCAL in analysis but it is not read!!\n!!Check the configuration file!!\n");
    abort();
  }

}


//____________________________________________________________________________
void AliAnaElectron::InitParameters()
{
  
  //Initialize the parameters of the analysis.
  SetOutputAODClassName("AliAODPWG4Particle");
  SetOutputAODName("PWG4Particle");

  AddToHistogramsName("AnaElectron_");

  fCalorimeter = "EMCAL" ;
  fpOverEmin = 0.5;
  fpOverEmax = 1.5;
  fResidualCut = 0.02;
  //DVM B-tagging
  fDrCut       = 1.0; 
  fPairDcaCut  = 0.02;
  fDecayLenCut = 1.0;
  fImpactCut   = 0.5;
  fAssocPtCut  = 1.0;
  fMassCut     = 1.5;
  fSdcaCut     = 0.1;
  fITSCut      = 4;
  //IPSig B-tagging
  fNTagTrkCut  = 2;
  fIPSigCut    = 3.0;
}

//__________________________________________________________________
void  AliAnaElectron::MakeAnalysisFillAOD() 
{
  //
  // Do analysis and fill aods with electron candidates
  // These AODs will be used to do subsequent histogram filling
  //
  // Also fill some QA histograms
  //

  TObjArray *cl = new TObjArray();

  Double_t bfield = 0.;
  if(GetReader()->GetDataType() != AliCaloTrackReader::kMC) bfield = GetReader()->GetBField();

  //Select the calorimeter of the electron
  if(fCalorimeter != "EMCAL") {
    printf("This class not yet implemented for PHOS\n");
    abort();
  }
  cl = GetAODEMCAL();
  
  ////////////////////////////////////////////////
  //Start from tracks and get associated clusters 
  ////////////////////////////////////////////////
  if(!GetAODCTS() || GetAODCTS()->GetEntriesFast() == 0) return ;
  Int_t ntracks = GetAODCTS()->GetEntriesFast();
  Int_t refmult = 0; Int_t refmult2 = 0;
  if(GetDebug() > 0)
    printf("AliAnaElectron::MakeAnalysisFillAOD() - In CTS aod entries %d\n", ntracks);

  //Unfortunately, AliAODTracks don't have associated EMCAL clusters.
  //we have to redo track-matching, I guess
  Int_t iCluster = -999;
  Int_t bt = 0; //counter for event b-tags

  for (Int_t itrk =  0; itrk <  ntracks; itrk++) {////////////// track loop
    iCluster = -999; //start with no match
    AliAODTrack * track = (AliAODTrack*) (GetAODCTS()->At(itrk)) ;
    if (TMath::Abs(track->Eta())< 0.5) refmult++;
    Double_t imp[2] = {-999.,-999.}; Double_t cov[3] = {-999.,-999.,-999.};
    Bool_t dcaOkay = GetDCA(track,imp,cov);  //homegrown dca calculation until AOD is fixed
    if(!dcaOkay) printf("AliAnaElectron::Problem computing DCA to primary vertex for track %d.  Skipping it...\n",itrk);
    if(TMath::Abs(track->Eta())< 0.5 && TMath::Abs(imp[0])<1.0 && TMath::Abs(imp[1])<1.0) refmult2++;
    fhImpactXY->Fill(imp[0]);

    AliAODPid* pid = (AliAODPid*) track->GetDetPid();
    if(pid == 0) {
      if(GetDebug() > 0) printf("AliAnaElectron::MakeAnalysisFillAOD() - No PID object - skipping track %d",itrk);
      continue;
    } else {
      Double_t emcpos[3];
      pid->GetEMCALPosition(emcpos);
      Double_t emcmom[3];
      pid->GetEMCALMomentum(emcmom);
      
      TVector3 pos(emcpos[0],emcpos[1],emcpos[2]);
      TVector3 mom(emcmom[0],emcmom[1],emcmom[2]);
      Double_t tphi = pos.Phi();
      Double_t teta = pos.Eta();
      Double_t tmom = mom.Mag();
      
      TLorentzVector mom2(mom,0.);
      Bool_t in =  GetFidutialCut()->IsInFidutialCut(mom2,fCalorimeter) ;
      if(GetDebug() > 1) printf("AliAnaElectron::MakeAnalysisFillAOD() - Track pt %2.2f, phi %2.2f, eta %2.2f in fidutial cut %d\n",track->Pt(), track->Phi(), track->Eta(), in);
      if(mom.Pt() > GetMinPt() && in) {
	
	Double_t dEdx = pid->GetTPCsignal();
		
	Int_t ntot = cl->GetEntriesFast();
	Double_t res = 999.;
	Double_t pOverE = -999.;
	
	Int_t pidProb = track->GetMostProbablePID();
	if(pidProb == AliAODTrack::kPion || pidProb == AliAODTrack::kKaon || pidProb == AliAODTrack::kProton) fhPtHadron->Fill(track->Pt());
	if(pidProb == AliAODTrack::kElectron) fhPtEleTrkDet->Fill(track->Pt());
	
	Bool_t isElectron = kFALSE;      
	//For tracks in EMCAL acceptance, pair them with all clusters
	//and fill the dEta vs dPhi for these pairs:
	for(Int_t iclus = 0; iclus < ntot; iclus++) {
	  AliAODCaloCluster * clus = (AliAODCaloCluster*) (cl->At(iclus));
	  if(!clus) continue;
	  
	  Double_t x[3];
	  clus->GetPosition(x);
	  TVector3 cluspos(x[0],x[1],x[2]);
	  Double_t deta = teta - cluspos.Eta();
	  Double_t dphi = tphi - cluspos.Phi();
	  if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
	  if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
	  fh2dEtadPhi->Fill(deta,dphi);
	  fh2TrackPVsClusterE->Fill(clus->E(),track->P());
	  fh2TrackPtVsClusterE->Fill(clus->E(),track->Pt());
	  fh2TrackPhiVsClusterPhi->Fill(cluspos.Phi(),mom.Phi());
	  fh2TrackEtaVsClusterEta->Fill(cluspos.Eta(),mom.Eta());
	  
	  res = sqrt(dphi*dphi + deta*deta);
	  fh1dR->Fill(res);
	  
	  /////////////////////////////////
	  //Perform electron cut analysis//
	  /////////////////////////////////
	  //Good match
	  if(res < fResidualCut) {
	    fh2dEtadPhiMatched->Fill(deta,dphi);
	    iCluster = iclus;
	    
	    Int_t tmctag = -1;
	    Int_t cmctag = -1;
	    
	    if(IsDataMC()) {
	      //Input from second AOD?
	      Int_t input = 0;
	      if(GetReader()->GetAODCTSNormalInputEntries() <= itrk) input = 1;
	      tmctag = GetMCAnalysisUtils()->CheckOrigin(track->GetLabel(),GetReader(),input);
	      
	      //Do you want the cluster or the track label?
	      input = 0;
	      if(GetReader()->GetAODEMCALNormalInputEntries() <= iclus) input = 1;
	      cmctag = GetMCAnalysisUtils()->CheckOrigin(clus->GetLabel(0),GetReader(),input);
	    }
	    
	    if(fWriteNtuple) {
	      fEleNtuple->Fill(tmctag,cmctag,track->Pt(),track->Phi(),track->Eta(),track->P(),clus->E(),deta,dphi,clus->GetNCells(),dEdx,pidProb,imp[0],imp[1]);
	    }
	    
	    fh2MatchdEdx->Fill(track->P(),dEdx);
	    
	    Double_t energy = clus->E(); 
	    if(energy > 0) pOverE = tmom/energy;
	    fh1pOverE->Fill(pOverE);
	    
	    Int_t mult = clus->GetNCells();
	    if(mult < 2 &&  GetDebug() > 0) printf("Single digit cluster.\n");
	    
	    //////////////////////////////
	    //Electron cuts happen here!//
	    //////////////////////////////
	    if(pOverE > fpOverEmin && pOverE < fpOverEmax) isElectron = kTRUE;
	  } else {
	    fh2dEtadPhiUnmatched->Fill(deta,dphi);
	  }
	  
	} //calocluster loop
	
	///////////////////////////
	//Fill AOD with electrons//
	///////////////////////////
	if(isElectron) {
	  
	  //B-tagging
	  if(GetDebug() > 1) printf("Found Electron - do b-tagging\n");
	  Int_t dvmbtag = GetDVMBtag(track); bt += dvmbtag;

	  fh2EledEdx->Fill(track->P(),dEdx);
	  
	  Double_t eMass = 0.511/1000; //mass in GeV
	  Double_t eleE = sqrt(track->P()*track->P() + eMass*eMass);
	  AliAODPWG4Particle tr = AliAODPWG4Particle(track->Px(),track->Py(),track->Pz(),eleE);
	  tr.SetLabel(track->GetLabel());
	  tr.SetCaloLabel(iCluster,-1); //sets the indices of the original caloclusters
	  tr.SetTrackLabel(itrk,-1); //sets the indices of the original tracks
	  tr.SetDetector(fCalorimeter);
	  if(GetReader()->GetAODCTSNormalInputEntries() <= itrk) tr.SetInputFileIndex(1);
	  //Make this preserve sign of particle
	  if(track->Charge() < 0) tr.SetPdg(11); //electron is 11
	  else  tr.SetPdg(-11); //positron is -11
	  Int_t btag = -1;
	  if(dvmbtag > 0) tr.SetBTagBit(btag,tr.kDVMTag0);
	  if(dvmbtag > 1) tr.SetBTagBit(btag,tr.kDVMTag1);
	  if(dvmbtag > 2) tr.SetBTagBit(btag,tr.kDVMTag2);
	  tr.SetBtag(btag);
	  
	  //Play with the MC stack if available
	  //Check origin of the candidates
	  if(IsDataMC()){
	    
	    //FIXME:  Need to re-think this for track-oriented analysis
	    //JLK DO WE WANT TRACK TAG OR CLUSTER TAG?
	    tr.SetTag(GetMCAnalysisUtils()->CheckOrigin(tr.GetLabel(),GetReader(),tr.GetInputFileIndex()));
	    
	    if(GetDebug() > 0) printf("AliAnaElectron::MakeAnalysisFillAOD() - Origin of candidate %d\n",tr.GetTag());
	  }//Work with stack also   
	  
	  AddAODParticle(tr);
	  
	  if(GetDebug() > 1) printf("AliAnaElectron::MakeAnalysisFillAOD() - Electron selection cuts passed: pT %3.2f, pdg %d\n",tr.Pt(),tr.GetPdg());	
	}//electron
      }//pt, fiducial selection
    }//pid check
  }//track loop                         
  
  //FIXME:  Should we also check from the calocluster side, just in
  //case?
  fhRefMult->Fill(refmult);
  fhRefMult2->Fill(refmult2);

  if(GetDebug() > 1 && bt > 0) printf("AliAnaElectron::MakeAnalysisFillAOD() *** Event Btagged *** \n");
  if(GetDebug() > 1) printf("AliAnaElectron::MakeAnalysisFillAOD()  End fill AODs \n");  
  
}

//__________________________________________________________________
void  AliAnaElectron::MakeAnalysisFillHistograms() 
{
  //Do analysis and fill histograms

  AliStack * stack = 0x0;
  TParticle * primary = 0x0;
  TClonesArray * mcparticles0 = 0x0;
  TClonesArray * mcparticles1 = 0x0;
  AliAODMCParticle * aodprimary = 0x0;

  Int_t ph1 = 0;  //photonic 1 count
  Int_t ph2 = 0;  //photonic 2 count
  Int_t phB = 0;  //both count

  if(IsDataMC()) {
    if(GetReader()->ReadStack()){
      stack =  GetMCStack() ;
      
      if(!stack)
	printf("AliAnaElectron::MakeAnalysisFillHistograms() *** no stack ***: \n");
      
    }
    else if(GetReader()->ReadAODMCParticles()){
      //Get the list of MC particles
      mcparticles0 = GetReader()->GetAODMCParticles(0);
      if(!mcparticles0 && GetDebug() > 0)     {
	printf("AliAnaElectron::MakeAnalysisFillHistograms() -  Standard MCParticles not available!\n");
      }
      if(GetReader()->GetSecondInputAODTree()){
	mcparticles1 = GetReader()->GetAODMCParticles(1);
	if(!mcparticles1 && GetDebug() > 0)     {
	  printf("AliAnaElectron::MakeAnalysisFillHistograms() -  Second input MCParticles not available!\n");
	}
      }
      
    }
  }// is data and MC

  ////////////////////////////////////
  //Loop over jets and check for b-tag
  ////////////////////////////////////
  Int_t njets = (GetReader()->GetOutputEvent())->GetNJets();
  if(njets > 0) {
    if(GetDebug() > 0) printf("AliAnaElectron::MakeAnalysisFillHistograms() - Jet AOD branch has %d jets.  Performing b-jet tag analysis",njets);
    
    for(Int_t ijet = 0; ijet < njets ; ijet++) {
      AliAODJet * jet = (AliAODJet*)(GetReader()->GetOutputEvent())->GetJet(ijet) ;
      if(GetDebug() > 3) {
	printf("AliAODJet ijet = %d\n",ijet);
	jet->Print("");
      }
      //To "tag" the jet, we will look for it to pass our various criteria
      //For e jet tag, we just look to see which ones have NPEs
      //For DVM jet tag, we will look for DVM electrons
      //For IPSig, we compute the IPSig for all tracks and if the
      //number passing is above the cut, it passes
      Bool_t eJet = kFALSE;  
      Bool_t dvmJet = kFALSE;  
      Bool_t ipsigJet = kFALSE;
      TRefArray* rt = jet->GetRefTracks();
      Int_t ntrk = rt->GetEntries();
      Int_t trackCounter = 0; //for ipsig
      for(Int_t itrk = 0; itrk < ntrk; itrk++) {
	AliAODTrack* jetTrack = (AliAODTrack*)jet->GetTrack(itrk);
	if( GetIPSignificance(jetTrack, jet->Phi()) > fIPSigCut) trackCounter++;
	Bool_t isNPE = CheckTrack(jetTrack,"NPE");
	if(isNPE) eJet = kTRUE;
	Bool_t isDVM = CheckTrack(jetTrack,"DVM");
	if(isDVM) dvmJet = kTRUE;
      }
      fhIPSigBtagQA1->Fill(trackCounter);
      if(trackCounter > fNTagTrkCut) ipsigJet = kTRUE;

      //Fill bjet histograms here
      if(!(eJet || ipsigJet || dvmJet)) fhJetType->Fill(0.,jet->Pt()); //none
      if(eJet && !(ipsigJet || dvmJet)) fhJetType->Fill(1.,jet->Pt()); //only ejet
      if(dvmJet && !(eJet || ipsigJet)) fhJetType->Fill(2.,jet->Pt()); //only dvm
      if(ipsigJet && !(eJet || dvmJet)) fhJetType->Fill(3.,jet->Pt()); //only ipsig
      if(eJet && dvmJet && !ipsigJet)   fhJetType->Fill(4.,jet->Pt()); //ejet & dvm
      if(eJet && ipsigJet && !dvmJet)   fhJetType->Fill(5.,jet->Pt()); //ejet & ipsig
      if(dvmJet && ipsigJet && !eJet)   fhJetType->Fill(6.,jet->Pt()); //dvm & ipsig
      if(dvmJet && ipsigJet && eJet)    fhJetType->Fill(7.,jet->Pt()); //all

      for(Int_t itrk = 0; itrk < ntrk; itrk++) {
	AliAODTrack* jetTrack = (AliAODTrack*)jet->GetTrack(itrk);
	Double_t xsi = TMath::Log(jet->Pt()/jetTrack->Pt());
	if(eJet || ipsigJet || dvmJet) {
	  if(GetDebug() > 0) printf("AliAnaElectron::MakeAnalysisFillHistograms - We have a bjet!\n");

	  fhBJetXsiFF->Fill(xsi,jet->Pt());
	  fhBJetPtFF->Fill(jetTrack->Pt(),jet->Pt());
	  fhBJetEtaPhi->Fill(jet->Eta(),jet->Phi());
	} else {
	  //Fill non-bjet histograms here
	  fhNonBJetXsiFF->Fill(xsi,jet->Pt());
	  fhNonBJetPtFF->Fill(jetTrack->Pt(),jet->Pt());
	  fhNonBJetEtaPhi->Fill(jet->Eta(),jet->Phi());
	}
      }
    } //jet loop
  } //jets exist
  
  //////////////////////////////
  //Loop on stored AOD electrons
  //////////////////////////////
  Int_t naod = GetOutputAODBranch()->GetEntriesFast();
  if(GetDebug() > 0) printf("AliAnaElectron::MakeAnalysisFillHistograms() - aod branch entries %d\n", naod);
  
  for(Int_t iaod = 0; iaod < naod ; iaod++){
    AliAODPWG4Particle* ele =  (AliAODPWG4Particle*) (GetOutputAODBranch()->At(iaod));
    Int_t pdg = ele->GetPdg();
    
    if(GetDebug() > 3) 
      printf("AliAnaElectron::MakeAnalysisFillHistograms() - PDG %d, MC TAG %d, Calorimeter %s\n", ele->GetPdg(),ele->GetTag(), (ele->GetDetector()).Data()) ;
    
    if(TMath::Abs(pdg) != AliCaloPID::kElectron) continue; 
    if(ele->GetDetector() != fCalorimeter) continue;
    
    if(GetDebug() > 1) 
      printf("AliAnaElectron::MakeAnalysisFillHistograms() - ID Electron: pt %f, phi %f, eta %f\n", ele->Pt(),ele->Phi(),ele->Eta()) ;

    //Filter for photonic electrons based on opening angle and Minv
    //cuts, also fill histograms
    Bool_t photonic = kFALSE;
    Bool_t photonic1 = kFALSE;
    photonic1 = IsItPhotonic(ele); //check against primaries
    if(photonic1) ph1++;
    Bool_t photonic2 = kFALSE;
    photonic2 = IsItPhotonic2(ele); //check against V0s
    if(photonic2) ph2++;
    if(photonic1 && photonic2) phB++;
    if(photonic1 || photonic2) photonic = kTRUE;

    //Fill electron histograms 
    Float_t ptele = ele->Pt();
    Float_t phiele = ele->Phi();
    Float_t etaele = ele->Eta();
    
    fhPtElectron  ->Fill(ptele);
    fhPhiElectron ->Fill(ptele,phiele);
    fhEtaElectron ->Fill(ptele,etaele);

    if(photonic) {
      fhPtPE->Fill(ptele);
      fhPhiPE->Fill(ptele,phiele);
      fhEtaPE->Fill(ptele,etaele);
    } else {
      fhPtNPE->Fill(ptele);
      fhPhiNPE->Fill(ptele,phiele);
      fhEtaNPE->Fill(ptele,etaele);
    }

    if(IsDataMC()){
      Int_t tag = ele->GetTag();
      if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCElectron)) {
	fhPtAll  ->Fill(ptele);
	fhPhiAll ->Fill(ptele,phiele);
	fhEtaAll ->Fill(ptele,etaele);	  

	if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCConversion)){
	  fhPtConversion  ->Fill(ptele);
	  fhPhiConversion ->Fill(ptele,phiele);
	  fhEtaConversion ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCEFromB)){
	  fhPtBottom  ->Fill(ptele);
	  fhPhiBottom ->Fill(ptele,phiele);
	  fhEtaBottom ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCEFromC) && !GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCEFromB)){
	  fhPtCharm  ->Fill(ptele);
	  fhPhiCharm ->Fill(ptele,phiele);
	  fhEtaCharm ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCEFromCFromB)){
	  fhPtCFromB  ->Fill(ptele);
	  fhPhiCFromB ->Fill(ptele,phiele);
	  fhEtaCFromB ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCPi0Decay) || GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCEtaDecay) || GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCOtherDecay)) {
	  fhPtDalitz  ->Fill(ptele);
	  fhPhiDalitz ->Fill(ptele,phiele);
	  fhEtaDalitz ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCWDecay)) {
	  fhPtWDecay  ->Fill(ptele);
	  fhPhiWDecay ->Fill(ptele,phiele);
	  fhEtaWDecay ->Fill(ptele,etaele);
	}
	else if(GetMCAnalysisUtils()->CheckTagBit(tag,AliMCAnalysisUtils::kMCZDecay)) {
	  fhPtZDecay  ->Fill(ptele);
	  fhPhiZDecay ->Fill(ptele,phiele);
	  fhEtaZDecay ->Fill(ptele,etaele);
	} 
	else {
	  fhPtUnknown  ->Fill(ptele);
	  fhPhiUnknown ->Fill(ptele,phiele);
	  fhEtaUnknown ->Fill(ptele,etaele);
	}
	//not electron:
      } else {
	fhPtMisidentified  ->Fill(ptele);
        fhPhiMisidentified ->Fill(ptele,phiele);
        fhEtaMisidentified ->Fill(ptele,etaele);
      }
    }//Histograms with MC
    
  }// aod loop

  ////////////////////////////////////////////////////////
  //Fill histograms of pure MC kinematics from the stack//
  ////////////////////////////////////////////////////////
  if(IsDataMC()) {
    if(GetReader()->ReadStack()) {
      for(Int_t ipart = 0; ipart < stack->GetNtrack(); ipart++) {
	primary = stack->Particle(ipart);
	TLorentzVector mom;
	primary->Momentum(mom);
	Bool_t in = GetFidutialCut()->IsInFidutialCut(mom,fCalorimeter);
	if(primary->Pt() < GetMinPt()) continue;
	if(!in) continue;

	Int_t pdgcode = primary->GetPdgCode();
	if(TMath::Abs(pdgcode) == 211 || TMath::Abs(pdgcode) == 321 || TMath::Abs(pdgcode) == 2212)
	  fhPtMCHadron->Fill(primary->Pt());

	//we only care about electrons
	if(TMath::Abs(pdgcode) != 11) continue;
	//we only want TRACKABLE electrons (TPC 85-250cm)
	if(primary->R() > 200.) continue;
	//Ignore low pt electrons
	if(primary->Pt() < 0.2) continue;
	
	//find out what the ancestry of this electron is
	Int_t mctag = -1;
	Int_t input = 0;
	mctag = GetMCAnalysisUtils()->CheckOrigin(ipart,GetReader(),input);

	//fill ntuple
	if(fWriteNtuple) {
	  fMCEleNtuple->Fill(mctag,primary->Pt(),primary->Phi(),primary->Eta(),primary->Vx(),primary->Vy(),primary->Vz());
	}
	if(!GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCConversion)) {
	  if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromB))
	    fhPtMCBottom->Fill(primary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromC))
	    fhPtMCCharm->Fill(primary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromCFromB))
	    fhPtMCCFromB->Fill(primary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCWDecay))
	    fhPtMCWDecay->Fill(primary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCZDecay))
	    fhPtMCZDecay->Fill(primary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCPi0Decay) 
		  || GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEtaDecay)
		  || GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCOtherDecay))
	    fhPtMCDalitz->Fill(primary->Pt());
	  else 
	    fhPtMCUnknown->Fill(primary->Pt());
	} else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCConversion)) {
	  fhPtMCConversion->Fill(primary->Pt());
	}
      }      
    } else if(GetReader()->ReadAODMCParticles()) {
      Int_t npart0 = mcparticles0->GetEntriesFast();
      Int_t npart1 = 0;
      if(mcparticles1) npart1 = mcparticles1->GetEntriesFast();
      Int_t npart = npart0+npart1;
      for(Int_t ipart = 0; ipart < npart; ipart++) {
	if(ipart < npart0) aodprimary = (AliAODMCParticle*)mcparticles0->At(ipart);
	else aodprimary = (AliAODMCParticle*)mcparticles1->At(ipart-npart0);
	if(!aodprimary) {
	  printf("AliAnaElectron::MakeAnalysisFillHistograms() *** no primary ***:  label %d \n", ipart);
	  continue;
	}

	Double_t mom[3] = {0.,0.,0.};
	aodprimary->PxPyPz(mom);
	TLorentzVector mom2(mom,0.);	
	Bool_t in = GetFidutialCut()->IsInFidutialCut(mom2,fCalorimeter);
	if(aodprimary->Pt() < GetMinPt()) continue;
	if(!in) continue;

	Int_t pdgcode = aodprimary->GetPdgCode();
	if(TMath::Abs(pdgcode) == 211 || TMath::Abs(pdgcode) == 321 || TMath::Abs(pdgcode) == 2212)
	  fhPtMCHadron->Fill(aodprimary->Pt());

	//we only care about electrons
	if(TMath::Abs(pdgcode) != 11) continue;
	//we only want TRACKABLE electrons (TPC 85-250cm)
	Double_t radius = TMath::Sqrt(aodprimary->Xv()*aodprimary->Xv() + aodprimary->Yv()*aodprimary->Yv());
	if(radius > 200.) continue;

	//find out what the ancestry of this electron is
	Int_t mctag = -1;
	Int_t input = 0;
	Int_t ival = ipart;
	if(ipart > npart0) { ival -= npart0; input = 1;}
	mctag = GetMCAnalysisUtils()->CheckOrigin(ival,GetReader(),input);
	
	//fill ntuple
	if(fWriteNtuple) {
	  fMCEleNtuple->Fill(mctag,aodprimary->Pt(),aodprimary->Phi(),aodprimary->Eta(),aodprimary->Xv(),aodprimary->Yv(),aodprimary->Zv());
	}
	if(!GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCConversion)) {
	  if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromB))
	    fhPtMCBottom->Fill(aodprimary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromC))
	    fhPtMCCharm->Fill(aodprimary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEFromCFromB))
	    fhPtMCCFromB->Fill(aodprimary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCWDecay))
	    fhPtMCWDecay->Fill(aodprimary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCZDecay))
	    fhPtMCZDecay->Fill(aodprimary->Pt());
	  else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCPi0Decay) 
		  || GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCEtaDecay)
		  || GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCOtherDecay))
	    fhPtMCDalitz->Fill(aodprimary->Pt());
	  else 
	    fhPtMCUnknown->Fill(aodprimary->Pt());
	} else if(GetMCAnalysisUtils()->CheckTagBit(mctag,AliMCAnalysisUtils::kMCConversion)) {
	  fhPtMCConversion->Fill(aodprimary->Pt());
	}	
      }
    }
  } //pure MC kine histos
    
  //if(GetDebug() > 0) 
    printf("\tAliAnaElectron::Photonic electron counts: ph1 %d, ph2 %d, Both %d\n",ph1,ph2,phB);
}

//__________________________________________________________________
Int_t AliAnaElectron::GetDVMBtag(AliAODTrack * tr )
{
  //This method uses the Displaced Vertex between electron-hadron
  //pairs and the primary vertex to determine whether an electron is
  //likely from a B hadron.

  Int_t ncls1 = 0;
  for(Int_t l = 0; l < 6; l++) if(TESTBIT(tr->GetITSClusterMap(),l)) ncls1++;

  fhDVMBtagQA3->Fill(ncls1);
  if (ncls1 < fITSCut) return 0;

  Double_t imp[2] = {-999.,-999.}; Double_t cov[3] = {-999.,-999.,-999.};
  Bool_t dcaOkay = GetDCA(tr,imp,cov);  //homegrown dca calculation until AOD is fixed                  
  if(!dcaOkay) {
    printf("AliAnaElectron::Problem computing DCA to primary vertex for track %d",tr->GetID());
    return 0;
  }

  fhDVMBtagQA4->Fill(imp[0]);
  if (TMath::Abs(imp[0])   > fImpactCut ) return 0;
  fhDVMBtagQA5->Fill(imp[1]);
  if (TMath::Abs(imp[1])   > fImpactCut ) return 0;

  Int_t nvtx1 = 0;
  Int_t nvtx2 = 0;
  Int_t nvtx3 = 0;

  for (Int_t k2 =0; k2 < GetAODCTS()->GetEntriesFast() ; k2++) {
    //loop over assoc
    AliAODTrack* track2 = (AliAODTrack*) (GetAODCTS()->At(k2));
    Int_t id1 = tr->GetID();
    Int_t id2 = track2->GetID();
    if(id1 == id2) continue;

    Int_t ncls2 = 0;
    for(Int_t l = 0; l < 6; l++) if(TESTBIT(track2->GetITSClusterMap(),l)) ncls2++;
    if (ncls2 < fITSCut) continue;

    if(track2->Pt() < fAssocPtCut) continue;

    Double_t dphi = tr->Phi() - track2->Phi();
    if(dphi > TMath::Pi()) dphi -= 2*TMath::Pi();
    if(dphi < -TMath::Pi()) dphi += 2*TMath::Pi();
    Double_t deta = tr->Eta() - track2->Eta();
    Double_t dr = sqrt(deta*deta + dphi*dphi);

    if(dr > fDrCut) continue;
    
    Double_t sDca1 = ComputeSignDca(tr, track2, 1.0);
    if (sDca1 > fSdcaCut) nvtx1++;
    Double_t sDca2 = ComputeSignDca(tr, track2, 1.5);
    if (sDca2 > fSdcaCut) nvtx2++;
    Double_t sDca3 = ComputeSignDca(tr, track2, 1.8);
    if (sDca3 > fSdcaCut) nvtx3++;

  } //loop over hadrons

  if(GetDebug() > 0) {
    if (nvtx1>0) printf("result1 of btagging: %d \n",nvtx1);
    if (nvtx2>0) printf("result2 of btagging: %d \n",nvtx2);
    if (nvtx3>0) printf("result3 of btagging: %d \n",nvtx3);
  }

  //fill QA histograms
  fhDVMBtagCut1->Fill(nvtx1,tr->Pt());
  fhDVMBtagCut2->Fill(nvtx2,tr->Pt());
  fhDVMBtagCut3->Fill(nvtx3,tr->Pt());

  return nvtx2;

}

//__________________________________________________________________
Double_t AliAnaElectron::ComputeSignDca(AliAODTrack *tr, AliAODTrack *tr2 , float masscut)
{
  //Compute the signed dca between two tracks
  //and return the result

  Double_t signDca=-999.;
  if(GetDebug() > 2 ) printf(">>ComputeSdca:: track1 %d, track2 %d, masscut %f \n", tr->GetLabel(), tr2->GetLabel(), masscut);

  //=====Now calculate DCA between both tracks=======  
  Double_t massE = 0.000511;
  Double_t massK = 0.493677;

  Double_t bfield = 5.; //kG
  if(GetReader()->GetDataType() != AliCaloTrackReader::kMC) bfield = GetReader()->GetBField();

  Double_t vertex[3] = {-999.,-999.,-999}; //vertex
  if(GetReader()->GetDataType() != AliCaloTrackReader::kMC) {
    GetReader()->GetVertex(vertex); //If only one file, get the vertex from there
    //FIXME:  Add a check for whether file 2 is PYTHIA or HIJING
    //If PYTHIA, then set the vertex from file 2, if not, use the
    //vertex from file 1
    if(GetReader()->GetSecondInputAODTree()) GetReader()->GetSecondInputAODVertex(vertex);
  }
  
  TVector3 primV(vertex[0],vertex[1],vertex[2]) ;

  if(GetDebug() > 5) printf(">>ComputeSdca:: primary vertex = %2.2f,%2.2f,%2.2f \n",vertex[0],vertex[1],vertex[2]) ;

  AliExternalTrackParam *param1 = new AliExternalTrackParam(tr);
  AliExternalTrackParam *param2 = new AliExternalTrackParam(tr2);

  Double_t xplane1 = 0.; Double_t xplane2 = 0.;
  Double_t pairdca = param1->GetDCA(param2,bfield,xplane1,xplane2);

  Int_t id1 = 0, id2 = 0;
  AliESDv0 bvertex(*param1,id1,*param2,id2);
  Double_t vx,vy,vz;
  bvertex.GetXYZ(vx,vy,vz);

  Double_t emom[3];
  Double_t hmom[3];
  param1->PxPyPz(emom);
  param2->PxPyPz(hmom);
  TVector3 emomAtB(emom[0],emom[1],emom[2]);
  TVector3 hmomAtB(hmom[0],hmom[1],hmom[2]);
  TVector3 secvtxpt(vx,vy,vz);
  TVector3 decayvector(0,0,0);
  decayvector = secvtxpt - primV; //decay vector from PrimVtx
  Double_t decaylength = decayvector.Mag();

  if(GetDebug() > 0) {
    printf(">>ComputeSdca:: mom1=%f, mom2=%f \n", emomAtB.Perp(), hmomAtB.Perp() );
    printf(">>ComputeSdca:: pairDCA=%f, length=%f \n", pairdca,decaylength );
  }

  if (masscut<1.1) fhDVMBtagQA1->Fill(pairdca,decaylength);

  if (emomAtB.Mag()>0 && pairdca < fPairDcaCut && decaylength < fDecayLenCut ) {
    TVector3 sumMom = emomAtB+hmomAtB;
    Double_t ener1 = sqrt(pow(emomAtB.Mag(),2) + massE*massE);
    Double_t ener2 = sqrt(pow(hmomAtB.Mag(),2) + massK*massK);
    Double_t ener3 = sqrt(pow(hmomAtB.Mag(),2) + massE*massE);
    Double_t mass = sqrt(pow((ener1+ener2),2) - pow(sumMom.Mag(),2));
    Double_t massPhot = sqrt(pow((ener1+ener3),2) - pow(sumMom.Mag(),2));
    Double_t sDca = decayvector.Dot(emomAtB)/emomAtB.Mag();

    if (masscut<1.1) fhDVMBtagQA2->Fill(sDca, mass);

    if (mass > masscut && massPhot > 0.1) signDca = sDca;
    
    if(GetDebug() > 0) printf("\t>>ComputeSdca:: mass=%f \n", mass);
    if(GetDebug() > 0) printf("\t>>ComputeSdca:: sec vtx-signdca :%f\n",signDca);
  }

  //clean up
  delete param1;
  delete param2;

  return signDca;
}

//__________________________________________________________________
Double_t AliAnaElectron::GetIPSignificance(AliAODTrack *tr, Double_t jetPhi)
{
  //get signed impact parameter significance of the given AOD track
  //for the given jet
  Int_t trackIndex = 0;
  for (int k2 =0; k2 < GetAODCTS()->GetEntriesFast() ; k2++) {
    //loop over assoc
    AliAODTrack* track2 = (AliAODTrack*) (GetAODCTS()->At(k2));
    int id1 = tr->GetID();
    int id2 = track2->GetID();
    if(id1 == id2) {
      trackIndex = k2;//FIXME: check if GetAODCTS stores tracks in the
		      //same order of the event
      break;
    }
  }
  
  Double_t significance=0;
  Double_t magField = 0;
  Double_t maxD = 10000.;
  Double_t impPar[] = {0,0};
  Double_t ipCov[]={0,0,0};
  Double_t ipVec2D[] = {0,0};

  AliVEvent* vEvent = (AliVEvent*)GetReader()->GetInputEvent();
  if(!vEvent) return -97;
  AliVVertex* vv = (AliVVertex*)vEvent->GetPrimaryVertex();
  if(!vv) return -98;
  AliVTrack* vTrack = (AliVTrack*)vEvent->GetTrack(trackIndex);
  if(!vTrack) return -99;
  AliESDtrack esdTrack(vTrack);
  if(!esdTrack.PropagateToDCA(vv, magField, maxD, impPar, ipCov)) return -100;
  if(ipCov[0]<0) return -101;

  Double_t Pxy[] = {esdTrack.Px(), esdTrack.Py()};
  Double_t Txy[] = {esdTrack.Xv(), esdTrack.Yv()};
  Double_t Vxy[] = {vv->GetX(),  vv->GetY()};
  GetImpactParamVect(Pxy, Txy, Vxy, ipVec2D);
  Double_t phiIP = TMath::ATan2(ipVec2D[1], ipVec2D[0]) + (fabs(ipVec2D[1])-ipVec2D[1])/fabs(ipVec2D[1])*TMath::Pi();
  Double_t cosTheta = TMath::Cos(jetPhi - phiIP);
  Double_t sign = cosTheta/TMath::Abs(cosTheta);
  significance = TMath::Abs(impPar[0])/TMath::Sqrt(ipCov[0])*sign;
  //ip = fabs(impPar[0]);
  fhIPSigBtagQA2->Fill(significance);
  return significance;
}

//__________________________________________________________________
void AliAnaElectron::GetImpactParamVect(Double_t Pxy[2], Double_t Txy[2], Double_t Vxy[2], Double_t IPxy[2])
{
  //px,py: momentum components at the origin of the track; tx, ty:
  //origin (x,y) of track; vx, vy: coordinates of primary vertex
  // analytical geometry auxiliary variables
  Double_t mr = Pxy[1]/Pxy[0]; //angular coeficient of the straight
			      //line that lies on top of track
			      //momentum
  Double_t br = Txy[1] - mr*Txy[0]; //linear coeficient of the straight
				   //line that lies on top of track
				   //momentum
  Double_t ms = -1./mr; //angular coeficient of the straight line that
		       //lies on top of the impact parameter line
  //  Double_t bs = Vxy[1] - ms*Vxy[0]; //linear coeficient of the straight
				   //line that lies on top of the
				   //impact parameter line 
  Double_t xIntersection = (mr*Txy[0] - ms*Vxy[0] + Vxy[1] - Txy[1])/(mr - ms);
  Double_t yIntersection = mr*xIntersection + br;
  //if(ceil(10000*yIntersection) - ceil(10000*(ms*xIntersection + bs))
  //!= 0 )cout<<yIntersection<<", "<<ms*xIntersection + bs<<endl;
  IPxy[0] = xIntersection - Vxy[0];
  IPxy[1] = yIntersection - Vxy[1];
  return;
}

//__________________________________________________________________
Bool_t AliAnaElectron::IsItPhotonic(const AliAODPWG4Particle* part) 
{
  //This method checks the opening angle and invariant mass of
  //electron pairs within the AliAODPWG4Particle list to see if 
  //they are likely to be photonic electrons

  Bool_t itIS = kFALSE;

  Double_t massE = 0.000511;
  Double_t bfield = 5.; //kG
  if(GetReader()->GetDataType() != AliCaloTrackReader::kMC) bfield = GetReader()->GetBField();

  Int_t pdg1 = part->GetPdg();
  Int_t trackId = part->GetTrackLabel(0);
  AliAODTrack* track = (AliAODTrack*)GetAODCTS()->At(trackId);
  if(!track) {
    if(GetDebug() > 0) printf("AliAnaElectron::IsItPhotonic - can't get the AOD Track from the particle!  Skipping the photonic check");
    return kFALSE; //Don't proceed because we can't get the track
  }

  AliExternalTrackParam *param1 = new AliExternalTrackParam(track);

  //Loop on stored AOD electrons and compute the angle differences and Minv
  for (Int_t k2 =0; k2 < GetOutputAODBranch()->GetEntriesFast() ; k2++) {
    AliAODPWG4Particle* part2 = (AliAODPWG4Particle*) GetOutputAODBranch()->At(k2);
    Int_t track2Id = part2->GetTrackLabel(0);
    if(trackId == track2Id) continue;
    Int_t pdg2 = part2->GetPdg();
    if(TMath::Abs(pdg2) != AliCaloPID::kElectron) continue;
    if(part2->GetDetector() != fCalorimeter) continue;

    //JLK: Check opp. sign pairs only
    if(pdg1*pdg2 > 0) continue; //skip same-sign pairs

    //propagate to common vertex and check opening angle
    AliAODTrack* track2 = (AliAODTrack*)GetAODCTS()->At(track2Id);
    if(!track2) {
      if(GetDebug() >0) printf("AliAnaElectron::IsItPhotonic - problem getting the partner track.  Continuing on to the next one");
      continue;
    }
    AliExternalTrackParam *param2 = new AliExternalTrackParam(track2);
    Int_t id1 = 0, id2 = 0;
    AliESDv0 photonVtx(*param1,id1,*param2,id2);
    Double_t vx,vy,vz;
    photonVtx.GetXYZ(vx,vy,vz);

    Double_t p1mom[3];
    Double_t p2mom[3];
    param1->PxPyPz(p1mom);
    param2->PxPyPz(p2mom);

    TVector3 p1momAtB(p1mom[0],p1mom[1],p1mom[2]);
    TVector3 p2momAtB(p2mom[0],p2mom[1],p2mom[2]);
    TVector3 sumMom = p1momAtB+p2momAtB;

    Double_t ener1 = sqrt(pow(p1momAtB.Mag(),2) + massE*massE);
    Double_t ener2 = sqrt(pow(p2momAtB.Mag(),2) + massE*massE);
    Double_t mass = sqrt(pow((ener1+ener2),2) - pow(sumMom.Mag(),2));

    Double_t dphi = p1momAtB.DeltaPhi(p2momAtB);
    fh1OpeningAngle->Fill(dphi);
    fh1MinvPhoton->Fill(mass);

    if(mass < 0.1) {
      if(GetDebug() > 0) printf("######PROBABLY A PHOTON\n");
      itIS = kTRUE;
    }

    //clean up
    delete param2;

  }

  delete param1;
  return itIS;

}

//__________________________________________________________________
Bool_t AliAnaElectron::IsItPhotonic2(const AliAODPWG4Particle* part) 
{
  //This method checks to see whether a track that has been flagged as
  //an electron was determined to match to a V0 candidate with
  //invariant mass consistent with photon conversion

  Bool_t itIS = kFALSE;
  Int_t id = part->GetTrackLabel(0);
  
  //---Get V0s---
  AliAODEvent *aod = (AliAODEvent*) GetReader()->GetInputEvent();
  int nv0s = aod->GetNumberOfV0s();
  for (Int_t iV0 = 0; iV0 < nv0s; iV0++) {
    AliAODv0 *v0 = aod->GetV0(iV0);
    if (!v0) continue;
    double radius = v0->RadiusV0();
    double mass = v0->InvMass2Prongs(0,1,11,11);
    if(GetDebug() > 0) {
      printf("## IsItPhotonic2() :: v0: %d, radius: %f \n", iV0 , radius );
      printf("## IsItPhotonic2() :: neg-id: %d, pos-id: %d \n", v0->GetNegID(), v0->GetPosID() );
      printf("## IsItPhotonic2() :: Minv(e,e): %f \n", v0->InvMass2Prongs(0,1,11,11) );
    }
    if (mass < 0.100) {
      if ( id == v0->GetNegID() || id == v0->GetPosID()) {
	itIS=kTRUE;
	printf("## IsItPhotonic2() :: It's a conversion electron!!! \n" );
      }
    } }
  return itIS;

}

//__________________________________________________________________
Bool_t AliAnaElectron::GetDCA(const AliAODTrack* track,Double_t impPar[2], Double_t cov[3]) 
{
  //Use the Event vertex and AOD track information to get
  //a real impact parameter for the track
  //Once alice-off gets its act together and fixes the AOD, this
  //should become obsolete.

  Double_t bfield = 5.; //kG
  Double_t maxD = 100000.; //max transverse IP
  if(GetReader()->GetDataType() != AliCaloTrackReader::kMC) {
    bfield = GetReader()->GetBField();
    AliVEvent* ve = (AliVEvent*)GetReader()->GetInputEvent();
    AliVVertex *vv = (AliVVertex*)ve->GetPrimaryVertex();
    AliESDtrack esdTrack(track);
    Bool_t gotit = esdTrack.PropagateToDCA(vv,bfield,maxD,impPar,cov);
    return gotit;
  }

  return kFALSE;

}


//__________________________________________________________________
Bool_t AliAnaElectron::CheckTrack(const AliAODTrack* track, const char* type) 
{
  //Check this track to see if it is also tagged as an electron in the
  //AliAODPWG4Particle list and if it is non-photonic

  Bool_t pass = kFALSE;

  Int_t trackId = track->GetID(); //get the index in the reader

  Int_t naod = GetOutputAODBranch()->GetEntriesFast();
  if(GetDebug() > 3) printf("AliAnaElectron::CheckTrack() - aod branch entries %d\n", naod);
  for(Int_t iaod = 0; iaod < naod ; iaod++){
    AliAODPWG4Particle* ele =  (AliAODPWG4Particle*) (GetOutputAODBranch()->At(iaod));
    Int_t label = ele->GetTrackLabel(0);
    if(label != trackId) continue;  //skip to the next one if they don't match

    if(type=="DVM") { 
      if(ele->CheckBTagBit(ele->GetBtag(),AliAODPWG4Particle::kDVMTag0) ||
	 ele->CheckBTagBit(ele->GetBtag(),AliAODPWG4Particle::kDVMTag1) ||
	 ele->CheckBTagBit(ele->GetBtag(),AliAODPWG4Particle::kDVMTag2))
	pass = kTRUE;

    } else if (type=="NPE") {

      Bool_t photonic = kFALSE;
      Bool_t photonic1 = kFALSE;
      photonic1 = IsItPhotonic(ele); //check against primaries
      Bool_t photonic2 = kFALSE;
      photonic2 = IsItPhotonic2(ele); //check against V0s
      if(photonic1 || photonic2) photonic = kTRUE;
      
      if(!photonic) pass = kTRUE;

    } else {
      return kFALSE;
    }
  }

  return pass;

}

//__________________________________________________________________
void AliAnaElectron::Print(const Option_t * opt) const
{
  //Print some relevant parameters set for the analysis
  
  if(! opt)
    return;
  
  printf("**** Print %s %s ****\n", GetName(), GetTitle() ) ;
  AliAnaPartCorrBaseClass::Print(" ");

  printf("Calorimeter            =     %s\n", fCalorimeter.Data()) ;
  printf("pOverE range           =     %f - %f\n",fpOverEmin,fpOverEmax);
  printf("residual cut           =     %f\n",fResidualCut);
  printf("---DVM Btagging\n");
  printf("max IP-cut (e,h)       =     %f\n",fImpactCut);
  printf("min ITS-hits           =     %d\n",fITSCut);
  printf("max dR (e,h)           =     %f\n",fDrCut);
  printf("max pairDCA            =     %f\n",fPairDcaCut);
  printf("max decaylength        =     %f\n",fDecayLenCut);
  printf("min Associated Pt      =     %f\n",fAssocPtCut);
  printf("---IPSig Btagging\n");
  printf("min tag track          =     %d\n",fNTagTrkCut);
  printf("min IP significance    =     %f\n",fIPSigCut);
  printf("    \n") ;
	
} 

//________________________________________________________________________
void AliAnaElectron::ReadHistograms(TList* outputList)
{
  // Needed when Terminate is executed in distributed environment                             
  // Refill analysis histograms of this class with corresponding
  // histograms in output list.   

  // Histograms of this analsys are kept in the same list as other
  // analysis, recover the position of
  // the first one and then add the next                                                      
  Int_t index = outputList->IndexOf(outputList->FindObject(GetAddedHistogramsStringToName()+"fh1pOverE"));

  //Read histograms, must be in the same order as in
  //GetCreateOutputObject.                   
  fh1pOverE     = (TH1F *) outputList->At(index);
  fh1dR         = (TH1F *) outputList->At(index++);
  fh2EledEdx    = (TH2F *) outputList->At(index++);
  fh2MatchdEdx  = (TH2F *) outputList->At(index++);
  
}

//__________________________________________________________________
void  AliAnaElectron::Terminate(TList* outputList)
{

  //Do some plots to end
  //Recover histograms from output histograms list, needed for
  //distributed analysis.                
  //ReadHistograms(outputList);

  printf(" AliAnaElectron::Terminate()  *** %s Report: %d outputs\n", GetName(), outputList->GetEntries()) ;

}

