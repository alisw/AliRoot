// Compare the contents of the calibration, bad channels, etc in OCDB and AODB
// You need connexion to the grid to run this macro
// The OADB file can be in a place different than the default in aliroot

// Author : Gustavo Conesa Balbastre (LPSC-CNRS)

void CompareAODB_OCDB(Int_t run = 153323, TString pathOADB = "$ALICE_ROOT/OADB/EMCAL",
                      Int_t checkObject = 1, TString pass = "pass1", Bool_t printAll = kFALSE)
{  

  gSystem->Load("libOADB");
  TGrid::Connect("alien://");
  
  AliCDBManager* man = AliCDBManager::Instance();
  man->SetDefaultStorage("raw://");
  man->SetRun(run);
  AliCDBStorage *storage = man->GetDefaultStorage();
  
  // Instantiate EMCAL geometry for the first time

  
  if     (run < 140000) geom = AliEMCALGeometry::GetInstance("EMCAL_FIRSTYEARV1"); // 2010
  else if(run < 198000) geom = AliEMCALGeometry::GetInstance("EMCAL_COMPLETEV1");  // 2011-2012-2013
  else                  geom = AliEMCALGeometry::GetInstance("EMCAL_COMPLETE12SMV1_DCAL_8SM"); // Run2
  
  if     (checkObject == 0) CheckBadChannels      (run, pathOADB, storage, geom,     printAll);
  else if(checkObject == 1) CheckEnergyCalibration(run, pathOADB, storage, geom,pass,printAll);
  else if(checkObject == 2) CheckTimeCalibration  (run, pathOADB, storage, geom,pass,printAll);
  else printf("non existing object option\n");
  
  printf("*** Comparisons ended *** \n");
}  

//___________________________
void CheckEnergyCalibration(Int_t run, TString pathOADB, AliCDBStorage * storage, AliEMCALGeometry *geom,
                            TString pass,Bool_t printAll = kTRUE)
{
  // Check energy recalibration, Uncomplete
  
  AliOADBContainer *contRF=new AliOADBContainer("");
  contRF->InitFromFile(Form("%s/EMCALRecalib.root",pathOADB.Data()),"AliEMCALRecalib");
  
  const Int_t nSM = geom->GetNumberOfSuperModules();
  
  // Get the OCDB object
  
  AliEMCALCalibData* cparam = (AliEMCALCalibData*) (storage->Get("EMCAL/Calib/Data", run)->GetObject());

//  // Access directly the OCDB file and not the latest version
//  TFile * f = TFile::Open("alien:///alice/data/2011/OCDB/EMCAL/Calib/Data/Run144484_999999999_v2_s0.root","READ");
//  AliCDBEntry * cdb = (AliCDBEntry*) f->Get("AliCDBEntry");
//  AliEMCALCalibData* cparam2 = (AliEMCALCalibData*)  cdb->GetObject();

  // Get the OADB object
  
  TH2F *h[12];
  
  TObjArray *recal=(TObjArray*)contRF->GetObject(run);
  
  if(!recal)
  {
    printf("Energy recalibration OADB not found  3\n");
    return;
  }
  
  TObjArray *recalpass=(TObjArray*)recal->FindObject(pass);
  
  if(!recalpass)
  {
    printf("Energy recalibration OADB not found 2\n");
    return;
  }
  
  TObjArray *recalib=(TObjArray*)recalpass->FindObject("Recalib");
  
  if(!recalib)
  {
    printf("Energy recalibration OADB not found 1\n");
    return;
  }
  
  for (Int_t i=0; i < nSM; ++i)
  {
    h[i] = (TH2F*)recalib->FindObject(Form("EMCALRecalFactors_SM%d",i));
    if (!h[i])
    {
      AliError(Form("Could not load EMCALRecalFactors_SM%d",i));
      continue;
    }
  }
  
  // Do the comparison
  Float_t paramOCDB = -1;
  Float_t paramAODB = -1;
  Int_t nDiff = 0;
//  Int_t nDiff2 = 0;
  Int_t iCol = -1, iRow = -1, iSM =-1, iMod = -1,iIphi =-1,iIeta = -1;
  for(Int_t i=0;i < nSM*24*48; i++)
  {
    //printf("AbsID %d\n",i);
    geom->GetCellIndex(i,iSM,iMod,iIphi,iIeta);
    geom->GetCellPhiEtaIndexInSModule(iSM,iMod, iIphi, iIeta,iRow,iCol);
    
    Float_t paramOCDB = -1;
    if(cparam) paramOCDB = cparam->GetADCchannel(iSM,iCol,iRow);

//    Float_t paramOCDB2 = -1;
//    if(cparam) paramOCDB2 = cparam2->GetADCchannel(iSM,iCol,iRow);
    
    Float_t paramAODB = -1;
    if(h[iSM]) paramAODB = h[iSM]->GetBinContent(iCol,iRow);
    paramAODB*=0.0162; // Transformation into OCDB parameter, it will not work for all channels
    
    if    (printAll)
    printf("STATUS: ID %d, col %d, row %d, sm %d  OCDB %1.4f, OADB %1.4f\n",
           i,iCol,iRow,iSM,paramOCDB, paramAODB);
    else if(paramAODB > 0)
    {
      if (paramOCDB/paramAODB > 1.01 || paramOCDB/paramAODB < 0.99) )
      {
        printf("DIFFERENT STATUS: ID %d, col %d, row %d, sm %d  OCDB %1.4f, OADB %1.4f, ratio OCDB/OADB %1.4f\n",//, old OCDB param %1.4f\n",
               i,iCol,iRow,iSM,paramOCDB, paramAODB,paramOCDB/paramAODB);//,paramOCDB2);
        nDiff++;
      }
      else if(paramAODB <= 0)
      {
        printf("DIFFERENT STATUS: ID %d, col %d, row %d, sm %d  OCDB %1.4f, OADB %1.4f\n",//, old OCDB param %1.4f\n",
               i,iCol,iRow,iSM,paramOCDB, paramAODB);//,paramOCDB2);
        nDiff++;
      }
      
//      if(TMath::Abs(paramOCDB2-0.0162)> 0.0001)
//      {
//        printf("\t Different initial calib! %1.4f\n",paramOCDB2);
//        nDiff2++;
//      }
    }
  }
  
  if(!printAll) printf("Total number of different channels %d / %d\n",nDiff,nSM*24*48);//, origin %d\n",nDiff,nSM*24*48,nDiff2);
}

//_____________________
void CheckBadChannels(Int_t run, TString pathOADB, AliCDBStorage * storage, AliEMCALGeometry *geom, Bool_t printAll = kTRUE)
{
  // Get the OCDB bad channels and compare to the OADB ones
  
  //const Int_t nSM = static_const (geom->GetNumberOfSuperModules());
  const Int_t nSM = static_cast<const Int_t> (geom->GetNumberOfSuperModules());

  // Access OCDB histograms
  AliCaloCalibPedestal* caloped  = (AliCaloCalibPedestal*) (storage->Get("EMCAL/Calib/Pedestals", run)->GetObject());
    
  // Access directly the OCDB file and not the latest version
  //TFile * f = TFile::Open("alien:///alice/data/2011/OCDB/EMCAL/Calib/Pedestals/Run145954_146856_v3_s0.root","READ");
  //AliCDBEntry * cdb = (AliCDBEntry*) f->Get("AliCDBEntry");
  //AliCaloCalibPedestal * caloped = (AliCaloCalibPedestal *) cdb->GetObject();  
  
  TObjArray map = caloped->GetDeadMap();

  // Access OADB histograms
  TH2I *hbm[12];
  
  AliOADBContainer *contBC=new AliOADBContainer("");
  
  contBC->InitFromFile(Form("%s/EMCALBadChannels.root",pathOADB.Data()),"AliEMCALBadChannels"); 
  
  TObjArray *arrayBC=(TObjArray*)contBC->GetObject(run);
  
  if(!arrayBC)
  {
    printf("--- Bad map not available \n");
    return;
  }
  
  for (Int_t i=0; i < nSM; ++i)
  {
    hbm[i]=(TH2I*)arrayBC->FindObject(Form("EMCALBadChannelMap_Mod%d",i));
    
    if (!hbm)
    {
      AliError(Form("Can not get EMCALBadChannelMap_Mod%d",i));
      continue;
    }
  }
  
  Int_t badMapOCDB = -1;
  Int_t badMapAODB = -1;
  Int_t iCol = -1, iRow = -1, iSM =-1, iMod = -1,iIphi =-1,iIeta = -1;    
  for(Int_t i=0;i < nSM*24*48; i++)
  {
    //printf("AbsID %d\n",i);
    geom->GetCellIndex(i,iSM,iMod,iIphi,iIeta); 
    geom->GetCellPhiEtaIndexInSModule(iSM,iMod, iIphi, iIeta,iRow,iCol);
    if(map.At(iSM))badMapOCDB = ((TH2F*)map.At(iSM))->GetBinContent(iCol,iRow);
    else badMapOCDB  = -1;
    if(hbm[iSM])   badMapAODB = hbm[iSM]->GetBinContent(iCol,iRow);
     else badMapAODB = -1;

    if    (printAll && badMapOCDB > 0)
      printf("STATUS: ID %d, col %d, row %d, sm %d  OCDB %d, OADB %d\n",
             i,iCol,iRow,iSM,badMapOCDB, badMapAODB);
    else if(badMapOCDB!=badMapAODB)
      printf("DIFFERENT STATUS: ID %d, col %d, row %d, sm %d  OCDB %d, OADB %d\n",
             i,iCol,iRow,iSM,badMapOCDB, badMapAODB);

  }
  
}

//_____________________
void CheckTimeCalibration(Int_t run, TString pathOADB, AliCDBStorage * storage, AliEMCALGeometry *geom, TString pass, Bool_t printAll = kTRUE)
{
  printf(" === Comparison for this object is not implemented === \n");
}

