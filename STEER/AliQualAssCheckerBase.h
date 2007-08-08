#ifndef ALIQUALASSCHECKERBASE_H
#define ALIQUALASSCHECKERBASE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

/*
  Base class for detectors quality assurance checkers 
  Compares Data made by QualAssDataMakers with reference data
  Y. Schutz CERN August 2007
*/


// --- ROOT system ---
#include <TNamed.h>
class TFile ; 

// --- Standard library ---

// --- AliRoot header files ---

class AliQualAssCheckerBase: public TNamed {

public:
  AliQualAssCheckerBase(const char * name = "", const char * title = "") ;          // ctor
  AliQualAssCheckerBase(const AliQualAssCheckerBase& qac) ;   
  AliQualAssCheckerBase& operator = (const AliQualAssCheckerBase& qac) ;
  virtual ~AliQualAssCheckerBase() {;} // dtor

  void   Exec(const Option_t * = ""); 

protected:
  void Init() ; 
  virtual const Double_t Check(const Option_t *) {AliInfo("To be implemented by detectors") ; } 
  const Double_t DiffC(const TH1 * href, const TH1 * hin) const ;   
  const Double_t DiffK(const TH1 * href, const TH1 * hin) const ;   
  void           Finish() const ; 

  TFile * fData ;                //! data root file produced by the QualAssDataMaker
  TDirectory * fDetectorDir ;    //! directory for the given detector in the data file
  TFile * fRef   ;               //! reference root file

  ClassDef(AliQualAssCheckerBase,1)  // description 

};

#endif // AliQUALASSCHECKERBASE_H
