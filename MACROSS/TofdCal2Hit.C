void TofdCal2Hit(const int RunId, int nev)
{
	TString runNumber1 = Form("%04d",RunId);
	float fRunId = (float)RunId;
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	TString path2 = "/lustre/land/ajedele/s473/rootfiles/TOFD";
	TString path3 = "/u/ajedele/R3BRoot/";

	//Create source using ucesb for input ----------------
	TString inputpar; TString filename; TString outputFileName;
	filename = Form("%sTofdTCal%04d_Test.root",path2.Data(),RunId);
	outputFileName = Form("%sTofdHit%04d_Test.root",path2.Data(),RunId);

	inputpar = Form("%s/parameter/tofd_HitPar_s473_meander221.root",path3.Data());

	//R3BStandard -------------------------------------------------
	bool fTofD = true;
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
        parList->Add(new TObjString("parameter/TofdTCal0191.root"));

        if (fLos) {parList->Add(new TObjString("../los/loscalpar_v66.root"));}
        parList->Add(new TObjString(inputpar.Data()));
        parIo1->open(parList);
        rtdb->setFirstInput(parIo1);

        //Ascii file
        FairParAsciiFileIo *parIo2 = new FairParAsciiFileIo();
        TList *parList2 = new TList();
        parList2->Add(new TObjString(path3 + "parameter/tofd_mapping_v1.par"));
        parIo2->open(parList2, "in");
        rtdb->setSecondInput(parIo2);
        rtdb->addRun(RunId);
        rtdb->getContainer("TofdTCalPar");
        rtdb->setInputVersion(fRunId, (char*)"TofdTCalPar", 1, 1);
        if (fLos) {
                rtdb->getContainer("LosTCalPar");
                rtdb->setInputVersion(RunId, (char *)"LosTCalPar", 1, 1);
        }
        rtdb->getContainer("tofdHitPar");
        rtdb->setInputVersion(RunId, (char*)"tofdHitPar", 1, 1);
        rtdb->print();

	
	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	const int minStats = 100;

	R3BTofDCal2Hit* tofdCal2Hit = new R3BTofDCal2Hit("TofdTCal", 1);
	tofdCal2Hit->SetNofModules(4,44);
	tofdCal2Hit->SetTofdQ(50);
	tofdCal2Hit->SetTofdTotPos(true);
	tofdCal2Hit->SetTpat(1,2);
	run->AddTask(tofdCal2Hit);

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

