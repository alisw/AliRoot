/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
* See cxx source for full Copyright notice */
/* $Id$ */

/*****************************************************************
  AliFlowEventSimple: A simple event 
  for flow analysis                  
                                     
  origin: Naomi van der Kolk (kolk@nikhef.nl)           
          Ante Bilandzic     (anteb@nikhef.nl)         
          Raimond Snellings  (Raimond.Snellings@nikhef.nl)    
  mods:   Mikolaj Krzewicki  (mikolaj.krzewicki@cern.ch)
*****************************************************************/

#ifndef ALIFLOWEVENTSIMPLE_H
#define ALIFLOWEVENTSIMPLE_H

#include "TObject.h"
#include "TParameter.h"
class TTree;
class AliFlowVector;
class AliFlowTrackSimple;
class AliFlowTrackSimpleCuts;

class AliFlowEventSimple: public TObject {

 public:
  AliFlowEventSimple();
  AliFlowEventSimple(Int_t aLenght);
  AliFlowEventSimple(TTree* anInput, const AliFlowTrackSimpleCuts* rpCuts, const AliFlowTrackSimpleCuts* poiCuts);
  AliFlowEventSimple(const AliFlowEventSimple& anEvent);
  AliFlowEventSimple& operator=(const AliFlowEventSimple& anEvent);
  virtual  ~AliFlowEventSimple();

  Bool_t  IsFolder() const {return kTRUE;};
  void    Browse(TBrowser *b); 
  void    Print(Option_t* option = "") const;      //method to print stats
  
  Int_t    NumberOfTracks() const                   { return fNumberOfTracks; }
  void     SetNumberOfTracks(Int_t nr)              { fNumberOfTracks = nr; }
  Int_t    GetEventNSelTracksRP() const             { return fEventNSelTracksRP; } 
  void     SetEventNSelTracksRP(Int_t nr)           { fEventNSelTracksRP = nr; }  
  Double_t GetMCReactionPlaneAngle() const          { return fMCReactionPlaneAngle; }
  void     SetMCReactionPlaneAngle(Double_t fPhiRP) { fMCReactionPlaneAngle=fPhiRP; fMCReactionPlaneAngleIsSet=kTRUE; }

  void CloneTracks(Int_t n);
 
  AliFlowTrackSimple* GetTrack(Int_t i);
  TObjArray* TrackCollection() const                { return fTrackCollection; }
 
  AliFlowVector GetQ(Int_t n=2, TList *weightsList=NULL, Bool_t usePhiWeights=kFALSE, Bool_t usePtWeights=kFALSE, Bool_t useEtaWeights=kFALSE);
  void Get2Qsub(AliFlowVector* Qarray, Int_t n=2, TList *weightsList=NULL, Bool_t usePhiWeights=kFALSE, Bool_t usePtWeights=kFALSE, Bool_t useEtaWeights=kFALSE);  

 private:
  TObjArray*              fTrackCollection;           // collection of tracks
  Int_t                   fNumberOfTracks;            // number of tracks
  Int_t                   fEventNSelTracksRP;         // number of tracks that have passed the RP selection
  Double_t                fMCReactionPlaneAngle;      // the angle of the reaction plane from the MC truth
  Bool_t                  fMCReactionPlaneAngleIsSet; // did we set it from MC?
  TParameter<Int_t>*      fNumberOfTracksWrap;        //! number of tracks in TBrowser
  TParameter<Int_t>*      fEventNSelTracksRPWrap;     //! number of tracks that have passed the RP selection in TBrowser
  TParameter<Double_t>*   fMCReactionPlaneAngleWrap;   //! the angle of the reaction plane from the MC truth in TBrowser

  ClassDef(AliFlowEventSimple,1)                       // simplified event used in flow analysis 
};

#endif


