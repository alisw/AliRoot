// $Id$
// Category: sector
//
// Class AliMpRowSegmentLSpecial
// -----------------------------
// Class describing a special inner row segment composed of the 
// pad rows.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_ROW_SEGMENT_L_SPECIAL_H
#define ALI_MP_ROW_SEGMENT_L_SPECIAL_H

#include <TVector2.h>

#include "AliMpSectorTypes.h"
#include "AliMpVRowSegmentSpecial.h"

class AliMpRow;
class AliMpPadRow;
class AliMpVPadRowSegment;
class AliMpIntPair;

class AliMpRowSegmentLSpecial : public AliMpVRowSegmentSpecial
{
  public:
    AliMpRowSegmentLSpecial(AliMpRow* row, Double_t offsetX);
    AliMpRowSegmentLSpecial();
    virtual ~AliMpRowSegmentLSpecial();
    
    // methods
    virtual void  UpdatePadsOffset();
    virtual Double_t  LeftBorderX() const;
    virtual Double_t  RightBorderX() const;

    // geometry
    virtual TVector2  Position() const;

    // set methods
    virtual Int_t  SetIndicesToMotifPosition(Int_t i, 
                             const AliMpIntPair& indices);

  protected:
    // methods
    virtual TVector2  MotifCenterSlow(Int_t motifPositionId) const;
    
  private:
    // methods
    AliMpVPadRowSegment* FindMostRightPadRowSegment(Int_t motifPositionId) const;
    
  ClassDef(AliMpRowSegmentLSpecial,1)  //Row segment
};

#endif //ALI_MP_ROW_SEGMENT_L_SPECIAL_H
