/***************************************************************************
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
/*
$Log$
Revision 1.31  2003/10/31 12:25:36  bnandi
variable names are changed according to ALICE convention

Revision 1.30  2003/10/23 16:32:19  hristov
MC-dependent part of AliRun extracted in AliMC (F.Carminati)

Revision 1.29  2003/10/13 05:28:59  bnandi
gaspmd[2] value changed 0.25->7.0 because of overlap

Revision 1.28  2003/10/08 12:59:08  bnandi
zpos is positive

Revision 1.27  2003/10/08 12:56:58  bnandi
gaspmd[2] value changed from 7.0 to 0.25

Revision 1.26  2003/10/03 06:04:10  bnandi
z_psa and z_psb bugs fixed

Revision 1.25  2003/10/01 11:08:04  bnandi
changes for NewIO

Revision 1.24  2003/10/01 08:32:51  hristov
CurrentTrack replaced by GetCurrentTrackNumber

Revision 1.23  2003/10/01 05:07:51  bnandi
New geometry in new Alice Coordinate system

New rectangular geometry for ALICE PMD - Bedanga Mohanty and Y. P. Viyogi
June 2003
*/
//
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Photon Multiplicity Detector Version 1                                   //
//                                                                           //
//Begin_Html
/*
<img src="picts/AliPMDv1Class.gif">
*/
//End_Html
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
////

#include "AliPMDv1.h"
#include "AliRun.h"
#include "AliConst.h" 
#include "AliMagF.h" 
#include "Riostream.h"
#include <TVirtualMC.h>
#include "AliMC.h"

/* 
   static Int_t     gAliNcolUM1, gAliNcolUM2, gAliNrowUM1, gAliNrowUM2;
   static Int_t     gAliKdet;
   static Float_t   gAliSMLengthax, gAliSMLengthay;
   static Float_t   gAliSMLengthbx, gAliSMLengthby;
   static Float_t   gAliZdist, gAliZdist1;
   static Float_t   gAliSMthick, gAliCellRadius, gAliCellWall, gAliCellDepth;
   static Float_t   gAliBoundary, gAliThBase, gAliThAir, gAliThPCB;
   static Float_t   gAliThLead, gAliThSteel;
*/
ClassImp(AliPMDv1)
 
  //_____________________________________________________________________________
  AliPMDv1::AliPMDv1()
{
  //
  // Default constructor 
  //
  fMedSens=0;
}
 
//_____________________________________________________________________________
AliPMDv1::AliPMDv1(const char *name, const char *title)
  : AliPMD(name,title)
{
  //
  // Standard constructor
  //
  fMedSens=0;
}

//_____________________________________________________________________________
void AliPMDv1::CreateGeometry()
{
  // Create geometry for Photon Multiplicity Detector

  GetParameters();
  CreateSupermodule();
  CreatePMD();
}

//_____________________________________________________________________________
void AliPMDv1::CreateSupermodule()
{
  // 
  // Creates the geometry of the cells of PMD, places them in  supermodule 
  // which is a rectangular object.
  // Basic unit is ECAR, a hexagonal cell made of Ar+CO2, which is 
  // placed inside another hexagonal cell made of Cu (ECCU) with larger 
  // radius, compared to ECAR. The difference in radius gives the dimension 
  // of half width of each cell wall.
  // These cells are placed in a rectangular strip which are of 2 types 
  // EST1 and EST2 
  // 2 types of unit modules are made EUM1 and EUM2 which contains these strips
  // placed repeatedly 
  // Each supermodule (ESMA, ESMB), made of G10 is filled with following 
  //components. They have 9 unit moudles inside them
  // ESMA, ESMB are placed in EPMD along with EMPB (Pb converter) 
  // and EMFE (iron support) 

  
  Int_t i,j;
  Int_t number;
  Int_t ihrotm,irotdm;
  Float_t xb, yb, zb;

  //  const Float_t kroot3by2 = TMath::Sqrt(3.) /2.; 
  //  const Float_t kroot3 = TMath::Sqrt(3.); 

  Int_t *idtmed = fIdtmed->GetArray()-599;
 
  AliMatrix(ihrotm, 90., 30.,   90.,  120., 0., 0.);
  AliMatrix(irotdm, 90., 180.,  90.,  270., 180., 0.);
 
  //  gAliZdist = TMath::Abs(gAliZdist1);

  // First create the sensitive medium of a hexagon cell (ECAR)
  // Inner hexagon filled with gas (Ar+CO2)
  
  Float_t hexd2[10] = {0.,360.,6,2,-0.25,0.,0.23,0.25,0.,0.23};
  hexd2[4] = -fgkCellDepth/2.;
  hexd2[7] =  fgkCellDepth/2.;
  hexd2[6] =  fgkCellRadius - fgkCellWall;
  hexd2[9] =  fgkCellRadius - fgkCellWall;
  
  gMC->Gsvolu("ECAR", "PGON", idtmed[604], hexd2,10);
  gMC->Gsatt("ECAR", "SEEN", 0);
  
  // Place the sensitive medium inside a hexagon copper cell (ECCU)
  // Outer hexagon made of Copper
  
  Float_t hexd1[10] = {0.,360.,6,2,-0.25,0.,0.25,0.25,0.,0.25};
  hexd1[4] = -fgkCellDepth/2.;
  hexd1[7] =  fgkCellDepth/2.;
  hexd1[6] =  fgkCellRadius;
  hexd1[9] =  fgkCellRadius;

  gMC->Gsvolu("ECCU", "PGON", idtmed[614], hexd1,10);
  gMC->Gsatt("ECCU", "SEEN", 0);

  // Place  inner hex (sensitive volume) inside outer hex (copper)
  
  gMC->Gsposp("ECAR", 1, "ECCU", 0., 0., 0., 0, "ONLY", hexd2, 10);
  
  // Now create Rectangular TWO strips (EST1, EST2) 
  // of 1 column and 48 or 96 cells length

  // volume for first strip EST1 made of AIR 

  Float_t dbox1[3];
  dbox1[0] = fgkNcolUM1*fgkCellRadius;
  dbox1[1] = fgkCellRadius/fgkSqroot3by2;
  dbox1[2] = fgkCellDepth/2.;
  
  gMC->Gsvolu("EST1","BOX", idtmed[698], dbox1, 3);
  gMC->Gsatt("EST1", "SEEN", 0);

  // volume for second strip EST2 

  Float_t dbox2[3];
  dbox2[0] = fgkNcolUM2*fgkCellRadius;
  dbox2[1] = dbox1[1];
  dbox2[2] = dbox1[2];

  gMC->Gsvolu("EST2","BOX", idtmed[698], dbox2, 3);
  gMC->Gsatt("EST2", "SEEN", 0);

  // Place hexagonal cells ECCU placed inside EST1 
  yb = 0.; 
  zb = 0.;
  xb = -(dbox1[0]) + fgkCellRadius; 
  for (i = 1; i <= fgkNcolUM1; ++i) 
	{
	  number = i;
	  gMC->Gsposp("ECCU", number, "EST1", xb,yb,zb, ihrotm, "ONLY", hexd1,10);
	  xb += (fgkCellRadius*2.);
	}
  // Place hexagonal cells ECCU placed inside EST2 
      yb = 0.; 
      zb = 0.;
      xb = -(dbox2[0]) + fgkCellRadius; 
      for (i = 1; i <= fgkNcolUM2; ++i) 
	{
	  number = i;
	  gMC->Gsposp("ECCU", number, "EST2", xb,yb,zb, ihrotm, "ONLY", hexd1,10);
	  xb += (fgkCellRadius*2.);
	}



  // 2 types of rectangular shaped unit modules EUM1 and EUM2 (defined by BOX) 

  // Create EUM1

  Float_t dbox3[3];
  dbox3[0] = dbox1[0]+fgkCellRadius/2.;
  dbox3[1] = (dbox1[1]*fgkNrowUM1)-(fgkCellRadius*fgkSqroot3*(fgkNrowUM1-1)/6.); 
  dbox3[2] = fgkCellDepth/2.;
  
  gMC->Gsvolu("EUM1","BOX", idtmed[698], dbox3, 3);
  gMC->Gsatt("EUM1", "SEEN", 1);
  
  // Place rectangular strips EST1 inside EUM1 unit module

  yb = -dbox3[1]+dbox1[1];  
  for (j = 1; j <= fgkNrowUM1; ++j)  
    {
      if(j%2 == 0)
	{
      xb = fgkCellRadius/2.0;
	}
      else
	{
	  xb = -fgkCellRadius/2.0;
	}
      number = j;
      gMC->Gsposp("EST1",number, "EUM1", xb, yb , 0. , 0, "MANY",dbox1,3);
      yb = (-dbox3[1]+dbox1[1])+j*1.0*fgkCellRadius*fgkSqroot3;
    }

  // Create EUM2

  Float_t dbox4[3];
  dbox4[0] = dbox2[0] + fgkCellRadius/2.;
  dbox4[1] =(dbox2[1]*fgkNrowUM2)-(fgkCellRadius*fgkSqroot3*(fgkNrowUM2-1)/6.); 
  dbox4[2] = dbox3[2];

  gMC->Gsvolu("EUM2","BOX", idtmed[698], dbox4, 3);
  gMC->Gsatt("EUM2", "SEEN", 1);

  // Place rectangular strips EST2 inside EUM2 unit module

  yb = -dbox4[1]+dbox2[1]; 
  for (j = 1; j <= fgkNrowUM2; ++j) 
      {
      if(j%2 == 0)
	{
      xb = fgkCellRadius/2.0;
	}
      else
	{
	  xb = -fgkCellRadius/2.0;
	}
      number = j;
      gMC->Gsposp("EST2",number, "EUM2", xb, yb , 0. , 0, "MANY",dbox2,3);
      yb = (-dbox4[1]+dbox2[1])+j*1.0*fgkCellRadius*fgkSqroot3;
    }

  // 2 types of Rectangular shaped supermodules (BOX) 
  //each with 6 unit modules 
  
  // volume for SUPERMODULE ESMA 
  //Space added to provide a gapping for HV between UM's

  Float_t dboxSM1[3];
  dboxSM1[0] = 3.0*dbox3[0]+(2.0*0.025);
  dboxSM1[1] = 2.0*dbox3[1]+0.025;
  dboxSM1[2] = fgkCellDepth/2.;

  gMC->Gsvolu("ESMA","BOX", idtmed[698], dboxSM1, 3);
  gMC->Gsatt("ESMA", "SEEN", 1);

  //Position the 6 unit modules in EMSA
  Float_t xa1,xa2,xa3,ya1,ya2; 
  xa1 = -dboxSM1[0] + dbox3[0];
  xa2 = 0.;
  xa3 = dboxSM1[0]  - dbox3[0]; 
  ya1 = dboxSM1[1]  - dbox3[1];
  ya2 = -dboxSM1[1] + dbox3[1];
  
  gMC->Gsposp("EUM1", 1, "ESMA", xa1, ya1, 0., 0, "ONLY",dbox3,3);
  gMC->Gsposp("EUM1", 2, "ESMA", xa2, ya1, 0., 0, "ONLY",dbox3,3);
  gMC->Gsposp("EUM1", 3, "ESMA", xa3, ya1, 0., 0, "ONLY",dbox3,3);
  gMC->Gsposp("EUM1", 4, "ESMA", xa1, ya2, 0., 0, "ONLY",dbox3,3);
  gMC->Gsposp("EUM1", 5, "ESMA", xa2, ya2, 0., 0, "ONLY",dbox3,3);
  gMC->Gsposp("EUM1", 6, "ESMA", xa3, ya2, 0., 0, "ONLY",dbox3,3);


  // volume for SUPERMODULE ESMB 
  //Space is added to provide a gapping for HV between UM's
  Float_t dboxSM2[3];
  dboxSM2[0] = 2.0*dbox4[0]+0.025;
  dboxSM2[1] = 3.0*dbox4[1]+(2.0*0.025);
  dboxSM2[2] = fgkCellDepth/2.;
  
  gMC->Gsvolu("ESMB","BOX", idtmed[698], dboxSM2, 3);
  gMC->Gsatt("ESMB", "SEEN", 1);
 
  //Position the 6 unit modules in EMSB
  Float_t xb1,xb2,yb1,yb2,yb3; 
  xb1 = -dboxSM2[0] +dbox4[0];
  xb2 = dboxSM2[0]-dbox4[0];
  yb1 = dboxSM2[1]-dbox4[1];
  yb2 = 0.; 
  yb3 = -dboxSM2[1]+dbox4[1];
  
  gMC->Gsposp("EUM2", 1, "ESMB", xb1, yb1, 0., 0, "ONLY",dbox4,3);
  gMC->Gsposp("EUM2", 2, "ESMB", xb2, yb1, 0., 0, "ONLY",dbox4,3);
  gMC->Gsposp("EUM2", 3, "ESMB", xb1, yb2, 0., 0, "ONLY",dbox4,3);
  gMC->Gsposp("EUM2", 4, "ESMB", xb2, yb2, 0., 0, "ONLY",dbox4,3);
  gMC->Gsposp("EUM2", 5, "ESMB", xb1, yb3, 0., 0, "ONLY",dbox4,3);
  gMC->Gsposp("EUM2", 6, "ESMB", xb2, yb3, 0., 0, "ONLY",dbox4,3);


  // Make a 3mm thick G10 Base plate for ESMA
  Float_t dboxG1a[3];
  dboxG1a[0] = dboxSM1[0]; 
  dboxG1a[1] = dboxSM1[1];       
  dboxG1a[2] = fgkThBase/2.;

  gMC->Gsvolu("EBPA","BOX", idtmed[607], dboxG1a, 3);
  gMC->Gsatt("EBPA", "SEEN", 1);

  // Make a 1.6mm thick G10 PCB for ESMA
  Float_t dboxG2a[3];
  dboxG2a[0] = dboxSM1[0]; 
  dboxG2a[1] = dboxSM1[1];       
  dboxG2a[2] = fgkThPCB/2.;

  gMC->Gsvolu("EPCA","BOX", idtmed[607], dboxG2a, 3);
  gMC->Gsatt("EPCA", "SEEN", 1);


  // Make a Full module EFPA of AIR to place EBPA, 
  // 1mm AIR, EPCA, ESMA,EPCA for PMD
  
  Float_t dboxAlla[3];
  dboxAlla[0] = dboxSM1[0]; 
  dboxAlla[1] = dboxSM1[1];       
  dboxAlla[2] = (fgkThBase+fgkThAir+fgkThPCB+dboxSM1[2]+fgkThPCB)/2.;

  gMC->Gsvolu("EFPA","BOX", idtmed[698], dboxAlla, 3);
  gMC->Gsatt("EFPA", "SEEN", 1);


  // Make a Full module EFCA of AIR to place EBPA, 
  // 1mm AIR, EPCA, ESMA,EPC for CPV
  Float_t dboxAlla2[3];
  dboxAlla2[0] = dboxSM1[0]; 
  dboxAlla2[1] = dboxSM1[1];       
  dboxAlla2[2] = (fgkThBase+fgkThAir+fgkThPCB+dboxSM1[2]+fgkThPCB)/2.;

  gMC->Gsvolu("EFCA","BOX", idtmed[698], dboxAlla2, 3);
  gMC->Gsatt("EFCA", "SEEN", 1);

  // Now place everything in EFPA for PMD

  Float_t zbpa,zpcba1,zpcba2,zsma; 
  zpcba1 = - dboxAlla[2]+fgkThPCB/2.0;
  gMC->Gsposp("EPCA", 1, "EFPA", 0., 0., zpcba1, 0, "ONLY",dboxG2a,3);
  zsma = zpcba1+dboxSM1[2];
  gMC->Gsposp("ESMA", 1, "EFPA", 0., 0., zsma, 0, "ONLY",dboxSM1,3);
  zpcba2 = zsma+fgkThPCB/2.0;
  gMC->Gsposp("EPCA", 2, "EFPA", 0., 0., zpcba2, 0, "ONLY",dboxG2a,3);
  zbpa = zpcba2+fgkThAir+fgkThBase/2.0;
  gMC->Gsposp("EBPA", 1, "EFPA", 0., 0., zbpa, 0, "ONLY",dboxG1a,3);

  // Now place everything in EFCA for CPV

  Float_t zbpa2,zpcba12,zpcba22,zsma2; 
  zbpa2 = - dboxAlla2[2]+fgkThBase/2.0;
  gMC->Gsposp("EBPA", 1, "EFCA", 0., 0., zbpa2, 0, "ONLY",dboxG1a,3);
  zpcba12 = zbpa2+fgkThAir+fgkThPCB/2.0;
  gMC->Gsposp("EPCA", 1, "EFCA", 0., 0., zpcba12, 0, "ONLY",dboxG2a,3);
  zsma2 = zpcba12+dboxSM1[2];
  gMC->Gsposp("ESMA", 1, "EFCA", 0., 0., zsma2, 0, "ONLY",dboxSM1,3);
  zpcba22 = zsma2+fgkThPCB/2.0;
  gMC->Gsposp("EPCA", 2, "EFCA", 0., 0., zpcba22, 0, "ONLY",dboxG2a,3);



  // Make a 3mm thick G10 Base plate for ESMB
  Float_t dboxG1b[3];
  dboxG1b[0] = dboxSM2[0]; 
  dboxG1b[1] = dboxSM2[1];       
  dboxG1b[2] = fgkThBase/2.;

  gMC->Gsvolu("EBPB","BOX", idtmed[607], dboxG1b, 3);
  gMC->Gsatt("EBPB", "SEEN", 1);

  // Make a 1.6mm thick G10 PCB for ESMB
  Float_t dboxG2b[3];
  dboxG2b[0] = dboxSM2[0]; 
  dboxG2b[1] = dboxSM2[1];       
  dboxG2b[2] = fgkThPCB/2.;

  gMC->Gsvolu("EPCB","BOX", idtmed[607], dboxG2b, 3);
  gMC->Gsatt("EPCB", "SEEN", 1);


  // Make a Full module EFPB of AIR to place EBPB, 
  //1mm AIR, EPCB, ESMB,EPCB for PMD
  Float_t dboxAllb[3];
  dboxAllb[0] = dboxSM2[0]; 
  dboxAllb[1] = dboxSM2[1];       
  dboxAllb[2] = (fgkThBase+fgkThAir+fgkThPCB+dboxSM2[2]+fgkThPCB)/2.;

  gMC->Gsvolu("EFPB","BOX", idtmed[698], dboxAllb, 3);
  gMC->Gsatt("EFPB", "SEEN", 1);

  // Make a Full module EFCB of AIR to place EBPB, 
  //1mm AIR, EPCB, ESMB,EPCB for CPV
  Float_t dboxAllb2[3];
  dboxAllb2[0] = dboxSM2[0]; 
  dboxAllb2[1] = dboxSM2[1];       
  dboxAllb2[2] = (fgkThBase+fgkThAir+fgkThPCB+dboxSM2[2]+fgkThPCB)/2.;

  gMC->Gsvolu("EFCB","BOX", idtmed[698], dboxAllb2, 3);
  gMC->Gsatt("EFCB", "SEEN", 1);


  // Now place everything in EFPB for PMD

  Float_t zbpb,zpcbb1,zpcbb2,zsmb; 
  zpcbb1 = - dboxAllb[2]+fgkThPCB/2.0;
  gMC->Gsposp("EPCB", 1, "EFPB", 0., 0., zpcbb1, 0, "ONLY",dboxG2b,3);
  zsmb = zpcbb1+dboxSM2[2];
  gMC->Gsposp("ESMB", 1, "EFPB", 0., 0., zsmb, 0, "ONLY",dboxSM2,3);
  zpcbb2 = zsmb+fgkThPCB/2.0;
  gMC->Gsposp("EPCB", 2, "EFPB", 0., 0., zpcbb2, 0, "ONLY",dboxG2b,3);
  zbpb = zpcbb2+fgkThAir+fgkThBase/2.0;
  gMC->Gsposp("EBPB", 1, "EFPB", 0., 0., zbpb, 0, "ONLY",dboxG1b,3);


  // Now place everything in EFCB for CPV

  Float_t zbpb2,zpcbb12,zpcbb22,zsmb2; 
  zbpb2 = - dboxAllb2[2]+fgkThBase/2.0;
  gMC->Gsposp("EBPB", 1, "EFCB", 0., 0., zbpb2, 0, "ONLY",dboxG1b,3);
  zpcbb12 = zbpb2+0.1+fgkThPCB/2.0;
  gMC->Gsposp("EPCB", 1, "EFCB", 0., 0., zpcbb12, 0, "ONLY",dboxG2b,3);
  zsmb2 = zpcbb12+dboxSM2[2];
  gMC->Gsposp("ESMB", 1, "EFCB", 0., 0., zsmb2, 0, "ONLY",dboxSM2,3);
  zpcbb22 = zsmb2+fgkThPCB/2.0;
  gMC->Gsposp("EPCB", 2, "EFCB", 0., 0., zpcbb22, 0, "ONLY",dboxG2b,3);


  // Master MODULE EMPA of aluminum for PMD
  fDboxmm1[0] = dboxSM1[0]+fgkBoundary; 
  fDboxmm1[1] = dboxSM1[1]+fgkBoundary;       
  fDboxmm1[2] = dboxAlla[2];

  gMC->Gsvolu("EMPA","BOX", idtmed[603], fDboxmm1, 3);
  gMC->Gsatt("EMPA", "SEEN", 1);

  // Master MODULE EMCA of aluminum for CPV
  fDboxmm12[0] = dboxSM1[0]+fgkBoundary; 
  fDboxmm12[1] = dboxSM1[1]+fgkBoundary;       
  fDboxmm12[2] = dboxAlla[2];

  gMC->Gsvolu("EMCA","BOX", idtmed[603], fDboxmm12, 3);
  gMC->Gsatt("EMCA", "SEEN", 1);


  //Position EFMA inside EMMA for PMD and CPV
  gMC->Gsposp("EFPA", 1, "EMPA", 0., 0., 0., 0, "ONLY",dboxAlla,3);
  gMC->Gsposp("EFCA", 1, "EMCA", 0., 0., 0., 0, "ONLY",dboxAlla2,3);


  // Master MODULE EMPB of aluminum for PMD
  fDboxmm2[0] = dboxSM2[0]+fgkBoundary; 
  fDboxmm2[1] = dboxSM2[1]+fgkBoundary;       
  fDboxmm2[2] = dboxAllb[2];

  gMC->Gsvolu("EMPB","BOX", idtmed[603], fDboxmm2, 3);
  gMC->Gsatt("EMPB", "SEEN", 1);

  // Master MODULE EMCB of aluminum for CPV
  fDboxmm22[0] = dboxSM2[0]+fgkBoundary; 
  fDboxmm22[1] = dboxSM2[1]+fgkBoundary;       
  fDboxmm22[2] = dboxAllb[2];

  gMC->Gsvolu("EMCB","BOX", idtmed[603], fDboxmm22, 3);
  gMC->Gsatt("EMCB", "SEEN", 1);

 
  //Position EFMB inside EMMB
  gMC->Gsposp("EFPB", 1, "EMPB", 0., 0., 0., 0, "ONLY",dboxAllb,3);
  gMC->Gsposp("EFCB", 1, "EMCB", 0., 0., 0., 0, "ONLY",dboxAllb2,3);


}
 
//_____________________________________________________________________________

void AliPMDv1::CreatePMD()
{
  //
  // Create final detector from supermodules
  // -- Author : Bedanga and Viyogi June 2003

  Float_t  xp, yp, zp;
  Int_t jhrot12,jhrot13, irotdm;
  Int_t *idtmed = fIdtmed->GetArray()-599;
  
  //VOLUMES Names : begining with "E" for all PMD volumes, 

  // --- DEFINE Iron, and lead volumes  for SM A
  
  Float_t dboxPba[3];
  dboxPba[0] = fSMLengthax;
  dboxPba[1] = fSMLengthay;
  dboxPba[2] = fgkThLead/2.;
  
  gMC->Gsvolu("EPBA","BOX", idtmed[600], dboxPba, 3);
  gMC->Gsatt ("EPBA", "SEEN", 0);
  
  //   Fe Support 
  Float_t dboxFea[3];
  dboxFea[0] = fSMLengthax;
  dboxFea[1] = fSMLengthay;
  dboxFea[2] = fgkThSteel/2.;
  
  gMC->Gsvolu("EFEA","BOX", idtmed[618], dboxFea, 3);
  gMC->Gsatt ("EFEA", "SEEN", 0);

  // --- DEFINE Iron, and lead volumes  for SM B

  Float_t dboxPbb[3];
  dboxPbb[0] = fSMLengthbx;
  dboxPbb[1] = fSMLengthby;
  dboxPbb[2] = fgkThLead/2.;
  
  gMC->Gsvolu("EPBB","BOX", idtmed[600], dboxPbb, 3);
  gMC->Gsatt ("EPBB", "SEEN", 0);
  
  //   Fe Support 
  Float_t dboxFeb[3];
  dboxFeb[0] = fSMLengthbx;
  dboxFeb[1] = fSMLengthby;
  dboxFeb[2] = fgkThSteel/2.;
  
  gMC->Gsvolu("EFEB","BOX", idtmed[618], dboxFeb, 3);
  gMC->Gsatt ("EFEB", "SEEN", 0);


  // Gaspmd, the dimension of RECTANGULAR mother volume of PMD,

  Float_t gaspmd[3] = {81.5,94.5,7.};
  gaspmd[0] = fSMLengthax + fSMLengthbx;
  gaspmd[1] = fSMLengthay + fSMLengthby;

  gMC->Gsvolu("EPMD", "BOX", idtmed[698], gaspmd, 3);
  gMC->Gsatt("EPMD", "SEEN", 1);

  AliMatrix(irotdm, 90., 0.,  90.,  90., 180., 0.);
   
  AliMatrix(jhrot12, 90., 180., 90., 270., 0., 0.);
  AliMatrix(jhrot13, 90., 240., 90., 330., 0., 0.);

  Float_t xsma,ysma;
  Float_t xsmb,ysmb;
  xsma = -fSMLengthbx;
  ysma =  fSMLengthby;
  xsmb = -fSMLengthax;
  ysmb = -fSMLengthay;

  //Complete detector for Type A
  //Position Super modules type A for both CPV and PMD in EPMD  
  Float_t zpsa,zpba,zfea,zcva; 

  zpsa = - gaspmd[2] + fSMthick/2.;

  gMC->Gsposp("EMPA", 1, "EPMD", xsma, ysma, zpsa, 0, "ONLY",fDboxmm1,3);
  gMC->Gsposp("EMPA", 2, "EPMD", -xsma, -ysma, zpsa, jhrot12, "ONLY",fDboxmm1,3);
  zpba=zpsa+fSMthick/2.+dboxPba[2];
  gMC->Gsposp("EPBA", 1, "EPMD", xsma, ysma, zpba, 0, "ONLY",dboxPba,3);
  gMC->Gsposp("EPBA", 2, "EPMD", -xsma, -ysma, zpba, 0, "ONLY",dboxPba,3);
  zfea=zpba+dboxPba[2]+dboxFea[2];
  gMC->Gsposp("EFEA", 1, "EPMD", xsma, ysma, zfea, 0, "ONLY",dboxFea,3);
  gMC->Gsposp("EFEA", 2, "EPMD", -xsma, -ysma, zfea, 0, "ONLY",dboxFea,3);
  zcva=zfea+dboxFea[2]+fSMthick/2.;
  gMC->Gsposp("EMCA", 1, "EPMD", xsma, ysma, zcva, 0, "ONLY",fDboxmm12,3);
  gMC->Gsposp("EMCA", 2, "EPMD", -xsma,-ysma, zcva, jhrot12, "ONLY",fDboxmm12,3);
 
  //Complete detector for Type B
  //Position Super modules type B for both CPV and PMD in EPMD  
  Float_t zpsb,zpbb,zfeb,zcvb; 
  zpsb = - gaspmd[2] + fSMthick/2.;
  
  gMC->Gsposp("EMPB", 3, "EPMD", xsmb, ysmb, zpsb, 0, "ONLY",fDboxmm2,3);
  gMC->Gsposp("EMPB", 4, "EPMD", -xsmb, -ysmb, zpsb, jhrot12, "ONLY",fDboxmm2,3);
  zpbb=zpsb+fSMthick/2.+dboxPbb[2];
  gMC->Gsposp("EPBB", 3, "EPMD", xsmb, ysmb, zpbb, 0, "ONLY",dboxPbb,3);
  gMC->Gsposp("EPBB", 4, "EPMD", -xsmb, -ysmb, zpbb, 0, "ONLY",dboxPbb,3);
  zfeb=zpbb+dboxPbb[2]+dboxFeb[2];
  gMC->Gsposp("EFEB", 3, "EPMD", xsmb, ysmb, zfeb, 0, "ONLY",dboxFeb,3);
  gMC->Gsposp("EFEB", 4, "EPMD", -xsmb, -ysmb, zfeb, 0, "ONLY",dboxFeb,3);
  zcvb=zfeb+dboxFeb[2]+fSMthick/2.;
  gMC->Gsposp("EMCB", 3, "EPMD", xsmb, ysmb, zcvb, 0, "ONLY",fDboxmm22,3);
  gMC->Gsposp("EMCB", 4, "EPMD", -xsmb,-ysmb, zcvb, jhrot12, "ONLY",fDboxmm22,3);
  
  // --- Place the EPMD in ALICE 
  xp = 0.;
  yp = 0.;
  zp = fgkZdist;

  //Position Full PMD in ALICE   
  gMC->Gsposp("EPMD", 1, "ALIC", xp,yp,zp, 0, "ONLY",gaspmd,3);

}

 
//_____________________________________________________________________________
void AliPMDv1::DrawModule() const
{
  // Draw a shaded view of the Photon Multiplicity Detector
  //
  //  cout << " Inside Draw Modules " << endl;

  gMC->Gsatt("*", "seen", -1);
  gMC->Gsatt("alic", "seen", 0);
  //
  // Set the visibility of the components
  // 
  gMC->Gsatt("ECAR","seen",0);
  gMC->Gsatt("ECCU","seen",1);
  gMC->Gsatt("EST1","seen",1);
  gMC->Gsatt("EST2","seen",1);
  gMC->Gsatt("EUM1","seen",1);
  gMC->Gsatt("EUM2","seen",1);
  gMC->Gsatt("ESMA","seen",1);
  gMC->Gsatt("EPMD","seen",1);
  //
  gMC->Gdopt("hide", "on");
  gMC->Gdopt("shad", "on");
  gMC->Gsatt("*", "fill", 7);
  gMC->SetClipBox(".");
  gMC->SetClipBox("*", 0, 3000, -3000, 3000, -6000, 6000);
  gMC->DefaultRange();
  gMC->Gdraw("alic", 40, 30, 0, 22, 20.5, .02, .02);
  gMC->Gdhead(1111, "Photon Multiplicity Detector Version 1");

  //gMC->Gdman(17, 5, "MAN");
  gMC->Gdopt("hide", "off");

  cout << " Outside Draw Modules " << endl;
}

//_____________________________________________________________________________
void AliPMDv1::CreateMaterials()
{
  // Create materials for the PMD
  //
  // ORIGIN    : Y. P. VIYOGI 
  //
  //  cout << " Inside create materials " << endl;
  // --- The Argon- CO2 mixture --- 
  Float_t ag[2] = { 39.95 };
  Float_t zg[2] = { 18. };
  Float_t wg[2] = { 0.7,0.3 };
  Float_t dar   = 0.001782;   // --- Ar density in g/cm3 --- 
  // --- CO2 --- 
  Float_t ac[2] = { 12.,16. };
  Float_t zc[2] = { 6.,8. };
  Float_t wc[2] = { 1.,2. };
  Float_t dc    = 0.001977;
  Float_t dco   = 0.002;  // --- CO2 density in g/cm3 ---
  
  Float_t absl, radl, a, d, z;
  Float_t dg;
  Float_t x0ar;
  Float_t buf[1];
  Int_t nbuf;
  Float_t asteel[4] = { 55.847,51.9961,58.6934,28.0855 };
  Float_t zsteel[4] = { 26.,24.,28.,14. };
  Float_t wsteel[4] = { .715,.18,.1,.005 };
  
  Int_t *idtmed = fIdtmed->GetArray()-599;
  Int_t isxfld = gAlice->Field()->Integ();
  Float_t sxmgmx = gAlice->Field()->Max();
  
  // --- Define the various materials for GEANT --- 
  AliMaterial(1, "Pb    $", 207.19, 82., 11.35, .56, 18.5);
  x0ar = 19.55 / dar;
  AliMaterial(2, "Argon$", 39.95, 18., dar, x0ar, 6.5e4);
  AliMixture(3, "CO2  $", ac, zc, dc, -2, wc);
  AliMaterial(4, "Al   $", 26.98, 13., 2.7, 8.9, 18.5);
  AliMaterial(6, "Fe   $", 55.85, 26., 7.87, 1.76, 18.5);
  AliMaterial(7, "W    $", 183.85, 74., 19.3, .35, 10.3);
  AliMaterial(8, "G10  $", 20., 10., 1.7, 19.4, 999.);
  AliMaterial(9, "SILIC$", 28.09, 14., 2.33, 9.36, 45.);
  AliMaterial(10, "Be   $", 9.01, 4., 1.848, 35.3, 36.7);
  AliMaterial(15, "Cu   $", 63.54, 29., 8.96, 1.43, 15.);
  AliMaterial(16, "C    $", 12.01, 6., 2.265, 18.8, 49.9);
  AliMaterial(17, "POLYCARBONATE    $", 20., 10., 1.2, 34.6, 999.);
  AliMixture(19, "STAINLESS STEEL$", asteel, zsteel, 7.88, 4, wsteel); 
  // AliMaterial(31, "Xenon$", 131.3, 54., dxe, x0xe, 6.5e4);
  
  AliMaterial(96, "MYLAR$", 8.73, 4.55, 1.39, 28.7, 62.);
  AliMaterial(97, "CONCR$", 20., 10., 2.5, 10.7, 40.);
  AliMaterial(98, "Vacum$", 1e-9, 1e-9, 1e-9, 1e16, 1e16);
  AliMaterial(99, "Air  $", 14.61, 7.3, .0012, 30420., 67500.);
 
  // 	define gas-mixtures 
  
  char namate[21];
  gMC->Gfmate((*fIdmate)[3], namate, a, z, d, radl, absl, buf, nbuf);
  ag[1] = a;
  zg[1] = z;
  dg = (dar * 4 + dco) / 5;
  AliMixture(5, "ArCO2$", ag, zg, dg, 2, wg);
  
  // Define tracking media 
  AliMedium(1, "Pb conv.$", 1,  0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(7, "W  conv.$", 7,  0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(8, "G10plate$", 8,  0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  AliMedium(4, "Al      $", 4,  0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(6, "Fe      $", 6,  0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(5, "ArCO2   $", 5,  1, 0, isxfld, sxmgmx, .1,  .1, .1,  .1);
  AliMedium(9, "SILICON $", 9,  1, 0, isxfld, sxmgmx, .1,  .1, .1,  .1);
  AliMedium(10, "Be      $", 10, 0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(98, "Vacuum  $", 98, 0, 0, isxfld, sxmgmx, 1., .1, .1,  10);
  AliMedium(99, "Air gaps$", 99, 0, 0, isxfld, sxmgmx, 1., .1, .1,  .1);
  AliMedium(15, "Cu      $", 15, 0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(16, "C       $", 16, 0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(17, "PLOYCARB$", 17, 0, 0, isxfld, sxmgmx, .1,  .1, .01, .1);
  AliMedium(19, " S steel$", 19, 0, 0, isxfld, sxmgmx, 1., .1, .01, .1);
  //  AliMedium(31, "Xenon   $", 31,  1, 0, isxfld, sxmgmx, .1,  .1, .1,  .1);
  
  // --- Generate explicitly delta rays in the iron, aluminium and lead --- 
  gMC->Gstpar(idtmed[600], "LOSS", 3.);
  gMC->Gstpar(idtmed[600], "DRAY", 1.);
  
  gMC->Gstpar(idtmed[603], "LOSS", 3.);
  gMC->Gstpar(idtmed[603], "DRAY", 1.);
  
  gMC->Gstpar(idtmed[604], "LOSS", 3.);
  gMC->Gstpar(idtmed[604], "DRAY", 1.);
  
  gMC->Gstpar(idtmed[605], "LOSS", 3.);
  gMC->Gstpar(idtmed[605], "DRAY", 1.);
  
  gMC->Gstpar(idtmed[606], "LOSS", 3.);
  gMC->Gstpar(idtmed[606], "DRAY", 1.);
  
  gMC->Gstpar(idtmed[607], "LOSS", 3.);
  gMC->Gstpar(idtmed[607], "DRAY", 1.);
  
  // --- Energy cut-offs in the Pb and Al to gain time in tracking --- 
  // --- without affecting the hit patterns --- 
  gMC->Gstpar(idtmed[600], "CUTGAM", 1e-4);
  gMC->Gstpar(idtmed[600], "CUTELE", 1e-4);
  gMC->Gstpar(idtmed[600], "CUTNEU", 1e-4);
  gMC->Gstpar(idtmed[600], "CUTHAD", 1e-4);
  gMC->Gstpar(idtmed[605], "CUTGAM", 1e-4);
  gMC->Gstpar(idtmed[605], "CUTELE", 1e-4);
  gMC->Gstpar(idtmed[605], "CUTNEU", 1e-4);
  gMC->Gstpar(idtmed[605], "CUTHAD", 1e-4);
  gMC->Gstpar(idtmed[606], "CUTGAM", 1e-4);
  gMC->Gstpar(idtmed[606], "CUTELE", 1e-4);
  gMC->Gstpar(idtmed[606], "CUTNEU", 1e-4);
  gMC->Gstpar(idtmed[606], "CUTHAD", 1e-4);
  gMC->Gstpar(idtmed[603], "CUTGAM", 1e-4);
  gMC->Gstpar(idtmed[603], "CUTELE", 1e-4);
  gMC->Gstpar(idtmed[603], "CUTNEU", 1e-4);
  gMC->Gstpar(idtmed[603], "CUTHAD", 1e-4);
  gMC->Gstpar(idtmed[609], "CUTGAM", 1e-4);
  gMC->Gstpar(idtmed[609], "CUTELE", 1e-4);
  gMC->Gstpar(idtmed[609], "CUTNEU", 1e-4);
  gMC->Gstpar(idtmed[609], "CUTHAD", 1e-4);
  
  // --- Prevent particles stopping in the gas due to energy cut-off --- 
  gMC->Gstpar(idtmed[604], "CUTGAM", 1e-5);
  gMC->Gstpar(idtmed[604], "CUTELE", 1e-5);
  gMC->Gstpar(idtmed[604], "CUTNEU", 1e-5);
  gMC->Gstpar(idtmed[604], "CUTHAD", 1e-5);
  gMC->Gstpar(idtmed[604], "CUTMUO", 1e-5);

  cout << " Outside create materials " << endl;

}

//_____________________________________________________________________________
void AliPMDv1::Init()
{
  //
  // Initialises PMD detector after it has been built
  //

  Int_t i;
  //  gAliKdet=1;
  //
  cout << " Inside Init " << endl;
  if(fDebug) {
      printf("\n%s: ",ClassName());
      for(i=0;i<35;i++) printf("*");
      printf(" PMD_INIT ");
      for(i=0;i<35;i++) printf("*");
      printf("\n%s: ",ClassName());
      printf("                 PMD simulation package (v1) initialised\n");
      printf("%s: parameters of pmd\n",ClassName());
      printf("%s: %10.2f %10.2f %10.2f \
      %10.2f\n",ClassName(),fgkCellRadius,fgkCellWall,fgkCellDepth,fgkZdist );
      printf("%s: ",ClassName());
      for(i=0;i<80;i++) printf("*");
      printf("\n");
  }
  
  Int_t *idtmed = fIdtmed->GetArray()-599;
  fMedSens=idtmed[605-1];

}

//_____________________________________________________________________________
void AliPMDv1::StepManager()
{
  //
  // Called at each step in the PMD
  //

  Int_t   copy;
  Float_t hits[4], destep;
  Float_t center[3] = {0,0,0};
  Int_t   vol[8];
  //const char *namep;
  
  if(gMC->GetMedium() == fMedSens && (destep = gMC->Edep())) {
  
    gMC->CurrentVolID(copy);
    //namep=gMC->CurrentVolName();
    //printf("Current vol  is %s \n",namep);
    vol[0]=copy;

    gMC->CurrentVolOffID(1,copy);
    //namep=gMC->CurrentVolOffName(1);
    //printf("Current vol 11 is %s \n",namep);
    vol[1]=copy;

    gMC->CurrentVolOffID(2,copy);
    //namep=gMC->CurrentVolOffName(2);
    //printf("Current vol 22 is %s \n",namep);
    vol[2]=copy;

    //	if(strncmp(namep,"EHC1",4))vol[2]=1;

    gMC->CurrentVolOffID(3,copy);
    //namep=gMC->CurrentVolOffName(3);
    //printf("Current vol 33 is %s \n",namep);
    vol[3]=copy;

    gMC->CurrentVolOffID(4,copy);
    //namep=gMC->CurrentVolOffName(4);
    //printf("Current vol 44 is %s \n",namep);
    vol[4]=copy;

    gMC->CurrentVolOffID(5,copy);
    //namep=gMC->CurrentVolOffName(5);
    //printf("Current vol 55 is %s \n",namep);
    vol[5]=copy;

    gMC->CurrentVolOffID(6,copy);
    //namep=gMC->CurrentVolOffName(6);
    //printf("Current vol 66 is %s \n",namep);
    vol[6]=copy;

    gMC->CurrentVolOffID(7,copy);
    //namep=gMC->CurrentVolOffName(7);
    //printf("Current vol 77 is %s \n",namep);
    vol[7]=copy;


    //printf("volume number %4d %4d %4d %4d %4d %4d %4d %4d %10.3f \n",vol[0],vol[1],vol[2],vol[3],vol[4],vol[5],vol[6],vol[7],destep*1000000);
    
    gMC->Gdtom(center,hits,1);
    hits[3] = destep*1e9; //Number in eV
    AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, hits);

  }
}

  
//------------------------------------------------------------------------
// Get parameters

void AliPMDv1::GetParameters()
{
  // This gives all the parameters of the detector
  // such as Length of Supermodules, type A, type B,
  // thickness of the Supermodule
  //
  
  fSMLengthax = (3.0*(fgkNcolUM1*fgkCellRadius+fgkCellRadius/2.)
		 + (2.0*fgkGap)) + fgkBoundary;
  fSMLengthbx = 2.0*(fgkNcolUM2*fgkCellRadius+fgkCellRadius/2.)
    + fgkGap + fgkBoundary; 
  
  fSMLengthay = 2.0*(((fgkCellRadius/fgkSqroot3by2)*fgkNrowUM1)
		     - (fgkCellRadius*fgkSqroot3*(fgkNrowUM1-1)/6.))
    + fgkGap + fgkBoundary;
  fSMLengthby = 3.0*(((fgkCellRadius/fgkSqroot3by2)*fgkNrowUM2)
		     - (fgkCellRadius*fgkSqroot3*(fgkNrowUM2-1)/6.))
    + (2.0*fgkGap) + fgkBoundary;
  
  fSMthick    = fgkThBase + fgkThAir + fgkThPCB 
    + fgkCellDepth + fgkThPCB + fgkThAir + fgkThPCB;
  
}
