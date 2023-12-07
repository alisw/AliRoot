Int_t n = 100; // This makes n "visible" to the compiled macro
void runsim(Int_t nev = 1){
  gSystem->Load("libzmq");
  gSystem->Load("liblhapdf");      // Parton density functions
  gSystem->Load("libEGPythia6");   // TGenerator interface
  gSystem->Load("libpythia6");     // Pythia
  gSystem->Load("libAliPythia6");  // ALICE specific implementations
  // Some gymnastics to find the g4lib.C macro
  TString g4vmcinstall(gSystem->Getenv("G4VMCINSTALL"));
  TObjArray * toks = g4vmcinstall.Tokenize("/");
  TObjString * strtag = (TObjString*)toks->At(toks->GetEntries()-1);
  TString stag = strtag->GetString();
  stag.Remove(stag.Last('-'));
  stag.Remove(stag.First('v'),1);
  stag.Replace(stag.First('-'),1,'.');
  stag.ReplaceAll("-p",".");
  g4vmcinstall += "/share/Geant4VMC-";
  g4vmcinstall += stag;
  g4vmcinstall += "/examples/macro/g4libs.C";
  // Additional libs on MacOSX
  if(strstr(gSystem->GetBuildArch(),"macos")) {
    gSystem->Load("libxerces-c.dylib");
  }
  gROOT->Macro(g4vmcinstall.Data());
  //
  n = nev;
  gROOT->Macro("sim.C(n)");
}
