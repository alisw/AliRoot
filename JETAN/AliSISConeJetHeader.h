#ifndef ALISISCONEJETHEADER_H
#define ALISISCONEJETHEADER_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
//---------------------------------------------------------------------
// SISCone (FastJet v2.3.4) finder algorithm interface
// Finder Header Class 
// Author: swensy.jangal@ires.in2p3.fr
//---------------------------------------------------------------------
 

#include "fastjet/AreaDefinition.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/JetDefinition.hh"
#include "AliJetHeader.h"
 
class AliSISConeJetHeader : public AliJetHeader
{
 public:
 
  AliSISConeJetHeader();
  virtual ~AliSISConeJetHeader() { }

  // Getters
  Bool_t                       GetBGMode()                     {return fBGMode;}
  Bool_t                       GetDebug()                      {return fDebug;}

  Int_t                        GetActiveAreaRepeats()          {return fActiveAreaRepeats;}
  Int_t                        GetAreaTypeNumber()             {return fAreaTypeNumber;}
  Int_t                        GetBGAlgorithm()                {return fBGAlgo;}        
  Int_t                        GetNPassMax()                   {return fNPassMax;}
  Int_t                        GetSplitMergeScale()            {return fSplitMergeScaleNumber;}

  Double_t                     GetGhostEtaMax()                {return fGhostEtaMax;}
  Double_t                     GetGhostArea()                  {return fGhostArea;}
  Double_t                     GetEffectiveRFact()             {return fEffectiveRFact;}
  Double_t                     GetRapMax()                     {return fRapMax;}
  Double_t                     GetRapMin()                     {return fRapMin;}
  Double_t                     GetPhiMax()                     {return fPhiMax;}
  Double_t                     GetPhiMin()                     {return fPhiMin;}
  Double_t                     GetConeRadius()                 {return fConeRadius;}
  Double_t                     GetOverlapThreshold()           {return fOverlapThreshold;}
  Double_t                     GetPtProtojetMin()              {return fPtProtoJetMin;}
  Double_t                     GetCaching()                    {return fCaching;}
  Double_t                     GetSplitMergeStoppingScale()    {return fSplitMergeStoppingScale;}
  Double_t                     GetMinJetPt()                   {return fMinJetPt;}  
  Double_t                     GetGridScatter()                {return fGridScatter;}
  Double_t                     GetKtScatter()                  {return fKtScatter;}
  Double_t                     GetMeanGhostKt()                {return fMeanGhostKt;}

  // Setters
  void SetBGAlgorithm(Int_t value)                     {fBGAlgo = value;}
  void SetBGMode(Bool_t value)                         {fBGMode = value;}
  void SetDebug(Bool_t value)                          {fDebug = value;}
  void SetCaching(Bool_t value)                        {fCaching = value;}
  void SetComment(TString com)                         {fComment=com;}
  void SetComment(const char* com)                     {AliJetHeader::SetComment(com);}
  void SetGhostEtaMax(Double_t f)                      {fGhostEtaMax = f;}
  void SetGhostArea(Double_t f)                        {fGhostArea = f;}
  void SetActiveAreaRepeats(Int_t f)                   {fActiveAreaRepeats =f;}
  void SetAreaTypeNumber(Int_t f)                      {fAreaTypeNumber = f;}
  void SetEffectiveRFact(Double_t value)               {fEffectiveRFact = value;}       
  void SetConeRadius(Double_t value)                   {fConeRadius = value;}
  void SetMinJetPt(Double_t value)                     {fMinJetPt = value;}
  void SetNPassMax(Int_t value)                        {fNPassMax = value;}
  void SetOverlapThreshold(Double_t value)             {fOverlapThreshold = value;}
  void SetPhiRange(Double_t fmin, Double_t fmax)       {fPhiMin = fmin; fPhiMax = fmax;}
  void SetPtProtojetMin(Double_t value)                {fPtProtoJetMin = value;}
  void SetRapRange(Double_t fmin, Double_t fmax)       {fRapMin = fmin; fRapMax = fmax;}
  void SetSplitMergeScale(Int_t value)                 {fSplitMergeScaleNumber = value;}
  void SetSplitMergeStoppingScale(Double_t value)      {fSplitMergeStoppingScale = value;}	  
  void SetGridScatter(Double_t value)                  {fGridScatter = value;}
  void SetKtScatter(Double_t value)                    {fKtScatter = value;}
  void SetMeanGhostKt(Double_t value)                  {fMeanGhostKt = value;}

  // others
  void PrintParameters() const;

 protected:


  Int_t    fActiveAreaRepeats;        // How many times do you want to caculate active areas?
  Int_t    fAreaTypeNumber;           // Kind of area
  Int_t    fBGAlgo;                   // Algorithm for rho calculus
  Bool_t   fBGMode;                   // Do we subtract BG or not?
  Bool_t   fCaching;                  // Do we record found cones for this set of data?
  Double_t fConeRadius;               // Cone radius
  Bool_t   fDebug;                    // Debug option
  Double_t fEffectiveRFact;           // Radius for Voronoi diagram
  Double_t fGhostEtaMax;              // Maximum eta in which a ghost can be generated
  Double_t fGhostArea;                // Area of one ghost
  Double_t fGridScatter;              // fractional random fluctuations of the position of the ghosts on the y-phi grid
  Double_t fKtScatter;                // fractional random fluctuations of the tranverse momentum of the ghosts on the y-phi grid
  Double_t fMeanGhostKt;              // average transverse momentum of the ghosts.
  Double_t fMinJetPt;                 // Jet minimum energy
  Int_t    fNPassMax;                 // maximum number of passes
  Double_t fOverlapThreshold;         // overlap parameter
  Double_t fPhiMax, fPhiMin;          // Phi range
  Double_t fPtProtoJetMin;            // pT min of protojets
  Double_t fRapMax, fRapMin;          // Eta range
  Int_t    fSplitMergeScaleNumber;    // Kind of recombination in split/merge procedure, there's only one
  Double_t fSplitMergeStoppingScale;  // Stopping scale for split/merge procedure in case of area calculus

  ClassDef(AliSISConeJetHeader,2)
};
 
#endif
