void SetupAnalysis(TString mode,
		   TString analysisMode="full",
		   Bool_t useMC = kFALSE,
		   Int_t nEvents=1.0*1e9, 
		   Int_t nEventsSkip=0,
		   TString format="esd")
{
  
  // ALICE stuff
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) mgr = new AliAnalysisManager("CAF train");
  
  // Create and configure the alien handler plugin 
  gROOT->LoadMacro("CreateAnalysisPlugin.C"); 
  AliAnalysisGrid *alienHandler = CreateAnalysisPlugin(analysisMode);   
  if (!alienHandler) return;
  mgr->SetGridHandler(alienHandler);
  
  // input handler for esd or AOD, real or MC data
  InputHandlerSetup(format,useMC);

  // physics selection
  if(!format.CompareTo("esd")){
    gROOT->LoadMacro("$ALICE_ROOT/OADB/macros/AddTaskPhysicsSelection.C");
    AliPhysicsSelectionTask* physSelTask = AddTaskPhysicsSelection(kFALSE);
    if(useMC) physSelTask->GetPhysicsSelection()->SetAnalyzeMC();   
    AliPhysicsSelection* physSel = physSelTask->GetPhysicsSelection();
    physSel->AddBackgroundIdentification(new AliBackgroundSelection());
  }
  
  gROOT->ProcessLine(Form(".include %s/include",gSystem->ExpandPathName("$ALICE_ROOT")));
  
  gROOT->LoadMacro("AliAnalysisTaskEfficiency.cxx+g");
  
  // load and run AddTask macro
  gROOT->LoadMacro("AddTaskEfficiency.C");
  

  AliAnalysisTaskSE* task1 = AddTaskEfficiency(-1);
  if(!task1){
    Printf("AddTask could not be run.");
  }

  // Run analysis
  mgr->InitAnalysis();
  
  if (!mode.CompareTo("local")) {
    mgr->StartAnalysis(mode.Data(),nEvents,nEventsSkip);
  }
  else {
    mgr->StartAnalysis(mode.Data());
    
  }
  
}


Bool_t InputHandlerSetup(TString format = "esd", Bool_t useKine = kTRUE)
{
  format.ToLower();

  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();

  AliAnalysisDataContainer *cin = mgr->GetCommonInputContainer();

  if (cin) return;

  if (!format.CompareTo("esd"))
    {
      AliESDInputHandler *esdInputHandler = 
	dynamic_cast<AliESDInputHandler*>(AliAnalysisManager::GetAnalysisManager()->
					  GetInputEventHandler());

      if (!esdInputHandler)
	{
	  Info("CustomAnalysisTaskInputSetup", "Creating esdInputHandler ...");
	  esdInputHandler = new AliESDInputHandler();
	  mgr->SetInputEventHandler(esdInputHandler);
	}

      if (useKine)
	{
	  AliMCEventHandler* mcInputHandler = 
	    dynamic_cast<AliMCEventHandler*>(AliAnalysisManager::GetAnalysisManager()->
					     GetMCtruthEventHandler());

	  if (!mcInputHandler)
	    {
	      Info("CustomAnalysisTaskInputSetup", "Creating mcInputHandler ...");
	      AliMCEventHandler* mcInputHandler = new AliMCEventHandler();
	      mgr->SetMCtruthEventHandler(mcInputHandler);
	    }
	}

    }
  else if (!format.CompareTo("aod"))
    {
      AliAODInputHandler *aodInputHandler = 
	dynamic_cast<AliAODInputHandler*>(AliAnalysisManager::GetAnalysisManager()->
					  GetInputEventHandler());

      if (!aodInputHandler)
	{
	  Info("CustomAnalysisTaskInputSetup", "Creating aodInputHandler ...");
	  aodInputHandler = new AliAODInputHandler();
	  mgr->SetInputEventHandler(aodInputHandler);
	}
    }
  else
    {
      AliWarning("Wrong input format!!! Only ESD and AOD are supported. Skipping Task ...");
      return kFALSE;
    }

  return kTRUE;
}
