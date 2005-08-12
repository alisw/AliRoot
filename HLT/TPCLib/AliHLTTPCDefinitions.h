// XEmacs -*-C++-*-
// @(#) $Id$

#ifndef ALIHLTTPCDEFINITIONS_H
#define ALIHLTTPCDEFINITIONS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* AliHLTTPCDefinitions
 */

#include "AliHLTDataTypes.h"
#include "TObject.h"

class AliHLTTPCDefinitions
    {
    public:

	static AliHLTUInt8_t GetMinSliceNr( const AliHLTComponent_BlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x00FF0000) >> 16 );
		}
	static AliHLTUInt8_t GetMaxSliceNr( const AliHLTComponent_BlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0xFF000000) >> 24 );
		}
	static AliHLTUInt8_t GetMinPatchNr( const AliHLTComponent_BlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x000000FF) );
		}
	static AliHLTUInt8_t GetMaxPatchNr( const AliHLTComponent_BlockData& block )
		{
		return (AliHLTUInt8_t)( (block.fSpecification & 0x0000FF00) >> 8 );
		}
	
	static AliHLTUInt32_t EncodeDataSpecification( AliHLTUInt8_t minSliceNr, 
						AliHLTUInt8_t maxSliceNr,
						AliHLTUInt8_t minPatchNr,
						AliHLTUInt8_t maxPatchNr )
		{
		return ((maxSliceNr & 0xFF) << 24) | ((minSliceNr & 0xFF) << 16) | ((maxPatchNr & 0xFF) << 8) | ((minPatchNr & 0xFF));
		}

	static const AliHLTComponent_DataType gkDDLPackedRawDataType;
	static const AliHLTComponent_DataType gkPackedRawDataType;
	static const AliHLTComponent_DataType gkUnpackedRawDataType;
	static const AliHLTComponent_DataType gkClustersDataType;
	static const AliHLTComponent_DataType gkTrackSegmentsDataType;
	static const AliHLTComponent_DataType gkVertexDataType;

	ClassDef(AliHLTTPCDefinitions, 0)

    };

#endif
