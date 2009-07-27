#ifndef ALIHLTTRDUTILS_H
#define ALIHLTTRDUTILS_H
/* Copyright(c) 1998-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  HLT TRD Utillities Class                                              //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


#include "AliHLTDataTypes.h"
#include "TObject.h"
//#include "AliHLTProcessor.h"

class TClonesArray;
class AliESDEvent;
class AliHLTTRDUtils
{
public:
  static AliHLTUInt32_t AddClustersToOutput(TClonesArray* inClusterArray, AliHLTUInt8_t* outBlockPtr);
  static AliHLTUInt32_t AddTracksToOutput(TClonesArray* inTrackArray,AliHLTUInt8_t* output);
  static AliHLTUInt32_t ReadClusters(TClonesArray *outArray, void* inputPtr, AliHLTUInt32_t size);
  static AliHLTUInt32_t ReadTracks(TClonesArray *outArray, void* inputPtr, AliHLTUInt32_t size);
  static AliHLTUInt32_t AddESDToOutput(const AliESDEvent* const esd, AliHLTUInt8_t* const outBlockPtr);

  ClassDef(AliHLTTRDUtils, 0)


};

#endif
