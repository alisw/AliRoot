#ifndef ALIITSCLUSTERFINDERV2SDD_H
#define ALIITSCLUSTERFINDERV2SDD_H
//--------------------------------------------------------------
//                       ITS clusterer V2 for SDD
//
//   This can be a "wrapping" for the V1 cluster finding classes
//   if compiled with uncommented "#define V1" line 
//   in the AliITSclustererV2.cxx file.
//
//   Origin: Iouri Belikov, CERN, Jouri.Belikov@cern.ch 
//--------------------------------------------------------------
#include "AliITSClusterFinderV2.h"
#include "AliITSDetTypeRec.h"

class TClonesArray;
class AliRawReader;
class AliITSRawStream;
class AliITSCalibrationSDD;
class AliITSsegmentationSDD;

class AliITSClusterFinderV2SDD : public AliITSClusterFinderV2 {
public:
  AliITSClusterFinderV2SDD(AliITSDetTypeRec* dettyp);
  virtual ~AliITSClusterFinderV2SDD(){;}
  virtual void FindRawClusters(Int_t mod);
  virtual void RawdataToClusters(AliRawReader* rawReader,TClonesArray** clusters);
  Float_t GetTimeOffset()const {return fTimeOffsetSDD;}
  void SetTimeOffset(Float_t to = 55.07){fTimeOffsetSDD = to;}
 protected:

  void FindClustersSDD(TClonesArray *digits);
  void FindClustersSDD(AliBin* bins[2], Int_t nMaxBin, Int_t nMaxZ,
		       TClonesArray *dig, TClonesArray *clusters=0x0);

  void FindClustersSDD(AliITSRawStream* input,TClonesArray** clusters);
  void CorrectPosition(Float_t &z, Float_t&y);
  virtual AliITSCalibrationSDD* GetResp(Int_t mod)const{
    return (AliITSCalibrationSDD*) fDetTypeRec->GetCalibrationModel(mod);}
  virtual AliITSsegmentationSDD* GetSeg()const{
    return (AliITSsegmentationSDD*)fDetTypeRec->GetSegmentationModel(1);} 


  Int_t fNySDD;           //number of "pixels" in Y
  Int_t fNzSDD;           //number of "pixels" in Z
  Float_t fZpitchSDD;     //"pixel sizes" in Z
  Float_t fHwSDD;         //half width of the SDD detector
  Float_t fHlSDD;         //half length of the SDD detector
  Float_t fTimeOffsetSDD;    //delay in the drift channel   

  ClassDef(AliITSClusterFinderV2SDD,2)  // ITS cluster finder V2 for SDD
};

#endif
