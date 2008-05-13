#ifndef ALIITSQASDDCHECKER_H
#define ALIITSQASDDCHECKER_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
//  Checks the quality assurance. 
//  By comparing with reference data
//  INFN Torino
//  P. Cerello - apr 2008
//


// --- ROOT system ---
class TFile ; 
class TH2F ;  

// --- AliRoot header files ---
#include "AliQA.h"
#include "AliQACheckerBase.h"
#include "AliITSQAChecker.h"
class AliITSLoader ; 

class AliITSQASDDChecker: public TObject {

public:
  AliITSQASDDChecker() {;}          // ctor
  AliITSQASDDChecker& operator = (const AliITSQASDDChecker& qac) ; //operator =
  virtual ~AliITSQASDDChecker() {;} // dtor
  const Double_t Check(AliQA::ALITASK_t /*index*/, TObjArray * /*list*/, Int_t SubDetOffset);

private:
  AliITSQASDDChecker(const AliITSQASDDChecker& /*qac*/):TObject() {;} // cpy ctor   
  
  ClassDef(AliITSQASDDChecker,1)  // description 

};

#endif // AliITSQASDDChecker_H
