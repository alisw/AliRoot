// $Id$
//
// Class AliMUONSt1GeometryBuilder
// -------------------------------
// MUON Station1 coarse geometry construction class.
// Extracted from AliMUONv1
// by Ivana Hrivnacova, IPN Orsay
// Included in AliRoot 2004/01/23

#include <TVirtualMC.h>
#include <TGeoMatrix.h>

#include "AliMUONSt1GeometryBuilder.h"
#include "AliMUON.h"
#include "AliMUONChamber.h"
#include "AliMUONChamberGeometry.h"

ClassImp(AliMUONSt1GeometryBuilder)

//______________________________________________________________________________
AliMUONSt1GeometryBuilder::AliMUONSt1GeometryBuilder(AliMUON* muon)
 : AliMUONVGeometryBuilder(&muon->Chamber(0), &muon->Chamber(1)),
   fMUON(muon)
{
// Standard constructor

}

//______________________________________________________________________________
AliMUONSt1GeometryBuilder::AliMUONSt1GeometryBuilder()
 : AliMUONVGeometryBuilder(),
   fMUON(0)
{
// Default constructor
}


//______________________________________________________________________________
AliMUONSt1GeometryBuilder::AliMUONSt1GeometryBuilder(const AliMUONSt1GeometryBuilder& rhs)
  : AliMUONVGeometryBuilder(rhs)
{
// Protected copy constructor

  Fatal("Copy constructor", 
        "Copy constructor is not implemented.");
}

//______________________________________________________________________________
AliMUONSt1GeometryBuilder::~AliMUONSt1GeometryBuilder() {
//
}

//______________________________________________________________________________
AliMUONSt1GeometryBuilder& 
AliMUONSt1GeometryBuilder::operator = (const AliMUONSt1GeometryBuilder& rhs) 
{
// Protected assignement operator

  // check assignement to self
  if (this == &rhs) return *this;

  Fatal("operator=", 
        "Assignment operator is not implemented.");
    
  return *this;  
}

//
// public methods
//

//______________________________________________________________________________
void AliMUONSt1GeometryBuilder::CreateGeometry()
{
// From AliMUONv1::CreateGeometry()
// ---

//********************************************************************
//                            Station 1                             **
//********************************************************************
//  CONCENTRIC
     // indices 1 and 2 for first and second chambers in the station
     // iChamber (first chamber) kept for other quanties than Z,
     // assumed to be the same in both chambers

     // Get tracking medias Ids     
     Int_t *idtmed = fMUON->GetIdtmed()->GetArray()-1099;
     Int_t idAir= idtmed[1100]; // medium 1
     Int_t idAlu1=idtmed[1103]; // medium 4
     Int_t idAlu2=idtmed[1104]; // medium 5
     Int_t idGas=idtmed[1108];  // medium 9 = Ar-CO2 gas (80%+20%)
     Bool_t frameCrosses=kTRUE;     

     // Rotation matrices in the x-y plane  
     // phi=   0 deg
     Int_t irot1;
     fMUON->AliMatrix(irot1,  90.,   0., 90.,  90., 0., 0.);
     // phi=  90 deg
     Int_t irot2;
     fMUON->AliMatrix(irot2,  90.,  90., 90., 180., 0., 0.);

     AliMUONChamber* iChamber1 = GetChamber(0);
     AliMUONChamber* iChamber2 = GetChamber(1);
     AliMUONChamber* iChamber = iChamber1;

     // DGas decreased from standard one (0.5)
     iChamber->SetDGas(0.4); 
     iChamber2->SetDGas(0.4);

     // DAlu increased from standard one (3% of X0),
     // because more electronics with smaller pads
     iChamber->SetDAlu(3.5 * 8.9 / 100.); 
     iChamber2->SetDAlu(3.5 * 8.9 / 100.);

     // Half of the total thickness of frame crosses (including DAlu)
     // for each chamber in stations 1 and 2:
     // 3% of X0 of composite material,
     // but taken as Aluminium here, with same thickness in number of X0
     Float_t dframez = 3. * 8.9 / 100;
     Float_t zfpos=-(iChamber->DGas()+dframez+iChamber->DAlu())/2;
             // The same parameters are defined in builder for station 2 

     // Mother volume
     // Outer excess and inner recess for mother volume radius
     // with respect to ROuter and RInner
     Float_t dframep=.001; // Value for station 3 should be 6 ...
     // Width (RdPhi) of the frame crosses for stations 1 and 2 (cm)
     // Float_t dframep1=.001;
     Float_t dframep1 = 11.0;
     Float_t phi=2*TMath::Pi()/12/2;
             // The same parameters are defined in builder for station 2 

     Float_t tpar[3];
     Float_t dstation = (-iChamber2->Z()) - (-iChamber1->Z());
     tpar[0] = iChamber->RInner()-dframep; 
     tpar[1] = (iChamber->ROuter()+dframep)/TMath::Cos(phi);
     tpar[2] = dstation/5;

     gMC->Gsvolu("S01M", "TUBE", idAir, tpar, 3);
     gMC->Gsvolu("S02M", "TUBE", idAir, tpar, 3);

     // CHANGED
     //gMC->Gspos("S01M", 1, "ALIC", 0., 0., zpos1 , 0, "ONLY");
     //gMC->Gspos("S02M", 1, "ALIC", 0., 0., zpos2 , 0, "ONLY");     
     GetChamber(0)->GetGeometry()->AddEnvelope("S01M", false);
     GetChamber(1)->GetGeometry()->AddEnvelope("S02M", false);

// // Aluminium frames
// // Outer frames
//      pgpar[0] = 360/12/2;
//      pgpar[1] = 360.;
//      pgpar[2] = 12.;
//      pgpar[3] =   2;
//      pgpar[4] = -dframez/2;
//      pgpar[5] = iChamber->ROuter();
//      pgpar[6] = pgpar[5]+dframep1;
//      pgpar[7] = +dframez/2;
//      pgpar[8] = pgpar[5];
//      pgpar[9] = pgpar[6];
//      gMC->Gsvolu("S01O", "PGON", idAlu1, pgpar, 10);
//      gMC->Gsvolu("S02O", "PGON", idAlu1, pgpar, 10);
//      gMC->Gspos("S01O",1,"S01M", 0.,0.,-zfpos,  0,"ONLY");
//      gMC->Gspos("S01O",2,"S01M", 0.,0.,+zfpos,  0,"ONLY");
//      gMC->Gspos("S02O",1,"S02M", 0.,0.,-zfpos,  0,"ONLY");
//      gMC->Gspos("S02O",2,"S02M", 0.,0.,+zfpos,  0,"ONLY");
// //
// // Inner frame
//      tpar[0]= iChamber->RInner()-dframep1;
//      tpar[1]= iChamber->RInner();
//      tpar[2]= dframez/2;
//      gMC->Gsvolu("S01I", "TUBE", idAlu1, tpar, 3);
//      gMC->Gsvolu("S02I", "TUBE", idAlu1, tpar, 3);

//      gMC->Gspos("S01I",1,"S01M", 0.,0.,-zfpos,  0,"ONLY");
//      gMC->Gspos("S01I",2,"S01M", 0.,0.,+zfpos,  0,"ONLY");
//      gMC->Gspos("S02I",1,"S02M", 0.,0.,-zfpos,  0,"ONLY");
//      gMC->Gspos("S02I",2,"S02M", 0.,0.,+zfpos,  0,"ONLY");
//
// Frame Crosses
     if (frameCrosses) {
         // outside gas
         // security for inside mother volume
         Float_t bpar[3];
	 bpar[0] = (iChamber->ROuter() - iChamber->RInner())
	   * TMath::Cos(TMath::ASin(dframep1 /
				   (iChamber->ROuter() - iChamber->RInner())))
	   / 2.0;
	 bpar[1] = dframep1/2;
	 // total thickness will be (4 * bpar[2]) for each chamber,
	 // which has to be equal to (2 * dframez) - DAlu
	 bpar[2] = (2.0 * dframez - iChamber->DAlu()) / 4.0;
	 gMC->Gsvolu("S01B", "BOX", idAlu1, bpar, 3);
	 gMC->Gsvolu("S02B", "BOX", idAlu1, bpar, 3);
	 
	 gMC->Gspos("S01B",1,"S01M", +iChamber->RInner()+bpar[0] , 0,-zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S01B",2,"S01M", -iChamber->RInner()-bpar[0] , 0,-zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S01B",3,"S01M", 0, +iChamber->RInner()+bpar[0] ,-zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S01B",4,"S01M", 0, -iChamber->RInner()-bpar[0] ,-zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S01B",5,"S01M", +iChamber->RInner()+bpar[0] , 0,+zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S01B",6,"S01M", -iChamber->RInner()-bpar[0] , 0,+zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S01B",7,"S01M", 0, +iChamber->RInner()+bpar[0] ,+zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S01B",8,"S01M", 0, -iChamber->RInner()-bpar[0] ,+zfpos, 
		    irot2,"ONLY");
	 
	 gMC->Gspos("S02B",1,"S02M", +iChamber->RInner()+bpar[0] , 0,-zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S02B",2,"S02M", -iChamber->RInner()-bpar[0] , 0,-zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S02B",3,"S02M", 0, +iChamber->RInner()+bpar[0] ,-zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S02B",4,"S02M", 0, -iChamber->RInner()-bpar[0] ,-zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S02B",5,"S02M", +iChamber->RInner()+bpar[0] , 0,+zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S02B",6,"S02M", -iChamber->RInner()-bpar[0] , 0,+zfpos, 
		    irot1,"ONLY");
	 gMC->Gspos("S02B",7,"S02M", 0, +iChamber->RInner()+bpar[0] ,+zfpos, 
		    irot2,"ONLY");
	 gMC->Gspos("S02B",8,"S02M", 0, -iChamber->RInner()-bpar[0] ,+zfpos, 
		    irot2,"ONLY");
     }
//
//   Chamber Material represented by Alu sheet
     tpar[0]= iChamber->RInner();
     tpar[1]= iChamber->ROuter();
     tpar[2] = (iChamber->DGas()+iChamber->DAlu())/2;
     gMC->Gsvolu("S01A", "TUBE",  idAlu2, tpar, 3);
     gMC->Gsvolu("S02A", "TUBE",idAlu2, tpar, 3);
     gMC->Gspos("S01A", 1, "S01M", 0., 0., 0.,  0, "ONLY");
     gMC->Gspos("S02A", 1, "S02M", 0., 0., 0.,  0, "ONLY");
//     
//   Sensitive volumes
     // tpar[2] = iChamber->DGas();
     tpar[2] = iChamber->DGas()/2;
     gMC->Gsvolu("S01G", "TUBE", idGas, tpar, 3);
     gMC->Gsvolu("S02G", "TUBE", idGas, tpar, 3);
     gMC->Gspos("S01G", 1, "S01A", 0., 0., 0.,  0, "ONLY");
     gMC->Gspos("S02G", 1, "S02A", 0., 0., 0.,  0, "ONLY");
//
// Frame Crosses to be placed inside gas
     // NONE: chambers are sensitive everywhere
//      if (frameCrosses) {

// 	 dr = (iChamber->ROuter() - iChamber->RInner());
// 	 bpar[0] = TMath::Sqrt(dr*dr-dframep1*dframep1/4)/2;
// 	 bpar[1] = dframep1/2;
// 	 bpar[2] = iChamber->DGas()/2;
// 	 gMC->Gsvolu("S01F", "BOX", idAlu1, bpar, 3);
// 	 gMC->Gsvolu("S02F", "BOX", idAlu1, bpar, 3);
	 
// 	 gMC->Gspos("S01F",1,"S01G", +iChamber->RInner()+bpar[0] , 0, 0, 
// 		    irot1,"ONLY");
// 	 gMC->Gspos("S01F",2,"S01G", -iChamber->RInner()-bpar[0] , 0, 0, 
// 		    irot1,"ONLY");
// 	 gMC->Gspos("S01F",3,"S01G", 0, +iChamber->RInner()+bpar[0] , 0, 
// 		    irot2,"ONLY");
// 	 gMC->Gspos("S01F",4,"S01G", 0, -iChamber->RInner()-bpar[0] , 0, 
// 		    irot2,"ONLY");
	 
// 	 gMC->Gspos("S02F",1,"S02G", +iChamber->RInner()+bpar[0] , 0, 0, 
// 		    irot1,"ONLY");
// 	 gMC->Gspos("S02F",2,"S02G", -iChamber->RInner()-bpar[0] , 0, 0, 
// 		    irot1,"ONLY");
// 	 gMC->Gspos("S02F",3,"S02G", 0, +iChamber->RInner()+bpar[0] , 0, 
// 		    irot2,"ONLY");
// 	 gMC->Gspos("S02F",4,"S02G", 0, -iChamber->RInner()-bpar[0] , 0, 
// 		    irot2,"ONLY");
//      }
}

//______________________________________________________________________________
void AliMUONSt1GeometryBuilder::SetTransformations()
{
// Defines the transformations for the station2 chambers.
// ---

  AliMUONChamber* iChamber1 = GetChamber(0);
  Double_t zpos1= - iChamber1->Z(); 
  iChamber1->GetGeometry()
    ->SetTranslation(TGeoTranslation(0., 0., zpos1));

  AliMUONChamber* iChamber2 = GetChamber(1);
  Double_t zpos2 = - iChamber2->Z(); 
  iChamber2->GetGeometry()
    ->SetTranslation(TGeoTranslation(0., 0., zpos2));
}

//______________________________________________________________________________
void AliMUONSt1GeometryBuilder::SetSensitiveVolumes()
{
// Defines the sensitive volumes for station1 chambers.
// ---

  GetChamber(0)->GetGeometry()->SetSensitiveVolume("S01G");
  GetChamber(1)->GetGeometry()->SetSensitiveVolume("S02G");
}
