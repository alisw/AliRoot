// -*- mode: C++ -*- 
#ifndef ALIESD_H
#define ALIESD_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */


/* $Id$ */

//-------------------------------------------------------------------------
//                          Class AliESD
//   This is the class to deal with during the physical analysis of data
//      
//         Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//-------------------------------------------------------------------------

#include <TClonesArray.h>
#include <TObject.h>
#include <TNamed.h>
#include <TList.h>
#include <TTree.h>
#include <TArrayF.h>
#include <TRef.h>

#include "AliESDMuonTrack.h"
#include "AliESDPmdTrack.h"
#include "AliESDTrdTrack.h"
#include "AliESDVertex.h"
#include "AliESDcascade.h"
#include "AliESDkink.h"
#include "AliESDtrack.h"
#include "AliESDfriend.h"
#include "AliESDHLTtrack.h"
#include "AliESDCaloCluster.h"
#include "AliESDv0.h"
#include "AliESDFMD.h"
#include "AliESDVZERO.h"
#include "AliMultiplicity.h"
#include "AliRawDataErrorLog.h"

class AliESDfriend;

class AliESDRun: public TObject {
public:

  AliESDRun();
  AliESDRun(const AliESDRun&);
  AliESDRun& operator=(const AliESDRun&);


  Int_t   GetRunNumber() const {return fRunNumber;}
  void    SetRunNumber(Int_t n) {fRunNumber=n;}
  void    SetMagneticField(Float_t mf){fMagneticField = mf;}
  Float_t GetMagneticField() const {return fMagneticField;}
  Int_t   GetPeriodNumber() const {return fPeriodNumber;}
  void    SetPeriodNumber(Int_t n) {fPeriodNumber=n;}
  void    Reset();
  void    Print(const Option_t *opt=0) const;

  void SetDiamond(const AliESDVertex *vertex) {
    fDiamondXY[0]=vertex->GetXv();
    fDiamondXY[1]=vertex->GetYv();
    Double32_t cov[6];
    vertex->GetCovMatrix(cov);
    fDiamondCovXY[0]=cov[0];
    fDiamondCovXY[1]=cov[1];
    fDiamondCovXY[2]=cov[2];
  }
  Float_t GetDiamondX() const {return fDiamondXY[0];}
  Float_t GetDiamondY() const {return fDiamondXY[1];}
  Float_t GetSigma2DiamondX() const {return fDiamondCovXY[0];}
  Float_t GetSigma2DiamondY() const {return fDiamondCovXY[2];}
  void GetDiamondCovXY(Float_t cov[3]) const {
    for(Int_t i=0;i<3;i++) cov[i]=fDiamondCovXY[i]; return;
  }
private:
  Int_t        fRunNumber;       // Run Number
  Int_t        fPeriodNumber;       // PeriodNumber
  Int_t        fRecoVersion;     // Version of reconstruction 
  Float_t      fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF
  Float_t      fDiamondXY[2];    // Interaction diamond (x,y) in RUN
  Float_t      fDiamondCovXY[3]; // Interaction diamond covariance (x,y) in RUN

  ClassDef(AliESDRun,1)
};





class AliESDHeader: public TObject {
public:
  AliESDHeader();
  AliESDHeader(const AliESDHeader&);
  AliESDHeader& operator=(const AliESDHeader&);

  void      SetTriggerMask(ULong64_t n) {fTriggerMask=n;}
  void      SetOrbitNumber(UInt_t n) {fOrbitNumber=n;}
  void      SetTimeStamp(UInt_t timeStamp){fTimeStamp = timeStamp;}
  void      SetEventType(UInt_t eventType){fEventType = eventType;}
  void      SetEventNumberInFile(Int_t n) {fEventNumberInFile=n;}
  void      SetBunchCrossNumber(UShort_t n) {fBunchCrossNumber=n;}
  void      SetTriggerCluster(UChar_t n) {fTriggerCluster = n;}

  ULong64_t GetTriggerMask() const {return fTriggerMask;}
  UInt_t    GetOrbitNumber() const {return fOrbitNumber;}
  UInt_t    GetTimeStamp()  const { return fTimeStamp;}
  UInt_t    GetEventType()  const { return fEventType;}
  Int_t     GetEventNumberInFile() const {return fEventNumberInFile;}
  UShort_t  GetBunchCrossNumber() const {return fBunchCrossNumber;}
  UChar_t   GetTriggerCluster() const {return fTriggerCluster;}



  void      Reset();
  void    Print(const Option_t *opt=0) const;
private:

  // Event Identification
  ULong64_t    fTriggerMask;       // Trigger Type (mask)
  UInt_t       fOrbitNumber;       // Orbit Number
  UInt_t       fTimeStamp;         // Time stamp
  UInt_t       fEventType;         // Type of Event
  Int_t        fEventNumberInFile; // running Event count in the file
  UShort_t     fBunchCrossNumber;  // Bunch Crossing Number
  UChar_t      fTriggerCluster;    // Trigger cluster (mask)
  
  ClassDef(AliESDHeader,1)
};

class AliESDZDC: public TObject {
public:
  AliESDZDC();
  AliESDZDC(const AliESDZDC&);
  AliESDZDC& operator=(const AliESDZDC&);

  Float_t GetZDCN1Energy() const {return fZDCN1Energy;}
  Float_t GetZDCP1Energy() const {return fZDCP1Energy;}
  Float_t GetZDCN2Energy() const {return fZDCN2Energy;}
  Float_t GetZDCP2Energy() const {return fZDCP2Energy;}
  Float_t GetZDCEMEnergy() const {return fZDCEMEnergy;}
  Int_t   GetZDCParticipants() const {return fZDCParticipants;}
  void    SetZDC(Float_t n1Energy, Float_t p1Energy, Float_t emEnergy,
                 Float_t n2Energy, Float_t p2Energy, Int_t participants) 
   {fZDCN1Energy=n1Energy; fZDCP1Energy=p1Energy; fZDCEMEnergy=emEnergy;
    fZDCN2Energy=n2Energy; fZDCP2Energy=p2Energy; fZDCParticipants=participants;}

  void    Reset();
  void    Print(const Option_t *opt=0) const;

private:

  Float_t      fZDCN1Energy;      // reconstructed energy in the neutron ZDC
  Float_t      fZDCP1Energy;      // reconstructed energy in the proton ZDC
  Float_t      fZDCN2Energy;      // reconstructed energy in the neutron ZDC
  Float_t      fZDCP2Energy;      // reconstructed energy in the proton ZDC
  Float_t      fZDCEMEnergy;     // reconstructed energy in the electromagnetic ZDC
  Int_t        fZDCParticipants; // number of participants estimated by the ZDC

  ClassDef(AliESDZDC,1)
};


class AliESDTZERO: public TObject {
public:
  AliESDTZERO();
  AliESDTZERO(const AliESDTZERO&);
  AliESDTZERO& operator=(const AliESDTZERO&);

  Float_t GetT0zVertex() const {return fT0zVertex;}
  void SetT0zVertex(Float_t z) {fT0zVertex=z;}
  Float_t GetT0() const {return fT0timeStart;}
  void SetT0(Float_t timeStart) {fT0timeStart = timeStart;}
  const Float_t * GetT0time() const {return fT0time;}
  void SetT0time(Float_t time[24]) {
    for (Int_t i=0; i<24; i++) fT0time[i] = time[i];
  }
  const Float_t * GetT0amplitude() const {return fT0amplitude;}
  void SetT0amplitude(Float_t amp[24]) {
    for (Int_t i=0; i<24; i++) fT0amplitude[i] = amp[i];
  }

  void    Reset();
  void    Print(const Option_t *opt=0) const;

private:

  Float_t      fT0zVertex;       // vertex z position estimated by the T0
  Float_t      fT0timeStart;     // interaction time estimated by the T0
  Float_t      fT0time[24];      // best TOF on each T0 PMT
  Float_t      fT0amplitude[24]; // number of particles(MIPs) on each T0 PMT

  ClassDef(AliESDTZERO,1)
};


class AliESDCaloTrigger : public TNamed {
public:
  AliESDCaloTrigger();
  AliESDCaloTrigger(const  AliESDCaloTrigger&);
  AliESDCaloTrigger& operator=(const  AliESDCaloTrigger&);
  virtual ~AliESDCaloTrigger();

  // does this create mem leak? CKB use new with placement?
  void AddTriggerPosition(const TArrayF array)  { fTriggerPosition     = new TArrayF(array); }
  void AddTriggerAmplitudes(const TArrayF array) { fTriggerAmplitudes  = new TArrayF(array); }
  
  void Reset(); 

  TArrayF* GetTriggerPosition()    {return fTriggerPosition;}
  TArrayF* GetTriggerAmplitudes()  {return fTriggerPosition;}
  

private:

  TArrayF *fTriggerAmplitudes;
  TArrayF *fTriggerPosition;

  ClassDef(AliESDCaloTrigger,1)
};

class AliESD : public TObject {
public:


  enum ESDListIndex_t {kESDRun,
		       kHeader,
		       kESDZDC,
		       kESDFMD,
		       kESDVZERO,
		       kESDTZERO,
		       kSPDVertex,
		       kPrimaryVertex,
		       kSPDMult,
		       kPHOSTrigger,
		       kEMCALTrigger,
		       kTracks,
		       kMuonTracks,
		       kPmdTracks,
		       kTrdTracks,
		       kV0s,
		       kCascades,
		       kKinks,
		       kCaloClusters,
		       kErrorLogs,
		       kESDListN
  };

  AliESD();
  virtual ~AliESD(); 


  // RUN
  // move this to the UserData!!!
  const AliESDRun*    GetESDRun() {return fESDRun;}

  // Delegated methods for fESDRun
  void    SetRunNumber(Int_t n) {fESDRun->SetRunNumber(n);}
  Int_t   GetRunNumber() const {return fESDRun->GetRunNumber();}
  void    SetPeriodNumber(Int_t n){fESDRun->SetPeriodNumber(n);}
  Int_t   GetPeriodNumber() const {return fESDRun->GetPeriodNumber();}
  void    SetMagneticField(Float_t mf){fESDRun->SetMagneticField(mf);}
  Float_t GetMagneticField() const {return fESDRun->GetMagneticField();}
  void SetDiamond(const AliESDVertex *vertex) { fESDRun->SetDiamond(vertex);}
  Float_t GetDiamondX() const {return fESDRun->GetDiamondX();}
  Float_t GetDiamondY() const {return fESDRun->GetDiamondY();}
  Float_t GetSigma2DiamondX() const {return  fESDRun->GetSigma2DiamondX();}
  Float_t GetSigma2DiamondY() const {return  fESDRun->GetSigma2DiamondY();}
  void GetDiamondCovXY(Float_t cov[3]) const {fESDRun->GetDiamondCovXY(cov);}   
  

  // HEADER
  const AliESDHeader* GetHeader() {return fHeader;}

  // Delegated methods for fHeader
  void      SetTriggerMask(ULong64_t n) {fHeader->SetTriggerMask(n);}
  void      SetOrbitNumber(UInt_t n) {fHeader->SetOrbitNumber(n);}
  void      SetTimeStamp(UInt_t timeStamp){fHeader->SetTimeStamp(timeStamp);}
  void      SetEventType(UInt_t eventType){fHeader->SetEventType(eventType);}
  void      SetEventNumberInFile(Int_t n) {fHeader->SetEventNumberInFile(n);}
  //  void      SetRunNumber(Int_t n) {fHeader->SetRunNumber(n);}
  void      SetBunchCrossNumber(UShort_t n) {fHeader->SetBunchCrossNumber(n);}
  void      SetTriggerCluster(UChar_t n) {fHeader->SetTriggerCluster(n);}
  ULong64_t GetTriggerMask() const {return fHeader->GetTriggerMask();}
  UInt_t    GetOrbitNumber() const {return fHeader->GetOrbitNumber();}
  UInt_t    GetTimeStamp()  const { return fHeader->GetTimeStamp();}
  UInt_t    GetEventType()  const { return fHeader->GetEventType();}
  Int_t     GetEventNumberInFile() const {return fHeader->GetEventNumberInFile();}
  UShort_t  GetBunchCrossNumber() const {return fHeader->GetBunchCrossNumber();}
  UChar_t   GetTriggerCluster() const {return fHeader->GetTriggerCluster();}

  // ZDC CKB: put this in the header?
  const AliESDZDC*    GetESDZDC() {return fESDZDC;}

  // Delegated methods for fESDZDC
  Float_t GetZDCN1Energy() const {return fESDZDC->GetZDCN1Energy();}
  Float_t GetZDCP1Energy() const {return fESDZDC->GetZDCP1Energy();}
  Float_t GetZDCN2Energy() const {return fESDZDC->GetZDCN2Energy();}
  Float_t GetZDCP2Energy() const {return fESDZDC->GetZDCP2Energy();}
  Float_t GetZDCEMEnergy() const {return fESDZDC->GetZDCEMEnergy();}
  Int_t   GetZDCParticipants() const {return fESDZDC->GetZDCParticipants();}
  void    SetZDC(Float_t n1Energy, Float_t p1Energy, Float_t emEnergy,
                 Float_t n2Energy, Float_t p2Energy, Int_t participants)
  {fESDZDC->SetZDC(n1Energy, p1Energy, emEnergy, n2Energy, p2Energy, participants);}


  // FMD
  void SetFMDData(AliESDFMD * obj) { new(fESDFMD) AliESDFMD(*obj); /*CKB test vs 0*/ }
  AliESDFMD *GetFMDData(){ return fESDFMD; }


  // TZERO CKB: put this in the header?
  
  const AliESDTZERO*    GetESDTZERO() {return fESDTZERO;}
  // delegetated methods for fESDTZERO

  Float_t GetT0zVertex() const {return fESDTZERO->GetT0zVertex();}
  void SetT0zVertex(Float_t z) {fESDTZERO->SetT0zVertex(z);}
  Float_t GetT0() const {return fESDTZERO->GetT0();}
  void SetT0(Float_t timeStart) {fESDTZERO->SetT0(timeStart);}
  const Float_t * GetT0time() const {return fESDTZERO->GetT0time();}
  void SetT0time(Float_t time[24]) {fESDTZERO->SetT0time(time);}
  const Float_t * GetT0amplitude() const {return fESDTZERO->GetT0amplitude();}
  void SetT0amplitude(Float_t amp[24]){fESDTZERO->SetT0amplitude(amp);}

  // VZERO 
  AliESDVZERO *GetVZEROData(){ return fESDVZERO; }
  void SetVZEROData(AliESDVZERO * obj) { new(fESDVZERO) AliESDVZERO(*obj);  /*CKB test vs 0*/ }


  void SetESDfriend(const AliESDfriend *f);
  void GetESDfriend(AliESDfriend *f) const;



  void SetVertex(const AliESDVertex *vertex) {
    *fSPDVertex  = *vertex;
    fSPDVertex->SetName("fSPDVertex");// error prone use class wide names?
    //CKB or new with placement
  }
  const AliESDVertex *GetVertex() const {return fSPDVertex;}

  void SetPrimaryVertex(const AliESDVertex *vertex) {
    *fPrimaryVertex = *vertex;
    fPrimaryVertex->SetName("fPrimaryVertex");// error prone use class wide names?
    // ckb
    //     new (&fPrimaryVertex) AliESDVertex(*vertex);
  }
  const AliESDVertex *GetPrimaryVertex() const {return fPrimaryVertex;}

  void SetMultiplicity(const AliMultiplicity *mul) {
    *fSPDMult = *mul;
    // CKB 
    //     new (&fSPDMult) AliMultiplicity(*mul);
  }
  const AliMultiplicity *GetMultiplicity() const {return fSPDMult;}
  
  AliESDtrack *GetTrack(Int_t i) const {
    return (AliESDtrack *)fTracks->UncheckedAt(i);
  }
  Int_t  AddTrack(const AliESDtrack *t) {
    // CKB inline this or better in .cxx
    TClonesArray &ftr = *fTracks;
    AliESDtrack * track = new(ftr[fTracks->GetEntriesFast()])AliESDtrack(*t);
    track->SetID(fTracks->GetEntriesFast()-1);
    return  track->GetID();    
  }

  
  AliESDHLTtrack *GetHLTConfMapTrack(Int_t i) const {
    //    return (AliESDHLTtrack *)fHLTConfMapTracks->UncheckedAt(i);
    return 0;
  }
  void AddHLTConfMapTrack(const AliESDHLTtrack *t) {
    /*
    TClonesArray &fhlt = *fHLTConfMapTracks;
    new(fhlt[fHLTConfMapTracks->GetEntriesFast()]) AliESDHLTtrack(*t);
    */
    printf("ESD:: AddHLTConfMapTrack do nothing \n");
  }
  

  AliESDHLTtrack *GetHLTHoughTrack(Int_t i) const {
    //    return (AliESDHLTtrack *)fHLTHoughTracks->UncheckedAt(i);
    return 0;
  }
  void AddHLTHoughTrack(const AliESDHLTtrack *t) {
    printf("ESD:: AddHLTHoughTrack do nothing \n");
    /*
    TClonesArray &fhlt = *fHLTHoughTracks;
    new(fhlt[fHLTHoughTracks->GetEntriesFast()]) AliESDHLTtrack(*t);
    */
  }
  
  AliESDMuonTrack *GetMuonTrack(Int_t i) const {
    return (AliESDMuonTrack *)fMuonTracks->UncheckedAt(i);
  }
  void AddMuonTrack(const AliESDMuonTrack *t) {
    TClonesArray &fmu = *fMuonTracks;
    new(fmu[fMuonTracks->GetEntriesFast()]) AliESDMuonTrack(*t);
  }

  AliESDPmdTrack *GetPmdTrack(Int_t i) const {
    return (AliESDPmdTrack *)fPmdTracks->UncheckedAt(i);
  }
  void AddPmdTrack(const AliESDPmdTrack *t) {
    TClonesArray &fpmd = *fPmdTracks;
    new(fpmd[fPmdTracks->GetEntriesFast()]) AliESDPmdTrack(*t);
  }

  AliESDTrdTrack *GetTrdTrack(Int_t i) const {
    return (AliESDTrdTrack *)fTrdTracks->UncheckedAt(i);
  }
  void AddTrdTrack(const AliESDTrdTrack *t) {
    TClonesArray &ftrd = *fTrdTracks;
    new(ftrd[fTrdTracks->GetEntriesFast()]) AliESDTrdTrack(*t);
  }

  AliESDv0 *GetV0(Int_t i) const {
    return (AliESDv0*)fV0s->UncheckedAt(i);
  }
  Int_t AddV0(const AliESDv0 *v);

  AliESDcascade *GetCascade(Int_t i) const {
    return (AliESDcascade *)fCascades->UncheckedAt(i);
  }
  void AddCascade(const AliESDcascade *c) {
    TClonesArray &fc = *fCascades;
    new(fc[fCascades->GetEntriesFast()]) AliESDcascade(*c);
  }

  AliESDkink *GetKink(Int_t i) const {
    return (AliESDkink *)fKinks->UncheckedAt(i);
  }
  Int_t AddKink(const AliESDkink *c) {
    TClonesArray &fk = *fKinks;
    AliESDkink * kink = new(fk[fKinks->GetEntriesFast()]) AliESDkink(*c);
    kink->SetID(fKinks->GetEntriesFast()); // CKB different from the other imps..
    return fKinks->GetEntriesFast()-1;
  }

  AliESDCaloCluster *GetCaloCluster(Int_t i) const {
    return (AliESDCaloCluster *)fCaloClusters->UncheckedAt(i);
  }
  Int_t AddCaloCluster(const AliESDCaloCluster *c) {
    TClonesArray &fc = *fCaloClusters;
    AliESDCaloCluster *clus = new(fc[fCaloClusters->GetEntriesFast()]) AliESDCaloCluster(*c);
    clus->SetID(fCaloClusters->GetEntriesFast()-1);
    return fCaloClusters->GetEntriesFast()-1;
  }


  AliRawDataErrorLog *GetErrorLog(Int_t i) const {
    return (AliRawDataErrorLog *)fErrorLogs->UncheckedAt(i);
  }
  void  AddRawDataErrorLog(const AliRawDataErrorLog *log) {
    // CKB inline this??
    TClonesArray &errlogs = *fErrorLogs;
    new(errlogs[errlogs.GetEntriesFast()])  AliRawDataErrorLog(*log);
  }
  Int_t GetNumberOfErrorLogs()   const {return fErrorLogs->GetEntriesFast();}

    
  void AddPHOSTriggerPosition(TArrayF array)   { fPHOSTrigger->AddTriggerPosition(array); }
  void AddPHOSTriggerAmplitudes(TArrayF array) { fPHOSTrigger->AddTriggerAmplitudes(array);}
  void AddEMCALTriggerPosition(TArrayF array)  { fEMCALTrigger->AddTriggerPosition(array); }
  void AddEMCALTriggerAmplitudes(TArrayF array){ fEMCALTrigger->AddTriggerAmplitudes(array); }


  Int_t GetNumberOfTracks()     const {return fTracks->GetEntriesFast();}
  Int_t GetNumberOfHLTConfMapTracks()     const {return 0;} 
  // fHLTConfMapTracks->GetEntriesFast();}
  Int_t GetNumberOfHLTHoughTracks()     const {return  0;  }
  //  fHLTHoughTracks->GetEntriesFast();  }

  Int_t GetNumberOfMuonTracks() const {return fMuonTracks->GetEntriesFast();}
  Int_t GetNumberOfPmdTracks() const {return fPmdTracks->GetEntriesFast();}
  Int_t GetNumberOfTrdTracks() const {return fTrdTracks->GetEntriesFast();}
  Int_t GetNumberOfV0s()      const {return fV0s->GetEntriesFast();}
  Int_t GetNumberOfCascades() const {return fCascades->GetEntriesFast();}
  Int_t GetNumberOfKinks() const {return fKinks->GetEntriesFast();}
  Int_t GetNumberOfCaloClusters() const {return fCaloClusters->GetEntriesFast();}

  Int_t GetNumberOfEMCALClusters() const {return fEMCALClusters;}
  void  SetNumberOfEMCALClusters(Int_t clus) {fEMCALClusters = clus;}
  Int_t GetFirstEMCALCluster() const {return fFirstEMCALCluster;}
  void  SetFirstEMCALCluster(Int_t index) {fFirstEMCALCluster = index;}
  TArrayF *GetEMCALTriggerPosition() const {return  fEMCALTrigger->GetTriggerPosition();}
  TArrayF *GetEMCALTriggerAmplitudes() const {return  fEMCALTrigger->GetTriggerAmplitudes();}

  Int_t GetNumberOfPHOSClusters() const {return fPHOSClusters;}
  void  SetNumberOfPHOSClusters(Int_t part) { fPHOSClusters = part ; }
  void  SetFirstPHOSCluster(Int_t index) { fFirstPHOSCluster = index ; } 
  Int_t GetFirstPHOSCluster() const  { return fFirstPHOSCluster ; }
  TArrayF *GetPHOSTriggerPosition() const {return  fPHOSTrigger->GetTriggerPosition();}
  TArrayF *GetPHOSTriggerAmplitudes() const {return  fPHOSTrigger->GetTriggerAmplitudes();}

  void ResetV0s() { fV0s->Clear(); }
  void ResetCascades() { fCascades->Clear(); }
  void Reset();

  void  Print(Option_t *option="") const;

  void AddObject(TObject* obj);
  void ReadFromTree(TTree *tree);
  void WriteToTree(TTree* tree) {tree->Branch(fESDObjects);}
  void GetStdContent();
  void CreateStdContent();
  void SetStdNames();
  TList* GetList(){return fESDObjects;}

protected:
  AliESD(const AliESD&);
  AliESD &operator=(const AliESD& source);


  TList *fESDObjects;             // List of esd Objects

  AliESDRun       *fESDRun;           //! Run information tmp put in the Userdata
  AliESDHeader    *fHeader;           //! ESD Event Header
  AliESDZDC       *fESDZDC;           //! ZDC information
  AliESDFMD       *fESDFMD;           //! FMD object containing rough multiplicity
  AliESDVZERO     *fESDVZERO;         //! VZERO object containing rough multiplicity
  AliESDTZERO     *fESDTZERO;         //! TZEROObject
  AliESDVertex    *fSPDVertex;        //! Primary vertex estimated by the SPD
  AliESDVertex    *fPrimaryVertex;    //! Primary vertex estimated using ESD tracks
  AliMultiplicity *fSPDMult;          //! SPD tracklet multiplicity
  AliESDCaloTrigger* fPHOSTrigger;     //! PHOS Trigger information
  AliESDCaloTrigger* fEMCALTrigger;    //! PHOS Trigger information

  TClonesArray *fTracks;           //! ESD tracks 
  //  TClonesArray *fHLTConfMapTracks; //! HLT ESD tracks from Conformal Mapper method
  //  TClonesArray *fHLTHoughTracks;   //! HLT ESD tracks from Hough Transform method
  TClonesArray *fMuonTracks;       //! MUON ESD tracks
  TClonesArray *fPmdTracks;        //! PMD ESD tracks
  TClonesArray *fTrdTracks;        //! TRD ESD tracks (triggered)
  TClonesArray *fV0s;              //! V0 vertices
  TClonesArray *fCascades;         //! Cascade vertices
  TClonesArray *fKinks;            //! Kinks
  TClonesArray *fCaloClusters;     //! Calorimeter clusters for PHOS/EMCAL
  TClonesArray *fErrorLogs;        //! Raw-data reading error messages


  // Remove this stuff CKB
  Int_t        fEMCALClusters;   // Number of EMCAL clusters (subset of caloclusters)
  Int_t        fFirstEMCALCluster; // First EMCAL cluster in the fCaloClusters list 

  Int_t        fPHOSClusters;     // Number of PHOS clusters (subset of caloclusters)
  Int_t        fFirstPHOSCluster; // First PHOS cluster in the fCaloClusters list 

  ClassDef(AliESD,23)  //ESD class 
};
#endif 

