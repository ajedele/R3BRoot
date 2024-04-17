#include <fstream>
#include <iostream>
extern "C" {
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/base/ext_h101_unpack.h"
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/fibers/ext_h101_fib30.h"
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/fibers/ext_h101_fib31.h"
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/fibers/ext_h101_fib32.h"
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/fibers/ext_h101_fib33.h"
   #include "/u/ajedele/R3BRoot_fork_modified/r3bsource/trloii/ext_h101_tpat.h"
}

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_FIBEO_onion_t fib30;
	EXT_STR_h101_FIBEI_onion_t fib31;
	EXT_STR_h101_FIBEZ_onion_t fib32;
	EXT_STR_h101_FIBEE_onion_t fib33;
	EXT_STR_h101_TPAT_onion_t tpat;
} EXT_STR_h101;


void FiberMapped2CalPar(int RunId);

void FiberMapped2CalPar(){
	FiberMapped2CalPar(220); // 226 is last full run
}

void FiberMapped2CalPar(int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =-1;

	TString path1 = "/lustre/r3b/202205_s522/lmd/";
	TString path2 = "/lustre/land/ajedele/s473/rootfiles/";
	TString path3 = "/u/ajedele/R3BRoot/";

	//Create source using ucesb for input ----------------
	TString filename;
	else filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	TString outputFileName = path2 + "FiberTCalPar" + runNumber1 + "_Test.root";
	cout << "Input file is" << filename << endl;

	TString ntuple_options = "RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path;
	ucesb_path = Form("%s/../upexps/202205_s522/202205_s522",ucesb_dir.Data());

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = 
			new R3BUcesbSource(filename, ntuple_options,ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	source->AddReader(new R3BUnpackReader(&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));

	auto FiberReader30 = new  R3BFiberMAPMTReader(&ucesb_struct.fib30, offsetof(EXT_STR_h101, fib30));
	source->AddReader(FiberReader30);

	auto FiberReader31 = new  R3BFiberMAPMTReader(&ucesb_struct.fib31, offsetof(EXT_STR_h101, fib31));
	source->AddReader(FiberReader31);

	auto FiberReader32 = new  R3BFiberMAPMTReader(&ucesb_struct.fib32, offsetof(EXT_STR_h101, fib32));
	source->AddReader(FiberReader32);

	auto FiberReader33 = new  R3BFiberMAPMTReader(&ucesb_struct.fib33, offsetof(EXT_STR_h101, fib33));
	source->AddReader(FiberReader33);

	auto TpatReader = new  R3BTrloiiTpatReader(&ucesb_struct.tpat, offsetof(EXT_STR_h101, tpat));
	source->AddReader(TpatReader);

	const int refresh = 100;

	// Create analysis run ---------------------------------------- 

	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
	run->SetSink(new FairRootFileSink(outputFileName.Data())); //This seems to be interesting

	// Set Up Header
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	FairRuntimeDb* rtdb = run->GetRuntimeDb();

	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	const int minStats = 100;

	R3BFiberMAPMTMapped2CalPar* fiberMapped2CalPar = new R3BFiberMAPMTMapped2CalPar("FiberTCalPar", 1);
	fiberMapped2CalPar->SetNofModules(4,44);
	run->AddTask(fiberMapped2CalPar);

	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	auto* parOut = new FairParRootFileIo();

	parOut->open("parameter/FiberTCal" + runNumber1 + ".root","RECREATE");
	printf("Parout: I exist?????\n");
	rtdb->setOutput(parOut);

	rtdb->addRun(RunId);
	rtdb->saveOutput();

	// ------------------------------------------------------------- 
	printf("It's the end\n");
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}

