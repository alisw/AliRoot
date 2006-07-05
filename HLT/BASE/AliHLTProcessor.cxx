// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          Timm Steinbeck <timm@kip.uni-heidelberg.de>                   *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTProcessor.cxx
    @author Matthias Richter, Timm Steinbeck
    @date   
    @brief  Base class implementation for HLT analysis components. */

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTProcessor.h"
#include <string.h>

ClassImp(AliHLTProcessor)

AliHLTProcessor::AliHLTProcessor()
{ 
}

AliHLTProcessor::~AliHLTProcessor()
{ 
}

int AliHLTProcessor::Init( AliHLTComponentEnvironment* environ, void* environ_param, int argc, const char** argv )
{
  int iResult=0;
  iResult=AliHLTComponent::Init(environ, environ_param, argc, argv);
  return iResult;
}

int AliHLTProcessor::Deinit()
{
  int iResult=0;
  iResult=AliHLTComponent::Deinit();
  return iResult;
}

int AliHLTProcessor::ProcessEvent( const AliHLTComponent_EventData& evtData, const AliHLTComponent_BlockData* blocks, 
			    AliHLTComponent_TriggerData& trigData, AliHLTUInt8_t* outputPtr, 
			    AliHLTUInt32_t& size, AliHLTUInt32_t& outputBlockCnt, 
			    AliHLTComponent_BlockData*& outputBlocks,
			    AliHLTComponent_EventDoneData*& edd )
{
  int iResult=0;
  vector<AliHLTComponent_BlockData> blockData;
  iResult=DoEvent(evtData, blocks, trigData, outputPtr, size, blockData);
  if (iResult>=0) {
    iResult=MakeOutputDataBlockList(blockData, &outputBlockCnt, &outputBlocks);
  }
  edd = NULL;
  return iResult;
}
