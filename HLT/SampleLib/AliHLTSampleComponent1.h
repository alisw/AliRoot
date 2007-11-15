// @(#) $Id$

#ifndef ALIHLTSAMPLECOMPONENT1_H
#define ALIHLTSAMPLECOMPONENT1_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTSampleComponent1.h
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  A sample processing component for the HLT. */

#include "AliHLTProcessor.h"

/**
 * @class AliHLTSampleComponent1
 * @brief An HLT sample component.
 * This component does not any data processing at all. It just
 * illustrates the existence of several components in ine library and
 * allows to set up a very simple chain with different components.
 * @ingroup alihlt_tutorial
 */
class AliHLTSampleComponent1 : public AliHLTProcessor {
public:
  AliHLTSampleComponent1();
  virtual ~AliHLTSampleComponent1();

  const char* GetComponentID() { return "Sample-component1";}
  void GetInputDataTypes( vector<AliHLTComponentDataType>& list) {
    list.push_back(kAliHLTAnyDataType);
  }
  AliHLTComponentDataType GetOutputDataType() {return kAliHLTVoidDataType;}
  virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier ) {constBase = 0;inputMultiplier = 0;};

  // Spawn function, return new class instance
  AliHLTComponent* Spawn() {return new AliHLTSampleComponent1;};

 protected:
  
  int DoInit( int argc, const char** argv );
  int DoDeinit();
  int DoEvent( const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks, 
		       AliHLTComponentTriggerData& trigData, AliHLTUInt8_t* outputPtr, 
		       AliHLTUInt32_t& size, vector<AliHLTComponentBlockData>& outputBlocks );

  using AliHLTProcessor::DoEvent;

private:

  ClassDef(AliHLTSampleComponent1, 0)
};
#endif
