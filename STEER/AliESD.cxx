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

//-----------------------------------------------------------------
//           Implementation of the ESD class
//   This is the class to deal with during the phisical analysis of data
//   This class is generated directly by the reconstruction methods
//      Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch
//-----------------------------------------------------------------

#include "TList.h"
#include <TNamed.h>

#include "AliESD.h"
#include "AliESDfriend.h"
#include "AliESDVZERO.h"
#include "AliESDHLTtrack.h"
#include "AliESDFMD.h"


ClassImp(AliESD)

//______________________________________________________________________________
AliESD::AliESD():
  fESDObjects(new TList()),
  fESDRun(0),
  fHeader(0),
  fESDZDC(0),
  fESDFMD(0),
  fESDVZERO(0),
  fESDTZERO(0),
  fSPDVertex(0),
  fPrimaryVertex(0),
  fSPDMult(0),
  fPHOSTrigger(0),
  fEMCALTrigger(0),
  fTracks(0),
  fMuonTracks(0),
  fPmdTracks(0),
  fTrdTracks(0),
  fV0s(0),  
  fCascades(0),
  fKinks(0),
  fCaloClusters(0),
  fErrorLogs(0),
  fEMCALClusters(0), 
  fFirstEMCALCluster(-1),
  fPHOSClusters(0), 
  fFirstPHOSCluster(-1)
{
}
//______________________________________________________________________________
AliESD::AliESD(const AliESD& esd):
  TObject(esd),
  fESDObjects(new TList()),
  fESDRun(new AliESDRun(*esd.fESDRun)),
  fHeader(new AliESDHeader(*esd.fHeader)),
  fESDZDC(new AliESDZDC(*esd.fESDZDC)),
  fESDFMD(new AliESDFMD(*esd.fESDFMD)),
  fESDVZERO(new AliESDVZERO(*esd.fESDVZERO)),
  fESDTZERO(new AliESDTZERO(*esd.fESDTZERO)),
  fSPDVertex(new AliESDVertex(*esd.fSPDVertex)),
  fPrimaryVertex(new AliESDVertex(*esd.fPrimaryVertex)),
  fSPDMult(new AliMultiplicity(*esd.fSPDMult)),
  fPHOSTrigger(new AliESDCaloTrigger(*esd.fPHOSTrigger)),
  fEMCALTrigger(new AliESDCaloTrigger(*esd.fEMCALTrigger)),
  fTracks(new TClonesArray(*esd.fTracks)),
  fMuonTracks(new TClonesArray(*esd.fMuonTracks)),
  fPmdTracks(new TClonesArray(*esd.fPmdTracks)),
  fTrdTracks(new TClonesArray(*esd.fTrdTracks)),
  fV0s(new TClonesArray(*esd.fV0s)),  
  fCascades(new TClonesArray(*esd.fCascades)),
  fKinks(new TClonesArray(*esd.fKinks)),
  fCaloClusters(new TClonesArray(*esd.fCaloClusters)),
  fErrorLogs(new TClonesArray(*esd.fErrorLogs)),
  fEMCALClusters(esd.fEMCALClusters), 
  fFirstEMCALCluster(esd.fFirstEMCALCluster),
  fPHOSClusters(esd.fPHOSClusters), 
  fFirstPHOSCluster(esd.fFirstPHOSCluster)

{
  // CKB init in the constructor list and only add here ...
  AddObject(fESDRun);
  AddObject(fHeader);
  AddObject(fESDZDC);
  AddObject(fESDFMD);
  AddObject(fESDVZERO);
  AddObject(fESDTZERO);
  AddObject(fSPDVertex);
  AddObject(fPrimaryVertex);
  AddObject(fSPDMult);
  AddObject(fPHOSTrigger);
  AddObject(fEMCALTrigger);
  AddObject(fTracks);
  AddObject(fMuonTracks);
  AddObject(fPmdTracks);
  AddObject(fTrdTracks);
  AddObject(fV0s);
  AddObject(fCascades);
  AddObject(fKinks);
  AddObject(fCaloClusters);
  AddObject(fErrorLogs);

  GetStdContent();

}

//______________________________________________________________________________
AliESD & AliESD::operator=(const AliESD& source) {

  // Assignment operator

  if(&source == this) return *this;
  TObject::operator=(source);

  fESDRun = new AliESDRun(*source.fESDRun);
  fHeader = new AliESDHeader(*source.fHeader);
  fESDZDC = new AliESDZDC(*source.fESDZDC);
  fESDFMD = new AliESDFMD(*source.fESDFMD);
  fESDVZERO = new AliESDVZERO(*source.fESDVZERO);
  fESDTZERO = new AliESDTZERO(*source.fESDTZERO);
  fSPDVertex = new AliESDVertex(*source.fSPDVertex);
  fPrimaryVertex = new AliESDVertex(*source.fPrimaryVertex);
  fSPDMult = new AliMultiplicity(*source.fSPDMult);
  fPHOSTrigger = new AliESDCaloTrigger(*source.fPHOSTrigger);
  fEMCALTrigger = new AliESDCaloTrigger(*source.fEMCALTrigger);
  fTracks = new TClonesArray(*source.fTracks);
  fMuonTracks = new TClonesArray(*source.fMuonTracks);
  fPmdTracks = new TClonesArray(*source.fPmdTracks);
  fTrdTracks = new TClonesArray(*source.fTrdTracks);
  fV0s = new TClonesArray(*source.fV0s);
  fCascades = new TClonesArray(*source.fCascades);
  fKinks = new TClonesArray(*source.fKinks);
  fCaloClusters = new TClonesArray(*source.fCaloClusters);
  fErrorLogs = new TClonesArray(*source.fErrorLogs);

  // CKB this way?? or 
  // or AddObject(  fESDZDC = new AliESDZDC(*source.fESDZDC));

  fESDObjects = new TList();
  AddObject(fESDRun);
  AddObject(fHeader);
  AddObject(fESDZDC);
  AddObject(fESDFMD);
  AddObject(fESDVZERO);
  AddObject(fESDTZERO);
  AddObject(fSPDVertex);
  AddObject(fPrimaryVertex);
  AddObject(fSPDMult);
  AddObject(fPHOSTrigger);
  AddObject(fEMCALTrigger);
  AddObject(fTracks);
  AddObject(fMuonTracks);
  AddObject(fPmdTracks);
  AddObject(fTrdTracks);
  AddObject(fV0s);
  AddObject(fCascades);
  AddObject(fKinks);
  AddObject(fCaloClusters);
  AddObject(fErrorLogs);


  fEMCALClusters = source.fEMCALClusters;
  fFirstEMCALCluster = source.fFirstEMCALCluster;
  fPHOSClusters = source.fPHOSClusters;
  fFirstPHOSCluster = source.fFirstPHOSCluster;



  return *this;

}


//______________________________________________________________________________
AliESD::~AliESD()
{
  //
  // Standard destructor
  //

  delete fESDObjects;
  fESDObjects = 0;

  // everthing on the list gets deleted automatically

  /*
  fHLTConfMapTracks.Delete();
  fHLTHoughTracks.Delete();
  fMuonTracks.Delete();  
  fPmdTracks.Delete();
  fTrdTracks.Delete();
  fV0s.Delete();
  fCascades.Delete();
  fKinks.Delete();
  fCaloClusters.Delete();
  */
//   fEMCALTriggerPosition->Delete();
//   fEMCALTriggerAmplitudes->Delete();
//   fPHOSTriggerPosition->Delete();
//   fPHOSTriggerAmplitudes->Delete();
//   delete fEMCALTriggerPosition;
//   delete fEMCALTriggerAmplitudes;
//   delete fPHOSTriggerPosition;
//   delete fPHOSTriggerAmplitudes;

}

//______________________________________________________________________________
void AliESD::Reset()
{
  // Reset the standard contents
  if(fESDRun) fESDRun->Reset();
  if(fHeader) fHeader->Reset();
  if(fESDZDC) fESDZDC->Reset();
  if(fESDFMD) fESDFMD->Clear(); // why clear.... need consistend names
  // if(fESDVZERO) fESDVZERO->; // NOT IMPLEMENTED 
  //  if(fESDVZERO) new (fESDVZERO) AliESDVZERO();
  if(fESDTZERO) fESDTZERO->Reset(); 
  // CKB no clear/reset implemented
  if(fSPDVertex){
    new (fSPDVertex) AliESDVertex();
    fSPDVertex->SetName("SPDVertex");
  }
  if(fPrimaryVertex){
    new (fPrimaryVertex) AliESDVertex();
    fPrimaryVertex->SetName("PrimaryVertex");
  }
  if(fSPDMult)new (fSPDMult) AliMultiplicity();
  if(fPHOSTrigger)fPHOSTrigger->Reset(); 
  if(fEMCALTrigger)fEMCALTrigger->Reset(); 
  if(fTracks)fTracks->Clear();
  if(fMuonTracks)fMuonTracks->Clear();
  if(fPmdTracks)fPmdTracks->Clear();
  if(fTrdTracks)fTrdTracks->Clear();
  if(fV0s)fV0s->Clear();
  if(fCascades)fCascades->Clear();
  if(fKinks)fKinks->Clear();
  if(fCaloClusters)fCaloClusters->Clear();
  if(fErrorLogs) fErrorLogs->Clear();


  fEMCALClusters=0; 
  fFirstEMCALCluster=-1; 
  fPHOSClusters=0; 
  fFirstPHOSCluster=-1; 
}

Int_t AliESD::AddV0(const AliESDv0 *v) {
  //
  // Add V0
  //
  TClonesArray &fv = *fV0s;
  Int_t idx=fV0s->GetEntriesFast();
  new(fv[idx]) AliESDv0(*v);
  return idx;
}  

//______________________________________________________________________________
void AliESD::Print(Option_t *) const 
{
  //
  // Print header information of the event
  //
  printf("ESD run information\n");
  printf("Event # in file %d Bunch crossing # %d Orbit # %d Period # %d Run # %d Trigger %lld Magnetic field %f \n",
	 GetEventNumberInFile(),
	 GetBunchCrossNumber(),
	 GetOrbitNumber(),
	 GetPeriodNumber(),
	 GetRunNumber(),
	 GetTriggerMask(),
	 GetMagneticField() );
  printf("Vertex: (%.4f +- %.4f, %.4f +- %.4f, %.4f +- %.4f) cm\n",
	   fPrimaryVertex->GetXv(), fPrimaryVertex->GetXRes(),
	   fPrimaryVertex->GetYv(), fPrimaryVertex->GetYRes(),
	   fPrimaryVertex->GetZv(), fPrimaryVertex->GetZRes());
    printf("Mean vertex in RUN: X=%.4f Y=%.4f cm\n",
	   GetDiamondX(),GetDiamondY());
    printf("SPD Multiplicity. Number of tracklets %d \n",
           fSPDMult->GetNumberOfTracklets());
  printf("Number of tracks: \n");
  printf("                 charged   %d\n", GetNumberOfTracks());
  printf("                 muon      %d\n", GetNumberOfMuonTracks());
  printf("                 pmd       %d\n", GetNumberOfPmdTracks());
  printf("                 trd       %d\n", GetNumberOfTrdTracks());
  printf("                 v0        %d\n", GetNumberOfV0s());
  printf("                 cascades  %d\n", GetNumberOfCascades());
  printf("                 kinks     %d\n", GetNumberOfKinks());
  printf("                 CaloClusters %d\n", GetNumberOfCaloClusters());
  printf("                 phos      %d\n", GetNumberOfPHOSClusters());
  printf("                 emcal     %d\n", GetNumberOfEMCALClusters());
  printf("                 FMD       %s\n", (fESDFMD ? "yes" : "no"));
  printf("                 VZERO     %s\n", (fESDVZERO ? "yes" : "no"));
}

void AliESD::SetESDfriend(const AliESDfriend *ev) {
  //
  // Attaches the complementary info to the ESD
  //
  if (!ev) return;

  Int_t ntrk=ev->GetNumberOfTracks();
 
  for (Int_t i=0; i<ntrk; i++) {
    const AliESDfriendTrack *f=ev->GetTrack(i);
    GetTrack(i)->SetFriendTrack(f);
  }
}

Int_t  AliESD::AddTrack(const AliESDtrack *t) {
    // Add track
    TClonesArray &ftr = *fTracks;
    AliESDtrack * track = new(ftr[fTracks->GetEntriesFast()])AliESDtrack(*t);
    track->SetID(fTracks->GetEntriesFast()-1);
    return  track->GetID();    
}

Int_t AliESD::AddKink(const AliESDkink *c) {
    // Add kink
    TClonesArray &fk = *fKinks;
    AliESDkink * kink = new(fk[fKinks->GetEntriesFast()]) AliESDkink(*c);
    kink->SetID(fKinks->GetEntriesFast()); // CKB different from the other imps..
    return fKinks->GetEntriesFast()-1;
}

Int_t AliESD::AddCaloCluster(const AliESDCaloCluster *c) {
    // Add calocluster
    TClonesArray &fc = *fCaloClusters;
    AliESDCaloCluster *clus = new(fc[fCaloClusters->GetEntriesFast()]) AliESDCaloCluster(*c);
    clus->SetID(fCaloClusters->GetEntriesFast()-1);
    return fCaloClusters->GetEntriesFast()-1;
  }


void AliESD::SetFMDData(AliESDFMD * obj) { 
  // use already allocated space
  if(fESDFMD){
    new(fESDFMD) AliESDFMD(*obj); 
  }
}

void AliESD::SetVZEROData(AliESDVZERO * obj){ 
  // use already allocated space
  if(fESDVZERO)
    new(fESDVZERO) AliESDVZERO(*obj);
}

void AliESD::GetESDfriend(AliESDfriend *ev) const {
  //
  // Extracts the complementary info from the ESD
  //
  if (!ev) return;

  Int_t ntrk=GetNumberOfTracks();

  for (Int_t i=0; i<ntrk; i++) {
    const AliESDtrack *t=GetTrack(i);
    const AliESDfriendTrack *f=t->GetFriendTrack();
    ev->AddTrack(f);
  }
}


void AliESD::AddObject(TObject* obj) 
{
  // Add an object to the list of object.
  // Please be aware that in order to increase performance you should
  // refrain from using TObjArrays (if possible). Use TClonesArrays, instead.
  fESDObjects->AddLast(obj);
}


void AliESD::GetStdContent() 
{
  // set pointers for standard content

  fESDRun = (AliESDRun*)fESDObjects->At(kESDRun);
  fHeader = (AliESDHeader*)fESDObjects->At(kHeader);
  fESDZDC = (AliESDZDC*)fESDObjects->At(kESDZDC);
  fESDFMD = (AliESDFMD*)fESDObjects->At(kESDFMD);
  fESDVZERO = (AliESDVZERO*)fESDObjects->At(kESDVZERO);
  fESDTZERO = (AliESDTZERO*)fESDObjects->At(kESDTZERO);
  fSPDVertex = (AliESDVertex*)fESDObjects->At(kSPDVertex);
  fPrimaryVertex = (AliESDVertex*)fESDObjects->At(kPrimaryVertex);
  fSPDMult =       (AliMultiplicity*)fESDObjects->At(kSPDMult);
  fPHOSTrigger = (AliESDCaloTrigger*)fESDObjects->At(kPHOSTrigger);
  fEMCALTrigger = (AliESDCaloTrigger*)fESDObjects->At(kEMCALTrigger);
  fTracks = (TClonesArray*)fESDObjects->At(kTracks);
  fMuonTracks = (TClonesArray*)fESDObjects->At(kMuonTracks);
  fPmdTracks = (TClonesArray*)fESDObjects->At(kPmdTracks);
  fTrdTracks = (TClonesArray*)fESDObjects->At(kTrdTracks);
  fV0s = (TClonesArray*)fESDObjects->At(kV0s);
  fCascades = (TClonesArray*)fESDObjects->At(kCascades);
  fKinks = (TClonesArray*)fESDObjects->At(kKinks);
  fCaloClusters = (TClonesArray*)fESDObjects->At(kCaloClusters);
  fErrorLogs = (TClonesArray*)fESDObjects->At(kErrorLogs);

}

void AliESD::SetStdNames(){
  // Set the names of the standard contents
  fSPDVertex->SetName("SPDVertex");
  fPrimaryVertex->SetName("PrimaryVertex");
  fPHOSTrigger->SetName("PHOSTrigger");
  fEMCALTrigger->SetName("EMCALTrigger");
  fTracks->SetName("Tracks");
  fMuonTracks->SetName("MuonTracks");
  fPmdTracks->SetName("PmdTracks");
  fTrdTracks->SetName("TrdTracks");
  fV0s->SetName("V0s");
  fCascades->SetName("Cascades");
  fKinks->SetName("Kinks");
  fCaloClusters->SetName("CaloClusters");

} 

void AliESD::CreateStdContent() 
{
  // create the standard AOD content and set pointers

  // create standard objects and add them to the TList of objects
  AddObject(new AliESDRun());
  AddObject(new AliESDHeader());
  AddObject(new AliESDZDC());
  AddObject(new AliESDFMD());
  AddObject(new AliESDVZERO());
  AddObject(new AliESDTZERO());
  AddObject(new AliESDVertex());
  AddObject(new AliESDVertex());
  AddObject(new AliMultiplicity());
  AddObject(new AliESDCaloTrigger());
  AddObject(new AliESDCaloTrigger());
  AddObject(new TClonesArray("AliESDtrack",0));
  AddObject(new TClonesArray("AliESDMuonTrack",0));
  AddObject(new TClonesArray("AliESDPmdTrack",0));
  AddObject(new TClonesArray("AliESDTrdTrack",0));
  AddObject(new TClonesArray("AliESDv0",0));
  AddObject(new TClonesArray("AliESDcascade",0));
  AddObject(new TClonesArray("AliESDkink",0));
  AddObject(new TClonesArray("AliESDCaloCluster",0));
  AddObject(new TClonesArray("AliRawDataErrorLog",0));

  // check the order of the indices against enum...

  // read back pointers
  GetStdContent();
  // set names
  SetStdNames();

}

void AliESD::ReadFromTree(TTree *tree){
  

  // is this really so smart that an ESDObject has a pointer to a list
  // of another ESDObject...

  fESDObjects = (TList*)((AliESD*)tree->GetTree()->GetUserInfo()->FindObject("AliESD"))->GetList(); 

  if(fESDObjects->GetEntries()<kESDListN){
    printf("%s %d AliESD::ReadFromTree() TList contains less than the standard contents %d < %d \n",(char*)__FILE__,__LINE__,fESDObjects->GetEntries(),kESDListN);
  }

  // if list is empty
  // we could still set the branch adresses based on 
  // tree->GetListOfBranches() CKB
  // or create standard list 

  // set the branch addresses
  TIter next(fESDObjects);
  TNamed *el;
  while((el=(TNamed*)next())){
    TString bname(el->GetName());

    if(bname.CompareTo("AliESDfriend")==0)
      {
	// AliESDfriend does not have a name ...
      tree->SetBranchStatus("ESDfriend.*",1);
      printf("Friend %s\n", bname.Data());
      tree->SetBranchAddress("ESDfriend.",fESDObjects->GetObjectRef(el));


    }
    else{
      printf("%s\n", bname.Data());
      tree->SetBranchAddress(bname.Data(),fESDObjects->GetObjectRef(el));
    }
  }

  GetStdContent();
}



