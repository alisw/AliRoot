#ifndef ALIRAWREADERROOT_H
#define ALIRAWREADERROOT_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "AliRawReader.h"

class AliRawEvent;
class AliRawData;
class TFile;


class AliRawReaderRoot: public AliRawReader {
  public :
    AliRawReaderRoot(const char* fileName, Int_t eventNumber);
    AliRawReaderRoot(AliRawEvent* event);
    AliRawReaderRoot(const AliRawReaderRoot& rawReader);
    AliRawReaderRoot& operator = (const AliRawReaderRoot& rawReader);
    virtual ~AliRawReaderRoot();

    virtual UInt_t   GetType() const;
    virtual UInt_t   GetRunNumber() const;
    virtual const UInt_t* GetEventId() const;
    virtual const UInt_t* GetTriggerPattern() const;
    virtual const UInt_t* GetDetectorPattern() const;
    virtual const UInt_t* GetAttributes() const;
    virtual UInt_t   GetGDCId() const;

    virtual Bool_t   ReadMiniHeader();
    virtual Bool_t   ReadNextData(UChar_t*& data);

    virtual Bool_t   Reset();

    virtual Int_t    CheckData() const;

  protected :
    virtual Bool_t   ReadNext(UChar_t* data, Int_t size);

    TFile*           fFile;         // raw data root file
    AliRawEvent*     fEvent;        // (super) event
    Int_t            fSubEventIndex; // index of current sub event
    AliRawEvent*     fSubEvent;     // current sub event
    AliRawData*      fRawData;      // current raw data
    UChar_t*         fPosition;     // current position in the raw data
    UChar_t*         fEnd;          // end position of the current subevent

    ClassDef(AliRawReaderRoot, 0) // class for reading raw digits from a root file
};

#endif
