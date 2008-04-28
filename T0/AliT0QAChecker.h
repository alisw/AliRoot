#ifndef ALIT0QACHECKER_H
#define ALIT0QACHECKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//
//  Checks the quality assurance. 
//  By comparing with reference data
//  Skeleton for T0
//


// --- ROOT system ---
class TFile ; 
class TH1F ; 

// --- Standard library ---

// --- AliRoot header files ---
#include "AliQACheckerBase.h"

class AliT0QAChecker: public AliQACheckerBase {

public:
  AliT0QAChecker() : AliQACheckerBase("T0","T0 Quality Assurance Data Checker") {;}          // ctor
  AliT0QAChecker(const AliT0QAChecker& qac) : AliQACheckerBase(qac.GetName(), qac.GetTitle()) {;} // ctor   
  AliT0QAChecker& operator = (const AliT0QAChecker& qac) ;
  virtual ~AliT0QAChecker() {;} // dtor

private:
  //  virtual const Double_t Check(TObjArray * list) ;
  virtual const Double_t Check(AliQA::ALITASK_t index, TObjArray * list) ;
  virtual const Double_t Check() {return 0.;} ;
  

  ClassDef(AliT0QAChecker,1)  // description 

};

#endif // AliT0QAChecker_H
