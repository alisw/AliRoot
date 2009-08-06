#ifndef ALITRDTRACKLET_H
#define ALITRDTRACKLET_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TObject.h>

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  A TRD tracklet                                                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

class AliTRDtracklet : public TObject {
 
 public:

  AliTRDtracklet();
  virtual          ~AliTRDtracklet();

           void     Set(Float_t x, Float_t y, Float_t z, Float_t alpha, Float_t error2)
	                                                   { fX = x; fY = y; fZ = z;
                                                             fAlpha = alpha; fSigma2 = error2; }
           void     SetP0(Float_t p0)                      { fP0             = p0;     }
           void     SetP1(Float_t p1)                      { fP1             = p1;     }
           void     SetN(Int_t n)                          { fNFound         = n;      }
           void     SetNCross(Int_t nc)                    { fNCross         = nc;     }
           void     SetPlane(Int_t plane)                  { fPlane          = plane;  }
           void     SetSigma2(Float_t sigma2)              { fExpectedSigma2 = sigma2; }
           void     SetChi2(Float_t chi2)                  { fChi2           = chi2;   }
           void     SetTilt(Float_t tilt)                  { fTilt           = tilt;   }
           void     SetMaxPos(Short_t pos, Short_t pos4, Short_t pos5)
                                                           { fMaxPos         = pos; 
                                                             fMaxPos4        = pos4; 
                                                             fMaxPos5        = pos5;   }

           Float_t  GetX() const                           { return fX;                }
           Float_t  GetY() const                           { return fY;                }
           Float_t  GetZ() const                           { return fZ;                }
           Float_t  GetAlpha() const                       { return fAlpha;            } 
           Float_t  GetTrackletSigma2() const              { return fSigma2;           }
           Float_t  GetP0() const                          { return fP0;               }
           Float_t  GetP1() const                          { return fP1;               }
           Int_t    GetN() const                           { return fNFound;           }
           Int_t    GetNCross() const                      { return fNCross;           }  
           Int_t    GetPlane() const                       { return fPlane;            }
           Float_t  GetClusterSigma2() const               { return fExpectedSigma2;   }
           Float_t  GetChi2() const                        { return fChi2;             }
           Float_t  GetTilt() const                        { return fTilt;             }

 protected:

           Float_t  fY;                  //  Y position
           Float_t  fZ;                  //  Z position
           Float_t  fX;                  //  X position
           Float_t  fAlpha;              //  Rotation angle
           Float_t  fSigma2;             //  Expected error of tracklet position
           Float_t  fP0;                 //  Offset in y
           Float_t  fP1;                 //  Offset in tangent
           Int_t    fNFound;             //  Number of found clusters
           Int_t    fNCross;             //  Number of crosses
           Int_t    fPlane;              //  Plane number
           Float_t  fExpectedSigma2;     //  Expected sigma of residual distribution of clusters
           Float_t  fChi2;               //  Chi2 of the tracklet
           Float_t  fTilt;               //  Tilt factor 
           Short_t  fMaxPos;             //  Time bin with max charge
           Short_t  fMaxPos4;            //  Time bin with max charge
           Short_t  fMaxPos5;            //  Time bin with max charge

  ClassDef(AliTRDtracklet,2)             //  The TRD tracklet in one ROC

};

#endif   
