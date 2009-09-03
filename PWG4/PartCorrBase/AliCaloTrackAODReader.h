#ifndef ALICALOTRACKAODREADER_H
#define ALICALOTRACKAODREADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */
/* $Id: $ */

//_________________________________________________________________________
// Class for reading data (AODs) in order to do prompt gamma or other particle
// identification and correlations.
// Mixing analysis can be done, input AOD with events
// is opened in the AliCaloTrackReader::Init()
//
//
// -- Author: Gustavo Conesa (INFN-LNF)

// --- ROOT system --- 

// --- AliRoot system ---
#include "AliCaloTrackReader.h" 

class AliCaloTrackAODReader : public AliCaloTrackReader {
	
public: 
	
  AliCaloTrackAODReader() ; // ctor
  AliCaloTrackAODReader(const AliCaloTrackAODReader & g)  ; // cpy ctor
  //AliCaloTrackAODReader & operator = (const AliCaloTrackAODReader & g) ;//cpy assignment
  virtual ~AliCaloTrackAODReader() {;} //virtual dtor
  
  void FillInputCTS()   ;
  void FillInputEMCAL() ;
  void FillInputPHOS()  ;
  void FillInputEMCALCells() ;
  void FillInputPHOSCells()  ;
	
  Double_t GetBField() const;

  void GetSecondInputAODVertex(Double_t v[3]) const ;
  void GetVertex(Double_t v[3]) const ;

  void SwitchOnWriteAOD()  {fWriteOutputAOD = kTRUE;}
  void SwitchOffWriteAOD() {fWriteOutputAOD = kFALSE;}
  
  void SetInputOutputMCEvent(AliVEvent* esd, AliAODEvent* aod, AliMCEvent* mc) ; 
  
private:
	
  Bool_t	fWriteOutputAOD; //Write selected tracks and caloclusters in output AOD
	
  ClassDef(AliCaloTrackAODReader,2)
} ;

#endif //ALICALOTRACKAODREADER_H



