#ifndef ALIITSCALIBRATIONSDD_H
#define ALIITSCALIBRATIONSDD_H
 
/* Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */
 
/* $Id$ */

#include "AliITSCalibration.h"
#include "AliITSresponseSDD.h"
#include "TArrayI.h"

class AliITSMapSDD;
class AliITSDriftSpeedArraySDD;
class AliITSresponseSDD;
///////////////////////////////////////////////////////
//  Response for SDD                                 //
///////////////////////////////////////////////////////

class AliITSCalibrationSDD : public AliITSCalibration {
  public:
    //
    // Configuration methods
    //
    AliITSCalibrationSDD();
    AliITSCalibrationSDD(const char *dataType);
    virtual ~AliITSCalibrationSDD();
    virtual void  SetNoiseParam(Double_t /*n*/, Double_t /*b*/){
      NotImplemented("SetNoiseParam");}
 
    virtual void  GetNoiseParam(Double_t &/*n*/, Double_t &/*b*/) const {
      NotImplemented("GetNoiseParam");}

    virtual Float_t GetBaseline(Int_t anode) const {return fBaseline[anode];}
    virtual void SetBaseline(Int_t anode,Double_t bas) {fBaseline[anode]=bas;}
    virtual Float_t GetNoise(Int_t anode) const {return fNoise[anode];}
    virtual void SetNoise(Int_t anode, Double_t noise) {fNoise[anode]=noise;}

    virtual void   SetThresholds(Double_t  mv, Double_t /* b */){
       // Min value used in 2D - could be used as a threshold setting
	fMinVal = mv;}
    virtual void   Thresholds(Double_t &  mv, Double_t & /* b */) const 
      {mv = fMinVal;}
    virtual void  GiveCompressParam(Int_t *x,Int_t ian) const;

    void  SetNoiseAfterElectronics(Int_t anode,Double_t n=2.38){
	// Noise after electronics (ADC units)
	// 2.36 for ALICE from beam test measurements 2001
	fNoiseAfterEl[anode]=n;}
    Float_t  GetNoiseAfterElectronics(Int_t anode) const {
	// Noise after electronics (ADC units)
	return fNoiseAfterEl[anode];} 
    //void SetDeadChannels(Int_t nchips=0, Int_t nchannels=0);
    void SetDeadChannels(Int_t ndead=0){fDeadChannels=ndead; fBadChannels.Set(ndead);}
    Int_t GetDeadChips() const { return fDeadChips; }
    Int_t GetDeadChannels() const { return fDeadChannels; }
    Float_t Gain(Int_t wing,Int_t chip,Int_t ch)const 
        {return fGain[wing][chip][ch]; }
    Float_t GetChannelGain(Int_t anode) const;
    virtual void SetGain(Double_t g,Int_t wing,Int_t chip, Int_t ch) 
      {fGain[wing][chip][ch]=g;}
    
    Int_t GetWing(Int_t anode) const{
      if(anode>=fgkChips*fgkChannels) return 1;
      else return 0;
    }
    Int_t GetChipChannel(Int_t anode) const {return anode%fgkChannels;}
    Int_t GetChip(Int_t anode) const {
      Int_t chip=anode/fgkChannels;
      if(GetWing(anode)==1)chip-=fgkChips;
      return chip;
    }
    
    void    PrintGains() const;
    void    Print();
    virtual void Print(ostream *os) const {AliITSCalibrationSDD::Print(os);}
    virtual void Print(Option_t *option="") const {AliITSCalibrationSDD::Print(option);}
    // not implemented virtual methods (devlared in the mother class
    virtual  void   SetDetParam(Double_t *) 
      {NotImplemented("SetDetParam");}
    virtual void   GetDetParam(Double_t *) const 
      {NotImplemented("GetDetParam");}
    virtual  void   SetNDetParam(Int_t /* n */)
      {NotImplemented("SetNDetParam");}
    virtual Int_t  NDetParam() const
      {NotImplemented("NDetParam"); return 0;}
    virtual void    SetSigmaSpread(Double_t, Double_t) 
      {NotImplemented("SetSigmaSpread");}
    virtual void    SigmaSpread(Double_t & /* p1 */,Double_t & /* p2 */) const 
      {NotImplemented("SigmaSpread");}

    void   SetDead() { fIsDead = kTRUE; };
    Bool_t IsDead() const { return fIsDead; };
    Int_t Wings()const{return fgkWings;}//Total number of SDD wings
    Int_t Chips() const{return fgkChips;} // Number of chips/module
    Int_t Channels() const{ return fgkChannels;}//Number of channels/chip
    
    virtual void SetBadChannel(Int_t i,Int_t anode);
    Int_t GetBadChannel(Int_t i) const {return fBadChannels[i];}
    Bool_t IsBadChannel(Int_t anode){
      if(GetChannelGain(anode)==0) return kTRUE;
      else return kFALSE;
    }
    Float_t GetMapACell(Int_t i,Int_t j) const {
      if(i<256) return fMapAW0->GetCellContent(i,j);
      else return fMapAW1->GetCellContent(i,j);
    }
    virtual void SetMapA(Int_t wing,AliITSMapSDD* mapA) {
      if(wing==0) fMapAW0=mapA;
      else fMapAW1=mapA;
    } 
    Float_t GetMapTCell(Int_t i,Int_t j) const {
      if(i<256) return fMapTW0->GetCellContent(i,j);
      else return fMapTW1->GetCellContent(i,j);
    }
    virtual void SetMapT(Int_t wing,AliITSMapSDD* mapT) {
      if(wing==0) fMapTW0=mapT;
      else fMapTW1=mapT;
    } 
    static Int_t GetMapTimeNBin() {return fgkMapTimeNBin;} 
    
    virtual void SetDriftSpeed(Int_t wing, AliITSDriftSpeedArraySDD* arr){
      if(wing==0) fDrSpeed0=arr;
      else fDrSpeed1=arr;
    }

    virtual void SetElectronics(Int_t p1=1) {((AliITSresponseSDD*)fResponse)->SetElectronics(p1);}
    virtual Int_t GetElectronics() const {return ((AliITSresponseSDD*)fResponse)->Electronics();}
    virtual void SetMaxAdc(Double_t p1) {((AliITSresponseSDD*)fResponse)->SetMaxAdc(p1);}
    virtual Float_t GetMaxAdc() const {return ((AliITSresponseSDD*)fResponse)->MaxAdc();} 
    virtual void SetChargeLoss(Double_t p1) {((AliITSresponseSDD*)fResponse)->SetChargeLoss(p1);}
    virtual Float_t GetChargeLoss() const {return ((AliITSresponseSDD*)fResponse)->ChargeLoss();}
    virtual void SetDynamicRange(Double_t p1) {((AliITSresponseSDD*)fResponse)->SetDynamicRange(p1);}
    virtual Float_t GetDynamicRange() const {return ((AliITSresponseSDD*)fResponse)->DynamicRange();} 

    virtual Float_t GetTimeOffset() const {return ((AliITSresponseSDD*)fResponse)->TimeOffset();}
    virtual Float_t GetADC2keV() const {return ((AliITSresponseSDD*)fResponse)->ADC2keV();}
    virtual Float_t GetDriftSpeedAtAnode(Float_t nAnode) const{
      if(fDrSpeed0==0 || fDrSpeed1==0) AliFatal("Drift speed not set\n");
      if(nAnode<256) return fDrSpeed0->GetDriftSpeed(0,nAnode);
      else return fDrSpeed1->GetDriftSpeed(0,nAnode-256);
    }
    virtual void SetParamOptions(const char *opt1,const char *opt2) {((AliITSresponseSDD*)fResponse)->SetParamOptions(opt1,opt2);}
    virtual void GetParamOptions(char *opt1,char *opt2) const {((AliITSresponseSDD*)fResponse)->ParamOptions(opt1,opt2);}
    virtual Bool_t Do10to8() const {return ((AliITSresponseSDD*)fResponse)->Do10to8();}
    virtual void SetZeroSupp (const char *opt) {((AliITSresponseSDD*)fResponse)->SetZeroSupp(opt);} 
    virtual const char *GetZeroSuppOption() const {return ((AliITSresponseSDD*)fResponse)->ZeroSuppOption();}
    virtual void SetNSigmaIntegration(Double_t p1) {((AliITSresponseSDD*)fResponse)->SetNSigmaIntegration(p1);}
    virtual Float_t GetNSigmaIntegration() const {return ((AliITSresponseSDD*)fResponse)->NSigmaIntegration();}
    virtual void SetNLookUp(Int_t p1) {((AliITSresponseSDD*)fResponse)->SetNLookUp(p1);}
    virtual Int_t GetGausNLookUp() const {return ((AliITSresponseSDD*)fResponse)->GausNLookUp();}
    virtual Float_t GetGausLookUp(Int_t i) const {return ((AliITSresponseSDD*)fResponse)->GausLookUp(i);}
    virtual Int_t Convert8to10(Int_t signal) const {return ((AliITSresponseSDD*)fResponse)->Convert8to10(signal);}
    virtual void  SetJitterError(Double_t jitter=20) {((AliITSresponseSDD*)fResponse)->SetJitterError(jitter);}
    virtual Float_t GetJitterError() const {return ((AliITSresponseSDD*)fResponse)->JitterError();}
    virtual Float_t GetDriftPath(Float_t time, Float_t xAnode) const {return time*GetDriftSpeedAtAnode(xAnode);}
    virtual Float_t GetThresholdAnode(Int_t anode,Int_t nsigma=3) const {
      return nsigma*fNoiseAfterEl[anode];}

    virtual void  SetDo10to8(Bool_t bitcomp=kTRUE) {((AliITSresponseSDD*)fResponse)->SetDo10to8(bitcomp);}
 protected:


    // these statis const should be move to AliITSsegmentationSDD
    static const Int_t fgkWings = 2;     // Number of wings per module
    static const Int_t fgkChips = 4;        // Number of chips/module
    static const Int_t fgkChannels = 64;    // Number of channels/chip
    static const Float_t fgkTemperatureDefault; // default for fT (Kelvin)
    static const Float_t fgkNoiseDefault; // default for fNoise
    static const Float_t fgkBaselineDefault; // default for fBaseline
    static const Float_t fgkMinValDefault; // default for fMinVal
    static const Float_t fgkGainDefault; //default for gain
    static const Int_t fgkMapTimeNBin = 72; //map granularity along drift direction
    Int_t fDeadChips;                     // Number of dead chips
    Int_t fDeadChannels;                  // Number of dead channels
    Float_t fGain[fgkWings][fgkChips][fgkChannels];//Array for channel gains
    Float_t fNoise[fgkWings*fgkChips*fgkChannels];          // Noise array
    Float_t fBaseline[fgkWings*fgkChips*fgkChannels];       // Baseline array
    Float_t fNoiseAfterEl[fgkWings*fgkChips*fgkChannels];   // Noise after electronics
    Float_t fMinVal;        // Min value used in 2D zero-suppression algo

    Bool_t   fIsDead;  // module is dead or alive ?
    TArrayI  fBadChannels; //Array with bad anodes number (0-512) 

    
    AliITSMapSDD* fMapAW0;     //! map of residuals on anode coord. wing 0
    AliITSMapSDD* fMapAW1;     //! map of residuals on anode coord. wing 1
    AliITSMapSDD* fMapTW0;     //! map of residuals on time coord. wing 0
    AliITSMapSDD* fMapTW1;     //! map of residuals on time coord. wing 1
    AliITSDriftSpeedArraySDD* fDrSpeed0; //! drift speed for wing 0
    AliITSDriftSpeedArraySDD* fDrSpeed1; //! drift speed for wing 1

 private:
    AliITSCalibrationSDD(const AliITSCalibrationSDD &ob); // copy constructor
    AliITSCalibrationSDD& operator=(const AliITSCalibrationSDD & /* source */); // ass. op.


    ClassDef(AliITSCalibrationSDD,8) // SDD response 
    
    };
#endif
