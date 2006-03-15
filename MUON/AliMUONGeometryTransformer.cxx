/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *      SigmaEffect_thetadegrees                                                                  *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpeateose. It is      *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

// $Id$
//
// ----------------------------
// Class AliMUONGeometryTransformer
// ----------------------------
// Top container class for geometry transformations
// Author: Ivana Hrivnacova, IPN Orsay

#include "AliMUONGeometryTransformer.h"
#include "AliMUONGeometryModuleTransformer.h"
#include "AliMUONGeometryDetElement.h"
#include "AliMUONGeometryStore.h"
#include "AliMUONGeometryBuilder.h"

#include "AliLog.h"
#include "AliAlignObjMatrix.h"
#include "AliAlignObj.h"

#include <Riostream.h>
#include <TSystem.h>
#include <TClonesArray.h>
#include <TGeoManager.h>
#include <TGeoPhysicalNode.h>
#include <TFile.h>

#include <sstream>

ClassImp(AliMUONGeometryTransformer)
 
//______________________________________________________________________________
AliMUONGeometryTransformer::AliMUONGeometryTransformer(Bool_t isOwner)
  : TObject(),
    fModuleTransformers(0),
    fMisAlignArray(0)
{
/// Standard constructor

  // Create array for geometry modules
  fModuleTransformers = new TObjArray();
  fModuleTransformers->SetOwner(isOwner);
}

//______________________________________________________________________________
AliMUONGeometryTransformer::AliMUONGeometryTransformer() 
  : TObject(),
    fModuleTransformers(0),
    fMisAlignArray(0)
{
/// Default constructor
} 

//______________________________________________________________________________
AliMUONGeometryTransformer::AliMUONGeometryTransformer(
                                   const AliMUONGeometryTransformer& right) 
  : TObject(right) 
{  
/// Copy constructor (not implemented)

  AliFatal("Copy constructor not provided.");
}

//______________________________________________________________________________
AliMUONGeometryTransformer::~AliMUONGeometryTransformer()
{
/// Destructor

  delete fModuleTransformers;
  delete fMisAlignArray;
}

//______________________________________________________________________________
AliMUONGeometryTransformer& 
AliMUONGeometryTransformer::operator=(const AliMUONGeometryTransformer& right)
{
/// Assignement operator (not implemented)

  // check assignement to self
  if (this == &right) return *this;

  AliFatal("Assignement operator not provided.");
    
  return *this;  
}    

//
// private methods
//

//_____________________________________________________________________________
AliMUONGeometryModuleTransformer* 
AliMUONGeometryTransformer::GetModuleTransformerNonConst(
                                          Int_t index, Bool_t warn) const
{
/// Return the geometry module specified by index

  if (index < 0 || index >= fModuleTransformers->GetEntriesFast()) {
    if (warn) {
      AliWarningStream() 
        << "Index: " << index << " outside limits" << std::endl;
    }			 
    return 0;  
  }  

  return (AliMUONGeometryModuleTransformer*) fModuleTransformers->At(index);
}    

//______________________________________________________________________________
TGeoHMatrix AliMUONGeometryTransformer::GetTransform(
                  Double_t x, Double_t y, Double_t z,
		  Double_t a1, Double_t a2, Double_t a3, 
 		  Double_t a4, Double_t a5, Double_t a6) const
{		  
// Builds the transformation from the given parameters
// ---

  // Compose transform
  return TGeoCombiTrans(TGeoTranslation(x, y, z), 
                        TGeoRotation("rot", a1, a2, a3, a4, a5, a6));
}


//______________________________________________________________________________
void AliMUONGeometryTransformer::FillModuleVolPath(Int_t moduleId,
                                           const TString& volPath) 
{
// Create module with the given moduleId and volPath
// ---

  // Get/Create geometry module transformer
  AliMUONGeometryModuleTransformer* moduleTransformer
    = GetModuleTransformerNonConst(moduleId, false);

  if ( !moduleTransformer ) {
    moduleTransformer = new AliMUONGeometryModuleTransformer(moduleId);
    AddModuleTransformer(moduleTransformer);
  }  
  moduleTransformer->SetVolumePath(volPath);
}		   
  
//______________________________________________________________________________
void AliMUONGeometryTransformer::FillDetElemVolPath(Int_t detElemId, 
                                           const TString& volPath) 
{
// Create detection element with the given detElemId and volPath

  // Module Id
  Int_t moduleId = AliMUONGeometryStore::GetModuleId(detElemId);

  // Get detection element store
  AliMUONGeometryStore* detElements = 
    GetModuleTransformer(moduleId)->GetDetElementStore();     

  // Add detection element
  AliMUONGeometryDetElement* detElement
    = new AliMUONGeometryDetElement(detElemId, volPath);
  detElements->Add(detElemId, detElement);
}		   
  

//______________________________________________________________________________
void AliMUONGeometryTransformer::FillModuleTransform(Int_t moduleId,
                  Double_t x, Double_t y, Double_t z,
		  Double_t a1, Double_t a2, Double_t a3,
 		  Double_t a4, Double_t a5, Double_t a6) 
{
// Fill the transformation of the module.
// ---

  // Get/Create geometry module transformer
  moduleId--;
      // Modules numbers in the file are starting from 1

  AliMUONGeometryModuleTransformer* moduleTransformer
    = GetModuleTransformerNonConst(moduleId, false);

  if ( !moduleTransformer) {
    AliErrorStream() 
      << "Module " << moduleId << " has not volume path defined." << endl;
  }  
      
  // Build the transformation from the parameters
  TGeoHMatrix transform 
    = GetTransform(x, y, z, a1, a2, a3, a4, a5, a6);
      
  moduleTransformer->SetTransformation(transform);
}		   
  
//______________________________________________________________________________
void AliMUONGeometryTransformer::FillDetElemTransform(
                  Int_t detElemId, 
                  Double_t x, Double_t y, Double_t z,
		  Double_t a1, Double_t a2, Double_t a3,
 		  Double_t a4, Double_t a5, Double_t a6) 
{
// Fill the transformation of the detection element.
// ---

  // Module Id
  Int_t moduleId = AliMUONGeometryStore::GetModuleId(detElemId);

  // Get module transformer
  const AliMUONGeometryModuleTransformer* kModuleTransformer
    = GetModuleTransformer(moduleId);

  if ( ! kModuleTransformer ) {
    AliFatal(Form("Module transformer not defined, detElemId: %d", detElemId));
    return;  
  }  

  // Get detection element
  AliMUONGeometryDetElement* detElement 
    = kModuleTransformer->GetDetElement(detElemId);     

  if ( ! detElement ) {
    AliFatal(Form("Det element %d has not volume path defined", detElemId));
    return;  
  }  
      
  // Build the transformation from the parameters
  TGeoHMatrix localTransform 
    = GetTransform(x, y, z, a1, a2, a3, a4, a5, a6);
  detElement->SetLocalTransformation(localTransform); 
   
  // Compute global transformation
  TGeoHMatrix globalTransform 
    = AliMUONGeometryBuilder::Multiply( 
                                  *kModuleTransformer->GetTransformation(),
				  localTransform );
  detElement->SetGlobalTransformation(globalTransform);
}		   

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadVolPaths(ifstream& in)
{
// Reads modules and detection element volume paths from stream
// ---

  Int_t id;
  TString key, volumePath;
  in >> key;
  
  while ( !in.eof() ) {

    in >> id >> volumePath;

    // cout << "id="     << id << "  "
    // 	 << "volPath= " << volumePath
    //	 << endl;   

    if ( key == TString("CH") ) 
      FillModuleVolPath(id, volumePath);
  
    else if ( key == TString("DE") )
      FillDetElemVolPath(id, volumePath);
  
    else {
      AliFatal(Form("%s key not recognized",  key.Data()));
      return false;
    }
    in >> key;
  }     

  return true;
}

//______________________________________________________________________________
TString  AliMUONGeometryTransformer::ReadModuleTransforms(ifstream& in)
{
// Reads and fills modules transformations from a file
// Returns true, if reading finished correctly.
// ---

  TString key("CH");
  while ( key == TString("CH") ) {
    Int_t id;
    Double_t  x, y, z;
    Double_t  a1, a2, a3, a4, a5, a6;
    TString dummy;
  
    in >> id;
    in >> dummy;
    in >> x;
    in >> y;
    in >> z;
    in >> dummy;
    in >> a1; 
    in >> a2; 
    in >> a3; 
    in >> a4; 
    in >> a5; 
    in >> a6; 

    //cout << "moduleId="     << id << "  "
    // 	 << "position= " << x << ", " << y << ", " << z << "  "
    //	 << "rotation= " << a1 << ", " << a2 << ", " << a3  << ", "
    //	                 << a4 << ", " << a5 << ", " << a6 
    //	 << endl;   
	 
    // Fill data
    FillModuleTransform(id, x, y, z, a1, a2, a3, a4, a5, a6);
    
    // Go to next line
    in >> key;
  }
  
  return key;   	 
}

//______________________________________________________________________________
TString  AliMUONGeometryTransformer::ReadDetElemTransforms(ifstream& in)
{
// Reads detection elements transformations from a file
// Returns true, if reading finished correctly.
// ---

  TString key("DE");
  while ( key == TString("DE") ) {

    // Input data
    Int_t detElemId;
    Double_t  x, y, z;
    Double_t  a1, a2, a3, a4, a5, a6;
    TString dummy;
  
    in >> detElemId;
    in >> dummy;
    in >> x;
    in >> y;
    in >> z;
    in >> dummy;
    in >> a1; 
    in >> a2; 
    in >> a3; 
    in >> a4; 
    in >> a5; 
    in >> a6; 

    //cout << "detElemId=" << detElemId << "  "
    //     << "position= " << x << ", " << y << ", " << z << "  "
    //     << "rotation= " << a1 << ", " << a2 << ", " << a3  << ", "
    //	                   << a4 << ", " << a5 << ", " << a6 
    //     << endl;   
	 
    // Fill data
    FillDetElemTransform(detElemId, x, y, z, a1, a2, a3, a4, a5, a6); 	 
    
    // Go to next line
    in >> key;
  } 
  
  return key;
}

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::LoadTransforms(TGeoManager* tgeoManager)
{
/// Loads transformations for defined modules and detection elements
/// from the root file

  for (Int_t i=0; i<fModuleTransformers->GetEntriesFast(); i++) {
    AliMUONGeometryModuleTransformer* moduleTransformer 
      = (AliMUONGeometryModuleTransformer*)fModuleTransformers->At(i);

    // Module path
    TString path = moduleTransformer->GetVolumePath();
    
    // Make physical node
    TGeoPhysicalNode* moduleNode = tgeoManager->MakePhysicalNode(path);
    if ( ! moduleNode ) {
      AliErrorStream() 
        << "Module id: " << moduleTransformer->GetModuleId()
	<< " volume path: " << path << " not found in geometry." << endl;
	return false;
    }	 
    
    // Set matrix from physical node
    TGeoHMatrix matrix = *moduleNode->GetMatrix();
    moduleTransformer->SetTransformation(matrix);
    
    // Loop over detection elements
    AliMUONGeometryStore* detElements 
      = moduleTransformer->GetDetElementStore();    
   
    for (Int_t j=0; j<detElements->GetNofEntries(); j++) {
      AliMUONGeometryDetElement* detElement
        = (AliMUONGeometryDetElement*)detElements->GetEntry(j);

      // Det element path
      TString dePath = detElement->GetVolumePath();

      // Make physical node
      TGeoPhysicalNode* deNode = tgeoManager->MakePhysicalNode(dePath);
      if ( ! deNode ) {
        AliErrorStream() 
          << "Det element id: " << detElement->GetId()
	  << " volume path: " << path << " not found in geometry." << endl;
	  return false;
      }	
	 
      // Set global matrix from physical node
      TGeoHMatrix globalMatrix = *deNode->GetMatrix();
      detElement->SetGlobalTransformation(globalMatrix);

      // Set local matrix
      TGeoHMatrix localMatrix = 
        AliMUONGeometryBuilder::Multiply(
	   matrix.Inverse(), globalMatrix );
      detElement->SetLocalTransformation(localMatrix);
    }  
  } 
  return true;    
}  

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadVolPaths(const TString& fileName)
{
// Reads detection element volume paths from a file
// Returns true, if reading finished correctly.
// ---

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Open input file
  ifstream in(filePath, ios::in);
  if (!in) {
    cerr << filePath << endl;	
    AliFatal("File not found.");
    return false;
  }

  ReadVolPaths(in);
  return true;
}

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadTransformations(const TString& fileName)
{
// Reads transformations from a file
// Returns true, if reading finished correctly.
// ---

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Open input file
  ifstream in(filePath, ios::in);
  if (!in) {
    cerr << filePath << endl;	
    AliFatal("File not found.");
    return false;
  }

  TString key;
  in >> key;
  while ( !in.eof() ) {
    if (key == TString("CH")) 
      key = ReadModuleTransforms(in);
    else if (key == TString("DE"))
      key = ReadDetElemTransforms(in);
    else {
      AliFatal(Form("%s key not recognized",  key.Data()));
      return false;
    }
  }     

  return true;
}

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadTransformations2(const TString& fileName)
{
// Reads transformations from root geometry file
// Returns true, if reading finished correctly.
// ---

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Load root geometry
  TGeoManager* tgeoManager = TGeoManager::Import(fileName);

  // Retrieve matrices
  LoadTransforms(tgeoManager);     

  return true;
}

//______________________________________________________________________________
void AliMUONGeometryTransformer::WriteTransform(ofstream& out,
                                   const TGeoMatrix* transform) const
{
// Writes the transformations 
// ---

  out << "   pos: ";
  const Double_t* xyz = transform->GetTranslation();
  out << setw(10) << setprecision(4) << xyz[0] << "  " 
      << setw(10) << setprecision(4) << xyz[1] << "  " 
      << setw(10) << setprecision(4) << xyz[2];

  out << "   rot: ";
  const Double_t* rm = transform->GetRotationMatrix();
  TGeoRotation rotation;
  rotation.SetMatrix(const_cast<Double_t*>(rm));
  Double_t a1, a2, a3, a4, a5, a6;
  rotation.GetAngles(a1, a2, a3, a4, a5, a6);
      
  out << setw(8) << setprecision(4) << a1 << "  " 
      << setw(8) << setprecision(4) << a2 << "  " 
      << setw(8) << setprecision(4) << a3 << "  " 
      << setw(8) << setprecision(4) << a4 << "  " 
      << setw(8) << setprecision(4) << a5 << "  " 
      << setw(8) << setprecision(4) << a6 << "  " << endl; 
}

//______________________________________________________________________________
void AliMUONGeometryTransformer::WriteModuleVolPaths(ofstream& out) const
{
// Write modules volume paths

  for (Int_t i=0; i<fModuleTransformers->GetEntriesFast(); i++) {
    AliMUONGeometryModuleTransformer* moduleTransformer 
      = (AliMUONGeometryModuleTransformer*)fModuleTransformers->At(i);

    // Write data on out
    out << "CH " 
        << setw(4) << moduleTransformer->GetModuleId() << "    " 
        << moduleTransformer->GetVolumePath() << endl;
  }     
  out << endl;	  	   	
}

//______________________________________________________________________________
void AliMUONGeometryTransformer::WriteDetElemVolPaths(ofstream& out) const
{
// Write detection elements volume paths

  for (Int_t i=0; i<fModuleTransformers->GetEntriesFast(); i++) {
    AliMUONGeometryModuleTransformer* moduleTransformer 
      = (AliMUONGeometryModuleTransformer*)fModuleTransformers->At(i);
    AliMUONGeometryStore* detElements 
      = moduleTransformer->GetDetElementStore();    

    for (Int_t j=0; j<detElements->GetNofEntries(); j++) {
      AliMUONGeometryDetElement* detElement
        = (AliMUONGeometryDetElement*)detElements->GetEntry(j);
	
      // Write data on out
      out << "DE " 
          << setw(4) << detElement->GetId() << "    " 
          << detElement->GetVolumePath() << endl;
    }
    out << endl;	  	   	
  }     
}

//______________________________________________________________________________
void AliMUONGeometryTransformer::WriteModuleTransforms(ofstream& out) const
{
// Write modules transformations

  for (Int_t i=0; i<fModuleTransformers->GetEntriesFast(); i++) {
    AliMUONGeometryModuleTransformer* moduleTransformer 
      = (AliMUONGeometryModuleTransformer*)fModuleTransformers->At(i);
    const TGeoMatrix* transform 
      = moduleTransformer->GetTransformation();    

    // Write data on out
    out << "CH " 
        << setw(4) << moduleTransformer->GetModuleId() + 1;
    
    WriteTransform(out, transform);
  }
  out << endl;
}

//______________________________________________________________________________
void AliMUONGeometryTransformer::WriteDetElemTransforms(ofstream& out) const
{
// Writes detection elements transformations
// ---

  for (Int_t i=0; i<fModuleTransformers->GetEntriesFast(); i++) {
    AliMUONGeometryModuleTransformer* moduleTransformer 
      = (AliMUONGeometryModuleTransformer*)fModuleTransformers->At(i);
    AliMUONGeometryStore* detElements 
      = moduleTransformer->GetDetElementStore();    

    for (Int_t j=0; j<detElements->GetNofEntries(); j++) {
      AliMUONGeometryDetElement* detElement
        = (AliMUONGeometryDetElement*)detElements->GetEntry(j);
      const TGeoMatrix* transform 
        = detElement->GetLocalTransformation(); 
	
      // Write data on out
      out << "DE " << setw(4) << detElement->GetId();
     
      WriteTransform(out, transform);
    }
    out << endl;	  	   	
  }     
}

//
// public functions
//

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadGeometryData(
                                const TString& volPathFileName,
                                const TString& transformFileName)
{
/// Read geometry data from given files;
/// if transformFileName has ".root" extension, the transformations
/// are loaded from root geometry file, otherwise ASCII file
/// format is supposed

  Bool_t result1 = ReadVolPaths(volPathFileName);

  // Get file extension
  std::string fileName = transformFileName.Data();
  std::string rootExt = fileName.substr(fileName.size()-5, fileName.size());
  Bool_t result2;
  if ( rootExt != ".root" ) 
    result2 = ReadTransformations(transformFileName);
  else   
    result2 = ReadTransformations2(transformFileName);
  
  return result1 && result2;
}  

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::ReadGeometryData(
                                const TString& volPathFileName,
                                TGeoManager* tgeoManager)
{
/// Load geometry data from root geometry using defined
/// voluem paths from file

  Bool_t result1 = ReadVolPaths(volPathFileName);

  Bool_t result2 = LoadTransforms(tgeoManager);
  
  return result1 && result2;
}  

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::WriteGeometryData(
                                 const TString& volPathFileName,
                                 const TString& transformFileName,
				 const TString& misalignFileName) const
{
/// Write geometry data into given files

  Bool_t result1 = WriteVolumePaths(volPathFileName);
  Bool_t result2 = WriteTransformations(transformFileName);
  
  Bool_t result3 = true;
  if ( misalignFileName != "" )
    result3 = WriteMisAlignmentData(misalignFileName);
  
  return result1 && result2 && result3;
}
				 
//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::WriteVolumePaths(const TString& fileName) const
{
// Writes volume paths for modules and detection element volumes into a file
// Returns true, if writing finished correctly.
// ---

  // No writing
  // if builder is not associated with any geometry module
  if (fModuleTransformers->GetEntriesFast() == 0) return false;

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Open output file
  ofstream out(filePath, ios::out);
  if (!out) {
    cerr << filePath << endl;	
    AliError("File not found.");
    return false;
  }
#if !defined (__DECCXX)
  out.setf(std::ios::fixed);
#endif
  WriteModuleVolPaths(out);
  WriteDetElemVolPaths(out);
  
  return true;
}  

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::WriteTransformations(const TString& fileName) const
{
// Writes transformations into a file
// Returns true, if writing finished correctly.
// ---

  // No writing
  // if builder is not associated with any geometry module
  if (fModuleTransformers->GetEntriesFast() == 0) return false;

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Open output file
  ofstream out(filePath, ios::out);
  if (!out) {
    cerr << filePath << endl;	
    AliError("File not found.");
    return false;
  }
#if !defined (__DECCXX)
  out.setf(std::ios::fixed);
#endif
  WriteModuleTransforms(out);
  WriteDetElemTransforms(out);
  
  return true;
}  

//______________________________________________________________________________
Bool_t  
AliMUONGeometryTransformer::WriteMisAlignmentData(const TString& fileName) const
{
// Writes misalignment data into a file
// Returns true, if writing finished correctly.
// ---

  // No writing
  // if builder is not associated with any geometry module
  if ( fModuleTransformers->GetEntriesFast() == 0 ) {
    AliWarningStream() << "No geometry modules defined." << endl;
    return false;
  }  
  
  // No writing
  // if builder has no mis-alignment data
  if ( ! fMisAlignArray ) {
    AliWarningStream() << "No mis-alignment data defined." << endl;
    return false;
  }  

  // File path
  TString filePath = gSystem->Getenv("ALICE_ROOT");
  filePath += "/MUON/data/";
  filePath += fileName;
  
  // Write mis-alignment data in the root file
  TFile file(fileName.Data(), "RECREATE");
  fMisAlignArray->Write();
  file.Close();
  
  return true;
}  

//_____________________________________________________________________________
void AliMUONGeometryTransformer::AddModuleTransformer(
                          AliMUONGeometryModuleTransformer* moduleTransformer)
{
/// Add the geometrymodule to the array

  fModuleTransformers->AddAt(moduleTransformer, 
                             moduleTransformer->GetModuleId());
}

//_____________________________________________________________________________
void  AliMUONGeometryTransformer::AddMisAlignModule(Int_t moduleId, 
                                              const TGeoHMatrix& matrix)
{
/// Build AliAlignObjMatrix with module ID, its volumePaths
/// and the given delta transformation matrix					      

  if ( ! fMisAlignArray )
    fMisAlignArray = new TClonesArray("AliAlignObjMatrix", 200);
    
  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformer(moduleId);
  if ( ! kTransformer ) {
    AliErrorStream() << "Module " << moduleId << " not found." << endl; 
    return;
  }   
  
  // Get path  
  TString path = kTransformer->GetVolumePath(); 
  
  // Get unique align object ID
  Int_t volId = AliAlignObj::LayerToVolUID(AliAlignObj::kMUON, moduleId); 

  // Create mis align matrix
  TClonesArray& refArray =*fMisAlignArray;
  Int_t pos = fMisAlignArray->GetEntriesFast();
  new (refArray[pos]) AliAlignObjMatrix(path.Data(), volId, 
                              const_cast<TGeoHMatrix&>(matrix));
}

//_____________________________________________________________________________
void  AliMUONGeometryTransformer::AddMisAlignDetElement(Int_t detElemId, 
                                              const TGeoHMatrix& matrix)
{
/// Build AliAlignObjMatrix with detection element ID, its volumePaths
/// and the given delta transformation matrix					      

  if ( ! fMisAlignArray )
    fMisAlignArray = new TClonesArray("AliAlignObjMatrix", 200);

  const AliMUONGeometryDetElement* kDetElement 
    = GetDetElement(detElemId);

  if ( ! kDetElement ) {
    AliErrorStream() << "Det element " << detElemId << " not found." << endl; 
    return;
  }   
  
  // Get path  
  TString path = kDetElement->GetVolumePath(); 
  
  // Get unique align object ID
  Int_t volId = AliAlignObj::LayerToVolUID(AliAlignObj::kMUON, detElemId); 

  // Create mis align matrix
  TClonesArray& refArray =*fMisAlignArray;
  Int_t pos = fMisAlignArray->GetEntriesFast();
  new(refArray[pos]) AliAlignObjMatrix(path.Data(), volId, 
                              const_cast<TGeoHMatrix&>(matrix));
}

//_____________________________________________________________________________
void AliMUONGeometryTransformer::Global2Local(Int_t detElemId,
                 Float_t xg, Float_t yg, Float_t zg, 
                 Float_t& xl, Float_t& yl, Float_t& zl) const
{
/// Transform point from the global reference frame (ALIC)
/// to the local reference frame of the detection element specified
/// by detElemId.

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId);
  
  if (kTransformer) 
    kTransformer->Global2Local(detElemId, xg, yg, zg, xl, yl, zl);
}   
		 
//_____________________________________________________________________________
void AliMUONGeometryTransformer::Global2Local(Int_t detElemId,
                 Double_t xg, Double_t yg, Double_t zg, 
                 Double_t& xl, Double_t& yl, Double_t& zl) const
{
/// Transform point from the global reference frame (ALIC)
/// to the local reference frame of the detection element specified
/// by detElemId.

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId);
  
  if (kTransformer) 
    kTransformer->Global2Local(detElemId, xg, yg, zg, xl, yl, zl);
}   

//_____________________________________________________________________________
void AliMUONGeometryTransformer::Local2Global(Int_t detElemId,
                 Float_t xl, Float_t yl, Float_t zl, 
                 Float_t& xg, Float_t& yg, Float_t& zg) const
{		 
/// Transform point from the local reference frame of the detection element 
/// specified by detElemId to the global reference frame (ALIC).

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId);
    
  if (kTransformer) 
    kTransformer->Local2Global(detElemId, xl, yl, zl, xg, yg, zg);
}   

//_____________________________________________________________________________
void AliMUONGeometryTransformer::Local2Global(Int_t detElemId,
                 Double_t xl, Double_t yl, Double_t zl, 
                 Double_t& xg, Double_t& yg, Double_t& zg) const
{		 
/// Transform point from the local reference frame of the detection element 
/// specified by detElemId to the global reference frame (ALIC).

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId);
    
  if (kTransformer) 
    kTransformer->Local2Global(detElemId, xl, yl, zl, xg, yg, zg);
}   

//_____________________________________________________________________________
const AliMUONGeometryModuleTransformer* 
AliMUONGeometryTransformer::GetModuleTransformer(Int_t index, Bool_t warn) const
{
/// Return the geometry module specified by index

  return GetModuleTransformerNonConst(index, warn);
}    

//_____________________________________________________________________________
const AliMUONGeometryModuleTransformer* 
AliMUONGeometryTransformer::GetModuleTransformerByDEId(Int_t detElemId, 
                                                       Bool_t warn) const
{
/// Return the geometry module specified by index

  // Get module index
  Int_t index = AliMUONGeometryStore::GetModuleId(detElemId);

  return GetModuleTransformer(index, warn);
}    

//_____________________________________________________________________________
const AliMUONGeometryDetElement* 
AliMUONGeometryTransformer::GetDetElement(Int_t detElemId, Bool_t warn) const
{
/// Return detection ellemnt with given detElemId			       

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId, warn);
    
  if (!kTransformer) return 0;
    
  return kTransformer->GetDetElement(detElemId, warn); 
}

//_____________________________________________________________________________
Bool_t  AliMUONGeometryTransformer::HasDE(Int_t detElemId) const
{
/// Return true if detection element with given detElemId is defined

  const AliMUONGeometryModuleTransformer* kTransformer 
    = GetModuleTransformerByDEId(detElemId, false);
    
  if (!kTransformer) return false;
    
  return ( kTransformer->GetDetElement(detElemId, false) != 0 );
}  
    

