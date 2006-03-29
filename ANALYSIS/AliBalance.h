#ifndef ALIBALANCE_H
#define ALIBALANCE_H
/*  See cxx source for full Copyright notice */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliBalance
//   This is the class for the Balance Function analysis
//
//    Origin: Panos Christakoglou, UOA-CERN, Panos.Christakoglou@cern.ch
//-------------------------------------------------------------------------

#include <TObject.h>

#define MAXIMUM_NUMBER_OF_STEPS	1024

class TLorentzVector;
class TGraphErrors;

class AliBalance : public TObject
{
 public:
  AliBalance();
  
  AliBalance(Double_t p2Start, Double_t p2Stop, Int_t p2Steps);
  
  ~AliBalance();
  
  void SetParticles(TLorentzVector *P, Double_t *charge, Int_t dim);
  void SetNumberOfBins(Int_t ibins);
  void SetAnalysisType(Int_t iType);
  void SetInterval(Double_t p2Start, Double_t p2Stop);

  void CalculateBalance();
  
  Double_t GetNp() const { return 1.0*fNp; }
  Double_t GetNn() const { return 1.0*fNn; }
  Double_t GetNnn(Int_t p2) const { return 1.0*fNnn[p2]; }
  Double_t GetNpp(Int_t p2) const { return 1.0*fNpp[p2]; }
  Double_t GetNpn(Int_t p2) const { return 1.0*fNpn[p2]; }
 
  Double_t GetBalance(Int_t p2);
  Double_t GetError(Int_t p2);

  TGraphErrors *DrawBalance();
  void PrintResults();

 private:
  Double_t *fCharge; //matrix with the charge of each track
  Int_t fNtrack; //number of tracks to compute the BF

  TLorentzVector *fV; //4-momentum vector - info for tracks used to compute the BF
  
  Int_t fNumberOfBins; //number of bins of the analyzed interval
  Int_t fAnalysisType; //0:y - 1:eta - 2:Qlong - 3:Qout - 4:Qside - 5:Qinv - 6:phi
  Int_t fAnalyzedEvents; //number of events that have been analyzed
 
  Double_t fP2Start, fP2Stop, fP2Step; //inerval info
 	
  Double_t fNnn[MAXIMUM_NUMBER_OF_STEPS]; //N(--)
  Double_t fNpp[MAXIMUM_NUMBER_OF_STEPS]; //N(++)
  Double_t fNpn[MAXIMUM_NUMBER_OF_STEPS]; //N(+-)
  Double_t fNn, fNp; //number of pos./neg. inside the analyzed interval
  
  Double_t fB[MAXIMUM_NUMBER_OF_STEPS]; //BF matrix
  Double_t ferror[MAXIMUM_NUMBER_OF_STEPS]; //error of the BF
   
  ClassDef(AliBalance, 1)
} ;

#endif
