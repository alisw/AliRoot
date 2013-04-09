//This a modification in a code sent by Marco Bregant, which, originally, created the OADB for misalignment matrices

//In this macro, the histograms with Recalibraton Factors are loaded and some TObjarrays are filled with these histograms.
// At the end, a OADB container is created receiving these arrays.
// This UpdateEMCAL_OADB_Recalib updates the information of a original OADB file and writes the output to BetaRecalib.root


void UpdateEMCAL_OADB_Recalib(const char *fileNameOADB="$ALICE_ROOT/OADB/EMCAL/EMCALRecalib.root",
			      const char *fileName12="RecalDB/RecalibrationFactors2012_10SM_iter8.root")
{

gSystem->Load("libOADB");  

AliOADBContainer *con	= new AliOADBContainer("");
con->InitFromFile(fileNameOADB, "AliEMCALRecalib"); //Updating the original OADB file, output will be written into BetaRecalib.root 

// **** Loading the root files with Recalibration Factors:
TFile* f12=new TFile(fileName12);

TObjArray array12_13(10); // 2012--2013 ---> Same R.F. for both 2012 and 1013 pass1.
array12_13.SetName("Recalib");

char name[30];
// Filling The objects above with the EMCALRecalFactors_SM Histos:
for (Int_t mod=0;mod<10;mod++){
    cout<<"SM "<< mod<<endl;
    // Recalib Objects for 2012: Still using 10 SM's
    sprintf(name,"EMCALRecalFactors_SM%d",mod);
    cout<<"Recalib2012 and 2013:"<<name<<endl;
    array12_13.Add(f12->Get(name));
	    
    } //mod
//********************************************************************

// ************** Establishing different configuration according to the pass ***********
// ************ Latest Tender Information : (September 3rd 2011) AliRootTrunk revision 51405 ***********************

// ******* 2012 -- 2013 ******************
// Setting Periods
TObjArray array12a(10); array12a.SetName("LHC12a"); 
TObjArray array12b(10); array12b.SetName("LHC12b");     
TObjArray array12c(10); array12c.SetName("LHC12c");     
TObjArray array12d(10); array12d.SetName("LHC12d");     
TObjArray array12e(10); array12e.SetName("LHC12e");     
TObjArray array12f(10); array12f.SetName("LHC12f");     
TObjArray array12g(10); array12g.SetName("LHC12g");     
TObjArray array12h(10); array12h.SetName("LHC12h");     
TObjArray array12i(10); array12i.SetName("LHC12i");     
// // TObjArray array13a(10);   // No runs reconstructed for 13a in RCT  
TObjArray array13b(10); array13b.SetName("LHC13b");          
TObjArray array13c(10); array13c.SetName("LHC13c");          
TObjArray array13d(10); array13d.SetName("LHC13d");
TObjArray array13e(10); array13e.SetName("LHC13e");         
TObjArray array13f(10); array13f.SetName("LHC13f");     
TObjArray array13g(10); array13g.SetName("LHC13g");     

// Establishing pass objects and adding recalib histos array to them.
TObjArray array12a_pass1(10); array12a_pass1.SetName("pass1");  array12a_pass1.Add(&array12_13);
TObjArray array12b_pass1(10); array12b_pass1.SetName("pass1");  array12b_pass1.Add(&array12_13);    
TObjArray array12c_pass1(10); array12c_pass1.SetName("pass1");  array12c_pass1.Add(&array12_13);    
TObjArray array12d_pass1(10); array12d_pass1.SetName("pass1");  array12d_pass1.Add(&array12_13);    
TObjArray array12e_pass1(10); array12e_pass1.SetName("pass1");  array12e_pass1.Add(&array12_13);    
TObjArray array12f_pass1(10); array12f_pass1.SetName("pass1");  array12f_pass1.Add(&array12_13);    
TObjArray array12g_pass1(10); array12g_pass1.SetName("pass1");  array12g_pass1.Add(&array12_13);    
TObjArray array12h_pass1(10); array12h_pass1.SetName("pass1");  array12h_pass1.Add(&array12_13);
TObjArray array12i_pass1(10); array12i_pass1.SetName("pass1");  array12i_pass1.Add(&array12_13);

// TObjArray array13a_pass1(10);   // No runs reconstructed for 13a in RCT  
TObjArray array13b_pass1(10); array13b_pass1.SetName("pass1");  array13b_pass1.Add(&array12_13);
TObjArray array13c_pass1(10); array13c_pass1.SetName("pass1");  array13c_pass1.Add(&array12_13);
TObjArray array13d_pass1(10); array13d_pass1.SetName("pass1");  array13d_pass1.Add(&array12_13);
TObjArray array13e_pass1(10); array13e_pass1.SetName("pass1");  array13e_pass1.Add(&array12_13);
TObjArray array13f_pass1(10); array13f_pass1.SetName("pass1");  array13f_pass1.Add(&array12_13);
TObjArray array13g_pass1(10); array13g_pass1.SetName("pass1");  array13g_pass1.Add(&array12_13);

// 13b pass2:
TObjArray array13b_pass2(10); array13b_pass2.SetName("pass2");  array13b_pass2.Add(&array12_13);


//**** Adding pass object to period Object ****/
array12a.Add(&array12a_pass1);
array12b.Add(&array12b_pass1);
array12c.Add(&array12c_pass1);
array12d.Add(&array12d_pass1);
array12e.Add(&array12e_pass1);
array12f.Add(&array12f_pass1);
array12g.Add(&array12g_pass1);
array12h.Add(&array12h_pass1);
array12i.Add(&array12i_pass1);
//
array13b.Add(&array13b_pass1);
array13c.Add(&array13c_pass1);
array13d.Add(&array13d_pass1);
array13e.Add(&array13e_pass1);
array13f.Add(&array13f_pass1);
array13g.Add(&array13g_pass1);
//
array13b.Add(&array13b_pass2);

// *****************
//2012
// Adding objects to the Container
con->AddDefaultObject((TObject*)&array12a);
con->AddDefaultObject((TObject*)&array12b);
con->AddDefaultObject((TObject*)&array12c);
con->AddDefaultObject((TObject*)&array12d);
con->AddDefaultObject((TObject*)&array12e);
con->AddDefaultObject((TObject*)&array12f);
con->AddDefaultObject((TObject*)&array12g);
con->AddDefaultObject((TObject*)&array12h);
con->AddDefaultObject((TObject*)&array12i);
//2013
con->AddDefaultObject((TObject*)&array13b);
con->AddDefaultObject((TObject*)&array13c);
con->AddDefaultObject((TObject*)&array13d);
con->AddDefaultObject((TObject*)&array13e);
con->AddDefaultObject((TObject*)&array13f);
con->AddDefaultObject((TObject*)&array13g);
//
// Appending objects to their specific Run number
con->AppendObject(&array12a,176326,177295);
con->AppendObject(&array12b,177381,178220);
con->AppendObject(&array12c,179569,182744);
con->AppendObject(&array12d,183913,186320);
con->AppendObject(&array12e,186365,186602);
con->AppendObject(&array12f,186668,188123);
con->AppendObject(&array12g,188356,188503);
con->AppendObject(&array12h,189122,192732);
con->AppendObject(&array12i,192745,193766);
//
con->AppendObject(&array13b,195344,195483);
con->AppendObject(&array13c,195529,195677);
con->AppendObject(&array13d,195681,195873);
con->AppendObject(&array13e,195935,196311);
con->AppendObject(&array13f,196433,197388);
con->AppendObject(&array13g,197470,197692);

con->WriteToFile("BetaRecalib.root");

test(); // If someone wants to test container
}

void test(){
//
// let's read back the file
AliOADBContainer *cont=new AliOADBContainer("");
cont->InitFromFile("BetaRecalib.root", "AliEMCALRecalib");
// 
cout<<"_________--------------- dump ---------------------___________"<<endl;
cont->Dump();
cout<<"_________--------------- list ---------------------___________"<<endl;
//cont0.List();
cout<<"cont->GetDefaultList()->Print()"<<endl;
cont->GetDefaultList()->Print();

TObjArray *recal=cont->GetObject(197388); //GetObject(int runnumber)
recal->ls();

TObjArray *recalpass=recal->FindObject("pass1");

if(!recalpass){
  cout<<" norecalpass"<<endl;
  return;
}

TObjArray *recalib=recalpass->FindObject("Recalib");

if(!recalib){
  cout<<" no recalib found"<<endl;
  return;
}

TH2F *h2=(TH2F*)recalib->FindObject("EMCALRecalFactors_SM0");
if(!h2){
  return;
cout<<" no histo found"<<endl;
}
h2->DrawCopy("colz");
cout<<"That's all folks!"<<endl;

  
}
