// $Id$
// Category: physics
//
// See the class description in the header file.

#include "TG4ModularPhysicsList.h"
#include "TG4G3PhysicsManager.h"
#include "TG4G3ControlVector.h"
#include "TG4ExtDecayer.h"
#include "AliDecayer.h"
#include "AliMC.h"

#include <G4ParticleDefinition.hh>
#include <G4ProcessManager.hh>
#include <G4BosonConstructor.hh>
#include <G4LeptonConstructor.hh>
#include <G4MesonConstructor.hh>
#include <G4BaryonConstructor.hh>
#include <G4IonConstructor.hh>
#include <G4ShortLivedConstructor.hh>
#include <G4ProcessTable.hh>
#include <G4Decay.hh>


//_____________________________________________________________________________
TG4ModularPhysicsList::TG4ModularPhysicsList()
  : G4VModularPhysicsList(),
    fExtDecayer(0)
{
//
  defaultCutValue = 1.0*mm;

  SetVerboseLevel(1);
}

//_____________________________________________________________________________
TG4ModularPhysicsList::TG4ModularPhysicsList(const TG4ModularPhysicsList& right)
{
//
  TG4Globals::Exception("TG4ModularPhysicsList is protected from copying.");
}

//_____________________________________________________________________________
TG4ModularPhysicsList::~TG4ModularPhysicsList() {
//
  //delete fExtDecayer;
       // fExtDecayer is deleted in G4Decay destructor
}

// operators

//_____________________________________________________________________________
TG4ModularPhysicsList& 
TG4ModularPhysicsList::operator=(const TG4ModularPhysicsList &right)
{
  // check assignement to self
  if (this == &right) return *this;
  
  TG4Globals::Exception("TG4ModularPhysicsList is protected from assigning.");

  return *this;
}

// protected methods

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructParticle()
{
// In this method, static member functions should be called
// for all particles which you want to use.
// This ensures that objects of these particle types will be
// created in the program. 
// ---

  // lock physics manager
  TG4G3PhysicsManager* g3PhysicsManager = TG4G3PhysicsManager::Instance();
  g3PhysicsManager->Lock();  
 
  // create all particles
  ConstructAllBosons();
  ConstructAllLeptons();
  ConstructAllMesons();
  ConstructAllBaryons();
  ConstructAllIons();
  ConstructAllShortLiveds();
  
  // create particles for registered physics
  G4VModularPhysicsList::ConstructParticle();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructProcess()
{
// Constructs all processes.
// ---

  // create processes for registered physics
  G4VModularPhysicsList::ConstructProcess();

  ConstructGeneral();

  // verbose
  if (verboseLevel>1) PrintAllProcesses();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllBosons()
{
// Construct all bosons
// ---

  G4BosonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllLeptons()
{
// Construct all leptons
// ---

  G4LeptonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllMesons()
{
// Construct all mesons
// ---

  G4MesonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllBaryons()
{
// Construct all barions
// ---

  G4BaryonConstructor pConstructor;
  pConstructor.ConstructParticle();
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllIons()
{
// Construct light ions
// ---

  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();  
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructAllShortLiveds()
{
// Construct  resonaces and quarks
// ---

  G4ShortLivedConstructor pConstructor;
  pConstructor.ConstructParticle();  
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::ConstructGeneral()
{
// Constructs general processes.
// ---

  // Add Decay Process
  G4Decay* theDecayProcess = new G4Decay();

  // Set external decayer
  AliDecayer* aliDecayer = gMC->Decayer(); 
  if (aliDecayer) {
    TG4ExtDecayer* tg4Decayer = new TG4ExtDecayer(aliDecayer);
       // the tg4Decayer is deleted in G4Decay destructor
    tg4Decayer->SetVerboseLevel(1);   
    theDecayProcess->SetExtDecayer(tg4Decayer);
    
    if (verboseLevel>0) G4cout << "### External decayer is set" << G4endl;
  } 

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager ->AddProcess(theDecayProcess);
      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}

// public methods

//_____________________________________________________________________________
void TG4ModularPhysicsList::SetCuts()
{
// Sets the default cut value for all particle types
// other then e-/e+. 
// The cut value for e-/e+ is high in oredr to supress
// tracking of delta electrons.
// ---

  // SetCutsWithDefault();   
         // "G4VUserPhysicsList::SetCutsWithDefault" method sets 
         // the default cut value for all particle types.

  // default cut value
  G4double cut  = defaultCutValue;
  //G4double ecut = 10.*m; 
  G4double ecut = cut; 

#ifdef G4VERBOSE    
  if (verboseLevel >1){
    G4cout << "G4VUserPhysicsList::SetCutsWithDefault:";
    G4cout << "CutLength : " << cut/mm << " (mm)" << G4endl;
  }  
#endif

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma 
  SetCutValue(cut, "gamma");
  SetCutValue(ecut, "e-");
  SetCutValue(ecut, "e+");
 
  // set cut values for proton and anti_proton before all other hadrons
  // because some processes for hadrons need cut values for proton/anti_proton 
  SetCutValue(cut, "proton");
  SetCutValue(cut, "anti_proton");
  
  SetCutValueForOthers(cut);

  if (verboseLevel>1) {
    DumpCutValuesTable();
  }
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::SetProcessActivation()
{
// (In)Activates built processes according
// to the setup in TG4G3PhysicsManager::fControlVector.
// ---

  TG4G3PhysicsManager* g3PhysicsManager = TG4G3PhysicsManager::Instance();
  TG4G3ControlVector* controlVector = g3PhysicsManager->GetControlVector();

  // uncomment following lines to print
  // the controlVector values
  //for (G4int i=0; i<kNoG3Controls; i++)
  //{ cout << i << " control: " << (*controlVector)[i] << G4endl; }

  if (controlVector) {
    theParticleIterator->reset();
    while ((*theParticleIterator)())
    {
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* processManager = particle->GetProcessManager(); 
      G4ProcessVector* processVector = processManager->GetProcessList();
    
      // set processes controls
      for (G4int i=0; i<processManager->GetProcessListLength(); i++) {
        G4int control = controlVector->GetControl((*processVector)[i]);
        if ((control == kInActivate) && 
            (processManager->GetProcessActivation(i))) {
          if (verboseLevel>1) {
             G4cout << "Set process inactivation for " 
                    << (*processVector)[i]->GetProcessName() << G4endl;
          }
          processManager->SetProcessActivation(i,false);
        }  
        else if (((control == kActivate) || (control == kActivate2)) &&
                 (!processManager->GetProcessActivation(i))) {
          if (verboseLevel>1) {
             G4cout << "Set process activation for " 
                    << (*processVector)[i]->GetProcessName() << G4endl;
          }
          processManager->SetProcessActivation(i,true);
        } 
      }
    }
  }
  else {
    G4String text = "TG4ModularPhysicsList::SetProcessActivation: \n";
    text = text + "    Vector of processes controls is not set.";
    TG4Globals::Warning(text);
  }    
}

//_____________________________________________________________________________
void TG4ModularPhysicsList::PrintAllProcesses() const
{
// Prints all processes.
// ---

  G4cout << "TG4ModularPhysicsList processes: " << G4endl;
  G4cout << "========================= " << G4endl;
 
  G4ProcessTable* processTable = G4ProcessTable::GetProcessTable();
  G4ProcessTable::G4ProcNameVector* processNameList 
    = processTable->GetNameList();

  for (G4int i=0; i <processNameList->size(); i++){
    G4cout << "   " << (*processNameList)[i] << G4endl;
  }  
}

