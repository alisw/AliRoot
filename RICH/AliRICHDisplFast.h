#ifndef AliRICHDisplFast_h
#define AliRICHDisplFast_h

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TTask.h>
#include "AliRICH.h"
#include <AliRun.h>

class AliRICH;

class AliRICHDisplFast : public TTask 
{
public :
              AliRICHDisplFast() {;}
  virtual    ~AliRICHDisplFast() {;}      
  static void DrawSectors();          //Draw sectors in plot 
  void        Exec();                 //virtual do the main job
protected:  
  ClassDef(AliRICHDisplFast,0)        //Utility class to draw the current event topology
};
    
#endif // #ifdef AliRICHDisplFast_cxx

