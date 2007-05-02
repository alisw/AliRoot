#ifndef ALITOFRECONSTRUCTOR_H
#define ALITOFRECONSTRUCTOR_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// class for TOF reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliReconstructor.h"

class TTree;

class AliESD;
class AliRawReader;
class AliRunLoader;

class AliTOFGeometry;
class AliTOFcalib;

class AliTOFReconstructor: public AliReconstructor {
public:
  AliTOFReconstructor();
  AliTOFReconstructor(const AliTOFReconstructor &source); // copy constructor
  AliTOFReconstructor& operator=(const AliTOFReconstructor &source); // ass. op.
  virtual ~AliTOFReconstructor();

  virtual Bool_t       HasLocalReconstruction() const { return kTRUE; }
  virtual void         Reconstruct(AliRunLoader* runLoader) const;
  virtual void         Reconstruct(AliRunLoader* runLoader,
				   AliRawReader* rawReader) const;
  virtual void         Reconstruct(AliRawReader* rawReader,
				   TTree* clusterTree) const;
  virtual void         Reconstruct(TTree* digitsTree, TTree* clusterTree) const;
  virtual void         ConvertDigits(AliRawReader* reader, TTree* digitsTree) const;
  virtual AliTracker*  CreateTracker(AliRunLoader* runLoader) const;
  virtual void         FillESD(AliRunLoader*, AliRawReader*, AliESD*) const { };
  virtual void         FillESD(AliRawReader*, TTree*, AliESD*) const { };
  virtual void         FillESD(TTree*, TTree*, AliESD*) const { };
  virtual void         FillESD(AliRunLoader* runLoader, AliESD* esd) const;

private:
  AliTOFGeometry *fTOFGeometry;
  AliTOFcalib    *fTOFcalib;
  AliTOFGeometry*      GetTOFGeometry(AliRunLoader* runLoader) const;

  ClassDef(AliTOFReconstructor, 1)   // class for the TOF reconstruction
};

#endif
