#ifndef ALIHLTTESTINPUTHANDLER_H
#define ALIHLTTESTINPUTHANDLER_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//     Reconstruction-specific input handler
//     Author: Andrei Gheata, CERN
//-------------------------------------------------------------------------

#ifndef ALIVEVENTHANDLER_H
#include "AliVEventHandler.h"
#endif

class TObjArray;
class AliVVevent;
class AliVVfriendevent;

class AliHLTTestInputHandler : public AliVEventHandler {

 public:
    AliHLTTestInputHandler() {}
    AliHLTTestInputHandler(const char* name, const char* title);
    virtual ~AliHLTTestInputHandler() {}
    virtual Bool_t Notify() { return kFALSE; }
    virtual Bool_t Notify(const char *) {return kTRUE;}
    virtual Bool_t Init(Option_t* /*opt*/) {return kTRUE;}
    virtual Bool_t Init(TTree* /*tree*/, Option_t* /*opt*/);
    virtual Bool_t BeginEvent(Long64_t entry);
    virtual Bool_t FinishEvent() {return kTRUE;}
    virtual void  SetOutputFileName(const char* /*fname*/) {};
    virtual const char* GetOutputFileName() const {return NULL;}
    // Input
    virtual void SetInputTree(TTree* /*tree*/) {};
    // Steering 
    virtual Bool_t GetEntry() {return kTRUE;}
    virtual Bool_t Terminate() {return kTRUE;}
    virtual Bool_t TerminateIO() {return kTRUE;}

    // Especially needed for HLT
    Bool_t InitTaskInputData(AliVVevent* /*esdEvent*/, AliVVfriendEvent* /*friendEvent*/, TObjArray* /*arrTasks*/);

    AliVEvent* GetEvent() const {return NULL;}
    AliVVevent* GetVVEvent() const {return fEvent;}
    void  SetVVEvent(AliVVevent *event) {fEvent = event;}

    AliVVfriendEvent* GetVVFriendEvent() const {return fFriendEvent;}
    void  SetVVFriendEvent(AliVVfriendEvent *friendEvent) {fFriendEvent = friendEvent;}
      
 private:
    AliHLTTestInputHandler(const AliVEventHandler& handler);             
    AliHLTTestInputHandler& operator=(const AliVEventHandler& handler);  
    
    AliVVevent       *fEvent;          //! Pointer to the event
    AliVVfriendEvent *fFriendEvent;    //! Pointer to the friend event

    ClassDef(AliHLTTestInputHandler, 1);
};

#endif
