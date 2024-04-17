//extern "C"
//{
//	#include "/u/ajedele/R3BRoot_oldest/psp/R3BPspxPoscal2Cal.h"
//	#include "/u/ajedele/R3BRoot_oldest/r3bdata/pspData/R3BPspxCalData.h"
//}
void PspxPoscal2Cal(int RunId);

void PspxPoscal2Cal()
{
	PspxPoscal2Cal(187);
}

void PspxPoscal2Cal(Int_t RunId)
{
	TString runNumber = Form("%04d",RunId);
	TStopwatch timer;
	timer.Start();
	
	TString path = "/lustre/land/ajedele/s473/rootfiles/";

	Double_t nev = 2e9;
	//Double_t nev = 2e6;

	//Create source using ucesb for input ----------------
	TString filename = path + "Test_Mapped2Poscal" + runNumber + ".root ";
	TString outputFileName = path + "Test_Poscal2Cal" + runNumber + ".root";

	/* Create analysis run ---------------------------------------- */
	FairRunAna* run = new FairRunAna();

	//Set up Header -----------------------------------------------
	R3BEventHeader *EvntHeader = new R3BEventHeader();
	run->SetEventHeader(EvntHeader);
	
	//Create source -----------------------------------------------
	TString runNo;
	R3BFileSource *source = new R3BFileSource(filename);
	source->SetRunId(RunId);
	run->SetSource(source); 
	run->SetSink(new FairRootFileSink(outputFileName)); 

	/* Runtime data base ------------------------------------------ */  
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	
	//Add Header Copy ---------------------------------------------- 
	R3BEventHeaderPropagator *RunIdTask = new R3BEventHeaderPropagator();
	run->AddTask(RunIdTask);

	/* Calibrate Pspx --------------------------------------------- */
	const Int_t numDet = 6;         //number of detectors 
	const Int_t numStrips = 32;      //number of detectors
	TString fParInName = Form("ParTest0194-0201.txt");
	
	R3BPspxPoscal2Cal* pspxCalibrator = new R3BPspxPoscal2Cal("R3BPspxPoscal2Cal", 1);
	pspxCalibrator->SetNumDet(numDet);
	pspxCalibrator->SetNumStrips(numStrips);
	pspxCalibrator->SetParInName(fParInName);
	run->AddTask(pspxCalibrator);
	
	/* Initialize -------------------------------------------------- */
	run->Init();
	//FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
	//FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
	FairLogger::GetLogger()->SetLogScreenLevel("ERROR");
	//FairLogger::GetLogger()->SetLogScreenLevel("INFO");
	/* ------------------------------------------------------------- */
	
	/* Run --------------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0: nev);
	//run->Run(185900, 186000);
	rtdb1->saveOutput();
	/* ------------------------------------------------------------- */
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << "Macro finished successfully." << endl;
	cout << "Output file is" << outputFileName << endl;
	cout << "Real time" << rtime << " s,CPU time " << ctime << " s" << endl << endl;
}
