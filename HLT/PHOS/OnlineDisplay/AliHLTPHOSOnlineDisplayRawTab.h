#ifndef ALIHLTPHOSONLINEDISPLAYRAWTAB_H
#define ALIHLTPHOSONLINEDISPLAYRAWTAB_H

#include "TGTab.h"
#include "AliHLTPHOSOnlineDisplayTab.h"
#include "TH2.h"
#include "AliHLTPHOSCommonDefs.h"
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include "AliHLTDataTypes.h"

class AliHLTPHOSGetEventButton;
class AliHLTPHOSOnlineDisplayRawTab : public AliHLTPHOSOnlineDisplayTab
{
 public:
  AliHLTPHOSOnlineDisplayRawTab();
  AliHLTPHOSOnlineDisplayRawTab(TGTab  *tabPtr, HOMERReader *homerSyncPtr, HOMERReader *homerPtrs[MAX_HOSTS], int nHosts);
  virtual ~AliHLTPHOSOnlineDisplayRawTab();

  void InitDisplay(TGTab *tabPtr);
  void UpdateDisplay();
  int GetNextEvent();
  virtual void ReadBlockData(HOMERReader *homeReaderPtr);
  void ResetDisplay();

  TH2D *fgRawHistPtr[N_GAINS];
  TH2I *fgHitsHistPtr[N_GAINS]; 
  TH2D *fgAveragePtr[N_GAINS];
  TGTab               *fTab;
  TGTab               *fSubTab3;
  TGCompositeFrame *fFrame1;
  TCanvas *fgTestCanvasPtr;
  TCanvas *fgCanvasHGPtr;
  TCanvas *fgCanvasLGPtr;
  TH2D *fgLegoPlotLGPtr;
  TH2D *fgLegoPlotHGPtr;
  AliHLTPHOSGetEventButton* fgEventButtPtr; 
  TH1D                *fgChannelDataPlotPtr[N_ZROWS_RCU][N_XCOLUMNS_RCU];
};


#endif
