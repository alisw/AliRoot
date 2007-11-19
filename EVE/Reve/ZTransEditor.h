// $Header$

#ifndef REVE_ZTransEditor_H
#define REVE_ZTransEditor_H

#include <TGedFrame.h>

class TGCheckButton;
class TGTextButton;

namespace Reve {

class ZTrans;
class RGTriVecValuator;

class ZTransSubEditor : public TGVerticalFrame
{
  ZTransSubEditor(const ZTransSubEditor&);            // Not implemented
  ZTransSubEditor& operator=(const ZTransSubEditor&); // Not implemented

protected:
  ZTrans            *fTrans;

  TGHorizontalFrame *fTopHorFrame;

  TGCheckButton     *fUseTrans;
  TGCheckButton     *fEditTrans;

  TGVerticalFrame   *fEditTransFrame;

  RGTriVecValuator  *fPos;
  RGTriVecValuator  *fRot;
  RGTriVecValuator  *fScale;

  TGCheckButton     *fAutoUpdate;
  TGTextButton      *fUpdate;          

public:
  ZTransSubEditor(TGWindow* p);
  virtual ~ZTransSubEditor() {}

  void SetDataFromTrans(ZTrans* t);
  void SetTransFromData();

  void UseTrans();     //*SIGNAL*
  void TransChanged(); //*SIGNAL*

  void DoUseTrans();
  void DoEditTrans();
  void DoTransChanged();

  ClassDef(ZTransSubEditor, 0); // Sub-editor for ZTrans class.
};

class ZTransEditor : public TGedFrame
{
private:
  ZTransEditor(const ZTransEditor&);            // Not implemented
  ZTransEditor& operator=(const ZTransEditor&); // Not implemented

protected:
  ZTrans* fM; // Model object.

public:
  ZTransEditor(const TGWindow* p=0, Int_t width=170, Int_t height=30, UInt_t options = kChildFrame, Pixel_t back=GetDefaultFrameBackground());
  virtual ~ZTransEditor();

  virtual void SetModel(TObject* obj);

  ClassDef(ZTransEditor, 1); // Editor for ZTrans class.
}; // endclass ZTransEditor

}

#endif
