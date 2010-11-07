#include "AliAnalysisMultPbTrackHistoManager.h"
#include "AliLog.h"
#include "TH1.h"
#include "TH3D.h"
#include "TH1I.h"
#include "TROOT.h"
#include "TMCProcess.h"

#include <iostream>
using namespace std;
ClassImp(AliAnalysisMultPbTrackHistoManager)

const char * AliAnalysisMultPbTrackHistoManager::kStatStepNames[]     = { "All Events", "After physics Selection", "After centrality selection", "With Vertex" };
const char * AliAnalysisMultPbTrackHistoManager::kHistoPtEtaVzNames[] = { "hGenPtEtaVz", "hRecPtEtaVz", "hRecPtEtaVzPrim", 
									  "hRecPtEtaVzSecWeak", "hRecPtEtaVzSecMaterial", "hRecPtEtaVzFake"};
const char * AliAnalysisMultPbTrackHistoManager::kHistoDCANames[]     = { "hGenDCA", "hRecDCA", "hRecDCAPrim", "hRecDCASecWeak","hRecDCASecMaterial", "hRecDCAFake"};
const char * AliAnalysisMultPbTrackHistoManager::kHistoPrefix[]     = { "hGen", "hRec", "hRecPrim", "hRecSecWeak","hRecSecMaterial", "hRecFake"};



AliAnalysisMultPbTrackHistoManager::AliAnalysisMultPbTrackHistoManager() : AliHistoListWrapper(), fHNameSuffix(""){ 
  // standard ctor

}

AliAnalysisMultPbTrackHistoManager::AliAnalysisMultPbTrackHistoManager(const char * name, const char * title): AliHistoListWrapper(name,title), fHNameSuffix("")  {
  //named ctor
};

AliAnalysisMultPbTrackHistoManager::AliAnalysisMultPbTrackHistoManager(const AliAnalysisMultPbTrackHistoManager& obj) : AliHistoListWrapper (obj) {
  // copy ctor
  AliError("Not Implemented");
};

AliAnalysisMultPbTrackHistoManager::~AliAnalysisMultPbTrackHistoManager() {
  // dtor

}

TH3D * AliAnalysisMultPbTrackHistoManager::GetHistoPtEtaVz(Histo_t id) {
  // Returns a 3D histo of Pt/eta/vtx. It it does not exist, books it.

  TH3D * h = (TH3D*) GetHisto(kHistoPtEtaVzNames[id]);
  if (!h) {
    h = BookHistoPtEtaVz(kHistoPtEtaVzNames[id], Form("Pt Eta Vz distribution (%s)",kHistoPtEtaVzNames[id]));
  }

  return h;

}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoDCA(Histo_t id) {
  // Returns a 3D histo of Pt/eta/vtx. It it does not exist, books it.

  TH1D * h = (TH1D*) GetHisto(kHistoDCANames[id]);
  if (!h) {
    h = BookHistoDCA(kHistoDCANames[id], Form("Pt Eta Vz distribution (%s)",kHistoDCANames[id]));
  }

  return h;

}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoMult(Histo_t id) {
  // Returns a 3D histo of Pt/eta/vtx. It it does not exist, books it.

  TString name = kHistoPrefix[id];
  name += "Mult";
  TH1D * h = (TH1D*) GetHisto(name.Data());
  if (!h) {
    h = BookHistoMult(name.Data(), Form("Multiplicity distribution (%s)",kHistoPrefix[id]));
  }

  return h;

}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoPt (Histo_t id, 
						       Float_t minEta, Float_t maxEta, 
						       Float_t minVz, Float_t maxVz, 
						       Bool_t scaleWidth) {
  // Returns a projection of the 3D histo pt/eta/vz.
  // WARNING: since that is a histo, the requested range will be discretized to the binning.
  // Always avoids under (over) flows
  // If scaleWidth = kTRUE, the projection is scaled for the bin width (default)

  TH3D * h3D = GetHistoPtEtaVz(id);

  // Get range in terms of bin numners.  If the float range is
  // less than -11111 take the range from the first to the last bin (i.e. no
  // under/over-flows)
  Int_t min1 = minEta  < -11111 ? 1 : h3D ->GetYaxis()->FindBin(minEta);
  Int_t min2  = minVz  < -11111 ? 1 : h3D ->GetZaxis()->FindBin(minVz) ;

  Int_t max1 = maxEta  < -11111 ? h3D->GetNbinsY() : h3D ->GetYaxis()->FindBin(maxEta-0.00001);
  Int_t max2  = maxVz  < -11111 ? h3D->GetNbinsZ() : h3D ->GetZaxis()->FindBin(maxVz -0.00001);


  TString hname = h3D->GetName();
  hname = hname +  "_pt_" + long (min1)  + "_" + long(max1) + "_" + long (min2)  + "_" + long(max2);

  
  if (gROOT->FindObjectAny(hname.Data())){
    AliError(Form("An object called %s already exists,adding suffix",hname.Data()));
    hname += "_2";
  }

  TH1D * h = h3D->ProjectionX(hname.Data(), min1, max1, min2, max2, "E");
  if(scaleWidth) h ->Scale(1.,"width");

  return h;

}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoVz (Histo_t id, 
						       Float_t minPt, Float_t maxPt,
						       Float_t minEta, Float_t maxEta,
						       Bool_t scaleWidth) { 
  // Returns a projection of the 3D histo pt/eta/vz.
  // WARNING: since that is a histo, the requested range will be discretized to the binning.
  // Always avoids under (over) flows
  // If scaleWidth = kTRUE, the projection is scaled for the bin width (default)

  TH3D * h3D = GetHistoPtEtaVz(id);

  // Get range in terms of bin numners.  If the float range is
  // less than -11111 take the range from the first to the last bin (i.e. no
  // under/over-flows)
  Int_t min1  = minPt  < -11111 ? 1 : h3D ->GetXaxis()->FindBin(minPt) ;
  Int_t min2  = minEta < -11111 ? 1 : h3D ->GetYaxis()->FindBin(minEta);

  Int_t max1  = maxPt  < -11111 ? h3D->GetNbinsX() : h3D ->GetXaxis()->FindBin(maxPt -0.00001);
  Int_t max2  = maxEta < -11111 ? h3D->GetNbinsY() : h3D ->GetYaxis()->FindBin(maxEta-0.00001);


  TString hname = h3D->GetName();
  hname = hname +  "_Vz_" + long (min1)  + "_" + long(max1) + "_" + long (min2)  + "_" + long(max2);

  if (gROOT->FindObjectAny(hname.Data())){
    AliError(Form("An object called %s already exists, adding suffix",hname.Data()));
    hname+="_2";
  }

  TH1D * h = h3D->ProjectionZ(hname.Data(), min1, max1, min2, max2, "E");
  if(scaleWidth) h ->Scale(1.,"width");
  return h;


}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoEta (Histo_t id, 
							Float_t minPt, Float_t maxPt, 
							Float_t minVz, Float_t maxVz,
							Bool_t scaleWidth) {
  // Returns a projection of the 3D histo pt/eta/vz.
  // WARNING: since that is a histo, the requested range will be discretized to the binning.
  // Always avoids under (over) flows
  // If scaleWidth = kTRUE, the projection is scaled for the bin width (default)

  TH3D * h3D = GetHistoPtEtaVz(id);

  // Get range in terms of bin numners.  If the float range is
  // less than -11111 take the range from the first to the last bin (i.e. no
  // under/over-flows)
  Int_t min1 = minPt < -11111 ? 1 : h3D ->GetXaxis()->FindBin(minPt) ;
  Int_t min2 = minVz < -11111 ? 1 : h3D ->GetYaxis()->FindBin(minVz);

  Int_t max1 = maxPt < -11111 ? h3D->GetNbinsX() : h3D ->GetXaxis()->FindBin(maxPt -0.00001);
  Int_t max2 = maxVz < -11111 ? h3D->GetNbinsY() : h3D ->GetYaxis()->FindBin(maxVz-0.00001);

  TString hname = h3D->GetName();
  hname = hname +  "_Eta_" + long (min1)  + "_" + long(max1) + "_" + long (min2)  + "_" + long(max2);

  if (gROOT->FindObjectAny(hname.Data())){
    AliError(Form("An object called %s already exists, adding suffix",hname.Data()));
    hname+="_2";
  }

  TH1D * h = h3D->ProjectionY(hname.Data(), min1, max1, min2, max2, "E");
  if(scaleWidth) h ->Scale(1.,"width");
  return h;
}

TH1D * AliAnalysisMultPbTrackHistoManager::GetHistoSpecies(Histo_t id) {

  // Returns histogram with particle specties

  TString name = TString(kHistoPrefix[id])+"_Species";

  TH1D * h =  (TH1D*) GetHisto(name);
  if (!h) {
    name+=fHNameSuffix;
    Bool_t oldStatus = TH1::AddDirectoryStatus();
    TH1::AddDirectory(kFALSE);

    AliInfo(Form("Booking histo %s",name.Data()));

    h = new TH1D (name.Data(), Form("Particle species (%s)",kHistoPrefix[id]), kPNoProcess+1, -0.5, kPNoProcess+1-0.5);			 
    Int_t nbin = kPNoProcess+1;
    for(Int_t ibin = 0; ibin < nbin; ibin++){
      h->GetXaxis()->SetBinLabel(ibin+1,TMCProcessName[ibin]);      
    }
    TH1::AddDirectory(oldStatus);
    fList->Add(h);


  }
  return h;
  

}



TH1I * AliAnalysisMultPbTrackHistoManager::GetHistoStats() {
  // Returns histogram with event statistiscs (processed events at each step)

  TH1I * h =  (TH1I*) GetHisto("hStats");
  if (!h) h = BookHistoStats();
  return h;
  
} 



TH1 * AliAnalysisMultPbTrackHistoManager::GetHisto(const char * name) {
  //Search list for histo
  // TODO: keep track of histo id rather than searching by name?
  return (TH1*) fList->FindObject(TString(name)+fHNameSuffix);

}

void AliAnalysisMultPbTrackHistoManager::ScaleHistos(Double_t nev, Option_t * option) {
  // Scales all histos in the list for nev
  // option can be used to pass further options to TH1::Scale
  TH1 * h = 0;
  TIter iter = fList->MakeIterator();
  while ((h = (TH1*) iter.Next())) {
    if (!h->InheritsFrom("TH1")) {
      AliFatal (Form("%s does not inherits from TH1, cannot scale",h->GetName()));
    }
    AliInfo(Form("Scaling %s, nev %2.2f", h->GetName(), nev));
    
    h->Scale(1./nev,option);
  }

}

TH3D * AliAnalysisMultPbTrackHistoManager::BookHistoPtEtaVz(const char * name, const char * title) {
  // Books a 3D histo of Pt/eta/vtx
  // TODO: make the binning settable, variable binning?

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  TString hname = name;
  hname+=fHNameSuffix;

  AliInfo(Form("Booking %s",hname.Data()));
  
  // Binning from Jacek task
  const Int_t nptbins = 68;
  const Double_t binsPt[] = {0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,18.0,20.0,22.0,24.0,26.0,28.0,30.0,32.0,34.0,36.0,40.0,45.0,50.0};
  
  const Int_t netabins=20;
  Double_t binsEta[netabins+1];
  Float_t minEta = -1;
  Float_t maxEta =  1;
  Float_t etaStep = (maxEta-minEta)/netabins;
  for(Int_t ibin = 0; ibin < netabins; ibin++){    
    binsEta[ibin]   = minEta + ibin*etaStep;
    binsEta[ibin+1] = minEta + ibin*etaStep + etaStep;
  }

  const Int_t nvzbins=10;
  Double_t binsVz[nvzbins+1];
  Float_t minVz = -10;
  Float_t maxVz =  10;
  Float_t vzStep = (maxVz-minVz)/nvzbins;
  for(Int_t ibin = 0; ibin < nvzbins; ibin++){    
    binsVz[ibin]   = minVz + ibin*vzStep;
    binsVz[ibin+1] = minVz + ibin*vzStep + vzStep;
  }
 

  TH3D * h = new TH3D (hname,title, 
		       nptbins,  binsPt,
		       netabins, binsEta,
		       nvzbins,  binsVz
		       );

  h->SetXTitle("p_{T}");
  h->SetYTitle("#eta");
  h->SetZTitle("V_{z} (cm)");
  h->Sumw2();
  
  fList->Add(h);

  TH1::AddDirectory(oldStatus);
  return h;
}

TH1D * AliAnalysisMultPbTrackHistoManager::BookHistoDCA(const char * name, const char * title) {
  // Books a DCA histo 

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  TString hname = name;
  hname+=fHNameSuffix;

  AliInfo(Form("Booking %s",hname.Data()));
  

  TH1D * h = new TH1D (hname,title, 200,0,200);

  h->SetXTitle("#Delta DCA");
  h->Sumw2();
  
  fList->Add(h);

  TH1::AddDirectory(oldStatus);
  return h;
}
TH1D * AliAnalysisMultPbTrackHistoManager::BookHistoMult(const char * name, const char * title) {
  // Books a multiplicity histo 

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  TString hname = name;
  hname+=fHNameSuffix;

  AliInfo(Form("Booking %s",hname.Data()));
  

  TH1D * h = new TH1D (hname,title, 600, 0,6000);

  h->SetXTitle("N tracks");
  h->Sumw2();
  
  fList->Add(h);

  TH1::AddDirectory(oldStatus);
  return h;
}

TH1I * AliAnalysisMultPbTrackHistoManager::BookHistoStats() {
  // Books histogram with event statistiscs (processed events at each step)

  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);

  AliInfo(Form("Booking Stat histo"));

  TH1I * h = new TH1I (TString("hStats")+fHNameSuffix, "Number of processed events", kNStatBins, -0.5, kNStatBins-0.5);
  for(Int_t istatbin = 0; istatbin < kNStatBins; istatbin++){
    h->GetXaxis()->SetBinLabel(istatbin+1,kStatStepNames[istatbin]);
  }
  TH1::AddDirectory(oldStatus);
  fList->Add(h);
  return h;
}




 
