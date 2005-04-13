#ifndef ALITASKLOADER_H
#define ALITASKLOADER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////
//                                        //
//  class AliTaskLoader                   //
//                                        //
//                                        //
////////////////////////////////////////////

/* $Id$ */

#include "AliBaseLoader.h"
 
class AliTaskLoader: public AliBaseLoader
 {
  public:
    AliTaskLoader():fParentalTask(0x0){};
    AliTaskLoader(const TString& name, AliDataLoader* dl, TTask* parentaltask, Bool_t storeontop = kFALSE);
    AliTaskLoader(const AliTaskLoader& source);
    AliTaskLoader& operator=(const AliTaskLoader& source);
    virtual ~AliTaskLoader(){};
    
    TObject*           Get() const; 
    virtual TTask*     Task() const {return dynamic_cast<TTask*>(Get());}
    virtual void       Clean();

  protected:
    Int_t              AddToBoard(TObject* obj);
    void               RemoveFromBoard(TObject* obj);
    TTask*             GetParentalTask() const;

  private:
    TTask*             fParentalTask; // Parental task

  ClassDef(AliTaskLoader,1)    
 };

#endif


