// $Id$
// Category: geometry
// by I. Hrivnacova, 12.10.2000 
//
// See the class description in the header file.

#include "TG4Polycone.h"
#include "TG4Globals.h"

//_____________________________________________________________________________
TG4Polycone::TG4Polycone(const G4Polycone& rhs) 
  : G4Polycone(rhs) {
//
}

//_____________________________________________________________________________
TG4Polycone::~TG4Polycone() {
//
}

// private methods

//_____________________________________________________________________________
void TG4Polycone::CheckOrigin() 
{
// Checks if polycone was created in a "historical way"
// and give exception otherwise.
// ---

  if (!original_parameters) {
    G4String text = "TG4Polycone::CheckOrigin: \n";
    text = text + "    Polycone has not defined original parameters.";
    TG4Globals::Exception(text);
  }  
}


// public methods

//_____________________________________________________________________________
G4int TG4Polycone::GetNofZPlanes()
{
// Returns nof z planes.
// ----
  
  CheckOrigin();

  return original_parameters->Num_z_planes;
}  

//_____________________________________________________________________________
G4double* TG4Polycone::GetRmin()
{
// Returns array of rmin parameters of the planes.
// ----
  
  CheckOrigin();

  return original_parameters->Rmin;
}  

//_____________________________________________________________________________
G4double* TG4Polycone::GetRmax()
{
// Returns array of rmax parameters of the planes.
// ----

  CheckOrigin();
  
  return original_parameters->Rmax;
}  

//_____________________________________________________________________________
G4double* TG4Polycone::GetZ()
{
// Returns array of z parameters of the planes.
// ----

  CheckOrigin();
  
  return original_parameters->Z_values;
}  
