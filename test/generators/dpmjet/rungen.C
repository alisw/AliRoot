Int_t n = 100; // This makes n "visible" to the compiled macro
void rungen(Int_t nev=1){
  // Simulation and reconstruction
  TStopwatch timer;
  timer.Start();
  gSystem->SetIncludePath("-I$ROOTSYS/include -I$ALICE_ROOT/include -I$ALICE_ROOT -I$ALICE_ROOT/EVGEN");
  gSystem->Load("libDPMJET");      // Parton density functions
  n = nev; // Use the requested number of events
  gROOT->Macro("fastGen.C++(n)");
  timer.Stop();
  timer.Print();
}
