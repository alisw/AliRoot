// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo$fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#include "bitio.h"
#include "AliL3RootTypes.h"
#include "AliL3Models.h"
#include "AliL3DigitData.h"
#include "AliL3Logging.h"
#include "AliL3TrackArray.h"
#include "AliL3ModelTrack.h"
#include "AliL3Transform.h"
#include "AliL3MemHandler.h"
#include "AliL3DataCompressorHelper.h"
#include "AliL3DataCompressor.h"
#include "AliL3SpacePointData.h"

#if 0
#ifdef use_root
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#endif
#ifdef use_aliroot
#include "AliL3FileHandler.h"
#endif
#endif

#include "AliL3Compress.h"

#if GCCVERSION == 3
using namespace std;
#endif

//_____________________________________________________________
//
//  AliL3Compress
//
// Class for compressing and uncompressing data.

ClassImp(AliL3Compress)

AliL3Compress::AliL3Compress()
{
  fTracks=0;
  fSlice =0;
  fPatch=0;
  fWriteShape=kFALSE;
  fEvent=-1;
}

AliL3Compress::AliL3Compress(Int_t slice,Int_t patch,Char_t *path,Bool_t writeshape,Int_t event)
{
  fEvent=event;
  fSlice=slice;
  fPatch=patch;
  fTracks=0;
  sprintf(fPath,"%s",path);
  fWriteShape=writeshape;
}

AliL3Compress::~AliL3Compress()
{
  if(fTracks)
    delete fTracks;
}

Bool_t AliL3Compress::WriteFile(AliL3TrackArray *tracks,Char_t *filename)
{
  Char_t fname[1024];
  if(filename)
    sprintf(fname,"%s/comp/%s",fPath,filename);
  else if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_m_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_m_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);

  FILE *file = fopen(fname,"w");
  if(!file)
    {
      cerr<<"AliL3Compress::WriteFile : Error opening file "<<fname<<endl;
      return kFALSE;
    }
  Short_t ntracks = tracks->GetNTracks();
    
  Int_t count=0;
  AliL3ClusterModel *clusters=0;
  AliL3TrackModel *model=0;
  for(Int_t i=0; i<ntracks; i++)
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)tracks->GetCheckedTrack(i);
      if(!track) continue;

      //Do not save useless tracks or clusters:
      //if(track->GetNPresentClusters() == 0)
      //continue;
      
      track->FillModel();
      model = track->GetModel();
      //if(model->fNClusters==0) continue;
      clusters = track->GetClusters();
      if(fwrite(model,sizeof(AliL3TrackModel),1,file)!=1) break;
      //if(fwrite(clusters,model->fNClusters*sizeof(AliL3ClusterModel),1,file)!=1) break;
      if(fwrite(clusters,AliL3Transform::GetNRows(fPatch)*sizeof(AliL3ClusterModel),1,file)!=1) break;
      count++;
      
    }
  fclose(file);
  return kTRUE;
}

Bool_t AliL3Compress::ReadFile(Char_t which,Char_t *filename)
{
  //Read the trackfile.

  Char_t fname[1024];
  if(filename)
    sprintf(fname,"%s/comp/%s",fPath,filename);
  else
    {
      if(which == 'm')
	{
	  if(fEvent<0)
	    sprintf(fname,"%s/comp/tracks_m_%d_%d.raw",fPath,fSlice,fPatch);
	  else
	    sprintf(fname,"%s/comp/tracks_m_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
	}
      else if(which == 'u')
	{
	  if(fEvent<0)
	    sprintf(fname,"%s/comp/tracks_u_%d_%d.raw",fPath,fSlice,fPatch);
	  else
	    sprintf(fname,"%s/comp/tracks_u_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
	}
      else
	{
	  cerr<<"AliL3Compress::ReadFile() : Wrong option"<<endl;
	  return kFALSE;
	}
    }

  FILE *file = fopen(fname,"r");
  if(!file)
    {
      cerr<<"AliL3Compress::ReadFile : Cannot open file "<<fname<<endl;
      return kFALSE;
    }

  if(fTracks)
    delete fTracks;
  fTracks = new AliL3TrackArray("AliL3ModelTrack");
  
  while(!feof(file))
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)fTracks->NextTrack();
      track->Init(fSlice,fPatch);
      AliL3TrackModel *model = track->GetModel();
      AliL3ClusterModel *clusters = track->GetClusters();
      if(fread(model,sizeof(AliL3TrackModel),1,file)!=1) break;
      //if(fread(clusters,model->fNClusters*sizeof(AliL3ClusterModel),1,file)!=1) break;
      if(fread(clusters,AliL3Transform::GetNRows(fPatch)*sizeof(AliL3ClusterModel),1,file)!=1) break;
      track->FillTrack();
    }

  fTracks->RemoveLast();
  fclose(file);
  return kTRUE;
}

Bool_t AliL3Compress::CompressFile()
{
  Char_t fname[100];
  if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_c_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_c_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
  BIT_FILE *output = OpenOutputBitFile(fname);
  
  if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_m_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_m_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
  
  FILE *input = fopen(fname,"r");
  if(!input)
    {
      cerr<<"AliL3Compress::CompressFile() : Error opening file: "<<fname<<endl;
      return kFALSE;
    }

  AliL3TrackModel track;
  AliL3ClusterModel cluster;
  Int_t temp;
  Int_t power;
  
  Int_t timeo,pado,chargeo,padshapeo,timeshapeo;
  timeo=pado=chargeo=padshapeo=timeshapeo=0;
  while(!feof(input))
    {
      if(fread(&track,sizeof(AliL3TrackModel),1,input)!=1) break;
      
      if(output->mask != 0x80) //Write the current byte to file.
	{
	  //cerr<<"\nAliL3Compress::CompressFile() : Writing overhead bits!!!"<<endl;
	  if(putc(output->rack,output->file )!=output->rack)
	    cerr<<"AliL3Compress::ComressFile : Error writing to bitfile"<<endl;
	  output->mask=0x80;
	  output->rack=0;
	}
      
      //Write track parameters:
      fwrite(&track,sizeof(AliL3TrackModel),1,output->file);
      
      Int_t origslice=-1,slice,clustercount=0;
      for(Int_t i=0; i<AliL3Transform::GetNRows(fPatch); i++)
	{
	  if(fread(&cluster,sizeof(AliL3ClusterModel),1,input)!=1) break;
	  
	  //Write empty flag:
	  temp = (Int_t)cluster.fPresent;
	  OutputBit(output,temp);
	  if(!temp) continue;
	  
	  if(cluster.fSlice<0 || cluster.fSlice>35)
	    {
	      cerr<<"AliL3DataCompress::CompressFile : Fucked up slice number :"<<cluster.fSlice<<endl;
	      exit(5);
	    }
	  
	  //Write slice number of first point
	  if(clustercount==0)
	    {
	      origslice = cluster.fSlice;
	      OutputBits(output,origslice,6); //Need 6 bits to encode slice number
	    }
	  else
	    {
	      slice = cluster.fSlice;
	      if(slice == origslice)
		OutputBit(output,0);  //No change of slice
	      else
		{
		  OutputBit(output,1);
		  OutputBits(output,slice,6);
		  origslice=slice;
		}
	    }
	  
	  //Write time information:
	  temp = (Int_t)rint(cluster.fDTime);
	  if(temp<0)
	    OutputBit(output,0);
	  else
	    OutputBit(output,1);
	  power = 1<<(AliL3DataCompressorHelper::GetNTimeBits()-1);
	  if(abs(temp)>=power)
	    {
	      //cout<<abs(temp)<<" "<<power<<endl;
	      timeo++;
	      temp=power - 1;
	    }
	  OutputBits(output,abs(temp),(AliL3DataCompressorHelper::GetNTimeBits()-1));
	  
	  //Write pad information:
	  temp = (Int_t)rint(cluster.fDPad);
	  if(temp<0)
	    OutputBit(output,0);
	  else
	    OutputBit(output,1);
	  power = 1<<(AliL3DataCompressorHelper::GetNPadBits()-1);
	  if(abs(temp)>=power)
	    {
	      pado++;
	      temp=power - 1;
	    }
	  OutputBits(output,abs(temp),(AliL3DataCompressorHelper::GetNPadBits()-1));
	  
	  //Write charge information:
	  temp = (Int_t)cluster.fDCharge;
	  power = 1<<(AliL3DataCompressorHelper::GetNChargeBits());
	  if(abs(temp)>=power)
	    {
	      chargeo++;
	      temp=power - 1;
	    }
	  OutputBits(output,abs(temp),(AliL3DataCompressorHelper::GetNChargeBits()));
	  
	  if(fWriteShape)
	    {
	      //Write shape information:
	      temp = (Int_t)cluster.fDSigmaY;
	      if(temp<0)
		OutputBit(output,0);
	      else
		OutputBit(output,1);
	      power = 1<<(AliL3DataCompressorHelper::GetNShapeBits()-1);
	      if(abs(temp) >= power)
		{
		  padshapeo++;
		  temp = power - 1;
		}
	      OutputBits(output,abs(temp),(AliL3DataCompressorHelper::GetNShapeBits()-1));
	      
	      temp = (Int_t)cluster.fDSigmaZ;
	      if(temp<0)
		OutputBit(output,0);
	      else
		OutputBit(output,1);
	      power = 1<<(AliL3DataCompressorHelper::GetNShapeBits()-1);
	      if(abs(temp) >= power)
		{
		  timeshapeo++;
		  temp=power - 1;
		}
	      OutputBits(output,abs(temp),(AliL3DataCompressorHelper::GetNShapeBits()-1));
	    }
	  
	  clustercount++;
	}
    }
  
  fclose(input);
  CloseOutputBitFile(output);
  if(pado || timeo || chargeo || padshapeo || timeshapeo)
    {
      cout<<endl<<"Saturations: "<<endl
	  <<"Pad "<<pado<<endl
	  <<"Time "<<timeo<<endl
	  <<"Charge "<<chargeo<<endl
	  <<"Padshape "<<padshapeo<<endl
	  <<"Timeshape "<<timeshapeo<<endl<<endl;
    }
  return kTRUE;
}

Bool_t AliL3Compress::ExpandFile()
{
  Char_t fname[100];
  if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_c_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_c_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
  BIT_FILE *input = OpenInputBitFile(fname);
  
  if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_u_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_u_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);
  FILE *output = fopen(fname,"w");
  if(!output)
    {
      cerr<<"AliL3Compress::ExpandFile() : Error opening file: "<<fname<<endl;
      return kFALSE;
    }

  AliL3TrackModel trackmodel;
  AliL3ClusterModel *clusters=0;
  Int_t count=0;
  
  clusters = new AliL3ClusterModel[(AliL3Transform::GetNRows(fPatch))];
  while(!feof(input->file))
    {
      input->mask=0x80;//make sure we read a new byte from file.
      
      //Read and write track:
      if(fread(&trackmodel,sizeof(AliL3TrackModel),1,input->file)!=1) break;
      fwrite(&trackmodel,sizeof(AliL3TrackModel),1,output);

      memset(clusters,0,AliL3Transform::GetNRows(fPatch)*sizeof(AliL3ClusterModel));
      Int_t origslice=-1,clustercount=0;
      for(Int_t i=0; i<AliL3Transform::GetNRows(fPatch); i++)
	{
	  Int_t temp,sign;
	  
	  //Read empty flag:
	  temp = InputBit(input);
	  if(!temp) 
	    {
	      clusters[i].fPresent=kFALSE;
	      continue;
	    }
	  clusters[i].fPresent=kTRUE;
	  
	  //Read slice information
	  if(clustercount==0)
	    {
	      temp = InputBits(input,6);
	      clusters[i].fSlice = temp;
	      origslice = temp;
	    }
	  else
	    {
	      temp = InputBit(input);
	      if(!temp)//no change
		clusters[i].fSlice = origslice;
	      else
		{
		  temp = InputBits(input,6);//read new slice
		  clusters[i].fSlice = temp;
		  origslice = temp;//store new slice
		}
	    }
	  
	  //Read time information:
	  sign=InputBit(input);
	  temp = InputBits(input,(AliL3DataCompressorHelper::GetNTimeBits()-1));
	  if(!sign)
	    temp*=-1;
	  clusters[i].fDTime = temp;
	  
	  //Read pad information:
	  sign=InputBit(input);
	  temp = InputBits(input,(AliL3DataCompressorHelper::GetNPadBits()-1));
	  if(!sign)
	    temp*=-1;
	  clusters[i].fDPad = temp;
	  
	  //Read charge information:
	  temp=InputBits(input,(AliL3DataCompressorHelper::GetNChargeBits()));
	  clusters[i].fDCharge = temp;
	  
	  if(fWriteShape)
	    {
	      //Read shape information:
	      sign = InputBit(input);
	      temp = InputBits(input,(AliL3DataCompressorHelper::GetNShapeBits()-1));
	      if(!sign)
		temp*=-1;
	      clusters[i].fDSigmaY = temp;
	      
	      sign = InputBit(input);
	      temp = InputBits(input,(AliL3DataCompressorHelper::GetNShapeBits()-1));
	      if(!sign)
		temp*=-1;
	      clusters[i].fDSigmaZ = temp;
	    }
	  clustercount++;
	}
      count++;
      //fwrite(clusters,(trackmodel.fNClusters)*sizeof(AliL3ClusterModel),1,output);
      fwrite(clusters,AliL3Transform::GetNRows(fPatch)*sizeof(AliL3ClusterModel),1,output);
    }
  
  delete [] clusters;
  fclose(output);
  CloseInputBitFile(input);
  return kTRUE;
}

void AliL3Compress::CompressRemaining(AliL3SpacePointData *clusters[36][6],UInt_t nclusters[36][6])
{
  //Write the remaining clusters in a compressed format.

  Char_t filename[1024];
  Int_t nrows = AliL3Transform::GetNRows();
  Int_t *npoints = new Int_t[nrows];
  for(Int_t slice=0; slice<=35; slice++)
    {
      for(Int_t patch=0; patch < 1; patch++)
	{
	  sprintf(filename,"%s/comp/remains_%d_%d_%d.raw",fPath,fEvent,slice,-1);
	  BIT_FILE *output = OpenOutputBitFile(filename);
	  if(!output)
	    {
	      cerr<<"AliL3Compress::CompressRemaining : Cannot open file "<<filename<<endl;
	      exit(5);
	    }
	  AliL3SpacePointData *cl = clusters[slice][patch];
	  memset(npoints,0,nrows*sizeof(Int_t));
	  
	  UInt_t i;
	  for(i=0; i<nclusters[slice][patch]; i++)
	    {
	      if(cl[i].fCharge == 0) continue; //has been used
	      npoints[cl[i].fPadRow]++;
	    }
	  Int_t rowspresent=0;
	  for(Int_t j=0; j<nrows; j++)
	    {
	      if(!npoints[j]) continue;
	      rowspresent++;
	    }
	  
	  //Write number of padrows with clusters
	  OutputBits(output,rowspresent,8);
	  
	  Int_t last_padrow=-1;
	  for(i=0; i<nclusters[slice][patch]; i++)
	    {
	      if(cl[i].fCharge == 0) continue; //has been used
	      Int_t padrow = cl[i].fPadRow;
	      if(padrow != last_padrow)
		{
		  OutputBits(output,padrow,8);//Write padrow #
		  if(npoints[padrow] >= 1<<10)
		    {
		      cerr<<"AliL3Compress::CompressRemaining : Too many remaining clusters "<<npoints[padrow]<<endl;
		      exit(5);
		    }
		  OutputBits(output,npoints[padrow],10);//Write number of clusters on this padrow
		  last_padrow = padrow;
		}
	      
	      Float_t xyz[3] = {cl[i].fX,cl[i].fY,cl[i].fZ};
	      Int_t sector,row,buff;
	      AliL3Transform::Slice2Sector(slice,padrow,sector,row);
	      AliL3Transform::Global2Raw(xyz,sector,row);
	      
	      Float_t padw = sqrt(cl[i].fSigmaY2) / AliL3Transform::GetPadPitchWidth(AliL3Transform::GetPatch(padrow));
	      Float_t timew = sqrt(cl[i].fSigmaZ2) / AliL3Transform::GetZWidth();
	      
	      //Write pad
	      buff = (Int_t)rint(xyz[1]*10);
	      if(buff<0)
		{
		  cerr<<"AliL3Compress:CompressRemaining : Wrong pad value "<<buff<<endl;
		  exit(5);
		}
	      OutputBits(output,buff,11);

	      //Write time
	      buff = (Int_t)rint(xyz[2]*10);
	      if(buff<0)
		{
		  cerr<<"AliL3Compress:CompressRemaining : Wrong time value "<<buff<<endl;
		  exit(5);
		}
	      OutputBits(output,buff,13);

	      //Write widths
	      buff = (Int_t)rint(padw*100);
	      OutputBits(output,buff,8);
	      buff = (Int_t)rint(timew*100);
	      OutputBits(output,buff,8);
	      
	      //Write charge 
	      buff = cl[i].fCharge;
	      if(buff >= 1<<14)
		buff = (1<<14)-1;
	      OutputBits(output,buff,14);
	    }
	  
	  CloseOutputBitFile(output);
	}
      
    }
  delete [] npoints;
}

void AliL3Compress::ExpandRemaining(TempCluster **clusters,Int_t *ncl,const Int_t maxpoints)
{
  //Expand the remaining clusters stored using function CompressRemaining
  
  Char_t filename[1024];
  Int_t buff;
  for(Int_t slice=0; slice<=35; slice++)
    {
      for(Int_t p=0; p<1; p++)
	{
	  sprintf(filename,"%s/comp/remains_%d_%d_%d.raw",fPath,fEvent,slice,-1);
	  BIT_FILE *input = OpenInputBitFile(filename);
	  
	  //Read number of padrows 
	  buff = InputBits(input,8);
	  Int_t nrows = buff;
	  
	  for(Int_t i=0; i<nrows; i++)
	    {
	      //Read padrow
	      buff = InputBits(input,8);
	      Int_t padrow = buff;
	      
	      //Read nclusters;
	      buff = InputBits(input,10);
	      Int_t npoints = buff;
	      
	      for(Int_t i=0; i<npoints; i++)
		{
		  clusters[slice][ncl[slice]].padrow = padrow;

		  //Read pad
		  buff = InputBits(input,11);
		  clusters[slice][ncl[slice]].pad = (Float_t)buff/10.;
		  
		  //Read time
		  buff = InputBits(input,13);
		  clusters[slice][ncl[slice]].time = (Float_t)buff/10.;
		  
		  //Read widths 
		  buff = InputBits(input,8);
		  clusters[slice][ncl[slice]].sigmaY2 = pow((Float_t)buff/100.,2);
		  buff = InputBits(input,8);
		  clusters[slice][ncl[slice]].sigmaZ2 = pow((Float_t)buff/100.,2);
		  
		  //Read charge
		  buff = InputBits(input,14);
		  clusters[slice][ncl[slice]].charge = buff;
		  //cout<<"padrow "<<padrow<<" pad "<<clusters[slice][ncl[slice]].pad<<" time "<<clusters[slice][ncl[slice]].time<<" charge "<<clusters[slice][ncl[slice]].charge<<" widths "<<clusters[slice][ncl[slice]].sigmaY2<<" "<<clusters[slice][ncl[slice]].sigmaZ2<<endl;
		  ncl[slice]++;
		}
	      
	    }
	  CloseInputBitFile(input);
	}
    }
}

void AliL3Compress::PrintCompRatio(ofstream *outfile)
{
  AliL3MemHandler *mem = new AliL3MemHandler();
  Char_t fname[1024];
  UInt_t remain_size=0,digit_size=0;
  for(Int_t i=0; i<36; i++)
    {
      if(fEvent<0)
	sprintf(fname,"%s/comp/remains_%d_%d.raw",fPath,i,-1);
      else
	sprintf(fname,"%s/comp/remains_%d_%d_%d.raw",fPath,fEvent,i,-1);
      mem->SetBinaryInput(fname);
      remain_size += mem->GetFileSize();
      mem->CloseBinaryInput();

      sprintf(fname,"%s/binaries/digits_c8_%d_%d_%d.raw",fPath,fEvent,i,-1);
      mem->SetBinaryInput(fname);
      digit_size += mem->GetFileSize();
      mem->CloseBinaryInput();
    }
  
  
  if(fEvent<0)
    sprintf(fname,"%s/comp/tracks_c_%d_%d.raw",fPath,fSlice,fPatch);
  else
    sprintf(fname,"%s/comp/tracks_c_%d_%d_%d.raw",fPath,fEvent,fSlice,fPatch);

  mem->SetBinaryInput(fname);
  UInt_t compress_size = mem->GetFileSize();
  mem->CloseBinaryInput();
  
  if(digit_size==0)
    {
      cerr<<"AliL3Compress::PrintCompRatio : Zero digit size, not able to obtain comp. ratios!"<<endl;
      return;
    }
  
  Float_t compratio = (Float_t)(compress_size + remain_size)/(Float_t)digit_size;
  Float_t entropy[2];
  GetEntropy(entropy[0],entropy[1]);
  if(outfile)
    {
      ofstream &out = *outfile;
      out<<compress_size<<' '<<remain_size<<' '<<digit_size<<' '<<entropy[0]<<' '<<entropy[1]<<endl;
    }
  
  cout<<"=========================================="<<endl;
  cout<<"Original digits size : "<<digit_size/1000<<" kByte ( 100 % )"<<endl;
  cout<<"Compressed file size : "<<compress_size/1000<<" kByte ( "<<(Float_t)compress_size*100/(Float_t)digit_size<<" % )"<<endl;
  cout<<"Entropy of residuals : "<<entropy[0]<<" "<<entropy[1]<<endl;
  cout<<"Remainig file size   : "<<remain_size/1000<<" kByte ( "<<(Float_t)remain_size*100/(Float_t)digit_size<<" % )"<<endl;
  cout<<"---------------------- "<<endl;
  cout<<"Compression ratio    : "<<compratio*100<<" %"<<endl;
  cout<<"=========================================="<<endl;
}

void AliL3Compress::GetEntropy(Float_t &pad_entropy,Float_t &time_entropy)
{
  //Calculate the entropy of the quantized residuals in both directions
  
  if(!ReadFile('m'))
    return;
  const Int_t nmax=100000;
  Float_t pads[nmax];
  Float_t times[nmax];
  memset(&pads[0],0,nmax*sizeof(Float_t));
  memset(&times[0],0,nmax*sizeof(Float_t));
  Float_t counter=0;

  for(Int_t i=0; i<fTracks->GetNTracks(); i++)
    {
      AliL3ModelTrack *track = (AliL3ModelTrack*)fTracks->GetCheckedTrack(i);
      if(!track) continue;
      for(Int_t padrow=0; padrow<AliL3Transform::GetNRows(); padrow++)
	{
	  if(!track->IsPresent(padrow)) continue;
	  Int_t dpad = abs((Int_t)rint(track->GetClusterModel(padrow)->fDPad));
	  Int_t dtime = abs((Int_t)rint(track->GetClusterModel(padrow)->fDTime));
	  if(dpad >= nmax || dtime >= nmax)
	    {
	      cerr<<"AliL3Compress::GetEntropy : Quantization out of range: "<<dpad<<" "<<dtime<<endl;
	      break;
	    }
	  pads[dpad]++;
	  times[dtime]++;
	  counter++;
	}
    }
  pad_entropy=time_entropy=0;
  for(Int_t i=0; i<nmax; i++)
    {
      if(pads[i]>0)
	pad_entropy += (pads[i]/counter)*(log(pads[i]/counter)/log(2.0));
      if(times[i]>0)
	time_entropy += (times[i]/counter)*(log(times[i]/counter)/log(2.0));
    }
  
  pad_entropy*=-1;
  time_entropy*=-1;
}
