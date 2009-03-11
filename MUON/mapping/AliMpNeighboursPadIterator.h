/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpNeighboursPadIterator.h,v 1.12 2006/05/24 13:58:21 ivana Exp $

/// \ingroup sector
/// \class AliMpNeighboursPadIterator
/// \brief An iterator over the pads surrounding a given pad
///
/// \author David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_NEIGHBOURS_PAD_ITERATOR_H
#define ALI_MP_NEIGHBOURS_PAD_ITERATOR_H

#include "AliMpVPadIterator.h"
#include "AliMpPad.h"

#include <TObjArray.h>

class AliMpVSegmentation;
class AliMpIntPair;

class AliMpNeighboursPadIterator : public AliMpVPadIterator
{
  public:
    AliMpNeighboursPadIterator();
    AliMpNeighboursPadIterator(const AliMpVSegmentation* segmentation,
                               const AliMpPad& centerPad,
                               Bool_t includeCenter=kFALSE);
    AliMpNeighboursPadIterator(const AliMpNeighboursPadIterator& right);
    virtual ~AliMpNeighboursPadIterator();

    // operators
    AliMpNeighboursPadIterator& 
      operator = (const AliMpNeighboursPadIterator& right);

    // methods
    virtual void First();
    virtual void Next();
    virtual Bool_t IsDone() const;
    virtual AliMpPad CurrentItem() const;
    virtual void Invalidate();

  private:
    // static members
    static const UInt_t   fgkInvalidIndex; ///< invalid index number

    // private methods
    Bool_t     IsNeighbour(const AliMpPad& pad) const;
    TObjArray* PadVectorLine(const AliMpPad& from,
                             const AliMpIntPair& direction) const;
    void       UpdateTotalSet(TObjArray& setTotal, TObjArray* from) const;
    void       FillPadsVector(Bool_t includeCenter);
    Bool_t     IsValid() const;

    // private data members
    const AliMpVSegmentation* fkSegmentation; ///< \brief The segmentation
                           ///  segmentation over which to iterate
    AliMpPad   fCenterPad; ///< Pad arround which we iterate
    TObjArray  fPads;      ///< The list of pad arround fCenterIndices
    UInt_t     fIndex;     ///< Current index inside the fPads vector

  ClassDef(AliMpNeighboursPadIterator,1) // iterator over motif's pads
};

#endif // ALI_MP_NEIGHBOURS_PAD_ITERATOR_H
