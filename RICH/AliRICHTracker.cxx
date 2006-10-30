#include "AliRICHTracker.h" //class header
#include "AliRICH.h"             //GetTrackPoint(),PropagateBack()   
#include "AliRICHCluster.h"      //GetTrackPoint(),PropagateBack() 
#include "AliRICHParam.h"        //GetTrackPoint(),PropagateBack()
#include "AliRICHRecon.h"        //PropagateBack()
#include <AliESD.h>              //PropagateBack()  
#include <AliRun.h>              //GetTrackPoint(),PropagateBack()  
#include <AliTrackPointArray.h>  //GetTrackPoint()
#include <AliAlignObj.h>         //GetTrackPoint()
ClassImp(AliRICHTracker)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AliRICHTracker::AliRICHTracker():AliTracker()
{
// AliRICHTracker is created from AliReconstraction::Run() which invokes AliReconstraction::CreateTrackers() 
// which in turn invokes AliRICHReconstructor::CreateTracker(). 
// Note that this is done just once per session before AliReconstruction::Run() goes to events loop.
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
Bool_t AliRICHTracker::GetTrackPoint(Int_t idx, AliTrackPoint& point) const
{
// Interface callback methode invoked from AliReconstruction::WriteAlignmentData() to get position of MIP cluster in MARS associated to a current track.
// MIP cluster is reffered by index which is stored in AliESDtrack  ???????
// Arguments: idx- cluster index which is stored by RICH in AliESDtrack
//            point- reference to the object where to store the point     
//   Returns: status of operation  if FALSE then AliReconstruction::WriteAlignmentData() do not store this point to array of points for current track. 
  if(idx<0) return kFALSE; //no MIP cluster assigned to this track in PropagateBack()
  Int_t iCham=idx/1000000;
  Int_t iClu=idx%1000000;
  point.SetVolumeID(AliAlignObj::LayerToVolUID(AliAlignObj::kRICH,iCham-1));//layer and chamber number
  AliRICH *pRich=((AliRICH*)gAlice->GetDetector("RICH"));  
  AliRICHCluster *pClu=(AliRICHCluster*)pRich->CluLst(iCham)->UncheckedAt(iClu);//get pointer to cluster
  Double_t mars[3];
  AliRICHParam::Instance()->Lors2Mars(iCham,pClu->X(),pClu->Y(),mars);
  point.SetXYZ(mars[0],mars[1],mars[2]);
  return kTRUE;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliRICHTracker::LoadClusters(TTree *pCluTree)
{
// Interface callback methode invoked from AliReconstruction::RunTracking() to load RICH clusters before PropagateBack() gets control 
// Arguments: pCluTree- pointer to clusters tree got by AliRICHLoader::LoadRecPoints("read") then AliRICHLoader::TreeR()
//   Returns: error code (currently ignored in AliReconstruction::RunTraking())    
  AliDebug(1,"Start.");  pCluTree->GetEntry(0);  AliDebug(1,"Stop."); return 0;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Int_t AliRICHTracker::PropagateBack(AliESD *pESD)
{
// Interface callback methode invoked by AliRecontruction::RunTracking() during tracking after TOF. It's done just once per event
// Arguments: pESD - pointer to Event Summary Data class instance which contains a list of tracks
//   Returns: error code, 0 if no errors   
  Int_t iNtracks=pESD->GetNumberOfTracks();  AliDebug(1,Form("Start with %i tracks",iNtracks));
  AliRICH *pRich=((AliRICH*)gAlice->GetDetector("RICH"));  
  AliRICHRecon recon;                                                        //instance of reconstruction class, nothing important in ctor
   
  AliRICHParam *pParam=AliRICHParam::Instance();

  for(Int_t iTrk=0;iTrk<iNtracks;iTrk++){                                                        //ESD tracks loop
    AliESDtrack *pTrk = pESD->GetTrack(iTrk);                                                    //get next reconstructed track    
        
    Float_t xRa=0,yRa=0,xPc=0,yPc=0,th=0,ph=0;                                                   //track intersection point and angles, LORS  
    Int_t iCh=-1;                                                                                //intersected chamber 
    for(Int_t i=0;i<7;i++){                                                                      //chambers loop
      Double_t p1[3],n1[3]; pParam->Norm(i,n1); pParam->Lors2Mars(i,0,0,p1,AliRICHParam::kRad);  //point & norm  for RAD
      Double_t p2[3],n2[3]; pParam->Norm(i,n2); pParam->Lors2Mars(i,0,0,p2,AliRICHParam::kPc);   //point & norm  for PC
      
      if(pTrk->Intersect(p1,n1,-GetBz())==kFALSE) continue;                                      //try to intersect track with the middle of radiator
      if(pTrk->Intersect(p2,n2,-GetBz())==kFALSE) continue;                                      //try to intersect track with PC
      
      pParam->Mars2LorsVec(i,n1,th,ph);                                                          //track angles
      pParam->Mars2Lors   (i,p1,xRa,yRa);                                                        //TRKxRAD position
      pParam->Mars2Lors   (i,p2,xPc,yPc);                                                        //TRKxPC position
      
      if(AliRICHDigit::IsInside(xPc,yPc)==kFALSE) continue;                                      //not in active area  
      iCh=i;
      break;
    }//chambers loop      
    
    if(iCh==-1) continue;                                                                  //no intersection at all, go after next track
    
    TClonesArray *pCluLst=pRich->CluLst(iCh);                                              //get clusters list for intersected chamber
    
    Double_t    dMin=999;                                                                  //distance between track-PC intersection point and current cluster
    Int_t   iMip=-1;                                                                       //index of cluster nearest to intersection point
    for(Int_t iClu=0;iClu<pCluLst->GetEntries();iClu++){                                   //clusters loop for intersected chamber
      AliRICHCluster *pClu=(AliRICHCluster*)pCluLst->At(iClu);                             //get pointer to current cluster
      if(pClu->Q()<100) continue;                                                          //QDC is incompartible with mip, go after another one
      
      Float_t dX=xPc-pClu->X();                                                            //distance between current cluster and intersection point
      Float_t dY=yPc-pClu->Y();
      Float_t d =TMath::Sqrt(dX*dX+dY*dY);
      
      if( d < dMin) {iMip=iClu; dMin=d;}                                                   //current cluster is closer, overwrite data for min cluster
    }//clusters loop for intersected chamber    
    
                   pTrk->SetRICHtrk      (xPc,yPc,th,ph);                                  //store track info
    if(iMip==-1)  {pTrk->SetRICHsignal   (kMipQdcCut);  continue;}                         //no clusters with QDC more the threshold at all
    
    AliRICHCluster *pMipClu=(AliRICHCluster*)pCluLst->At(iMip);                            //take mip cluster 
    
                   pTrk->SetRICHmip      (pMipClu->X(),pMipClu->Y(),pMipClu->Q());          //store mip info 
    if(dMin>1)    {pTrk->SetRICHsignal   (kMipDistCut); continue;}                          //closest cluster with enough charge is still too far 
                   pTrk->SetRICHcluIdx   (iCh,iMip);                                        //set mip cluster index
  recon.SetTrack(th,ph,xRa,yRa); Int_t iNphot=0;                                            //initialize track parameters  
                   pTrk->SetRICHsignal   (recon.CkovAngle(pCluLst,iNphot));                 //search for Cerenkov angle for this track
                   pTrk->SetRICHchi2     (recon.CkovSigma2());                              //error squared 
                   pTrk->SetRICHmip      (pMipClu->X(),pMipClu->Y(),pMipClu->Q(),iMip);     //info on mip cluster + n. phot.
    Printf("Ch=%i MIP-TRK=%5.2f cm Th=%f+-%f", iCh, dMin,pTrk->GetRICHsignal(),pTrk->GetRICHchi2());
 }//ESD tracks loop
  AliDebug(1,"Stop pattern recognition");
  return 0; // error code: 0=no error;
}//PropagateBack()
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
