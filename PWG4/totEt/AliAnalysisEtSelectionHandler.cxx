#include "AliAnalysisEtSelectionHandler.h"
#include "AliAnalysisEtSelectionContainer.h"
#include "AliPhysicsSelection.h"
#include "TFile.h"
#include <iostream>

ClassImp(AliAnalysisEtSelectionHandler);


AliAnalysisEtSelectionHandler::AliAnalysisEtSelectionHandler() :
fSelections(0)
{

}

AliAnalysisEtSelectionHandler::AliAnalysisEtSelectionHandler(const char* name) : 
fSelections(0)
{
  // Constructor
  TFile *mapFile = TFile::Open(name);
  
  //mapFile->Open(name);
  std::cout  << name << " " << mapFile << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  mapFile->ls();
  fSelections = dynamic_cast<AliAnalysisEtSelectionContainer*>(mapFile->Get("physicsSelections"));
  
}

AliAnalysisEtSelectionHandler::~AliAnalysisEtSelectionHandler()
{
  // Destructor
}

AliAnalysisEtSelectionHandler::AliAnalysisEtSelectionHandler(const AliAnalysisEtSelectionHandler& other): TObject(other)
,fSelections(other.GetSelectionContainer())
{
  // Copy constructor
}

AliAnalysisEtSelectionHandler& AliAnalysisEtSelectionHandler::operator=(const AliAnalysisEtSelectionHandler& /*other*/)
{
  // Assignment operator, not properly implemented
  return *this;
}
