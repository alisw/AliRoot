/**************************************************************************
 * Copyright(c) 2007, ALICE Experiment at CERN, All rights reserved.      *
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

// --- Root header files ---
#include "TMath.h"
// --- AliRoot header files ---
#include "AliEMCALSimParam.h"
#include "AliLog.h"

#include <iostream>

/// \cond CLASSIMP
ClassImp(AliEMCALSimParam);
/// \endcond

AliEMCALSimParam  * AliEMCALSimParam::fgSimParam = 0 ;

///
/// Constructor 
//-----------------------------------------------------------------------------
AliEMCALSimParam::AliEMCALSimParam() :
TNamed(),    
fDigitThreshold(0),
fMeanPhotonElectron(0),
fGainFluctuations(0),
fPinNoise(0),
fTimeNoise(0),
fTimeDelay(0),
fTimeDelayFromOCDB(0),
fTimeResolutionPar0(0),
fTimeResolutionPar1(0),
fNADCEC(0),//Digitizer
fA(0.),
fB(0.),
fECPrimThreshold(0.), //SDigitizer
fL1ADCNoise(0.0)//TriggerElectronics
{
  // Parameters in Digitizer
  fMeanPhotonElectron = 4400;    // electrons per GeV 
  fGainFluctuations   = 15.;     // obtained empiricaly to match beam test, from Paraskevi Ganoti   
  fPinNoise           = 0.012;   // APD noise in GeV from analysis test beam data 
  fDigitThreshold     = 3;       // 3 ADC counts not anymore cut in energy: //fPinNoise * 3; // 3 * sigma
  fTimeNoise          = 1.28e-5; // time noise in s
  fTimeResolutionPar0 = 0.26666; // From F. Blanco: 0.51639^2
  fTimeResolutionPar1 = 1.45861; // From F. Blanco: 1.20773^2
  fTimeDelay          = 600e-9 ; // 600 ns
  fTimeDelayFromOCDB  = kFALSE ; 
  
  fNADCEC             = (Int_t) TMath::Power(2,16) ; // number of channels in Tower ADC - 65536
  
  //SDigitizer
  fA                  = 0;
  fB                  = 1.e+6; // Dynamic range now 2 TeV
  fECPrimThreshold    = 0.05;  // GeV	// threshold for deposit energy of hit

}

///
/// Copy constructor.
//-----------------------------------------------------------------------------
/*AliEMCALSimParam::AliEMCALSimParam(const AliEMCALSimParam& ):
TNamed(),
fDigitThreshold(0),
fMeanPhotonElectron(0),
fGainFluctuations(0),
fPinNoise(0),
fTimeNoise(0),
fTimeDelay(0),
fTimeDelayFromOCDB(0),
fTimeResolutionPar0(0),
fTimeResolutionPar1(0),
fNADCEC(0),
fA(0.),
fB(0.),
fECPrimThreshold(0.),//SDigitizer
fL1ADCNoise(0)//TriggerElectronics
{
  AliError("Should not use copy constructor for singleton") ;
  
  fgSimParam = this ;
}*/

///
/// Get Instance
//-----------------------------------------------------------------------------                                                            
AliEMCALSimParam * AliEMCALSimParam::GetInstance()
{
  if(!fgSimParam)
    fgSimParam = new AliEMCALSimParam() ;
  
  return fgSimParam ;
}

///
/// Assignment operator.
//-----------------------------------------------------------------------------
/*AliEMCALSimParam& AliEMCALSimParam::operator = (const AliEMCALSimParam& simParam)
{
  if(this != &simParam) 
    AliError("Should not use operator= for singleton\n") ;
  
  return *this;
}*/

///
/// Print simulation parameters to stdout
//-----------------------------------------------------------------------------
void AliEMCALSimParam::Print(Option_t *) const
{ 
  AliInfoStream() << *this << std::endl;
  return;
}



///
/// Print simulation parameters to stream
//-----------------------------------------------------------------------------
void AliEMCALSimParam::PrintStream(std::ostream &stream) const {
  stream << "=== Sim Params ===" << std::endl;


  stream << "=== Parameters in Digitizer === " << std::endl;
  stream << "\t Electronics noise in EMC (fPinNoise)       = "<<fPinNoise<<", (fTimeNoise) = "<<fTimeNoise ;
  stream << "\t Threshold  in EMC  (fDigitThreshold)       = "<<fDigitThreshold<<std::endl ;
  stream << "\t Time Resolution (fTimeResolutionPar0)      = "<<fTimeResolutionPar0<<std::endl ;
  stream << "\t Time Resolution (fTimeResolutionPar1)      = "<<fTimeResolutionPar1<<std::endl;
  stream << "\t Time Delay (fTimeDelay)                    = "<<fTimeDelay<<std::endl ;
  stream << "\t Time Delay OCDB (fTimeDelayFromOCDB)       = "<<fTimeDelayFromOCDB<<std::endl ;
  stream << "\t Mean Photon-Electron (fMeanPhotonElectron) = "<<fMeanPhotonElectron<<", Gain Fluc. (fGainFluctuations) "<<fGainFluctuations<<std::endl;
  stream << "\t N channels in EC section ADC (fNADCEC)     = "<<fNADCEC<<std::endl ;

  stream << std::endl;

  stream << "=== Parameters in SDigitizer === "<<std::endl;
  stream << "\t sdigitization parameters       A = "<<fA<<std::endl;
  stream << "\t                                B = "<<fB<<std::endl;
  stream << "\t Threshold for EC Primary assignment  = "<<fECPrimThreshold<<std::endl;

  stream << "=== Parameters in TriggerElectronics === "<<std::endl;
  stream << "\t L1 FastOR timesum noise (fL1ADCNoise) = "<<fL1ADCNoise<<std::endl;

}

std::ostream &operator<<(std::ostream &stream, const AliEMCALSimParam &param) {
  param.PrintStream(stream);
  return stream;
}
