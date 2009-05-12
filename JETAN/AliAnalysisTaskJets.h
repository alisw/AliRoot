#ifndef ALIANALYSISTASKJETS_H
#define ALIANALYSISTASKJETS_H
 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
#include "AliAnalysisTaskSE.h"
class AliJetFinder;
class AliESDEvent;
class TTree;
class TChain;
class AliAODEvent;
class AliJetHistos;


class AliAnalysisTaskJets : public AliAnalysisTaskSE
{
 public:
    AliAnalysisTaskJets();
    AliAnalysisTaskJets(const char* name);
    AliAnalysisTaskJets(const char* name, TChain* chain);
    virtual ~AliAnalysisTaskJets() {;}
    // Implementation of interface methods
    virtual void UserCreateOutputObjects();
    virtual void Init();
    virtual void LocalInit() {Init();}
    virtual void UserExec(Option_t *option);
    virtual void SetConfigFile(const char *c){fConfigFile = c;}
    virtual void SetNonStdBranch(const char *c){fNonStdBranch = c;}
    virtual void Terminate(Option_t *option);

 private:
  AliAnalysisTaskJets(const AliAnalysisTaskJets &det);
  AliAnalysisTaskJets &operator=(const AliAnalysisTaskJets &det);
    
 private:
  TString       fConfigFile;      // the name of the ConfigFile
  TString       fNonStdBranch;    // the name of the non-std branch name//commented by syssy
  AliJetFinder* fJetFinder;       //  Pointer to the jet finder 
  AliJetHistos* fHistos;          //  Histogram manager class
  TList*        fListOfHistos;    //  Output list of histograms
  TChain*       fChain;           //  Chain 
  Int_t         fOpt;             //  Detector configuration used

  ClassDef(AliAnalysisTaskJets, 3); // Analysis task for standard jet analysis
};
 
#endif
