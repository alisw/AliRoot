//
// *** Class AliRsnEvent ***
//
// A container for a collection of AliRsnDaughter objects from an event.
// Contains also the primary vertex, useful for some cuts.
// In order to retrieve easily the tracks which have been identified
// as a specific type and charge, there is an array of indexes which
// allows to avoid to loop on all tracks and have only the neede ones.
//
// authors: A. Pulvirenti (email: alberto.pulvirenti@ct.infn.it)
//          M. Vala (email: martin.vala@cern.ch)
//

#include <TArrayF.h>

#include "AliLog.h"
#include "AliVEvent.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliGenEventHeader.h"
#include "AliAODEvent.h"
#include "AliRsnCutPID.h"
#include "AliESDtrackCuts.h"

#include "AliRsnEvent.h"

ClassImp(AliRsnEvent)

//_____________________________________________________________________________
AliRsnEvent::AliRsnEvent(AliVEvent *ref, AliVEvent *refMC) :
  fRef(ref),
  fRefMC(refMC),
  fLeading(-1)
{
//
// Default constructor.
//
}

//_____________________________________________________________________________
AliRsnEvent::AliRsnEvent(const AliRsnEvent &event) :
  TObject(event),
  fRef(event.fRef),
  fRefMC(event.fRefMC),
  fLeading(event.fLeading)
{
//
// Copy constructor.
//
}

//_____________________________________________________________________________
AliRsnEvent& AliRsnEvent::operator= (const AliRsnEvent & event)
{
//
// Works in the same way as the copy constructor.
//

  (TObject)(*this) = (TObject)event;
  fRef             = event.fRef;
  fRefMC           = event.fRefMC;
  fLeading         = event.fLeading;

  return (*this);
}

//_____________________________________________________________________________
Bool_t AliRsnEvent::SetDaughter(AliRsnDaughter &out, Int_t i, AliRsnDaughter::ERefType type)
{
//
// Using the second and third arguments, retrieves the i-th object in the
// appropriate sample (tracks or V0s) and sets the first reference object
// in order to point to that.
// If a MonteCarlo information is provided, sets the useful informations from there,
// and in case of a V0, sets the 'label' data member only when the two daughters
// of the V0 point to the same mother.
// Returns kFALSE whenever the operation fails (out of range, NULL references).
//

  Int_t result = 0;
  
  if (IsESD() && type == AliRsnDaughter::kTrack) result = SetDaughterESDtrack(out, i);
  if (IsAOD() && type == AliRsnDaughter::kTrack) result = SetDaughterAODtrack(out, i);
  if (IsESD() && type == AliRsnDaughter::kV0   ) result = SetDaughterESDv0   (out, i);
  if (IsAOD() && type == AliRsnDaughter::kV0   ) result = SetDaughterAODv0   (out, i);
  
  return (result == 0);
}

//_____________________________________________________________________________
Bool_t AliRsnEvent::SetDaughterMC(AliRsnDaughter &out, Int_t label)
{
//
// Using the second argument, retrieves the i-th object in the
// MC sample (if present) and assigns the track using only that,
// so that it is considered both as main reference and MC reference.
// (used for MC-only analysis).
//

  if (!fRefMC)
  {
    out.SetBad();
    return kFALSE;
  }
  
  // try to convert into both types
  Int_t        imum;
  AliMCEvent  *esd = GetRefMCESD();
  AliAODEvent *aod = GetRefMCAOD();
  
  // ESD
  if (esd)
  {
    // if the MC track exists, assign it
    AliMCParticle *track = (AliMCParticle*)fRef->GetTrack(label);
    if (!track)
    {
      out.SetBad();
      return kFALSE;
    }
    out.SetRef(track);
    out.SetRefMC(track);
    out.SetLabel(label);
    out.SetGood();
    
    // search for its mother in stack
    imum = track->GetMother();
    if (imum >= 0 && imum < esd->Stack()->GetNtrack())
    {
      TParticle *mum = esd->Stack()->Particle(imum);
      if (mum) out.SetMotherPDG(TMath::Abs(mum->GetPdgCode()));
    }
  }
  
  // AOD
  if (aod)
  {
    // checks that array of MC particles exists
    TClonesArray *mcArray = (TClonesArray*)aod->GetList()->FindObject(AliAODMCParticle::StdBranchName());
    if(!mcArray)
    {
      out.SetBad();
      return kFALSE;
    }
    
    // in this case one has to loop over the sample to find the good one
    TObjArrayIter next(mcArray);
    AliAODMCParticle *part = 0x0;
    while ( (part = (AliAODMCParticle*)next()) )
    {
      if (TMath::Abs(part->GetLabel()) == label)
      {
        // if the MC track exists, assign it
        out.SetRef(part);
        out.SetRefMC(part);
        out.SetLabel(label);
        out.SetGood();
        
        // search for the mother
        imum = part->GetMother();
        if (imum >= 0 && imum < mcArray->GetEntriesFast())
        {
          AliAODMCParticle *mum = (AliAODMCParticle*)mcArray->At(imum);
          if (mum) out.SetMotherPDG(TMath::Abs(mum->GetPdgCode()));
        }
        break;
      }
    }
    return kTRUE;
  }
  
  return kFALSE;
}

//_____________________________________________________________________________
AliRsnDaughter AliRsnEvent::GetDaughter(Int_t i, AliRsnDaughter::ERefType type)
{
//
// Returns a daughter set using same criteria as SetDaughter
//
  
  AliRsnDaughter d; 
  SetDaughter(d, i, type); 
  return d;
}

//_____________________________________________________________________________
AliRsnDaughter AliRsnEvent::GetDaughterMC(Int_t i)
{
//
// Returns a daughter set using same criteria as SetDaughterMC
//
  
  AliRsnDaughter d; 
  SetDaughterMC(d, i); 
  return d;
}

//_____________________________________________________________________________
Int_t AliRsnEvent::GetMultiplicity(AliESDtrackCuts *cuts)
{
//
// Returns event multiplicity as the number of tracks.
// If the argument is not NULL, returns instead the 
// number of tracks passing the cuts hereby defined.
//

  if (!fRef) return 0;
  
  AliESDEvent *esd = GetRefESD();
  if (cuts && esd) return cuts->CountAcceptedTracks(esd); 
  else return fRef->GetNumberOfTracks();
}

//_____________________________________________________________________________
Double_t AliRsnEvent::GetVz()
{
//
// Return Z coord of primary vertex
//

  return fRef->GetPrimaryVertex()->GetZ();
}

//_____________________________________________________________________________
Int_t AliRsnEvent::SelectLeadingParticle
(Double_t ptMin, AliRsnCutPID *cutPID)
{
//
// Searches the collection of all particles with given PID type and charge,
// and returns the one with largest momentum, provided that it is greater than 1st argument.
// If one specifies AliRsnPID::kUnknown as type or AliRsnDaughter::kNoPID as method,
// the check is done over all particles irrespectively of their PID.
// If the sign argument is '+' or '-', the check is done over the particles of that charge,
// otherwise it is done irrespectively of the charge.
//

  Int_t i, nTracks = fRef->GetNumberOfTracks();
  fLeading = -1;
  AliRsnDaughter leading;
  leading.SetBad();

  for (i = 0; i < nTracks; i++) 
  {
    AliRsnDaughter track = GetDaughter(i);
    if (cutPID) if (!cutPID->IsSelected(&track)) continue;
    const AliVParticle *ref = track.GetRef();
    if (ref->Pt() < ptMin) continue;
    //double pt = track.P().Perp();
    //Printf("track %d %g", i, pt);
    if (!leading.IsOK() || ref->Pt() > ptMin)
    {
      fLeading = i;
      //leading = track;
      ptMin = ref->Pt();
    }
  }
  return fLeading;
}

//_________________________________________________________________________________________________
Double_t AliRsnEvent::GetAverageMomentum(Int_t &count, AliRsnCutPID *cutPID)
{
//
// Loops on the list of tracks and computes average total momentum.
//

  Int_t i, nTracks = fRef->GetNumberOfTracks();
  Double_t pmean = 0.0;

  for (i = 0, count = 0; i < nTracks; i++) {
    AliRsnDaughter track = GetDaughter(i);
    if (cutPID) if (!cutPID->IsSelected(&track)) continue;
    pmean += track.P().Mag();
    count++;
  }

  if (count > 0) pmean /= (Double_t)count;
  else pmean = 0.0;

  return pmean;
}

//_____________________________________________________________________________
Bool_t AliRsnEvent::GetAngleDistr
(Double_t &angleMean, Double_t &angleRMS, AliRsnDaughter leading)
{
//
// Takes the leading particle and computes the mean and RMS
// of the distribution of directions of all other tracks
// with respect to the direction of leading particle.
//

  if (!leading.IsOK()) return kFALSE;

  Int_t i, count, nTracks = fRef->GetNumberOfTracks();
  Double_t angle, angle2Mean = 0.0;

  angleMean = angle2Mean = 0.0;

  for (i = 0, count = 0; i < nTracks; i++) {
    AliRsnDaughter trk = GetDaughter(i);
    if (trk.GetID() == leading.GetID()) continue;

    angle = leading.P().Angle(trk.P().Vect());

    angleMean += angle;
    angle2Mean += angle * angle;
    count++;
  }

  if (!count) return kFALSE;

  angleMean /= (Double_t)count;
  angle2Mean /= (Double_t)count;
  angleRMS = TMath::Sqrt(angle2Mean - angleMean * angleMean);

  return kTRUE;
}

//_____________________________________________________________________________
Int_t AliRsnEvent::SetDaughterESDtrack(AliRsnDaughter &out, Int_t i)
{
//
// Setup the first argument to the track identified by the index.
// When available, adds the MC information and references.
// ---
// Version #1: ESD tracks
//
  
  // check 1: index in good range
  if (i < 0 || i >= fRef->GetNumberOfTracks())
  {
    out.SetBad();
    return 1;
  }
  
  // check 2: not NULL object
  AliVTrack *track = (AliVTrack*)fRef->GetTrack(i);
  if (!track)
  {
    out.SetBad();
    return 2;
  }
  
  // assign references of reconstructed track
  Int_t label = TMath::Abs(track->GetLabel());
  out.SetRef(track);
  out.SetLabel(label);
  out.SetGood();
  
  // search for MC track, if available
  AliMCEvent *mc = GetRefMCESD();
  if (!mc) return 0;
  
  // loop on particles using the track label as reference
  // and then assign also the mother type, if found
  AliStack *stack = mc->Stack();
  if (label >= 0 && label < stack->GetNtrack())
  {
    TParticle *part = stack->Particle(label);
    if (part)
    {
      Int_t imum = part->GetFirstMother();
      if (imum >= 0 && imum <= stack->GetNtrack())
      {
        TParticle *mum = stack->Particle(imum);
        if (mum) out.SetMotherPDG(TMath::Abs(mum->GetPdgCode()));
      }
    }
    else
    {
      return 3;
    }
  }
  
  return 0;
}

//_____________________________________________________________________________
Int_t AliRsnEvent::SetDaughterAODtrack(AliRsnDaughter &out, Int_t i)
{
//
// Setup the first argument to the track identified by the index.
// When available, adds the MC information and references.
// ---
// Version #2: AOD tracks
//
  
  // check 1: index in good range
  if (i < 0 || i >= fRef->GetNumberOfTracks())
  {
    out.SetBad();
    return 1;
  }
  
  // check 2: not NULL object
  AliVTrack *track = (AliVTrack*)fRef->GetTrack(i);
  if (!track)
  {
    out.SetBad();
    return 2;
  }
  
  // assign references of reconstructed track
  Int_t label = TMath::Abs(track->GetLabel());
  out.SetRef(track);
  out.SetLabel(label);
  out.SetGood();
  
  // search for MC track, if available
  AliAODEvent *mc = GetRefMCAOD();
  if (!mc) return 0;
  
  // loop on particles using the track label as reference
  // and then assign also the mother type, if found
  TClonesArray *mcArray = (TClonesArray*)mc->GetList()->FindObject(AliAODMCParticle::StdBranchName());
  if(!mcArray) return 0;
  TObjArrayIter next(mcArray);
  AliAODMCParticle *part = 0x0;
  while ( (part = (AliAODMCParticle*)next()) )
  {
    if (TMath::Abs(part->GetLabel()) == label)
    {
      out.SetRefMC(part);
      Int_t imum = part->GetMother();
      if (imum >= 0 && imum <= mcArray->GetEntriesFast())
      {
        AliAODMCParticle *mum = (AliAODMCParticle*)mcArray->At(imum);
        if (mum) out.SetMotherPDG(TMath::Abs(mum->GetPdgCode()));
      }
      break;
    }
  }
  
  return 0;
}

//_____________________________________________________________________________
Int_t AliRsnEvent::SetDaughterESDv0(AliRsnDaughter &out, Int_t i)
{
//
// Setup the first argument to the track identified by the index.
// When available, adds the MC information and references.
// ---
// Version #3: ESD v0
//

  // check 1: index in good range
  if (i > fRef->GetNumberOfV0s())
  {
    out.SetBad();
    return 1;
  }
  
  // check 2: not NULL object
  AliESDEvent *ev = GetRefESD();
  AliESDv0    *v0 = ev->GetV0(i);
  if (!v0)
  {
    out.SetBad();
    return 2;
  }
  
  // assign references of reconstructed track
  out.SetRef(v0);
  out.SetGood();
  
  // this time, assigning label is not trivial,
  // it is done only if MC is present and both
  // daughters come from a true particle
  AliMCEvent  *mc = GetRefMCESD();
  AliESDtrack *tp = ev->GetTrack(v0->GetPindex());
  AliESDtrack *tn = ev->GetTrack(v0->GetNindex());
  if (mc && tp && tn)
  {
    Int_t        lp = TMath::Abs(tp->GetLabel());
    Int_t        ln = TMath::Abs(tn->GetLabel());
    TParticle   *pp = mc->Stack()->Particle(lp);
    TParticle   *pn = mc->Stack()->Particle(ln);
    // if their first mothers are the same, the V0 is true
    // otherwise no label can be assigned
    if (pp->GetFirstMother() == pn->GetFirstMother()) out.SetLabel(pp->GetFirstMother());
  }
  
  return 0;
}

//_____________________________________________________________________________
Int_t AliRsnEvent::SetDaughterAODv0(AliRsnDaughter &out, Int_t i)
{
//
// Setup the first argument to the track identified by the index.
// When available, adds the MC information and references.
// ---
// Version #4: AOD v0
//

  // check 1: index in good range
  if (i > fRef->GetNumberOfV0s())
  {
    out.SetBad();
    return 1;
  }
  
  // check 2: not NULL object
  AliAODEvent *ev = GetRefAOD();
  AliAODv0    *v0 = ev->GetV0(i);
  if (!v0)
  {
    out.SetBad();
    return 2;
  }
  
  // assign references of reconstructed track
  out.SetRef(v0);
  out.SetGood();
  out.SetLabel(-1);
  
  // this time, assigning label is not trivial,
  // it is done only if MC is present and both
  // daughters come from a true particle
  TClonesArray *mcArray = (TClonesArray*)ev->GetList()->FindObject(AliAODMCParticle::StdBranchName());
  AliAODTrack  *tp = ev->GetTrack(v0->GetPosID());
  AliAODTrack  *tn = ev->GetTrack(v0->GetNegID());
  if (mcArray && tp && tn)
  {
    Int_t        lp = TMath::Abs(tp->GetLabel());
    Int_t        ln = TMath::Abs(tn->GetLabel());
    // loop on array to find MC daughters
    AliAODMCParticle *pp = 0x0, *pn = 0x0;
    TObjArrayIter next(mcArray);
    AliAODMCParticle *part = 0x0;
    while ( (part = (AliAODMCParticle*)next()) )
    {
      if (TMath::Abs(part->GetLabel()) == lp) pp = (AliAODMCParticle*)mcArray->IndexOf(part);
      if (TMath::Abs(part->GetLabel()) == ln) pn = (AliAODMCParticle*)mcArray->IndexOf(part);
    }
    // assign a MC reference and a label only to true V0s
    if (pp->GetMother() == pn->GetMother())
    {
      AliAODMCParticle *mcv0 = (AliAODMCParticle*)mcArray->At(pp->GetMother());
      out.SetRefMC(mcv0);
      out.SetLabel(pp->GetMother());
      Int_t imum = mcv0->GetMother();
      if (imum >= 0 && imum <= mcArray->GetEntriesFast())
      {
        AliAODMCParticle *mum = (AliAODMCParticle*)mcArray->At(imum);
        if (mum) out.SetMotherPDG(TMath::Abs(mum->GetPdgCode()));
      }
    }
  }
  
  return 0;
}
