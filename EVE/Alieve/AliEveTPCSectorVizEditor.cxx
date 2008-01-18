// $Id$
// Main authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 * 
 **************************************************************************/

#include "AliEveTPCSectorVizEditor.h"
#include <Alieve/AliEveTPCSectorViz.h>

#include <TEveGValuators.h>
#include <TEveTransEditor.h>

#include <TVirtualPad.h>
#include <TColor.h>

#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>


//______________________________________________________________________
// AliEveTPCSectorVizEditor
//

ClassImp(AliEveTPCSectorVizEditor)

AliEveTPCSectorVizEditor::AliEveTPCSectorVizEditor(const TGWindow *p,
                                       Int_t width, Int_t height,
                                       UInt_t options, Pixel_t back) :
  TGedFrame(p, width, height, options | kVerticalFrame, back),
  fM(0),
  fHMTrans   (0),
  fSectorID  (0), fAutoTrans (0),
  fRnrInn    (0), fRnrOut1   (0), fRnrOut2(0),
  fThreshold (0), fMaxVal    (0),
  fTime      (0)
{
  fPriority = 40;

  Int_t labelW = 60;

  MakeTitle("Transformation matrix");

  fHMTrans = new TEveTransSubEditor(this);
  fHMTrans->Connect("UseTrans()",     "AliEveTPCSectorVizEditor", this, "Update()");
  fHMTrans->Connect("TransChanged()", "AliEveTPCSectorVizEditor", this, "Update()");
  AddFrame(fHMTrans, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 0, 0));

  MakeTitle("AliEveTPCSectorViz");

  fSectorID = new TEveGValuator(this, "SectorID", 110, 0);
  fSectorID->SetLabelWidth(labelW);
  fSectorID->SetShowSlider(kFALSE);
  fSectorID->SetNELength(4);
  fSectorID->Build();
  fSectorID->SetLimits(0, 35);
  fSectorID->SetToolTip("0-17 +z plate; 18-35 -z plate");
  fSectorID->Connect("ValueSet(Double_t)",
		     "AliEveTPCSectorVizEditor", this, "DoSectorID()");
  // Reuse sectorID for auto-transformation button
  fAutoTrans = new TGCheckButton(fSectorID, "AutoTrans");
  fAutoTrans->SetToolTipText("Automatically set transformation to true position");
  fSectorID->AddFrame(fAutoTrans, new TGLayoutHints(kLHintsLeft, 12, 0, 1, 0));
  fAutoTrans->Connect("Toggled(Bool_t)","AliEveTPCSectorVizEditor", this, "DoAutoTrans()");
  AddFrame(fSectorID, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

  {
    TGHorizontalFrame* f = new TGHorizontalFrame(this);

    fRnrInn = new TGCheckButton(f, "Inner");
    f->AddFrame(fRnrInn, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fRnrInn->Connect("Toggled(Bool_t)","AliEveTPCSectorVizEditor", this, "DoRnrInn()");

    fRnrOut1 = new TGCheckButton(f, "Outer 1");
    f->AddFrame(fRnrOut1, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fRnrOut1->Connect("Toggled(Bool_t)","AliEveTPCSectorVizEditor", this, "DoRnrOut1()");

    fRnrOut2 = new TGCheckButton(f, "Outer 2");
    f->AddFrame(fRnrOut2, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fRnrOut2->Connect("Toggled(Bool_t)","AliEveTPCSectorVizEditor", this, "DoRnrOut2()");

    AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
  }

  fThreshold = new TEveGValuator(this, "Threshold", 200, 0);
  fThreshold->SetNELength(4);
  fThreshold->SetLabelWidth(labelW);
  fThreshold->Build();
  fThreshold->GetSlider()->SetWidth(120);
  fThreshold->SetLimits(0,250);
  fThreshold->Connect("ValueSet(Double_t)",
		      "AliEveTPCSectorVizEditor", this, "DoThreshold()");
  AddFrame(fThreshold, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

  fMaxVal = new TEveGValuator(this,"MaxVal", 200, 0);
  fMaxVal->SetNELength(4);
  fMaxVal->SetLabelWidth(labelW);
  fMaxVal->Build();
  fMaxVal->GetSlider()->SetWidth(120);
  fMaxVal->SetLimits(0, 500);
  fMaxVal->Connect("ValueSet(Double_t)",
		   "AliEveTPCSectorVizEditor", this, "DoMaxVal()");
  AddFrame(fMaxVal, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));

  fTime = new TEveGDoubleValuator(this,"Time", 200, 0);
  fTime->SetNELength(4);
  fTime->SetLabelWidth(labelW);
  fTime->Build();
  fTime->GetSlider()->SetWidth(224);
  fTime->SetLimits(0, 1023, TGNumberFormat::kNESInteger);
  fTime->Connect("ValueSet()",
		 "AliEveTPCSectorVizEditor", this, "DoTime()");
  AddFrame(fTime, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));
}

AliEveTPCSectorVizEditor::~AliEveTPCSectorVizEditor()
{}

/**************************************************************************/

void AliEveTPCSectorVizEditor::SetModel(TObject* obj)
{
  fM = dynamic_cast<AliEveTPCSectorViz*>(obj);

  fHMTrans->SetModel(&fM->fHMTrans);

  fSectorID->SetValue(fM->fSectorID);
  fAutoTrans->SetState(fM->fAutoTrans  ? kButtonDown : kButtonUp);

  fRnrInn ->SetState(fM->fRnrInn  ? kButtonDown : kButtonUp);
  fRnrOut1->SetState(fM->fRnrOut1 ? kButtonDown : kButtonUp);
  fRnrOut2->SetState(fM->fRnrOut2 ? kButtonDown : kButtonUp);

  fThreshold->SetValue(fM->fThreshold);
  fMaxVal->SetValue(fM->fMaxVal);

  fTime->SetValues(fM->fMinTime, fM->fMaxTime);
}

/**************************************************************************/

void AliEveTPCSectorVizEditor::DoSectorID()
{
  fM->SetSectorID((Int_t) fSectorID->GetValue());
  Update();
}

void AliEveTPCSectorVizEditor::DoAutoTrans()
{
  fM->SetAutoTrans(fAutoTrans->IsOn());
  Update();
}

/**************************************************************************/

void AliEveTPCSectorVizEditor::DoRnrInn()
{
  fM->SetRnrInn(fRnrInn->IsOn());
  Update();
}

void AliEveTPCSectorVizEditor::DoRnrOut1()
{
  fM->SetRnrOut1(fRnrOut1->IsOn());
  Update();
}

void AliEveTPCSectorVizEditor::DoRnrOut2()
{
  fM->SetRnrOut2(fRnrOut2->IsOn());
  Update();
}

/**************************************************************************/

void AliEveTPCSectorVizEditor::DoThreshold()
{
  fM->SetThreshold((Short_t) fThreshold->GetValue());
  fThreshold->SetValue(fM->fThreshold);
  Update();
}

void AliEveTPCSectorVizEditor::DoMaxVal()
{
  fM->SetMaxVal((Int_t) fMaxVal->GetValue());
  fMaxVal->SetValue(fM->fMaxVal);
  Update();
}

/**************************************************************************/

void AliEveTPCSectorVizEditor::DoTime()
{ 
  fM->SetMinTime((Int_t) fTime->GetMin());
  fM->SetMaxTime((Int_t) fTime->GetMax());
  Update();
}
