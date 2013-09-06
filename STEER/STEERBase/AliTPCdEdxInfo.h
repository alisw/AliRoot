#ifndef AliTPCdEdxInfo_H
#define AliTPCdEdxInfo_H

#include <TObject.h>

class AliTPCdEdxInfo : public TObject 
{
public:
  AliTPCdEdxInfo();
  AliTPCdEdxInfo(const AliTPCdEdxInfo& source);
  AliTPCdEdxInfo& operator=(const AliTPCdEdxInfo& source);
  //
  // qTot info
  void     GetTPCSignalRegionInfo(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]) const;
  void     GetTPCSignals(Double_t signal[4]) const;

  void     SetTPCSignalRegionInfo(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]);
  void     SetTPCSignals(Double_t signal[4]);
  
  
  // qMax info
  void     GetTPCSignalRegionInfoQmax(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]) const;
  void     GetTPCSignalsQmax(Double_t signal[4]) const;

  void     SetTPCSignalRegionInfoQmax(Double_t signal[4], Char_t ncl[3], Char_t nrows[3]);
  void     SetTPCSignalsQmax(Double_t signal[4]);
  
  
  //
  Double_t GetTPCsignalShortPad()      const {return fTPCsignalRegion[0];}
  Double_t GetTPCsignalMediumPad()     const {return fTPCsignalRegion[1];}
  Double_t GetTPCsignalLongPad()       const {return fTPCsignalRegion[2];}
  Double_t GetTPCsignalOROC()          const {return fTPCsignalRegion[3];}
  
  Double_t GetTPCsignalShortPadQmax()  const {return fTPCsignalRegionQmax[0];}
  Double_t GetTPCsignalMediumPadQmax() const {return fTPCsignalRegionQmax[1];}
  Double_t GetTPCsignalLongPadQmax()   const {return fTPCsignalRegionQmax[2];}
  Double_t GetTPCsignalOROCQmax()      const {return fTPCsignalRegionQmax[3];}
  
private:

  Double32_t  fTPCsignalRegion[4]; //[0.,0.,10] TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used)  - for qTot
  Double32_t  fTPCsignalRegionQmax[4]; //[0.,0.,10] TPC dEdx signal in 4 different regions - 0 - IROC, 1- OROC medium, 2 - OROC long, 3- OROC all, (default truncation used) - for qMax
  Char_t      fTPCsignalNRegion[3]; // number of clusters above threshold used in the dEdx calculation
  Char_t      fTPCsignalNRowRegion[3]; // number of crosed rows used in the dEdx calculation - signal below threshold included

  
  ClassDef(AliTPCdEdxInfo,3)
};

#endif
