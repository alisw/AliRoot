/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

///////////////////////////////////////////////////////////////////////////
//
//  implementation of a GUI for the event/trigger selection
//
//  origin: Mikolaj Krzewicki, mikolaj.krzewicki@cern.ch
//
//////////////////////////////////////////////////////////////////////////

#include "AliEveEventSelectorWindow.h"
#include "AliEveEventSelector.h"
#include "AliEveEventManager.h"
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TTree.h>
#include <AliESDEvent.h>
#include <AliESDRun.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TROOT.h>

ClassImp(AliEveEventSelectorWindow)
//______________________________________________________________________________
AliEveEventSelectorWindow::AliEveEventSelectorWindow(const TGWindow *p, UInt_t w, UInt_t h,
                                                     AliEveEventSelector* sel) :
    TGMainFrame(p, w, h),
    fPSelector(sel),
    fPCanvas(NULL),
    fPDrawFormula(NULL),
    fPEntryFormula(NULL),
    fPEntryLowerBound(NULL),
    fPEntryHigherBound(NULL),
    fPButtonTextDone(NULL),
    fPComboBoxTrigger(NULL),
    fPEntryTriggerSelection(NULL),
    fPCheckTriggerSimple(NULL),
    fPCheckTriggerString(NULL)
{
  //ctor

  //the canvas
  fPCanvas = new TRootEmbeddedCanvas(0,this);
  Int_t wid = fPCanvas->GetCanvasWindowId();
  TCanvas *myc = new TCanvas("MyCanvas", 10,10,wid);
  fPCanvas->AdoptCanvas(myc);
  AddFrame(fPCanvas, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

  //draw box frame
  TGHorizontalFrame* hframedraw = new TGHorizontalFrame(this);
  AddFrame(hframedraw, new TGLayoutHints((kLHintsExpandX), 5,5));

  TGLabel* drawlabel = new TGLabel(hframedraw, "Draw expression:");
  hframedraw->AddFrame(drawlabel, new TGLayoutHints((kLHintsLeft)));
  fPDrawFormula = new TGTextEntry(hframedraw);
  hframedraw->AddFrame(fPDrawFormula, new TGLayoutHints(kLHintsExpandX));
  TGTextButton* buttondraw = new TGTextButton(hframedraw,"Draw");
  buttondraw->Connect("Clicked()","AliEveEventSelectorWindow",
                              this, "DoDrawHistogram()");
  hframedraw->AddFrame(buttondraw, new TGLayoutHints(kLHintsLeft));
  TGTextButton* buttonview = new TGTextButton(hframedraw,"Browse");
  buttonview->Connect("Clicked()","TTree",fPSelector->GetESDTree(),"StartViewer()");
  hframedraw->AddFrame(buttonview, new TGLayoutHints(kLHintsLeft));


  //text selection frame
  TGHorizontalFrame *hframetxtsel = new TGHorizontalFrame(this);
  AddFrame(hframetxtsel, new TGLayoutHints((kLHintsExpandX), 5,5));

  TGLabel* exprlabel = new TGLabel(hframetxtsel, "Expression:");
  hframetxtsel->AddFrame(exprlabel, new TGLayoutHints(kLHintsLeft));

  TGCheckButton* checktextsel = new TGCheckButton(hframetxtsel);
  checktextsel->Connect("Toggled(Bool_t)", "AliEveEventSelector",
                        fPSelector, "SetSelectOnString(Bool_t)");
  checktextsel->SetState(fPSelector->GetSelectOnString() ? kButtonDown : kButtonUp);
  hframetxtsel->AddFrame(checktextsel, new TGLayoutHints(kLHintsLeft));

  fPEntryLowerBound = new TGNumberEntry(hframetxtsel);
  hframetxtsel->AddFrame(fPEntryLowerBound, new TGLayoutHints(kLHintsLeft));
  fPEntryFormula = new TGTextEntry(hframetxtsel);
  hframetxtsel->AddFrame(fPEntryFormula, new TGLayoutHints(kLHintsExpandX));
  fPEntryHigherBound = new TGNumberEntry(hframetxtsel);
  hframetxtsel->AddFrame(fPEntryHigherBound, new TGLayoutHints(kLHintsLeft));
  fPButtonTextDone = new TGTextButton(hframetxtsel,"Set");
  fPButtonTextDone->Connect("Clicked()", "AliEveEventSelectorWindow",
                            this, "DoSetSelectionString()");
  hframetxtsel->AddFrame(fPButtonTextDone, new TGLayoutHints(kLHintsLeft));

  //trigger cuts frame
  TGHorizontalFrame* hframetrigger = new TGHorizontalFrame(this);
  AddFrame(hframetrigger, new TGLayoutHints((kLHintsLeft), 5,5));

  TGLabel* triglabel = new TGLabel(hframetrigger, "Trigger type:");
  hframetrigger->AddFrame(triglabel, new TGLayoutHints(kLHintsLeft));
  fPCheckTriggerSimple = new TGCheckButton(hframetrigger);
  fPCheckTriggerSimple->Connect("Toggled(Bool_t)","AliEveEventSelector",
                          fPSelector,"SetSelectOnTriggerType(Bool_t)");
  fPCheckTriggerSimple->SetState(fPSelector->GetSelectOnTriggerType() ? kButtonDown : kButtonUp );
  hframetrigger->AddFrame(fPCheckTriggerSimple, new TGLayoutHints(kLHintsLeft));
  fPComboBoxTrigger = new TGComboBox(hframetrigger);
  fPComboBoxTrigger->Resize(100,20);
  fPComboBoxTrigger->Connect("Selected(const char*)","AliEveEventSelectorWindow",
                             this,"DoHandleTriggerFromComboBox(const char*)");
  hframetrigger->AddFrame(fPComboBoxTrigger, new TGLayoutHints(kLHintsLeft));

  TGLabel* labelfreetrig = new TGLabel(hframetrigger, "Trigger type expression:");
  hframetrigger->AddFrame(labelfreetrig, new TGLayoutHints(kLHintsLeft));
  fPCheckTriggerString = new TGCheckButton(hframetrigger);
  hframetrigger->AddFrame(fPCheckTriggerString, new TGLayoutHints(kLHintsLeft));
  fPCheckTriggerString->Connect("Toggled(Bool_t)","AliEveEventSelector",
                           fPSelector,"SetSelectOnTriggerString(Bool_t)");
  fPCheckTriggerString->SetState(fPSelector->GetSelectOnTriggerString() ? kButtonDown : kButtonUp );
  fPEntryTriggerSelection = new TGTextEntry(hframetrigger);
  hframetrigger->AddFrame(fPEntryTriggerSelection, new TGLayoutHints(kLHintsExpandX));
  TGTextButton* buttontrigsel = new TGTextButton(hframetrigger,"Set");
  hframetrigger->AddFrame(buttontrigsel, new TGLayoutHints(kLHintsLeft));
  buttontrigsel->Connect("Clicked()","AliEveEventSelectorWindow",
                         this, "DoSetTriggerSelectionString()");

  //multiplicity cuts frame
  TGHorizontalFrame* hframemult = new TGHorizontalFrame(this);
  AddFrame(hframemult, new TGLayoutHints((kLHintsLeft), 5,5));

  TGLabel* multlabel = new TGLabel(hframemult, "Multiplicity:");
  hframemult->AddFrame(multlabel, new TGLayoutHints(kLHintsLeft));
  TGCheckButton* checkmult = new TGCheckButton(hframemult);
  checkmult->Connect("Toggled(Bool_t)", "AliEveEventSelector",
                      fPSelector,"SetSelectOnMultiplicity(Bool_t)");
  checkmult->SetState(fPSelector->GetSelectOnMultiplicity() ? kButtonDown : kButtonUp );
  hframemult->AddFrame(checkmult, new TGLayoutHints(kLHintsLeft));
  fPEntryMultLow = new TGNumberEntry(hframemult);
  hframemult->AddFrame(fPEntryMultLow, new TGLayoutHints(kLHintsLeft));
  fPEntryMultHigh = new TGNumberEntry(hframemult);
  hframemult->AddFrame(fPEntryMultHigh, new TGLayoutHints(kLHintsLeft));
  TGTextButton* buttonmultset = new TGTextButton(hframemult,"Set");
  hframemult->AddFrame(buttonmultset, new TGLayoutHints(kLHintsLeft));
  buttonmultset->Connect("Clicked()","AliEveEventSelectorWindow",
                         this, "DoSetMultiplicityRange()");

  //wrap around
  TGHorizontalFrame* hframewrap = new TGHorizontalFrame(this);
  AddFrame(hframewrap, new TGLayoutHints((kLHintsLeft), 5,5));

  TGLabel* wraplabel = new TGLabel(hframewrap, "Wrap around at the end.");
  hframewrap->AddFrame(wraplabel, new TGLayoutHints(kLHintsLeft));
  TGCheckButton* checkwraparound = new TGCheckButton(hframewrap);
  hframewrap->AddFrame(checkwraparound, new TGLayoutHints(kLHintsLeft));
  checkwraparound->Connect("Toggled(Bool_t)","AliEveEventSelector",
                           fPSelector, "SetWrapAround(Bool_t)");
  checkwraparound->SetState(fPSelector->GetWrapAround() ? kButtonDown : kButtonUp );

  SetupTriggerSelect();
  
  //bookkeeping
  SetWindowName("Event Selection");
  MapSubwindows();

  Resize(GetDefaultSize());
  MapWindow();
}

//______________________________________________________________________________
AliEveEventSelectorWindow::~AliEveEventSelectorWindow()
{
  //dtor
}

//______________________________________________________________________________
void AliEveEventSelectorWindow::SetupTriggerSelect()
{ 
  //Set up the list of available triggers

  // Do nothing if already enabled.
  if (fPComboBoxTrigger->GetNumberOfEntries() > 0)
    return;
  AliEveEventManager* fM = fPSelector->GetEventManager();
  AliESDEvent* esd = fM->GetESD();
    
  if (esd && fM->GetESDFile() != 0)
  { 
    TString activetrg = esd->GetESDRun()->GetActiveTriggerClasses();  //Get list of active classes
    TObjArray* activetrgarr = activetrg.Tokenize(" "); //break up the classes string, space as separator
    Int_t entries = activetrgarr->GetEntries();  //how many triggerclasses
    TString entry;  //to hold the triger class name
    TObjString* entryobj;
    if (entries == 0)
    {
      fPComboBoxTrigger->SetEnabled(kFALSE);  //no trigger classes
    }
    else
    {
      fPComboBoxTrigger->RemoveAll(); //some initial cleanup
      fPComboBoxTrigger->SetEnabled(kTRUE);  //no trigger classes
      //fPComboBoxTrigger->AddEntry("",-1);  //first entry empty - select to not filter by trigger
      for (Int_t i=0;i<entries;i++)
      {
        entryobj = (TObjString*)activetrgarr->At(i);
        entry = entryobj->GetString();
        fPComboBoxTrigger->AddEntry(entry.Data(), i);
      }
    }
    fPComboBoxTrigger->Select(0, kTRUE); //set default no filtering and emit
    fPComboBoxTrigger->SetEnabled(kTRUE);
  } 
  else
  { 
    fPComboBoxTrigger->SetEnabled(kFALSE);
  } 
} 

//______________________________________________________________________________
void AliEveEventSelectorWindow::DoSetSelectionString()
{
  //process the entered formula and send it to the selector

  TString str = fPEntryFormula->GetText();
  TString s;
  if (fPEntryLowerBound->GetNumber()==0&&fPEntryHigherBound->GetNumber()==0)
  {
    s = str;
  }
  else
  {
    s += "(";
    s += str;
    s += ")>";
    s += fPEntryLowerBound->GetNumber();
    s += "&&";
    s += "(";
    s += str;
    s += ")<";
    s += fPEntryHigherBound->GetNumber();
  }
  fPSelector->SetSelectionString(s);
}

//______________________________________________________________________________
void AliEveEventSelectorWindow::DoDrawHistogram()
{
  //Draw histogram with formula in the draw field

  TTree* tree = fPSelector->GetESDTree();
  TString str = fPDrawFormula->GetText();
  str += ">>selectionhist";
  tree->Draw(str);
  TH1* hist = dynamic_cast<TH1*>(gDirectory->Get("selectionhist"));
  if (hist) hist->Draw();
  TCanvas* canvas = fPCanvas->GetCanvas();
  canvas->Update();
}

//______________________________________________________________________________
void AliEveEventSelectorWindow::DoSetTriggerSelectionString()
{
  //Set trigger selection formula
  
  TString string = fPEntryTriggerSelection->GetText();
  fPSelector->SetTriggerSelectionString(string);
  fPEntryTriggerSelection->SetToolTipText(fPSelector->GetTriggerSelectionString());
}

//______________________________________________________________________________
void AliEveEventSelectorWindow::DoHandleTriggerFromComboBox(const char* str)
{
  //Dispatch the trigger selection to proper destination
  
  if (fPSelector->GetSelectOnTriggerString() && !fPSelector->GetSelectOnTriggerType())
  {
    fPEntryTriggerSelection->Insert(str);
    fPEntryTriggerSelection->SetFocus();
  }
  if (fPSelector->GetSelectOnTriggerType())
    fPSelector->SetTriggerType(str);
}

//______________________________________________________________________________
void AliEveEventSelectorWindow::DoSetMultiplicityRange()
{
  //Set the multiplicity range
  
  fPSelector->SetMultiplicityLow(fPEntryMultLow->GetNumber());
  fPSelector->SetMultiplicityHigh(fPEntryMultHigh->GetNumber());
}
