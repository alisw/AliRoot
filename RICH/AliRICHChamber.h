#ifndef AliRICHChamber_h
#define AliRICHChamber_h

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id$ */

#include <TRotMatrix.h>
#include <TVector3.h>
#include <TMath.h>
#include <TRotation.h>

#include "AliRICHTresholdMap.h"
#include "AliSegmentation.h"
#include "AliRICHGeometry.h"
#include "AliRICHResponse.h"

class AliRICHClusterFinder;

typedef enum {kMip, kCerenkov} ResponseType;
class AliRICHParam;

class AliRICHChamber : public TNamed
{
public:
    
   Int_t                fIndexMap[50];   //indeces of tresholds
   AliRICHTresholdMap*  fTresh;          //map of tresholds

public:
           AliRICHChamber();                                        //default ctor
           AliRICHChamber(Int_t iModuleN,AliRICHParam *pParam);
           AliRICHChamber(const AliRICHChamber &chamber ) : TNamed(chamber) {}//copy ctor 
  virtual ~AliRICHChamber()                               {;}//dtor
  AliRICHChamber& operator=(const AliRICHChamber&){return *this;}
  void LocaltoGlobal(Float_t pos[3],Float_t Localpos[3]);//Transformation from local to global coordinates, chamber-dependant
  void GlobaltoLocal(Float_t pos[3],Float_t localpos[3]);//Transformation from Global to local coordinates, chamber-dependant 
  void GenerateTresholds();                              //Generate pad dependent tresholds
  void DisIntegration(Float_t eloss, Float_t xhit, Float_t yhit, Int_t&x, Float_t newclust[6][500], ResponseType res);// Cluster formation method
  void    Init(Int_t id)           {fSegmentation->Init(id);} // Recalculates all the values after some of them have been changed
  void    SetGid(Int_t id)         {fGid=id;}           // Set and get GEANT id  
  Int_t   GetGid()            const{return fGid;}       // Get GEANT id  
  void SetRInner(Float_t rmin)     {frMin=rmin;}        // Set inner radius of sensitive volume   
  Float_t RInner()            const{return frMin;}      // Return inner radius of sensitive volume 
  void SetROuter(Float_t rmax)     {frMax=rmax;}        // Set outer radius of sensitive volum  
  Float_t ROuter()            const{return frMax;}      // Return outer radius of sensitive volum  
  void    SetZPOS(Float_t p1)      {fzPos=p1;}
  Float_t ZPosition()         const{return fzPos;}
  void              SetGeometryModel(AliRICHGeometry* pRICHGeometry)            {fGeometry=pRICHGeometry;}        
  AliRICHGeometry*  GetGeometryModel()                                     const{return fGeometry;}
  void              SetResponseModel(AliRICHResponse* pRICHResponse)            {fResponse=pRICHResponse;}
  AliRICHResponse*  GetResponseModel()                                     const{return fResponse;}
  void              SetSegmentationModel(AliSegmentation* pRICHSegmentation)    {fSegmentation=pRICHSegmentation;}
  AliSegmentation*  GetSegmentationModel()                                 const{return fSegmentation;}
  void                  SetReconstructionModel(AliRICHClusterFinder *pRICHReconstruction)    {fReconstruction=pRICHReconstruction;}
  AliRICHClusterFinder* &GetReconstructionModel()                                            {return fReconstruction;}
  void   SigGenInit(Float_t x, Float_t y, Float_t z)   {fSegmentation->SigGenInit(x, y, z) ;}
  Int_t  SigGenCond(Float_t x, Float_t y, Float_t z)	{return fSegmentation->SigGenCond(x, y, z);}
  Int_t  Sector(Float_t x, Float_t y)                  {return fSegmentation->Sector((Int_t)x, (Int_t)y);} // Returns number of sector containing (x,y) position    
  void   SetPadSize(Float_t p1, Float_t p2)            {fSegmentation->SetPadSize(p1,p2);}
  Float_t IntPH(Float_t eloss, Float_t yhit)                        {return fResponse->IntPH(eloss,yhit);}
  Float_t IntPH(Float_t yhit)                                       {return fResponse->IntPH(yhit);}
  void  SetSigmaIntegration(Float_t p)                             {fResponse->SetSigmaIntegration(p);}
  void  SetChargeSlope(Float_t p)                                  {fResponse->SetChargeSlope(p);}
  void  SetChargeSpread(Float_t p1, Float_t p2)                    {fResponse->SetChargeSpread(p1,p2);}
  void  SetMaxAdc(Float_t p)                                       {fResponse->SetMaxAdc(p);}
  void  SetSqrtKx3(Float_t p)                                      {fResponse->SetSqrtKx3(p);}
  void  SetKx2(Float_t p)                                          {fResponse->SetKx2(p);}
  void  SetKx4(Float_t p)                                          {fResponse->SetKx4(p);}
  void  SetSqrtKy3(Float_t p)                                      {fResponse->SetSqrtKy3(p);}
  void  SetKy2(Float_t p)                                          {fResponse->SetKy2(p);}
  void  SetKy4(Float_t p)                                          {fResponse->SetKy4(p);}    
  void  SetPitch(Float_t p)                                        {fResponse->SetPitch(p);}
  void  SetWireSag(Int_t p)                                        {fResponse->SetWireSag(p);}
  void  SetVoltage(Int_t p)                                        {fResponse->SetVoltage(p);}       
  void  SetGapThickness(Float_t thickness)                         {fGeometry->SetGapThickness(thickness);} 
  void  SetProximityGapThickness(Float_t thickness)                {fGeometry->SetProximityGapThickness(thickness);}
  void  SetQuartzLength(Float_t length)                            {fGeometry->SetQuartzLength(length);}
  void  SetQuartzWidth(Float_t width)                              {fGeometry->SetQuartzWidth(width);}
  void  SetQuartzThickness(Float_t thickness)                      {fGeometry->SetQuartzThickness(thickness);}
  void  SetOuterFreonLength(Float_t length)                        {fGeometry->SetOuterFreonLength(length);}
  void  SetOuterFreonWidth(Float_t width)                          {fGeometry->SetOuterFreonWidth(width);}
  void  SetInnerFreonLength(Float_t length)                        {fGeometry->SetInnerFreonLength(length);}
  void  SetInnerFreonWidth(Float_t width)                          {fGeometry->SetInnerFreonWidth(width);}
  void  SetFreonThickness(Float_t thickness)                       {fGeometry->SetFreonThickness(thickness);}
  TRotMatrix* RotMatrix()          const{return fpRotMatrix;}
  const char* RotMatrixName()      const{return "rot"+fName;}
  TRotation   Rot()                const{return fRot;}
  Double_t    Rho()                const{return fCenterV3.Mag();} 
  Double_t    Theta()              const{return fCenterV3.Theta();}    
  Double_t    Phi()                const{return fCenterV3.Phi();}    
  void        RotateX(Double_t a)       {fRot.RotateX(a);fCenterV3.RotateX(a);}
  void        RotateY(Double_t a)       {fRot.RotateY(a);fCenterV3.RotateY(a);}
  void        RotateZ(Double_t a)       {fRot.RotateZ(a);fCenterV3.RotateZ(a);}
  Double_t    X()                  const{return fCenterV3.X();}  
  Double_t    Y()                  const{return fCenterV3.Y();}   
  Double_t    Z()                  const{return fCenterV3.Z();}
  Double_t    GetX()               const{return fX;}
  Double_t    GetY()               const{return fY;}
  Double_t    GetZ()               const{return fZ;}    
  Double_t    GetOffset()          const{return TMath::Sqrt(fX*fX+fY*fY+fZ*fZ);}    
  inline void SetCenter(Double_t x,Double_t y,Double_t z);
  TRotMatrix *GetRotMatrix()       const{return fpRotMatrix;}
  void        SetChamberTransform(Float_t x,Float_t y,Float_t z,TRotMatrix *pRotMatrix) {fX=x; fY=y; fZ=z; fpRotMatrix=pRotMatrix;}
  
  virtual void Print(Option_t *sOption)const;      
protected:
  Float_t frMin;                                         // Minimum Chamber size
  Float_t frMax;                                         // Maximum Chamber size 
  Int_t   fGid;                                          // Id tag 
  Float_t fzPos;                                         // z-position of this chamber

  Float_t fX,fY,fZ;                                      // Position of the center of the chamber in MRS (cm)

  AliSegmentation               *fSegmentation;          // ??????????Segmentation model for each chamber
  AliRICHResponse               *fResponse;              // ??????????Response model for each chamber
  AliRICHGeometry               *fGeometry;              // ??????????Geometry model for each chamber
  AliRICHClusterFinder          *fReconstruction;        // ??????????Reconstruction model for each chamber
   
  TVector3      fCenterV3;        //chamber center position in MRS (cm)
  TRotation     fRot;             //chamber rotation in MRS
  TRotMatrix   *fpRotMatrix;      //rotation matrix of the chamber with respect to MRS 
  AliRICHParam *fpParam;          //main RICH parameters description  
  ClassDef(AliRICHChamber,1)      //single RICH chamber description
};//class AliRICHChamber

void AliRICHChamber::SetCenter(Double_t x,Double_t y,Double_t z)
{
  fCenterV3.SetXYZ(x,y,z);
  fX=x;fY=y;fZ=z;
}
  
#endif //AliRICHChamber_h
