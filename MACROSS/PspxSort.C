#include "boost/multi_array.hpp"

//==========================================
//Variable Initialised
std::vector<double> timing;
std::vector<double> los_tot;
int RunId;
const int det = 6;
const int strip = 32;
double los_leading = 0;	
double los_trailing = 0;	
bool LOS = false;

//==========================================
//Hist Initialised
TH1D *lostot;
TH2D *lospsp1;
TH2D *lospsp2;
TH2D *lospsp3;
TCanvas *Los_tot;
TCanvas *LosPsp1;
TCanvas *LosPsp2;
TCanvas *LosPsp3;

TH2D *energyfront;
TH2D *energyback;
TH2D *energydet2;
TH2D *energydet3;
TH2D *positionfront;
TH2D *positionfront2;
TH2D *positionback;
TH2D *positionback2;
TH2D *positionDet2;
TH2D *positionDet3;
TH2D *position2Det2;
TH2D *position2Det3;
TH2D *efronttarget;
TH2D *ebacktarget;
TH2D *posfronttarget;
TH2D *posbacktarget;

TCanvas *Pspposdet2;
TCanvas *Pspposdet3;
TCanvas *Pspposf;
TCanvas *Pspposb;
TCanvas *Pspposf2;
TCanvas *Pspposb2;
TCanvas *Pspef;
TCanvas *Pspeb;
TCanvas *Pspedet2;
TCanvas *Pspedet3;
TCanvas *Pspposft;
TCanvas *Pspposbt;
TCanvas *Pspeft;
TCanvas *Pspebt;

TH1D *Timing[det][2];
TString histName;

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

void FindingTime(TFile*file)
{
	for (int idet = 0; idet < det; idet++)
	{
		for (int iface = 0; iface < 2; iface++)
		{
			histName = Form("FinalTime_Det%d_Face%d",idet+1,iface+1);					
			Timing[idet][iface] = (TH1D*)(file->FindObjectAny(histName));
			int binmax = Timing[idet][iface]->GetMaximumBin();
			double max = Timing[idet][iface]->GetXaxis()->GetBinCenter(binmax);
			printf("Det: %d, Face: %d, Max: %lf", idet+1, iface+1, max);
			timing.push_back(max);
		}
	}	
	return timing;
}

//det, face, strip, energy, position, time, mult
void FillingHist(std::vector<std::tuple<int, int, int, double, double, double, int>> vect_pspx, std::vector<double> vect_los)
{
	//printf("\nInside filling \n\n");

	double frontEbt[10] = {0};
	double frontE1[10] = {0};
	double frontE2[10] = {0};
	double backEbt[10] = {0};
	double backE1[10] = {0};
	double backE2[10] = {0};
	double frontposbt[10] = {0};
	double frontpos1[10] = {0};
	double frontpos2[10] = {0};
	double backposbt[10] = {0};
	double backpos1[10] = {0};
	double backpos2[10] = {0};
	int frontstripbt[10] = {0};
	int frontstrip1[10] = {0};
	int frontstrip2[10] = {0};
	int backstripbt[10] = {0};
	int backstrip1[10] = {0};
	int backstrip2[10] = {0};
	
	int max_value = 0;

	for(int jj=0; jj < vect_pspx.size(); jj++)
	{
		//printf("Before Sorting: Det: %d, Face: %d, Strip: %d, Energy: %lf, Position: %lf, Time: %lf, Mult: %d\n",get<0>(vect_pspx[jj]), get<1>(vect_pspx[jj]),get<2>(vect_pspx[jj]), get<3>(vect_pspx[jj]),get<4>(vect_pspx[jj]), get<5>(vect_pspx[jj]), get<6>(vect_pspx[jj]));

		if (get<0>(vect_pspx[jj])==2) 
		{
			if (get<1>(vect_pspx[jj])==1) 
			{
				frontposbt[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
			       	frontEbt[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		frontstripbt[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
			else 
			{	
				backposbt[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
			       	backEbt[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		backstripbt[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
		}
		if (get<0>(vect_pspx[jj])==4) 
		{
			if (get<1>(vect_pspx[jj])==1) 
			{
				frontpos1[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
			       	frontE1[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		frontstrip1[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
			else 
			{	
				backpos1[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
			       	backE1[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		backstrip1[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
		}
		if (get<0>(vect_pspx[jj])==6) 
		{
			if (get<1>(vect_pspx[jj])==1) 
			{
				frontpos2[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
		       		frontE2[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		frontstrip2[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
			else 
			{	
				backpos2[get<6>(vect_pspx[jj])] = get<4>(vect_pspx[jj]);
		       		backE2[get<6>(vect_pspx[jj])] = get<3>(vect_pspx[jj]);
		       		backstrip2[get<6>(vect_pspx[jj])] = get<2>(vect_pspx[jj]);
			}
		}
		if (get<6>(vect_pspx[jj]) > max_value) max_value = get<6>(vect_pspx[jj]) + 1;
	}	

	//printf("After Sorting: Front pos bt: %lf, Energy bt: %lf, Back pos bt: %lf, Energy bt: %lf, Front pos 1: %lf, Energy 1: %lf, Back pos 1: %lf, Energy 1: %lf, Front pos 2: %lf, Energy 2: %lf, Back pos 2: %lf, Energy 2: %lf\n", frontposbt[0], frontEbt[0], backposbt[0], backEbt[0], frontpos1[0], frontE1[0], backpos1[0], backE1[0], frontpos2[0], frontE2[0], backpos2[0], backE2[0]);
	//printf("After Sorting: Front strip bt: %d, Back strip bt: %d, Front strip 1: %d, Back strip 1: %d, Front strip 2: %d, Back strip 2: %d\n", frontstripbt[0], backstripbt[0], frontstrip1[0], backstrip1[0], frontstrip2[0], backstrip2[0]);

	for (int ii = 0; ii < max_value; ii++)
	{
		//printf("vect_los[0]: %lf\n",vect_los[0]);
		if (frontstrip1[ii]>0 && frontstripbt[ii]>0 && frontstrip1[ii]<=strip && frontstripbt[ii]<=strip) 
		{
			//PosFrontTarget[frontstripbt[ii]-1][frontstrip1[ii]-1]->Fill(frontposbt[ii], frontpos1[ii]);	
			posfronttarget->Fill(frontposbt[ii], frontpos1[ii]);
			printf("Frontstripbt: %d, Frontstrip1: %d\n", frontstripbt[ii], frontstrip1[ii]);
			efronttarget->Fill(frontEbt[ii], frontE1[ii]);
		}
		if (backstrip1[ii]>0 && backstripbt[ii]>0 && backstrip1[ii]<=strip && backstripbt[ii]<=strip) 
		{
			//PosBackTarget[backstripbt[ii]-1][backstrip1[ii]-1]->Fill(backposbt[ii], backpos1[ii]);
			posbacktarget->Fill(backposbt[ii], backpos1[ii]);
			ebacktarget->Fill(backEbt[ii], backE1[ii]);
		}	

		if (frontstrip1[ii]>0 && frontstrip2[ii]>0 && frontstrip1[ii]<=strip && frontstrip2[ii]<=strip) 
		{
			//PositionFront[frontstrip1[ii]-1][frontstrip2[ii]-1]->Fill(frontpos1[ii], frontpos2[ii]);	
			if (frontE1[ii]>490 && frontE2[ii]>490) positionfront2->Fill(frontpos1[ii], frontpos2[ii]);
			else positionfront->Fill(frontpos1[ii], frontpos2[ii]);
			energyfront->Fill(frontE1[ii], frontE2[ii]);
		}
		if (backstrip1[ii]>0 && backstrip2[ii]>0 && backstrip1[ii]<=strip && backstrip2[ii]<=strip) 
		{
			//PositionBack[backstrip1[ii]-1][backstrip2[ii]-1]->Fill(backpos1[ii], backpos2[ii]);
			if (frontE1[ii]>490 && frontE2[ii]>490) positionback2->Fill(backpos1[ii], backpos2[ii]);
			else positionback->Fill(backpos1[ii], backpos2[ii]);
			energyback->Fill(backE1[ii], backE2[ii]);
		}	
		if (frontstripbt[ii]>0 && backstripbt[ii] && frontstripbt[ii]<=strip && backstripbt[ii]<=strip)
		{
			if (LOS==true) {lospsp1->Fill(vect_los[0], frontEbt[ii]);}
		}
		if (frontstrip1[ii]>0 && backstrip1[ii] && frontstrip1[ii]<=strip && backstrip1[ii]<=strip)
		{
			if (frontE1[ii]>490 && backE1[ii]>490) position2Det2->Fill(frontpos1[ii], backpos1[ii]);
			else positionDet2->Fill(frontpos1[ii], backpos1[ii]);
			energydet2->Fill(frontE1[ii], backE1[ii]);
			if (LOS == true) {lospsp2->Fill(vect_los[0], frontE1[ii]);}
		}
		if (frontstrip2[ii]>0 && backstrip2[ii] && frontstrip2[ii]<=strip && backstrip2[ii]<=strip)
		{
			if (frontE2[ii]>490 && backE2[ii]>490) position2Det3->Fill(frontpos1[ii], backpos1[ii]);
			else positionDet3->Fill(frontpos2[ii], backpos2[ii]);
			energydet3->Fill(frontE2[ii], backE2[ii]);
			if (LOS == true) {lospsp3->Fill(vect_los[0], frontE2[ii]);}
		}

		frontposbt[ii] = -99;
		frontpos1[ii]  = -99;
		frontpos2[ii]  = -99;
		backposbt[ii] = -99;
		backpos1[ii]  = -99;
		backpos2[ii]  = -99;
		frontEbt[ii] = -999999;
		frontE1[ii]  = -999999;
		frontE2[ii]  = -999999;
		backEbt[ii] = -999999;
		backE1[ii]  = -999999;
		backE2[ii]  = -999999;
		
		frontstripbt[ii] = 0;
		frontstrip1[ii]  = 0;
		frontstrip2[ii]  = 0;
		backstripbt[ii] = 0;
		backstrip1[ii]  = 0;
		backstrip2[ii]  = 0;
	}
}

void Hist()
{
	TString nom;
	if (LOS == true)
	{
		nom = Form("LosToT_Run%d",RunId);
		lostot = new TH1D(nom.Data(), nom.Data(), 3000,-500,2500);
		nom = Form("LosPsp1_Run%d",RunId);
		lospsp1 = new TH2D(nom.Data(), nom.Data(), 500,-0,500,900,0,900);
		nom = Form("LosPsp2_Run%d",RunId);
		lospsp2 = new TH2D(nom.Data(), nom.Data(), 500,-0,500,900,0,900);
		nom = Form("LosPsp3_Run%d",RunId);
		lospsp3 = new TH2D(nom.Data(), nom.Data(), 500,-0,500,900,0,900);
	}

	nom = Form("PositionFront_Run%d",RunId);
	positionfront  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("PositionBack_Run%d", RunId);
	positionback   = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("PositionFront2_Run%d",RunId);
	positionfront2 = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("PositionBack2_Run%d", RunId);
	positionback2  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("Position_Det2_Run%d", RunId);
	positionDet2  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("Position_Det3_Run%d", RunId);
	positionDet3  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("Position2_Det2_Run%d", RunId);
	position2Det2  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("Position2_Det3_Run%d", RunId);
	position2Det3  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	
	nom = Form("EnergyFront_Run%d", RunId);
	energyfront = new TH2D(nom.Data(), nom.Data(), 1000,0,1000,1000,0,1000);
	nom = Form("EnergyBack_Run%d", RunId);
	energyback  = new TH2D(nom.Data(), nom.Data(), 1000,0,1000,1000,0,1000);
	nom = Form("EnergyDet2_Run%d", RunId);
	energydet2 = new TH2D(nom.Data(), nom.Data(), 1000,0,1000,1000,0,1000);
	nom = Form("EnergyDet3_Run%d", RunId);
	energydet3  = new TH2D(nom.Data(), nom.Data(), 1000,0,1000,1000,0,1000);
	
	nom = Form("PositionFrontTarget_Run%d", RunId);
	posfronttarget = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("PositionBackTarget_Run%d", RunId);
	posbacktarget  = new TH2D(nom.Data(), nom.Data(), 1000,-5.0,5.0,1000,-5.0,5.0);
	nom = Form("EnergyFrontTarget_Run%d", RunId);
	efronttarget = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
	nom = Form("EnergyBackTarget_Run%d", RunId);
	ebacktarget  = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
}

//det, face, side, energy, position, time, mult, mainstrip
std::vector<std::tuple<int, int, int, double, double, double, int>> Adding(std::vector<std::tuple<int, int, int, double, double, double, int, bool>> vect1)
{
	//printf("\nInside adding \n\n");
	std::vector<std::tuple<int, int, int, double, double, double, int>> vect_adding;
	double newenergy  = 0;
	bool replace = false;

	for (int ii = 0; ii < vect1.size(); ii++)
	{
		if (get<7>(vect1[ii]) == true)
		{
			for (int jj = 0; jj < vect1.size(); jj++)
			{
				if ((ii!=jj) && (get<0>(vect1[jj]) == get<0>(vect1[ii])) && (get<1>(vect1[jj]) == get<1>(vect1[ii])) && (std::abs(get<2>(vect1[ii])-get<2>(vect1[jj]))==1))
				{
					if (get<7>(vect1[jj]) == false && std::abs(get<5>(vect1[ii])-get<5>(vect1[jj]))<15)
					{
						//continue;
						newenergy = get<3>(vect1[ii]) + get<3>(vect1[jj]);
						vect_adding.push_back(make_tuple(get<0>(vect1[ii]), get<1>(vect1[ii]), get<2>(vect1[ii]), newenergy, get<4>(vect1[ii]), get<5>(vect1[ii]), get<6>(vect1[ii])));
						replace = true;
						printf("Det: %d, Face: %d, Strip: %d, Energy: %lf, Position: %lf, Timing: %lf, Mult: %d\n", get<0>(vect1[ii]), get<1>(vect1[ii]), get<2>(vect1[ii]), newenergy, get<4>(vect1[ii]), get<5>(vect1[ii]), get<6>(vect1[ii]));
					}
				}
			}
			if (replace == false) 
			{
				vect_adding.push_back(make_tuple(get<0>(vect1[ii]), get<1>(vect1[ii]), get<2>(vect1[ii]), get<3>(vect1[ii]), get<4>(vect1[ii]), get<5>(vect1[ii]), get<6>(vect1[ii])));
				//printf("Det: %d, Face: %d, Strip: %d, Energy: %lf, Position: %lf, Timing: %lf, Mult: %d\n", get<0>(vect1[ii]), get<1>(vect1[ii]), get<2>(vect1[ii]), get<3>(vect1[ii]),  get<4>(vect1[ii]), get<5>(vect1[ii]), get<6>(vect1[ii]));
			}
			replace = false;
		}
	}
	return vect_adding; 
}

std::vector<std::tuple<int, int, int, double, double, double, int>> Matching(std::vector<tuple<int, int, int, double, double, double, int>> vect1)
{
	//printf("\nInside matching \n\n");
	std::vector<std::tuple<int,int,int,double,double,double,int>> vect_matching;
	int index[100] = {0};

        double timediff=0;
        double posdifffront=0;
        double posdiffback=0;

	int temp_front = -1;
	int temp_back = -1;

	int counter = 0;
	int counter_mult = 0;

        for (int ii = 0; ii < vect1.size(); ii++)
        {
		if (index [ii] == 1) continue;
                for (int jj = 0; jj < vect1.size(); jj++)
                {
			if (ii != jj && index[jj] == 0)
			{
				timediff  = get<5>(vect1[ii]) - get<5>(vect1[jj]);

                        	double energyfront = 0; 
				double energyback = 0;
                        	double posfront = 0; 
				double posback = 0;

                        	if (std::abs(timediff) < 15)
				{
					if (counter == 0) 
					{
						counter_mult++;
						vect_matching.push_back(make_tuple(get<0>(vect1[ii]),get<1>(vect1[ii]),get<2>(vect1[ii]),get<3>(vect1[ii]),get<4>(vect1[ii]),get<5>(vect1[ii]),counter_mult-1));
					}
					vect_matching.push_back(make_tuple(get<0>(vect1[jj]),get<1>(vect1[jj]),get<2>(vect1[jj]),get<3>(vect1[jj]),get<4>(vect1[jj]),get<5>(vect1[jj]),counter_mult-1));
					index[jj] = 1;
					counter++;
				}
			}
			posdiffback = 0;
			timediff = 0;
		}
		counter = 0;
		posdifffront = 0;
	}
	counter_mult = 0;
	return vect_matching;
}

//std::vector<std::tuple<int, int, int, double, double, double, int>> ChargeCalib(std::vector<tuple<int, int, int, double, double, double, int>> vect1)
//{
//	std::vector<std::tuple<>> vect_charge;
//
//	int ;
//
//	return vect_charge;
//}

void function0(TFile*file,TFile*file1)
{
        TTree*tree = (TTree*)file->Get("evt");//Edit file of choice here
        if(tree){std::cout<<"Event Tree Loaded"<<std::endl;}
        if(!tree){std::cout<<"Event Tree Not Loaded"<<std::endl;}
        int nHits = tree->GetEntries();
	//nHits=10000000;//Smaller Number To make life nicer
	nHits=500000;//Smaller Number To make life nicer
        std::cout << "Number of Entries in tree: " << nHits << std::endl;

        R3BEventHeader*DataCA = new R3BEventHeader();
        TBranch*branchData = tree->GetBranch("EventHeader.");
        if(branchData){std::cout<< "EventHeader Loaded"<<std::endl;}
        if(!branchData){std::cout<< "EventHeader Loaded"<<std::endl;}
        branchData->SetAddress(&DataCA);

        TClonesArray*pspxHit = new TClonesArray("R3BPspxHitData",10);
        TBranch*bpspx  = tree->GetBranch("PspxHitData");
        if(bpspx){std::cout<< "PspxHitData Loaded"<<std::endl;}
        if(!bpspx){std::cout<< "PspxHitData Loaded"<<std::endl;}
        bpspx->SetAddress(&pspxHit);

	TClonesArray*losCal;
	TTree*tree1;
	if (LOS == true)
	{
        	tree1 = (TTree*)file1->Get("evt");//Edit file of choice here
        	if(tree1){std::cout<<"Event Tree Loaded"<<std::endl;}
        	if(!tree1){std::cout<<"Event Tree Not Loaded"<<std::endl;}
        	int nHits1 = tree1->GetEntries();
		//nHits=10000000;//Smaller Number To make life nicer
		//nHits=500;//Smaller Number To make life nicer
        	std::cout << "Number of Entries in tree: " << nHits1 << std::endl;

        	losCal = new TClonesArray("R3BLosCalData",10);
        	TBranch*blos  = tree1->GetBranch("LosCal");
        	if(blos){std::cout<< "LosCal Loaded"<<std::endl;}
        	if(!blos){std::cout<< "LosCal Not Loaded"<<std::endl;}
        	blos->SetAddress(&losCal);
	}

	for(int ii=0 ; ii<nHits; ii++)
	{
		//Progress Bar
        	double per = (ii+1.)/nHits;
        	if (nHits%10000 == 0) printProgress(per);

		//Clear Array
		pspxHit->Clear();
		//Dont Forget to get your event 
		tree->GetEntry(ii);
		int pspxMaxHit = pspxHit->GetEntriesFast(); 
		
		if (LOS== true)
		{
			tree1->GetEntry(ii);
			losCal->Clear();
			int losMaxHit  = losCal->GetEntriesFast(); 
		
			if(losMaxHit>0)
			{
				R3BLosCalData** losCalData = new R3BLosCalData*[losMaxHit];
				for(int jj=0; jj<losMaxHit; jj++)
				{
					losCalData[jj]=(R3BLosCalData*)(losCal->At(jj));
                	                los_trailing = losCalData[jj]->GetMeanTimeTAMEXT();
                	                los_leading  = losCalData[jj]->GetMeanTimeTAMEXL();

                	                //printf("Tot: %lf\n", los_trailing-los_leading);

                	                los_tot.push_back(los_trailing - los_leading);

                	                lostot->Fill(los_tot[jj]);
                	                los_trailing = -999999999;
                	                los_leading = -999999999;
                	        }
                	        if(losCalData){delete[] losCalData;}
			}
		}

		if(pspxMaxHit>0)
		{
			//printf("\nInside function\n\n"); 
			std::vector<std::tuple<int, int, int, double, double, double, int, bool>> vect1;
			std::vector<std::tuple<int, int, int, double, double, double, int>> vect_adding;
			std::vector<std::tuple<int, int, int, double, double, double, int>> vect_matching;

			
			R3BPspxHitData** pspxHitData = new R3BPspxHitData*[pspxMaxHit];
			for(int jj=0; jj<pspxMaxHit; jj++)
			{
				pspxHitData[jj]=(R3BPspxHitData*)(pspxHit->At(jj));
				int timing_int = 2*(pspxHitData[jj]->GetDetector()-1)+(pspxHitData[jj]->GetFace()-1);
				vect1.push_back(make_tuple(pspxHitData[jj]->GetDetector(), pspxHitData[jj]->GetFace(), pspxHitData[jj]->GetStrip(), pspxHitData[jj]->GetEnergy(), pspxHitData[jj]->GetPosition(), (pspxHitData[jj]->GetTime()-timing[timing_int]), pspxHitData[jj]->GetMult(), pspxHitData[jj]->IsMainStrip()));
				//printf("Det: %d, Face: %d, Energy: %lf, Actual Timing: %lf, Offset: %lf, Diff: %lf, MainStrip: %d\n", pspxHitData[jj]->GetDetector(), pspxHitData[jj]->GetFace(), pspxHitData[jj]->GetEnergy(), pspxHitData[jj]->GetTime(), timing[timing_int], pspxHitData[jj]->GetTime()-timing[timing_int], pspxHitData[jj]->IsMainStrip());
			}
			if (pspxHitData) {delete[] pspxHitData;}
			vect_adding = Adding(vect1);
			vect_matching = Matching(vect_adding);
			//if (los_tot.size()==1) FillingHist(vect_matching,los_tot);
			FillingHist(vect_matching,los_tot);
			
			los_tot.clear();
			vect1.clear();
			vect_adding.clear();
			vect_matching.clear();
		}
	}	
}	


void DrawHist()
{
	TString cannom;
	if (LOS == true)
	{
		cannom = Form("los_tot_run%d",RunId);
		Los_tot = new TCanvas(cannom.Data(),cannom.Data(),500,500);
		lostot->SetStats(0);
		lostot->GetXaxis()->SetTitle("Los ToT");
		lostot->Draw("");
		Los_tot->SaveAs(".png");
		
		cannom = Form("lospsp1_run%d",RunId);
		LosPsp1 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
		lospsp1->SetStats(0);
		lospsp1->GetXaxis()->SetTitle("Los ToT");
		lospsp1->GetYaxis()->SetTitle("Psp2");
		lospsp1->Draw("colz");
		lospsp1->SetMarkerColor(3);
		lospsp1->SetMinimum(1);
		gPad->SetLogz();
		LosPsp1->SaveAs(".png");
		
		cannom = Form("lospsp2_run%d",RunId);
		LosPsp2 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
		lospsp2->SetStats(0);
		lospsp2->GetXaxis()->SetTitle("Los ToT");
		lospsp2->GetYaxis()->SetTitle("Psp2");
		lospsp2->Draw("colz");
		lospsp2->SetMarkerColor(3);
		lospsp2->SetMinimum(1);
		gPad->SetLogz();
		LosPsp2->SaveAs(".png");
		
		cannom = Form("lospsp3_run%d",RunId);
		LosPsp3 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
		lospsp3->SetStats(0);
		lospsp3->GetXaxis()->SetTitle("Los ToT");
		lospsp3->GetYaxis()->SetTitle("Psp3");
		lospsp3->Draw("colz");
		lospsp3->SetMarkerColor(3);
		lospsp3->SetMinimum(1);
		gPad->SetLogz();
		LosPsp3->SaveAs(".png");
	}

	cannom = Form("psppos_det2_run%d",RunId);
	Pspposdet2 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionDet2->SetStats(0);
	positionDet2->GetXaxis()->SetTitle("y-Position Detector 2");
	positionDet2->GetYaxis()->SetTitle("y-Position Detector 3");
	positionDet2->Draw("");
	positionDet2->SetMarkerColor(3);
	positionDet2->SetMinimum(1);
	position2Det2->SetStats(0);
	position2Det2->Draw("same");
	position2Det2->SetMarkerColor(4);
	position2Det2->SetMinimum(1);
	gPad->SetLogz();
	Pspposdet2->SaveAs(".png");
	
	cannom = Form("psppos_det3_run%d",RunId);
	Pspposdet3 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionDet3->SetStats(0);
	positionDet3->GetXaxis()->SetTitle("y-Position Detector 2");
	positionDet3->GetYaxis()->SetTitle("y-Position Detector 3");
	positionDet3->Draw("");
	positionDet3->SetMinimum(1);
	positionDet3->SetMarkerColor(3);
	position2Det3->SetStats(0);
	position2Det3->Draw("same0");
	position2Det3->SetMarkerColor(4);
	position2Det3->SetMinimum(1);
	gPad->SetLogz();
	Pspposdet3->SaveAs(".png");
	
	cannom = Form("pspposf_run%d",RunId);
	Pspposf = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionfront->SetStats(0);
	positionfront->GetXaxis()->SetTitle("y-Position Detector 2");
	positionfront->GetYaxis()->SetTitle("y-Position Detector 3");
	positionfront->Draw("COLZ");
	positionfront->SetMinimum(1);
	gPad->SetLogz();
	Pspposf->SaveAs(".png");
	
	cannom = Form("pspposb_run%d",RunId);
	Pspposb = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionback->SetStats(0);
	positionback->GetXaxis()->SetTitle("y-Position Detector 2");
	positionback->GetYaxis()->SetTitle("y-Position Detector 3");
	positionback->Draw("COLZ");
	positionback->SetMinimum(1);
	gPad->SetLogz();
	Pspposb->SaveAs(".png");
	
	cannom = Form("pspposf2ndpeak_run%d",RunId);
	Pspposf2 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionfront2->SetStats(0);
	positionfront2->GetXaxis()->SetTitle("y-Position Detector 2");
	positionfront2->GetYaxis()->SetTitle("y-Position Detector 3");
	positionfront2->Draw("COLZ");
	positionfront2->SetMinimum(1);
	gPad->SetLogz();
	Pspposf->SaveAs(".png");
	
	cannom = Form("pspposb2ndpeak_run%d",RunId);
	Pspposb2 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionback2->SetStats(0);
	positionback2->GetXaxis()->SetTitle("y-Position Detector 2");
	positionback2->GetYaxis()->SetTitle("y-Position Detector 3");
	positionback2->Draw("COLZ");
	positionback2->SetMinimum(1);
	gPad->SetLogz();
	Pspposb->SaveAs(".png");
	
	cannom = Form("pspef_run%d",RunId);
	Pspef = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energyfront->SetStats(0);
	energyfront->GetXaxis()->SetTitle("y-Position Detector 2");
	energyfront->GetYaxis()->SetTitle("y-Position Detector 3");
	energyfront->Draw("COLZ");
	energyfront->SetMinimum(1);
	gPad->SetLogz();
	Pspef->SaveAs(".png");
	
	cannom = Form("pspeb_run%d",RunId);
	Pspeb = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energyback->SetStats(0);
	energyback->GetXaxis()->SetTitle("y-Position Detector 2");
	energyback->GetYaxis()->SetTitle("y-Position Detector 3");
	energyback->Draw("COLZ");
	energyback->SetMinimum(1);
	gPad->SetLogz();
	Pspeb->SaveAs(".png");
	
	cannom = Form("pspe_det2_run%d",RunId);
	Pspedet2 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energydet2->SetStats(0);
	energydet2->GetXaxis()->SetTitle("y-Position front");
	energydet2->GetYaxis()->SetTitle("y-Position back");
	energydet2->Draw("COLZ");
	energydet2->SetMinimum(1);
	gPad->SetLogz();
	Pspedet2->SaveAs(".png");
	
	cannom = Form("pspe_det3_run%d",RunId);
	Pspedet3 = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energydet3->SetStats(0);
	energydet3->GetXaxis()->SetTitle("y-Position front");
	energydet3->GetYaxis()->SetTitle("y-Position back");
	energydet3->Draw("COLZ");
	energydet3->SetMinimum(1);
	gPad->SetLogz();
	Pspedet3->SaveAs(".png");
	
	cannom = Form("pspposftarget_run%d",RunId);
	Pspposft = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	posfronttarget->SetStats(0);
	posfronttarget->GetXaxis()->SetTitle("y-Position Detector 2");
	posfronttarget->GetYaxis()->SetTitle("y-Position Detector 3");
	posfronttarget->Draw("COLZ");
	posfronttarget->SetMinimum(1);
	gPad->SetLogz();
	Pspposft->SaveAs(".png");
	
	cannom = Form("pspposbtarget_run%d",RunId);
	Pspposbt = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	posbacktarget->SetStats(0);
	posbacktarget->GetXaxis()->SetTitle("y-Position Detector 2");
	posbacktarget->GetYaxis()->SetTitle("y-Position Detector 3");
	posbacktarget->Draw("COLZ");
	posbacktarget->SetMinimum(1);
	gPad->SetLogz();
	Pspposbt->SaveAs(".png");
	
	cannom = Form("pspeftarget_run%d",RunId);
	Pspeft = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	efronttarget->SetStats(0);
	efronttarget->GetXaxis()->SetTitle("y-Position Detector 2");
	efronttarget->GetYaxis()->SetTitle("y-Position Detector 3");
	efronttarget->Draw("COLZ");
	efronttarget->SetMinimum(1);
	gPad->SetLogz();
	Pspeft->SaveAs(".png");
	
	cannom = Form("pspebtarget_run%d",RunId);
	Pspebt = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	ebacktarget->SetStats(0);
	ebacktarget->GetXaxis()->SetTitle("y-Position Detector 2");
	ebacktarget->GetYaxis()->SetTitle("y-Position Detector 3");
	ebacktarget->Draw("COLZ");
	ebacktarget->SetMinimum(1);
	gPad->SetLogz();
	Pspebt->SaveAs(".png");
}


void PspxSort(int runId)
{
	RunId = runId;
	TString fRunId = Form("%04d",RunId);
	Hist();
	TString dir = "/lustre/land/ajedele/s473/rootfiles/PSPX/";
	TFile*file1 = new TFile(dir + "PspxLosUnpack" + fRunId + ".root","READ");
	//TFile*file  = new TFile(dir + "PspxCalib" + fRunId + ".root","READ");
	TFile*file  = new TFile(dir + "Test_Cal2Hit" + fRunId + ".root","READ");
	if(file){std::cout<<"file found"<<endl;}
	if(!file){std::cout<<"file  Not found"<<endl;}
	if(file1){std::cout<<"file1 found"<<endl;}
	if(!file1){std::cout<<"file1  Not found"<<endl;}
	FindingTime(file);
	function0(file,file1);
	DrawHist();
}
