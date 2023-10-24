#include <fstream>
#include <iostream>
 extern "C" {
    #include "/u/ajedele/R3BRoot/r3bsource/base/ext_h101_unpack.h"
    #include "/u/ajedele/R3BRoot/r3bsource/tofd/ext_h101_tofd.h"
    #include "/u/ajedele/R3BRoot/r3bsource/trloii/ext_h101_tpat.h"
    #include "/u/ajedele/R3BRoot/r3bsource/trloii/R3BTrloiiTpatReader.h"
 }

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_TOFD_onion_t tofd;
	EXT_STR_h101_TPAT_t tpat;
} EXT_STR_h101;


void TofdMapped2Cal(int RunId);

void TofdMapped2Cal(){
	TofdMapped2Cal(220); // 226 is last full run
}



void TofdMapped2Cal(int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =-1;

	TString path1 = "/lustre/land/201902_s473/lustre/hebe/r3b/201902_s473/";
	TString path2 = "/lustre/land/ajedele/s473/rootfiles/";
	TString path3 = "/u/ajedele/R3BRoot/";

	//Create source using ucesb for input ----------------
	TString filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	TString outputFileName = path2 + "TofdTCal" + runNumber1 + "_Test.root";
	cout << "Input file is" << filename << endl;

	TString ntuple_options = "RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path;
	ucesb_path = Form("%s/../upexps/201902_s473/201902_s473",ucesb_dir.Data());

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = 
			new R3BUcesbSource(filename, ntuple_options,ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	source->AddReader(new R3BUnpackReader(&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));

	auto TofdReader = new R3BTofdReader(&ucesb_struct.tofd, offsetof(EXT_STR_h101, tofd));
	source->AddReader(TofdReader);

	auto TpatReader = new R3BTrloiiTpatReader(&ucesb_struct.tpat, offsetof(EXT_STR_h101, tpat));
	source->AddReader(TpatReader);

	const int refresh = 100;

	// Create analysis run ---------------------------------------- 
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
	run->SetSink(new FairRootFileSink(outputFileName.Data()));

	// Set Up Header
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	FairRuntimeDb* rtdb = run->GetRuntimeDb();

	Bool_t kParameterMerged = kTRUE;
	auto* parIn = new FairParRootFileIo(kParameterMerged);

	TList *parList = new TList();
	parList->Add(new TObjString(path3 + "parameter/TofdTCal0190.root"));

	parIn->open(parList);
	rtdb->setFirstInput(parIn);

	rtdb->addRun(RunId);
	rtdb->getContainer("TofdTCalPar");
	rtdb->setInputVersion(RunId, (char*)"TofdTCalPar", 1, 1);
	rtdb->print();


	//Add Analysis Task --------------------------------------------
	const int updateRate = 1000;
	const int minStats = 100;

	R3BTofDMapped2Cal* tofdMapped2Cal = new R3BTofDMapped2Cal("TofdTCal", 1);
	tofdMapped2Cal->SetNofModules(4,44);
	run->AddTask(tofdMapped2Cal);

	//((R3BTofdMapped2Cal*)FairRuntimeDb::instance()->getContainer("R3BSampCal"))->printParams();
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	//auto* parOut = new FairParRootFileIo();

	//parOut->open("parameter/TofdTCal" + runNumber1 + ".root","RECREATE");
	//rtdb->setOutput(parOut);

	//rtdb->addRun(RunId);
	//rtdb->saveOutput();

	// ------------------------------------------------------------- 
	printf("It's the end\n");
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}

