// @(#) $Id$

// Author: Constantin Loizides <mailto:loizides@fi.uib.no>
//*-- Copyright &copy ALICE HLT Group

#include "AliL3StandardIncludes.h"

#include "AliL3RootTypes.h"
#include "AliL3Logging.h"
#include "AliL3MemHandler.h"
#include "AliL3Transform.h"
#include "AliL3DigitData.h"
#include "AliL3HoughTransformerVhdl.h"
#include "AliL3FFloat.h"

#if GCCVERSION == 3
using namespace std;
#endif

/** \class AliL3HoughTransformerVhdl
// <pre>
//_____________________________________________________________
// AliL3HoughTransformerVhdl
//
// Hough transformation class for VHDL comparism.
//
//</pre>
*/

ClassImp(AliL3HoughTransformerVhdl)


AliL3HoughTransformerVhdl::AliL3HoughTransformerVhdl()
  : AliL3HoughTransformerLUT()

{
  fEpsilon=0;
  fSinEpsilon=0;
  fCosEpsilon=1;
  fIts=0;
}

AliL3HoughTransformerVhdl::AliL3HoughTransformerVhdl(Int_t slice,Int_t patch,Int_t n_eta_segments,Int_t n_its) 
  : AliL3HoughTransformerLUT(slice,patch,n_eta_segments)
{
  fEpsilon=0;
  fSinEpsilon=0;
  fCosEpsilon=1;
  fIts=n_its;
}

AliL3HoughTransformerVhdl::~AliL3HoughTransformerVhdl()
{

}

void AliL3HoughTransformerVhdl::CreateHistograms(Int_t nxbin,Double_t pt_min,Int_t nybin,Double_t phimin,Double_t phimax)
{
  AliL3HoughTransformerLUT::CreateHistograms(nxbin,pt_min,nybin,phimin,phimax);
}

void AliL3HoughTransformerVhdl::CreateHistograms(Int_t nxbin,Double_t xmin,Double_t xmax,
					         Int_t nybin,Double_t ymin,Double_t ymax)
{
  AliL3HoughTransformerLUT::CreateHistograms(nxbin,xmin,xmax,nybin,ymin,ymax);

  fEpsilon=(ymax-ymin)/nybin;
  fSinEpsilon=sin(fEpsilon);
  fCosEpsilon=cos(fEpsilon);

  fNxbin=nxbin;
  fXmin=xmin;
  fXmax=xmax;
  fNybin=nybin;
  fYmin=ymin;
  fYmax=ymax;
  //cout << fEpsilon << " - " << (xmax-xmin)/nxbin << endl;
}

void AliL3HoughTransformerVhdl::Init(Int_t slice,Int_t patch,Int_t n_eta_segments,Int_t n_its)
{
  AliL3HoughTransformerLUT::Init(slice,patch,n_eta_segments);
}

void AliL3HoughTransformerVhdl::TransformCircle()
{
  //Transform the input data with a circle HT.
  //The function loops over all the data, and transforms each pixel with the equation:
  // 
  //kappa      = lastkappa +- epsilon * lastkappaprime
  //
  //kappaprime = lastkappaprime -+ epsilon * lastkappa 
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

//#define use_error
#ifdef use_error
      Float_t max_error=0;
      Float_t rel_error=0;
      Int_t counter=0;
#endif
  
  //Loop over the padrows:
  for(Int_t i=fMinRow, row=0; i<=fMaxRow; i++, row++)
    {
      //Get the data on this padrow:
      AliL3DigitData *digPt = tempPt->fDigitData;
      if(i != (Int_t)tempPt->fRow)
	{
	  LOG(AliL3Log::kError,"AliL3HoughTransformerVhdl::TransformCircle","Data")
	    <<"AliL3HoughTransformerLUT::TransformCircle : Mismatching padrow numbering "<<i<<" != "<<(Int_t)tempPt->fRow<<ENDLOG;
	  continue;
	}

      Float_t x = CalcX(row);
      Float_t x2=x*x;
      Float_t y=0,y2=0;
      Float_t r2=0;
      Float_t R2=0;
      fLastPad=-1;


      //Loop over the data on this padrow:
      for(UInt_t j=0; j<tempPt->fNDigit; j++)
	{
	  UShort_t charge = digPt[j].fCharge;

	  //check threshold
	  if((Int_t)charge <= GetLowerThreshold() || (Int_t)charge > GetUpperThreshold())
	    continue;

	  UChar_t pad = digPt[j].fPad;
	  UShort_t time = digPt[j].fTime;

	  if(fLastPad!=pad){ //only update if necessary
	    fLastIndex=fNEtas-1;   
	    y = CalcY(pad,row);
	    y2 = y*y;
	    r2 = x2 + y2;
	    R2 = 1. / r2;
	    for(Int_t b=0; b<fNPhi0; b++)
	      fLUTKappa[b]=R2*(y*fLUT2cosphi0[b]-x*fLUT2sinphi0[b]);

	    //Fill the histogram along the phirange
	    Float_t kappa=0;
	    Float_t kappaprime=0;
	    Float_t lastkappa=0;
	    Float_t lastkappaprime=0;
	    Int_t its=0;
	    Float_t R2=1/(x*x+y*y);
	    Float_t A=2*y*R2;
	    Float_t B=-2*x*R2;

	    //starting point
	    Float_t phi = fLUTphi0[fNPhi0/2];
	    kappa=A*cos(phi)+B*sin(phi);
	    kappaprime=B*cos(phi)-A*sin(phi);
	    its=fIts;
	    lastkappa=kappa;
	    lastkappaprime=kappaprime;
	    //	  hist->Fill(kappa,phi,charge);

	    for(Int_t b=fNPhi0/2+1; b<fNPhi0; b++){ 
	      //Float_t exact_kappa=R2*(y*fLUT2cosphi0[b]-x*fLUT2sinphi0[b]);
	      
	      phi=fLUTphi0[b];

	      if(its==0) { //initialize or re-adjust values
		kappa=A*cos(phi)+B*sin(phi); //equals exact_kappa!
		kappaprime=B*cos(phi)-A*sin(phi);
		its=fIts;
	      } else {
		//kappa=fCosEpsilon*lastkappa+fSinEpsilon*lastkappaprime;
		//kappaprime=fCosEpsilon*lastkappaprime-fSinEpsilon*lastkappa;
		kappa=lastkappa+fEpsilon*lastkappaprime;
		kappaprime=lastkappaprime-fEpsilon*lastkappa;
		its--;
	      }

	      lastkappa=kappa;
	      lastkappaprime=kappaprime;
	      //hist->Fill(kappa,phi,charge);
	    } // end positive running values

	    phi = fLUTphi0[fNPhi0/2];
	    kappa=A*cos(phi)+B*sin(phi);
	    kappaprime=B*cos(phi)-A*sin(phi);
	    its=fIts;
	    lastkappa=kappa;
	    lastkappaprime=kappaprime;
	    //hist->Fill(kappa,fLUTphi0[b],charge);

	    for(Int_t b=fNPhi0/2-1; b>=0; b--){ 
	      //Float_t exact_kappa=R2*(y*fLUT2cosphi0[b]-x*fLUT2sinphi0[b]);

	      Float_t phi = fLUTphi0[b];

	      if(its==0) { //initialize or re-adjust values
		kappa=A*cos(phi)+B*sin(phi); //equals exact_kappa!
		kappaprime=B*cos(phi)-A*sin(phi);
		its=fIts;
	      } else {
		//kappa=fCosEpsilon*lastkappa-fSinEpsilon*lastkappaprime;
		//kappaprime=fCosEpsilon*lastkappaprime+fSinEpsilon*lastkappa;
		kappa=lastkappa-fEpsilon*lastkappaprime;
		kappaprime=lastkappaprime+fEpsilon*lastkappa;
		its--;
	      }

	      lastkappa=kappa;
	      lastkappaprime=kappaprime;
	      //hist->Fill(kappa,phi,charge);
	    }

	    fLastPad=pad;
	  }

	  Float_t z = CalcZ(time);

	  //find eta slice
	  Float_t rz2 = 1 + (x*x+y*y)/(z*z);
	  Int_t eta_index = FindIndex(rz2);

	  if(eta_index < 0 || eta_index >= fNEtas){
	    //LOG(AliL3Log::kWarning,"AliL3HoughTransformerVhdl::TransformCircle","Histograms")<<"No histograms corresponding to eta index value of "<<eta_index<<"."<<ENDLOG;
	    continue;
	  }	  

	  //Get the correct histogrampointer:
	  AliL3Histogram *hist = fParamSpace[eta_index];
	  if(!hist){
	    //LOG(AliL3Log::kWarning,"AliL3HoughTransformerVhdl::TransformCircle","Histograms")<<"Error getting histogram in index "<<eta_index<<"."<<ENDLOG;
	    continue;
	  }

	  for(Int_t b=0; b<fNPhi0; b++){ 

#ifdef use_error
	    Float_t err=fabs((exact_kappa-kappa)/exact_kappa);

	    if(err>max_error) {
	      //cout << max_error << " - " << err << " " << kappa << " " << exact_kappa << " " << kappa/exact_kappa<< endl;
	      max_error=err;
	    }

	    rel_error+=err;
	    counter++;
#endif
	    //hist->Fill(kappa,fLUTphi0[b],charge);

	    //cout << kappa << " " << fLUTphi0[b] << " " << charge << endl;
	  }
	}
      //Move the data pointer to the next padrow:
      AliL3MemHandler::UpdateRowPointer(tempPt);
    }

#ifdef use_error
  cout <<"Max Error: " << max_error << endl;
  cout <<"Rel Error average: " << rel_error/counter << endl;
#endif

}

void AliL3HoughTransformerVhdl::Print()
{
  AliL3HoughTransformerLUT::Print();

  cout << "fEpsilon: " << fEpsilon << endl;
  cout << "fIts: " << fIts << endl;
}

void AliL3HoughTransformerVhdl::PrintVhdl()
{
  cout << "fSlice := " << GetSlice() << ";" << endl;
  cout << "fPatch := " << GetPatch() << ";" << endl;
  //cout << "fSector := " << fSector << ";" << endl;
  //cout << "fSectorRow := " << fSectorRow << ";" << endl;
  //cout << "fMinRow := " << fMinRow << ";" << endl;
  //cout << "fMaxRow := " << fMaxRow << ";" << endl;
  //cout << "fNRows := " << fNRows << ";" << endl;
  //cout << "fNEtas := " << fNEtas << ";" << endl;
  //cout << "fNPhi0 := " << fNPhi0 << ";" << endl;
  cout << "fZSign := " << fZSign << ";" << endl;
  cout << "fZLengthPlusOff := " << fZLengthPlusOff << ";" << endl;
  cout << "fPadPitch := " << fPadPitch << ";" << endl;
  cout << "fTimeWidth := " << fTimeWidth << ";" << endl;
  
  if(!fNRows) return;
  cout << "fNLUTX :=" << fNRows << ";" << endl;
  cout << "fLUTX := ( ";
  for(Int_t i=0;i<fNRows-1;i++) cout << fLUTX[i] << ", ";
  cout << fLUTX[fNRows-1] << " );\n" << endl; 
  cout << "fNLUTY := " << fNRows << ";" << endl;
  cout << "fLUTY := ( ";
  for(Int_t i=0;i<fNRows-1;i++) cout << fLUTY[i] << ", ";
  cout << fLUTY[fNRows-1] << " );\n" << endl;
 
  if(!fNEtas) return;
  cout << "fNLUTEta := " << fNEtas << ";" << endl;
  cout << "fLUTEta := ("; 
  for(Int_t i=0;i<fNEtas-1;i++) cout << fLUTEta[i] << ", ";
  cout << fLUTEta[fNEtas-1] << " );\n" << endl; 

  if(!fNPhi0) return;
  cout << "-- Kappa -- " << endl;
  cout << "fNxbin := " << fNxbin << endl;
  cout << "fxmin := " << fXmin << endl;
  cout << "fxmax := " << fXmax << endl;
  Float_t kdiff=(fXmax-fXmin)/fNxbin;
  Float_t k=fXmin;
  cout << "fKappa := ";
  for(Int_t i=0;i<fNxbin;i++){
    k+=kdiff;
    cout << k << ", ";
  }
  cout << k+kdiff << " );\n" << endl;

  cout << "-- Phi --" << endl;
  cout << "fNybin := " << fNybin << endl;
  cout << "fymin := " << fYmin << endl;
  cout << "fymax := " << fYmax << endl;

  cout << "\nfNLUTphi0 := " << fNPhi0 << ";" << endl;
  cout << "fLUTphi0 := (";
  for(Int_t i=0;i<fNPhi0-1;i++) cout << fLUTphi0[i] << ", ";
  cout << fLUTphi0[fNPhi0-1] << " );\n" << endl; 

  cout << "\nfNLUT2sinphi0 := " << fNPhi0 << ";" << endl;
  cout << "fLUT2sinphi0 := (";
  for(Int_t i=0;i<fNPhi0-1;i++) cout << fLUT2sinphi0[i] << ", ";
  cout << fLUT2sinphi0[fNPhi0-1] << " );\n" << endl; 

  cout << "\nfNLUT2cosphi0 := " << fNPhi0 << ";" << endl;
  cout << "fLUT2cosphi0 := (";
  for(Int_t i=0;i<fNPhi0-1;i++) cout << fLUT2cosphi0[i] << ", ";
  cout << fLUT2cosphi0[fNPhi0-1] << " );\n" << endl; 

  //cout << "\nfEpsilon := " << fEpsilon << endl;
  //cout << "\nfSinEpsilon := " << fSinEpsilon << endl;
  //cout << "\nfCosEpsilon := " << fCosEpsilon << endl;
}
