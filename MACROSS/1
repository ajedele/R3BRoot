#include <fstream>
#include <iostream>
 extern "C" {
    //#include "/u/ajedele/R3BRoot/r3bsource/base/ext_h101_unpack.h"
    //#include "/u/ajedele/R3BRoot/r3bsource/los/ext_h101_los.h"
    //#include "/u/ajedele/R3BRoot/r3bsource/trloii/ext_h101_tpat.h"
    #include "/u/land/r3broot/202402_s091_s118/R3BRoot/r3bsource/base/ext_h101_unpack.h"
    #include "/u/land/r3broot/202402_s091_s118/R3BRoot/r3bsource/los/ext_h101_los.h"
 }

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_LOS_t los;
	EXT_STR_h101_TPAT_t tpat;
} EXT_STR_h101;


void LosMapped2CalPar(int RunId);

void LosMapped2CalPar(){
	LosMapped2CalPar(220); // 226 is last full run
}

void LosMapped2CalPar(int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =1;

	TString path1 = "/lustre/r3b/202402_s118/lmd/";
	TString path2 = "/lustre/land/ajedele/s515/rootfiles/";
	TString path3 = "/u/land/r3broot/202402_s091_s118/R3BParams_s091_s118/macros/exp/online/los/";

	//TString path1 = "/lustre/land/201902_s473/lustre/hebe/r3b/201902_s473/";
	//TString path2 = "/lustre/land/ajedele/s473/rootfiles/";
	//TString path3 = "/u/ajedele/R3BRoot/";

	//Create source using ucesb for input ----------------
	TString filename;
	//if (RunId == 190 || RunId == 191) filename = path1 + "main0191_*.lmd " + path1 + "main0190_*.lmd  --allow-errors --input-buffer=47Mi";
	//else filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
	filename = path1 + "losrolu_" + runNumber1 + ".lmd --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	TString outputFileName = path2 + "LosTCalPar" + runNumber1 + "_Test.root";
	cout << "Input file is" << filename << endl;

	TString ntuple_options = "RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path;
	ucesb_path = Form("%s/../upexps/standalone/los_standalone/los",ucesb_dir.Data());
	//ucesb_path = Form("%s/../upexps/201902_s473/201902_s473",ucesb_dir.Data());

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	source->AddReader(new R3BUnpackReader(&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));

	auto LosReader = new  R3BLosReader(&ucesb_struct.los, offsetof(EXT_STR_h101, los));
	source->AddReader(LosReader);

	//auto TpatReader = new  R3BTrloiiTpatReader(&ucesb_struct.tpat, offsetof(EXT_STR_h101, tpat));
	//source->AddReader(TpatReader);

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

	R3BLosMapped2CalPar* losMapped2CalPar = new R3BLosMapped2CalPar("R3BLosMapped2CalPar", 1);
	losMapped2CalPar->SetNofModules(4,44);
	run->AddTask(losMapped2CalPar);

	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	auto* parOut = new FairParRootFileIo();

	parOut->open("parameter/LosTCal" + runNumber1 + ".root","RECREATE");
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

