/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
/// 
/// This is a class for reading raw data memory buffers.
///
///////////////////////////////////////////////////////////////////////////////

#include "AliRawReaderMemory.h"
#include <TSystem.h>


ClassImp(AliRawReaderMemory)


AliRawReaderMemory::AliRawReaderMemory() :
  fBuffer(NULL),
  fBufferSize(0),
  fPosition(0)
{
// create an object to read digits from
// the given memory location

  fHeader = new AliRawDataHeader;
}

AliRawReaderMemory::AliRawReaderMemory(UChar_t* memory, UInt_t size) :
  fBuffer(memory),
  fBufferSize(size),
  fPosition(0)
{
// create an object to read digits from the given memory

  fHeader = new AliRawDataHeader;
}

AliRawReaderMemory::~AliRawReaderMemory()
{
// close the input memory

  delete fHeader;
}


AliRawReaderMemory::AliRawReaderMemory(const AliRawReaderMemory& rawReader) :
  AliRawReader(rawReader)
{
  Fatal("AliRawReaderMemory", "copy constructor not implemented");
}

AliRawReaderMemory& AliRawReaderMemory::operator = (const AliRawReaderMemory& 
					      /* rawReader */)
{
  Fatal("operator =", "assignment operator not implemented");
  return *this;
}

Bool_t AliRawReaderMemory::ReadHeader()
{
// read a data header at the current buffer position
// returns kFALSE if the mini header could not be read

  if (!fBuffer) return kFALSE;
  do {
    if ( fPosition+fCount+sizeof(AliRawDataHeader) > fBufferSize ) return kFALSE;

    memcpy( fHeader, fBuffer+fPosition+fCount, sizeof(AliRawDataHeader) );
    fPosition += fCount+sizeof(AliRawDataHeader);

    if (fHeader->fSize != 0xFFFFFFFF) {
      // Check for fHeader->fSize < sizeof(AliRawDataHeader) ????
      fCount = fHeader->fSize - sizeof(AliRawDataHeader);
    } else {
      fCount = fBufferSize-fPosition;
    }
  } while (!IsSelected());
  return kTRUE;
}

Bool_t AliRawReaderMemory::ReadNextData(UChar_t*& data)
{
// reads the next payload at the current buffer position
// returns kFALSE if the data could not be read

  while (fCount == 0) {
    if (!ReadHeader()) return kFALSE;
  }
  UInt_t currentPosition = fPosition;
  fPosition += fCount;
  fCount = 0;

  data = fBuffer+currentPosition;
  return kTRUE;
}

Bool_t AliRawReaderMemory::ReadNext(UChar_t* data, Int_t size)
{
// reads the next block of data at the current buffer position
// returns kFALSE if the data could not be read

  if ( fBufferSize-fPosition < (UInt_t)size ) return kFALSE;

  memcpy( data, fBuffer+fPosition, size );
  fCount -= size;
  fPosition += size;
  return kTRUE;
}


Bool_t AliRawReaderMemory::Reset()
{
// reset the current position in the buffer to the beginning of the curevent

  fCount = 0;
  fPosition = 0;
  return kTRUE;
}

Bool_t AliRawReaderMemory::NextEvent()
{
// each memory buffer always contains only one event
 return kFALSE; 
}

Bool_t AliRawReaderMemory::RewindEvents()
{
// reset the event counter

  return Reset();
}

Bool_t AliRawReaderMemory::SetMemory( UChar_t* memory, ULong_t size )
{
  fBuffer = memory;
  fBufferSize = size;
  fCount = 0;
  fPosition = 0;
  return (fBuffer && fBufferSize>0) ? kTRUE : kFALSE;
}

