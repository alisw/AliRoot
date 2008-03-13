
/**************************************************************************
 * This file is property of and copyright by the ALICE HLT Project        * 
 * All rights reserved.                                                   *
 *                                                                        *
 * Primary Authors: Oystein Djuvsland                                     *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          * 
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
#include "AliHLTPHOSBaselineAnalyzerComponent.h"
#include "AliHLTPHOSBaselineAnalyzer.h"
#include "AliHLTPHOSRcuCellEnergyDataStruct.h"
#include "AliHLTPHOSBaseline.h"
#include "TTree.h"
#include "TChain.h"
#include "AliHLTPHOSProcessor.h"
#include "TClonesArray.h"
#include "TFile.h"
//#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>

/** 
 * Class does baseline analysis
 * 
 * @file   AliHLTPHOSBaselineAnalyzerComponent.cxx
 * @author Oystein Djuvsland
 * @date   
 * @brief  A baseline analyzer for PHOS HLT
*/

// see below for class documentation
// or
// refer to README to build package
// or
// visit http://web.ift.uib.no/~kjeks/doc/alice-hlt

const AliHLTComponentDataType AliHLTPHOSBaselineAnalyzerComponent::fgkInputDataTypes[]={kAliHLTVoidDataType,{0,"",""}};

AliHLTPHOSBaselineAnalyzerComponent gAliHLTPHOSBaselineAnalyzerComponent;

AliHLTPHOSBaselineAnalyzerComponent::AliHLTPHOSBaselineAnalyzerComponent() :AliHLTPHOSProcessor(),
									    fBaselineAnalyzerPtr(0),
									    fTreePtr(0),
									    fBaselineArrayPtr(0),
									    fEvCnt(0),
									    fWriteInterval(100),
									    fFillInterval(100),
									    fFilename(0),
									    fDirectory(0),
									    fHistPath(0),
									    fRunNb(0),
									    fCalculateAll(false)
{
   //See header file for documentation
}

AliHLTPHOSBaselineAnalyzerComponent::~AliHLTPHOSBaselineAnalyzerComponent()
{
 //See header file for documentation
}



int 
AliHLTPHOSBaselineAnalyzerComponent::Deinit()
{
  //See header file for documentation
  fBaselineAnalyzerPtr->CalculateChannelsBaselineRMS();
  char filename [256];
  sprintf(filename, "%s/run%d_baselineTree_%d.root", fDirectory, fRunNb,fEvCnt/fWriteInterval);
  
  if( CheckFileLog( __FILE__ ,  filename,  "w") == true)
    {
      fBaselineAnalyzerPtr->WriteAccumulatedBaselines(filename);
      DoneWritingLog( __FILE__  , filename);
    }

  sprintf(filename, "%s/run%d_channelHistograms.root", fHistPath, fRunNb);
  
  if( CheckFileLog( __FILE__ ,  filename,  "w") == true)
    {
      fBaselineAnalyzerPtr->WriteChannelHistograms(filename);
      DoneWritingLog( __FILE__  , filename);
    }

  sprintf(filename, "%s/run%d_RMSHistogram.root", fHistPath, fRunNb);

  if( CheckFileLog( __FILE__ ,  filename,  "w") == true)
    {
      fBaselineAnalyzerPtr->WriteRMSHistogram(filename);
      DoneWritingLog( __FILE__  , filename);
    }

  if(fCalculateAll)
    {
      CalculateAll(); 
    }
  if(fBaselineAnalyzerPtr)
    {
      delete fBaselineAnalyzerPtr;
      fBaselineAnalyzerPtr = 0;
    }
  if(fTreePtr)
    {
      fTreePtr = 0;
    }
  if(fFilename)
    {
      delete fFilename;
      fFilename = 0;
    }
  return 0;
}

const char*
AliHLTPHOSBaselineAnalyzerComponent::GetComponentID()
{
  //See header file for documentation
  return "PhosBaselineAnalyzer";
}

void
AliHLTPHOSBaselineAnalyzerComponent::GetInputDataTypes(vector<AliHLTComponentDataType>& list)
{ 
 //Get datatypes for input
  const AliHLTComponentDataType* pType=fgkInputDataTypes;
  while (pType->fID!=0) {
    list.push_back(*pType); 
    pType++;
  }
}

AliHLTComponentDataType 
AliHLTPHOSBaselineAnalyzerComponent::GetOutputDataType()
{
 //See header file for documentation
  return AliHLTPHOSDefinitions::fgkBaselineDataType;
}


void 
AliHLTPHOSBaselineAnalyzerComponent::GetOutputDataSize(unsigned long& constBase, double& inputMultiplier)
{
  //See header file for documentation
  constBase = 30;
  inputMultiplier = 1;
}

int 
AliHLTPHOSBaselineAnalyzerComponent::DoEvent(const AliHLTComponentEventData& evtData, const AliHLTComponentBlockData* blocks,
					AliHLTComponentTriggerData& /*trigData*/, AliHLTUInt8_t* outputPtr, AliHLTUInt32_t& /*size*/,  //TODO: I think that not setting the size explicitly to zero when returning from this method may be a subtle bug in this component. Please check.
					std::vector<AliHLTComponentBlockData>& /*outputBlocks*/)
{
  AliHLTUInt8_t* outBPtr;
  outBPtr = outputPtr;
  const AliHLTComponentBlockData* iter = 0; 
  unsigned long ndx; 

  for( ndx = 0; ndx < evtData.fBlockCnt; ndx++ )
    {
      iter = blocks+ndx;
      
      if(iter->fDataType != AliHLTPHOSDefinitions::fgkCellEnergyDataType)
	{
	  Logging(kHLTLogWarning, __FILE__ , "wrong datatype" , "dat is not of type fgkCellEnergyDataType");
	  continue;

	}
      fBaselineAnalyzerPtr->CalculateRcuBaselines(reinterpret_cast<AliHLTPHOSRcuCellEnergyDataStruct*>(iter->fPtr));
    }
  
  fEvCnt++;

  //PushBack(fDigitArrayPtr, kAliHLTAnyDataType, (AliHLTUInt32_t)0);
  

  if(fEvCnt % fFillInterval == 0)
    {
      fBaselineAnalyzerPtr->FillTree(); 
    }
  if(fEvCnt % fWriteInterval == 0)
    {
      char filename [256];
      sprintf(filename, "%s/run%d_baselineTree_%d.root", fDirectory, fRunNb,fEvCnt/fWriteInterval - 1);
      //:CheckFileLog(const char *origin,   const char *filename,  const char opt)
      if( (CheckFileLog( __FILE__ , filename,  "w") == true))
	{
	  fBaselineAnalyzerPtr->WriteAccumulatedBaselines(filename);
	  delete fTreePtr;
	  fTreePtr = new TTree("baselineTree", "Baselines");
	  fBaselineAnalyzerPtr->SetRootObjects(fTreePtr, fBaselineArrayPtr);
	  DoneWritingLog( __FILE__ , filename);
	}
      
      return 0;
    }
  
  return 0;
}

int
AliHLTPHOSBaselineAnalyzerComponent::DoInit(int argc, const char** argv )
{
  //See header file for documentation
  Bool_t pathSet = false;
  Bool_t histPathSet = false;
  Bool_t nSamplesSet = false;
  fFilename = new char[50];
  fDirectory = new char[50];
  fHistPath = new char[50];

  /*
  fstream runNbFile;
  runNbFile.open("/opt/HLT-public/rundir/runNumber.txt");
  runNbFile >> fRunNb;
  runNbFile.close();
  */

  fBaselineAnalyzerPtr = new AliHLTPHOSBaselineAnalyzer();
  fTreePtr = new TTree("baselineTree", "Baselines");
  fBaselineArrayPtr = new TClonesArray("AliHLTPHOSBaseline",N_XCOLUMNS_MOD*N_ZROWS_MOD*N_GAINS);
  fBaselineAnalyzerPtr->SetRootObjects(fTreePtr, fBaselineArrayPtr);
  fBaselineAnalyzerPtr->SetMaxCrazyDifference(15);
  fBaselineAnalyzerPtr->SetMaxSignal(120);
   
  for(int i = 0; i < argc; i++)
    {
      if(!strcmp("-totalbaselineoutput", argv[i]))
	{
	  fCalculateAll = true;
	  strcpy(fFilename, argv[i+1]);
	}
      if(!strcmp("-path", argv[i]))
	{
	  strcpy(fDirectory, argv[i+1]);
	  pathSet = true;
	}
	if( !strcmp("-histpath", argv[i]))
	{
	  strcpy(fHistPath, argv[i+1]);
	  histPathSet = true;
	}     
      if(!strcmp("-nsamples", argv[i]))
	{
          fBaselineAnalyzerPtr->SetNumberOfSamples(atoi(argv[i+1]));
	  nSamplesSet = true;
	}	      
      if(!strcmp("-maxsignal", argv[i]))
	{
	  fBaselineAnalyzerPtr->SetMaxSignal(atoi(argv[i+1]));
	}
    }
	
  
  fWriteInterval = 100;
  fFillInterval = 100;

  if(fCalculateAll) 
    {
      CalculateAll();
    }
    

  return 0;
}
 

 AliHLTComponent*
   AliHLTPHOSBaselineAnalyzerComponent::Spawn()
 {
   //See header file for documentation
   return new AliHLTPHOSBaselineAnalyzerComponent();
 }
 
 void 
   AliHLTPHOSBaselineAnalyzerComponent::CalculateAll()
{
  //See header file for documentation
  AliHLTPHOSBaseline *baselineObject = 0;
  TChain* chain = new TChain("baselineTree");
  TTree* totalTree = new TTree("baselineTree", "Baselines");
  TClonesArray* baselineArray = new TClonesArray("AliHLTPHOSBaseline", N_XCOLUMNS_MOD*N_ZROWS_MOD*N_GAINS);
  TClonesArray* totalBaselineArray = new TClonesArray("AliHLTPHOSBaseline", N_XCOLUMNS_MOD*N_ZROWS_MOD*N_GAINS);
      
  char filepath [50];

  Float_t tmpBaselines[N_XCOLUMNS_MOD][N_ZROWS_MOD][N_GAINS][2];
  
  for(int x = 0; x < N_XCOLUMNS_MOD; x++)
    {
      for(int z = 0; z < N_ZROWS_MOD; z++)
	{
	  for(int gain = 0; gain < N_GAINS; gain++)
	    {
	      for(int d = 0; d < 2; d++)
		{
		  tmpBaselines[x][z][gain][d] = 0;
		}
	    }
	}
    }

  sprintf(filepath, "%s/run%d*", fDirectory, fRunNb);
  Logging(kHLTLogInfo, __FILE__ , "Adding files" , "Adding files from %s", filepath);
  chain->Add(filepath);
  Logging(kHLTLogInfo, __FILE__ , "Adding files  " , "Gives a total number of %d file",chain->GetEntries());
  chain->SetBranchAddress("Baselines", &baselineArray);
  totalTree->Branch("Baselines", &totalBaselineArray);

  Int_t nEntries = 0;
  Int_t totEntries = 0;
  Float_t baseline = 0;
  Float_t oldBaseline = 0;

  Int_t x = 0;
  Int_t z = 0;
  Int_t gain = 0;

  for(int i = 0; i < chain->GetEntries(); i++)
    {
      chain->GetEntry(i);
      for(int j = 0; j < baselineArray->GetEntriesFast(); j++)
	{
	  baselineObject = (AliHLTPHOSBaseline*)baselineArray->At(j);
	  x = baselineObject->GetX();
	  z = baselineObject->GetZ();
	  gain = baselineObject->GetGain();
	  nEntries = baselineObject->GetEntries();
	  baseline = baselineObject->GetBaseline();
	  oldBaseline =  tmpBaselines[x][z][gain][0];
	  totEntries =  (Int_t)tmpBaselines[x][z][gain][1];
	  tmpBaselines[x][z][gain][0] = (oldBaseline*totEntries + baseline)/(totEntries + 1);
	  tmpBaselines[x][z][gain][1] = totEntries + 1;
	}
    }

  Int_t n = 0;
  
  for(x = 0; x < (Int_t)N_XCOLUMNS_MOD; x++)
    {
      for(z = 0; z < (Int_t)N_ZROWS_MOD; z++)
	{
	  for(gain = 0; gain < (Int_t)N_GAINS; gain++)
	    {
	      baselineObject = (AliHLTPHOSBaseline*)totalBaselineArray->New(n);
	      baselineObject->SetBaseline(tmpBaselines[x][z][gain][0]);
	      baselineObject->SetX(x);
	      baselineObject->SetZ(z);
	      baselineObject->SetGain(gain);
	      if( tmpBaselines[x][z][gain][1] == 0)
	      {
		Logging(kHLTLogWarning, __FILE__ , "zero entries", " for x:= %d, z=%d, gain = %d, Setting baseline to 40", x, z, gain);	
		baselineObject->SetBaseline(40);
		continue;
	      }
	      baselineObject->SetEntries( Int_t(tmpBaselines[x][z][gain][1]) );
	      n++;
	    }
	}
    }

  totalTree->Fill();
 

  if(CheckFileLog(__FILE__, fFilename, "w") == true)
    {
      TFile *outfile = new TFile(fFilename,"recreate");
      totalTree->Write();
      outfile->Close();
      DoneWritingLog(__FILE__ , fFilename);
      delete outfile;
    }
}
