#ifndef ALICRTCONSTANTS_H
#define ALICRTCONSTANTS_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

////////////////////////////////////////////////////////////////////////
//
// AliCRTConstants class
//
// This class serves to group constants needed by ACORDE detector in 1
// easily accessible place. All constants are public const static data 
// members. The class is never instatiated.
//
// Author: Arturo Fernandez, Enrique Gamez
//         FCFM-UAP, Mexico.
//
/////////////////////////////////////////////////////////////////////////

#include <TObject.h>

enum ECRMode {
  kSingleMuons,
  kMuonBundle,
  kMuonFlux
};

class AliCRTConstants : public TObject {
public:
  virtual ~AliCRTConstants();

  static AliCRTConstants* Instance();

  const Float_t CageLenght() const;
  const Float_t CageWidth() const;
  const Float_t CageHeight() const;

  const Float_t SinglePaletteLenght() const;
  const Float_t SinglePaletteWidth() const;
  const Float_t SinglePaletteHeight() const;

  const Float_t ActiveAreaGap() const;
  const Float_t ActiveAreaLenght() const;
  const Float_t ActiveAreaWidth() const;
  const Float_t ActiveAreaHeight() const;

  const Float_t MagnetWidth() const;
  const Float_t MagnetLenght() const;
  const Float_t MagMinRadius() const;
  const Float_t MagMaxRadius() const;

  const Float_t Depth() const;

protected:
  AliCRTConstants();
  AliCRTConstants(const AliCRTConstants& ct);
  AliCRTConstants& operator=(const AliCRTConstants& ct);

  static AliCRTConstants* fgInstance; // static instanton

  static const Float_t fgkCageLenght; // Cage lenght
  static const Float_t fgkCageWidth;  // Cage width
  static const Float_t fgkCageHeight; // Cage height

  static const Float_t fgkSinglePaletteLenght; // Palette lenght
  static const Float_t fgkSinglePaletteWidth;  // Palette width
  static const Float_t fgkSinglePaletteHeight; // Palette height

  static const Float_t fgkActiveAreaGap; // Scintillator active area

  static const Float_t fgkActiveAreaLenght; // Active area lenght
  static const Float_t fgkActiveAreaWidth;  // Active area width
  static const Float_t fgkActiveAreaHeight; // Active area height

  static const Float_t fgkMagnetWidth;  // Magnet widht
  static const Float_t fgkMagnetLenght; // Magnet lenght
  static const Float_t fgkMagMinRadius; // Magnet inside radius
  static const Float_t fgkMagMaxRadius; // Magnet outer radius

  static const Float_t fgkDepth; // Alice IP depth from surface

 private:
  ClassDef(AliCRTConstants, 0)   // CRT(ACORDE) global constants
};
#endif // ALICRTCONSTANTS_H
