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
// $MpId: AliMpTrigger.cxx,v 1.4 2006/05/24 13:58:52 ivana Exp $

//-----------------------------------------------------------------------------
// Class AliMUONTriggerCrateConfig
// --------------------
// The class defines the configuration of trigger crate
// Author: Ch. Finck, Subatech Nantes
//-----------------------------------------------------------------------------

#include "AliMUONTriggerCrateConfig.h"

#include "AliLog.h"

#include <Riostream.h>

/// \cond CLASSIMP
ClassImp(AliMUONTriggerCrateConfig)
/// \endcond

 //______________________________________________________________________________
AliMUONTriggerCrateConfig::AliMUONTriggerCrateConfig(AliMpTriggerCrate* mpTriggerCrate)
  : TObject(),
    fMpCrate(mpTriggerCrate),
    fMask(0),
    fMode(0),
    fCoinc(0)
{
/// Standard constructor for Shuttle + DA
}


 //______________________________________________________________________________
AliMUONTriggerCrateConfig::AliMUONTriggerCrateConfig(TRootIOCtor* ioCtor)
  : TObject(),
    fMpCrate(0x0),
    fMask(0),
    fMode(0),
    fCoinc(0)
{
/// Standard constructor for Shuttle + DA
}


//______________________________________________________________________________
AliMUONTriggerCrateConfig::~AliMUONTriggerCrateConfig()
{
/// Destructor
}

//
// public methods
//

//______________________________________________________________________________
Bool_t AliMUONTriggerCrateConfig::AddLocalBoard(Int_t localBoardId)
{
/// Add local boards with given detElemId.
/// Return true if the local board was added

  return fMpCrate->AddLocalBoard(localBoardId);
}   


//______________________________________________________________________________
Int_t AliMUONTriggerCrateConfig::GetNofLocalBoards() const
{  
/// Return the number of local board in this crate

  return fMpCrate->GetNofLocalBoards(); 
}

//______________________________________________________________________________
Int_t  AliMUONTriggerCrateConfig::GetLocalBoardId(Int_t index) const
{  
/// Return the local board by index (in loop)

  return fMpCrate->GetLocalBoardId(index);
}

//______________________________________________________________________________
Bool_t  AliMUONTriggerCrateConfig::HasLocalBoard(Int_t localBoardId) const
{  
/// Return true if crate has local boardwith given localBoardId

  return fMpCrate->HasLocalBoard(localBoardId); 
}

