void runAODFilterMC(const char *inputFile="AliESDs.root")
{
  // Local analysis example

      gSystem->Load("libTree");
      gSystem->Load("libPhysics");
      gSystem->Load("libGeom");
      gSystem->Load("libVMC");

      bool bKineFilter = true;
      for (const char *lib : {"STEERBase", "ESD", "AOD", "ANALYSIS", "ANALYSISalice"})
        gSystem->Load(Form("lib%s", lib));
      TChain *chain = new TChain("esdTree");
      chain->Add(inputFile);

      
      //
      if (gApplication) gApplication->InitializeGraphics();
      // Create the chain
      //


	/////////////////////////////////////////////////////////////////////////////////// 
	// Create the analysis manager
	//
	// Input 
      AliESDInputHandler* inpHandler = new AliESDInputHandler();
      // Output
      AliAODHandler* aodHandler = new AliAODHandler();
      aodHandler->SetOutputFileName("aod_ckb2.root");
      // MC Truth
      AliMCEventHandler* mcHandler = new AliMCEventHandler();
      AliAnalysisManager *mgr  = new AliAnalysisManager("Filter Manager", "Filter Manager");
     if(bKineFilter){
       mgr->SetMCtruthEventHandler(mcHandler);
     }

      mgr->SetInputEventHandler  (inpHandler);
      mgr->SetOutputEventHandler (aodHandler);
      aodHandler->Dump();

      mgr->SetDebugLevel(10);

      // Filtering of MC particles (decays conversions etc)
      // this task is also needed to set the MCEventHandler
      // to the AODHandler, this will not be needed when
      // AODHandler goes to ANALYSISalice
      AliAnalysisTaskMCParticleFilter *kinefilter = new AliAnalysisTaskMCParticleFilter("Particle Filter");
      if(bKineFilter)mgr->AddTask(kinefilter);
      
      
      // 
      AliESDtrackCuts* esdTrackCutsL = new AliESDtrackCuts("AliESDtrackCuts", "Loose");
      esdTrackCutsL->SetMinNClustersTPC(50);
      esdTrackCutsL->SetMaxChi2PerClusterTPC(3.5);
      esdTrackCutsL->SetMaxCovDiagonalElements(2,2,0.5,0.5,2);
      esdTrackCutsL->SetRequireTPCRefit(kTRUE);
      esdTrackCutsL->SetDCAToVertexZ(3.0);
      esdTrackCutsL->SetDCAToVertexXY(3.0);
      esdTrackCutsL->SetDCAToVertex2D(kTRUE);
      esdTrackCutsL->SetRequireSigmaToVertex(kFALSE);
      esdTrackCutsL->SetAcceptKinkDaughters(kFALSE);

      AliESDtrackCuts* esdTrackCutsITSsa = new AliESDtrackCuts("AliESDtrackCuts", "ITS stand-alone");
      esdTrackCutsITSsa->SetRequireITSStandAlone(kTRUE);
      
      AliAnalysisFilter* trackFilter = new AliAnalysisFilter("trackFilter");
      trackFilter->AddCuts(esdTrackCutsL);
      trackFilter->AddCuts(esdTrackCutsITSsa);
      
      AliAnalysisTaskESDfilter *esdfilter = new AliAnalysisTaskESDfilter("ESD Filter");
      esdfilter->SetTrackFilter(trackFilter);
      
      mgr->AddTask(esdfilter);
    
    
      //
      // Create containers for input/output
      AliAnalysisDataContainer *cinput1 = mgr->GetCommonInputContainer();
      AliAnalysisDataContainer *coutput1 = mgr->GetCommonOutputContainer();
      
      coutput1->SetSpecialOutput();

      if(bKineFilter){
	mgr->ConnectInput  (kinefilter,     0, cinput1  );
	mgr->ConnectOutput (kinefilter,     0, coutput1 );
	AliAnalysisDataContainer *coutputEx = mgr->CreateContainer("cFilterList", TList::Class(),
								   AliAnalysisManager::kOutputContainer,"pyxsec_hists.root");
	mgr->ConnectOutput (kinefilter,     1, coutputEx );
      }

	mgr->ConnectInput  (esdfilter,     0, cinput1  );
	mgr->ConnectOutput (esdfilter,     0, coutput1 );
      
      //
      // Run the analysis
      //    
      mgr->InitAnalysis();
      mgr->PrintStatus();
      mgr->StartAnalysis("local",chain,10000);

}

