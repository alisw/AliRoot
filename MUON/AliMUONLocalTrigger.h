#ifndef ALIMUONLOCALTRIGGER_H
#define ALIMUONLOCALTRIGGER_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*    */

#include <TObject.h>

class AliMUONLocalTrigger : public TObject {
 public:
  AliMUONLocalTrigger();
  AliMUONLocalTrigger(const AliMUONLocalTrigger& ); // copy constructor !
  AliMUONLocalTrigger(Int_t *localtr);
  virtual ~AliMUONLocalTrigger(){;}
  AliMUONLocalTrigger& operator=(const AliMUONLocalTrigger& ); 

  Int_t LoCircuit() const {return fLoCircuit;}; 
  Int_t LoStripX() const {return fLoStripX;};    
  Int_t LoDev() const {return fLoDev;};     
  Int_t LoStripY() const {return fLoStripY;};  
  Int_t LoLpt() const {return fLoLpt;};     
  Int_t LoHpt() const {return fLoHpt;};     
  Int_t LoApt() const {return fLoApt;};    

  ClassDef(AliMUONLocalTrigger,1)  // reconstructed Local Trigger object

private:
  Int_t fLoCircuit; // circuit number 
  Int_t fLoStripX;  // X strip in MT11 
  Int_t fLoDev;     // deviation 
  Int_t fLoStripY;  // Y strip in MT11 
  Int_t fLoLpt;     // Low pt  0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
  Int_t fLoHpt;     // High pt 0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
  Int_t fLoApt;     // All pt  0 : nothing, 1 : Minus, 2 : Plus, 3 : Undef
   
};
#endif






