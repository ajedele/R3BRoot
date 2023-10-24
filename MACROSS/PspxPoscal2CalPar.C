extern "C"
{
	#include "/u/ajedele/R3BRoot_oldest/psp/R3BPspxPoscal2CalPar.h"
	//#include "/u/ajedele/R3BRoot_oldest/r3bdata/pspData/R3BPspxPoscalData.h"
}

//void PspxPoscal2CalPar(int RunId, int RunId2);
void PspxPoscal2CalPar(int RunId);

void PspxPoscal2CalPar()
{
	//PspxPoscal2CalPar(186,187); //non-empty target runs E=400 MeV/u
	//PspxPoscal2CalPar(194,197); //non-empty target runs E=400 MeV/u
	//PspxPoscal2CalPar(204,210); //non-empty target runs E=400 MeV/u
	//PspxPoscal2CalPar(213,219); //non-empty target runs E=550 MeV/u  
	//PspxPoscal2CalPar(221,228); //non-empty target runs E=650 MeV/u  
	//PspxPoscal2CalPar(230,232); //non-empty target runs E=800 MeV/u  
	PspxPoscal2CalPar(194); //non-empty target runs E=800 MeV/u  
}

//void PspxPoscal2CalPar(Int_t RunId, Int_t RunId2)
void PspxPoscal2CalPar(Int_t RunId)
{
	//Int_t numExpt = 444;      //number of detectors
	Int_t numExpt = 473;      //number of detectors
	
	TString runNumber1 = Form("%04d",RunId);
	//TString runNumber2 = Form("%04d",RunId2);
	TStopwatch timer;
	timer.Start();
	
	const Int_t nev = 1e9; //Number of events to read, -1 - until CTRC+C

	TString path = Form("/lustre/land/ajedele/s%03d/rootfiles/",numExpt);
	
	//Create source using ucesb for input ----------------
	TString filename = path + "Test_Mapped2Poscal" + runNumber1 + ".root ";
	//printf("Input Filename: %s\n", filename.Data());
	//TString outputFileName = path + "Test_Poscal2CalPar" + runNumber1 + "-" + runNumber2 + ".root";
	TString outputFileName = path + "Test_Poscal2CalPar" + runNumber1 + ".root";

	/* Create analysis run ---------------------------------------- */
	FairRunAna* run = new FairRunAna();

	//Set up Header -----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);
	
	//Create source -----------------------------------------------
	TString runNo;
	R3BFileSource *source = new R3BFileSource(filename);
	source->SetRunId(RunId);
	//for (int irun = RunId+1; irun <=RunId2; irun++)
	//{
	//	runNo = Form("%04d",irun);
	//	filename = path + "Test_Mapped2Poscal" + runNo + ".root ";
	//  	source->AddFile(filename);
	//	source->SetRunId(irun);
	//}	
	run->SetSource(source); 
	run->SetSink(new FairRootFileSink(outputFileName)); 

	/* Runtime data base ------------------------------------------ */  
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	
	/* Calibrate Pspx --------------------------------------------- */
	const Int_t numDet = 6;         //number of detectors 
	const Int_t numStrips = 32;      //number of detectors
	//TString fParOutName = Form("PspxPoscal2CalPar%d-%d.par", RunId, RunId2);
	TString fParOutName = Form("PspxPoscal2CalPar%d.par", RunId);
    
	std::vector<double> maxenergy;
	if (numExpt == 444) 
		maxenergy = {380000, 22000, 230000, 13000, 270000, 15000};
	 else if (numExpt == 473)
		 maxenergy = {300000, 18000, 450000, 26000, 450000, 25000};
    
	
	R3BPspxPoscal2CalPar* pspxCalibrator = new R3BPspxPoscal2CalPar("R3BPspxPoscal2CalPar", 1);
	pspxCalibrator->SetNumDet(numDet);
	pspxCalibrator->SetNumStrips(numStrips);
	pspxCalibrator->SetMaxEnergies(maxenergy);
	pspxCalibrator->SetParOutName(fParOutName);
	run->AddTask(pspxCalibrator);
	
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
