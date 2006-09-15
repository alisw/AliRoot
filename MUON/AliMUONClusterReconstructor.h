#ifndef ALIMUONCLUSTERRECONSTRUCTOR_H
#define ALIMUONCLUSTERRECONSTRUCTOR_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/*$Id$*/
// Revision of includes 07/05/2004

/// \ingroup rec
/// \class AliMUONClusterReconstructor
/// \brief MUON cluster reconstructor in ALICE

#include <TObject.h>

class AliMUONClusterFinderVS;
class AliMUONData;
class TClonesArray;

class AliMUONClusterReconstructor : public TObject 
{
 public:
  AliMUONClusterReconstructor(AliMUONData* data = 0x0); // Constructor
  virtual ~AliMUONClusterReconstructor(void); // Destructor

 
  // Cluster Finding & Trigger
  virtual void   Digits2Clusters(Int_t chBeg = 0);
  virtual void   Trigger2Trigger() ;

//  // pointer to data container
//  AliMUONData*   GetMUONData() {return fMUONData;}
  // Reco Model
  AliMUONClusterFinderVS* GetRecoModel() {return fRecModel;}

  void SetRecoModel(AliMUONClusterFinderVS* rec);

 protected:
  void ClusterizeOneDE(Int_t detElemId);
  
 private:
  AliMUONClusterReconstructor (const AliMUONClusterReconstructor& rhs); // copy constructor
  AliMUONClusterReconstructor& operator=(const AliMUONClusterReconstructor& rhs); // assignment operator

  AliMUONData*            fMUONData;           //!< Data container for MUON subsystem 
  AliMUONClusterFinderVS* fRecModel;           //!< cluster recontruction model

  TClonesArray* fDigitsCath0; //!< digits for cathode 0 of the current DE
  TClonesArray* fDigitsCath1; //!< digits for cathode 1 of the current DE
  
  ClassDef(AliMUONClusterReconstructor,0) // MUON cluster reconstructor in ALICE
};
	
#endif
