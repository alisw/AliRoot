// $Id$
// Category: physics
//
// Author: I. Hrivnacova
//
// Class TG4PhysicsConstructorOptical
// ----------------------------------
// See the class description in the header file.
// According to ExN06PhysicsList (geant4 1.1)

#include "TG4PhysicsConstructorOptical.h"
#include "TG4ProcessControlMap.h"
#include "TG4ProcessMCMap.h"

#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4Cerenkov.hh>
#include <G4OpAbsorption.hh>
#include <G4OpRayleigh.hh>
#include <G4OpBoundaryProcess.hh>

//_____________________________________________________________________________
TG4PhysicsConstructorOptical::TG4PhysicsConstructorOptical(const G4String& name)
  : G4VPhysicsConstructor(name)
{
//
  SetVerboseLevel(1);
}

//_____________________________________________________________________________
TG4PhysicsConstructorOptical::~TG4PhysicsConstructorOptical() {
//
}

// protected methods

//_____________________________________________________________________________
void TG4PhysicsConstructorOptical::ConstructParticle()
{
// The particles are constructed in the 
// TG4ModularPhysicsList.
// ---
}

//_____________________________________________________________________________
void TG4PhysicsConstructorOptical::ConstructProcess()
{
// Constructs optical processes.
// According to ExN06PhysicsList.cc.
// (geant4 1.1)
// ---

  G4Cerenkov*     theCerenkovProcess = new G4Cerenkov("Cerenkov");
  G4OpAbsorption* theAbsorptionProcess = new G4OpAbsorption();
  G4OpRayleigh*   theRayleighScatteringProcess = new G4OpRayleigh();
  G4OpBoundaryProcess* theBoundaryProcess = new G4OpBoundaryProcess();

  theCerenkovProcess->DumpPhysicsTable();
  //theAbsorptionProcess->DumpPhysicsTable();
  //theRayleighScatteringProcess->DumpPhysicsTable();

  // add verbose 
  //theCerenkovProcess->SetVerboseLevel(1);
  //theAbsorptionProcess->SetVerboseLevel(1);
  //theRayleighScatteringProcess->SetVerboseLevel(1);
  //theBoundaryProcess->SetVerboseLevel(1);

  G4int maxNumPhotons = 300;

  theCerenkovProcess->SetTrackSecondariesFirst(true);
  theCerenkovProcess->SetMaxNumPhotonsPerStep(maxNumPhotons);

  //G4OpticalSurfaceModel themodel = unified;   
  // model from GEANT3
  G4OpticalSurfaceModel themodel = glisur;
  theBoundaryProcess->SetModel(themodel);

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* processManager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (theCerenkovProcess->IsApplicable(*particle)) {
      processManager->AddContinuousProcess(theCerenkovProcess);
    }
    if (particleName == "opticalphoton") {
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
      processManager->AddDiscreteProcess(theAbsorptionProcess);
      processManager->AddDiscreteProcess(theRayleighScatteringProcess);
      processManager->AddDiscreteProcess(theBoundaryProcess);
    }
  }

  // map to G3 controls
  TG4ProcessControlMap* controlMap = TG4ProcessControlMap::Instance();
  controlMap->Add(theCerenkovProcess, kCKOV); 
  controlMap->Add(theAbsorptionProcess, kLABS); 
  controlMap->Add(theRayleighScatteringProcess, kRAYL); 
  controlMap->Add(theBoundaryProcess, kLABS); 

  // map to AliMCProcess codes
  TG4ProcessMCMap* mcMap = TG4ProcessMCMap::Instance();
  mcMap->Add(theCerenkovProcess, kPCerenkov); 
  mcMap->Add(theAbsorptionProcess, kPLightAbsorption); 
  mcMap->Add(theRayleighScatteringProcess, kPRayleigh); 
  mcMap->Add(theBoundaryProcess, kPLightScattering); 

  if (verboseLevel>0)
    G4cout << "### " << namePhysics << " physics constructed." << G4endl;
}

