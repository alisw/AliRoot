#ifndef ALIMCPROCESS_H
#define ALIMCPROCESS_H

/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id $ */

// 
// List of MonteCarlo processes
//

static const Int_t kMaxMCProcess = 33;

enum AliMCProcess
{
  kPPrimary,  // Primary interaction

  kPMultipleScattering, // multiple scattering
  kPEnergyLoss, // continuous energy loss
  kPMagneticFieldL, // bending in mag. field
  kPDecay, // particle decay
  kPPair, // photon pair production or
             // muon direct pair production
  kPCompton, // Compton scattering
  kPPhotoelectric, // photoelectric effect
  kPBrem, // bremsstrahlung
  kPDeltaRay, // delta-ray production
  kPAnnihilation, // positron annihilation

  kPHadronic, // hadronic interaction
  kPEvaporation, // nuclear evaporation
  kPNuclearFission, // nuclear fission
  kPNuclearAbsorption, // nuclear absorption
  kPPbarAnnihilation, // antiproton annihilation
  kPNCapture, // neutron capture
  kPHElastic, // hadronic elastic incoherent 
             // scattering
  kPHInhelastic, // hadronic inelastic scattering

  kPMuonNuclear, // muon nuclear interaction

  kPTOFlimit, // exceeded time of flight cut
  kPPhotoFission, // nuclear photofission
	     	
  kPRayleigh, // Rayleigh scattering

  kPNull, // no mechanism is active, usually at the entrance 
             // of a new volume
  kPStop, // particle has fallen below energy threshold
             // and tracking stops
  
  kPLightAbsorption, // Cerenkov photon absorption
  kPLightScattering, // Cerenkov photon  
             // reflection/refraction
  kStepMax, // step limited by STEMAX

  kPCerenkov, // Cerenkov photon generation
  kPFeedBackPhoton, //Feed back photon in RICH -- ALICE specific
  kPLightReflection, // Cerenkov photon reflection
  kPLightRefraction, // Cerenkov photon refraction
  kPSynchrotron, // synchrotron radiation 
             // generation
  kPNoProcess // unknown process
};

static const char * const AliMCProcessName[kMaxMCProcess] = {
  "Primary particle emission",
  "Multiple scattering",
  "Energy loss",
  "Bending in magnetic field",
  "Decay",
  "Lepton pair production",
  "Compton scattering",
  "Photoelectric effect",
  "Bremstrahlung",
  "Delta ray",
  "Positron annihilation",
  "Hadronic interaction",
  "Nuclear evaporation",
  "Nuclear fission",
  "Nuclear absorbtion",
  "Antiproton annihilation",
  "Neutron capture",
  "Hadronic elastic",
  "Hadronic inelastic",
  "Muon nuclear interaction",
  "Time of flight limit",
  "Nuclear photofission",
  "Rayleigh effect",
  "No active process",
  "Energy threshold",
  "Light absorption",
  "Light scattering",
  "Maximum allowed step",
  "Cerenkov production",
  "Cerenkov feed back photon",
  "Cerenkov photon reflection",
  "Cerenkov photon refraction",
  "Synchrotron radiation"
  "Unknown process"
};

#endif //ALIMCPROCESS_H
