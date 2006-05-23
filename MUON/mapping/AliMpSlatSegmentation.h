/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpSlatSegmentation.h,v 1.11 2006/05/23 13:07:47 ivana Exp $

/// \ingroup slat
/// \class AliMpSlatSegmentation
/// \brief Implementation of AliMpVSegmentation for St345 slats.
/// 
/// Note that integer indices start at (0,0) on the bottom-left of the slat,
/// while floating point positions are relative to the center of the slat
/// (where the slat is to be understood as N PCBs of fixed size = 40cm
/// even if not all pads of a given PCBs are actually physically there).
///
/// Author: Laurent Aphecetche

#ifndef ALI_MP_SLAT_SEGMENTATION_H
#define ALI_MP_SLAT_SEGMENTATION_H

#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ALI_MP_V_SEGMENTATION_H
#include "AliMpVSegmentation.h"
#endif

#ifndef ALI_MP_PAD_H
#include "AliMpPad.h"
#endif

class AliMpMotifPosition;
class AliMpPCB;
class AliMpSlat;

class AliMpSlatSegmentation : public AliMpVSegmentation
{
 public:
  AliMpSlatSegmentation();
  AliMpSlatSegmentation(const AliMpSlat* slat);
  virtual ~AliMpSlatSegmentation();

  virtual AliMpVPadIterator* CreateIterator(const AliMpArea& area) const;

  const char* GetName() const;
  
  Bool_t HasPad(const AliMpIntPair& indices) const;

  Int_t MaxPadIndexX() const;
  Int_t MaxPadIndexY() const;
  Int_t NofPads() const;
  
  virtual AliMpPad PadByLocation(const AliMpIntPair& location, 
			 Bool_t warning) const;

  virtual AliMpPad PadByIndices(const AliMpIntPair& indices,  
			Bool_t warning) const;

  virtual AliMpPad PadByPosition(const TVector2& position,
			 Bool_t warning) const;

  virtual void Print(Option_t* opt) const;
  
  const AliMpSlat* Slat() const;
  
  void GetAllElectronicCardIDs(TArrayI& ecn) const;
  
  AliMpPlaneType PlaneType() const;
  
  TVector2 Dimensions() const;
  
 protected:
  AliMpSlatSegmentation(const AliMpSlatSegmentation& right);
  AliMpSlatSegmentation&  operator = (const AliMpSlatSegmentation& right);
     
 private:
  const AliMpSlat* fkSlat; ///< Slat

  ClassDef(AliMpSlatSegmentation,1) // A slat for stations 3,4,5
};

#endif
