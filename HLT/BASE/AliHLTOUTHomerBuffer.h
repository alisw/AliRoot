//-*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTOUTHOMERBUFFER_H
#define ALIHLTOUTHOMERBUFFER_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               */

/** @file   AliHLTOUTHomerBuffer.h
    @author Matthias Richter
    @date   
    @brief  HLTOUT data wrapper for a data buffer.
*/

#include "AliHLTOUT.h"
#include "AliHLTLogging.h"

class AliHLTHOMERReader;
class AliHLTMonitoringReader;
class AliHLTHOMERLibManager;

/**
 * @class AliHLTOUTHomerBuffer
 * Handler of HLTOUT data for buffer input.
 */
class AliHLTOUTHomerBuffer : public AliHLTOUT, public AliHLTLogging {
 public:
  /** constructor */
  AliHLTOUTHomerBuffer(const AliHLTUInt8_t* pBuffer, int size);
  /** destructor */
  virtual ~AliHLTOUTHomerBuffer();

 protected:
  /**
   * Step trough data blocks of a HOMER reader and generate index.
   */
  int ScanReader(AliHLTMonitoringReader* pReader, AliHLTUInt32_t majorIndex=0);

  /** dynamic loader manager for HOMER library */
  AliHLTHOMERLibManager* fpManager; //!transient

 private:
  /** standard constructor prohibited */
  AliHLTOUTHomerBuffer();
  /** copy constructor prohibited */
  AliHLTOUTHomerBuffer(const AliHLTOUTHomerBuffer&);
  /** assignment operator prohibited */
  AliHLTOUTHomerBuffer& operator=(const AliHLTOUTHomerBuffer&);

  /**
   * Generate the index of the HLTOUT data from the data buffer.
   */
  virtual int GenerateIndex();

  /**
   * Get the data buffer
   * @param index   [in]  index of the block
   * @param pBuffer [out] buffer of the selected data block
   * @param size    [out] size of the selected data block
   */
  virtual int GetDataBuffer(AliHLTUInt32_t index, const AliHLTUInt8_t* &pBuffer, 
			    AliHLTUInt32_t& size);

  /**
   * Check byte order of data block
   */
  virtual AliHLTOUTByteOrder CheckBlockByteOrder(AliHLTUInt32_t index);

  /**
   * Check alignment of data block
   */
  virtual int CheckBlockAlignment(AliHLTUInt32_t index, AliHLTOUT::AliHLTOUTDataType type);

  /** data buffer */
  const AliHLTUInt8_t* fpBuffer; //! transient

  /** size of data buffer */
  int fSize; //! transient

  /** instance of the HOMER reader */
  AliHLTHOMERReader* fpReader;  //!transient

  ClassDef(AliHLTOUTHomerBuffer, 0)
};
#endif
