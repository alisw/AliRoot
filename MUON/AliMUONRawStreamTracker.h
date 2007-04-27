#ifndef ALIMUONRAWSTREAMTRACKER_H
#define ALIMUONRAWSTREAMTRACKER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

/// \ingroup raw
/// \class AliMUONRawStreamTracker
/// \brief Class for reading MUON raw digits
///
//  Author: Christian Finck

#include <TObject.h>
#include "AliMUONPayloadTracker.h"

class AliRawReader;
class AliMUONDDLTracker;
class AliMUONDspHeader;
class AliMUONBusStruct;
class AliMUONBlockHeader;

class AliMUONRawStreamTracker: public TObject {
  public :
    AliMUONRawStreamTracker();
    AliMUONRawStreamTracker(AliRawReader* rawReader);
    virtual ~AliMUONRawStreamTracker();

    /// Initialize iterator
    void First();

    /// Returns current DDL object during iteration
    AliMUONDDLTracker* CurrentDDL() const { return fCurrentDDL; }
    
    /// Returns current BlockHeader object during iteration
    AliMUONBlockHeader* CurrentBlockHeader() const { return fCurrentBlockHeader; }
    
    /// Returns current DspHeader object during iteration
    AliMUONDspHeader* CurrentDspHeader() const { return fCurrentDspHeader; }
    
    /// Returns current BusStruct object during iteration
    AliMUONBusStruct* CurrentBusStruct() const { return fCurrentBusStruct; }
    
    /// Advance one step in the iteration. Returns false if finished.
    virtual Bool_t Next(Int_t& busPatchId, 
                        UShort_t& manuId, UChar_t& manuChannel, 
                        UShort_t& adc);
    
    virtual Bool_t NextDDL();

    /// Return maximum number of DDL in DATE file
    Int_t GetMaxDDL()   const {return fMaxDDL;}
    /// Return maximum number of block per DDL in DATE file
    Int_t GetMaxBlock() const {return  fPayload->GetMaxBlock();}
    /// Return maximum number of Dsp per block in DATE file
    Int_t GetMaxDsp()   const {return  fPayload->GetMaxDsp();}
    /// Return maximum number of Buspatch per Dsp in DATE file
    Int_t GetMaxBus()   const {return  fPayload->GetMaxBus();}

    // check input before assigment
    void SetMaxDDL(Int_t ddl);
    void SetMaxBlock(Int_t blk);

    /// Set maximum number of Dsp per block in DATE file
    /// does not check, done via BusPatchManager
    void SetMaxDsp(Int_t dsp) {fPayload->SetMaxDsp(dsp);}
    /// Set maximum number of Buspatch per Dsp in DATE file
    /// does not check, done via BusPatchManager
    void SetMaxBus(Int_t bus) {fPayload->SetMaxBus(bus);}

    /// Set object for reading the raw data
    void SetReader(AliRawReader* rawReader) {fRawReader = rawReader;}

    /// Return pointer for DDL
    AliMUONDDLTracker*      GetDDLTracker() const {return fPayload->GetDDLTracker();}

    /// Return pointer for payload
    AliMUONPayloadTracker*  GetPayLoad()    const {return fPayload;}

    /// Return number of DDL
    Int_t                   GetDDL()        const {return fDDL - 1;}

    /// Whether the iteration is finished or not
    Bool_t IsDone() const;

  private :
    /// Not implemented
    AliMUONRawStreamTracker(const AliMUONRawStreamTracker& stream);
    /// Not implemented
    AliMUONRawStreamTracker& operator = (const AliMUONRawStreamTracker& stream);

    Bool_t GetNextDDL();
    Bool_t GetNextBlockHeader();
    Bool_t GetNextDspHeader();
    Bool_t GetNextBusStruct();

 private:
    AliRawReader*    fRawReader;    ///< object for reading the raw data  
    Int_t  fDDL;          ///< number of DDL    
    Int_t  fMaxDDL;       ///< maximum number of DDL in DATE file    
    AliMUONPayloadTracker* fPayload; ///< pointer to payload decoder
    
    AliMUONDDLTracker* fCurrentDDL; //!< for iterator: current ddl ptr
    Int_t fCurrentDDLIndex; //!< for iterator: current ddl index
    AliMUONBlockHeader* fCurrentBlockHeader; //!< for iterator: current block ptr
    Int_t fCurrentBlockHeaderIndex; //!< for iterator: current block index    
    AliMUONDspHeader* fCurrentDspHeader; //!< for iterator: current dsp ptr
    Int_t fCurrentDspHeaderIndex; //!< for iterator: current dsp index    
    AliMUONBusStruct* fCurrentBusStruct; //!< for iterator: current bus ptr
    Int_t fCurrentBusStructIndex; //!< for iterator: current bus index    
    Int_t fCurrentDataIndex; //!< for iterator: current data index
    
    ClassDef(AliMUONRawStreamTracker, 3)    // base class for reading MUON raw digits
};

#endif
