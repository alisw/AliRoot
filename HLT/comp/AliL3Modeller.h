#ifndef AliL3_Modeller
#define AliL3_Modeller


#include "AliL3RootTypes.h"

class AliL3TrackArray;
class AliL3MemHandler;
class AliL3DigitRowData;
class AliL3ModelTrack;

struct Cluster {
  UInt_t fCharge;
  UInt_t fPad;
  UInt_t fTime;
  UInt_t fSigmaY2;
  UInt_t fSigmaZ2;
};

struct Digit {
  Short_t fCharge;
  Bool_t fUsed;
};

class AliL3Modeller {
  
 private:
  
  AliL3TrackArray *fTracks; //!
  AliL3MemHandler *fMemHandler; //!
  AliL3DigitRowData *fRowData;//!
  
  Int_t fNClusters;
  Int_t fMaxClusters;
  
  Float_t fPadOverlap;
  Float_t fTimeOverlap;
  Int_t fTrackThreshold; //minimum weigth track need in order to be included.(=Nhits/weight)
  
  Int_t fSlice;
  Int_t fPatch;
  Char_t fPath[100];

  void FillCluster(AliL3ModelTrack *track,Cluster *cluster,Int_t row,Int_t npads);
  void CalcClusterWidth(Cluster *cl,Float_t &sigmaY2,Float_t &sigmaZ2);
  void FillZeros(AliL3DigitRowData *digPt,Digit *row);

 public:
  
  AliL3Modeller();
  virtual ~AliL3Modeller();
  
  void Init(Int_t slice,Int_t patch,Char_t *trackdata,Char_t *path,Bool_t houghtracks=kFALSE);
  void FindClusters();
  void CheckForOverlaps();
  void CalculateCrossingPoints();
  void WriteRemaining();
  
  void SetInputData(AliL3DigitRowData *digits) {fRowData = digits;}
  void SetTrackThreshold(Int_t i) {fTrackThreshold=i;}

  AliL3TrackArray *GetTracks() {return fTracks;}
    
  ClassDef(AliL3Modeller,1) //Modeller class
    
};

#endif
