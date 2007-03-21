#ifndef ALIMUONPAYLOADTRACKER_H
#define ALIMUONPAYLOADTRACKER_H 
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/

/// \ingroup raw
/// \class AliMUONPayloadTracker
/// \brief Class for decoding the payload for tracker raw data 
///
//  Author Christian Finck

#include <TObject.h>
#include <TArrayI.h>

class AliMUONDDLTracker;
class AliMUONBusStruct;
class AliMUONDspHeader;
class AliMUONBlockHeader;

class AliMUONPayloadTracker: public TObject {
  public :
    AliMUONPayloadTracker();
    virtual ~AliMUONPayloadTracker();

    /// Return maximum number of block per DDL in DATE file
    Int_t GetMaxBlock() const {return fMaxBlock;}
    /// Return maximum number of Dsp per block in DATE file
    Int_t GetMaxDsp()   const {return fMaxDsp;}
    /// Return maximum number of Buspatch per Dsp in DATE file
    Int_t GetMaxBus()   const {return fMaxBus;}

    // check input before assigment
    void SetMaxBlock(Int_t blk);

    /// \brief Set maximum number of Dsp per block in DATE file
    /// does not check, done via BusPatchManager
    void SetMaxDsp(Int_t dsp) {fMaxDsp = dsp;}
    /// \brief Set maximum number of Buspatch per Dsp in DATE file
    /// does not check, done via BusPatchManager
    void SetMaxBus(Int_t bus) {fMaxBus = bus;}

    void ResetDDL();

    Bool_t Decode(UInt_t* buffer, Int_t datasize);

    /// Return pointer for local structure
    AliMUONBusStruct*       GetBusPatchInfo() const {return fBusStruct;}
    /// Return pointer for buspatch structure
    AliMUONDDLTracker*      GetDDLTracker()   const {return fDDLTracker;}

    /// Get number of parity errors
    Int_t   GetParityErrors() const {return fParityErrBus.GetSize();} // for online
    /// Get parity errors in buspatch
    TArrayI GetParityErrBus() const {return fParityErrBus;} // for MOOD
    /// Get number of glitch errors
    Int_t   GetGlitchErrors() const {return fGlitchErrors;}

  private :
    /// Not implemented
    AliMUONPayloadTracker(const AliMUONPayloadTracker& stream);
    /// Not implemented
    AliMUONPayloadTracker& operator = (const AliMUONPayloadTracker& stream);

    Bool_t CheckDataParity();
    void   AddParityErrBus(Int_t buspatch);

    Int_t  fBusPatchId;   ///< entry of buspatch structure
    Int_t  fDspId;        ///< entry of Dsp header
    Int_t  fBlkId;        ///< entry of Block header

    Int_t fMaxDDL;        ///< maximum number of DDL in DATE file
    Int_t fMaxBlock;      ///< maximum number of block per DDL in DATE file
    Int_t fMaxDsp;        ///< maximum number of Dsp per block in DATE file
    Int_t fMaxBus;        ///< maximum number of Buspatch per Dsp in DATE file

    AliMUONDDLTracker*      fDDLTracker;      //!< pointer for buspatch structure
    AliMUONBusStruct*       fBusStruct;       //!< pointer for local structure
    AliMUONBlockHeader*     fBlockHeader;     //!< pointer for block structure 
    AliMUONDspHeader*       fDspHeader;       //!< pointer for dsp structure 

    TArrayI fParityErrBus;                    //!< list of buspatch with at least one parity errors;
    Int_t   fGlitchErrors;                    //!< number of glitch errors;

    ClassDef(AliMUONPayloadTracker, 2)    // base class for reading MUON raw digits
};

#endif
