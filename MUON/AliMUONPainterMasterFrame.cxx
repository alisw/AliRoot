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

// $Id$

#include "AliMUONPainterMasterFrame.h"

#include "AliMUONChamberPainter.h"
#include "AliMUONPainterGroup.h"
#include "AliMUONPainterMatrix.h"
#include "AliMUONPainterMatrixFrame.h"
#include "AliMUONPainterInterfaceHelper.h"
#include "AliMUONPainterRegistry.h"
#include "AliMUONAttPainterSelectorFrame.h"
#include "AliMUONVPainter.h"
#include "AliCodeTimer.h"
#include "AliLog.h"
#include <Riostream.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TEnv.h>
#include <TGComboBox.h>
#include <TGLabel.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TGButtonGroup.h>
#include <TGMsgBox.h>

/// \class AliMUONPainterMasterFrame
///
/// Main window of the 2D display
///
/// \author Laurent Aphecetche, Subatech

///\cond CLASSIMP
ClassImp(AliMUONPainterMasterFrame)
///\endcond

namespace
{
  UInt_t UniqueID(Int_t nx, Int_t ny)
  {
    return ny | (nx << 8);
  }
  
  Int_t Nx(UInt_t uniqueID)
  {
    return ( uniqueID & 0xFF00 ) >> 8;
  }

  Int_t Ny(UInt_t uniqueID)
  {
    return uniqueID & 0xFF;
  }

}

const Int_t AliMUONPainterMasterFrame::fgkBorderSize = 10;

//_____________________________________________________________________________
AliMUONPainterMasterFrame::AliMUONPainterMasterFrame(const TGWindow* p, 
                                                     UInt_t w, UInt_t h)
: TGCompositeFrame(p,w,h,kVerticalFrame),
  fNavigationFrame(0x0),
  fPainterMatrixFrame(0x0),
  fBackButton(0x0),
  fForwardButton(0x0),
  fGroupTitle(0x0),
  fNavigation(),
  fCurrentNavigationPosition(-1),
  fAttPainterSelectorFrame(0x0)
{  
  /// ctor
    
  UInt_t wi = w - fgkBorderSize*2;
  UInt_t hi = h - fgkBorderSize*3;
  
  fNavigationFrame = new TGHorizontalFrame(this,wi);
  
  AddFrame(fNavigationFrame,new TGLayoutHints(kLHintsExpandX|kLHintsTop,
                                              fgkBorderSize,fgkBorderSize,
                                              fgkBorderSize,fgkBorderSize));
    
  fBackButton = new TGPictureButton(fNavigationFrame,
                                       gClient->GetPicture("tb_back.xpm"));
  
  fForwardButton = new TGPictureButton(fNavigationFrame,
                                       gClient->GetPicture("tb_forw.xpm"));

  fAttPainterSelectorFrame = new AliMUONAttPainterSelectorFrame(fNavigationFrame,w/2,20);
  
  fGroupTitle = new TGLabel(fNavigationFrame,"");
  
  fNavigationFrame->AddFrame(fBackButton,new TGLayoutHints(kLHintsCenterY));
  fNavigationFrame->AddFrame(fForwardButton,new TGLayoutHints(kLHintsCenterY));
  
  fNavigationFrame->AddFrame(fAttPainterSelectorFrame,new TGLayoutHints(kLHintsCenterY,10));
  
  fAttPainterSelectorFrame->Connect("Clicked(AliMUONAttPainter*)",
                                    "AliMUONPainterMasterFrame",
                                    this,
                                    "AttributesChanged(AliMUONAttPainter*)");
  
  fNavigationFrame->AddFrame(fGroupTitle,new TGLayoutHints(kLHintsExpandX|kLHintsCenterX|kLHintsCenterY,10));
  
  fForwardButton->Connect("Clicked()","AliMUONPainterMasterFrame",
                          this,
                          "Forward()");

  fBackButton->Connect("Clicked()","AliMUONPainterMasterFrame",
                          this,
                          "Backward()");
    
  
                                                                   
  UInt_t w1 = wi;
  //  UInt_t h1 = hi - fNavigationFrame->GetHeight() - 3*fgkBorderSize;
  UInt_t h1 = hi - 7*12;
  
  MakeTopPainterMatrix(w1,h1);

  AddFrame(fPainterMatrixFrame,new TGLayoutHints(kLHintsExpandX,
                                                fgkBorderSize,fgkBorderSize,
                                                0,fgkBorderSize));
  
  AliMUONPainterInterfaceHelper::SetBackgroundColor("MasterFrame.Navigation",*fNavigationFrame);
  AliMUONPainterInterfaceHelper::SetBackgroundColor("MasterFrame.Main",*this);
  
  AliDebug(1,Form("fNavigation=%p",&fNavigation));
  
  AliMUONPainterRegistry::Instance()->Connect("PainterMatrixWantToShow(AliMUONPainterMatrix*)",
                                              "AliMUONPainterMasterFrame",
                                              this,
                                              "PainterMatrixWantToShow(AliMUONPainterMatrix*)");
  
  fPainterMatrixFrame->DataSourceWasChanged("*",0x0,-1);
}

//_____________________________________________________________________________
AliMUONPainterMasterFrame::~AliMUONPainterMasterFrame()
{
  /// dtor
  Cleanup();
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::AddPainterMatrix(AliMUONPainterMatrix* painterMatrix)
{
  /// array is adopted (by the registry)

  AliDebug(1,Form("matrix=%x %s",painterMatrix,painterMatrix->GetName()));
  
  Int_t i = AliMUONPainterRegistry::Instance()->Register(painterMatrix);

  SetNavigation(i);
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::PainterMatrixWantToShow(AliMUONPainterMatrix* group)
{
  /// FIXME: should check whether we are the active window before
  /// responding to this message ?

  AliDebug(1,Form("group=%x %s",group,group->GetName()));
  
  Int_t i = AliMUONPainterRegistry::Instance()->FindIndexOf(group);

  Int_t alreadyThere(-1);
  
  for ( Int_t j = 0; j < fNavigation.GetSize(); ++j )
  {
    if ( fNavigation[j] == i ) alreadyThere = j;
  }
  
  if (alreadyThere<0) 
  {
    SetNavigation(i);
  }
  else
  {
    fCurrentNavigationPosition = alreadyThere;
  }
  
  ShowPainterMatrix(group);  
}
                                                
//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::SetNavigation(Int_t i)
{
  /// Change navigation position
  
  ++fCurrentNavigationPosition;
  fNavigation.Set(fCurrentNavigationPosition+1);
  fNavigation[fCurrentNavigationPosition] = i;
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::ShowPainterMatrix(AliMUONPainterMatrix* painterMatrix)
{
  /// Change the painter matrix we show
  
  fPainterMatrixFrame->Use(painterMatrix);
  
  painterMatrix->Connect("Clicked(AliMUONVPainter*,Double_t*)",
                         "AliMUONPainterMasterFrame",this,
                         "Clicked(AliMUONVPainter*,Double_t*)");

  painterMatrix->Connect("ShiftClicked(AliMUONVPainter*,Double_t*)",
                         "AliMUONPainterMasterFrame",this,
                         "ShiftClicked(AliMUONVPainter*,Double_t*)");
  
  fPainterMatrixFrame->Connect("TitleHasChanged(const char*)",
                              "AliMUONPainterMasterFrame",this,
                              "ChangeTitle(const char*)");
  UpdateNavigation();
  
  UpdateAttributes(*(fPainterMatrixFrame->Matrix()));
  
  AliMUONPainterRegistry::Instance()->AddToHistory(painterMatrix);
  
  Layout();
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::ChangeTitle(const char* newTitle)
{
  /// Change the top title
  
  fGroupTitle->SetText(newTitle);
  fGroupTitle->Resize();
  Layout();
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::Backward()
{
  /// Move back one step in the history
  --fCurrentNavigationPosition;
  
  AliMUONPainterMatrix* group = 
    AliMUONPainterRegistry::Instance()->PainterMatrix(fNavigation[fCurrentNavigationPosition]);
  
  ShowPainterMatrix(group);
  
  UpdateNavigation();
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::Forward()
{
  /// Move forward one step in history
  
  ++fCurrentNavigationPosition;
  
  AliMUONPainterMatrix* group = 
    AliMUONPainterRegistry::Instance()->PainterMatrix(fNavigation[fCurrentNavigationPosition]);
  
  ShowPainterMatrix(group);
  
  UpdateNavigation();
}

//_____________________________________________________________________________
void 
AliMUONPainterMasterFrame::Clicked(AliMUONVPainter* painter, Double_t* values)
{
  /// A given painter was (singly) clicked
  
  AliDebug(1,Form("%s x %7.3f y %7.3f",painter->GetName(),values[0],values[1]));

  AliCodeTimerAuto("")
  
  fPainterMatrixFrame->MouseLeave(painter);
  
  AliMUONPainterMatrix* matrix = new AliMUONPainterMatrix(painter->Name().Data());

  AliMUONVPainter* p = painter->Detach();

  p->SetResponder(1);

  matrix->Adopt(p);
  
  AddPainterMatrix(matrix);
  ShowPainterMatrix(matrix);
}

//_____________________________________________________________________________
void 
AliMUONPainterMasterFrame::ShiftClicked(AliMUONVPainter* painter, Double_t*)
{
  /// A given painter was shift-clicked
  
  AliMUONPainterMatrix* currentMatrix = fPainterMatrixFrame->Matrix();
  
  AliMUONAttPainter a = painter->Attributes();
  
  TString basename(Form("%s-DUAL",painter->GetName()));
  
  TString newName = AliMUONPainterMatrix::NameIt(basename.Data(),a);
  
  AliMUONPainterMatrix* matrix = 
    AliMUONPainterRegistry::Instance()->FindPainterMatrix(newName.Data());
  
  if (!matrix)
  {
    // No. So we must make a new matrix painter from the existing one,
    // and add to this new matrix the painters of the other one, but
    // using the new attributes...
    
    // create "opposite" attributes
    AliMUONAttPainter a1(a);
    AliMUONAttPainter a2(a);
    
    if ( a.IsCathodeDefined() ) 
    {
      a2.SetCathode(!a.IsCathode0(),!a.IsCathode1());
    }
    
    if ( a.IsPlaneDefined() ) 
    {
      a2.SetPlane(!a.IsBendingPlane(),!a.IsNonBendingPlane());
    }
    
    a1.SetCathodeAndPlaneDisabled(kTRUE);
    a2.SetCathodeAndPlaneDisabled(kTRUE);

    AliMUONVPainter* p1 = AliMUONVPainter::CreatePainter(painter->ClassName(),
                                                         a1,
                                                         painter->ID0(),
                                                         painter->ID1());
    
    AliMUONVPainter* p2 = AliMUONVPainter::CreatePainter(painter->ClassName(),
                                                        a2,
                                                        painter->ID0(),
                                                        painter->ID1());
    
    if (!p1 || !p2)
    {
      Int_t ret;
      new TGMsgBox(gClient->GetRoot(), this,
                   "Invalid combination", "Cannot create 2 views from this painter",
                   kMBIconExclamation, kMBOk, &ret);
      PainterMatrixWantToShow(currentMatrix);
      delete p1;
      delete p2;
      return;
    }
    
    AliInfo(Form("Starting from %s will generate %s and %s",
                 a.GetName(),
                 a1.GetName(),
                 a2.GetName()));
    
    p1->UpdateGroupsFrom(*(painter->Master()));
    p2->UpdateGroupsFrom(*(painter->Master()));
    
    p1->SetResponder(1);
    p2->SetResponder(1);
    
    Int_t nx(2);
    Int_t ny(1);
    
    AliMpArea area(painter->Area());

    if ( area.Dimensions().X() > 1.2*area.Dimensions().Y() ) 
    {
      nx = 1;
      ny = 2;
    }
    
    matrix = new AliMUONPainterMatrix(basename.Data(),nx,ny);
    
    matrix->Adopt(p1);
    matrix->Adopt(p2);
    
    AddPainterMatrix(matrix);
  }
  
  matrix->SetData(currentMatrix->DataPattern(),
                  currentMatrix->Data(),
                  currentMatrix->DataIndex());
  
  fPainterMatrixFrame->MouseLeave(painter);
  
  PainterMatrixWantToShow(matrix);
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::Update()
{
  /// Update ourselves
  
  fPainterMatrixFrame->Update();
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::UpdateAttributes(const AliMUONPainterMatrix& painterMatrix)
{
  /// Update the view buttons from the matrix we actually plot
  
  fAttPainterSelectorFrame->Update(painterMatrix.Attributes());
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::MakeTopPainterMatrix(UInt_t w, UInt_t h)
{
  /// Create the first painter matrix that appears when we are create
  /// FIXME: how to make this more flexible ?
  
  fPainterMatrixFrame = new AliMUONPainterMatrixFrame(this,w,h);

  AliMUONAttPainter att;
  
  att.SetCathode(kTRUE,kFALSE);
  att.SetViewPoint(kTRUE,kFALSE);
    
  TString name = AliMUONPainterMatrix::NameIt("Tracker",att);
  
  AliMUONPainterMatrix* painterMatrix = AliMUONPainterRegistry::Instance()->FindPainterMatrix(name);
  
  if (!painterMatrix)
  {
    AliError(Form("Could not get pre-defined painter matrix %s : check that !",name.Data()));
  }
  else
  {
    PainterMatrixWantToShow(painterMatrix);
//    fPainterMatrixFrame->Use(painterMatrix);
//    ShowPainterMatrix(painterMatrix);
  }
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::UpdateNavigation()
{
  /// Update navigation frame

  fBackButton->SetEnabled(kTRUE);
  fForwardButton->SetEnabled(kTRUE);

  if ( fCurrentNavigationPosition == 0 ) 
  {
    fBackButton->SetEnabled(kFALSE);
  }
  if ( fCurrentNavigationPosition == fNavigation.GetSize()-1 ) 
  {
    fForwardButton->SetEnabled(kFALSE);
  }
}

//_____________________________________________________________________________
void
AliMUONPainterMasterFrame::AttributesChanged(AliMUONAttPainter* newValues)
{
  /// Attributes changed (e.g. from cath0 to cath1 or bending to nonbending, etc...)
  
  AliMUONPainterMatrix* currentMatrix = fPainterMatrixFrame->Matrix();
  
  AliMUONAttPainter a = currentMatrix->Validate(*newValues);
  
  if (!a.IsValid())
  {
    Int_t ret;
    new TGMsgBox(gClient->GetRoot(), this,
                 "Invalid combination", "Change of attributes not possible for this object",
                 kMBIconExclamation, kMBOk, &ret);
    PainterMatrixWantToShow(currentMatrix);
    return;
  }
  
  // First check if we already have this matrix available
  
  TString newName = AliMUONPainterMatrix::NameIt(currentMatrix->Basename(),a);
  
  AliMUONPainterMatrix* matrix = 
    AliMUONPainterRegistry::Instance()->FindPainterMatrix(newName.Data());

  if (!matrix)
  {
    // No. So we must make a new matrix painter from the existing one,
    // and add to this new matrix the painters of the other one, but
    // using the new attributes...
    
    matrix = currentMatrix->Clone(a);
  
    AddPainterMatrix(matrix);
  }
  
  matrix->SetData(currentMatrix->DataPattern(),
                  currentMatrix->Data(),
                  currentMatrix->DataIndex());
  
  PainterMatrixWantToShow(matrix);
}
