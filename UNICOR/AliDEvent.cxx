// Author: Dariusz Miskowiec <mailto:d.miskowiec@gsi.de> 2007

//=============================================================================
// parent class of all events; analyzers access data via this class
//=============================================================================

#include <TObject.h>
#include "AliDEvent.h"

ClassImp(AliDEvent)

//=============================================================================
void AliDEvent::RP(Double_t &qx, Double_t &qy, Int_t harmonic) const 
{
  // simplest flow vector

  qx=0; 
  qy=0;
  for (int i=0; i<NParticles(); i++) {
    if (!ParticleGood(i,0)) continue;
    double pt = ParticlePt(i);
    if (pt>2.0) pt = 2.0; // from 2 GeV flow saturates anyway
    qx += pt*cos(harmonic*ParticlePhi(i));
    qy += pt*sin(harmonic*ParticlePhi(i));
  }
}
//=============================================================================
Double_t AliDEvent::ParticleEta(Int_t i) const 
{
  // pseudorapidity

  double the = ParticleTheta(i); 
  if (the<0.0001) return 10; 
  else if (the>TMath::Pi()-0.0001) return -10;
  return -TMath::Log(TMath::Tan(the/2));
}
//=============================================================================
Double_t AliDEvent::ParticleY(Int_t i, Double_t mass) const 
{
  // rapidity

  double pp = ParticleP(i);
  double ee = TMath::Sqrt(TMath::Abs(mass*mass + pp*pp));
  double pz = ParticlePz(i);
  double yy = log((ee+pz)/(ee-pz))/2;
  return yy;
}
//=============================================================================
Double_t AliDEvent::PairDCA(Int_t i0, Int_t i1, 
			 Double_t *x0, Double_t *y0, 
			 Double_t *x1, Double_t *y1) const 
{
  // distance of closest approach for pair of tracks; under construction

  return 10;
}
//=============================================================================

