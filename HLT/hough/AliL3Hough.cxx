// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"
#include <sys/time.h>

#include "AliL3Logging.h"
#include "AliL3HoughMerger.h"
#include "AliL3HoughIntMerger.h"
#include "AliL3HoughGlobalMerger.h"
#include "AliL3Histogram.h"
#include "AliL3Hough.h"
#include "AliL3HoughTransformer.h"
#include "AliL3HoughClusterTransformer.h"
#include "AliL3HoughTransformerLUT.h"
#include "AliL3HoughTransformerVhdl.h"
#include "AliL3HoughTransformerGap.h"
#include "AliL3HoughMaxFinder.h"
#include "AliL3Benchmark.h"
#ifdef use_aliroot
#include "AliL3FileHandler.h"
#else
#include "AliL3MemHandler.h"
#endif
#include "AliL3DataHandler.h"
#include "AliL3DigitData.h"
#include "AliL3HoughEval.h"
#include "AliL3Transform.h"
#include "AliL3TrackArray.h"
#include "AliL3HoughTrack.h"
#include "AliL3DDLDataFileHandler.h"

#if GCCVERSION == 3
using namespace std;
#endif

/** /class AliL3Hough
//<pre>
//_____________________________________________________________
// AliL3Hough
//
// Interface class for the Hough transform
//
// Example how to use:
//
// AliL3Hough *hough = new AliL3Hough(path,kTRUE,NumberOfEtaSegments);
// hough->ReadData(slice);
// hough->Transform();
// hough->FindTrackCandidates();
// 
// AliL3TrackArray *tracks = hough->GetTracks(patch);
//</pre>
*/

ClassImp(AliL3Hough)

AliL3Hough::AliL3Hough()
{
  //Constructor
  
  fBinary        = kFALSE;
  fAddHistograms = kFALSE;
  fDoIterative   = kFALSE; 
  fWriteDigits   = kFALSE;
  fUse8bits      = kFALSE;

  fMemHandler       = 0;
  fHoughTransformer = 0;
  fEval             = 0;
  fPeakFinder       = 0;
  fTracks           = 0;
  fGlobalTracks     = 0;
  fMerger           = 0;
  fInterMerger      = 0;
  fGlobalMerger     = 0;
  fBenchmark        = 0;
  
  fNEtaSegments     = 0;
  fNPatches         = 0;
  fVersion          = 0;
  fCurrentSlice     = 0;
  fEvent            = 0;
  
  fKappaSpread=6;
  fPeakRatio=0.5;
  
  SetTransformerParams();
  SetThreshold();
  SetNSaveIterations();
  SetPeakThreshold();
#ifdef use_aliroot
  //just be sure that index is empty for new event
    AliL3FileHandler::CleanStaticIndex(); 
#endif
}

AliL3Hough::AliL3Hough(Char_t *path,Bool_t binary,Int_t n_eta_segments,Bool_t bit8,Int_t tv,Char_t *infile)
{
  fBinary = binary;
  strcpy(fPath,path);
  fNEtaSegments  = n_eta_segments;
  fAddHistograms = kFALSE;
  fDoIterative   = kFALSE; 
  fWriteDigits   = kFALSE;
  fUse8bits      = bit8;
  fVersion       = tv;
  fKappaSpread=6;
  fPeakRatio=0.5;
  if(!fBinary)
    fInputFile = infile;
  else
    fInputFile = 0;

#ifdef use_aliroot
  //just be sure that index is empty for new event
    AliL3FileHandler::CleanStaticIndex(); 
#endif
}

AliL3Hough::~AliL3Hough()
{
  //dtor

  CleanUp();
  if(fMerger)
    delete fMerger;
  //cout << "Cleaned class merger " << endl;
  if(fInterMerger)
    delete fInterMerger;
  //cout << "Cleaned class inter " << endl;
  if(fPeakFinder)
    delete fPeakFinder;
  //cout << "Cleaned class peak " << endl;
  if(fGlobalMerger)
    delete fGlobalMerger;
  //cout << "Cleaned class global " << endl;
  if(fBenchmark)
    delete fBenchmark;
  //cout << "Cleaned class bench " << endl;
  if(fGlobalTracks)
    delete fGlobalTracks;
  //cout << "Cleaned class globaltracks " << endl;
}

void AliL3Hough::CleanUp()
{
  //Cleanup memory
  
  for(Int_t i=0; i<fNPatches; i++)
    {
      if(fTracks[i]) delete fTracks[i];
      //cout << "Cleaned tracks " << i << endl;
      if(fEval[i]) delete fEval[i];
      //cout << "Cleaned eval " << i << endl;
      if(fHoughTransformer[i]) delete fHoughTransformer[i];
      //cout << "Cleaned traf " << i << endl;
      if(fMemHandler[i]) delete fMemHandler[i];
      //cout << "Cleaned mem " << i << endl;
    }
  
  if(fTracks) delete [] fTracks;
  //cout << "Cleaned class tracks " << endl;
  if(fEval) delete [] fEval;
  //cout << "Cleaned class eval " << endl;
  if(fHoughTransformer) delete [] fHoughTransformer;
  //cout << "Cleaned cleass trafo " << endl;
  if(fMemHandler) delete [] fMemHandler;
  //cout << "Cleaned class mem " << endl;
}

void AliL3Hough::Init(Char_t *path,Bool_t binary,Int_t n_eta_segments,Bool_t bit8,Int_t tv,Char_t *infile)
{
  fBinary = binary;
  strcpy(fPath,path);
  fNEtaSegments = n_eta_segments;
  fWriteDigits  = kFALSE;
  fUse8bits     = bit8;
  fVersion      = tv;
  if(!fBinary)
    fInputFile = infile;
  else
    fInputFile = 0;

  Init(); //do the rest
}

void AliL3Hough::Init(Bool_t doit, Bool_t addhists)
{
  fDoIterative   = doit; 
  fAddHistograms = addhists;

  fNPatches = AliL3Transform::GetNPatches();
  
  fHoughTransformer = new AliL3HoughBaseTransformer*[fNPatches];
  fMemHandler = new AliL3MemHandler*[fNPatches];

  fTracks = new AliL3TrackArray*[fNPatches];
  fEval = new AliL3HoughEval*[fNPatches];
  
  fGlobalTracks = new AliL3TrackArray("AliL3HoughTrack");
  
  for(Int_t i=0; i<fNPatches; i++)
    {
      switch (fVersion){ //choose Transformer
      case 1: 
	fHoughTransformer[i] = new AliL3HoughTransformerLUT(0,i,fNEtaSegments);
	break;
      case 2:
	fHoughTransformer[i] = new AliL3HoughClusterTransformer(0,i,fNEtaSegments);
	break;
      case 3:
	fHoughTransformer[i] = new AliL3HoughTransformerVhdl(0,i,fNEtaSegments,fNSaveIterations);
	break;
      case 4:
	fHoughTransformer[i] = new AliL3HoughTransformerGap(0,i,fNEtaSegments);
	break;
      default:
	fHoughTransformer[i] = new AliL3HoughTransformer(0,i,fNEtaSegments,kFALSE,kFALSE);
      }

      fHoughTransformer[i]->CreateHistograms(fNBinX[i],fLowPt[i],fNBinY[i],-fPhi[i],fPhi[i]);
      //fHoughTransformer[i]->CreateHistograms(fLowPt[i],fUpperPt[i],fPtRes[i],fNBinY[i],fPhi[i]);

      fHoughTransformer[i]->SetLowerThreshold(fThreshold[i]);
      fHoughTransformer[i]->SetUpperThreshold(100);

      LOG(AliL3Log::kInformational,"AliL3Hough::Init","Version")
	<<"Initializing Hough transformer version "<<fVersion<<ENDLOG;
      
      fEval[i] = new AliL3HoughEval();
      fTracks[i] = new AliL3TrackArray("AliL3HoughTrack");
      if(fUse8bits)
	fMemHandler[i] = new AliL3DataHandler();
      else
#ifdef use_aliroot
      	{
	  if(!fInputFile) {
	    /* In case of reading digits file */
	    fMemHandler[i] = new AliL3FileHandler(kTRUE); //use static index
	    if(!fBinary) {
	      Char_t filename[1024];
	      sprintf(filename,"%s/digitfile.root",fPath);
              fMemHandler[i]->SetAliInput(filename);
	    }
	  }
	  else {
	    /* In case of reading rawdata from ROOT file */
	    fMemHandler[i] = new AliL3DDLDataFileHandler();
	    fMemHandler[i]->SetReaderInput(fInputFile);
	  }
	}
#else
      fMemHandler[i] = new AliL3MemHandler();
#endif
    }

  fPeakFinder = new AliL3HoughMaxFinder("KappaPhi",1000);
  fMerger = new AliL3HoughMerger(fNPatches);
  fInterMerger = new AliL3HoughIntMerger();
  fGlobalMerger = 0;
  fBenchmark = new AliL3Benchmark();
}

void AliL3Hough::SetTransformerParams(Float_t ptres,Float_t ptmin,Float_t ptmax,Int_t ny,Int_t patch)
{

  Int_t mrow;
  Float_t psi=0;
  if(patch==-1)
    mrow = 80;
  else
    mrow = AliL3Transform::GetLastRow(patch);
  if(ptmin)
    {
      Double_t lineradius = sqrt(pow(AliL3Transform::Row2X(mrow),2) + pow(AliL3Transform::GetMaxY(mrow),2));
      Double_t kappa = -1*AliL3Transform::GetBField()*AliL3Transform::GetBFact()/ptmin;
      psi = AliL3Transform::Deg2Rad(10) - asin(lineradius*kappa/2);
      cout<<"Calculated psi range "<<psi<<" in patch "<<patch<<endl;
    }

  if(patch==-1)
    {
      Int_t i=0;
      while(i < 6)
	{
	  fPtRes[i] = ptres;
	  fLowPt[i] = ptmin;
	  fUpperPt[i] = ptmax;
	  fNBinY[i] = ny;
	  fPhi[i] = psi;
	  fNBinX[i]=0;
	  i++;
	}
      return;
    }

  fPtRes[patch] = ptres;
  fLowPt[patch] = ptmin;
  fUpperPt[patch] = ptmax;
  fNBinY[patch] = ny;
  fPhi[patch] = psi;
}

void AliL3Hough::SetTransformerParams(Int_t nx,Int_t ny,Float_t ptmin,Int_t patch)
{

  Int_t mrow=80;
  Double_t lineradius = sqrt(pow(AliL3Transform::Row2X(mrow),2) + pow(AliL3Transform::GetMaxY(mrow),2));
  Double_t kappa = -1*AliL3Transform::GetBField()*AliL3Transform::GetBFact()/ptmin;
  Double_t psi = AliL3Transform::Deg2Rad(10) - asin(lineradius*kappa/2);
  cout<<"Calculated psi range "<<psi<<" in patch "<<patch<<endl;
  
  Int_t i=0;
  while(i < 6)
    {
      fLowPt[i] = ptmin;
      fNBinY[i] = ny;
      fNBinX[i] = nx;
      fPhi[i] = psi;
      i++;
    }
}

void AliL3Hough::SetTransformerParams(Int_t nx,Int_t ny,Float_t lpt,Float_t phi)
{
  Int_t i=0;
  while(i < 6)
    {
      fLowPt[i] = lpt;
      fNBinY[i] = ny;
      fNBinX[i] = nx;
      fPhi[i] = phi;
      i++;
    }
}

void AliL3Hough::SetThreshold(Int_t t3,Int_t patch)
{
  if(patch==-1)
    {
      Int_t i=0;
      while(i < 6)
	fThreshold[i++]=t3;
      return;
    }
  fThreshold[patch]=t3;
}

void AliL3Hough::SetPeakThreshold(Int_t threshold,Int_t patch)
{
  if(patch==-1)
    {
      Int_t i=0;
      while(i < 6)
	fPeakThreshold[i++]=threshold;
      return;
    }
  fPeakThreshold[patch]=threshold;
}

void AliL3Hough::DoBench(Char_t *name)
{
  fBenchmark->Analyze(name);
}

void AliL3Hough::Process(Int_t minslice,Int_t maxslice)
{
  //Process all slices [minslice,maxslice].
  fGlobalMerger = new AliL3HoughGlobalMerger(minslice,maxslice);
  
  for(Int_t i=minslice; i<=maxslice; i++)
    {
      ReadData(i);
      Transform();
      if(fAddHistograms)
	AddAllHistograms();
      FindTrackCandidates();
      //Evaluate();
      //fGlobalMerger->FillTracks(fTracks[0],i);
    }
}

void AliL3Hough::ReadData(Int_t slice,Int_t eventnr)
{
  //Read data from files, binary or root.
  
#ifdef use_aliroot
  if(fEvent!=eventnr) //just be sure that index is empty for new event
    AliL3FileHandler::CleanStaticIndex(); 
#endif
  fCurrentSlice = slice;

  for(Int_t i=0; i<fNPatches; i++)
    {
      fMemHandler[i]->Free();
      UInt_t ndigits=0;
      AliL3DigitRowData *digits =0;
      Char_t name[256];
      fMemHandler[i]->Init(slice,i);
      if(fBinary)//take input data from binary files
	{
	  if(fUse8bits)
	    sprintf(name,"%s/binaries/digits_c8_%d_%d_%d.raw",fPath,eventnr,slice,i);
	  else
	    sprintf(name,"%s/binaries/digits_%d_%d_%d.raw",fPath,eventnr,slice,i);

	  fMemHandler[i]->SetBinaryInput(name);
	  digits = (AliL3DigitRowData *)fMemHandler[i]->CompBinary2Memory(ndigits);
	  fMemHandler[i]->CloseBinaryInput();
	}
      else //read data from root file
	{
#ifdef use_aliroot
	  if(fEvent!=eventnr)
	    fMemHandler[i]->FreeDigitsTree();//or else the new event is not loaded
	  digits=(AliL3DigitRowData *)fMemHandler[i]->AliAltroDigits2Memory(ndigits,eventnr);
#else
	  cerr<<"You cannot read from rootfile now"<<endl;
#endif
	}

      //set input data and init transformer
      fHoughTransformer[i]->SetInputData(ndigits,digits);
      fHoughTransformer[i]->Init(slice,i,fNEtaSegments);
    }

  fEvent=eventnr;
}

void AliL3Hough::Transform(Int_t *row_range)
{
  //Transform all data given to the transformer within the given slice
  //(after ReadData(slice))
  
  Double_t initTime,cpuTime;
  initTime = GetCpuTime();
  for(Int_t i=0; i<fNPatches; i++)
    {
      fHoughTransformer[i]->Reset();//Reset the histograms
      fBenchmark->Start("Hough Transform");
      if(!row_range)
	fHoughTransformer[i]->TransformCircle();
      else
	fHoughTransformer[i]->TransformCircleC(row_range,1);
      fBenchmark->Stop("Hough Transform");
    }
  cpuTime = GetCpuTime() - initTime;
  LOG(AliL3Log::kInformational,"AliL3Hough::Transform()","Timing")
    <<"Transform done in average per patch of "<<cpuTime*1000/fNPatches<<" ms"<<ENDLOG;
}

void AliL3Hough::MergePatches()
{
  if(fAddHistograms) //Nothing to merge here
    return;
  fMerger->MergePatches(kTRUE);
}

void AliL3Hough::MergeInternally()
{
  if(fAddHistograms)
    fInterMerger->FillTracks(fTracks[0]);
  else
    fInterMerger->FillTracks(fMerger->GetOutTracks());
  
  fInterMerger->MMerge();
}

void AliL3Hough::ProcessSliceIter()
{
  //Process current slice (after ReadData(slice)) iteratively.
  
  if(!fAddHistograms)
    {
      for(Int_t i=0; i<fNPatches; i++)
	{
	  ProcessPatchIter(i);
	  fMerger->FillTracks(fTracks[i],i); //Copy tracks to merger
	}
    }
  else
    {
      for(Int_t i=0; i<10; i++)
	{
	  Transform();
	  AddAllHistograms();
	  InitEvaluate();
	  AliL3HoughBaseTransformer *tr = fHoughTransformer[0];
	  for(Int_t j=0; j<fNEtaSegments; j++)
	    {
	      AliL3Histogram *hist = tr->GetHistogram(j);
	      if(hist->GetNEntries()==0) continue;
	      fPeakFinder->Reset();
	      fPeakFinder->SetHistogram(hist);
	      fPeakFinder->FindAbsMaxima();
	      AliL3HoughTrack *track = (AliL3HoughTrack*)fTracks[0]->NextTrack();
	      track->SetTrackParameters(fPeakFinder->GetXPeak(0),fPeakFinder->GetYPeak(0),fPeakFinder->GetWeight(0));
	      track->SetEtaIndex(j);
	      track->SetEta(tr->GetEta(j,fCurrentSlice));
	      for(Int_t k=0; k<fNPatches; k++)
		{
		  fEval[i]->SetNumOfPadsToLook(2);
		  fEval[i]->SetNumOfRowsToMiss(2);
		  fEval[i]->RemoveFoundTracks();
		  /*
		  Int_t nrows=0;
		  if(!fEval[i]->LookInsideRoad(track,nrows))
		    {
		      fTracks[0]->Remove(fTracks[0]->GetNTracks()-1);
		      fTracks[0]->Compress();
		    }
		  */
		}
	    }
	  
	}
      
    }
}

void AliL3Hough::ProcessPatchIter(Int_t patch)
{
  //Process patch in a iterative way. 
  //transform + peakfinding + evaluation + transform +...

  Int_t num_of_tries = 5;
  AliL3HoughBaseTransformer *tr = fHoughTransformer[patch];
  AliL3TrackArray *tracks = fTracks[patch];
  tracks->Reset();
  AliL3HoughEval *ev = fEval[patch];
  ev->InitTransformer(tr);
  //ev->RemoveFoundTracks();
  ev->SetNumOfRowsToMiss(3);
  ev->SetNumOfPadsToLook(2);
  AliL3Histogram *hist;
  for(Int_t t=0; t<num_of_tries; t++)
    {
      tr->Reset();
      tr->TransformCircle();
      for(Int_t i=0; i<fNEtaSegments; i++)
	{
	  hist = tr->GetHistogram(i);
	  if(hist->GetNEntries()==0) continue;
	  fPeakFinder->Reset();
	  fPeakFinder->SetHistogram(hist);
	  fPeakFinder->FindAbsMaxima();
	  //fPeakFinder->FindPeak1();
	  AliL3HoughTrack *track = (AliL3HoughTrack*)tracks->NextTrack();
	  track->SetTrackParameters(fPeakFinder->GetXPeak(0),fPeakFinder->GetYPeak(0),fPeakFinder->GetWeight(0));
	  track->SetEtaIndex(i);
	  track->SetEta(tr->GetEta(i,fCurrentSlice));
	  /*
	  Int_t nrows=0;
	  if(!ev->LookInsideRoad(track,nrows))
	    {	
	      tracks->Remove(tracks->GetNTracks()-1);
	      tracks->Compress();
	    }
	  */
	}
    }
  fTracks[0]->QSort();
  LOG(AliL3Log::kInformational,"AliL3Hough::ProcessPatch","NTracks")
    <<AliL3Log::kDec<<"Found "<<tracks->GetNTracks()<<" tracks in patch "<<patch<<ENDLOG;
}

void AliL3Hough::AddAllHistograms()
{
  //Add the histograms within one etaslice.
  //Resulting histogram are in patch=0.

  Double_t initTime,cpuTime;
  initTime = GetCpuTime();
  fBenchmark->Start("Add Histograms");
  for(Int_t i=0; i<fNEtaSegments; i++)
    {
      AliL3Histogram *hist0 = fHoughTransformer[0]->GetHistogram(i);
      for(Int_t j=1; j<fNPatches; j++)
	{
	  AliL3Histogram *hist = fHoughTransformer[j]->GetHistogram(i);
	  hist0->Add(hist);
	}
    }
  fBenchmark->Stop("Add Histograms");
  fAddHistograms = kTRUE;
  cpuTime = GetCpuTime() - initTime;
  LOG(AliL3Log::kInformational,"AliL3Hough::AddAllHistograms()","Timing")
    <<"Adding histograms in "<<cpuTime*1000<<" ms"<<ENDLOG;
}

void AliL3Hough::AddTracks()
{
  if(!fTracks[0])
    {
      cerr<<"AliL3Hough::AddTracks : No tracks"<<endl;
      return;
    }
  AliL3TrackArray *tracks = fTracks[0];
  for(Int_t i=0; i<tracks->GetNTracks(); i++)
    {
      AliL3Track *track = tracks->GetCheckedTrack(i);
      if(!track) continue;
      if(track->GetNHits()!=1) cerr<<"NHITS "<<track->GetNHits()<<endl;
      UInt_t *ids = track->GetHitNumbers();
      ids[0] = (fCurrentSlice&0x7f)<<25;
    }
  
  fGlobalTracks->AddTracks(fTracks[0],0,fCurrentSlice);
}

void AliL3Hough::FindTrackCandidates()
{
  //Look for peaks in histograms, and find the track candidates
  
  Int_t n_patches;
  if(fAddHistograms)
    n_patches = 1; //Histograms have been added.
  else
    n_patches = fNPatches;
  
  Double_t initTime,cpuTime;
  initTime = GetCpuTime();
  fBenchmark->Start("Find Maxima");
  for(Int_t i=0; i<n_patches; i++)
    {
      AliL3HoughBaseTransformer *tr = fHoughTransformer[i];
      fTracks[i]->Reset();
      
      for(Int_t j=0; j<fNEtaSegments; j++)
	{
	  AliL3Histogram *hist = tr->GetHistogram(j);
	  if(hist->GetNEntries()==0) continue;
	  fPeakFinder->Reset();
	  fPeakFinder->SetHistogram(hist);

	  fPeakFinder->SetThreshold(fPeakThreshold[i]);
	  fPeakFinder->FindAdaptedPeaks(fKappaSpread,fPeakRatio);

	  //fPeakFinder->FindMaxima(fPeakThreshold[i]); //Simple maxima finder
	  
	  for(Int_t k=0; k<fPeakFinder->GetEntries(); k++)
	    {
	      AliL3HoughTrack *track = (AliL3HoughTrack*)fTracks[i]->NextTrack();
	      track->SetTrackParameters(fPeakFinder->GetXPeak(k),fPeakFinder->GetYPeak(k),fPeakFinder->GetWeight(k));
	      track->SetEtaIndex(j);
	      track->SetEta(tr->GetEta(j,fCurrentSlice));
	      track->SetRowRange(AliL3Transform::GetFirstRow(0),AliL3Transform::GetLastRow(5));
	    }
	}
      cout<<"Found "<<fTracks[i]->GetNTracks()<<" tracks in patch "<<i<<endl;
      fTracks[i]->QSort();
    }
  fBenchmark->Stop("Find Maxima");
  cpuTime = GetCpuTime() - initTime;
  LOG(AliL3Log::kInformational,"AliL3Hough::FindTrackCandidates()","Timing")
    <<"Maxima finding done in "<<cpuTime*1000<<" ms"<<ENDLOG;
}

void AliL3Hough::InitEvaluate()
{
  //Pass the transformer objects to the AliL3HoughEval objects:
  //This will provide the evaluation objects with all the necessary
  //data and parameters it needs.
  
  for(Int_t i=0; i<fNPatches; i++) 
    fEval[i]->InitTransformer(fHoughTransformer[i]);
}

Int_t AliL3Hough::Evaluate(Int_t road_width,Int_t nrowstomiss)
{
  //Evaluate the tracks, by looking along the road in the raw data.
  //If track does not cross all padrows - rows2miss, it is removed from the arrray.
  //If histograms were not added, the check is done locally in patch,
  //meaning that nrowstomiss is the number of padrows the road can miss with respect
  //to the number of rows in the patch.
  //If the histograms were added, the comparison is done globally in the _slice_, 
  //meaing that nrowstomiss is the number of padrows the road can miss with
  //respect to the total number of padrows in the slice.
  //
  //Return value = number of tracks which were removed (only in case of fAddHistograms)
  
  if(!fTracks[0])
    {
      LOG(AliL3Log::kError,"AliL3Hough::Evaluate","Track Array")
	<<"No tracks to work with..."<<ENDLOG;
      return 0;
    }
  
  Int_t removed_tracks=0;
  AliL3TrackArray *tracks=0;

  if(fAddHistograms)
    {
      tracks = fTracks[0];
      for(Int_t i=0; i<tracks->GetNTracks(); i++)
	{
	  AliL3Track *track = tracks->GetCheckedTrack(i);
	  if(!track) continue;
	  track->SetNHits(0);
	}
    }
  
  for(Int_t i=0; i<fNPatches; i++)
    EvaluatePatch(i,road_width,nrowstomiss);
  
  //Here we check the tracks globally; 
  //how many good rows (padrows with signal) 
  //did it cross in the slice
  if(fAddHistograms) 
    {
      for(Int_t j=0; j<tracks->GetNTracks(); j++)
	{
	  AliL3HoughTrack *track = (AliL3HoughTrack*)tracks->GetCheckedTrack(j);
	  
	  if(track->GetNHits() < AliL3Transform::GetNRows() - nrowstomiss)
	    {
	      tracks->Remove(j);
	      removed_tracks++;
	    }
	}
      tracks->Compress();
      tracks->QSort();
    }
    
  return removed_tracks;
}

void AliL3Hough::EvaluatePatch(Int_t i,Int_t road_width,Int_t nrowstomiss)
{
  //Evaluate patch i.
  
  fEval[i]->InitTransformer(fHoughTransformer[i]);
  fEval[i]->SetNumOfPadsToLook(road_width);
  fEval[i]->SetNumOfRowsToMiss(nrowstomiss);
  //fEval[i]->RemoveFoundTracks();
  
  AliL3TrackArray *tracks=0;
  
  if(!fAddHistograms)
    tracks = fTracks[i];
  else
    tracks = fTracks[0];
  
  Int_t nrows=0;
  for(Int_t j=0; j<tracks->GetNTracks(); j++)
    {
      AliL3HoughTrack *track = (AliL3HoughTrack*)tracks->GetCheckedTrack(j);
      if(!track)
	{
	  LOG(AliL3Log::kWarning,"AliL3Hough::EvaluatePatch","Track array")
	    <<"Track object missing!"<<ENDLOG;
	  continue;
	} 
      nrows=0;
      Int_t rowrange[2] = {AliL3Transform::GetFirstRow(i),AliL3Transform::GetLastRow(i)};
      Bool_t result = fEval[i]->LookInsideRoad(track,nrows,rowrange);
      if(fAddHistograms)
	{
	  Int_t pre=track->GetNHits();
	  track->SetNHits(pre+nrows);
	}
      else//the track crossed too few good padrows (padrows with signal) in the patch, so remove it
	{
	  if(result == kFALSE)
	    tracks->Remove(j);
	}
    }
  
  tracks->Compress();

}

void AliL3Hough::MergeEtaSlices()
{
  //Merge tracks found in neighbouring eta slices.
  //Removes the track with the lower weight.
  
  fBenchmark->Start("Merge Eta-slices");
  AliL3TrackArray *tracks = fTracks[0];
  if(!tracks)
    {
      cerr<<"AliL3Hough::MergeEtaSlices : No tracks "<<endl;
      return;
    }
  for(Int_t j=0; j<tracks->GetNTracks(); j++)
    {
      AliL3HoughTrack *track1 = (AliL3HoughTrack*)tracks->GetCheckedTrack(j);
      if(!track1) continue;
      for(Int_t k=j+1; k<tracks->GetNTracks(); k++)
	{
	  AliL3HoughTrack *track2 = (AliL3HoughTrack*)tracks->GetCheckedTrack(k);
	  if(!track2) continue;
	  if(abs(track1->GetEtaIndex() - track2->GetEtaIndex()) != 1) continue;
	  if(fabs(track1->GetKappa()-track2->GetKappa()) < 0.006 && 
	     fabs(track1->GetPsi()- track2->GetPsi()) < 0.1)
	    {
	      //cout<<"Merging track in slices "<<track1->GetEtaIndex()<<" "<<track2->GetEtaIndex()<<endl;
	      if(track1->GetWeight() > track2->GetWeight())
		tracks->Remove(k);
	      else
		tracks->Remove(j);
	    }
	}
    }
  fBenchmark->Stop("Merge Eta-slices");
  tracks->Compress();
}

void AliL3Hough::WriteTracks(Char_t *path)
{
  //cout<<"AliL3Hough::WriteTracks : Sorting the tracsk"<<endl;
  //fGlobalTracks->QSort();
  
  Char_t filename[1024];
  sprintf(filename,"%s/tracks_%d.raw",path,fEvent);
  AliL3MemHandler mem;
  mem.SetBinaryOutput(filename);
  mem.TrackArray2Binary(fGlobalTracks);
  mem.CloseBinaryOutput();
  fGlobalTracks->Reset();
}

void AliL3Hough::WriteTracks(Int_t slice,Char_t *path)
{
  
  AliL3MemHandler mem;
  Char_t fname[100];
  if(fAddHistograms)
    {
      sprintf(fname,"%s/tracks_ho_%d_%d.raw",path,fEvent,slice);
      mem.SetBinaryOutput(fname);
      mem.TrackArray2Binary(fTracks[0]);
      mem.CloseBinaryOutput();
    }
  else 
    {
      for(Int_t i=0; i<fNPatches; i++)
	{
	  sprintf(fname,"%s/tracks_ho_%d_%d_%d.raw",path,fEvent,slice,i);
	  mem.SetBinaryOutput(fname);
	  mem.TrackArray2Binary(fTracks[i]);
	  mem.CloseBinaryOutput();
	}
    }
}

void AliL3Hough::WriteDigits(Char_t *outfile)
{
#ifdef use_aliroot  
  //Write the current data to a new rootfile.

  for(Int_t i=0; i<fNPatches; i++)
    {
      AliL3DigitRowData *tempPt = (AliL3DigitRowData*)fHoughTransformer[i]->GetDataPointer();
      fMemHandler[i]->AliDigits2RootFile(tempPt,outfile);
    }
#else
  cerr<<"AliL3Hough::WriteDigits : You need to compile with AliROOT!"<<endl;
  return;
#endif  
}

Double_t AliL3Hough::GetCpuTime()
{
  //Return the Cputime in seconds.
 struct timeval tv;
 gettimeofday( &tv, NULL );
 return tv.tv_sec+(((Double_t)tv.tv_usec)/1000000.);
}

