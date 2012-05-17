/*
This a modification in a code sent by Marco Bregant, which, originally, created the OADB for misalignment matrices
In this macro, the histograms with BadChannels Factors are loaded and some TObjarrays are filled with these histograms.
At the end, a OADB container is created receiving these arrays.
The histograms with badchannels can be made by a list of towers ID ( see the macro CreateEMCAL_OADB_BadChannelsHistos.C ) or 
directly from the OCDB ( see AliEMCALOCDBTenderConverter.cxx from Jiri Kral )
*/
// ******* Create OADB Container for EMCal Bad Channels
void CreateEMCAL_OADB_BadChannels(const char *fileName10	=	"BadChannels2010.root",
				  const char *fileName11a	=	"BadChannels2011_11a.root", 
				  const char *fileName11b	=	"BadChannels2011_11b.root",
				  const char *fileName11c1	=	"BadChannels2011_11c1.root", 
				  const char *fileName11c2	=	"BadChannels2011_11c2.root", //Subperiod with different histograms
				  const char *fileName11c3	=	"BadChannels2011_11c3.root",
				  const char *fileName11d	=	"BadChannels2011_11d.root",
				  const char *fileName11e	=	"BadChannels2011_11e.root",
				  const char *fileName11f	=	"BadChannels2011_11f.root",
				  const char *fileName11h	=	"BadChannels2011_11h.root",
				  const char *fileName12a	=	"BadChannels2012_12a.root",
				)

{

    gSystem->Load("libOADB");      
    //gSystem->Load("libOADB"); //comment if compiled
      
    //LHC10
    TObjArray array10(12);		array10.SetName("BadChannels10");
   
    //LHC11a
    TObjArray array11a(12);		array11a.SetName("BadChannels11a");

    //LHC11b
    TObjArray array11b(12);		array11b.SetName("BadChannels11b");
    
    //LHC11c
    TObjArray array11c1(12);		array11c1.SetName("BadChannels11c1");
    TObjArray array11c2(12);		array11c2.SetName("BadChannels11c2");
    TObjArray array11c3(12);		array11c3.SetName("BadChannels11c3");

    //LHC11d
    TObjArray array11d(12);		array11d.SetName("BadChannels11d");

    //LHC11e
    TObjArray array11e(12);		array11e.SetName("BadChannels11e");

    //LHC11f
    TObjArray array11f(12);		array11f.SetName("BadChannels11f");

    //LHC11h
    TObjArray array11h(12);		array11h.SetName("BadChannels11h");
    
    //LHC12a
    TObjArray array12a(12);		array12a.SetName("BadChannels12a");
    

    TFile	*f10	=	new TFile(fileName10,"read"); 
    TFile	*f11a	=	new TFile(fileName11a,"read");
    TFile	*f11b	=	new TFile(fileName11b,"read");
    TFile	*f11c1	=	new TFile(fileName11c1,"read");
    TFile	*f11c2	=	new TFile(fileName11c2,"read");
    TFile	*f11c3	=	new TFile(fileName11c3,"read");
    TFile	*f11d	=	new TFile(fileName11d,"read");
    TFile	*f11e	=	new TFile(fileName11e,"read");
    TFile	*f11f	=	new TFile(fileName11f,"read");
    TFile	*f11h	=	new TFile(fileName11h,"read");
    TFile	*f12a	=	new TFile(fileName11h,"read");

    //Create OADB container for BadChannels
    AliOADBContainer* con = new AliOADBContainer("AliEMCALBadChannels");

    char name[30];
    for (Int_t mod=0;mod<12;mod++){
	cout<<"SM "<< mod<<endl;
	
	if (mod<4)  {
	    sprintf(name,"EMCALBadChannelMap_Mod%d",mod);
	    cout<<"BadChannels2010:"<<name<<endl;
	    array10.Add(f10->Get(name));
	}
	    sprintf(name,"EMCALBadChannelMap_Mod%d",mod);
	    cout<<"BadChannels2011:"<<name<<endl;
	    array11a.Add(f11a->Get(name));
	    array11b.Add(f11b->Get(name));
	    array11c1.Add(f11c1->Get(name));
	    array11c2.Add(f11c2->Get(name));
	    array11c3.Add(f11c3->Get(name));
	    array11d.Add(f11d->Get(name));
	    array11e.Add(f11e->Get(name));
	    array11f.Add(f11f->Get(name));
	    array11h.Add(f11h->Get(name));
	    array12a.Add(f12a->Get(name));
	    
    } //mod
    
    con->AddDefaultObject(&array10);
    con->AddDefaultObject(&array11a);
    con->AddDefaultObject(&array11b);
    con->AddDefaultObject(&array11c1);
    con->AddDefaultObject(&array11c2);
    con->AddDefaultObject(&array11c3);
    con->AddDefaultObject(&array11d);
    con->AddDefaultObject(&array11e);
    con->AddDefaultObject(&array11f);
    con->AddDefaultObject(&array11h);
    con->AddDefaultObject(&array12a);
   
    
    //Establishing run number with the correct objects
    con->AppendObject(&array10,100000,140000);
    con->AppendObject(&array11a,144871,146860);
    con->AppendObject(&array11b,148531,150629);
    con->AppendObject(&array11c1,151636,153569);
    con->AppendObject(&array11c2,153570,154733);
    con->AppendObject(&array11c3,154734,155384);
    con->AppendObject(&array11d,156477,159635);
    con->AppendObject(&array11e,160670,162740);
    con->AppendObject(&array11f,162933,165746);
    con->AppendObject(&array11h,166529,170673);
    con->AppendObject(&array12a,177320,999999999);
    
    con->WriteToFile("BetaBadChannels.root");   
}

