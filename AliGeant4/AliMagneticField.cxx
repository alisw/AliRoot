// $Id$
// Category: geometry
//
// Author: I. Hrivnacova
//
// Class TG4ParticlesManager
// -------------------------
// See the class description in the header file.
// According to:
// Id: ExN02MagneticField.cc,v 1.1 1999/01/07 16:05:49 gunter Exp 
// GEANT4 tag Name: geant4-00-01

#include "AliMagneticField.h"

#include <G4FieldManager.hh>
#include <G4TransportationManager.hh>

//  Constructors

//_____________________________________________________________________________
AliMagneticField::AliMagneticField()
  : G4UniformMagField(G4ThreeVector()) 
{
//
  GetGlobalFieldManager()->SetDetectorField(this);
  GetGlobalFieldManager()->CreateChordFinder(this);
}

//_____________________________________________________________________________
AliMagneticField::AliMagneticField(G4ThreeVector fieldVector)
  : G4UniformMagField(fieldVector)
{    
//
  GetGlobalFieldManager()->SetDetectorField(this);
  GetGlobalFieldManager()->CreateChordFinder(this);
}

//_____________________________________________________________________________
AliMagneticField::AliMagneticField(const AliMagneticField& right)
  : G4UniformMagField(right)
{
//  
  GetGlobalFieldManager()->SetDetectorField(this);
  GetGlobalFieldManager()->CreateChordFinder(this);
}

//_____________________________________________________________________________
AliMagneticField::~AliMagneticField() {
//
}

// operators

//_____________________________________________________________________________
AliMagneticField& 
AliMagneticField::operator=(const AliMagneticField& right)
{				  
  // check assignement to self
  if (this == &right) return *this;

  // base class assignement
  G4UniformMagField::operator=(right);
  
  return *this;
}  

// public methods

//_____________________________________________________________________________
void AliMagneticField::SetFieldValue(G4double fieldValue)
{
// Sets the value of the Global Field to fieldValue along Z.
// ---

  G4UniformMagField::SetFieldValue(G4ThreeVector(0,0,fieldValue));
}

//_____________________________________________________________________________
void AliMagneticField::SetFieldValue(G4ThreeVector fieldVector)
{
// Sets the value of the Global Field.
// ---

  // Find the Field Manager for the global field
  G4FieldManager* fieldMgr= GetGlobalFieldManager();
    
  if(fieldVector!=G4ThreeVector(0.,0.,0.)) { 
    G4UniformMagField::SetFieldValue(fieldVector); 
    fieldMgr->SetDetectorField(this);
  } 
  else {
    // If the new field's value is Zero, then it is best to
    //  insure that it is not used for propagation.
    G4MagneticField* magField = 0;
    fieldMgr->SetDetectorField(magField);
  }
}

//_____________________________________________________________________________
G4FieldManager*  AliMagneticField::GetGlobalFieldManager()
{
// Utility method
// ---

  return G4TransportationManager::GetTransportationManager()
           ->GetFieldManager();
}
    
