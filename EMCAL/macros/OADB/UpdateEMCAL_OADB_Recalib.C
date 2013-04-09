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

TObjArray array12_iter8(10); // 2012--2013 ---> Same R.F. for both 2012 and 1013 pass1.
array12_iter8.SetName("Recalib");

char name[30];
// Filling The objects above with the EMCALRecalFactors_SM Histos:
for (Int_t mod=0;mod<10;mod++){
    cout<<"SM "<< mod<<endl;
    // Recalib Objects for 2012: Still using 10 SM's
    sprintf(name,"EMCALRecalFactors_SM%d",mod);
    cout<<"Recalib2012 and 2013:"<<name<<endl;
    array12_iter8.Add(f12->Get(name));
	    
    } //mod
//********************************************************************

// ************** Establishing different configuration according to the pass ***********
// ************ Latest Tender Information : (September 3rd 2011) AliRootTrunk revision 51405 ***********************

// ******* 2012 -- 2013 ******************
// Setting Periods
TObjArray array12_13x(10);	array12_13x.SetName("LHC12_13x"); 
TObjArray array12_13_pass1(10); array12_13_pass1.SetName("pass1");  array12_13_pass1.Add(&array12_iter8);

//**** Adding pass object to period Object ****/
array12_13x.Add(&array12_13_pass1);
// *****************
//2012
// Adding objects to the Container
con->AddDefaultObject((TObject*)&array12_13x);

// Appending objects to their specific Run number
con->AppendObject(&array12_13x,176326,197692);  //LHC12x--13x runs

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
