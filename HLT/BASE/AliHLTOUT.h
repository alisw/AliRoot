//-*- Mode: C++ -*-
// @(#) $Id$

#ifndef ALIHLTOUT_H
#define ALIHLTOUT_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTOUT.h
    @author Matthias Richter
    @date   
    @brief  The control class for HLTOUT data.
*/

#include <vector>
#include "AliHLTLogging.h"
#include "AliHLTModuleAgent.h"

class AliHLTOUTHandler;
class AliHLTOUTHandlerDesc; // AliHLTModuleAgent.h

#define AliHLTOUTInvalidIndex (~(AliHLTUInt32_t)0)

typedef vector<AliHLTUInt32_t> AliHLTOUTIndexList;

/**
 * @class AliHLTOUT
 * The control class for HLTOUT data.
 * The output of the HLT, either from the HLTOUT nodes or simulated output,
 * is transferred and stored in the HOMER format. The AliHLTOUT class 
 * implements scanning of the HOMER data for all HLTOUT DDL links and
 * abstracts access to the complete HLTOUT data.
 * 
 */
class AliHLTOUT : public AliHLTLogging {
 public:
  /** standard constructor */
  AliHLTOUT();
  /** standard destructor */
  virtual ~AliHLTOUT();

  /**
   * Init for processing.
   * The HLTOUT is scanned for all available data blocks and the
   * AliHLTOUTHandler objects for the data blocks are created according
   * to the module agents (see AliHLTModuleAgent).
   */
  int Init();

  /**
   * Get number of data blocks in the HLTOUT data
   */
  int GetNofDataBlocks();

  /**
   * Select the first data block of a certain data type and specification.
   * The selection criteria can be of kAliHLTAnyDataType and/or
   * kAliHLTVoidDataSpec in order to be ignored and just match any data block.
   *
   * The search criteria can be combined with a handler type (e.g. kRawReader)
   * @param dt    [in]  data type to match                                <br>
   * @param spec  [in]  data specification to match                       <br>
   * @param handlerType [in]  type of the handler
   * @return identifier >0 if success, 0 if no block found                <br>
   *         neg. error code if failed                                    <br>
   *                        -EPERM if access denied (object locked)
   */
  int SelectFirstDataBlock(AliHLTComponentDataType dt=kAliHLTAnyDataType,
			   AliHLTUInt32_t spec=kAliHLTVoidDataSpec,
			   AliHLTModuleAgent::AliHLTOUTHandlerType handlerType=AliHLTModuleAgent::kUnknownOutput);

  /**
   * Select the next data block of data type and specification of the previous
   * call to @ref SelectFirstDataBlock.
   * @return identifier >0 if success, 0 if no block found                <br>
   *         neg. error code if failed                                    <br>
   *                        -EPERM if access denied (object locked)
   */
  int SelectNextDataBlock();

  /**
   * Get properties of the selected data block.
   * @param dt    [out] data type of the selected block
   * @param spec  [out] data specification of the selected block
   */
  int GetDataBlockDescription(AliHLTComponentDataType& dt, AliHLTUInt32_t& spec);

  /**
   * Get the index of the current data block.
   * @return index, AliHLTOUTInvalidIndex if no block selected
   */
  AliHLTUInt32_t GetDataBlockIndex();

  /**
   * Get buffer of the selected data block.
   * @param pBuffer [out] buffer of the selected data block
   * @param size    [out] size of the selected data block
   */
  int GetDataBuffer(const AliHLTUInt8_t* &pBuffer, AliHLTUInt32_t& size);

  /**
   * Release buffer after use.
   * @param pBuffer [in]  buffer of the selected data block
   */
  int ReleaseDataBuffer(const AliHLTUInt8_t* pBuffer);

  /**
   * Get handler for the selected data block.
   */
  AliHLTOUTHandler* GetHandler();

  /**
   * Locking guard for the AliHLTOUT object.
   * If the object is locked, the selection of data blocks can not be changed.
   */
  class AliHLTOUTLockGuard {
  public:
    /** constructor */
    AliHLTOUTLockGuard(AliHLTOUT* pInstance) : fpInstance(pInstance)
    {if (fpInstance) fpInstance->SetStatusFlag(kLocked);}
    /** destructor */
    ~AliHLTOUTLockGuard()
    {if (fpInstance) fpInstance->ClearStatusFlag(kLocked);}

  private:
    /** standard constructor prohibited */
    AliHLTOUTLockGuard();
    /** copy constructor prohibited */
    AliHLTOUTLockGuard(const AliHLTOUTLockGuard&);
    /** assignment operator prohibited */
    AliHLTOUTLockGuard& operator=(const AliHLTOUTLockGuard&);

    /** the AliHLTOUT instance the guard is locking */
    AliHLTOUT* fpInstance; //!transient
  };

  /**
   * The HLT OUT Event Header.
   * Defined between HLT and DAQ.
   */
  struct AliHLTOUTEventHeader {
    /**Total length of the data in bytes, including HLT event header, excluding CDH. */
    AliHLTUInt32_t fLength; //! see above
    /** version of the header */
    AliHLTUInt32_t fVersion; //! see above
    /** event id */
    AliHLTUInt64_t fEventID; //! see above
  };

  // definitions from ALICE internal notes ALICE-INT-2002-010 and
  // ALICE-INT-2006-XXX
  /** the 32bit word in the CDH containing the status flags */
  static const unsigned char fgkCDHStatusWord; //! see above
  /** start of the flags in word fgkCDHStatusWord */
  static const unsigned char fgkCDHStatusFlagsOffset; //! see above
  /** bit indicating HLT decision in the HLTOUT*/
  static const unsigned char fgkCDHFlagsHLTDecision; //! see above
  /** bit indicating HLT payload in the HLTOUT*/
  static const unsigned char fgkCDHFlagsHLTPayload; //! see above

  /**
   * Block descriptor.
   */
  class AliHLTOUTBlockDescriptor {
  public:
    AliHLTOUTBlockDescriptor(AliHLTComponentDataType dt, AliHLTUInt32_t spec, AliHLTUInt32_t index)
      : fDataType(dt), fSpecification(spec), fIndex(index) {};
    ~AliHLTOUTBlockDescriptor() {}

    operator AliHLTComponentDataType() const {return fDataType;}
    operator AliHLTUInt32_t() const {return fSpecification;}
    int operator==(AliHLTComponentDataType dt) const {return dt==fDataType;}
    int operator==(AliHLTUInt32_t spec) const {return spec==fSpecification;}

    AliHLTUInt32_t GetIndex() const {return fIndex;}
  private:
    /** data type of the block */
    AliHLTComponentDataType fDataType; //!transient
    /** data specification of the block */
    AliHLTUInt32_t          fSpecification; //!transient
    /** index in the data stream */
    AliHLTUInt32_t          fIndex; //!transient
  };

  enum AliHLTOUTByteOrder_t {
    /** no data block selected */
    kInvalidByteOrder=-1,
    kUnknownByteOrder=0,
    kLittleEndian,
    kBigEndian
  };

  enum AliHLTOUTDataType_t {
    kUint64 = 0,
    kUint32 = 1,
    kUint16 = 2,
    kUint8  = 3,
    kDouble = 4,
    kFloat  = 5
  };

  /**
   * Check byte order of selected block
   */
  AliHLTOUTByteOrder_t CheckByteOrder();

  /**
   * Check alignment of selected block
   */
  int CheckAlignment(AliHLTOUT::AliHLTOUTDataType_t type);

 protected:
  /**
   * Add a block descriptor.
   * This is done by the child classes generating the index. The AliHLTOUT
   * object must be locked for index generation.
   * @param desc    the block descriptor
   * @return 0 if success, -EPERM if access denied
   */
  int AddBlockDescriptor(const AliHLTOUTBlockDescriptor desc);

 private:
  /** copy constructor prohibited */
  AliHLTOUT(const AliHLTOUT&);
  /** assignment operator prohibited */
  AliHLTOUT& operator=(const AliHLTOUT&);

  /**
   * Internal status flags
   */
  enum {
    /** the HLTOUT object is locked with the current data block */
    kLocked = 0x1,
    /** childs can add block descriptors */
    kCollecting = 0x2,
    /** user of the data block has checked the byte order */
    kByteOrderChecked = 0x4,
    /** warning on byte order missmatch has been printed */
    kByteOrderWarning = 0x8,
    /** user of the data block has checked the alignment */
    kAlignmentChecked = 0x10,
    /** warning on alignment missmatch has been printed */
    kAlignmentWarning = 0x20
  };

  class AliHLTOUTHandlerListEntry {
  public:
    AliHLTOUTHandlerListEntry(AliHLTOUTHandler* pHandler, 
			      AliHLTModuleAgent::AliHLTOUTHandlerDesc& handlerDesc,
			      AliHLTModuleAgent* pAgent,
			      AliHLTUInt32_t index);

    /** copy constructor for vector handling */
    AliHLTOUTHandlerListEntry(const AliHLTOUTHandlerListEntry& src);

    /** assignment operator for vector handling */
    AliHLTOUTHandlerListEntry& operator=(const AliHLTOUTHandlerListEntry& src);

    ~AliHLTOUTHandlerListEntry();

    static const AliHLTOUTHandlerListEntry fgkVoidHandlerListEntry;

    operator AliHLTOUTHandler*() const {return fpHandler;}
    operator AliHLTModuleAgent::AliHLTOUTHandlerDesc&() {return *fpHandlerDesc;}
    operator AliHLTModuleAgent*() const {return fpAgent;}

    bool operator==(const AliHLTOUTHandlerListEntry& entry) const;

    AliHLTUInt32_t operator[](int i) const;

    void AddIndex(AliHLTUInt32_t index);

    bool HasIndex(AliHLTUInt32_t index);

  private:
    /** standard constructor prohibited */
    AliHLTOUTHandlerListEntry();

    /** pointer to the handler */
    AliHLTOUTHandler* fpHandler; //! transient

    /** pointer to handler description */
    AliHLTModuleAgent::AliHLTOUTHandlerDesc* fpHandlerDesc; //! transient

    /** pointer to module agent */
    AliHLTModuleAgent* fpAgent; //! transient

    /** list of block indexes */
    AliHLTOUTIndexList fBlocks; //!transient
  };
  
  /**
   * Generate the index of the HLTOUT data.
   * Must be implemented by the child classes.
   */
  virtual int GenerateIndex()=0;

  /**
   * Find AliHLTOUTHandler objects for the data blocks.
   * The available AliHLTModuleAgents are probed whether they provide
   * handlers for data processing.
   */
  int InitHandlers();

  /**
   * Get the data buffer
   * @param index   [in]  index of the block
   * @param pBuffer [out] buffer of the selected data block
   * @param size    [out] size of the selected data block
   */
  virtual int GetDataBuffer(AliHLTUInt32_t index, const AliHLTUInt8_t* &pBuffer, 
			    AliHLTUInt32_t& size)=0;

  /**
   * Check byte order of data block
   */
  virtual AliHLTOUTByteOrder_t CheckBlockByteOrder(AliHLTUInt32_t index)=0;

  /**
   * Check alignment of data block
   */
  virtual int CheckBlockAlignment(AliHLTUInt32_t index, AliHLTOUT::AliHLTOUTDataType_t type)=0;

  /**
   * Select the data block of data type and specification of the previous
   * call to @ref SelectFirstDataBlock. Core function of @ref SelectFirstDataBlock
   * and @ref SelectNextDataBlock, starts to find a block at the current list
   * position. 
   * @return identifier >=0 if success, neg. error code if failed         <br>
   *                        -ENOENT if no block found                     <br>
   *                        -EPERM if access denied (object locked)
   */
  int FindAndSelectDataBlock();

  /**
   * Set status flag.
   * @param flag     flag to set
   * @return current status flags
   */
  unsigned int SetStatusFlag(unsigned int flag) {return fFlags|=flag;}

  /**
   * Clear status flag.
   * @param flag     flag to clear
   * @return current status flags
   */
  unsigned int ClearStatusFlag(unsigned int flag) {return fFlags&=~flag;}

  /**
   * Check status flag.
   * @param flag     flag to check
   * @return 1 if flag is set
   */
  int CheckStatusFlag(unsigned int flag) const {return (fFlags&flag)==flag;}

  /**
   * Insert a handler item.
   * The current list entries are checked if the handler is already in
   * the list. It is added if not in the list, otherwise the block index
   * is added to the existing entry.
   * @param entry    handler list entry
   * @return 0 if added, EEXIST (non negative!) if merged with existing entry <br>
   *         neg. error code if failed
   */
  int InsertHandler(const AliHLTOUTHandlerListEntry &entry);

  /**
   * Find handler description for a certain block index.
   */
  AliHLTOUTHandlerListEntry FindHandlerDesc(AliHLTUInt32_t blockIndex);

  /** data type for the current block search, set from @ref SelectFirstDataBlock */
  AliHLTComponentDataType fSearchDataType; //!transient

  /** data specification for the current block search */
  AliHLTUInt32_t fSearchSpecification; //!transient

  /** instance flags: locked, collecting, ... */
  unsigned int fFlags; //!transient

  /** list of block descriptors */
  vector<AliHLTOUTBlockDescriptor> fBlockDescList; //!transient

  /** current position in the list */
  vector<AliHLTOUTBlockDescriptor>::iterator fCurrent; //!transient

  /** data buffer under processing */
  const AliHLTUInt8_t* fpBuffer; //!transient

  /** list of AliHLTOUTHandlers */
  vector<AliHLTOUTHandlerListEntry> fDataHandlers; // !transient

  ClassDef(AliHLTOUT, 1)
};
#endif
