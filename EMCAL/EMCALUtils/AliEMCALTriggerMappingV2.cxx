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

/*

 


Author: 
H. YOKOYAMA Tsukuba University
R. GUERNANE LPSC Grenoble CNRS/IN2P3
*/

#include "AliEMCALTriggerMapping.h"
#include "AliEMCALTriggerMappingV2.h"
#include "AliEMCALGeometry.h"
#include "AliLog.h"

ClassImp(AliEMCALTriggerMappingV2)

//________________________________________________________________________________________________
AliEMCALTriggerMappingV2::AliEMCALTriggerMappingV2() : AliEMCALTriggerMapping()
{
  // Ctor
  SetUniqueID(2);
  
  for(Int_t iTRU=0; iTRU<46; iTRU++){
    fTRUFastOROffsetX[iTRU] = 0 ;
    fTRUFastOROffsetY[iTRU] = 0 ;
    fnFastORInTRUPhi[ iTRU] = 0 ;
    fnFastORInTRUEta[ iTRU] = 0 ;
  }
  for(Int_t iSM=0; iSM<20; iSM++){
    fSMFastOROffsetX[ iSM]  = 0 ;
    fSMFastOROffsetY[ iSM]  = 0 ;
    fnFastORInSMPhi[  iSM]  = 0 ;
    fnFastORInSMEta[  iSM]  = 0 ;
  }
  fnModuleInEMCALPhi  = 0 ;

  Init_TRU_offset()  ;
  Init_SM_offset()   ;
}
//________________________________________________________________________________________________
AliEMCALTriggerMappingV2::AliEMCALTriggerMappingV2(const Int_t ntru, const AliEMCALGeometry* geo) : AliEMCALTriggerMapping(ntru, geo)
{
  // Ctor
  SetUniqueID(2);
  
  for(Int_t iTRU=0; iTRU<46; iTRU++){
    fTRUFastOROffsetX[iTRU] = 0;
    fTRUFastOROffsetY[iTRU] = 0;
    fnFastORInTRUPhi[ iTRU] = 0;
    fnFastORInTRUEta[ iTRU] = 0;
  }
  for(Int_t iSM=0; iSM<20; iSM++){
    fSMFastOROffsetX[ iSM]  = 0;
    fSMFastOROffsetY[ iSM]  = 0;
    fnFastORInSMPhi[  iSM]  = 0;
    fnFastORInSMEta[  iSM]  = 0;
  }
  fnModuleInEMCALPhi  = 0;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetAbsFastORIndexFromTRU(const Int_t iTRU, const Int_t iADC, Int_t& id) const
{
  //Trigger mapping method, get  FastOr Index from TRU
  if (iTRU > fNTotalTRU-1     || iTRU < 0 || 
      iADC > fNModulesInTRU-1 || iADC < 0
  ){
    AliError(Form("Out of range! iTRU=%d, iADC=%d", iTRU, iADC));	
    return kFALSE;
  }
  Int_t x = fTRUFastOROffsetX[iTRU] +    (iADC % fnFastORInTRUEta[iTRU])  ;
  Int_t y = fTRUFastOROffsetY[iTRU] + int(iADC / fnFastORInTRUEta[iTRU])  ;
  id      = y*fNEta*2 + x         ;
  return kTRUE  ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetAbsFastORIndexFromPositionInTRU(const Int_t iTRU, const Int_t iEta, const Int_t iPhi, Int_t& id) const
{
  //Trigger mapping method, get Index if FastOr from Position in TRU
  if (iTRU > fNTotalTRU-1               || iTRU < 0 || 
      iEta > fnFastORInTRUEta[iTRU] - 1 || iEta < 0 ||
      iPhi > fnFastORInTRUPhi[iTRU] - 1 || iPhi < 0
  ){
    AliError(Form("Out of range! iTRU=%d, iEta=%d, iPhi=%d", iTRU, iEta, iPhi));	
    return kFALSE;
  }
  Int_t x = fTRUFastOROffsetX[iTRU] + iEta  ;
  Int_t y = fTRUFastOROffsetY[iTRU] + iPhi  ;
  id      = y*fNEta*2 + x         ;
  return kTRUE  ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetAbsFastORIndexFromPositionInSM(const Int_t  iSM, const Int_t iEta, const Int_t iPhi, Int_t& id) const
{
  //Trigger mapping method, from position in SM Index get FastOR index 
  if (iSM  > fNumberOfSuperModules-1  || iSM  < 0 || 
      iEta > fnFastORInSMEta[iSM] -1  || iEta < 0 ||
      iPhi > fnFastORInSMPhi[iSM] -1  || iPhi < 0
  ){
    AliError(Form("Out of range! iSM=%d, iEta=%d, iPhi=%d", iSM, iEta, iPhi));	
    return kFALSE;
  }
  Int_t x = fSMFastOROffsetX[iSM] + iEta  ;
  Int_t y = fSMFastOROffsetY[iSM] + iPhi  ;
  id      = y*fNEta*2 + x         ;
  return kTRUE  ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetAbsFastORIndexFromPositionInEMCAL(const Int_t iEta, const Int_t iPhi, Int_t& id) const
{
  //Trigger mapping method, from position in EMCAL Index get FastOR index 
  if (
      iEta > 2 * fNEta - 1          || iEta < 0 || 
      iPhi > fnModuleInEMCALPhi - 1 || iPhi < 0  
  ){
    AliError(Form("Out of range! eta: %2d phi: %2d", iEta, iPhi));
    return kFALSE;
  }
  id      = iPhi*fNEta*2 + iEta       ;
  return kTRUE  ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetTRUFromAbsFastORIndex(const Int_t id, Int_t& iTRU, Int_t& iADC) const
{
  //Trigger mapping method, get TRU number from FastOr Index
  Int_t iEta_TRU , iPhi_TRU , iSM , iEta_SM , iPhi_SM ;
  return GetInfoFromAbsFastORIndex(
    id   , 
    iTRU , iADC , iEta_TRU , iPhi_TRU , 
    iSM  ,        iEta_SM  , iPhi_SM  
    );
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetPositionInTRUFromAbsFastORIndex(const Int_t id, Int_t& iTRU, Int_t& iEta, Int_t& iPhi) const
{
  //Trigger mapping method, get position in TRU from FasOr Index
  Int_t iADC , iSM , iEta_SM , iPhi_SM ;
  return GetInfoFromAbsFastORIndex(
    id   , 
    iTRU , iADC , iEta     , iPhi     , 
    iSM  ,        iEta_SM  , iPhi_SM  
    );
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetPositionInSMFromAbsFastORIndex(const Int_t id, Int_t& iSM, Int_t& iEta, Int_t& iPhi) const
{
  //Trigger mapping method, get position in Super Module from FasOr Index
  Int_t iTRU , iADC , iEta_TRU , iPhi_TRU ;
  return GetInfoFromAbsFastORIndex(
    id   , 
    iTRU , iADC , iEta_TRU , iPhi_TRU , 
    iSM  ,        iEta     , iPhi  
    );
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetPositionInEMCALFromAbsFastORIndex(const Int_t id, Int_t& iEta, Int_t& iPhi) const
{
  //Trigger mapping method, get position in EMCAL from FastOR index
  Int_t nModule = fNEta * 2 * fnModuleInEMCALPhi;
  if (id > nModule-1 || id < 0){
    AliError("Id out of range!");
    return kFALSE;
  }
  iEta  = id % (2*fNEta) ;
  iPhi  = id / (2*fNEta) ;
  return kTRUE;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetFastORIndexFromCellIndex(const Int_t id, Int_t& idx) const
{
  // Trigger mapping method, from cell index get FastOR index 

  Int_t iSupMod, nModule, nIphi, nIeta, iphim, ietam;
  Bool_t isOK = fGeometry->GetCellIndex( id, iSupMod, nModule, nIphi, nIeta );
  fGeometry->GetModulePhiEtaIndexInSModule( iSupMod, nModule, iphim, ietam );
  
  if (isOK && GetAbsFastORIndexFromPositionInSM(iSupMod, ietam, iphim, idx)) return kTRUE;
  return kFALSE;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetCellIndexFromFastORIndex(const Int_t id, Int_t idx[4]) const
{
  //Trigger mapping method, from FASTOR index get cell index 
  Int_t iSM=-1, iEta=-1, iPhi=-1;
  if (GetPositionInSMFromAbsFastORIndex(id, iSM, iEta, iPhi))
  {
    Int_t ix = 2 * iEta;
    Int_t iy = 2 * iPhi;
    idx[0] = fGeometry->GetAbsCellIdFromCellIndexes(iSM, iy    , ix    );
    idx[1] = fGeometry->GetAbsCellIdFromCellIndexes(iSM, iy    , ix + 1);
    idx[2] = fGeometry->GetAbsCellIdFromCellIndexes(iSM, iy + 1, ix    );
    idx[3] = fGeometry->GetAbsCellIdFromCellIndexes(iSM, iy + 1, ix + 1);
    return kTRUE;
  }
  return kFALSE;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetTRUIndexFromSTUIndex(const Int_t id, Int_t& idx) const
{
  //Trigger mapping method, from STU index get TRU index 
  idx = GetTRUIndexFromSTUIndex(id);
  return kTRUE;
}

//________________________________________________________________________________________________
Int_t AliEMCALTriggerMappingV2::GetTRUIndexFromSTUIndex(const Int_t id) const
{
  //Trigger mapping method, from STU index get TRU index 
  if (id > fNTotalTRU-1 || id < 0){
    AliError(Form("TRU index out of range: %d",id));
  }
  return id ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetTRUIndexFromOnlineIndex(const Int_t id, Int_t& idx) const
{
  //Trigger mapping method, from STU index get TRU index 
  idx = GetTRUIndexFromOnlineIndex(id);
  return kTRUE;
}

//________________________________________________________________________________________________
Int_t AliEMCALTriggerMappingV2::GetTRUIndexFromOnlineIndex(const Int_t id) const
{
  //Trigger mapping method, from STU index get TRU index 
  if (id > fNTotalTRU-1 || id < 0){
    AliError(Form("TRU index out of range: %d",id));
  }
  return id;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetOnlineIndexFromTRUIndex(const Int_t id, Int_t& idx) const
{
  //Trigger mapping method, from STU index get TRU index 
  idx = GetOnlineIndexFromTRUIndex(id);
  return kTRUE;
}

//________________________________________________________________________________________________
Int_t AliEMCALTriggerMappingV2::GetOnlineIndexFromTRUIndex(const Int_t id) const
{
  //Trigger mapping method, from STU index get TRU index 
  if (id > fNTotalTRU-1 || id < 0){
    AliError(Form("TRU index out of range: %d",id));
  }
  return id;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetFastORIndexFromL0Index(const Int_t iTRU, const Int_t id, Int_t idx[], const Int_t size) const
{
  //Trigger mapping method, from L0 index get FastOR index 

  if (size <= 0 ||size > 4){
    AliError("Size not supported!");
    return kFALSE;
  }
		
  Int_t motif[4];
  motif[0] = 0;
  motif[1] = 1;
  motif[2] = fnFastORInTRUEta[iTRU]     ;
  motif[3] = fnFastORInTRUEta[iTRU] + 1 ;
 
  switch (size)
  {
    case 1: // Cosmic trigger
      if (!GetAbsFastORIndexFromTRU(iTRU, id, idx[1])) return kFALSE;
      break;
    case 4: // 4 x 4
      for (Int_t k = 0; k < 4; k++)
      {
        Int_t iADC = motif[k] + fnFastORInTRUEta[iTRU] * int(id/(fnFastORInTRUEta[iTRU]-1)) + (id%(fnFastORInTRUEta[iTRU]-1));
				
        if (!GetAbsFastORIndexFromTRU(iTRU, iADC, idx[k])) return kFALSE;
      }
      break;
    default:
      break;
  }
	
  return kTRUE;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::Init_TRU_offset(){
  fTRUFastOROffsetX[0]   = 0 ;
  fTRUFastOROffsetY[0]   = 0 ;
  Int_t iTRU  = 0 ;

  for(int iSM=0; iSM<fNumberOfSuperModules; iSM++){
    Int_t       SM_type   = GetSMType(iSM);
    Int_t       TRU_type  = 0             ;
    
    Int_t nTRU_inSM         = fNTRU                ;
    Int_t nTRU_inSM_phi     = fNTRUPhi             ;
    Int_t nTRU_inSM_eta     = fNTRUEta             ;
    Int_t nModule_inTRU_phi = fNModulesInTRUPhi    ;
    Int_t nModule_inTRU_eta = fNModulesInTRUEta    ;
    
    //kEMCAL_Standard -> default value
    if(     SM_type == kEMCAL_3rd      ){
      nTRU_inSM         = (Int_t)((Float_t)nTRU_inSM         / 3. );
      nTRU_inSM_eta     = (Int_t)((Float_t)nTRU_inSM_eta     / 3. );
      nModule_inTRU_phi = (Int_t)((Float_t)nModule_inTRU_phi / 3. );
      nModule_inTRU_eta = nModule_inTRU_eta                  * 3   ;
    }
    else if(SM_type == kDCAL_Standard  ){
      nTRU_inSM         = (Int_t)((Float_t)nTRU_inSM      * 2./3. );
      nTRU_inSM_eta     = (Int_t)((Float_t)nTRU_inSM_eta  * 2./3. );
    }
    else if(SM_type == kDCAL_Ext       ){
      nTRU_inSM         = (Int_t)((Float_t)nTRU_inSM         / 3. );
      nTRU_inSM_eta     = (Int_t)((Float_t)nTRU_inSM_eta     / 3. );
      nModule_inTRU_phi = (Int_t)((Float_t)nModule_inTRU_phi / 3. );
      nModule_inTRU_eta =                  nModule_inTRU_eta * 3   ;
    }

    //TRU ieta/iphi offset calculation 
    for(Int_t i=0; i<nTRU_inSM; i++){
      fnFastORInTRUPhi[iTRU]  = nModule_inTRU_phi ;
      fnFastORInTRUEta[iTRU]  = nModule_inTRU_eta ;
      if((iTRU+1) >= fNTotalTRU)break;
      
      TRU_type  = 0 ;
      if( i==nTRU_inSM-1 ){//last TRU in SM
        if(      iSM%2==0 && SM_type==kDCAL_Standard ) TRU_type = 2  ;//left  DCAL 
        else if( iSM%2==1 && SM_type==kDCAL_Standard ) TRU_type = 3  ;//right DCAL 
        else if( iSM%2==1                            ) TRU_type = 1  ;//right EMCAL
      }
      if(      TRU_type == 0){
        fTRUFastOROffsetX[iTRU+1]  = fTRUFastOROffsetX[iTRU] + nModule_inTRU_eta      ;
        fTRUFastOROffsetY[iTRU+1]  = fTRUFastOROffsetY[iTRU]                          ;
      }else if(TRU_type == 1){
        fTRUFastOROffsetX[iTRU+1]  = 0                                                ;
        fTRUFastOROffsetY[iTRU+1]  = fTRUFastOROffsetY[iTRU] + nModule_inTRU_phi      ;
      }else if(TRU_type == 2){
        fTRUFastOROffsetX[iTRU+1]  = fTRUFastOROffsetX[iTRU] + nModule_inTRU_eta * 3  ;
        fTRUFastOROffsetY[iTRU+1]  = fTRUFastOROffsetY[iTRU]                          ;
      }else if(TRU_type == 3){
        fTRUFastOROffsetX[iTRU+1]  = 0                                                ;
        fTRUFastOROffsetY[iTRU+1]  = fTRUFastOROffsetY[iTRU] + nModule_inTRU_phi      ;
      }
      iTRU++  ;
    }//TRU loop
  }//SM loop
  
  return kTRUE ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::Init_SM_offset(){

  fSMFastOROffsetX[0]  = 0 ;
  fSMFastOROffsetY[0]  = 0 ;
  fnModuleInEMCALPhi  = 0 ;

  for(int iSM=0; iSM<fNumberOfSuperModules; iSM++){
    Int_t SM_type = GetSMType(iSM);
    
    Int_t nModule_inSM_phi  = fNPhi    ;
    Int_t nModule_inSM_eta  = fNEta    ;
    
    //kEMCAL_Standard:kEMCAL_Half -> default value
    if(     SM_type == kEMCAL_3rd      ){
      nModule_inSM_phi  = (Int_t)((Float_t)nModule_inSM_phi      / 3.);
    }
    else if(SM_type == kDCAL_Standard  ){
      nModule_inSM_eta  = (Int_t)((Float_t)nModule_inSM_eta * 2. / 3.);
    }
    else if(SM_type == kDCAL_Ext       ){
      nModule_inSM_phi  = (Int_t)((Float_t)nModule_inSM_phi      / 3.);
    }

    fnFastORInSMPhi[iSM]  = nModule_inSM_phi ;
    fnFastORInSMEta[iSM]  = nModule_inSM_eta ;
    if( (iSM+1) >= fNumberOfSuperModules)break  ;

    if(iSM%2 == 1){//right SM
      fSMFastOROffsetX[iSM+1]  = 0                                        ;
      fSMFastOROffsetY[iSM+1]  = fSMFastOROffsetY[iSM] + nModule_inSM_phi ;
    }
    else{//left SM
      fnModuleInEMCALPhi += nModule_inSM_phi  ;
      if(SM_type == kDCAL_Standard){
        fSMFastOROffsetX[iSM+1]  = fSMFastOROffsetX[iSM] + nModule_inSM_eta * 2 ;
        fSMFastOROffsetY[iSM+1]  = fSMFastOROffsetY[iSM]                        ;
      }else{
        fSMFastOROffsetX[iSM+1]  = fSMFastOROffsetX[iSM] + nModule_inSM_eta     ;
        fSMFastOROffsetY[iSM+1]  = fSMFastOROffsetY[iSM]                        ;
      }
    }
  }//SM loop
  return kTRUE ;
}

//________________________________________________________________________________________________
Bool_t AliEMCALTriggerMappingV2::GetInfoFromAbsFastORIndex(
    const Int_t id, 
    Int_t& iTRU , Int_t& iADC , Int_t& iEta_TRU , Int_t& iPhi_TRU , 
    Int_t& iSM  ,               Int_t& iEta_SM  , Int_t& iPhi_SM  
    ) const
{

  Int_t nModule = fNEta * 2 * fnModuleInEMCALPhi;
  if (id > nModule-1 || id < 0){
    AliError("Id out of range!");
    return kFALSE;
  }
	
  Int_t x       = id % (fNEta * 2) ;
  Int_t y       = id / (fNEta * 2) ;
  if(y >= fNPhi*5 + 4)  y = y + 8 ;
  
  Int_t y_class     = int(y/fNModulesInTRUPhi)   ;
  Int_t x_class     = int(x/fNModulesInTRUEta)   ;
  iTRU = y_class*(fNTRUEta*2) + x_class ;
  
  Int_t Cside       = int(x/fNEta);//Cside SM
  //y_class = 5 : EMCAL 1/3 SM
  //y_class = 9 : DCAL  1/3 SM
  Int_t DCAL_sepC   = (y_class>5 && y_class<9 && Cside==1)? 1 : 0 ;
  Bool_t IsPHOS     = (y_class>5 && y_class<9 && (x_class==2 || x_class==3))? kTRUE : kFALSE ;

  if(y_class==5 || y_class==9 ) iTRU = iTRU - (x_class%3) - Cside*2 ;
  if(y_class> 5               ) iTRU = iTRU - 2 - (y_class-5)*2 - DCAL_sepC*2 ;

  iADC = (y_class==5 || y_class==9)? (y%(fNModulesInTRUPhi/3))*(fNModulesInTRUEta*3) + (x%(fNModulesInTRUEta*3)) :
                                     (y% fNModulesInTRUPhi   )* fNModulesInTRUEta    + (x% fNModulesInTRUEta   ) ;

  iSM       = int(y/fNPhi)*2 + Cside  ; 
  iEta_TRU  = (y_class==5 || y_class==9)? x%(fNModulesInTRUEta*3) : x%(fNModulesInTRUEta);
  iPhi_TRU  = y%fNModulesInTRUPhi;
  iEta_SM   = (DCAL_sepC == 1)? (x%fNEta - 8) : (x%fNEta);
  iPhi_SM   = y%fNPhi;

  if(IsPHOS){
    iTRU      = -1  ;
    iADC      = -1  ;
    iSM       = -1  ;
    iEta_TRU  = -1  ;
    iPhi_TRU  = -1  ;
    iEta_SM   = -1  ;
    iPhi_SM   = -1  ;
  }
  return kTRUE;
}

