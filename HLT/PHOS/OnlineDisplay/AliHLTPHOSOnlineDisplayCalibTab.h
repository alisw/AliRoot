#ifndef ALIHLTPHOSONLINEDISPLAYCALIBTAB_H
#define ALIHLTPHOSONLINEDISPLAYCALIBTAB_H

#include "TGTab.h"
#include "AliHLTPHOSOnlineDisplayTab.h"
#include "TH2.h"
#include "AliHLTPHOSCommonDefs.h"
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include "AliHLTDataTypes.h"

#include "AliHLTPHOSConstants.h"
using namespace PhosHLTConst;


class AliHLTPHOSGetEventButton;
class AliHLTPHOSOnlineDisplayCalibTab : public AliHLTPHOSOnlineDisplayTab
{
 public:
  AliHLTPHOSOnlineDisplayCalibTab();
  AliHLTPHOSOnlineDisplayCalibTab(TGTab  *tabPtr, HOMERReader *homerSyncPtr, HOMERReader *homerPtrs[MAX_HOSTS], int nHosts);
  virtual ~AliHLTPHOSOnlineDisplayCalibTab();

  void InitDisplay(TGTab *tabPtr);
  void UpdateDisplay();
  int GetNextEvent();
  virtual void ReadBlockData(HOMERReader *homeReaderPtr);
  void ResetDisplay();

  TH2D *fgCalibHistPtr[N_GAINS];
  TH2I *fgHitsHistPtr[N_GAINS]; 
  TH2D *fgAveragePtr[N_GAINS];
  TGTab               *fTab;
  TRootEmbeddedCanvas *fEc7, *fEc8, *fEc9, *fEc10, *fEc11, *fEc12, *fEc13, *fEc14;
  TGTab               *fSubTab2;
  TGCompositeFrame    *fSubF4, *fSubF5, *fSubF6, *fSubF7,*fSubF8;
  TCanvas *fgCanvasHGPtr;
  TCanvas *fgCanvasLGPtr;
  TH2D *fgLegoPlotLGPtr;
  TH2D *fgLegoPlotHGPtr;
  AliHLTPHOSGetEventButton* fgEventButtPtr; 
};


#endif
