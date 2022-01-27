void runrec(){
  gSystem->Load("libzmq");
  gROOT->Macro("rec.C");
}
