void TofdCal2HitPar(const int RunId, int nev, int fParameter)
{
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	float fRunId = (float)RunId;
	//const int nev =1e9;

        //Filename and path	
	TString path4 = "/lustre/land/ajedele/s473/rootfiles/";
	TString path1 = "/lustre/land/ajedele/s473/rootfiles/TOFD/";
	TString path2 = "/lustre/land/ajedele/s473/rootfiles/PSPX/";
	TString path3 = "/u/ajedele/R3BRoot_fork/MACROSS/";

	TString filename; TString filename1; TString filename2; TString outputFileName;

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
	 
	int tpat = -1;
	if (fParameter == 4) tpat = 3;
	 
	if(fParameter==1)
	{
		inputpar = "";
		outputpar = Form("%sparameter/tofd_HitPar_s473_sweep191.root", path3.Data());
		filename  = Form("%sTofdTCal0190_Test.root", path1.Data());
		filename1 = Form("%sTofdTCal0191_Test.root", path1.Data());
		outputFileName = Form("%sTofdHitPar0191_Test.root", path1.Data());
	}
	else if(fParameter==2)
	{
		inputpar = Form("%sparameter/tofd_HitPar_s473_sweep191.root", path3.Data());
		outputpar = Form("%sparameter/tofd_HitPar_s473_sweep192.root", path3.Data());
		//outputpar = Form("%sparameter/test.root", path3.Data());
		filename  = Form("%sTofdTCal0192_Test.root", path1.Data());
		//filename  = Form("%sTofdTCal0190_Test.root", path1.Data());
		//filename1 = Form("%sTofdTCal0191_Test.root", path1.Data());
		//outputFileName = Form("%sTofdHitPar0191_Verification.root", path1.Data());
		outputFileName = Form("%sTofdHitPar0192_Test.root", path1.Data());
	}
	else if(fParameter==3)
	{
		inputpar = Form("%sparameter/tofd_HitPar_s473_sweep192.root", path3.Data());
		//filename  = Form("%sTofdTCal0197_Test.root",path1.Data());
		//outputFileName = Form("%sTofdHitPar0197_Histo.root",path1.Data());
		filename  = Form("%sTofdTCal0221_Test.root",path1.Data());
		outputFileName = Form("%sTofdHitPar0221_Histo.root",path1.Data());
	}
	else if(fParameter==-3)
	{
		inputpar = Form("%sparameter/tofd_HitPar_s473_sweep192.root",path3.Data());
		//outputpar = Form("%sparameter/tofd_HitPar_s473_meander197.root", path3.Data());
		outputpar = Form("%sparameter/tofd_HitPar_s473_meander221.root", path3.Data());
		
		//filename  = Form("%sTofdTCal0197_Test.root",path1.Data());
		//outputFileName = Form("%sTofdHitPar0197_Cal_Test.root",path1.Data());
		filename  = Form("%sTofdTCal0221_Test.root",path1.Data());
		outputFileName = Form("%sTofdHitPar0221_Cal_Test.root",path1.Data());
	}	
	else if(fParameter==4)
	{
		inputpar = Form("%sparameter/tofd_HitPar_s473_meander197.root", path3.Data());
		filename  = Form("%sTofdTCal%04d_Test.root",path1.Data(),RunId);
		//filename  = Form("%sPspxTofdLosUnpack%04d.root",path4.Data(),RunId);
		outputFileName = Form("%sTofdHitPar%04d_ZHisto.root",path1.Data(),RunId);
		if (RunId == 205 || RunId == 210) {filename = Form("%sTofdTCal0205_Test.root",path1.Data()); filename1 = Form("%sTofdTCal0210_Test.root",path1.Data());}
		if (RunId == 208 || RunId == 209) {filename = Form("%sTofdTCal0208_Test.root",path1.Data()); filename1 = Form("%sTofdTCal0209_Test.root",path1.Data());}
		if (RunId == 222 || RunId == 223) {filename = Form("%sTofdTCal0222_Test.root",path1.Data()); filename1 = Form("%sTofdTCal0223_Test.root",path1.Data());}
		//inputpar = Form("%sparameter/tofd_HitPar_s473_meander221.root", path3.Data());
	}
	else if(fParameter==5)
	{
		inputpar = Form("%sparameter/tofd_HitPar_s473_meander197.root", path3.Data());
		outputpar = Form("%sparameter/tofd_HitPar_s473_Z%03d.root", path3.Data(), RunId);
		
		filename  = Form("%sTofdTCal%04d_Test.root",path1.Data(),RunId);
		outputFileName = Form("%sTofdHitPar%04d_ZCal_Test.root",path1.Data(),RunId);
	}	

	// R3BStandard ------------------------------------------------
	bool fTofD = true;
	bool fLos = false;
	
	// Create analysis run ---------------------------------------- 
	FairRunAna* run = new FairRunAna();

	// Set Up Header ----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	//Create source using root file for input ---------------------
	R3BFileSource *source = new R3BFileSource(filename);
	if (RunId==191 || RunId==190) source->AddFile(filename1);
	if (fParameter>3 && fParameter!=5)
	{
	       	if (RunId==205 || RunId==210) source->AddFile(filename1);
	       	if (RunId==208 || RunId==209) source->AddFile(filename1);
	       	if (RunId==222 || RunId==223) source->AddFile(filename1);
	}
	source->SetRunId(RunId);
	run->SetSource(source);
	run->SetSink(new FairRootFileSink(outputFileName.Data())); 

	//Runtime database --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();

	//Bool_t kParameterMerged = kFALSE;
	FairParRootFileIo* parIo1 = new FairParRootFileIo(kFALSE);
	TList *parList = new TList();
	parList->Add(new TObjString("parameter/TofdTCal0191.root"));

	if (fLos) {parList->Add(new TObjString("parameter/LosTCal0197.root"));}
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
	rtdb->setInputVersion(fRunId, (char*)"TofdTCalPar", 1, 1);
	if (fLos) {
		rtdb->getContainer("LosTCalPar");
		rtdb->setInputVersion(RunId, (char *)"LosTCalPar", 1, 1);
	}
	if(fParameter>1 || fParameter==-3)
	{
		rtdb->getContainer("tofdHitPar");
		rtdb->setInputVersion(RunId, (char*)"tofdHitPar", 1, 1);
	}
	rtdb->print();

	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	int minStats = 1000;
	double TofdPosition = 20.0;   // Set y-position of calibration run (horizontal sweep)
	double ToTLowRange = 50.;    // 
	double ToTHighRange = 200;   // if they have a value, veff and lambda will be calculated according to this value
	bool fSmileyOn = false;
	bool fChargecalib = true;
	int fNbZPeaks = 21;
	int fMaxQ = 60;
	int fMinQ = 10;
	int fBarMin = 25;
	int fBarMax = 28;
	TString fittype = "pol1";

	//R3BTofDCal2HitPar_elorenz* tofdCal2HitPar = new R3BTofDCal2HitPar_elorenz("TofdHitPar", 1);
	R3BTofDCal2HitPar_elorenz* tofdCal2HitPar = new R3BTofDCal2HitPar_elorenz();
	if(fParameter == -3) 
	{
		ToTLowRange = 95.;    // 
		ToTHighRange = 155;   // if they have a value, veff and lambda will be calculated according to this value
		//TString inputfile = Form("%sTofdHitPar0197_Histo.root",path1.Data());
		TString inputfile = Form("%sTofdHitPar0221_Histo.root",path1.Data());
		printf("Filename: %s\n",inputfile.Data());	
		tofdCal2HitPar->ReadHistoFile(inputfile.Data());
		minStats = 100000;
	}
	if(fParameter == 5) 
	{
		
		TString inputfile;
	        inputfile = Form("%sPspxTofdHit_%04d_ZHisto.root",path4.Data(),RunId);
		if (RunId == 205 || RunId == 210) inputfile = Form("%sPspxTofdHit_0205_ZHisto.root",path4.Data());
		if (RunId == 208 || RunId == 209) inputfile = Form("%sPspxTofdHit_0208_ZHisto.root",path4.Data());
		if (RunId == 222 || RunId == 223) inputfile = Form("%sPspxTofdHit_0222_ZHisto.root",path4.Data());
		printf("Filename: %s\n",inputfile.Data());	
		tofdCal2HitPar->ReadHistoFile(inputfile.Data());
		minStats = 1000;
	}
	tofdCal2HitPar->SetMinStats(minStats);
	tofdCal2HitPar->SetTpat(tpat);
	tofdCal2HitPar->SetTrigger(1);
	tofdCal2HitPar->SetParameterOption(fParameter);
	tofdCal2HitPar->SetTofdPos(TofdPosition);
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
	tofdCal2HitPar->SetTotRange(ToTLowRange, ToTHighRange);
	tofdCal2HitPar->SetChargeFit(fittype);

	run->AddTask(tofdCal2HitPar);

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
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}
