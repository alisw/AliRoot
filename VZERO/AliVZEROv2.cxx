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

/* $Id$ */

//////////////////////////////////////////////////////////////////////
//                                                                  //
//  (V-zero) detector  version 2  as designed by the Lyon group     //
//   All comments should be sent to Brigitte CHEYNIS :              //
//                                  b.cheynis@ipnl.in2p3.fr         // 
//   Geometry of the  4th of november 2002                          //
//  (circular instead of trapezoidal shapes as in previous versions //
//   plus changes in cell dimensions and offsets)                   // 
//   New coordinate system implemented in october 2003              //
//                                                                  //
//////////////////////////////////////////////////////////////////////

// --- Standard libraries ---
#include <Riostream.h>
#include <stdlib.h>
#include <string.h>

// --- ROOT libraries ---
#include <TClonesArray.h>
#include <TGeant3.h>
#include <TGeometry.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <TNode.h>
#include <TObjectTable.h>
#include <TTUBE.h>
#include <TTUBS.h>
#include <TVirtualMC.h>
#include <TParticle.h>

// --- AliRoot header files ---
#include "AliConst.h"
#include "AliMagF.h"
#include "AliVZEROLoader.h"
#include "AliVZEROdigit.h"
#include "AliVZEROhit.h"
#include "AliVZEROv2.h"
#include "AliMC.h"

ClassImp(AliVZEROv2)

//--------------------------------------------------------------------
AliVZEROv2:: AliVZEROv2():AliVZERO()
{
// Standard default constructor 
}
//--------------------------------------------------------------------
AliVZEROv2::AliVZEROv2(const char *name, const char *title):
 AliVZERO(name,title)
{

// Standard constructor for V-zero Detector  version 2

  Int_t i;

  printf("\n");
  for(i=0;i<26;i++) printf("*");
  printf(" Create VZERO object ");
  for(i=0;i<26;i++) printf("*");
  printf("\n");
  
}

//-------------------------------------------------------------------------
void AliVZEROv2::CreateGeometry()
{

// Creates the GEANT geometry of the V-zero Detector  version 2
  
  Int_t i;
  
  printf("\n");
  for(i=0;i<26;i++) printf("*");
  printf(" Create VZERO Geometry ");
  for(i=0;i<26;i++) printf("*");
  printf("\n");
      
  Int_t    *idtmed = fIdtmed->GetArray()-2999;

  Int_t    ndetR = 1;
  Int_t    ndetL = 1;
 
  Int_t    ncellsR = 1;
  Int_t    ncellsL = 1;
  
  Int_t    idrotm[999];
 
  Float_t  height1, height2, height3, height4, height5; 
  Float_t  height;
  Float_t  theta;  
  
  Float_t  halfThickQua;
  
  Float_t  zdet;
  Float_t  r0, r5;
  Float_t  pi = TMath::Pi();
    
  height1           =     1.82;           // height of cell 1, in cm
  height2           =     3.81;           // height of cell 2, in cm
  height3           =     4.72;           // height of cell 3, in cm
  height4           =     7.12;           // height of cell 4, in cm
  height5           =    10.83;           // height of cell 5, in cm
  
  theta             = pi/6.0/2.0;       // half angular opening = 15 degrees
    
  halfThickQua    = fThickness1/2.0;   // half thickness of elementary cell (inner ring)
    
  zdet              =    90.0 - 0.6 -fThickness/2.0;  // distance to vertex (along Z axis)
  r0                =    4.05;            // closest distance to center of the beam pipe
  height            =    height1 + height2 + height3 + height4 + height5;
  r5                =    r0 + height;

// Creation of mother volume v0LE - left part - :
// Entrance face at  +350.0 cm  (new coordinate system) ...

   Float_t   partube[3];
   
   partube[0] =  4.3;
   partube[1] = 45.0;
   partube[2] = fThickness1/2.0;   
    
   gMC->Gsvolu("V0LE","TUBE",idtmed[3005],partube,3);
     
// Creation of five rings - left part - :
// Entrance face at +350.0 cm  (new coordinate system) ... 

// Mother volume v0L0 in which will be set 5 scintillator cells 

  Float_t   partubs[5];  
    
  Float_t   r0Left      =   4.3;   
  Float_t   height1Left =   2.6; 
  Float_t   height2Left =   4.1;
  Float_t   height3Left =   6.4;
  Float_t   height4Left =  10.2;
  Float_t   height5Left =  16.9;
  Float_t   heightLeft  = height1Left + height2Left + height3Left 
                                      + height4Left + height5Left;
  Float_t   r5Left      = r0Left  + heightLeft; 
  
  partubs[0]      =  r0Left;
  partubs[1]      =  r5Left;
  partubs[2]      =  fThickness1/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;

  gMC->Gsvolu("V0L0","TUBS",idtmed[3010],partubs,5);  // air volume
  
  Float_t  r1Left =  r0Left + height1Left;        
     
  partubs[0]     =  r0Left;
  partubs[1]     =  r1Left;

  gMC->Gsvolu("V0L1","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L1",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY"); 

  Float_t   r2Left  =   r1Left + height2Left;       
  
  partubs[0]     =  r1Left;
  partubs[1]     =  r2Left;

  gMC->Gsvolu("V0L2","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L2",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY"); 
  
  Float_t   r3Left =   r2Left + height3Left;
   
  partubs[0]     =  r2Left;
  partubs[1]     =  r3Left;

  gMC->Gsvolu("V0L3","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L3",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");
  
  Float_t   r4Left =  r3Left + height4Left;
   
  partubs[0]     =  r3Left;
  partubs[1]     =  r4Left;

  gMC->Gsvolu("V0L4","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L4",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");

  partubs[0]     =  r4Left;
  partubs[1]     =  r5Left;
  partubs[3]     =  90.0-15.0;
  partubs[4]     = 120.0-30.0;
  
  gMC->Gsvolu("V0L5","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L5",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");
  
  partubs[3]     = 120.0-30.0;
  partubs[4]     = 120.0-15.0;
  
  gMC->Gsvolu("V0L6","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L6",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");
  

// Creation of mother volume v0RI - right part - :
  
  partube[0] = r0 - 0.2;
  partube[1] = r5 + 1.0;
  partube[2] = fThickness/2.0; 
      
  gMC->Gsvolu("V0RI","TUBE",idtmed[3010],partube,3);
  
// Creation of  carbon lids (3 mm thick) to keep v0RI box shut...
 
  partube[0] =   r0;
  partube[1] =   r5;
  partube[2] =   +0.3/2.0;
    
  gMC->Gsvolu("V0CA","TUBE",idtmed[3001],partube,3); 
  gMC->Gspos("V0CA",1,"V0RI",0.0,0.0, fThickness/2.0-partube[2],0,"ONLY");
  gMC->Gspos("V0CA",2,"V0RI",0.0,0.0,-fThickness/2.0+partube[2],0,"ONLY");
  
// Creation of aluminum rings to maintain the v0RI pieces ...

  partube[0] =   r0 - 0.2;
  partube[1] =   r0;
  partube[2] =   +fThickness/2.0;
   
  gMC->Gsvolu("V0IR","TUBE",idtmed[3003],partube,3);    
  gMC->Gspos("V0IR",1,"V0RI",0.0,0.0,0.0,0,"ONLY");

  partube[0] =   r5;
  partube[1] =   r5 + 1.0;
  partube[2] =   +fThickness/2.0;
 
  gMC->Gsvolu("V0ER","TUBE",idtmed[3003],partube,3);    
  gMC->Gspos("V0ER",1,"V0RI",0.0,0.0,0.0,0,"ONLY");
  
// Mother volume v0R0 in which will be set 5  scintillator cells 
  
  partubs[0]      =  r0;
  partubs[1]      =  r5;
  partubs[2]      =  fThickness/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;

  gMC->Gsvolu("V0R0","TUBS",idtmed[3010],partubs,5);  // air volume 

// Elementary cell of ring 1 :
// (the cells will be shifted by 3 mm to output fibers) 
   
  Float_t   offsetFibers =  0.7;
  Float_t   offset        =  fThickness/2.0 - 0.3 - fThickness1/2.0; 
  Float_t   r1            =  r0 + height1;
      
  partubs[0]     =  r0;
  partubs[1]     =  r1;
  partubs[2]     =  fThickness1/2.0;
  
  gMC->Gsvolu("V0R1","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R1",1,"V0R0", 0.0, 0.0 , -offset, 0,"ONLY"); 

// Elementary cell of ring 2 :

  Float_t   r2   =  r1 + height2;       
  
  partubs[0]     =  r1;
  partubs[1]     =  r2;

  gMC->Gsvolu("V0R2","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R2",1,"V0R0", 0.0, 0.0 , -offset + offsetFibers, 0,"ONLY"); 


// Elementary cell of ring 3 :
  
  Float_t   r3   =  r2 + height3;
   
  partubs[0]     =  r2;
  partubs[1]     =  r3;

  gMC->Gsvolu("V0R3","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R3",1,"V0R0", 0.0, 0.0 , -offset + 2.0 * offsetFibers, 0,"ONLY");

// Elementary cell of ring 4 :
  
  Float_t   r4   =  r3 + height4 ;
  
  partubs[0]     =  r3;
  partubs[1]     =  r4;

  gMC->Gsvolu("V0R4","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R4",1,"V0R0", 0.0, 0.0 ,  -offset + 3.0 * offsetFibers, 0,"ONLY");

// Elementary cells of ring 5 :

  partubs[0]     =  r4;
  partubs[1]     =  r5;
  partubs[3]     =  90.0-15.0;
  partubs[4]     = 120.0-30.0;
  
  gMC->Gsvolu("V0R5","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R5",1,"V0R0", 0.0, 0.0 , -offset + 4.0 * offsetFibers, 0,"ONLY");  

  partubs[3]     = 120.0-30.0;
  partubs[4]     = 120.0-15.0;
  
  gMC->Gsvolu("V0R6","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R6",1,"V0R0", 0.0, 0.0 ,  -offset + 4.0 * offsetFibers, 0,"ONLY");
   
  Float_t  phiDeg = 180./6.; 

// Right part : 
 
  for(Float_t  phi = 15.0; phi < 360.0; phi = phi + phiDeg)
      {        
      	AliMatrix(idrotm[902], 90.0, phi, 90.0, 90.0 +phi, 0.0 , 0.0);
        gMC->Gspos("V0R0",ndetR,"V0RI",0.0,
	                  0.0,0.0,idrotm[902],"ONLY");
	ndetR++;
       }

  gMC->Gspos("V0RI",1,"ALIC",0.0,0.0,-zdet,0,"ONLY");
 
  ncellsR = (ndetR - 1) * 6;  
  printf("    Number of cells on Right side =   %d\n",  ncellsR);    

// Left part :

  for(Float_t  phi = 15.0; phi < 360.0; phi = phi + phiDeg)
      {       
      	AliMatrix(idrotm[902], 90.0, phi, 90.0, 90.0 +phi, 0.0 , 0.0);
        gMC->Gspos("V0L0",ndetL,"V0LE",0.0,
	                  0.0,0.0,idrotm[902],"ONLY");
        ndetL++;
       }

  gMC->Gspos("V0LE",1,"ALIC",0.0,0.0,350.0+fThickness1/2.0,0,"ONLY");
 
  ncellsL = (ndetL - 1) * 6;
  printf("    Number of cells on Left side  =   %d\n",  ncellsL);    
  for(i=0;i<75;i++) printf("*");
  printf("\n");
           
}
            
//_____________________________________________________________________________
void AliVZEROv2::BuildGeometry()
{
  
// Builds simple ROOT TNode geometry for event display

  Int_t i;

  printf("\n");
  for(i=0;i<30;i++) printf("*");
  printf(" VZERO BuildGeometry ");
  for(i=0;i<30;i++) printf("*");
  printf("\n");

  TNode *top; 

  TNode *v0Rnode, *v0Rnode0, *v0Rnode6 , *v0Rnode7, *v0Rnode8, *v0Rnode9;
  TNode *v0Rnode1, *v0Rnode2, *v0Rnode3, *v0Rnode4, *v0Rnode5;
  TNode *v0Lnode, *v0Lnode0;
  TNode *v0Lnode1, *v0Lnode2, *v0Lnode3, *v0Lnode4, *v0Lnode5, *v0Lnode6;
   
  const int kColorVZERO  = kGreen;
 
  top = gAlice->GetGeometry()->GetNode("alice");

  Float_t  height1, height2, height3, height4, height5; 
  Float_t  height;
  Float_t  theta;  
  
  Float_t  halfThickQua;
  Float_t  zdet;
  Float_t  r0, r5;
  Float_t  pi = TMath::Pi();

  height1           =     1.82;           // height of cell 1, in cm
  height2           =     3.81;           // height of cell 2, in cm
  height3           =     4.72;           // height of cell 3, in cm
  height4           =     7.12;           // height of cell 4, in cm
  height5           =    10.83;           // height of cell 5, in cm  

  theta             =    pi/6.0/2.0;    
   
  halfThickQua    =    fThickness1/2.0; 
  
  zdet              =    90.0 - 0.6 - fThickness/2.0;   
  r0                =    4.05;         
  height            =    height1 + height2 + height3 + height4 + height5;
  r5                =    r0 + height;
  
  Int_t     ndiv    =     1;  

  Float_t   partube[3];

  partube[0] =  r0 - 0.2;
  partube[1] =  r5 + 1.0;
  partube[2] = fThickness/2.0;   
  
  TTUBE *v0RI = new TTUBE("V0RI", "V0RI", "void", partube[0], partube[1], partube[2]);
 		
  top->cd();
  
  v0Rnode = new TNode("V0RI","V0RI",v0RI,0.0,0.0,-zdet,0);
  
  v0Rnode->SetLineColor(kYellow);
  fNodes->Add(v0Rnode);  
  v0Rnode->SetVisibility(2);     
 
// Rondelles de carbone (epaisseur 3 mm) de maintien des cellules ...
  
  partube[0] =   r0;
  partube[1] =   r5;
  partube[2] =   +0.3/2.0;
  
  TTUBE  *v0CA = new TTUBE("V0CA", "V0CA", "void",partube[0], partube[1], partube[2]);
  
  v0Rnode->cd();
  v0Rnode6 = new TNode("V0CA", "V0CA",v0CA,0.0,0.0, fThickness/2.0-partube[2],0);	 
  v0Rnode6->SetLineColor(kYellow);
  fNodes->Add(v0Rnode6); 
  v0Rnode->cd();
  v0Rnode7 = new TNode("V0CA", "V0CA",v0CA,0.0,0.0,-fThickness/2.0+partube[2],0);	 
  v0Rnode7->SetLineColor(kYellow);
  fNodes->Add(v0Rnode7);
  
  partube[0] =   r0 - 0.2;
  partube[1] =   r0;
  partube[2] =   +fThickness/2.0;
  
  TTUBE *v0IR = new TTUBE("V0IR","V0IR","void", partube[0], partube[1], partube[2]);
 
  v0Rnode->cd();
  v0Rnode8 = new TNode("V0IR", "V0IR",v0IR,0.0,0.0,0.0,0);
  v0Rnode8->SetLineColor(kYellow);
  fNodes->Add(v0Rnode8);

  partube[0] =   r5;
  partube[1] =   r5 + 1.0; 
  partube[2] =   +fThickness/2.0;

  TTUBE  *v0ER = new TTUBE("V0ER","V0ER","void", partube[0], partube[1], partube[2]);
  
  v0Rnode->cd();
  v0Rnode9 = new TNode("V0ER", "V0ER",v0ER,0.0,0.0,0.0,0);
  v0Rnode9->SetLineColor(kYellow);
  fNodes->Add(v0Rnode9);
  
  Float_t   partubs[5];
 
  partubs[0]      =  r0;
  partubs[1]      =  r5;
  partubs[2]      =  fThickness/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;

  TTUBS  *v0R0 = new TTUBS("V0R0", "V0R0", "void",partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]); 
						  
  v0R0->SetNumberOfDivisions(ndiv); 						  

  Float_t   r1     =  r0 + height1;
  Float_t   offset = fThickness/2.0 - 0.3 - fThickness1/2.0; 
  Float_t   offsetFibers = 0.7;
    
  partubs[0]     =  r0;
  partubs[1]     =  r1;
  partubs[2]     =  fThickness1/2.0;

  TTUBS *v0R1 = new TTUBS("V0R1", "V0R1", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R1->SetNumberOfDivisions(ndiv);
  
  Float_t   r2   =  r1 + height2;       
  
  partubs[0]     =  r1;
  partubs[1]     =  r2;

  TTUBS *v0R2 = new TTUBS("V0R2", "V0R2", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R2->SetNumberOfDivisions(ndiv);
  
  Float_t   r3   =  r2 + height3;
   
  partubs[0]     =  r2;
  partubs[1]     =  r3;
  
  TTUBS *v0R3 = new TTUBS("V0R3", "V0R3", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0R3->SetNumberOfDivisions(ndiv);
 
  Float_t   r4   =  r3 + height4;
   
  partubs[0]     =  r3;
  partubs[1]     =  r4;

  TTUBS *v0R4 = new TTUBS("V0R4", "V0R4", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R4->SetNumberOfDivisions(ndiv);

  partubs[0]     =  r4;
  partubs[1]     =  r5;
  partubs[3]     =  90.0-15.0;
  partubs[4]     = 120.0-30.0;
  
  TTUBS *v0R5 = new TTUBS("V0R5", "V0R5", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R5->SetNumberOfDivisions(ndiv);

  partubs[3]     = 120.0-30.0;
  partubs[4]     = 120.0-15.0;
  
  TTUBS *v0R6 = new TTUBS("V0R6", "V0R6", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
						  
  v0R6->SetNumberOfDivisions(ndiv);
		
  Float_t  phi;
  Float_t  phiDeg= 180./6.;
    
  Int_t    ndetR = 1; 

  char     nameNode[12];  
 
  for (phi = 15.0; phi < 360.0; phi = phi + phiDeg)
  
  {
     
    TRotMatrix* mat920 = new TRotMatrix("rot920","rot920", 90.0, +phi, 90., 90.+phi, 0.0, 0.0 );        
     
    sprintf(nameNode,"SUBDER%d",ndetR);
    
    v0Rnode->cd();
    v0Rnode0 = new TNode(nameNode,nameNode,v0R0,0.0,0.0, 0.0,mat920);	 
    v0Rnode0->SetLineColor(kYellow);
    fNodes->Add(v0Rnode0);
    ndetR++;
    
    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode1 = new TNode(nameNode,nameNode,v0R1,0.0,0.0, -offset,0);	 
    v0Rnode1->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode1);
    ndetR++;
    
    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode2 = new TNode(nameNode,nameNode,v0R2,0.0,0.0, -offset + offsetFibers,0);	 
    v0Rnode2->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode2);
    ndetR++;

    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode3 = new TNode(nameNode,nameNode,v0R3,0.0,0.0, -offset + 2.0*offsetFibers,0);	 
    v0Rnode3->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode3);
    ndetR++;

    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode4 = new TNode(nameNode,nameNode,v0R4,0.0,0.0, -offset + 3.0*offsetFibers,0);	 
    v0Rnode4->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode4);
    ndetR++;
     
    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode5 = new TNode(nameNode,nameNode,v0R5,0.0,0.0, -offset + 4.0*offsetFibers,0);	 
    v0Rnode5->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode5);
    ndetR++;
    
    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode6 = new TNode(nameNode,nameNode,v0R6,0.0,0.0, -offset + 4.0*offsetFibers,0);	 
    v0Rnode6->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode6);
    ndetR++;
       
    v0Rnode0->SetVisibility(2);
    
  }    

// Left side of VZERO :
     
  Float_t   r0Left      =   4.3;   
  Float_t   height1Left =   2.6; 
  Float_t   height2Left =   4.1;
  Float_t   height3Left =   6.4;
  Float_t   height4Left =  10.2;
  Float_t   height5Left =  16.9;
  Float_t   heightLeft  = height1Left + height2Left + height3Left 
                                        + height4Left + height5Left;
  Float_t   r5Left      = r0Left  + heightLeft; 

  partube[0] =  r0Left;
  partube[1] =  r5Left;
  partube[2] =  fThickness1/2.0; 
  
  TTUBE *v0LE = new TTUBE("V0LE", "V0LE", "void", partube[0], partube[1], partube[2]);
 		
  top->cd();
  
  v0Lnode = new TNode("V0LE","V0LE",v0LE,0.0,0.0,350.0+fThickness1/2.0,0);
  
  v0Lnode->SetLineColor(kBlue);
  fNodes->Add(v0Lnode);
  
  v0Lnode->SetVisibility(2);

  partubs[0]      =  r0Left;
  partubs[1]      =  r5Left;
  partubs[2]      =  fThickness1/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;
  
  TTUBS *v0L0 = new TTUBS("V0L0", "V0L0", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0L0->SetNumberOfDivisions(ndiv); 
  v0L0->SetLineColor(7);
  
  Float_t   offset_left;
  offset_left    = - fThickness1/2.0; 

  Float_t   r1Left =  r0Left + height1Left;        
      
  partubs[0]     =  r0Left;
  partubs[1]     =  r1Left;

  TTUBS *v0L1 = new TTUBS("V0L1", "V0L1", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0L1->SetNumberOfDivisions(ndiv);
  
  Float_t   r2Left =  r1Left + height2Left;       
  
  partubs[0]     =  r1Left;
  partubs[1]     =  r2Left;

  TTUBS *v0L2 = new TTUBS("V0L2", "V0L2", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0L2->SetNumberOfDivisions(ndiv);
  
  Float_t   r3Left  =  r2Left + height3Left;
  
  partubs[0]     =  r2Left;
  partubs[1]     =  r3Left;
  
  TTUBS *v0L3 = new TTUBS("V0L3", "V0L3", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0L3->SetNumberOfDivisions(ndiv);
 
  Float_t   r4Left  =   r3Left + height4Left;
  
  partubs[0]     =  r3Left;
  partubs[1]     =  r4Left;

  TTUBS *v0L4 = new TTUBS("V0L4", "V0L4", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0L4->SetNumberOfDivisions(ndiv);

  partubs[0]     =  r4Left;
  partubs[1]     =  r5Left;
  partubs[3]     =  90.0-15.0;
  partubs[4]     = 120.0-30.0;
  
  TTUBS *v0L5 = new TTUBS("V0L5", "V0L5", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);


  v0L5->SetNumberOfDivisions(ndiv);

  partubs[3]     = 120.0-30.0;
  partubs[4]     = 120.0-15.0;
  
  TTUBS *v0L6 = new TTUBS("V0L6", "V0L6", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
						  
  v0L6->SetNumberOfDivisions(ndiv);

  Int_t    ndetL   = 1;
 
  for (phi = 15.0; phi < 360.0; phi = phi + phiDeg)
  
  {
     
    TRotMatrix* mat920 = new TRotMatrix("rot920","rot920", 90.0, +phi, 90., 90.+phi, 0.0, 0.0 );        
    
 
    sprintf(nameNode,"SUBDEL%d",ndetL);
    
    v0Lnode->cd();
    v0Lnode0 = new TNode(nameNode,nameNode,v0L0,0.0,0.0, offset_left + halfThickQua,mat920);	 
    v0Lnode0->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode0);
    ndetL++;
    
    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode1 = new TNode(nameNode,nameNode,v0L1,0.0,0.0, 0.0,0);	 
    v0Lnode1->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode1);
    ndetL++;
    
    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode2 = new TNode(nameNode,nameNode,v0L2,0.0,0.0, 0.0,0);	 
    v0Lnode2->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode2);
    ndetL++;


    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode3 = new TNode(nameNode,nameNode,v0L3,0.0,0.0, 0.0,0);	 
    v0Lnode3->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode3);
    ndetL++;

    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode4 = new TNode(nameNode,nameNode,v0L4,0.0,0.0, 0.0,0);	 
    v0Lnode4->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode4);
    ndetL++;
     
    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode5 = new TNode(nameNode,nameNode,v0L5,0.0,0.0, 0.0,0);	 
    v0Lnode5->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode5);
    ndetL++;
    
    sprintf(nameNode,"SUBDEL%d",ndetL);
    v0Lnode0->cd();    
    v0Lnode6 = new TNode(nameNode,nameNode,v0L6,0.0,0.0, 0.0,0);	 
    v0Lnode6->SetLineColor(kColorVZERO);
    fNodes->Add(v0Lnode6);
    ndetL++;
       
    v0Lnode0->SetVisibility(2);
    
  }    
     
}  
    
//------------------------------------------------------------------------
void AliVZEROv2::CreateMaterials()
{

// Creates materials used for geometry 

    Int_t i;

    printf("\n");
    for(i=0;i<25;i++) printf("*");
    printf(" VZERO create materials ");
    for(i=0;i<26;i++) printf("*");
    printf("\n");
    
/*
    Float_t ppckov[14] = { 5.5e-9, 5.7e-9, 5.9e-9, 6.1e-9, 6.3e-9, 6.5e-9, 6.7e-9, 
                           6.9e-9, 7.1e-9, 7.3e-9, 7.5e-9, 7.7e-9, 7.9e-9, 8.1e-9 };

           
    Float_t ppckov_alu[14] = { 5.5e-9, 5.7e-9, 5.9e-9, 6.1e-9, 6.3e-9, 6.5e-9, 6.7e-9, 
                               6.9e-9, 7.1e-9, 7.3e-9, 7.5e-9, 7.7e-9, 7.9e-9, 8.1e-9 };
			   
    Float_t rindex_quarz[14] = { 1.52398,  1.53090, 1.53835, 1.54641, 1.55513, 1.56458, 
                                 1.57488,  1.58611, 1.59842, 1.61197, 1.62696, 1.64362, 
                                 1.662295, 1.68337 };
				 
    Float_t absco_quarz[14] = { 105.8,  45.656, 35.665, 28.598, 25.007, 21.04, 17.525, 
                                14.177, 9.282, 4.0925, 1.149, 0.3627, 0.1497, 0.05 }; 	
				  							
    Float_t effic_all[14]   = { 1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1. };
    
        
    Float_t rindex_alu[14] = { 1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1. }; 
    
    
    Float_t absco_alu[14]  = { 1e-4,1e-4,1e-4,1e-4,1e-4,1e-4,1e-4,1e-4,1e-4,1e-4,
			       1e-4,1e-4,1e-4,1e-4 };
    Float_t effic_alu[14]  = { 1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1. };

*/	  

    Int_t *idtmed = fIdtmed->GetArray()-2999;
    
//    TGeant3 *geant3 = (TGeant3*) gMC;
    
//  Parameters related to Quarz (SiO2) :
 
    Float_t aqua[2], zqua[2], densqua, wmatqua[2];
    Int_t nlmatqua;
    
    aqua[0]    = 28.09;
    aqua[1]    = 16.;
    zqua[0]    = 14.;
    zqua[1]    = 8.;
    densqua    = 2.64;
    nlmatqua   = -2;
    wmatqua[0] = 1.;
    wmatqua[1] = 2.;

// Parameters  related to aluminum sheets :
    
    Float_t  aal   = 26.98;
    Float_t  zal   = 13.00; 
    Float_t  densal=   2.7; 
    Float_t  radlal=   8.9;
       
// Parameters  related to scintillator CH :
    
    Float_t ascin[2] = {1.00794,12.011};
    Float_t zscin[2] = {1.,6.};
    Float_t wscin[2] = {1.,1.};
    Float_t denscin  = 1.032;
    
//  Definition of materials :
       
    AliMaterial( 1, "AIR A$", 14.61, 7.3, .001205, 30420., 67500, 0, 0);
    AliMaterial(11, "AIR I$", 14.61, 7.3, .001205, 30420., 67500, 0, 0);
    AliMaterial( 2, "CARBON$"  , 12.01, 6.0, 2.265, 18.8, 49.9, 0, 0);
    AliMixture(  3, "QUA", aqua, zqua, densqua, nlmatqua, wmatqua);
    AliMaterial( 4, "ALUMINIUM1$", 26.98, 13., 2.7, 8.9, 37.2, 0, 0);
    AliMaterial( 5, "ALUMINIUM2$", aal, zal, densal, radlal, 0, 0, 0);
 
    AliMixture( 6, "Scintillator$",ascin,zscin,denscin,-2,wscin);
    
     
    Int_t   iSXFLD = gAlice->Field()->Integ();
    Float_t sXMGMX = gAlice->Field()->Max();
    
    Float_t tmaxfd, stemax, deemax, epsil, stmin;
        
    tmaxfd = 10.;
    stemax = 0.1;
    deemax = 0.1;     
    epsil  = 0.001;
    stmin  = 0.001;
  
//  Active Air :    
    AliMedium(1, "ACTIVE AIR$", 1, 1, iSXFLD, sXMGMX,
              10.0, 1.0, 0.1, 0.1, 10.0, 0, 0) ;

//  Inactive air : 
  
    AliMedium(11, "INACTIVE AIR$", 11, 0, iSXFLD, sXMGMX,
              10.0, 1.0, 0.1, 0.1, 10.0, 0, 0) ;
    
    AliMedium(2, "CARBON$ ", 2,  1, iSXFLD, sXMGMX,
              tmaxfd, stemax, deemax, epsil, stmin, 0, 0);   

    AliMedium(3, "QUARZ$", 3, 1, iSXFLD, sXMGMX,
              tmaxfd, fMaxStepQua, fMaxDestepQua, epsil, stmin, 0, 0);
    
    AliMedium(4,"ALUMINUM1$",4, 1, iSXFLD, sXMGMX,
              tmaxfd, fMaxStepAlu, fMaxDestepAlu, epsil, stmin, 0, 0);
	      

    AliMedium(5,"ALUMINUM2$",5, 1, iSXFLD, sXMGMX,
              tmaxfd, fMaxStepAlu, fMaxDestepAlu, epsil, stmin, 0, 0);    

    AliMedium(6,"SCINTILLATOR$",6, 1, iSXFLD, sXMGMX, 10.0, 0.1, 0.1, 0.003, 0.003, 0, 0);

    gMC->Gstpar(idtmed[3000], "LOSS", 1.);  //  [3000] = air ACTIF  [3010] = air INACTIF
    gMC->Gstpar(idtmed[3000], "HADR", 1.);
    gMC->Gstpar(idtmed[3000], "DCAY", 1.);
    gMC->Gstpar(idtmed[3000], "DRAY", 1.);
    
    gMC->Gstpar(idtmed[3001], "LOSS", 1.);  //  [3001] = carbon
    gMC->Gstpar(idtmed[3001], "HADR", 1.);
    gMC->Gstpar(idtmed[3001], "DCAY", 1.);
    gMC->Gstpar(idtmed[3001], "DRAY", 1.);

    gMC->Gstpar(idtmed[3002], "LOSS", 1.);  //  [3002] = quartz
    gMC->Gstpar(idtmed[3002], "HADR", 1.);
    gMC->Gstpar(idtmed[3002], "DCAY", 1.);
    gMC->Gstpar(idtmed[3002], "DRAY", 1.);  
    gMC->Gstpar(idtmed[3002], "CUTGAM",0.5E-4) ; 
    gMC->Gstpar(idtmed[3002], "CUTELE",1.0E-4) ;
    
    gMC->Gstpar(idtmed[3003], "LOSS", 1.);  //  [3003] = normal aluminum
    gMC->Gstpar(idtmed[3003], "HADR", 1.);
    gMC->Gstpar(idtmed[3003], "DCAY", 1.);
    gMC->Gstpar(idtmed[3003], "DRAY", 1.);
    
    gMC->Gstpar(idtmed[3004], "LOSS", 1.);  //  [3004] = reflecting aluminum
    gMC->Gstpar(idtmed[3004], "HADR", 1.);
    gMC->Gstpar(idtmed[3004], "DCAY", 1.);
    gMC->Gstpar(idtmed[3004], "DRAY", 1.);
    gMC->Gstpar(idtmed[3004], "CUTGAM",0.5E-4) ; 
    gMC->Gstpar(idtmed[3004], "CUTELE",1.0E-4) ;
    
    gMC->Gstpar(idtmed[3005], "LOSS", 1.);  //  [3005] = scintillator
    gMC->Gstpar(idtmed[3005], "HADR", 1.);
    gMC->Gstpar(idtmed[3005], "DCAY", 1.);
    gMC->Gstpar(idtmed[3005], "DRAY", 1.); 
    gMC->Gstpar(idtmed[3005], "CUTGAM",0.5E-4) ; 
    gMC->Gstpar(idtmed[3005], "CUTELE",1.0E-4) ;
      
    
//    geant3->Gsckov(idtmed[3002], 14, ppckov, absco_quarz, effic_all,rindex_quarz);    
//    geant3->Gsckov(idtmed[3004], 14, ppckov_alu, absco_alu, effic_alu, rindex_alu);

//    gMC->SetCerenkov(idtmed[3002], 14, ppckov, absco_quarz, effic_all,rindex_quarz);    
//    gMC->SetCerenkov(idtmed[3004], 14, ppckov_alu, absco_alu, effic_alu, rindex_alu);
                                   
    
}
//---------------------------------------------------------------------
void AliVZEROv2::DrawModule()
{

//  Drawing is done in DrawVZERO.C

   Int_t i;

   printf("\n");
   for(i=0;i<30;i++) printf("*");
   printf(" VZERO DrawModule ");
   for(i=0;i<30;i++) printf("*");
   printf("\n");


}

//-------------------------------------------------------------------
void AliVZEROv2::Init()
{
// Initialises version 1 of the VZERO Detector
// Just prints an information message
  
   printf(" VZERO version %d initialized \n",IsVersion());
   
//   gMC->SetMaxStep(fMaxStepAlu);
//   gMC->SetMaxStep(fMaxStepQua);
   
    AliVZERO::Init();
  
}

//-------------------------------------------------------------------

void AliVZEROv2::StepManager()
{
 
// Step Manager, called at each step 
   
     Int_t     copy;
     static    Int_t   vol[4];
     static    Float_t hits[19];
     static    Float_t eloss, tlength;
     
     TLorentzVector pos;     
     TLorentzVector mom;
     
     Float_t        theta;
     Float_t        phi;
     Float_t        kRaddeg = 180/TMath::Pi();
     Float_t        ringNumber;

     Int_t          ipart;
     Float_t        destep, step;
          

//   We keep only charged tracks :
     
     if ( !gMC->TrackCharge() || !gMC->IsTrackAlive() ) return; 


     vol[0]    = gMC->CurrentVolOffID(1, vol[1]);
     vol[2]    = gMC->CurrentVolID(copy);
     vol[3]    = copy;
     

     if      ( gMC->CurrentVolID(copy) == gMC->VolId("V0R1") ||
               gMC->CurrentVolID(copy) == gMC->VolId("V0L1") )
	       ringNumber = 1.0;
     else if ( gMC->CurrentVolID(copy) == gMC->VolId("V0R2") ||
               gMC->CurrentVolID(copy) == gMC->VolId("V0L2") ) 
	       ringNumber = 2.0;  
     else if ( gMC->CurrentVolID(copy) == gMC->VolId("V0R3") ||
               gMC->CurrentVolID(copy) == gMC->VolId("V0L3") )
	       ringNumber = 3.0;
     else if ( gMC->CurrentVolID(copy) == gMC->VolId("V0R4") ||
               gMC->CurrentVolID(copy) == gMC->VolId("V0L4") ) 	 
	       ringNumber = 4.0; 
     else if ( gMC->CurrentVolID(copy) == gMC->VolId("V0R5") ||
               gMC->CurrentVolID(copy) == gMC->VolId("V0L5") ||
	       gMC->CurrentVolID(copy) == gMC->VolId("V0L6") ||
	       gMC->CurrentVolID(copy) == gMC->VolId("V0R6") )	  
               ringNumber = 5.0; 
     else
     	       ringNumber = 0.0;

     if  (  ringNumber > 0.5  ) { 
     
        destep    = gMC->Edep();
	step      = gMC->TrackStep();
        eloss    += destep;
	tlength  += step; 
	
	 	 
        if  ( gMC->IsTrackEntering()  )  {  
       
            gMC->TrackPosition(pos);
     
            gMC->TrackMomentum(mom);      
            Double_t tc   = mom[0]*mom[0]+mom[1]*mom[1];
            Double_t pt   = TMath::Sqrt(tc);
	    Double_t pmom = TMath::Sqrt(tc+mom[2]*mom[2]);
            theta   = Float_t(TMath::ATan2(pt,Double_t(mom[2])))*kRaddeg;
            phi     = Float_t(TMath::ATan2(Double_t(mom[1]),Double_t(mom[0])))*kRaddeg;
     
            ipart  = gMC->TrackPid();

            hits[0]  = pos[0];
            hits[1]  = pos[1];
            hits[2]  = pos[2];	 	 
	    hits[3]  =  Float_t (ipart); 

	    hits[4]  = gMC->TrackTime();
            hits[5]  = gMC->TrackCharge();
	    hits[6]  = theta;
	    hits[7]  = phi;
	    hits[8]  = ringNumber;
	 
	    hits[9]  = pt;
	    hits[10] = pmom;
	    hits[11] = mom[0];
	    hits[12] = mom[1];
	    hits[13] = mom[2];
	    
	    TParticle *par = gAlice->GetMCApp()->Particle(gAlice->GetMCApp()->GetCurrentTrackNumber());
            hits[14] = par->Vx();
            hits[15] = par->Vy();
            hits[16] = par->Vz();
            
 	    tlength  = 0.0;
	    eloss    = 0.0;
	    
         }
	 
	 if( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()){
	 
	 hits[17] =   eloss;
	 hits[18] = tlength;
	 
         AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, hits);
	 	 
	 tlength  = 0.0;
	 eloss    = 0.0; 

	  
	 } 
    }
      
}

//_____________________________________________________________________________
void AliVZEROv2::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  
//  Adds a VZERO hit
  
  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliVZEROhit(fIshunt,track,vol,hits);
}

//---------------------------------------------------------------------
void AliVZEROv2::AddDigits(Int_t *tracks, Int_t* digits) 
{

//  Adds a VZERO digit

   TClonesArray  &ldigits = *fDigits;
   new(ldigits[fNdigits++]) AliVZEROdigit(tracks, digits);
}

//---------------------------------------------------------------------
void AliVZEROv2::MakeBranch(Option_t *option)
{
  
// Creates new branches in the current Root Tree
    
  char branchname[10];
  sprintf(branchname,"%s",GetName());
  printf(" fBufferSize = %d \n",fBufferSize);
  
  const char *cH = strstr(option,"H");
  
  if (fHits   && TreeH() && cH) {
    TreeH()->Branch(branchname,&fHits, fBufferSize);
    printf("* AliDetector::MakeBranch * Making Branch %s for hits\n",branchname);
  }     

  const char *cD = strstr(option,"D");
  //
  if (fDigits   && fLoader->TreeD() && cD) {
    fLoader->TreeD()->Branch(branchname,&fDigits, fBufferSize);
    printf("* AliDetector::MakeBranch * Making Branch %s for digits\n",branchname);
  }  
   
}
