// $Id$

/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * ALICE Experiment at CERN, All rights reserved.                         *
 *                                                                        *
 * Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *
 *                  for The ALICE HLT Project.                            *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/** @file   AliHLTDataGenerator.cxx
    @author Matthias Richter
    @date   
    @brief  HLT file publisher component implementation. */

#if __GNUC__>= 3
using namespace std;
#endif

#include "AliHLTDataGenerator.h"
#include "TString.h"

/** the global object for component registration */
AliHLTDataGenerator gAliHLTDataGenerator;

/** ROOT macro for the implementation of ROOT specific class methods */
ClassImp(AliHLTDataGenerator)

AliHLTDataGenerator::AliHLTDataGenerator()
  :
  AliHLTDataSource(),
  fDataType(kAliHLTVoidDataType),
  fSpecification(~(AliHLTUInt32_t)0),
  fSize(0),
  fCurrSize(0),
  fRange(0),
  fDivisor(0),
  fSubtractor(0),
  fModulo(0)
{
  // see header file for class documentation
  // or
  // refer to README to build package
  // or
  // visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

  // make the lists owners of their objects in order to automatically
  // de-allocate the objects
}

AliHLTDataGenerator::~AliHLTDataGenerator()
{
  // see header file for class documentation

}

const char* AliHLTDataGenerator::GetComponentID()
{
  // see header file for class documentation
  return "DataGenerator";
}

AliHLTComponentDataType AliHLTDataGenerator::GetOutputDataType()
{
  // see header file for class documentation
  return kAliHLTMultipleDataType;
}

int AliHLTDataGenerator::GetOutputDataTypes(vector<AliHLTComponentDataType>& tgtList)
{
  int count=0;
  tgtList.clear();
  tgtList.push_back(fDataType);
  return count;
}

void AliHLTDataGenerator::GetOutputDataSize( unsigned long& constBase, double& inputMultiplier )
{
  // see header file for class documentation
  constBase=fCurrSize+fRange;
  inputMultiplier=1.0;
}

AliHLTComponent* AliHLTDataGenerator::Spawn()
{
  // see header file for class documentation
  return new AliHLTDataGenerator;
}

int AliHLTDataGenerator::DoInit( int argc, const char** argv )
{
  // see header file for class documentation

  //HLTDebug("%d %s", argc, argv[0]);
  int iResult=0;
  TString argument="";
  int bMissingParam=0;
  for (int i=0; i<argc && iResult>=0; i++) {
    argument=argv[i];
    if (argument.IsNull()) continue;

    // -datatype
    if (argument.CompareTo("-datatype")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fID, argv[i], TMath::Min(kAliHLTComponentDataTypefIDsize, (Int_t)strlen(argv[i])));
      if ((bMissingParam=(++i>=argc))) break;
      memcpy(&fDataType.fOrigin, argv[i], TMath::Min(kAliHLTComponentDataTypefOriginSize, (Int_t)strlen(argv[i])));

      // -dataspec
    } else if (argument.CompareTo("-dataspec")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      TString parameter(argv[i]);
      parameter.Remove(TString::kLeading, ' '); // remove all blanks
      if (parameter.IsDigit()) {
	fSpecification=(AliHLTUInt32_t)parameter.Atoi();
      } else if (parameter.BeginsWith("0x") &&
		 parameter.Replace(0,2,"",0).IsHex()) {
	sscanf(parameter.Data(),"%x", &fSpecification);
      } else {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -size
    } else if (argument.CompareTo("-size")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      if ((iResult=ScanSizeArgument(fSize, argv[i]))==-ERANGE) {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -range
    } else if (argument.CompareTo("-range")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      if ((iResult=ScanSizeArgument(fRange, argv[i]))==-ERANGE) {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -divisor
    } else if (argument.CompareTo("-divisor")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      if ((iResult=ScanSizeArgument(fDivisor, argv[i]))==-ERANGE) {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -offset
    } else if (argument.CompareTo("-offset")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      if ((iResult=ScanSizeArgument(fSubtractor, argv[i]))==-ERANGE) {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
      // -modulo
    } else if (argument.CompareTo("-modulo")==0) {
      if ((bMissingParam=(++i>=argc))) break;
      if ((iResult=ScanSizeArgument(fModulo, argv[i]))==-ERANGE) {
	HLTError("wrong parameter for argument %s, number expected", argument.Data());
	iResult=-EINVAL;
      }
    } else {
      if ((iResult=ScanArgument(argc-i, &argv[i]))==-EINVAL) {
	HLTError("unknown argument %s", argument.Data());
	break;
      } else if (iResult==-EPROTO) {
	bMissingParam=1;
	break;
      } else if (iResult>=0) {
	i+=iResult;
	iResult=0;
      }
    }
  }

  if (bMissingParam) {
    HLTError("missing parameter for argument %s", argument.Data());
    iResult=-EINVAL;
  }

  fCurrSize=fSize;

  return iResult;
}

int AliHLTDataGenerator::ScanSizeArgument(AliHLTUInt32_t &size, const char* arg)
{
  int iResult=0;
  if (arg) {
    TString parameter(arg);
    AliHLTUInt32_t base=1;
    parameter.Remove(TString::kLeading, ' '); // remove all blanks
    if (parameter.EndsWith("k")) {
      base=0x400; // one k
      parameter.Remove(TString::kTrailing, 'k');
    } else if (parameter.EndsWith("M")) {
      base=0x100000; // one M
      parameter.Remove(TString::kTrailing, 'M');
    }
    if (parameter.IsDigit()) {
      size=(AliHLTUInt32_t)parameter.Atoi()*base;
    } else {
      iResult=-ERANGE;
    }
  } else {
    iResult=-EINVAL;
  }
  return iResult;
}

int AliHLTDataGenerator::ScanArgument(int argc, const char** argv)
{
  // see header file for class documentation

  // there are no other arguments than the standard ones
  if (argc==0 && argv==NULL) {
    // this is just to get rid of the warning "unused parameter"
  }
  return -EPROTO;
}

int AliHLTDataGenerator::DoDeinit()
{
  // see header file for class documentation
  int iResult=0;
  return iResult;
}

int AliHLTDataGenerator::GetEvent( const AliHLTComponentEventData& /*evtData*/,
				   AliHLTComponentTriggerData& /*trigData*/,
				   AliHLTUInt8_t* outputPtr, 
				   AliHLTUInt32_t& size,
				   vector<AliHLTComponentBlockData>& outputBlocks )
{
  int iResult=0;
  AliHLTUInt32_t generated=fCurrSize;
  if (generated<=size ) {
    AliHLTComponentBlockData bd;
    FillBlockData(bd);
    bd.fPtr=outputPtr;
    bd.fOffset=0;
    bd.fSize=generated;
    bd.fDataType=fDataType;
    bd.fSpecification=fSpecification;
    outputBlocks.push_back(bd);
    size=generated;

    if (fModulo>0 && ((GetEventCount()+1)%fModulo)==0) {
      // manipulate the size
      if (fDivisor>0) {
	fCurrSize/=fDivisor;
	if (fCurrSize==0) fCurrSize=fSize; //reset
      }
      if (fSubtractor>0) {
	if (fCurrSize<fSubtractor) {
	  fCurrSize=fSize; // reset
	} else {
	  fCurrSize-=fSubtractor;
	}
      }
      HLTDebug("manipulated output size: %d", fCurrSize);
    }

  } else {
    iResult=-ENOSPC;
  }

  return iResult;
}
