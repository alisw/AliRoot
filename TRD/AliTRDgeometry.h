#ifndef ALITRDGEOMETRY_H
#define ALITRDGEOMETRY_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include "AliGeometry.h"

class AliTRDgeometry : public AliGeometry {

 public:

  enum { kNplan = 6, kNcham = 5, kNsect = 18, kNdet = 540 };

  AliTRDgeometry();
  virtual ~AliTRDgeometry();

  virtual void     CreateGeometry(Int_t *idtmed);
  virtual Int_t    IsVersion() const = 0;
  virtual void     Init();
  virtual Bool_t   Local2Global(Int_t d, Float_t *local, Float_t *global) const;
  virtual Bool_t   Local2Global(Int_t p, Int_t c, Int_t s, Float_t *local, Float_t *global) const;
  virtual Bool_t   Rotate(Int_t d, Float_t *pos, Float_t *rot) const;
  virtual Bool_t   RotateBack(Int_t d, Float_t *rot, Float_t *pos) const;

  static  Int_t    Nsect()   { return fgkNsect; };
  static  Int_t    Nplan()   { return fgkNplan; };
  static  Int_t    Ncham()   { return fgkNcham; };
  static  Int_t    Ndet()    { return fgkNdet;  };

  static  Float_t  Rmin()    { return fgkRmin;  };
  static  Float_t  Rmax()    { return fgkRmax;  };
  static  Float_t  Zmax1()   { return fgkZmax1; };
  static  Float_t  Zmax2()   { return fgkZmax2; };

  static  Float_t  Cwidcha() { return (fgkSwidth2 - fgkSwidth1) 
                             / fgkSheight * (fgkCheight + fgkCspace); };
  static  Float_t  Cheight() { return fgkCheight; };
  static  Float_t  Cspace()  { return fgkCspace;  };
  static  Float_t  Ccframe() { return fgkCcframe; };
  static  Float_t  MyThick() { return fgkMyThick; };
  static  Float_t  DrThick() { return fgkDrThick; };
  static  Float_t  AmThick() { return fgkAmThick; };
  static  Float_t  DrZpos()  { return fgkDrZpos;  };

  virtual void     SetPHOShole() = 0;
  virtual void     SetRICHhole() = 0;

  virtual void     SetNRowPad(const Int_t p, const Int_t c, const Int_t npad) {};
  virtual void     SetNColPad(const Int_t npad);
  virtual void     SetNTimeBin(const Int_t nbin);
  virtual void     SetExpandTimeBin(const Int_t nbefore, const Int_t nafter)
                                                                  { fTimeBefore = nbefore;
                                                                    fTimeAfter  = nafter; };

  virtual Bool_t   GetPHOShole() const = 0;
  virtual Bool_t   GetRICHhole() const = 0;

  virtual Int_t    GetDetector(const Int_t p, const Int_t c, const Int_t s) const;
  virtual Int_t    GetPlane(const Int_t d)   const;
  virtual Int_t    GetChamber(const Int_t d) const;
  virtual Int_t    GetSector(const Int_t d)  const;

          Float_t  GetChamberWidth(const Int_t p)           const { return fCwidth[p]; };
   
          Int_t    GetRowMax(const Int_t p, const Int_t c, const Int_t s)     
                                                            const { return fRowMax[p][c][s]; };
          Int_t    GetColMax(const Int_t p)                 const { return fColMax[p];       };
          Int_t    GetTimeMax()                             const { return fTimeMax;         };
          Int_t    GetTimeBefore()                          const { return fTimeBefore;      }; 
          Int_t    GetTimeAfter()                           const { return fTimeAfter;       }; 
          Int_t    GetTimeTotal()                           const { return fTimeMax 
                                                                         + fTimeBefore 
                                                                         + fTimeAfter; };

          Float_t  GetRow0(const Int_t p, const Int_t c, const Int_t s)       
                                                            const { return fRow0[p][c][s]; };
          Float_t  GetCol0(const Int_t p)                   const { return fCol0[p];       };
          Float_t  GetTime0(const Int_t p)                  const { return fTime0[p];      };

          Float_t  GetRowPadSize(const Int_t p, const Int_t c, const Int_t s) 
                                                            const { return fRowPadSize[p][c][s]; };
          Float_t  GetColPadSize(const Int_t p)             const { return fColPadSize[p];       };
          Float_t  GetTimeBinSize()                         const { return fTimeBinSize;         };

  virtual void     GetGlobal(const AliRecPoint *p, TVector3 &pos, TMatrix &mat) const; 
  virtual void     GetGlobal(const AliRecPoint *p, TVector3 &pos) const;   

  static  Double_t GetAlpha()  { return 2 * 3.14159265358979323846 / fgkNsect; }; 

 protected:

  static const Int_t   fgkNsect;                            // Number of sectors in the full detector (18)
  static const Int_t   fgkNplan;                            // Number of planes of the TRD (6)
  static const Int_t   fgkNcham;                            // Number of chambers in z-direction (5)
  static const Int_t   fgkNdet;                             // Total number of detectors (18 * 6 * 5 = 540)

  static const Float_t fgkRmin;                             // Minimal radius of the TRD
  static const Float_t fgkRmax;                             // Maximal radius of the TRD

  static const Float_t fgkZmax1;                            // Half-length of the TRD at outer radius
  static const Float_t fgkZmax2;                            // Half-length of the TRD at inner radius

  static const Float_t fgkSheight;                          // Height of the TRD-volume in spaceframe (BTR1-3)
  static const Float_t fgkSwidth1;                          // Lower width of the TRD-volume in spaceframe (BTR1-3)
  static const Float_t fgkSwidth2;                          // Upper width of the TRD-volume in spaceframe (BTR1-3)
  static const Float_t fgkSlenTR1;                          // Length of the TRD-volume in spaceframe (BTR1)
  static const Float_t fgkSlenTR2;                          // Length of the TRD-volume in spaceframe (BTR2)
  static const Float_t fgkSlenTR3;                          // Length of the TRD-volume in spaceframe (BTR3)

  static const Float_t fgkCheight;                          // Height of the chambers
  static const Float_t fgkCspace;                           // Vertical spacing of the chambers
  static const Float_t fgkCaframe;                          // Height of the aluminum frame
  static const Float_t fgkCcframe;                          // Height of the carbon frame
  static const Float_t fgkCathick;                          // Thickness of the aluminum frame
  static const Float_t fgkCcthick;                          // Thickness of the carbon frame

  static const Float_t fgkRaThick;                          // Thickness of the radiator
  static const Float_t fgkMyThick;                          // Thickness of the mylar-layer
  static const Float_t fgkXeThick;                          // Thickness of the gas volume
  static const Float_t fgkDrThick;                          // Thickness of the drift region
  static const Float_t fgkAmThick;                          // Thickness of the amplification region
  static const Float_t fgkCuThick;                          // Thickness of the pad plane
  static const Float_t fgkSuThick;                          // Thickness of the HEXCEL+G10 support structure
  static const Float_t fgkFeThick;                          // Thickness of the FEE + signal lines
  static const Float_t fgkCoThick;                          // Thickness of the PE of the cooling device
  static const Float_t fgkWaThick;                          // Thickness of the cooling water

  static const Float_t fgkRaZpos;                           // Position of the radiator
  static const Float_t fgkMyZpos;                           // Position of the mylar-layer
  static const Float_t fgkDrZpos;                           // Position of the drift region
  static const Float_t fgkAmZpos;                           // Position of the amplification region
  static const Float_t fgkCuZpos;                           // Position of the pad plane
  static const Float_t fgkSuZpos;                           // Position of the HEXCEL+G10 support structure
  static const Float_t fgkFeZpos;                           // Position of the FEE + signal lines
  static const Float_t fgkCoZpos;                           // Position of the PE of the cooling device
  static const Float_t fgkWaZpos;                           // Position of the colling water

  Int_t                fRowMax[kNplan][kNcham][kNsect];     // Number of pad-rows
  Int_t                fColMax[kNplan];                     // Number of pad-columns
  Int_t                fTimeMax;                            // Number of timebins in the drift region
  Int_t                fTimeBefore;                         // Number of timebins before the drift region
  Int_t                fTimeAfter;                          // Number of timebins after the drift region

  Float_t              fCwidth[kNplan];                     // Width of the chambers

  Float_t              fRow0[kNplan][kNcham][kNsect];       // Row-position of pad 0
  Float_t              fCol0[kNplan];                       // Column-position of pad 0
  Float_t              fTime0[kNplan];                      // Time-position of pad 0

  Float_t              fRowPadSize[kNplan][kNcham][kNsect]; // Pad size in z-direction
  Float_t              fColPadSize[kNplan];                 // Pad size in rphi-direction
  Float_t              fTimeBinSize;                        // Size of the time buckets

  Float_t              fRotA11[kNsect];                     // Matrix elements for the rotation
  Float_t              fRotA12[kNsect];                     // Matrix elements for the rotation
  Float_t              fRotA21[kNsect];                     // Matrix elements for the rotation
  Float_t              fRotA22[kNsect];                     // Matrix elements for the rotation

  Float_t              fRotB11[kNsect];                     // Matrix elements for the backward rotation
  Float_t              fRotB12[kNsect];                     // Matrix elements for the backward rotation
  Float_t              fRotB21[kNsect];                     // Matrix elements for the backward rotation
  Float_t              fRotB22[kNsect];                     // Matrix elements for the backward rotation

  ClassDef(AliTRDgeometry,3)                                // TRD geometry base class

};

#endif
