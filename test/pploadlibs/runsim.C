void runsim(){
  gSystem->Load("libzmq");
  gSystem->Load("liblhapdf");
  gSystem->Load("libpythia6");
  gSystem->Load("libVMC");
  gSystem->Load("libgeant321");

  gROOT->Macro("sim.C");
}
