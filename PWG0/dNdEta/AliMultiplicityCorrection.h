/* $Id$ */

#ifndef ALIMULTIPLICITYCORRECTION_H
#define ALIMULTIPLICITYCORRECTION_H

#include "TNamed.h"

//
// class that contains the correction matrix and the functions for
// correction the multiplicity spectrum
//

class TH1;
class TH2;
class TH1F;
class TH2F;
class TH3F;
class TF1;
class TCollection;

#include <TMatrixD.h>
#include <TVectorD.h>

class AliMultiplicityCorrection : public TNamed {
  public:
    enum EventType { kTrVtx = 0, kMB, kINEL };
    enum RegularizationType { kNone = 0, kPol0, kPol1, kEntropy, kCurvature, kTest };

    AliMultiplicityCorrection();
    AliMultiplicityCorrection(const Char_t* name, const Char_t* title);
    virtual ~AliMultiplicityCorrection();

    virtual Long64_t Merge(TCollection* list);

    void FillMeasured(Float_t vtx, Int_t measured05, Int_t measured10, Int_t measured15, Int_t measured20);
    void FillGenerated(Float_t vtx, Bool_t triggered, Bool_t vertex, Int_t generated05, Int_t generated10, Int_t generated15, Int_t generated20, Int_t generatedAll);

    void FillCorrection(Float_t vtx, Int_t generated05, Int_t generated10, Int_t generated15, Int_t generated20, Int_t generatedAll, Int_t measured05, Int_t measured10, Int_t measured15, Int_t measured20);

    Bool_t LoadHistograms(const Char_t* dir);
    void SaveHistograms();
    void DrawHistograms();
    void DrawComparison(const char* name, Int_t inputRange, Bool_t fullPhaseSpace, Bool_t normalizeESD, TH1* mcHist, Bool_t simple = kFALSE);

    Int_t ApplyMinuitFit(Int_t inputRange, Bool_t fullPhaseSpace, EventType eventType, Bool_t check = kFALSE, TH1* inputDist = 0);
    void SetRegularizationParameters(RegularizationType type, Float_t weight);

    void ApplyNBDFit(Int_t inputRange, Bool_t fullPhaseSpace);

    void ApplyBayesianMethod(Int_t inputRange, Bool_t fullPhaseSpace, EventType eventType, Float_t regPar = 0.1, Int_t nIterations = 15, TH1* inputDist = 0);

    void ApplyGaussianMethod(Int_t inputRange, Bool_t fullPhaseSpace);

    void ApplyLaszloMethod(Int_t inputRange, Bool_t fullPhaseSpace, EventType eventType);

    TH2F* GetMultiplicityESD(Int_t i) { return fMultiplicityESD[i]; }
    TH2F* GetMultiplicityVtx(Int_t i) { return fMultiplicityVtx[i]; }
    TH2F* GetMultiplicityMB(Int_t i) { return fMultiplicityMB[i]; }
    TH2F* GetMultiplicityINEL(Int_t i) { return fMultiplicityINEL[i]; }
    TH2F* GetMultiplicityMC(Int_t i, EventType eventType);
    TH3F* GetCorrelation(Int_t i) { return fCorrelation[i]; }
    TH1F* GetMultiplicityESDCorrected(Int_t i) { return fMultiplicityESDCorrected[i]; }

    void SetMultiplicityESD(Int_t i, TH2F* hist)  { fMultiplicityESD[i] = hist; }
    void SetMultiplicityVtx(Int_t i, TH2F* hist)  { fMultiplicityVtx[i] = hist; }
    void SetMultiplicityMB(Int_t i, TH2F* hist)   { fMultiplicityMB[i] = hist; }
    void SetMultiplicityINEL(Int_t i, TH2F* hist) { fMultiplicityINEL[i] = hist; }
    void SetCorrelation(Int_t i, TH3F* hist) { fCorrelation[i] = hist; }
    void SetMultiplicityESDCorrected(Int_t i, TH1F* hist) { fMultiplicityESDCorrected[i] = hist; }

    void SetGenMeasFromFunc(TF1* inputMC, Int_t id);
    TH2F* CalculateMultiplicityESD(TH1* inputMC, Int_t correlationMap);

    static void NormalizeToBinWidth(TH1* hist);
    static void NormalizeToBinWidth(TH2* hist);

    void GetComparisonResults(Float_t* mc, Int_t* mcLimit, Float_t* residuals);

  protected:
    enum { kESDHists = 4, kMCHists = 5, kCorrHists = 8 };

    static const Int_t fgMaxParams;  // bins in unfolded histogram = number of fit params
    static const Int_t fgMaxInput;   // bins in measured histogram

    static Double_t RegularizationPol0(TVectorD& params);
    static Double_t RegularizationPol1(TVectorD& params);
    static Double_t RegularizationTotalCurvature(TVectorD& params);
    static Double_t RegularizationEntropy(TVectorD& params);
    static Double_t RegularizationTest(TVectorD& params);

    static void MinuitFitFunction(Int_t&, Double_t*, Double_t& chi2, Double_t *params, Int_t);
    static void MinuitNBD(Int_t& unused1, Double_t* unused2, Double_t& chi2, Double_t *params, Int_t unused3);

    void SetupCurrentHists(Int_t inputRange, Bool_t fullPhaseSpace, EventType eventType, Bool_t createBigBin);

    Float_t BayesCovarianceDerivate(Float_t matrixM[251][251], TH2* hResponse, TH1* fCurrentEfficiency, Int_t k, Int_t i, Int_t r, Int_t u);

    static TH1* fCurrentESD;         // static variable to be accessed by MINUIT
    static TH1* fCurrentCorrelation; // static variable to be accessed by MINUIT
    static TH1* fCurrentEfficiency;  // static variable to be accessed by MINUIT

    static TMatrixD* fCorrelationMatrix;            // contains fCurrentCorrelation in matrix form
    static TMatrixD* fCorrelationCovarianceMatrix;  // contains the errors of fCurrentESD
    static TVectorD* fCurrentESDVector;             // contains fCurrentESD
    static TVectorD* fEntropyAPriori;               // a-priori distribution for entropy regularization

    static TF1* fNBD;   // negative binomial distribution

    static RegularizationType fRegularizationType; // regularization that is used during Chi2 method
    static Float_t fRegularizationWeight;          // factor for regularization term

    TH2F* fMultiplicityESD[kESDHists]; // multiplicity histogram: vtx vs multiplicity; array: |eta| < 0.5, 1, 1.5, 2 (0..3)
    TH2F* fMultiplicityVtx[kMCHists];  // multiplicity histogram of events that have a reconstructed vertex : vtx vs multiplicity; array: |eta| < 0.5, 1, 1.5, 2, inf (0..4)
    TH2F* fMultiplicityMB[kMCHists];   // multiplicity histogram of triggered events                        : vtx vs multiplicity; array: |eta| < 0.5, 1, 1.5, 2, inf (0..4)
    TH2F* fMultiplicityINEL[kMCHists]; // multiplicity histogram of all (inelastic) events                  : vtx vs multiplicity; array: |eta| < 0.5, 1, 1.5, 2, inf (0..4)

    TH3F* fCorrelation[kCorrHists];              // vtx vs. (gene multiplicity (trig+vtx)) vs. (meas multiplicity); array: |eta| < 0.5, 1, 1.5, 2 (0..3 and 4..7), the first corrects to the eta range itself, the second to full phase space
    TH1F* fMultiplicityESDCorrected[kCorrHists]; // corrected histograms

    Float_t fLastChi2MC;        // last Chi2 between MC and unfolded ESD (calculated in DrawComparison)
    Int_t   fLastChi2MCLimit;   // bin where the last chi2 breached a certain threshold, used to evaluate the multiplicity reach (calc. in DrawComparison)
    Float_t fLastChi2Residuals; // last Chi2 of the ESD and the folded unfolded ESD (calculated in DrawComparison)

 private:
    AliMultiplicityCorrection(const AliMultiplicityCorrection&);
    AliMultiplicityCorrection& operator=(const AliMultiplicityCorrection&);

  ClassDef(AliMultiplicityCorrection, 1);
};

#endif

