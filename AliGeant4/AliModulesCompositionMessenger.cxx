// $Id$
// Category: geometry
//
// Author: I. Hrivnacova
//
// Class AliModulesCompositionMessenger
// ------------------------------------
// See the class description in the header file.

#include "AliModulesCompositionMessenger.h"
#include "AliModulesComposition.h"
#include "AliGlobals.h"

#include <G4UIdirectory.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithoutParameter.hh>
#include <G4UIcmdWithABool.hh>
#include <G4UIcmdWithADoubleAndUnit.hh>

//_____________________________________________________________________________
AliModulesCompositionMessenger::AliModulesCompositionMessenger(
                                   AliModulesComposition* modulesComposition)
  : fModulesComposition(modulesComposition)
{
//
  fDirectory = new G4UIdirectory("/aliDet/");
  fDirectory->SetGuidance("Detector construction control commands.");

  fSwitchOnCmd = new G4UIcmdWithAString("/aliDet/switchOn", this);
  fSwitchOnCmd->SetGuidance("Define the module to be built.");
  fSwitchOnCmd->SetGuidance("Available modules:");
  G4String listAvailableDets = "NONE, ALL, ";  
  listAvailableDets 
    = listAvailableDets + modulesComposition->GetAvailableDetsListWithCommas();
  fSwitchOnCmd->SetGuidance(listAvailableDets);
  fSwitchOnCmd->SetParameterName("module", false);
  fSwitchOnCmd->AvailableForStates(PreInit);;

  fSwitchOffCmd = new G4UIcmdWithAString("/aliDet/switchOff", this);
  fSwitchOffCmd->SetGuidance("Define the module not to be built.");
  fSwitchOffCmd->SetGuidance("Available modules:");
  G4String listDetsNames = "ALL, "; 
  listDetsNames
    = listDetsNames + modulesComposition->GetDetNamesListWithCommas();
  fSwitchOffCmd->SetGuidance(listDetsNames);
  fSwitchOffCmd->SetParameterName("module", false);
  fSwitchOffCmd->AvailableForStates(PreInit);;

  fListCmd 
    = new G4UIcmdWithoutParameter("/aliDet/list", this);
  fListCmd->SetGuidance("List the currently switched modules.");
  fListCmd
    ->AvailableForStates(PreInit, Init, Idle, GeomClosed, EventProc);

  fListAvailableCmd 
    = new G4UIcmdWithoutParameter("/aliDet/listAvailable", this);
  fListAvailableCmd->SetGuidance("List all available modules.");
  fListAvailableCmd
    ->AvailableForStates(PreInit, Init, Idle, GeomClosed, EventProc);

  fFieldValueCmd = new G4UIcmdWithADoubleAndUnit("/aliDet/fieldValue", this);
  fFieldValueCmd->SetGuidance("Define magnetic field in Z direction.");
  fFieldValueCmd->SetParameterName("fieldValue", false, false);
  fFieldValueCmd->SetDefaultUnit("tesla");
  fFieldValueCmd->SetUnitCategory("Magnetic flux density");
  fFieldValueCmd->AvailableForStates(PreInit,Idle);  
  
  fSetReadGeometryCmd 
    = new G4UIcmdWithABool("/aliDet/readGeometry", this);
  fSetReadGeometryCmd->SetGuidance("Read geometry from g3calls.dat files");
  fSetReadGeometryCmd->SetParameterName("readGeometry", false);
  fSetReadGeometryCmd->AvailableForStates(PreInit);  
 
  fSetWriteGeometryCmd 
    = new G4UIcmdWithABool("/aliDet/writeGeometry", this);
  fSetWriteGeometryCmd->SetGuidance("Write geometry to g3calls.dat file");
  fSetWriteGeometryCmd->SetParameterName("writeGeometry", false);
  fSetWriteGeometryCmd->AvailableForStates(PreInit);   

  fPrintMaterialsCmd 
    = new G4UIcmdWithoutParameter("/aliDet/printMaterials", this);
  fPrintMaterialsCmd->SetGuidance("Prints all materials.");
  fPrintMaterialsCmd->AvailableForStates(PreInit, Init, Idle);   

  fGenerateXMLCmd 
    = new G4UIcmdWithoutParameter("/aliDet/generateXML", this);
  fGenerateXMLCmd->SetGuidance("Generate geometry XML file.");
  fGenerateXMLCmd->AvailableForStates(Idle);   


  // set candidates list
  SetCandidates();

  // set default values to a detector
  fModulesComposition->SwitchDetOn("NONE");
}

//_____________________________________________________________________________
AliModulesCompositionMessenger::AliModulesCompositionMessenger() {
//
}

//_____________________________________________________________________________
AliModulesCompositionMessenger::AliModulesCompositionMessenger(
                                const AliModulesCompositionMessenger& right)
{
//
  AliGlobals::Exception(
    "AliModulesCompositionMessenger is protected from copying.");
}

//_____________________________________________________________________________
AliModulesCompositionMessenger::~AliModulesCompositionMessenger() {
//
  delete fDirectory;
  delete fSwitchOnCmd;
  delete fSwitchOffCmd;
  delete fListCmd;
  delete fListAvailableCmd;
  delete fFieldValueCmd;
  delete fSetReadGeometryCmd;
  delete fSetWriteGeometryCmd;
  delete fPrintMaterialsCmd;
  delete fGenerateXMLCmd;
}

// operators

//_____________________________________________________________________________
AliModulesCompositionMessenger& 
AliModulesCompositionMessenger::operator=(
                                const AliModulesCompositionMessenger& right)
{
  // check assignement to self
  if (this == &right) return *this;

  AliGlobals::Exception(
     "AliModulesCompositionMessenger is protected from assigning.");
    
  return *this;  
}    
          
// public methods
  
//_____________________________________________________________________________
void AliModulesCompositionMessenger::SetNewValue(G4UIcommand* command, G4String newValues)
{
// Applies command to the associated object.
// ---

  if (command == fSwitchOnCmd) {  
    fModulesComposition->SwitchDetOn(newValues); 
  }
  else if (command == fSwitchOffCmd) {  
    fModulesComposition->SwitchDetOff(newValues); 
  }
  else if (command == fListCmd) {  
    fModulesComposition->PrintSwitchedDets(); 
  }
  else if (command == fListAvailableCmd) {  
    fModulesComposition->PrintAvailableDets(); 
  }
  else if (command == fFieldValueCmd) {  
    fModulesComposition
      ->SetMagField(fFieldValueCmd->GetNewDoubleValue(newValues)); 
  }
  else if (command == fSetReadGeometryCmd) {
    fModulesComposition->SetReadGeometry(
                         fSetReadGeometryCmd->GetNewBoolValue(newValues));
  }  
  else if (command == fSetWriteGeometryCmd) {
    fModulesComposition->SetWriteGeometry(
                         fSetWriteGeometryCmd->GetNewBoolValue(newValues));
  }    
  else if (command == fPrintMaterialsCmd) {
    fModulesComposition->PrintMaterials();
  }    
  else if (command == fGenerateXMLCmd) {
    fModulesComposition->GenerateXMLGeometry();
  }    
}

//_____________________________________________________________________________
void AliModulesCompositionMessenger::SetCandidates() 
{
// Builds candidates list.
// ---

  G4String candidatesList = "NONE ALL ";
  candidatesList += fModulesComposition->GetDetNamesList();;
  candidatesList += fModulesComposition->GetAvailableDetsList();
  fSwitchOnCmd->SetCandidates(candidatesList);

  candidatesList = "ALL ";
  candidatesList += fModulesComposition->GetDetNamesList();;
  fSwitchOffCmd->SetCandidates(candidatesList);
}
