/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$
// $MpId: AliMpSector.h,v 1.13 2006/05/23 13:07:44 ivana Exp $

/// \ingroup sector
/// \class AliMpSector
/// \brief A sector (quadrant) of the MUON chamber of stations 1 and 2.
///
/// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_SECTOR_H
#define ALI_MP_SECTOR_H

#include <TNamed.h>

#include "AliMpContainers.h"

#include "AliMpDirection.h"
#include "AliMpPlaneType.h"
#include "AliMpIntPair.h"

#include <TString.h>
#include <TVector2.h>
#ifdef WITH_ROOT
#include <TObjArray.h>
#endif

#ifdef WITH_STL
#include <vector>
#endif

class AliMpZone;
class AliMpRow;
class AliMpVRowSegment;
class AliMpVMotif;
class AliMpVPadIterator;
class AliMpMotifMap;

class TArrayI;

class AliMpSector : public TNamed
{
  public:
#ifdef WITH_STL
    typedef std::vector<AliMpRow*> RowVector;
    typedef std::vector<AliMpZone*> ZoneVector;
#endif
#ifdef WITH_ROOT
    typedef TObjArray  RowVector;
    typedef TObjArray  ZoneVector;
#endif

  public:
    AliMpSector(const TString& id, Int_t nofZones, Int_t nofRows,
                AliMpDirection direction, const TVector2& offset);
    AliMpSector();
    virtual ~AliMpSector();
  
    // methods  
    virtual AliMpVPadIterator* CreateIterator() const;
    void  SetRowSegmentOffsets();
    void  Initialize(); 
    void  PrintGeometry() const;

    // find methods   
    AliMpRow*     FindRow(const TVector2& position) const;    
    AliMpVMotif*  FindMotif(const TVector2& position) const;
    Int_t         FindMotifPositionId(const TVector2& position) const;

    AliMpRow*          FindRow(Int_t motifPositionId) const;
    AliMpVRowSegment*  FindRowSegment(Int_t motifPositionId) const;
    TVector2           FindPosition(Int_t motifPositionId) const;

    AliMpZone*  FindZone(const TVector2& padDimensions) const;

    // geometry 
    TVector2  Position() const;
    TVector2  Dimensions() const;
   
    //
    // get methods

    Int_t       GetNofZones() const;
    AliMpZone*  GetZone(Int_t i) const;    

    Int_t       GetNofRows() const;
    AliMpRow*   GetRow(Int_t i) const;

    AliMpDirection  GetDirection() const;  
    AliMpPlaneType  GetPlaneType() const;  

    TVector2        GetMinPadDimensions() const;
    AliMpIntPair    GetMaxPadIndices() const;
    Int_t           GetNofPads() const;

    AliMpMotifMap*  GetMotifMap() const;
    void            GetAllMotifPositionsIDs(TArrayI& ecn) const;
    
    virtual void Print(Option_t* opt="") const;
    
    
  protected:
    AliMpSector(const AliMpSector& right);
    AliMpSector&  operator = (const AliMpSector& right);

  private:
    // methods
    AliMpVRowSegment* FindRowSegment(const TVector2& position) const;
    void SetRowOffsets();
    void SetMotifPositions();
    void SetGlobalIndices();
    void SetMinPadDimensions();
    void SetMaxPadIndices();
    void SetNofPads();

    // data members        
    TString    fID;       ///< sector ID
    TVector2   fOffset;   ///< sector position
    ZoneVector fZones;    ///< zones
    RowVector  fRows;     ///< rows
    AliMpMotifMap* fMotifMap; ///< motif map
    AliMpDirection fDirection;///< the direction of constant pad size
    TVector2       fMinPadDimensions; ///< minimal pad dimensions
    AliMpIntPair   fMaxPadIndices;    ///< maximum pad indices    
    Int_t          fNofPads;          ///<  total number of pads


  ClassDef(AliMpSector,1)  // Sector
};

// inline functions

inline AliMpDirection AliMpSector::GetDirection() const 
{ return fDirection; }    

inline TVector2   AliMpSector::GetMinPadDimensions() const
{ return fMinPadDimensions; }

inline AliMpIntPair  AliMpSector::GetMaxPadIndices() const
{ return fMaxPadIndices; }

inline Int_t  AliMpSector::GetNofPads() const
{ return fNofPads; }

inline AliMpMotifMap* AliMpSector::GetMotifMap() const 
{ return fMotifMap; }    

#endif //ALI_MP_SECTOR_H

