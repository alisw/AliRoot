// $Id$

#ifndef ALIHLTTRDCLUSTERIZERCOMPONENTV2_H
#define ALIHLTTRDCLUSTERIZERCOMPONENTV2_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/** @file   AliHLTTRDClusterizerComponentV2.h
    @author Timm Steinbeck, Matthias Richter
    @date   
    @brief  Declaration of a TRDClusterizer component. */


#include "AliHLTProcessor.h"
class AliCDBManager;
class AliTRDclusterizerV2HLT;
class AliRawReaderMemory;

/**
 * @class AliHLTTRDClusterizerComponentV2
 * @brief A TRDClusterizer HLT processing component. 
 *
 * An implementiation of a TRDClusterizer component that just copies its input data
 * as a test, demonstration, and example of the HLT component scheme.
 * @ingroup alihlt_tutorial
 */
class AliHLTTRDClusterizerComponentV2 : public AliHLTProcessor
    {
    public:
	AliHLTTRDClusterizerComponentV2();
	virtual ~AliHLTTRDClusterizerComponentV2();

	// Public functions to implement AliHLTComponent's interface.
	// These functions are required for the registration process

	const char* GetComponentID();
	void GetInputDataTypes( vector<AliHLTComponent_DataType>& list);
	AliHLTComponent_DataType GetOutputDataType();
	virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );
	AliHLTComponent* Spawn();
	
    protected:
	
	// Protected functions to implement AliHLTComponent's interface.
	// These functions provide initialization as well as the actual processing
	// capabilities of the component. 

	int DoInit( int argc, const char** argv );
	int DoDeinit();
	int DoEvent( const AliHLTComponent_EventData& evtData, const AliHLTComponent_BlockData* blocks, 
		     AliHLTComponent_TriggerData& trigData, AliHLTUInt8_t* outputPtr, 
		     AliHLTUInt32_t& size, vector<AliHLTComponent_BlockData>& outputBlocks );
	
    private:

	// The size of the output data produced, as a percentage of the input data's size.
	// Can be greater than 100 (%)

	unsigned fOutputPercentage; // Output volume in percentage of the input

	string fStrorageDBpath; // Default path for OCDB

	AliTRDclusterizerV2HLT *fClusterizer; //! Offline derived HLT clusterizer
	AliCDBManager *fCDB; //! Pointer to OCDB
	AliRawReaderMemory *fMemReader; //! Input raw data reader
	
	ClassDef(AliHLTTRDClusterizerComponentV2, 0)

    };
#endif
