#ifndef ALIL3_HOUGH_MaxFinder
#define ALIL3_HOUGH_MaxFinder

#include "AliL3RootTypes.h"
#include <stream.h>

class AliL3Histogram;
class AliL3TrackArray;
class AliL3HoughTrack;
class TNtuple;

struct AxisWindow
{
  Int_t ymin;
  Int_t ymax;
  Int_t xbin;
  Int_t weight;
};

class AliL3HoughMaxFinder {
  
 private:

  Int_t fThreshold;
  AliL3Histogram *fCurrentHisto;  //!
  
  Float_t *fXPeaks; //!
  Float_t *fYPeaks; //!
  Int_t *fWeight;   //!
  Int_t fNPeaks;
  Int_t fNMax;
  
  Char_t fHistoType;

#ifndef no_root
  TNtuple *fNtuppel; //!
#endif

 public:
  AliL3HoughMaxFinder(); 
  AliL3HoughMaxFinder(Char_t *histotype,Int_t nmax,AliL3Histogram *hist=0);
  virtual ~AliL3HoughMaxFinder();
  void Reset();

  void CreateNtuppel();
  void WriteNtuppel(Char_t *filename);

  //Simple maxima finders:
  void FindAbsMaxima();
  void FindBigMaxima();
  void FindMaxima(Double_t grad_x,Double_t grad_y);
  
  //More sophisticated peak finders:
  AliL3TrackArray *LookForPeaks(AliL3Histogram *hist,Int_t nbins);
  void FindPeak(Int_t t1,Double_t t2,Int_t t3,Float_t &kappa,Float_t &phi0);
  AliL3HoughTrack *FindPeakLine(Double_t rho,Double_t theta);
  AliL3HoughTrack *CalculatePeakInWindow(Int_t *maxbin,Int_t t0,Int_t t1,Double_t t2,Int_t t3);
  void FindPeak1(Int_t y_window=2,Int_t x_bin_sides=1);
  void SortPeaks(struct AxisWindow **a,Int_t first,Int_t last);
  Int_t PeakCompare(struct AxisWindow *a,struct AxisWindow *b);
  
  //Setters:
  void SetThreshold(Int_t f) {fThreshold = f;}
  void SetHistogram(AliL3Histogram *hist) {fCurrentHisto = hist;}
  
  //Getters:
  Float_t GetXPeak(Int_t i);
  Float_t GetYPeak(Int_t i);
  Int_t GetWeight(Int_t i);
  Int_t GetEntries() {return fNPeaks;}

  ClassDef(AliL3HoughMaxFinder,1) //Maximum finder class

};

inline Float_t AliL3HoughMaxFinder::GetXPeak(Int_t i)
{
  if(i<0 || i>fNMax)
    {
      cerr<<"AliL3HoughMaxFinder::GetXPeak : Invalid index "<<i<<endl;
      return 0;
    }
  return fXPeaks[i];
}

inline Float_t AliL3HoughMaxFinder::GetYPeak(Int_t i)
{
  if(i<0 || i>fNMax)
    {
      cerr<<"AliL3HoughMaxFinder::GetYPeak : Invalid index "<<i<<endl;
      return 0;
    }
  return fYPeaks[i];

}

inline Int_t AliL3HoughMaxFinder::GetWeight(Int_t i)
{
  if(i<0 || i>fNMax)
    {
      cerr<<"AliL3HoughMaxFinder::GetWeight : Invalid index "<<i<<endl;
      return 0;
    }
  return fWeight[i];
}

#endif

