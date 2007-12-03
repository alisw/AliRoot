#ifndef ALIITSTRACKERMI_H
#define ALIITSTRACKERMI_H
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//                          ITS tracker
//     reads AliITSclusterMI clusters and creates AliITStrackMI tracks
//           Origin: Marian Ivanov, CERN, Marian.Ivanov@cern.ch
//-------------------------------------------------------------------------

class TTree;
class TTreeSRedirector;
class AliESDEvent;

#include <TObjArray.h>
#include "AliITSRecPoint.h"
#include "AliITStrackMI.h"
#include "AliTracker.h"
#include "AliITSPlaneEff.h"

//-------------------------------------------------------------------------
class AliITStrackerMI : public AliTracker {
public:
  AliITStrackerMI();
  AliITStrackerMI(const Char_t *geom);
  virtual ~AliITStrackerMI();
  AliCluster *GetCluster(Int_t index) const;
  virtual Bool_t GetTrackPoint(Int_t index, AliTrackPoint& p) const;
  virtual Bool_t GetTrackPointTrackingError(Int_t index, 
			AliTrackPoint& p, const AliESDtrack *t);
  AliITSRecPoint *GetClusterLayer(Int_t layn, Int_t ncl) const
                        {return fgLayers[layn].GetCluster(ncl);}
  Int_t GetNumberOfClustersLayer(Int_t layn) const 
                        {return fgLayers[layn].GetNumberOfClusters();}
  Int_t LoadClusters(TTree *cf);
  void UnloadClusters();
  Int_t Clusters2Tracks(AliESDEvent *event);
  Int_t PropagateBack(AliESDEvent *event);
  Int_t RefitInward(AliESDEvent *event);
  Bool_t RefitAt(Double_t x, AliITStrackMI *seed, 
		 const AliITStrackMI *t, Bool_t extra=kFALSE);
  Bool_t RefitAt(Double_t x, AliITStrackMI *seed, const Int_t *clindex);
  void SetupFirstPass(Int_t *flags, Double_t *cuts=0);
  void SetupSecondPass(Int_t *flags, Double_t *cuts=0);

  void SetLastLayerToTrackTo(Int_t l=0) {fLastLayerToTrackTo=l;} 
  void SetLayersNotToSkip(Int_t *l);
  void UseClusters(const AliKalmanTrack *t, Int_t from=0) const;

  void  GetDCASigma(AliITStrackMI* track, Float_t & sigmarfi, Float_t &sigmaz);
  Double_t GetPredictedChi2MI(AliITStrackMI* track, const AliITSRecPoint *cluster,Int_t layer);
  Int_t UpdateMI(AliITStrackMI* track, const AliITSRecPoint* cl,Double_t chi2,Int_t layer) const;
  class AliITSdetector { 
  public:
    AliITSdetector():fR(0),fPhi(0),fSinPhi(0),fCosPhi(0),fYmin(0),fYmax(0),fZmin(0),fZmax(0){}
    AliITSdetector(Double_t r,Double_t phi):fR(r),fPhi(phi),fSinPhi(TMath::Sin(phi)),fCosPhi(TMath::Cos(phi)),fYmin(10000),fYmax(-1000),fZmin(10000),fZmax(-1000) {}
    inline void GetGlobalXYZ( const AliITSRecPoint *cl, Double_t xyz[3]) const;
    Double_t GetR()   const {return fR;}
    Double_t GetPhi() const {return fPhi;}
    Double_t GetYmin() const {return fYmin;}
    Double_t GetYmax() const {return fYmax;}
    Double_t GetZmin() const {return fZmin;}
    Double_t GetZmax() const {return fZmax;}
    void SetYmin(Double_t min) {fYmin = min;}
    void SetYmax(Double_t max) {fYmax = max;}
    void SetZmin(Double_t min) {fZmin = min;}
    void SetZmax(Double_t max) {fZmax = max;}
  private:
    Double_t fR;    // polar coordinates 
    Double_t fPhi;  // of this detector
    Double_t fSinPhi; // sin of phi;
    Double_t fCosPhi; // cos of phi 
    Double_t fYmin;   //  local y minimal
    Double_t fYmax;   //  local max y
    Double_t fZmin;   //  local z min
    Double_t fZmax;   //  local z max
  };

  class AliITSlayer {
  public:
    AliITSlayer();
    AliITSlayer(Double_t r, Double_t p, Double_t z, Int_t nl, Int_t nd);
   ~AliITSlayer();
    Int_t InsertCluster(AliITSRecPoint *c);
    void  SortClusters();
    void ResetClusters();
    void ResetWeights();
    void SelectClusters(Double_t zmin,Double_t zmax,Double_t ymin,Double_t ymax);
    const AliITSRecPoint *GetNextCluster(Int_t &ci);
    void ResetRoad();
    Double_t GetRoad() const {return fRoad;}
    Double_t GetR() const {return fR;}
    Int_t FindClusterIndex(Float_t z) const;
    AliITSRecPoint *GetCluster(Int_t i) const {return i<fN? fClusters[i]:0;} 
    Float_t         *GetWeight(Int_t i) {return i<fN ?&fClusterWeight[i]:0;}
    AliITSdetector &GetDetector(Int_t n) const { return fDetectors[n]; }
    Int_t FindDetectorIndex(Double_t phi, Double_t z) const;
    Double_t GetThickness(Double_t y, Double_t z, Double_t &x0) const;
    Int_t InRoad() const ;
    Int_t GetNumberOfClusters() const {return fN;}
    Int_t GetNladders() const {return fNladders;}
    Int_t GetNdetectors() const {return fNdetectors;}
    Int_t GetSkip() const {return fSkip;}
    void  SetSkip(Int_t skip){fSkip=skip;}
    void IncAccepted(){fAccepted++;}
    Int_t GetAccepted() const {return fAccepted;}    
    Int_t GetClusterTracks(Int_t i, Int_t j) const {return fClusterTracks[i][j];}
    void SetClusterTracks(Int_t i, Int_t j, Int_t c) {fClusterTracks[i][j]=c;}
  protected:
    AliITSlayer(const AliITSlayer& layer);
    AliITSlayer & operator=(const AliITSlayer& layer){
      this->~AliITSlayer();new(this) AliITSlayer(layer);
      return *this;}
    Double_t fR;                // mean radius of this layer
    Double_t fPhiOffset;        // offset of the first detector in Phi
    Int_t fNladders;            // number of ladders
    Double_t fZOffset;          // offset of the first detector in Z
    Int_t fNdetectors;          // detectors/ladder
    AliITSdetector *fDetectors; // array of detectors
    Int_t fN;                   // number of clusters
    AliITSRecPoint *fClusters[AliITSRecoParam::fgkMaxClusterPerLayer]; // pointers to clusters
    Int_t        fClusterIndex[AliITSRecoParam::fgkMaxClusterPerLayer]; // pointers to clusters
    Float_t fY[AliITSRecoParam::fgkMaxClusterPerLayer];                // y position of the clusters      
    Float_t fZ[AliITSRecoParam::fgkMaxClusterPerLayer];                // z position of the clusters      
    Float_t fYB[2];                                       // ymin and ymax
    //
    AliITSRecPoint *fClusters5[6][AliITSRecoParam::fgkMaxClusterPerLayer5]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex5[6][AliITSRecoParam::fgkMaxClusterPerLayer5]; // pointers to clusters -     slice in y    
    Float_t fY5[6][AliITSRecoParam::fgkMaxClusterPerLayer5];                // y position of the clusters  slice in y    
    Float_t fZ5[6][AliITSRecoParam::fgkMaxClusterPerLayer5];                // z position of the clusters  slice in y 
    Int_t fN5[6];                                       // number of cluster in slice
    Float_t fDy5;                                       //delta y
    Float_t fBy5[6][2];                                    //slice borders
    //
    AliITSRecPoint *fClusters10[11][AliITSRecoParam::fgkMaxClusterPerLayer10]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex10[11][AliITSRecoParam::fgkMaxClusterPerLayer10]; // pointers to clusters -     slice in y    
    Float_t fY10[11][AliITSRecoParam::fgkMaxClusterPerLayer10];                // y position of the clusters  slice in y    
    Float_t fZ10[11][AliITSRecoParam::fgkMaxClusterPerLayer10];                // z position of the clusters  slice in y 
    Int_t fN10[11];                                       // number of cluster in slice
    Float_t fDy10;                                        // delta y
    Float_t fBy10[11][2];                                 // slice borders
    //
    AliITSRecPoint *fClusters20[21][AliITSRecoParam::fgkMaxClusterPerLayer20]; // pointers to clusters -     slice in y
    Int_t        fClusterIndex20[21][AliITSRecoParam::fgkMaxClusterPerLayer20]; // pointers to clusters -     slice in y    
    Float_t fY20[21][AliITSRecoParam::fgkMaxClusterPerLayer20];                // y position of the clusters  slice in y    
    Float_t fZ20[21][AliITSRecoParam::fgkMaxClusterPerLayer20];                // z position of the clusters  slice in y 
    Int_t fN20[21];                                       // number of cluster in slice
    Float_t fDy20;                                        //delta y 
    Float_t fBy20[21][2];                                 //slice borders
    //
    AliITSRecPoint** fClustersCs;                         //clusters table in current slice
    Int_t   *fClusterIndexCs;                             //cluster index in current slice 
    Float_t *fYcs;                                        //y position in current slice
    Float_t *fZcs;                                        //z position in current slice
    Int_t    fNcs;                                        //number of clusters in current slice    
    Int_t fCurrentSlice;                                  //current slice
    //
    Float_t  fClusterWeight[AliITSRecoParam::fgkMaxClusterPerLayer]; // probabilistic weight of the cluster
    Int_t    fClusterTracks[4][AliITSRecoParam::fgkMaxClusterPerLayer]; //tracks registered to given cluster
    Float_t fZmax;      //    edges
    Float_t fYmin;      //   of  the
    Float_t fYmax;      //   "window"
    Int_t fI;            // index of the current cluster within the "window"
    Int_t fImax;            // index of the last cluster within the "window"    
    Int_t fSkip;     // indicates possibility to skip cluster
    Int_t fAccepted;     // accept indicator 
    Double_t fRoad;      // road defined by the cluster density
  };
  AliITStrackerMI::AliITSlayer    & GetLayer(Int_t layer) const;
  AliITStrackerMI::AliITSdetector & GetDetector(Int_t layer, Int_t n) const {return GetLayer(layer).GetDetector(n); }

protected:
  Int_t GetNearestLayer(const Double_t *xr) const;  //get nearest upper layer close to the point xr
  void FindV02(AliESDEvent *event);  //try to find V0
  void RefitV02(AliESDEvent *event);  //try to refit  V0's
  void UpdateTPCV0(AliESDEvent *event);  //try to update, or reject TPC  V0s
  void CookLabel(AliKalmanTrack *t,Float_t wrong) const;
  void CookLabel(AliITStrackMI *t,Float_t wrong) const;
  Double_t GetEffectiveThickness();
  void FollowProlongationTree(AliITStrackMI * otrack, Int_t esdindex, Bool_t constrain);
  void ResetBestTrack() {
     fBestTrack.~AliITStrackMI();
     new(&fBestTrack) AliITStrackMI(fTrackToFollow);
  }
  void ResetTrackToFollow(const AliITStrackMI &t) {
     fTrackToFollow.~AliITStrackMI();
     new(&fTrackToFollow) AliITStrackMI(t);
  }
  void CookdEdx(AliITStrackMI* track);
  Double_t GetNormalizedChi2(AliITStrackMI * track, Int_t mode);
  Double_t GetTruncatedChi2(AliITStrackMI * track, Float_t fac);
  Double_t NormalizedChi2(AliITStrackMI * track, Int_t layer);
  Double_t GetInterpolatedChi2(AliITStrackMI * forwardtrack, AliITStrackMI * backtrack);  
  Double_t GetMatchingChi2(AliITStrackMI * track1, AliITStrackMI * track2);
  Double_t GetSPDDeadZoneProbability(Double_t zpos, Double_t zerr);

  Float_t    *GetWeight(Int_t index);
  void AddTrackHypothesys(AliITStrackMI * track, Int_t esdindex);
  void SortTrackHypothesys(Int_t esdindex, Int_t maxcut, Int_t mode);
  AliITStrackMI * GetBestHypothesys(Int_t esdindex, AliITStrackMI * original, Int_t checkmax); 
  void  GetBestHypothesysMIP(TObjArray &itsTracks); 
  void RegisterClusterTracks(AliITStrackMI* track, Int_t id);
  void UnRegisterClusterTracks(AliITStrackMI* track, Int_t id);
  Float_t GetNumberOfSharedClusters(AliITStrackMI* track,Int_t id, Int_t list[6], AliITSRecPoint *clist[6]);
  Int_t GetOverlapTrack(AliITStrackMI *track, Int_t trackID, Int_t &shared, Int_t clusterlist[6], Int_t overlist[6]);
  AliITStrackMI * GetBest2Tracks(Int_t trackID1, Int_t treackID2, Float_t th0, Float_t th1);
  Float_t  * GetErrY(Int_t trackindex) const {return &fCoefficients[trackindex*48];}
  Float_t  * GetErrZ(Int_t trackindex) const {return &fCoefficients[trackindex*48+12];}
  Float_t  * GetNy(Int_t trackindex) const {return &fCoefficients[trackindex*48+24];}
  Float_t  * GetNz(Int_t trackindex) const {return &fCoefficients[trackindex*48+36];}
  void       SignDeltas( TObjArray *ClusterArray, Float_t zv);
  void MakeCoefficients(Int_t ntracks);
  void BuildMaterialLUT(TString material);
  void MakeTrksMaterialLUT(Int_t ntracks);
  void DeleteTrksMaterialLUT();
  Int_t CorrectForPipeMaterial(AliITStrackMI *t, TString direction="inward");
  Int_t CorrectForShieldMaterial(AliITStrackMI *t, TString shield, TString direction="inward");
  Int_t CorrectForLayerMaterial(AliITStrackMI *t, Int_t layerindex, Double_t oldGlobXYZ[3], TString direction="inward");
  void UpdateESDtrack(AliITStrackMI* track, ULong_t flags) const;
  Int_t CheckSkipLayer(AliITStrackMI *track,Int_t ilayer,Int_t idet) const;
  Int_t CheckDeadZone(/*AliITStrackMI *track,*/Int_t ilayer,/*Int_t idet,*/Double_t zmin,Double_t zmax/*,Double_t ymin,Double_t ymax*/) const;
  Int_t fI;                              // index of the current layer
  static AliITSlayer fgLayers[AliITSgeomTGeo::kNLayers];// ITS layers
  AliITStrackMI fTracks[AliITSgeomTGeo::kNLayers];      // track estimations at the ITS layers
  AliITStrackMI fBestTrack;              // "best" track 
  AliITStrackMI fTrackToFollow;          // followed track
  TObjArray     fTrackHypothesys;        // ! array with track hypothesys- ARRAY is the owner of tracks- MI
  TObjArray     fBestHypothesys;         // ! array with track hypothesys- ARRAY is the owner of tracks- MI
  TObjArray     fOriginal;               // ! array with seeds from the TPC
  Int_t         fBestTrackIndex[100000]; // ! index of the best track
  Int_t         fCurrentEsdTrack;        // ! current esd track           - MI
  Int_t fPass;                           // current pass through the data 
  Int_t fConstraint[2];                  // constraint flags
  Bool_t fAfterV0;                       //indicates V0 founded
  Int_t fLayersNotToSkip[AliITSgeomTGeo::kNLayers];     // layer masks
  Int_t fLastLayerToTrackTo;             // the innermost layer to track to
  Float_t * fCoefficients;               //! working array with errors and mean cluster shape
  AliESDEvent  * fEsd;                   //! pointer to the ESD event
  Double_t fSPDdetzcentre[4];            // centres of SPD modules in z
  TString fTrackingPhase;                // current tracking phase
  Int_t fUseTGeo;                        // use TGeo to get material budget
  Int_t   fNtracks;                      // number of tracks to prolong
  Float_t fxOverX0Pipe;                  // material budget
  Float_t fxTimesRhoPipe;                // material budget
  Float_t fxOverX0Shield[2];             // material budget
  Float_t fxTimesRhoShield[2];           // material budget
  Float_t fxOverX0Layer[6];              // material budget
  Float_t fxTimesRhoLayer[6];            // material budget
  Float_t *fxOverX0PipeTrks;             //! material budget
  Float_t *fxTimesRhoPipeTrks;           //! material budget
  Float_t *fxOverX0ShieldTrks;           //! material budget
  Float_t *fxTimesRhoShieldTrks;         //! material budget
  Float_t *fxOverX0LayerTrks;            //! material budget
  Float_t *fxTimesRhoLayerTrks;          //! material budget
  TTreeSRedirector *fDebugStreamer;      //!debug streamer
//
  AliITSPlaneEff *fPlaneEff;             //| Pointer to the ITS plane efficicency
private:
  AliITStrackerMI(const AliITStrackerMI &tracker);
  AliITStrackerMI & operator=(const AliITStrackerMI &tracker);
  ClassDef(AliITStrackerMI,4)   //ITS tracker MI
};






/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////





inline void AliITStrackerMI::SetupFirstPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the first tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[0]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerMI::SetupSecondPass(Int_t *flags, Double_t *cuts) {
  // This function sets up flags and cuts for the second tracking pass   
  //
  //   flags[0] - vertex constaint flag                                
  //              negative means "skip the pass"                        
  //              0        means "no constraint"                        
  //              positive means "normal constraint"                    

   fConstraint[1]=flags[0];
   if (cuts==0) return;
}

inline void AliITStrackerMI::CookLabel(AliKalmanTrack *t,Float_t wrong) const {
  //--------------------------------------------------------------------
  //This function "cooks" a track label. If label<0, this track is fake.
  //--------------------------------------------------------------------
   Int_t tpcLabel=t->GetLabel();
   if (tpcLabel<0) return;
   AliTracker::CookLabel(t,wrong);
   if (tpcLabel!=TMath::Abs(t->GetLabel())){
     t->SetFakeRatio(1.);
   }
   if (tpcLabel !=t->GetLabel()) {
     t->SetLabel(-tpcLabel);      
   }
}

inline Double_t AliITStrackerMI::NormalizedChi2(AliITStrackMI * track, Int_t layer)
{
  //--------------------------------------------------------------------
  //get normalize chi2
  //--------------------------------------------------------------------
  track->SetNormChi2(layer,2.*track->GetNSkipped()+0.25*track->GetNDeadZone()+track->GetdEdxMismatch()+track->GetChi2()/
  //track->fNormChi2[layer] = 2.*track->fNSkipped+0.25*track->fNDeadZone+track->fdEdxMismatch+track->fChi22/
    TMath::Max(double(track->GetNumberOfClusters()-track->GetNSkipped()),
	       1./(1.+track->GetNSkipped())));
  return track->GetNormChi2(layer);
}
inline void  AliITStrackerMI::AliITSdetector::GetGlobalXYZ(const AliITSRecPoint *cl, Double_t xyz[3]) const
{
  //
  // get cluster coordinates in global cooordinate 
  //
  xyz[2] = cl->GetZ();
  xyz[0] = fR*fCosPhi - cl->GetY()*fSinPhi;
  xyz[1] = fR*fSinPhi + cl->GetY()*fCosPhi;
}
#endif
