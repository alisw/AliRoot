/*
This a modification in a code sent by Marco Bregant, which, originally, created the OADB for misalignment matrices
In this macro, the histograms with BadChannels Factors are loaded and some TObjarrays are filled with these histograms.
At the end, a OADB container is created receiving these arrays.
The histograms with badchannels can be made by a list of towers ID ( see the macro CreateEMCAL_OADB_BadChannelsHistos.C ) or 
directly from the OCDB ( see AliEMCALOCDBTenderConverter.cxx from Jiri Kral )
*/
// ******* Create OADB Container for EMCal Bad Channels
void UpdateEMCAL_OADB_BadChannels(const char *fileNameOADB	="$ALICE_ROOT/OADB/EMCAL/EMCALBadChannels.root",
				  const char *fileName13b	="BadChannels2013_13b.root",
				  const char *fileName13c	="BadChannels2013_13b.root",
				  const char *fileName13d	="BadChannels2013_13b.root",
				  const char *fileName13e	="BadChannels2013_13b.root",
				  const char *fileName13f	="BadChannels2013_13b.root",
				  const char *fileName13f1	="BadChannels2013_13f1.root",
				  const char *fileName13f2	="BadChannels2013_13f2.root",
				  const char *fileName13f3	="BadChannels2013_13f3.root",
				  const char *fileName13f4	="BadChannels2013_13f4.root",
				  const char *fileName13g	="BadChannels2013_13g.root"
				)
{

    gSystem->Load("libOADB");      
    //gSystem->Load("libOADB"); //comment if compiled

    //Create OADB container for BadChannels
    AliOADBContainer *con=new AliOADBContainer("");
    con->InitFromFile(Form(fileNameOADB),"AliEMCALBadChannels"); 

    //! List of arrays to be updated (new histograms and/or new run range
    TObjArray *array13b = (TObjArray*)con->GetObject(195345,"BadChannels13b");
    TObjArray *array13c = (TObjArray*)con->GetObject(195529,"BadChannels13c");
    TObjArray *array13d = (TObjArray*)con->GetObject(195681,"BadChannels13d");
    TObjArray *array13e = (TObjArray*)con->GetObject(195935,"BadChannels13e");
    TObjArray *array13f = (TObjArray*)con->GetObject(196433,"BadChannels13f");
    TObjArray *array13f1 = (TObjArray*)con->GetObject(196563,"BadChannels13f1");
    TObjArray *array13f2 = (TObjArray*)con->GetObject(196646,"BadChannels13f2");     
    TObjArray *array13f3 = (TObjArray*)con->GetObject(197094,"BadChannels13f3");
    TObjArray *array13f4 = (TObjArray*)con->GetObject(197348,"BadChannels13f4");
    array13b->Clear();
    array13c->Clear();
    array13d->Clear();
    array13e->Clear();
    array13f->Clear();
    array13f1->Clear();
    array13f2->Clear();
    array13f3->Clear();
    array13f4->Clear();
    
    
    //! List of brand new arrays
    //LHC13g
    TObjArray *array13g = new TObjArray(12);		array13g->SetName("BadChannels13g");
    
    
    TFile	*f13b	=	new TFile(fileName13b,"read");
    TFile	*f13c	=	new TFile(fileName13c,"read");
    TFile	*f13d	=	new TFile(fileName13d,"read");
    TFile	*f13e	=	new TFile(fileName13e,"read");
    
    TFile	*f13f	=	new TFile(fileName13f,"read");
    TFile	*f13f1	=	new TFile(fileName13f1,"read");
    TFile	*f13f2	=	new TFile(fileName13f2,"read");
    TFile	*f13f3	=	new TFile(fileName13f3,"read");
    TFile	*f13f4	=	new TFile(fileName13f4,"read");

    TFile	*f13g	=	new TFile(fileName13g,"read");
      
    char name[30];
    


    
    for (Int_t mod=0;mod<12;mod++){
	cout<<"SM "<< mod<<endl;

	    sprintf(name,"EMCALBadChannelMap_Mod%d",mod);
	    cout<<"BadChannels 2012 and 2013:"<<name<<endl;
	    
	    array13b->Add(f13b->Get(name));
	    array13c->Add(f13c->Get(name));
	    array13d->Add(f13d->Get(name));
	    array13e->Add(f13e->Get(name));
	    array13f->Add(f13f->Get(name));
	    array13f1->Add(f13f1->Get(name));
	    array13f2->Add(f13f2->Get(name));
	    array13f3->Add(f13f3->Get(name));
	    array13f4->Add(f13f4->Get(name));
	    array13g->Add(f13g->Get(name));
	    
    } //mod
    
    //con->AddDefaultObject(&array12i1);
    //con->AddDefaultObject(&array12i2);
    
//     con->AddDefaultObject(*&array13c);
//     con->AddDefaultObject(*&array13d);
//     con->AddDefaultObject(*&array13e);
//     con->AddDefaultObject(*&array13f);
//     con->AddDefaultObject(*&array13f1);
//     con->AddDefaultObject(*&array13f2);
//     con->AddDefaultObject(*&array13f3);
//     con->AddDefaultObject(*&array13f4);
//     con->AddDefaultObject(*&array13g);

//     con->AddDefaultObject(*&array13c);
    
    con->UpdateObject(con->GetIndexForRun(195344),*&array13b,195344,195483);
    con->UpdateObject(con->GetIndexForRun(195529),*&array13c,195529,195677);
    con->UpdateObject(con->GetIndexForRun(195681),*&array13d,195681,195873);
    con->UpdateObject(con->GetIndexForRun(195935),*&array13e,195935,196311);
    con->UpdateObject(con->GetIndexForRun(196433),*&array13f,196433,196535);
    con->UpdateObject(con->GetIndexForRun(196563),*&array13f1,196563,196608);
    con->UpdateObject(con->GetIndexForRun(196646),*&array13f2,196646,197092);     
    con->UpdateObject(con->GetIndexForRun(197094),*&array13f3,197094,197099);
    con->UpdateObject(con->GetIndexForRun(197138),*&array13f2,197138,197302);
    con->UpdateObject(con->GetIndexForRun(197341),*&array13f,197341,197342);
    con->UpdateObject(con->GetIndexForRun(197348),*&array13f4,197348,197388);

    //Establishing run number with the correct objects
    
    //con->AppendObject(&array13b,195344,195483);
    //con->AppendObject(&array13c,195529,195677);
    //con->AppendObject(&array13d,195681,195873);
    //con->AppendObject(&array13e,195935,196311);
    //con->AppendObject(&array13f,196433,196535);
    //con->AppendObject(&array13f1,196563,196608);
    //con->AppendObject(&array13f2,196646,197092);     
    //con->AppendObject(&array13f3,197094,197099);
    //con->AppendObject(&array13f2,197138,197302);
    //con->AppendObject(&array13f,197341,197342);
    //con->AppendObject(&array13f4,197348,197388);

    con->AppendObject(*&array13g,197470,197692);

//     delete array13b;
    
    
    con->WriteToFile("BetaBadChannels.root");   
}

