#ifndef ALIHLTMUONHITRECONSTRUCTORCOMPONENT_H
#define ALIHLTMUONHITRECONSTRUCTORCOMPONENT_H
/* This file is property of and copyright by the ALICE HLT Project        *
 * ALICE Experiment at CERN, All rights reserved.                         *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///
///  @file   AliHLTMUONHitReconstructorComponent.h
///  @author Indranil Das <indra.das@saha.ac.in> | <indra.ehep@gmail.com>, Artur Szostak <artursz@iafrica.com>
///  @date   28 May 2007
///  @brief  Hit Reconstruction processing component for the dimuon HLT.
///

#include "AliHLTMUONProcessor.h"
#include "AliHLTMUONHitReconstructor.h"

#if __GNUC__ && __GNUC__ < 3
#define std
#endif


extern "C" struct AliHLTMUONHitRecoLutRow;

/**
 * @class AliHLTMUONHitReconstructorComponent
 * @brief A processing component for the dHLT tracker DDL reconstruction.
 *
 * This is the hit reconstruction component which forms part of the online dHLT
 * reconstruction algorithm. It processes the raw DDL data from a dimuon spectrometer
 * tracker station, applies simple 3 pad cluster finding and then a centre of gravity
 * calculation to reconstruct the hit coordinate.
 *
 * <h2>General properties:</h2>
 *
 * Component ID: \b MUONHitReconstructor <br>
 * Library: \b libAliHLTMUON.so  <br>
 * Input Data Types: ('DDL_RAW ', 'MUON') <br>
 * Output Data Types: ('RECHITS ', 'MUON') <br>
 *
 * <h2>Mandatory arguments:</h2>
 * \li -ddl <i>number</i> <br>
 *      This indicates the DDL from which the component is expect to receive data
 *      and for which it should load the appropriate electronics mapping lookup
 *      table.
 *      The <i>number</i> should be in the range [13..20], following local dimuon
 *      spectrometer DDL numbering. If either the -ddlid, -lut or -delaysetup
 *      arguments are used, then -ddl becomes optional. <br>
 * \li -ddlid <i>number</i> <br>
 *      This indicates the DDL by equipment ID, from which the component is expect
 *      to receive data and for which it should load the appropriate electronics
 *      mapping lookup table.
 *      The <i>number</i> should be in the range [2572..2579].
 *      If either the -ddl, -lut or -delaysetup arguments are used, then -ddlid
 *      becomes optional. <br>
 * \li -delaysetup <br>
 *      Specifying this option causes the component to initialise the lookup table
 *      and DC cut parameters from CDB only after receiving the first event to
 *      process in DoEvent.
 *      If -ddl or -ddlid were not used, then the DDL number will be taken from
 *      the first block's specification during runtime from the first
 *      event (Start-of-Run event).
 *      Using the -lut or -dccut arguments will override loading from CDB for a
 *      delayed setup. <br>
 *
 * <h2>Optional arguments:</h2>
 * \li -lut <i>filename</i> <br>
 *      A pree-built lookup table for the electronics mapping and calibration
 *      information can be loaded with this argument. The file should have been
 *      generated with the GenerateLookupTable method. The location of the file
 *      is given by the parameter <i>filename</i> <br>
 * \li -cdb <br>
 *      Indicates that the component should load from CDB. This option is implicit
 *      if the -cdbpath is given. It will also override the -lut option.<br>
 * \li -cdbpath <i>path</i> <br>
 *      Specifies the CDB path to use, given by <i>path</i>. This option will override
 *      the CDB path automatically set by the HLT framework. <br>
 * \li -run <i>number</i> <br>
 *      Specifies the run number to use, given by <i>number</i>. This option will
 *      override the current run number automatically set by the HLT framework. <br>
 * \li -dccut <i>number</i> <br>
 *      Used to override the DC cut parameter in the CDB with the value given by
 *      <i>number</i>. <br>
 * \li -warn_on_unexpected_block <br>
 *      This will cause the component to generate warnings when it receives data block
 *      types it does not know how to handle. Without this option the component only
 *      generates debug messages when they are compiled in. <br>
 * \li -tryrecover <br>
 *      This is a special option to the raw data decoder to turn on logic which will
 *      try and recover from corrupt raw DDL data. This is off by default. <br>
 *
 * <h2>Standard configuration:</h2>
 * This component should normally be configured with either of the two sets of
 * options in the XML configuration. <br>
 * \li -delaysetup <br>
 * \li -ddlid ${DDL_ID} <br>
 *
 * <h2>Default CDB entries:</h2>
 * The component loads electronics mapping and calibration information from the MUON
 * subdirectory in the CDB, MUON/Calib and MUON/Align.
 * The DC cut parameter is stored in a TMap under HLT/ConfigMUON/HitReconstructor
 * with a default value of 50 ADC channels.
 *
 * <h2>Performance:</h2>
 * Can achieve about 2kHz processing rate for nominal event sizes containing
 * 150 tracks per event.
 *
 * <h2>Memory consumption:</h2>
 * The lookup table requires about 3.5 MBytes of memory.
 *
 * <h2>Output size:</h2>
 * Output size is about 10% of incoming raw input data for nominal p+p events.
 *
 * @ingroup alihlt_dimuon_component
 */
class AliHLTMUONHitReconstructorComponent : public AliHLTMUONProcessor
{
public:
	AliHLTMUONHitReconstructorComponent();
	virtual ~AliHLTMUONHitReconstructorComponent();

	// Public functions to implement AliHLTComponent's interface.
	// These functions are required for the registration process

	virtual const char* GetComponentID();
	virtual void GetInputDataTypes(std::vector<AliHLTComponentDataType>& list);
	virtual AliHLTComponentDataType GetOutputDataType();
	virtual void GetOutputDataSize( unsigned long& constBase, double& inputMultiplier );
	virtual AliHLTComponent* Spawn();
	
	/**
	 * Generates an ASCII text file containing the lookup table (LUT) from
	 * the CDB, which can be used for the hit reconstructor component later.
	 * @param ddl  Must be the DDL for which to generate the DDL,
	 *             in the range [12..19].
	 * @param filename  The name of the LUT file to generate.
	 * @param cdbPath  The CDB path to use.
	 * @param run  The run number to use for the CDB.
	 * @return  True if the generation of the LUT file succeeded.
	 */
	static bool GenerateLookupTable(
			AliHLTInt32_t ddl, const char* filename,
			const char* cdbPath, Int_t run
		);
	
protected:
	
	// Protected functions to implement AliHLTComponent's interface.
	// These functions provide initialization as well as the actual processing
	// capabilities of the component.

	virtual int DoInit(int argc, const char** argv);
	virtual int Reconfigure(const char* cdbEntry, const char* componentId);
	virtual int ReadPreprocessorValues(const char* modules);
	virtual int DoDeinit();
	virtual int DoEvent(
			const AliHLTComponentEventData& evtData,
			const AliHLTComponentBlockData* blocks,
			AliHLTComponentTriggerData& trigData,
			AliHLTUInt8_t* outputPtr,
			AliHLTUInt32_t& size,
			std::vector<AliHLTComponentBlockData>& outputBlocks
		);
	
	using AliHLTProcessor::DoEvent;
	
private:

	// Do not allow copying of this class.
	/// Not implemented.
	AliHLTMUONHitReconstructorComponent(const AliHLTMUONHitReconstructorComponent& /*obj*/);
	/// Not implemented.
	AliHLTMUONHitReconstructorComponent& operator = (const AliHLTMUONHitReconstructorComponent& /*obj*/);
	
	void FreeMemory();
	int ReadLookUpTable(const char* lutpath);
	int ReadLutFromCDB();
	int ReadDCCutFromCDB();
	
	AliHLTMUONHitReconstructor* fHitRec;  ///< Internal class instance implementing the hit reconstruction algorithm.
	AliHLTInt32_t fDDL;  ///< DDL number in the range [12..19]. Set to -1 for invalid/unspecified value.
	AliHLTUInt32_t fLutSize;  ///< The number of rows / entries in the LUT.
	AliHLTMUONHitRecoLutRow* fLut;  ///< The lookup table used by the hit reconstruction algorithm (Owned by this component however).
	IdManuChannelToEntry fIdToEntry; ///< id to line mapping.
	bool fWarnForUnexpecedBlock;  ///< Flag indicating if we should log a warning if we got a block of an unexpected type.
	bool fDelaySetup;  ///< Indicates if the component should delay loading and initialising from the CDB to the start of run event.
	
	ClassDef(AliHLTMUONHitReconstructorComponent, 0) // Hit reconstructor component for dHLT tracker DDL raw data.
};

#endif // ALIHLTMUONHITRECONSTRUCTORCOMPONENT_H
