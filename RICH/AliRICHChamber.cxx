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

#include "AliRICHChamber.h"
#include "AliRICHConst.h" //for r2d
#include "AliRICHParam.h"
#include <TRandom.h>
#include <TRotMatrix.h>
#include "AliRICHTresholdMap.h"
#include "AliSegmentation.h"
#include "AliRICHSegmentationV0.h"
#include "AliRICHGeometry.h"
#include "AliRICHResponse.h"

ClassImp(AliRICHChamber)	
//______________________________________________________________________________    
AliRICHChamber::AliRICHChamber() 
{//default ctor
  fpParam=0;    
  fpRotMatrix=0;
  
    fSegmentation = 0;
    fResponse = 0;
    fGeometry = 0;
    fReconstruction = 0;
    fTresh = 0;
    frMin = 0.1;
    frMax = 140;
    for(Int_t i=0; i<50; ++i) fIndexMap[i] = 0;
}
//______________________________________________________________________________
AliRICHChamber::AliRICHChamber(Int_t iModuleN,AliRICHParam *pParam)
{//named ctor. Defines all geometry parameters for the given module.
 // 4 5 6 |----> X
 // 1 2 3 | 
 //   0   V Z  
  SetCenter(0,pParam->Offset(),0);//put to 2 position   
  switch(iModuleN){
    case 0:
      RotateX(pParam->AngleYZ());
      fName="RICHc0";fTitle="RICH chamber 0";
      break;      
    case 1:
      RotateZ(pParam->AngleXY());      
      fName="RICHc1";fTitle="RICH chamber 1";
      break;      
    case 2:
      fName="RICHc2";fTitle="RICH chamber 2";
      break;      
    case 3:
      RotateZ(-pParam->AngleXY());      
      fName="RICHc3";fTitle="RICH chamber 3";
      break;      
    case 4:
      RotateX(-pParam->AngleYZ());  //������� �����, ������� �� �����������    
      RotateZ( pParam->AngleXY());      
      fName="RICHc4";fTitle="RICH chamber 4";
      break;      
    case 5:
      RotateX(-pParam->AngleYZ());
      fName="RICHc5";fTitle="RICH chamber 5";
      break;      
    case 6:
      RotateX(-pParam->AngleYZ());            
      RotateZ(-pParam->AngleXY());      
      fName="RICHc6";fTitle="RICH chamber 6";
      break;      
    default:
      Fatal("named ctor","Wrong chamber number %i, check muster class ctor",iModuleN);
  }//switch(iModuleN)
  RotateZ(pParam->AngleRot());//apply common rotation  
  new TRotMatrix("rot"+fName,"rot"+fName, Rot().ThetaX()*r2d, Rot().PhiX()*r2d,
                                          Rot().ThetaY()*r2d, Rot().PhiY()*r2d,
                                          Rot().ThetaZ()*r2d, Rot().PhiZ()*r2d);
  fpParam=pParam;
}
//______________________________________________________________________________

void AliRICHChamber::LocaltoGlobal(Float_t pos[3],Float_t Globalpos[3])
{//Local coordinates to global coordinates transformation

    Double_t *pMatrix;
    pMatrix =  fpRotMatrix->GetMatrix();
    Globalpos[0]=pos[0]*pMatrix[0]+pos[1]*pMatrix[3]+pos[2]*pMatrix[6];
    Globalpos[1]=pos[0]*pMatrix[1]+pos[1]*pMatrix[4]+pos[2]*pMatrix[7];
    Globalpos[2]=pos[0]*pMatrix[2]+pos[1]*pMatrix[5]+pos[2]*pMatrix[8];
    Globalpos[0]+=fX;
    Globalpos[1]+=fY;
    Globalpos[2]+=fZ;
}

void AliRICHChamber::GlobaltoLocal(Float_t pos[3],Float_t Localpos[3])
{// Global coordinates to local coordinates transformation
    TMatrix matrixCopy(3,3);
    Double_t *pMatrixOrig = fpRotMatrix->GetMatrix();
    for(Int_t i=0;i<3;i++)
      {
	for(Int_t j=0;j<3;j++)
	  matrixCopy(j,i)=pMatrixOrig[j+3*i];
      }
    matrixCopy.Invert();
    Localpos[0] = pos[0] - fX;
    Localpos[1] = pos[1] - fY;
    Localpos[2] = pos[2] - fZ;
    Localpos[0]=Localpos[0]*matrixCopy(0,0)+Localpos[1]*matrixCopy(0,1)+Localpos[2]*matrixCopy(0,2);
    Localpos[1]=Localpos[0]*matrixCopy(1,0)+Localpos[1]*matrixCopy(1,1)+Localpos[2]*matrixCopy(1,2);
    Localpos[2]=Localpos[0]*matrixCopy(2,0)+Localpos[1]*matrixCopy(2,1)+Localpos[2]*matrixCopy(2,2);
} 

void AliRICHChamber::DisIntegration(Float_t eloss, Float_t xhit, Float_t yhit,
				    Int_t& nnew,Float_t newclust[5][500],ResponseType res) 
{
//  Generates pad hits (simulated cluster) 
//  using the segmentation and the response model
    
    Float_t dx, dy;
    Float_t local[3];
    //Float_t source[3];
    Float_t global[3];
    //
    // Width of the integration area
    //
    dx=(fResponse->SigmaIntegration())*(fResponse->ChargeSpreadX());
    dy=(fResponse->SigmaIntegration())*(fResponse->ChargeSpreadY());
    //
    // Get pulse height from energy loss and generate feedback photons
    Float_t qtot=0;

    local[0]=xhit;
    // z-position of the wires relative to the RICH mother volume 
    // (2 mmm before CsI) old value: 6.076
    local[1]=1.276 + fGeometry->GetGapThickness()/2  - .2;
    //printf("AliRICHChamber feedback origin:%f",local[1]);
    local[2]=yhit;

    LocaltoGlobal(local,global);

    Int_t nFp=0;
    

    // To calculate wire sag, the origin of y-position must be the middle of the photcathode
    AliRICHSegmentationV0* segmentation = (AliRICHSegmentationV0*) GetSegmentationModel();
    Float_t newy;
    if (yhit>0)
      newy = yhit - segmentation->GetPadPlaneLength()/2;
    else
      newy = yhit + segmentation->GetPadPlaneLength()/2;
    
    if (res==kMip) {
	qtot = fResponse->IntPH(eloss, newy);
	nFp  = fResponse->FeedBackPhotons(global,qtot);
	//printf("feedbacks:%d\n",nFp);
    } else if (res==kCerenkov) {
	qtot = fResponse->IntPH(newy);
	nFp  = fResponse->FeedBackPhotons(global,qtot);
	//printf("feedbacks:%d\n",nFp);
    }

    //printf("Feedbacks:%d\n",nFp);
    
    //
    // Loop Over Pads
    
    Float_t qcheck=0, qp=0;
    
    nnew=0;
    for (fSegmentation->FirstPad(xhit, yhit, 0, dx, dy); 
	 fSegmentation->MorePads(); 
	 fSegmentation->NextPad()) 
      {
	qp= fResponse->IntXY(fSegmentation);
	qp= TMath::Abs(qp);
	
	//printf("Qp:%f Qtot %f\n",qp,qtot);
	
	if (qp > 1.e-4) {
	  qcheck+=qp;
	  //
	  // --- store signal information
	  newclust[0][nnew]=qp*qtot;
	  newclust[1][nnew]=fSegmentation->Ix();
	  newclust[2][nnew]=fSegmentation->Iy();
	  newclust[3][nnew]=fSegmentation->ISector();
	  nnew++;	
	  //printf("Newcluster:%d\n",i);
	}
      } // Pad loop
}//void AliRICHChamber::DisIntegration(...
//______________________________________________________________________________
void AliRICHChamber::GenerateTresholds()
{//Generates random treshold charges for all pads 
  Int_t nx = fSegmentation->Npx();
  Int_t ny = fSegmentation->Npy();

  fTresh = new AliRICHTresholdMap(fSegmentation);
  for(Int_t i=-nx/2;i<nx/2;i++){
    for(Int_t j=-ny/2;j<ny/2;j++){
      Int_t pedestal = (Int_t)(gRandom->Gaus(50, 10));
      fTresh->SetHit(i,j,pedestal);
    }
  }      
}//void AliRICHChamber::GenerateTresholds()
//______________________________________________________________________________
void AliRICHChamber::Print(Option_t *) const
{
  Info(fName.Data(),"r=%8.3f theta=%5.1f phi=%5.1f x=%8.3f y=%8.3f z=%8.3f",
                     Rho(), Theta()*r2d,Phi()*r2d ,   X(),    Y(),    Z());
}//void AliRICHChamber::Print(Option_t *option)const
