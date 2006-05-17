#ifndef ALIALTRORAWSTREAM_H
#define ALIALTRORAWSTREAM_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
///
/// This is a base class for reading raw data digits in Altro format
///
///////////////////////////////////////////////////////////////////////////////

#include <TObject.h>

class AliRawReader;

class AliAltroRawStream: public TObject {
  public :
    AliAltroRawStream(AliRawReader* rawReader);
    virtual ~AliAltroRawStream();

    virtual void             Reset();
    virtual Bool_t           Next();

    Int_t GetDDLNumber()  const { return fDDLNumber; }  // Provide current DDL number
    Int_t GetPrevDDLNumber() const { return fPrevDDLNumber; }// Provide previous DDL number
    Bool_t  IsNewDDLNumber() const {return (fDDLNumber != fPrevDDLNumber);};
    Int_t GetRCUId()      const { return fRCUId; }      // Provide current RCU identifier
    Int_t GetPrevRCUId()  const { return fPrevRCUId; }  // Provide previous RCU identifier
    Bool_t  IsNewRCUId() const {return (fRCUId != fPrevRCUId);};
    Int_t GetHWAddress()  const { return fHWAddress; }  // Provide current hardware address
    Int_t GetPrevHWAddress() const { return fPrevHWAddress; }  // Provide previous hardware address
    Bool_t  IsNewHWAddress() const {return (fHWAddress != fPrevHWAddress) || IsNewDDLNumber();};
    Int_t GetTime()       const { return fTime; }       // Provide index of current time bin
    Int_t GetPrevTime()   const { return fPrevTime; }   // Provide index of previous time bin
    Bool_t  IsNewTime()   const {return (fTime != fPrevTime) || IsNewHWAddress();};
    Int_t GetSignal()     const { return fSignal; }     // Provide signal in ADC counts
    Int_t GetTimeLength() const { return fTimeBunch; }  // Provide total length of current time bunch

    Bool_t  GetRCUTrailerData(UChar_t*& data) const;              // Provide a pointer to RCU trailer
    Int_t   GetRCUTrailerSize() const { return fRCUTrailerSize; } // Provide size of RCU trailer

    void SelectRawData(Int_t detId);                           // Select raw data for specific detector id

  protected:
    AliAltroRawStream(const AliAltroRawStream& stream);
    AliAltroRawStream& operator = (const AliAltroRawStream& stream);

    Bool_t           fNoAltroMapping;  // temporary flag in case of no altro mapping is provided
    Short_t          fSegmentation[3]; // temporary container for the dummy trailer, to be removed

  private :

    UShort_t         GetNextWord();
    Bool_t           ReadTrailer();
    Bool_t           ReadDummyTrailer();
    void             ReadBunch();
    void             ReadAmplitude();
    Int_t            GetPosition();
    UInt_t           Get32bitWord(Int_t &index);

    Int_t            fDDLNumber;    // index of current DDL number
    Int_t            fPrevDDLNumber;// index of previous DDL number
    Int_t            fRCUId;        // current RCU identifier
    Int_t            fPrevRCUId;    // previous RCU identifier
    Short_t          fHWAddress;    // current hardware address
    Short_t          fPrevHWAddress;// previous hardware address
    Int_t            fTime;         // index of current time bin
    Int_t            fPrevTime;     // index of previous time bin
    Int_t            fSignal;       // signal in ADC counts
    Int_t            fTimeBunch;    // total length of the current time bunch

    AliRawReader*    fRawReader;    // object for reading the raw data


    UChar_t*         fData;         // raw data

    Int_t            fPosition;     // current (10 bit) position in fData
    Int_t            fCount;        // counter of words to be read for current trailer
    Int_t            fBunchLength;  // remaining number of signal bins in the current bunch

    UChar_t*         fRCUTrailerData; // pointer to RCU trailer data
    Int_t            fRCUTrailerSize; // size of RCU trailer data in bytes

    ClassDef(AliAltroRawStream, 0)  // base class for reading Altro raw digits
};

#endif
