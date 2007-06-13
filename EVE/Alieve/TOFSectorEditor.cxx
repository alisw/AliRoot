// $Header$

#include "TOFSectorEditor.h"
#include <Alieve/TOFSector.h>

#include <TVirtualPad.h>
#include <TColor.h>
#include <Reve/RGValuators.h>

#include <TGLabel.h>
#include <TGButton.h>
#include <TGNumberEntry.h>
#include <TGColorSelect.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// TOFSectorEditor
//

ClassImp(TOFSectorEditor)
  
  TOFSectorEditor::TOFSectorEditor(const TGWindow *p, Int_t width, Int_t height,
				   UInt_t options, Pixel_t back) :
    TGedFrame(p, width, height, options | kVerticalFrame, back),
    fM(0) ,
    //fHMTrans   (0),
    fSectorID  (0), fAutoTrans (0),
    fPlate0(0x0), fPlate1(0x0), fPlate2(0x0), fPlate3(0x0), fPlate4(0x0),
    fThreshold (0), fMaxVal    (0)
    // fTime      (0)
    // Initialize widget pointers to 0
    //{
    
    /*
      fHMTrans = new ZTransSubEditor(this);
      fHMTrans->Connect("UseTrans()",     "Alieve::TPCSectorVizEditor", this, "Update()");
      fHMTrans->Connect("TransChanged()", "Alieve::TPCSectorVizEditor", this, "Update()");
      AddFrame(fHMTrans, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 0, 0));
      
      MakeTitle("TPCSectorViz");*/
{
  
  fPlate = new TGCheckButton*[5];
  for (Int_t ii=0; ii<5; ii++) fPlate[ii] = new TGCheckButton;
  
  //fPriority = 40;
  MakeTitle("TOFSector");  

  fSectorID = new RGValuator(this, "SectorID", 110, 0);
  fSectorID->SetLabelWidth(60);
  fSectorID->SetShowSlider(kFALSE);
  fSectorID->SetNELength(4);
  fSectorID->Build();
  fSectorID->SetLimits(0, 17);
  fSectorID->SetToolTip("The 18 Tof Sector's");
  fSectorID->Connect("ValueSet(Double_t)",
		     "Alieve::TOFSectorEditor", this, "DoSectorID()");
  // Reuse sectorID for auto-transformation button
  fAutoTrans = new TGCheckButton(fSectorID, "AutoTrans");
  fAutoTrans->SetToolTipText("Automatically set transformation to true position");
  fSectorID->AddFrame(fAutoTrans, new TGLayoutHints(kLHintsLeft, 12, 0, 1, 0));
  fAutoTrans->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoAutoTrans()");
  AddFrame(fSectorID, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));
  
  // Create widgets
  // fXYZZ = new TGSomeWidget(this, ...);
  // AddFrame(fXYZZ, new TGLayoutHints(...));
  // fXYZZ->Connect("SignalName()", "Alieve::TOFSectorEditor", this, "DoXYZZ()"); {
    TGHorizontalFrame* f = new TGHorizontalFrame(this);

    Int_t nPlate = 0;
    fPlate0 = new TGCheckButton(f, "Plate0");
    f->AddFrame(fPlate0, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fPlate0->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate0()");
    //fPlate0->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate(Int_t)");

    nPlate = 1;
    fPlate1 = new TGCheckButton(f, "Plate 1");
    f->AddFrame(fPlate1, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fPlate1->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate1()");
    //fPlate1->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate(Int_t)");

    nPlate = 2;
    fPlate2 = new TGCheckButton(f, "Plate 2");
    f->AddFrame(fPlate2, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fPlate2->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate2()");
    //fPlate2->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate(Int_t)");

    nPlate = 3;
    fPlate3 = new TGCheckButton(f, "Plate 3");
    f->AddFrame(fPlate3, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fPlate3->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate3()");
    //fPlate3->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate(Int_t)");

    nPlate = 4;
    fPlate4 = new TGCheckButton(f, "Plate 4");
    f->AddFrame(fPlate4, new TGLayoutHints(kLHintsTop, 3, 1, 1, 0));
    fPlate4->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate4()");
    //fPlate4->Connect("Toggled(Bool_t)","Alieve::TOFSectorEditor", this, "DoPlate(Int_t)");


    fPlate[0] = fPlate0;
    fPlate[1] = fPlate1;
    fPlate[2] = fPlate2;
    fPlate[3] = fPlate3;
    fPlate[4] = fPlate4;

    AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 1));

    fThreshold = new RGValuator(this, "Threshold", 200, 0);
    fThreshold->SetNELength(4);
    fThreshold->SetLabelWidth(60);
    fThreshold->Build();
    fThreshold->GetSlider()->SetWidth(120);
    fThreshold->SetLimits(0,250);
    fThreshold->Connect("ValueSet(Double_t)",
			"Alieve::TOFSectorEditor", this, "DoThreshold()");
    AddFrame(fThreshold, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));
    
    fMaxVal = new RGValuator(this,"MaxVal", 200, 0);
    fMaxVal->SetNELength(4);
    fMaxVal->SetLabelWidth(60);
    fMaxVal->Build();
    fMaxVal->GetSlider()->SetWidth(60);
    fMaxVal->SetLimits(0, 500);
    fMaxVal->Connect("ValueSet(Double_t)",
		     "Alieve::TOFSectorEditor", this, "DoMaxVal()");
    AddFrame(fMaxVal, new TGLayoutHints(kLHintsTop, 1, 1, 2, 1));
    
}




TOFSectorEditor::~TOFSectorEditor()
{}

/**************************************************************************/

void TOFSectorEditor::SetModel(TObject* obj)
{
  fM = dynamic_cast<TOFSector*>(obj);

  fSectorID->SetValue(fM->GetSectorID());
  fAutoTrans->SetState(fM->GetAutoTrans()  ? kButtonDown : kButtonUp);

  fPlate0->SetState(fM->GetPlate(0) ? kButtonDown : kButtonUp);
  fPlate1->SetState(fM->GetPlate(1) ? kButtonDown : kButtonUp);
  fPlate2->SetState(fM->GetPlate(2) ? kButtonDown : kButtonUp);
  fPlate3->SetState(fM->GetPlate(3) ? kButtonDown : kButtonUp);
  fPlate4->SetState(fM->GetPlate(4) ? kButtonDown : kButtonUp);

  // Set values of widgets
  // fXYZZ->SetValue(fM->GetXYZZ());
}

/**************************************************************************/
void TOFSectorEditor::DoSectorID()
{
  fM->SetSectorID((Int_t) fSectorID->GetValue());
  Update();
}

void TOFSectorEditor::DoAutoTrans()
{
  fM->SetAutoTrans(fAutoTrans->IsOn());
  Update();
}

/**************************************************************************/

void TOFSectorEditor::DoPlate(Int_t nPlate)
{
  fM->SetPlate(nPlate, fPlate[nPlate]->IsOn());
  Update();
}

void TOFSectorEditor::DoPlate0()
{
  fM->SetPlate(0, fPlate0->IsOn());
  Update();
}

void TOFSectorEditor::DoPlate1()
{
  fM->SetPlate(1, fPlate1->IsOn());
  Update();
}

void TOFSectorEditor::DoPlate2()
{
  fM->SetPlate(2, fPlate2->IsOn());
  Update();
}
void TOFSectorEditor::DoPlate3()
{
  fM->SetPlate(3, fPlate3->IsOn());
  Update();
}

void TOFSectorEditor::DoPlate4()
{
  fM->SetPlate(4, fPlate4->IsOn());
  Update();
}


void TOFSectorEditor::DoThreshold()
{
  fM->SetThreshold((Short_t) fThreshold->GetValue());
  fThreshold->SetValue(fM->GetThreshold());
  Update();
}

void TOFSectorEditor::DoMaxVal()
{
  fM->SetMaxVal((Int_t) fMaxVal->GetValue());
  fMaxVal->SetValue(fM->GetMaxVal());
  Update();
}

/**************************************************************************/
/*
void TOFSectorEditor::DoTime()
{ 
  fM->SetMinTime((Int_t) fTime->GetMin());
  fM->SetMaxTime((Int_t) fTime->GetMax());
  Update();
}
*/

/**************************************************************************/

















// Implements callback/slot methods

// void TOFSectorEditor::DoXYZZ()
// {
//   fM->SetXYZZ(fXYZZ->GetValue());
//   Update();
// }
