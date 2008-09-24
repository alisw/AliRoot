/**************************************************************************
 * Copyright(c) 2007-2009, ALICE Experiment at CERN, All rights reserved. *
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
#include <TFile.h>
#include "AliITSOnlineSDDTP.h"
#include "AliLog.h"
#include <TH2F.h>
#include <TMath.h>


///////////////////////////////////////////////////////////////////
//                                                               //
// Implemetation of the class SDD Test Pulse analysis            //
// Origin: F.Prino, Torino, prino@to.infn.it                     //
//                                                               //
///////////////////////////////////////////////////////////////////


ClassImp(AliITSOnlineSDDTP)

//______________________________________________________________________
AliITSOnlineSDDTP::AliITSOnlineSDDTP():AliITSOnlineSDD(),fDAC(0.),fLowThreshold(0),fHighThreshold(0),fNSigmaGain(0.),fNSigmaNoise(0.)
{
  // default constructor
  Reset();
  SetNSigmaGain();
  SetNSigmaNoise();
}
//______________________________________________________________________
AliITSOnlineSDDTP::AliITSOnlineSDDTP(Int_t nddl, Int_t ncarlos, Int_t sid, Float_t xDAC):AliITSOnlineSDD(nddl,ncarlos,sid),fDAC(xDAC),fLowThreshold(0),fHighThreshold(0),fNSigmaGain(0.),fNSigmaNoise(0.)
{
  // standard constructor
  Reset();
  SetNSigmaGain();
  SetNSigmaNoise();
}
//______________________________________________________________________
AliITSOnlineSDDTP::~AliITSOnlineSDDTP(){
  // Destructor
}
//______________________________________________________________________
void AliITSOnlineSDDTP::Reset(){
  //
  for(Int_t i=0;i<fgkNAnodes;i++){
    fNEvents[i]=0;
    fGoodAnode[i]=1;
    fBaseline[i]=0.;
    fCMN[i]=0.;
    fRawNoise[i]=0.;
    fCorrNoise[i]=0.;
    fSumTPPeak[i]=0.;
    fTPPos[i]=0.;
  }
  ReadBaselines();
}

//______________________________________________________________________
void AliITSOnlineSDDTP::AddEvent(TH2F* hrawd){
  // 
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    Float_t auxmax=0.;
    Int_t auxtb=0;
    if(!fGoodAnode[ian]) continue;
    for(Int_t itb=fFirstGoodTB;itb<=fLastGoodTB;itb++){
      Float_t cnt=hrawd->GetBinContent(itb+1,ian+1);
      if(cnt>auxmax){ 
	auxmax=cnt;
	auxtb=itb;
      }
    }
    if(auxmax>fBaseline[ian]+fNSigmaNoise*fRawNoise[ian]){
      fSumTPPeak[ian]+=auxmax-fBaseline[ian];
      fTPPos[ian]+=auxtb;
      fNEvents[ian]++;
    }
  }
}
//______________________________________________________________________
void AliITSOnlineSDDTP::ReadBaselines(){
  // assume baselines and good anodes are taken from previous run
  Char_t basfilnam[100];
  sprintf(basfilnam,"SDDbase_step2_ddl%02dc%02d_sid%d.data",fDDL,fCarlos,fSide);
  FILE* basf=fopen(basfilnam,"r");
  if(basf==0){
    AliWarning(Form("Baseline file not present (ddl %d  carlos %d side %d, Set all baselines to 20",fDDL,fCarlos,fSide));
    for(Int_t ian=0;ian<fgkNAnodes;ian++){ 
      fBaseline[ian]=20.;
      fEqBaseline[ian]=20;
      fOffsetBaseline[ian]=0;
      fGoodAnode[ian]=1;
    }
    return;
  }
  fscanf(basf,"%d\n",&fHighThreshold);
  fscanf(basf,"%d\n",&fLowThreshold);
  Int_t n,ok,eqbase,offbase;
  Float_t base,rms,cmn,corrnoi;
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    fscanf(basf,"%d %d %f %d %d %f %f %f\n",&n,&ok,&base,&eqbase,&offbase,&rms,&cmn,&corrnoi);
    fBaseline[ian]=base;
    fEqBaseline[ian]=eqbase;
    fOffsetBaseline[ian]=offbase;
    fGoodAnode[ian]=ok;
    fRawNoise[ian]=rms;
    fCMN[ian]=cmn;
    fCorrNoise[ian]=corrnoi;
  }
  fclose(basf);
}

//______________________________________________________________________
Bool_t AliITSOnlineSDDTP::IsModuleGood() const{
  //
  // Check if there is at least 1 good anode
  //
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    if(fGoodAnode[ian]) return kTRUE;
  }
  return kFALSE;
}
//______________________________________________________________________
void AliITSOnlineSDDTP::ValidateAnodes(){
  //
  Float_t meang,rmsg;
  StatGain(meang,rmsg);
  Float_t lowlim=meang-fNSigmaGain*rmsg;
  Float_t hilim=meang+fNSigmaGain*rmsg;

  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    if(!fGoodAnode[ian]) continue;
    if(GetChannelGain(ian)<lowlim||GetChannelGain(ian)>hilim) fGoodAnode[ian]=0;
  }
}


//______________________________________________________________________
void AliITSOnlineSDDTP::StatGain(Float_t &mean, Float_t  &rms){
  //
  Float_t sum=0.,sumq=0.;
  Int_t cnt=0;
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    if(!fGoodAnode[ian]) continue;
    if(fNEvents[ian]==0) continue;
    sum+=GetChannelGain(ian);
    sumq+=TMath::Power(GetChannelGain(ian),2);
    cnt++;
  }
  if(cnt>0){ 
    mean=sum/(Float_t)cnt;
    Float_t variance=sumq/(Float_t)cnt-mean*mean;
    if(variance>0.) rms=TMath::Sqrt(variance);
    else rms = 0;
  }else{ 
    mean=0.;
    rms=0.;
  }
  return;
}

//______________________________________________________________________
void AliITSOnlineSDDTP::WriteToASCII(){
  //
  Char_t outfilnam[100];
  sprintf(outfilnam,"SDDbase_ddl%02dc%02d_sid%d.data",fDDL,fCarlos,fSide);
  FILE* outf=fopen(outfilnam,"w");
  fprintf(outf,"%d %d %d\n",fCarlos,fSide,IsModuleGood());
  fprintf(outf,"%d\n",fHighThreshold);
  fprintf(outf,"%d\n",fLowThreshold);
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    fprintf(outf,"%d %d %8.3f %d %d %8.3f %8.3f %8.3f %8.3f\n",ian,IsAnodeGood(ian),GetAnodeBaseline(ian),GetAnodeEqualizedBaseline(ian),GetAnodeBaselineOffset(ian),GetAnodeRawNoise(ian),GetAnodeCommonMode(ian),GetAnodeCorrNoise(ian),GetChannelGain(ian));
  }
  fclose(outf);  
}
//______________________________________________________________________
Bool_t AliITSOnlineSDDTP::WriteToROOT(TFile *fil){
  //
  if(fil==0){ 
    AliWarning("Invalid pointer to ROOT file");
    return kFALSE;    
  }
  Char_t hisnam[20];
  fil->cd();
  sprintf(hisnam,"hgood%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hgood(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"hbase%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hbase(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"hnois%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hnois(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"hcmn%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hcmn(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"hcorn%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hcorn(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"hgain%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F hgain(hisnam,"",256,-0.5,255.5);
  sprintf(hisnam,"htptb%02dc%02ds%d",fDDL,fCarlos,fSide);
  TH1F htptb(hisnam,"",256,-0.5,255.5);
  for(Int_t ian=0;ian<fgkNAnodes;ian++){
    hgood.SetBinContent(ian+1,float(IsAnodeGood(ian)));
    hbase.SetBinContent(ian+1,GetAnodeBaseline(ian));
    hnois.SetBinContent(ian+1,GetAnodeRawNoise(ian));
    hcmn.SetBinContent(ian+1,GetAnodeCommonMode(ian));
    hcorn.SetBinContent(ian+1,GetAnodeCorrNoise(ian));
    hgain.SetBinContent(ian+1,GetChannelGain(ian));
    htptb.SetBinContent(ian+1,GetTimeBinTPPeak(ian));
  }
  hgood.Write();
  hbase.Write();
  hnois.Write();
  hcmn.Write();
  hcorn.Write();
  hgain.Write();
  htptb.Write();
  return kTRUE;
}

