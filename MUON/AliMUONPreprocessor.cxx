/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/

// $Id$

#include "AliMUONPreprocessor.h"
#include "TObjArray.h"
#include "AliMUONPedestalSubprocessor.h"
#include "Riostream.h"

/// \class AliMUONPreprocessor
///
/// Shuttle preprocessor for MUON subsystems (TRK and TRG)
/// 
/// It's simply a manager class that deals with a list of sub-tasks 
/// (of type AliMUONVSubprocessor).
///
/// \author Laurent Aphecetche

/// \cond CLASSIMP
ClassImp(AliMUONPreprocessor)
/// \endcond

//_____________________________________________________________________________
AliMUONPreprocessor::AliMUONPreprocessor(const char* detector, 
                                         AliShuttleInterface* shuttle) 
: AliPreprocessor(detector,shuttle), fSubprocessors(new TObjArray[kLast])
{
  /// ctor. Builds the list of subtasks
  ///
  /// \todo FIXME: should test detector wrt to tracker or trigger to 
  /// instantiate the correct list of subtasks, which should be : 
  /// Tracker : 
  /// pedestals
  /// gains
  /// deadchannels
  /// gms
  ///
  /// Trigger : 
  /// masks
  /// lut
  ///
  fSubprocessors->AddAt(new AliMUONPedestalSubprocessor(this),kPedestal);
}

//_____________________________________________________________________________
AliMUONPreprocessor::~AliMUONPreprocessor()
{
  /// dtor
  delete fSubprocessors;
}

//_____________________________________________________________________________
void
AliMUONPreprocessor::Initialize(Int_t run, UInt_t startTime, UInt_t endTime)
{
  /// loop over subtasks and initialize them
  for ( Int_t i = 0; i <= fSubprocessors->GetLast(); ++i )
  {
    Subprocessor(i)->Initialize(run,startTime,endTime);
  }
}

//_____________________________________________________________________________
UInt_t
AliMUONPreprocessor::Process(TMap* dcsAliasMap)
{
  /// loop over subtasks to make them work
  UInt_t rv(0);
  
  for ( Int_t i = 0; i <= fSubprocessors->GetLast(); ++i )
  {
    rv += Subprocessor(i)->Process(dcsAliasMap);
  }
  return rv;
}

//_____________________________________________________________________________
void
AliMUONPreprocessor::Print(Option_t* opt) const
{
  /// output to screen
  cout << "<AliMUONPreprocessor> subprocessors :" << endl;
  for ( Int_t i=0; i <= fSubprocessors->GetLast(); ++i )
  {
    Subprocessor(i)->Print(opt);
  }
}

//_____________________________________________________________________________
AliMUONVSubprocessor*
AliMUONPreprocessor::Subprocessor(Int_t i) const
{
  /// return i-th subprocessor
  if ( i >= 0 && i <= fSubprocessors->GetLast() )
  {
    return static_cast<AliMUONVSubprocessor*>(fSubprocessors->At(i));
  }
  return 0x0;
}
