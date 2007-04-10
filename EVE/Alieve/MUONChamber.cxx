#include "MUONChamber.h"

#include <Alieve/MUONData.h>
#include <Alieve/MUONChamberData.h>

#include <TBuffer3D.h>
#include <TBuffer3DTypes.h>
#include <TVirtualPad.h>
#include <TVirtualViewer3D.h>

#include <TStyle.h>
#include <TColor.h>
#include <TMath.h>

using namespace Reve;
using namespace Alieve;

//______________________________________________________________________
// MUONChamber
//

ClassImp(MUONChamber)

//______________________________________________________________________
MUONChamber::MUONChamber(Int_t id, const Text_t* n, const Text_t* t) :
Reve::RenderElement(fFrameColor),
TNamed(n,t),
fMUONData(0),
fFrameColor((Color_t)2),
fRTS(1),
fChamberID(0),
fQuadSet1(n,t),
fQuadSet2(n,t),
fPointSet1(n),
fPointSet2(n),
fThreshold(0),
fMaxVal(4096),
fClusterSize(5),
fHitSize(5)
{
  //
  // constructor
  //

  Char_t name[256];
  if (id < 10) {
    sprintf(name,"Chamber %02d (trac)",id);
  } else {
    sprintf(name,"Chamber %02d (trig)",id);
  }
  SetName(name);

  ComputeBBox();

}

//______________________________________________________________________
MUONChamber::~MUONChamber()
{
  //
  // destructor
  //

  if(fMUONData) fMUONData->DecRefCount();

}

//______________________________________________________________________
void MUONChamber::ComputeBBox()
{
  //
  // bounding box
  //

#if ROOT_VERSION_CODE <= ROOT_VERSION(5,11,2)
  bbox_init();
#else
  BBoxInit();
#endif
  
  fBBox[0] = - 400.0;
  fBBox[1] = + 400.0;
  fBBox[2] = - 400.0;
  fBBox[3] = + 400.0;
  fBBox[4] = -1800.0;
  fBBox[5] = + 500.0;

  Float_t* b1 = fQuadSet1.AssertBBox();
  for(Int_t i=0; i<6; ++i) { b1[i] = fBBox[i]; }
  Float_t* b2 = fQuadSet2.AssertBBox();
  for(Int_t i=0; i<6; ++i) { b2[i] = fBBox[i]; }
  Float_t* b3 = fPointSet1.AssertBBox();
  for(Int_t i=0; i<6; ++i) { b3[i] = fBBox[i]; }
  Float_t* b4 = fPointSet2.AssertBBox();
  for(Int_t i=0; i<6; ++i) { b4[i] = fBBox[i]; }
  
}

//______________________________________________________________________
void MUONChamber::Paint(Option_t*)
{
  //
  // draw...
  //

  if(fRnrSelf == kFALSE)
    return;

  TBuffer3D buffer(TBuffer3DTypes::kGeneric);

  buffer.fID           = this;
  buffer.fColor        = 2;
  buffer.fTransparency = 0;
  buffer.fLocalFrame   = 0;

  buffer.SetSectionsValid(TBuffer3D::kCore);
  Int_t reqSections = gPad->GetViewer3D()->AddObject(buffer);
  if (reqSections == TBuffer3D::kNone) {
    //printf("MUONChamber::Paint viewer was happy with Core buff3d.\n");
    return;
  }

  printf("MUONChamber::Paint only GL supported.\n");
  return;

}

//______________________________________________________________________
void MUONChamber::SetThreshold(Short_t t)
{
  //
  // digits amplitude threshold
  //

  fThreshold = TMath::Min(t, (Short_t)(fMaxVal - 1));
  ClearColorArray();
  IncRTS();

}

//______________________________________________________________________
void MUONChamber::SetMaxVal(Int_t mv)
{
  //
  // digits amplitude maximum value
  //

  fMaxVal = TMath::Max(mv, (Int_t)(fThreshold + 1));
  ClearColorArray();
  IncRTS();

}

//______________________________________________________________________
void MUONChamber::SetClusterSize(Int_t size)
{
  //
  // cluster point size
  //

  fClusterSize = TMath::Max(1, size);
  IncRTS();

}

//______________________________________________________________________
void MUONChamber::SetHitSize(Int_t size)
{
  //
  // hit point size
  //

  fHitSize = TMath::Max(1, size);
  IncRTS();

}

//______________________________________________________________________
void MUONChamber::SetupColor(Int_t val, UChar_t* pixel) const
{
  //
  // RGBA color for amplitude "val"
  //

  Float_t div  = TMath::Max(1, fMaxVal - fThreshold);
  Int_t   nCol = gStyle->GetNumberOfColors();
  Int_t   cBin = (Int_t) TMath::Nint(nCol*(val - fThreshold)/div);

  ColorFromIdx(gStyle->GetColorPalette(TMath::Min(nCol - 1, cBin)), pixel);

}

//______________________________________________________________________
Int_t MUONChamber::ColorIndex(Int_t val) const
{
  //
  // index color 
  //

  if(val < fThreshold) val = fThreshold;
  if(val > fMaxVal)    val = fMaxVal;

  Float_t div  = TMath::Max(1, fMaxVal - fThreshold);
  Int_t   nCol = gStyle->GetNumberOfColors();
  Int_t   cBin = (Int_t) TMath::Nint(nCol*(val - fThreshold)/div);

  return gStyle->GetColorPalette(TMath::Min(nCol - 1, cBin));

}

//______________________________________________________________________
void MUONChamber::SetupColorArray() const
{
  //
  // build array of colors
  //

  if(fColorArray)
    return;

  fColorArray = new UChar_t [4 * (fMaxVal - fThreshold + 1)];
  UChar_t* p = fColorArray;
  for(Int_t v=fThreshold; v<=fMaxVal; ++v, p+=4)
    SetupColor(v, p);

}

//______________________________________________________________________
void MUONChamber::ClearColorArray()
{
  //
  // delete array of colors
  //

  if(fColorArray) {
    delete [] fColorArray;
    fColorArray = 0;
  }
}

//______________________________________________________________________
void MUONChamber::SetDataSource(MUONData* data)
{

  if (data == fMUONData) return;
  if(fMUONData) fMUONData->DecRefCount();
  fMUONData = data;
  if(fMUONData) fMUONData->IncRefCount();
  IncRTS();

}

//______________________________________________________________________
MUONChamberData* MUONChamber::GetChamberData() const
{
  
  return fMUONData ? fMUONData->GetChamberData(fChamberID) : 0;

}

//______________________________________________________________________
void MUONChamber::UpdateQuads()
{

  fQuadSet1.Quads().clear();
  fQuadSet2.Quads().clear();
  fPointSet1.Reset();
  fPointSet2.Reset();

  MUONChamberData* data = GetChamberData();
  
  Float_t *buffer;
  Float_t x0, y0, x1, y1, z, clsq;
  Int_t charge, cathode, nDigits, nClusters, nHits, oldSize, ic1, ic2;
  Double_t clsX, clsY, clsZ;
  Float_t hitX, hitY, hitZ;
    
  if (data != 0) {

    SetupColorArray();

    // digits

    nDigits = data->GetNDigits();
    
    for (Int_t id = 0; id < nDigits; id++) {

      buffer = data->GetDigitBuffer(id);

      x0 = buffer[0]-buffer[2];
      y0 = buffer[1]-buffer[3];
      x1 = buffer[0]+buffer[2];
      y1 = buffer[1]+buffer[3];
      z  = buffer[4];
      charge = (Int_t)buffer[5];
      cathode = (Int_t)buffer[6];
      
      if (charge <= fThreshold) continue;

      if (cathode == 0) {

	fQuadSet1.Quads().push_back(Reve::Quad());
	
	fQuadSet1.Quads().back().ColorFromIdx(ColorIndex(charge));
	//ColorFromArray(charge,(UChar_t*)&fQuadSet1.fQuads.back().color);
	
	//UChar_t* c = (UChar_t*)&fQuadSet1.fQuads.back().color; 
	//printf("%d %d %d %d \n",c[0],c[1],c[2],c[3]);
	
	Float_t* p = fQuadSet1.Quads().back().vertices;
	
	p[0] = x0;  p[1] = y0;  p[2] = z;  p += 3;
	p[0] = x1;  p[1] = y0;  p[2] = z;  p += 3;
	p[0] = x1;  p[1] = y1;  p[2] = z;  p += 3;
	p[0] = x0;  p[1] = y1;  p[2] = z;  p += 3;
	
      }

      if (cathode == 1) {

	fQuadSet2.Quads().push_back(Reve::Quad());
	
	fQuadSet2.Quads().back().ColorFromIdx(ColorIndex(charge));
	//ColorFromArray(charge,(UChar_t*)&fQuadSet2.fQuads.back().color);
	
	//UChar_t* c = (UChar_t*)&fQuadSet2.fQuads.back().color; 
	//printf("%d %d %d %d \n",c[0],c[1],c[2],c[3]);
	
	Float_t* p = fQuadSet2.Quads().back().vertices;
	
	p[0] = x0;  p[1] = y0;  p[2] = z;  p += 3;
	p[0] = x1;  p[1] = y0;  p[2] = z;  p += 3;
	p[0] = x1;  p[1] = y1;  p[2] = z;  p += 3;
	p[0] = x0;  p[1] = y1;  p[2] = z;  p += 3;
	
      }

    } // end digits loop

    // clusters
    
    nClusters = data->GetNClusters()/2;  // only one cathode plane
    oldSize = fPointSet1.GrowFor(nClusters);
    ic1 = ic2 = 0;
    for (Int_t ic = 0; ic < (nClusters*2); ic++) {

      buffer = data->GetClusterBuffer(ic);

      clsX    = (Double_t)buffer[0];
      clsY    = (Double_t)buffer[1];
      clsZ    = (Double_t)buffer[2];
      clsq    = buffer[3];      
      cathode = (Int_t)buffer[4];

      if (cathode == 0) {
	fPointSet1.SetPoint(ic1,clsX,clsY,clsZ);
	ic1++;
      }

    } // end clusters loop

    // hits

    nHits = data->GetNHits();
    oldSize = fPointSet2.GrowFor(nHits);
    for (Int_t ih = 0; ih < nHits; ih++) {
      buffer = data->GetHitBuffer(ih);
      hitX = buffer[0];
      hitY = buffer[1];
      hitZ = buffer[2];
      fPointSet2.SetPoint(ih,hitX,hitY,hitZ);
    }

  } // end data

}

//______________________________________________________________________
void MUONChamber::SetChamberID(Int_t id)
{

  if (id <  0) id = 0;
  if (id > 13) id = 13;

  fChamberID = id;
  IncRTS();

}

