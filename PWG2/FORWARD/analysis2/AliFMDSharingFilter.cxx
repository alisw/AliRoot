//
// Class to do the sharing correction of FMD ESD data
//
#include "AliFMDSharingFilter.h"
#include <AliESDFMD.h>
#include <TAxis.h>
#include <TList.h>
#include <TH1.h>
#include <TMath.h>
#include "AliFMDAnaParameters.h"
#include <AliLog.h>

ClassImp(AliFMDSharingFilter)
#if 0
; // This is for Emacs
#endif 


//____________________________________________________________________
AliFMDSharingFilter::AliFMDSharingFilter()
  : TNamed(), 
    fRingHistos(),
    fLowCut(0.3),
    fCorrectAngles(kFALSE), 
    fDebug(0)
{}

//____________________________________________________________________
AliFMDSharingFilter::AliFMDSharingFilter(const char* title)
  : TNamed("fmdSharingFilter", title), 
    fRingHistos(), 
    fLowCut(0.3),
    fCorrectAngles(kFALSE), 
    fDebug(0)
{
  fRingHistos.Add(new RingHistos(1, 'I'));
  fRingHistos.Add(new RingHistos(2, 'I'));
  fRingHistos.Add(new RingHistos(2, 'O'));
  fRingHistos.Add(new RingHistos(3, 'I'));
  fRingHistos.Add(new RingHistos(3, 'O'));
}

//____________________________________________________________________
AliFMDSharingFilter::AliFMDSharingFilter(const AliFMDSharingFilter& o)
  : TNamed(o), 
    fRingHistos(), 
    fLowCut(o.fLowCut),
    fCorrectAngles(o.fCorrectAngles), 
    fDebug(o.fDebug)
{
  TIter    next(&o.fRingHistos);
  TObject* obj = 0;
  while ((obj = next())) fRingHistos.Add(obj);
}

//____________________________________________________________________
AliFMDSharingFilter::~AliFMDSharingFilter()
{
  fRingHistos.Delete();
}

//____________________________________________________________________
AliFMDSharingFilter&
AliFMDSharingFilter::operator=(const AliFMDSharingFilter& o)
{
  TNamed::operator=(o);

  fLowCut        = o.fLowCut;
  fCorrectAngles = o.fCorrectAngles;
  fDebug         = o.fDebug;

  fRingHistos.Delete();
  TIter    next(&o.fRingHistos);
  TObject* obj = 0;
  while ((obj = next())) fRingHistos.Add(obj);
  
  return *this;
}

//____________________________________________________________________
AliFMDSharingFilter::RingHistos*
AliFMDSharingFilter::GetRingHistos(UShort_t d, Char_t r) const
{
  Int_t idx = -1;
  switch (d) { 
  case 1: idx = 0; break;
  case 2: idx = 1 + (r == 'I' || r == 'i' ? 0 : 1); break;
  case 3: idx = 3 + (r == 'I' || r == 'i' ? 0 : 1); break;
  }
  if (idx < 0 || idx >= fRingHistos.GetEntries()) return 0;
  
  return static_cast<RingHistos*>(fRingHistos.At(idx));
}

//____________________________________________________________________
Bool_t
AliFMDSharingFilter::Filter(const AliESDFMD& input, 
			    Bool_t           lowFlux,
			    AliESDFMD&       output)
{
  output.Clear();
  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next())))
    o->Clear();

  // AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();

  for(UShort_t d = 1; d <= 3; d++) {
    Int_t nRings = (d == 1 ? 1 : 2);
    for (UShort_t q = 0; q < nRings; q++) {
      Char_t      r    = (q == 0 ? 'I' : 'O');
      UShort_t    nsec = (q == 0 ?  20 :  40);
      UShort_t    nstr = (q == 0 ? 512 : 256);

      RingHistos* histos = GetRingHistos(d, r);
      
      for(UShort_t s = 0; s < nsec;  s++) {
	Bool_t usedThis = kFALSE;
	Bool_t usedPrev = kFALSE;
	
	for(UShort_t t = 0; t < nstr; t++) {
	  output.SetMultiplicity(d,r,s,t,0.);
	  Float_t mult = SignalInStrip(input,d,r,s,t);
	  
	  // Keep dead-channel information. 
	  if(mult == AliESDFMD::kInvalidMult)
	    output.SetMultiplicity(d,r,s,t,AliESDFMD::kInvalidMult);

	  // If no signal or dead strip, go on. 
	  if (mult == AliESDFMD::kInvalidMult || mult == 0) continue;

	  // Get the pseudo-rapidity 
	  Double_t eta = input.Eta(d,r,s,t);
	  
	  // Fill the diagnostics histogram 
	  histos->fBefore->Fill(mult);

	  // Get next and previous signal - if any 
	  Double_t prevE = 0;
	  Double_t nextE = 0;
	  if (t != 0) {
	    prevE = SignalInStrip(input,d,r,s,t-1);
	    if (prevE == AliESDFMD::kInvalidMult) prevE = 0;
	  }
	  if (t != nstr - 1) {
	    nextE = SignalInStrip(input,d,r,s,t+1);
	    if (nextE == AliESDFMD::kInvalidMult) nextE = 0;
	  }
	  
	  Double_t mergedEnergy = MultiplicityOfStrip(mult,eta,prevE,nextE,
						      lowFlux,d,r,s,t, 
						      usedPrev,usedThis);
	  if (mergedEnergy > fLowCut) histos->Incr();
	  histos->fAfter->Fill(mergedEnergy);

	  output.SetMultiplicity(d,r,s,t,mergedEnergy);
	  output.SetEta(d,r,s,t,eta);
	} // for strip
      } // for sector
    } // for ring 
  } // for detector

  next.Reset();
  while ((o = static_cast<RingHistos*>(next())))
    o->Finish();

  return kTRUE;
}

//_____________________________________________________________________
Double_t 
AliFMDSharingFilter::SignalInStrip(const AliESDFMD& input, 
				   UShort_t         d,
				   Char_t           r,
				   UShort_t         s,
				   UShort_t         t) const
{
  Double_t mult = input.Multiplicity(d,r,s,t);
  if (mult == AliESDFMD::kInvalidMult || mult == 0) return mult;

  if (fCorrectAngles && !input.IsAngleCorrected()) 
    mult = AngleCorrect(mult, input.Eta(d,r,s,t));
  else if (!fCorrectAngles && input.IsAngleCorrected()) 
    mult = DeAngleCorrect(mult, input.Eta(d,r,s,t));
  return mult;
}
			
//_____________________________________________________________________
Double_t 
AliFMDSharingFilter::GetHighCut(UShort_t d, Char_t r, Double_t eta) const
{
  AliFMDAnaParameters* pars = AliFMDAnaParameters::Instance();
 
  // Get the high cut.  The high cut is defined as the 
  // most-probably-value peak found from the energy distributions, minus 
  // 2 times the width of the corresponding Landau.
  Double_t mpv     = pars->GetMPV(d,r,eta);
  Double_t w       = pars->GetSigma(d,r,eta);
  if (mpv > 100) 
    AliWarning(Form("FMD%d%c, eta=%f, MPV=%f, W=%f", d, r, eta, mpv, w));
  Double_t highCut = (mpv - 2 * w);
  return highCut;
}
	   
//_____________________________________________________________________
Double_t 
AliFMDSharingFilter::MultiplicityOfStrip(Double_t mult,
					 Double_t eta,
					 Double_t prevE,
					 Double_t nextE,
					 Bool_t   lowFlux,
					 UShort_t d,
					 Char_t   r,
					 UShort_t /*s*/,
					 UShort_t /*t*/,
					 Bool_t&  usedPrev, 
					 Bool_t&  usedThis) 
{
  // IF the multiplicity is very large, or below the cut, reject it, and 
  // flag it as candidate for sharing 
  if(mult > 12 || mult < fLowCut) {
    usedThis      = kFALSE;
    usedPrev      = kFALSE;
    return 0;
  }

  // If this was shared with the previous signal, return 0 and mark it as 
  // not a candiate for sharing 
  if(usedThis) {
    usedThis      = kFALSE;
    usedPrev      = kTRUE;
    return 0.;
  }

  //analyse and perform sharing on one strip
  
  // Calculate the total energy loss 
  Double_t highCut = GetHighCut(d, r, eta);

  // If this signal is smaller than the next, and the next signal is smaller 
  // than then the high cut, and it's a low-flux event, then mark this and 
  // the next signal as candidates for sharing 
  // 
  // This is the test if the signal is the smaller of two possibly
  // shared signals.   
  if (mult  < nextE   && 
      nextE > highCut && 
      lowFlux) {
    usedThis      = kFALSE;
    usedPrev      = kFALSE;
    return 0;
  }
  
  Double_t totalE  = mult;
  
  // If the previous signal was larger than the low cut, and 
  // the previous was smaller than high cut, and the previous signal 
  // isn't marked as used, then add it's energy to this signal 
  if (prevE > fLowCut && 
      prevE < highCut && 
      !usedPrev) 
    totalE += prevE;

  // If the next signal is larger than the low cut, and 
  // the next signal is smaller than the low cut, then add the next signal 
  // to this, and marked the next signal as used 
  if(nextE > fLowCut && nextE < highCut ) {
    totalE      += nextE;
    usedThis =  kTRUE;
  }

  // If we're processing on non-angle corrected data, we should do the 
  // angle correction here 
  if (!fCorrectAngles) 
    totalE = AngleCorrect(totalE, eta);

  // Fill post processing histogram
  // if(totalE > fLowCut)
  //   GetRingHistos(d,r)->fAfter->Fill(totalE);

  // Return value 
  Double_t mergedEnergy = 0;
  
  if(totalE > 0) {
    // If we have a signal, then this is used (sharedPrev=true) and
    // the signal is set to the result
    mergedEnergy = totalE;
    usedPrev     = kTRUE;
  }
  else {
    // If we do not have a signal (too low), then this is not shared, 
    // and the next strip is not shared either 
    usedThis  = kFALSE;
    usedPrev  = kFALSE;
  }
  
  return mergedEnergy; 
}
//____________________________________________________________________
Double_t
AliFMDSharingFilter::AngleCorrect(Double_t mult, Double_t eta) const
{
  Double_t theta =  2 * TMath::ATan(TMath::Exp(-eta));
  if (eta < 0) theta -= TMath::Pi();
  return mult * TMath::Cos(theta);
}
//____________________________________________________________________
Double_t
AliFMDSharingFilter::DeAngleCorrect(Double_t mult, Double_t eta) const
{
  Double_t theta =  2 * TMath::ATan(TMath::Exp(-eta));
  if (eta < 0) theta -= TMath::Pi();
  return mult / TMath::Cos(theta);
}

//____________________________________________________________________
void
AliFMDSharingFilter::ScaleHistograms(TList* dir, Int_t nEvents)
{
  if (nEvents <= 0) return;
  TList* d = static_cast<TList*>(dir->FindObject(GetName()));
  if (!d) return;

  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next())))
    o->ScaleHistograms(d, nEvents);
}

//____________________________________________________________________
void
AliFMDSharingFilter::DefineOutput(TList* dir)
{
  TList* d = new TList;
  d->SetName(GetName());
  dir->Add(d);
  TIter    next(&fRingHistos);
  RingHistos* o = 0;
  while ((o = static_cast<RingHistos*>(next()))) {
    o->Output(d);
  }
}
  
//====================================================================
AliFMDSharingFilter::RingHistos::RingHistos()
  : AliForwardUtil::RingHistos(), 
    fBefore(0), 
    fAfter(0), 
    fHits(0),
    fNHits(0)
{}

//____________________________________________________________________
AliFMDSharingFilter::RingHistos::RingHistos(UShort_t d, Char_t r)
  : AliForwardUtil::RingHistos(d,r), 
    fBefore(0), 
    fAfter(0),
    fHits(0),
    fNHits(0)
{
  fBefore = new TH1D(Form("%s_ESD_Eloss", fName.Data()), 
		     Form("Energy loss in %s (reconstruction)", fName.Data()), 
		     1000, 0, 25);
  fAfter  = new TH1D(Form("%s_Ana_Eloss", fName.Data()), 
		     Form("Energy loss in %s (sharing corrected)",
			  fName.Data()), 1000, 0, 25);
  fBefore->SetXTitle("#Delta E/#Delta E_{mip}");
  fBefore->SetYTitle("P(#Delta E/#Delta E_{mip})");
  fBefore->SetFillColor(kRed+1);
  fBefore->SetFillStyle(3001);
  // fBefore->Sumw2();
  fBefore->SetDirectory(0);
  fAfter->SetXTitle("#Delta E/#Delta E_{mip}");
  fAfter->SetYTitle("P(#Delta E/#Delta E_{mip})");
  fAfter->SetFillColor(kBlue+1);
  fAfter->SetFillStyle(3001);
  // fAfter->Sumw2();
  fAfter->SetDirectory(0);

  fHits = new TH1D(Form("%s_Hits", fName.Data()), 
		   Form("Number of hits in %s", fName.Data()), 
		   200, 0, 200000);
  fHits->SetDirectory(0);
  fHits->SetXTitle("# of hits");
  fHits->SetFillColor(kGreen+1);
}
//____________________________________________________________________
AliFMDSharingFilter::RingHistos::RingHistos(const RingHistos& o)
  : AliForwardUtil::RingHistos(o), 
    fBefore(o.fBefore), 
    fAfter(o.fAfter),
    fHits(o.fHits),
    fNHits(o.fNHits)
{}

//____________________________________________________________________
AliFMDSharingFilter::RingHistos&
AliFMDSharingFilter::RingHistos::operator=(const RingHistos& o)
{
  AliForwardUtil::RingHistos::operator=(o);
  fDet = o.fDet;
  fRing = o.fRing;
  
  if (fBefore) delete  fBefore;
  if (fAfter)  delete  fAfter;
  if (fHits)   delete fHits;
  
  fBefore = static_cast<TH1D*>(o.fBefore->Clone());
  fAfter  = static_cast<TH1D*>(o.fAfter->Clone());
  fHits  = static_cast<TH1D*>(o.fHits->Clone());
  
  return *this;
}
//____________________________________________________________________
AliFMDSharingFilter::RingHistos::~RingHistos()
{
  if (fBefore) delete fBefore;
  if (fAfter)  delete fAfter;
  if (fHits)   delete fHits;
}
//____________________________________________________________________
void
AliFMDSharingFilter::RingHistos::Finish()
{
  fHits->Fill(fNHits);
}

//____________________________________________________________________
void
AliFMDSharingFilter::RingHistos::ScaleHistograms(TList* dir, Int_t nEvents)
{
  TList* l = GetOutputList(dir);
  if (!l) return; 

  TH1D* before = static_cast<TH1D*>(l->FindObject(Form("%s_ESD_ELoss",
						       fName.Data())));
  TH1D* after  = static_cast<TH1D*>(l->FindObject(Form("%s_Ana_ELoss",
						       fName.Data())));
  if (before) before->Scale(1./nEvents);
  if (after)  after->Scale(1./nEvents);
}

//____________________________________________________________________
void
AliFMDSharingFilter::RingHistos::Output(TList* dir)
{
  TList* d = DefineOutputList(dir);
  d->Add(fBefore);
  d->Add(fAfter);
  d->Add(fHits);
  dir->Add(d);
}

//____________________________________________________________________
//
// EOF
//
