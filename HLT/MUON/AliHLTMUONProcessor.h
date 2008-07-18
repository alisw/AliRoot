#ifndef ALIHLTMUONPROCESSOR_H
#define ALIHLTMUONPROCESSOR_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/* $Id: $ */

///
/// @file   AliHLTMUONProcessor.h
/// @author Artur Szostak <artursz@iafrica.com>
/// @date   19 May 2008
/// @brief  Declaration of a common processor component abstract interface for dHLT components.
///

#include "AliHLTProcessor.h"
#include "AliHLTMUONDataBlockReader.h"
#include "AliHLTMUONUtils.h"

class TMap;

/**
 * @class AliHLTMUONProcessor
 * This component class is an abstract base class for dHLT components.
 * Some common methods useful to all dHLT specific components are implemented
 * by this class.
 *
 * @ingroup alihlt_dimuon_component
 */
class AliHLTMUONProcessor : public AliHLTProcessor
{
public:
	/// Default constructor.
	AliHLTMUONProcessor() : AliHLTProcessor() {}
	
	/// Default destructor.
	virtual ~AliHLTMUONProcessor() {}

protected:

	/**
	 * Method to check the block structure and log appropriate error messages.
	 * If a problem is found with the data block then an appropriate HLT error
	 * message is logged and the method returns false.
	 * \param block  The lightweight block reader whose data block should be checked.
	 * \param name  A string containing a descriptive name of the data block
	 *          type. This name is used in the logged error messages.
	 * \param checkHeader  Indicates if the common data block header should be checked.
	 * \returns  true if the structure of the block looks OK and false otherwise.
	 * \note  The BlockType should be a class deriving from AliHLTMUONDataBlockReader.
	 */
	template <class BlockType>
	bool BlockStructureOk(
			const BlockType& block, const char* name,
			bool checkHeader = true
		) const;
	
	/// Checks the structure of a trigger records data block.
	bool BlockStructureOk(
			const AliHLTMUONTriggerRecordsBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "trigger records", checkHeader);
	}
	
	/// Checks the structure of a trigger records debug information data block.
	bool BlockStructureOk(
			const AliHLTMUONTrigRecsDebugBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "trigger records debug information", checkHeader);
	}

	/// Checks the structure of a reconstructed hits data block.
	bool BlockStructureOk(
			const AliHLTMUONRecHitsBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "reconstructed hits", checkHeader);
	}
	
	/// Checks the structure of a clusters data block.
	bool BlockStructureOk(
			const AliHLTMUONClustersBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "clusters", checkHeader);
	}
	
	/// Checks the structure of a ADC channels data block.
	bool BlockStructureOk(
			const AliHLTMUONChannelsBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "channels", checkHeader);
	}

	/// Checks the structure of a Manso tracks data block.
	bool BlockStructureOk(
			const AliHLTMUONMansoTracksBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "Manso tracks", checkHeader);
	}
	
	/// Checks the structure of a Manso track candidates data block.
	bool BlockStructureOk(
			const AliHLTMUONMansoCandidatesBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "Manso track candidates", checkHeader);
	}

	/// Checks the structure of a single track trigger decision data block.
	bool BlockStructureOk(
			const AliHLTMUONSinglesDecisionBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "singles decision", checkHeader);
	}

	/// Checks the structure of a track pairs trigger decision data block.
	bool BlockStructureOk(
			const AliHLTMUONPairsDecisionBlockReader& block,
			bool checkHeader = true
		) const
	{
		return BlockStructureOk(block, "pairs decision", checkHeader);
	}
	
	/**
	 * Sets the CDB path and run number to read from.
	 * \param cdbPath  The CDB path to use. If set to NULL and the path has
	 *      not been set in the CDB manager then the default path
	 *      "local://$ALICE_ROOT" is used if the 'useDefault' flag is also true.
	 * \param run  The run number to use. If set to -1 and the run number has
	 *      not been set in the CDB manager then a value of zero is used if
	 *      the 'useDefault' flag is also true.
	 * \param useDefault  If set to true then a default CDB path and/or run number
	 *      is used if they have not been set and 'cdbPath' == NULL or
	 *      'run' == -1. (false by default).
	 * \return Zero if the object could be loaded. Otherwise an error code,
	 *      compatible with the HLT framework, is returned.
	 */
	int SetCDBPathAndRunNo(
			const char* cdbPath, Int_t run, bool useDefault = false
		) const;
	
	/**
	 * Fetches the DDL and detector element store objects for MUON mapping.
	 * \return Zero if the objects could be loaded. Otherwise an error code,
	 *      which is compatible with the HLT framework, is returned.
	 * \note AliMpDDLStore::Instance() and AliMpDEStore::Instance() must be used
	 *      to fetch the objects after this method returns a code equal to zero.
	 */
	int FetchMappingStores() const;
	
	/**
	 * Fetches a TMap object from the CDB.
	 * [in] \param pathToEntry  The relative path to the entry in the CDB to fetch.
	 * [out] \param map  This will be filled with the TMap object found if
	 *      a successful status code is returned. Otherwise it will be unchanged.
	 * \return Zero if the object could be found. Otherwise an error code,
	 *      which is compatible with the HLT framework, is returned.
	 */
	int FetchTMapFromCDB(const char* pathToEntry, TMap*& map) const;
	
	/**
	 * Tries to find the string value associated with a certain parameter in a TMap.
	 * [in] \param map  The TMap object to search in.
	 * [in] \param paramName  The name of the parameter to search for.
	 * [out] \param value  Will be filled with the object found.
	 * [in] \param prettyName  Should be the name of the parameter which will
	 *      be used when printing error messages. If this is set to NULL then
	 *      the paramName will be used instead (default is NULL).
	 * \return Zero if the object could be found. Otherwise an error code,
	 *      which is compatible with the HLT framework, is returned.
	 */
	int GetValueFromTMap(
			TMap* map, const char* paramName, TString& value,
			const char* pathToEntry = NULL, const char* prettyName = NULL
		) const;
	
	/**
	 * Tries to find a certain parameter in the TMap object and convert it to
	 * an integer value.
	 * [in] \param map  The TMap object to search in.
	 * [in] \param paramName  The name of the parameter to search for.
	 * [out] \param value  Will be filled with the integer value for the parameter,
	 *       if it was found and it was an integer value.
	 * [in] \param prettyName  Should be the name of the parameter which will
	 *      be used when printing error messages. If this is set to NULL then
	 *      the paramName will be used instead (default is NULL).
	 * \return Zero if the object could be found and is valid. Otherwise an
	 *       error code, which is compatible with the HLT framework, is returned.
	 */
	int GetIntFromTMap(
			TMap* map, const char* paramName, Int_t& value,
			const char* pathToEntry = NULL, const char* prettyName = NULL
		) const;
	
	/**
	 * Tries to find a certain parameter in the TMap object and convert it to
	 * a positive integer value.
	 * [in] \param map  The TMap object to search in.
	 * [in] \param paramName  The name of the parameter to search for.
	 * [out] \param value  Will be filled with the integer value for the parameter,
	 *       if it was found and it was a positive integer value.
	 * [in] \param prettyName  Should be the name of the parameter which will
	 *      be used when printing error messages. If this is set to NULL then
	 *      the paramName will be used instead (default is NULL).
	 * \return Zero if the object could be found and is valid. Otherwise an
	 *       error code, which is compatible with the HLT framework, is returned.
	 */
	int GetPositiveIntFromTMap(
			TMap* map, const char* paramName, Int_t& value,
			const char* pathToEntry = NULL, const char* prettyName = NULL
		) const;
	
	/**
	 * Tries to find a certain parameter in the TMap object and convert it to
	 * an floating point value.
	 * [in] \param map  The TMap object to search in.
	 * [in] \param paramName  The name of the parameter to search for.
	 * [out] \param value  Will be filled with the floating point value for the
	 *       parameter, if it was found and it was a floating point value.
	 * [in] \param prettyName  Should be the name of the parameter which will
	 *      be used when printing error messages. If this is set to NULL then
	 *      the paramName will be used instead (default is NULL).
	 * \return Zero if the object could be found and is valid. Otherwise an
	 *       error code, which is compatible with the HLT framework, is returned.
	 */
	int GetFloatFromTMap(
			TMap* map, const char* paramName, Double_t& value,
			const char* pathToEntry = NULL, const char* prettyName = NULL
		) const;
	
	/**
	 * Tries to find a certain parameter in the TMap object and convert it to
	 * an positive floating point value.
	 * [in] \param map  The TMap object to search in.
	 * [in] \param paramName  The name of the parameter to search for.
	 * [out] \param value  Will be filled with the floating point value for the
	 *       parameter, if it was found and it was a positive floating point value.
	 * [in] \param prettyName  Should be the name of the parameter which will
	 *      be used when printing error messages. If this is set to NULL then
	 *      the paramName will be used instead (default is NULL).
	 * \return Zero if the object could be found and is valid. Otherwise an
	 *       error code, which is compatible with the HLT framework, is returned.
	 */
	int GetPositiveFloatFromTMap(
			TMap* map, const char* paramName, Double_t& value,
			const char* pathToEntry = NULL, const char* prettyName = NULL
		) const;
	
private:

	// Do not allow copying of this class.
	/// Not implemented.
	AliHLTMUONProcessor(const AliHLTMUONProcessor& /*obj*/);
	/// Not implemented.
	AliHLTMUONProcessor& operator = (const AliHLTMUONProcessor& /*obj*/);
	
	ClassDef(AliHLTMUONProcessor, 0)  // Abstract base class for dHLT specific components.
};

//______________________________________________________________________________

template <class BlockType>
bool AliHLTMUONProcessor::BlockStructureOk(
		const BlockType& block, const char* name, bool checkHeader
	) const
{
	/// Performs basic checks to see if the input data block structure is OK,
	/// that it is not corrupt, too short etc...
	
	if (not block.BufferSizeOk())
	{
		size_t headerSize = sizeof(typename BlockType::HeaderType);
		if (block.BufferSize() < headerSize)
		{
			HLTError("Received a %s data block with a size of %d bytes,"
				" which is smaller than the minimum valid header size of %d bytes."
				" The block must be corrupt.",
				name, block.BufferSize(), headerSize
			);
			return false;
		}
		
		size_t expectedWidth = sizeof(typename BlockType::ElementType);
		if (block.CommonBlockHeader().fRecordWidth != expectedWidth)
		{
			HLTError("Received a %s data block with a record"
				" width of %d bytes, but the expected value is %d bytes."
				" The block might be corrupt.",
				name,
				block.CommonBlockHeader().fRecordWidth,
				expectedWidth
			);
			return false;
		}
		
		HLTError("Received a %s data block with a size of %d bytes,"
			" but the block header claims the block should be %d bytes."
			" The block might be corrupt.",
			name, block.BufferSize(), block.BytesUsed()
		);
		return false;
	}
	
	if (checkHeader)
	{
		AliHLTMUONUtils::WhyNotValid reason;
		if (not AliHLTMUONUtils::HeaderOk(block.BlockHeader(), &reason))
		{
			HLTError("Received a %s data block which might be corrupt. %s",
				name, AliHLTMUONUtils::FailureReasonToMessage(reason)
			);
			return false;
		}
	}
	
	return true;
}

#endif // ALIHLTMUONPROCESSOR_H
