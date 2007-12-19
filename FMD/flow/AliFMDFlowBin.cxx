/* Copyright (C) 2007 Christian Holm Christensen <cholm@nbi.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
/** @file 
    @brief implementation of a Bin in a Flow histogram */
//____________________________________________________________________
//
// This contains an of class AliFMDFlowHarmonic and an object of
// class AliFMDFlowEventPlane to calculate v_n and \Psi_k.  It contain
// two objects of class AliFMDFlowEventPlane to calculate the
// sub-event event planes Psi_A, \Psi_B.  It also contain 3 objects of
// class AliFMDFlowResolution to calculate the event plane angle
// resolution. 
//
#include "flow/AliFMDFlowBin.h"
#include "flow/AliFMDFlowUtil.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <TBrowser.h>

//====================================================================
AliFMDFlowBin::AliFMDFlowBin(UShort_t order, UShort_t k) 
  : fPsi(order / k), 
    fPsiA(order / k), 
    fPsiB(order / k), 
    fRes(order / k), 
    fResStar(order / k), 
    fResTdr(order / k),
    fHarmonic(order), 
    fSplit("ab_split", "Relative split in A,B sub-event", 2, 0, 2, 100, 0, 1),
    fPhi("phi", "Distribution of #varphi-#Psi", 40, 0, 2*TMath::Pi()), 
    fAB("psiAB", "Distribution of #Psi_{A} vs. #Psi_{B}", 
	40, 0, 2*TMath::Pi()/fPsiA.Order(), 40, 0, 2*TMath::Pi()/fPsiA.Order())
{
  fSplit.SetDirectory(0);
  fSplit.GetXaxis()->SetBinLabel(1, "A");
  fSplit.GetXaxis()->SetBinLabel(2, "B");
  fSplit.SetXTitle("Sub-event");
  fSplit.SetYTitle("Fraction");
  fPhi.SetDirectory(0);
  fPhi.SetXTitle("#varphi");
  fAB.SetDirectory(0);
  fAB.SetXTitle("#Psi_{A}");
  fAB.SetYTitle("#Psi_{B}");
}

//____________________________________________________________________
AliFMDFlowBin::AliFMDFlowBin(const AliFMDFlowBin& o) 
  : TObject(o), 
    fPsi(o.fPsi), 
    fPsiA(o.fPsiA), 
    fPsiB(o.fPsiB), 
    fRes(o.fRes), 
    fResStar(o.fResStar), 
    fResTdr(o.fResTdr), 
    fHarmonic(o.fHarmonic),
    fSplit(o.fSplit), 
    fPhi(o.fPhi), 
    fAB(o.fAB)
{
  // Copy constructor 
  // Parameters: 
  //    o Object to copy from 
  fSplit.SetDirectory(0);
  fSplit.GetXaxis()->SetBinLabel(1, "A");
  fSplit.GetXaxis()->SetBinLabel(2, "B");
  fSplit.SetXTitle("Sub-event");
  fSplit.SetYTitle("Fraction");

  fPhi.SetDirectory(0);
  fPhi.SetXTitle("#varphi");

  fAB.SetDirectory(0);
  fAB.SetXTitle("#Psi_{A}");
  fAB.SetYTitle("#Psi_{B}");
}

//____________________________________________________________________
AliFMDFlowBin&
AliFMDFlowBin::operator=(const AliFMDFlowBin& o) 
{
  // Assignment operator 
  // Parameters: 
  //    o Object to assign from
  fPsi      = o.fPsi;
  fPsiA     = o.fPsiA;
  fPsiB     = o.fPsiB;
  fRes      = o.fRes;
  fResStar  = o.fResStar;
  fResTdr   = o.fResTdr;
  fHarmonic = o.fHarmonic;

  fSplit.Reset();
  fSplit.Add(&(o.fSplit));

  fPhi.Reset();
  fPhi.Add(&(o.fPhi));

  fAB.Reset();
  fAB.Add(&(o.fAB));

  return *this;
}

//____________________________________________________________________
void 
AliFMDFlowBin::Begin() 
{
  // Clear event plane calculators 
  fPsi.Clear();
  fPsiA.Clear();
  fPsiB.Clear();
  fNA = fNB = fN = 0;
}

//____________________________________________________________________
void 
AliFMDFlowBin::AddToEventPlane(Double_t phi, Double_t w, Bool_t a) 
{
  // Called to add a contribution to the event plane 
  // Parameters 
  //    phi   The angle phi in [0,2pi] 
  //    w     Weight
  //    a    If true, add to sub-event A, otherwise to sub-event B.
  fPsi.Add(phi, w);
  if (a) fPsiA.Add(phi, w);
  else   fPsiB.Add(phi, w);
  if (a) fNA++; else fNB++;
  fN++;
}

//____________________________________________________________________
void 
AliFMDFlowBin::AddToHarmonic(Double_t phi, Double_t wp, Double_t wh)
{
  // Called to add a contribution to the harmonic. 
  // Parameters: 
  //   phi   The angle phi in [0,2pi]
  //   wp    Weight of phi (only used in the calculation of  
  //         the event plane).
  //   wh    Weight of observation.
  
  // Disregard the obervation of phi from the event plane angle. 
  Double_t psi   = fPsi.Psi(phi, wp);
  fHarmonic.Add(phi, psi, wh);
  fPhi.Fill(NormalizeAngle(phi-psi));
}

//____________________________________________________________________
void 
AliFMDFlowBin::End()
{
  // Should be called at the end of an event
  fPsi.End();
  fPsiA.End();
  fPsiB.End();
  Double_t psiA = fPsiA.Psi();
  Double_t psiB = fPsiB.Psi();

  // Update the resolutions 
  fRes.Add(psiA, psiB);
  fResStar.Add(psiA, psiB);
  fResTdr.Add(psiA, psiB);
  if (fN != 0) { 
    fSplit.Fill(.5,  float(fNA)/fN);
    fSplit.Fill(1.5, float(fNB)/fN);
  }
  fAB.Fill(psiA, psiB);
}

//____________________________________________________________________
void 
AliFMDFlowBin::Event(UInt_t n, Double_t* phis, Double_t* wp, Double_t* wh) 
{ 
  // Analyse events 
  // Parameters 
  //  n      Size of phis and possibly ws
  //  phis   Array of phi, (phi_1, ..., phi_n)
  //  wp     Weights of event plane (optional)
  //  wh     Weights of harmonic (optional)
  Begin();
  
  // Calculate split. 
  UInt_t split = n / 2;
  // First sub-event. 
  for (UInt_t i = 0; i < split; i++) 
    AddToEventPlane(phis[i], (wp ? wp[i] : 1), kTRUE);
  // Second sub-event. 
  for (UInt_t i = split; i < n; i++) 
    AddToEventPlane(phis[i], (wp ? wp[i] : 1), kFALSE);
  // Add contributions to the harmonic. 
  for (UInt_t i = 0; i < n; i++)     
    AddToHarmonic(phis[i], (wp ? wp[i] : 1), (wh ? wh[i] : 1));

  End();
}

//____________________________________________________________________
Double_t 
AliFMDFlowBin::Value(CorType t) const
{ 
  // Get the value in this bin 
  // Parameters: 
  //   t  Which type of correction
  // 
  // return the value of the harmonic 
  Double_t e;
  return Value(e, t);
}

//____________________________________________________________________
Double_t 
AliFMDFlowBin::EValue(CorType t) const 
{ 
  // Get the value in this bin 
  // Parameters: 
  //    t    Which type of correction 
  // 
  // return the error on the value of the harmonic 
  Double_t e2;
  Value(e2, t);
  return sqrt(e2);
}

//____________________________________________________________________
Double_t 
AliFMDFlowBin::Value(Double_t& e2, CorType t) const
{ 
  // Get the value in this bin 
  // Parameters: 
  //    e2   On return, the square error. 
  //    t    Which type  of correction
  // 
  // return the value of the harmonic 
  Double_t r, er2;
  r = Correction(er2, t);
  return fHarmonic.Value(r, er2, e2);
}

//____________________________________________________________________
Double_t 
AliFMDFlowBin::Correction(Double_t& er2, CorType t) const
{
  // Get the value in this bin 
  // Parameters: 
  //    e2   On return, the square error. 
  //    t    Which type  of correction
  // 
  // return the value of the Correction
  Double_t r = 1;
  UShort_t k = fHarmonic.Order()/fRes.Order();
  switch (t) { 
  case kNaive: r = fRes.Correction(k, er2);     break;
  case kStar:  r = fResStar.Correction(k, er2); break;
  case kTdr:   r = fResTdr.Correction(k, er2);  break;
  default:     r = 1; er2 = 0;                  break;
  }
  return r;
}

//____________________________________________________________________
ULong_t 
AliFMDFlowBin::Counts() const
{
  // Return the number of counts used in this bin.
  // Return:
  //  Number of counts that is used in this bin.
  return fHarmonic.N();
}

//____________________________________________________________________
void 
AliFMDFlowBin::Finish() 
{
  // Called at the end of the event
}

//____________________________________________________________________
void
AliFMDFlowBin::Browse(TBrowser* b) 
{
  // Browse this item
  b->Add(&fPsi,      "Full event plane");
  b->Add(&fPsiA,     "Sub-event A event plane");
  b->Add(&fPsiB,     "Sub-event B event plane");
  b->Add(&fRes,      "Naive resolution");
  b->Add(&fResStar,  "STAR resolution");
  b->Add(&fResTdr,   "TDR resolution");
  b->Add(&fHarmonic, "Harmonic");
  b->Add(&fSplit,    "Split");
  b->Add(&fPhi,      "Phi");
  b->Add(&fAB,       "AB");
}

//____________________________________________________________________
void 
AliFMDFlowBin::Print(Option_t*) const
{
  // Print information 
  Double_t e2v[4], v[4], r[4], e2r[4];
  const char* names[] = { "Bare", "Naive", "STAR", "TDR" };
  v[0] = 100 * Value(e2v[0], AliFMDFlowBin::kNone);
  v[1] = 100 * Value(e2v[1], AliFMDFlowBin::kNaive);
  v[2] = 100 * Value(e2v[2], AliFMDFlowBin::kStar);
  v[3] = 100 * Value(e2v[3], AliFMDFlowBin::kTdr);
  r[0] = 100 * Correction(e2r[0], AliFMDFlowBin::kNone);
  r[1] = 100 * Correction(e2r[1], AliFMDFlowBin::kNaive);
  r[2] = 100 * Correction(e2r[2], AliFMDFlowBin::kStar);
  r[3] = 100 * Correction(e2r[3], AliFMDFlowBin::kTdr);
  
  std::streamsize         oldP = std::cout.precision(3);
  std::ios_base::fmtflags oldF = std::cout.setf(std::ios_base::fixed, 
						std::ios_base::floatfield);
  std::cout << "  v" << std::setw(1) << fHarmonic.Order() << ": ";
  for (UInt_t i = 0; i < 4; i++) 
    std::cout << std::setw(6+(i == 0 ? 0 : 6)) << names[i] << ": " 
	      << std::setw(6) << v[i] << " +/- " 
	      << std::setw(6) << 100*sqrt(e2v[i]) << " ["
	      << std::setw(7) << r[i] << " +/- " 
	      << std::setw(7) << 100*sqrt(e2r[i]) << "]\n";
  std::cout << std::flush;
  std::cout.precision(oldP);
  std::cout.setf(oldF, std::ios_base::floatfield);
}


//____________________________________________________________________
//
// EOF
// 
