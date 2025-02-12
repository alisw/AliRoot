//**************************************************************************\
//* This file is property of and copyright by the ALICE Project            *\
//* ALICE Experiment at CERN, All rights reserved.                         *\
//*                                                                        *\
//* Primary Authors: Matthias Richter <Matthias.Richter@ift.uib.no>        *\
//*                  for The ALICE HLT Project.                            *\
//*                                                                        *\
//* Permission to use, copy, modify and distribute this software and its   *\
//* documentation strictly for non-commercial purposes is hereby granted   *\
//* without fee, provided that the above copyright notice appears in all   *\
//* copies and that both the copyright notice and this permission notice   *\
//* appear in the supporting documentation. The authors make no claims     *\
//* about the suitability of this software for any purpose. It is          *\
//* provided "as is" without express or implied warranty.                  *\
//**************************************************************************

/// \file GPUTPCGMTracksToTPCSeeds.cxx
/// \author David Rohr

#include "GPUTPCGMTracksToTPCSeeds.h"
#include "GPUTPCGlobalMergerComponent.h"
#include "GPUTPCGMMergerTypes.h"
#include "GPUTPCGMMerger.h"
#include "GPULogging.h"
#include "AliTPCtracker.h"
#include "AliTPCtrack.h"
#include "AliTPCseed.h"
#include "AliTPCtrackerSector.h"
#include "TObjArray.h"
#include "AliTPCclusterMI.h"

using namespace GPUCA_NAMESPACE::gpu;

void GPUTPCGMTracksToTPCSeeds::CreateSeedsFromHLTTracks(TObjArray* seeds, AliTPCtracker* tpctracker)
{
  const GPUTPCGMMerger* merger = GPUTPCGlobalMergerComponent::GetCurrentMerger();
  if (merger == nullptr) {
    return;
  }
  seeds->Clear();
  int32_t index = 0;
  for (int32_t i = 0; i < merger->NOutputTracks(); i++) {
    const GPUTPCGMMergedTrack& track = merger->OutputTracks()[i];
    if (!track.OK()) {
      continue;
    }

    AliTPCtrack tr;
    tr.Set(track.GetParam().GetX(), track.GetAlpha(), track.GetParam().GetPar(), track.GetParam().GetCov());
    AliTPCseed* seed = new (tpctracker->NextFreeSeed()) AliTPCseed(tr);
    for (int32_t j = 0; j < GPUCA_ROW_COUNT; j++) {
      seed->SetClusterPointer(j, nullptr);
      seed->SetClusterIndex(j, -1);
    }
    int32_t ncls = 0;
    int32_t lastrow = -1;
    int32_t lastleg = -1;
    for (int32_t j = track.NClusters() - 1; j >= 0; j--) {
      const GPUTPCGMMergedTrackHit& cls = merger->Clusters()[track.FirstClusterRef() + j];
      if (cls.state & GPUTPCGMMergedTrackHit::flagReject) {
        continue;
      }
      if (lastrow != -1 && (cls.row < lastrow || cls.leg != lastleg)) {
        break;
      }
      if (cls.row == lastrow) {
        continue;
      }

      AliTPCtrackerRow& row = tpctracker->GetRow(cls.slice % 18, cls.row);
      uint32_t clIndexOffline = 0;
      AliTPCclusterMI* clOffline = row.FindNearest2(cls.y, cls.z, 0.01f, 0.01f, clIndexOffline);
      if (!clOffline) {
        continue;
      }
      clIndexOffline = row.GetIndex(clIndexOffline);

      clOffline->Use(10);
      seed->SetClusterPointer(cls.row, clOffline);
      seed->SetClusterIndex2(cls.row, clIndexOffline);

      lastrow = cls.row;
      lastleg = cls.leg;
      ncls++;
    }

    seed->SetRelativeSector(track.GetAlpha() / (M_PI / 9.f));
    seed->SetNumberOfClusters(ncls);
    seed->SetNFoundable(ncls);
    seed->SetChi2(track.GetParam().GetChi2());

    float alpha = seed->GetAlpha();
    if (alpha >= 2.f * M_PI) {
      alpha -= 2.f * M_PI;
    }
    if (alpha < 0) {
      alpha += 2.f * M_PI;
    }
    seed->SetRelativeSector(track.GetAlpha() / (M_PI / 9.f));

    seed->SetPoolID(tpctracker->GetLastSeedId());
    seed->SetIsSeeding(kTRUE);
    seed->SetSeed1(GPUCA_ROW_COUNT - 1);
    seed->SetSeed2(GPUCA_ROW_COUNT - 2);
    seed->SetSeedType(0);
    seed->SetFirstPoint(-1);
    seed->SetLastPoint(-1);
    seeds->AddLast(seed); // note, track is seed, don't free the seed
    index++;
  }
}

void GPUTPCGMTracksToTPCSeeds::UpdateParamsOuter(TObjArray* seeds)
{
  const GPUTPCGMMerger* merger = GPUTPCGlobalMergerComponent::GetCurrentMerger();
  if (merger == nullptr) {
    return;
  }
  int32_t index = 0;
  for (int32_t i = 0; i < merger->NOutputTracks(); i++) {
    const GPUTPCGMMergedTrack& track = merger->OutputTracks()[i];
    if (!track.OK()) {
      continue;
    }
    if (index > seeds->GetEntriesFast()) {
      GPUError("Invalid number of offline seeds");
      return;
    }
    AliTPCseed* seed = (AliTPCseed*)seeds->UncheckedAt(index++);
    const gputpcgmmergertypes::GPUTPCOuterParam& param = track.OuterParam();
    seed->Set(param.X, param.alpha, param.P, param.C);
  }
}

void GPUTPCGMTracksToTPCSeeds::UpdateParamsInner(TObjArray* seeds)
{
  const GPUTPCGMMerger* merger = GPUTPCGlobalMergerComponent::GetCurrentMerger();
  if (merger == nullptr) {
    return;
  }
  int32_t index = 0;
  for (int32_t i = 0; i < merger->NOutputTracks(); i++) {
    const GPUTPCGMMergedTrack& track = merger->OutputTracks()[i];
    if (!track.OK()) {
      continue;
    }
    if (index > seeds->GetEntriesFast()) {
      GPUError("Invalid number of offline seeds");
      return;
    }
    AliTPCseed* seed = (AliTPCseed*)seeds->UncheckedAt(index++);
    seed->Set(track.GetParam().GetX(), track.GetAlpha(), track.GetParam().GetPar(), track.GetParam().GetCov());
  }
}
