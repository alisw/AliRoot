// ******* Example to Create or Read OADB Container 
// ******* for EMCal Run by Run calibration dependent 
// ******* on Temperature variations
// Author : Gustavo Conesa Balbastre (LPSC-Grenoble)

void CreateEMCAL_OADB_RunByRunTemperatureECalibCorrection(Int_t opt = 1, Int_t runNumber = 170387)
{
  if(opt == 0) Read(runNumber);
  if(opt == 1) Create();
}

void Create()
{
  //Create OADB container for Temperature calibration parameters

  gSystem->Load("libOADB");            

  AliOADBContainer* con = new AliOADBContainer("AliEMCALRunDepTempCalibCorrections");
      
  // Get the list of run numbers to be added to the OADB, parameters provided usually in a 
  // root file per run
  // Tar ball with all the files can be found here
  // https://twiki.cern.ch/twiki/bin/viewauth/ALICE/EMCalTimeDependentCalibrations
  
  ifstream fList;
  fList.open("CorrectionFiles/runlist.txt");
  
  Int_t runNumber  = 0;
  TString string;
  Int_t nRuns=0;
  Int_t nSM = 12;

  AliEMCALGeometry* geom = AliEMCALGeometry::GetInstance("EMCAL_COMPLETE12SMV1");
 
  if (fList.good()) 
  {
    while(string.ReadLine(fList, kFALSE) && !fList.eof()) 
    {
      sscanf(string.Data(), "%d",&runNumber);
      
      //printf("Run %d \n",runNumber);

      if     (runNumber < 140000) nSM = 4;
      else if(runNumber < 173000) nSM = 10;
      if(runNumber==170387){
      // Access class that contains methods to read the content of 
      // the calibration file per run
      AliEMCALCalibTimeDepCorrection  *corr =  new AliEMCALCalibTimeDepCorrection();
      corr->ReadRootInfo(Form("CorrectionFiles/Run%d_Correction.root",runNumber));
      
      // Init the histogram
      TH1S *h = new TH1S(Form("h%d",runNumber),"",24*48*nSM,0,24*48*nSM);
            
      for(Int_t ism = 0; ism < nSM; ism++)
      {
        for(Int_t icol = 0; icol < 48; icol++)
        {
          for(Int_t irow = 0; irow < 24; irow++)
          {
            Float_t recalFactor = corr->GetCorrection(ism, icol,irow,0);
            
            Int_t absID = geom->GetAbsCellIdFromCellIndexes(ism, irow, icol);
            
            //if(recalFactor < 0.1) 
              printf("ism %d, icol %d, irow %d,absID %d, corrections : factor %d\n",ism, icol, irow, absID, recalFactor*10000);
            
            h->SetBinContent(absID,(Short_t)(recalFactor*10000));
            //h->SetBinContent(absID,recalFactor);
            
          }
        }
      }
      
      con->AddDefaultObject(h);
      
      //Establishing run number with the correct objects
      con->AppendObject(h,runNumber,runNumber);
      
      delete corr;
      
      nRuns++;
      }
    }
  }
  
  fList.close();
  
  printf(" *** nRuns ***  %d\n",nRuns);
  
  con->WriteToFile("EMCALTemperatureCorrCalib.root");   
    

}

void Read(Int_t runNumber = 170387)
{
  
  gSystem->Load("libOADB");            
  
  AliOADBContainer *cont=new AliOADBContainer("");
  cont->InitFromFile("$ALICE_ROOT/OADB/EMCAL/EMCALTemperatureCorrCalib.root", "AliEMCALRunDepTempCalibCorrections");
  
  //cout<<"_________--------------- dump ---------------------___________"<<endl;
  //cont->Dump();
  
  //cout<<"cont->GetDefaultList()->Print()"<<endl;
  //cont->GetDefaultList()->Print();
  
  TH1S *h=cont->GetObject(runNumber); //GetObject(int runnumber)
  
  printf("h entries %d\n");
  
  
  AliEMCALGeometry* geom = AliEMCALGeometry::GetInstance("EMCAL_COMPLETE12SMV1");
  
  // Read parameter file line-by-line  
  // Get number of lines first
  
  Int_t nSM = 10;
  
  for(Int_t iabsID = 0; iabsID < 24*48*nSM; iabsID++)
  {
    printf("absID %d, content %d\n",iabsID,h->GetBinContent(iabsID));
    
  }
  
  h->Draw();
  
}


