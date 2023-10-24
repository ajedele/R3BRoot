//==========================================
//Variable
Int_t tpatbin;
double los_trailing=0;
double los_leading=0;
std::vector<double> los_tot{};
std::vector<double> psp1_energy{};
std::vector<double> psp2_energy{};
int counter1=0;
int counter2=0;

//==========================================
//Hist Initialised
TH1D *LOS;
TH1D *LOS_PSP1;
TH1D *LOS_PSP2;

//==========================================
//Progress Bar
#define BAR ">>>>>>>>>>>>>>>>>>>>>>>>>>>><"
#define BARW (sizeof BAR - 1 )
void printProgress(double percentage)
{       
        int val = (int)(percentage*100);
        int lpad =(int)(percentage*BARW);
        int rpad = BARW-lpad;
        printf("\r%3d%% [%.*s%*s]",val,lpad,BAR,rpad,"");
        fflush(stdout);
}

//==========================================
void Hist(){
	LOS = new TH1D("LOS","LOS",4096,0,4095);
	LOS_PSP1 = new TH1D("LOSvsPSP1","LOSvsPSP1",4096,0,4095);
	LOS_PSP2 = new TH1D("LOSvsPSP2","LOSvsPSP2",4096,0,4095);
}

void function0(TFile*file)
{
        TTree*tree = (TTree*)file->Get("evt");//Edit file of choice here
        if(tree){std::cout<<"Event Tree Loaded"<<std::endl;}
        if(!tree){std::cout<<"Event Tree Not Loaded"<<std::endl;}
        float nn = tree->GetEntries();
	//nn=1000;//Smaller Number To make life nicer
        std::cout << "Number of Entries in tree: " << nn << std::endl;

        R3BEventHeader*DataCA = new R3BEventHeader();
        TBranch*branchData = tree->GetBranch("EventHeader.");
        if(branchData){std::cout<< "EventHeader Loaded"<<std::endl;}
        branchData->SetAddress(&DataCA);

        TClonesArray*losCal = new TClonesArray("R3BLosCalData",10);
        TBranch*blosCal  = tree->GetBranch("LosCal");
        if(blosCal){std::cout<< "LosCal Loaded"<<std::endl;}
        if(!blosCal){std::cout<< "LosCal Not Loaded"<<std::endl;}
        blosCal->SetAddress(&losCal);

        TClonesArray*pspxHit = new TClonesArray("R3BPspxHitData",10);
        TBranch*bpspx  = tree->GetBranch("PspxHitData");
        if(bpspx){std::cout<< "PspxHitData Loaded"<<std::endl;}
        if(!bpspx){std::cout<< "PspxHitData Not Loaded"<<std::endl;}
        bpspx->SetAddress(&pspxHit);

	for(Int_t ii = 0 ; ii<nn ; ii++)
	{
		//Progress Bar
        	double per = (ii+1.) /nn;
        	printProgress(per);

		//Clear Array
		losCal->Clear();
		//Dont Forget to get your event 
		tree->GetEntry(ii);

		Int_t losCalMax = losCal->GetEntriesFast();//sometimes 2 ?? 
		Int_t pspxHitMax = pspxHit->GetEntriesFast();//sometimes 2 ?? 
		
		//for(Int_t j = 0; j < 16; j++)
		//{
		//	tpatbin = DataCA->GetTpat();
		//}

		if(losCalMax>0)
		{
			R3BLosCalData** losCalData = new R3BLosCalData*[losCalMax];
			for(Int_t j = 0 ; j<losCalMax;j++)
			{
				losCalData[j]=(R3BLosCalData*)(losCal->At(j));
				los_trailing = losCalData[j]->GetMeanTimeTAMEXT();
				los_leading  = losCalData[j]->GetMeanTimeTAMEXL();
			
				los_tot.push_back(los_trailing - los_leading);	
			

				LOS->Fill(los_tot[j]);
				los_trailing = -999999999;
				los_leading = -999999999;
			}
			if(losCalData){delete[] losCalData;}
		}
		
		if (pspxHitMax>0)
		{
			R3BPspxHitData** pspxHitData = new R3BPspxHitData*[pspxHitMax];
			for(Int_t j = 0 ; j<pspxHitMax;j++)
			{
				pspxHitData[j]=(R3BPspxHitData*)(pspxHit->At(j));
				if (pspxHitData[j]->GetDetector()==2 && pspxHitData[j]->GetEnergy()>200)
				{	
					psp1_energy.push_back(pspxHitData[j]->GetEnergy());
				}
				if (pspxHitData[j]->GetDetector()==4 && pspxHitData[j]->GetEnergy()>200)
				{	
					psp2_energy.push_back(pspxHitData[j]->GetEnergy());
				}
			}
			if(pspxHitData){delete[] pspxHitData;}
		}

		if (los_tot.size() == psp1_energy.size() && los_tot.size() == psp2_energy.size())
		{
			for (int ii = 0; ii < los_tot.size(); ii++)
			{
				LOS_PSP1->Fill(los_tot[ii], psp1_energy[ii]);
				LOS_PSP2->Fill(los_tot[ii], psp1_energy[ii]);
			}
		}

		los_tot.clear();
		psp1_energy.clear();
		psp2_energy.clear();
	}
}


void DrawHist(){
	TCanvas *los = new TCanvas("los","los",500,500);
	LOS->Draw("COLZ");
	los->SaveAs(".png");
}

void LosTime(){
	Hist();
	TString dir = "/lustre/land/ajedele/s473/rootfiles/";
	TFile*file = new TFile(dir+"LosTCal0187_Test.root","READ");
	if(file){std::cout<<"file found"<<endl;}
	if(!file){std::cout<<"file  Not found"<<endl;}
	function0(file);
	DrawHist();
}
