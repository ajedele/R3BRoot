void LosCal2HitPar(const int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	float fRunId = (float)RunId;
	int nev =1e9;

        //Filename and path	
	TString path1 = "/lustre/land/ajedele/s473/rootfiles/";
	TString path3 = "/u/ajedele/R3BRoot/";

	TString filename; TString filename1; TString outputFileName;

	TString inputpar, outputpar;
	// Select the calibration stage  ---------------------------
	// 1) fParameter=1
	//    - Determine ToT offset between top and bottom PMT
	//    - Determine sync offset between paddles
	//    - Determine ToF offset with respect to the start detector 
	// 
	// ODO: Walk correction should be implemented here, before the next steps
	// 
	// 2) fParameter=2 (this requires Y != 0)
	//    - Determine effective speed of light in [cm/s] for each paddle
	//    - Determine light attenuation lambda for each paddle
	// 
	// 3) fParameter=3
	//    - Calculation of position dependence for the charge/energy
	// 
	// 4) fParameter=4
	//    - Charge calibration in Z units
	 
	 
	int fParameter=4;
	TString cParameter = Form("%02d", fParameter);
	inputpar = "";
	outputpar = Form("%sparameter/los_HitPar_s473_191.root", path3.Data());
	filename  = Form("%sLosTCal0190_Test.root", path1.Data());
	if (RunId = 191 || RunId == 190) filename1 = Form("%sLosTCal0191_Test.root", path1.Data());
	outputFileName = Form("%sLosHitPar0191_Test.root", path1.Data());

	// R3BStandard ------------------------------------------------
	bool fLos = true;
	bool fLos = false;
	
	// Create analysis run ---------------------------------------- 
	FairRunAna* run = new FairRunAna();
		printf("Filename: %s\n",filename.Data());	

	// Set Up Header ----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);
		printf("Filename: %s\n",filename.Data());	

	//Create source using root file for input ---------------------
	R3BFileSource *source = new R3BFileSource(filename);
	if (RunId==191 || RunId==190) source->AddFile(filename1);
	source->SetRunId(RunId);
	run->SetSource(source);
	run->SetSink(new FairRootFileSink(outputFileName.Data())); 
		printf("Filename: %s\n",filename.Data());	

	//Runtime database --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();

		printf("Filename: %s\n",filename.Data());	
	//Bool_t kParameterMerged = kFALSE;
	FairParRootFileIo* parIo1 = new FairParRootFileIo(kFALSE);
	TList *parList = new TList();
	parList->Add(new TObjString("parameter/LosTCal0191.root"));

	if (fLos) {parList->Add(new TObjString("../los/loscalpar_v66.root"));}
	if(fParameter>1 || fParameter ==-3) {parList->Add(new TObjString(inputpar.Data()));}
	parIo1->open(parList);
	rtdb->setFirstInput(parIo1);
	
		printf("Filename: %s\n",filename.Data());	
	//Ascii file
	FairParAsciiFileIo *parIo2 = new FairParAsciiFileIo();
	TList *parList2 = new TList();
	parList2->Add(new TObjString(path3 + "parameter/los_mapping_v1.par"));
	parIo2->open(parList2, "in");
	rtdb->setSecondInput(parIo2);
	rtdb->addRun(RunId);
	rtdb->getContainer("LosTCalPar");
	rtdb->setInputVersion(fRunId, (char*)"LosTCalPar", 1, 1);
	if (fLos) {
		rtdb->getContainer("LosTCalPar");
		rtdb->setInputVersion(RunId, (char *)"LosTCalPar", 1, 1);
	}
	if(fParameter>1 || fParameter==-3)
	{
		rtdb->getContainer("losHitPar");
		rtdb->setInputVersion(RunId, (char*)"losHitPar", 1, 1);
	}
	rtdb->print();
		printf("Filename: %s\n",filename.Data());	

	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	int minStats = 1000;

	R3BLosCal2HitPar* losCal2HitPar = new R3BLosCal2HitPar();
	losCal2HitPar->SetMinStats(minStats);
	losCal2HitPar->SetTrigger(1);
	losCal2HitPar->SetTPat(1);

	run->AddTask(losCal2HitPar);

	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");

	// Run --------------------------------------------------------- 
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);

	if (fParameter != 3 && fParameter != 4)
	{
		printf("I'm in outputparams\n");	
		auto* parOut = new FairParRootFileIo(kTRUE);
		parOut->open(outputpar.Data());
		rtdb->setOutput(parOut);
		rtdb->saveOutput();
	} 

	// ------------------------------------------------------------- 
	printf("It's the end\n");
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is" << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}
