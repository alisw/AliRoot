#ifndef AliCaloTrackAODReader_H
#define AliCaloTrackAODReader_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id: $ */

/* History of cvs commits:
 *
 * $Log$
 *
 */

//_________________________________________________________________________
// Class for reading data (AODs) in order to do prompt gamma or other particle
// identification and correlations
//
//
// -- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system --- 

// --- AliRoot system ---
#include "AliCaloTrackReader.h" 

class AliCaloTrackAODReader : public AliCaloTrackReader {
  
 public: 
  
  AliCaloTrackAODReader() ; // ctor
  AliCaloTrackAODReader(const AliCaloTrackAODReader & g) ; // cpy ctor
  AliCaloTrackAODReader & operator = (const AliCaloTrackAODReader & g) ;//cpy assignment
  virtual ~AliCaloTrackAODReader() {;} //virtual dtor
  
  void FillInputCTS();
  void FillInputEMCAL();
  void FillInputPHOS();
  
  void FillInputEMCALCells();
  void FillInputPHOSCells();

  void GetVertex(Double_t v[3]) ;
  
  void SetInputEvent(TObject* esd, TObject* aod, TObject* mc) ; 
  
  
  ClassDef(AliCaloTrackAODReader,1)
    } ;


#endif //AliCaloTrackAODReader_H



