#ifndef AliHBTTwoTrackEffFctn_H
#define AliHBTTwoTrackEffFctn_H
//classes for calculating two track efficiency of the tracking
//binning is done using value of simulated pair montum difference
//pair must be recontructed, that is why we need both pairs 
//(simulated and recontructed), thus functions are "two pair"
//Piotr.Skowronski@cern.ch
#include "AliHBTPair.h"
#include "AliHBTFunction.h"

class AliHBTTwoTrackEffFctn: public AliHBTOnePairFctn1D
 {
  public:
    AliHBTTwoTrackEffFctn();
    AliHBTTwoTrackEffFctn(Int_t nbins, Double_t maxval, Double_t minval);
    virtual ~AliHBTTwoTrackEffFctn(){}
    TH1* GetResult();
  protected:
    Double_t GetValue(AliHBTPair* pair){return pair->GetDeltaP();}
  private:
  public:
    ClassDef(AliHBTTwoTrackEffFctn,1)
 };

class AliHBTTwoTrackEffFctn3D: public AliHBTOnePairFctn3D
 {
  public:
    AliHBTTwoTrackEffFctn3D();
    virtual ~AliHBTTwoTrackEffFctn3D(){}

    void ProcessSameEventParticles(AliHBTPair* pair){}
    void ProcessDiffEventParticles(AliHBTPair* pair){}

  protected:
    void GetValues(AliHBTPair*,Double_t&, Double_t&,Double_t&);
  private:
  public:
    ClassDef(AliHBTTwoTrackEffFctn3D,1)
 };
#endif
