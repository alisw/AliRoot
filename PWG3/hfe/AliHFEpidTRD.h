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
#ifndef ALIHFEPIDTRD_H
#define ALIHFEPIDTRD_H

 #ifndef ALIHFEPIDBASE_H
 #include "AliHFEpidBase.h"
 #endif

class AliESDtrack;
class AliVParticle;
class TList;
class TH2F;

class AliHFEpidTRD : public AliHFEpidBase{
  public:
    typedef enum{
      kLQ = 0,
      kNN = 1
    } PIDMethodTRD_t;
    enum{
      kThreshParams = 24
    };
    enum{
      kHistTRDSigV1 = 0,
      kHistTRDSigV2 = 1,
      kHistOverallSpecies = 2
    };
    AliHFEpidTRD(const Char_t *name);
    AliHFEpidTRD(const AliHFEpidTRD &ref);
    AliHFEpidTRD& operator=(const AliHFEpidTRD &ref);
    virtual ~AliHFEpidTRD();
    
    virtual Bool_t InitializePID();
    virtual Int_t IsSelected(AliVParticle *track);
    virtual Bool_t HasQAhistos() const { return kTRUE; };

    Double_t GetTRDSignalV1(AliESDtrack *track);
    Double_t GetTRDSignalV2(AliESDtrack *track);

    void SetPIDMethod(PIDMethodTRD_t method) { fPIDMethod = method; };
  protected:
    void Copy(TObject &ref) const;
    Double_t GetTRDthresholds(Double_t electronEff, Double_t p);
    Int_t GetMCpid(AliESDtrack *track);
    void InitParameters();
    virtual void AddQAhistograms(TList *l);
    void GetParameters(Double_t electronEff, Double_t *parameters);

    void FillHistogramsTRDSignalV1(Double_t signal, Double_t p, Int_t species);
    void FillHistogramsTRDSignalV2(Double_t signal, Double_t p, Int_t species);
  private:
    static const Double_t fgkVerySmall;                       // Check for 0
    PIDMethodTRD_t fPIDMethod;                              // PID Method: 2D Likelihood or Neural Network
    Double_t fThreshParams[kThreshParams];                  // Threshold parametrisation
    TList *fContainer;                                      // QA  Histogram Container
  ClassDef(AliHFEpidTRD, 1)     // TRD electron ID class
};

#endif
