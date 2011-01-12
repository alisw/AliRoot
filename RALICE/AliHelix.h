#ifndef ALIHELIX_H
#define ALIHELIX_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

// $Id$

#include "THelix.h"
#include "TObjArray.h"

#include "Ali3Vector.h"

class AliEvent;
class AliTrack;
class AliPosition;
 
class AliHelix : public THelix
{
 public:
  AliHelix();                  // Default constructor
  virtual ~AliHelix()
    {
      // Destructor to delete dynamically allocated memory.
      if (fCurves)
	{
	  delete fCurves;
	  fCurves=0;
	}
      if (fExt)
	{
	  delete fExt;
	  fExt=0;
	}
    }
  AliHelix(const AliHelix& h); // Copy constructor
  void SetB(Ali3Vector& b);    // Set the magnetic field vector in Tesla
  Ali3Vector& GetB();          // Provide the magnetic field vector in Tesla
  void SetTofmax(Float_t tof); // Set maximum time of flight
  Float_t GetTofmax() const;   // Provide the maximum time of flight
  void Display(AliTrack* t,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1);// Show curve for this track
  void Display(AliEvent* e,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1);// Show curves for this event
  void Display(TObjArray* a,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1);// Show curves for this track array
  void Refresh(Int_t mode=0);  // Refresh the view before drawing the next one
  AliPosition* Extrapolate(AliTrack* t,Double_t* pars=0,Double_t scale=-1); // Extrapolate this track
  void MakeCurve(AliTrack* t,Double_t* range=0,Int_t iaxis=3,Double_t scale=-1); // Helix curve for this track
  void SetMarker(Int_t marker=8,Float_t size=0.2,Int_t color=-1); // Set marker for track starting point 
  void UseEndPoint(Int_t mode=1); // (De)Select (0/1) usage of track endpoint for drawing and extrapolation

 protected:
  Ali3Vector fB;       // The magnetic field vector in Tesla
  Float_t fTofmax;     // The maximum time of flight
  Int_t fRefresh;      // Auto-refresh flag for drawings
  Int_t fMstyle;       // The marker style to indicate the track starting point
  Float_t fMsize;      // The size of the marker
  Int_t fMcol;         // The colour of the marker
  Int_t fEnduse;       // Flag to denote tracks endpoint usage
  TObjArray* fCurves;  //! Temp. storage for the curves on the drawing
  AliPosition* fExt;   //! The extrapolation result
 
 ClassDef(AliHelix,3) // Representation and extrapolation of AliTracks in a magnetic field.
};
#endif
