#ifndef ALITRDMATRIX_H
#define ALITRDMATRIX_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TObject.h>

class TObjArray;

class AliTRDpixel;

///////////////////////////////////////////////////////
//                                                   //
//  Stores the pixel-information of one TRD chamber  //
//                                                   //
///////////////////////////////////////////////////////

class AliTRDmatrix : public TObject {

public:

  AliTRDmatrix();
  AliTRDmatrix(Int_t nRow, Int_t nCol, Int_t nTime, Int_t iSec, Int_t iCha, Int_t iPla);
  AliTRDmatrix(const AliTRDmatrix &m);
  virtual ~AliTRDmatrix();
  AliTRDmatrix &operator=(const AliTRDmatrix &m);

  virtual void         AddSignal(Int_t iRow, Int_t iCol, Int_t iTime, Float_t signal);
  virtual Bool_t       AddTrack(Int_t iRow, Int_t iCol, Int_t iTime, Int_t track);

  virtual void         Copy(TObject &m);
  virtual void         Draw(Option_t *opt = " ");
  virtual void         DrawRow(Int_t iRow);
  virtual void         DrawCol(Int_t iCol);
  virtual void         DrawTime(Int_t iTime);
  virtual void         ProjRow();  
  virtual void         ProjCol();  
  virtual void         ProjTime();  

  virtual void         SetSignal(Int_t iRow, Int_t iCol, Int_t iTime, Float_t signal);
  virtual void         SetTrack(Int_t iRow, Int_t iCol, Int_t iTime
                              , Int_t iTrack, Int_t track);

  virtual Float_t      GetSignal(Int_t iRow, Int_t iCol, Int_t iTime) const;
  virtual Int_t        GetTrack(Int_t iRow, Int_t iCol, Int_t iTime, Int_t iTrack) const;

  virtual Int_t        GetSector() const  { return fSector;  };
  virtual Int_t        GetChamber() const { return fChamber; };
  virtual Int_t        GetPlane() const   { return fPlane;   };

protected:

  virtual Int_t        GetIndex(Int_t iRow, Int_t iCol, Int_t iTime) const;
  virtual AliTRDpixel *GetPixel(Int_t iRow, Int_t iCol, Int_t iTime) const;

  Int_t         fRow;            // Number of pad-rows
  Int_t         fCol;            // Number of pad-columns
  Int_t         fTime;           // Number of time buckets
  Int_t         fPixel;          // Number of pixels
  Int_t         fSector;         // Sector number
  Int_t         fChamber;        // Chamber number
  Int_t         fPlane;          // Plane number
  TObjArray    *fPixelArray;     // Array of pixels

  ClassDef(AliTRDmatrix,1)       // The TRD detector matrix for one readout chamber

};

#endif
