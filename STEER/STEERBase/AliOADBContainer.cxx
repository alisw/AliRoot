/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
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

/* $Id$ */

//-------------------------------------------------------------------------
//     Offline Analysis Database Container and Service Class
//     Author: Andreas Morsch, CERN
//-------------------------------------------------------------------------

#include "AliOADBContainer.h"
#include "AliLog.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TFile.h"
#include "TList.h"
#include "TBrowser.h"
#include "TSystem.h"
#include "TGrid.h"
#include "TError.h"
#include "TROOT.h"
#include "TObjString.h"
#include "AliDataFile.h"
#include "AliLog.h"

AliOADBContainer::AliOADBContainer() :
  TNamed(),
  fArray(0),
  fDefaultList(0),
  fPassNames(0),
  fLowerLimits(),
  fUpperLimits(),
  fEntries(0)
{
  // Default constructor
}

AliOADBContainer::AliOADBContainer(const char* name) :
  TNamed(name, "OADBContainer"),
  fArray(new TObjArray(100)),
  fDefaultList(new TList()),
  fPassNames(new TObjArray(100)),
  fLowerLimits(),
  fUpperLimits(),
  fEntries(0)
{
  // Constructor
}

AliOADBContainer::~AliOADBContainer()
{
  // destructor
  delete fArray;
  delete fDefaultList;
  delete fPassNames;
}

AliOADBContainer::AliOADBContainer(const AliOADBContainer& cont) :
  TNamed(cont),
  fArray(cont.fArray),
  fDefaultList(cont.fDefaultList),
  fPassNames(cont.fPassNames),
  fLowerLimits(cont.fLowerLimits),
  fUpperLimits(cont.fUpperLimits),
  fEntries(cont.fEntries)
{
  // Copy constructor
  AliFatal("using flawed copy constructor");
}

AliOADBContainer::AliOADBContainer(AliOADBContainer&& cont) :
  TNamed(cont),
  fArray(cont.fArray),
  fDefaultList(cont.fDefaultList),
  fPassNames(cont.fPassNames),
  fLowerLimits(cont.fLowerLimits),
  fUpperLimits(cont.fUpperLimits),
  fEntries(cont.fEntries)
{
  // move constructor

  cont.fArray = nullptr;
  cont.fDefaultList = nullptr;
  cont.fPassNames = nullptr;
}

//______________________________________________________________________________
AliOADBContainer& AliOADBContainer::operator=(const AliOADBContainer& cont)
{
  //
  // Assignment operator
  // Copy objects related to run ranges
  if(this!=&cont) {
    TNamed::operator=(cont);
    fEntries = cont.fEntries;
    fLowerLimits.Set(fEntries);
    fUpperLimits.Set(fEntries);
    for (Int_t i = 0; i < fEntries; i++) {
	fLowerLimits[i] = cont.fLowerLimits[i];
	fUpperLimits[i] = cont.fUpperLimits[i];
	fArray->AddAt(cont.fArray->At(i), i);
	if (cont.fPassNames) if (cont.fPassNames->At(i)) fPassNames->AddAt(cont.fPassNames->At(i), i);
    }
    // Copy default objects
    TList* list = cont.GetDefaultList();
    TIter next(list);
    TObject* obj;
    while((obj = next()))
      fDefaultList->Add(obj);
  }

  return *this;
}

void AliOADBContainer::AppendObject(TObject* obj, Int_t lower, Int_t upper, TString passName)
{
  if (!fPassNames) { // create array of pass names for compatibility with old format
    fPassNames = new TObjArray(100);
    for (Int_t i=0;i<fArray->GetEntriesFast();i++) fPassNames->Add(new TObjString(""));
  }
  //
  // Append a new object to the list
  //
  // Check that there is no overlap with existing run ranges
  Int_t index = HasOverlap(lower, upper, passName);

  if (index != -1) {
    AliFatal(Form("Ambiguos validity range (%5d, %5.5d-%5.5d) !\n", index,lower,upper));
    return;
  }
  //
  // Adjust arrays
  fEntries++;
  fLowerLimits.Set(fEntries);
  fUpperLimits.Set(fEntries);
  // Add the object
  fLowerLimits[fEntries - 1] = lower;
  fUpperLimits[fEntries - 1] = upper;
  fArray->Add(obj);
  fPassNames->Add(new TObjString(passName.Data()));
}

void AliOADBContainer::RemoveObject(Int_t idx)
{
  if (!fPassNames) { // create array of pass names for compatibility with old format
    fPassNames = new TObjArray(100);
    for (Int_t i=0;i<fArray->GetEntriesFast();i++) fPassNames->Add(new TObjString(""));
  }

  //
  // Remove object from the list

  //
  // Check that index is inside range
  if (idx < 0 || idx >= fEntries)
    {
      AliError(Form("Index out of Range %5d >= %5d", idx, fEntries));
      return;
    }
  //
  // Remove the object
  TObject* obj = fArray->RemoveAt(idx);
  delete obj;

  TObject* pass = fPassNames->RemoveAt(idx);
  delete pass;
  //
  // Adjust the run ranges and shrink the array
  for (Int_t i = idx; i < (fEntries-1); i++) {
    fLowerLimits[i] = fLowerLimits[i + 1];
    fUpperLimits[i] = fUpperLimits[i + 1];
    fArray->AddAt(fArray->At(i+1), i);
    fPassNames->AddAt(fPassNames->At(i+1),i);
  }
  fArray->RemoveAt(fEntries - 1);
  fPassNames->RemoveAt(fEntries - 1);
  fEntries--;
}

void AliOADBContainer::UpdateObject(Int_t idx, TObject* obj, Int_t lower, Int_t upper, TString passName)
{
  //
  // Update an existing object, at a given position

  // Check that index is inside range
  if (idx < 0 || idx >= fEntries)
    {
      AliError(Form("Index out of Range %5d >= %5d", idx, fEntries));
      return;
    }
  //
  // Remove the old object and reset the range
  //  TObject* obj2 =
  fArray->RemoveAt(idx);
  // don't delete it: if you are updating it may be pointing to the same location of obj...
  //  delete obj2;
  fLowerLimits[idx] = -1;
  fUpperLimits[idx] = -1;
  // Check that there is no overlap with existing run ranges
  Int_t index = HasOverlap(lower, upper,passName);
  if (index != -1) {
    AliFatal(Form("Ambiguos validity range (%5d, %5.5d-%5.5d) !\n", index,lower,upper));
    return;
  }
  //
  // Add object at the same position
  //printf("idx %d obj %llx\n", idx, obj);
  fLowerLimits[idx] = lower;
  fUpperLimits[idx] = upper;
  TObjString* pass = (TObjString*) fPassNames->At(idx);
  pass->SetString(passName.Data());
  fArray->AddAt(obj, idx);
}

void  AliOADBContainer::AddDefaultObject(TObject* obj)
{
  // Add a default object
  fDefaultList->Add(obj);
}

void  AliOADBContainer::CleanDefaultList()
{
  // Clean default list
  fDefaultList->Delete();
}

Int_t AliOADBContainer::GetIndexForRun(Int_t run, TString passName) const
{
  //
  // Find the index for a given run

  Int_t found = 0;
  Int_t index = -1;
  for (Int_t i = 0; i < fEntries; i++)
    {
	if (fPassNames) if (fPassNames->At(i)) if (passName.CompareTo(fPassNames->At(i)->GetName())) continue;
	if (run >= fLowerLimits[i] && run <= fUpperLimits[i])
	{
	  found++;
	  index = i;
	}
    }

  if (found > 1) {
    AliError(Form("More than one (%5d) object found; return last (%5d) !\n", found, index));
  } else if (index == -1) {
    AliWarning(Form("No object (%s) found for run %5d !\n", GetName(), run));
  }

  return index;
}

TObject* AliOADBContainer::GetObject(Int_t run, const char* def, TString passName) const
{
  // Return object for given run or default if not found
  TObject* obj = 0;
  Int_t idx = GetIndexForRun(run, passName);
  if (idx == -1) idx = GetIndexForRun(run); // try default pass for this run range
  if (idx == -1) {
    // no object found, try default
    obj = fDefaultList->FindObject(def);
    if (!obj) {
      AliError(Form("Default Object (%s) not found !\n", GetName()));
      return (0);
    } else {
      return (obj);
    }
  } else {
    if (fArray!=0) {
      return (fArray->At(idx));
    } else {
      return (GetObjectFromFile(gFile, run, def, passName));
    }
  }
}

TObject* AliOADBContainer::GetObjectFromFile(TFile* file, Int_t run, const char* def, TString passName) const
{
  // Return object for given run or default if not found
  TObject* obj = 0;
  Int_t idx = GetIndexForRun(run, passName);
  if (idx == -1) idx = GetIndexForRun(run); // try default pass for this run range
  if (idx == -1) {
    // no object found, try default
    obj = fDefaultList->FindObject(def);
    if (!obj) {
      AliError(Form("Default Object (%s) not found !\n", GetName()));
      return (0);
    } else {
      return (obj);
    }
  } else {
    char keyst[20];
    sprintf(keyst, "multSel;%d", idx);
    obj = file->Get(keyst);
    return obj;
  }
}

TObject* AliOADBContainer::GetObjectByIndex(Int_t run) const
{
  // Return object for given index
  return (fArray->At(run));
}

TObject* AliOADBContainer::GetPassNameByIndex(Int_t idx) const
{
  // Return object for given index
  if (!fPassNames) return NULL;
  return (fPassNames->At(idx));
}


void AliOADBContainer::WriteToFile(const char* fname) const
{
  //
  // Write object to file
  TFile* f = new TFile(fname, "update");
  Write();
  f->Purge();
  f->Close();
}

Int_t AliOADBContainer::InitFromFile(const char* fname, const char* key)
{
    // Initialize object from file
    *this = AliDataFile::GetOADBContainer(fname, key);
    return 0;
}


void AliOADBContainer::List()
{
  //
  // List Objects
  printf("Entries %d\n", fEntries);

  for (Int_t i = 0; i < fEntries; i++) {
    printf("Lower %5d Upper %5d \n", fLowerLimits[i], fUpperLimits[i]);
    (fArray->At(i))->Dump();
  }
  TIter next(fDefaultList);
  TObject* obj;
  while((obj = next())) obj->Dump();

}

Int_t AliOADBContainer::HasOverlap(Int_t lower, Int_t upper, TString passName) const
{
  //
  // Checks for overlpapping validity regions
  for (Int_t i = 0; i < fEntries; i++) {
    if (fPassNames) if (fPassNames->At(i)) if (passName.CompareTo(fPassNames->At(i)->GetName())) continue;
    if ((lower >= fLowerLimits[i] && lower <= fUpperLimits[i]) ||
	(upper >= fLowerLimits[i] && upper <= fUpperLimits[i]))
      {
	return (i);
      }
  }
  return (-1);
}

void AliOADBContainer::Browse(TBrowser *b)
{
   // Browse this object.
   // If b=0, there is no Browse call TObject::Browse(0) instead.
   //         This means TObject::Inspect() will be invoked indirectly


  if (b) {
    for (Int_t i = 0; i < fEntries; i++) {
      TString pass = !fPassNames ? " - " : (fPassNames->At(i) ? Form(" - %s",fPassNames->At(i)->GetName()) : " - ");
      b->Add(fArray->At(i),Form("%9.9d - %9.9d%s", fLowerLimits[i], fUpperLimits[i],pass.CompareTo(" - ")? pass.Data() :""));
    }
    TIter next(fDefaultList);
    TObject* obj;
    while((obj = next())) b->Add(obj);

  }
   else
      TObject::Browse(b);
}
