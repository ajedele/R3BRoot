#include <stdio.h>
#include <fstream>
#include <iostream>
#include <TPad.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TLatex.h>
#include <TProfile.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TMath.h>

double fitf(double *x, double *par)
{
	double fitval = par[0] + par[1]*TMath::Tan(x[0]);
	return fitval;
}

void PspxP2CFits(int runstart, int runend)
{
	int numDet = 6;
	int numStrip = 32;

	TString cannom;
	TCanvas *CanPspxX[numDet];
	TCanvas *CanPspxY[numDet];

	TString histName;
	TH1D *hPosX[numDet][numStrip][numStrip];
	TH1D *hPosY[numDet][numStrip][numStrip];

	TH2D *BackgroundX = new TH2D("","",100,-0.1,0.5,100,-0.1,0.5); 
	TH2D *BackgroundY = new TH2D("","",100,-0.4,0.4,100,-0.4,0.4); 
	BackgroundX->SetStats(0);
	BackgroundY->SetStats(0);
	BackgroundX->SetTitle("");
	BackgroundY->SetTitle("");
	BackgroundX->GetXaxis()->CenterTitle();
	BackgroundY->GetXaxis()->CenterTitle(); 
	BackgroundX->GetYaxis()->CenterTitle(); 
	BackgroundY->GetYaxis()->CenterTitle(); 
	BackgroundX->GetYaxis()->SetTitle("Position from Strip Position");
	BackgroundY->GetYaxis()->SetTitle("Position from Strip Position");
	BackgroundX->GetXaxis()->SetTitle("Position calculated from Channel #");
	BackgroundY->GetXaxis()->SetTitle("Position calculated from Channel #");

	ofstream outputfile2;
	TString myname = Form("Poscal2CalPar%04d-%04d.txt",runstart,runend);
	outputfile2.open(myname.Data());

	ofstream outputfile;
	myname = Form("ParTest%04d-%04d.txt",runstart,runend);
	outputfile.open(myname.Data());

	TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TFile *file1 = new TFile(Form("%sTest_Poscal2CalPar%04d-%04d.root",path.Data(),runstart,runend));
	//TFile *file1 = new TFile(Form("%sTest_Poscal2CalPar%04d.root",path.Data(),runstart));

	for (int ii=0; ii<numDet; ii++)
	//for (int ii=5; ii<6; ii++)
	{
		printf("***************************************************************\n Detector: %d \n***************************************************************\n",ii);
	
		cannom = Form("PosX_Det%d",ii+1);
		CanPspxX[ii] = new TCanvas(cannom.Data(),cannom.Data(),800,800);
		gPad->SetLeftMargin(0.15);
      		cannom = Form("PosY_Det%d",ii+1);
      		CanPspxY[ii] = new TCanvas(cannom.Data(),cannom.Data(),800,800);
		gPad->SetLeftMargin(0.15);
	
		CanPspxX[ii]->cd(); BackgroundX->Draw(); 
		CanPspxY[ii]->cd(); BackgroundY->Draw(); 
		
		for (int ll=0; ll < numStrip; ll++)
		{
			int color;
			if (ll<19) color = ll-9;
			else color = ll-18;
			double labelx =  0.45 - 0.03 * (ll-10);
			double labely =  0.15 - 0.04 * (ll-10);
			TLatex *LabelX = new TLatex(0.0,labelx,Form("#color[%d]{#bullet} %d",color,ll+1));
			TLatex *LabelY = new TLatex(0.2,labely,Form("#color[%d]{#bullet} %d",color,ll+1));
			printf("PosY: Front Strip: %d\n",ll+1);
			int xcounter1=0;
			
			TGraphErrors *graph1 = new TGraphErrors();
			TGraphErrors *graph2 = new TGraphErrors();
			int countY = 0; int countX = 0;
			for (int kk=0; kk < numStrip; kk++)
			{
				//x-position calc from face 1 signals and is (E2-E1)/(E1+E2)
				histName = Form("PosX_Det%dStripFront%dStripBack%d",ii+1,ll+1,kk+1);					
				hPosX[ii][ll][kk] = (TH1D*)(file1->FindObjectAny(histName));
				if (hPosX[ii][ll][kk] != NULL)
				{
					if (hPosX[ii][ll][kk]->GetEntries() > 100)
					{
						hPosX[ii][ll][kk]->Scale(1./hPosX[ii][ll][kk]->GetEntries());
						double tempYY = hPosX[ii][ll][kk]->GetBinCenter(hPosX[ii][ll][kk]->GetMaximumBin());
						double posXX = ((double)kk+1.-16.)/16.;
						
						double res = 0.02; 
						TF1 *GausFit = new TF1("Gaussian Fit","gaus(0)",(tempYY-0.05),(tempYY+0.05));
						GausFit->SetParLimits(1,tempYY-res,tempYY+res);
						hPosX[ii][ll][kk]->Fit(GausFit,"NRE","");
						double YY = -GausFit->GetParameter(1);
						double Error = GausFit->GetParError(1);

						double offset = (std::abs(posXX-YY));
						//printf("Offset: %lf\n", offset);
						//if (offset < 0.10) 
						//{
							double ratio = (1-YY)/(1+YY);
							double constant = ratio*(posXX+1)/(1-posXX);
							//printf("Inside Loop - Strip: %d, XX: %lf, tempYY: %lf, YY: %lf, count: %d\n",kk+1,posXX,tempYY,YY, countX);
							//printf("Ratio: %lf, Constant: %lf\n",ratio, constant);
							outputfile2 << ii <<"\t"<< 0 <<"\t"<< kk <<"\t"<< ll <<"\t"<< YY <<"\t"<< constant << endl;
							graph1->SetPoint(countX, posXX, YY);
							graph1->SetPointError(countX, 0, Error);
							countX++;
						//}
						YY=0; posXX=0; offset=0;
					}
				}
				
				//y-position calc from face 2 signals and is (E1-E2)/(E1+E2)
				histName = Form("PosY_Det%dStripFront%dStripBack%d",ii+1,ll+1,kk+1);					
				hPosY[ii][ll][kk] = (TH1D*)(file1->FindObjectAny(histName));
				if (hPosY[ii][ll][kk] != NULL)
				{
					if (hPosY[ii][ll][kk]->GetEntries() > 100)
					{
						hPosY[ii][ll][kk]->Scale(1./hPosY[ii][ll][kk]->GetEntries());
						double tempYY = hPosY[ii][ll][kk]->GetBinCenter(hPosY[ii][ll][kk]->GetMaximumBin());
						double posXX = ((double)kk+1.-16.)/16.;
						
						double res = 0.02; 
						TF1 *GausFit = new TF1("Gaussian Fit","gaus(0)",(tempYY-0.05),(tempYY+0.05));
						GausFit->SetParLimits(1,tempYY-res,tempYY+res);
						hPosY[ii][ll][kk]->Fit(GausFit,"NQRE","");
						double YY = GausFit->GetParameter(1);
						double Error = GausFit->GetParError(1);

						double offset = (std::abs(posXX-YY));
						if (offset < 0.10) 
						{
							double ratio = (1+YY)/(1-YY);
							double constant = ratio*(1-posXX)/(1+posXX);
							graph2->SetPoint(countY, posXX, YY);
							graph2->SetPointError(countY, 0, Error);
							outputfile2 << ii <<"\t"<< 1 <<"\t"<< kk <<"\t"<< ll <<"\t"<< YY <<"\t"<< constant << endl;
							countY++;
						}
						YY=0; posXX=0; offset=0;
					}
				}
			}
			CanPspxX[ii]->cd(); 
			graph1->Draw("P");  
			if (graph1->GetN()>0) LabelX->Draw(); 
			graph1->SetMarkerColor(color); 
			graph1->SetMarkerStyle(20); 
			graph1->SetLineColor(color); 
			graph1->SetLineWidth(2); 

			int fitstatus1 = 10;
			double parameter11 = 0;
			if (graph1->GetN()>3)
			{
				TF1 *f12 = new TF1("f12",fitf,-0.5,0.5,2);
				graph1->Fit(f12,"");
				f12->SetLineColor(color);
				TFitResultPtr fitinfo = graph1->Fit(f12, "");
				fitstatus1 = fitinfo;
				printf("FitStatus: %d\n",fitstatus1);
				if (fitstatus1 >= 0) 
				{
					f12->Draw("same");
					//cout<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f12->GetParameter(0)<<"\t"<<f12->GetParameter(1)<<"\t"<<f12->GetParameter(2)<<"\t"<<f12->GetParameter(3)<<endl;
					outputfile<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f12->GetParameter(0)<<"\t"<<f12->GetParameter(1)<<"\t"<<0<<endl;
				}
			}
			if ((graph1->GetN()>1 && graph1->GetN()<4) || (graph1->GetN()>3 && fitstatus1 < 0))
			{
				TF1 *f11= new TF1("f11","[0]+[1]*x",-0.5,0.5);
				f11->SetParLimits(1,0.8,1.2);
				graph1->Fit(f11,"Q");
				f11->SetLineColor(color);
				f11->Draw("same");
				//cout<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f11->GetParameter(0)<<"\t"<<f11->GetParameter(1)<<endl;
				parameter11 = f11->GetParameter(1);
				if (parameter11 > 0.85 || parameter11 < 1.15) outputfile<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f11->GetParameter(0)<<"\t"<<f11->GetParameter(1)<<"\t"<<1<<endl;
			}

			CanPspxY[ii]->cd(); 
			graph2->Draw("same P"); 
		       	if (graph2->GetN()>0) LabelY->Draw(); 
			graph2->SetMarkerColor(color); 
			graph2->SetMarkerStyle(20);
			graph2->SetLineColor(color); 
			graph2->SetLineWidth(2);
			
			int fitstatus2 = 10;
			if (graph2->GetN()>3)
			{
				TF1 *f22 = new TF1("f22",fitf,-0.5,0.5,2);
				//TF1 *f22 = new TF1("f22","pol3",-0.5,0.5);
				graph2->Fit(f22,"");
				f22->SetLineColor(color);
				TFitResultPtr fitinfo = graph2->Fit(f22, "");
				fitstatus2 = fitinfo;
				if (fitstatus2 >= 0) 
				{
					f22->Draw("same");
					//cout<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f22->GetParameter(0)<<"\t"<<f22->GetParameter(1)<<"\t"<<f22->GetParameter(2)<<"\t"<<f22->GetParameter(3)<<endl;
					outputfile<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f22->GetParameter(0)<<"\t"<<f22->GetParameter(1)<<"\t"<<0<<endl;
				}
			}
			if ((graph2->GetN()>1 && graph2->GetN()<4) || (graph2->GetN()>3 && fitstatus2 < 0))
			{
				TF1 *f21 = new TF1("f21","[0]+[1]*x",-0.5,0.5);
				f21->SetParLimits(1,0.8,1.2);
				graph2->Fit(f21,"Q");
				f21->SetLineColor(color);
				f21->Draw("same");
				//cout<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f21->GetParameter(0)<<"\t"<<f21->GetParameter(1)<<endl;
				double parameter21 = f21->GetParameter(1);
				if (parameter21 > 0.85 || parameter21 < 1.15) outputfile<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f21->GetParameter(0)<<"\t"<<f21->GetParameter(1)<<"\t"<<1<<endl;
			}
			fitstatus1 = 10;
			fitstatus2 = 10;
		}
		//CanPspxX[ii]->SaveAs(".png");
		//CanPspxY[ii]->SaveAs(".png");
	}
}
