// Author: Anders Vestbo <mailto:vestbo@fi.uib.no>
//*-- Copyright &copy ASV

#include "AliL3Logging.h"
#include "AliL3Histogram1D.h"

//_____________________________________________________________
// AliL3Histogram1D
//
// 1D histogram class.

ClassImp(AliL3Histogram1D)

AliL3Histogram1D::AliL3Histogram1D()
{
  fNbins = 0;
  fNcells = 0;
  fEntries = 0;
  fXmin = 0;
  fXmax = 0;
#ifdef use_root
  fRootHisto = 0;
#endif
  fThreshold = 0;
  fContent = 0;
  
}

  
AliL3Histogram1D::AliL3Histogram1D(Char_t *name,Char_t *id,Int_t nxbin,Double_t xmin,Double_t xmax)

{
  
  strcpy(fName,name);
  fNbins = nxbin;
  fNcells = fNbins + 2;
  fEntries = 0;
  fXmin = xmin;
  fXmax = xmax;
  
  fRootHisto = 0;
  fThreshold = 0;
  
  fContent = new Double_t[fNcells];
  Reset();
}

AliL3Histogram1D::~AliL3Histogram1D()
{
  //Destructor
  if(fContent)
    delete [] fContent;
#ifdef use_root
  if(fRootHisto)
    delete fRootHisto;
#endif
}


void AliL3Histogram1D::Reset()
{
  bzero(fContent,fNcells*sizeof(Double_t));
  fEntries=0;
}

void AliL3Histogram1D::Fill(Double_t x,Int_t weight)
{
  Int_t bin = FindBin(x);
  AddBinContent(bin,weight);
}


Int_t AliL3Histogram1D::FindBin(Double_t x)
{
  if(x < fXmin || x > fXmax)
    return 0;
  
  return 1 + (Int_t)(fNbins*(x-fXmin)/(fXmax-fXmin));

}

Int_t AliL3Histogram1D::GetMaximumBin()
{
  Double_t max_value=0;
  Int_t max_bin=0;
  for(Int_t i=0; i<fNcells; i++)
    {
      if(fContent[i] > max_value)
	{
	  max_value=fContent[i];
	  max_bin = i;
	}
    }
  return max_bin;
}

Double_t AliL3Histogram1D::GetBinContent(Int_t bin)
{
  if(bin >= fNcells)
    {
      LOG(AliL3Log::kError,"AliL3Histogram::GetBinContent","array")<<AliL3Log::kDec<<
	"bin out of range "<<bin<<ENDLOG;
      return 0;
    }
  
  if(fContent[bin] < fThreshold)
    return 0;
  return fContent[bin];
}


void AliL3Histogram1D::SetBinContent(Int_t bin,Int_t value)
{

  if(bin >= fNcells)
    {
      LOG(AliL3Log::kError,"AliL3Histogram::SetBinContent","array")<<AliL3Log::kDec<<
	"bin out of range "<<bin<<ENDLOG;
      return;
    }
  if(bin == 0)
    return;
  fContent[bin]=value;
  
}

void AliL3Histogram1D::AddBinContent(Int_t bin,Int_t weight)
{
  if(bin < 0 || bin > fNcells)
    {
      LOG(AliL3Log::kError,"AliL3Histogram::AddBinContent","array")<<AliL3Log::kDec<<
	"bin-value out of range "<<bin<<ENDLOG;
      return;
    }
  if(bin == 0)
    return;
  fEntries++;
  fContent[bin] += weight;
}

Double_t AliL3Histogram1D::GetBinCenter(Int_t bin)
{
  
  Double_t binwidth = (fXmax - fXmin) / fNbins;
  return fXmin + (bin-1) * binwidth + 0.5*binwidth;
  
}

#ifdef use_root
void AliL3Histogram1D::Draw(Char_t *option)
{
  fRootHisto = new TH1F(fName,"",fNbins,fXmin,fXmax);
  for(Int_t bin=0; bin<fNcells; bin++)
    fRootHisto->AddBinContent(bin,GetBinContent(bin));
  
  fRootHisto->Draw(option);
  
}
#endif
