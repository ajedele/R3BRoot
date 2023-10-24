void PspxTofdTest(const int RunId, double pspx_energy)
{
	TString runNumber = Form("%04d",RunId);
	TStopwatch timer;
	timer.Start();

	float fRunId = (float)RunId;

	TString path = "/lustre/land/ajedele/s473/rootfiles/";
        TString path3 = "/u/ajedele/R3BRoot_fork/MACROSS/";
	
	//Create source using ucesb for input ----------------
	TString filename = path + "PspxTofdLosUnpack" + runNumber + ".root ";
	TString outputFileName = path + "PspxTofdHit_" + runNumber + "_ZHisto.root";

	TString inputpar;
	TString filename1;
	TString filename2;

	int nev = 2e9;	
	
	int fParameter = 4;	
        if (fParameter==4)
        {
                if (RunId<213) inputpar = Form("%sparameter/tofd_HitPar_s473_meander197.root", path3.Data());
		else inputpar = Form("%sparameter/tofd_HitPar_s473_meander221.root", path3.Data());
                if (RunId == 205 || RunId == 206 || RunId == 210) {filename = Form("%sPspxTofdLosUnpack0205.root",path.Data()); filename1 = Form("%sPspxTofdLosUnpack0206.root",path.Data());  filename2 = Form("%sPspxTofdLosUnpack0210.root",path.Data());}
                if (RunId == 207 || RunId == 208 || RunId == 209) {filename = Form("%sPspxTofdLosUnpack0207.root",path.Data()); filename1 = Form("%sPspxTofdLosUnpack0208.root",path.Data()); filename2 = Form("%sPspxTofdLosUnpack0209.root",path.Data());}
                if (RunId == 214 || RunId == 215) {filename = Form("%sPspxTofdLosUnpack0214.root",path.Data()); filename1 = Form("%sPspxTofdLosUnpack0215.root",path.Data());}
                if (RunId == 222 || RunId == 223) {filename = Form("%sPspxTofdLosUnpack0222.root",path.Data()); filename1 = Form("%sPspxTofdLosUnpack0223.root",path.Data());}
        }

	// R3BStandard ------------------------------------------------
	bool fTofD = true;
	bool bPspx = true;

	// Create analysis run ---------------------------------------- 
	FairRunAna* run = new FairRunAna();

	//Set up Header -----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	//Create source -----------------------------------------------
	TString runNo;
	R3BFileSource *source = new R3BFileSource(filename);
	if (RunId==191 || RunId==190) source->AddFile(filename1);
        if (fParameter>3)
        {
                if (RunId==205 || RunId==206 || RunId==210) {source->AddFile(filename1); source->AddFile(filename2);}
                if (RunId==207 || RunId==208 || RunId==209) {source->AddFile(filename1); source->AddFile(filename2);}
                if (RunId==214 || RunId==215) source->AddFile(filename1);
                if (RunId==222 || RunId==223) source->AddFile(filename1);
        }
	source->SetRunId(RunId);
	run->SetSource(source); 
	run->SetSink(new FairRootFileSink(outputFileName.Data())); 

	// Runtime data base ------------------------------------------   
	FairRuntimeDb* rtdb = run->GetRuntimeDb();

	// Add Header Copy --------------------------------------------- 
	R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
	run->AddTask(RunIdTask);

	// Add Parameters ----------------------------------------------- 
        FairParRootFileIo* parIo1 = new FairParRootFileIo(kFALSE);
        TList *parList = new TList();
        parList->Add(new TObjString("parameter/TofdTCal0191.root"));

        if(fParameter>1 || fParameter ==-3) {parList->Add(new TObjString(inputpar.Data()));}
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
        rtdb->setInputVersion(RunId, (char*)"TofdTCalPar", 1, 1);
        if(fParameter>1 || fParameter==-3)
        {
                rtdb->getContainer("tofdHitPar");
                rtdb->setInputVersion(RunId, (char*)"tofdHitPar", 1, 1);
        }
        rtdb->print();

	// Calibrate Pspx --------------------------------------------- 
	int runtag;
        if (RunId<203) runtag = 0;          
	else if (RunId<213) runtag = 1;          
	else if (RunId<221) runtag = 2;          
	else if (RunId<230) runtag = 3;          
	const int numDet = 6;         //number of detectors 
	const int numStrips = 32;      //number of strips

	TString fParInName = Form("%s/parameter/ParTest0194-0201.txt",path3.Data());

        R3BPspxPoscal2Cal* pspxPoscal2Cal = new R3BPspxPoscal2Cal("R3BPspxPoscal2Cal", 1);
        pspxPoscal2Cal->SetNumDet(numDet);
        pspxPoscal2Cal->SetNumStrips(numStrips);
        pspxPoscal2Cal->SetParInName(fParInName);
        run->AddTask(pspxPoscal2Cal);

	TString fParName  = Form("%s/parameter/PspxCal2HitParEPos.par",path3.Data());
	TString fParInName1 = Form("%s/parameter/PspXCal2HitPar0184-0193.par",path3.Data());
	TString fParInName2 = Form("%s/parameter/PspXCal2HitPar0211-0212.par",path3.Data());
	TString fParInName3 = Form("%s/parameter/PspXCal2HitPar0220.par",path3.Data());
	TString fParInName4 = Form("%s/parameter/PspXCal2HitPar0229.par",path3.Data());
        TString	fParOutName = Form("%s/parameter/PspxEnergyCalibPar.par",path3.Data());
        TString	fParOutName2 = Form("%s/parameter/PspxEnergyMatchPar.par",path3.Data());
	
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
	run->AddTask(pspxCal2Hit);

	// Calibrate Tofd -----------------------------------------------	
        const int updateRate = 1000;
        int minStats = 1000;
        double TofdPosition = 20.0;   // Set y-position of calibration run (horizontal sweep)
        double ToTLowRange = 50.;    // 
        double ToTHighRange = 200;   // if they have a value, veff and lambda will be calculated according to this value
        bool fSmileyOn = true;
        bool fChargecalib = true;
        int fNbZPeaks = 21;
        int fMaxQ = 51;
        int fMinQ = 30;
        int fBarMin = 24;
        int fBarMax = 29;
        int tpat = -1;
        if (fParameter == 4) tpat = 3;

	printf("fParameter: %d\n",fParameter);
	R3BTofDCal2HitPar_elorenz* tofdCal2HitPar = new R3BTofDCal2HitPar_elorenz();
        tofdCal2HitPar->SetMinStats(minStats);
        tofdCal2HitPar->SetTpat(tpat);
        tofdCal2HitPar->SetTrigger(1);
        tofdCal2HitPar->SetParameterOption(fParameter);
        tofdCal2HitPar->SetTofdPos(TofdPosition);
        tofdCal2HitPar->SetMeanTof(25.);
        tofdCal2HitPar->SetMeanToT(125.);
        tofdCal2HitPar->SetTofdSmiley(fSmileyOn);
        if (fParameter==-3 || (fParameter>1)) tofdCal2HitPar->SetTofdQ(50.);
        tofdCal2HitPar->SetTofdZ(fChargecalib);
        tofdCal2HitPar->SetNbZPeaks(50);
        tofdCal2HitPar->SetMinQ(fMinQ);
        tofdCal2HitPar->SetMaxQ(fMaxQ);
        tofdCal2HitPar->SetBarMin(fBarMin);
        tofdCal2HitPar->SetBarMax(fBarMax);
        tofdCal2HitPar->SetTofdTotLow(ToTLowRange);
        tofdCal2HitPar->SetTofdTotHigh(ToTHighRange);
        tofdCal2HitPar->SetTofdTotLow(ToTLowRange);
        tofdCal2HitPar->SetTofdTotHigh(ToTHighRange);
        tofdCal2HitPar->SetTotRange(ToTLowRange, ToTHighRange);
        tofdCal2HitPar->SetPspx(bPspx);
        tofdCal2HitPar->SetPspxEnergy(pspx_energy);

        run->AddTask(tofdCal2HitPar);
	
	/* Initialize -------------------------------------------------- */
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	/* ------------------------------------------------------------- */
	
	/* Run --------------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	rtdb->saveOutput();
	/* ------------------------------------------------------------- */
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time " << rtime << " s,CPU time " << ctime << " s " << endl << endl;
}
