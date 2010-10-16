#ifndef ALIANALYSISTASKEMCALPI0CALIBSELECTION_H
#define ALIANALYSISTASKEMCALPI0CALIBSELECTION_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

//---------------------------------------------------------------------------// 
// Fill histograms with two-cluster invariant mass                           //
// using calibration coefficients of the previous iteration.                 //
//---------------------------------------------------------------------------//

// Root includes
class TH1F;
#include "TH2I.h"
#include "TObjArray.h"

// AliRoot includes
#include "AliAnalysisTaskSE.h"
class AliEMCALGeometry;
class AliAODCaloCluster;
class AliAODCaloCells;
//class AliEMCALCalibData ;
#include "AliEMCALGeoParams.h"
class AliEMCALRecoUtils;

class AliAnalysisTaskEMCALPi0CalibSelection : public AliAnalysisTaskSE
{
public:

  AliAnalysisTaskEMCALPi0CalibSelection(const char* name);
  virtual ~AliAnalysisTaskEMCALPi0CalibSelection();

private:
  
  AliAnalysisTaskEMCALPi0CalibSelection(const AliAnalysisTaskEMCALPi0CalibSelection&); 
  AliAnalysisTaskEMCALPi0CalibSelection& operator=(const AliAnalysisTaskEMCALPi0CalibSelection&); 
  
public:
  
  // Implementation of interface methods
  virtual void UserCreateOutputObjects();
  virtual void UserExec(Option_t * opt);
  virtual void LocalInit() ;
  
  void SetAsymmetryCut(Float_t asy)      {fAsyCut      = asy ;}
  void SetClusterMinEnergy(Float_t emin) {fEmin        = emin;}
  void SetClusterMaxEnergy(Float_t emax) {fEmax        = emax;}
  void SetClusterMinNCells(Int_t n)      {fMinNCells   = n   ;}
  void SetNCellsGroup(Int_t n)           {fGroupNCells = n   ;}
  void SetLogWeight(Float_t w)           {fLogWeight   = w   ;}
  
  //void SetCalibCorrections(AliEMCALCalibData* const cdata);
  void CreateAODFromESD();
  void CreateAODFromAOD();	

  void CopyAOD(Bool_t copy)  { fCopyAOD = copy ; }
  Bool_t IsAODCopied() const { return fCopyAOD ; }
	
  void SwitchOnSameSM()    {fSameSM = kTRUE  ; }
  void SwitchOffSameSM()   {fSameSM = kFALSE ; }
  
  Int_t  GetEMCALClusters(AliVEvent* event, TRefArray *clusters) const;
  Bool_t IsEMCALCluster(AliVCluster *clus) const;
  void SwitchOnOldAODs()   {fOldAOD = kTRUE  ; }
  void SwitchOffOldAODs()  {fOldAOD = kFALSE ; }  
  
  void SetGeometryName(TString name) { fEMCALGeoName = name ; }
  TString GeometryName() const       { return fEMCALGeoName ; }
 
  //Modules fiducial region
  Bool_t CheckCellFiducialRegion(AliVCluster* cluster, AliVCaloCells* cells) ;
  void   SetNumberOfCellsFromEMCALBorder(Int_t n) {fNCellsFromEMCALBorder = n; }
  Int_t  GetNumberOfCellsFromEMCALBorder() const  {return fNCellsFromEMCALBorder; }
  
  // Bad channels, copy from PWG4/PartCorrBase/AliCalorimeterUtils
  Bool_t IsBadChannelsRemovalSwitchedOn()  const { return fRemoveBadChannels ; }
  void SwitchOnBadChannelsRemoval ()  {fRemoveBadChannels = kTRUE  ; InitEMCALBadChannelStatusMap();}
  void SwitchOffBadChannelsRemoval()  {fRemoveBadChannels = kFALSE ; }
	
  void InitEMCALBadChannelStatusMap() ;
	
  Int_t GetEMCALChannelStatus(Int_t iSM , Int_t iCol, Int_t iRow) const { 
	if(fEMCALBadChannelMap) return (Int_t) ((TH2I*)fEMCALBadChannelMap->At(iSM))->GetBinContent(iCol,iRow); 
	else return 0;}//Channel is ok by default
	
  void SetEMCALChannelStatus(Int_t iSM , Int_t iCol, Int_t iRow, Double_t c = 1) { 
	if(!fEMCALBadChannelMap)InitEMCALBadChannelStatusMap() ;
	((TH2I*)fEMCALBadChannelMap->At(iSM))->SetBinContent(iCol,iRow,c);}
	
  TH2I * GetEMCALChannelStatusMap(Int_t iSM) const {return (TH2I*)fEMCALBadChannelMap->At(iSM);}
  void   SetEMCALChannelStatusMap(TObjArray *map)  {fEMCALBadChannelMap = map;}
	
  Bool_t ClusterContainsBadChannel(UShort_t* cellList, Int_t nCells);
	
  // Recalibration
  Bool_t IsRecalibrationOn() const { return fRecalibration  ; }
  void SwitchOnRecalibration()     {fRecalibration = kTRUE  ; InitEMCALRecalibrationFactors();}
  void SwitchOffRecalibration()    {fRecalibration = kFALSE ; }
	
  void InitEMCALRecalibrationFactors() ;
  
  Float_t GetEMCALChannelRecalibrationFactor(Int_t iSM , Int_t iCol, Int_t iRow) const { 
	if(fEMCALRecalibrationFactors) return (Float_t) ((TH2F*)fEMCALRecalibrationFactors->At(iSM))->GetBinContent(iCol,iRow); 
	else return 1;}
	
  void SetEMCALChannelRecalibrationFactor(Int_t iSM , Int_t iCol, Int_t iRow, Double_t c = 1) { 
	if(!fEMCALRecalibrationFactors) InitEMCALRecalibrationFactors();
	((TH2F*)fEMCALRecalibrationFactors->At(iSM))->SetBinContent(iCol,iRow,c);}
	
  void SetEMCALChannelRecalibrationFactors(Int_t iSM , TH2F* h) {fEMCALRecalibrationFactors->AddAt(h,iSM);}
	
  TH2F * GetEMCALChannelRecalibrationFactors(Int_t iSM) const {return (TH2F*)fEMCALRecalibrationFactors->At(iSM);}
	
  void SetEMCALChannelRecalibrationFactors(TObjArray *map) {fEMCALRecalibrationFactors = map;}
  Float_t RecalibrateClusterEnergy(AliAODCaloCluster* cluster, AliAODCaloCells * cells);
	
  void SetEMCALRecoUtils(AliEMCALRecoUtils * ru) {fRecoUtils = ru;}
  AliEMCALRecoUtils* GetEMCALRecoUtils() const   {return fRecoUtils;}
  
  void SetInvariantMassHistoBinRange(Int_t nBins, Float_t minbin, Float_t maxbin){
	fNbins = nBins; fMinBin = minbin; fMaxBin = maxbin; }
	  
  void GetMaxEnergyCellPosAndClusterPos(AliVCaloCells* cells, AliVCluster* clu, Int_t& iSM, Int_t& ieta, Int_t& iphi);

private:

  AliEMCALGeometry * fEMCALGeo;  //! EMCAL geometry
  //AliEMCALCalibData* fCalibData; // corrections to CC from the previous iteration
	
  Float_t fEmin;          // min. cluster energy
  Float_t fEmax;          // max. cluster energy
  Float_t fAsyCut;        // Asymmetry cut
  Int_t   fMinNCells;     // min. ncells in cluster
  Int_t   fGroupNCells;   // group n cells
  Float_t fLogWeight;     // log weight used in cluster recalibration
  Bool_t  fCopyAOD;       // Copy calo information only to AOD?
  Bool_t  fSameSM;        // Combine clusters in channels on same SM
  Bool_t  fOldAOD;        // Reading Old AODs, created before release 4.20
  TString fEMCALGeoName;  // Name of geometry to use.
  Int_t   fNCellsFromEMCALBorder; //  Number of cells from EMCAL border the cell with maximum amplitude has to be.

  Bool_t     fRemoveBadChannels;         // Check the channel status provided and remove clusters with bad channels
  TObjArray *fEMCALBadChannelMap;        // Array of histograms with map of bad channels, EMCAL
  Bool_t     fRecalibration;             // Switch on or off the recalibration
  TObjArray *fEMCALRecalibrationFactors; // Array of histograms with map of recalibration factors, EMCAL                 
 
  AliEMCALRecoUtils * fRecoUtils;  // Access to reconstruction utilities
  
  //Output histograms	
  Int_t   fNbins;  // N       mass bins of invariant mass histograms
  Float_t fMinBin; // Minimum mass bins of invariant mass histograms
  Float_t fMaxBin; // Maximum mass bins of invariant mass histograms

  TList*  fOutputContainer; //!histogram container
  TH1F*   fHmpi0[AliEMCALGeoParams::fgkEMCALModules][AliEMCALGeoParams::fgkEMCALCols][AliEMCALGeoParams::fgkEMCALRows];//! two-cluster inv. mass assigned to each cell.

  TH2F*   fHmgg;            //! two-cluster inv.mass vs pt of pair
  TH2F*   fHmggDifferentSM; //! two-cluster inv.mass vs pt of pair, each cluster in different SM
  TH2F*   fHmggSM[4];       //! two-cluster inv.mass per SM
  TH2F*   fHmggPairSM[4];   //! two-cluster inv.mass per Pair
  
  TH2F*   fHOpeningAngle;            //! two-cluster opening angle vs pt of pair, with mass close to pi0
  TH2F*   fHOpeningAngleDifferentSM; //! two-cluster opening angle vs pt of pair, each cluster in different SM, with mass close to pi0
  TH2F*   fHOpeningAngleSM[4];       //! two-cluster opening angle vs pt per SM,with mass close to pi0
  TH2F*   fHOpeningAnglePairSM[4];   //! two-cluster opening angle vs pt per Pair,with mass close to pi0

  TH2F*   fHIncidentAngle;            //! cluster incident angle vs pt of pair, with mass close to pi0
  TH2F*   fHIncidentAngleDifferentSM; //! cluster incident angle vs pt of pair, each cluster in different SM, with mass close to pi0
  TH2F*   fHIncidentAngleSM[4];       //! cluster incident angle vs pt per SM,with mass close to pi0
  TH2F*   fHIncidentAnglePairSM[4];   //! cluster incident angle vs pt per Pair,with mass close to pi0
  
  TH2F*   fHAsymmetry;            //! two-cluster asymmetry vs pt of pair, with mass close to pi0
  TH2F*   fHAsymmetryDifferentSM; //! two-cluster asymmetry vs pt of pair, each cluster in different SM, with mass close to pi0
  TH2F*   fHAsymmetrySM[4];       //! two-cluster asymmetry vs pt per SM,with mass close to pi0
  TH2F*   fHAsymmetryPairSM[4];   //! two-cluster asymmetry vs pt per Pair,with mass close to pi0
  
  TH2F*   fhTowerDecayPhotonHit[4] ;       //! Cells ordered in column/row for different module, number of times a decay photon hits
  TH2F*   fhTowerDecayPhotonEnergy[4] ;    //! Cells ordered in column/row for different module, accumulated energy in the tower by decay photons.
  TH2F*   fhTowerDecayPhotonAsymmetry[4] ; //! Cells ordered in column/row for different module, accumulated asymmetry in the tower by decay photons.

  TH1I*   fhNEvents;        //! Number of events counter histogram
  TList * fCuts ;           //! List with analysis cuts

  ClassDef(AliAnalysisTaskEMCALPi0CalibSelection,8);

};

#endif //ALIANALYSISTASKEMCALPI0CALIBSELECTION_H
