#ifdef __CLING__
R__LOAD_LIBRARY(libzmq)
#endif
void rec() {
  gSystem->Load("libpythia6.so");
  AliReconstruction reco;

  reco.SetWriteESDfriend();
  reco.SetWriteAlignmentData();

  reco.SetDefaultStorage("local://$ALIROOT_OCDB_ROOT/OCDB");
  reco.SetSpecificStorage("GRP/GRP/Data",
			  Form("local://%s",gSystem->pwd()));
  reco.SetRunPlaneEff(kTRUE);

  reco.SetRunQA("ALL:ALL") ;
  
  reco.SetQARefDefaultStorage("local://$ALICE_ROOT/QAref") ;
  
  for (Int_t det = 0 ; det < AliQA::kNDET ; det++) {
    reco.SetQACycles((AliQAv1::DETECTORINDEX_t)det, 999) ;
    reco.SetQAWriteExpert((AliQAv1::DETECTORINDEX_t)det) ; 
  }
  
  TStopwatch timer;
  timer.Start();
  reco.Run();
  timer.Stop();
  timer.Print();
}
