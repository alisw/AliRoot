#ifndef ALIITSCLUSTERFINDERSSD_H
#define ALIITSCLUSTERFINDERSSD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

#include <TMath.h>
#include "AliITSClusterFinder.h"

class TArrayI;
class AliITSclusterSSD;
class AliITSpackageSSD;

class AliITSClusterFinderSSD: public AliITSClusterFinder 
{
    
public:       
    
  AliITSClusterFinderSSD(AliITSsegmentation *seg, TClonesArray *digits);
		
  virtual ~AliITSClusterFinderSSD();


  void FindRawClusters(Int_t mod);

  /*
  void SetAlpha1(Float_t a) {fAlpha1 =a;}
  void SetAlpha2(Float_t a) {fAlpha2 =a;}
  void SetAlpha3(Float_t a) {fAlpha3 =a;}
  */
 protected:
   
  void      InitReconstruction();
  Bool_t    CreateNewRecPoint(Float_t P, Float_t dP, Float_t N, Float_t dN,
                              Float_t Sig,Float_t dSig,
                              AliITSclusterSSD *clusterP, AliITSclusterSSD *clusterN,
                              Stat_t prob);
  
  AliITSclusterSSD* GetPSideCluster(Int_t idx);
  AliITSclusterSSD* GetNSideCluster(Int_t idx);
  AliITSclusterSSD* GetCluster(Int_t idx, Bool_t side);

  void      FindNeighbouringDigits();
  void      SeparateOverlappedClusters();
  void      SplitCluster(TArrayI *list, Int_t nsplits, Int_t index, Bool_t side);
  Int_t     SortDigitsP(Int_t start, Int_t end);
  Int_t     SortDigitsN(Int_t start, Int_t end);
  void      FillDigitsIndex();
  void      SortDigits();
  void      FillClIndexArrays(Int_t* arrayP, Int_t *arrayN);
  void      SortClusters(Int_t* arrayP, Int_t *arrayN);
  Int_t     SortClustersP(Int_t start, Int_t end,Int_t *array);
  Int_t     SortClustersN(Int_t start, Int_t end,Int_t *array);
  void      ClustersToPackages();
  
  Int_t     GetDiff(Float_t *retx, Float_t *rety) {return 0;}
  
  void      CalcStepFactor(Float_t Psteo, Float_t Nsteo );
  
  Bool_t GetCrossing(Float_t &x, Float_t &z);     //x, y of strips crossing
  void   GetCrossingError(Float_t&, Float_t&);    //x, y of strips crossing errors

  // Data memebers

  AliITS          *fITS;          //!Pointer to AliITS object

  TClonesArray    *fClusterP;     //!
  Int_t            fNClusterP;    //!Number of P side clusters in the array
		
  TClonesArray    *fClusterN;     //!Number of N side clusters in the array
  Int_t            fNClusterN;    //!
    
  TClonesArray    *fPackages;     //!packages  
  Int_t            fNPackages;    //!
    
  TArrayI         *fDigitsIndexP; //!Digits on P side
  Int_t            fNDigitsP;      //!Number of Digits on P side
		
  TArrayI         *fDigitsIndexN;  //!Digits on N side
  Int_t            fNDigitsN;      //!Number of Digits on N side


  Float_t          fPitch;         //!Strip pitch
  Float_t          fTanP;          //!Pside stereo angle tangent
  Float_t          fTanN;          //!Nside stereo angle tangent

/*************************************************/
/**  parameters for reconstruction            ****/
/**  to be tune when slow simulation raliable ****/
/*************************************************/ 
  
  //Float_t fAlpha1;         //!
  //Float_t fAlpha2;         //!
  //Float_t fAlpha3;         //!
  Float_t fPNsignalRatio;  //!

    
  static const Bool_t fgkSIDEP;  //!
  static const Bool_t fgkSIDEN;  //!


  Int_t fSFF;              //!forward stepping factor 
  Int_t fSFB;              //!backward stepping factor 

public:
    ClassDef(AliITSClusterFinderSSD, 1) //Class for clustering and reconstruction of space points in SSDs 

};


#endif
