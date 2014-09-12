#ifndef ALIFLATESDEVENT_H
#define ALIFLATESDEVENT_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 * Primary Authors : Sergey Gorbunov, Jochen Thaeder, Chiara Zampolli     */

/*
 * See implementation file for documentation
 */

#include "Rtypes.h"
#include "AliVMisc.h"
#include "AliVEvent.h"
#include "AliFlatESDTrack.h"
#include "AliFlatESDVertex.h"
#include "AliFlatESDFriend.h"
#include "AliESDVertex.h"

class AliFlatESDV0;
class AliFlatESDTrigger;

class AliESDEvent;
class AliESDVertex;

class AliFlatESDEvent :public AliVEvent {
 public:
  // --------------------------------------------------------------------------------
  // -- Constructor / Destructors
  AliFlatESDEvent();
  ~AliFlatESDEvent() {}

  // constructor and method for reinitialisation of virtual table
  AliFlatESDEvent( AliVConstructorReinitialisationFlag );
  void Reinitialize() { new (this) AliFlatESDEvent( AliVReinitialize ); }

  // --------------------------------------------------------------------------------

  Double_t  GetMagneticField() const { return fMagneticField; }
  UInt_t    GetPeriodNumber()  const { return fPeriodNumber; }
  Int_t     GetRunNumber()     const { return fRunNumber; }
  UInt_t    GetOrbitNumber()   const { return fOrbitNumber; }
  UShort_t  GetBunchCrossNumber() const { return fBunchCrossNumber; }
  ULong64_t GetTriggerMask()      const { return fTriggerMask; }  
  ULong64_t GetTriggerMaskNext50()   const { return fTriggerMaskNext50; }  
  TString   GetFiredTriggerClasses() const ;
 
  Int_t GetNumberOfTracks() const { return fNTracks; }
  Int_t GetNumberOfV0s()    const { return fNV0s; }

  void Reset();
  void ConnectTracks(){} // initialisation, needed by the ESD event, part of VVevent interface

  UInt_t GetTimeStamp()     const { return fTimeStamp; }
  UInt_t GetEventSpecie()   const { return fEventSpecie; }
  Int_t  GetNumberOfKinks() const { return 0; }  

  AliESDkink* GetKink(Int_t /*i*/) const { return NULL;}

  Int_t GetPrimaryVertex( AliESDVertex &v ) const ;
  Int_t GetPrimaryVertexTPC( AliESDVertex &v ) const ;
  Int_t GetPrimaryVertexSPD( AliESDVertex &v ) const ;
  Int_t GetPrimaryVertexTracks( AliESDVertex &v ) const ;

  AliVfriendEvent* FindFriend() const { return fFriendEvent; }

  // --------------------------------------------------------------------------------
  // Own methods 

  // -- Set methods

  Int_t SetFromESD( size_t allocatedMemorySize, const AliESDEvent *esd, const Bool_t fillV0s=kTRUE );

  void SetFriendEvent( AliFlatESDFriend *f ) { fFriendEvent=f; }

  void  SetMagneticField( Double_t mf ){ fMagneticField = mf; }
  void  SetPeriodNumber( Int_t n ) { fPeriodNumber = n; }
  void  SetRunNumber( Int_t n ) { fRunNumber = n; }
  void  SetOrbitNumber( UInt_t n ) { fOrbitNumber = n; }
  void  SetBunchCrossNumber( UShort_t n ) { fBunchCrossNumber = n; }
  void  SetTimeStamp( UInt_t timeStamp ){ fTimeStamp = timeStamp; }
  void  SetEventSpecie(UInt_t eventSpecie){ fEventSpecie = eventSpecie; }
  void  SetTriggerMask(ULong64_t n) { fTriggerMask = n; }
  void  SetTriggerMaskNext50(ULong64_t n) { fTriggerMaskNext50 = n; }
  
  Int_t  SetPrimaryVertexTracks( const AliESDVertex *v, size_t allocatedVtxMemory );
  Int_t  SetPrimaryVertexSPD( const AliESDVertex *v, size_t allocatedVtxMemory  );

  AliFlatESDTrigger *SetTriggersStart();
  void SetTriggersEnd( Int_t nTriggerClasses, size_t triggersSize );

  Int_t SetTracksStart( AliFlatESDTrack* &t, Long64_t* &table, Int_t nTracks, size_t freeMem);
  void SetTracksEnd( Int_t nTracks, size_t tracksSize );

  AliFlatESDV0 *SetV0sStart();
  void SetV0sEnd( Int_t nV0s, size_t v0sSize );

  // --------------------------------------------------------------------------------
  // -- Getter methods

  const AliFlatESDVertex* GetFlatPrimaryVertexSPD() const ;
  const AliFlatESDVertex* GetFlatPrimaryVertexTracks() const ;

  Int_t GetNumberOfTriggerClasses() const { return fNTriggerClasses; }
   
  const AliFlatESDTrigger *GetTriggerClasses() const { return reinterpret_cast<const AliFlatESDTrigger*>( fContent + fTriggerPointer ); }
  const AliFlatESDTrack   *GetTracks() const { return reinterpret_cast<const AliFlatESDTrack*>( fContent + fTracksPointer ); }
  const AliFlatESDV0      *GetV0s() const { return reinterpret_cast<const AliFlatESDV0*>( fContent + fV0Pointer ); }

  const AliFlatESDTrack  *GetFlatTrack( Int_t i ) const ;

  // --------------------------------------------------------------------------------
  // -- Size methods

  ULong64_t  GetSize()  const { return fContent + fContentSize - reinterpret_cast<const Byte_t*>(this); }

  static ULong64_t EstimateSize(AliESDEvent*esd, Bool_t fillV0s=kTRUE );

  // --------------------------------------------------------------------------------
  // AliVEvent interface
  virtual void Print(Option_t *option="") const {if (option){}; return;}
  virtual void AddObject(TObject* /*obj*/) {}
  virtual TObject* FindListObject(const char* /*name*/) const {return NULL;}
  virtual TList* GetList() const {return NULL;}
  virtual void CreateStdContent() {}
  virtual void GetStdContent() {}
  virtual void ReadFromTree(TTree* /*tree*/, Option_t* /*opt*/) {}
  virtual void WriteToTree(TTree* /*tree*/) const {}
  virtual void SetStdNames() {}
  virtual AliVHeader* GetHeader() const {return NULL;}
  virtual void SetPeriodNumber(UInt_t) {}
  virtual void SetEventType(UInt_t) {}
  virtual void SetTriggerCluster(UChar_t) {}
  virtual UInt_t GetEventType() const {return 0;}
  virtual UChar_t GetTriggerCluster() const {return 0;}
  virtual Double_t GetZDCN1Energy() const {return 0.;}
  virtual Double_t GetZDCP1Energy() const {return 0.;}
  virtual Double_t GetZDCN2Energy() const {return 0.;}
  virtual Double_t GetZDCP2Energy() const {return 0.;}
  virtual Double_t GetZDCEMEnergy(Int_t) const {return 0.;}
  virtual AliVParticle* GetTrack(Int_t) const;
  virtual Int_t GetNumberOfCascades() const {return 0;}
  virtual AliCentrality* GetCentrality() {return NULL;}
  virtual AliEventplane* GetEventplane() {return NULL;}
  virtual Int_t EventIndex(Int_t) const {return 0;}
  virtual Int_t EventIndexForCaloCluster(Int_t) const {return 0;}
  virtual Int_t EventIndexForPHOSCell(Int_t) const {return 0;}
  virtual Int_t EventIndexForEMCALCell(Int_t) const {return 0;}
  virtual AliVVZERO* GetVZEROData() const {return NULL;}
  virtual AliVZDC   *GetZDCData() const {return NULL;}

 private:

  AliFlatESDEvent( const AliFlatESDEvent& );
  AliFlatESDEvent& operator=( const AliFlatESDEvent& );

  // --------------------------------------------------------------------------------

  // -- Fixed size member objects ( try to align in memory )

  size_t     fContentSize;     // Size of fContent
  Double32_t fMagneticField;   // Solenoid Magnetic Field in kG : for compatibility with AliMagF
  UInt_t     fPeriodNumber;    // PeriodNumber
  Int_t      fRunNumber;       // Run Number
  UInt_t     fOrbitNumber;     // Orbit Number
  UInt_t     fTimeStamp;         // Time stamp
  UInt_t     fEventSpecie;       // Reconstruction event specie (1-default,2-lowM,4-highM,8-cosmic,16-cal)
  UShort_t   fBunchCrossNumber;  // Bunch Crossing Number
  Byte_t     fPrimaryVertexMask;            // Bit mask specfifying which primary vertices are present
  ULong64_t  fTriggerMask;      // Trigger mask, first 50 bits
  ULong64_t  fTriggerMaskNext50;      // Trigger mask, next 50 bits

  UInt_t  fNTriggerClasses;  // N trigger classes
  UInt_t  fNPrimaryVertices; // Number of primary vertices in array
  UInt_t  fNTracks;          // Number of tracks in array
  UInt_t  fNV0s;             // Number of v0's
  
  // Pointers to specific data in fContent
  
  size_t fTriggerPointer;        // position of the first trigger description in fContent
  size_t fPrimaryVertexTracksPointer; // position of primary vertex tracks in fContent
  size_t fPrimaryVertexSPDPointer;  // position of primary vertex SPD in fContent
  size_t fTrackTablePointer;     // position of the first track pointer in fContent
  size_t fTracksPointer;         // position of the first track in fContent
  size_t fV0Pointer;             // position of the first V0 in fContent  
  
  //
  AliFlatESDFriend *fFriendEvent;

  // --------------------------------------------------------------------------------

  // -- Variable Size Object

  Byte_t fContent[1];                  // Variale size object, which contains all data
};

// Inline implementations 

inline const AliFlatESDTrack  *AliFlatESDEvent::GetFlatTrack( Int_t i ) const 
{ 
  const Long64_t *table = reinterpret_cast<const Long64_t*> (fContent + fTrackTablePointer);
  if( i<0 || i>(int) fNTracks || table[i]<0 ) return NULL;
  return reinterpret_cast<const AliFlatESDTrack*>( fContent + table[i] );
}


inline Int_t AliFlatESDEvent::SetTracksStart( AliFlatESDTrack* &t, Long64_t* &table, Int_t nTracks, size_t freeMem)
{
  fNTracks = 0;
  if( nTracks*sizeof(Long64_t)  > freeMem ) return -1;
  fTrackTablePointer = fContentSize;
  fContentSize += nTracks*sizeof(Long64_t);
  fTracksPointer = fContentSize;
  table = reinterpret_cast< Long64_t* >( fContent + fTrackTablePointer );
  t = reinterpret_cast< AliFlatESDTrack* >( fContent + fTracksPointer );
  return 0;
}

inline void AliFlatESDEvent::SetTracksEnd( Int_t nTracks, size_t tracksSize )
{
  if( nTracks<0 ) return;
  Long64_t *table = reinterpret_cast< Long64_t*> (fContent + fTrackTablePointer);
  for( int i=0; i<nTracks; i++ ) table[i]+=fTracksPointer;
  fNTracks = nTracks;
  fContentSize += tracksSize;
}

inline AliFlatESDV0 *AliFlatESDEvent::SetV0sStart()
{
  fNV0s = 0;
  fV0Pointer = fContentSize;
  return reinterpret_cast< AliFlatESDV0* >( fContent + fContentSize );
}
   
inline void AliFlatESDEvent::SetV0sEnd( Int_t nV0s, size_t v0sSize )
{
  fNV0s = nV0s;
  fContentSize += v0sSize;
}
  
inline AliFlatESDTrigger *AliFlatESDEvent::SetTriggersStart()
{
  fNTriggerClasses = 0;
  fTriggerPointer = fContentSize;
  return reinterpret_cast< AliFlatESDTrigger* >( fContent + fContentSize );
}

inline void AliFlatESDEvent::SetTriggersEnd( Int_t nTriggerClasses, size_t triggersSize )
{
  fNTriggerClasses = nTriggerClasses;
  fContentSize += triggersSize;
}

inline const AliFlatESDVertex* AliFlatESDEvent::GetFlatPrimaryVertexSPD() const 
{
  return (fPrimaryVertexMask & 0x1) ? reinterpret_cast<const AliFlatESDVertex*>(fContent + fPrimaryVertexSPDPointer) : NULL;
} 

inline const AliFlatESDVertex* AliFlatESDEvent::GetFlatPrimaryVertexTracks() const 
{ 
  return (fPrimaryVertexMask & 0x2) ? (reinterpret_cast<const AliFlatESDVertex*>(fContent + fPrimaryVertexTracksPointer)  ) : NULL;
} 

inline Int_t AliFlatESDEvent::GetPrimaryVertexSPD( AliESDVertex &v ) const 
{
  const AliFlatESDVertex* flatVertex = GetFlatPrimaryVertexSPD();
  if( !flatVertex ) return -1;
  flatVertex->GetESDVertex( v );
  return 0;
}

inline Int_t AliFlatESDEvent::GetPrimaryVertexTracks( AliESDVertex &v ) const 
{
  const AliFlatESDVertex* flatVertex = GetFlatPrimaryVertexTracks();
  if( !flatVertex ) return -1;
  flatVertex->GetESDVertex( v );
  return 0;
}

inline Int_t AliFlatESDEvent::GetPrimaryVertexTPC( AliESDVertex &/*v*/ ) const 
{
  return -1;
}

inline Int_t AliFlatESDEvent::GetPrimaryVertex( AliESDVertex &v ) const 
{
  int ret = GetPrimaryVertexTracks( v );
  if( ret<0 ) ret = GetPrimaryVertexSPD( v );
  return ret;
}


#endif
