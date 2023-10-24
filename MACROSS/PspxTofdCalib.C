void PspxTofdCalib(int RunId);

void PspxTofdCalib()
{
	PspxTofdCalib(187);
}

void PspxTofdCalib(int RunId)
{
	TString runNumber = Form("%04d",RunId);
	TStopwatch timer;
	timer.Start();

	int nev=2e9;	
	//int nev=1;	
	TString path = "/lustre/land/ajedele/s473/rootfiles/";
	
	//Create source using ucesb for input ----------------
	//TString filename = path + "Test_Poscal2Cal" + runNumber + ".root ";
	TString filename = path + "PspxTofdLosUnpack" + runNumber + ".root ";
	TString outputFileName = path + "PspxTofdCalib" + runNumber + ".root";

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

	TString fParInName = Form("ParTest0194-0201.txt");

        R3BPspxPoscal2Cal* pspxPoscal2Cal = new R3BPspxPoscal2Cal("R3BPspxPoscal2Cal", 1);
        pspxPoscal2Cal->SetNumDet(numDet);
        pspxPoscal2Cal->SetNumStrips(numStrips);
        pspxPoscal2Cal->SetParInName(fParInName);
        run->AddTask(pspxPoscal2Cal);

	TString fParName  = Form("PspxCal2HitParEPos.par");
	TString fParInName1 = Form("PspXCal2HitPar0184-0193.par");
	TString fParInName2 = Form("PspXCal2HitPar0211-0212.par");
	TString fParInName3 = Form("PspXCal2HitPar0220.par");
	TString fParInName4 = Form("PspXCal2HitPar0229.par");
        TString	fParOutName = Form("PspxEnergyCalibPar.par");
        TString	fParOutName2 = Form("PspxEnergyMatchPar.par");
	
	R3BPspxCal2Hit* pspxCal2Hit = new R3BPspxCal2Hit("R3BPspxCal2Hit", 1);
	pspxCal2Hit->SetRunTag(runtag);
	pspxCal2Hit->SetNumDet(numDet);
	pspxCal2Hit->SetNumStrips(numStrips);
	pspxCal2Hit->SetParOutName(fParOutName);
	pspxCal2Hit->SetParOutName2(fParOutName2);
	pspxCal2Hit->SetParName(fParName);
	pspxCal2Hit->SetParInName1(fParInName1);
	pspxCal2Hit->SetParInName2(fParInName2);
	pspxCal2Hit->SetParInName3(fParInName3);
	pspxCal2Hit->SetParInName4(fParInName4);
	//pspxCal2Hit->SetParInName5(fParInName5);
	run->AddTask(pspxCal2Hit);
	
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
