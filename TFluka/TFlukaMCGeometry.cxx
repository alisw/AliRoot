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

// Class TFlukaMCGeometry
// --------------------
// Implementation of the TVirtualMCGeometry interface
// for defining and using TGeo geometry with FLUKA.
// This allows the FLUKA MonteCarlo to run with the TGeo 
// geometrical modeller
// Author: Andrei Gheata 10/07/2003

#include "Riostream.h"

#include "TCallf77.h"
#include "TFluka.h"
#include "TFlukaMCGeometry.h"
#include "TGeoManager.h" 
#include "TGeoVolume.h" 
#include "TObjString.h"

#ifndef WIN32 
# define idnrwr idnrwr_
# define g1wr   g1wr_
# define g1rtwr g1rtwr_
# define conhwr conhwr_
# define inihwr inihwr_
# define jomiwr jomiwr_
# define lkdbwr lkdbwr_
# define lkfxwr lkfxwr_
# define lkmgwr lkmgwr_
# define lkwr lkwr_
# define magfld magfld_
# define nrmlwr nrmlwr_
# define rgrpwr rgrpwr_
# define isvhwr isvhwr_

#else

# define idnrwr IDNRWR
# define g1wr   G1WR
# define g1rtwr G1RTWR
# define conhwr CONHWR
# define inihwr INIHWR
# define jomiwr JOMIWR
# define lkdbwr LKDBWR
# define lkfxwr LKFXWR
# define lkmgwr LKMGWR
# define lkwr   LKWR
# define magfld MAGFLD
# define nrmlwr NRMLWR
# define rgrpwr RGRPWR
# define isvhwr ISVHWR

#endif

//____________________________________________________________________________ 
extern "C" 
{
   //
   // Prototypes for FLUKA navigation methods
   //
   Int_t type_of_call idnrwr(const Int_t & /*nreg*/, const Int_t & /*mlat*/);
   void  type_of_call   g1wr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/, 
                             Double_t * /*pV*/,  Int_t & /*oldReg*/ , const Int_t & /*oldLttc*/, Double_t & /*propStep*/,
                             Int_t & /*nascFlag*/, Double_t & /*retStep*/, Int_t & /*newReg*/,
	                          Double_t & /*saf*/, Int_t & /*newLttc*/, Int_t & /*LttcFlag*/,
                             Double_t *s /*Lt*/, Int_t * /*jrLt*/);
   
   void  type_of_call g1rtwr();
   void  type_of_call conhwr(Int_t & /*intHist*/, Int_t * /*incrCount*/); 
   void  type_of_call inihwr(Int_t & /*intHist*/);
   void  type_of_call jomiwr(const Int_t & /*nge*/, const Int_t & /*lin*/, const Int_t & /*lou*/,
                             Int_t & /*flukaReg*/);
   void  type_of_call lkdbwr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/,
                             Double_t * /*pV*/, const Int_t & /*oldReg*/, const Int_t & /*oldLttc*/,
                             Int_t & /*newReg*/, Int_t & /*flagErr*/, Int_t & /*newLttc*/);
   void  type_of_call lkfxwr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/,
                             Double_t * /*pV*/, const Int_t & /*oldReg*/, const Int_t & /*oldLttc*/,
                             Int_t & /*newReg*/, Int_t & /*flagErr*/, Int_t & /*newLttc*/);
   void  type_of_call lkmgwr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/,
                             Double_t * /*pV*/, const Int_t & /*oldReg*/, const Int_t & /*oldLttc*/,
		                       Int_t & /*flagErr*/, Int_t & /*newReg*/, Int_t & /*newLttc*/);
   void  type_of_call   lkwr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/,
                             Double_t * /*pV*/, const Int_t & /*oldReg*/, const Int_t & /*oldLttc*/,
	                          Int_t & /*newReg*/, Int_t & /*flagErr*/, Int_t & /*newLttc*/);
//   void  type_of_call magfld(const Double_t & /*pX*/, const Double_t & /*pY*/, const Double_t & /*pZ*/,
//                             Double_t & /*cosBx*/, Double_t & /*cosBy*/, Double_t & /*cosBz*/, 
//                             Double_t & /*Bmag*/, Int_t & /*reg*/, Int_t & /*idiscflag*/);	    
   void  type_of_call nrmlwr(Double_t & /*pSx*/, Double_t & /*pSy*/, Double_t & /*pSz*/,
                             Double_t & /*pVx*/, Double_t & /*pVy*/, Double_t & /*pVz*/,
	                          Double_t * /*norml*/, const Int_t & /*oldReg*/, 
	                          const Int_t & /*newReg*/, Int_t & /*flagErr*/);
   void  type_of_call rgrpwr(const Int_t & /*flukaReg*/, const Int_t & /*ptrLttc*/, Int_t & /*g4Reg*/,
                             Int_t * /*indMother*/, Int_t * /*repMother*/, Int_t & /*depthFluka*/);
   Int_t type_of_call isvhwr(const Int_t & /*fCheck*/, const Int_t & /*intHist*/);
};
   
// TFluka global pointer
TFluka *gFluka = 0;
TFlukaMCGeometry *gMCGeom = 0;
Int_t gNstep = 0;

ClassImp(TFlukaMCGeometry)

TFlukaMCGeometry* TFlukaMCGeometry::fgInstance=0;

//_____________________________________________________________________________
TFlukaMCGeometry::TFlukaMCGeometry(const char *name, const char *title) 
                 : TNamed(name, title)
{
  //
  // Standard constructor
  //
  fDebug        = kFALSE;
  fLastMaterial = 0;
  fNextRegion   = 0;
  fNextLattice  = 0;
  fRegionList   = 0;
  gFluka = (TFluka*)gMC;
  gMCGeom = this;
  gNstep = 0;
  fMatList = new TObjArray(256);
  fMatNames = new TObjArray(256);
}

//_____________________________________________________________________________
TFlukaMCGeometry::TFlukaMCGeometry()
{    
  //
  // Default constructor
  //
  fDebug        = kFALSE;
  fLastMaterial = 0;
  fNextRegion   = 0;
  fNextLattice  = 0;
  fRegionList   = 0;
  gFluka = (TFluka*)gMC;
  gMCGeom = this;
  gNstep = 0;
  fMatList = 0;
  fMatNames = 0;
}

//_____________________________________________________________________________
TFlukaMCGeometry::~TFlukaMCGeometry() 
{
  //
  // Destructor
  //
  fgInstance=0;
  if (fRegionList) delete [] fRegionList;
  if (fMatList) delete fMatList;
  if (fMatNames) {fMatNames->Delete(); delete fMatNames;}
  if (gGeoManager) delete gGeoManager;
}

//
// private methods
//
//_____________________________________________________________________________
TFlukaMCGeometry::TFlukaMCGeometry(const TFlukaMCGeometry &)
  : TNamed()
{    
  //
  // Copy constructor
  //
}

//_____________________________________________________________________________
Double_t* TFlukaMCGeometry::CreateDoubleArray(Float_t* array, Int_t size) const
{
// Converts Float_t* array to Double_t*,
// !! The new array has to be deleted by user.
// ---

  Double_t* doubleArray;
  if (size>0) {
    doubleArray = new Double_t[size]; 
    for (Int_t i=0; i<size; i++) doubleArray[i] = array[i];
  }
  else {
    //doubleArray = 0; 
    doubleArray = new Double_t[1]; 
  }  
  return doubleArray;
}
//
// public methods


//_____________________________________________________________________________
Int_t TFlukaMCGeometry::GetMedium() const
{
// Get current medium number
   Int_t imed = 0;
   TGeoNode *node = gGeoManager->GetCurrentNode();
   if (!node) imed = gGeoManager->GetTopNode()->GetVolume()->GetMedium()->GetId();
   else       imed = node->GetVolume()->GetMedium()->GetId();
   if (fDebug) printf("GetMedium=%i\n", imed);
   return imed;
}

//_____________________________________________________________________________
Int_t TFlukaMCGeometry::GetFlukaMaterial(Int_t imed) const
{
// Returns FLUKA material index for medium IMED
   TGeoMedium *med = (TGeoMedium*)gGeoManager->GetListOfMedia()->At(imed-1);
   if (!med) {
      Error("GetFlukaMaterial", "MEDIUM %i nor found", imed);
      return -1;
   }
   Int_t imatfl = med->GetMaterial()->GetIndex();
   return imatfl;   
}

//_____________________________________________________________________________
Int_t *TFlukaMCGeometry::GetRegionList(Int_t imed, Int_t &nreg)
{
// Get an ordered list of regions matching a given medium number
   nreg = 0;
   if (!fRegionList) fRegionList = new Int_t[NofVolumes()+1];
   TIter next(gGeoManager->GetListOfUVolumes());
   TGeoVolume *vol;
   Int_t imedium, ireg;
   while ((vol = (TGeoVolume*)next())) {
      imedium = vol->GetMedium()->GetId();
      if (imedium == imed) {
         ireg = vol->GetNumber();
         fRegionList[nreg++] = ireg;
      }
   }
   return fRegionList;
}         

//_____________________________________________________________________________
Int_t *TFlukaMCGeometry::GetMaterialList(Int_t imat, Int_t &nreg)
{
// Get an ordered list of regions matching a given medium number
   nreg = 0;
   if (!fRegionList) fRegionList = new Int_t[NofVolumes()+1];
   TIter next(gGeoManager->GetListOfUVolumes());
   TGeoVolume *vol;
   Int_t imaterial, ireg;
   while ((vol = (TGeoVolume*)next())) {
      imaterial = vol->GetMedium()->GetMaterial()->GetIndex();
      if (imaterial == imat) {
         ireg = vol->GetNumber();
         fRegionList[nreg++] = ireg;
      }
   }
   return fRegionList;
}         
 
//_____________________________________________________________________________
Int_t TFlukaMCGeometry::NofVolumes() const 
{
  //
  // Return total number of volumes in the geometry
  //

  return gGeoManager->GetListOfUVolumes()->GetEntriesFast()-1;
}
  
//_____________________________________________________________________________
TGeoMaterial * TFlukaMCGeometry::GetMakeWrongMaterial(Double_t z)
{
// Try to replace a wrongly-defined material
   static Double_t kz[23] = {7.3, 17.8184, 7.2167, 10.856, 8.875, 8.9, 7.177,
      25.72, 6.2363, 7.1315, 47.7056, 10.6467, 7.8598, 2.10853, 10.6001, 9.1193, 
      15.3383, 4.55,   9.6502, 6.4561, 21.7963, 29.8246, 15.4021};

   Int_t ind;
   Double_t dz;
   for (ind=0; ind<23; ind++) {
      dz = TMath::Abs(z-kz[ind]);
      if (dz<1E-4) break;
   }
   if (ind>22) {
      printf("Cannot patch material with Z=%g\n", z);
      return 0;
   }
   TGeoMixture *mix = 0;
   TGeoElement *element;
   TGeoElementTable *table = TGeoElementTable::Instance();
   switch (ind) {
      case 0: // AIR
         mix = new TGeoMixture("AIR", 4, 0.001205);
         element = table->GetElement(6); // C
         mix->DefineElement(0, element, 0.000124);
         element = table->GetElement(7); // N
         mix->DefineElement(1, element, 0.755267);
         element = table->GetElement(8); // O
         mix->DefineElement(2, element, 0.231781);
         element = table->GetElement(18); // AR
         mix->DefineElement(3, element, 0.012827);
         break;
      case 1: //SDD SI CHIP
         mix = new TGeoMixture("SDD_SI", 6, 2.4485);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.004367771);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.039730642);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.001396798);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.01169634);
         element = table->GetElement(14);
         mix->DefineElement(4, element, 0.844665);
         element = table->GetElement(47);
         mix->DefineElement(5, element, 0.09814344903);
         break;
      case 2:  // WATER
         mix = new TGeoMixture("WATER", 2, 1.0);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.111898344);
         element = table->GetElement(8);
         mix->DefineElement(1, element, 0.888101656);
         break;
      case 3: // CERAMICS
         mix = new TGeoMixture("CERAMICS", 5, 3.6);
         element = table->GetElement(8);
         mix->DefineElement(0, element, 0.59956);
         element = table->GetElement(13);
         mix->DefineElement(1, element, 0.3776);
         element = table->GetElement(14);
         mix->DefineElement(2, element, 0.00933);
         element = table->GetElement(24);
         mix->DefineElement(3, element, 0.002);
         element = table->GetElement(25);
         mix->DefineElement(4, element, 0.0115);
         break;
      case 4: // EPOXY
         mix = new TGeoMixture("G10FR4", 4, 1.8);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.19);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.18);
         element = table->GetElement(8);
         mix->DefineElement(2, element, 0.35);
         element = table->GetElement(14);
         mix->DefineElement(3, element, 0.28);
         break;
      case 5: // EPOXY
         mix = new TGeoMixture("G10FR4", 4, 1.8);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.19);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.18);
         element = table->GetElement(8);
         mix->DefineElement(2, element, 0.35);
         element = table->GetElement(14);
         mix->DefineElement(3, element, 0.28);
         break;
      case 6: // KAPTON
         mix = new TGeoMixture("KAPTON", 4, 1.3);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.026363415);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.6911272);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.073271325);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.209238060);
         break;
      case 7: // INOX
         mix = new TGeoMixture("INOX", 9, 7.9);
         element = table->GetElement(6);
         mix->DefineElement(0, element, 0.0003);
         element = table->GetElement(14);
         mix->DefineElement(1, element, 0.01);          
         element = table->GetElement(15);
         mix->DefineElement(2, element, 0.00045);
         element = table->GetElement(16);
         mix->DefineElement(3, element, 0.0003);
         element = table->GetElement(24);
         mix->DefineElement(4, element, 0.17);
         element = table->GetElement(25);
         mix->DefineElement(5, element, 0.02);
         element = table->GetElement(26);
         mix->DefineElement(6, element, 0.654);
         element = table->GetElement(28);
         mix->DefineElement(7, element, 0.12);
         element = table->GetElement(42);
         mix->DefineElement(8, element, 0.025);
         break;
      case 8: // ROHACELL
         mix = new TGeoMixture("ROHACELL", 4, 0.05);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.07836617);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.64648941);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.08376983);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.19137459);
         break;
      case 9: // SDD-C-AL
         mix = new TGeoMixture("SDD-C-AL", 5, 1.9837);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.022632);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.8176579);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.0093488);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.0503618);
         element = table->GetElement(13);
         mix->DefineElement(4, element, 0.1);
         break;
      case 10: // X7R-CAP
         mix = new TGeoMixture("X7R-CAP", 7, 6.72);
         element = table->GetElement(8);
         mix->DefineElement(0, element, 0.085975822);
         element = table->GetElement(22);
         mix->DefineElement(1, element, 0.084755042);
         element = table->GetElement(28);
         mix->DefineElement(2, element, 0.038244751);
         element = table->GetElement(29);
         mix->DefineElement(3, element, 0.009471271);
         element = table->GetElement(50);
         mix->DefineElement(4, element, 0.321736471);
         element = table->GetElement(56);
         mix->DefineElement(5, element, 0.251639432);
         element = table->GetElement(82);
         mix->DefineElement(6, element, 0.2081768);
         break;
      case 11: // SDD ruby sph. Al2O3
         mix = new TGeoMixture("AL2O3", 2, 3.97);
         element = table->GetElement(8);
         mix->DefineElement(0, element, 0.5293);
         element = table->GetElement(13);
         mix->DefineElement(1, element, 0.4707);
         break;
      case 12: // SDD HV microcable
         mix = new TGeoMixture("HV-CABLE", 5, 1.6087);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.01983871336);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.520088819984);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.0551367996);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.157399667056);
         element = table->GetElement(13);
         mix->DefineElement(4, element, 0.247536);
         break;
      case 13: //SDD LV+signal cable
         mix = new TGeoMixture("LV-CABLE", 5, 2.1035);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.0082859922);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.21722436468);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.023028867);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.06574077612);
         element = table->GetElement(13);
         mix->DefineElement(4, element, 0.68572);
         break;
      case 14: //SDD hybrid microcab
         mix = new TGeoMixture("HYB-CAB", 5, 2.0502);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.00926228815);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.24281879711);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.02574224025);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.07348667449);
         element = table->GetElement(13);
         mix->DefineElement(4, element, 0.64869);
         break;
      case 15: //SDD anode microcab
         mix = new TGeoMixture("ANOD-CAB", 5, 1.7854);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.0128595919215);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.392653705471);
         element = table->GetElement(7);
         mix->DefineElement(2, element, 0.041626868025);
         element = table->GetElement(8);
         mix->DefineElement(3, element, 0.118832707289);
         element = table->GetElement(13);
         mix->DefineElement(4, element, 0.431909);
         break;
      case 16: // inox/alum
         mix = new TGeoMixture("INOX-AL", 5, 3.0705);
         element = table->GetElement(13);
         mix->DefineElement(0, element, 0.816164);
         element = table->GetElement(14);
         mix->DefineElement(1, element, 0.000919182);
         element = table->GetElement(24);
         mix->DefineElement(2, element, 0.0330906);
         element = table->GetElement(26);
         mix->DefineElement(3, element, 0.131443);
         element = table->GetElement(28);
         mix->DefineElement(4, element, 0.0183836);
      case 17: // MYLAR
         mix = new TGeoMixture("MYLAR", 3, 1.39);
         element = table->GetElement(1);
         mix->DefineElement(0, element, 0.0416667);
         element = table->GetElement(6);
         mix->DefineElement(1, element, 0.625);
         element = table->GetElement(8);
         mix->DefineElement(2, element, 0.333333);
         break;
      case 18: // SPDBUS(AL+KPT+EPOX)   - unknown composition
         mix = new TGeoMixture("SPDBUS", 1, 1.906);
         element = table->GetElement(9);
         mix->DefineElement(0, element, 1.);
         z = element->Z();
         break;
      case 19: // SDD/SSD rings   - unknown composition
         mix = new TGeoMixture("SDDRINGS", 1, 1.8097);
         element = table->GetElement(6);
         mix->DefineElement(0, element, 1.);
         z = element->Z();
         break;
      case 20: // SPD end ladder   - unknown composition
         mix = new TGeoMixture("SPDEL", 1, 3.6374);
         element = table->GetElement(22);
         mix->DefineElement(0, element, 1.);
         z = element->Z();
         break;
      case 21: // SDD end ladder   - unknown composition
         mix = new TGeoMixture("SDDEL", 1, 0.3824);
         element = table->GetElement(30);
         mix->DefineElement(0, element, 1.);
         z = element->Z();
         break;
      case 22: // SSD end ladder   - unknown composition
         mix = new TGeoMixture("SSDEL", 1, 0.68);
         element = table->GetElement(16);
         mix->DefineElement(0, element, 1.);
         z = element->Z();
         break;
   }
   mix->SetZ(z);      
   printf("Patched with mixture %s\n", mix->GetName());
   return mix;
}   

//_____________________________________________________________________________
void TFlukaMCGeometry::CreateFlukaMatFile(const char *fname)
{
  // ==== from FLUGG ====
  // NAMES OF ELEMENTS AND COMPOUNDS: the names must be written in upper case,
  // according to the fluka standard. In addition,. they must be equal to the
  // names of the fluka materials - see fluka manual - in order that the 
  // program load the right cross sections, and equal to the names included in
  // the .pemf. Otherwise the user must define the LOW-MAT CARDS, and make his
  // own .pemf, in order to get the right cross sections loaded in memory.


   TString sname;
   gGeoManager->Export("flgeom.root");
   if (fname) sname = fname;
   else       sname = "flukaMat.inp";
   ofstream out;
   out.open(sname.Data(), ios::out);
   if (!out.good()) {
      Fatal("CreateFlukaMatFile", "could not open file %s for writing", sname.Data());
      return;
   }
   PrintHeader(out, "MATERIALS AND COMPOUNDS");
   PrintHeader(out, "MATERIALS");   
   Int_t i,j,idmat;
   Int_t counttothree, nelem;
   Double_t a,z,rho, w;
   TGeoElementTable *table = TGeoElementTable::Instance();
   TGeoElement *element;
   element = table->GetElement(13);
   element->SetTitle("ALUMINUM"); // this is how FLUKA likes it ...
   element = table->GetElement(15);
   element->SetTitle("PHOSPHO");  // same story ...
//   element = table->GetElement(10);
//   element->SetTitle("ARGON");  // NEON not in neutron xsec table
   Int_t nelements = table->GetNelements();
   TList *matlist = gGeoManager->GetListOfMaterials();
//   TList *medlist = gGeoManager->GetListOfMedia();
//   Int_t nmed = medlist->GetSize();
   TIter next(matlist);
   Int_t nmater = matlist->GetSize();
   Int_t nfmater = 0;
   TGeoMaterial *mat;
   TGeoMixture *mix = 0;
   TString matname;
   TObjString *objstr;
   // Create all needed elements
   for (Int_t i=1; i<nelements; i++) {
      element = table->GetElement(i);
      // skip elements which are not defined
      if (!element->IsUsed() && !element->IsDefined()) continue;
      matname = element->GetTitle();
      ToFlukaString(matname);
      rho = 0.999;
      mat = new TGeoMaterial(matname, element->A(), element->Z(), rho);
      mat->SetIndex(nfmater+3);
      mat->SetUsed(kTRUE);
      fMatList->Add(mat);
      objstr = new TObjString(matname.Data());
      fMatNames->Add(objstr);
      nfmater++;
   }
   Int_t indmat = nfmater;
//   TGeoMedium *med;
   // Adjust material names and add them to FLUKA list
   for (i=0; i<nmater; i++) {
      mat = (TGeoMaterial*)matlist->At(i);
      if (!mat->IsUsed()) continue;
      z = mat->GetZ();
      a = mat->GetA();
      rho = mat->GetDensity();
      if (mat->GetZ()<0.001) {
         mat->SetIndex(2); // vacuum, built-in inside FLUKA
         continue;
      } 
      matname = mat->GetName();
      FlukaMatName(matname);
/*
      // material with one element: create it as mixture since it can be duplicated    
      if (!mat->IsMixture()) {
         // normal materials
         mix = new TGeoMixture(matname.Data(), 1, rho);
         mix->DefineElement(0, mat->GetElement(), 1.);
         mat->SetIndex(nfmater+3);
         for (j=0; j<nmed; j++) {
            med = (TGeoMedium*)medlist->At(j);
            if (med->GetMaterial() == mat) {
               med->SetMaterial(mix);
               if (mat->GetCerenkovProperties()) {
                  mix->SetCerenkovProperties(mat->GetCerenkovProperties());
                  mat->SetCerenkovProperties(0);
               }
               break;
            }
         }                              
         mat = (TGeoMaterial*)mix;
      }
*/    
      mat->SetIndex(nfmater+3);
      objstr = new TObjString(matname.Data());
      fMatList->Add(mat);
      fMatNames->Add(objstr);
      nfmater++;
   }   

   // Dump all elements with MATERIAL cards         
   for (i=0; i<nfmater; i++) {
      mat = (TGeoMaterial*)fMatList->At(i);
//      mat->SetUsed(kFALSE);
      mix = 0;
      out << setw(10) << "MATERIAL  ";
      out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
      objstr = (TObjString*)fMatNames->At(i);
      matname = objstr->GetString();
      z = mat->GetZ();
      a = mat->GetA();
      rho = mat->GetDensity();
      if (mat->IsMixture()) {
         out << setw(10) << " ";
         out << setw(10) << " ";
         mix = (TGeoMixture*)mat;
      } else {   
         out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << z;
         out << setw(10) << setprecision(3) << a;
      }
      out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
      out << setw(10) << setiosflags(ios::scientific) << setprecision(3) << rho;
      out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
      out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << Double_t(mat->GetIndex());   
      out << setw(10) << " ";
      out << setw(10) << " ";
      out << setw(8) << matname.Data() << endl;
      if (!mix) {
         // add LOW-MAT card for NEON to associate with ARGON neutron xsec
         if (z==10) {
            out << setw(10) << "LOW-MAT   ";
            out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
            out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << Double_t(mat->GetIndex());
            out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << 18.;
            out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << -2.;
            out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << 293.;
            out << setw(10) << " ";
            out << setw(10) << " ";
//            out << setw(8) << matname.Data() << endl;
            out << setw(8) << " " << endl;
         } 
         else { 
            element = table->GetElement((int)z);
            TString elename = element->GetTitle();
            ToFlukaString(elename);
            if( matname.CompareTo( elename ) != 0 ) {
               out << setw(10) << "LOW-MAT   ";
               out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
               out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << Double_t(mat->GetIndex());
               out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << z;
               out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << " ";
               out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << " ";
               out << setw(10) << " ";
               out << setw(10) << " ";
               // missing material at Low Energy Cross Section Table
               if( (int)z==10 || (int)z==21 || (int)z==34 || (int)z==37 || (int)z==39 || (int)z==44 ||
                   (int)z==45 || (int)z==46 || (int)z==52 || (int)z==57 || (int)z==59 || (int)z==60 ||
                   (int)z==61 || (int)z==65 || (int)z==66 || (int)z==67 || (int)z==68 || (int)z==69 ||
                   (int)z==70 || (int)z==71 || (int)z==72 || (int)z==76 || (int)z==77 || (int)z==78 ||
                   (int)z==81 || (int)z==84 || (int)z==85 || (int)z==86 || (int)z==87 || (int)z==88 ||
                   (int)z==89 || (int)z==91 )
                  out << setw(8) << "UNKNOWN " << endl;
               else
                  out << setw(8) << elename.Data() << endl;
   //               out << setw(8) << " " << endl;
            }
         }
         continue;
      }   
      counttothree = 0;
      out << setw(10) << "COMPOUND  ";
      nelem = mix->GetNelements();
      objstr = (TObjString*)fMatNames->At(i);
      matname = objstr->GetString();
      for (j=0; j<nelem; j++) {
         w = (mix->GetWmixt())[j];
         if (w<0.00001) w=0.00001;
         z = (mix->GetZmixt())[j];       
         a = (mix->GetAmixt())[j];
         idmat = GetElementIndex(Int_t(z));
         if (!idmat) Error("CreateFlukaMatFile", "element with Z=%f not found", z);
         out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
         out << setw(10) << setiosflags(ios::fixed) << setprecision(6) << -w;   
         out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
         out << setw(10) << setiosflags(ios::fixed) << setprecision(1) << Double_t(idmat);
         counttothree++;
         if (counttothree == 3) {
            out << matname.Data();
            out << endl;
            if ( (j+1) != nelem) out << setw(10) << "COMPOUND  ";
            counttothree = 0;
         } 
      }               
      if (nelem%3) {
         for (j=0; j<(3-(nelem%3)); j++)
            out << setw(10) << " " << setw(10) << " ";
         out << matname.Data();
         out << endl;
      } 
   }     
   Int_t nvols = gGeoManager->GetListOfUVolumes()->GetEntriesFast()-1;
   TGeoVolume *vol;
   // Now print the material assignments
   Double_t flagfield = 0.;
   printf("#############################################################\n");
   if (gFluka->IsFieldEnabled()) {
      flagfield = 1.;
      printf("Magnetic field enabled\n");
   } else printf("Magnetic field disabled\n");   
   printf("#############################################################\n");
   
   PrintHeader(out, "TGEO MATERIAL ASSIGNMENTS");   
   for (i=1; i<=nvols; i++) {
      vol = gGeoManager->GetVolume(i);
      mat = vol->GetMedium()->GetMaterial();
//      mat->SetUsed(kTRUE);
      idmat = mat->GetIndex();
      for (Int_t j=0; j<nfmater; j++) {
         mat = (TGeoMaterial*)fMatList->At(j);
         if (mat->GetIndex() == idmat) mat->SetUsed(kTRUE);
      }   
      out << setw(10) << "ASSIGNMAT ";
      out.setf(static_cast<std::ios::fmtflags>(0),std::ios::floatfield);
      out << setw(10) << setiosflags(ios::fixed) << Double_t(idmat);
      out << setw(10) << setiosflags(ios::fixed) << Double_t(i);
      out << setw(10) << "0.0";
      out << setw(10) << "0.0";
      out << setw(10) << setiosflags(ios::fixed) << flagfield;
      out << setw(10) << "0.0";
      out << endl;
   }
   out.close();
   fLastMaterial = nfmater+2;
/*   
   TGeoMaterial *mat1 = 0;
   for (i=1; i<=nvols; i++) {
      vol = gGeoManager->GetVolume(i);
      med = vol->GetMedium();
      mat = med->GetMaterial();
      printf("Region %d: %s\n", i, vol->GetName());
      printf("   medium %d: %s\n", med->GetId(), med->GetName());
      for (j=0; j<nfmater; j++) {
         mat1 = (TGeoMaterial*)fMatList->At(j);
         if (mat1 != mat) continue;
         objstr = (TObjString*)fMatNames->At(j);
         matname = objstr->GetString();
         break;
      } 
      if (mat1 != mat) printf("   (*) material not found in Fluka list\n");
      printf("   material %s (at ind=%d): FlukaID=%d FlukaName=%s\n", 
             mat->GetName(), j, mat->GetIndex(), matname.Data());
      if (mat->GetCerenkovProperties()) printf("     Cerenkov properties found\n");      
   }   
*/   
   
   if (!gFluka->IsGeneratePemf()) return;
   // Write peg files
   char number[20];
   Int_t countMatOK = 0;
   Int_t countElemError = 0;
   Int_t countNoStern = 0;
   Int_t countMixError = 0;
   Int_t countGas = 0;
 //  Int_t countGasError = 0;
   Int_t countPemfError = 0;
   for (i=indmat; i<nfmater; i++) {
      mat = (TGeoMaterial*)fMatList->At(i);
      if (!mat->IsUsed()) continue;
      sname = "mat";
      sprintf(number, "%d", i);
      sname.Append(number);
      cout << endl;
      cout << endl;
      cout << "******************************************************************************" << endl;
      cout << "******************************************************************************" << endl;
      cout << endl;
      WritePegFile(i, &countNoStern, &countElemError, &countMixError, &countGas);
      sname.Prepend("$FLUPRO/pemf/rpemf peg/");
      gSystem->Exec(sname.Data());
      
      // check if the pemf file was created
      TString sname = Form("peg/mat%d.pemf", i);
      ifstream in( sname.Data() );
      if ( in ) {
         countMatOK++;
         in.close();
      }
      else {
         cout << "ERROR Fail to create the pemf file " << sname << endl;
         countPemfError++; 
      }
   }
   cout << "Materials (pemf created)   " << countMatOK         << endl;
   cout << "Not Sternheimer par. found  " << countNoStern   << endl;
   cout << "Elements with error definitions (Z not integer)  " << countElemError      << endl;
   cout << "Mixtures with error definitions (Z not integer) " << countMixError  << endl;
   cout << "Posible Gas (rho < 0.01) " << countGas           << endl;
  // cout << "Posible Gas (without pressure information) " << countGasError           << endl;
    cout << "Pemf files Error    " << countPemfError     << endl;
   cout << endl << endl;
   
   sname = "cat peg/*.pemf > peg/FlukaVmc.pemf";         
   gSystem->Exec(sname.Data());
   sname = "mv peg/FlukaVmc.pemf FlukaVmc.pemf";
   gSystem->Exec(sname.Data());
}

//_____________________________________________________________________________
void TFlukaMCGeometry::WritePegFile(Int_t imat, Int_t *NoStern, Int_t *ElemError,
                       Int_t *MixError, Int_t *countGas) const
{
   // Write the .peg file for one material
   
   TGeoMaterial *mat = (TGeoMaterial*)fMatList->At(imat);
   TString name = ((TObjString*)fMatNames->At(imat))->GetString();
   TString line;
   char number[20];
   TGeoElement *elem = mat->GetElement();
   name = name.Strip();
   TString sname = "mat";
   sprintf(number, "%d", imat);
   sname.Append(number);
   sname.Append(".peg");
   sname.Prepend("peg/");
   ofstream out;
   out.open(sname.Data(), ios::out);
   if (!out.good()) return;
   Double_t dens = mat->GetDensity();
   TGeoMixture *mix = 0;
   Int_t nel = 1;
   Int_t i;
   if (mat->IsMixture()) {
      mix = (TGeoMixture*)mat;
      nel = mix->GetNelements();
   } 
     
   if (nel==1) {
      cout  << "( Element ) " << name << "  Z=" << mat->GetZ() << " Rho " << mat->GetDensity() << endl;

      Double_t zel = mat->GetZ();
      if( (zel-Int_t(zel))>0.001 || zel < 1 ) {
         cout << " ERROR: A Element with not integer Z=" << zel << endl;
         cout << endl;
         (*ElemError)++;
         return;
      }
      
      out << "ELEM" << endl;
      out << " &INP IRAYL=1, RHO=" << dens << ", " << endl;
      
      // check for the Sternheimer parameters
      Double_t *issb_parm = GetISSB( mat->GetDensity(), 1, &zel, 0 );
      if( issb_parm[0] > 0 && issb_parm[1] > 0 ) {
         cout << "Sternheimer parameters found" << endl;
         out << ", ISSB=1, IEV=" << issb_parm[0] << ", CBAR=" << issb_parm[1]
             << ", X0=" << issb_parm[2] << "," << endl;
         out << "X1=" <<issb_parm[3] <<", AFACT="<<issb_parm[4] <<", SK="
             << issb_parm[5] << ", DELTA0=" << issb_parm[6];
      } 
      else {
         cout << "WARNING: Strange element, Sternheimer parameters  not found" << endl;
        (*NoStern)++;
      }

      if (dens<0.01) {
        (*countGas)++;
        out << " GASP=1." << endl;
      }
      
      out << " &END" <<  endl;
      out << name.Data() << endl;
      out << elem->GetName() << endl;
      
   } 
   else {
   
      cout  << "( Mixture )  " << name << "  Rho " << dens << " nElem " << nel << endl;
    
      Double_t *zt = new Double_t[nel];
      Double_t *wt = new Double_t[nel];
      for (int j=0; j<nel; j++) {
         zt[j] = (mix->GetZmixt())[j];
         wt[j] = (mix->GetWmixt())[j];
         if( (zt[j]-Int_t(zt[j])) > 0.001 || zt[j] < 1 ) {
            cout << "ERROR Mixture " << name << " with an element with not integer Z=" << zt[j] << endl;
            cout << endl;
            (*MixError)++;
            // just continue since the mixtures are not patch, 
            // but the final release should include the return   
            //  return;         
         }
      }
      Double_t *issb_parm = GetISSB( mat->GetDensity(), nel, zt, wt );
      out << "MIXT" << endl;
      out << " &INP IRAYL=1, NE=" << nel << ", RHOZ=" << wt[0] << ",";
      line = Form(" &INP IRAYL=1, NE=%d RHOZ=%g", nel, wt[0]);
      for(int j=1; j<nel; j++) {
         out << " " << wt[j] << ",";
         line += Form(" %g,", wt[j] );
         if( line.Length() > 60 ) { out << endl; line = ""; }
      }
      out << " RHO=" << mat->GetDensity() << ", ";
      line += Form(" RHO=%g, ", mat->GetDensity());
      if( line.Length() > 60 ) { out << endl; line = ""; }
      
      if( issb_parm[0] > 0 && issb_parm[1] > 0 ) {
         cout << "Sternheimer parameters found" << endl;
         out << " ISSB=1, IEV=" << issb_parm[0] << ",";
         line += Form(" ISSB=1, IEV=%g,", issb_parm[0]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
         out << " CBAR=" << issb_parm[1] << ",";
         line += Form(" CBAR=%g,",issb_parm[1]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
         out << " X0=" << issb_parm[2] << ",";
         line += Form(" X0=%g,", issb_parm[2]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
         out << " X1=" << issb_parm[3] << ",";
         line += Form(" X1=%g,", issb_parm[3]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
         out << " AFACT="<< issb_parm[4] << ",";
         line += Form(" AFACT=%g,", issb_parm[4]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
         out << " SK=" << issb_parm[5] << ",";
         line += Form(" SK=%g,", issb_parm[5]);
         if( line.Length() > 60 ) { out << endl; line = "";  }
      }
      else {
         cout << "Sternheimer parameters  not found" << endl;
         (*NoStern)++;
      }
      
      if (dens<0.01){
         (*countGas)++;
         out << " GASP=1." << endl;
      }
      
      out << " &END" <<  endl;
      out << name.Data() << endl;
      for (i=0; i<nel; i++) {
         elem = mix->GetElement(i);
         line = elem->GetName();
         if (line.Length()==1) line.Append(" ");
         out << line.Data() << " ";
      }
      out << endl;
      
      delete [] zt;
      delete [] wt;
   }
   
   out << "ENER" << endl;
   out << " $INP AE=0.56099906, UE=3000000., AP=.03, UP=3000000. $END" << endl;
   out << "PWLF" << endl;
   out << " $INP NALE=300, NALG=400, NALR=100 $END" << endl;
   out << "DECK" << endl;
   out << " $INP $END" << endl;
   out << "TEST" << endl;
   out << " $INP $END" << endl;
   out.close();
}

Double_t * TFlukaMCGeometry::GetISSB(Double_t rho, Int_t nElem, Double_t *zelem, Double_t *welem ) const
{
   // Read the density effect parameters
   // from R.M. Sternheimer et al. Atomic Data
   // and Nuclear Data Tables, Vol. 30 No. 2
   //
   // return the parameters if the element/mixture match with one of the list
   // otherwise returns the parameters set to 0
   
   struct sternheimerData {
      TString     longname;           // element/mixture name
      Int_t       nelems;             // number of constituents N
      Int_t       Z[20];              //[nelems] Z
      Double_t    wt[20];             //[nelems] weight fraction
      Double_t    density;            // g/cm3
      Double_t    iev;                // Average Ion potential (eV)
                                   // ****   Sternheimer parameters  ****
      Double_t    cbar;               // CBAR
      Double_t    x0;                 // X0
      Double_t    x1;                 // X1
      Double_t    afact;              // AFACT
      Double_t    sk;                 // SK
      Double_t    delta0;             // DELTA0
   };
   
   TString     shortname;
   TString     formula;
   Int_t       num;
   char        state;
   
   static Double_t parameters[7];
   memset( parameters, 0, sizeof(Double_t) );

   static sternheimerData sternDataArray[300];
   static Bool_t isFileRead = kFALSE;
   
   // Read the data file if is needed
   if( isFileRead == kFALSE ) {
      TString sSternheimerInp = getenv("ALICE_ROOT");
      sSternheimerInp +="/TFluka/input/Sternheimer.data";
   
      ifstream in(sSternheimerInp);
      char line[100];
      in.getline(line, 100);   
      in.getline(line, 100);   
      in.getline(line, 100);   
      in.getline(line, 100);   
      in.getline(line, 100);   
      in.getline(line, 100);   
      
      
      Int_t is = 0;
      while( !in.eof() ) {
         in >> shortname >> num     >> sternDataArray[is].nelems 
            >> sternDataArray[is].longname  >> formula >> state;
         if( in.eof() ) break;
         for(int i=0; i<sternDataArray[is].nelems; i++) {
            in >> sternDataArray[is].Z[i] >> sternDataArray[is].wt[i]; 
         }
         in >> sternDataArray[is].density; 
         in >> sternDataArray[is].iev; 
         in >> sternDataArray[is].cbar; 
         in >> sternDataArray[is].x0; 
         in >> sternDataArray[is].x1; 
         in >> sternDataArray[is].afact; 
         in >> sternDataArray[is].sk;
         if( sternDataArray[is].nelems == 1 ) in >> sternDataArray[is].delta0;
         is++;
      }
      isFileRead = kTRUE;
      in.close();
   }   
   
   Int_t is = 0;
   while( is < 280 ) {
   
      // check for elements
      if( sternDataArray[is].nelems == 1 && nElem == 1
          && sternDataArray[is].Z[0] == Int_t(*zelem)
          && TMath::Abs( (sternDataArray[is].density - rho)/sternDataArray[is].density ) < 0.1 ) {
         cout << sternDataArray[is].longname << "   #elems:" <<  sternDataArray[is].nelems << "  Rho:" 
              << sternDataArray[is].density << endl;
         cout << sternDataArray[is].iev   << " " 
              << sternDataArray[is].cbar  << " " 
              << sternDataArray[is].x0    << " " 
              << sternDataArray[is].x1    << " " 
              << sternDataArray[is].afact << " " 
              << sternDataArray[is].sk    << " " 
              << sternDataArray[is].delta0 << endl;
         
         parameters[0] = sternDataArray[is].iev;
         parameters[1] = sternDataArray[is].cbar;
         parameters[2] = sternDataArray[is].x0;
         parameters[3] = sternDataArray[is].x1;
         parameters[4] = sternDataArray[is].afact;
         parameters[5] = sternDataArray[is].sk;
         parameters[6] = sternDataArray[is].delta0;
         return parameters;        
      }
      
      // check for mixture
      int nmatch = 0;
      if( sternDataArray[is].nelems > 1 && sternDataArray[is].nelems == nElem ) {
         for(int j=0; j<sternDataArray[is].nelems; j++) {
            if( sternDataArray[is].Z[j] == Int_t(zelem[j]) && 
               TMath::Abs( (sternDataArray[is].wt[j] - welem[j])/sternDataArray[is].wt[j] ) < 0.1 )
            nmatch++;            
         }
      }

      if( sternDataArray[is].nelems > 1 && 
          TMath::Abs( (sternDataArray[is].density - rho)/sternDataArray[is].density ) < 0.1 
          && nmatch == sternDataArray[is].nelems ) {
         cout << sternDataArray[is].longname << "   #elem:" <<  sternDataArray[is].nelems << "  Rho:" 
              << sternDataArray[is].density << endl;
         cout << sternDataArray[is].iev   << " " 
              << sternDataArray[is].cbar  << " " 
              << sternDataArray[is].x0    << " " 
              << sternDataArray[is].x1    << " " 
              << sternDataArray[is].afact << " " 
              << sternDataArray[is].sk    << " " 
              << sternDataArray[is].delta0 << endl;

         parameters[0] = sternDataArray[is].iev;
         parameters[1] = sternDataArray[is].cbar;
         parameters[2] = sternDataArray[is].x0;
         parameters[3] = sternDataArray[is].x1;
         parameters[4] = sternDataArray[is].afact;
         parameters[5] = sternDataArray[is].sk;
         parameters[6] = 0;
         return parameters;        
      }
      is++; 
   }   
   return parameters;        
}

//_____________________________________________________________________________
void TFlukaMCGeometry::PrintHeader(ofstream &out, const char *text) const
{
// Print a FLUKA header.
  out << "*\n" << "*\n" << "*\n";
  out << "*********************  " << text << " *********************\n"
     << "*\n";
  out << "*...+....1....+....2....+....3....+....4....+....5....+....6....+....7..."
     << endl;
  out << "*" << endl;
}

//_____________________________________________________________________________
Int_t TFlukaMCGeometry::RegionId() const
{
// Returns current region id <-> TGeo node id
   if (gGeoManager->IsOutside()) return 0;
   return gGeoManager->GetCurrentNode()->GetUniqueID();
}

//_____________________________________________________________________________
Int_t TFlukaMCGeometry::GetElementIndex(Int_t z) const
{
// Get index of a material having a given Z element.
   TIter next(fMatList);
   TGeoMaterial *mat;
   Int_t index = 0;
   while ((mat=(TGeoMaterial*)next())) {
      if (mat->IsMixture()) continue;
      if (mat->GetElement()->Z() == z) return mat->GetIndex();
   }
   return index;   
}

//_____________________________________________________________________________
void TFlukaMCGeometry::SetMreg(Int_t mreg)
{
// Update if needed next history;
   if (gFluka->GetDummyBoundary()==2) {
      gGeoManager->CdNode(fNextLattice-1);
      return;
   }   
   Int_t curreg = (gGeoManager->IsOutside())?(gMCGeom->NofVolumes()+1):gGeoManager->GetCurrentVolume()->GetNumber();
   if (mreg==curreg) return;
   if (mreg==fNextRegion) {
      if (fNextLattice!=999999999) gGeoManager->CdNode(fNextLattice-1);
      return;
   } else {
      if (mreg == fCurrentRegion) {
         if (fCurrentLattice!=999999999) gGeoManager->CdNode(fCurrentLattice-1);
         return;
      }   
   }     
   if (fDebug) printf("ERROR: mreg=%i neither current nor next region\n", mreg);
}

//_____________________________________________________________________________
void TFlukaMCGeometry::SetCurrentRegion(Int_t mreg, Int_t latt)
{
// Set index/history for next entered region
   fCurrentRegion = mreg;
   fCurrentLattice = latt;
}   

//_____________________________________________________________________________
void TFlukaMCGeometry::SetNextRegion(Int_t mreg, Int_t latt)
{
// Set index/history for next entered region
   fNextRegion = mreg;
   fNextLattice = latt;
}   

//_____________________________________________________________________________
void TFlukaMCGeometry::ToFlukaString(TString &str) const
{
// ToFlukaString converts an string to something usefull in FLUKA:
// * Capital letters
// * Only 8 letters
// * Replace ' ' by '_'
   if (str.Length()<8) {
      str += "        ";
   }   
   str.Remove(8);
   Int_t ilast;
   for (ilast=7; ilast>0; ilast--) if (str(ilast)!=' ') break;
   str.ToUpper();
   for (Int_t pos=0; pos<ilast; pos++)
      if (str(pos)==' ') str.Replace(pos,1,"_",1);
   return;
}   

//_____________________________________________________________________________
void TFlukaMCGeometry::FlukaMatName(TString &str) const
{
// Convert a name to upper case 8 chars.
   ToFlukaString(str);
   Int_t ilast;
   for (ilast=7; ilast>0; ilast--) if (str(ilast)!=' ') break;
   if (ilast>5) ilast = 5;
   char number[3];
   TIter next(fMatNames);
   TObjString *objstr;
   TString matname;
   Int_t index = 0;
   while ((objstr=(TObjString*)next())) {
      matname = objstr->GetString();
      if (matname == str) {
         index++;
         if (index<10) {
            number[0] = '0';
            sprintf(&number[1], "%d", index);
         } else if (index<100) {
            sprintf(number, "%d", index);            
         } else {
            Error("FlukaMatName", "Too many materials %s", str.Data());
            return;
         }
         str.Replace(ilast+1, 2, number);
         str.Remove(8);
      }   
   }   
}   
         
//______________________________________________________________________________
void TFlukaMCGeometry::Vname(const char *name, char *vname) const
{
  //
  //  convert name to upper case. Make vname at least 4 chars
  //
  Int_t l = strlen(name);
  Int_t i;
  l = l < 4 ? l : 4;
  for (i=0;i<l;i++) vname[i] = toupper(name[i]);
  for (i=l;i<4;i++) vname[i] = ' ';
  vname[4] = 0;      
}


// FLUKA GEOMETRY WRAPPERS - to replace FLUGG wrappers

//_____________________________________________________________________________
Int_t idnrwr(const Int_t & /*nreg*/, const Int_t & /*mlat*/)
{
//   from FLUGG:
// Wrapper for setting DNEAR option on fluka side. Must return 0 
// if user doesn't want Fluka to use DNEAR to compute the 
// step (the same effect is obtained with the GLOBAL (WHAT(3)=-1)
// card in fluka input), returns 1 if user wants Fluka always to 
// use DNEAR (in this case, be sure that GEANT4 DNEAR is unique, 
// coming from all directions!!!)
   if (gMCGeom->IsDebugging()) printf("========== Dummy IDNRWR\n");
   return 0;
}

//_____________________________________________________________________________
void g1wr(Double_t &pSx, Double_t &pSy, Double_t &pSz, 
          Double_t *pV,  Int_t &oldReg , const Int_t &oldLttc, Double_t &propStep,
          Int_t &nascFlag, Double_t &retStep, Int_t &newReg,
	       Double_t &saf, Int_t &newLttc, Int_t &lttcFlag,
          Double_t *sLt, Int_t *jrLt)

{
   // Initialize FLUKa point and direction;
   gNstep++;
/*
   if (kNstep>0) {
      gMCGeom->SetDebugMode(kTRUE);
      gFluka->SetVerbosityLevel(3);
   }   
   if (kNstep>6520) {
      gMCGeom->SetDebugMode(kFALSE);
      gFluka->SetVerbosityLevel(0);
   }   
   if ((kNstep%10)==0) printf("step %i\n", kNstep);
*/

   if (gMCGeom->IsDebugging()) {
      printf("========== Inside G1WR\n");
      printf("   point/dir:(%14.9f, %14.9f, %14.9f, %g, %g, %g)\n", pSx,pSy,pSz,pV[0],pV[1],pV[2]);
      printf("   oldReg=%i  oldLttc=%i  pstep=%f\n",oldReg, oldLttc, propStep);
   }   
   gGeoManager->SetCurrentPoint(pSx, pSy, pSz);
   gGeoManager->SetCurrentDirection(pV);
   gMCGeom->SetCurrentRegion(oldReg, oldLttc);
   // Initialize default return values
   lttcFlag = 0;
   jrLt[lttcFlag] = oldLttc;
   sLt[lttcFlag] = propStep;
   jrLt[lttcFlag+1] = -1;
   sLt[lttcFlag+1] = 0.;
   newReg = oldReg;
   newLttc = oldLttc;
   // check if dummy boundary flag is set
   Int_t curLttc, curReg;
   if (gFluka->IsDummyBoundary()) {
      // printf("Dummy boundary intercepted. Point is: %f, %f, %f\n", pSx, pSy, pSz);
      Bool_t crossedDummy = (oldLttc == TFlukaMCGeometry::kLttcVirtual)?kTRUE:kFALSE;
      if (crossedDummy) {
      // FLUKA crossed the dummy boundary - update new region/history
         retStep = 0.;
         saf = 0.;
         gMCGeom->GetNextRegion(newReg, newLttc);
         gMCGeom->SetMreg(newReg);
         if (gMCGeom->IsDebugging()) printf("   virtual newReg=%i newLttc=%i\n", newReg, newLttc);
         sLt[lttcFlag] = 0.; // null step in current region
         lttcFlag++;
         jrLt[lttcFlag] = newLttc;
         sLt[lttcFlag] = 0.; // null step in next region
         jrLt[lttcFlag+1] = -1;
         sLt[lttcFlag+1] = 0.;
         gFluka->SetDummyBoundary(0);
         return;
      }   
   }   
      
   // Reset outside flag
   if (gGeoManager->IsOutside()) {
      gGeoManager->SetOutside(kFALSE);
      gGeoManager->CdTop();
   } 
   
   // Reset dummy boundary flag
   gFluka->SetDummyBoundary(0); 
    
   curLttc = gGeoManager->GetCurrentNodeId()+1;
   curReg = gGeoManager->GetCurrentVolume()->GetNumber();
   if (oldLttc != curLttc) {
      // FLUKA crossed the boundary : we trust that the given point is really there,
      // so we just update TGeo state
      gGeoManager->CdNode(oldLttc-1);
      curLttc = gGeoManager->GetCurrentNodeId()+1;
      curReg  = gGeoManager->GetCurrentVolume()->GetNumber();
      if (gMCGeom->IsDebugging()) printf("   re-initialized point: curReg=%i  curLttc=%i\n", curReg, curLttc);
   }  
   // Now the current TGeo state reflects the FLUKA state       
   if (gMCGeom->IsDebugging()) printf("   current path: %s\n", gGeoManager->GetPath());
   Double_t extra = 1E-6;
   Double_t tmpStep = propStep + extra;
   gGeoManager->FindNextBoundary(-tmpStep);
   Double_t snext = gGeoManager->GetStep();
   // !!!!!
   if (snext<=0) {
      // FLUKA is in the wrong region, notify it
      if (gMCGeom->IsDebugging()) printf("ERROR: snext=%f\n", snext);
//      newReg = -3;
//      return;
      snext = extra;
   }   
   saf = gGeoManager->GetSafeDistance();
   Bool_t cross = kFALSE;
   Bool_t onBound = kFALSE;
   if (snext<tmpStep) {
      // We have some boundary in the way
      Double_t dd = snext-propStep;
      if (dd < 0) {
         cross = kTRUE;
         dd = -dd;
      }   
      if (dd < 1E-8) onBound = kTRUE;
   }
   snext += 1.E-8;
   if (gMCGeom->IsDebugging()) {
      if (!cross) printf("   physical step approved: %f\n", propStep);
      else printf("   boundary crossing at: %f\n", snext);
      if (onBound) printf("   step on boundary limit ! NASC=%i\n", nascFlag);
   }   
   if (!cross) {
   // Next boundary further than proposed step, which is approved
      retStep = propStep;
      sLt[lttcFlag] = propStep;
      return;
   }
   // The next boundary is closer. We try to cross it.
   Double_t *point = gGeoManager->GetCurrentPoint();
   Double_t *dir = gGeoManager->GetCurrentDirection();
   Double_t pt[3];
   memcpy(pt, point, 3*sizeof(Double_t));
   
   Int_t i;
   for (i=0;i<3;i++) point[i] += snext*dir[i];
   gGeoManager->FindNode();
   newLttc = (gGeoManager->IsOutside())?(TFlukaMCGeometry::kLttcOutside):gGeoManager->GetCurrentNodeId()+1;
   if (newLttc == oldLttc) {
      // brute force ...
      // Just try a fast extra small step
      snext += 1E-6;
      for (i=0;i<3;i++) point[i] = pt[i]+snext*dir[i];
      gGeoManager->FindNode();
      newLttc = (gGeoManager->IsOutside())?(TFlukaMCGeometry::kLttcOutside):gGeoManager->GetCurrentNodeId()+1;
      if (newLttc == oldLttc) {
         // check if state changes at the end of the proposed step
         for (i=0;i<3;i++) point[i] = pt[i]+propStep*dir[i];
         gGeoManager->FindNode();
         newLttc = (gGeoManager->IsOutside())?(TFlukaMCGeometry::kLttcOutside):gGeoManager->GetCurrentNodeId()+1;
         if (newLttc==oldLttc) {
            // approve step
            retStep = propStep;
            sLt[lttcFlag] = propStep;
            return;
         }
         // snext is underestimated - we will create a virtual one to overcome the error
//         printf("some boundary in the way...\n");
      }    
   }
   gGeoManager->SetCurrentPoint(pt);
//   newLttc = (gGeoManager->IsOutside())?(TFlukaMCGeometry::kLttcOutside):gGeoManager->GetCurrentNodeId()+1;
   newReg = (gGeoManager->IsOutside())?(gMCGeom->NofVolumes()+1):gGeoManager->GetCurrentVolume()->GetNumber();
   if (gMCGeom->IsDebugging()) printf("   newReg=%i newLttc=%i\n", newReg, newLttc);

   // We really crossed the boundary, but is it the same region ?
   gMCGeom->SetNextRegion(newReg, newLttc);
   if (newReg == oldReg) {
      // Virtual boundary between replicants
      if (gMCGeom->IsDebugging()) printf("   DUMMY boundary\n");
      newReg = 1;  // cheat FLUKA telling it it crossed the TOP region
      newLttc = TFlukaMCGeometry::kLttcVirtual;
      // mark that next boundary is virtual
      gFluka->SetDummyBoundary(1);
   } 
   retStep = snext;
   sLt[lttcFlag] = snext;
   lttcFlag++;
   jrLt[lttcFlag] = newLttc;
   sLt[lttcFlag] = snext;
   jrLt[lttcFlag+1] = -1;
   sLt[lttcFlag+1] = 0.;      

   if (newLttc!=oldLttc) {
      if (gGeoManager->IsOutside()) {
         gGeoManager->SetOutside(kFALSE);
         gGeoManager->CdTop();
      } 
      gGeoManager->CdTop();
      if (!gGeoManager->GetCurrentMatrix()->IsIdentity()) printf("ERROR  at step %i\n", gNstep);
      gGeoManager->CdNode(oldLttc-1);
   }   
   if (gMCGeom->IsDebugging()) {
      printf("=> snext=%g safe=%g\n", snext, saf);
      for (Int_t i=0; i<lttcFlag+1; i++) printf("   jrLt[%i]=%i  sLt[%i]=%g\n", i,jrLt[i],i,sLt[i]);
   }   
   if (gMCGeom->IsDebugging()) printf("<= G1WR (in: %s)\n", gGeoManager->GetPath());
}

//_____________________________________________________________________________
void g1rtwr()
{
   if (gMCGeom->IsDebugging()) printf("========== Dummy G1RTWR\n");
} 

//_____________________________________________________________________________
void conhwr(Int_t & /*intHist*/, Int_t * /*incrCount*/)
{
   if (gMCGeom->IsDebugging()) printf("========== Dummy CONHWR\n");
}

//_____________________________________________________________________________
void inihwr(Int_t &intHist)
{
   if (gMCGeom->IsDebugging()) printf("========== Inside INIHWR -> reinitializing history: %i\n", intHist);
   if (gGeoManager->IsOutside()) gGeoManager->CdTop();
   if (intHist<=0) {
//      printf("=== wrong history number\n");
      return;
   }
   if (intHist==0) gGeoManager->CdTop();
   else gGeoManager->CdNode(intHist-1);
   if (gMCGeom->IsDebugging()) {
      printf(" --- current path: %s\n", gGeoManager->GetPath());
      printf("<= INIHWR\n");
   }   
}

//_____________________________________________________________________________
void  jomiwr(const Int_t & /*nge*/, const Int_t & /*lin*/, const Int_t & /*lou*/,
             Int_t &flukaReg)
{
// Geometry initialization wrapper called by FLUKAM. Provides to FLUKA the
// number of regions (volumes in TGeo)
   // build application geometry
   if (gMCGeom->IsDebugging()) printf("========== Inside JOMIWR\n");
   flukaReg = gGeoManager->GetListOfUVolumes()->GetEntriesFast();
   if (gMCGeom->IsDebugging()) printf("<= JOMIWR: last region=%i\n", flukaReg);
}   

//_____________________________________________________________________________
void lkdbwr(Double_t &pSx, Double_t &pSy, Double_t &pSz,
            Double_t * /*pV*/, const Int_t &oldReg, const Int_t &oldLttc,
            Int_t &newReg, Int_t &flagErr, Int_t &newLttc)             
{
   if (gMCGeom->IsDebugging()) {
      printf("========== Inside LKDBWR (%f, %f, %f)\n",pSx, pSy, pSz);
//      printf("   in: pV=(%f, %f, %f)\n", pV[0], pV[1], pV[2]);
      printf("   in: oldReg=%i oldLttc=%i\n", oldReg, oldLttc);
   }   
   TGeoNode *node = gGeoManager->FindNode(pSx, pSy, pSz);
   if (gGeoManager->IsOutside()) {
      newReg = gMCGeom->NofVolumes()+1;
//      newLttc = gGeoManager->GetCurrentNodeId();
      newLttc = 999999999;
      if (gMCGeom->IsDebugging()) {
         printf("OUTSIDE\n");
         printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
         printf("<= LKMGWR\n");
      }   
      flagErr = newReg;
      return;
   } 
   newReg = node->GetVolume()->GetNumber();
   newLttc = gGeoManager->GetCurrentNodeId()+1; 
   gMCGeom->SetNextRegion(newReg, newLttc);
   flagErr = newReg;
   if (gMCGeom->IsDebugging()) {
      printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
      printf("<= LKDBWR\n");
   }   
}

//_____________________________________________________________________________
void lkfxwr(Double_t &pSx, Double_t &pSy, Double_t &pSz,
            Double_t * /*pV*/, const Int_t &oldReg, const Int_t &oldLttc,
            Int_t &newReg, Int_t &flagErr, Int_t &newLttc)
{
   if (gMCGeom->IsDebugging()) {
      printf("========== Inside LKFXWR (%f, %f, %f)\n",pSx, pSy, pSz);
//      printf("   in: pV=(%f, %f, %f)\n", pV[0], pV[1], pV[2]);
      printf("   in: oldReg=%i oldLttc=%i\n", oldReg, oldLttc);
   }   
   TGeoNode *node = gGeoManager->FindNode(pSx, pSy, pSz);
   if (gGeoManager->IsOutside()) {
      newReg = gMCGeom->NofVolumes()+1;
//      newLttc = gGeoManager->GetCurrentNodeId();
      newLttc = 999999999;
      if (gMCGeom->IsDebugging()) {
         printf("OUTSIDE\n");
         printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
         printf("<= LKMGWR\n");
      }   
      flagErr = newReg;
      return;
   } 
   newReg = node->GetVolume()->GetNumber();
   newLttc = gGeoManager->GetCurrentNodeId()+1; 
   gMCGeom->SetNextRegion(newReg, newLttc);
   flagErr = newReg;
   if (gMCGeom->IsDebugging()) {
      printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
      printf("<= LKFXWR\n");
   }   
}

//_____________________________________________________________________________
void lkmgwr(Double_t &pSx, Double_t &pSy, Double_t &pSz,
            Double_t * /*pV*/, const Int_t &oldReg, const Int_t &oldLttc,
		      Int_t &flagErr, Int_t &newReg, Int_t &newLttc)
{
   if (gMCGeom->IsDebugging()) {
      printf("========== Inside LKMGWR (%f, %f, %f)\n",pSx, pSy, pSz);
//      printf("   in: pV=(%f, %f, %f)\n", pV[0], pV[1], pV[2]);
      printf("   in: oldReg=%i oldLttc=%i\n", oldReg, oldLttc);
   }   
   TGeoNode *node = gGeoManager->FindNode(pSx, pSy, pSz);
   if (gGeoManager->IsOutside()) {
      newReg = gMCGeom->NofVolumes()+1;
//      newLttc = gGeoManager->GetCurrentNodeId();
      newLttc = 999999999;
      if (gMCGeom->IsDebugging()) {
         printf("OUTSIDE\n");
         printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
         printf("<= LKMGWR\n");
      }   
      flagErr = newReg;
      return;
   } 
   newReg = node->GetVolume()->GetNumber();
   newLttc = gGeoManager->GetCurrentNodeId()+1; 
   gMCGeom->SetNextRegion(newReg, newLttc);
   flagErr = newReg;
   if (gMCGeom->IsDebugging()) {
      printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
      printf("<= LKMGWR\n");
   }   
}

//_____________________________________________________________________________
void lkwr(Double_t &pSx, Double_t &pSy, Double_t &pSz,
          Double_t * /*pV*/, const Int_t &oldReg, const Int_t &oldLttc,
	       Int_t &newReg, Int_t &flagErr, Int_t &newLttc)
{
   if (gMCGeom->IsDebugging()) {
      printf("========== Inside LKWR (%f, %f, %f)\n",pSx, pSy, pSz);
//      printf("   in: pV=(%f, %f, %f)\n", pV[0], pV[1], pV[2]);
      printf("   in: oldReg=%i oldLttc=%i\n", oldReg, oldLttc);
   }   
   TGeoNode *node = gGeoManager->FindNode(pSx, pSy, pSz);
   if (gGeoManager->IsOutside()) {
      newReg = gMCGeom->NofVolumes()+1;
//      newLttc = gGeoManager->GetCurrentNodeId();
      newLttc = 999999999;
      if (gMCGeom->IsDebugging()) {
         printf("OUTSIDE\n");
         printf("  out: newReg=%i newLttc=%i\n", newReg, newLttc);
         printf("<= LKMGWR\n");
      }   
      flagErr = newReg;
      return;
   } 
   newReg = node->GetVolume()->GetNumber();
   newLttc = gGeoManager->GetCurrentNodeId()+1;
   gMCGeom->SetNextRegion(newReg, newLttc);
   flagErr = newReg;
   if (gMCGeom->IsDebugging()) {
      printf("  out: newReg=%i newLttc=%i in %s\n", newReg, newLttc, gGeoManager->GetPath());
      printf("<= LKWR\n");
   }   
}

//_____________________________________________________________________________
void nrmlwr(Double_t &pSx, Double_t &pSy, Double_t &pSz,
            Double_t &pVx, Double_t &pVy, Double_t &pVz,
	         Double_t *norml, const Int_t &oldReg, 
	         const Int_t &newReg, Int_t &flagErr)
{
   if (gMCGeom->IsDebugging()) {
      printf("========== Inside NRMLWR (%g, %g, %g, %g, %g, %g)\n", pSx,pSy,pSz,pVx,pVy,pVz);
      printf("   oldReg=%i, newReg=%i\n", oldReg,newReg);
   }   
//   Int_t curreg = (gGeoManager->IsOutside())?(gMCGeom->NofVolumes()+1):gGeoManager->GetCurrentVolume()->GetNumber();
//   Int_t curLttc = gGeoManager->GetCurrentNodeId()+1;
//   if (gMCGeom->IsDebugging()) printf("   curReg=%i, curLttc=%i in: %s\n", curreg, curLttc, gGeoManager->GetPath());
//   Bool_t regsame = (curreg==oldReg)?kTRUE:kFALSE;
   gGeoManager->SetCurrentPoint(pSx, pSy, pSz);
   gGeoManager->SetCurrentDirection(pVx,pVy,pVz);
/*
   if (!regsame) {
      if (gMCGeom->IsDebugging()) printf("   REGIONS DOEN NOT MATCH\n");
      gGeoManager->FindNode();
      curreg = (gGeoManager->IsOutside())?(gMCGeom->NofVolumes()+1):gGeoManager->GetCurrentVolume()->GetNumber();
      curLttc = gGeoManager->GetCurrentNodeId()+1;
      if (gMCGeom->IsDebugging()) printf("   re-initialized point: curReg=%i  curLttc=%i curPath=%s\n", curreg, curLttc, gGeoManager->GetPath());
   }
*/
   Double_t *dnorm = gGeoManager->FindNormalFast();
   flagErr = 0;
   if (!dnorm) {
      printf("   ERROR: Cannot compute fast normal\n");
      flagErr = 1;
      norml[0] = -pVx;   
      norml[1] = -pVy;   
      norml[2] = -pVz; 
   }
   norml[0] = -dnorm[0];   
   norml[1] = -dnorm[1];   
   norml[2] = -dnorm[2]; 
   if (gMCGeom->IsDebugging()) printf("   normal to boundary: (%g, %g, %g)\n", norml[0], norml[1], norml[2]);  
//   curreg = (gGeoManager->IsOutside())?(gMCGeom->NofVolumes()+1):gGeoManager->GetCurrentVolume()->GetNumber();
//   curLttc = gGeoManager->GetCurrentNodeId()+1;
   if (gMCGeom->IsDebugging()) {
//      printf("   final location: curReg=%i, curLttc=%i in %s\n", curreg,curLttc,gGeoManager->GetPath());
      printf("<= NRMLWR\n");
   }   
}

//_____________________________________________________________________________
void rgrpwr(const Int_t & /*flukaReg*/, const Int_t & /*ptrLttc*/, Int_t & /*g4Reg*/,
            Int_t * /*indMother*/, Int_t * /*repMother*/, Int_t & /*depthFluka*/)
{
   if (gMCGeom->IsDebugging()) printf("=> Dummy RGRPWR\n");
}

//_____________________________________________________________________________
Int_t isvhwr(const Int_t &check, const Int_t & intHist)
{
//   from FLUGG:
// Wrapper for saving current navigation history (fCheck=default) 
// and returning its pointer. If fCheck=-1 copy of history pointed 
// by intHist is made in NavHistWithCount object, and its pointer 
// is returned. fCheck=1 and fCheck=2 cases are only in debugging 
// version: an array is created by means of FGeometryInit functions
// (but could be a static int * ptrArray = new int[10000] with 
// file scope as well) that stores a flag for deleted/undeleted 
// histories and at the end of event is checked to verify that 
// all saved history objects have been deleted.

// For TGeo, just return the current node ID. No copy need to be made.

   if (gMCGeom->IsDebugging()) printf("=> Inside ISVHWR\n");
   if (check<0) return intHist;
   Int_t histInt = gGeoManager->GetCurrentNodeId()+1;
   if (gMCGeom->IsDebugging()) printf("<= ISVHWR: history is: %i in: %s\n", histInt, gGeoManager->GetPath());
   return histInt;
}



   
