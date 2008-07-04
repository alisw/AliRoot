#ifndef ALIZDCQACHECKER_H
#define ALIZDCQACHECKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//////////////////////////////////////////////////////
//  						    //
//  Checks the quality assurance.                   //
//  C. Oppedisano Chiara.Oppedisano@to.infn.it      //
//  						    //
//////////////////////////////////////////////////////

#include "AliQA.h"
#include "AliQACheckerBase.h"

class AliZDCQAChecker: public AliQACheckerBase {

public:
  AliZDCQAChecker() : AliQACheckerBase("ZDC","ZDC Quality Assurance Data Maker") {;}          // ctor
  AliZDCQAChecker(const AliZDCQAChecker& qac) : AliQACheckerBase(qac.GetName(), qac.GetTitle()) {;} // cpy ctor   
  virtual ~AliZDCQAChecker() {;} // dtor

 protected:

  virtual const Double_t Check(AliQA::ALITASK_t index, TObjArray * list);
  virtual const Double_t Check(AliQA::ALITASK_t /*index*/) {return 0.5;} 
  
  ClassDef(AliZDCQAChecker,1)  // description 

};

#endif // AliZDCQAChecker_H
