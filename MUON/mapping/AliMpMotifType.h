// $Id$
// Category: motif
//
// Class AliMpMotifType
// --------------------
// Class that defines the motif properties.
//
// Authors: David Guez, Ivana Hrivnacova; IPN Orsay

#ifndef ALI_MP_MOTIF_TYPE_H
#define ALI_MP_MOTIF_TYPE_H

#include <TObject.h>
#include <TString.h>

#include "AliMpMotifTypes.h"
#include "AliMpIntPair.h"

class AliMpConnection;
class AliMpVPadIterator;

class AliMpMotifType : public TObject
{
  public:
    AliMpMotifType(const TString &id);
    AliMpMotifType();
    virtual ~AliMpMotifType();

    virtual AliMpVPadIterator* CreateIterator() const;

    // find methods
    AliMpConnection *FindConnectionByPadNum(Int_t padNum) const;
    AliMpConnection *FindConnectionByLocalIndices(
                               const AliMpIntPair& localIndices) const;
    AliMpConnection *FindConnectionByGassiNum(Int_t gassiNum) const;
    AliMpConnection *FindConnectionByKaptonNum(Int_t kaptonNum) const;
    AliMpConnection *FindConnectionByBergNum(Int_t bergNum) const;

    AliMpIntPair FindLocalIndicesByPadNum(Int_t padNum) const;
    AliMpIntPair FindLocalIndicesByGassiNum(Int_t gassiNum) const;
    AliMpIntPair FindLocalIndicesByKaptonNum(Int_t kaptonNum) const;
    AliMpIntPair FindLocalIndicesByBergNum(Int_t bergNum) const;
    AliMpIntPair FindLocalIndicesByConnection(
                               const AliMpConnection* connection) const;

    // set methods
    void SetNofPads(Int_t nofPadsX, Int_t nofPadY);
    void SetVerboseLevel(Int_t level){fVerboseLevel=level;}
    
    // get methods
    TString  GetID() const        {return fID;}
    Int_t    GetNofPadsX() const  {return fNofPadsX;}
    Int_t    GetNofPadsY() const  {return fNofPadsY;}
    Int_t    GetNofPads() const;
    
    // Other methods
    void AddConnection(const AliMpIntPair &localIndices, 
                       AliMpConnection* connection);
    virtual void Print(Option_t *option) const;
    Int_t   PadNum(const TString &padName) const;
    TString PadName(Int_t padNum) const;
    Bool_t  HasPad(const AliMpIntPair& localIndices) const;
    Bool_t  IsFull() const;

  private:
    // static data members
    static const Int_t   fgkPadNumForA; // the pad number for the pad "A"

#ifdef WITH_ROOT
    static const Int_t   fgkSeparator;  // the separator used for conversion
                                        // of AliMpIntPair to Int_t
    
    // methods
    Int_t  GetIndex(const AliMpIntPair& pair) const;
    AliMpIntPair  GetPair(Int_t index) const;
#endif
  
    // data members
    TString   fID;        // unique motif ID
    Int_t     fNofPadsX;  // number of pads in x direction
    Int_t     fNofPadsY;  // number of pads in y direction
    Int_t     fVerboseLevel;  // verbose level

#ifdef WITH_STL
    ConnectionMap_t fConnections; //! Map (ix,iy) of connections
#endif    
#ifdef WITH_ROOT
    mutable ConnectionMap_t fConnections; // Map (ix,iy) of connections
#endif    
    
  ClassDef(AliMpMotifType,1)  //Motif type
};

// inline functions

inline Bool_t AliMpMotifType::IsFull() const 
{ return GetNofPads() == fNofPadsX*fNofPadsY; }

#endif //ALI_MP_MOTIF_TYPE_H

