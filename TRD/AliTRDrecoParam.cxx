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

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Parameter class for the TRD reconstruction                               //
//                                                                           //
//  Authors:                                                                 //
//    Alex Bercuci <A.Bercuci@gsi.de>                                        //
//    Markus Fasel <M.Fasel@gsi.de>                                          //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AliTRDrecoParam.h"
#include "AliTRDrawStreamBase.h"

ClassImp(AliTRDrecoParam)

//______________________________________________________________
AliTRDrecoParam::AliTRDrecoParam()
  :AliDetectorRecoParam()
  ,fkPIDMethod(kLQPID) // LQ PID
  ,fkMaxTheta(1.0)
  ,fkMaxPhi(2.0)
  ,fkRoad0y(6.0)
  ,fkRoad0z(8.5) 
  ,fkRoad1y(2.0)
  ,fkRoad1z(20.0)	
  ,fkRoad2y(3.0)
  ,fkRoad2z(20.0)
  ,fkPlaneQualityThreshold(5.0)// 4.2? under Investigation
  ,fkFindable(.333)
  ,fkChi2Z(30./*14.*//*12.5*/)
  ,fkChi2Y(.25)
  ,fkTrackLikelihood(-15.)
  ,fkStreamLevel(0)
  ,fMinMaxCutSigma(4.)
  ,fMinLeftRightCutSigma(8.)
  ,fClusMaxThresh(4.5)
  ,fClusSigThresh(3.5)
  ,fTCnexp(1)
  ,fADCbaseline(0)
{
  //
  // Default constructor
  //
  SetClusterSharing(kFALSE);
  SetSeeding(kFALSE);
  SetVertexConstrained();
  SetLUT();
  SetTailCancelation();

  fSysCovMatrix[0] = 1.; // y direction (1 cm)
  fSysCovMatrix[1] = 1.; // z direction (1 cm)
  fSysCovMatrix[2] = 0.; // snp
  fSysCovMatrix[3] = 0.; // tgl
  fSysCovMatrix[4] = 0.; // 1/pt
}

//______________________________________________________________
AliTRDrecoParam::AliTRDrecoParam(const AliTRDrecoParam &ref)
  :AliDetectorRecoParam(ref)
  ,fkPIDMethod(ref.fkPIDMethod)
  ,fkMaxTheta(ref.fkMaxTheta)
  ,fkMaxPhi(ref.fkMaxPhi)
  ,fkRoad0y(ref.fkRoad0y)
  ,fkRoad0z(ref.fkRoad0z) 
  ,fkRoad1y(ref.fkRoad1y)
  ,fkRoad1z(ref.fkRoad1z)	
  ,fkRoad2y(ref.fkRoad2y)
  ,fkRoad2z(ref.fkRoad2z)
  ,fkPlaneQualityThreshold(ref.fkPlaneQualityThreshold)
  ,fkFindable(ref.fkFindable)
  ,fkChi2Z(ref.fkChi2Z)
  ,fkChi2Y(ref.fkChi2Y)
  ,fkTrackLikelihood(ref.fkTrackLikelihood)
  ,fkStreamLevel(ref.fkStreamLevel)
  ,fMinMaxCutSigma(ref.fMinMaxCutSigma)
  ,fMinLeftRightCutSigma(ref.fMinLeftRightCutSigma)
  ,fClusMaxThresh(ref.fClusMaxThresh)
  ,fClusSigThresh(ref.fClusSigThresh)
  ,fTCnexp(ref.fTCnexp)
  ,fADCbaseline(ref.fADCbaseline)
{
  //
  // Copy constructor
  //
  SetClusterSharing(ref.IsClusterSharing());
  SetSeeding(ref.IsSeeding());
  SetVertexConstrained(ref.IsVertexConstrained());
  SetLUT(ref.IsLUT());
  SetTailCancelation(ref.IsTailCancelation());

  memcpy(fSysCovMatrix, ref.fSysCovMatrix, 5*sizeof(Double_t));
}

//______________________________________________________________
AliTRDrecoParam *AliTRDrecoParam::GetLowFluxParam()
{
  //
  // Parameters for the low flux environment
  //

  return new AliTRDrecoParam();

}

//______________________________________________________________
AliTRDrecoParam *AliTRDrecoParam::GetHighFluxParam()
{
  //
  // Parameters for the high flux environment
  //

  return new AliTRDrecoParam();

}

//______________________________________________________________
AliTRDrecoParam *AliTRDrecoParam::GetCosmicTestParam()
{
  //
  // Parameters for the cosmics data
  //

  AliTRDrawStreamBase::SetRawStreamVersion("TB");
  AliTRDrecoParam *par = new AliTRDrecoParam();
  par->SetADCbaseline(10);
  par->SetSeeding();
  par->SetVertexConstrained();
  return par;

}
