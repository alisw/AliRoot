#ifndef ALIGAMMACONVERSIONHISTOGRAMS_H
#define ALIGAMMACONVERSIONHISTOGRAMS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice     */

////////////////////////////////////////////////
//--------------------------------------------- 
// Class used to do analysis on conversion pairs
//---------------------------------------------
////////////////////////////////////////////////

#include "TString.h"
//#include "TH1F.h"
//#include "TH2F.h"
#include "Riostream.h"
#include <vector>
//#include "TList.h"
//#include "TMap.h"

class TMap;
class TList;
class TH1F;
class TH2F;

class AliGammaConversionHistograms{

 public: 
  
  AliGammaConversionHistograms();                                                                  //constructor
  AliGammaConversionHistograms(const AliGammaConversionHistograms & original);                     //copy constructor
  AliGammaConversionHistograms & operator = (const AliGammaConversionHistograms & original);       //assignment operator
  virtual ~AliGammaConversionHistograms();                                                         //virtual destructor
  

  //  TList * GetOutputContainer();
  void GetOutputContainer(TList *fOutputContainer) const;
  
  Int_t GetRBin(Double_t radius) const;
  Int_t GetPhiBin(Double_t phi) const;

  void InitializeMappingValues(Int_t nPhiHistograms, Int_t nRHistograms, Int_t nBinsR, Double_t minRadius, Double_t maxRadius,Int_t nBinsPhi, Double_t minPhi, Double_t maxPhi);

  void AddMappingHistograms(Int_t nPhiHistograms, Int_t nRHistograms,Int_t nXBins, Double_t firstX, Double_t lastX, Int_t nYBins, Double_t firstY, Double_t lastY, TString xAxisTitle="", TString yAxisTitle="");

  /*
   * Adds a TH1F histogram to the histogram map and create a key for it 
   */
  void AddHistogram(TString histogramName, TString histogramTitle, Int_t nXBins, Double_t firstX,Double_t lastX,TString xAxisTitle="", TString yAxisTitle="");

  /*
   * Adds a TH2F histogram to the histogram map and create a key for it 
   */
  void AddHistogram(TString histogramName, TString histogramTitle, Int_t nXBins, Double_t firstX, Double_t lastX, Int_t nYBins, Double_t firstY, Double_t lastY, TString xAxisTitle="", TString yAxisTitle="");

  /*
   * Fills a TH1F histogram with the given name with the given value 
   */
  void FillHistogram(TString histogramName, Double_t xValue) const;

  /*
   * Fills a TH2F histogram with the given name with the given value 
   */
  void FillHistogram(TString histogramName, Double_t xValue, Double_t yValue) const;

 private:
  TMap* fHistogramMap;

  Int_t fNPhiIndex;
  Int_t fNRIndex;
  Double_t fMinRadius;
  Double_t fMaxRadius;
  Double_t fDeltaR;
  Double_t fMinPhi;
  Double_t fMaxPhi;
  Double_t fDeltaPhi;
  
  ClassDef(AliGammaConversionHistograms,1)
} ;


#endif



