#include <stdio.h>
#include <fstream>
#include <iostream>
#include <TPad.h>
#include <TFile.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH2.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TProfile.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>


void PspxC2HCan()
{
	int numDet = 6;
	int numStrip = 32;
	int numExpt = 473;

	ofstream outputfile;
	ofstream outputfile1;
	outputfile.open("PspxCal2HitParEPos.par");

	TString cnom;
	TCanvas *CanPspx[numDet][2][numStrip];

	TString histName;
	TH2D *RawEnergy[numDet][2][numStrip];
	TH2D *RawEnergyPars[numDet][2][numStrip];

	TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TFile *file1 = new TFile(Form("%sTest_Cal2HitPar0184-0193.root",path.Data()));

	//if numExpt == 444
	//double maxenergy[10] = {380000., 22000., 230000., 13000., 270000., 15000.};
	// numExpt == 473
	double maxenergy[4][6] = {{300000., 18000., 450000., 26000., 450000., 25000.},
		{260000., 14000., 390000., 23000., 380000., 21000.},
		{240000., 13000., 370000., 21000., 350000., 20000.},
		{230000., 12000., 340000., 19000., 330000., 18000.}};
	
	for (int ii = 0; ii < numDet; ii++)
	{
		double range;
        	if (ii%2==0) range = 25000;
		else range = 1500;
		for (int jj = 0; jj < 2; jj++)
		{
			for (int kk=0; kk<numStrip; kk++)
			{
				histName = Form("RawEnergy_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);					
				RawEnergy[ii][jj][kk] = (TH2D*)(file1->FindObjectAny(histName));
				if (RawEnergy[ii][jj][kk] != NULL) 
				{
					TGraphErrors *tgm = new TGraphErrors();

					RawEnergy[ii][jj][kk]->Rebin2D(2);
					if (ii%2==0) RawEnergy[ii][jj][kk]->GetYaxis()->SetRangeUser(100000,700000);
					else if (ii%2==1) RawEnergy[ii][jj][kk]->GetYaxis()->SetRangeUser(10000,40000);
					
					int entries = 0;
					double totalmax = 0;
					int binnum = 100;
					int counter =0;
					double minx=0.; double maxx=0.;

					TProfile *prof = RawEnergy[ii][jj][kk]->ProfileX();
					for (int ll=0; ll<prof->GetNbinsX(); ll++)
					{
						TH1D * proj = RawEnergy[ii][jj][kk]->ProjectionY("proj", ll, ll, "");
						int binmax = proj->GetMaximumBin();
						double max = proj->GetXaxis()->GetBinCenter(binmax);
						//printf("Bin: %d XX: %lf YY: %lf\n", ll, prof->GetBinCenter(ll+1), max);
						if (proj->GetEntries()>3000)
						{
							if (counter==0) minx = prof->GetBinCenter(ll);
							maxx = prof->GetBinCenter(ll);
							if (max>0.8*maxenergy[0][ii] && max<1.2*maxenergy[0][ii]) 
							{	
								tgm->SetPoint(counter,prof->GetBinCenter(ll),max);
								tgm->SetPointError(counter,0,proj->GetStdDev());
								counter++;
							}
							if (proj->GetEntries()>entries) {totalmax = max; entries = proj->GetEntries();}
						}
					}
					if (totalmax > 0) printf("Det: %d, Face: %d, Strip: %d, Entries: %d, TotalMax: %lf, mainx: %lf, maxx: %lf\n", ii, jj, kk, entries, totalmax, minx, maxx);
					double minimum =  totalmax - range;
					double maximum =  totalmax + range;
			
					
					for (int igraph = 0; igraph<tgm->GetN(); igraph++) 
					{
						printf("X: %lf, Y: %lf, Error: %lf\n", tgm->GetPointX(igraph), tgm->GetPointY(igraph), tgm->GetErrorY(igraph));
					       	if (tgm->GetPointY(igraph)<minimum || tgm->GetPointY(igraph)>maximum) tgm->RemovePoint(igraph);
					}

					if (tgm->GetN()>5)
					{
						TF1* QuadFit = new TF1("Quadratic Fit", "pol2", minx, maxx);
						QuadFit->SetParLimits(0,minimum,maximum);
						QuadFit->SetParLimits(2,0,5000000);
						tgm->Fit(QuadFit, "B");
						QuadFit->SetLineWidth(2);
				
						TFitResultPtr fitinfo = tgm->Fit(QuadFit, "");
						int fitstatus = fitinfo;
						if (fitstatus >= 0) {outputfile<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<QuadFit->GetParameter(0)<<"\t"<<QuadFit->GetParameter(1)<<"\t"<<QuadFit->GetParameter(2)<<"\t"<<entries<<endl;}
						
						//TString cannom = Form("EnergyCorrectionDet%dFace%dStrip%d",ii+1,jj+1,kk+1);
						//CanPspx[ii][jj][kk] = new TCanvas(cannom.Data(),cannom.Data(),500,500);
						//RawEnergy[ii][jj][kk]->Draw("colz");
						//gPad->SetLogz();
						//tgm->Draw("same P");
						//QuadFit->Draw("same L");
					}
					minimum = 0; maximum = 0;
					counter = 0; entries = 0;
				}
			}
		}
	}
	outputfile.close();
		
	double maxstrip[12] = {12.,17.,12.,17.,17.,19.,17.,19.,17.,19.,17.,19.};
	for (int ipars=0; ipars<4; ipars++)
	{	
		printf("\n********************************************************\nRun Set: %d\n********************************************************\n",ipars);
		TFile *file2;
       	
		if (ipars==0) {file2 = new TFile(Form("%sTest_Cal2HitPar0184-0193.root",path.Data())); outputfile1.open("PspXCal2HitPar0184-0193.par");}
		else if (ipars==1) {file2 = new TFile(Form("%sTest_Cal2HitPar0211-0212.root",path.Data())); outputfile1.open("PspXCal2HitPar0211-0212.par");}
		else if (ipars==2) {file2 = new TFile(Form("%sTest_Cal2HitPar0220.root",path.Data())); outputfile1.open("PspXCal2HitPar0220.par");}
		else {file2 = new TFile(Form("%sTest_Cal2HitPar0229.root",path.Data())); outputfile1.open("PspXCal2HitPar0229.par");}

		for (int ii = 0; ii < numDet; ii++)
		{
			double range;
        		if (ii%2==0) range = 20000;
			else range = 2000;
			for (int jj = 0; jj < 2; jj++)
			{
				for (int kk=0; kk<numStrip; kk++)
				//for (int kk=15; kk<20; kk++)
				{
					histName = Form("RawEnergy_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);					
					RawEnergyPars[ii][jj][kk] = (TH2D*)(file2->FindObjectAny(histName));
					if (RawEnergyPars[ii][jj][kk] != NULL) 
					{
						RawEnergyPars[ii][jj][kk]->Rebin2D(2);
						if (ii%2==0) RawEnergyPars[ii][jj][kk]->GetYaxis()->SetRangeUser(200000,700000);
						else if (ii%2==1) RawEnergyPars[ii][jj][kk]->GetYaxis()->SetRangeUser(10000,40000);
						int entries = 0;
						double totalmax = 0;
						double totalpos = 0;
						double totalmaxerror = 0;

						TProfile *prof = RawEnergyPars[ii][jj][kk]->ProfileX();
						//printf("entries: %d\n",prof->GetNbinsX());
						double totalmaxpos = prof->GetNbinsX()/2. + prof->GetNbinsX()/2 * ((maxstrip[2*ii+jj] - 16.)/16.);
						int maxpos = (int)totalmaxpos;

						TH1D * proj = RawEnergyPars[ii][jj][kk]->ProjectionY("proj", maxpos+1, maxpos+1, "");
						int binmax = proj->GetMaximumBin();
						double max = proj->GetXaxis()->GetBinCenter(binmax);
					
						printf("Det: %d, Face: %d, Strip: %d, max: %lf, maxenergy: %lf, entries: %f\n",ii,jj,kk,max, maxenergy[ipars][ii], proj->GetEntries());	
						if (max<1.2*maxenergy[ipars][ii] && max>0.8*maxenergy[ipars][ii] && proj->GetEntries()>1000)
						{
							TString cannom = Form("EnergyCorrectionDet%dFace%dStrip%d",ii+1,jj+1,kk+1);
							CanPspx[ii][jj][kk] = new TCanvas(cannom.Data(),cannom.Data(),500,500);
							
							totalmax = max; entries = proj->GetEntries(); totalpos = prof->GetBinCenter(maxpos);
							totalmaxerror = proj->GetStdDev();
						
							RawEnergyPars[ii][jj][kk]->Draw("colz");
							TGraphErrors *gr1 = new TGraphErrors();
							gr1->SetPoint(0, totalpos, totalmax);
							gr1->SetPointError(0, 0, totalmaxerror);
							gr1->Draw("same P");
							gr1->SetLineWidth(3);
							gr1->SetMarkerStyle(20);
						}
						if (totalmax > 0) outputfile1<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<totalpos<<"\t"<<totalmax<<"\t"<<totalmaxerror<<"\t"<<entries<<endl;
						if (totalmax > 0) printf("Det: %d, Face: %d, Strip: %d totalpos: %lf max value: %lf error: %lf entries: %d\n",ii,jj,kk,totalpos,totalmax,totalmaxerror,entries);

						totalmax=0; entries=0; totalpos=0; totalmaxpos=0; totalmaxerror=0; max=0; binmax=0; maxpos=0;
					}
				}
			}
		}
		outputfile1.close();
	}
}

