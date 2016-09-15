// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/**************************************************************************
 * Copyright(c) 1998-2008, ALICE Experiment at CERN, all rights reserved. *
 * See http://aliceinfo.cern.ch/Offline/AliRoot/License.html for          *
 * full copyright notice.                                                 *
 **************************************************************************/

#include "AliEveEventManagerEditor.h"
#include "AliEveDataSource.h"
#include "AliEveDataSourceOffline.h"
#include "AliEveGeomGentle.h"
#include "AliEveInit.h"
#ifdef ZMQ
#include "AliStorageAdministratorPanelListEvents.h"
#include "AliStorageAdministratorPanelMarkEvent.h"
#endif

#include <AliESDEvent.h>

#include <TEveProjectionAxes.h>
#include <TVirtualPad.h>
#include <TColor.h>
#include <TEveGValuators.h>
#include <TGButton.h>
#include <TGTextView.h>
#include <TGLabel.h>
#include <TSystem.h>
#include <TTimeStamp.h>
#include <TSystemDirectory.h>
#include <TRegexp.h>
#include <TEnv.h>

#include <AliRawReader.h>
#include <AliDAQ.h>
#include <AliRawEventHeaderBase.h>

#include <vector>
#include <string>
#include "Riostream.h"

//______________________________________________________________________________
// GUI editor for AliEveEventManager.
//

using namespace std;

ClassImp(AliEveEventManagerEditor)

//______________________________________________________________________________
AliEveEventManagerEditor::AliEveEventManagerEditor(const TGWindow *p, Int_t width, Int_t height,
                                                   UInt_t options, Pixel_t back) :
TGedFrame(p, width, height, options | kVerticalFrame, back),
fM(0),
fDumpEventInfo(0),
fEventInfo(0)
{
    // Constructor.
    
    MakeTitle("AliEveEventManager");
    
    {
        TGHorizontalFrame* f = new TGHorizontalFrame(this);
        fDumpEventInfo = new TGTextButton(f, "Dump Event Info");
        fDumpEventInfo->SetToolTipText("Append information about current event to event_info.txt file.");
        fDumpEventInfo->SetWidth(120);
        fDumpEventInfo->ChangeOptions(fDumpEventInfo->GetOptions() | kFixedWidth);
        f->AddFrame(fDumpEventInfo, new TGLayoutHints(kLHintsNormal, 4,0,0,0));
        fDumpEventInfo->Connect("Clicked()",
                                "AliEveEventManagerEditor", this, "DumpEventInfo()");
        AddFrame(f, new TGLayoutHints(kLHintsExpandX, 8,8,8,0));
    }
    {
        TGVerticalFrame* f = new TGVerticalFrame(this);
        
        TGLabel *eventInfoLabel = new TGLabel(f, "Event Information:");
        f->AddFrame(eventInfoLabel, new TGLayoutHints(kLHintsNormal, 0,0,6,2));
        
        fEventInfo = new TGTextView(f, 200, 300);
        f->AddFrame(fEventInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX));
        
        AddFrame(f, new TGLayoutHints(kLHintsNormal | kLHintsExpandX));
    }
}

/******************************************************************************/

//______________________________________________________________________________
void AliEveEventManagerEditor::SetModel(TObject* obj)
{
    // Set model object.
    
    fM = static_cast<AliEveEventManager*>(obj);
    
    fEventInfo->LoadBuffer(GetEventInfoVertical());
}

/******************************************************************************/

//______________________________________________________________________________
void AliEveEventManagerEditor::DumpEventInfo()
{
    // Dump event-info into event_info.txt.
    // The info is appended into the file.
    
    ofstream f("event_info.txt", ios::out | ios::app);
    
    f << "================================================================================\n\n";
    f << GetEventInfoHorizontal() << std::endl << std::endl;
    
    f.close();
}


//==============================================================================
// AliEveEventManagerWindow
//==============================================================================

//______________________________________________________________________________
//
// Horizontal GUI for AliEveEventManager, to be placed in the
// bottom part of ROOT browser.

ClassImp(AliEveEventManagerWindow)

AliEveEventManagerWindow* AliEveEventManagerWindow::fInstance = nullptr;

AliEveEventManagerWindow::AliEveEventManagerWindow() :
TGMainFrame(gClient->GetRoot(), 400, 100, kVerticalFrame),
fM            (AliEveEventManager::Instance()),
fFirstEvent   (0),
fPrevEvent    (0),
fNextEvent    (0),
fLastEvent    (0),
fRefresh      (0),
fMarkEvent    (0),
fScreenshot   (0),
fSwitchToHLT  (0),
fSwitchToOnline (0),
fSwitchToOffline (0),
fEventId      (0),
fInfoLabel    (0),
fAutoLoad     (0),
fAutoLoadTime (0),
fTrigSel      (0),
fEventInfo    (0)
{
    // Constructor.
    
    fInstance = this;
    
    const TString cls("AliEveEventManagerWindow");
    TGTextButton *b = 0;
    {
        Int_t width = 50;
        
        TGHorizontalFrame* f = new TGHorizontalFrame(this);
        AddFrame(f, new TGLayoutHints(kLHintsExpandX, 0,0,2,2));
        
        fFirstEvent = b = MkTxtButton(f, "First", width);
        b->Connect("Clicked()", cls, this, "DoFirstEvent()");
        fPrevEvent = b = MkTxtButton(f, "Prev", width);
        b->Connect("Clicked()", cls, this, "DoPrevEvent()");
        
        fEventId = new TGNumberEntry(f, 0, 5, -1,TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                     TGNumberFormat::kNELLimitMinMax, 0, 10000);
        f->AddFrame(fEventId, new TGLayoutHints(kLHintsNormal, 10, 5, 0, 0));
        fEventId->Connect("ValueSet(Long_t)", cls, this, "DoSetEvent()");
        fInfoLabel = new TGLabel(f);
        f->AddFrame(fInfoLabel, new TGLayoutHints(kLHintsNormal, 5, 10, 4, 0));
        
        fNextEvent = b = MkTxtButton(f, "Next", width);
        b->Connect("Clicked()", cls, this, "DoNextEvent()");
        fLastEvent = b = MkTxtButton(f, "Last", width);
        b->Connect("Clicked()", cls, this, "DoLastEvent()");
        fMarkEvent = b = MkTxtButton(f, "Mark", width);
        b->Connect("Clicked()", cls, this, "DoMarkEvent()");
        
        fScreenshot = b = MkTxtButton(f, "Screenshot", 2*width);
        b->Connect("Clicked()", cls, this, "DoScreenshot()");
        
        MkLabel(f, "||", 0, 8, 8);
        
        fRefresh = b = MkTxtButton(f, "Refresh", width + 8);
        b->Connect("Clicked()",cls, this, "DoRefresh()");
        
        MkLabel(f, "||", 0, 8, 8);
        
        fAutoLoad = new TGCheckButton(f, "Autoload");
        f->AddFrame(fAutoLoad, new TGLayoutHints(kLHintsLeft, 0, 4, 3, 0));
        fAutoLoad->SetToolTipText("Automatic event loading.");
        fAutoLoad->Connect("Toggled(Bool_t)", cls, this, "DoSetAutoLoad()");
        
        fAutoLoadTime = new TEveGValuator(f, "Time: ", 110, 0);
        f->AddFrame(fAutoLoadTime);
        fAutoLoadTime->SetShowSlider(kFALSE);
        fAutoLoadTime->SetNELength(4);
        fAutoLoadTime->Build();
        fAutoLoadTime->SetLimits(0, 1000);
        fAutoLoadTime->SetToolTip("Automatic event loading time in seconds.");
        fAutoLoadTime->Connect("ValueSet(Double_t)", cls, this, "DoSetAutoLoadTime()");
        
        fTrigSel = new TGComboBox(f);
        fTrigSel->Resize(5*width,b->GetDefaultHeight());
        fTrigSel->AddEntry("No trigger selection",0);
        fTrigSel->Select(0,kFALSE);
        f->AddFrame(fTrigSel, new TGLayoutHints(kLHintsNormal, 10, 5, 0, 0));
        fTrigSel->Connect("Selected(char*)", cls, this, "DoSetTrigSel()");
        
        fReloadOffline = new TGTextButton(f, "Reload");
        fReloadOffline->Connect("Clicked()", cls, this, "DoReloadOffline()");
        fOfflineRunNumber = new TGNumberEntry(f);
        f->AddFrame(fOfflineRunNumber,new TGLayoutHints(kLHintsNormal,10,5,0,0));
        f->AddFrame(fReloadOffline,new TGLayoutHints(kLHintsNormal,10,5,0,0));
        
        
        fDataSourceGroup = new TGHButtonGroup(f, "Data Source");
        //        horizontal->SetTitlePos(TGGroupFrame::kCenter);
        fSwitchToHLT     = new TGRadioButton(fDataSourceGroup, "HLT",AliEveEventManager::kSourceHLT);
        fSwitchToOnline  = new TGRadioButton(fDataSourceGroup, "Online",AliEveEventManager::kSourceOnline);
        fSwitchToOffline = new TGRadioButton(fDataSourceGroup, "Offline",AliEveEventManager::kSourceOffline);
        
        fDataSourceGroup->Connect("Pressed(Int_t)", cls, this,"DoSwitchDataSource(AliEveEventManager::EDataSource)");
#ifndef ZMQ
        fSwitchToHLT->SetEnabled(false);
        fSwitchToOnline->SetEnabled(false);
#endif
        
        f->AddFrame(fDataSourceGroup, new TGLayoutHints(kLHintsNormal));
        
    }
    
    fEventInfo = new TGTextView(this, 400, 600);
    AddFrame(fEventInfo, new TGLayoutHints(kLHintsNormal | kLHintsExpandX | kLHintsExpandY));
    
    fM->Connect("NewEventLoaded()", cls, this, "Update()");
    
    SetCleanup(kDeepCleanup);
    Layout();
    MapSubwindows();
    MapWindow();
    
    DoSetTrigSel();
}

//______________________________________________________________________________
AliEveEventManagerWindow::~AliEveEventManagerWindow()
{
    // Destructor.
    
    fM->Disconnect("NewEventLoaded()", this);
}

AliEveEventManagerWindow* AliEveEventManagerWindow::GetInstance()
{
    if(!fInstance) new AliEveEventManagerWindow();
    return fInstance;
}


void AliEveEventManagerWindow::SetCurrentDataSource(AliEveEventManager::EDataSource defaultDataSource)
{
    fDataSourceGroup->SetButton(defaultDataSource);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoFirstEvent()
{
    // Load previous event
    AliEveDataSource *dataSource = fM->GetCurrentDataSource();
    dataSource->GotoEvent(0);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoPrevEvent()
{
    // Load previous event
    // fM->PrevEvent();
    //    if (fM->IsOnlineMode()) {
    //        fM->GotoEvent(1);
    //    }
    //    else {
    AliEveDataSource *dataSource = fM->GetCurrentDataSource();
    dataSource->GotoEvent((Int_t) fEventId->GetNumber()-1);
    
    //    }
    
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoNextEvent()
{
    // Load next event
    // fM->NextEvent();
    //    if (fM->IsOnlineMode()) {
    //        cout<<"next event, online node"<<endl;
    //        fM->GotoEvent(2);
    //    }
    //    else {
    //        cout<<"next event, offline mode"<<endl;
    AliEveDataSource *dataSource = fM->GetCurrentDataSource();
    dataSource->GotoEvent((Int_t) fEventId->GetNumber()+1);
    //    }
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoLastEvent()
{
    // Load previous event
    AliEveDataSource *dataSource = fM->GetCurrentDataSource();
    dataSource->GotoEvent(-1);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoMarkEvent()
{
    // Mark current event
    cout<<"\n\n mark event not implemented!!\n\n"<<endl;
    //    fM->MarkCurrentEvent();
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoScreenshot()
{
    AliEveSaveViews *viewsSaver = new AliEveSaveViews();
    viewsSaver->Save();
}

void AliEveEventManagerWindow::DoReloadOffline()
{
    TEnv settings;
    AliEveInit::GetConfig(&settings);
    
    const char* path = Form("%s%ld",settings.GetValue("offline.base.path",""),fOfflineRunNumber->GetIntNumber());
    
    cout<<"Changing path to:"<<path<<endl;
    
    AliEveDataSourceOffline *dataSource = (AliEveDataSourceOffline*)fM->GetDataSourceOffline();
    dataSource->SetFilesPath(path);
    fM->ChangeDataSource(AliEveEventManager::kSourceOffline);
    fDataSourceGroup->SetButton(AliEveEventManager::kSourceOffline);
    dataSource->GotoEvent(0);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoSetEvent()
{
    // Set current event
    AliEveDataSource *dataSource = fM->GetCurrentDataSource();
    dataSource->GotoEvent((Int_t) fEventId->GetNumber());
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoRefresh()
{
    // Refresh event status.
    AliEveMultiView::Instance()->DestroyAllGeometries();
    
    AliEveInit::SetupBackground();
    AliEveInit::SetupGeometry();
    AliEveInit::AddMacros();
    AliEveInit::SetupCamera();
    
    TEveScene *rPhiScene = AliEveMultiView::Instance()->GetRPhiScene();
    TEveScene *rhoZScene = AliEveMultiView::Instance()->GetRhoZScene();
    
    TEveElement::List_i rPhiElement = rPhiScene->BeginChildren();
    TEveElement::List_i rhoZElement = rhoZScene->BeginChildren();
    
    TEveProjectionAxes* rPhiAxes = ((TEveProjectionAxes*)*rPhiElement);
    TEveProjectionAxes* rhoZAxes = ((TEveProjectionAxes*)*rhoZElement);
    
    TEnv settings;
    AliEveInit::GetConfig(&settings);
    rPhiAxes->SetRnrSelf(settings.GetValue("axes.show",false));
    rhoZAxes->SetRnrSelf(settings.GetValue("axes.show",false));
    
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    gEve->Redraw3D();
    
    Int_t ev = fM->GetEventId();
    AliEveDataSource *currentDataSource = fM->GetCurrentDataSource();
    currentDataSource->GotoEvent(ev);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoSetAutoLoad()
{
    // Set the auto-load flag
    
    fM->SetAutoLoad(fAutoLoad->IsOn());
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoSetAutoLoadTime()
{
    // Set the auto-load time in seconds
    
    fM->SetAutoLoadTime(fAutoLoadTime->GetValue());
}

//______________________________________________________________________________
void AliEveEventManagerWindow::DoSetTrigSel()
{
    cout<<"setting selected trigger:"<<fTrigSel->GetSelectedEntry()->GetTitle()<<endl;
    fM->SetSelectedTrigger(fTrigSel->GetSelectedEntry()->GetTitle());
}

void AliEveEventManagerWindow::ResetTriggerSelection()
{
    fTrigSel->Select(0,kFALSE);
    fTrigSel->SetEnabled();
    DoSetTrigSel();
    Layout();
}

void AliEveEventManagerWindow::SetActiveTriggerClasses()
{
    string s = fM->GetESD()->GetESDRun()->GetActiveTriggerClasses().Data();
    stringstream ss(s);
    istream_iterator<string> begin(ss);
    istream_iterator<string> end;
    vector<string> vstrings(begin, end);
    copy(vstrings.begin(), vstrings.end(), ostream_iterator<string>(cout, "\n"));
    
    fTrigSel->RemoveAll();
    fTrigSel->AddEntry("No trigger selection",0);
    fTrigSel->Select(0,kFALSE);
    
    for(int i=0;i<vstrings.size();i++)
    {
        fTrigSel->AddEntry(vstrings[i].c_str(),i);
    }
    fTrigSel->SetEnabled();
    DoSetTrigSel();
    Layout();
}

void AliEveEventManagerWindow::DoSwitchDataSource(AliEveEventManager::EDataSource source)
{
    fM->ChangeDataSource(source);
}

//______________________________________________________________________________
void AliEveEventManagerWindow::Update()
{
    fEventId->SetNumber(fM->GetEventId());
    fEventId->SetState(kTRUE);
    
    fAutoLoad->SetState(fM->GetAutoLoad() ? kButtonDown : kButtonUp);
    fAutoLoadTime->SetValue(fM->GetAutoLoadTime());
    
    Layout();
}

//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//______________________________________________________________________________
TGTextButton* AliEveEventManagerWindow::MkTxtButton(TGCompositeFrame* p,
                                                    const char* txt, Int_t width,
                                                    Int_t lo, Int_t ro, Int_t to, Int_t bo)
{
    // Create a standard button.
    // If width is not zero, the fixed-width flag is set.
    
    TGTextButton* b = new TGTextButton(p, txt);
    if (width > 0) {
        b->SetWidth(width);
        b->ChangeOptions(b->GetOptions() | kFixedWidth);
    }
    p->AddFrame(b, new TGLayoutHints(kLHintsNormal, lo,ro,to,bo));
    return b;
}

//______________________________________________________________________________
TGLabel* AliEveEventManagerWindow::MkLabel(TGCompositeFrame* p,
                                           const char* txt, Int_t width,
                                           Int_t lo, Int_t ro, Int_t to, Int_t bo)
{
    // Create a standard button.
    // If width is not zero, the fixed-width flag is set.
    
    TGLabel* l = new TGLabel(p, txt);
    if (width > 0) {
        l->SetWidth(width);
        l->ChangeOptions(l->GetOptions() | kFixedWidth);
    }
    p->AddFrame(l, new TGLayoutHints(kLHintsNormal, lo,ro,to,bo));
    return l;
}

TString AliEveEventManagerEditor::GetEventInfoHorizontal() const
{
    // Dumps the event-header contents in vertical formatting.
    
    TString rawInfo, esdInfo;
    
    if (!AliEveEventManager::AssertRawReader())
    {
        rawInfo = "No raw-data event info is available!\n";
    }
    else
    {
        const UInt_t* attr = AliEveEventManager::AssertRawReader()->GetAttributes();
        TTimeStamp ts(AliEveEventManager::AssertRawReader()->GetTimestamp());
        rawInfo.Form("RAW event info: Run#: %d  Event type: %d (%s)  Period: %x  Orbit: %x  BC: %x\n"
                     "Trigger: %llx\nDetectors: %x (%s)\nAttributes:%x-%x-%x  Timestamp: %s\n",
                     AliEveEventManager::AssertRawReader()->GetRunNumber(),AliEveEventManager::AssertRawReader()->GetType(),AliRawEventHeaderBase::GetTypeName(AliEveEventManager::AssertRawReader()->GetType()),
                     AliEveEventManager::AssertRawReader()->GetPeriod(),AliEveEventManager::AssertRawReader()->GetOrbitID(),AliEveEventManager::AssertRawReader()->GetBCID(),
                     AliEveEventManager::AssertRawReader()->GetClassMask(),
                     *(AliEveEventManager::AssertRawReader())->GetDetectorPattern(),AliDAQ::ListOfTriggeredDetectors(*(AliEveEventManager::AssertRawReader())->GetDetectorPattern()),
                     attr[0],attr[1],attr[2], ts.AsString("s"));
    }
    
    if (!AliEveEventManager::AssertESD())
    {
        esdInfo = "No ESD event info is available!";
    }
    else
    {
        TString acttrclasses   = AliEveEventManager::AssertESD()->GetESDRun()->GetActiveTriggerClasses();
        TString firedtrclasses = AliEveEventManager::AssertESD()->GetFiredTriggerClasses();
        TTimeStamp ts(AliEveEventManager::AssertESD()->GetTimeStamp());
        esdInfo.Form("ESD event info: Run#: %d  Event type: %d (%s)  Period: %x  Orbit: %x  BC: %x\n"
                     "Active trigger classes: %s\nTrigger: %llx (%s)\nEvent# in file: %d  Timestamp: %s, MagField: %.2e",
                     AliEveEventManager::AssertESD()->GetRunNumber(),
                     AliEveEventManager::AssertESD()->GetEventType(),AliRawEventHeaderBase::GetTypeName(AliEveEventManager::AssertESD()->GetEventType()),
                     AliEveEventManager::AssertESD()->GetPeriodNumber(),AliEveEventManager::AssertESD()->GetOrbitNumber(),AliEveEventManager::AssertESD()->GetBunchCrossNumber(),
                     acttrclasses.Data(),
                     AliEveEventManager::AssertESD()->GetTriggerMask(),firedtrclasses.Data(),
                     AliEveEventManager::AssertESD()->GetEventNumberInFile(), ts.AsString("s"), AliEveEventManager::AssertESD()->GetMagneticField());
    }
    
    return rawInfo + esdInfo;
}

TString AliEveEventManagerEditor::GetEventInfoVertical() const
{
    // Dumps the event-header contents in vertical formatting.
    
    TString rawInfo, esdInfo;
    
    if (!AliEveEventManager::AssertRawReader())
    {
        rawInfo = "No raw-data event info is available!\n";
    }
    else
    {
        const UInt_t* attr = AliEveEventManager::AssertRawReader()->GetAttributes();
        rawInfo.Form("Raw-data event info:\nRun#: %d\nEvent type: %d (%s)\nPeriod: %x\nOrbit: %x   BC: %x\nTrigger: %llx\nDetectors: %x (%s)\nAttributes:%x-%x-%x\nTimestamp: %x\n",
                     AliEveEventManager::AssertRawReader()->GetRunNumber(),AliEveEventManager::AssertRawReader()->GetType(),AliRawEventHeaderBase::GetTypeName(AliEveEventManager::AssertRawReader()->GetType()),
                     AliEveEventManager::AssertRawReader()->GetPeriod(),AliEveEventManager::AssertRawReader()->GetOrbitID(),AliEveEventManager::AssertRawReader()->GetBCID(),
                     AliEveEventManager::AssertRawReader()->GetClassMask(),
                     *(AliEveEventManager::AssertRawReader())->GetDetectorPattern(),AliDAQ::ListOfTriggeredDetectors(*(AliEveEventManager::AssertRawReader())->GetDetectorPattern()),
                     attr[0],attr[1],attr[2],
                     AliEveEventManager::AssertRawReader()->GetTimestamp());
    }
    
    if (!AliEveEventManager::AssertESD())
    {
        esdInfo = "No ESD event info is available!\n";
    }
    else
    {
        TString acttrclasses   = AliEveEventManager::AssertESD()->GetESDRun()->GetActiveTriggerClasses();
        TString firedtrclasses = AliEveEventManager::AssertESD()->GetFiredTriggerClasses();
        esdInfo.Form("ESD event info:\nRun#: %d\nActive trigger classes: %s\nEvent type: %d (%s)\nPeriod: %x\nOrbit: %x   BC: %x\nTrigger: %llx (%s)\nEvent# in file:%d\nTimestamp: %x\n",
                     AliEveEventManager::AssertESD()->GetRunNumber(),
                     acttrclasses.Data(),
                     AliEveEventManager::AssertESD()->GetEventType(),AliRawEventHeaderBase::GetTypeName(AliEveEventManager::AssertESD()->GetEventType()),
                     AliEveEventManager::AssertESD()->GetPeriodNumber(),AliEveEventManager::AssertESD()->GetOrbitNumber(),AliEveEventManager::AssertESD()->GetBunchCrossNumber(),
                     AliEveEventManager::AssertESD()->GetTriggerMask(),firedtrclasses.Data(),
                     AliEveEventManager::AssertESD()->GetEventNumberInFile(),
                     AliEveEventManager::AssertESD()->GetTimeStamp());
    }
    
    return rawInfo + "\n" + esdInfo;
}
