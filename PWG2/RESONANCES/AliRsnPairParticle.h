/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               *
 **************************************************************************/

//
// Class AliRsnPairParticle
//
// Implementation of a pair of tracks, for several purposes
// - computing the total 4-momentum & inv. mass for output histos filling
// - evaluating cut checks on the pair of particles
//
// author: Martin Vala (martin.vala@cern.ch)
// revised by: Alberto Pulvirenti (alberto.pulvirenti@ct.infn.it)
//

#ifndef ALIRSNPAIRPARTICLE_H
#define ALIRSNPAIRPARTICLE_H

#include <TMath.h>

#include "AliRsnDaughter.h"
#include "AliRsnMCInfo.h"

class AliRsnPairParticle : public TObject
{
public:

    AliRsnPairParticle();
    AliRsnPairParticle(const AliRsnPairParticle &obj);
    AliRsnPairParticle& operator=(const AliRsnPairParticle &obj);
    virtual ~AliRsnPairParticle();

    Double_t          GetInvMass (Double_t m1, Double_t m2);
    Double_t          GetInvMassMC (Double_t m1 = -1.0, Double_t m2 = -1.0);
    
    Double_t          GetP2() const {return (fPTot[0]*fPTot[0] + fPTot[1]*fPTot[1] + fPTot[2]*fPTot[2]);}
    Double_t          GetPt2() const {return (fPTot[0]*fPTot[0] + fPTot[1]*fPTot[1]);}
    Double_t          GetP() const {return TMath::Sqrt(GetP2());}
    Double_t          GetPx() const {return fPTot[0];}
    Double_t          GetPy() const {return fPTot[1];}
    Double_t          GetPz() const {return fPTot[2];}
    Double_t          GetPt() const {return TMath::Sqrt(GetPt2());}

    Double_t          GetP2MC() const {return (fPTotMC[0]*fPTotMC[0] + fPTotMC[1]*fPTotMC[1] + fPTotMC[2]*fPTotMC[2]);}
    Double_t          GetPt2MC() const {return (fPTotMC[0]*fPTotMC[0] + fPTotMC[1]*fPTotMC[1]);}
    Double_t          GetPMC() const {return TMath::Sqrt(GetP2MC());}
    Double_t          GetPxMC() const {return fPTotMC[0];}
    Double_t          GetPyMC() const {return fPTotMC[1];}
    Double_t          GetPzMC() const {return fPTotMC[2];}
    Double_t          GetPtMC() const {return TMath::Sqrt(GetPt2MC());}
    
    Double_t          GetAngle() const;

    AliRsnDaughter*   GetDaughter(const Int_t &index) const {return fDaughter[index];}

    Bool_t            IsLabelEqual() {return abs(fDaughter[0]->Label()) == abs(fDaughter[1]->Label());}
    Bool_t            IsIndexEqual() {return (fDaughter[0]->Index() == fDaughter[1]->Index());}
    Bool_t            IsTruePair(Int_t refPDG = 0);

    void              SetPair(AliRsnDaughter *daughter1, AliRsnDaughter *daughter2);
    void              PrintInfo (const Option_t *option = "");

private:

    Double_t         fPTot[3];          // total momentum computed with rec. values
    Double_t         fPTotMC[3];        // total momentum computed with MC values
    Double_t         fPTrack[2][3];     // rec. momentum of single tracks
    Double_t         fPTrackMC[2][3];   // MC momentum of single tracks

    Int_t            fMotherLabel[2];   // GEANT label of tracks
    Int_t            fMotherPDG[2];     // PDG code of mother of tracks

    AliRsnDaughter  *fDaughter[2];      // elements of the pair

    ClassDef (AliRsnPairParticle,1)
};

#endif
