// @(#) $Id$

#ifndef ALIL3_KALMANTRACK
#define ALIL3_KALMANTRACK

/*
* The state vector is:
*  fP0 (x[0]) : local y-coordinate
*  fP1 (x[1]) : local z-coordinate
*  fP2 (x[2]) : local sine of track momentum dip angle 
*  fP3 (x[3]) : tangent of track momentum dip angle
*  fP4 (x[4]) : 1/pt
*
* The covariance matrix is:
*  fC00                     
*  fC10 fC11
*  fC20 fC21 fC22
*  fC30 fC31 fC32 fC33
*  fC40 fC41 fC42 fC43 fC44
*
* To accsess this use: GetStateVector(Float_t xx[5])
*                      GetCovariance(Float_t xx[15])              
*/

#include "AliL3RootTypes.h"
#include "AliL3Track.h"
class AliL3SpacePointData;

class AliL3KalmanTrack : public AliL3Track {
//class AliL3KalmanTrack {

 private:

  Float_t fP0;             // Y-coordinate of a track
  Float_t fP1;             // Z-coordinate of a track
  Float_t fP2;             // C*x0
  Float_t fP3;             // tangent of the track momentum dip angle
  Float_t fP4;             // track curvature

  Float_t fC00;                         // covariance
  Float_t fC10, fC11;                   // matrix
  Float_t fC20, fC21, fC22;             // of the
  Float_t fC30, fC31, fC32, fC33;       // track
  Float_t fC40, fC41, fC42, fC43, fC44; // parameters

  Float_t fChisq; 

  Float_t fX;

 public:

  AliL3KalmanTrack();
  virtual ~AliL3KalmanTrack();
  void Init();
  Int_t MakeTrackSeed(AliL3SpacePointData *points1, UInt_t pos1, AliL3SpacePointData *points2, UInt_t pos2, AliL3SpacePointData *points3, UInt_t pos3);
  Int_t Propagate(AliL3SpacePointData *points, UInt_t pos);
  Int_t UpdateTrack(AliL3SpacePointData *points, UInt_t pos);
  Int_t UpdateTrackII(AliL3SpacePointData *points, UInt_t pos);
  void AddTrack();
  Float_t GetStateVector(Float_t xx[5]) const {
    xx[0] = fP0;
    xx[1] = fP1;
    xx[2] = fP2;
    xx[3] = fP3;
    xx[4] = fP4;
  }
  Float_t GetCovariance(Float_t cc[15]) const {
    cc[0 ]=fC00;
    cc[1 ]=fC10;  cc[2 ]=fC11;
    cc[3 ]=fC20;  cc[4 ]=fC21;  cc[5 ]=fC22;
    cc[6 ]=fC40;  cc[7 ]=fC41;  cc[8 ]=fC42;  cc[9 ]=fC44;
    cc[10]=fC30;  cc[11]=fC31;  cc[12]=fC32;  cc[13]=fC43;  cc[14]=fC33;
  }

  Float_t GetChisq() {if(!fChisq) return 0; return fChisq;} 
  Float_t GetX() {return fX;}
  void SetStateVector(Float_t f[5]) {fP0 = f[0]; fP1 = f[1]; fP2 = f[2]; 
                                     fP3 = f[3]; fP4 = f[4];}
  void SetCovariance(Float_t f[15]) {fC00 = f[0];  fC10 = f[1]; fC11 = f[2];
  fC21 = f[3]; fC21 = f[4]; fC22 = f[5]; fC30 = f[6]; fC31 = f[7]; fC32 = f[8];
  fC33 = f[9]; fC40 = f[10]; fC41 = f[11]; fC42 = f[12]; fC43 = f[13]; 
  fC44 = f[14];}
  void SetChisq(Float_t f) {fChisq = f;}

   Float_t f2(Float_t x1,Float_t y1, Float_t x2,Float_t y2, Float_t x3,Float_t y3);
   Float_t f3(Float_t x1,Float_t y1, Float_t x2,Float_t y2, Float_t z1,Float_t z2);
   Float_t f4(Float_t x1,Float_t y1, Float_t x2,Float_t y2, Float_t x3,Float_t y3);
};

#endif
