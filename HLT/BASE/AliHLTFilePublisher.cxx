// $Id$

/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Authors: Matthias Richter <Matthias.Richter@ift.uib.no>                *
 *          for The ALICE Off-line Project.                               *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTFilePublisher.cxx
    @author Matthias Richter
    @date   
    @brief  HLT file publisher component implementation. */

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTFilePublisher.h"
#include <TObjString.h>
#include <TMath.h>
#include <TFile.h>

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTFilePublisher)

AliHLTFilePublisher::AliHLTFilePublisher()
  :
  fFileNames(),
  fFiles(),
  fpCurrent(NULL),
  fDataType(kAliHLTVoidDataType),
  fSpecification(~(AliHLTUInt32_t)0),
  fMaxSize(0)
{
  // make the lists owners of their objects in order to automatically
  // de-allocate the objects
  fFileNames.SetOwner();
  fFiles.SetOwner();
}

AliHLTFilePublisher::AliHLTFilePublisher(const AliHLTFilePublisher&)
  :
  fFileNames(),
  fFiles(),
  fpCurrent(NULL),
  fDataType(kAliHLTVoidDataType),
  fSpecification(0),
  fMaxSize(0)
{
  HLTFatal("copy constructor untested");
}

AliHLTFilePublisher& AliHLTFilePublisher::operator=(const AliHLTFilePublisher&)
{ 
  HLTFatal("assignment operator untested");
  return *this;
}

AliHLTFilePublisher::~AliHLTFilePublisher()
{
  // file list and file name list are owner of their objects and
  // delete all the objects
}

const char* AliHLTFilePublisher::GetComponentID()
{
  return "FilePublisher";
}

void AliHLTFilePublisher::GetInputDataTypes( vector<AliHLTComponentDataType>& list)
{
  list.clear();
}

AliHLTComponentDataType AliHLTFilePublisher::GetOutputDataType()
{
  return (AliHLTComponentDataType){ sizeof(AliHLTComponentDataType), kAliHLTVoidDataTypeID, kAliHLTVoidDataOrigin};
}

void AliHLTFilePublisher::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  constBase=fMaxSize;
  inputMultiplier=1.0;
}

AliHLTComponent* AliHLTFilePublisher::Spawn()
{
  return new AliHLTFilePublisher;
}

int AliHLTFilePublisher::DoInit( int argc, const char** argv )
{
  int iResult=0;
  TString argument="";
  int bMissingParam=0;
  for (int i; i<argc; i++) {
    argument=argv[i];

    // -datafile
    if (argument.CompareTo("-datafile")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TObjString* parameter=new TObjString(argv[i]);
      if (parameter) {
	fFileNames.Add(parameter);
      } else {
	iResult=-ENOMEM;
      }

      // -datafilelist
    } else if (argument.CompareTo("-datafilelist")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      HLTWarning("-datafilelist option not yet implemented");

      // -datatype
    } else if (argument.CompareTo("-datatype")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fID, argv[i], TMath::Min(kAliHLTComponentDataTypefIDsize, (Int_t)strlen(argv[i])));

      // -dataspec
    } else if (argument.CompareTo("-dataspec")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      if (parameter.IsDigit()) {
	fSpecification=(AliHLTUInt32_t)parameter.Atoi();
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -dataorigin
    } else if (argument.CompareTo("-dataorigin")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fOrigin, argv[i], TMath::Min(kAliHLTComponentDataTypefOriginSize,(Int_t)strlen(argv[i])));
    } else {
      HLTError("unknown argument %s", argument.Data());
      iResult=-EINVAL;
    }
  }
  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }
  if (fFileNames.GetSize()==0) {
    HLTError("the publisher needs at least one file argument");
    iResult=-EINVAL;
  }
  if (iResult>=0) iResult=OpenFiles();
  if (iResult<0) {
    fFileNames.Clear();
  }
  return iResult;
}

int AliHLTFilePublisher::OpenFiles()
{
  int iResult=0;
  TObjLink *lnk=fFileNames.FirstLink();
  while (lnk && iResult>=0) {
    TObjString* pFileName=(TObjString*)lnk->GetObject();
    if (pFileName) {
      TString fullFN= pFileName->GetString() + "?filetype=raw";
      TFile* pFile = new TFile(fullFN);
      if (pFile) {
	if (pFile->IsZombie()==0) {
	  fFiles.Add(pFile);
	  if (pFile->GetSize()>fMaxSize) fMaxSize=pFile->GetSize();
	} else {
	  HLTError("can not open file %s", (pFileName->GetString()).Data());
	  fFiles.Clear();
	  iResult=-ENOENT;
	}
      }
    }
    lnk = lnk->Next();
  }

  return iResult;
}

int AliHLTFilePublisher::DoDeinit()
{
  int iResult=0;
  fFileNames.Clear();
  fFiles.Clear();
  return iResult;
}

int AliHLTFilePublisher::GetEvent( const AliHLTComponentEventData& evtData,
	      AliHLTComponentTriggerData& trigData,
	      AliHLTUInt8_t* outputPtr, 
	      AliHLTUInt32_t& size,
	      vector<AliHLTComponentBlockData>& outputBlocks )
{
  int iResult=0;
  TFile * pFile=NULL;
  TObjLink *lnk;
  if (fpCurrent) lnk=fpCurrent->Next();
  if (lnk==NULL) lnk=fFiles.FirstLink();
  fpCurrent=lnk;
  if (lnk) {
    TFile* pFile=(TFile*)lnk->GetObject();
    if (pFile) {
      int iCopy=pFile->GetSize();
      if (iCopy>size) {
	iCopy=size;
	HLTWarning("buffer to small, data of file %s truncated", pFile->GetName());
      }
      if (pFile->ReadBuffer((char*)outputPtr, iCopy)!=0) {
	// ReadBuffer returns 1 in case of failure and 0 in case of success
	iResult=-EIO;
      } else {
	AliHLTComponentBlockData bd;
	FillBlockData(bd);
	bd.fPtr=outputPtr;
	bd.fOffset=0;
	bd.fSize=iCopy;
	bd.fDataType=fDataType;
	bd.fSpecification=fSpecification;
	outputBlocks.push_back(bd);
	size=iCopy;
      }
    } else {
      iResult=-EFAULT;
    }
  } else {
    iResult=-ENOENT;
  }
  return iResult;
}
