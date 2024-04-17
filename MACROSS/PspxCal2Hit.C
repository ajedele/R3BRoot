void PspxCal2Hit(int RunId);

void PspxCal2Hit()
{
	PspxCal2Hit(187);
}

void PspxCal2Hit(int RunId)
{
	TString runNumber = Form("%04d",RunId);
	TStopwatch timer;
	timer.Start();

	int nev=2e9;	
	//int nev=1;	
	TString path = "/lustre/land/ajedele/s473/rootfiles/";
	
	//Create source using ucesb for input ----------------
	//TString filename = path + "Test_Poscal2Cal" + runNumber + ".root ";
	TString filename = path + "PspxLosUnpack" + runNumber + ".root ";
	TString outputFileName = path + "PspxCal2Hit" + runNumber + ".root";

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

	//Add Header Copy ---------------------------------------------- 
	R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
	run->AddTask(RunIdTask);

	/* Calibrate Pspx --------------------------------------------- */
	int runtag;
        if (RunId<203) runtag = 0;          
	else if (RunId<213) runtag = 1;          
	else if (RunId<221) runtag = 2;          
	else if (RunId<230) runtag = 3;          
	const int numDet = 6;         //number of detectors 
	const int numStrips = 32;      //number of detectors
	TString fParName  = Form("PspxCal2HitParEPos.par");
	TString fParInName1 = Form("PspXCal2HitPar0184-0193.par");
	TString fParInName2 = Form("PspXCal2HitPar0211-0212.par");
	TString fParInName3 = Form("PspXCal2HitPar0220.par");
	TString fParInName4 = Form("PspXCal2HitPar0229.par");
        TString	fParOutName = Form("PspxEnergyCalibPar.par");
        TString	fParOutName2 = Form("PspxEnergyMatchPar.par");
	
	R3BPspxCal2Hit* pspxCalibrator = new R3BPspxCal2Hit("R3BPspxCal2Hit", 1);
	pspxCalibrator->SetRunTag(runtag);
	pspxCalibrator->SetNumDet(numDet);
	pspxCalibrator->SetNumStrips(numStrips);
	pspxCalibrator->SetParOutName(fParOutName);
	pspxCalibrator->SetParOutName2(fParOutName2);
	pspxCalibrator->SetParName(fParName);
	pspxCalibrator->SetParInName1(fParInName1);
	pspxCalibrator->SetParInName2(fParInName2);
	pspxCalibrator->SetParInName3(fParInName3);
	pspxCalibrator->SetParInName4(fParInName4);
	//pspxCalibrator->SetParInName5(fParInName5);
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
