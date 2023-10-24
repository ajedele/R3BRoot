void PspxCal2HitPar(int RunId);

void PspxCal2HitPar()
{
	PspxCal2HitPar(194); //non-empty target runs E=800 MeV/u  
}

void PspxCal2HitPar(int RunId)
{
	//int numExpt = 444;      //number of detectors
	int numExpt = 473;      //number of detectors
	
	TString runNumber1 = Form("%04d",RunId);
	TStopwatch timer;
	timer.Start();
	
	const int nev = 1e9; //Number of events to read, -1 - until CTRC+C

	TString path = Form("/lustre/land/ajedele/s%03d/rootfiles/",numExpt);
	
	//Create source using ucesb for input ----------------
	TString filename = path + "Test_Poscal2Cal" + runNumber1 + ".root ";
	TString outputFileName = path + "Test_Cal2HitPar" + runNumber1 + ".root";

	/* Create analysis run ---------------------------------------- */
	FairRunAna* run = new FairRunAna();

	//Set up Header -----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);
	
	//Create source -----------------------------------------------
	TString runNo;
	R3BFileSource *source = new R3BFileSource(filename);
	source->SetRunId(RunId);
	run->SetSource(source); 
	run->SetSink(new FairRootFileSink(outputFileName)); 

	/* Runtime data base ------------------------------------------ */  
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	
	/* Calibrate Pspx --------------------------------------------- */
	const int numCalib = 4;      //number of calibration points expected
	const int numDet = 6;        //number of detectors 
	const int numStrips = 32;    //number of strips
	const int calibpar = 0;      //calibration number - how many energies were sampled?
	//TString fParOutName = Form("PspxCal2HitPar%d-%d.par", RunId, RunId2);
	TString fParOutName = Form("PspxCal2HitEPar%d.par", RunId);
	TString fParOutName1 = Form("PspxCal2HitPar%d.par", RunId);

	std::vector<double> maxenergy = {300000., 18000., 450000., 26000., 450000., 25000., 
                260000., 14000., 390000., 23000., 380000., 21000., 
                240000., 13000., 370000., 21000., 350000., 20000., 
                230000., 12000., 340000., 19000., 330000., 18000.};	
	std::vector<double> maxstrip = {12.,17.,12.,17.,17.,19.,17.,19.,17.,19.,17.,19.};
	
	R3BPspxCal2HitPar* pspxCalibrator = new R3BPspxCal2HitPar("R3BPspxCal2HitPar", 1);
	pspxCalibrator->SetNumCalib(numDet);
	pspxCalibrator->SetNumDet(numDet);
	pspxCalibrator->SetNumStrips(numStrips);
	pspxCalibrator->SetCalibPar(calibpar);
	pspxCalibrator->SetMaxStrip(maxstrip);
	pspxCalibrator->SetMaxEnergy(maxenergy);
	pspxCalibrator->SetParOutName(fParOutName);
	pspxCalibrator->SetParOutName1(fParOutName1);
	run->AddTask(pspxCalibrator);
	
	/* Initialize -------------------------------------------------- */
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	/* ------------------------------------------------------------- */
	
	/* Run --------------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	rtdb1->saveOutput();
	/* ------------------------------------------------------------- */
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is" << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;
}
