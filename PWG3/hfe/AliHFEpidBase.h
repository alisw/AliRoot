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
#ifndef __ALIHFEPIDBASE_H__
#define __ALIHFEPIDBASE_H__
 
 #ifndef ROOT_TNamed
 #include <TNamed.h>
 #endif

class TList;
class AliVParticle;
class AliMCEvent;

class AliHFEpidBase : public TNamed{
  enum{
    kQAon = BIT(14),
    kHasMCData = BIT(15)
  };
  public:
    AliHFEpidBase(const Char_t *name);
    AliHFEpidBase(const AliHFEpidBase &c);
    AliHFEpidBase &operator=(const AliHFEpidBase &c);
    virtual ~AliHFEpidBase() {};
    // Framework functions that have to be implemented by the detector PID classes
    virtual Bool_t InitializePID() = 0;
    virtual Int_t IsSelected(AliVParticle *track) = 0;
    virtual Bool_t HasQAhistos() const = 0;

    Int_t GetDebugLevel() const { return fDebugLevel; };
    Bool_t IsQAon() const { return TestBit(kQAon);};
    Bool_t HasMCData() const { return TestBit(kHasMCData); };

    void SetDebugLevel(Int_t debugLevel) { fDebugLevel = debugLevel; }; 
    inline void SetQAOn(TList *fQAlist);
    void SetHasMCData(Bool_t hasMCdata = kTRUE) { SetBit(kHasMCData,hasMCdata); };
    void SetMCEvent(AliMCEvent *mcEvent) { fMCEvent = mcEvent; };

  protected:
    void Copy(TObject &ref) const;
    virtual void AddQAhistograms(TList *){};
    Int_t GetPdgCode(AliVParticle *track);
  private:
    AliMCEvent *fMCEvent;       //! Monte Carlo Event
    Int_t fDebugLevel;          // Debug Level

    ClassDef(AliHFEpidBase, 1)    // Base class for detector Electron ID
};

//___________________________________________________________________
void AliHFEpidBase::SetQAOn(TList *qaList){
  //
  // Initialize QA for Detector PID class
  //
  if(HasQAhistos()){
    SetBit(kQAon, kTRUE);
    AddQAhistograms(qaList);
  }
}
#endif
