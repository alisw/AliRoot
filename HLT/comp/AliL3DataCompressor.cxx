// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#include "AliL3Logging.h"
#include "AliL3RootTypes.h"
#include "AliL3Transform.h"
#include "AliL3MemHandler.h"
#include "AliL3SpacePointData.h"
#include "AliL3Compress.h"
#include "AliL3TrackArray.h"
#include "AliL3ModelTrack.h"
#include "AliL3Benchmark.h"
#include "AliL3ClusterFitter.h"

#ifdef use_aliroot
#include "AliL3FileHandler.h"
#include <AliTPCcluster.h>
#include <AliTPCParamSR.h>
#include <AliTPCDigitsArray.h>
#include <AliTPCClustersArray.h>
#include <AliTPCClustersRow.h>
#include <AliSimDigits.h>
#include <AliTPC.h>
#include <AliTPCv2.h>
#include <AliRun.h>
#endif

#ifdef use_root
#include <TFile.h>
#include <TMath.h>
#include <TDirectory.h>
#include <TSystem.h>
#include <TH2F.h>
#endif

#include "AliL3DataCompressorHelper.h"
#include "AliL3DataCompressor.h"

#if GCCVERSION == 3
using namespace std;
#endif

//_____________________________________________________________
//
//  AliL3DataCompression
//
// Interface class; binary <-> AliROOT handling of TPC data compression classes.
//


ClassImp(AliL3DataCompressor)

AliL3DataCompressor::AliL3DataCompressor()
{
  fBenchmark=0;
  fInputTracks=0;
  fKeepRemaining=kTRUE;
  fEvent=0;
  fWriteClusterShape=kFALSE;
  fOutputFile=0;
  fCompRatioFile=0;
  fNusedClusters=0;
  fNunusedClusters=0;
  memset(fClusters,0,36*6*sizeof(AliL3SpacePointData*));
}

AliL3DataCompressor::AliL3DataCompressor(Char_t *path,Bool_t keep,Bool_t writeshape)
{
  strcpy(fPath,path);
  fBenchmark = new AliL3Benchmark();
  fInputTracks=0;
  fKeepRemaining=keep;
  fWriteClusterShape = writeshape;
  fEvent=0;
  fOutputFile=0;
  fNusedClusters=0;
  fNunusedClusters=0;
  memset(fClusters,0,36*6*sizeof(AliL3SpacePointData*));
#ifdef use_root
  Char_t name[1024];
  sprintf(name,"rm -f %s/comp/*",path);//Clean the directory
  gSystem->Exec(name);
#endif
  OpenOutputFile();
}

AliL3DataCompressor::~AliL3DataCompressor()
{
  if(fInputTracks)
    delete fInputTracks;
  if(fBenchmark)
    delete fBenchmark;
  if(fClusters)
    {
      for(Int_t i=0; i<36; i++)
	for(Int_t j=0; j<6; j++)
	  if(fClusters[i][j])
	    delete fClusters[i][j];
    }
  CloseOutputFile();
}

void AliL3DataCompressor::DoBench(Char_t *fname)
{
  fBenchmark->Analyze(fname);
}

void AliL3DataCompressor::OpenOutputFile()
{
#ifndef use_aliroot
   LOG(AliL3Log::kError,"AliL3DataCompressor::OpenOutputFile","Version")
     <<"You have to compile with use_aliroot flag in order to use this function"<<ENDLOG;
#else
  Char_t filename[1024];
  
  sprintf(filename,"%s/comp/comprates.txt",fPath);
  fCompRatioFile = new ofstream(filename);
  
  if(fOutputFile)
    if(fOutputFile->IsOpen())
      fOutputFile->Close();

  sprintf(filename,"%s/alirunfile.root",fPath);
  TFile *f = TFile::Open(filename);
  AliTPCParam *param = (AliTPCParam*)f->Get(AliL3Transform::GetParamName());
  sprintf(filename,"%s/comp/AliTPCclusters.root",fPath);
  fOutputFile = TFile::Open(filename,"RECREATE");
  param->Write(param->GetTitle());
  f->Close();
#endif
}

void AliL3DataCompressor::CloseOutputFile()
{
  if(fCompRatioFile)
    {
      fCompRatioFile->close();
      delete fCompRatioFile;
    }
  
  if(!fOutputFile)
    return;
#ifdef use_root
  if(!fOutputFile->IsOpen())
    return;
  fOutputFile->Close();
#else
  fclose(fOutputFile);
#endif
  fOutputFile=0;
}

void AliL3DataCompressor::LoadData(Int_t event,Bool_t sp)
{
  fSinglePatch=sp;
  fEvent=event;
  AliL3MemHandler *clusterfile[36][6];
  Char_t fname[1024];
  for(Int_t s=0; s<=35; s++)
    {
      for(Int_t p=0; p<6; p++)
	{
	  if(fClusters[s][p])
	    delete fClusters[s][p];
	  fClusters[s][p] = 0;
	  clusterfile[s][p] = new AliL3MemHandler();
	  if(fSinglePatch)
	    sprintf(fname,"%s/cf/points_%d_%d_%d.raw",fPath,fEvent,s,-1);
	  else
	    sprintf(fname,"%s/cf/points_%d_%d_%d.raw",fPath,fEvent,s,p);
	  clusterfile[s][p]->SetBinaryInput(fname);
	  
	  fClusters[s][p] = (AliL3SpacePointData*)clusterfile[s][p]->Allocate();
	  clusterfile[s][p]->Binary2Memory(fNcl[s][p],fClusters[s][p]);
	  clusterfile[s][p]->CloseBinaryInput();
	  
	  if(fSinglePatch)
	    break;
	}
    }
  
  sprintf(fname,"%s/cf/tracks_%d.raw",fPath,fEvent);
  AliL3MemHandler *tfile = new AliL3MemHandler();
  tfile->SetBinaryInput(fname);
  
  if(fInputTracks)
    delete fInputTracks;
  fInputTracks = new AliL3TrackArray();
  tfile->Binary2TrackArray(fInputTracks);
  tfile->CloseBinaryInput();
  delete tfile;
}

void AliL3DataCompressor::FillData(Int_t min_hits,Bool_t expand)
{
  
  //Fill the track data into track and cluster structures, and write to file.
  //Preparation for compressing it.
  
  cout<<"Filling data; "<<fInputTracks->GetNTracks()<<" tracks"<<endl;
  AliL3TrackArray *comptracks = new AliL3TrackArray("AliL3ModelTrack");
  fInputTracks->QSort();
  for(Int_t i=0; i<fInputTracks->GetNTracks(); i++)
    {
      AliL3Track *intrack = fInputTracks->GetCheckedTrack(i);
      if(!intrack) continue;

      if(intrack->GetNHits()<min_hits) break;
      if(intrack->GetPt()<0.1) continue;
      
      intrack->CalculateHelix();
      
      AliL3ModelTrack *outtrack = (AliL3ModelTrack*)comptracks->NextTrack();
      outtrack->SetNHits(intrack->GetNHits());
      outtrack->SetRowRange(intrack->GetFirstRow(),intrack->GetLastRow());
      outtrack->SetFirstPoint(intrack->GetFirstPointX(),intrack->GetFirstPointY(),intrack->GetFirstPointZ());
      outtrack->SetLastPoint(intrack->GetLastPointX(),intrack->GetLastPointY(),intrack->GetLastPointZ());
      outtrack->SetPt(intrack->GetPt());
      outtrack->SetPsi(intrack->GetPsi());
      outtrack->SetTgl(intrack->GetTgl());
      outtrack->SetCharge(intrack->GetCharge());
      outtrack->CalculateHelix();
      Int_t nhits = intrack->GetNHits();
      UInt_t *hitids = intrack->GetHitNumbers();
      Int_t origslice = (hitids[nhits-1]>>25)&0x7f;
      outtrack->Init(origslice,-1);
      
      for(Int_t j=nhits-1; j>=0; j--)
	{
	  UInt_t id=hitids[j];
	  Int_t slice = (id>>25)&0x7f;
	  Int_t patch = (id>>22)&0x7;
	  UInt_t pos = id&0x3fffff;	     

	  //UInt_t size;
	  AliL3SpacePointData *points = fClusters[slice][patch];//->GetDataPointer(size);
	  Float_t xyz[3] = {points[pos].fX,points[pos].fY,points[pos].fZ};
	  Int_t padrow = points[pos].fPadRow;

	  //Calculate the crossing point between track and padrow
	  Float_t angle = 0; //Perpendicular to padrow in local coordinates
	  AliL3Transform::Local2GlobalAngle(&angle,slice);
	  if(!intrack->CalculateReferencePoint(angle,AliL3Transform::Row2X(padrow)))
	    {
	      cerr<<"AliL3DataCompressor::FillData : Error in crossing point calc on slice "<<slice<<" row "<<padrow<<endl;
	      break;
	      //outtrack->Print(kFALSE);
	      //exit(5);
	    }
	  
	  Float_t xyz_cross[3] = {intrack->GetPointX(),intrack->GetPointY(),intrack->GetPointZ()};

	  Int_t sector,row;
	  AliL3Transform::Slice2Sector(slice,padrow,sector,row);
	  AliL3Transform::Global2Raw(xyz_cross,sector,row);
	  AliL3Transform::Global2Raw(xyz,sector,row);
	  
	  outtrack->SetPadHit(padrow,xyz_cross[1]);
	  outtrack->SetTimeHit(padrow,xyz_cross[2]);

	  outtrack->SetCrossingAngleLUT(padrow,intrack->GetCrossingAngle(padrow,slice));
	  outtrack->CalculateClusterWidths(padrow,kTRUE);

	  if(fWriteClusterShape)
	    {
	      Int_t patch = AliL3Transform::GetPatch(padrow);
	      Float_t sigmaY2 = points[pos].fSigmaY2 / pow(AliL3Transform::GetPadPitchWidth(patch),2);
	      Float_t sigmaZ2 = points[pos].fSigmaZ2 / pow(AliL3Transform::GetZWidth(),2);
	      outtrack->SetCluster(padrow,xyz[1],xyz[2],points[pos].fCharge,sigmaY2,sigmaZ2,3);
	    }
	  else
	    outtrack->SetCluster(padrow,xyz[1],xyz[2],points[pos].fCharge,0,0,3);
	  
	  //IMPORTANT: Set the slice in which cluster is, you need it in AliL3ModelTrack::FillTrack!
	  outtrack->GetClusterModel(padrow)->fSlice=slice;
	  points[pos].fCharge = 0;//Mark this cluster as used.
	  fNusedClusters++;
	}
      if(!expand)
	outtrack->SetNClusters(AliL3Transform::GetNRows(-1));
    }
  if(expand)
    ExpandTrackData(comptracks);
  
  cout<<"Writing "<<comptracks->GetNTracks()<<" tracks to file"<<endl;
  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  comp->WriteFile(comptracks);
  delete comp;
  delete comptracks;
  
}

void AliL3DataCompressor::ExpandTrackData(AliL3TrackArray *tracks)
{
  //Loop over tracks and try to assign unused clusters.
  //Only clusters which are closer than the max. residual are taken.
  
  cout<<"Expanding "<<tracks->GetNTracks()<<" tracks"<<endl;
  for(Int_t i=0; i<tracks->GetNTracks(); i++)
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)tracks->GetCheckedTrack(i);
      if(!track) continue;
      if(track->GetNHits() == AliL3Transform::GetNRows()) continue;
      
      Int_t nhits = track->GetNHits();
      //cout<<"Expanding track with "<<nhits<<" clusters"<<endl;
      
      Int_t last_slice=-1;
      for(Int_t padrow=AliL3Transform::GetNRows()-1; padrow>=0; padrow--)
	{
	  if(track->IsPresent(padrow))
	    {
	      last_slice = track->GetClusterModel(padrow)->fSlice;
	      continue;
	    }
	  
	  if(last_slice < 0) //the outer cluster is missing, so skip it - it will be written anyhow.
	    continue;
	  
	  //Check the slice of the next padrow:
	  Int_t next_padrow = padrow-1;
	  Int_t next_slice = -1;
	  while(next_padrow >=0)
	    {
	      if(track->IsPresent(next_padrow))
		{
		  next_slice = track->GetClusterModel(next_padrow)->fSlice;
		  break;
		}
	      next_padrow--;
	    }
	  if(next_slice>=0)
	    if(next_slice != last_slice)//The track crosses a slice boundary here
	      continue;
	  
 	  //UInt_t size;
	  AliL3SpacePointData *points = fClusters[last_slice][0];//->GetDataPointer(size);
	  
	  Float_t angle = 0;
	  AliL3Transform::Local2GlobalAngle(&angle,last_slice);
	  if(!track->CalculateReferencePoint(angle,AliL3Transform::Row2X(padrow)))
	    continue;
	  Float_t xyz_cross[3] = {track->GetPointX(),track->GetPointY(),track->GetPointZ()};
	  AliL3Transform::Global2Local(xyz_cross,last_slice,kTRUE);
	  Float_t mindist = 123456789;
	  AliL3SpacePointData *closest=0;
	  for(UInt_t j=0; j<fNcl[last_slice][0]; j++)
	    {
	      if(points[j].fCharge == 0) continue;// || points[j].fPadRow != padrow) continue;
	      if(points[j].fPadRow < padrow) continue;
	      if(points[j].fPadRow > padrow) break;
	      Float_t xyz[3] = {points[j].fX,points[j].fY,points[j].fZ};
	      AliL3Transform::Global2Local(xyz,last_slice,kTRUE);
	      
	      //Check for overflow:
	      Int_t temp = (Int_t)rint((xyz_cross[1]-xyz[1])/AliL3DataCompressorHelper::GetXYResidualStep(padrow));
	      if( abs(temp) > 1<<(AliL3DataCompressorHelper::GetNPadBits()-1))
		continue;
	      
	      temp = (Int_t)rint((xyz_cross[2]-xyz[2])/AliL3DataCompressorHelper::GetZResidualStep(padrow));
	      if( abs(temp) > 1<<(AliL3DataCompressorHelper::GetNTimeBits()-1))
		continue;
	      
	      Float_t dist = sqrt( pow(xyz_cross[1]-xyz[1],2) + pow(xyz_cross[2]-xyz[2],2) );
	      if(dist < mindist)
		{
		  closest = &points[j];
		  mindist = dist;
		}
	    }
	  if(closest) //there was a cluster assigned
	    {
	      Int_t sector,row;
	      Float_t xyz[3] = {closest->fX,closest->fY,closest->fZ};
	      AliL3Transform::Slice2Sector(last_slice,padrow,sector,row);
	      AliL3Transform::Local2Raw(xyz_cross,sector,row);
	      AliL3Transform::Global2Raw(xyz,sector,row);
	      
	      track->SetPadHit(padrow,xyz_cross[1]);
	      track->SetTimeHit(padrow,xyz_cross[2]);
	      
	      if(fWriteClusterShape)
		{
		  Float_t angle = track->GetCrossingAngle(padrow,last_slice);
		  track->SetCrossingAngleLUT(padrow,angle);
		  track->CalculateClusterWidths(padrow,kTRUE);
		  Int_t patch = AliL3Transform::GetPatch(padrow);
		  Float_t sigmaY2 = closest->fSigmaY2 / pow(AliL3Transform::GetPadPitchWidth(patch),2);
		  Float_t sigmaZ2 = closest->fSigmaZ2 / pow(AliL3Transform::GetZWidth(),2);
		  track->SetCluster(padrow,xyz[1],xyz[2],closest->fCharge,sigmaY2,sigmaZ2,3);
		}
	      else
		track->SetCluster(padrow,xyz[1],xyz[2],closest->fCharge,0,0,3);
	      nhits++;
	      
	      //IMPORTANT: Set the slice in which cluster is, you need it in AliL3ModelTrack::FillTrack!
	      track->GetClusterModel(padrow)->fSlice=last_slice;
	      closest->fCharge = 0;//Mark this cluster as used.
	    }
	}
      track->SetNClusters(AliL3Transform::GetNRows());
      //cout<<"Track was assigned "<<nhits<<" clusters"<<endl;
    }
  
}

void AliL3DataCompressor::DetermineMinBits()
{
  //Make a pass through the modelled data (after FillData has been done) to determine
  //how many bits is needed to encode the residuals _without_ overflows.
  
  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  comp->ReadFile('m');
  AliL3TrackArray *tracks = comp->GetTracks();
  if(tracks->GetNTracks()==0)
    {
      delete comp;
      return;
    }
  
  Int_t maxtime=0,maxpad=0,maxsigma=0,maxcharge=0;
  Int_t dpad,dtime,charge,dsigmaY,dsigmaZ,npadbits,ntimebits,nchargebits,nshapebits=0;
  for(Int_t i=0; i<tracks->GetNTracks(); i++)
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)tracks->GetCheckedTrack(i);
      if(!track) continue;
      for(Int_t padrow=0; padrow<AliL3Transform::GetNRows(); padrow++)
	{
	  if(!track->IsPresent(padrow)) continue;
	  dpad = TMath::Abs(TMath::Nint(track->GetClusterModel(padrow)->fDPad));
	  dtime = TMath::Abs(TMath::Nint(track->GetClusterModel(padrow)->fDTime));
	  charge = TMath::Abs((Int_t)track->GetClusterModel(padrow)->fDCharge);
	  dsigmaY = TMath::Abs(TMath::Nint(track->GetClusterModel(padrow)->fDSigmaY));
	  dsigmaZ = TMath::Abs(TMath::Nint(track->GetClusterModel(padrow)->fDSigmaZ));
	  if(dpad > maxpad)
	    maxpad=dpad;
	  if(dtime > maxtime)
	    maxtime=dtime;
	  if(charge > maxcharge)
	    maxcharge=charge;
	  if(dsigmaY > maxsigma)
	    maxsigma=dsigmaY;
	  if(dsigmaZ > maxsigma)
	    maxsigma=dsigmaZ;
	}
    }
  cout<<"maxpad "<<maxpad<<" maxtime "<<maxtime<<" maxcharge "<<maxcharge<<endl;
  npadbits = (Int_t)TMath::Ceil(TMath::Log(maxpad)/TMath::Log(2)) + 1; //need 1 extra bit to encode the sign
  ntimebits = (Int_t)TMath::Ceil(TMath::Log(maxtime)/TMath::Log(2)) + 1;
  nchargebits = (Int_t)TMath::Ceil(TMath::Log(maxcharge)/TMath::Log(2)); //Store as a absolute value
  if(fWriteClusterShape)
    nshapebits = (Int_t)TMath::Ceil(TMath::Log(maxsigma)/TMath::Log(2)) + 1;
  
  cout<<"Updating bitnumbers; pad "<<npadbits<<" time "<<ntimebits<<" charge "<<nchargebits<<" shape "<<nshapebits<<endl;
  AliL3DataCompressorHelper::SetBitNumbers(npadbits,ntimebits,nchargebits,nshapebits);
}

void AliL3DataCompressor::WriteRemaining(Bool_t select)
{
  //Write remaining clusters (not assigned to any tracks) to file

  
  if(!fKeepRemaining)
    return;
  
  if(select)
    SelectRemainingClusters();
  
  if(!fSinglePatch)
    {
      cerr<<"AliL3Compressor::WriteRemaining : You have to modify this function when not running singlepatch"<<endl;
      return;
    }
  cout<<"Writing remaining clusters "<<endl;
  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  comp->CompressRemaining(fClusters,fNcl);
  delete comp;
  return;
}

void AliL3DataCompressor::SelectRemainingClusters()
{
  //Select which remaining clusters to write in addition to the compressed data.
  //In particular one can here make sure that "important" clusters are not missed:
  //The offline track finder perform seed finding in the outer padrows;
  //the first seeding is using pair of points on outermost padrow and 
  //0.125*nrows more rows towards the vertex. The second seeding uses pair
  //of points on the outermost padrow-0.5*0.125*nrows and 0.125*nrows + 0.5*0.125*nrows
  //more rows towards the vertex. In order to evaluate the seeds, the track offline
  //track finder checks whether a certain amount of possible clusters (padrows) is 
  //attached to the track, and then the kalman filtering starts.
  //To ensure a minimal loss off efficiency, all clusters in this region should be
  //intact.....
  
  cout<<"Cleaning up clusters"<<endl;
  Int_t nrows = AliL3Transform::GetNRows();
  Int_t gap=(Int_t)(0.125*nrows), shift=(Int_t)(0.5*gap);
  
  for(Int_t slice=0; slice<36; slice++)
    {
      AliL3SpacePointData *points = fClusters[slice][0];
      for(UInt_t i=0; i<fNcl[slice][0]; i++)
	{
	  if(points[i].fCharge == 0) continue; //Already removed
	  Int_t padrow = (Int_t)points[i].fPadRow;
	  
	  //Check the widths (errors) of the cluster, and remove big bastards:
	  Float_t padw = sqrt(points[i].fSigmaY2) / AliL3Transform::GetPadPitchWidth(AliL3Transform::GetPatch(padrow));
	  Float_t timew = sqrt(points[i].fSigmaZ2) / AliL3Transform::GetZWidth();
	  if(padw >= 2.55 || timew >= 2.55)//Because we use 1 byte to store
	    {
	      points[i].fCharge = 0;
	      continue;
	    }

	  Float_t xyz[3] = {points[i].fX,points[i].fY,points[i].fZ};
	  Int_t sector,row;
	  AliL3Transform::Slice2Sector(slice,padrow,sector,row);
	  AliL3Transform::Global2Raw(xyz,sector,row);
	  
	  if(padrow >= nrows-1-gap-shift) continue;//save all the clusters in this region
	  
	  //if(padrow >= nrows-1-shift) continue;

	  //Save the clusters at the borders:
	  //if(xyz[1] < 3 || xyz[1] >= AliL3Transform::GetNPads(padrow)-4)
	  // continue;

	  //Save clusters on padrows used for offline seeding:
	  if(padrow == nrows - 1 || padrow == nrows - 1 - gap ||                 //First seeding
	     padrow == nrows - 1 - shift || padrow == nrows - 1 - gap - shift)   //Second seeding
	    continue;
	  
	  //Cluster did not meet any of the above criteria, so disregard it:
	  points[i].fCharge = 0;
	}
    }
  
}

void AliL3DataCompressor::CompressAndExpand()
{
  //Read tracks/clusters from file, compress data and uncompress it. Write compression rates to file.
  cout<<"Compressing and expanding data"<<endl;
  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  comp->CompressFile();
  comp->ExpandFile();
  comp->PrintCompRatio(fCompRatioFile);
  delete comp;
  
  ofstream &out = *fCompRatioFile;
  out<<AliL3DataCompressorHelper::GetNPadBits()<<' '<<AliL3DataCompressorHelper::GetNTimeBits()<<' '
     <<AliL3DataCompressorHelper::GetNChargeBits()<<' '<<AliL3DataCompressorHelper::GetNShapeBits()<<endl;
  /*
  //Write the ratio between used and unused clusters to comp file:
  out<<fNusedClusters<<' '<<fNunusedClusters<<endl;
  */
}


void AliL3DataCompressor::RestoreData(Bool_t remaining_only)
{
  //Restore the uncompressed data together with the remaining clusters,
  //and write to a final cluster file which serves as an input to the
  //final offline tracker.
  
#ifndef use_aliroot
   LOG(AliL3Log::kError,"AliL3DataCompressor::RestoreData","Version")
     <<"You have to compile with use_aliroot flag in order to use this function"<<ENDLOG;
#else

  cout<<"Restoring data"<<endl;
  
  const Int_t maxpoints=500000;
  TempCluster **clusters = new TempCluster*[36];
  Int_t *ncl = new Int_t[36];
  for(Int_t i=0; i<36; i++)
    {
      ncl[i]=0;
      clusters[i] = new TempCluster[maxpoints];
    }
  
  if(!remaining_only)
    ReadUncompressedData(clusters,ncl,maxpoints);
  
  if(fKeepRemaining)
    ReadRemaining(clusters,ncl,maxpoints);
  
  Char_t filename[1024];
  sprintf(filename,"%s/digitfile.root",fPath);
  TFile *rootfile = TFile::Open(filename);
  rootfile->cd();
  AliTPCParam *param = (AliTPCParam*)rootfile->Get(AliL3Transform::GetParamName());

  AliTPCDigitsArray *darray = new AliTPCDigitsArray();
  darray->Setup(param);
  darray->SetClass("AliSimDigits");
  sprintf(filename,"TreeD_%s_%d",AliL3Transform::GetParamName(),fEvent);
  Bool_t ok = darray->ConnectTree(filename);
  if(!ok)
    {
      cerr<<"AliL3DataCompressor::RestoreData : Problems connecting tree"<<endl;
      return;
    }

  fOutputFile->cd();
    
  AliTPCClustersArray *carray = new AliTPCClustersArray();
  carray->Setup(param);
  carray->SetClusterType("AliTPCcluster");
  carray->MakeTree();
  
  Int_t totcounter=0;
  for(Int_t slice=0; slice<=35; slice++)
    {
      TempCluster **clPt = new TempCluster*[maxpoints];
      cout<<"Sorting "<<ncl[slice]<<" clusters in slice "<<slice<<endl;
      for(Int_t i=0; i<ncl[slice]; i++)
	clPt[i] = &clusters[slice][i];
      
      QSort(clPt,0,ncl[slice]);
      
      //cout<<"padrow "<<clPt[i]->padrow<<" pad "<<clPt[i]->pad<<" time "<<clPt[i]->time<<endl;

      Int_t falseid=0;
      Int_t counter=0;
      for(Int_t padrow=AliL3Transform::GetFirstRow(-1); padrow<=AliL3Transform::GetLastRow(-1); padrow++)
	{
	  Int_t sec,row;
	  AliL3Transform::Slice2Sector(slice,padrow,sec,row);
	  AliTPCClustersRow *clrow=carray->CreateRow(sec,row);
	  AliSimDigits *digits = (AliSimDigits*)darray->LoadRow(sec,row);
	  digits->ExpandBuffer();
	  digits->ExpandTrackBuffer();
	  Int_t patch = AliL3Transform::GetPatch(padrow);
	  while(counter < ncl[slice] && clPt[counter]->padrow == padrow)
	    {
	      Float_t temp[3];
	      AliL3Transform::Raw2Local(temp,sec,row,clPt[counter]->pad,clPt[counter]->time);
	      
	      AliTPCcluster *c = new AliTPCcluster();
	      c->SetY(temp[1]);
	      c->SetZ(temp[2]);
	      c->SetQ(clPt[counter]->charge);
	      
	      c->SetSigmaY2(clPt[counter]->sigmaY2*pow(AliL3Transform::GetPadPitchWidth(patch),2));
	      c->SetSigmaZ2(clPt[counter]->sigmaZ2*pow(AliL3Transform::GetZWidth(),2));
	      Int_t pad = TMath::Nint(clPt[counter]->pad);
	      Int_t time = TMath::Nint(clPt[counter]->time);
	      
	      if(pad < 0)
		pad=0;
	      if(pad >= AliL3Transform::GetNPads(padrow))
		pad = AliL3Transform::GetNPads(padrow)-1;
	      if(time < 0 || time >= AliL3Transform::GetNTimeBins())
		cerr<<"row "<<padrow<<" pad "<<pad<<" time "<<time<<endl;
	      
	      for(Int_t lab=0; lab<3; lab++)
		{
		  Int_t label = digits->GetTrackIDFast(time,pad,lab);
		  if(label > 1)
		    c->SetLabel(label-2,lab);
		  else if(label==0)
		    c->SetLabel(-2,lab);
		  else
		    c->SetLabel(-1,lab);
		  if(lab==0 && c->GetLabel(0) < 0)
		    {
		      falseid++;
		      //AliL3Transform::Local2Global(temp,slice);
		      //cout<<"slice "<<slice<<" padrow "<<padrow<<" y "<<temp[1]<<" z "<<temp[2]<<" label "<<c->GetLabel(0)<<endl;
		    }
		}
	      //cout<<"row "<<padrow<<" pad "<<clPt[counter]->pad<<" time "<<clPt[counter]->time<<" sigmaY2 "<<c->GetSigmaY2()<<" sigmaZ2 "<<c->GetSigmaZ2()<<endl;
	      clrow->InsertCluster(c);
	      delete c;
	      counter++;
	      totcounter++;
	    }
	  carray->StoreRow(sec,row);
	  carray->ClearRow(sec,row);
	  darray->ClearRow(sec,row);
	}
      //cerr<<"Slice "<<slice<<" nclusters "<<counter<<" falseones "<<falseid<<endl;
      if(counter != ncl[slice])
	cerr<<"AliLDataCompressor::RestoreData : Mismatching cluster count :"<<counter<<" "<<ncl[slice]<<endl;
      delete [] clPt;
    }

  cout<<"Writing "<<totcounter<<" clusters to rootfile "<<endl;

  sprintf(filename,"TreeC_TPC_%d",fEvent);
  carray->GetTree()->SetName(filename);
  carray->GetTree()->Write();
  delete carray;
  delete darray;
  rootfile->Close();
  
  for(Int_t i=0; i<36; i++)
    delete [] clusters[i];
  delete [] clusters;
  delete [] ncl;
#endif
}

void AliL3DataCompressor::ReadUncompressedData(TempCluster **clusters,Int_t *ncl,const Int_t maxpoints)
{

  cout<<"Reading uncompressed tracks "<<endl;
  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  
  if(!comp->ReadFile('u'))
    return;
  
  AliL3TrackArray *tracks = comp->GetTracks();
  
  Int_t charge;
  Float_t pad,time,sigmaY2,sigmaZ2;
  for(Int_t i=0; i<tracks->GetNTracks(); i++)
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)tracks->GetCheckedTrack(i);
      if(!track) continue;
      for(Int_t padrow=0; padrow < AliL3Transform::GetNRows(-1); padrow++)
	{
	  if(!track->IsPresent(padrow)) continue;
	  track->GetPad(padrow,pad);
	  track->GetTime(padrow,time);
	  track->GetClusterCharge(padrow,charge);
	  track->GetSigmaY2(padrow,sigmaY2);
	  track->GetSigmaZ2(padrow,sigmaZ2);
	  Int_t slice = track->GetClusterModel(padrow)->fSlice;
	  /*
	    if(pad < -1 || pad > AliL3Transform::GetNPads(padrow) || time < -1 || time > AliL3Transform::GetNTimeBins())
	    {
	    cerr<<"AliL3DataCompressor::ReadUncompressData : Wrong pad "<<pad<<" or time "<<time<<" on row "<<padrow<<" track index "<<i<<endl;
	    track->Print();
	    exit(5);
	    }
	  */
	  if(ncl[slice] >= maxpoints)
	    {
	      cerr<<"AliL3DataCompressor::ReadUncompressedData : Too many clusters"<<endl;
	      exit(5);
	    }
	  clusters[slice][ncl[slice]].pad = pad;
	  clusters[slice][ncl[slice]].time = time;
	  clusters[slice][ncl[slice]].charge = charge;
	  clusters[slice][ncl[slice]].sigmaY2 = sigmaY2;
	  clusters[slice][ncl[slice]].sigmaZ2 = sigmaZ2;
	  clusters[slice][ncl[slice]].padrow = padrow;
	  //cout<<"row "<<padrow<<" pad "<<pad<<" time "<<time<<" charge "<<charge<<" sigmas "<<sigmaY2<<" "<<sigmaZ2<<endl;
	  ncl[slice]++;
	}
    }

  delete comp;
}

void AliL3DataCompressor::ReadRemaining(TempCluster **clusters,Int_t *ncl,const Int_t maxpoints)
{
  
  cout<<"Reading remaining clusters "<<endl;

  AliL3Compress *comp = new AliL3Compress(-1,-1,fPath,fWriteClusterShape,fEvent);
  comp->ExpandRemaining(clusters,ncl,maxpoints);
  delete comp;
  return;
}

void AliL3DataCompressor::QSort(TempCluster **a, Int_t first, Int_t last)
{
  static TempCluster *tmp;
   static int i;           // "static" to save stack space
   int j;

   while (last - first > 1) {
      i = first;
      j = last;
      for (;;) {
	while (++i < last && Compare(a[i], a[first]) < 0)
	  ;
	while (--j > first && Compare(a[j], a[first]) > 0)
	  ;
         if (i >= j)
            break;

         tmp  = a[i];
         a[i] = a[j];
         a[j] = tmp;
      }
      if (j == first) {
         ++first;
         continue;
      }
      tmp = a[first];
      a[first] = a[j];
      a[j] = tmp;
      if (j - first < last - (j + 1)) {
         QSort(a, first, j);
         first = j + 1;   // QSort(j + 1, last);
      } else {
         QSort(a, j + 1, last);
         last = j;        // QSort(first, j);
      }
   }
}

Int_t AliL3DataCompressor::Compare(TempCluster *a,TempCluster *b)
{
  if(a->padrow < b->padrow) return -1;
  if(a->padrow > b->padrow) return 1;

  if(rint(a->pad) == rint(b->pad) && rint(a->time) == rint(b->time)) return 0;
  
  if(rint(a->pad) < rint(b->pad)) return -1;
  if(rint(a->pad) == rint(b->pad) && rint(a->time) < rint(b->time)) return -1;
  
  return 1;
}

