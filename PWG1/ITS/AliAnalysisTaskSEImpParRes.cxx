/**************************************************************************
 * Copyright(c) 1998-2010, ALICE Experiment at CERN, All rights reserved. *
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

/////////////////////////////////////////////////////////////
//
// AliAnalysisTaskSE for the study of the impact parameter resolution
//
// Authors:A.Dainese,    andrea.dainese@pd.infn.it
//     and Xianbao Yuan, yuanxb@iopp.ccnu.edu.cn; xianbao.yuan@pd.infn.it
/////////////////////////////////////////////////////////

#include <fstream.h>
#include <Riostream.h>
#include <TList.h>
#include <TH1F.h>

#include "AliAnalysisManager.h"
#include "AliMCEventHandler.h"
#include "AliMCEvent.h"
#include "AliStack.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"   
#include "AliVertexerTracks.h"
#include "AliAnalysisTaskSEImpParRes.h"

ClassImp(AliAnalysisTaskSEImpParRes)

//________________________________________________________________________
AliAnalysisTaskSEImpParRes::AliAnalysisTaskSEImpParRes():
AliAnalysisTaskSE(),
fReadMC(kFALSE),
fSelectedPdg(-1),
fOutputitspureSARec(0),
fOutputitspureSASkip(0), 
fOutputallPointRec(0),
fOutputallPointSkip(0),
fOutputpartPointRec(0),
fOutputpartPointSkip(0),
fOutputonepointSPDRec(0),
fOutputonepointSPDSkip(0),
fOutputpostvTracRec(0),
fOutputpostvTracSkip(0),
fOutputnegtvTracRec(0),
fOutputnegtvTracSkip(0),
fOutputpullAllpointRec(0),
fOutputpullAllpointSkip(0),
fOutputOnlyRefitRec(0),
fOutputOnlyRefitSkip(0),
fOutputPt(0),
fNentries(0),
fEstimVtx(0)
{
  //
  // Default constructor
  //
}

//________________________________________________________________________
AliAnalysisTaskSEImpParRes::AliAnalysisTaskSEImpParRes(const char *name):
AliAnalysisTaskSE(name),
fReadMC(kFALSE),
fSelectedPdg(-1),
fOutputitspureSARec(0),
fOutputitspureSASkip(0), 
fOutputallPointRec(0),
fOutputallPointSkip(0),
fOutputpartPointRec(0),
fOutputpartPointSkip(0),
fOutputonepointSPDRec(0),
fOutputonepointSPDSkip(0),
fOutputpostvTracRec(0),
fOutputpostvTracSkip(0),
fOutputnegtvTracRec(0),
fOutputnegtvTracSkip(0),
fOutputpullAllpointRec(0),
fOutputpullAllpointSkip(0),
fOutputOnlyRefitRec(0),
fOutputOnlyRefitSkip(0),
fOutputPt(0),
fNentries(0),
fEstimVtx(0)
{
  //
  // Default constructor
  //

  DefineOutput(1, TList::Class());  //My private output
  DefineOutput(2, TList::Class());  //My private output
  DefineOutput(3, TList::Class());  //My private output
  DefineOutput(4, TList::Class());  //My private output
  DefineOutput(5, TList::Class());
  DefineOutput(6, TList::Class());  //My private output
  DefineOutput(7, TList::Class());
  DefineOutput(8, TList::Class());  //My private output
  DefineOutput(9, TList::Class());  //My private output
  DefineOutput(10, TList::Class());  //My private output
  DefineOutput(11, TList::Class());  //My private output
  DefineOutput(12, TList::Class());
  DefineOutput(13, TList::Class());  //My private output
  DefineOutput(14, TList::Class());
  DefineOutput(15, TList::Class());  //My private output
  DefineOutput(16, TList::Class());
  DefineOutput(17, TList::Class());  //My private output
  DefineOutput(18, TH1F::Class());
  DefineOutput(19, TH1F::Class());
}

//________________________________________________________________________
AliAnalysisTaskSEImpParRes::~AliAnalysisTaskSEImpParRes()
{
  //
  // default distructor  
  // 
  if (fOutputitspureSARec)            { delete fOutputitspureSARec; fOutputitspureSARec=0x0;}
  if (fOutputitspureSASkip)         { delete fOutputitspureSASkip; fOutputitspureSASkip=0x0;}
  if (fOutputallPointRec)              { delete fOutputallPointRec; fOutputallPointRec=0x0; }
  if (fOutputallPointSkip)           { delete fOutputallPointSkip; fOutputallPointSkip=0x0; }
  if (fOutputpartPointRec)           { delete fOutputpartPointRec; fOutputpartPointRec=0x0; }
  if (fOutputpartPointSkip)        { delete fOutputpartPointSkip; fOutputpartPointSkip=0x0; }
  if (fOutputonepointSPDRec)       { delete fOutputonepointSPDRec;fOutputonepointSPDRec=0x0;}
  if (fOutputonepointSPDSkip)    { delete fOutputonepointSPDSkip;fOutputonepointSPDSkip=0x0;}
  if (fOutputpostvTracRec)            { delete fOutputpostvTracRec; fOutputpostvTracRec=0x0;}
  if (fOutputpostvTracSkip)        {  delete fOutputpostvTracSkip; fOutputpostvTracSkip=0x0;}
  if (fOutputnegtvTracRec)            { delete fOutputnegtvTracRec; fOutputnegtvTracRec=0x0;}
  if (fOutputnegtvTracSkip)         { delete fOutputnegtvTracSkip; fOutputnegtvTracSkip=0x0;}
  if (fOutputpullAllpointRec)    {delete fOutputpullAllpointRec; fOutputpullAllpointRec=0x0;}
  if (fOutputpullAllpointSkip) {delete fOutputpullAllpointSkip; fOutputpullAllpointSkip=0x0;}
  if (fOutputOnlyRefitRec)             {delete fOutputOnlyRefitRec; fOutputOnlyRefitRec=0x0;}
  if (fOutputOnlyRefitSkip)          {delete fOutputOnlyRefitSkip; fOutputOnlyRefitSkip=0x0;}
  if (fOutputPt)                                          { delete fOutputPt; fOutputPt=0x0;}
  if (fNentries)                                 { delete fNentries;     fNentries    =0x0; }
  if (fEstimVtx)                                 { delete fEstimVtx;     fEstimVtx    =0x0; }

} 

//________________________________________________________________________
void AliAnalysisTaskSEImpParRes::UserCreateOutputObjects()
{
  // 
  // Create the output container
  //
  
  if(fDebug>1) printf("AnalysisTaskSEImpParRes::UserCreateOutputObjects() \n");
  
  // Several histograms are more conveniently managed in a TList
  if (!fOutputitspureSARec) {
    fOutputitspureSARec = new TList();
    fOutputitspureSARec->SetOwner();
    fOutputitspureSARec->SetName("ITSpureSARec");
  }

  if (!fOutputitspureSASkip) {
    fOutputitspureSASkip = new TList();
    fOutputitspureSASkip->SetOwner();
    fOutputitspureSASkip->SetName("ITSpureSASkip");
  }

  if (!fOutputallPointRec) {
    fOutputallPointRec = new TList();
    fOutputallPointRec->SetOwner();
    fOutputallPointRec->SetName("allpointRec");
  }

  if (!fOutputallPointSkip) {
    fOutputallPointSkip = new TList();
    fOutputallPointSkip->SetOwner();
    fOutputallPointSkip->SetName("allpointSkip");
  }

  if (!fOutputpartPointRec) {
    fOutputpartPointRec = new TList();
    fOutputpartPointRec->SetOwner();
    fOutputpartPointRec->SetName("partpointRec");
  }

  if (!fOutputpartPointSkip) {
    fOutputpartPointSkip = new TList();
    fOutputpartPointSkip->SetOwner();
    fOutputpartPointSkip->SetName("partpointSkip");
  }

  if (!fOutputonepointSPDRec) {
    fOutputonepointSPDRec = new TList();
    fOutputonepointSPDRec->SetOwner();
    fOutputonepointSPDRec->SetName("onepointSPDRec");
  }

  if (!fOutputonepointSPDSkip) {
    fOutputonepointSPDSkip = new TList();
    fOutputonepointSPDSkip->SetOwner();
    fOutputonepointSPDSkip->SetName("onepointSPDSkip");
  }

  if (!fOutputpostvTracRec) {
    fOutputpostvTracRec = new TList();
    fOutputpostvTracRec->SetOwner();
    fOutputpostvTracRec->SetName("postvtracRec");
  }

  if (!fOutputpostvTracSkip) {
    fOutputpostvTracSkip = new TList();
    fOutputpostvTracSkip->SetOwner();
    fOutputpostvTracSkip->SetName("postvtracSkip");
  }
 
  if (!fOutputnegtvTracRec) {
    fOutputnegtvTracRec = new TList();
    fOutputnegtvTracRec->SetOwner();
    fOutputnegtvTracRec->SetName("negtvtracRe");
  }

  if (!fOutputnegtvTracSkip) {
    fOutputnegtvTracSkip = new TList();
    fOutputnegtvTracSkip->SetOwner();
    fOutputnegtvTracSkip->SetName("negtvtracSkip");
  }

  if (!fOutputpullAllpointSkip) {
    fOutputpullAllpointSkip = new TList();
    fOutputpullAllpointSkip->SetOwner();
    fOutputpullAllpointSkip->SetName("pullAllpointSkip");
  }

  if (!fOutputpullAllpointRec) {
    fOutputpullAllpointRec = new TList();
    fOutputpullAllpointRec->SetOwner();
    fOutputpullAllpointRec->SetName("pullAllpointRec");
  }

  if (!fOutputOnlyRefitRec) {
    fOutputOnlyRefitRec = new TList();
    fOutputOnlyRefitRec->SetOwner();
    fOutputOnlyRefitRec->SetName("onlyRefitRec");
  }

  if (!fOutputOnlyRefitSkip) {
    fOutputOnlyRefitSkip = new TList();
    fOutputOnlyRefitSkip->SetOwner();
    fOutputOnlyRefitSkip->SetName("onlyRefitRec");
  }

  if (!fOutputPt) {
    fOutputPt = new TList();
    fOutputPt->SetOwner();
    fOutputPt->SetName("Pt");
  }

  const Int_t nhist=26;
  const TString d0ITSpureSArphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0ITSpureSAzTitle = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0allpointrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0allpointzTitle  = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0partpointrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0partpointzTitle  = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0onepointSPDrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0onepointSPDzTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0postvtracrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0postvtraczTitle  = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0negtvtracrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0negtvtraczTitle  = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0pullAllpointrphiTitle = "d_0 Pull Distribution_rphi; d_0 pull; Entries";
  const TString d0pullAllpointzTitle  = "d_0 Pull Distribution_z; d_0 pull; Entries";
  const TString d0onlyRefitrphiTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  const TString d0onlyRefitzTitle  = "d_0 Distribution_z; d_0 [#mum]; Entries";
  const TString d0ptTitle = "d_0 Distribution_rphi; d_0 [#mum]; Entries";
  TString  named0itspureSArphiRec,named0itspureSAzRec,named0allpointrphiRec, named0allpointzRec,named0partpointrphiRec, named0partpointzRec,named0onepointSPDrphiRec, named0onepointSPDzRec,named0postvtracrphiRec, named0postvtraczRec,named0negtvtracrphiRec, named0negtvtraczRec,named0pt,named0pullAllpointrphiRec,named0pullAllpointzRec,named0onlyRefitrphiRec,named0onlyRefitzRec;
  TH1F *d0ITSpureSArphiRec=0,*d0ITSpureSAzRec=0,*d0AllpointrphiRec=0, *d0AllpointzRec=0,*d0PartpointrphiRec=0, *d0PartpointzRec=0,
    *d0OnepointSPDrphiRec=0,*d0OnepointSPDzRec=0,*d0PostvtracrphiRec=0, *d0PostvtraczRec=0,*d0NegtvtracrphiRec=0, *d0NegtvtraczRec=0,*d0Pt=0,*d0PullAllpointrphiRec,*d0PullAllpointzRec,*d0OnlyRefitrphiRec,*d0OnlyRefitzRec;

  TString  named0itspureSArphiSkip,named0itspureSAzSkip,named0allpointrphiSkip, named0allpointzSkip,named0partpointrphiSkip, named0partpointzSkip,named0onepointSPDrphiSkip, named0onepointSPDzSkip,named0postvtracrphiSkip, named0postvtraczSkip,named0negtvtracrphiSkip, named0negtvtraczSkip,named0ptSkip,named0pullAllpointrphiSkip,named0pullAllpointzSkip,named0onlyRefitrphiSkip,named0onlyRefitzSkip;
  TH1F *d0ITSpureSArphiSkip=0,*d0ITSpureSAzSkip=0,*d0AllpointrphiSkip=0, *d0AllpointzSkip=0,*d0PartpointrphiSkip=0, *d0PartpointzSkip=0,*d0OnepointSPDrphiSkip=0,*d0OnepointSPDzSkip=0,*d0PostvtracrphiSkip=0, *d0PostvtraczSkip=0,*d0NegtvtracrphiSkip=0,*d0NegtvtraczSkip=0,*d0PullAllpointrphiSkip,*d0PullAllpointzSkip,*d0OnlyRefitrphiSkip,*d0OnlyRefitzSkip;


  for(Int_t i=0; i<nhist; i++) {

    named0itspureSArphiRec = "d0itspureSArphiRec_";
    named0itspureSArphiRec += (i+1);
    named0itspureSAzRec = "d0itspureSAzRec_";
    named0itspureSAzRec += (i+1);
    d0ITSpureSArphiRec = new TH1F(named0itspureSArphiRec.Data(), d0ITSpureSArphiTitle.Data(), 400, -2000., 2000.);
    d0ITSpureSArphiRec->Sumw2();
    d0ITSpureSArphiRec->SetMinimum(0);  
    fOutputitspureSARec->Add(d0ITSpureSArphiRec);
    d0ITSpureSAzRec = new TH1F(named0itspureSAzRec.Data(), d0ITSpureSAzTitle.Data(), 400, -2000., 2000.);
    d0ITSpureSAzRec->Sumw2();
    d0ITSpureSAzRec->SetMinimum(0);  
    fOutputitspureSARec->Add(d0ITSpureSAzRec);

    named0itspureSArphiSkip = "d0itspureSArphiSkip_";
    named0itspureSArphiSkip += (i+1);
    named0itspureSAzSkip = "d0itspureSAzSkip_";
    named0itspureSAzSkip += (i+1);
    d0ITSpureSArphiSkip = new TH1F(named0itspureSArphiSkip.Data(), d0ITSpureSArphiTitle.Data(), 400, -2000., 2000.);
    d0ITSpureSArphiSkip->Sumw2();
    d0ITSpureSArphiSkip->SetMinimum(0);  
    fOutputitspureSASkip->Add(d0ITSpureSArphiSkip);
    d0ITSpureSAzSkip = new TH1F(named0itspureSAzSkip.Data(), d0ITSpureSAzTitle.Data(), 400, -2000., 2000.);
    d0ITSpureSAzSkip->Sumw2();
    d0ITSpureSAzSkip->SetMinimum(0);  
    fOutputitspureSASkip->Add(d0ITSpureSAzSkip);

    named0allpointrphiRec = "d0allpointrphiRec_";
    named0allpointrphiRec += (i+1);
    named0allpointzRec = "d0allpointzRec_";
    named0allpointzRec += (i+1);
    d0AllpointrphiRec = new TH1F(named0allpointrphiRec.Data(), d0allpointrphiTitle.Data(), 400, -2000., 2000.);
    d0AllpointrphiRec->Sumw2();
    d0AllpointrphiRec->SetMinimum(0);  
    fOutputallPointRec->Add(d0AllpointrphiRec);
    d0AllpointzRec= new TH1F(named0allpointzRec.Data(), d0allpointzTitle.Data(), 400, -2000., 2000.);
    d0AllpointzRec->Sumw2();
    d0AllpointzRec->SetMinimum(0);  
    fOutputallPointRec->Add(d0AllpointzRec);

    named0allpointrphiSkip = "d0allpointrphiSkip_";
    named0allpointrphiSkip += (i+1);
    named0allpointzSkip = "d0allpointzSkip_";
    named0allpointzSkip += (i+1);
    d0AllpointrphiSkip = new TH1F(named0allpointrphiSkip.Data(), d0allpointrphiTitle.Data(), 400, -2000., 2000.);
    d0AllpointrphiSkip->Sumw2();
    d0AllpointrphiSkip->SetMinimum(0);  
    fOutputallPointSkip->Add(d0AllpointrphiSkip);
    d0AllpointzSkip = new TH1F(named0allpointzSkip.Data(), d0allpointzTitle.Data(), 400, -2000., 2000.);
    d0AllpointzSkip->Sumw2();
    d0AllpointzSkip->SetMinimum(0);  
    fOutputallPointSkip->Add(d0AllpointzSkip);

    named0partpointrphiRec = "d0partpointrphiRec_";
    named0partpointrphiRec += (i+1);
    named0partpointzRec = "d0partpointzRec_";
    named0partpointzRec += (i+1);
    d0PartpointrphiRec = new TH1F(named0partpointrphiRec.Data(), d0partpointrphiTitle.Data(), 400, -2000., 2000.);
    d0PartpointrphiRec->Sumw2();
    d0PartpointrphiRec->SetMinimum(0);  
    fOutputpartPointRec->Add(d0PartpointrphiRec);
    d0PartpointzRec = new TH1F(named0partpointzRec.Data(), d0partpointzTitle.Data(), 400, -2000., 2000.);
    d0PartpointzRec->Sumw2();
    d0PartpointzRec->SetMinimum(0);  
    fOutputpartPointRec->Add(d0PartpointzRec);

    named0partpointrphiSkip = "d0partpointrphiSkip_";
    named0partpointrphiSkip += (i+1);
    named0partpointzSkip = "d0partpointzSkip_";
    named0partpointzSkip += (i+1);
    d0PartpointrphiSkip = new TH1F(named0partpointrphiSkip.Data(), d0partpointrphiTitle.Data(), 400, -2000., 2000.);
    d0PartpointrphiSkip->Sumw2();
    d0PartpointrphiSkip->SetMinimum(0);  
    fOutputpartPointSkip->Add(d0PartpointrphiSkip);
    d0PartpointzSkip = new TH1F(named0partpointzSkip.Data(), d0partpointzTitle.Data(), 400, -2000., 2000.);
    d0PartpointzSkip->Sumw2();
    d0PartpointzSkip->SetMinimum(0);  
    fOutputpartPointSkip->Add(d0PartpointzSkip);

    named0onepointSPDrphiRec = "d0onepointSPDrphiRec_";
    named0onepointSPDrphiRec += (i+1);
    named0onepointSPDzRec = "d0onepointSPDzRec_";
    named0onepointSPDzRec += (i+1);
    d0OnepointSPDrphiRec = new TH1F(named0onepointSPDrphiRec.Data(), d0onepointSPDrphiTitle.Data(), 400, -2000., 2000.);
    d0OnepointSPDrphiRec->Sumw2();
    d0OnepointSPDrphiRec->SetMinimum(0);  
    fOutputonepointSPDRec->Add(d0OnepointSPDrphiRec);
    d0OnepointSPDzRec = new TH1F(named0onepointSPDzRec.Data(), d0onepointSPDzTitle.Data(), 400, -2000., 2000.);
    d0OnepointSPDzRec->Sumw2();
    d0OnepointSPDzRec->SetMinimum(0);  
    fOutputonepointSPDRec->Add(d0OnepointSPDzRec);

    named0onepointSPDrphiSkip = "d0onepointSPDrphiSkip_";
    named0onepointSPDrphiSkip += (i+1);
    named0onepointSPDzSkip = "d0onepointSPDzSkip_";
    named0onepointSPDzSkip += (i+1);
    d0OnepointSPDrphiSkip = new TH1F(named0onepointSPDrphiSkip.Data(), d0onepointSPDrphiTitle.Data(), 400, -2000., 2000.);
    d0OnepointSPDrphiSkip->Sumw2();
    d0OnepointSPDrphiSkip->SetMinimum(0);  
    fOutputonepointSPDSkip->Add(d0OnepointSPDrphiSkip);
    d0OnepointSPDzSkip = new TH1F(named0onepointSPDzSkip.Data(), d0onepointSPDzTitle.Data(), 400, -2000., 2000.);
    d0OnepointSPDzSkip->Sumw2();
    d0OnepointSPDzSkip->SetMinimum(0);  
    fOutputonepointSPDSkip->Add(d0OnepointSPDzSkip);

    named0postvtracrphiRec = "d0postvtracrphiRec_";
    named0postvtracrphiRec += (i+1);
    named0postvtraczRec = "d0postvtraczRec_";
    named0postvtraczRec += (i+1);
    d0PostvtracrphiRec = new TH1F(named0postvtracrphiRec.Data(), d0postvtracrphiTitle.Data(), 400, -2000., 2000.);
    d0PostvtracrphiRec->Sumw2();
    d0PostvtracrphiRec->SetMinimum(0);  
    fOutputpostvTracRec->Add(d0PostvtracrphiRec);
    d0PostvtraczRec = new TH1F(named0postvtraczRec.Data(), d0postvtraczTitle.Data(), 400, -2000., 2000.);
    d0PostvtraczRec->Sumw2();
    d0PostvtraczRec->SetMinimum(0);  
    fOutputpostvTracRec->Add(d0PostvtraczRec);

    named0postvtracrphiSkip = "d0postvtracrphiSkip_";
    named0postvtracrphiSkip += (i+1);
    named0postvtraczSkip = "d0postvtraczSkip_";
    named0postvtraczSkip += (i+1);
    d0PostvtracrphiSkip = new TH1F(named0postvtracrphiSkip.Data(), d0postvtracrphiTitle.Data(), 400, -2000., 2000.);
    d0PostvtracrphiSkip->Sumw2();
    d0PostvtracrphiSkip->SetMinimum(0);  
    fOutputpostvTracSkip->Add(d0PostvtracrphiSkip);
    d0PostvtraczSkip = new TH1F(named0postvtraczSkip.Data(), d0postvtraczTitle.Data(), 400, -2000., 2000.);
    d0PostvtraczSkip->Sumw2();
    d0PostvtraczSkip->SetMinimum(0);  
    fOutputpostvTracSkip->Add(d0PostvtraczSkip);

    named0negtvtracrphiRec = "d0negtvtracrphiRec_";
    named0negtvtracrphiRec += (i+1);
    named0negtvtraczRec = "d0negtvtraczRec_";
    named0negtvtraczRec += (i+1);
    d0NegtvtracrphiRec = new TH1F(named0negtvtracrphiRec.Data(), d0negtvtracrphiTitle.Data(), 400, -2000., 2000.);
    d0NegtvtracrphiRec->Sumw2();
    d0NegtvtracrphiRec->SetMinimum(0);  
    fOutputnegtvTracRec->Add(d0NegtvtracrphiRec);
    d0NegtvtraczRec = new TH1F(named0negtvtraczRec.Data(), d0negtvtraczTitle.Data(), 400, -2000., 2000.);
    d0NegtvtraczRec->Sumw2();
    d0NegtvtraczRec->SetMinimum(0);  
    fOutputnegtvTracRec->Add(d0NegtvtraczRec);

    named0negtvtracrphiSkip = "d0negtvtracrphiSkip_";
    named0negtvtracrphiSkip += (i+1);
    named0negtvtraczSkip = "d0negtvtraczSkip_";
    named0negtvtraczSkip += (i+1);
    d0NegtvtracrphiSkip = new TH1F(named0negtvtracrphiSkip.Data(), d0negtvtracrphiTitle.Data(), 400, -2000., 2000.);
    d0NegtvtracrphiSkip->Sumw2();
    d0NegtvtracrphiSkip->SetMinimum(0);  
    fOutputnegtvTracSkip->Add(d0NegtvtracrphiSkip);
    d0NegtvtraczSkip = new TH1F(named0negtvtraczSkip.Data(), d0negtvtraczTitle.Data(), 400, -2000., 2000.);
    d0NegtvtraczSkip->Sumw2();
    d0NegtvtraczSkip->SetMinimum(0);  
    fOutputnegtvTracSkip->Add(d0NegtvtraczSkip);

    named0pullAllpointrphiSkip = "d0pullAllpointrphiSkip_";
    named0pullAllpointrphiSkip +=(i+1);
    named0pullAllpointzSkip = "d0pullAllpointzSkip_";
    named0pullAllpointzSkip +=(i+1);
    d0PullAllpointrphiSkip = new TH1F(named0pullAllpointrphiSkip.Data(),d0pullAllpointrphiTitle.Data(),400,-10.,10.);
    d0PullAllpointrphiSkip->Sumw2();
    d0PullAllpointrphiSkip->SetMinimum(0);
    fOutputpullAllpointSkip->Add(d0PullAllpointrphiSkip);
    d0PullAllpointzSkip = new TH1F(named0pullAllpointzSkip.Data(),d0pullAllpointzTitle.Data(),400,-10.,10.);
    d0PullAllpointzSkip->Sumw2();
    d0PullAllpointzSkip->SetMinimum(0);
    fOutputpullAllpointSkip->Add(d0PullAllpointzSkip);

    named0pullAllpointrphiRec = "d0pullAllpointrphiRec_";
    named0pullAllpointrphiRec +=(i+1);
    named0pullAllpointzRec = "d0pullAllpointzRec_";
    named0pullAllpointzRec +=(i+1);
    d0PullAllpointrphiRec = new TH1F(named0pullAllpointrphiRec.Data(),d0pullAllpointrphiTitle.Data(),400,-10.,10.);
    d0PullAllpointrphiRec->Sumw2();
    d0PullAllpointrphiRec->SetMinimum(0);
    fOutputpullAllpointRec->Add(d0PullAllpointrphiRec);
    d0PullAllpointzRec = new TH1F(named0pullAllpointzRec.Data(),d0pullAllpointzTitle.Data(),400,-10.,10.);
    d0PullAllpointzRec->Sumw2();
    d0PullAllpointzRec->SetMinimum(0);
    fOutputpullAllpointRec->Add(d0PullAllpointzRec);


    named0onlyRefitrphiRec = "d0onlyrefitrphiRec_";
    named0onlyRefitrphiRec +=(i+1);
    named0onlyRefitzRec = "d0onlyrefitzRec_";
    named0onlyRefitzRec +=(i+1);
    d0OnlyRefitrphiRec = new TH1F(named0onlyRefitrphiRec.Data(),d0onlyRefitrphiTitle.Data(),400,-2000.,2000.);
    d0OnlyRefitrphiRec->Sumw2();
    d0OnlyRefitrphiRec->SetMinimum(0);
    fOutputOnlyRefitRec->Add(d0OnlyRefitrphiRec);
    d0OnlyRefitzRec = new TH1F(named0onlyRefitzRec.Data(),d0onlyRefitzTitle.Data(),400,-2000.,2000.);
    d0OnlyRefitzRec->Sumw2();
    d0OnlyRefitzRec->SetMinimum(0);
    fOutputOnlyRefitRec->Add(d0OnlyRefitzRec);


    named0onlyRefitrphiSkip = "d0onlyrefitrphiSkip_";
    named0onlyRefitrphiSkip +=(i+1);
    named0onlyRefitzSkip = "d0onlyrefitzSkip_";
    named0onlyRefitzSkip +=(i+1);
    d0OnlyRefitrphiSkip = new TH1F(named0onlyRefitrphiSkip.Data(),d0onlyRefitrphiTitle.Data(),400,-2000.,2000.);
    d0OnlyRefitrphiSkip->Sumw2();
    d0OnlyRefitrphiSkip->SetMinimum(0);
    fOutputOnlyRefitSkip->Add(d0OnlyRefitrphiSkip);
    d0OnlyRefitzSkip = new TH1F(named0onlyRefitzSkip.Data(),d0onlyRefitzTitle.Data(),400,-2000.,2000.);
    d0OnlyRefitzSkip->Sumw2();
    d0OnlyRefitzSkip->SetMinimum(0);
    fOutputOnlyRefitSkip->Add(d0OnlyRefitzSkip);

    named0pt = "d0pt_";
    named0pt += (i+1);
    d0Pt = new TH1F(named0pt.Data(), d0ptTitle.Data(), 100, 0, 35.);
    d0Pt->Sumw2();
    d0Pt->SetMinimum(0);  
    fOutputPt->Add(d0Pt);

  }

  if(!fNentries) fNentries = new TH1F("hNentries", "number of entries", 26, 0., 40.);
  if(!fEstimVtx) fEstimVtx = new TH1F("vtxRes","Resolution of vertex",400,-1000.,1000);

  return;
}

//________________________________________________________________________
void AliAnalysisTaskSEImpParRes::UserExec(Option_t */*option*/)
{
  //
  // Track selection and filling of d0 histograms
  //
  AliESDEvent *esd = dynamic_cast<AliESDEvent*>(InputEvent());
  if (!esd) {
    AliError("ESD event not found. Nothing done!");
    return;
  }

  Float_t diamondcovxy[3];
  esd->GetDiamondCovXY(diamondcovxy);
  Double_t pos[3]={esd->GetDiamondX(),esd->GetDiamondY(),0.};
  diamondcovxy[0]=0.05*0.05;
  diamondcovxy[2]=0.05*0.05;
  Double_t cov[6]={diamondcovxy[0],diamondcovxy[1],diamondcovxy[2],0.,0.,10.};
  AliESDVertex *diamond = new AliESDVertex(pos,cov,1.,1);

  AliVertexerTracks *vertexer0 = new AliVertexerTracks(esd->GetMagneticField());
  vertexer0->SetITSMode();
  vertexer0->SetMinClusters(4);  
  vertexer0->SetVtxStart(diamond);
  AliESDVertex *vtxESDRec = (AliESDVertex*)vertexer0->FindPrimaryVertex(esd);
  delete vertexer0; vertexer0=0;
  if(vtxESDRec->GetNContributors()<1) return;


  AliStack *stack=0; 
  if (fReadMC) { 
    AliMCEventHandler *eventHandler = dynamic_cast<AliMCEventHandler*>(AliAnalysisManager::GetAnalysisManager()->GetMCtruthEventHandler()); 
    if (!eventHandler) { 
      Printf("ERROR: Could not retrieve MC event handler"); 
      return; 
    } 

    AliMCEvent* mcEvent = eventHandler->MCEvent(); 
    if (!mcEvent) { 
      Printf("ERROR: Could not retrieve MC event"); 
      return; 
    } 

    stack = mcEvent->Stack(); 
    if (!stack) { 
      AliDebug(AliLog::kError, "Stack not available"); 
      return; 
    } 
  } 

  
  Int_t nTrks = esd->GetNumberOfTracks();
  for (Int_t it=0; it<nTrks; it++) {  // loop over tracks
    // read track
    AliESDtrack *esdtrack = esd->GetTrack(it);

    // ask for ITS refit
    if (!(esdtrack->GetStatus()&AliESDtrack::kITSrefit)) {
      continue;
    }
    
    Int_t pdgCode=0; 
    if(fReadMC && stack) { 
      Int_t trkLabel = esdtrack->GetLabel(); 
      //Int_t trkLabel = TMath::Abs(esdtrack->GetLabel()); 
      if(trkLabel<0) continue;
      TParticle  *part = (TParticle*)stack->Particle(trkLabel); 
      pdgCode = TMath::Abs(part->GetPdgCode()); 
      //printf("pdgCode===%d\n", pdgCode);
      if(fSelectedPdg>0 && pdgCode!=fSelectedPdg) continue;
    }


    Int_t npointsITS=0,npointsSPD=0;
    for (Int_t i=0;i<6;i++){ 
      if (i<2 && esdtrack->HasPointOnITSLayer(i)) npointsSPD +=1;
      if (esdtrack->HasPointOnITSLayer(i)) npointsITS +=1;  
    }

    //Get specific primary vertex--Reconstructed primary vertex do not include the track considering.
    AliVertexerTracks *vertexer = new AliVertexerTracks(esd->GetMagneticField());
    vertexer->SetITSMode();
    vertexer->SetMinClusters(4);
    vertexer->SetVtxStart(diamond);
    Int_t skipped[2];
    skipped[0] = (Int_t)esdtrack->GetID();
    vertexer->SetSkipTracks(1,skipped);      
    AliESDVertex *vtxESDSkip = (AliESDVertex*)vertexer->FindPrimaryVertex(esd); 
    delete vertexer; vertexer=NULL;
    if(vtxESDSkip->GetNContributors()<1) continue;
     

    Double_t dzRec[2], covdzRec[3], dzRecSkip[2], covdzRecSkip[3];
    Double_t beampiperadius=3.;
    Double_t pt= esdtrack->Pt();
    Int_t bin = PtBin(pt);

    if(bin==-1) {
      delete vtxESDSkip; vtxESDSkip = 0x0;
      continue;
    }


    esdtrack->PropagateToDCA(vtxESDRec, esd->GetMagneticField(), beampiperadius, dzRec, covdzRec);
    esdtrack->PropagateToDCA(vtxESDSkip, esd->GetMagneticField(), beampiperadius, dzRecSkip, covdzRecSkip);

    if(covdzRec[0]<0 || covdzRec[2]<0 || covdzRecSkip[0]<0 || covdzRecSkip[2]<0) {
      delete vtxESDSkip; vtxESDSkip = 0x0;
      continue;
    }
      
    //printf("Pt: %f GeV/c; Impact parameter: rphi %f cm  z %f cm\n", pt, dzRec[0], dzRec[1]);

    /*
    // RUBEN'S METHOD, NOT FULLY IMPLEMENTED YET    
    Double_t      fIPCenIni[3], xyzDCA[3];
    for (int i=3;i--;) fIPCenIni[i] = 0.;
    //Int_t nTracks = nTrks - 1;
    esdtrack->GetXYZ(xyzDCA);
    //double pTrack = esdtrack->GetP();
    double phiTrack = esdtrack->Phi();
    double cs = TMath::Cos(phiTrack);
    double sn = TMath::Sin(phiTrack);
    double trDCA = (xyzDCA[0]-fIPCenIni[0])         *sn - (xyzDCA[1]-fIPCenIni[1])         *cs;  // track signed DCA to origin
    double vtDCA = (vtxESDSkip->GetXv()-fIPCenIni[0])*sn - (vtxESDSkip->GetYv()-fIPCenIni[1])*cs;  // vertex signed DCA to origin
    
    // update the estimator values
    //double estIP  = rvD*rtD;
    double estVtx = vtDCA*( vtDCA- trDCA);
    //double estTrc = rtD*(rtD - rvD);
    //
    //if (nTracks >= fMinTracksForIP) fEstimIP->Fill(phiTrack, estIP);
    fEstimVtx->Fill(10000*estVtx);
    //if (pTrack<1e-6) pTrack = GetTrackMinP()+1e6;
    //fEstimTrc->Fill(1./pTrack,estTrc);
    */


    //fill the histgram with all particle 
    //-------------------------------------------1----------------------------------------------
    
    /*
    TString named0AllrphiRec=" ",named0AllzRec =" ";//named0AllrphiTrue=" ",named0AllzTrue =" ";
      //named0AllrphiSkip=" ",named0AllzSkip =" ";
      
      named0AllrphiRec ="d0allrphiRec_";
      named0AllrphiRec += bin;
      named0AllzRec = "d0allzRec_";
      named0AllzRec += bin;
      ((TH1F*)(fOutputallRec->FindObject(named0AllrphiRec)))->Fill(10000*dzRec[0]);
      ((TH1F*)(fOutputallRec->FindObject(named0AllzRec)))->Fill(10000*dzRec[1]);
    */

    //-------------------------------------------2----------------------------------------------
    //TSting named0AllrphiRec = "d0allrphiRec_" + bin;
    //TSting named0AllzRec = "d0allzRec_" + bin;
    //((TH1F*)(fOutputallRec->FindObject(named0AllrphiRec.Data())))->Fill(10000.*dzRec[0]);
    //((TH1F*)(fOutputallRec->FindObject(named0AllzRec.Data())))->Fill(10000.*dzRec[1]);

    //-------------------------------------------3------------------------------------------------


    // ITS standalone
    if (!(esdtrack->GetStatus()&AliESDtrack::kTPCrefit) && (esdtrack->GetStatus()&AliESDtrack::kITSpureSA)
	&& npointsSPD>0 && npointsITS>=4) {
      char *named0ITSpureSArphiRec = Form("d0itspureSArphiRec_%d", bin);
      char *named0ITSpureSArphiSkip = Form("d0itspureSArphiSkip_%d", bin);
      char *named0ITSpureSAzRec = Form("d0itspureSAzRec_%d", bin); 
      char *named0ITSpureSAzSkip = Form("d0itspureSAzSkip_%d", bin); 
      ((TH1F*)(fOutputitspureSARec->FindObject(named0ITSpureSArphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputitspureSARec->FindObject(named0ITSpureSAzRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputitspureSASkip->FindObject(named0ITSpureSArphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputitspureSASkip->FindObject(named0ITSpureSAzSkip)))->Fill(10000.*dzRecSkip[1]);
    }

    
    // ask for TPC refit
    if (!(esdtrack->GetStatus()&AliESDtrack::kTPCrefit) || esdtrack->GetNcls(1)<70) {
      delete vtxESDSkip; vtxESDSkip = 0x0;
      continue;
    }

    // only ITS and TPC refit
    char *named0OnlyrefitrphiRec = Form("d0onlyrefitrphiRec_%d", bin);
    char *named0OnlyrefitrphiSkip = Form("d0onlyrefitrphiSkip_%d", bin);
    char *named0OnlyrefitzRec = Form("d0onlyrefitzRec_%d", bin);
    char *named0OnlyrefitzSkip = Form("d0onlyrefitzSkip_%d", bin); 
    ((TH1F*)(fOutputOnlyRefitRec->FindObject(named0OnlyrefitrphiRec)))->Fill(10000.*dzRec[0]);
    ((TH1F*)(fOutputOnlyRefitRec->FindObject(named0OnlyrefitzRec)))->Fill(10000.*dzRec[1]);
    ((TH1F*)(fOutputOnlyRefitSkip->FindObject(named0OnlyrefitrphiSkip)))->Fill(10000.*dzRecSkip[0]);
    ((TH1F*)(fOutputOnlyRefitSkip->FindObject(named0OnlyrefitzSkip)))->Fill(10000.*dzRecSkip[1]);  
    
    // with 6 ITS points
    if(npointsITS==6) {
      char *named0AllpointrphiRec = Form("d0allpointrphiRec_%d", bin);
      char *named0AllpointrphiSkip = Form("d0allpointrphiSkip_%d", bin);
      char *named0AllpointzRec = Form("d0allpointzRec_%d", bin);
      char *named0AllpointzSkip = Form("d0allpointzSkip_%d", bin); 
      ((TH1F*)(fOutputallPointRec->FindObject(named0AllpointrphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputallPointRec->FindObject(named0AllpointzRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputallPointSkip->FindObject(named0AllpointrphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputallPointSkip->FindObject(named0AllpointzSkip)))->Fill(10000.*dzRecSkip[1]);
 
      // pulls
      char *named0PullAllpointrphiRec = Form("d0pullAllpointrphiRec_%d", bin);
      char *named0PullAllpointrphiSkip = Form("d0pullAllpointrphiSkip_%d", bin);
      char *named0PullAllpointzRec = Form("d0pullAllpointzRec_%d", bin);
      char *named0PullAllpointzSkip = Form("d0pullAllpointzSkip_%d", bin); 
      ((TH1F*)(fOutputpullAllpointRec->FindObject(named0PullAllpointrphiRec)))->Fill(dzRec[0]/TMath::Sqrt(covdzRec[0]));
      
      ((TH1F*)(fOutputpullAllpointRec->FindObject(named0PullAllpointzRec)))->Fill(dzRec[1]/TMath::Sqrt(covdzRec[2]));
      ((TH1F*)(fOutputpullAllpointSkip->FindObject(named0PullAllpointrphiSkip)))->Fill(dzRecSkip[0]/TMath::Sqrt(covdzRecSkip[0]));
      ((TH1F*)(fOutputpullAllpointSkip->FindObject(named0PullAllpointzSkip)))->Fill(dzRecSkip[1]/TMath::Sqrt(covdzRecSkip[2]));
    }
   
    if(npointsITS>=4 && npointsSPD>0) {
      char *named0PartpointrphiRec = Form("d0partpointrphiRec_%d", bin);
      char *named0PartpointrphiSkip = Form("d0partpointrphiSkip_%d", bin);
      char *named0PartpointzRec = Form("d0partpointzRec_%d", bin); 
      char *named0PartpointzSkip = Form("d0partpointzSkip_%d", bin); 
      ((TH1F*)(fOutputpartPointRec->FindObject(named0PartpointrphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputpartPointRec->FindObject(named0PartpointzRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputpartPointSkip->FindObject(named0PartpointrphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputpartPointSkip->FindObject(named0PartpointzSkip)))->Fill(10000.*dzRecSkip[1]);
    }

    if(npointsSPD>0) {
      char *named0OnepointSPDrphiRec = Form("d0onepointSPDrphiRec_%d", bin);
      char *named0OnepointSPDrphiSkip = Form("d0onepointSPDrphiSkip_%d", bin);
      char *named0OnepointSPDzRec = Form("d0onepointSPDzRec_%d", bin); 
      char *named0OnepointSPDzSkip = Form("d0onepointSPDzSkip_%d", bin); 
      ((TH1F*)(fOutputonepointSPDRec->FindObject(named0OnepointSPDrphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputonepointSPDRec->FindObject(named0OnepointSPDzRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputonepointSPDSkip->FindObject(named0OnepointSPDrphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputonepointSPDSkip->FindObject(named0OnepointSPDzSkip)))->Fill(10000.*dzRecSkip[1]);
    }

    Short_t charge=esdtrack->Charge();
    if(charge==1 && npointsITS==6) {
      char *named0PostvtracrphiRec = Form("d0postvtracrphiRec_%d", bin);
      char *named0PostvtracrphiSkip = Form("d0postvtracrphiSkip_%d", bin);
      char *named0PostvtraczRec = Form("d0postvtraczRec_%d", bin); 
      char *named0PostvtraczSkip = Form("d0postvtraczSkip_%d", bin); 
      ((TH1F*)(fOutputpostvTracRec->FindObject(named0PostvtracrphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputpostvTracRec->FindObject(named0PostvtraczRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputpostvTracSkip->FindObject(named0PostvtracrphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputpostvTracSkip->FindObject(named0PostvtraczSkip)))->Fill(10000.*dzRecSkip[1]);
    }

    if(charge==-1 && npointsITS==6) {
      char *named0NegtvtracrphiRec = Form("d0negtvtracrphiRec_%d", bin);
      char *named0NegtvtracrphiSkip = Form("d0negtvtracrphiSkip_%d", bin);
      char *named0NegtvtraczRec = Form("d0negtvtraczRec_%d", bin); 
      char *named0NegtvtraczSkip = Form("d0negtvtraczSkip_%d", bin); 
      ((TH1F*)(fOutputnegtvTracRec->FindObject(named0NegtvtracrphiRec)))->Fill(10000.*dzRec[0]);
      ((TH1F*)(fOutputnegtvTracRec->FindObject(named0NegtvtraczRec)))->Fill(10000.*dzRec[1]);
      ((TH1F*)(fOutputnegtvTracSkip->FindObject(named0NegtvtracrphiSkip)))->Fill(10000.*dzRecSkip[0]);
      ((TH1F*)(fOutputnegtvTracSkip->FindObject(named0NegtvtraczSkip)))->Fill(10000.*dzRecSkip[1]);
    }
    char *named0Pt = Form("d0pt_%d",bin);
    if(npointsITS==6) ((TH1F*)(fOutputPt->FindObject(named0Pt)))->Fill(pt);

    delete vtxESDSkip; vtxESDSkip = 0x0;
  }  // end loop of tracks
  
  delete diamond; diamond=NULL;
  delete vtxESDRec; vtxESDRec = 0x0;
  fNentries->Fill(1);
  PostData(1, fOutputitspureSARec);
  PostData(2, fOutputitspureSASkip);
  PostData(3, fOutputallPointRec);
  PostData(4, fOutputallPointSkip);
  PostData(5, fOutputpartPointRec);
  PostData(6, fOutputpartPointSkip);
  PostData(7, fOutputonepointSPDRec);
  PostData(8, fOutputonepointSPDSkip);
  PostData(9, fOutputpostvTracRec);
  PostData(10, fOutputpostvTracSkip);
  PostData(11, fOutputnegtvTracRec);
  PostData(12, fOutputnegtvTracSkip);
  PostData(13, fOutputpullAllpointRec);
  PostData(14, fOutputpullAllpointSkip);
  PostData(15, fOutputOnlyRefitRec);
  PostData(16, fOutputOnlyRefitSkip);
  PostData(17, fOutputPt);
  PostData(18, fNentries);
  PostData(19,fEstimVtx);
  
  return;
}

//________________________________________________________________________
Int_t AliAnalysisTaskSEImpParRes::PtBin(Double_t pt) const {
  //
  // return the number of the pt bin
  //

  if (pt>0.22 && pt<0.23) return 1 ;
  if (pt>0.26 && pt<0.27) return 2 ; 
  if (pt>0.35 && pt<0.36) return 3 ;
  if (pt>0.45 && pt<0.46) return 4 ; 
  if (pt>0.55 && pt<0.56) return 5 ;
  if (pt>0.65 && pt<0.66) return 6 ; 
  if (pt>0.75 && pt<0.76) return 7 ; 
  if (pt>0.85 && pt<0.865) return 8 ; 
  if (pt>1.05 && pt<1.07) return 9 ;
  if (pt>1.25 && pt<1.30) return 10; 
  if (pt>1.4 && pt<1.55) return 11; 
  if (pt>1.6 && pt<1.8) return 12; 
  if (pt>1.8 && pt<2.0) return 13; 
  if (pt>2.1 && pt<2.3) return 14; 
  if (pt>2.34 && pt<2.64) return 15; 
  if (pt>2.65 && pt<3.0) return 16; 
  if (pt>3.1 && pt<4.) return 17; 
  if (pt>4.1 && pt<5.2) return 18; 
  if (pt>5.3 && pt<6.8)  return 19; 
  if (pt>7.0 && pt<8.8) return 20; 
  if (pt>9. && pt<30.) return 2;
  if (pt>30.1 && pt<32.) return 22; 
  if (pt>32.5  && pt<33.)  return 23;
  if (pt>33.2  && pt<33.8) return 24; 
  if (pt>33.9  && pt<34.)  return 25;
  if (pt>34.05  && pt<35.)  return 26;

  /*
  if (pt>0.22 && pt<0.23) return 1 ;
  if (pt>0.26 && pt<0.27) return 2 ; 
  if (pt>0.35 && pt<0.36) return 3 ;
  if (pt>0.45 && pt<0.46) return 4 ; 
  if (pt>0.55 && pt<0.56) return 5 ;
  if (pt>0.65 && pt<0.66) return 6 ; 
  if (pt>0.75 && pt<0.76) return 7 ; 
  if (pt>0.85 && pt<0.86) return 8 ; 
  if (pt>1.05 && pt<1.06) return 9 ;
  if (pt>1.25 && pt<1.27) return 10; 
  if (pt>1.45 && pt<1.47) return 11; 
  if (pt>1.65 && pt<1.67) return 12; 
  if (pt>1.85 && pt<1.87) return 13; 
  if (pt>2.15 && pt<2.17) return 14; 
  if (pt>2.45 && pt<2.48) return 15; 
  if (pt>2.65 && pt<2.67) return 16; 
  if (pt>2.85 && pt<2.87) return 17; 
  if (pt>3.25 && pt<3.27) return 18; 
  if (pt>3.75 && pt<3.8)  return 19; 
  if (pt>4.15 && pt<4.20) return 20; 
  if (pt>4.95 && pt<5.15) return 21;
  if (pt>5.35 && pt<5.55) return 22; 
  if (pt>6.0  && pt<6.8)  return 23;
  if (pt>8.5  && pt<10.5) return 24; 
  if (pt>12.  && pt<19.)  return 25;
  if (pt>21.  && pt<32.)  return 26;
  */  
  return -1; 
}

//_________________________________________________________________________
void AliAnalysisTaskSEImpParRes::Terminate(Option_t */*option*/) {
  //
  // Terminate analysis
  //

  if (fDebug>1) printf("AnalysisTaskSEImpParRes: Terminate() \n");

  fOutputitspureSARec = dynamic_cast<TList*>(GetOutputData(1));
  if (!fOutputitspureSARec) printf("ERROR: fOutputitspureSARec not available\n");

  fOutputitspureSASkip = dynamic_cast<TList*>(GetOutputData(2));
  if (!fOutputitspureSASkip) printf("ERROR: fOutputitspureSASkip not available\n");

  fOutputallPointRec = dynamic_cast<TList*>(GetOutputData(3));
  if (!fOutputallPointRec) printf("ERROR: fOutputallPointRec not available\n");

  fOutputallPointSkip = dynamic_cast<TList*>(GetOutputData(4));
  if (!fOutputallPointSkip) printf("ERROR: fOutputallPointSkip not available\n");

  fOutputpartPointRec = dynamic_cast<TList*>(GetOutputData(5));
  if (!fOutputpartPointRec) printf("ERROR: fOutputpartPointRec not available\n");

  fOutputpartPointSkip = dynamic_cast<TList*>(GetOutputData(6));
  if (!fOutputpartPointSkip) printf("ERROR: fOutputpartPointSkip not available\n");

  fOutputonepointSPDRec = dynamic_cast<TList*>(GetOutputData(7));
  if (!fOutputonepointSPDRec) printf("ERROR: fOutputonepointSPDSRec not available\n");

  fOutputonepointSPDSkip = dynamic_cast<TList*>(GetOutputData(8));
  if (!fOutputonepointSPDSkip) printf("ERROR: fOutputonepointSPDSkip not available\n");

  fOutputpostvTracRec = dynamic_cast<TList*>(GetOutputData(9));
  if (!fOutputpostvTracRec) printf("ERROR: fOutputpostvTracRec not available\n");

  fOutputpostvTracSkip = dynamic_cast<TList*>(GetOutputData(10));
  if (!fOutputpostvTracSkip) printf("ERROR: fOutputpostvTracSkip not available\n");

  fOutputnegtvTracRec = dynamic_cast<TList*>(GetOutputData(11));
  if (!fOutputnegtvTracRec) printf("ERROR: fOutputnegtvTracRec not available\n");

  fOutputnegtvTracSkip = dynamic_cast<TList*>(GetOutputData(12));
  if (!fOutputnegtvTracSkip) printf("ERROR: fOutputnegtvTracSkip not available\n");


  fOutputpullAllpointRec = dynamic_cast<TList*>(GetOutputData(13));
  if (!fOutputpullAllpointRec) printf("ERROR: fOutputpullAllpointRec not available\n");

  fOutputpullAllpointSkip = dynamic_cast<TList*>(GetOutputData(14));
  if (!fOutputpullAllpointSkip) printf("ERROR: fOutputpullAllpointSkip not available\n");

  fOutputOnlyRefitRec = dynamic_cast<TList*>(GetOutputData(15));
  if (!fOutputOnlyRefitRec) printf("ERROR: fOutputOnlyRefitRec not available\n");

  fOutputOnlyRefitSkip = dynamic_cast<TList*>(GetOutputData(16));
  if (!fOutputOnlyRefitSkip) printf("ERROR: fOutputOnlyRefitSkip not available\n");

  fOutputPt = dynamic_cast<TList*>(GetOutputData(17));
  if (!fOutputPt) printf("ERROR: fOutputPt not available\n");

  fNentries = dynamic_cast<TH1F*>(GetOutputData(18));
  if (!fNentries) printf("ERROR: fNentries not available\n");
  
  fEstimVtx = dynamic_cast<TH1F*>(GetOutputData(19));
  if (!fEstimVtx) printf("ERROR: fEstimVtx not available\n");
  
  return;
}
