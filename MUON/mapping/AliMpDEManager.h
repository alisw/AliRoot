/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// \ingroup management
/// \class AliMpDEManager
/// \brief The manager class for definition of detection element types
//
/// The detection element types are defined via unique names
/// in denames.dat file for each station in the mapping data.
/// Detection element name is composed of DETypeName and planeTypeName.
/// DETypeName is only one per station in case of station1 and 2 quadrants, 
/// there are more DETypes in case of slat and trigger stations. 
///
/// Authors: Ivana Hrivnacova, IPN Orsay
///          Laurent Aphecetche, SUBATECH Nantes

#ifndef ALI_MP_DE_MANAGER_H
#define ALI_MP_DE_MANAGER_H

#include "AliMpExMap.h"
#include "AliMpPlaneType.h"
#include "AliMpStationType.h"

#include <TObject.h>

class AliMpVSegmentation;

class AliMpDEManager : public  TObject {

  public:
    virtual ~AliMpDEManager();
    
    // methods
    static Bool_t IsValidDetElemId(Int_t detElemId, Bool_t warn = false);
    static Bool_t IsValidCathod(Int_t cath, Bool_t warn = false);
    static Bool_t IsValid(Int_t detElemId, Int_t cath, Bool_t warn = false);
    static Bool_t IsValidModuleId(Int_t moduleId, Bool_t warn = false);

    static TString GetDEName(Int_t detElemId, Int_t cath, Bool_t warn = true);
    static TString GetDETypeName(Int_t detElemId, Int_t cath, Bool_t warn = true);
    static Int_t   GetModuleId(Int_t detElemId, Bool_t warn = true);    
    static AliMpPlaneType   GetPlaneType(Int_t detElemId, Int_t cath);
    static AliMpStationType GetStationType(Int_t detElemId);

  protected:
    AliMpDEManager();
    AliMpDEManager(const AliMpDEManager& rhs);
    AliMpDEManager& operator=(const AliMpDEManager& rhs);

  private:
    // methods
    static Bool_t IsPlaneType(const TString& planeTypeName);
    static AliMpPlaneType   PlaneType(const TString& planeTypeName);
    static AliMpStationType StationType(const TString& stationTypeName);

    static Bool_t ReadDENames(AliMpStationType station);
    static void   FillDENames();

    // static data members	
    static const char  fgkNameSeparator; // Separator character used in DE names
    static const char  fgkCommentPrefix; // Comment prefix in DE names file
    static const Int_t fgkCoefficient;   // Coefficient used in DE Id <-> station

    // data members	
    static  AliMpExMap fgDENamesMap; // Map between DE Ids and 
                                     // a pair of DE names for 2 cathods

  ClassDef(AliMpDEManager,0)  // 
};

#endif //ALI_MP_MANAGER_H















