//-*- Mode: C++ -*-
// $Id$

#ifndef ALI_HLT_PENDOLINO_H
#define ALI_HLT_PENDOLINO_H
//* This file is property of and copyright by the ALICE HLT Project        * 
//* ALICE Experiment at CERN, All rights reserved.                         *
//* See cxx source for full Copyright notice                               *

/** @file   AliHLTPendolino.h
    @author Sebastian Bablok
    @date   
    @brief  
*/

//#include <TObject.h>
#include <TString.h>
#include <TMap.h>


#include <AliShuttleInterface.h>

//#include "AliHLTPredictionProcessorInterface.h"


class AliHLTPendolinoLogger;

/**
 * Class that implements the AliShuttleInterface and provides the required
 * features for the contacting the PredictionProcessor and storing the result
 * to the HCDB
 * 
 * @author Sebastian Bablok
 *
 * @date 2007-10-22
 */
class AliHLTPendolino : public AliShuttleInterface {
	public:

		/**
 		 * Static string that defines the local storage indicator in path
 		 */  
		static const TString kLOCAL_STORAGE_DEFINE;  // defines the local storage

		/**
		 * Static string defining the name of this inteface module.
		 */
		static const char* kHLTInterfaceModule;  // defines the name of inteface module

		/**
 		 * Static value defining error value for a Pendolino exception.
 		 */ 
		static const Int_t kHLTPendolinoException;  // error value for a Pendolino exception

		/**
 		 * Static value defining error value for a bad cast
 		 */ 
		static const Int_t kHLTPendolinoBadCast;  // error value for a bad cast

		/**
 		 * Static value defining error value for handed in module is not 
 		 * implementing the PredictionProcessor interface.
 		 */ 
		static const Int_t kHLTPendolinoNotPredictProc;  //  error value for "not implementing the PredictionProcessor interface"

		/**
		 * Static value defining error value for module not existing.
		 */ 
		static const Int_t kHLTPendolinoModuleNotExisting;  // error value for module not existing

		/**
		 * Static value defining error value for PredictionProc does not
		 * process DCS values.
		 */	
		static const Int_t kHLTPendolinoNoDCS; // error value for PredictionProc does not process DCS

		/**
 		 * Static string that defines the base folder for the Taxi list files.
 		 */ 
		static const TString kTaxiListBaseFolder;  // defines the base folder for the Taxi list files

		/**
		 * Static string that defines list folder name for taxi list
		 */
		static const TString kTaxiListFolderName; // defines list folder name for taxi list

		/**
 		 * Static string that defines the filename for the Taxi list required
 		 * by the Pendolino
 		 */  
		static const TString kTaxiListPendolino; // defines the filename for the Taxi list 

		/**
 		 * Static value that defines the max length of a line that can be read
 		 * when browsing through the list file
 		 */ 
		static const Int_t kMAX_LINE_LENGTH; // defines the max length of a line
		

		/**
		 * Constructor for AliHLTPendolino.
		 *
		 * @param run the current run number
		 * @param HCDBbase path to the HCDB base folder
		 * @param runType the current run type
		 * @param logger pointer to the desired logger; default is the command
		 * 			line logger.
		 */
		AliHLTPendolino(Int_t run, TString HCDBbase, TString runType = "TEST", 
				AliHLTPendolinoLogger* logger = 0);

		/**
		 * Destructor for
		 */
		virtual ~AliHLTPendolino();

		/**
		 * Function to store processed data in the HCDB
		 *
		 * @param path the storage path consisting of "detector/type/objectname"
		 * 			where detector should be the detector name used by offline,
		 * 			type is something like "CALIB" or "ALIGN", while 
		 * 			objectname the name represents, by which it can be requested
		 * 			from the HCDB.
		 * @param object the object to store
		 * @param metaData metaData to store
		 * @param validityStart object is valid from current run number minus 
		 * 			validityStart
		 * @param validityInfinite if true, validity is set to infinity
		 *
		 * @return true on success, else false
		 */
		virtual Bool_t Store(const AliCDBPath& path, TObject* object, 
				AliCDBMetaData* metaData, Int_t validityStart = 0, 
				Bool_t validityInfinite = kFALSE);
		/**
		 * Function is required from interface, but is disbled in its 
		 * implementation since additional refernce data will not be used by 
		 * the DAs inside the HLT. Therefore always returns false
		 *
		 * @return always false, since function is disabled.
		 */
	    virtual Bool_t StoreReferenceData(const AliCDBPath& path, 
				TObject* object, AliCDBMetaData* metaData);
		/**
		 * Function is required from interface, but is disbled in its
         * implementation since additional reference data will not be used by
         * the DAs inside the HLT. Therefore always returns false
         *
         * @return always false, since function is disabled.
         */
	    virtual Bool_t StoreReferenceFile(const char* detector, 
				const char* localFile, const char* gridFileName);

		/**
		 * Function is required from interface, but is disbled in its
		 * implementation since additional run meta data will only be stored
		 * by the the GPR - Preprocessor of the Offline Shuttle.
		 * 
		 * @return always false, since function is disabled.
		 */                  
		virtual Bool_t StoreRunMetadataFile(const char* localFile, 
				const char* gridFileName);

		/**
		 * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
		 * If this feature is lateron required by HLT, inherit from this class
		 * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
//	    virtual const char* GetFile(Int_t system, const char* detector, 
//				const char* id, const char* source);  --> is private now
	    
        /**
         * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
         * If this feature is lateron required by HLT, inherit from this class
         * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
//	    virtual TList* GetFileSources(Int_t system, const char* detector, 
//				const char* id = 0);  -> is priavte now
		
        /**
         * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
         * If this feature is lateron required by HLT, inherit from this class
         * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
//	    virtual TList* GetFileIDs(Int_t system, const char* detector, 
//				const char* source);  -> is private now
	    
		/**
		 * Retrieves current run parameter.
		 * 
		 * @param lbEntry name of the run parameter
		 *
		 * @return value of the run parameter
		 */
	    virtual const char* GetRunParameter(const char* lbEntry);

		/**
		 * Retrieves the current run type.		
		 *
		 * @return the current run type
		 */
		virtual const char* GetRunType();
		
		/**
		 * Returns the HLT status.
		 * Since the Pendolino is part of the HLT, the function will always
		 * return true
		 *
		 * @return always true - see above
		 */
	    virtual Bool_t GetHLTStatus();

		/**
		 * Retrieves a file from the OCDB, but does not uses the OCDB but the 
		 * HCDB (local copy of the OCDB). - Since the HCDB only contains OCDB
		 * objects, that are included in the list the Taxi uses for fetching 
		 * the data from the OCDB, it can happen, that the corresponding 
		 * AliCDBEntry is missing. 
		 * TODO: Think of mechanism to automatically include the object in the
		 * Taxi list.
		 * 
		 * @param detector the detectorname, to which the object belongs
		 * @param path the path of the object
		 *
		 * @return pointer to the fetched HCDB entry
		 */
	    virtual AliCDBEntry* GetFromOCDB(const char* detector, 
				const AliCDBPath& path);
	    
		/**
		 * Function to allow Pendolino and PredictionProcessor to make log 
		 * entries.
		 *
		 * @param detector the detector, that wants to issue this message
		 * @param message the log message
		 */
	    virtual void Log(const char* detector, const char* message);

		/**
		 * Function is required from interface, but is disbled in the Pendolino
		 * because at the moment there is no Trigger configuration available 
		 * for Pendolino. At the moment it just return a NULL pointer and makes
		 * some log output.
		 *
		 * @return NULL pointer, since it should not be used.
		 */
//		virtual const char* GetTriggerConfiguration(); --> is private now

		/**
		 * Registers a preprocessor; actually it has to be a PredictionProcessor
		 * since the Pendolino requires a PredictionProcessor. If the registered
		 * preprocessor is not implementing a PredictionProcessor, a log entry
		 * is made and the registration discarded.
		 *
		 * @param preprocessor the PredictionProcessor that shall be registered.
		 * 			For more details please see above !!
		 */ 
		virtual void RegisterPreprocessor(AliPreprocessor* preprocessor);
//					AliHLTPredictionProcessorInterface* preprocessor);

		/**
		 * Function to get the current run number
		 *
		 * @return current run number
		 */
		virtual Int_t GetRunNumber();

		/**
		 * Function to enable prediction making in all registered 
		 * PredictionProcessors, if they implement the required interface
		 *          
		 * @return number of PredictionProcessor, which has been switched to
		 * 				prediction making (NOTE: the internal list may conatin 
		 * 				more PredictionProcessors, but if switching on failed 
		 * 				for one, this one is not counted.)
		 */
		virtual UInt_t setToPredictMaking(); 

		/**
		 * Function to get the number of registered PredictionProcessors
		 *
		 * @return number of registered PredictionProcessors
		 */
		Int_t getNumberOfPredictProc();

		/**
		 * Function to check if given PredtionProc allows for processing DCS
		 * and enable prediction making
		 *
		 * @param detector the detector in whose PredictionProcessor the 
		 * 			prediction making shall be enabled.
		 *
		 * @return 0 on success, else an error code is returned
		 */
		virtual Int_t setToPredictMaking(TString detector);

		/**
		 * Function to initlaize a dedicated Prediction Processor
		 * 
		 * @param detector the detector, whose PredictProc shall be initialized
		 * @param run the current run number
		 * @param startTime the start time of the fetched data
		 * @param endTime the end time of the fetched data
		 *
		 * @return 0 on success, else an error code is returned
		 */
		virtual Int_t initPredictProc(TString detector, Int_t run, 
					UInt_t startTime, UInt_t endTime);

		/**
		 * Function to hand in retrieved DCS values. These values are handed to
		 * the corresponding PredictionProcessor of the according detector.
		 * The PredictionProcessor should prepare the data inside and store 
		 * them to the HCDB.
		 *
		 * @param detector the according detector, to whose PredictionProcessor
		 * 			shall prepare the handed-in data
		 * @param DCSValues pointer to the map containing the fetched DCS values
		 *
		 * @return 0 on success, else an error code is returned.
		 */
		virtual Int_t prepareDCSValues(TString detector, TMap* DCSValues);

		/**
		 * Function to retrieve dummy data for testing the Pendolino from a
		 * given PredictionProcessor. The function called is handed further to
		 * the corresponding PredictionProcessor.
		 * NOTE: The returned TMap can be NULL, if no corresponding 
		 * PredictionProcessor is registered.
		 *
		 * @param detector the according detector, from whom the 
		 * 			PredictionProcessor shall produce the dummy data.
		 * @param aliasName optional parameter to hand in a alias name for 
		 * 			producing a DCSMap for the given alias.
		 *
		 * @return the DCSMap with the dummy data to test (given by the 
		 * 			PredictionProcessor). NOTE: can be NULL, if no corresponding
		 * 			PredictionProcessor is registered.
		 */
		virtual TMap* emulateDCSMap(TString detector, TString aliasName = "");
   
		/**
 		 * Function to add a entry request to the Taxi lists.
 		 *
 		 * @param entryPath the path entry, that shall be included in the 
 		 * 				list file.
 		 *
 		 * @return true, when successful included or entry already existing in 
 		 * 				list; else false.
 		 */
		virtual Bool_t includeAliCDBEntryInList(const TString& entryPath); 
		
	protected:

		
	private:
        /**
         * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
         * If this feature is lateron required by HLT, inherit from this class
         * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
        virtual TList* GetFileIDs(Int_t system, const char* detector,
                const char* source);

        /**
         * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
         * If this feature is lateron required by HLT, inherit from this class
         * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
        virtual TList* GetFileSources(Int_t system, const char* detector,
                const char* id = 0);

        /**
         * Function is required from interface, but is disbled in its
         * implementation since additional refernce data will not be used by
         * the DAs inside the HLT. Therefore always returns NULL.
         * If this feature is lateron required by HLT, inherit from this class
         * and overwrite this function with the required functionality.
         *
         * @return always NULL, since not used.
         */
        virtual const char* GetFile(Int_t system, const char* detector,
                const char* id, const char* source);

        /**
         * Function is required from interface, but is disbled in the Pendolino
         * because at the moment there is no Trigger configuration available
         * for Pendolino. At the moment it just return a NULL pointer and makes
         * some log output.
         *
         * @return NULL pointer, since it should not be used.
         */
		virtual const char* GetTriggerConfiguration();

		/**
		 * Disable the default constructor.
		 */
		AliHLTPendolino();

		/** copy constructor prohibited */
		AliHLTPendolino(const AliHLTPendolino&);
		/** assignment operator prohibited */
		AliHLTPendolino& operator=(const AliHLTPendolino&);

		/**
		 * Stores the current run type
		 */
		TString mRunType;  // Stores the current run type

		/**
		 * Map that stores the all PredictionProcessors with their name
		 * (detector)
		 */
		TMap mPredictionProcessorMap;  // stores the all PredictionProcessors

		/**
		 * Pointer to the used Pendolino logger
		 */
		AliHLTPendolinoLogger* mpLogger; // Pointer to the used Pendolino logger

		/**
 		 * Stores the current run number
 		 */
		Int_t mRunNumber;  // Stores the current run number

		/**
 		 * Stores the HCDBpath
 		 */
		TString mHCDBPath;	 // Stores the HCDBpath

		/**
		 * Indicates, if Logger is owned by Pendolino
		 */
		Bool_t mOwnLogger;  //  Indicates, if Logger is owned by Pendolino
		
		ClassDef(AliHLTPendolino, 4);

};


inline const char* AliHLTPendolino::GetRunType() {
	// getter for run type
	return mRunType.Data();
}

inline Int_t AliHLTPendolino::GetRunNumber() {
	// getter for run number
	return mRunNumber;
}

inline Int_t AliHLTPendolino::getNumberOfPredictProc() {
	// getter for number of registered PredictionProcessors
	return mPredictionProcessorMap.GetSize();
}


#endif

