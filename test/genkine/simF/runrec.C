void runrec(){
  gSystem->Load("libzmq");
  gSystem->Load("libpythia6");
  gROOT->Macro("rec.C");
}
