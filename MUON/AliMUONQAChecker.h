#ifndef ALIMUONQACHECKER_H
#define ALIMUONQACHECKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

/// \ingroup rec 
/// \class AliMUONQAChecker
/// \brief Implementation of AliQACheckerBase for MCH and MTR
///
//  Author: Laurent Aphecetche

// --- AliRoot header files ---
#include "AliQACheckerBase.h"

class AliMUONQAChecker: public AliQACheckerBase {

public:
  AliMUONQAChecker();
  AliMUONQAChecker(const AliMUONQAChecker& qac);
  virtual ~AliMUONQAChecker();

protected:

	virtual const Double_t Check(AliQA::ALITASK_t index) ;
  virtual const Double_t Check(AliQA::ALITASK_t index, TObjArray * list) ;
  virtual void SetQA(AliQA::ALITASK_t index, const Double_t value) const ;	
	
  const Double_t CheckRaws(TObjArray* list);
  
private:
  
  ClassDef(AliMUONQAChecker,1)  // MUON quality assurance checker

};
#endif 
