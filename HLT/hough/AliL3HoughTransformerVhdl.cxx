//$Id$

// Author: Constantin Loizides <mailto:loizides@fi.uib.no>
//*-- Copyright &copy CL and ASV

#include "AliL3MemHandler.h"
#include "AliL3Logging.h"
#include "AliL3Transform.h"
#include "AliL3DigitData.h"
#include "AliL3Histogram.h"
#include "AliL3HoughTransformerVhdl.h"

//_____________________________________________________________
// AliL3HoughTransformerVhdl
//
// Hough transformation class
//

ClassImp(AliL3HoughTransformerVhdl)

AliL3HoughTransformerVhdl::AliL3HoughTransformerVhdl()
{
  //Default constructor
  fParamSpace = 0;
}

AliL3HoughTransformerVhdl::AliL3HoughTransformerVhdl(Int_t slice,Int_t patch,Int_t n_eta_segments) : AliL3HoughBaseTransformer(slice,patch,n_eta_segments)
{
  //Normal constructor
  fParamSpace = 0;
}

AliL3HoughTransformerVhdl::~AliL3HoughTransformerVhdl()
{
  DeleteHistograms();
}

void AliL3HoughTransformerVhdl::DeleteHistograms()
{
  if(!fParamSpace)
    return;
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    {
      if(!fParamSpace[i]) continue;
      delete fParamSpace[i];
    }
  delete [] fParamSpace;
}

void AliL3HoughTransformerVhdl::CreateHistograms(Int_t nxbin,Double_t pt_min,
					     Int_t nybin,Double_t phimin,Double_t phimax)
{
  //Create the histograms (parameter space).
  //These are 2D histograms, span by kappa (curvature of track) and phi0 (emission angle with x-axis).
  //The arguments give the range and binning; 
  //nxbin = #bins in kappa
  //nybin = #bins in phi0
  //pt_min = mimium Pt of track (corresponding to maximum kappa)
  //phi_min = mimimum phi0 (degrees)
  //phi_max = maximum phi0 (degrees)
    
  Double_t bfact = 0.0029980;
  Double_t bfield = AliL3Transform::GetBField();
  Double_t x = bfact*bfield/pt_min;
  Double_t torad = AliL3Transform::Pi()/180;
  CreateHistograms(nxbin,-1.*x,x,nybin,phimin*torad,phimax*torad);
}

void AliL3HoughTransformerVhdl::CreateHistograms(Int_t nxbin,Double_t xmin,Double_t xmax,
					     Int_t nybin,Double_t ymin,Double_t ymax)
{
  
  fParamSpace = new AliL3Histogram*[GetNEtaSegments()];
  
  Char_t histname[256];
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    {
      sprintf(histname,"paramspace_%d",i);
      fParamSpace[i] = new AliL3Histogram(histname,"",nxbin,xmin,xmax,nybin,ymin,ymax);
    }
}

void AliL3HoughTransformerVhdl::Reset()
{
  //Reset all the histograms

  if(!fParamSpace)
    {
      LOG(AliL3Log::kWarning,"AliL3HoughTransformerVhdl::Reset","Histograms")
	<<"No histograms to reset"<<ENDLOG;
      return;
    }
  
  for(Int_t i=0; i<GetNEtaSegments(); i++)
    fParamSpace[i]->Reset();
}


Int_t AliL3HoughTransformerVhdl::GetEtaIndex(Double_t eta)
{
  //Return the histogram index of the corresponding eta. 

  Double_t etaslice = (GetEtaMax() - GetEtaMin())/GetNEtaSegments();
  Double_t index = (eta-GetEtaMin())/etaslice;
  return (Int_t)index;
}

void AliL3HoughTransformerVhdl::TransformCircle()
{
  //Transform the input data with a circle HT.
  //The function loops over all the data, and transforms each pixel with the equations:
  // 
  //kappa = 2/R*sin(phi - phi0)
  //
  //where R = sqrt(x*x +y*y), and phi = arctan(y/x)
  //
  //Each pixel then transforms into a curve in the (kappa,phi0)-space. In order to find
  //which histogram in which the pixel should be transformed, the eta-value is calcluated
  //and the proper histogram index is found by GetEtaIndex(eta).


  AliL3DigitRowData *tempPt = GetDataPointer();
  if(!tempPt)
    {
      LOG(AliL3Log::kError,"AliL3HoughTransformerVhdl::TransformCircle","Data")
	<<"No input data "<<ENDLOG;
      return;
    }
  
  //Loop over the padrows:
  for(Int_t i=AliL3Transform::GetFirstRow(GetPatch()); i<=AliL3Transform::GetLastRow(GetPatch()); i++)
    {
      //Get the data on this padrow:
      AliL3DigitData *digPt = tempPt->fDigitData;
      if(i != (Int_t)tempPt->fRow)
	{
	  printf("AliL3HoughTransform::TransformCircle : Mismatching padrow numbering\n");
	  continue;
	}
      
      //Loop over the data on this padrow:
      for(UInt_t j=0; j<tempPt->fNDigit; j++)
	{
	  UShort_t charge = digPt[j].fCharge;
	  UChar_t pad = digPt[j].fPad;
	  UShort_t time = digPt[j].fTime;
	  if((Int_t)charge <= GetLowerThreshold())
	    continue;
	  Int_t sector,row;
	  Float_t xyz[3];
	  
	  //Transform data to local cartesian coordinates:
	  AliL3Transform::Slice2Sector(GetSlice(),i,sector,row);
	  AliL3Transform::Raw2Local(xyz,sector,row,(Int_t)pad,(Int_t)time);
	  
	  //Calculate the eta:
	  Double_t eta = AliL3Transform::GetEta(xyz);
	  
	  //Get the corresponding index, which determines which histogram to fill:
	  Int_t eta_index = GetEtaIndex(eta);
	  if(eta_index < 0 || eta_index >= GetNEtaSegments())
	    continue;
	  
	  //Get the correct histogrampointer:
	  AliL3Histogram *hist = fParamSpace[eta_index];
	  if(!hist)
	    {
	      printf("AliL3HoughTransformerVhdl::TransformCircle : Error getting histogram in index %d\n",eta_index);
	      continue;
	    }

	  //Do the transformation:
	  Float_t R = sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1]); 
	  Float_t phi = AliL3Transform::GetPhi(xyz);
	  
	  //Fill the histogram along the phirange
	  for(Int_t b=hist->GetFirstYbin(); b<=hist->GetLastYbin(); b++)
	    {
	      Float_t phi0 = hist->GetBinCenterY(b);
	      Float_t kappa = 2*sin(phi - phi0)/R;
	      hist->Fill(kappa,phi0,charge);
	    }
	}
      
      //Move the data pointer to the next padrow:
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }
}

void AliL3HoughTransformerVhdl::TransformCircleC(Int_t row_range)
{
  //Circle transform, using combinations of every 2 points lying
  //on different padrows and within the same etaslice.
  
  AliL3DigitRowData *tempPt = GetDataPointer();
  if(!tempPt)
    LOG(AliL3Log::kError,"AliL3HoughTransformerVhdl::TransformCircleC","Data")
      <<"No input data "<<ENDLOG;
 
  Int_t counter=0;
  for(Int_t i=AliL3Transform::GetFirstRow(GetPatch()); i<=AliL3Transform::GetLastRow(GetPatch()); i++)
    {
      counter += tempPt->fNDigit;
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }
  
  struct Digit {
    Int_t row;
    Double_t r;
    Double_t phi;
    Int_t eta_index;
    Int_t charge;
  };
  
  Digit *digits = new Digit[counter];
  cout<<"Allocating "<<counter*sizeof(Digit)<<" bytes to digitsarray"<<endl;
  
  Int_t total_digits=counter;
  Int_t sector,row,tot_charge,pad,time,charge;
  Double_t r1,r2,phi1,phi2,eta,kappa,phi_0;
  Float_t xyz[3];
  
  counter=0;
  tempPt = GetDataPointer();
  
  for(Int_t i=AliL3Transform::GetFirstRow(GetPatch()); i<=AliL3Transform::GetLastRow(GetPatch()); i++)
    {
      AliL3DigitData *digPt = tempPt->fDigitData;
      for(UInt_t di=0; di<tempPt->fNDigit; di++)
	{
	  charge = digPt[di].fCharge;
	  pad = digPt[di].fPad;
	  time = digPt[di].fTime;
	  AliL3Transform::Slice2Sector(GetSlice(),i,sector,row);
	  AliL3Transform::Raw2Local(xyz,sector,row,(Int_t)pad,(Int_t)time);
	  eta = AliL3Transform::GetEta(xyz);
	  digits[counter].row = i;
	  digits[counter].r = sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1]);
	  digits[counter].phi = atan2(xyz[1],xyz[0]);
	  digits[counter].eta_index = GetEtaIndex(eta);
	  digits[counter].charge = charge;
	  counter++;
	}
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }
  
  for(Int_t i=0; i<total_digits; i++)
    {
      if(digits[i].eta_index < 0 || digits[i].eta_index >= GetNEtaSegments()) continue;
      Int_t ind = digits[i].eta_index;
      
      for(Int_t j=i+1; j<total_digits; j++)
	{
	  if(digits[i].row == digits[j].row) continue;
	  if(digits[i].eta_index != digits[j].eta_index) continue;
	  if(digits[i].row + row_range < digits[j].row) break;
	  
	  //Get the correct histogrampointer:
	  AliL3Histogram *hist = fParamSpace[ind];
	  if(!hist)
	    {
	      printf("AliL3HoughTransformerVhdl::TransformCircleC() : No histogram at index %d\n",ind);
	      continue;
	    }
	  
	  r1 = digits[i].r;
	  phi1 = digits[i].phi;
	  r2 = digits[j].r;
	  phi2 = digits[j].phi;
	  phi_0 = atan( (r2*sin(phi1)-r1*sin(phi2))/(r2*cos(phi1)-r1*cos(phi2)) );
	  kappa = 2*sin(phi2-phi_0)/r2;
	  tot_charge = digits[i].charge + digits[j].charge;
	  hist->Fill(kappa,phi_0,tot_charge);
	}
    }
  delete [] digits;
}

void AliL3HoughTransformerVhdl::TransformLine()
{
  //Do a line transform on the data.

  
  AliL3DigitRowData *tempPt = GetDataPointer();
  if(!tempPt)
    {
      LOG(AliL3Log::kError,"AliL3HoughTransformerVhdl::TransformLine","Data")
	<<"No input data "<<ENDLOG;
      return;
    }
    
  for(Int_t i=AliL3Transform::GetFirstRow(GetPatch()); i<=AliL3Transform::GetLastRow(GetPatch()); i++)
    {
      AliL3DigitData *digPt = tempPt->fDigitData;
      if(i != (Int_t)tempPt->fRow)
	{
	  printf("AliL3HoughTransform::TransformLine : Mismatching padrow numbering\n");
	  continue;
	}
      for(UInt_t j=0; j<tempPt->fNDigit; j++)
	{
	  UShort_t charge = digPt[j].fCharge;
	  UChar_t pad = digPt[j].fPad;
	  UShort_t time = digPt[j].fTime;
	  if((Int_t)charge < GetLowerThreshold())
	    continue;
	  Int_t sector,row;
	  Float_t xyz[3];
	  AliL3Transform::Slice2Sector(GetSlice(),i,sector,row);
	  AliL3Transform::Raw2Local(xyz,sector,row,(Int_t)pad,(Int_t)time);
	  Float_t eta = AliL3Transform::GetEta(xyz);
	  Int_t eta_index = GetEtaIndex(eta);//(Int_t)(eta/etaslice);
	  if(eta_index < 0 || eta_index >= GetNEtaSegments())
	    continue;
	  
	  //Get the correct histogram:
	  AliL3Histogram *hist = fParamSpace[eta_index];
	  if(!hist)
	    {
	      printf("AliL3HoughTransformerVhdl::TransformLine : Error getting histogram in index %d\n",eta_index);
	      continue;
	    }
	  for(Int_t xbin=hist->GetFirstXbin(); xbin<hist->GetLastXbin(); xbin++)
	    {
	      Double_t theta = hist->GetBinCenterX(xbin);
	      Double_t rho = xyz[0]*cos(theta) + xyz[1]*sin(theta);
	      hist->Fill(theta,rho,charge);
	    }
	}
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }
  
}
