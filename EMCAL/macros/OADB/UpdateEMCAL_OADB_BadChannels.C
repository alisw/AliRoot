/*
This a modification in a code sent by Marco Bregant, which, originally, created the OADB for misalignment matrices
In this macro, the histograms with BadChannels Factors are loaded and some TObjarrays are filled with these histograms.
At the end, a OADB container is created receiving these arrays.
The histograms with badchannels can be made by a list of towers ID ( see the macro CreateEMCAL_OADB_BadChannelsHistos.C ) or 
directly from the OCDB ( see AliEMCALOCDBTenderConverter.cxx from Jiri Kral )
*/
// ******* Create OADB Container for EMCal Bad Channels
void UpdateEMCAL_OADB_BadChannels(const char *fileNameOADB	="$ALICE_ROOT/OADB/EMCAL/EMCALBadChannels.root",
				  const char *fileName12i1	="BadChannels2012_12i1.root",
				  const char *fileName12i2	="BadChannels2012_12i2.root",
				  const char *fileName13b	="BadChannels2012_13b.root",
				  const char *fileName13c	="BadChannels2012_13c.root",
				  const char *fileName13d	="BadChannels2012_13d.root",
				  const char *fileName13e	="BadChannels2012_13e.root",
				  const char *fileName13f	="BadChannels2012_13f.root",
				  const char *fileName13f1	="BadChannels2012_13f1.root",
				  const char *fileName13f2	="BadChannels2012_13f2.root",
				  const char *fileName13f3	="BadChannels2012_13f3.root",
				  const char *fileName13f4	="BadChannels2012_13f4.root"
				)
{

    gSystem->Load("libOADB");      
    //gSystem->Load("libOADB"); //comment if compiled

    //Create OADB container for BadChannels
    AliOADBContainer *con=new AliOADBContainer("");
    con->InitFromFile(Form(fileNameOADB),"AliEMCALBadChannels"); 
    
    
    //LHC12i
    TObjArray array12i1(12);		array12i1.SetName("BadChannels12i1");
    TObjArray array12i2(12);		array12i2.SetName("BadChannels12i2");

    
    //LHC13b
    TObjArray array13b(12);		array13b.SetName("BadChannels13b");
    
    //LHC13c
    TObjArray array13c(12);		array13c.SetName("BadChannels13c");
    
    //LHC13d
    TObjArray array13d(12);		array13d.SetName("BadChannels13d");

    //LHC13e
    TObjArray array13e(12);		array13e.SetName("BadChannels13e");

    //LHC13f
    TObjArray array13f(12);		array13f.SetName("BadChannels13f");
    TObjArray array13f1(12);		array13f1.SetName("BadChannels13f1");
    TObjArray array13f2(12);		array13f2.SetName("BadChannels13f2");
    TObjArray array13f3(12);		array13f3.SetName("BadChannels13f3");
    TObjArray array13f4(12);		array13f4.SetName("BadChannels13f4");
    
    
    
    
    TFile	*f12i1	=	new TFile(fileName12i1,"read"); 
    TFile	*f12i2	=	new TFile(fileName12i2,"read"); 
    
    TFile	*f13b	=	new TFile(fileName13b,"read");
    TFile	*f13c	=	new TFile(fileName13c,"read");
    TFile	*f13d	=	new TFile(fileName13d,"read");
    TFile	*f13e	=	new TFile(fileName13e,"read");
    
    TFile	*f13f	=	new TFile(fileName13f,"read");
    TFile	*f13f1	=	new TFile(fileName13f1,"read");
    TFile	*f13f2	=	new TFile(fileName13f2,"read");
    TFile	*f13f3	=	new TFile(fileName13f3,"read");
    TFile	*f13f4	=	new TFile(fileName13f4,"read");
    
    char name[30];
    for (Int_t mod=0;mod<12;mod++){
	cout<<"SM "<< mod<<endl;

	    sprintf(name,"EMCALBadChannelMap_Mod%d",mod);
	    cout<<"BadChannels 2012 and 2013:"<<name<<endl;
	    array12i1.Add(f12i1->Get(name));
	    array12i2.Add(f12i2->Get(name));
	    array13b.Add(f13b->Get(name));
	    array13c.Add(f13c->Get(name));
	    array13d.Add(f13d->Get(name));
	    array13e.Add(f13e->Get(name));
	    array13f.Add(f13f->Get(name));
	    array13f1.Add(f13f1->Get(name));
	    array13f2.Add(f13f2->Get(name));
	    array13f3.Add(f13f3->Get(name));
	    array13f4.Add(f13f4->Get(name));
	    
    } //mod
    
    con->AddDefaultObject(&array12i1);
    con->AddDefaultObject(&array12i2);
    con->AddDefaultObject(&array13b);
    con->AddDefaultObject(&array13c);
    con->AddDefaultObject(&array13d);
    con->AddDefaultObject(&array13e);
    con->AddDefaultObject(&array13f);
    con->AddDefaultObject(&array13f1);
    con->AddDefaultObject(&array13f2);
    con->AddDefaultObject(&array13f3);
    con->AddDefaultObject(&array13f4);
    
    //Establishing run number with the correct objects
    con->AppendObject(&array12i1,192745,192824);
    con->AppendObject(&array12i2,193004,193766);
    con->AppendObject(&array13b,195344,195483);
    con->AppendObject(&array13c,195529,195677);
    con->AppendObject(&array13d,195681,195873);
    con->AppendObject(&array13e,195935,196311);
    con->AppendObject(&array13f,196433,196535);
    con->AppendObject(&array13f1,196563,196608);
    con->AppendObject(&array13f2,196646,197092);     
    con->AppendObject(&array13f3,197094,197099);
    con->AppendObject(&array13f2,197138,197302);
    con->AppendObject(&array13f,197341,197342);
    con->AppendObject(&array13f4,197348,197388);
    con->WriteToFile("BetaBadChannels.root");   
}

