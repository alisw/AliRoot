#ifndef ALIL3ITSTRACK_H
#define ALIL3ITSTRACK_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//-------------------------------------------------------------------------
//                  High Level Trigger ITS Track Class
//
//        Origin: Cvetan Cheshkov, CERN, Cvetan.Cheshkov@cern.ch 
//-------------------------------------------------------------------------


/*****************************************************************************
 *                          October 11, 2004                                 *
 * The class inherits all the properties of the off-line AliITStrackV2 class *
 * and in addition it contains an interface to the HLT ESD track             *
 *****************************************************************************/

#include <AliITStrackV2.h>

class AliL3ITStrack : public AliITStrackV2 {
public:
  AliL3ITStrack();
  AliL3ITStrack(AliESDtrack& t);
  AliL3ITStrack(const AliL3ITStrack& t);

  Int_t Compare(const TObject *o) const;

  ClassDef(AliL3ITStrack,2)   //HLT ITS reconstructed track
};

#endif
