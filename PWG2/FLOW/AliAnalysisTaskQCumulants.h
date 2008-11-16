/* 
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. 
 * See cxx source for full Copyright notice 
 * $Id$ 
 */

/**************************************
 * analysis task for Q-cumulants      * 
 *                                    * 
 * authors: Naomi van der Kolk        *
 *           (kolk@nikhef.nl)         *  
 *          Raimond Snellings         *
 *           (snelling@nikhef.nl)     * 
 *          Ante Bilandzic            *
 *           (anteb@nikhef.nl)        * 
 * ***********************************/

#ifndef AliAnalysisTaskQCumulants_H
#define AliAnalysisTaskQCumulants_H

#include "AliAnalysisTask.h"

class AliESDEvent;
class AliAODEvent;
class AliCFManager;
class AliFlowAnalysisWithQCumulants;
class AliFlowEventSimpleMaker;
class TFile;

//================================================================================================================

class AliAnalysisTaskQCumulants : public AliAnalysisTask{
 public:
  AliAnalysisTaskQCumulants();
  AliAnalysisTaskQCumulants(const char *name, Bool_t QAon = kFALSE);
  virtual ~AliAnalysisTaskQCumulants(){}; 
  
  virtual void   ConnectInputData(Option_t *);
  virtual void   CreateOutputObjects();
  virtual void   Exec(Option_t *option);
  virtual void   Terminate(Option_t *);
  
  void           SetAnalysisType(TString type) {this->fAnalysisType = type;}
  TString GetAnalysisType() const {return this->fAnalysisType;}
  
  void SetCFManager1(AliCFManager* cfmgr) {this->fCFManager1 = cfmgr;} 
  AliCFManager* GetCFManager1()           {return this->fCFManager1;}
  void SetCFManager2(AliCFManager* cfmgr) {this->fCFManager2 = cfmgr;} 
  AliCFManager* GetCFManager2()           {return this->fCFManager2;} 
  void          SetQAList1(TList* list)   {this->fQAInt = list; }
  TList*        GetQAList1()              {return this->fQAInt; }
  void          SetQAList2(TList* list)   {this->fQADiff = list; }
  TList*        GetQAList2()              {return this->fQADiff; }
  void          SetQAOn(Bool_t kt)        {this->fQA = kt; }
  Bool_t        GetQAOn()                 {return this->fQA; }
 
 private:
  AliAnalysisTaskQCumulants(const AliAnalysisTaskQCumulants& aatqc);
  AliAnalysisTaskQCumulants& operator=(const AliAnalysisTaskQCumulants& aatqc);

  AliESDEvent *fESD;                      //ESD object
  AliAODEvent* fAOD;                      //AOD object
  AliFlowAnalysisWithQCumulants* fQCA;    //Q-cumulant Analysis (QCA) object
  AliFlowEventSimpleMaker* fEventMaker;   //FlowEventSimple maker object
  TString fAnalysisType;                  //string to select which kind of input to analyse (ESD, AOD or MC)
  AliCFManager* fCFManager1;              //correction framework manager
  AliCFManager* fCFManager2;              //correction framework manager
  TList  *fListHistos;                    //collection of output 
     
  TList*        fQAInt;                   // QA histogram list
  TList*        fQADiff;                  // QA histogram list

  Bool_t fQA;                             // flag to set the filling of the QA hostograms  
     
  ClassDef(AliAnalysisTaskQCumulants, 1); 
};

//================================================================================================================

#endif











