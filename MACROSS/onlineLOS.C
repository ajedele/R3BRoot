#include "/u/ajedele/R3BRoot_fork_modified/r3bsource/rolu/ext_h101_rolu_online.h"

struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_ROLU_NOTRIG_onion_t rolu;
	//EXT_STR_h101_TPAT_t TPAT;
	EXT_STR_h101_LOS_t los;	
};

void onlineLOS(int RunId=1)
{
	TString runNumber=Form ("%03d", RunId);

	TStopwatch timer;
	timer.Start();

	// 0 = server, 1=lmd
	int iReadOption = 1;
	TString lmdNumber;
	TString filename;

	// Turn off automatic backtrace 
	gSystem->ResetSignals();   

	const int nev = -1; /* number of events to read, -1 - until CTRL+C */
	if(iReadOption == 0)
	{
		filename = " --stream=lxir133:7302 --allow-errors --input-buffer=100Mi"; //stream output
	}

	if(iReadOption == 1)
	{
		filename = "/lustre/r3b/202402_s118/lmd/losrolu_run0003.lmd --allow-errors --input-buffer=50Mi"; //std
		//filename ="/d/land4/los/lmd/Los_Cosmics_2mm.lmd --allow-errors --input-buffer=100Mi";	
	}



	//TString	outputFileName="/lustre/r3b/202402_s118/rootfiles/losrolu_run0004.root";
	TString	outputFileName="/lustre/land/ajedele/s515/rootfiles/losrolu_run0004.root";

	TString ntuple_options = "RAW";
	TString ucesb_dir = getenv("UCESB_DIR");

	TString ucesb_path;
        //ucesb_path = ucesb_dir + "/../upexps/202402_s118/202402_s118";
        ucesb_path = ucesb_dir + "/../upexps/standalone/los_standalone/los";
	printf("upexps dir: %s\n", ucesb_path.Data());

	EXT_STR_h101_t ucesb_struct;
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
			ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);

	source->AddReader(new R3BUnpackReader(
				&ucesb_struct.unpack,
				offsetof(EXT_STR_h101_t, unpack)));

	//source->AddReader(new R3BTrloiiTpatReader(
	//			(EXT_STR_h101_TPAT_t *)&ucesb_struct.TPAT,
	//			offsetof(EXT_STR_h101, TPAT)));	

	source->AddReader(new R3BLosReader(&ucesb_struct.los, offsetof(EXT_STR_h101_t, los)));
	source->AddReader(new R3BRoluReaderOnline(&ucesb_struct.rolu, offsetof(EXT_STR_h101_t, rolu)));

	const int refresh = 100;                 // refresh rate for saving 
	/* Create online run ------------------------------------ */
#define RUN_ONLINE
#define USE_HTTP_SERVER
#ifdef RUN_ONLINE
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
#ifdef USE_HTTP_SERVER
	int port=8899;
	run->ActivateHttpServer(refresh,port);
#endif
#else
	/* Create analysis run ---------------------------------- */
	FairRunAna* run = new FairRunAna();
#endif

	// Create analysis run ----------------------------------
	run->SetSink(new FairRootFileSink(outputFileName));

	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
	TList *parList = new TList();

	parList->Add(new TObjString("parameter/LosTCal0003.root"));//This is a standin. File not existent right now.
	parList->Add(new TObjString("parameter/RoluTCal0003.root"));//This is a standin. File not existent right now.

	parOut1->open(parList);
	rtdb1->setFirstInput(parOut1);
	rtdb1->print();

	rtdb1->addRun(RunId);
	rtdb1->getContainer("LosTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"LosTCalPar", 1, 1);
	rtdb1->getContainer("RoluTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"RoluTCalPar", 1, 1);


	/* Add analysis task ------------------------------------ */
	// convert Mapped => Cal
	R3BLosMapped2Cal* losMapped2Cal=new R3BLosMapped2Cal("LosTCalPar", 1);
	losMapped2Cal->SetTrigger(1);
	run->AddTask(losMapped2Cal);

	R3BRoluMapped2Cal* roluMapped2Cal=new R3BRoluMapped2Cal("RoluTCalPar", 1);
	roluMapped2Cal->SetSkipTrigger(true); // 1 or 2 LOS detectors  
	run->AddTask(roluMapped2Cal);
	
	auto r3bLosOnlineSpectra = new R3BLosOnlineSpectra();
	run->AddTask(r3bLosOnlineSpectra);

	r3bLosOnlineSpectra->SetNofLosModules(1); // 1 or 2 LOS detectors  
	r3bLosOnlineSpectra->SetLosXYTAMEX(0,0,1,1,0,0,1,1);
	r3bLosOnlineSpectra->SetLosXYMCFD(-1.25,0.8,2.25,2.7,0,0,1,1);                       
	r3bLosOnlineSpectra->SetLosXYToT(-0.002373,0.007423,2.27,3.22,0,0,1,1);//(-0.02054,-0.02495,2.5,3.6,0,0,1,1);
	r3bLosOnlineSpectra->SetEpileup(10000000000000.);  // Events with ToT>Epileup are not considered
	// -1 = no trigger selection
	r3bLosOnlineSpectra->SetTrigger(1);
	// if 0, no tpat selection
	r3bLosOnlineSpectra->SetTpat(0);
	
	auto r3bRoluOnlineSpectra = new R3BRoluOnlineSpectra();
	r3bRoluOnlineSpectra->SetNofRoluModules(1); // 1 or 2 LOS detectors  
	run->AddTask(r3bRoluOnlineSpectra);

	/* Initialize ------------------------------------------- */
	run->Init();
	rtdb1->print();
	//  FairLogger::GetLogger()->SetLogScreenLevel("INFO");    
	//  FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//  FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	// FairLogger::GetLogger()->SetLogScreenLevel("DEBUG1");
	FairLogger::GetLogger()->SetLogScreenLevel("ERROR");

	/* Run -------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
	//    rtdb1->saveOutput();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
		<< endl << endl;
}

