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

/* $Id: AliTRDefficiencyMC.cxx 27496 2008-07-22 08:35:45Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Reconstruction QA                                                     //
//                                                                        //
//  Authors:                                                              //
//    Markus Fasel <M.Fasel@gsi.de>                                       //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include <TObjArray.h>
#include <TClonesArray.h>
#include <TProfile.h>
#include <TMath.h>
#include "TTreeStream.h"

#include "AliMagF.h"
#include "AliPID.h"
#include "AliMathBase.h"
#include "AliTrackReference.h"
#include "AliAnalysisManager.h"

#include "AliTRDcluster.h"
#include "AliTRDseedV1.h"
#include "AliTRDtrackV1.h"
#include "Cal/AliTRDCalPID.h"
#include "info/AliTRDtrackInfo.h"
#include "AliTRDinfoGen.h"
#include "AliTRDefficiencyMC.h"

ClassImp(AliTRDefficiencyMC)

//_____________________________________________________________________________
AliTRDefficiencyMC::AliTRDefficiencyMC()
  :AliTRDrecoTask("EfficiencyMC", "Combined Tracking Efficiency")
{
  //
  // Default constructor
  //
}


//_____________________________________________________________________________
void AliTRDefficiencyMC::CreateOutputObjects(){
  //
  // Create output objects
  //

  OpenFile(0, "RECREATE");
  fContainer = Histos();

}

//_____________________________________________________________________________
void AliTRDefficiencyMC::Exec(Option_t *){
  //
  // Execute the task:
  //
  // Loop over TrackInfos
  // 1st: check if there is a trackTRD
  // 2nd: put conditions on the track:
  //      - check if we did not register it before
  //      - check if we have Track References for the track 
  // 3rd: Register track:
  //      - accepted if both conditions are fulfilled
  //      - contamination if at least one condition is not fulfilled
  // 4th: check Monte-Carlo Track wheter findable or not if there is no TRD track in this track info
  // 5th: register MC track as rejected if findable and not jet registered
  // Match the registers accepted and rejected and clean register rejected
  // Fill the histograms
  //
  const Int_t kArraySize = 10000;     // Store indices of track references in an array
  Int_t indexAccepted[kArraySize], indexRejected[kArraySize], indexContamination[kArraySize];
  memset(indexAccepted, 0, sizeof(Int_t) * kArraySize);
  memset(indexRejected, 0, sizeof(Int_t) * kArraySize);
  memset(indexContamination, 0, sizeof(Int_t) * kArraySize);
  Int_t naccepted = 0, nrejected = 0, ncontamination = 0;
  Bool_t isContamination = kFALSE;
  
  Int_t nTrackInfos = fTracks->GetEntriesFast();
  AliTRDtrackV1 *trackTRD = 0x0;
  AliTRDtrackInfo *trkInf = 0x0;
  for(Int_t itinf = 0; itinf < nTrackInfos; itinf++){
    trkInf = dynamic_cast<AliTRDtrackInfo *>(fTracks->UncheckedAt(itinf));
    if(!trkInf) continue;
    if(trkInf->GetTrack() || trkInf->GetNumberOfClustersRefit()){
      isContamination = IsRegistered(trkInf,indexAccepted,naccepted);
      if(!trkInf->GetNTrackRefs()){
        // We reject the track since the Monte Carlo Information is missing
        AliDebug(1, Form("Error: Track Reference missing for Track %d", trkInf->GetLabel()));
        isContamination = kTRUE;
        // Debugging
        if(trackTRD && DebugLevel() > 5) FillStreamTrackWOMC(trkInf);
      }	
      if(isContamination){
        // reject kink (we count these only once)
        if(trkInf->GetKinkIndex()) continue;
        // Register track as contamination
        indexContamination[ncontamination++]=itinf;
        continue;
      }
      // Accept track
      AliDebug(4, "Accept track");
      // Register track as accepted
      indexAccepted[naccepted++] = itinf;
    }else{
      if(IsFindable(trkInf)){
        // register track as rejected if not already registered there
        // Attention:
        // These track infos are not!!! registered as contamination
        if(!IsRegistered(trkInf, indexRejected, nrejected)) indexRejected[nrejected++] = itinf;
      }
    }
  }
  // we have to check if the rejected tracks are registered as found
  // a possible source for this:
  // first the track is not found by the barrel tracking but it is later found
  // by the stand alone tracking, then two track info objects with the same 
  // label would be created
  // Attention:
  // these tracks are not! registered as contamination
  Int_t tmprejected[kArraySize]; Int_t nrej = nrejected;
  memcpy(tmprejected, indexRejected, sizeof(Int_t) * nrejected);
  nrejected = 0;
  for(Int_t irej = 0; irej < nrej; irej++){
    trkInf = dynamic_cast<AliTRDtrackInfo *>(fTracks->At(tmprejected[irej]));
    if(!IsRegistered(trkInf,indexAccepted,naccepted)) indexRejected[nrejected++] = tmprejected[irej];
  }
  // Fill Histograms
  FillHistograms(naccepted, &indexAccepted[0], kAccepted);
  FillHistograms(nrejected, &indexRejected[0], kRejected);
  FillHistograms(ncontamination, &indexContamination[0], kContamination);
  Int_t nall = naccepted + nrejected;
  //if(DebugLevel()>=1)
  AliInfo(Form("%3d Tracks: MC[%3d] TRD[%3d | %5.2f%%] \n", (Int_t)AliAnalysisManager::GetAnalysisManager()->GetCurrentEntry(), nall, naccepted, nall ? 1.E2*Float_t(naccepted)/Float_t(nall) : 0.));
  AliInfo(Form("%3d Tracks: ALL[%3d] Contamination[%3d | %5.2f%%] \n", (Int_t)AliAnalysisManager::GetAnalysisManager()->GetCurrentEntry(), nall + ncontamination, ncontamination, nall ? 1.E2*Float_t(ncontamination)/Float_t(nall + ncontamination) : 0.));

  PostData(0, fContainer);
}


//_____________________________________________________________________________
Bool_t AliTRDefficiencyMC::PostProcess()
{
  //
  // Post Process 
  //
  // Change the histogram style
  // For species histograms apply the colors connected with the given particle species
  //
  TH1 *histo = dynamic_cast<TH1 *>(fContainer->At(kEfficiencyHistogram));
  histo->SetMarkerStyle(22);
  histo->SetMarkerColor(kBlue);
  histo->GetXaxis()->SetTitle("p [GeV/c]");
  histo->GetXaxis()->SetMoreLogLabels();
  histo->GetYaxis()->SetTitle("Efficiency [%]");
  histo->GetYaxis()->SetRangeUser(0.99, 1.005);

  histo = dynamic_cast<TH1 *>(fContainer->At(kContaminationHistogram));
  histo->SetMarkerStyle(22);
  histo->SetMarkerColor(kBlue);
  histo->GetXaxis()->SetTitle("p [GeV/c]");
  histo->GetXaxis()->SetMoreLogLabels();
  histo->GetYaxis()->SetTitle("Contamination [%]");
  
  // Species Efficiency Histograms
  for(Int_t ispec = 0; ispec < AliPID::kSPECIES; ispec++){
    histo = dynamic_cast<TH1 *>(fContainer->At(kEfficiencySpeciesHistogram  + ispec));
    histo->SetMarkerStyle(22);
    histo->SetLineColor(AliTRDCalPID::GetPartColor(ispec));
    histo->SetMarkerColor(AliTRDCalPID::GetPartColor(ispec));
    histo->GetXaxis()->SetTitle("p [GeV/c]");
    histo->GetXaxis()->SetMoreLogLabels();
    histo->GetYaxis()->SetTitle("Efficiency [%]");
    histo->GetYaxis()->SetRangeUser(0.99, 1.005);
  }
  
  // Species Contamination Histograms
  for(Int_t ispec = 0; ispec < AliPID::kSPECIES; ispec++){
    histo = dynamic_cast<TH1 *>(fContainer->At(kContaminationSpeciesHistogram  + ispec));
    histo->SetMarkerStyle(22);
    histo->SetLineColor(AliTRDCalPID::GetPartColor(ispec));
    histo->SetMarkerColor(AliTRDCalPID::GetPartColor(ispec));
    histo->GetXaxis()->SetTitle("p [GeV/c]");
    histo->GetXaxis()->SetMoreLogLabels();
    histo->GetYaxis()->SetTitle("Contamination [%]");
  }
  
  fNRefFigures = 6;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t AliTRDefficiencyMC::GetRefFigure(Int_t ifig){
  //
  // Plot the histograms
  //
  if(ifig >= fNRefFigures) return kFALSE;
  if(ifig < 2){
    (dynamic_cast<TH1 *>(fContainer->At(ifig)))->Draw("e1");
    return kTRUE;
  }
  switch(ifig){
  case 2:
    (dynamic_cast<TH1 *>(fContainer->At(kEfficiencySpeciesHistogram)))->Draw("e1");
    for(Int_t ispec = 1; ispec < AliPID::kSPECIES; ispec++)
      (dynamic_cast<TH1 *>(fContainer->At(kEfficiencySpeciesHistogram + ispec)))->Draw("e1same");
    break;
  case 3:
    (dynamic_cast<TH1 *>(fContainer->At(kContaminationSpeciesHistogram)))->Draw("e1");
    for(Int_t ispec = 1; ispec < AliPID::kSPECIES; ispec++)
      (dynamic_cast<TH1 *>(fContainer->At(kContaminationSpeciesHistogram + ispec)))->Draw("e1same");
    break;
  case 4:
    (dynamic_cast<TH1 *>(fContainer->At(kEfficiencyNoPID)))->Draw("e1");
    break;
  case 5:
    (dynamic_cast<TH1 *>(fContainer->At(kContaminationNoPID)))->Draw("e1");
    break;
  }
  return kTRUE;
}

//_____________________________________________________________________________
TObjArray *AliTRDefficiencyMC::Histos(){
  //
  // Create the histograms
  //
  const Int_t nbins = 11;

  if(fContainer) return fContainer;
  Float_t xbins[nbins+1] = {.5, .7, .9, 1.3, 1.7, 2.4, 3.5, 4.5, 5.5, 7., 9., 11.};
  
  TString species[AliPID::kSPECIES] = {"Pions", "Muons", "Electrons", "Kaons", "Protons"};
  TString species_short[AliPID::kSPECIES] = {"Pi", "Mu", "El", "Ka", "Pr"};
  
  fContainer = new TObjArray();
  fContainer->AddAt(new TProfile("trEffComb", "Combined Tracking Efficiency", nbins, xbins), kEfficiencyHistogram);
  fContainer->AddAt(new TProfile("trContComb", "Combined Tracking Contamination", nbins, xbins), kContaminationHistogram);
  for(Int_t ispec = 0; ispec < AliPID::kSPECIES; ispec++)
    fContainer->AddAt(new TProfile(Form("trEffComb%s", species_short[ispec].Data()), Form("Combined Tracking Efficiency %s", species[ispec].Data()), nbins, xbins), kEfficiencySpeciesHistogram + ispec);
  for(Int_t ispec = 0; ispec < AliPID::kSPECIES; ispec++)
    fContainer->AddAt(new TProfile(Form("trContComb%s", species_short[ispec].Data()), Form("Combined Tracking Contamination %s", species[ispec].Data()), nbins, xbins), kContaminationSpeciesHistogram + ispec);
  fContainer->AddAt(new TProfile("trEffCombNoPID", "Combined Tracking Efficiency", nbins, xbins), kEfficiencyNoPID);
  fContainer->AddAt(new TProfile("trContCombNoPID", "Combined Tracking Contamination", nbins, xbins), kContaminationNoPID);
  return fContainer;
}

//_____________________________________________________________________________
Bool_t AliTRDefficiencyMC::IsFindable(AliTRDtrackInfo * const trkInf){
  //
  // Apply Cuts on the Monte Carlo track references
  // return whether track is findable or not
  //
  const Float_t kAlpha = 0.349065850;
  
  AliDebug(10, "Analysing Track References\n");
  // Check if track is findable
  Double_t mom = 0.;
  Float_t xmin = 10000.0, xmax = 0.0; 
  Float_t ymin = 0.0, ymax = 0.0;
  Float_t zmin = 0.0, zmax = 0.0;
  Float_t lastx = 0.0, x = 0.0;
  Int_t nLayers = 0;
  Int_t sector[20];
  AliTrackReference *trackRef = 0x0;
  for(Int_t itr = 0; itr < trkInf->GetNTrackRefs(); itr++){
    trackRef = trkInf->GetTrackRef(itr);
    AliDebug(10, Form("%d. x[%f], y[%f], z[%f]\n", itr, trackRef->LocalX(), trackRef->LocalY(), trackRef->Z()));
    x = trackRef->LocalX(); 
        
    // Be Sure that we are inside TRD
    if(x < AliTRDinfoGen::GetTPCx() || x > AliTRDinfoGen::GetTOFx()) continue;	
    sector[itr] = Int_t(trackRef->Alpha()/kAlpha);
    if(x < xmin){
      xmin = trackRef->LocalX();
      ymin = trackRef->LocalY();
      zmin = trackRef->Z();
      mom = trackRef->P();
    } else if(x > xmax){
      xmax = trackRef->LocalX();
      ymax = trackRef->LocalY();
      zmax = trackRef->Z();
    }
    if(itr > 0){
      Float_t dist = TMath::Abs(x - lastx);
      AliDebug(10, Form("x = %f, lastx = %f, dist = %f\n", x, lastx, dist));
      if(TMath::Abs(dist - 3.7) < 0.1) nLayers++; 	// ref(i+1) has to be larger than ref(i)
    }
    lastx = x;
  }

  // Apply cuts
  Bool_t findable = kTRUE;
  if(trkInf->GetNTrackRefs() > 2 && xmax > xmin){
    if(mom < 0.55) findable = kFALSE;									// momentum cut at 0.6
    Double_t yangle = (ymax -ymin)/(xmax - xmin);
    Double_t zangle = (zmax -zmin)/(xmax - xmin);
    AliDebug(10, Form("\ntrack: y-Angle = %f, z-Angle = %f\nnLayers = %d", yangle, zangle, nLayers));
    if(TMath::ATan(TMath::Abs(yangle)) > 45.) findable = kFALSE;
    if(TMath::ATan(TMath::Abs(zangle)) > 45.) findable = kFALSE;
    if(nLayers < 4) findable = kFALSE;
    if(!trkInf->IsPrimary()) findable = kFALSE;
    Bool_t samesec = kTRUE;
    for(Int_t iref = 1; iref < trkInf->GetNTrackRefs(); iref++)
      if(sector[iref] != sector[0]) samesec = kFALSE;
    if(!samesec) findable = kFALSE;		// Discard all tracks which are passing more than one sector
    if(DebugLevel()){
      Double_t trackAngle = TMath::ATan(yangle);
      Bool_t primary = trkInf->IsPrimary();
      (*DebugStream()) << "NotFoundTrack"
        << "Momentum=" 	<< mom
        << "trackAngle="<< trackAngle
        << "NLayers="	<< nLayers
        << "Primary="	<< primary
        << "\n";
    }
  }
  else
    findable = kFALSE;
  return findable;
}

//_____________________________________________________________________________
void AliTRDefficiencyMC::FillHistograms(Int_t nTracks, Int_t *indices, FillingMode_t mode){
  //
  // Fill Histograms in three different modes:
  // 1st tracks which are found and accepted
  // 2nd tracks which are not found and not already counted
  // 3rd contaminating tracks: either double counts (kinks!) or tracks with no MC hit inside TRD
  //
  const Int_t pid[AliPID::kSPECIES] = {211,13,11,321,2212};
  Double_t trkmom = 0.;   // the track momentum
  Int_t trkpid = -1;      // particle species
  AliTRDtrackInfo *trkInf = 0x0;
  for(Int_t itk = 0; itk < nTracks; itk++){
    trkInf = dynamic_cast<AliTRDtrackInfo *>(fTracks->At(indices[itk]));
    AliDebug(3, Form("Accepted MC track: %d\n", trkInf->GetLabel()));
    if(trkInf->GetNTrackRefs()){
      // use Monte-Carlo Information for Momentum and PID
      trkmom = trkInf->GetTrackRef(0)->P();
      trkpid = trkInf->GetPDG();
    }else{
      // Use TPC Momentum
      trkmom = trkInf->GetTrack()->P();
    }
    switch(mode){
      case kAccepted:
        (dynamic_cast<TProfile *>(fContainer->At(kEfficiencyHistogram)))->Fill(trkmom, 1);
        (dynamic_cast<TProfile *>(fContainer->At(kContaminationHistogram)))->Fill(trkmom, 0);
        break;
      case kRejected:
        (dynamic_cast<TProfile *>(fContainer->At(kEfficiencyHistogram)))->Fill(trkmom, 0);
        (dynamic_cast<TProfile *>(fContainer->At(kContaminationHistogram)))->Fill(trkmom, 0);
        break;
      case kContamination:
        (dynamic_cast<TProfile *>(fContainer->At(kContaminationHistogram)))->Fill(trkmom, 1);
        break;
    }
    // Fill species histogram
    Int_t partSpec = -1;
    for(Int_t ispec = 0; ispec < AliPID::kSPECIES; ispec++){
      if(trkpid == pid[ispec]) partSpec = ispec;
    }
    if(partSpec >= 0){
      switch(mode){
        case kAccepted:
          (dynamic_cast<TProfile *>(fContainer->At(kEfficiencySpeciesHistogram + partSpec)))->Fill(trkmom, 1);
          (dynamic_cast<TProfile *>(fContainer->At(kContaminationSpeciesHistogram + partSpec)))->Fill(trkmom, 0);
          break;
        case kRejected:
          (dynamic_cast<TProfile *>(fContainer->At(kEfficiencySpeciesHistogram + partSpec)))->Fill(trkmom, 0);          (dynamic_cast<TProfile *>(fContainer->At(kContaminationSpeciesHistogram + partSpec)))->Fill(trkmom, 0);
          break;
        case kContamination:
          (dynamic_cast<TProfile *>(fContainer->At(kContaminationSpeciesHistogram + partSpec)))->Fill(trkmom, 1);
          break;
      }
    } else {
      // The particle Type is not registered
      (dynamic_cast<TProfile *>(fContainer->At(kEfficiencyNoPID)))->Fill(trkmom, 1);
      (dynamic_cast<TProfile *>(fContainer->At(kContaminationNoPID)))->Fill(trkmom, 1);
    }
  }
}

//_____________________________________________________________________________
void AliTRDefficiencyMC::FillStreamTrackWOMC(AliTRDtrackInfo * const trkInf){
  // fill debug stream
  // we want to know:
  //  1. The event number
  //  2. The track label
  //  3. The TRD track label
  //  4. The frequency of the TRD Label
  //  5. Momentum from TPC (NO STAND ALONE TRACK)
  //  6. TPC Phi angle
  //  7. the TRD track
  //  8. Monte Carlo PID
  //  9. We check the Labels of the TRD track according to them we search the maching Monte-Carlo track.
  //     From the matching Monte-Carlo track we store trackRefs, phi and momentum
  // 10. We may also want to keep the kink index
  Double_t mom = trkInf->GetESDinfo()->GetOuterParam()->P();
  Int_t event = (Int_t)AliAnalysisManager::GetAnalysisManager()->GetCurrentEntry();
  Int_t label = trkInf->GetLabel();
  Int_t kinkIndex = trkInf->GetKinkIndex();
  Int_t pdg = trkInf->GetPDG();
  Double_t phiTPC = trkInf->GetESDinfo()->GetOuterParam()->Phi();
  Int_t labelsTRD[180];	// Container for the cluster labels
  Int_t sortlabels[360];	// Cluster Labels sorted according their occurancy
  AliTRDseedV1 *tracklet = 0x0;
  AliTRDcluster *c = 0x0;
  Int_t nclusters = 0x0;
  AliTRDtrackV1 *trackTRD = trkInf->GetTrack();
  for(Int_t il = 0; il < AliTRDgeometry::kNlayer; il++){
    tracklet = trackTRD->GetTracklet(il);
    if(!tracklet) continue;
    tracklet->ResetClusterIter();
    c = 0x0;
    while((c = tracklet->NextCluster())) labelsTRD[nclusters++] = c->GetLabel(0);
  }
  // Determine Label and Frequency
  AliMathBase::Freq(nclusters, const_cast<const Int_t *>(&labelsTRD[0]), &sortlabels[0], kTRUE);
  Int_t labelTRD = sortlabels[0];
  Int_t freqTRD = sortlabels[1];
  // find the track info object matching to the TRD track
  AliTRDtrackInfo *realtrack = 0;
  TObjArrayIter rtiter(fTracks);
  while((realtrack = (AliTRDtrackInfo *)rtiter())){
    if(realtrack->GetLabel() != labelTRD) continue;
    break;
  }
  TClonesArray trackRefs("AliTrackReference");
  Int_t realPdg = -1;
  Double_t realP = 0.;
  Double_t realPhi = 0.;
  if(realtrack){
    // pack the track references into the trackRefsContainer
    for(Int_t iref = 0; iref < realtrack->GetNTrackRefs(); iref++){
    new(trackRefs[iref])AliTrackReference(*(realtrack->GetTrackRef(iref)));
    }
    realPdg = realtrack->GetPDG();
    if(realtrack->GetNTrackRefs()){
      realP = realtrack->GetTrackRef(0)->P();
      realPhi = realtrack->GetTrackRef(0)->Phi();
    }
  }
  (*DebugStream()) << "TrackingEffMCfake"
    << "Event="	<< event
    << "Label=" << label
    << "labelTRD=" << labelTRD
    << "FreqTRDlabel=" << freqTRD
    << "TPCp="	<< mom
    << "phiTPC=" << phiTPC
    << "trackTRD=" << trackTRD
    << "PDG="	<< pdg
    << "TrackRefs=" << &trackRefs
    << "RealPDG=" << realPdg
    << "RealP="	<< realP
    << "RealPhi" << realPhi
    << "KinkIndex=" << kinkIndex
    << "\n";
}

//_____________________________________________________________________________
Bool_t AliTRDefficiencyMC::IsRegistered(AliTRDtrackInfo * const trkInf, Int_t *indices, Int_t nTracks){
  //
  // Checks if track is registered in a given mode
  //
  Bool_t isRegistered = kFALSE;
  for(Int_t il = 0; il < nTracks; il++){
    if((dynamic_cast<AliTRDtrackInfo *>(fTracks->At(indices[il])))->GetLabel() == trkInf->GetLabel()){
      isRegistered = kTRUE;        
      break;
    }
  }
  return isRegistered;
}

