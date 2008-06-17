/* $Id$ */

#ifndef AlidNdEtaTask_H
#define AlidNdEtaTask_H

#include "AliAnalysisTask.h"
#include "AliPWG0Helper.h"
#include <TString.h>

class AliESDtrackCuts;
class dNdEtaAnalysis;
class TH1F;
class TH2F;
class TH3F;
class AliESDEvent;

class AlidNdEtaTask : public AliAnalysisTask {
  public:
    AlidNdEtaTask(const char* opt = "");
    virtual ~AlidNdEtaTask();

    virtual void   ConnectInputData(Option_t *);
    virtual void   CreateOutputObjects();
    virtual void   Exec(Option_t*);
    virtual void   Terminate(Option_t*);
    virtual Bool_t   Notify();

    void SetTrackCuts(AliESDtrackCuts* cuts) { fEsdTrackCuts = cuts; }
    void SetAnalysisMode(AliPWG0Helper::AnalysisMode mode) { fAnalysisMode = mode; }
    void SetReadMC(Bool_t flag = kTRUE) { fReadMC = flag; }
    void SetUseMCVertex(Bool_t flag = kTRUE) { fUseMCVertex = flag; }
    void SetUseMCKine(Bool_t flag = kTRUE) { fUseMCKine = flag; }
    void SetTrigger(AliPWG0Helper::Trigger trigger) { fTrigger = trigger; }

    void SetOption(const char* opt) { fOption = opt; }

 protected:
    AliESDEvent *fESD;    //! ESD object
    TList* fOutput;                  //! list send on output slot 0

    TString fOption;      // option string
    AliPWG0Helper::AnalysisMode fAnalysisMode; // detector that is used for analysis
    AliPWG0Helper::Trigger fTrigger;           // trigger that is used

    Bool_t  fReadMC;       // if true reads MC data (to build correlation maps)
    Bool_t  fUseMCVertex;  // the MC vtx is used instead of the ESD vertex (for syst. check)
    Bool_t  fUseMCKine;    // Process only primaries by using the MC informatin (for syst. check)

    AliESDtrackCuts* fEsdTrackCuts;         // Object containing the parameters of the esd track cuts

    // Gathered from ESD
    dNdEtaAnalysis* fdNdEtaAnalysisESD;     //! contains the dndeta from the ESD
    // control hists
    TH1F* fMult;                            //! raw multiplicity histogram (control histogram)
    TH1F* fMultVtx;                            //! raw multiplicity histogram of evts with vtx (control histogram)
    TH1F* fPartEta[3];            //! counted particles as function of eta (full vertex range, below 0 range, above 0 range)
    TH1F* fEvents;                //! events counted as function of vtx
    TH1F* fVertexResolution;      //! z resolution of the vertex

    // Gathered from MC (when fReadMC is set)
    dNdEtaAnalysis* fdNdEtaAnalysis;        //! contains the dndeta from the full sample
    dNdEtaAnalysis* fdNdEtaAnalysisNSD;        //! contains the dndeta for the NSD sample
    dNdEtaAnalysis* fdNdEtaAnalysisTr;      //! contains the dndeta from the triggered events
    dNdEtaAnalysis* fdNdEtaAnalysisTrVtx;   //! contains the dndeta from the triggered events with vertex
    dNdEtaAnalysis* fdNdEtaAnalysisTracks;  //! contains the dndeta from the triggered events with vertex counted from the mc particles associated to the tracks (comparing this to the raw values from the esd shows the effect of the detector resolution)

    // control histograms (MC)
    TH3F* fVertex;                //! vertex of counted particles
    TH1F* fPartPt;                //! counted particles as function of pt

    // control histograms (ESD)
    TH1F* fPhi;                   //! raw phi distribution
    TH2F* fEtaPhi;                //! raw eta - phi distribution
    TH1F* fDeltaPhi;              //! histogram of delta_phi values for tracklets (only for SPD analysis)

 private:
    AlidNdEtaTask(const AlidNdEtaTask&);
    AlidNdEtaTask& operator=(const AlidNdEtaTask&);

  ClassDef(AlidNdEtaTask, 1);
};

#endif
