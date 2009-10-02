#ifndef ALICFCONTAINER_H
#define ALICFCONTAINER_H

/* $Id$ */

//--------------------------------------------------------------------//
//                                                                    //
// AliCFContainer Class                                               //
// Class to handle input data for correction Framework                // 
//                                                                    //
//--------------------------------------------------------------------//

#include "AliCFFrame.h"
#include "AliCFGridSparse.h"

class TH1D;
class TH2D;
class TH3D;
class TCollection;

class AliCFContainer : public AliCFFrame
{
 public:
  AliCFContainer();
  AliCFContainer(const Char_t* name,const Char_t* title);
  AliCFContainer(const Char_t* name, const Char_t* title,const Int_t nSelStep, const Int_t nVarIn, const Int_t* nBinIn);
  AliCFContainer(const AliCFContainer& c);
  AliCFContainer& operator=(const AliCFContainer& corr);
  virtual void Copy(TObject& c) const;

  virtual ~AliCFContainer();

  // AliCFFrame functions
  virtual Int_t      GetNVar()                                       const {return fGrid[0]->GetNVar();}
  virtual void       PrintBinLimits()                                const {fGrid[0]->PrintBinLimits();}
  virtual void       PrintNBins()                                    const {fGrid[0]->PrintNBins();}
  virtual void       SetBinLimits(Int_t ivar, const Double_t * array) ;
  virtual void       GetBinLimits(Int_t ivar, Double_t * array)      const {return fGrid[0]->GetBinLimits(ivar,array);}
  virtual Double_t * GetBinLimits(Int_t ivar)                        const {return fGrid[0]->GetBinLimits(ivar);}
  virtual Long_t     GetNBinsTotal()                                 const {return fGrid[0]->GetNBinsTotal()*fNStep;}
  virtual Int_t      GetNBins(Int_t ivar)                            const {return fGrid[0]->GetNBins(ivar);}
  virtual Int_t    * GetNBins()                                      const {return fGrid[0]->GetNBins();}
  virtual Float_t    GetBinCenter(Int_t ivar,Int_t ibin)             const {return fGrid[0]->GetBinCenter(ivar,ibin);}
  virtual Float_t    GetBinSize  (Int_t ivar,Int_t ibin)             const {return fGrid[0]->GetBinSize  (ivar,ibin);}

  //virtual void       GetBinCenters(const Int_t *ibin, const Double_t *binCenter) const {return fGrid[0]->GetBinCenters(ibin,binCenter);}
  //virtual void       GetBinSizes(const Int_t *ibin, const Double_t *binSizes)    const {return fGrid[0]->GetBinSizes(ibin,binSizes);}

  //probably not needed anymore
  //virtual Int_t      GetBinIndex(const Int_t *ibin)                  const {return fGrid[0]->GetBinIndex(ibin);}
  //virtual void       GetBinIndex(Int_t iel, const Int_t *ibin)       const {return fGrid[0]->GetBinIndex(iel,ibin);}
  //virtual Int_t      GetBinIndex(Int_t ivar, Int_t ind)              const {return fGrid[0]->GetBinIndex(ivar,ind);}

  virtual TAxis       * GetAxis(Int_t ivar, Int_t istep) const {return fGrid[istep]->GetAxis(ivar);}
  virtual void          SetVarTitle (Int_t ivar,  const Char_t* title) ;
  virtual void          SetStepTitle(Int_t istep, const Char_t* title) ;
  virtual const Char_t* GetVarTitle (Int_t ivar)  const {return GetAxis(ivar,0)->GetTitle();}
  virtual const Char_t* GetStepTitle(Int_t istep) const {return fGrid[istep]->GetTitle();}
  virtual Int_t         GetStep(const Char_t* title) const ; // returns the step     corresponding to the given title
  virtual Int_t         GetVar (const Char_t* title) const ; // returns the variable corresponding to the given title

  virtual Int_t GetNStep() const {return fNStep;};
  virtual void  SetNStep(Int_t nStep) {fNStep=nStep;}
  virtual void  Fill(const Double_t *var, Int_t istep, Double_t weight=1.) ;

  virtual Float_t  GetOverFlows (Int_t var,Int_t istep,Bool_t excl=kFALSE) const;
  virtual Float_t  GetUnderFlows(Int_t var,Int_t istep,Bool_t excl=kFALSE) const ;
  virtual Float_t  GetEntries  (Int_t istep) const ;
  virtual Long_t   GetEmptyBins(Int_t istep) const {return fGrid[istep]->GetEmptyBins();}
  //virtual Int_t    GetEmptyBins(Int_t istep, Double_t *varMin,Double_t *varMax) const ;
  virtual Double_t GetIntegral (Int_t istep) const ;
  //virtual Double_t GetIntegral (Int_t istep, Double_t *varMin,Double_t *varMax) const ;


  //basic operations
  virtual void     Add(const AliCFContainer* aContainerToAdd, Double_t c=1.);
  virtual Long64_t Merge(TCollection* list);

  virtual TH1D* ShowProjection( Int_t ivar, Int_t istep)                           const {return Project(ivar             ,istep);}
  virtual TH2D* ShowProjection( Int_t ivar1, Int_t ivar2, Int_t istep)             const {return Project(ivar1,ivar2      ,istep);}
  virtual TH3D* ShowProjection( Int_t ivar1, Int_t ivar2,Int_t ivar3, Int_t istep) const {return Project(ivar1,ivar2,ivar3,istep);}
  virtual TH1D* Project( Int_t ivar, Int_t istep) const;
  virtual TH2D* Project( Int_t ivar1, Int_t ivar2, Int_t istep) const;
  virtual TH3D* Project( Int_t ivar1, Int_t ivar2,Int_t ivar3, Int_t istep) const;

  virtual TH1D* ShowSlice(Int_t ivar, const Double_t *varMin, const Double_t *varMax, Int_t istep, Bool_t useBins=0) const ;
  virtual TH2D* ShowSlice(Int_t ivar1, Int_t ivar2, const Double_t *varMin, const Double_t *varMax, Int_t istep, Bool_t useBins=0) const ;
  virtual TH3D* ShowSlice(Int_t ivar1, Int_t ivar2, Int_t ivar3, const Double_t *varMin, const Double_t *varMax, Int_t istep, Bool_t useBins=0) const ;
  virtual AliCFContainer* MakeSlice(Int_t nVars, const Int_t* vars, const Double_t* varMin, const Double_t* varMax, Bool_t useBins=0) const ;
  virtual AliCFContainer* MakeSlice(Int_t nVars, const Int_t* vars, const Double_t* varMin, const Double_t* varMax, Int_t nStep, const Int_t* steps, Bool_t useBins=0) const ;

  virtual void  SetRangeUser(Int_t ivar, Double_t varMin, Double_t varMax, Int_t istep) ;
  virtual void  SetRangeUser(Double_t* varMin, Double_t* varMax, Int_t istep) ;
  virtual void  SetGrid(Int_t step, AliCFGridSparse* grid) {fGrid[step]=grid;}
  virtual AliCFGridSparse * GetGrid(Int_t istep) const {return fGrid[istep];};
  
 private:
  Int_t    fNStep; //number of selection steps
  AliCFGridSparse **fGrid;//[fNStep]
  
  ClassDef(AliCFContainer,5);
};

inline void AliCFContainer::SetBinLimits(Int_t ivar, const Double_t* array) {
  for (Int_t iStep=0; iStep<GetNStep(); iStep++) {
    fGrid[iStep]->SetBinLimits(ivar,array);
  }
}

inline void AliCFContainer::SetVarTitle(Int_t ivar, const Char_t* title) {
  for (Int_t iStep=0; iStep<fNStep; iStep++) {
    GetAxis(ivar,iStep)->SetTitle(title);
  }
}

inline void AliCFContainer::SetStepTitle(Int_t istep, const Char_t* title) {
  fGrid[istep]->SetTitle(title);
}

inline Int_t AliCFContainer::GetStep(const Char_t* title) const {
  TString str(title);
  for (Int_t iStep=0; iStep<fNStep; iStep++) {
    if (!str.CompareTo(GetStepTitle(iStep))) return iStep;
  }
  AliError("Step not found");
  return -1;
}

inline Int_t AliCFContainer::GetVar(const Char_t* title) const {
  return fGrid[0]->GetVar(title);
}
#endif

