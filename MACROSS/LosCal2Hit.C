void LosCal2Hit(const int RunId, int nev)
{
	TString runNumber1 = Form("%04d",RunId);
	float fRunId = (float)RunId;
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	TString path2 = "/lustre/land/ajedele/s473/rootfiles/";
	TString path3 = "/u/ajedele/R3BRoot/";

	//Create source using ucesb for input ----------------
	TString inputpar; TString filename; TString outputFileName;
	filename = Form("%sLosTCal%04d_Test.root",path2.Data(),RunId);
	outputFileName = Form("%sLosHit%04d_Test.root",path2.Data(),RunId);

	inputpar = Form("%s/parameter/los_HitPar_s473_meander221.root",path3.Data());

	//R3BStandard -------------------------------------------------
	bool fLos = false;

	// Create analysis run ---------------------------------------- 
	FairRunAna* run = new FairRunAna();
	
	// Set Up Header
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	R3BFileSource *source = new R3BFileSource(filename);
	source->SetRunId(RunId);
	run->SetSource(source);
	run->SetSink(new FairRootFileSink(outputFileName.Data()));

	//Runtime database --------------------------------------------
        FairRuntimeDb* rtdb = run->GetRuntimeDb();

        //Bool_t kParameterMerged = kFALSE;
        FairParRootFileIo* parIo1 = new FairParRootFileIo(kFALSE);
        TList *parList = new TList();
        parList->Add(new TObjString("parameter/LosTCal0221.root"));
        rtdb->addRun(RunId);
       
       	rtdb->getContainer("LosTCalPar");
        rtdb->setInputVersion(fRunId, (char*)"LosTCalPar", 1, 1);
        
	rtdb->getContainer("losHitPar");
        rtdb->setInputVersion(RunId, (char*)"losHitPar", 1, 1);
        rtdb->print();

	
	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	const int minStats = 100;

	R3BLosCal2Hit* losCal2Hit = new R3BLosCal2Hit("LosTCal", 1);
	run->AddTask(losCal2Hit);

	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);

	// ------------------------------------------------------------- 
	printf("It's the end\n");
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}

