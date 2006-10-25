#ifndef MUONCDB_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

/// By Laurent Aphecetche

#include "Rtypes.h"

class TList;
class AliMUONV1DStore;
class AliMUONV2DStore;

// Use the following for testing the Shuttle preprocessor
//static const char* CDBPath = "local://$ALICE_ROOT/SHUTTLE/TestShuttle/TestCDB";
// Or this one for "normal" use
static const char* CDBPath = "local://$ALICE_ROOT";

void generateTrigger(const char* cdbpath=CDBPath);

void getBoundaries(const AliMUONV2DStore& store,
                   Float_t& x0min, Float_t& x0max,
                   Float_t& x1min, Float_t& x1max);

void plot(const AliMUONV2DStore& store, const char* name, Int_t nbins=512);

void plotCDB(const char* calibType="MUON/Calib/Pedestals", Int_t runNumber=0);

AliMUONV2DStore* diff(AliMUONV2DStore& store1, AliMUONV2DStore& store2, const char* opt="abs");

AliMUONV2DStore* read2D(const char* calibType="MUON/Calib/Pedestals", Int_t runNumber=0);

AliMUONV1DStore* read1D(const char* calibType="MUON/Calib/LocalBoardMasks", Int_t runNumber=0);

void testMakeStores(Int_t readLoop=10);

void writeToCDB(const char* cdbpath, const char* calibpath, TObject* object, 
                Int_t startRun, Int_t endRun, Bool_t defaultValues);

void writePedestals(const char* cdbpath, Bool_t defaultValues,
                    Int_t startRun, Int_t endRun);

void writeGains(const char* cdbpath, Bool_t defaultValues,
                Int_t startRun, Int_t endRun);

void writeDeadChannels(const char* cdbpath, Bool_t defaultValues,
                       Int_t startRun, Int_t endRun);

#endif
