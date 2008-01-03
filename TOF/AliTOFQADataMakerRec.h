#ifndef AliTOFQADataMakerRec_H
#define AliTOFQADataMakerRec_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////////////////////////
//                                                                // 
//  Produces the data needed to calculate the quality assurance.  //
//    All data must be mergeable objects.                         //
//    S. Arcelli                                                  //
//                                                                // 
////////////////////////////////////////////////////////////////////


#include "AliQADataMakerRec.h"
class AliTOFQADataMakerRec: public AliQADataMakerRec {

public:
  AliTOFQADataMakerRec() ;          // ctor
  AliTOFQADataMakerRec(const AliTOFQADataMakerRec& qadm) ;   
  AliTOFQADataMakerRec& operator = (const AliTOFQADataMakerRec& qadm) ;
  virtual ~AliTOFQADataMakerRec() {;} // dtor
  
private:
  virtual void   InitESDs() ; 
  virtual void   InitRecPoints() ; 
  virtual void   InitRaws() ; 
  virtual void   MakeESDs(AliESDEvent * esd) ;
  virtual void   MakeRecPoints(TTree * recTree) ; 
  virtual void   MakeRaws(AliRawReader* rawReader) ; 
  virtual void   StartOfDetectorCycle() ; 
  virtual void   EndOfDetectorCycle(AliQA::TASKINDEX task, TObjArray * list) ;
  virtual void   GetMapIndeces(Int_t *in, Int_t *out) ; 

  ClassDef(AliTOFQADataMakerRec,1)  // description 

};

#endif // AliTOFQADataMakerRec_H
