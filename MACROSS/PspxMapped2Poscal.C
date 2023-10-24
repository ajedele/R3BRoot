
#include <fstream>
#include <iostream>

extern "C" {
	#include "/u/ajedele/R3BRoot_temp/r3bsource/pspx/ext_h101_pspx.h"
}

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_PSPX_t pspx;
	EXT_STR_h101_TPAT_t tpat;
} EXT_STR_h101;


void PspxMapped2Poscal(int RunId);

void PspxMapped2Poscal(){
	PspxMapped2Poscal(187); // 226 is last full run
}

void PspxMapped2Poscal(int RunId)
{
	//const Int_t numExpt = 444;
	const Int_t numExpt = 473;
	TString runNumber1 = Form("%04d",RunId);
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =-1;

	Int_t date;
	TString path1;
	if (numExpt==444 || numExpt==473) {date = 1902;}
	else if (numExpt==515) {date = 2104;}
	if (numExpt==444 | numExpt==473) {path1 = Form("/lustre/land/20%04d_s%03d/lustre/hebe/r3b/20%04d_s%03d/", date, numExpt, date, numExpt);}
	else if (numExpt==515) {path1 = Form("/lustre/r3b/20%d04_s%03d/", date, numExpt);}
	TString path2 = Form("/lustre/land/ajedele/s%03d/rootfiles/", numExpt);

	//Create source using ucesb for input ----------------
	TString filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	TString outputFileName = path2 + "Test_Mapped2Poscal" + runNumber1 + ".root";

	TString ntuple_options = "RAW,PSPX,TPAT";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path;
	ucesb_path = Form("%s/../upexps/20%04d_s%03d/20%04d_s%03d", ucesb_dir.Data(), date, numExpt, date, numExpt);

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = 
			new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	
	source->AddReader(new R3BUnpackReader(&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
	
	source->AddReader(new R3BTrloiiTpatReader(&ucesb_struct.tpat, offsetof(EXT_STR_h101, tpat)));

	//for s473 and s444
	auto PspxReader = new  R3BPspxReader(&ucesb_struct.pspx, offsetof(EXT_STR_h101, pspx));
	source->AddReader(PspxReader);

	//for s515
	//auto PspxReader_s515 = new  R3BPspxReader_s515(&ucesb_struct.pspx, offsetof(EXT_STR_h101, pspx));
	//source->AddReader(PspxReader_s515);

	// Create analysis run ---------------------------------------- 

	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
	run->SetSink(new FairRootFileSink(outputFileName.Data())); //This seems to be interesting

	// Set Up Header
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);

	// Runtime data base ------------------------------------------   
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	//TString parFileName = "PspxCalTest.root";


	// Calibrate Pspx --------------------------------------------- 

	const Int_t numDet = 6;         //number of detectors 
	//const Int_t numDet = 3;         //number of detectors 
	const Int_t numStrips = 32;         //number of detectors 
	std::vector<double> maxenergy;
	if (numExpt == 444) 
		maxenergy = {380000, 22000, 230000, 13000, 270000, 15000};
	 else if (numExpt == 473)
		 maxenergy = {300000, 18000, 450000, 26000, 450000, 25000};
    
	R3BPspxMapped2Poscal* pspxMapped2Poscal = new R3BPspxMapped2Poscal("R3BPspxMapped2Poscal", 1);
	pspxMapped2Poscal->SetNumExpt(numExpt);
	pspxMapped2Poscal->SetNumDet(numDet);
	pspxMapped2Poscal->SetNumStrips(numStrips);
	pspxMapped2Poscal->SetMaxEnergies(maxenergy);
	run->AddTask(pspxMapped2Poscal);

	// Initialize -------------------------------------------------- 

	//((R3BPspxMapped2Poscal*)FairRuntimeDb::instance()->getContainer("R3BPspxCal"))->printParams();
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	//run->Run(185900, 186000);
	rtdb1->saveOutput();
	// ------------------------------------------------------------- 
	printf("It's the end\n");
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is" << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;

}

