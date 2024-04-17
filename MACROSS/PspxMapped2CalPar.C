 extern "C" {
    #include "/u/ajedele/R3BRoot_oldest/r3bsource/ext/ext_h101_unpack.h"
    #include "/u/ajedele/R3BRoot_oldest/r3bsource/ext/ext_h101_pspx.h"
    #include "/u/ajedele/R3BRoot_oldest/psp/R3BPspxMapped2CalPar.h"
 }

typedef struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	//EXT_STR_h101_PSPX_onion_t pspx;
	EXT_STR_h101_PSPX_t pspx;
} EXT_STR_h101;


//void PspxMapped2CalPar(int RunId);
void PspxMapped2CalPar(int RunId, int RunId2);

void PspxMapped2CalPar(){
	
	//PspxMapped2CalPar(186);
	PspxMapped2CalPar(186,187);
	//PspxMapped2CalPar(194,196);
	//PspxMapped2CalPar(203,210);
	//PspxMapped2CalPar(213,219);
	//PspxMapped2CalPar(221,228);
	//PspxMapped2CalPar(230,232);
	//PspxMapped2CalPar(441);
	//PspxMapped2CalPar(4);
}



void PspxMapped2CalPar(int RunId, int RunId2)
//void PspxMapped2CalPar(int RunId)
{
	TString runNumber1 = Form("%04d",RunId);
	TString runNumber2 = Form("%04d",RunId2);
	std::cout <<"I am Run1: "<< runNumber1 << "\tRun2: "<< runNumber2 << std::endl;
	//std::cout <<"I am Run1: "<< runNumber1 << std::endl;
	TStopwatch timer;
	timer.Start();

	const Int_t nev =-1;
	//const Int_t nev = 1e8; //Number of events to read, -1 - until CTRC+C

	TString path1 = "/lustre/land/201902_s473/lustre/hebe/r3b/201902_s473/";
	//TString path1 = "/lustre/r3b/202104_s515/lmd/";
	TString path2 = "/lustre/land/ajedele/s473/rootfiles/";

	//Create source using ucesb for input ----------------
	TString filename;
	//filename = path1 + "main" + runNumber1 + "_*.lmd --allow-errors --input-buffer=47Mi";
        for (Int_t irun = RunId; irun <= RunId2; irun++)
	{
		TString runid = Form("%04d",irun);
		filename += path1 + "main" + runid + "_*.lmd ";
	}
	filename += " --allow-errors --input-buffer=47Mi";
	printf("Input Filename: %s\n", filename.Data());
	//TString filename = path1 + "main" + runNumber + "_0003.lmd --allow-errors --input-buffer=47Mi";
	TString outputFileName = path2 + "TestTest_Mapped2CalPar" + runNumber1 + "-" + runNumber2 + ".root";
	//TString outputFileName = path2 + "TestTest_Mapped2CalPar" + runNumber1 + ".root";
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

	//Set up Header -----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);
	
	// Runtime data base ------------------------------------------   
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();


	// Calibrate Pspx --------------------------------------------- 
	const Int_t numDet = 6;         //number of detectors 
		//const Int_t numDet = 3;         //number of detectors 
	const Int_t numStrips = 32;         //number of detectors 
	//const Int_t numExpt = 444;         //number of the experiment 
	const Int_t numExpt = 473;         //number of the experiment
        TString	paroutName = TString::Format("PspxMapped2CalParTest%04d-%04d.par",RunId,RunId2);
        //TString	paroutName = TString::Format("PspxMapped2CalParTest%04d.par",RunId);
	R3BPspxMapped2CalPar* pspxCalibrator = new R3BPspxMapped2CalPar("R3BPspxMapped2CalPar", 10);
	pspxCalibrator->SetNumDet(numDet);
	pspxCalibrator->SetNumStrips(numStrips);
	pspxCalibrator->SetNumExpt(numExpt);
	pspxCalibrator->SetParOutName(paroutName.Data());
	run->AddTask(pspxCalibrator);

	// Initialize -------------------------------------------------- 
	run->Init();
	//((R3BPspxMapped2CalPar*)FairRuntimeDb::instance()->getContainer("R3BPspxCalPar"))->printParams();

	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	//FairLogger::GetLogger()->SetLogScreenLevel("INFO");
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

