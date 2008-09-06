#include "AliHLTPHOSOnlineDisplayFourierTab.h"
#include <iostream>
#include "TGFrame.h"
#include "AliHLTPHOSGetEventButton.h"
#include "AliHLTPHOSCommonDefs.h"
#include "AliHLTDataTypes.h"
#include "AliHLTHOMERData.h"
#include "AliHLTHOMERReader.h"
#include "AliHLTHOMERWriter.h"
#include "AliHLTPHOSRcuCellEnergyDataStruct.h"
//#include "AliHLTPHOSRcuCellEnergyDataStruct.h"
#include "AliHLTPHOSRcuCellEnergyDataStruct.h" 
#include "AliHLTPHOSOnlineDisplay.h"
#include "AliHLTPHOSSharedMemoryInterface.h"
#include "AliHLTPHOSFourier.h"

#include "AliHLTPHOSRcuFFTDataStruct.h"
#include "TStyle.h"

#define SAMPLING_FREQUENCY 10

using namespace std;

// MT Crap
#include <TMath.h>
//#include <TEveManager.h>
//#include <TEveBoxSet.h>

//TEveBoxSet* gAliEveBoxSet = 0;

AliHLTPHOSOnlineDisplayFourierTab::AliHLTPHOSOnlineDisplayFourierTab()
{
  cout << "ERROR: You cannot create a onlinedisplay Tab without arguments" << endl;
}


AliHLTPHOSOnlineDisplayFourierTab::AliHLTPHOSOnlineDisplayFourierTab(AliHLTPHOSOnlineDisplay *onlineDisplayPtr, TGTab  *tabPtr, 
								     AliHLTHOMERReader *homerSyncPtr, AliHLTHOMERReader *homerPtrs[MAX_HOSTS], int nHosts) :  AliHLTPHOSOnlineDisplayTab(), fEvtCnt(0)
{     

  // gStyle->SetOptLogy();
  // gStyle->SetOptStat(false);
  

  fShmPtr = new AliHLTPHOSSharedMemoryInterface();
  fOnlineDisplayPtr =  onlineDisplayPtr;
  fFourierPtr = new AliHLTPHOSFourier();

  for(int gain = 0; gain < N_GAINS; gain ++ )
    {
      fFourierHistoNew[gain] = 0;
      fFourierHistoOld[gain] = 0;
      fFourierHistoAccumulated[gain] = 0;
    }

  for(int i=0; i<MAX_HOSTS; i++)
    {
       fgHomerReadersPtr[i] = 0;
    }

  fgHomerReaderPtr = homerSyncPtr;
  
  for(int i=0; i<nHosts; i++)
    {
      fgHomerReadersPtr[i] = homerPtrs[i] ;

    }

  fgNHosts = nHosts;
  InitDisplay(tabPtr);
}


AliHLTPHOSOnlineDisplayFourierTab::~AliHLTPHOSOnlineDisplayFourierTab()
{

}



int
AliHLTPHOSOnlineDisplayFourierTab::GetNextEvent()
{
  //  ResetDisplay();
  DoGetNextEvent();
  //  FillHistograms();
  UpdateDisplay();
  fEvtCnt ++;
  // fgEvntCnt ++;

 
}



void 
AliHLTPHOSOnlineDisplayFourierTab::ReadBlockData(AliHLTHOMERReader *homeReaderPtr)
{  
  cout << "  PTH!!!!!!!!!!!!!!!!!!!!!!!!"<< endl;
  AliHLTPHOSValidCellDataStruct *currentChannel =0;
  cout << "AliHLTPHOSOnlineDisplayFourierTab::ReadBlockDat, Reading block data, therere are " <<  homeReaderPtr->GetBlockCnt() << " blocks " <<endl;
  unsigned long blk = homeReaderPtr->FindBlockNdx("RENELLEC","SOHP", 0xFFFFFFFF );

  while ( blk != ~(unsigned long)0 ) 
    {
      Int_t moduleID;
      Int_t rcuX = 0;
      Int_t rcuZ = 0;
      AliHLTPHOSRcuCellEnergyDataStruct* cellEnergiesPtr = (AliHLTPHOSRcuCellEnergyDataStruct*)homeReaderPtr->GetBlockData( blk ); 
      
      unsigned int *t = (unsigned int*)cellEnergiesPtr;
      
      moduleID = cellEnergiesPtr->fModuleID ;
      rcuX = cellEnergiesPtr->fRcuX;
      rcuZ = cellEnergiesPtr->fRcuZ;

      cout << "AliHLTPHOSOnlineDisplayFourierTab::ReadBlockData,  fModuleID =" <<moduleID << endl; 

      Int_t tmpZ;
      Int_t tmpX;
      Int_t tmpGain;
      int cnt = 0;
      Int_t* tmpPtr = 0;

      fShmPtr->SetMemory(cellEnergiesPtr);
      currentChannel = fShmPtr->NextChannel();

      while(currentChannel != 0)
	{
	  cnt ++;
	  tmpZ = currentChannel->fZ;
	  tmpX = currentChannel->fX;
	  tmpGain =  currentChannel->fGain;

	  if(cellEnergiesPtr->fHasRawData == true)
	    {
	      Int_t nSamples = 0;
	      Int_t* rawPtr = 0;
	      rawPtr = fShmPtr->GetRawData(nSamples);
	      fFourierPtr->ProcessFourier(rawPtr, nSamples, tmpZ, tmpX, tmpGain, fEvtCnt);
	    }
	  
	  currentChannel = fShmPtr->NextChannel();
	}
      blk = homeReaderPtr->FindBlockNdx("RENELLEC","SOHP", 0xFFFFFFFF, blk+1);
    }
  
  FillHistograms(fFourierPtr->GetPSD(), fFourierPtr->GetDataSize());
}



void 
AliHLTPHOSOnlineDisplayFourierTab::FillHistograms(const AliHLTPHOSRcuFFTDataStruct psd, const int size)
{
  //  gStyle->SetOptLogy();
  //  gStyle->SetOptStat(false);

  char tmpname[256];
  char tmptitle[256];

  int linewidth = 0;
  // double  linewidth = 1.2;

  for(int gain = 0; gain < N_GAINS; gain ++ )
    {
      if( fFourierHistoNew[gain] == 0)
	{
	  sprintf(tmptitle, "PSD averaged over all %s channels: Most recent event", Gain2Text(gain, ' ')); 
	  sprintf(tmpname,  "PSD_averaged_over_all_%s_channels__most_recent_event", Gain2Text(gain, '_'));  
	  fFourierHistoNew[gain] = new TH1D(tmpname, tmptitle,  (size/2) +1, 0, SAMPLING_FREQUENCY/2);
	  fFourierHistoNew[gain]->GetXaxis()->SetTitle("f/MHz");
	  fFourierHistoNew[gain]->GetYaxis()->SetTitle("Power (arbitrary units)"); 
	  fFourierHistoNew[gain]->SetLineWidth(linewidth);

	}
      if (fFourierHistoOld[gain] == 0)
	{
	  sprintf(tmptitle, "PSD averaged over all %s channels: Previous event", Gain2Text(gain, ' ')); 
	  sprintf(tmpname,  "PSD_averaged_over_all_%s_channels__previous_event", Gain2Text(gain, '_')); 
	  fFourierHistoOld[gain] = new TH1D(tmpname, tmptitle,  (size/2) +1, 0, SAMPLING_FREQUENCY/2);
	  fFourierHistoOld[gain]->GetXaxis()->SetTitle("f/MHz");
	  fFourierHistoOld[gain]->GetYaxis()->SetTitle("Power (arbitrary units)"); 
	  fFourierHistoOld[gain]->SetLineWidth(linewidth);

	}
      if( fFourierHistoAccumulated[gain] == 0 )
	{
	  sprintf(tmptitle, "PSD averaged over all %s channels: All events", Gain2Text(gain, ' ')); 
	  sprintf(tmpname,  "PSD_averaged_over_all_%s_channels__All_events", Gain2Text(gain, '_')); 
	  fFourierHistoAccumulated[gain] = new TH1D(tmpname, tmptitle,  (size/2) +1, 0, SAMPLING_FREQUENCY/2);
	  fFourierHistoAccumulated[gain]->GetXaxis()->SetTitle("f/MHz");
	  fFourierHistoAccumulated[gain]->GetYaxis()->SetTitle("Power (arbitrary units)"); 
	  fFourierHistoAccumulated[gain]->SetLineWidth(linewidth);

	}

      for(int i = 0; i <size/2; i++)
	{
	  fFourierHistoOld[gain]->SetBinContent(i+1,  fFourierHistoNew[gain]->GetBinContent(i+1));
	  fFourierHistoNew[gain]->SetBinContent(i+1,  psd.fGlobalLastPSD[gain][i] );
	  fFourierHistoAccumulated[gain]->SetBinContent(i+1,  psd.fGlobalAccumulatedPSD[gain][i] );
	}

    }
}




void
AliHLTPHOSOnlineDisplayFourierTab::InitDisplay(TGTab  *tabPtr)
{
  for(int gain=0; gain < N_GAINS; gain++)
    {
      char gainLabel[100];
      char label[256];
 
      //     Gain2Text
      fOnlineDisplayPtr->Gain2Text(gain,gainLabel);
      sprintf(label, "PHOS Fourier transform %s", gainLabel);
      fgLegoPlotPtr[gain] = new AliHLTPHOSOnlineDisplayTH2D(fOnlineDisplayPtr, label, label, 
      							    N_XCOLUMNS_MOD*N_MODULES , 0, N_XCOLUMNS_MOD*N_MODULES,  
      							    N_ZROWS_MOD,   0, N_ZROWS_MOD);   
	   //    fgLegoPlotPtr[gain]->SetGain(HIGH_GAIN);
      fgLegoPlotPtr[gain]->SetMaximum(1023);
      fgLegoPlotPtr[gain]->Reset();
      fgLegoPlotPtr[gain]->GetXaxis()->SetRange(X_RANGE_START, X_RANGE_END);
 
   }
  


  TGLayoutHints *fL1 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
					 kLHintsExpandY, 2, 2, 15, 1);

  TGCompositeFrame *tf = tabPtr->AddTab("Power spectrum");
  fSubTab1 = new TGTab(tf, 100, 100);
  TGCompositeFrame *tf2 = fSubTab1->AddTab("Most recent event");  
  fSubF1 = new TGCompositeFrame(tf2, 60, 20, kVerticalFrame);
  fEc1 = new TRootEmbeddedCanvas("ecf1", fSubF1, 100, 100);
  fSubF1->AddFrame(fEc1, fL1);
  fEc2 = new TRootEmbeddedCanvas("ecf2", fSubF1, 100, 100);
  fSubF1->AddFrame(fEc2, fL1);
  tf2->AddFrame(fSubF1, fL1);
  
  tf2 = fSubTab1->AddTab("Previous event"); 
  fSubF2 = new TGCompositeFrame(tf2, 60, 20, kVerticalFrame);
  tf2->AddFrame(fSubF2, fL1);
  fEc3 = new TRootEmbeddedCanvas("ecf3", fSubF2, 100, 100);
  fSubF2->AddFrame(fEc3, fL1);
  fEc4 = new TRootEmbeddedCanvas("ecf4", fSubF2, 100, 100);
  fSubF2->AddFrame(fEc4, fL1);
 
  
  tf2 = fSubTab1->AddTab("Accumulated"); 
  fSubF3 = new TGCompositeFrame(tf2, 60, 20, kVerticalFrame);
  tf2->AddFrame(fSubF3, fL1);
  fEc5 = new TRootEmbeddedCanvas("ecf5", fSubF3, 100, 100);
  fSubF3->AddFrame(fEc5, fL1);
  fEc6 = new TRootEmbeddedCanvas("ecf6", fSubF3, 100, 100);
  fSubF3->AddFrame(fEc6, fL1);
  fSubTab1->Resize();
  tf->AddFrame(fSubTab1, fL1);
  

  fgEventButtPtr = new  AliHLTPHOSGetEventButton(fSubF1, "get fourier", 'e');
}



void
AliHLTPHOSOnlineDisplayFourierTab::UpdateDisplay()
{
 
  fgCanvasPtr[HIGH_GAIN] =  fEc1->GetCanvas();
  fgCanvasPtr[HIGH_GAIN]->cd();
  gPad->SetLogy();
 //  fgLegoPlotPtr[HIGH_GAIN]->Draw("LGZ");
  fFourierHistoNew[HIGH_GAIN]->Draw();
  fgCanvasPtr[HIGH_GAIN]->Update();


  fgCanvasPtr[LOW_GAIN] = fEc2->GetCanvas();
  fgCanvasPtr[LOW_GAIN]->cd();
  gPad->SetLogy();
 //  fgLegoPlotPtr[LOW_GAIN]->Draw("HGZ");
  fFourierHistoNew[LOW_GAIN]->Draw();
  fgCanvasPtr[LOW_GAIN]->Update();


  fgCanvasPtr[HIGH_GAIN] =  fEc3->GetCanvas();
  fgCanvasPtr[HIGH_GAIN]->cd();
  gPad->SetLogy();
  // fgLegoPlotPtr[HIGH_GAIN]->Draw("Low gain");
  fFourierHistoOld[HIGH_GAIN]->Draw();
  fgCanvasPtr[HIGH_GAIN]->Update();


  fgCanvasPtr[LOW_GAIN] = fEc4->GetCanvas();
  fgCanvasPtr[LOW_GAIN]->cd();
  //fgLegoPlotPtr[LOW_GAIN]->Draw("High gain");
  gPad->SetLogy();
  fFourierHistoOld[LOW_GAIN]->Draw();
  fgCanvasPtr[LOW_GAIN]->Update();
  
  fgCanvasPtr[HIGH_GAIN] =  fEc5->GetCanvas();
  fgCanvasPtr[HIGH_GAIN]->cd();
  gPad->SetLogy();
  fFourierHistoAccumulated[HIGH_GAIN]->Draw();
  //  fgLegoPlotPtr[HIGH_GAIN]->Draw("CONTZ");
  fgCanvasPtr[HIGH_GAIN]->Update();


  fgCanvasPtr[LOW_GAIN] = fEc6->GetCanvas();
  fgCanvasPtr[LOW_GAIN]->cd();
  gPad->SetLogy();
  //  fgLegoPlotPtr[LOW_GAIN]->Draw("CONTZ");
  fFourierHistoAccumulated[LOW_GAIN]->Draw();
  fgCanvasPtr[LOW_GAIN]->Update();
  
}


const  char* 
AliHLTPHOSOnlineDisplayFourierTab::Gain2Text(const int gain, const char delimeter)
{
  if(gain ==  LOW_GAIN)
    {
      sprintf(fGainText, "low%cgain", delimeter);

    }
  else if(gain ==  HIGH_GAIN)
    {
      sprintf(fGainText, "high%cgain", delimeter);
    }
  else
    {
      sprintf(fGainText, "Error, invalid gain");
    }
  return fGainText;
}
