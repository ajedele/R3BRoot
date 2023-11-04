#define c_range 8192*1000/200

//==========================================
//Variable
Int_t tpatbin;
double los_trailing=0;
double los_leading=0;
double TOT=0;
double LOS_trailing=0;
double LOS_leading=0;
double tot[8]={0};

//==========================================
//Hist Initialised
TH1D *LOS;
TH2D *LOS_Channel;
TH2D *LOS_MCFD_POS;
TH2D *LOS_TAMEX_POS;

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
	LOS = new TH1D("LOS","LOS",500,50,100);
	LOS_Channel = new TH2D("LOS_Channel","LOS_Channel",11,0,10,500,50,100);
	LOS_MCFD_POS  = new TH2D("LOS_MCFD_POS","LOS_MCFD_POS",100,-1,1,100,-1,1);
	LOS_TAMEX_POS = new TH2D("LOS_TAMEX_POS","LOS_TAMEX_POS",100,-1,1,100,-1,1);
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
		
		if(losCalMax>0)
		{
			R3BLosCalData** losCalData = new R3BLosCalData*[losCalMax];
			for(Int_t j = 0 ; j<losCalMax;j++)
			{
				losCalData[j]=(R3BLosCalData*)(losCal->At(j));
				los_trailing = losCalData[j]->GetMeanTimeTAMEXT();
				los_leading  = losCalData[j]->GetMeanTimeTAMEXL();
				LOS_trailing = losCalData[j]->GetTimeT_ns(losCalData[j]->GetChannel());
				LOS_leading  = losCalData[j]->GetTimeL_ns(losCalData[j]->GetChannel());
				
				TOT = fmod(los_trailing-los_leading + c_range + c_range/2., c_range) - c_range/2.;	
				tot[losCalData[j]->GetChannel()] = fmod(los_trailing-los_leading + c_range + c_range/2., c_range) - c_range/2.;	


				LOS->Fill(TOT);
				LOS_Channel->Fill(losCalData[j]->GetChannel(),tot[losCalData[j]->GetChannel()]);
				//LOS->Fill(los_tot[j]);
				//LOS->Fill(los_tot[j]);
				los_trailing = -999999999;
				los_leading = -999999999;
				TOT = -999999999;
				LOS_trailing = -999999999;
				LOS_leading = -999999999;
				tot[losCalData[j]->GetChannel()] = -999999999;
			}
			if(losCalData){delete[] losCalData;}
		}
	}
}


void DrawHist(){
	TCanvas *los = new TCanvas("los","los",500,500);
	LOS->Draw("COLZ");
	los->SaveAs(".png");
}

void LosToT(){
	Hist();
	TString dir = "/lustre/land/ajedele/s515/rootfiles/";
	TFile*file = new TFile(dir+"LosTCal0010.root","READ");
	if(file){std::cout<<"file found"<<endl;}
	if(!file){std::cout<<"file  Not found"<<endl;}
	function0(file);
	DrawHist();
}
