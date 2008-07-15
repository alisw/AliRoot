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
// Category: management

//-----------------------------------------------------------------------------
// Class AliMpManuStore
// --------------------
// The container class for manu serial numbersd
// Authors: Ivana Hrivnacova, IPN Orsay
//          Christian Finck, SUBATECH Nantes
//-----------------------------------------------------------------------------

#include "AliMpManuStore.h"

#include "AliMpDEStore.h"
#include "AliMpDEManager.h"
#include "AliMpDetElement.h"
#include "AliMpConstants.h"
#include "AliMpDataStreams.h"
#include "AliMpFiles.h"
#include "AliMpHelper.h"
#include "AliMpIntPair.h"
#include "AliMpConstants.h"

#include "AliLog.h"

#include <Riostream.h>
#include <TClass.h>
#include <TSystem.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TMap.h>

#include <fstream>

/// \cond CLASSIMP
ClassImp(AliMpManuStore)
/// \endcond

AliMpManuStore* AliMpManuStore::fgInstance = 0;
Bool_t          AliMpManuStore::fgWarnIfDoublon = kFALSE;

//
// static methods
//

//______________________________________________________________________________
AliMpManuStore* AliMpManuStore::Instance(Bool_t warn) 
{
    /// Create the DDL store if it does not yet exist
    /// and return its instance

    if ( ! fgInstance && warn  ) {
        AliWarningClass("Manu Store has not been loaded");
    }

    return fgInstance;
}

//______________________________________________________________________________
AliMpManuStore* AliMpManuStore::ReadData(const AliMpDataStreams& dataStreams, 
                                         Bool_t warn) 
{
    /// Load the DDL store from ASCII data files
    /// and return its instance

    if ( fgInstance ) {
        if ( warn )
            AliWarningClass("DDL Store has been already loaded");
        return fgInstance;
    }

    if ( dataStreams.GetReadFromFiles() )
      AliInfoClass("Reading Manu Store from ASCII files.");

    fgInstance = new AliMpManuStore(dataStreams);
    return fgInstance;
}

//
// ctors, dtor
//


//______________________________________________________________________________
AliMpManuStore::AliMpManuStore(const AliMpDataStreams& dataStreams)
: TObject(),
  fDataStreams(dataStreams),
  fManuToSerialNbs(),
  fSerialNbToManus(),
  fNofManusInDE(),
  fNofManus(0)
{  
/// Standard constructor

  AliDebug(1,"");

  // Check if DE store is loaded
  if ( ! AliMpDEStore::Instance(false) ) {
     AliErrorStream()
       << "Mapping segmentation has not be loaded. Cannont load Manu store"
       << endl;
     return;
  }      

  ReadManuSerial();
}

//______________________________________________________________________________
AliMpManuStore::AliMpManuStore(TRootIOCtor* ioCtor)
: TObject(),
  fDataStreams(ioCtor),
  fManuToSerialNbs(),
  fSerialNbToManus(),
  fNofManusInDE(),
  fNofManus(0)
{  
/// Constructor for IO

  AliDebug(1,"");
}


//______________________________________________________________________________
AliMpManuStore::~AliMpManuStore()
{
/// Destructor

  AliDebug(1,"");
}

//
// private methods
//

//______________________________________________________________________________
Bool_t AliMpManuStore::ReadData(const AliMpDetElement* de, Int_t& nofManus)
{
/// Read manu serial numbers for the given detection element

  Int_t deId = de->GetId();
  TString deName = de->GetDEName();
  AliMp::StationType stationType 
    =  AliMpDEManager::GetStationType(de->GetId());

  // Nothing to be done for trigger
  if ( stationType == AliMp::kStationTrigger ) {
    nofManus = 0;
    return kTRUE;
  }  

  static Int_t manuMask = AliMpConstants::ManuMask(AliMp::kNonBendingPlane);

  istream& in 
    = fDataStreams.
        CreateDataStream(AliMpFiles::ManuToSerialPath(deName, stationType));

  char line[80];

  nofManus = 0;
  while ( in.getline(line,80) ) {

    if ( line[0] == '#' ) continue;

    TString tmp(AliMpHelper::Normalize(line));

    TObjArray* stringList = tmp.Tokenize(TString(" "));

    Int_t manuId     = atoi( ((TObjString*)stringList->At(0))->GetName());
    Int_t manuSerial = atoi( ((TObjString*)stringList->At(2))->GetName());
      
    TString sPlane = ((TObjString*)stringList->At(1))->GetString();

    // filling manuId <> manuSerial
    if (!sPlane.CompareTo(PlaneTypeName(AliMp::kBendingPlane)))
	AddManu(deId, manuId, manuSerial);
    else 
	AddManu(deId, manuId + manuMask, manuSerial);
        
    ++nofManus;    

    delete stringList;
  }
   
  return kTRUE;
}

//______________________________________________________________________________
Bool_t  AliMpManuStore::ReadManuSerial()
{
/// Read data files for all detection elements.
/// Return true if reading was successful.

  Bool_t isOk = kTRUE;

  // Loop over DE
  AliMpDEIterator it;
  for ( it.First(); ! it.IsDone(); it.Next() ) {

    AliMpDetElement* detElement = it.CurrentDE();

    Int_t nofManus;
    Bool_t result = ReadData(detElement, nofManus);  
    fNofManusInDE.Add(detElement->GetId(), nofManus);
    fNofManus += nofManus;
    
    AliDebugStream(2) 
      << "Adding " << nofManus << " manus for de " 
      << detElement->GetId() << endl;
    
    isOk = isOk && result;
  }
  
  return isOk;  
}   

//______________________________________________________________________________
void  AliMpManuStore::ReplaceManu(Int_t detElemId, Int_t manuId, Int_t serialNb) 
{
/// Replace manu in the map.
/// As TExMap has no replcae function, we have to rebuild map once again.
/// Not yet in use, declared private.

  Long_t index = AliMpExMap::GetIndex(AliMpIntPair(detElemId, manuId));

  TExMap newManuToSerialNbs;
  // Loop over map
  TExMapIter it(&fManuToSerialNbs);
  Long_t key;
  Long_t value;
  while ( ( it.Next(key, value) ) ) {

    if ( key != index ) 
      newManuToSerialNbs.Add(key, value);
    else
      newManuToSerialNbs.Add(index, Long_t(serialNb));
  }
      
  TExMap newSerialNbToManus;
  // Loop over map
  TExMapIter it2(&fSerialNbToManus);
  while ( ( it2.Next(key, value) ) ) {

    if ( value != index ) 
      newSerialNbToManus.Add(key, value);
    else
      newSerialNbToManus.Add(Long_t(serialNb), index);
  }

  // And now replace the maps
  fManuToSerialNbs = newManuToSerialNbs;
  fSerialNbToManus = newManuToSerialNbs;
}     


//______________________________________________________________________________
Bool_t  AliMpManuStore::WriteData(const TString& outDir)
{
/// Write data files for all detection elements.
/// Return true if reading was successful.
/// Not yet in use, declared private.

  TString curDir = gSystem->pwd();

  // Create top directory
  //
  if ( gSystem->OpenDirectory(outDir.Data()) ) {
    AliErrorStream() 
      << "Directory " << outDir.Data() << " already exists" << endl;
    return kFALSE;
  }
  else {
    AliDebugStream(2) << "Making directory " <<  outDir.Data() << endl;
    gSystem->mkdir(outDir.Data());
  }  

  // Loop over DE
  AliMpDEIterator it;
  for ( it.First(); ! it.IsDone(); it.Next() ) {
  
    AliMpDetElement* detElement = it.CurrentDE();
    Int_t detElemId = detElement->GetId();
    TString deName = detElement->GetDEName();
    AliMp::StationType stationType 
      =  AliMpDEManager::GetStationType(detElemId);
      
    if ( stationType == AliMp::kStationTrigger ) continue;  

    // Create directory if it does not yet exist
    //
    TString dirPath = outDir + AliMpFiles::StationDataDir(stationType);
    if ( ! gSystem->OpenDirectory(dirPath.Data()) ) {
      AliDebugStream(2) << "Making directory " <<  dirPath.Data() << endl;
      gSystem->mkdir(dirPath.Data());
    }  

    // Compose output file path 
    //
    string dataPath = AliMpFiles::ManuToSerialPath(deName, stationType).Data();
    string top = AliMpFiles::GetTop().Data();
    if ( dataPath.find(top) != string::npos ) dataPath.erase(0, top.size()+1);
    dataPath.erase(0,dataPath.find('/')+1);
    TString filePath = outDir + "/" + dataPath; 

    // Open file
    //
    ofstream out(filePath.Data());
    if ( ! out.good() ) {
      AliErrorStream() 
        << "Cannot open output file  " << filePath.Data() << endl;
      return kFALSE;  
    }
    
    // Loop over map
    TExMapIter it(&fManuToSerialNbs);
    Long_t key;
    Long_t value;
    while ( ( it.Next(key, value) ) ) {
      AliMpIntPair pair = AliMpExMap::GetPair(key);
      
      if ( pair.GetFirst() != detElemId ) continue;
      
      AliDebugStream(3) 
        << "Go to write " << key << " " << pair << " " << value << endl;

      Int_t manuId = pair.GetSecond();
      static Int_t manuMask = AliMpConstants::ManuMask(AliMp::kNonBendingPlane);

      TString planeName = PlaneTypeName(AliMp::kBendingPlane);
      if ( pair.GetSecond()> manuMask ) {
        planeName = PlaneTypeName(AliMp::kNonBendingPlane);
        manuId -= manuMask;
      } 
      out << manuId << " " << planeName.Data() <<  " " << value << endl;
      
      AliDebugStream(3) 
        << manuId << " " << planeName.Data() <<  " " << value << endl;
    }  
    out.close();
  }   
  gSystem->cd(curDir); 
  return kTRUE;  
}   

//
// public methods
//


//______________________________________________________________________________
Int_t  AliMpManuStore::NofManus() const
{ 
/// Return total number of manus in the store

  return fNofManus;
}  
  

//______________________________________________________________________________
Int_t  AliMpManuStore::NofManus(Int_t detElemId) const 
{ 
/// Return number of manus in given detection element

   if ( ! AliMpDEManager::IsValidDetElemId(detElemId, kTRUE) ) return 0;   

   return fNofManusInDE.GetValue(detElemId); 
}

//______________________________________________________________________________
Bool_t  AliMpManuStore::AddManu(Int_t detElemId, Int_t manuId, Int_t serialNb) 
{
/// Add manu to the map

  Long_t index = AliMpExMap::GetIndex(AliMpIntPair(detElemId, manuId));
  
  AliDebugStream(2)
    << "Adding (" << detElemId << "," <<  manuId 
    << ") as index=" << index << " and serialNb=" << serialNb << endl;
  
  fManuToSerialNbs.Add(index, Long_t(serialNb));
  
  Long_t value = fSerialNbToManus.GetValue(Long_t(serialNb));
  if ( value ) {
    if ( fgWarnIfDoublon ) {
      AliWarningStream() 
        << "Serial number " << serialNb 
        << " already present for (detElemId, manuId) = " << AliMpExMap::GetPair(value)
        << ", it will nod be added for (" 
        << detElemId << "," << manuId << ")" << endl;
     }
     return kFALSE;    
  }
  else {
    fSerialNbToManus.Add(Long_t(serialNb), index);
    return kTRUE;
  }  
}     


//______________________________________________________________________________
Int_t AliMpManuStore::GetManuSerial(Int_t detElemId, Int_t manuId) const
{
/// Return manu serial number for given detElemId and manuId

  Long_t index = AliMpExMap::GetIndex(AliMpIntPair(detElemId, manuId));
  // cout << index << " " << fManuToSerialNbs.GetValue(index) << endl;
  
  return fManuToSerialNbs.GetValue(index);
}  

//______________________________________________________________________________
AliMpIntPair  AliMpManuStore::GetDetElemIdManu(Int_t manuSerial) const
{
/// Return detElemId and manuId for given manu serial number 

  Long_t value = fSerialNbToManus.GetValue(Long_t(manuSerial));
  // cout << manuSerial << " " << value << endl;
  
  return AliMpExMap::GetPair(value);
}  

