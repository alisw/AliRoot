// $Id$
// Category: sector
//
// Class AliMpPadRow
// ------------------
// Class describing a pad row composed of the pad row segments.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_PAD_ROW_H
#define ALI_MP_PAD_ROW_H

#include <TObject.h>

#include "AliMpSectorTypes.h"
#include "AliMpXDirection.h"

class AliMpVPadRowSegment;
class AliMpMotif;

class AliMpPadRow : public TObject
{
  public:
    AliMpPadRow(AliMpXDirection direction);
    AliMpPadRow();
    virtual ~AliMpPadRow();
  
    // methods
    AliMpVPadRowSegment*  AddPadRowSegment(AliMpMotif* motif, 
                                          Int_t motifPositionId, 
                                          Int_t nofPads);
    AliMpVPadRowSegment*  FindPadRowSegment(Double_t x) const;
    Double_t  HalfSizeY() const;
    
    // set methods
    void  SetID(Int_t id);
    void  SetOffsetX(Double_t offsetX);
    
    // get methods
    Int_t   GetID() const;
    Int_t   GetNofPadRowSegments() const;
    AliMpVPadRowSegment*  GetPadRowSegment(Int_t i) const;
    Int_t   GetNofPads() const;

  private:
    // methods
    Double_t CurrentBorderX() const;

    // data members
    AliMpXDirection     fDirection;//the pad row x direction
    Int_t               fID;       //the pad row ID
    Double_t            fOffsetX;  //the x position of the border
    PadRowSegmentVector fSegments; //the pad row segments

  ClassDef(AliMpPadRow,1)  //Pad row
};

#endif //ALI_MP_PAD_ROW_H

