#ifndef ALIRAWREADERFILE_H
#define ALIRAWREADERFILE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include "AliRawReader.h"
#ifdef __CINT__
class fstream;
#else
#include <Riostream.h>
#endif
#include <TString.h>


class AliRawReaderFile: public AliRawReader {
  public :
    AliRawReaderFile(const char* fileName, Bool_t addNumber = kTRUE);
    virtual ~AliRawReaderFile();

    virtual UInt_t   GetType() {return 0;};
    virtual UInt_t   GetRunNumber() {return 0;};
    virtual const UInt_t* GetEventId() {return 0;};
    virtual const UInt_t* GetTriggerPattern() {return 0;};
    virtual const UInt_t* GetDetectorPattern() {return 0;};
    virtual const UInt_t* GetAttributes() {return 0;};
    virtual UInt_t   GetGDCId() {return 0;};


    virtual Bool_t   ReadMiniHeader();
    virtual Bool_t   ReadNextData(UChar_t*& data);

    virtual Bool_t   Reset();

  protected :
    Bool_t           OpenNextFile();

    virtual Bool_t   ReadNext(UChar_t* data, Int_t size);

    TString          fFileName;    // name of input files
    Int_t            fFileNumber;  // number of current input file
    fstream*         fStream;      // stream of raw digits
    UChar_t*         fBuffer;      // buffer for payload
    Int_t            fBufferSize;  // size of fBuffer in bytes

    ClassDef(AliRawReaderFile, 0) // class for reading raw digits from a file
};

#endif
