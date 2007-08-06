// $Header$

#ifndef ALIEVE_JetPlane_H
#define ALIEVE_JetPlane_H

#include <Reve/Reve.h>
#include <Reve/RenderElement.h>
#include <Reve/ZTrans.h>

#include <TAtt3D.h>
#include <TAttBBox.h>

#include <AliAODJet.h>
#include <AliAODTrack.h>

#include <vector>

namespace Alieve {

class JetPlane : public Reve::RenderElementList,
			      public TAtt3D,
			      public TAttBBox
{
  friend class JetPlaneGL;

private:
  JetPlane(const JetPlane&);            // Not implemented
  JetPlane& operator=(const JetPlane&); // Not implemented

protected:
  Float_t fMinEta;
  Float_t fMaxEta;
  Float_t fMinPhi;
  Float_t fMaxPhi;
  Int_t   fNEtaDiv;
  Int_t   fNPhiDiv;

  Float_t fEtaScale;
  Float_t fPhiScale;
  Float_t fEnergyScale;
  Float_t fEnergyColorScale;

  Color_t fGridColor;

  Reve::ZTrans  fHMTrans;

  std::vector<AliAODJet>   fJets;
  std::vector<AliAODTrack> fTracks;

  Bool_t                 fRnrJets;
  Bool_t                 fRnrTracks;
  Bool_t                 fOneSelection;
  Bool_t                 fTwoSelection;

  AliAODJet             *fJet1,   *fJet2;
  AliAODTrack           *fTrack1, *fTrack2;

  Int_t                  fSelectionFlag;

  static Bool_t fgOneMomentumXYZ;
  static Bool_t fgOneMomentumPhiTheta;
  static Bool_t fgOneEta;
  static Bool_t fgOneE;
  static Bool_t fgOneChgMass;

public:
  JetPlane(Int_t iev);
  virtual ~JetPlane() {}

  void AddJet(AliAODJet jet);
  void AddTrack(AliAODTrack track);

  Int_t GetNEtaDiv() const   { return fNEtaDiv; }
  void  SetNEtaDiv(Int_t r) { fNEtaDiv = r; }

  Int_t GetNPhiDiv() const   { return fNPhiDiv; }
  void  SetNPhiDiv(Int_t r) { fNPhiDiv = r; }

  Bool_t GetRnrJets() const   { return fRnrJets; }
  void   SetRnrJets(Bool_t r) { fRnrJets = r; }

  Bool_t GetRnrTracks() const   { return fRnrTracks; }
  void   SetRnrTracks(Bool_t r) { fRnrTracks = r; }

  Bool_t GetOneSelection() const   { return fOneSelection; }
  void   SetOneSelection(Bool_t r) { fOneSelection = r; }

  Bool_t GetTwoSelection() const   { return fTwoSelection; }
  void   SetTwoSelection(Bool_t r) { fTwoSelection = r; }

  Float_t GetEnergyScale() const { return fEnergyScale; }
  void    SetEnergyScale(Float_t s) { fEnergyScale = s; }

  Float_t GetEnergyColorScale() const { return fEnergyColorScale; }
  void    SetEnergyColorScale(Float_t s) { fEnergyColorScale = s; }

  const AliAODJet& GetJet1() const { return *fJet1; }
  const AliAODJet& GetJet2() const { return *fJet2; }
  const AliAODTrack& GetTrack1() const { return *fTrack1; }
  const AliAODTrack& GetTrack2() const { return *fTrack2; }

  void    SetJet1(AliAODJet* s) { fJet1 = s; }
  void    SetJet2(AliAODJet* s) { fJet2 = s; }
  void    SetTrack1(AliAODTrack* s) { fTrack1 = s; }
  void    SetTrack2(AliAODTrack* s) { fTrack2 = s; }

  void    SetSelectionFlag(Int_t s) { fSelectionFlag = s;}

  virtual Bool_t  CanEditMainColor()   { return kTRUE; }

  virtual Bool_t  CanEditMainHMTrans() { return kTRUE; }
  virtual Reve::ZTrans* PtrMainHMTrans()     { return &fHMTrans; }

  virtual void ComputeBBox();
  virtual void Paint(Option_t* option = "");

  Reve::ZTrans& RefHMTrans() { return fHMTrans; }
  void SetTransMatrix(Double_t* carr)        { fHMTrans.SetFrom(carr); }
  void SetTransMatrix(const TGeoMatrix& mat) { fHMTrans.SetFrom(mat);  }

  ClassDef(JetPlane, 1);
}; // endclass JetPlane

}

#endif
