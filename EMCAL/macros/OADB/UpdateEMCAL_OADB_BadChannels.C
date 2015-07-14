/*
This a modification in a code sent by Marco Bregant, which, originally, created the OADB for misalignment matrices
In this macro, the histograms with BadChannels Factors are loaded and some TObjarrays are filled with these histograms.
At the end, a OADB container is created receiving these arrays.
The histograms with badchannels can be made by a list of towers ID ( see the macro CreateEMCAL_OADB_BadChannelsHistos.C ) or 
directly from the OCDB ( see AliEMCALOCDBTenderConverter.cxx from Jiri Kral )
*/
// ******* Create OADB Container for EMCal Bad Channels

TObjArray GetHistoObject( Int_t runNum, Bool_t LocalOCDB=0){
	Int_t i;
	char buf[100];

	TH2D *histo;

	TFile *outFile;
	
	AliCDBManager *man;
	AliCDBStorage *stor;
	AliCaloCalibPedestal *ped;

	// created the OCDB manager
	man = AliCDBManager::Instance();
	if(LocalOCDB) stor = man->GetStorage( "local://$ALICE_ROOT/OCDB"); //if you download the OCDB files locally.
	else {  
	  man->SetDefaultStorage("raw://");
	  man->SetRun(runNum);
	  stor = man->GetDefaultStorage();
	}
	ped = (AliCaloCalibPedestal*)(stor->Get("EMCAL/Calib/Pedestals", runNum)->GetObject());

	// get the array of histos
	TObjArray map = ped->GetDeadMap();
	for(int i = 0; i < map.GetEntries(); i++ ){
		histo = (TH2D*)(map[i]);
		printf("\n !!! EMCALBadChannelMap_Mod%d",i );
		histo->SetName( Form("EMCALBadChannelMap_Mod%d", i ));
		histo->SetTitle( Form("EMCALBadChannelMap_Mod%d", i ));
	}

	return map;
}

void UpdateEMCAL_OADB_BadChannels(const char *fileNameOADB="$ALICE_PHYSICS/OADB/EMCAL/EMCALBadChannels.root",Bool_t LocalOCDB=0)
{

 ///   gSystem->Load("libOADB");      
    //gSystem->Load("libOADB"); //comment if compiled

    //Create OADB container for BadChannels
    AliOADBContainer *con=new AliOADBContainer("");
    con->InitFromFile(Form(fileNameOADB),"AliEMCALBadChannels"); 

//AliOADBContainer* con = new AliOADBContainer("AliEMCALBadChannels");

    //! List of brand new arrays
    //LHC13g
    TObjArray *array = new TObjArray(12);		
    array->SetName("BadChannels");
   // 'AliEMCALOCDBTenderConverter.cxx(176326,"BadChannels2012_12a1.root")
    char name[30];

    TObjArray array12a1 = GetHistoObject(176326,LocalOCDB);// Run176326_176700_v4_s0.root
    TObjArray array12a2 = GetHistoObject(176701,LocalOCDB);// Run176701_176714_v4_s0.root
    TObjArray array12a3 = GetHistoObject(176715,LocalOCDB);// Run176715_176729_v4_s0.root
    TObjArray array12a4 = GetHistoObject(176730,LocalOCDB);// Run176730_176858_v4_s0.root
    TObjArray array12a5 = GetHistoObject(176859,LocalOCDB);// Run176859_177295_v4_s0.root
    
    TObjArray array12b1 = GetHistoObject(177320,LocalOCDB);// Run177320_999999999_v2_s0 --> 177381-177383
    TObjArray array12b2 = GetHistoObject(177384,LocalOCDB);// Run177384_178220_v5_s0.root
    TObjArray array12b3 = GetHistoObject(177444,LocalOCDB);// Run177444_177682_v6_s0.root
    TObjArray array12b4 = GetHistoObject(177844,LocalOCDB);// Run177844_177849_v6_s0.root     
    TObjArray array12b5 = GetHistoObject(178220,LocalOCDB);// Run177384_178220_v5_s0.root

        
    con->AddDefaultObject(&array12a1);
    con->AddDefaultObject(&array12a2);
    con->AddDefaultObject(&array12a3);
    con->AddDefaultObject(&array12a4);
    con->AddDefaultObject(&array12a5);
    con->UpdateObject(con->GetIndexForRun(176326),&array12a1,176326,176700);
    con->AppendObject(&array12a2,176701,176714);
    con->AppendObject(&array12a3,176715,176729);
    con->AppendObject(&array12a4,176730,176858);
    con->AppendObject(&array12a5,176859,177295);
 
    con->AddDefaultObject(&array12b1);
    con->AddDefaultObject(&array12b2);
    con->AddDefaultObject(&array12b3);
    con->AddDefaultObject(&array12b4);
    con->AddDefaultObject(&array12b5);
    
   con->UpdateObject(con->GetIndexForRun(177383),&array12b1,177320,177383);
   con->UpdateObject(con->GetIndexForRun(177384),&array12b2,177384,177443);
   con->UpdateObject(con->GetIndexForRun(177444),&array12b3,177444,177682);
   con->UpdateObject(con->GetIndexForRun(177844),&array12b4,177844,177849);
   con->UpdateObject(con->GetIndexForRun(177850),&array12b5,177850,178220);

    
    
    con->WriteToFile("BetaBadChannels.root");   
}

