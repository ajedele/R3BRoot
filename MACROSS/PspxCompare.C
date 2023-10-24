//==========================================
//Hist Initialised
const int strip = 32;
TH2D *EnergyFront[strip][strip];
TH2D *EnergyBack[strip][strip];
TH2D *PositionFront[strip][strip];
TH2D *PositionBack[strip][strip];
TH2D *energyfront;
TH2D *energyback;
TH2D *positionfront;
TH2D *positionback;
TH2D *efronttarget;
TH2D *ebacktarget;
TH2D *posfronttarget;
TH2D *posbacktarget;
TCanvas *pspposf[strip][strip];
TCanvas *pspposb[strip][strip];
TCanvas *pspef[strip][strip];
TCanvas *pspeb[strip][strip];
TCanvas *Pspposf;
TCanvas *Pspposb;
TCanvas *Pspef;
TCanvas *Pspeb;
TCanvas *Pspposft;
TCanvas *Pspposbt;
TCanvas *Pspeft;
TCanvas *Pspebt;

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

void Hist()
{
	TString nom;
	nom = Form("PositionFront");
	positionfront = new TH2D(nom.Data(), nom.Data(), 1000,-2.0,2.0,1000,-2.0,2.0);
	nom = Form("PositionBack");
	positionback  = new TH2D(nom.Data(), nom.Data(), 1000,-2.0,2.0,1000,-2.0,2.0);
	nom = Form("EnergyFront");
	energyfront = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
	nom = Form("EnergyBack");
	energyback  = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
	nom = Form("PositionFrontTarget");
	posfronttarget = new TH2D(nom.Data(), nom.Data(), 1000,-2.0,2.0,1000,-2.0,2.0);
	nom = Form("PositionBackTarget");
	posbacktarget  = new TH2D(nom.Data(), nom.Data(), 1000,-2.0,2.0,1000,-2.0,2.0);
	nom = Form("EnergyFrontTarget");
	efronttarget = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
	nom = Form("EnergyBackTarget");
	ebacktarget  = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
	
	for (int ii = 0; ii<strip; ii++)
	{
		for (int jj = 0; jj<strip; jj++)
		{
			TString nom;
			nom = Form("PositionFront_Strip%d_Strip%d",ii,jj);
			PositionFront[ii][jj] = new TH2D(nom.Data(), nom.Data(), 1000,-2.0,2.0,1000,-2.0,2.0);
			nom = Form("PositionBack_Strip%d_Strip%d",ii,jj);
			PositionBack[ii][jj]  = new TH2D(nom.Data(), nom.Data(), 1000,0.01,2.0,1000,0.01,2.0);
			nom = Form("EnergyFront_Strip%d_Strip%d",ii,jj);
			EnergyFront[ii][jj] = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
			nom = Form("EnergyBack_Strip%d_Strip%d",ii,jj);
			EnergyBack[ii][jj]  = new TH2D(nom.Data(), nom.Data(), 700,0,700,700,0,700);
		}
	}
}

void function0(TFile*file)
{
        TTree*tree = (TTree*)file->Get("evt");//Edit file of choice here
        if(tree){std::cout<<"Event Tree Loaded"<<std::endl;}
        if(!tree){std::cout<<"Event Tree Not Loaded"<<std::endl;}
        int nHits = tree->GetEntries();
	nHits=10000000;//Smaller Number To make life nicer
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

	for(int ii=0 ; ii<nHits; ii++)
	{
		//Progress Bar
        	double per = (ii+1.)/nHits;
        	if (nHits%10000 == 0) printProgress(per);

		//Clear Array
		pspxHit->Clear();
		//Dont Forget to get your event 
		tree->GetEntry(ii);
		int MaxHit = pspxHit->GetEntriesFast(); 
		//printf("MaxHits: %d\n", MaxHit);

		double frontEbt = 0;
		double frontE1 = 0;
		double frontE2 = 0;
		double backEbt = 0;
		double backE1 = 0;
		double backE2 = 0;
		double frontposbt = -3;
		double frontpos1 = -3;
		double frontpos2 = -3;
		double backposbt = 0;
		double backpos1 = 0;
		double backpos2 = 0;
		int frontstripbt = -3;
		int frontstrip1 = -3;
		int frontstrip2 = -3;
		int backstripbt = 0;
		int backstrip1 = 0;
		int backstrip2 = 0;
		if(MaxHit>0)
		{
			R3BPspxHitData** pspxHitData = new R3BPspxHitData*[MaxHit];
			for(Int_t jj = 0; jj<MaxHit; jj++){
				pspxHitData[jj]=(R3BPspxHitData*)(pspxHit->At(jj));

				if (pspxHitData[jj]->GetDetector()==2) 
				{
					if (pspxHitData[jj]->GetFace()==1) 
					{
						frontposbt = pspxHitData[jj]->GetPosition();
					       	frontEbt = pspxHitData[jj]->GetEnergy();
				       		frontstripbt = pspxHitData[jj]->GetStrip();
					}
					else 
					{	
						backposbt = pspxHitData[jj]->GetPosition();
					       	backEbt = pspxHitData[jj]->GetEnergy();
				       		backstripbt = pspxHitData[jj]->GetStrip();
					}
				}
				if (pspxHitData[jj]->GetDetector()==4) 
				{
					if (pspxHitData[jj]->GetFace()==1) 
					{
						frontpos1 = pspxHitData[jj]->GetPosition();
					       	frontE1 = pspxHitData[jj]->GetEnergy();
				       		frontstrip1 = pspxHitData[jj]->GetStrip();
					}
					else 
					{	
						backpos1 = pspxHitData[jj]->GetPosition();
					       	backE1 = pspxHitData[jj]->GetEnergy();
				       		backstrip1 = pspxHitData[jj]->GetStrip();
					}
				}
				if (pspxHitData[jj]->GetDetector()==6) 
				{
					if (pspxHitData[jj]->GetFace()==1) 
					{
						frontpos2 = pspxHitData[jj]->GetPosition();
				       		frontE2 = pspxHitData[jj]->GetEnergy();
				       		frontstrip2 = pspxHitData[jj]->GetStrip();
					}
					else 
					{	
						backpos2 = pspxHitData[jj]->GetPosition();
				       		backE2 = pspxHitData[jj]->GetEnergy();
				       		backstrip2 = pspxHitData[jj]->GetStrip();
					}
				}
			}
			if (frontstrip1>0 && frontstripbt>0) 
			{
				posfronttarget->Fill(frontposbt, frontpos1);
				posbacktarget->Fill(backposbt, backpos1);
				efronttarget->Fill(frontEbt, frontE1);
				ebacktarget->Fill(backEbt, backE1);
			}	

			if (frontstrip1>0 && frontstrip2>0) 
			{
				PositionFront[frontstrip1-1][frontstrip2-1]->Fill(frontpos1, frontpos2);	
				positionfront->Fill(frontpos1, frontpos2);
				PositionBack[backstrip1-1][backstrip2-1]->Fill(backpos1, backpos2);
				positionback->Fill(backpos1, backpos2);
				EnergyFront[frontstrip1-1][frontstrip2-1]->Fill(frontE1, frontE2);
				energyfront->Fill(frontE1, frontE2);
				EnergyBack[backstrip1-1][backstrip2-1]->Fill(backE1, backE2);
				energyback->Fill(backE1, backE2);
			}	

			if(pspxHitData){delete[] pspxHitData;}
		}
		frontpos1 = -99;
		frontpos2 = -99;
		backpos1 = -99;
		backpos2 = -99;
		frontE1 = -999999;
		frontE2 = -999999;
		backE1 = -999999;
		backE2 = -999999;
	}
}

void DrawHist()
{
	TString cannom;
	cannom = Form("pspposf");
	Pspposf = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionfront->SetStats(0);
	positionfront->GetXaxis()->SetTitle("y-Position Detector 2");
	positionfront->GetYaxis()->SetTitle("y-Position Detector 3");
	positionfront->Draw("COLZ");
	gPad->SetLogz();
	//Pspposf->SaveAs(".png");
	
	cannom = Form("pspposb");
	Pspposb = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	positionback->SetStats(0);
	positionback->GetXaxis()->SetTitle("y-Position Detector 2");
	positionback->GetYaxis()->SetTitle("y-Position Detector 3");
	positionback->Draw("COLZ");
	gPad->SetLogz();
	//Pspposb->SaveAs(".png");
	
	cannom = Form("pspef");
	Pspef = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energyfront->SetStats(0);
	energyfront->GetXaxis()->SetTitle("y-Position Detector 2");
	energyfront->GetYaxis()->SetTitle("y-Position Detector 3");
	energyfront->Draw("COLZ");
	gPad->SetLogz();
	//Pspef->SaveAs(".png");
	
	cannom = Form("pspeb");
	Pspeb = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	energyback->SetStats(0);
	energyback->GetXaxis()->SetTitle("y-Position Detector 2");
	energyback->GetYaxis()->SetTitle("y-Position Detector 3");
	energyback->Draw("COLZ");
	gPad->SetLogz();
	//Pspeb->SaveAs(".png");
	
	cannom = Form("pspposft");
	Pspposft = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	posfronttarget->SetStats(0);
	posfronttarget->GetXaxis()->SetTitle("y-Position Detector 2");
	posfronttarget->GetYaxis()->SetTitle("y-Position Detector 3");
	posfronttarget->Draw("COLZ");
	gPad->SetLogz();
	//Pspposf->SaveAs(".png");
	
	cannom = Form("pspposbt");
	Pspposbt = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	posbacktarget->SetStats(0);
	posbacktarget->GetXaxis()->SetTitle("y-Position Detector 2");
	posbacktarget->GetYaxis()->SetTitle("y-Position Detector 3");
	posbacktarget->Draw("COLZ");
	gPad->SetLogz();
	//Pspposb->SaveAs(".png");
	
	cannom = Form("pspeft");
	Pspeft = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	efronttarget->SetStats(0);
	efronttarget->GetXaxis()->SetTitle("y-Position Detector 1");
	efronttarget->GetYaxis()->SetTitle("y-Position Detector 2");
	efronttarget->Draw("COLZ");
	gPad->SetLogz();
	//Pspef->SaveAs(".png");
	
	cannom = Form("pspebt");
	Pspebt = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	ebacktarget->SetStats(0);
	ebacktarget->GetXaxis()->SetTitle("y-Position Detector 1");
	ebacktarget->GetYaxis()->SetTitle("y-Position Detector 2");
	ebacktarget->Draw("COLZ");
	gPad->SetLogz();
	//Pspeb->SaveAs(".png");
	
	//for (int istrip1=0; istrip1<strip; istrip1++)
	//{
	//	for (int istrip2=0; istrip2<strip; istrip2++)
	//	{
	//		if (PositionFront[istrip1][istrip2]->GetEntries()>100)
	//		{
	//	        	cannom = Form("pspposf_strip%d_strip%d",istrip1,istrip2);
	//			pspposf[istrip1][istrip2] = new TCanvas(cannom.Data(),cannom.Data(),500,500);
	//			PositionFront[istrip1][istrip2]->SetStats(0);
	//			PositionFront[istrip1][istrip2]->GetXaxis()->SetTitle("y-Position Detector 2");
	//			PositionFront[istrip1][istrip2]->GetYaxis()->SetTitle("y-Position Detector 3");
	//			//PositionFront[istrip1][istrip2]->Draw("COLZ");
	//			gPad->SetLogz();
	//			//pspposf[istrip1][istrip2]->SaveAs(".png");
	//		}

	//		if (PositionBack[istrip1][istrip2]->GetEntries()>100)
	//		{
	//			cannom = Form("pspposb_strip%d_strip%d",istrip1,istrip2);
	//			pspposb[istrip1][istrip2] = new TCanvas(cannom.Data(), cannom.Data(),500,500);
	//			PositionBack[istrip1][istrip2]->SetStats(0);
	//			PositionBack[istrip1][istrip2]->GetXaxis()->SetTitle("x-Position Detector 2");
	//			PositionBack[istrip1][istrip2]->GetYaxis()->SetTitle("x-Position Detector 3");
	//			//PositionBack[istrip1][istrip2]->Draw("COLZ");
	//			gPad->SetLogz();
	//			//pspposb[istrip1][istrip2]->SaveAs(".png");
	//		}

	//		if (EnergyFront[istrip1][istrip2]->GetEntries()>100)
	//		{
	//			cannom = Form("pspef_strip%d_strip%d",istrip1,istrip2);
	//			pspef[istrip1][istrip2] = new TCanvas(cannom.Data(), cannom.Data(),500,500);
	//			EnergyFront[istrip1][istrip2]->SetStats(0);
	//			EnergyFront[istrip1][istrip2]->GetXaxis()->SetTitle("Energy from Front Face Detector 2");
	//			EnergyFront[istrip1][istrip2]->GetYaxis()->SetTitle("Energy from Front Face Detector 3");
	//			//EnergyFront[istrip1][istrip2]->Draw("COLZ");
	//			gPad->SetLogz();
	//			//pspef[istrip1][istrip2]->SaveAs(".png");
	//		}

	//		if (EnergyBack[istrip1][istrip2]->GetEntries()>100)
	//		{
	//			cannom = Form("pspeb_strip%d_strip%d",istrip1,istrip2);
	//			pspeb[istrip1][istrip2] = new TCanvas(cannom.Data(), cannom.Data(),500,500);
	//			EnergyBack[istrip1][istrip2]->SetStats(0);
	//			EnergyBack[istrip1][istrip2]->GetXaxis()->SetTitle("Energy from Back Face Detector 2");
	//			EnergyBack[istrip1][istrip2]->GetYaxis()->SetTitle("Energy from Back Face Detector 3");
	//			//EnergyBack[istrip1][istrip2]->Draw("COLZ");
	//			gPad->SetLogz();
	//			//pspeb[istrip1][istrip2]->SaveAs(".png");
	//		}
	//	}
	//}
}

void PspxCompare(int RunId)
{
	TString fRunId = Form("%04d",RunId);
	Hist();
	TString dir = "/lustre/land/ajedele/s473/rootfiles/";
	TFile*file = new TFile(dir + "Test_Cal2Hit" + fRunId + ".root","READ");
	if(file){std::cout<<"file found"<<endl;}
	if(!file){std::cout<<"file  Not found"<<endl;}
	function0(file);
	DrawHist();
}
