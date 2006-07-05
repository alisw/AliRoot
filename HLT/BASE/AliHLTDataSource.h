// @(#) $Id$

#ifndef ALIHLTDATASOURCE_H
#define ALIHLTDATASOURCE_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTDataSource.h
    @author Matthias Richter
    @date   
    @brief  Base class declaration for HLT data source components. */

#include "AliHLTComponent.h"

/**
 * @class AliHLTDataSource
 * Base class of HLT data source components.
 * The class provides a common interface for the implementation of HLT data
 * source components. The child class must implement the functions:
 * - DoInit (optional)
 * - DoDeinit (optional)
 * - GetEvent
 * - GetComponentID
 * - GetInputDataTypes
 * - GetOutputDataType
 * - GetOutputDataSize
 * - Spawn
 *
 * @ingroup AliHLTbase
 */
class AliHLTDataSource : public AliHLTComponent {
 public:
  /** standard constructor */
  AliHLTDataSource();
  /** standard destructor */
  virtual ~AliHLTDataSource();

  /**
   * Event processing function.
   * The method is called by the framework to process one event. After 
   * preparation of data structures. The call is redirected to GetEvent.
   * @return neg. error code if failed
   */
  int ProcessEvent( const AliHLTComponent_EventData& evtData,
		    const AliHLTComponent_BlockData* blocks, 
		    AliHLTComponent_TriggerData& trigData,
		    AliHLTUInt8_t* outputPtr, 
		    AliHLTUInt32_t& size,
		    AliHLTUInt32_t& outputBlockCnt, 
		    AliHLTComponent_BlockData*& outputBlocks,
		    AliHLTComponent_EventDoneData*& edd );

  // Information member functions for registration.

  /**
   * Return @ref AliHLTComponent::kSource type as component type.
   * @return component type id
   */
  TComponentType GetComponentType() { return AliHLTComponent::kSource;}

 private:
  /**
   * Data processing method for the component.
   * @param evtData       event data structure
   * @param trigData	  trigger data structure
   * @param outputPtr	  pointer to target buffer
   * @param size	  <i>input</i>: size of target buffer
   *            	  <i>output</i>:size of produced data
   * @param outputBlocks  list to receive output block descriptors
   * @return
   */
  virtual int GetEvent( const AliHLTComponent_EventData& evtData,
		        AliHLTComponent_TriggerData& trigData,
		        AliHLTUInt8_t* outputPtr, 
		        AliHLTUInt32_t& size,
		        vector<AliHLTComponent_BlockData>& outputBlocks ) = 0;

  ClassDef(AliHLTDataSource, 0)
};
#endif
