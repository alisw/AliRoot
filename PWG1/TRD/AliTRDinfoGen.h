#ifndef ALITRDINFOGEN_H
#define ALITRDINFOGEN_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: AliTRDinfoGen.h 27496 2008-07-22 08:35:45Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Reconstruction QA                                                     //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ALITRDRECOTASK_H
#include "AliTRDrecoTask.h"
#endif

class AliESDEvent;
class AliMCEvent;
class AliESDfriend;
class AliTRDtrackInfo;
class AliTRDeventInfo;
class AliTRDv0Info;
class TObjArray;
class TTreeSRedirector;
class AliTRDinfoGen : public AliTRDrecoTask{
public:

  AliTRDinfoGen();
  virtual ~AliTRDinfoGen();
  
  void  ConnectInputData(Option_t *);
  void  CreateOutputObjects();
  void  Exec(Option_t *);
  static Float_t GetTPCx() { return fgkTPC;}
  static Float_t GetTOFx() { return fgkTOF;}

private:
  static const Float_t fgkTPC; // end TPC radial position
  static const Float_t fgkTOF; // begin TOF radial position

  AliTRDinfoGen(const AliTRDinfoGen&);
  AliTRDinfoGen& operator=(const AliTRDinfoGen&);

  AliESDEvent      *fESDev;          //! ESD event
  AliMCEvent       *fMCev;           //! MC event
  AliESDfriend     *fESDfriend;      //! ESD friends
  AliTRDtrackInfo  *fTrackInfo;      //! Track info
  AliTRDeventInfo  *fEventInfo;		   //! Event info
  TObjArray        *fV0container;    //! V0 container
  AliTRDv0Info     *fV0Info;		     //! V0 info

  ClassDef(AliTRDinfoGen, 2)         // entry to TRD analysis train
};
#endif
