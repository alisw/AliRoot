// $Id$
// Category: run
//
// See the class description in the header file.

#include "AliRunActionMessenger.h"
#include "AliRunAction.h"
#include "AliGlobals.h"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAnInteger.hh>

//_____________________________________________________________________________
AliRunActionMessenger::AliRunActionMessenger(AliRunAction* runAction)
  : fRunAction(runAction)
{
// 
  fRunActionDirectory = new G4UIdirectory("/aliRunAction/");
  fRunActionDirectory->SetGuidance("AliRunAction control commands.");

  fVerboseCmd = new G4UIcmdWithAnInteger("/aliRunAction/verbose", this);
  fVerboseCmd->SetGuidance("Set verbose level for AliRunAction");
  fVerboseCmd->SetParameterName("VerboseLevel", true);
  fVerboseCmd->SetDefaultValue(0);
  fVerboseCmd->SetRange("VerboseLevel >= 0 && VerboseLevel <= 2");
  fVerboseCmd->AvailableForStates(PreInit, Init, Idle, GeomClosed, EventProc);
}

//_____________________________________________________________________________
AliRunActionMessenger::AliRunActionMessenger() {
//
}

//_____________________________________________________________________________
AliRunActionMessenger::AliRunActionMessenger(const AliRunActionMessenger& right)
{
//
  AliGlobals::Exception("AliRunActionMessenger is protected from copying.");
}

//_____________________________________________________________________________
AliRunActionMessenger::~AliRunActionMessenger() {
//
  delete fRunActionDirectory;
  delete fVerboseCmd;
}

// operators

//_____________________________________________________________________________
AliRunActionMessenger& 
AliRunActionMessenger::operator=(const AliRunActionMessenger &right)
{
  // check assignement to self
  if (this == &right) return *this;
  
  AliGlobals::Exception("AliRunActionMessenger is protected from assigning.");

  return *this;
}

// public methods

//_____________________________________________________________________________
void AliRunActionMessenger::SetNewValue(G4UIcommand* command, 
       G4String newValue)
{ 
// Applies command to the associated object.
// ---

  if(command == fVerboseCmd) { 
    fRunAction
      ->SetVerboseLevel(fVerboseCmd->GetNewIntValue(newValue)); 
  }   
}
