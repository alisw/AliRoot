/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Graphical User Interface utility class for the MUON trigger          //
// - digits maps of the trigger chambers                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TCanvas.h>
#include <TGFrame.h>
#include <TGButton.h>
#include <TGTab.h>
#include <TRootEmbeddedCanvas.h>
#include <TBox.h>
#include <TClonesArray.h>
#include <TPave.h>
#include <TPaveText.h>
#include <TStyle.h>
#include <TObjArray.h>

#include "AliRun.h"

#include "AliMUON.h"
#include "AliMUONDigit.h"
#include "AliMpSegmentation.h"
#include "AliMpVSegmentation.h"
#include "AliMUONGeometryTransformer.h"
#include "AliMUONData.h"

#include "AliMUONTriggerGUIboard.h"
#include "AliMUONTriggerGUIdimap.h"

/// \cond CLASSIMP
ClassImp(AliMUONTriggerGUIdimap)
/// \endcond

//__________________________________________________________________________
AliMUONTriggerGUIdimap::AliMUONTriggerGUIdimap(AliLoader *loader, TObjArray *boards, const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h)
  : TObject(),
    fQObject(),
    fMain(0),
    fLoader(0),
    fMUONData(0),
    fBoards(0),
    fIsOn(0)
{
  /// frame constructor

  fLoader = loader;
  fIsOn   = kTRUE;
  fBoards = boards;

  fMUONData = new AliMUONData(loader,"MUON","MUON");
  fMUONData->SetTreeAddress("D");
 
  gStyle->SetPadLeftMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.05);

  gStyle->SetOptDate(0);
  gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);

  fMain = new TGTransientFrame(p, main, w, h, kVerticalFrame);
  fMain->Connect("CloseWindow()", "AliMUONTriggerGUIdimap", this, "DoClose()");
  fMain->DontCallClose(); // to avoid double deletions.
  
  // use hierarchical cleaning
  fMain->SetCleanup(kDeepCleanup);

  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain, 60, 20, kFixedWidth);

  TGTextButton *closeButton = new TGTextButton(hframe, "Close", 1);
  closeButton->Connect("Clicked()", "AliMUONTriggerGUIdimap", this, "DoClose()");

  hframe->AddFrame(closeButton, 
		   new TGLayoutHints(kLHintsTop | 
				     kLHintsLeft,
				     2, 2, 2, 2)
		   );

  hframe->Resize(150, closeButton->GetDefaultHeight());

  TGTextButton *updateButton = new TGTextButton(hframe, "Update", 2);
  updateButton->Connect("Clicked()", "AliMUONTriggerGUIdimap", this, "DoUpdate()");
  updateButton->SetToolTipText("Read the digits for the current event");

  hframe->AddFrame(updateButton, 
		   new TGLayoutHints(kLHintsTop | 
				     kLHintsLeft,
				     2, 2, 2, 2)
		   );

  hframe->Resize(150, updateButton->GetDefaultHeight());
  /*
  TGTextButton *resetButton = new TGTextButton(hframe, "Reset", 3);
  resetButton->Connect("Clicked()", "AliMUONTriggerGUIdimap", this, "DoReset()");
  resetButton->SetToolTipText("Redraw unselected boards");

  hframe->AddFrame(resetButton, 
		   new TGLayoutHints(kLHintsTop | 
				     kLHintsLeft,
				     2, 2, 2, 2)
		   );

  hframe->Resize(150, resetButton->GetDefaultHeight());
  */
  fMain->AddFrame(hframe, 
		  new TGLayoutHints(kLHintsBottom | 
				    kLHintsLeft, 
				    2, 2, 2, 2)
		  );
  
  TGTab *tab = new TGTab(fMain, 300, 300);
  //tab->Connect("Selected(Int_t)", "AliMUONTriggerGUIdimap", this, "DoTab(Int_t)");

  // Tabs for MT11, MT12, MT21, MT22
  TGLayoutHints *lot = new TGLayoutHints(kLHintsTop | 
					 kLHintsLeft | 
					 kLHintsExpandX |
					 kLHintsExpandY, 
					 5, 5, 5, 5);
  
  TGCompositeFrame *tf = 0;

  // Tab for MT11
  tf = tab->AddTab("MT11");

  TGCompositeFrame *cf1 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);
  fEc[0] = new TRootEmbeddedCanvas("ec1", cf1, 500, 500);
  cf1->AddFrame(fEc[0], lot);

  tf->AddFrame(cf1, lot);
  fEc[0]->GetCanvas()->SetBorderMode(0);
  fEc[0]->GetCanvas()->SetBit(kNoContextMenu);
  
  // Tab for MT12
  tf = tab->AddTab("MT12");

  TGCompositeFrame *cf2 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);
  fEc[1] = new TRootEmbeddedCanvas("ec2", cf2, 500, 500);
  cf2->AddFrame(fEc[1], lot);

  tf->AddFrame(cf2, lot);
  fEc[1]->GetCanvas()->SetBorderMode(0);
  fEc[1]->GetCanvas()->SetBit(kNoContextMenu);
  
  // Tab for MT21
  tf = tab->AddTab("MT21");

  TGCompositeFrame *cf3 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);
  fEc[2] = new TRootEmbeddedCanvas("ec3", cf3, 500, 500);
  cf3->AddFrame(fEc[2], lot);

  tf->AddFrame(cf3, lot);
  fEc[2]->GetCanvas()->SetBorderMode(0);
  fEc[2]->GetCanvas()->SetBit(kNoContextMenu);
  
  // Tab for MT22
  tf = tab->AddTab("MT22");

  TGCompositeFrame *cf4 = new TGCompositeFrame(tf, 60, 60, kHorizontalFrame);
  fEc[3] = new TRootEmbeddedCanvas("ec4", cf4, 500, 500);
  cf4->AddFrame(fEc[3], lot);

  tf->AddFrame(cf4, lot);
  fEc[3]->GetCanvas()->SetBorderMode(0);
  fEc[3]->GetCanvas()->SetBit(kNoContextMenu);
  
  fMain->AddFrame(tab, 
		  new TGLayoutHints(kLHintsBottom | 
				    kLHintsExpandX |
				    kLHintsExpandY, 
				    2, 2, 2, 2)
		  );
  
  fMain->MapSubwindows();
  fMain->Resize();
  
  fMain->CenterOnParent();
  
  fMain->SetWindowName("Chambers digit maps");
  
  fMain->MapWindow();

  DrawMaps(11);
  DrawMaps(12);
  DrawMaps(13);
  DrawMaps(14);

}

//__________________________________________________________________________
AliMUONTriggerGUIdimap::~AliMUONTriggerGUIdimap()
{
  /// destructor

  for (Int_t it = 0; it < kNMT; it++) {
    for (Int_t ib = 0; ib < kNBoards; ib++) {
      delete fPaveBoard[it][ib];
    }
  }

  delete fMUONData;
  fMain->DeleteWindow();

}

//__________________________________________________________________________
AliMUONTriggerGUIdimap::AliMUONTriggerGUIdimap(const AliMUONTriggerGUIdimap& dimap)
  : TObject(),
    fQObject(),
    fMain(0),
    fLoader(0),
    fMUONData(0),
    fBoards(0),
    fIsOn(0)
{
  /// copy constructor

  dimap.Dump();
  Fatal("AliMUONTriggerGUIdimap","copy constructor not implemented");

}

//__________________________________________________________________________
AliMUONTriggerGUIdimap & AliMUONTriggerGUIdimap::operator=(const AliMUONTriggerGUIdimap& dimap)
{
  /// asignment operator

  dimap.Dump();
  Fatal("AliMUONTriggerGUIdimap","assignment operator not implemented");

  return *this;

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::DrawMaps(Int_t chamber)
{
  /// draw the digits map for chamber-

  TString mapspath = gSystem->Getenv("ALICE_ROOT");
  mapspath.Append("/MUON/data");

  TCanvas *canvas = fEc[chamber-11]->GetCanvas();
  canvas->cd();
  canvas->Clear();

  AliMUONDigit *mdig;
  AliMpPad      mpad;
  
  AliRunLoader *runLoader = fLoader->GetRunLoader();
  gAlice = runLoader->GetAliRun();
  AliMUON *pMUON = (AliMUON*)gAlice->GetModule("MUON");
  const AliMUONGeometryTransformer* kGeomTransformer = pMUON->GetGeometryTransformer();
  
  TClonesArray *muonDigits = fMUONData->Digits(chamber-1);
  if (muonDigits == 0) { printf("No muonDigits \n"); return; }
  gAlice->ResetDigits();
  fMUONData->GetDigits();
  Int_t nDigits = muonDigits->GetEntriesFast(); 
  
  TPaveText *label;
  TBox *boxd;

  Char_t fntxt[6], name[8], cln[2];
  Int_t detElemId, cathode, ix, iy, charge, color;
  Int_t side, col, line, nbx;
  Int_t holdS, holdL, holdC;
  Float_t xCenter, yCenter, zCenter, xWidth, yWidth, holdXC, holdYC;
  Float_t xMin, xMax, yMin, yMax;
  Float_t ptx1, ptx2, pty1, pty2;
  Float_t xpmin, xpmax, ypmin, ypmax;
  Float_t xg1, xg2, yg1, yg2, zg1;
    
  Float_t frameXmax = 0., frameYmax = 0.;

  switch(chamber) {
  case 11:
    frameXmax = 257.0;
    frameYmax = 306.6;
    break;
  case 12:
    frameXmax = 259.7;
    frameYmax = 309.9;
    break;
  case 13:
    frameXmax = 273.0;
    frameYmax = 325.7;
    break;
  case 14:
    frameXmax = 275.7;
    frameYmax = 329.0;
    break;
  }

  canvas->Range(
		-(1.15*frameXmax),
		-(1.15*frameYmax),
		+(1.15*frameXmax),
		+(1.15*frameYmax)
		);

  // draw the boards
  
  sprintf(fntxt,"%s/guimapp%2d.txt",mapspath.Data(),chamber);
  FILE *ftxt = fopen(fntxt,"r");
  
  for (Int_t ib = 0; ib < kNBoards; ib++) {
    
    fscanf(ftxt,"%d   %d   %d   %d   %f   %f   %f   %f   %f   %s   \n",
	   &side,&col,&line,&nbx,
	   &xCenter,&yCenter,&xWidth,&yWidth,&zCenter,
	   &name[0]);

    //printf("%d   %d   %d   %d   %f   %f   %f   %f   %f   %s   \n",side,col,line,nbx,xCenter,yCenter,xWidth,yWidth,zCenter,name);

    holdS = side;
    holdC = col;
    holdL = line;

    holdXC = xCenter;
    holdYC = yCenter;

    xMin = xCenter - xWidth/2;
    xMax = xCenter + xWidth/2;
    yMin = yCenter - yWidth/2;
    yMax = yCenter + yWidth/2;
    
    fPaveBoard[chamber-11][ib] = new TPave(xMin,yMin,xMax,yMax,1);
    fPaveBoard[chamber-11][ib]->SetBit(kCannotPick);
    fPaveBoard[chamber-11][ib]->SetFillStyle(0);
    fPaveBoard[chamber-11][ib]->Draw();

    Float_t lWidth = 12.0;

    if (holdL == 9) {

      sprintf(cln,"C%1d",holdC);

      ptx1 = holdXC - lWidth;
      ptx2 = holdXC + lWidth;
      pty1 = +1.065*frameYmax - lWidth;
      pty2 = +1.065*frameYmax + lWidth;

      label = new TPaveText(ptx1,pty1,ptx2,pty2,cln);
      label->AddText(cln);
      label->SetBorderSize(0);
      label->SetBit(kCannotPick);
      label->Draw();
    }
    if (holdL == 1) {

      sprintf(cln,"C%1d",holdC);

      ptx1 = holdXC - lWidth;
      ptx2 = holdXC + lWidth;
      pty1 = -1.065*frameYmax - lWidth;
      pty2 = -1.065*frameYmax + lWidth;

      label = new TPaveText(ptx1,pty1,ptx2,pty2,cln);
      label->AddText(cln);
      label->SetBorderSize(0);
      label->SetBit(kCannotPick);
      label->Draw();
    }
    if (holdS == 0 && holdC == 7) {

      sprintf(cln,"L%1d",holdL);

      ptx1 = -1.07*frameXmax - lWidth;
      ptx2 = -1.07*frameXmax + lWidth;
      pty1 = holdYC - lWidth;
      pty2 = holdYC + lWidth;

      label = new TPaveText(ptx1,pty1,ptx2,pty2,cln);
      label->AddText(cln);
      label->SetBorderSize(0);
      label->SetBit(kCannotPick);
      label->Draw();
    }
    if (holdS == 1 && holdC == 7) {

      sprintf(cln,"L%1d",holdL);

      ptx1 = +1.07*frameXmax - lWidth;
      ptx2 = +1.07*frameXmax + lWidth;
      pty1 = holdYC - lWidth;
      pty2 = holdYC + lWidth;

      label = new TPaveText(ptx1,pty1,ptx2,pty2,cln);
      label->AddText(cln);
      label->SetBorderSize(0);
      label->SetBit(kCannotPick);
      label->Draw();
   }
    
  }

  fclose(ftxt);

  // draw digits set from the board GUI

  AliMUONTriggerGUIboard *board;
  Int_t imt = chamber -11;
  Int_t nStripX, nStripY;
  TBox *box;
  for (Int_t ib = 0; ib < kNBoards; ib++) {

    board = (AliMUONTriggerGUIboard*)fBoards->At(ib);

    xCenter = board->GetXCenter(imt);
    yCenter = board->GetYCenter(imt);

    nStripX = board->GetXSiy2() - board->GetXSiy1() + 1;
    nStripY = board->GetYSix2() - board->GetYSix1() + 1 ;

    for (Int_t is = 0; is < nStripX; is++) {

      if (board->GetXDig(imt,is) == 0) continue;

      box = board->GetXDigBox(imt,is);

      xpmin = box->GetX1();
      xpmax = box->GetX2();
      ypmin = box->GetY1();
      ypmax = box->GetY2();

      xpmin += xCenter;
      xpmax += xCenter;
      ypmin += yCenter;
      ypmax += yCenter;

      box->DrawBox(xpmin,ypmin,xpmax,ypmax);

    }

    for (Int_t is = 0; is < nStripY; is++) {

      if (board->GetYDig(imt,is) == 0) continue;

      box = board->GetYDigBox(imt,is);

      xpmin = box->GetX1();
      xpmax = box->GetX2();
      ypmin = box->GetY1();
      ypmax = box->GetY2();

      xpmin += xCenter;
      xpmax += xCenter;
      ypmin += yCenter;
      ypmax += yCenter;

      box->DrawBox(xpmin,ypmin,xpmax,ypmax);

    }

  }
  
  // draw the digits

  for (Int_t id = 0; id < nDigits; id++) {
    
    mdig  = (AliMUONDigit*)muonDigits->UncheckedAt(id);
    
    cathode = mdig->Cathode()+1;
    
    ix=mdig->PadX();
    iy=mdig->PadY();
    detElemId=mdig->DetElemId();      
    charge = (Int_t)mdig->Signal();
    color  = 261+5*(charge-1);
    if (color > 282) color = 282;
    
    const AliMpVSegmentation* seg2 = AliMpSegmentation::Instance()->GetMpSegmentation(detElemId,AliMp::GetCathodType(cathode-1));
    
    mpad = seg2->PadByIndices(AliMpIntPair(ix,iy),kTRUE);
    
    // get the pad position and dimensions
    Float_t xlocal1 = mpad.Position().X();
    Float_t ylocal1 = mpad.Position().Y();
    Float_t xlocal2 = mpad.Dimensions().X();
    Float_t ylocal2 = mpad.Dimensions().Y();
    
    kGeomTransformer->Local2Global(detElemId, xlocal1, ylocal1, 0, xg1, yg1, zg1);
    // (no transformation for pad dimensions)
    xg2 = xlocal2;
    yg2 = ylocal2;

    // transform in the monitor coordinate system
    //xpmin = -(xg1+xg2);
    //xpmax = -(xg1-xg2);
    //ypmin = -(yg2-yg1);
    //ypmax = +(yg2+yg1);
    // ALICE SC
    xpmin = +(xg1-xg2);
    xpmax = +(xg1+xg2);
    ypmin = -(yg2-yg1);
    ypmax = +(yg2+yg1);
	      
    boxd = new TBox(xpmin,ypmin,xpmax,ypmax);
    boxd->SetBit(kCannotPick);
    boxd->SetFillStyle(1001);
    boxd->SetFillColor(2);
    boxd->Draw();
    
  }  // end digits loop

  canvas->Modified();
  canvas->Update();

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::DoTab(Int_t id) const
{
  /// some action when chamber tab is selected...

  switch (id) {
  case 0:
    break;
  case 1:
    break;
  case 2:
    break;
  case 3:
    break;
  default:
    break;
  }

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::DoClose()
{
  /// close the main frame

  fIsOn = kFALSE;
  CloseWindow();

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::DoUpdate()
{
  /// update maps for another run/event

  //fMUONData->SetLoader(fLoader);
  fMUONData = new AliMUONData(fLoader,"MUON","MUON");
  fMUONData->SetTreeAddress("D");

  for (Int_t it = 0; it < kNMT; it++) {
    for (Int_t ib = 0; ib < kNBoards; ib++) {
      delete fPaveBoard[it][ib];
    }
  }

  DrawMaps(11);
  DrawMaps(12);
  DrawMaps(13);
  DrawMaps(14);

  AliMUONTriggerGUIboard *board;
  for (Int_t ib = 0; ib < kNBoards; ib++) {
    board = (AliMUONTriggerGUIboard*)fBoards->At(ib);
    if (board->IsOpen()) SelectBoard(ib);
  }

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::DoReset()
{
  /// draw unselected boards

  for (Int_t it = 0; it < kNMT; it++) {

    TCanvas *canvas = fEc[it]->GetCanvas();
    canvas->cd();
    
    for (Int_t ib = 0; ib < kNBoards; ib++) { 
      fPaveBoard[it][ib]->SetFillStyle(0);
    }

    canvas->Modified();
    canvas->Update();

  }

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::SelectBoard(Int_t id)
{
  /// highlight on the map the open board

  for (Int_t it = 0; it < kNMT; it++) {

    TCanvas *canvas = fEc[it]->GetCanvas();
    canvas->cd();
    
    fPaveBoard[it][id]->SetFillStyle(4020);
    fPaveBoard[it][id]->SetFillColor(7);

    canvas->Modified();
    canvas->Update();

  }

}

//__________________________________________________________________________
void AliMUONTriggerGUIdimap::CloseWindow()
{
  /// called when window is closed via the window manager.

  fMain->CloseWindow();

}


