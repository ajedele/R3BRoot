
#include <fstream>
#include <iostream>
 extern "C" {
    #include "/u/ajedele/R3BRoot_oldest/r3bsource/ext/ext_h101_unpack.h"
    #include "/u/ajedele/R3BRoot_oldest/r3bsource/ext/ext_h101_pspx.h"
    #include "/u/ajedele/R3BRoot_oldest/psp/R3BPspxMapped2Cal.h"
 }

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	//EXT_STR_h101_PSPX_onion_t pspx;
	EXT_STR_h101_PSPX_t pspx;
} EXT_STR_h101;


//void PspxMapped2Cal(int RunId, int RunId2);
void PspxMapped2Cal(int RunId);

void PspxMapped2Cal(){
	//PspxMapped2Cal(186,193); // 226 is last full run
	//PspxMapped2Cal(211,212); // 226 is last full run
	//PspxMapped2Cal(220,193); // 226 is last full run
	//PspxMapped2Cal(186,193); // 226 is last full run
	PspxMapped2Cal(220); // 226 is last full run
	//PspxMapped2Cal(229); // 226 is last full run
}



//void PspxMapped2CalPar(int RunId)
//void PspxMapped2Cal(int RunId, int RunId2)
void PspxMapped2Cal(int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	//TString runNumber2 = Form("%04d",RunId2);
	//std::cout << "I am Run1: "<< runNumber1 << "\tI am Run2: "<< runNumber2 << std::endl;
	std::cout << "I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =-1;
	//const Int_t nev = 1e8; //Number of events to read, -1 - until CTRC+C

	TString path1 = "/lustre/land/201902_s473/lustre/hebe/r3b/201902_s473/";
	//TString path1 = "/lustre/r3b/20210_s515/lmd/";
	TString path2 = "/lustre/land/ajedele/s473/rootfiles/";

	//Create source using ucesb for input ----------------
	TString filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
	//TString filename;
       	//for (int irun = RunId; irun <= RunId2; irun++)
	//{
	//	TString runid = Form("%04d",irun);
	//	filename += path1 + "main" + runid + "_*.lmd ";
	//}
	//filename += "--allow-errors --input-buffer=47Mi";
	//TString filename = path1 + "main" + runNumber + "_0001.lmd  --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	//TString filename = path1 + "main" + runNumber + "_0003.lmd --allow-errors --input-buffer=47Mi";
	//TString outputFileName = path2 + "Test_Mapped2Cal" + runNumber1 + "-" + runNumber2 + ".root";
	TString outputFileName = path2 + "Test_Mapped2Cal" + runNumber1 + ".root";
	//TString outputFileName = "Test_" + runNumber + ".root";
	cout << "Input file is" << filename << endl;

	TString ntuple_options = "RAW,PSPX,TPAT";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path;
	//ucesb_path = "/u/land/fake_cvmfs/9.13/upexps/201902_s444/201902_s444";
	ucesb_path = "/u/land/fake_cvmfs/9.13/upexps/201902_s473/201902_s473";
	//ucesb_path = "/u/land/fake_cvmfs/9.13/upexps/202104_s515/202104_s515";

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = 
			new R3BUcesbSource(filename, ntuple_options,ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	source->AddReader(new R3BUnpackReader(&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));

	//for s473 and s444
	auto PspxReader = new  R3BPspxReader(&ucesb_struct.pspx, offsetof(EXT_STR_h101, pspx));
	source->AddReader(PspxReader);

	//for s515
	//auto PspxReader_s515 = new  R3BPspxReader_s515(&ucesb_struct.pspx, offsetof(EXT_STR_h101, pspx));
	//source->AddReader(PspxReader_s515);

	// Create analysis run ---------------------------------------- 

	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
	run->SetOutputFile(outputFileName.Data()); //This seems to be interesting

	// Set Up Header
	//R3BEventHeader *EvntHeader = new R3BEventHeader();
	//run->SetEventHeader(EvntHeader);

	// Runtime data base ------------------------------------------   

	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	//TString parFileName = "PspxCalTest.root";


	// Calibrate Pspx --------------------------------------------- 

	const Int_t numDet = 6;         //number of detectors 
	//const Int_t numDet = 3;         //number of detectors 
	const Int_t numStrips = 32;         //number of detectors 
	//const Int_t numExpt = 444;         //number of the experiment 
	const Int_t numExpt = 473;         //number of the experiment 
	R3BPspxMapped2Cal* pspxMapped2Cal = new R3BPspxMapped2Cal("R3BPspxMapped2Cal", 1);
	pspxMapped2Cal->SetNumExpt(numExpt);
	pspxMapped2Cal->SetNumDet(numDet);
	pspxMapped2Cal->SetNumStrips(numStrips);
	run->AddTask(pspxMapped2Cal);

	// Initialize -------------------------------------------------- 

	//((R3BPspxMapped2Cal*)FairRuntimeDb::instance()->getContainer("R3BPspxCal"))->printParams();
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	//FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	// ------------------------------------------------------------- 

	// Run --------------------------------------------------------- 

	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
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

