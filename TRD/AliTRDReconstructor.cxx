/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id$ */

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Class for TRD reconstruction                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <TFile.h>

#include "AliRunLoader.h"
#include "AliRawReader.h"
#include "AliLog.h"
#include "AliESDTrdTrack.h"
#include "AliESD.h"

#include "AliTRDReconstructor.h"
#include "AliTRDclusterizerV1.h"
#include "AliTRDtracker.h"
#include "AliTRDpidESD.h"
#include "AliTRDgtuTrack.h"
#include "AliTRDrawData.h"
#include "AliTRDdigitsManager.h"

ClassImp(AliTRDReconstructor)

Bool_t AliTRDReconstructor::fgkSeedingOn  = kFALSE;
Int_t  AliTRDReconstructor::fgStreamLevel = 0;      // Stream (debug) level

//_____________________________________________________________________________
void AliTRDReconstructor::ConvertDigits(AliRawReader *rawReader
				      , TTree *digitsTree) const
{
  //
  // Convert raw data digits into digit objects in a root tree
  //

  AliTRDrawData rawData;
  rawReader->Reset();
  rawReader->Select("TRD");
  AliTRDdigitsManager *manager = rawData.Raw2Digits(rawReader);
  manager->MakeBranch(digitsTree);
  manager->WriteDigits();

}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(AliRunLoader *runLoader
                                    , AliRawReader *rawReader) const
{
  //
  // Reconstruct clusters
  //

  AliInfo("Reconstruct TRD clusters from RAW data");

  AliLoader *loader = runLoader->GetLoader("TRDLoader");
  loader->LoadRecPoints("recreate");

  runLoader->CdGAFile();
  Int_t nEvents = runLoader->GetNumberOfEvents();

  rawReader->Reset();
  rawReader->Select("TRD");

  for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
    if (!rawReader->NextEvent()) break;
    AliTRDclusterizerV1 clusterer("clusterer","TRD clusterizer");
    clusterer.Open(runLoader->GetFileName(),iEvent);
    clusterer.ReadDigits(rawReader);
    clusterer.MakeClusters();
    clusterer.WriteClusters(-1);
  }

  loader->UnloadRecPoints();

}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(AliRawReader *rawReader
                                    , TTree *clusterTree) const
{
  //
  // Reconstruct clusters
  //

  AliInfo("Reconstruct TRD clusters from RAW data");

  rawReader->Reset();
  rawReader->Select("TRD");

  AliTRDclusterizerV1 clusterer("clusterer","TRD clusterizer");
  clusterer.OpenOutput(clusterTree);
  clusterer.ReadDigits(rawReader);
  clusterer.MakeClusters();

}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(TTree *digitsTree
                                    , TTree *clusterTree) const
{
  //
  // Reconstruct clusters
  //

  AliTRDclusterizerV1 clusterer("clusterer","TRD clusterizer");
  clusterer.OpenOutput(clusterTree);
  clusterer.ReadDigits(digitsTree);
  clusterer.MakeClusters();

}

//_____________________________________________________________________________
void AliTRDReconstructor::Reconstruct(AliRunLoader *runLoader) const
{
  //
  // Reconstruct clusters
  //

  AliLoader *loader = runLoader->GetLoader("TRDLoader");
  loader->LoadRecPoints("recreate");

  runLoader->CdGAFile();
  Int_t nEvents = runLoader->GetNumberOfEvents();

  for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
    AliTRDclusterizerV1 clusterer("clusterer","TRD clusterizer");
    clusterer.Open(runLoader->GetFileName(),iEvent);
    clusterer.ReadDigits();
    clusterer.MakeClusters();
    clusterer.WriteClusters(-1);
  }

  loader->UnloadRecPoints();

}

//_____________________________________________________________________________
AliTracker *AliTRDReconstructor::CreateTracker(AliRunLoader *runLoader) const
{
  //
  // Create a TRD tracker
  //

  runLoader->CdGAFile();

  return new AliTRDtracker(gFile);

}

//_____________________________________________________________________________
void AliTRDReconstructor::FillESD(AliRunLoader* /*runLoader*/
				, AliRawReader* /*rawReader*/
		         	, AliESD *esd) const
{
  //
  // Make PID
  //

  AliTRDpidESD trdPID;
  trdPID.MakePID(esd);

}

//_____________________________________________________________________________
void AliTRDReconstructor::FillESD(AliRawReader* /*rawReader*/
				, TTree* /*clusterTree*/
				, AliESD *esd) const
{
  //
  // Make PID
  //

  AliTRDpidESD trdPID;
  trdPID.MakePID(esd);

}

//_____________________________________________________________________________
void AliTRDReconstructor::FillESD(TTree* /*digitsTree*/
				, TTree* /*clusterTree*/
				, AliESD *esd) const
{
  //
  // Make PID
  //

  AliTRDpidESD trdPID;
  trdPID.MakePID(esd);

}

//_____________________________________________________________________________
void AliTRDReconstructor::FillESD(AliRunLoader* /*runLoader*/
				, AliESD *esd) const
{
  //
  // Make PID
  //

  AliTRDpidESD trdPID;
  trdPID.MakePID(esd);

}
