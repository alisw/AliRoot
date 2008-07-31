/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors:                                                       *
 *   Indranil Das <indra.das@saha.ac.in>                                  *
 *   Artur Szostak <artursz@iafrica.com>                                  *
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

/**
 * @file   AliHLTMUONConstants.cxx
 * @author Indranil Das <indra.das@saha.ac.in>,
 *         Artur Szostak <artursz@iafrica.com>
 * @date   
 * @brief  Definitions of the various dimuon HLT constants.
 */

#include "AliHLTMUONConstants.h"
#include "AliHLTMUONTriggerRecordsBlockStruct.h"
#include "AliHLTMUONTrigRecsDebugBlockStruct.h"
#include "AliHLTMUONRecHitsBlockStruct.h"
#include "AliHLTMUONClustersBlockStruct.h"
#include "AliHLTMUONChannelsBlockStruct.h"
#include "AliHLTMUONMansoTracksBlockStruct.h"
#include "AliHLTMUONMansoCandidatesBlockStruct.h"
#include "AliHLTMUONSinglesDecisionBlockStruct.h"
#include "AliHLTMUONPairsDecisionBlockStruct.h"


const AliHLTMUONTriggerRecordStruct
AliHLTMUONConstants::fgkNilTriggerRecordStruct = {
	0, 0, 0, 0, 0,
	{
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct
	}
};

const AliHLTMUONTrigRecInfoStruct
AliHLTMUONConstants::fgkNilTrigRecInfoStruct = {0, {0, 0, 0, 0}, 0, 0, {0, 0, 0, 0, 0}};
	
const AliHLTMUONRecHitStruct
AliHLTMUONConstants::fgkNilRecHitStruct = {0, 0, 0, 0};

const AliHLTMUONClusterStruct
AliHLTMUONConstants::fgkNilClusterStruct = {
	0, AliHLTMUONConstants::fgkNilRecHitStruct, 0, 0
};

const AliHLTMUONChannelStruct
AliHLTMUONConstants::fgkNilChannelStruct = {0, 0, 0, 0, 0, 0};

const AliHLTMUONMansoTrackStruct
AliHLTMUONConstants::fgkNilMansoTrackStruct = {
	0, 0, 0, 0, 0, 0, 0,
	{
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct,
	 AliHLTMUONConstants::fgkNilRecHitStruct
	}
};
	
const AliHLTMUONMansoRoIStruct
AliHLTMUONConstants::fgkNilMansoRoIStruct = {0, 0, 0, 0};

const AliHLTMUONMansoCandidateStruct
AliHLTMUONConstants::fgkNilMansoCandidateStruct = {
	AliHLTMUONConstants::fgkNilMansoTrackStruct,
	{
	 AliHLTMUONConstants::fgkNilMansoRoIStruct,
	 AliHLTMUONConstants::fgkNilMansoRoIStruct,
	 AliHLTMUONConstants::fgkNilMansoRoIStruct,
	 AliHLTMUONConstants::fgkNilMansoRoIStruct
	},
	0, 0
};

const AliHLTMUONTrackDecisionStruct
AliHLTMUONConstants::fgkNilTrackDecisionStruct = {0, 0, 0};

const AliHLTMUONPairDecisionStruct
AliHLTMUONConstants::fgkNilPairDecisionStruct = {0, 0, 0, 0};


const AliHLTComponentDataType
AliHLTMUONConstants::fgkDDLRawDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	kAliHLTDDLRawDataTypeID,
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkTriggerRecordsBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'T','R','I','G','R','E','C','S'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkTrigRecsDebugBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'T','R','I','G','R','D','B','G'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkRecHitsBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'R','E','C','H','I','T','S',' '},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkClusterBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'C','L','U','S','T','E','R','S'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkChannelBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'C','H','A','N','N','E','L','S'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkMansoTracksBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'M','A','N','T','R','A','C','K'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkMansoCandidatesBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'M','N','C','A','N','D','I','D'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkSinglesDecisionBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'D','E','C','I','D','S','I','N'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkPairsDecisionBlockDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'D','E','C','I','D','P','A','R'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkESDDataType = kAliHLTDataTypeESDObject | kAliHLTDataOriginMUON;

const AliHLTComponentDataType
AliHLTMUONConstants::fgkClusterStoreDataType = (AliHLTComponentDataType){
	sizeof(AliHLTComponentDataType),
	{'C','L','U','S','T','O','R','E'},
	kAliHLTDataOriginAny
} | kAliHLTDataOriginMUON;


const char* AliHLTMUONConstants::fgkRecHitsSourceId = "MUONRecHitsSource";
const char* AliHLTMUONConstants::fgkTriggerRecordsSourceId = "MUONTriggerRecordsSource";
const char* AliHLTMUONConstants::fgkTracksSourceId = "MUONTracksSource";
const char* AliHLTMUONConstants::fgkDigitPublisherId = "MUONDigitPublisher";
const char* AliHLTMUONConstants::fgkTriggerReconstructorId = "MUONTriggerReconstructor";
const char* AliHLTMUONConstants::fgkHitReconstructorId = "MUONHitReconstructor";
const char* AliHLTMUONConstants::fgkMansoTrackerFSMId = "MUONMansoTrackerFSM";
const char* AliHLTMUONConstants::fgkDecisionComponentId = "MUONDecisionComponent";
const char* AliHLTMUONConstants::fgkESDMakerId = "MUONESDMaker";
const char* AliHLTMUONConstants::fgkRootifierComponentId = "MUONRootifier";
const char* AliHLTMUONConstants::fgkEmptyEventFilterComponentId = "MUONEmptyEventFilter";
const char* AliHLTMUONConstants::fgkDataCheckerComponentId = "MUONDataChecker";
const char* AliHLTMUONConstants::fgkClusterFinderId = "MUONClusterFinder";

const char* AliHLTMUONConstants::fgkTriggerReconstructorCDBPath = "HLT/ConfigMUON/TriggerReconstructor";
const char* AliHLTMUONConstants::fgkHitReconstructorCDBPath = "HLT/ConfigMUON/HitReconstructor";
const char* AliHLTMUONConstants::fgkMansoTrackerFSMCDBPath = "HLT/ConfigMUON/MansoTrackerFSM";
const char* AliHLTMUONConstants::fgkDecisionComponentCDBPath = "HLT/ConfigMUON/DecisionComponent";

