/* $Id$ */
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
//--------------------------------------------------------------------//
//                                                                    //
// AliCFContainer Class                                           //
// Class to accumulate data on an N-dimensional grids, at different    //
// selection stages. To be used as an input to get corrections for    //
// Reconstruction & Trigger efficiency                                // 
//                                                                    //
// -- Author : S.Arcelli                                              //
//--------------------------------------------------------------------//
//
//
#include <AliLog.h>
#include "AliCFGrid.h"
//#include "AliCFGridSparse.h"
#include "AliCFContainer.h"

//____________________________________________________________________
ClassImp(AliCFContainer)

//____________________________________________________________________
AliCFContainer::AliCFContainer() : 
  AliCFFrame(),
  fNStep(0),
  fGrid(0x0)
{
  //
  // default constructor
  //
}
//____________________________________________________________________
AliCFContainer::AliCFContainer(const Char_t* name, const Char_t* title) : 
  AliCFFrame(name,title),
  fNStep(0),
  fGrid(0x0)
{
  // default constructor
}

//____________________________________________________________________
AliCFContainer::AliCFContainer(const Char_t* name, const Char_t* title,const Int_t nSelSteps, const Int_t nVarIn, const Int_t * nBinIn, const Double_t *binLimitsIn) :  
  AliCFFrame(name,title,nVarIn,nBinIn,binLimitsIn),
  fNStep(0),
  fGrid(0x0)
{
  //
  // main constructor
  //

  // The selection steps
  fNStep=nSelSteps;

  // The grids 
  fGrid = new AliCFVGrid*[fNStep]; //the grids at the various selection steps
  char gname[30];
  for(Int_t istep=0;istep<fNStep;istep++){
    sprintf(gname,"%s%s%i",GetName(),"_SelStep", istep);
    fGrid[istep] = new AliCFGrid(gname,title,nVarIn,nBinIn,binLimitsIn); 
    //fGrid[istep] = new AliCFGridSparse(gname,title,nVarIn,nBinIn,binLimitsIn); 
    fGrid[istep]->SumW2(); 
  }
}
//____________________________________________________________________
AliCFContainer::AliCFContainer(const AliCFContainer& c) : 
  AliCFFrame(),
  fNStep(0),
  fGrid(0x0)
{
  //
  // copy constructor
  //
  ((AliCFContainer &)c).Copy(*this);
}
//____________________________________________________________________
AliCFContainer::~AliCFContainer()
{
  //
  // destructor
  //
  if(fGrid)delete [] fGrid;

}
//____________________________________________________________________
AliCFContainer &AliCFContainer::operator=(const AliCFContainer &c)
{
  //
  // assigment operator
  //
  if (this != &c)
    ((AliCFContainer &) c).Copy(*this);
  return *this;
} 
//____________________________________________________________________
void AliCFContainer::SetBinLimits(Int_t varindex, Double_t *array)
{
  //
  // setting the arrays containing the bin limits 
  //
  Int_t nbins=fNVarBins[varindex]+1;
  for(Int_t i=0;i<nbins;i++){
    fVarBinLimits[fOffset[varindex]+i] =array[i];
  } 
  for(Int_t istep=0;istep<fNStep;istep++){
    fGrid[istep]->SetBinLimits(varindex,array);
  }
} 
//____________________________________________________________________
void AliCFContainer::Copy(TObject& c) const
{
  //
  // copy function
  //
  AliCFContainer& target = (AliCFContainer &) c;
  target.fNStep=fNStep;
  target.fNVar=fNVar;
  target.fNDim=fNDim;
  target.fNVarBinLimits=fNVarBinLimits;
  if (fNVarBins)
    target.fNVarBins = fNVarBins;
  if (fVarBinLimits)
    target.fVarBinLimits = fVarBinLimits;
  if (fGrid)
    target.fGrid = fGrid;
    for(Int_t istep=0;istep<fNStep;istep++){
      for(Int_t iel=0;iel<fNDim;iel++){
	target.fGrid[istep]->SetElement(iel,fGrid[istep]->GetElement(iel));
      } 
    }  
}
//____________________________________________________________________
void AliCFContainer::Fill(Double_t *var, Int_t istep, Double_t weight)
{
  //
  // Fills the grid at selection step istep for a set of values of the 
  // input variables, with a given weight (by default w=1)
  //
  fGrid[istep]->Fill(var,weight);
}
//___________________________________________________________________
TH1D *AliCFContainer::ShowProjection(Int_t ivar, Int_t istep) const
{
  //
  // returns 1-D projection along variable ivar at selection step istep
  //
  return fGrid[istep]->Project(ivar);
}
//___________________________________________________________________
TH2D *AliCFContainer::ShowProjection(Int_t ivar1, Int_t ivar2, Int_t istep) const
{
  //
  // returns 2-D projection along variables ivar1,ivar2 at selection step istep
  //
  return fGrid[istep]->Project(ivar1,ivar2);
}
//___________________________________________________________________
TH3D *AliCFContainer::ShowProjection(Int_t ivar1, Int_t ivar2, Int_t ivar3, Int_t istep) const
{
  //
  // returns 3-D projection along variables ivar1,ivar2,ivar3 
  // at selection step istep
  //
  return fGrid[istep]->Project(ivar1,ivar2,ivar3);
}
//___________________________________________________________________
TH1D *AliCFContainer::ShowSlice(Int_t ivar, Double_t *varMin, Double_t* varMax, Int_t istep) const
{
  //
  // Make a slice along variable ivar at selection level istep in range [varMin,varMax]
  //
  return (TH1D*)fGrid[istep]->Slice(ivar,varMin,varMax);
}
//____________________________________________________________________
Long64_t AliCFContainer::Merge(TCollection* list)
{
  // Merge a list of AliCorrection objects with this (needed for
  // PROOF). 
  // Returns the number of merged objects (including this).

  if (!list)
    return 0;
  
  if (list->IsEmpty())
    return 1;

  TIterator* iter = list->MakeIterator();
  TObject* obj;
  
  Int_t count = 0;
  while ((obj = iter->Next())) {
    AliCFContainer* entry = dynamic_cast<AliCFContainer*> (obj);
    if (entry == 0) 
      continue;
    this->Add(entry);
    count++;
  }

  return count+1;
}

//____________________________________________________________________
void AliCFContainer::Add(AliCFContainer* aContainerToAdd, Double_t c)
{
  //
  //add the content of container aContainerToAdd to the current one
  //

  if(aContainerToAdd->GetNStep()!=fNStep)AliError("Different number of steps, cannot add the containers");
  if(aContainerToAdd->GetNVar()!=fNVar)AliError("Different number of variables, cannot add the containers");
  if(aContainerToAdd->GetNDim()!=fNDim)AliError("Different number of dimensions, cannot add the containers!");
  
  for(Int_t istep=0;istep<fNStep;istep++){
    fGrid[istep]->Add(aContainerToAdd->GetGrid(istep),c);
  }
}
//____________________________________________________________________
Float_t AliCFContainer::GetOverFlows( Int_t ivar, Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetOverFlows(ivar);
} 
//____________________________________________________________________
Float_t AliCFContainer::GetOverFlows( Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetOverFlows();
} 
//____________________________________________________________________
Float_t AliCFContainer::GetUnderFlows( Int_t ivar, Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetUnderFlows(ivar);
} 
//____________________________________________________________________
Float_t AliCFContainer::GetUnderFlows( Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetUnderFlows();
} 
//____________________________________________________________________
Float_t AliCFContainer::GetEntries( Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetEntries();
} 
//____________________________________________________________________
Int_t AliCFContainer::GetEmptyBins( Int_t istep) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetEmptyBins();
} 
//____________________________________________________________________
Int_t AliCFContainer::GetEmptyBins( Int_t istep, Double_t *varMin, Double_t* varMax) const {
  //
  // Get overflows in variable var at selection level istep
  //
  return fGrid[istep]->GetEmptyBins(varMin,varMax);
} 
//_____________________________________________________________________
Double_t AliCFContainer::GetIntegral( Int_t istep) const 
{
  //
  // Get Integral at selection level istep
  //
  return fGrid[istep]->GetIntegral();
}
//_____________________________________________________________________
Double_t AliCFContainer::GetIntegral( Int_t istep, Double_t *varMin, Double_t* varMax ) const 
{
  //
  // Get Integral at selection level istep
  //
  return fGrid[istep]->GetIntegral(varMin,varMax);
}
