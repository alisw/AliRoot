#ifndef ALICASCADEVERTEXER_H
#define ALICASCADEVERTEXER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//------------------------------------------------------------------
//                    Cascade Vertexer Class
//
//    Origin: Christian Kuhn, IReS, Strasbourg, christian.kuhn@ires.in2p3.fr
//------------------------------------------------------------------

#include "TObject.h"

class TFile;
class AliITStrackV2;
class AliV0vertex;

//_____________________________________________________________________________
class AliCascadeVertexer : public TObject {
public:
  AliCascadeVertexer();
  AliCascadeVertexer(const Double_t cuts[8]);
  void SetCuts(const Double_t cuts[8]);

  Int_t V0sTracks2CascadeVertices(const TFile *in, TFile *out);
  Double_t PropagateToDCA(AliV0vertex *vtx, AliITStrackV2 *trk);

  void GetCuts(Double_t cuts[8]) const;

private:
  Double_t fChi2max;    // maximal allowed chi2 
  Double_t fDV0min;     // min. allowed V0 impact parameter
  Double_t fMassWin;    // window around the Lambda mass
  Double_t fDBachMin;   // min. allowed bachelor impact parameter
  Double_t fDCAmax;     // maximal allowed DCA between the V0 and the track 
  Double_t fCPAmax;     // maximal allowed cosine of the cascade pointing angle
  Double_t fRmin, fRmax;// max & min radii of the fiducial volume
  
  ClassDef(AliCascadeVertexer,1)  // cascade verterxer 
};

inline AliCascadeVertexer::AliCascadeVertexer() {
 fChi2max=33.; 
 fDV0min=0.015; fMassWin=0.05; fDBachMin=0.015;
 fDCAmax=0.01;  fCPAmax=0.025; 
 fRmin=0.5;     fRmax=2.5; 
}

inline AliCascadeVertexer::AliCascadeVertexer(const Double_t cuts[8]) {
  fChi2max=cuts[0]; 
  fDV0min=cuts[1];   fMassWin=cuts[2]; fDBachMin=cuts[3];
  fDCAmax=cuts[4];   fCPAmax=cuts[5];
  fRmin=cuts[6];     fRmax=cuts[7]; 
}

inline void AliCascadeVertexer::SetCuts(const Double_t cuts[8]) {
  fChi2max=cuts[0]; 
  fDV0min=cuts[1];   fMassWin=cuts[2]; fDBachMin=cuts[3];
  fDCAmax=cuts[4];   fCPAmax=cuts[5];
  fRmin=cuts[6];     fRmax=cuts[7]; 
}

inline void AliCascadeVertexer::GetCuts(Double_t cuts[8]) const {
  cuts[0]=fChi2max; 
  cuts[1]=fDV0min;   cuts[2]=fMassWin;  cuts[3]=fDBachMin;
  cuts[4]=fDCAmax;   cuts[5]=fCPAmax;
  cuts[6]=fRmin;     cuts[7]=fRmax; 
}

#endif


