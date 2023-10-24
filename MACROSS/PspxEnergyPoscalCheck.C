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


void PspxEnergyPoscalCheck()
{
	Int_t numDet = 6;
	Int_t numStrip = 32;
	Int_t color = 0;

	TCanvas *CanFace1Side1[numDet];
	TCanvas *CanFace1Side2[numDet];
	TCanvas *CanFace2Side1[numDet];
	TCanvas *CanFace2Side2[numDet];

	TCanvas *CanF1S1[numDet];
	TCanvas *CanF1S2[numDet];
	TCanvas *CanF2S1[numDet];
	TCanvas *CanF2S2[numDet];

	TString histName;
	TString canName;
	TH2F *hEnergyFace1Side1[numDet][numStrip];
	TH2F *hEnergyFace1Side2[numDet][numStrip];
	TH2F *hEnergyFace2Side1[numDet][numStrip];
	TH2F *hEnergyFace2Side2[numDet][numStrip];
	
	TH2F *hF1S1 = new TH2F("","",20,-1,1,1000,0,50000);
	TH2F *hF1S2 = new TH2F("","",20,-1,1,1000,0,50000);
	TH2F *hF2S1 = new TH2F("","",20,-1,1,1000,0,50000);
	TH2F *hF2S2 = new TH2F("","",20,-1,1,1000,0,50000);
	
	TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TFile *file1 = new TFile(Form("%sTest_Poscal2CalPar0194-0201.root",path.Data()));
	//TFile *file1 = new TFile(Form("%sTest_Poscal2CalPar0194.root",path.Data()));
	
	TLatex *Lab[32];
	TLatex *Title[numDet];

	for (Int_t ii=0; ii<numDet; ii++)
	{
		Int_t counter11=0; Int_t counter12=0; Int_t counter21=0; Int_t counter22=0;
		
		canName = Form("Maximum_Det%d_Face1_Side1",ii+1);	
		CanF1S1[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Maximum_Det%d_Face1_Side2",ii+1);	
		CanF1S2[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Maximum_Det%d_Face2_Side1",ii+1);	
		CanF2S1[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Maximum_Det%d_Face2_Side2",ii+1);	
		CanF2S2[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
	
		TString nomnom = Form("Detector: %d",ii+1); 
		Title[ii]	= new TLatex(0.45,0.95,nomnom.Data());
		Title[ii]->SetNDC();
		Title[ii]->SetTextSize(0.05);
					
		for (Int_t ll=0; ll < numStrip; ll++)
		{
			TGraphErrors *GraphDet1Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet2Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet3Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet4Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet5Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet6Face1Side1 = new TGraphErrors();
			TGraphErrors *GraphDet1Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet2Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet3Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet4Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet5Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet6Face1Side2 = new TGraphErrors();
			TGraphErrors *GraphDet1Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet2Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet3Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet4Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet5Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet6Face2Side1 = new TGraphErrors();
			TGraphErrors *GraphDet1Face2Side2 = new TGraphErrors();
			TGraphErrors *GraphDet2Face2Side2 = new TGraphErrors();
			TGraphErrors *GraphDet3Face2Side2 = new TGraphErrors();
			TGraphErrors *GraphDet4Face2Side2 = new TGraphErrors();
			TGraphErrors *GraphDet5Face2Side2 = new TGraphErrors();
			TGraphErrors *GraphDet6Face2Side2 = new TGraphErrors();

			histName = Form("PosvsEX_Det%dSide1Strip%d",ii+1,ll+1);	
			hEnergyFace1Side1[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("PosvsEX_Det%dSide2Strip%d",ii+1,ll+1);	
			hEnergyFace1Side2[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("PosvsEY_Det%dSide1Strip%d",ii+1,ll+1);	
			hEnergyFace2Side1[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("PosvsEY_Det%dSide2Strip%d",ii+1,ll+1);	
			hEnergyFace2Side2[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));

			if (ll==8) {color = 1;}	else if (ll==9) {color = 2;} 
			else if (ll==10) {color = 3;} else if (ll==11) {color = 4;}
			else if (ll==12) {color = 5;} else if (ll==13) {color = 6;} 
			else if (ll==14) {color = 7;} else if (ll==15) {color = 8;} 
			else if (ll==16) {color = 9;} else if (ll==17) {color = 1;} 
			else if (ll==18) {color = 2;} else if (ll==19) {color = 3;} 
			else if (ll==20) {color = 4;} else if (ll==21) {color = 5;} 
			else if (ll==22) {color = 6;} else if (ll==23) {color = 7;} 

			TString nomnom = Form("#color[%d]{#bullet} %d",color,ll); 
		      	Lab[ll]	= new TLatex(0.8,0.9-ll*0.03,nomnom.Data());
			Lab[ll]->SetNDC();
			Lab[ll]->SetTextSize(0.03);
					
			Int_t counter = 0;
			if (hEnergyFace1Side1[ii][ll] != NULL)
			{
				if (hEnergyFace1Side1[ii][ll]->GetEntries() > 100)
				{
					TProfile *profx = hEnergyFace1Side1[ii][ll]->ProfileX();
					Int_t maxentries=0;
					for (int ibin=0; ibin<profx->GetNbinsX(); ibin++)
					{
						if (profx->GetBinEntries(ibin)>0)
						{
							double YY=profx->GetBinContent(ibin);
							double Error=profx->GetBinError(ibin);
							double XX=profx->GetBinCenter(ibin)+0.005*counter11;
							printf("Hi! Det: %d, Strip: %d, XX: %lf, YY: %lf, Counter: %d\n",ii,ll,XX,YY,counter);
							
							if (ii==0) {GraphDet1Face1Side1->SetPoint(counter,XX,YY); GraphDet1Face1Side1->SetPointError(counter,0,Error);}
							else if (ii==1) {GraphDet2Face1Side1->SetPoint(counter,XX,YY); GraphDet2Face1Side1->SetPointError(counter,0,Error);}
							else if (ii==2) {GraphDet3Face1Side1->SetPoint(counter,XX,YY); GraphDet3Face1Side1->SetPointError(counter,0,Error);}
							else if (ii==3) {GraphDet4Face1Side1->SetPoint(counter,XX,YY); GraphDet4Face1Side1->SetPointError(counter,0,Error);}
							else if (ii==4) {GraphDet5Face1Side1->SetPoint(counter,XX,YY); GraphDet5Face1Side1->SetPointError(counter,0,Error);}
							else if (ii==5) {GraphDet6Face1Side1->SetPoint(counter,XX,YY); GraphDet6Face1Side1->SetPointError(counter,0,Error);}
							counter++;
						}
					}
					counter11++;
				}
			}
			CanF1S1[ii]->cd();
			printf("\n***************************\nNumber of entries: %d\n***************************\n",GraphDet1Face1Side1->GetN());
			if (ii==0) {
				if (GraphDet1Face1Side1->GetN()>0 && counter11==1) {GraphDet1Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet1Face1Side1->GetN()>0) {GraphDet1Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet1Face1Side1->SetMarkerStyle(20);
				GraphDet1Face1Side1->SetMarkerColor(color);
				GraphDet1Face1Side1->SetLineWidth(2);
				GraphDet1Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==1) {
				if (GraphDet2Face1Side1->GetN()>0 && counter11==1) {GraphDet2Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet2Face1Side1->GetN()>0) {GraphDet2Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet2Face1Side1->SetMarkerStyle(20);
				GraphDet2Face1Side1->SetMarkerColor(color);
				GraphDet2Face1Side1->SetLineWidth(2);
				GraphDet2Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==2) {
				if (GraphDet3Face1Side1->GetN()>0 && counter11==1) {GraphDet3Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet3Face1Side1->GetN()>0) {GraphDet3Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet3Face1Side1->SetMarkerStyle(20);
				GraphDet3Face1Side1->SetMarkerColor(color);
				GraphDet3Face1Side1->SetLineWidth(2);
				GraphDet3Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==3) {
				if (GraphDet4Face1Side1->GetN()>0 && counter11==1) {GraphDet4Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet4Face1Side1->GetN()>0) {GraphDet4Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet4Face1Side1->SetMarkerStyle(20);
				GraphDet4Face1Side1->SetMarkerColor(color);
				GraphDet4Face1Side1->SetLineWidth(2);
				GraphDet4Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==4) {
				if (GraphDet5Face1Side1->GetN()>0 && counter11==1) {GraphDet5Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet5Face1Side1->GetN()>0) {GraphDet5Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet5Face1Side1->SetMarkerStyle(20);
				GraphDet5Face1Side1->SetMarkerColor(color);
				GraphDet5Face1Side1->SetLineWidth(2);
				GraphDet5Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==5) {
				if (GraphDet6Face1Side1->GetN()>0 && counter11==1) {GraphDet6Face1Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet6Face1Side1->GetN()>0) {GraphDet6Face1Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet6Face1Side1->SetMarkerStyle(20);
				GraphDet6Face1Side1->SetMarkerColor(color);
				GraphDet6Face1Side1->SetLineWidth(2);
				GraphDet6Face1Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			
			counter = 0;
			if (hEnergyFace1Side2[ii][ll] != NULL)
			{
				if (hEnergyFace1Side2[ii][ll]->GetEntries() > 100)
				{
					TProfile *profx = hEnergyFace1Side2[ii][ll]->ProfileX();
					Int_t maxentries=0;
					for (int ibin=0; ibin<profx->GetNbinsX(); ibin++)
					{
						if (profx->GetBinEntries(ibin)>0)
						{
							double YY=profx->GetBinContent(ibin);
							double XX=profx->GetBinCenter(ibin)+0.005*counter12;
							double Error=profx->GetBinError(ibin);
							printf("Hi! Det: %d, Strip: %d, XX: %lf, YY: %lf, Counter: %d\n",ii,ll,XX,YY,counter);
							
							if (ii==0) {GraphDet1Face1Side2->SetPoint(counter,XX,YY); GraphDet1Face1Side2->SetPointError(counter,0,Error);}
							else if (ii==1) {GraphDet2Face1Side2->SetPoint(counter,XX,YY); GraphDet2Face1Side2->SetPointError(counter,0,Error);}
							else if (ii==2) {GraphDet3Face1Side2->SetPoint(counter,XX,YY); GraphDet3Face1Side2->SetPointError(counter,0,Error);}
							else if (ii==3) {GraphDet4Face1Side2->SetPoint(counter,XX,YY); GraphDet4Face1Side2->SetPointError(counter,0,Error);}
							else if (ii==4) {GraphDet5Face1Side2->SetPoint(counter,XX,YY); GraphDet5Face1Side2->SetPointError(counter,0,Error);}
							else if (ii==5) {GraphDet6Face1Side2->SetPoint(counter,XX,YY); GraphDet6Face1Side2->SetPointError(counter,0,Error);}
							counter++;
						}
					}
					counter12++;
				}
			}
			CanF1S2[ii]->cd();
			if (ii==0) {
				if (GraphDet1Face1Side2->GetN()>0 && counter12==1) {GraphDet1Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet1Face1Side2->GetN()>0) {GraphDet1Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet1Face1Side2->SetMarkerStyle(20);
				GraphDet1Face1Side2->SetMarkerColor(color);
				GraphDet1Face1Side2->SetLineWidth(2);
				GraphDet1Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==1) {
				if (GraphDet2Face1Side2->GetN()>0 && counter12==1) {GraphDet2Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet2Face1Side2->GetN()>0) {GraphDet2Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet2Face1Side2->SetMarkerStyle(20);
				GraphDet2Face1Side2->SetMarkerColor(color);
				GraphDet2Face1Side2->SetLineWidth(2);
				GraphDet2Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==2) {
				if (GraphDet3Face1Side2->GetN()>0 && counter12==1) {GraphDet3Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet3Face1Side2->GetN()>0) {GraphDet3Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet3Face1Side2->SetMarkerStyle(20);
				GraphDet3Face1Side2->SetMarkerColor(color);
				GraphDet3Face1Side2->SetLineWidth(2);
				GraphDet3Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==3) {
				if (GraphDet4Face1Side2->GetN()>0 && counter12==1) {GraphDet4Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet4Face1Side2->GetN()>0) {GraphDet4Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet4Face1Side2->SetMarkerStyle(20);
				GraphDet4Face1Side2->SetMarkerColor(color);
				GraphDet4Face1Side2->SetLineWidth(2);
				GraphDet4Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==4) {
				if (GraphDet5Face1Side2->GetN()>0 && counter12==1) {GraphDet5Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet5Face1Side2->GetN()>0) {GraphDet5Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet5Face1Side2->SetMarkerStyle(20);
				GraphDet5Face1Side2->SetMarkerColor(color);
				GraphDet5Face1Side2->SetLineWidth(2);
				GraphDet5Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==5) {
				if (GraphDet6Face1Side2->GetN()>0 && counter12==1) {GraphDet6Face1Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet6Face1Side2->GetN()>0) {GraphDet6Face1Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet6Face1Side2->SetMarkerStyle(20);
				GraphDet6Face1Side2->SetMarkerColor(color);
				GraphDet6Face1Side2->SetLineWidth(2);
				GraphDet6Face1Side2->SetLineColor(color);
				Title[ii]->Draw();
			}

			counter = 0;
			if (hEnergyFace2Side1[ii][ll] != NULL)
			{
				if (hEnergyFace2Side1[ii][ll]->GetEntries() > 100)
				{
					TProfile *profx = hEnergyFace2Side1[ii][ll]->ProfileX();
					Int_t maxentries=0;
					for (int ibin=0; ibin<profx->GetNbinsX(); ibin++)
					{
						if (profx->GetBinEntries(ibin)>0)
						{
							double YY=profx->GetBinContent(ibin);
							double Error=profx->GetBinError(ibin);
							double XX=profx->GetBinCenter(ibin)+0.005*counter21;
							printf("Hi! Det: %d, Strip: %d, XX: %lf, YY: %lf, Counter: %d\n",ii,ll,XX,YY,counter);
							
							if (ii==0) {GraphDet1Face2Side1->SetPoint(counter,XX,YY); GraphDet1Face2Side1->SetPointError(counter,0,Error);}
							else if (ii==1) {GraphDet2Face2Side1->SetPoint(counter,XX,YY); GraphDet2Face2Side1->SetPointError(counter,0,Error);}
							else if (ii==2) {GraphDet3Face2Side1->SetPoint(counter,XX,YY); GraphDet3Face2Side1->SetPointError(counter,0,Error);}
							else if (ii==3) {GraphDet4Face2Side1->SetPoint(counter,XX,YY); GraphDet4Face2Side1->SetPointError(counter,0,Error);}
							else if (ii==4) {GraphDet5Face2Side1->SetPoint(counter,XX,YY); GraphDet5Face2Side1->SetPointError(counter,0,Error);}
							else if (ii==5) {GraphDet6Face2Side1->SetPoint(counter,XX,YY); GraphDet6Face2Side1->SetPointError(counter,0,Error);}
							counter++;
						}
					}
					counter21++;
				}
			}
			CanF2S1[ii]->cd();
			printf("\n***************************\nNumber of entries: %d\n***************************\n",GraphDet1Face2Side1->GetN());
			if (ii==0) {
				if (GraphDet1Face2Side1->GetN()>0 && counter21==1) {GraphDet1Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet1Face2Side1->GetN()>0) {GraphDet1Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet1Face2Side1->SetMarkerStyle(20);
				GraphDet1Face2Side1->SetMarkerColor(color);
				GraphDet1Face2Side1->SetLineWidth(2);
				GraphDet1Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==1) {
				if (GraphDet2Face2Side1->GetN()>0 && counter21==1) {GraphDet2Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet2Face2Side1->GetN()>0) {GraphDet2Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet2Face2Side1->SetMarkerStyle(20);
				GraphDet2Face2Side1->SetMarkerColor(color);
				GraphDet2Face2Side1->SetLineWidth(2);
				GraphDet2Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==2) {
				if (GraphDet3Face2Side1->GetN()>0 && counter21==1) {GraphDet3Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet3Face2Side1->GetN()>0) {GraphDet3Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet3Face2Side1->SetMarkerStyle(20);
				GraphDet3Face2Side1->SetMarkerColor(color);
				GraphDet3Face2Side1->SetLineWidth(2);
				GraphDet3Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==3) {
				if (GraphDet4Face2Side1->GetN()>0 && counter21==1) {GraphDet4Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet4Face2Side1->GetN()>0) {GraphDet4Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet4Face2Side1->SetMarkerStyle(20);
				GraphDet4Face2Side1->SetMarkerColor(color);
				GraphDet4Face2Side1->SetLineWidth(2);
				GraphDet4Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==4) {
				if (GraphDet5Face2Side1->GetN()>0 && counter21==1) {GraphDet5Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet5Face2Side1->GetN()>0) {GraphDet5Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet5Face2Side1->SetMarkerStyle(20);
				GraphDet5Face2Side1->SetMarkerColor(color);
				GraphDet5Face2Side1->SetLineWidth(2);
				GraphDet5Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==5) {
				if (GraphDet6Face2Side1->GetN()>0 && counter21==1) {GraphDet6Face2Side1->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet6Face2Side1->GetN()>0) {GraphDet6Face2Side1->Draw("P"); Lab[ll]->Draw();}
				GraphDet6Face2Side1->SetMarkerStyle(20);
				GraphDet6Face2Side1->SetMarkerColor(color);
				GraphDet6Face2Side1->SetLineWidth(2);
				GraphDet6Face2Side1->SetLineColor(color);
				Title[ii]->Draw();
			}
			
			counter = 0;
			if (hEnergyFace2Side2[ii][ll] != NULL)
			{
				if (hEnergyFace2Side2[ii][ll]->GetEntries() > 100)
				{
					TProfile *profx = hEnergyFace2Side2[ii][ll]->ProfileX();
					Int_t maxentries=0;
					for (int ibin=0; ibin<profx->GetNbinsX(); ibin++)
					{
						if (profx->GetBinEntries(ibin)>0)
						{
							double YY=profx->GetBinContent(ibin);
							double Error=profx->GetBinError(ibin);
							double XX=profx->GetBinCenter(ibin)+0.005*(counter22);
							printf("Hi! Det: %d, Strip: %d, XX: %lf, YY: %lf, Counter: %d\n",ii,ll,XX,YY,counter);
							
							if (ii==0) {GraphDet1Face2Side2->SetPoint(counter,XX,YY); GraphDet1Face2Side2->SetPointError(counter,0,Error);}
							else if (ii==1) {GraphDet2Face2Side2->SetPoint(counter,XX,YY); GraphDet2Face2Side2->SetPointError(counter,0,Error);}
							else if (ii==2) {GraphDet3Face2Side2->SetPoint(counter,XX,YY); GraphDet3Face2Side2->SetPointError(counter,0,Error);}
							else if (ii==3) {GraphDet4Face2Side2->SetPoint(counter,XX,YY); GraphDet4Face2Side2->SetPointError(counter,0,Error);}
							else if (ii==4) {GraphDet5Face2Side2->SetPoint(counter,XX,YY); GraphDet5Face2Side2->SetPointError(counter,0,Error);}
							else if (ii==5) {GraphDet6Face2Side2->SetPoint(counter,XX,YY); GraphDet6Face2Side2->SetPointError(counter,0,Error);}
							counter++;
						}
					}
					counter22++;
				}
			}
			CanF2S2[ii]->cd();
			if (ii==0) {	
				if (GraphDet1Face2Side2->GetN()>0 && counter22==1) {GraphDet1Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet1Face2Side2->GetN()>0) {GraphDet1Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet1Face2Side2->SetMarkerStyle(20);
				GraphDet1Face2Side2->SetMarkerColor(color);
				GraphDet1Face2Side2->SetLineWidth(2);
				GraphDet1Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==1) {	
				if (GraphDet2Face2Side2->GetN()>0 && counter22==1) {GraphDet2Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet2Face2Side2->GetN()>0) {GraphDet2Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet2Face2Side2->SetMarkerStyle(20);
				GraphDet2Face2Side2->SetMarkerColor(color);
				GraphDet2Face2Side2->SetLineWidth(2);
				GraphDet2Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==2) {	
				if (GraphDet3Face2Side2->GetN()>0 && counter22==1) {GraphDet3Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet3Face2Side2->GetN()>0) {GraphDet3Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet3Face2Side2->SetMarkerStyle(20);
				GraphDet3Face2Side2->SetMarkerColor(color);
				GraphDet3Face2Side2->SetLineWidth(2);
				GraphDet3Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==3) {	
				if (GraphDet4Face2Side2->GetN()>0 && counter22==1) {GraphDet4Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet4Face2Side2->GetN()>0) {GraphDet4Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet4Face2Side2->SetMarkerStyle(20);
				GraphDet4Face2Side2->SetMarkerColor(color);
				GraphDet4Face2Side2->SetLineWidth(2);
				GraphDet4Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==4) {	
				if (GraphDet5Face2Side2->GetN()>0 && counter22==1) {GraphDet5Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet5Face2Side2->GetN()>0) {GraphDet5Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet5Face2Side2->SetMarkerStyle(20);
				GraphDet5Face2Side2->SetMarkerColor(color);
				GraphDet5Face2Side2->SetLineWidth(2);
				GraphDet5Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
			else if (ii==5) {	
				if (GraphDet6Face2Side2->GetN()>0 && counter22==1) {GraphDet6Face2Side2->Draw("AP"); Lab[ll]->Draw();}
				else if (GraphDet6Face2Side2->GetN()>0) {GraphDet6Face2Side2->Draw("P"); Lab[ll]->Draw();}
				GraphDet6Face2Side2->SetMarkerStyle(20);
				GraphDet6Face2Side2->SetMarkerColor(color);
				GraphDet6Face2Side2->SetLineWidth(2);
				GraphDet6Face2Side2->SetLineColor(color);
				Title[ii]->Draw();
			}
		}
		CanF1S1[ii]->SaveAs(".png");
		CanF1S2[ii]->SaveAs(".png");
		CanF2S1[ii]->SaveAs(".png");
		CanF2S2[ii]->SaveAs(".png");
	}
	//CanPosX->SaveAs(".png");
}

