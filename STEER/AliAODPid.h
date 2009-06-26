#ifndef AliAODPid_H
#define AliAODPid_H
/* Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

//-------------------------------------------------------------------------
//     AOD Pid object for additional pid information
//     Author: Annalisa Mastroserio, CERN
//-------------------------------------------------------------------------

#include <TObject.h>

class AliAODPid : public TObject {

 public:
  AliAODPid();
  virtual ~AliAODPid();
  AliAODPid(const AliAODPid& pid); 
  AliAODPid& operator=(const AliAODPid& pid);
  
  enum{kSPECIES=5, kTRDnPlanes=6};

 //setters
  void      SetITSsignal(Double_t its)                         {fITSsignal=its;}
  void      SetTPCsignal(Double_t tpc)                         {fTPCsignal=tpc;}
  void      SetTRDsignal(Int_t nslices, Double_t* trdslices)   {fTRDnSlices = nslices; fTRDslices=trdslices;}
  void      SetTOFsignal(Double_t tof)                         {fTOFesdsignal=tof;}
  void      SetIntegratedTimes(Double_t timeint[5]);
  void      SetHMPIDsignal(Double_t hmpid)                     {fHMPIDsignal=hmpid;}
  void      SetEMCALPosition(Double_t emcalpos[3]);
  void      SetEMCALMomentum(Double_t emcalmom[3]);

  Double_t  GetITSsignal()       {return  fITSsignal;}
  Double_t  GetTPCsignal()       {return  fTPCsignal;}
  Int_t     GetTRDnSlices()      {return  fTRDnSlices;}
  Double_t* GetTRDsignal()       {return  fTRDslices;}
  Double_t  GetTOFsignal()       {return  fTOFesdsignal;} 
  void      GetIntegratedTimes(Double_t timeint[5]); 
  Double_t  GetHMPIDsignal()     {return  fHMPIDsignal;}
  void      GetEMCALPosition(Double_t emcalpos[3]);
  void      GetEMCALMomentum(Double_t emcalmom[3]);

 private :
  Double32_t fITSsignal;        //[0.,0.,10] detector raw signal
  Double32_t fTPCsignal;        //[0.,0.,10] detector raw signal
  Int_t      fTRDnSlices;       //N slices used for PID in the TRD
  Double32_t* fTRDslices;       //[fTRDnSlices]
  Double32_t fTOFesdsignal;     //TOF signal - t0 (T0 interaction time)
  Double32_t fIntTime[5];       //track time hypothesis
  Double32_t fHMPIDsignal;      //detector raw signal
  Double32_t fEMCALPosition[3]; //global position of track
				//extrapolated to EMCAL surface
  Double32_t fEMCALMomentum[3]; //momentum of track
				//extrapolated to EMCAL surface

  ClassDef(AliAODPid,2);
};

#endif
