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
//  (V-zero) detector  version 4  as designed by the Lyon group     //
//   All comments should be sent to Brigitte CHEYNIS :              //
//                                  b.cheynis@ipnl.in2p3.fr         // 
//   Geometry of the   24th of february 2004                        //
//  (now 4 rings instead of 3 rings as in previous version)         // 
//   V0R (now V0C) sits between Z values  -89.4 and  -84.9 cm       //
//   V0L (now V0A) sits between Z values +350.0 and +352.0 cm       //
//   New coordinate system has been implemented in october 2003     //
//                                                                  //
//////////////////////////////////////////////////////////////////////

// --- Standard libraries ---
#include <Riostream.h>
#include <stdlib.h>
#include <string.h>

// --- ROOT libraries ---
#include <TClonesArray.h>
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
#include "AliRun.h"
#include "AliMC.h"
#include "AliConst.h"
#include "AliLog.h"
#include "AliMagF.h"
#include "AliVZEROLoader.h"
#include "AliVZEROdigit.h"
#include "AliVZEROhit.h"
#include "AliVZEROv4.h"
 
ClassImp(AliVZEROv4)

//_____________________________________________________________________________
AliVZEROv4:: AliVZEROv4():AliVZERO()
{
// Standard default constructor 
}

//_____________________________________________________________________________
AliVZEROv4::AliVZEROv4(const char *name, const char *title):
 AliVZERO(name,title)
{

// Standard constructor for V-zero Detector  version 2

  AliDebug(2,"Create VZERO object");
      
  fLightYield              =  93.75; // Light yield in BC408 (93.75 eV per photon)
  fLightAttenuation        =   0.05; // Light attenuation in fiber (0.05 per meter)
  fnMeters                 =   15.0; // Number of meters of clear fibers to PM
  fFibToPhot               =    0.3; // Attenuation at fiber-photocathode interface
}

//_____________________________________________________________________________
void AliVZEROv4::CreateGeometry()
{

// Creates the GEANT geometry of the V-zero Detector  version 3
  
  AliDebug(2,"Create VZERO Geometry ");
      
  Int_t    *idtmed = fIdtmed->GetArray()-2999;

  Int_t    ndetR = 1;
  Int_t    ndetL = 1;
 
  Int_t    ncellsR = 1;
  Int_t    ncellsL = 1;
  
  Int_t    idrotm[999];
 
  Float_t  height1Right, height2Right, height3Right, height4Right; 
  Float_t  heightRight;
  Float_t  theta;  
  
  Float_t  halfThickQua;
  
  Float_t  zdet;
  Float_t  r0Right, r4Right;
  Float_t  pi = TMath::Pi();
    
  height1Right    =     3.42;  // height of cell 1, in cm
  height2Right    =     6.78;  // height of cell 2, in cm
  height3Right    =     7.11;  // height of cell 3, in cm
  height4Right    =    10.91;  // height of cell 4, in cm
  
  theta       = pi/6.0/2.0;       // half angular opening = 15 degrees
    
  halfThickQua= fThickness1/2.0;  // half thickness of elementary cell (inner ring)
  
// distance 0.6 cm in zdet accounts for the fact V0R box back lid sits 0.6 away from 
// absorber nose sitting at 90 cm. Will use -zdet later...
// size of V0R box  (fThickness) is increased by 1 mm as compared to version v2 

  fThickness  =    fThickness + 0.1;  
  zdet        =    90.0 - 0.6 - fThickness/2.0;  // distance to vertex (along Z axis)   
  r0Right     =    4.05;          // closest distance to center of the beam pipe
  heightRight =    height1Right + height2Right + height3Right + height4Right;
  r4Right     =    r0Right + heightRight;

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
  Float_t   height1Left =   3.6;  
  Float_t   height2Left =   6.4;  
  Float_t   height3Left =  14.9; 
  Float_t   height4Left =  14.4;  
  Float_t   heightLeft  =   height1Left + height2Left + height3Left + height4Left; 
                                      
  Float_t   r4Left      =   r0Left  + heightLeft; 
  
  partubs[0]     =  r0Left;
  partubs[1]     =  r4Left;
  partubs[2]     =  fThickness1/2.0;
  partubs[3]     =  90.0-15.0;
  partubs[4]     = 120.0-15.0;

  gMC->Gsvolu("V0L0","TUBS",idtmed[3010],partubs,5);  // air volume
  
  Float_t  r1Left =  r0Left + height1Left;        
     
  partubs[0]     =  r0Left;
  partubs[1]     =  r1Left;

  gMC->Gsvolu("V0L1","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L1",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY"); 

  Float_t  r2Left  =  r1Left + height2Left;       
  
  partubs[0]     =  r1Left;
  partubs[1]     =  r2Left;

  gMC->Gsvolu("V0L2","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L2",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY"); 
  
  Float_t  r3Left  =  r2Left + height3Left;       
      
  partubs[0]     =  r2Left;
  partubs[1]     =  r3Left;

  gMC->Gsvolu("V0L3","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L3",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");

  partubs[0]     =  r3Left;
  partubs[1]     =  r4Left;

  gMC->Gsvolu("V0L4","TUBS",idtmed[3005],partubs,5);  // quartz volume
  gMC->Gspos("V0L4",1,"V0L0", 0.0, 0.0 , 0.0, 0,"ONLY");  

// Creation of mother volume v0RI - right part - :
  
  partube[0] = r0Right - 0.2;
  partube[1] = r4Right + 1.0;
  partube[2] = fThickness/2.0; 
      
  gMC->Gsvolu("V0RI","TUBE",idtmed[3010],partube,3);
  
// Creation of  carbon lids (3.5 mm thick) to keep v0RI box shut...

  Float_t  lidThickness = 0.35;
 
  partube[0] =   r0Right;
  partube[1] =   r4Right;
  partube[2] =   +lidThickness/2.0;
    
  gMC->Gsvolu("V0CA","TUBE",idtmed[3001],partube,3); 
  gMC->Gspos("V0CA",1,"V0RI",0.0,0.0, fThickness/2.0-partube[2],0,"ONLY");
  gMC->Gspos("V0CA",2,"V0RI",0.0,0.0,-fThickness/2.0+partube[2],0,"ONLY");
  
// Creation of aluminum rings to maintain the v0RI pieces ...

  partube[0] =   r0Right - 0.2;
  partube[1] =   r0Right;
  partube[2] =   +fThickness/2.0;
   
  gMC->Gsvolu("V0IR","TUBE",idtmed[3003],partube,3);    
  gMC->Gspos("V0IR",1,"V0RI",0.0,0.0,0.0,0,"ONLY");

  partube[0] =   r4Right;
  partube[1] =   r4Right + 1.0;
  partube[2] =   +fThickness/2.0;
 
  gMC->Gsvolu("V0ER","TUBE",idtmed[3003],partube,3);    
  gMC->Gspos("V0ER",1,"V0RI",0.0,0.0,0.0,0,"ONLY");
  
// Mother volume v0R0 in which will be set 4  scintillator cells 
  
  partubs[0]      =  r0Right;
  partubs[1]      =  r4Right;
  partubs[2]      =  fThickness/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;

  gMC->Gsvolu("V0R0","TUBS",idtmed[3010],partubs,5);  // air volume 

// Elementary cell of ring 1 :
// (cells of ring 1  will be shifted by 1.7 cm towards vertex to output fibers) 
   
  Float_t   offsetFibers  =  1.7;
  Float_t   offset        =  fThickness/2.0 - lidThickness - fThickness1/2.0; 
  Float_t   r1Right       =  r0Right + height1Right;
      
  partubs[0]     =  r0Right;
  partubs[1]     =  r1Right;
  partubs[2]     =  fThickness1/2.0;
  
  gMC->Gsvolu("V0R1","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R1",1,"V0R0", 0.0, 0.0 , -offset + offsetFibers, 0,"ONLY"); 

// Elementary cell of ring 2 :

  Float_t   r2Right   =  r1Right + height2Right;       
  
  partubs[0]     =  r1Right;
  partubs[1]     =  r2Right;

  gMC->Gsvolu("V0R2","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R2",1,"V0R0", 0.0, 0.0 , -offset, 0,"ONLY"); 


// Elementary cell of ring 3 :

  Float_t   r3Right   =  r2Right + height3Right;   
     
  partubs[0]     =  r2Right;
  partubs[1]     =  r3Right;

  gMC->Gsvolu("V0R3","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R3",1,"V0R0", 0.0, 0.0 , -offset, 0,"ONLY");

// Elementary cell of ring 4 :
   
  partubs[0]     =  r3Right;
  partubs[1]     =  r4Right;

  gMC->Gsvolu("V0R4","TUBS",idtmed[3005],partubs,5);  // scintillator volume
  gMC->Gspos("V0R4",1,"V0R0", 0.0, 0.0 , -offset, 0,"ONLY");

  Float_t  phiDeg = 180./6.; 

// Right part : 
 
  for(Float_t  phi = 15.0 ; phi < 360.0; phi = phi + phiDeg)
      {        
      	AliMatrix(idrotm[902], 90.0, phi, 90.0, 90.0 +phi, 0.0 , 0.0);
        gMC->Gspos("V0R0",ndetR,"V0RI",0.0,
	                  0.0,0.0,idrotm[902],"ONLY");
	ndetR++;
       }

  gMC->Gspos("V0RI",1,"ALIC",0.0,0.0,-zdet,0,"ONLY");
 
  ncellsR = (ndetR - 1) * 4;  
  AliInfo(Form("Number of cells on Right side =   %d",  ncellsR));    

// Left part :
  
  for(Float_t  phi = 15.0 ; phi < 360.0; phi = phi + phiDeg)
      {       
      	AliMatrix(idrotm[902], 90.0, phi, 90.0, 90.0 +phi, 0.0 , 0.0);
        gMC->Gspos("V0L0",ndetL,"V0LE",0.0,
	                  0.0,0.0,idrotm[902],"ONLY");
        ndetL++;
       }

  gMC->Gspos("V0LE",1,"ALIC",0.0,0.0,350.0+fThickness1/2.0,0,"ONLY");
 
  ncellsL = (ndetL - 1) * 4;
  AliInfo(Form("Number of cells on Left side  =   %d",  ncellsL));    
           
}
            
//_____________________________________________________________________________
void AliVZEROv4::BuildGeometry()
{
  
// Builds simple ROOT TNode geometry for event display

  AliDebug(2,"VZERO BuildGeometry ");

  TNode *top; 

  TNode *v0Rnode, *v0Rnode0, *v0Rnode6 , *v0Rnode7, *v0Rnode8, *v0Rnode9;
  TNode *v0Rnode1, *v0Rnode2, *v0Rnode3, *v0Rnode4;
  TNode *v0Lnode, *v0Lnode0;
  TNode *v0Lnode1, *v0Lnode2, *v0Lnode3, *v0Lnode4;
   
  const int kColorVZERO  = kGreen;
 
  top = gAlice->GetGeometry()->GetNode("alice");

  Float_t  height1Right, height2Right, height3Right, height4Right; 
  Float_t  heightRight;
  Float_t  theta;  
  
  Float_t  halfThickQua;
  Float_t  zdet;
  Float_t  r0Right, r4Right;
  Float_t  pi = TMath::Pi();

  height1Right      =     3.42;  // height of cell 1, in cm
  height2Right      =     6.78;  // height of cell 2, in cm
  height3Right      =     7.11;  // height of cell 3, in cm
  height4Right      =    10.91;  // height of cell 4, in cm
    
  theta             =    pi/6.0/2.0;    
   
  halfThickQua      =    fThickness1/2.0; 
  
  zdet              =    90.0 - 0.6 - fThickness/2.0;   
  r0Right           =    4.05;         
  heightRight       =    height1Right + height2Right + height3Right + height4Right;
  r4Right           =    r0Right + heightRight;
  
  Int_t     ndiv    =     1;  

  Float_t   partube[3];

  partube[0] =  r0Right - 0.2;
  partube[1] =  r4Right + 1.0;
  partube[2] = fThickness/2.0;   
  
  TTUBE *v0RI = new TTUBE("V0RI", "V0RI", "void", partube[0], partube[1], partube[2]);
 		
  top->cd();
  
  v0Rnode = new TNode("V0RI","V0RI",v0RI,0.0,0.0,-zdet,0);
  
  v0Rnode->SetLineColor(kYellow);
  fNodes->Add(v0Rnode);  
  v0Rnode->SetVisibility(2);     
 
// Rondelles de carbone (epaisseur 3.5 mm) de maintien des cellules ...

  Float_t  lidThickness = 0.35;
  
  partube[0] =   r0Right;
  partube[1] =   r4Right;
  partube[2] =   +lidThickness/2.0;
  
  TTUBE  *v0CA = new TTUBE("V0CA", "V0CA", "void",partube[0], partube[1], partube[2]);
  
  v0Rnode->cd();
  v0Rnode6 = new TNode("V0CA", "V0CA",v0CA,0.0,0.0, fThickness/2.0-partube[2],0);	 
  v0Rnode6->SetLineColor(kYellow);
  fNodes->Add(v0Rnode6); 
  v0Rnode->cd();
  v0Rnode7 = new TNode("V0CA", "V0CA",v0CA,0.0,0.0,-fThickness/2.0+partube[2],0);	 
  v0Rnode7->SetLineColor(kYellow);
  fNodes->Add(v0Rnode7);
  
  partube[0] =   r0Right - 0.2;
  partube[1] =   r0Right;
  partube[2] =   +fThickness/2.0;
  
  TTUBE *v0IR = new TTUBE("V0IR","V0IR","void", partube[0], partube[1], partube[2]);
 
  v0Rnode->cd();
  v0Rnode8 = new TNode("V0IR", "V0IR",v0IR,0.0,0.0,0.0,0);
  v0Rnode8->SetLineColor(kYellow);
  fNodes->Add(v0Rnode8);

  partube[0] =   r4Right;
  partube[1] =   r4Right + 1.0; 
  partube[2] =   +fThickness/2.0;

  TTUBE  *v0ER = new TTUBE("V0ER","V0ER","void", partube[0], partube[1], partube[2]);
  
  v0Rnode->cd();
  v0Rnode9 = new TNode("V0ER", "V0ER",v0ER,0.0,0.0,0.0,0);
  v0Rnode9->SetLineColor(kYellow);
  fNodes->Add(v0Rnode9);
  
  Float_t   partubs[5];
 
  partubs[0]      =  r0Right;
  partubs[1]      =  r4Right;
  partubs[2]      =  fThickness/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;

  TTUBS  *v0R0 = new TTUBS("V0R0", "V0R0", "void",partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]); 
						  
  v0R0->SetNumberOfDivisions(ndiv); 						  

  Float_t   r1Right =  r0Right + height1Right;
  Float_t   offset  = fThickness/2.0 - lidThickness - fThickness1/2.0; 
  Float_t   offsetFibers = 1.7;
    
  partubs[0]     =  r0Right;
  partubs[1]     =  r1Right;
  partubs[2]     =  fThickness1/2.0;

  TTUBS *v0R1 = new TTUBS("V0R1", "V0R1", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R1->SetNumberOfDivisions(ndiv);
  
  Float_t   r2Right  =  r1Right + height2Right;       
  
  partubs[0]     =  r1Right;
  partubs[1]     =  r2Right;

  TTUBS *v0R2 = new TTUBS("V0R2", "V0R2", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0R2->SetNumberOfDivisions(ndiv);

  Float_t   r3Right  =  r2Right + height3Right;     
   
  partubs[0]     =  r2Right;
  partubs[1]     =  r3Right;
  
  TTUBS *v0R3 = new TTUBS("V0R3", "V0R3", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0R3->SetNumberOfDivisions(ndiv);

  partubs[0]     =  r3Right;
  partubs[1]     =  r4Right;
  
  TTUBS *v0R4 = new TTUBS("V0R4", "V0R4", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0R4->SetNumberOfDivisions(ndiv);
 	
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
    v0Rnode1 = new TNode(nameNode,nameNode,v0R1,0.0,0.0, -offset+ offsetFibers ,0);	 
    v0Rnode1->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode1);
    ndetR++;
    
    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode2 = new TNode(nameNode,nameNode,v0R2,0.0,0.0, -offset,0);	 
    v0Rnode2->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode2);
    ndetR++;

    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode3 = new TNode(nameNode,nameNode,v0R3,0.0,0.0, -offset,0);	 
    v0Rnode3->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode3);
    ndetR++;

    sprintf(nameNode,"SUBDER%d",ndetR);
    v0Rnode0->cd();    
    v0Rnode4 = new TNode(nameNode,nameNode,v0R4,0.0,0.0, -offset,0);	 
    v0Rnode4->SetLineColor(kColorVZERO);
    fNodes->Add(v0Rnode4);
    ndetR++;
       
    v0Rnode0->SetVisibility(2);    
  }    

// Left side of VZERO :
     
  Float_t   r0Left      =    4.3;   
  Float_t   height1Left =    3.6; 
  Float_t   height2Left =    6.4;
  Float_t   height3Left =   14.9; 
  Float_t   height4Left =   14.4;     
  Float_t   heightLeft  = height1Left + height2Left + height3Left + height4Left; 
                                        
  Float_t   r4Left      = r0Left  + heightLeft; 

  partube[0] =  r0Left;
  partube[1] =  r4Left;
  partube[2] =  fThickness1/2.0; 
  
  TTUBE *v0LE = new TTUBE("V0LE", "V0LE", "void", partube[0], partube[1], partube[2]);
 		
  top->cd();
  
  v0Lnode = new TNode("V0LE","V0LE",v0LE,0.0,0.0,350.0+fThickness1/2.0,0);
  
  v0Lnode->SetLineColor(kBlue);
  fNodes->Add(v0Lnode);
  
  v0Lnode->SetVisibility(2);

  partubs[0]      =  r0Left;
  partubs[1]      =  r4Left;
  partubs[2]      =  fThickness1/2.0;
  partubs[3]      =  90.0-15.0;
  partubs[4]      = 120.0-15.0;
  
  TTUBS *v0L0 = new TTUBS("V0L0", "V0L0", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);

  v0L0->SetNumberOfDivisions(ndiv); 
  v0L0->SetLineColor(7);
  
  Float_t   offsetLeft;
  offsetLeft    = - fThickness1/2.0; 

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

  Float_t   r3Left =  r2Left + height3Left; 
   
  partubs[0]     =  r2Left;
  partubs[1]     =  r3Left;
  
  TTUBS *v0L3 = new TTUBS("V0L3", "V0L3", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0L3->SetNumberOfDivisions(ndiv);

  partubs[0]     =  r3Left;
  partubs[1]     =  r4Left;
  
  TTUBS *v0L4 = new TTUBS("V0L4", "V0L4", "void", partubs[0], partubs[1], partubs[2], 
                                                  partubs[3], partubs[4]);
  v0L4->SetNumberOfDivisions(ndiv);
 
  Int_t    ndetL   = 1;
 
  for (phi = 15.0; phi < 360.0; phi = phi + phiDeg)
  
  {
     
    TRotMatrix* mat920 = new TRotMatrix("rot920","rot920", 90.0, +phi, 90., 90.+phi, 0.0, 0.0 );        
     
    sprintf(nameNode,"SUBDEL%d",ndetL);
    
    v0Lnode->cd();
    v0Lnode0 = new TNode(nameNode,nameNode,v0L0,0.0,0.0, offsetLeft + halfThickQua,mat920);	 
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
          
    v0Lnode0->SetVisibility(2);    
  }         
}  
    
//_____________________________________________________________________________
void AliVZEROv4::CreateMaterials()
{

// Creates materials used for geometry 

    AliDebug(2,"VZERO create materials");
    
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
    
// AIR

    Float_t aAir[4]={12.,14.,16.,36.};
    Float_t zAir[4]={6.,7.,8.,18.};
    Float_t wAir[4]={0.000124,0.755267,0.231781,0.012827};
    Float_t dAir = 1.20479E-3;
    
//  Definition of materials :

// AIR

    AliMixture( 1, "AIR A$", aAir,zAir,dAir,4,wAir);
    AliMixture(11, "AIR I$", aAir,zAir,dAir,4,wAir);
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

//_____________________________________________________________________________
void AliVZEROv4::DrawModule() const
{

//  Drawing is done in DrawVZERO.C

   AliDebug(2,"VZERO DrawModule");
}

//_____________________________________________________________________________
void AliVZEROv4::Init()
{
// Initialises version 2 of the VZERO Detector
// Just prints an information message
  
  AliDebug(2,Form("VZERO version %d initialized",IsVersion()));
   
//   gMC->SetMaxStep(fMaxStepAlu);
//   gMC->SetMaxStep(fMaxStepQua);
   
    AliVZERO::Init();  
}

//_____________________________________________________________________________
void AliVZEROv4::StepManager()
{
 
// Step Manager, called at each step 
 
     Int_t     copy;
     static    Int_t   vol[4];
     static    Float_t hits[21];
     static    Float_t eloss, tlength;
     static    Int_t   nPhotonsInStep;
     static    Int_t   nPhotons; 
     static    Int_t   numStep;
     Float_t   ringNumber;
     Float_t   destep, step;
     
     numStep += 1; 
          
//   We keep only charged tracks :
     
     if ( !gMC->TrackCharge() || !gMC->IsTrackAlive() ) return; 

     vol[0]    = gMC->CurrentVolOffID(1, vol[1]);
     vol[2]    = gMC->CurrentVolID(copy);
     vol[3]    = copy;
     
     static Int_t idV0R1 = gMC->VolId("V0R1");
     static Int_t idV0L1 = gMC->VolId("V0L1");
     static Int_t idV0R2 = gMC->VolId("V0R2");
     static Int_t idV0L2 = gMC->VolId("V0L2");
     static Int_t idV0R3 = gMC->VolId("V0R3");
     static Int_t idV0L3 = gMC->VolId("V0L3");
     static Int_t idV0R4 = gMC->VolId("V0R4");
     static Int_t idV0L4 = gMC->VolId("V0L4");

     if      ( gMC->CurrentVolID(copy) == idV0R1 ||
               gMC->CurrentVolID(copy) == idV0L1 )
	       ringNumber = 1.0;
     else if ( gMC->CurrentVolID(copy) == idV0R2 ||
               gMC->CurrentVolID(copy) == idV0L2 ) 
	       ringNumber = 2.0;  
     else if ( gMC->CurrentVolID(copy) == idV0R3 ||
               gMC->CurrentVolID(copy) == idV0L3 )
	       ringNumber = 3.0;
     else if ( gMC->CurrentVolID(copy) == idV0R4 ||
               gMC->CurrentVolID(copy) == idV0L4 )
	       ringNumber = 4.0;	       
     else
     	       ringNumber = 0.0;

     if  (  ringNumber > 0.5  ) { 
     
        destep    = gMC->Edep();
	step      = gMC->TrackStep();
        
	nPhotonsInStep  = Int_t(destep / (fLightYield *1e-9) );	
	nPhotonsInStep  = gRandom->Poisson(nPhotonsInStep);
	
	eloss    += destep;
	tlength  += step; 	 
	
        if  ( gMC->IsTrackEntering()  )  { 
	 
            nPhotons  =  nPhotonsInStep;       
	    gMC->TrackPosition(fTrackPosition);
	    gMC->TrackMomentum(fTrackMomentum);
	    
            Float_t pt  = TMath::Sqrt( fTrackMomentum.Px() * fTrackMomentum.Px() +
	                               fTrackMomentum.Py() * fTrackMomentum.Py() );
               
            hits[0]  = fTrackPosition.X();
            hits[1]  = fTrackPosition.Y();
            hits[2]  = fTrackPosition.Z();	 	 
	    hits[3]  = Float_t (gMC->TrackPid()); 

	    hits[4]  = gMC->TrackTime();
            hits[5]  = gMC->TrackCharge();
	    hits[6]  = fTrackMomentum.Theta()*TMath::RadToDeg();
	    hits[7]  = fTrackMomentum.Phi()*TMath::RadToDeg();
	    hits[8]  = ringNumber;
	 
	    hits[9]  = pt;
	    hits[10] = fTrackMomentum.P();
	    hits[11] = fTrackMomentum.Px();
	    hits[12] = fTrackMomentum.Py();
	    hits[13] = fTrackMomentum.Pz();
	    
	    TParticle *par = gAlice->GetMCApp()->Particle(gAlice->GetMCApp()->GetCurrentTrackNumber());
            hits[14] = par->Vx();
            hits[15] = par->Vy();
            hits[16] = par->Vz();
            
 	    tlength  = 0.0;
	    eloss    = 0.0;	    
         }
	 
	 nPhotons  = nPhotons + nPhotonsInStep;
	 
	 if( gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared()){
	 
	 nPhotons  = nPhotons - Int_t((Float_t(nPhotons) * fLightAttenuation * fnMeters));	 
	 nPhotons  = nPhotons - Int_t( Float_t(nPhotons) * fFibToPhot);	 
	 
	 hits[17] =   eloss;
	 hits[18] = tlength;
	 hits[19] = nPhotons;
	 hits[20] = GetCellId (vol, hits); 
	 	 
         AddHit(gAlice->GetMCApp()->GetCurrentTrackNumber(), vol, hits);
	 	 
	 tlength         = 0.0;
	 eloss           = 0.0; 
	 nPhotons        =   0;
	 nPhotonsInStep  =   0;
	 
	 numStep         =   0;  
	 } 
    }
      
}

//_____________________________________________________________________________
void AliVZEROv4::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  
//  Adds a VZERO hit
  
  TClonesArray &lhits = *fHits;
  new(lhits[fNhits++]) AliVZEROhit(fIshunt,track,vol,hits);
}

//_____________________________________________________________________________
void AliVZEROv4::AddDigits(Int_t *tracks, Int_t* digits) 
{

//  Adds a VZERO digit

   TClonesArray  &ldigits = *fDigits;
   new(ldigits[fNdigits++]) AliVZEROdigit(tracks, digits);
}

//_____________________________________________________________________________
void AliVZEROv4::MakeBranch(Option_t *option)
{
  
// Creates new branches in the current Root Tree
    
  char branchname[10];
  sprintf(branchname,"%s",GetName());
  AliDebug(2,Form("fBufferSize = %d",fBufferSize));
  
  const char *cH = strstr(option,"H");
  
  if (fHits   && TreeH() && cH) {
    TreeH()->Branch(branchname,&fHits, fBufferSize);
    AliDebug(2,Form("Making Branch %s for hits",branchname));
  }     

  const char *cD = strstr(option,"D");
  
  if (fDigits   && fLoader->TreeD() && cD) {
    fLoader->TreeD()->Branch(branchname,&fDigits, fBufferSize);
    AliDebug(2,Form("Making Branch %s for digits",branchname));
  }  
   
}

//_____________________________________________________________________________
Int_t AliVZEROv4::GetCellId(Int_t *vol, Float_t *hits) 
{

  //   Returns Id of scintillator cell
  //   Right side from  0 to 47
  //   Left  side from 48 to 95

   Int_t index = vol[1];
   fCellId     = 0;
   
   if (index < 10) index = index + 12; 
   
   if (hits[2] < 0.0) { 
      index = (index - 10) + ( ( Int_t(hits[8]) - 1 ) * 12);
      fCellId   = index;
   }
   else if (hits[2] > 0.0)
   {
      index = (index + 38) + ( ( Int_t(hits[8]) - 1 ) * 12);
      fCellId   = index;}
           
   return fCellId;
}
