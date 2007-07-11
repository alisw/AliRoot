#ifndef ALIMUONTRIGGERGUIBOARD_H
#define ALIMUONTRIGGERGUIBOARD_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/// \ingroup evaluation
/// \class AliMUONTriggerGUIboard
/// \brief Trigger GUI utility class: single board object
//  Author Bogdan Vulpescu, LPC Clermont-Ferrand

#include <TString.h>
#include <TObject.h>

class TObjArray;
class TBox;

class AliMUONTriggerGUIboard : public TObject
{

public:

  AliMUONTriggerGUIboard(Int_t id, Char_t *name);
  virtual ~AliMUONTriggerGUIboard();

  /// returns the standard name of this board
  Char_t  *GetBoardName()   const { return (Char_t*)(fName->Data()); };
  /// returns the working status of this board
  UShort_t GetStatus() const { return fStatus; };
  /// returns the number of this board
  Int_t    GetNumber() const { return fID; };

  /// initialize the board
  void Init() {};
  /// set the working status of this board
  void SetStatus(UShort_t s) { fStatus = s; };
  /// set the standard name of this board
  void SetBoardName(Char_t *name) { fName = new TString(name); };
  /// set the geometrical location and dimensions of this board
  void SetDimensions(Int_t imt, 
		     Float_t xc, Float_t yc, Float_t zc,
		     Float_t xw, Float_t yw) { 
    fXCenter[imt] = xc; fYCenter[imt] = yc; fZCenter[imt] = zc;
    fXWidth[imt]  = xw; fYWidth[imt]  = yw; 
  };
  /// set the index range for the x strips
  void SetXSindex(Int_t ix, Int_t iy1, Int_t iy2) {
    fXSix  = ix;
    fXSiy1 = iy1;
    fXSiy2 = iy2;
  };
  /// set the index range for the y strips
  void SetYSindex(Int_t ix1, Int_t ix2, Int_t iy) {
    fYSix1 = ix1;
    fYSix2 = ix2;
    fYSiy  = iy;
  };
  /// set the number of the detector element containing this board
  void SetDetElemId(Int_t id) { fDetElemId = id; };
  /// set the number of this board 
  void SetIdCircuit(Int_t id) { fIdCircuit = id; };
  /// set an x-strip digit in a chamber with amplitude = amp
  void SetDigitX(Int_t imt, Int_t is, Int_t amp) { 
    fXDig[imt][is] = (UChar_t)amp; }; 
  /// set a  y-strip digit in a chamber with amplitude = amp
  void SetDigitY(Int_t imt, Int_t is, Int_t amp) { 
    fYDig[imt][is] = (UChar_t)amp; }; 
  /// set neighbouring boards with common y strips
  void SetYOver(Int_t over) { fYOver = (UChar_t)over; };
  /// get neighbouring boards with common y strips
  UChar_t GetYOver() const  { return fYOver; };
  /// set the board position inside the detector element in y direction
  void SetPosition(Int_t pos) { fPosition = (UChar_t)pos; };
  /// get the board position inside the detector element in y direction
  UChar_t GetPosition() const { return fPosition; };
  /// get the digit amplitude for an x-strip in a given chamber
  Int_t GetXDig(Int_t imt, Int_t is) const { return fXDig[imt][is]; };
  /// get the digit amplitude for a  y-strip in a given chamber
  Int_t GetYDig(Int_t imt, Int_t is) const { return fYDig[imt][is]; };

  void SetXDigBox(Int_t imt, Int_t is, Double_t x1, Double_t y1, Double_t x2, Double_t y2);
  void SetYDigBox(Int_t imt, Int_t is, Double_t x1, Double_t y1, Double_t x2, Double_t y2);
  /// get the graphical box of an x-trip
  TBox *GetXDigBox(Int_t imt, Int_t is) const { return fXDigBox[imt][is]; };
  /// get the graphical box of a  y-trip
  TBox *GetYDigBox(Int_t imt, Int_t is) const { return fYDigBox[imt][is]; };

  /// get x-center of the board in chamber imt
  Float_t GetXCenter(Int_t imt) const { return fXCenter[imt]; };
  /// get y-center of the board in chamber imt
  Float_t GetYCenter(Int_t imt) const { return fYCenter[imt]; };
  /// get z-center of the board in chamber imt
  Float_t GetZCenter(Int_t imt) const { return fZCenter[imt]; };
  /// get x-width of the board in chamber imt
  Float_t GetXWidth(Int_t imt)  const { return fXWidth[imt]; };
  /// get y-width of the board in chamber imt
  Float_t GetYWidth(Int_t imt)  const { return fYWidth[imt]; };

  /// get x-index in detector element for an x-strip
  Int_t GetXSix()  const { return fXSix;  };
  /// get first y-index in detector element for an x-strip
  Int_t GetXSiy1() const { return fXSiy1; };
  /// get last  y-index in detector element for an x-strip
  Int_t GetXSiy2() const { return fXSiy2; };
  /// get first x-index in detector element for a  y-strip
  Int_t GetYSix1() const { return fYSix1; };
  /// get last  x-index in detector element for a  y-strip
  Int_t GetYSix2() const { return fYSix2; };
  /// get y-index in detector element for a y-strip
  Int_t GetYSiy()  const { return fYSiy;  };
  /// get number of x strips
  Int_t GetNStripX() const { return GetXSiy2() - GetXSiy1() + 1; };
  /// get number of y strips
  Int_t GetNStripY() const { return GetYSix2() - GetYSix1() + 1; };
  /// get the id of the detector element
  Int_t GetDetElemId() const { return fDetElemId; };
  /// get the id of the circuit
  Int_t GetIdCircuit() const { return fIdCircuit; };

  /// set true if this board has a gui active
  void   SetOpen(Bool_t open) { fIsOpen = open; };
  /// true if this board has a gui active
  Bool_t IsOpen() const       { return fIsOpen; };

  void  ClearXDigits();
  void  ClearYDigits();

private:

  enum { kNMT = 4, kNS = 16 };     ///< constants

  /// Not implemented
  AliMUONTriggerGUIboard (const AliMUONTriggerGUIboard& board);
  /// Not implemented
  AliMUONTriggerGUIboard& operator=(const AliMUONTriggerGUIboard& board);

  TString       *fName;            ///< Board name LCxLxBx or RCxLxBx
  Int_t          fID;              ///< Board serial number
  UShort_t       fStatus;          ///< Board status
  UChar_t        fPosition;        ///< Y-boards position
  UChar_t        fYOver;           ///< Y-boards with common y-strips

  Float_t        fXCenter[kNMT];   ///< X-center of the board
  Float_t        fYCenter[kNMT];   ///< Y-center of the board
  Float_t        fZCenter[kNMT];   ///< Z-center of the board
  Float_t        fXWidth[kNMT];    ///< X-width  of the board
  Float_t        fYWidth[kNMT];    ///< Y-width  of the board

  Int_t          fXSix;            ///< X-strips ix index in the board
  Int_t          fXSiy1;           ///< X-strips first iy index in the board
  Int_t          fXSiy2;           ///< X-strips last  iy index in the board

  Int_t          fYSix1;           ///< Y-strips first ix index in the board
  Int_t          fYSix2;           ///< Y-strips last  ix index in the board
  Int_t          fYSiy;            ///< Y-strips iy index in the board

  Int_t          fDetElemId;       ///< Detector element ID (modulo 100)

  Int_t          fIdCircuit;       ///< Circuit number

  UChar_t        fXDig[kNMT][kNS]; ///< X-digits amplitude, set by GUI
  UChar_t        fYDig[kNMT][kNS]; ///< Y-digits amplitude, set by GUI

  TBox          *fXDigBox[kNMT][kNS]; ///< X-digits boxes
  TBox          *fYDigBox[kNMT][kNS]; ///< Y-digits boxes

  Bool_t         fIsOpen;          ///< Selection flag for the digits map

  ClassDef(AliMUONTriggerGUIboard,1) //Trigger GUI utility class: single board object

};

#endif
