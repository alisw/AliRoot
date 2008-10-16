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

/* $Id: AliTRDgtuParam.cxx 28397 2008-09-02 09:33:00Z cblume $ */

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//  Parameters for GTU simulation                                         //
//                                                                        //
//  Author: J. Klein (Jochen.Klein@cern.ch)                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "TMath.h"
#include "TMatrix.h"
#include "TDecompLU.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"

#include "AliLog.h"
#include "AliTRDgtuParam.h"
#include "AliTRDgeometry.h"
#include "AliTRDpadPlane.h"

ClassImp(AliTRDgtuParam)

AliTRDgtuParam *AliTRDgtuParam::fgInstance = 0;

// ----- Bin widths (granularity) -----
const Float_t 	AliTRDgtuParam::fgkBinWidthY  = 160e-4;
const Float_t 	AliTRDgtuParam::fgkBinWidthdY = 140e-4;

// ----- Bit widths (used for internal representation) -----
const Int_t 	AliTRDgtuParam::fgkBitWidthY      = 13;
const Int_t 	AliTRDgtuParam::fgkBitWidthdY     = 7; 
const Int_t 	AliTRDgtuParam::fgkBitWidthYProj  = 10;
const Int_t 	AliTRDgtuParam::fgkBitExcessY 	  = 4; 
const Int_t 	AliTRDgtuParam::fgkBitExcessAlpha = 10; 
const Int_t 	AliTRDgtuParam::fgkBitExcessYProj = 2; 

// ----- Tracking parameters -----
/*
const Int_t AliTRDgtuParam::fgkNZChannels = 3; // No. of z-channels
const Int_t AliTRDgtuParam::fgkNLinks = 12;	// No. of links
const Int_t AliTRDgtuParam::fgkFixLayer = 2;	// which layer is fixed for the generation of the z-channel map
const Int_t AliTRDgtuParam::fgkDeltaY = 39;	// accepted deviation in y_proj, default: 9
const Int_t AliTRDgtuParam::fgkDeltaAlpha = 31; // accepted deviation in alpha, default: 11
const Int_t AliTRDgtuParam::fgkNRefLayers = 3;	 // no. of reference layers
*/

AliTRDgtuParam::AliTRDgtuParam() :
  fVertexSize(20.0),
  fCurrTrackletMask(0),
  fRefLayers(0x0),
  fGeo(0x0)
{
  // default ctor
  fGeo = new AliTRDgeometry();
  fRefLayers = new Int_t[fgkNRefLayers];
  fRefLayers[0] = 3;
  fRefLayers[1] = 2;
  fRefLayers[2] = 1;
  zChannelGen(); 
}

AliTRDgtuParam::~AliTRDgtuParam() 
{
  // dtor

  delete fGeo;
  delete [] fRefLayers;
}

AliTRDgtuParam* AliTRDgtuParam::Instance() 
{
  // get (or create) the single instance

  if (fgInstance == 0) 
    fgInstance = new AliTRDgtuParam();

  return fgInstance;
}

void AliTRDgtuParam::Terminate() 
{
  // destruct the instance

  if (fgInstance != 0) {
    delete fgInstance;
    fgInstance = 0x0;
  }
}

Bool_t AliTRDgtuParam::IsInZChannel(Int_t stack, Int_t layer, Int_t zchannel, Int_t zpos) const 
{
  return (fZSubChannel[stack][zchannel][layer][zpos] != 0);
}

Int_t AliTRDgtuParam::GetZSubchannel(Int_t stack, Int_t layer, Int_t zchannel, Int_t zpos) const
{
  return fZSubChannel[stack][zchannel][layer][zpos];
}

Int_t AliTRDgtuParam::GetRefLayer(Int_t refLayerIdx) const 
{
  if (refLayerIdx >= 0 && refLayerIdx < fgkNRefLayers)
    return fRefLayers[refLayerIdx];
  else 
    return -1;
}

Int_t AliTRDgtuParam::zChannelGen() 
{
  // generate the z-channel map
  // assuming that the tracks come from the vertex 
  // +/- fVertexSize in z-direction

  Int_t iSec = 0; // sector is irrelevant
  Bool_t collision = kFALSE;

  for (Int_t iStack = 0; iStack < fGeo->Nstack(); iStack++) {

    Float_t X[6] = { 0 };
    Float_t Z[6][16] = {{ 0 }};
    Float_t dZ[6][16] = {{ 0 }};
    
    for (Int_t iLayer = 0; iLayer < fGeo->Nlayer(); iLayer++) {
      AliTRDpadPlane *pp = fGeo->GetPadPlane(iLayer, iStack);
      X[iLayer]  = fGeo->GetTime0(iLayer) - fGeo->CdrHght(); // ???
      for (Int_t iRow = 0; iRow < fGeo->GetRowMax(iLayer, iStack, iSec); iRow++) {
	Z[iLayer][iRow]  = pp->GetRowPos(iRow); // this is the right (pos. z-direction) border of the pad
	dZ[iLayer][iRow] = pp->GetRowSize(iRow); // length of the pad in z-direction
	for (Int_t i = 0; i < fgkNZChannels; i++) 
	    fZSubChannel[iStack][i][iLayer][iRow] = 0;
      }
    }

    for (Int_t fixRow = 0; fixRow < fGeo->GetRowMax(fgkFixLayer, iStack, iSec); fixRow++) {
	
      Double_t fixZmin = Z[fgkFixLayer][fixRow] - dZ[fgkFixLayer][fixRow];  
      Double_t fixZmax = Z[fgkFixLayer][fixRow];
      Double_t fixX    = X[fgkFixLayer] + 1.5; // ??? 1.5 from where? 

      for (Int_t iLayer = 0; iLayer < fGeo->Nlayer(); iLayer++) {
	Double_t leftZ, rightZ;
	
	if (iLayer <= fgkFixLayer) {
	  leftZ  = (fixZmin + fVertexSize) * (X[iLayer] + 1.5) / fixX - fVertexSize;
	  rightZ = (fixZmax - fVertexSize) * (X[iLayer] + 1.5) / fixX + fVertexSize;
	}
	else {
	  leftZ  = (fixZmin - fVertexSize) * (X[iLayer] + 1.5) / fixX + fVertexSize;
	  rightZ = (fixZmax + fVertexSize) * (X[iLayer] + 1.5) / fixX - fVertexSize;
	}
	
	Double_t epsilon = 0.001;
	for (Int_t iRow = 0; iRow < fGeo->GetRowMax(iLayer, iStack, iSec); iRow++) {
	  if ( (Z[iLayer][iRow] )                    > (leftZ  + epsilon) && 
	       (Z[iLayer][iRow] - dZ[iLayer][iRow] ) < (rightZ - epsilon) ) {
	    fZChannelMap[iStack][fixRow][iLayer][iRow] = 1;
	    if (fZSubChannel[iStack][fixRow % fgkNZChannels][iLayer][iRow] != 0) {
	      AliError("Collision in Z-Channel assignment occured! No reliable tracking!!!");
	      collision = kTRUE;
	    }
	    else 
	      fZSubChannel[iStack][fixRow % fgkNZChannels][iLayer][iRow] = fixRow / fgkNZChannels + 1;
	  }

	}
      }
    }
  }

  return ~collision;
}

Bool_t AliTRDgtuParam::DisplayZChannelMap(Int_t zchannel, Int_t subchannel) const 
{
  // display the z-channel map 

  if (zchannel > fgkNZChannels) {
    AliError("Invalid Z channel!");
    return kFALSE;
  }

  Int_t zchmin = zchannel >= 0 ? zchannel : 0;
  Int_t zchmax = zchannel >= 0 ? zchannel + 1 : fgkNZChannels;
  Int_t i = 0;
  Int_t j = 0;
  TCanvas *c = new TCanvas("zchmap", "Z-Chhannel Mapping");
  c->cd();
  TGraph **graphz = new TGraph*[fgkNZChannels];
  for (Int_t zch = zchmin; zch < zchmax; zch++) 
    graphz[zch] = new TGraph;
  TGraphAsymmErrors *graph = new TGraphAsymmErrors();
  graph->SetTitle("Z-Channel Map");
  graph->SetPoint(i, 0, 0); // vertex
  graph->SetPointError(i++, 20, 20, 0, 0);
  //  graph->SetRange //????
  for (Int_t iLayer = 0; iLayer < fGeo->Nlayer(); iLayer++) {
    for (Int_t iStack = 0; iStack < fGeo->Nstack(); iStack++) {
      AliTRDpadPlane *pp = fGeo->GetPadPlane(iLayer, iStack);
      for (Int_t iRow = 0; iRow < fGeo->GetRowMax(iLayer, iStack, 0); iRow++) {
	graph->SetPoint(i, pp->GetRowPos(iRow), fGeo->GetTime0(iLayer) - fGeo->CdrHght());
	graph->SetPointError(i++, pp->GetRowSize(iRow), 0, 0, 0);
	for (Int_t zch = zchmin; zch < zchmax; zch++)
	  if (fZSubChannel[iStack][zch][iLayer][iRow] != 0)
	    if (subchannel == 0 || fZSubChannel[iStack][zch][iLayer][iRow] == subchannel)
	      graphz[zch]->SetPoint(j++, pp->GetRowPos(iRow)  - pp->GetRowSize(iRow)/2, fGeo->GetTime0(iLayer) - fGeo->CdrHght());
      }
    }
  }
  graph->SetMarkerStyle(kDot);
  graph->Draw("AP");
  for (Int_t zch = zchmin; zch < zchmax; zch++) {
    graphz[zch]->SetMarkerStyle(kCircle);
    graphz[zch]->SetMarkerColor(zch+2);
    graphz[zch]->SetMarkerSize(0.3 + zch*0.2);
    graphz[zch]->Draw("P");
  }
  return kTRUE;
}

Int_t AliTRDgtuParam::GetCiAlpha(Int_t layer) const 
{
  // get the constant for the calculation of alpha

  Int_t Ci = (Int_t) (GetChamberThickness() / fGeo->GetTime0(layer) * GetBinWidthY() / GetBinWidthdY() * (1 << (GetBitExcessAlpha() + GetBitExcessY() + 1)) );
  return Ci;
}

Int_t AliTRDgtuParam::GetCiYProj(Int_t layer) const 
{
  // get the constant for the calculation of y_proj

  Float_t Xmid = (fGeo->GetTime0(0) + fGeo->GetTime0(fGeo->Nlayer()-1)) / 2.; 
  Int_t Ci = (Int_t) (- (fGeo->GetTime0(layer) - Xmid) / GetChamberThickness() * GetBinWidthdY() / GetBinWidthY() * (1 << GetBitExcessYProj()) );
  return Ci;
}

Int_t AliTRDgtuParam::GetYt(Int_t stack, Int_t layer, Int_t zrow) const
{
    return (Int_t) (- ( (layer % 2 ? 1 : -1) * 
			(GetGeo()->GetPadPlane(layer, stack)->GetRowPos(zrow) - GetGeo()->GetPadPlane(layer, stack)->GetRowSize(zrow) / 2) * 
			TMath::Tan(- 2.0 / 180.0 * TMath::Pi()) ) / 0.016 );
}

Bool_t AliTRDgtuParam::GenerateRecoCoefficients(Int_t trackletMask) 
{
  fCurrTrackletMask = trackletMask;

  TMatrix a(GetNLayers(), 3);
  TMatrix b(3, GetNLayers());
  TMatrix c(3, 3);

  for (Int_t layer = 0; layer < GetNLayers(); layer++) {
      if ( (trackletMask & (1 << layer)) == 0) {
	  a(layer, 0) = 0;
	  a(layer, 1) = 0;
	  a(layer, 2) = 0;
      } 
      else {
	  a(layer, 0) = 1;
	  a(layer, 1) = fGeo->GetTime0(layer);
	  a(layer, 2) = (layer % 2 ? 1 : -1) * fGeo->GetTime0(layer);
      }
  }

  b.Transpose(a);
  c = b * a;
  c.InvertFast();
  b = c * b;

  for (Int_t layer = 0; layer < GetNLayers(); layer++) {
      fAki[layer] = b.GetMatrixArray()[layer];
      fBki[layer] = b.GetMatrixArray()[GetNLayers() + layer];
      fCki[layer] = b.GetMatrixArray()[2 * GetNLayers() + layer];
    }
  return kTRUE;
}

Float_t AliTRDgtuParam::GetAki(Int_t k, Int_t i) 
{
  // get A_ki for the calculation of the tracking parameters
  if (fCurrTrackletMask != k)
    GenerateRecoCoefficients(k);

  return fAki[i];
}

Float_t AliTRDgtuParam::GetBki(Int_t k, Int_t i) 
{
  // get B_ki for the calculation of the tracking parameters

  if (fCurrTrackletMask != k)
    GenerateRecoCoefficients(k);

  return fBki[i];
}

Float_t AliTRDgtuParam::GetCki(Int_t k, Int_t i) 
{
  // get B_ki for the calculation of the tracking parameters

  if (fCurrTrackletMask != k)
    GenerateRecoCoefficients(k);

  return fCki[i];
}

/*
Float_t AliTRDgtuParam::GetD(Int_t k) const 
{
  // get the determinant for the calculation of the tracking parameters

  TMatrix t(3, 3);
  for (Int_t i = 0; i < GetNLayers(); i++) {
    if ( !((k >> i) & 0x1) )
      continue;
    Float_t xi = fGeo->GetTime0(i);
    t(0,0) += 1;
    t(1,0) += xi;
    t(2,0) += TMath::Power(-1, i) * xi;
    t(0,1) += xi;
    t(1,1) += TMath::Power(xi, 2);
    t(2,1) += TMath::Power(-1, i) * TMath::Power(xi, 2);
    t(0,2) += TMath::Power(-1, i) * xi;
    t(1,2) += TMath::Power(-1, i) * TMath::Power(xi, 2);
    t(2,2) += TMath::Power(xi, 2);
  }
  return t.Determinant();
}

Bool_t AliTRDgtuParam::GetFitParams(TVectorD& rhs, Int_t k) 
{
  // calculate the fitting parameters
  // will be changed!

  TMatrix t(3,3);
  for (Int_t i = 0; i < GetNLayers(); i++) {
    if ( !((k >> i) & 0x1) )
      continue;
    Float_t xi = fGeo->GetTime0(i);
    t(0,0) += 1;
    t(1,0) += xi;
    t(2,0) += TMath::Power(-1, i) * xi;
    t(0,1) += xi;
    t(1,1) += TMath::Power(xi, 2);
    t(2,1) += TMath::Power(-1, i) * TMath::Power(xi, 2);
    t(0,2) -= TMath::Power(-1, i) * xi;
    t(1,2) -= TMath::Power(-1, i) * TMath::Power(xi, 2);
    t(2,2) -= TMath::Power(xi, 2);
  }
  TDecompLU lr(t);
  lr.Solve(rhs);
  return lr.Decompose();
}
*/

Bool_t AliTRDgtuParam::GetIntersectionPoints(Int_t k, Float_t &x1, Float_t &x2) 
{
  // get the x-coord. of the assumed circle/straight line intersection points

  Int_t l1 = -1;
  Int_t l2 = -1;
  Int_t nHits = 0;
  for (Int_t layer = 0; layer < GetNLayers(); layer++) {
    if ( (k >> layer) & 0x1 ) {
      if (l1 < 0) 
	l1 = layer;
      l2 = layer;
      nHits++;
    }
  }

  x1 = fGeo->GetTime0(l1) + 10./6 * (nHits -1);
  x2 = fGeo->GetTime0(l2) - 10./6 * (nHits -1);

  return ( (l1 >= 0) && (l2 >= 0) );
}

Float_t AliTRDgtuParam::GetRadius(Int_t a, Float_t b, Float_t x1, Float_t x2) 
{
  // get the radius for the track
  Float_t d = (1 + b * b /2 ) * (x2 - x1);
  Float_t c1 = x1 * x2 / 2;
//  Float_t c2 = (x1 + x2) / (x1 * x2);
  printf("c1: %f\n", c1);
  Float_t r = (375. / 10000.) * c1 * 256 / (a >> 1);
  return r;

  Float_t y1 = a + b*x1;
  Float_t y2 = a + b*x2;
  Float_t alpha = TMath::Abs( TMath::ATan(y2/x2) - TMath::ATan(y1/x1) );
  d = TMath::Sqrt( TMath::Power(x2-x1, 2) + TMath::Power(y2-y1, 2) );
  r = d / 2. / TMath::Sin(alpha);
  return r;
}
