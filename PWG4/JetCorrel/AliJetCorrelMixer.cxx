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
/* $Id: $ */

//__________________________________________
// Event mixing class. A 6-dimensinal pool fPool is maintained:
// type(fixed), type_idx(fixed), vertex(fixed), centrality(fixed), 
// and event(dynamic), particle(dynamic)
// fixed dimensions are fixed at task initialization time (via AliJetCorrelSelector)
// event&particle are allow to float during runtime (via linked lists TList=event & CorrelList_t=particle)
//-- Author: Paul Constantin

#include "AliJetCorrelMixer.h"

using namespace std;
using namespace JetCorrelHD;

ClassImp(AliJetCorrelMixer)

AliJetCorrelMixer::AliJetCorrelMixer() :
  fSelector(NULL), fMaker(NULL), fWriter(NULL),
  TriggEvnt(new CorrelList_t), AssocEvnt(new CorrelList_t),
  fNumCentBins(0), fNumVertBins(0), fPoolDepth(0), fNumCorrel(0), fNumTriggs(0), fNumAssocs(0){
  // constructor
}

AliJetCorrelMixer::~AliJetCorrelMixer(){
  // destructor
  CleanPool(triggs);
  CleanPool(assocs);
  if(TriggEvnt) delete TriggEvnt;
  if(AssocEvnt) delete AssocEvnt;
}

void AliJetCorrelMixer::Init(AliJetCorrelSelector * const s, AliJetCorrelMaker * const m, AliJetCorrelWriter * const w){
  // initialization method
  fSelector = s;
  fMaker = m;
  fWriter = w;
  
  fNumCentBins = fSelector->NoOfBins(centr);  
  fNumVertBins = fSelector->NoOfBins(zvert);
  fPoolDepth   = fSelector->PoolDepth();
  fNumCorrel = fMaker->NoOfCorrel();
  fNumTriggs = fMaker->NoOfTrigg();
  fNumAssocs = fMaker->NoOfAssoc();
  
  for(UInt_t vBin=0; vBin<fNumVertBins; vBin++){
    for(UInt_t cBin=0; cBin<fNumCentBins; cBin++){
      for(UInt_t it=0; it<fNumTriggs; it++)
	fPool[triggs][it][vBin][cBin] = new TList;
      for(UInt_t ia=0; ia<fNumAssocs; ia++)
	fPool[assocs][ia][vBin][cBin] = new TList;
    }
  }
}

void AliJetCorrelMixer::FillPool(CorrelList_t *partList, UInt_t pIdx, UInt_t vBin, UInt_t cBin){
  // pool filling method
  if(partList->Size()<1) return;
  PoolType_t pType = partList->PoolID();
  UInt_t pSize = fPool[pType][pIdx][vBin][cBin]->GetSize();
  // when pool depth is reached, pop pool before new event push (keep const depth)
  if(pSize>=fPoolDepth) fPool[pType][pIdx][vBin][cBin]->RemoveFirst();
  // incoming list is cleared at end-of-event; hence, store in pool a deep copy:
  fPool[pType][pIdx][vBin][cBin]->AddLast(partList->DeepCopy());
}

void AliJetCorrelMixer::Mix(UInt_t vBin, UInt_t cBin, UInt_t it, UInt_t ia, UInt_t ic){
  // rolling buffer mixing method
  TListIter* iterAssocPool=(TListIter*)fPool[assocs][ia][vBin][cBin]->MakeIterator();
  while(fPool[triggs][it][vBin][cBin]->GetSize()>0){ // evaluate here due to popping
    TriggEvnt = (CorrelList_t*)fPool[triggs][it][vBin][cBin]->First();
    TriggIter = TriggEvnt->Head();
    
    while((AssocEvnt=(CorrelList_t*)iterAssocPool->Next())){
      AssocIter = AssocEvnt->Head();
      
      if(TriggEvnt->EvtID()==AssocEvnt->EvtID()) continue; // don't mix same event!
      
      while(!TriggIter.HasEnded()){
	while(!AssocIter.HasEnded()){
	  fWriter->FillCorrelations(mixed,ic,cBin,vBin,TriggIter.Data(),AssocIter.Data()); // trigg first!
	  AssocIter.Move();
	} // loop over associated particles
	AssocIter = AssocEvnt->Head(); // reset associated particle iterator to list head
	TriggIter.Move();
      } // loop over trigger particles
      TriggIter = TriggEvnt->Head(); // reset trigger particle iterator to list head
    } // loop over associated pool
    fPool[triggs][it][vBin][cBin]->RemoveFirst();
  } // if trigger pool is not empty
}

void AliJetCorrelMixer::CleanPool(PoolType_t pType){
  // pool cleaning
  UInt_t size = 0;
  if(pType==triggs) size = fNumTriggs;
  if(pType==assocs) size = fNumAssocs;
  for(UInt_t k=0; k<size; k++){
    for(UInt_t vBin=0; vBin<fNumVertBins; vBin++)
      for(UInt_t cBin=0; cBin<fNumCentBins; cBin++)
	fPool[pType][k][vBin][cBin]->Delete(); // Remove all list objects AND delete all heap based objects
  }
}

UInt_t AliJetCorrelMixer::PoolSize(PoolType_t pType, UInt_t vBin, UInt_t cBin){
  // computes (static) pool size
  UInt_t totalPoolSize=0;
  UInt_t partSize = sizeof(CorrelParticle_t);
  UInt_t listOverhead = sizeof(CorrelListNode_t);
  CorrelList_t* partList; TListIter* iter;
  UInt_t fNum = fNumAssocs; if(pType==triggs) fNum = fNumTriggs;
  for(UInt_t i=0; i<fNum; i++){
    iter=(TListIter*)fPool[pType][i][vBin][cBin]->MakeIterator();
    while((partList=(CorrelList_t*)iter->Next()))
      totalPoolSize += partList->Size()*(partSize+listOverhead);
  }
  return totalPoolSize/1024;
}

void AliJetCorrelMixer::ShowSummary(PoolType_t pType, UInt_t pIdx, UInt_t vBin, UInt_t cBin){
  // pool printout method
  UInt_t totalPoolSize=0;
  TListIter* iter=(TListIter*)fPool[pType][pIdx][vBin][cBin]->MakeIterator();
  CorrelList_t* partList;
  while((partList=(CorrelList_t*)iter->Next())) totalPoolSize += partList->Size();

  if(pType==triggs) std::cout<<"TriggPool[";
  if(pType==assocs) std::cout<<"AssocPool[";
  std::cout<<vBin<<"]["<<cBin<<"]: nevt="<<fPool[pType][pIdx][vBin][cBin]->GetSize()<<" npart="<<totalPoolSize<<std::endl;
}

void AliJetCorrelMixer::ShowPool(PoolType_t pType, UInt_t vBin, UInt_t cBin){
  // all pools printout
  CorrelList_t* tempList; TListIter* iter;
  UInt_t size = 0;
  if(pType==triggs) size = fNumTriggs;
  if(pType==assocs) size = fNumAssocs;
  for(UInt_t k=0; k<size; k++){
    iter=(TListIter*)fPool[pType][k][vBin][cBin]->MakeIterator();
    while((tempList=(CorrelList_t*)iter->Next())) tempList->Show();
  }
}
