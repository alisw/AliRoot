// @(#) $Id$

// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#ifndef no_root
#include <TNtuple.h>
#include <TFile.h>
#endif

#include "AliL3Logging.h"
#include "AliL3HoughMaxFinder.h"
#include "AliL3Histogram.h"
#include "AliL3TrackArray.h"
#include "AliL3HoughTrack.h"

#if __GNUC__ == 3
using namespace std;
#endif

/** \class AliL3HoughMaxFinder
<pre>
//_____________________________________________________________
// AliL3HoughMaxFinder
//
// Maximum finder
//
</pre>
*/

ClassImp(AliL3HoughMaxFinder)

  
AliL3HoughMaxFinder::AliL3HoughMaxFinder()
{
  //Default constructor
  fThreshold = 0;
  fHistoType=0;
  fXPeaks=0;
  fYPeaks=0;
  fNPeaks=0;
  fNMax=0;
  fGradX=1;
  fGradY=1;
#ifndef no_root
  fNtuppel = 0;
#endif
}

AliL3HoughMaxFinder::AliL3HoughMaxFinder(Char_t *histotype,Int_t nmax,AliL3Histogram *hist)
{
  //Constructor

  //fTracks = new AliL3TrackArray("AliL3HoughTrack");
  if(strcmp(histotype,"KappaPhi")==0) fHistoType='c';
  if(strcmp(histotype,"DPsi")==0) fHistoType='l';
  
  if(hist)
    fCurrentHisto = hist;
  
  fGradX=1;
  fGradY=1;
  fNMax=nmax;
  fXPeaks = new Float_t[fNMax];
  fYPeaks = new Float_t[fNMax];
  fWeight = new Int_t[fNMax];
#ifndef no_root
  fNtuppel = 0;
#endif
  fThreshold=0;
}

AliL3HoughMaxFinder::~AliL3HoughMaxFinder()
{
  //Destructor
  if(fXPeaks)
    delete [] fXPeaks;
  if(fYPeaks)
    delete [] fYPeaks;
  if(fWeight)
    delete [] fWeight;
#ifndef no_root
  if(fNtuppel)
    delete fNtuppel;
#endif
}

void AliL3HoughMaxFinder::Reset()
{
  for(Int_t i=0; i<fNMax; i++)
    {
      fXPeaks[i]=0;
      fYPeaks[i]=0;
      fWeight[i]=0;
    }
  fNPeaks=0;
}

void AliL3HoughMaxFinder::CreateNtuppel()
{
#ifndef no_root
  //content#; neighbouring bins of the peak.
  fNtuppel = new TNtuple("ntuppel","Peak charateristics","kappa:phi0:weigth:content3:content5:content1:content7");
  fNtuppel->SetDirectory(0);
#endif  
}

void AliL3HoughMaxFinder::WriteNtuppel(Char_t *filename)
{
#ifndef no_root
  TFile *file = TFile::Open(filename,"RECREATE");
  if(!file)
    {
      cerr<<"AliL3HoughMaxFinder::WriteNtuppel : Error opening file "<<filename<<endl;
      return;
    }
  fNtuppel->Write();
  file->Close();
#endif
}

void AliL3HoughMaxFinder::FindAbsMaxima()
{
  
  if(!fCurrentHisto)
    {
      cerr<<"AliL3HoughMaxFinder::FindAbsMaxima : No histogram"<<endl;
      return;
    }
  AliL3Histogram *hist = fCurrentHisto;
  
  if(hist->GetNEntries() == 0)
    return;
  
  Int_t xmin = hist->GetFirstXbin();
  Int_t xmax = hist->GetLastXbin();
  Int_t ymin = hist->GetFirstYbin();
  Int_t ymax = hist->GetLastYbin();  
  Int_t bin;
  Double_t value,max_value=0;
  
  Int_t max_xbin=0,max_ybin=0;
  for(Int_t xbin=xmin; xbin<=xmax; xbin++)
    {
      for(Int_t ybin=ymin; ybin<=ymax; ybin++)
	{
	  bin = hist->GetBin(xbin,ybin);
	  value = hist->GetBinContent(bin);
	  if(value>max_value)
	    {
	      max_value = value;
	      max_xbin = xbin;
	      max_ybin = ybin;
	    }
	}
    }
  
  if(max_value == 0)
    return;
  
  if(fNPeaks > fNMax)
    {
      cerr<<"AliL3HoughMaxFinder::FindAbsMaxima : Array out of range : "<<fNPeaks<<endl;
      return;
    }
  
  Double_t max_x = hist->GetBinCenterX(max_xbin);
  Double_t max_y = hist->GetBinCenterY(max_ybin);
  fXPeaks[fNPeaks] = max_x;
  fYPeaks[fNPeaks] = max_y;
  fWeight[fNPeaks] = (Int_t)max_value;

  fNPeaks++;
#ifndef no_root
  if(fNtuppel)
    {
      Int_t bin3 = hist->GetBin(max_xbin-1,max_ybin);
      Int_t bin5 = hist->GetBin(max_xbin+1,max_ybin);
      Int_t bin1 = hist->GetBin(max_xbin,max_ybin-1);
      Int_t bin7 = hist->GetBin(max_xbin,max_ybin+1);
      
      fNtuppel->Fill(max_x,max_y,max_value,hist->GetBinContent(bin3),hist->GetBinContent(bin5),hist->GetBinContent(bin1),hist->GetBinContent(bin7));
    }
#endif  
}

void AliL3HoughMaxFinder::FindBigMaxima()
{
  
  AliL3Histogram *hist = fCurrentHisto;
  
  if(hist->GetNEntries() == 0)
    return;
  
  Int_t xmin = hist->GetFirstXbin();
  Int_t xmax = hist->GetLastXbin();
  Int_t ymin = hist->GetFirstYbin();
  Int_t ymax = hist->GetLastYbin();
  Int_t bin[25],bin_index;
  Double_t value[25];
  
  for(Int_t xbin=xmin+2; xbin<xmax-3; xbin++)
    {
      for(Int_t ybin=ymin+2; ybin<ymax-3; ybin++)
	{
	  bin_index=0;
	  for(Int_t xb=xbin-2; xb<=xbin+2; xb++)
	    {
	      for(Int_t yb=ybin-2; yb<=ybin+2; yb++)
		{
		  bin[bin_index]=hist->GetBin(xb,yb);
		  value[bin_index]=hist->GetBinContent(bin[bin_index]);
		  bin_index++;
		}
	    }
	  if(value[12]==0) continue;
	  Int_t b=0;
	  while(1)
	    {
	      if(value[b] > value[12] || b==bin_index) break;
	      b++;
	      //printf("b %d\n",b);
	    }
	  if(b == bin_index)
	    {
	      //Found maxima
	      if(fNPeaks > fNMax)
		{
		  cerr<<"AliL3HoughMaxFinder::FindBigMaxima : Array out of range "<<fNPeaks<<endl;
		  return;
		}
	      
	      Double_t max_x = hist->GetBinCenterX(xbin);
	      Double_t max_y = hist->GetBinCenterY(ybin);
	      fXPeaks[fNPeaks] = max_x;
	      fYPeaks[fNPeaks] = max_y;
	      fNPeaks++;
	    }
	}
    }
}

void AliL3HoughMaxFinder::FindMaxima(Int_t threshold)
{
  //Locate all the maxima in input histogram.
  //Maxima is defined as bins with more entries than the
  //immediately neighbouring bins. 
  
  if(fCurrentHisto->GetNEntries() == 0)
    return;
  
  Int_t xmin = fCurrentHisto->GetFirstXbin();
  Int_t xmax = fCurrentHisto->GetLastXbin();
  Int_t ymin = fCurrentHisto->GetFirstYbin();
  Int_t ymax = fCurrentHisto->GetLastYbin();
  Int_t bin[9];
  Double_t value[9];
  
  //Float_t max_kappa = 0.001;
  //Float_t max_phi0 = 0.08;

  for(Int_t xbin=xmin+1; xbin<=xmax-1; xbin++)
    {
      for(Int_t ybin=ymin+1; ybin<=ymax-1; ybin++)
	{
	  bin[0] = fCurrentHisto->GetBin(xbin-1,ybin-1);
	  bin[1] = fCurrentHisto->GetBin(xbin,ybin-1);
	  bin[2] = fCurrentHisto->GetBin(xbin+1,ybin-1);
	  bin[3] = fCurrentHisto->GetBin(xbin-1,ybin);
	  bin[4] = fCurrentHisto->GetBin(xbin,ybin);
	  bin[5] = fCurrentHisto->GetBin(xbin+1,ybin);
	  bin[6] = fCurrentHisto->GetBin(xbin-1,ybin+1);
	  bin[7] = fCurrentHisto->GetBin(xbin,ybin+1);
	  bin[8] = fCurrentHisto->GetBin(xbin+1,ybin+1);
	  value[0] = fCurrentHisto->GetBinContent(bin[0]);
	  value[1] = fCurrentHisto->GetBinContent(bin[1]);
	  value[2] = fCurrentHisto->GetBinContent(bin[2]);
	  value[3] = fCurrentHisto->GetBinContent(bin[3]);
	  value[4] = fCurrentHisto->GetBinContent(bin[4]);
	  value[5] = fCurrentHisto->GetBinContent(bin[5]);
	  value[6] = fCurrentHisto->GetBinContent(bin[6]);
	  value[7] = fCurrentHisto->GetBinContent(bin[7]);
	  value[8] = fCurrentHisto->GetBinContent(bin[8]);
	  
	  
	  if(value[4]>value[0] && value[4]>value[1] && value[4]>value[2]
	     && value[4]>value[3] && value[4]>value[5] && value[4]>value[6]
	     && value[4]>value[7] && value[4]>value[8])
	    {
	      //Found a local maxima
	      Float_t max_x = fCurrentHisto->GetBinCenterX(xbin);
	      Float_t max_y = fCurrentHisto->GetBinCenterY(ybin);
	      
	      if((Int_t)value[4] <= threshold) continue;//central bin below threshold
	      if(fNPeaks >= fNMax)
		{
		  cout<<"AliL3HoughMaxFinder::FindMaxima : Array out of range "<<fNPeaks<<endl;
		  return;
		}
	      
	      //Check the gradient:
	      if(value[3]/value[4] > fGradX && value[5]/value[4] > fGradX)
		continue;

	      if(value[1]/value[4] > fGradY && value[7]/value[4] > fGradY)
		continue;

	      fXPeaks[fNPeaks] = max_x;
	      fYPeaks[fNPeaks] = max_y;
	      fWeight[fNPeaks] = (Int_t)value[4];
	      fNPeaks++;

	      /*
	      //Check if the peak is overlapping with a previous:
	      Bool_t bigger = kFALSE;
	      for(Int_t p=0; p<fNPeaks; p++)
	        {
		  if(fabs(max_x - fXPeaks[p]) < max_kappa && fabs(max_y - fYPeaks[p]) < max_phi0)
		    {
		      bigger = kTRUE;
		      if(value[4] > fWeight[p]) //this peak is bigger.
			{
		 	  fXPeaks[p] = max_x;
			  fYPeaks[p] = max_y;
			  fWeight[p] = (Int_t)value[4];
			}
		      else
			continue; //previous peak is bigger.
		    }
		}
	      if(!bigger) //there were no overlapping peaks.
		{
		  fXPeaks[fNPeaks] = max_x;
		  fYPeaks[fNPeaks] = max_y;
		  fWeight[fNPeaks] = (Int_t)value[4];
		  fNPeaks++;
		}
	      */
	    }
	}
    }
  
}

struct Window 
{
  Int_t start;
  Int_t sum;
};

void AliL3HoughMaxFinder::FindAdaptedPeaks(Int_t kappawindow,Float_t cut_ratio)
{
  //Peak finder which looks for peaks with a certain shape.
  //The first step involves a pre-peak finder, which looks for peaks
  //in windows (size controlled by kappawindow) summing over each psi-bin.
  //These pre-preaks are then matched between neighbouring kappa-bins to
  //look for real 2D peaks exhbiting the typical cross-shape in the Hough circle transform.
  //The maximum bin within this region is marked as the peak itself, and
  //a few checks is performed to avoid the clear fake peaks (asymmetry check etc.)
  
  
  AliL3Histogram *hist = fCurrentHisto;
  
  if(!hist)
    {
      cerr<<"AliL3HoughMaxFinder : No histogram!"<<endl;
      return;
    }
  
  if(hist->GetNEntries() == 0)
    return;

  Int_t xmin = hist->GetFirstXbin();
  Int_t xmax = hist->GetLastXbin();
  Int_t ymin = hist->GetFirstYbin();
  Int_t ymax = hist->GetLastYbin();


  //Start by looking for pre-peaks:
  
  Window **local_maxima = new Window*[hist->GetNbinsY()];
  
  Short_t *nmaxs = new Short_t[hist->GetNbinsY()];
  Int_t n,last_sum,sum;
  Bool_t sum_was_rising;
  for(Int_t ybin=ymin; ybin<=ymax; ybin++)
    {
      local_maxima[ybin-ymin] = new Window[hist->GetNbinsX()];
      nmaxs[ybin-ymin] = 0;
      sum_was_rising=0;
      last_sum=0;
      n=0;
      for(Int_t xbin=xmin; xbin<=xmax-kappawindow; xbin++)
	{
	  sum=0;
	  for(Int_t lbin=xbin; lbin<xbin+kappawindow; lbin++)
	    sum += hist->GetBinContent(hist->GetBin(lbin,ybin));
	  
	  if(sum < last_sum)
	    {
	      if(sum > fThreshold)
		if(sum_was_rising)//Previous sum was a local maxima
		  {
		    local_maxima[ybin-ymin][nmaxs[ybin-ymin]].start = xbin-1;
		    local_maxima[ybin-ymin][nmaxs[ybin-ymin]].sum = last_sum;
		    nmaxs[ybin-ymin]++;
		  }
	      
	      sum_was_rising=0;
	    }
	  else if(sum > 0) 
	    sum_was_rising=1;
	  last_sum=sum;
	}
    }
  
  Int_t match=0;
  Int_t *starts = new Int_t[hist->GetNbinsY()+1];
  Int_t *maxs = new Int_t[hist->GetNbinsY()+1];
  
  for(Int_t ybin=ymax; ybin >= ymin+1; ybin--)
    {
      for(Int_t i=0; i<nmaxs[ybin-ymin]; i++)
	{
	  Int_t lw = local_maxima[ybin-ymin][i].sum;

	  if(lw<0)
	    continue; //already used

	  Int_t maxvalue=0,maxybin=0,maxxbin=0,maxwindow=0;
	  for(Int_t k=local_maxima[ybin-ymin][i].start; k<local_maxima[ybin-ymin][i].start + kappawindow; k++)
	    if(hist->GetBinContent(hist->GetBin(k,ybin)) > maxvalue)
	      {
		maxvalue = hist->GetBinContent(hist->GetBin(k,ybin));
		maxybin = ybin;
		maxxbin = k;
	      }
	  
	  //start expanding in the psi-direction:

	  Int_t lb = local_maxima[ybin-ymin][i].start;
	  //Int_t ystart=ybin;
	  starts[ybin] = local_maxima[ybin-ymin][i].start;
	  maxs[ybin] = maxxbin;
	  Int_t yl=ybin-1,nybins=1;
	  
	  //cout<<"Starting search at ybin "<<ybin<<" start "<<lb<<" with sum "<<local_maxima[ybin-ymin][i].sum<<endl;
	  while(yl >= ymin)
	    {
	      Bool_t found=0;
	      for(Int_t j=0; j<nmaxs[yl-ymin]; j++)
		{
		  if( local_maxima[yl-ymin][j].start - lb < 0) continue;
		  if( local_maxima[yl-ymin][j].start < lb + kappawindow + match &&
		      local_maxima[yl-ymin][j].start >= lb && local_maxima[yl-ymin][j].sum > 0)
		    {
		      
		      //cout<<"match at ybin "<<yl<<" yvalue "<<hist->GetBinCenterY(yl)<<" start "<<local_maxima[yl-ymin][j].start<<" sum "<<local_maxima[yl-ymin][j].sum<<endl;
		      
		      Int_t lmaxvalue=0,lmaxxbin=0;
		      for(Int_t k=local_maxima[yl-ymin][j].start; k<local_maxima[yl-ymin][j].start + kappawindow; k++)
			{
			  if(hist->GetBinContent(hist->GetBin(k,yl)) > maxvalue)
			    {
			      maxvalue = hist->GetBinContent(hist->GetBin(k,yl));
			      maxxbin = k;
			      maxybin = yl;
			      maxwindow = j;
			    }
			  if(hist->GetBinContent(hist->GetBin(k,yl)) > lmaxvalue)//local maxima value
			    {
			      lmaxvalue=hist->GetBinContent(hist->GetBin(k,yl));
			      lmaxxbin=k;
			    }
			}
		      nybins++;
		      starts[yl] = local_maxima[yl-ymin][j].start;
		      maxs[yl] = lmaxxbin;
		      local_maxima[yl-ymin][j].sum=-1; //Mark as used
		      found=1;
		      lb = local_maxima[yl-ymin][j].start;
		      break;//Since we found a match in this bin, we dont have to search it anymore, goto next bin.
		    }
		}
	      if(!found || yl == ymin)//no more local maximas to be matched, so write the final peak and break the expansion:
		{
		  if(nybins > 4)
		    {
		      //cout<<"ystart "<<ystart<<" and nybins "<<nybins<<endl;

		      Bool_t truepeak=kTRUE;
		      
		      //cout<<"Maxima found at xbin "<<maxxbin<<" ybin "<<maxybin<<" value "<<maxvalue<<endl;
		      //cout<<"Starting to sum at xbin "<<starts[maxybin-ymin]<<endl;
		      
		      
		      //Look in a window on both sides to probe the asymmetry
		      Float_t right=0,left=0;
		      for(Int_t w=maxxbin+1; w<=maxxbin+3; w++)
			{
			  for(Int_t r=maxybin+1; r<=maxybin+3; r++)
			    {
			      right += (Float_t)hist->GetBinContent(hist->GetBin(w,r));
			    }
			}
		      
		      for(Int_t w=maxxbin-1; w>=maxxbin-3; w--)
			{
			  for(Int_t r=maxybin+1; r<=maxybin+3; r++)
			    {
			      left += (Float_t)hist->GetBinContent(hist->GetBin(w,r));
			    }
			}
		      
		      //cout<<"ratio "<<right/left<<endl;
		      
		      Float_t upper_ratio=1,lower_ratio=1;
		      if(left)
			upper_ratio = right/left;
		      
		      right=left=0;
		      for(Int_t w=maxxbin+1; w<=maxxbin+3; w++)
			{
			  for(Int_t r=maxybin-1; r>=maxybin-3; r--)
			    {
			      right += (Float_t)hist->GetBinContent(hist->GetBin(w,r));
			    }
			}
		      
		      for(Int_t w=maxxbin-1; w>=maxxbin-3; w--)
			{
			  for(Int_t r=maxybin-1; r>=maxybin-3; r--)
			    {
			      left += (Float_t)hist->GetBinContent(hist->GetBin(w,r));
			    }
			}
		      
		      //cout<<"ratio "<<left/right<<endl;
		      
		      if(right)
			lower_ratio = left/right;
		      
		      if(upper_ratio > cut_ratio || lower_ratio > cut_ratio)
 			truepeak=kFALSE;
		      
		      if(truepeak)
			{
			  
			  fXPeaks[fNPeaks] = hist->GetBinCenterX(maxxbin);
			  fYPeaks[fNPeaks] = hist->GetBinCenterY(maxybin);
			  fWeight[fNPeaks] = maxvalue;
			  fNPeaks++;
			  
			  /*
			  //Calculate the peak using weigthed means:
			  Float_t sum=0;
			  fYPeaks[fNPeaks]=0;
			  for(Int_t k=maxybin-1; k<=maxybin+1; k++)
			    {
			      Float_t lsum = 0;
			      for(Int_t l=starts[k]; l<starts[k]+kappawindow; l++)
				{
				  lsum += (Float_t)hist->GetBinContent(hist->GetBin(l,k));
				  sum += (Float_t)hist->GetBinContent(hist->GetBin(l,k));
				}
			      fYPeaks[fNPeaks] += lsum*hist->GetBinCenterY(k);
			    }
			  fYPeaks[fNPeaks] /= sum;
			  Int_t ybin1,ybin2;
			  if(fYPeaks[fNPeaks] < hist->GetBinCenterY(hist->FindYbin(fYPeaks[fNPeaks])))
			    {
			      ybin1 = hist->FindYbin(fYPeaks[fNPeaks])-1;
			      ybin2 = ybin1+1;
			    }
			  else
			    {
			      ybin1 = hist->FindYbin(fYPeaks[fNPeaks]);
			      ybin2 = ybin1+1;
			    }

			  Float_t kappa1=0,kappa2=0;
			  sum=0;
			  for(Int_t k=starts[ybin1]; k<starts[ybin1] + kappawindow; k++)
			    {
			      kappa1 += hist->GetBinCenterX(k)*hist->GetBinContent(hist->GetBin(k,ybin1));
			      sum += (Float_t)hist->GetBinContent(hist->GetBin(k,ybin1));
			    }
			  kappa1 /= sum;
			  sum=0;
			  for(Int_t k=starts[ybin2]; k<starts[ybin2] + kappawindow; k++)
			    {
			      kappa2 += hist->GetBinCenterX(k)*hist->GetBinContent(hist->GetBin(k,ybin2));
			      sum += (Float_t)hist->GetBinContent(hist->GetBin(k,ybin2));
			    }
			  kappa2 /= sum;
			  
			  fXPeaks[fNPeaks] = ( kappa1*( hist->GetBinCenterY(ybin2) - fYPeaks[fNPeaks] ) + 
					       kappa2*( fYPeaks[fNPeaks] - hist->GetBinCenterY(ybin1) ) )  / 
 			    (hist->GetBinCenterY(ybin2) - hist->GetBinCenterY(ybin1));

			  fNPeaks++;
			  */
			}
		    }
		  break;
		}
	      else
		yl--;//Search continues...
	    }
	}
    }

  for(Int_t i=0; i<hist->GetNbinsY(); i++)
    delete local_maxima[i];

  delete [] local_maxima;
  delete [] nmaxs;
  delete [] starts;
  delete [] maxs;
}

struct PreYPeak 
{
  Int_t start_position;
  Int_t end_position;
  Int_t value;
  Int_t prev_value;
};

struct Pre2DPeak 
{
  Int_t start_x_position;
  Int_t end_x_position;
  Int_t start_y_position;
  Int_t end_y_position;
  Int_t value;
};

void AliL3HoughMaxFinder::FindAdaptedRowPeaks(Int_t kappawindow,Int_t xsize,Int_t ysize)
{
  
  AliL3Histogram *hist = fCurrentHisto;
  
  if(!hist)
    {
      cerr<<"AliL3HoughMaxFinder : No histogram!"<<endl;
      return;
    }
  
  if(hist->GetNEntries() == 0)
    return;
  
  Int_t xmin = hist->GetFirstXbin();
  Int_t xmax = hist->GetLastXbin();
  Int_t ymin = hist->GetFirstYbin();
  Int_t ymax = hist->GetLastYbin();
  
  //Start by looking for pre-peaks:
  
  PreYPeak **local_maxima = new PreYPeak*[hist->GetNbinsY()];
  
  Short_t *nmaxs = new Short_t[hist->GetNbinsY()];
  Int_t n2dmax = 0;
  Int_t last_value,value;
  for(Int_t ybin=ymin; ybin<=ymax; ybin++)
    {
      local_maxima[ybin-ymin] = new PreYPeak[hist->GetNbinsX()];
      nmaxs[ybin-ymin] = 0;
      last_value = 0;
      Bool_t found = 0;
      for(Int_t xbin=xmin; xbin<=xmax; xbin++)
	{
	  value = hist->GetBinContent(hist->GetBin(xbin,ybin));
	  if(value >= fThreshold)
	    {
	      if(value > last_value)
	      //	      if(value > (last_value + 1))
		{
		  local_maxima[ybin-ymin][nmaxs[ybin-ymin]].start_position = xbin;
		  local_maxima[ybin-ymin][nmaxs[ybin-ymin]].end_position = xbin;
		  local_maxima[ybin-ymin][nmaxs[ybin-ymin]].value = value;
		  local_maxima[ybin-ymin][nmaxs[ybin-ymin]].prev_value = 0;
		  found = 1;
		}
	      if(value == last_value)
		//	      if(abs(value - last_value) <= 1)
		if(found)
		  local_maxima[ybin-ymin][nmaxs[ybin-ymin]].end_position = xbin;
	    }

	  if((value < fThreshold) || (value < last_value))
	    //	  if((value < fThreshold) || (value < (last_value - 1)))
	    {
	      if(found)
		{
		  nmaxs[ybin-ymin]++;
		  found = 0;
		}
	    }
	  last_value = value;
	      
	}
      n2dmax += nmaxs[ybin-ymin];
    }
  
  //  Pre2DPeak *maxima = new Pre2DPeak[n2dmax];
  for(Int_t ybin=ymax; ybin >= ymin; ybin--)
    {
      for(Int_t i=0; i<nmaxs[ybin-ymin]; i++)
	{
	  Int_t local_value = local_maxima[ybin-ymin][i].value;
	  Int_t local_prev_value = local_maxima[ybin-ymin][i].prev_value;
	  Int_t local_next_value = 0;

	  if(local_value<0)
	    continue; //already used

	  //start expanding in the psi-direction:

	  Int_t local_x_start = local_maxima[ybin-ymin][i].start_position;
	  Int_t local_x_end = local_maxima[ybin-ymin][i].end_position;
	  Int_t temp_x_start = local_maxima[ybin-ymin][i].start_position;
	  Int_t temp_x_end = local_maxima[ybin-ymin][i].end_position;

	  Int_t local_y=ybin-1,nybins=1;
	  
	  while(local_y >= ymin)
	    {
	      Bool_t found=0;
	      for(Int_t j=0; j<nmaxs[local_y-ymin]; j++)
		{
		  Int_t adapted_kappawindow;
		  Float_t adapted_x,adapted_y;
		  adapted_x = ((Float_t)local_x_start+(Float_t)local_x_end)/2.0;
		  adapted_y = ((Float_t)ybin+(Float_t)(local_y+1))/2.0;
		  //		  if(adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56) && adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))
		  //		  if(adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))
		  //		  if((adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56)) && !(adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40)))
		  if(((adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56)) && !(adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))) || 
		     ((adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))&& !(adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56))))
		    adapted_kappawindow = 1;
		  else
		    adapted_kappawindow = 0;
		  //temprorary here
		  //		  adapted_kappawindow = 0;
		  
		  if( (local_maxima[local_y-ymin][j].start_position <= (temp_x_end + kappawindow + adapted_kappawindow)) && (local_maxima[local_y-ymin][j].end_position >= temp_x_start)) 
		    {
		      if( local_maxima[local_y-ymin][j].value == local_value )
			{
			  local_x_end = local_maxima[local_y-ymin][j].end_position;
			  temp_x_start = local_maxima[local_y-ymin][j].start_position;
			  temp_x_end = local_maxima[local_y-ymin][j].end_position;
			  local_maxima[local_y-ymin][j].value = -1;
			  found = 1;
			  nybins++;
			  break;
			}
		      else
			{
			  local_maxima[local_y-ymin][j].prev_value = local_value;
			  local_next_value = local_maxima[local_y-ymin][j].value;
			}
		    }
		}
	      if(!found || local_y == ymin)//no more local maximas to be matched, so write the final peak and break the expansion:
		{
		  Int_t adapted_xsize;
		  Float_t adapted_x,adapted_y;
		  adapted_x = ((Float_t)local_x_start+(Float_t)local_x_end)/2.0;
		  adapted_y = ((Float_t)ybin+(Float_t)(local_y+1))/2.0;
		  //		  if(adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56) && adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))
		  //		  if(adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40))
		  if((adapted_y<(-0.46*adapted_x+86) && adapted_y>(-0.46*adapted_x+56)) && !(adapted_y<(-0.215*adapted_x+68) && adapted_y>(-0.215*adapted_x+40)))
		    adapted_xsize = 1;
		  else
		    adapted_xsize = 2;
		  //temprorary here
		  //		  adapted_xsize = 1;
		  if((nybins > ysize) && ((local_x_end-local_x_start+1) > adapted_xsize) && (local_value > local_prev_value) && (local_value > local_next_value))
		    {
		      fXPeaks[fNPeaks] = hist->GetPreciseBinCenterX(adapted_x);
		      fYPeaks[fNPeaks] = hist->GetPreciseBinCenterY(adapted_y);
		      fWeight[fNPeaks] = local_value;
#ifdef do_mc
		      cout<<"Peak found at: "<<((Float_t)local_x_start+(Float_t)local_x_end)/2.0<<" "<<((Float_t)ybin+(Float_t)(local_y+1))/2.0<<" "<<fXPeaks[fNPeaks]<<" "<<fYPeaks[fNPeaks]<<" with weight "<<fWeight[fNPeaks]<<" and size "<<local_x_end-local_x_start+1<<" by "<<nybins<<endl;
#endif
		      fNPeaks++;
		    }
		  break;
		}
	      else
		local_y--;//Search continues...
	    }
	}
    }

  for(Int_t i=0; i<hist->GetNbinsY(); i++)
    delete local_maxima[i];

  delete [] local_maxima;
  delete [] nmaxs;
  //delete [] maxima;
}

void AliL3HoughMaxFinder::FindPeak1(Int_t y_window,Int_t x_bin_sides)
{
  //Testing mutliple peakfinding.
  //The algorithm searches the histogram for prepreaks by looking in windows
  //for each bin on the xaxis. The size of these windows is controlled by y_window.
  //Then the prepreaks are sorted according to their weight (sum inside window),
  //and the peak positions are calculated by taking the weighted mean in both
  //x and y direction. The size of the peak in x-direction is controlled by x_bin_sides.

  if(!fCurrentHisto)
    {
      printf("AliL3HoughMaxFinder::FindPeak1 : No input histogram\n");
      return;
    }  
  if(fCurrentHisto->GetNEntries()==0)
    return;
  
  //Int_t y_window=2;
  //Int_t x_bin_sides=1;
  
  //Float_t max_kappa = 0.001;
  //Float_t max_phi0 = 0.08;
  
  Int_t max_sum=0;
  
  Int_t xmin = fCurrentHisto->GetFirstXbin();
  Int_t xmax = fCurrentHisto->GetLastXbin();
  Int_t ymin = fCurrentHisto->GetFirstYbin();
  Int_t ymax = fCurrentHisto->GetLastYbin();
  Int_t nbinsx = fCurrentHisto->GetNbinsX()+1;
  
  AxisWindow **windowPt = new AxisWindow*[nbinsx];
  AxisWindow **anotherPt = new AxisWindow*[nbinsx];
  
  for(Int_t i=0; i<nbinsx; i++)
    {
      windowPt[i] = new AxisWindow;
      bzero((void*)windowPt[i],sizeof(AxisWindow));
      anotherPt[i] = windowPt[i];
    }
  
  for(Int_t xbin=xmin; xbin<=xmax; xbin++)
    {
      max_sum = 0;
      for(Int_t ybin=ymin; ybin<=ymax-y_window; ybin++)
	{
	  Int_t sum_in_window=0;
	  for(Int_t b=ybin; b<ybin+y_window; b++)
	    {
	      //inside window
	      Int_t bin = fCurrentHisto->GetBin(xbin,b);
	      sum_in_window += (Int_t)fCurrentHisto->GetBinContent(bin);
	    }
	  
	  if(sum_in_window > max_sum)
	    {
	      max_sum = sum_in_window;
	      windowPt[xbin]->ymin = ybin;
	      windowPt[xbin]->ymax = ybin + y_window;
	      windowPt[xbin]->weight = sum_in_window;
	      windowPt[xbin]->xbin = xbin;
	    }
	}
    }

  //Sort the windows according to the weight
  SortPeaks(windowPt,0,nbinsx);
  
  Float_t top,butt;
  for(Int_t i=0; i<nbinsx; i++)
    {
      top=butt=0;
      Int_t xbin = windowPt[i]->xbin;
      
      if(xbin<xmin || xbin > xmax-1) continue;
      
      //Check if this is really a local maxima
      if(anotherPt[xbin-1]->weight > anotherPt[xbin]->weight ||
	 anotherPt[xbin+1]->weight > anotherPt[xbin]->weight)
	continue;

      for(Int_t j=windowPt[i]->ymin; j<windowPt[i]->ymax; j++)
	{
	  //Calculate the mean in y direction:
	  Int_t bin = fCurrentHisto->GetBin(windowPt[i]->xbin,j);
	  top += (fCurrentHisto->GetBinCenterY(j))*(fCurrentHisto->GetBinContent(bin));
	  butt += fCurrentHisto->GetBinContent(bin);
	}
      
      if(butt < fThreshold)
      	continue;
      
      fXPeaks[fNPeaks] = fCurrentHisto->GetBinCenterX(windowPt[i]->xbin);
      fYPeaks[fNPeaks] = top/butt;
      fWeight[fNPeaks] = (Int_t)butt;
      //cout<<"mean in y "<<ypeaks[n]<<" on x "<<windowPt[i]->xbin<<" content "<<butt<<endl;
      fNPeaks++;
      if(fNPeaks==fNMax) 
	{
	  cerr<<"AliL3HoughMaxFinder::FindPeak1 : Peak array out of range!!!"<<endl;
	  break;
	}
    }

  
  //Improve the peaks by including the region around in x.
  Float_t ytop,ybutt;
  Int_t prev;
  Int_t w;
  for(Int_t i=0; i<fNPeaks; i++)
    {
      Int_t xbin = fCurrentHisto->FindXbin(fXPeaks[i]);
      if(xbin - x_bin_sides < xmin || xbin + x_bin_sides > xmax) continue;
      top=butt=0;
      ytop=0,ybutt=0;	  
      w=0;
      prev = xbin - x_bin_sides+1;
      for(Int_t j=xbin-x_bin_sides; j<=xbin+x_bin_sides; j++)
	{
	  /*
	  //Check if the windows are overlapping:
	  if(anotherPt[j]->ymin > anotherPt[prev]->ymax) {prev=j; continue;}
	  if(anotherPt[j]->ymax < anotherPt[prev]->ymin) {prev=j; continue;}
	  prev = j;
	  */
	  
	  top += fCurrentHisto->GetBinCenterX(j)*anotherPt[j]->weight;
	  butt += anotherPt[j]->weight;
	  
	  for(Int_t k=anotherPt[j]->ymin; k<anotherPt[j]->ymax; k++)
	    {
	      Int_t bin = fCurrentHisto->GetBin(j,k);
	      ytop += (fCurrentHisto->GetBinCenterY(k))*(fCurrentHisto->GetBinContent(bin));
	      ybutt += fCurrentHisto->GetBinContent(bin);
	      w+=(Int_t)fCurrentHisto->GetBinContent(bin);
	    }
	}
      
      fXPeaks[i] = top/butt;
      fYPeaks[i] = ytop/ybutt;
      fWeight[i] = w;
      //cout<<"Setting weight "<<w<<" kappa "<<fXPeaks[i]<<" phi0 "<<fYPeaks[i]<<endl;
      
      /*
      //Check if this peak is overlapping with a previous:
      for(Int_t p=0; p<i; p++)
	{
	  //cout<<fabs(fXPeaks[p] - fXPeaks[i])<<" "<<fabs(fYPeaks[p] - fYPeaks[i])<<endl;
	  if(fabs(fXPeaks[p] - fXPeaks[i]) < max_kappa &&
	     fabs(fYPeaks[p] - fYPeaks[i]) < max_phi0)
	    {
	      fWeight[i]=0;
	      //break;
	    }
	}
      */
    }
  
  for(Int_t i=0; i<nbinsx; i++)
    delete windowPt[i];
  delete [] windowPt;
  delete [] anotherPt;
}

void AliL3HoughMaxFinder::SortPeaks(struct AxisWindow **a,Int_t first,Int_t last)
{
  //General sorting routine
  //Sort according to PeakCompare()

  static struct AxisWindow *tmp;
  static int i;           // "static" to save stack space
  int j;
  
  while (last - first > 1) {
    i = first;
    j = last;
    for (;;) {
      while (++i < last && PeakCompare(a[i], a[first]) < 0)
	;
      while (--j > first && PeakCompare(a[j], a[first]) > 0)
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
      SortPeaks(a, first, j);
      first = j + 1;   // QSort(j + 1, last);
    } else {
      SortPeaks(a, j + 1, last);
      last = j;        // QSort(first, j);
    }
  }
  
}

Int_t AliL3HoughMaxFinder::PeakCompare(struct AxisWindow *a,struct AxisWindow *b)
{
  if(a->weight < b->weight) return 1;
  if(a->weight > b->weight) return -1;
  return 0;
}

void AliL3HoughMaxFinder::FindPeak(Int_t t1,Double_t t2,Int_t t3)
{
  //Attempt of a more sophisticated peak finder.
  //Finds the best peak in the histogram, and returns the corresponding
  //track object.

  if(!fCurrentHisto)
    {
      printf("AliL3HoughMaxFinder::FindPeak : No histogram!!\n");
      return;
    }
  AliL3Histogram *hist = fCurrentHisto;
  if(hist->GetNEntries()==0)
    return;

  Int_t xmin = hist->GetFirstXbin();
  Int_t xmax = hist->GetLastXbin();
  Int_t ymin = hist->GetFirstYbin();
  Int_t ymax = hist->GetLastYbin();
  Int_t nbinsx = hist->GetNbinsX()+1;
  
  Int_t *m = new Int_t[nbinsx];
  Int_t *m_low = new Int_t[nbinsx];
  Int_t *m_up = new Int_t[nbinsx];
  
  
 recompute:  //this is a goto.
  
  for(Int_t i=0; i<nbinsx; i++)
    {
      m[i]=0;
      m_low[i]=0;
      m_up[i]=0;
    }

  Int_t max_x=0,sum=0,max_xbin=0,bin=0;

  for(Int_t xbin=xmin; xbin<=xmax; xbin++)
    {
      for(Int_t ybin=ymin; ybin <= ymax - t1; ybin++)
	{
	  sum = 0;
	  for(Int_t y=ybin; y <= ybin+t1; y++)
	    {
	      if(y>ymax) break;
	      //Inside window
	      bin = hist->GetBin(xbin,y);
	      sum += (Int_t)hist->GetBinContent(bin);
	      
	    }
	  if(sum > m[xbin]) //Max value locally in this xbin
	    {
	      m[xbin]=sum;
	      m_low[xbin]=ybin;
	      m_up[xbin]=ybin + t1;
	    }
	  
	}
      
      if(m[xbin] > max_x) //Max value globally in x-direction
	{
	  max_xbin = xbin;
	  max_x = m[xbin];//sum;
	}
    }
  //printf("max_xbin %d max_x %d m_low %d m_up %d\n",max_xbin,max_x,m_low[max_xbin],m_up[max_xbin]);
  //printf("ylow %f yup %f\n",hist->GetBinCenterY(m_low[max_xbin]),hist->GetBinCenterY(m_up[max_xbin]));

  //Determine a width in the x-direction
  Int_t x_low=0,x_up=0;
  
  for(Int_t xbin=max_xbin-1; xbin >= xmin; xbin--)
    {
      if(m[xbin] < max_x*t2)
	{
	  x_low = xbin+1;
	  break;
	}
    }
  for(Int_t xbin = max_xbin+1; xbin <=xmax; xbin++)
    {
      if(m[xbin] < max_x*t2)
	{
	  x_up = xbin-1;
	  break;
	}
    }
  
  Double_t top=0,butt=0,value,x_peak;
  if(x_up - x_low + 1 > t3)
    {
      t1 -= 1;
      printf("\nxrange out if limit x_up %d x_low %d t1 %d\n\n",x_low,x_up,t1);
      if(t1 > 1)
	goto recompute;
      else
	{
	  x_peak = hist->GetBinCenterX(max_xbin);
	  goto moveon;
	}
    }
  
  //printf("xlow %f xup %f\n",hist->GetBinCenterX(x_low),hist->GetBinCenterX(x_up));
  //printf("Spread in x %d\n",x_up-x_low +1);

  //Now, calculate the center of mass in x-direction
  for(Int_t xbin=x_low; xbin <= x_up; xbin++)
    {
      value = hist->GetBinCenterX(xbin);
      top += value*m[xbin];
      butt += m[xbin];
    }
  x_peak = top/butt;
  
 moveon:
  
  //Find the peak in y direction:
  Int_t x_l = hist->FindXbin(x_peak);
  if(hist->GetBinCenterX(x_l) > x_peak)
    x_l--;

  Int_t x_u = x_l + 1;
  
  if(hist->GetBinCenterX(x_l) > x_peak || hist->GetBinCenterX(x_u) <= x_peak)
    printf("\nAliL3HoughMaxFinder::FindPeak : Wrong xrange %f %f %f\n\n",hist->GetBinCenterX(x_l),x_peak,hist->GetBinCenterX(x_u));
    
    //printf("\nxlow %f xup %f\n",hist->GetBinCenterX(x_l),hist->GetBinCenterX(x_u));

  value=top=butt=0;
  
  //printf("ylow %f yup %f\n",hist->GetBinCenterY(m_low[x_l]),hist->GetBinCenterY(m_up[x_l]));
  //printf("ylow %f yup %f\n",hist->GetBinCenterY(m_low[x_u]),hist->GetBinCenterY(m_up[x_u]));
  
  for(Int_t ybin=m_low[x_l]; ybin <= m_up[x_l]; ybin++)
    {
      value = hist->GetBinCenterY(ybin);
      bin = hist->GetBin(x_l,ybin);
      top += value*hist->GetBinContent(bin);
      butt += hist->GetBinContent(bin);
    }
  Double_t y_peak_low = top/butt;
  
  //printf("y_peak_low %f\n",y_peak_low);

  value=top=butt=0;
  for(Int_t ybin=m_low[x_u]; ybin <= m_up[x_u]; ybin++)
    {
      value = hist->GetBinCenterY(ybin);
      bin = hist->GetBin(x_u,ybin);
      top += value*hist->GetBinContent(bin);
      butt += hist->GetBinContent(bin);
    }
  Double_t y_peak_up = top/butt;
  
  //printf("y_peak_up %f\n",y_peak_up);

  Double_t x_value_up = hist->GetBinCenterX(x_u);
  Double_t x_value_low = hist->GetBinCenterX(x_l);

  Double_t y_peak = (y_peak_low*(x_value_up - x_peak) + y_peak_up*(x_peak - x_value_low))/(x_value_up - x_value_low);


  //Find the weight:
  //bin = hist->FindBin(x_peak,y_peak);
  //Int_t weight = (Int_t)hist->GetBinContent(bin);

  //AliL3HoughTrack *track = new AliL3HoughTrack();
  //track->SetTrackParameters(x_peak,y_peak,weight);
  fXPeaks[fNPeaks]=x_peak;
  fYPeaks[fNPeaks]=y_peak;
  fWeight[fNPeaks]=(Int_t)hist->GetBinContent(bin);
  fNPeaks++;
  
  delete [] m;
  delete [] m_low;
  delete [] m_up;
  
  //return track;
}

