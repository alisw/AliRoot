#ifdef __CLING__
  R__LOAD_LIBRARY(libDPMJET) // DPMJET, PhoJet and Pythia6115 library
  R__LOAD_LIBRARY(liblhapdf) // Parton density functions
  R__LOAD_LIBRARY(libpythia6) // Parton density functions
  R__LOAD_LIBRARY(libEGPythia6) // TGenerator interface
  R__LOAD_LIBRARY(libgeant321)

  R__LOAD_LIBRARY(libAliPythia6) // ALICE specific implementations
  R__LOAD_LIBRARY(libTDPMjet) // DPMJET interface
#endif

void sim(Int_t nev=5) {
  // Libraries required by the simulation

  AliSimulation simulator;
  simulator.SetMakeSDigits("TRD TOF PHOS HMPID EMCAL MUON FMD ZDC PMD T0 VZERO");
  simulator.SetMakeDigitsFromHits("ITS TPC");
  simulator.SetWriteRawData("ALL","raw.root",kTRUE);

  simulator.SetDefaultStorage("local://$ALIROOT_OCDB_ROOT/OCDB");
  simulator.SetSpecificStorage("GRP/GRP/Data",
			       Form("local://%s",gSystem->pwd()));
 
  simulator.SetRunHLT("default"); // In case we do not have ancored production

  TStopwatch timer;
  timer.Start();
  simulator.Run(nev);
  timer.Stop();
  timer.Print();
}
