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


void PspxEnergyCheck()
{
	Int_t numDet = 6;
	Int_t numStrip = 32;
	Int_t color = 0;
	Int_t counter11=0; Int_t counter12=0; Int_t counter21=0; Int_t counter22=0;
	TLatex *Label[numStrip];

	TCanvas *CanFace1Side1[numDet];
	TCanvas *CanFace1Side2[numDet];
	TCanvas *CanFace2Side1[numDet];
	TCanvas *CanFace2Side2[numDet];

	TCanvas *CanF1S1;
	TCanvas *CanF1S2;
	TCanvas *CanF2S1;
	TCanvas *CanF2S2;

	TString histName;
	TString canName;
	TH2F *hEnergyFace1Side1[numDet][numStrip];
	TH2F *hEnergyFace1Side2[numDet][numStrip];
	TH2F *hEnergyFace2Side1[numDet][numStrip];
	TH2F *hEnergyFace2Side2[numDet][numStrip];
	
	TH2F *hF1S1 = new TH2F("","",20,5,25,200,0,200);
	TH2F *hF1S2 = new TH2F("","",20,5,25,200,0,200);
	TH2F *hF2S1 = new TH2F("","",20,5,25,200,0,200);
	TH2F *hF2S2 = new TH2F("","",20,5,25,200,0,200);
	
	TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TFile *file1 = new TFile(Form("%sTest_Mapped2Poscal0194-0201.root",path.Data()));
	//TFile *file1 = new TFile(Form("%sTest_Poscal2CalPar0194.root",path.Data()));

	canName = Form("Maximum_Face1_Side1");	
	CanF1S1 = new TCanvas(canName.Data(),canName.Data(),800,800);
	canName = Form("Maximum_Face1_Side2");	
	CanF1S2 = new TCanvas(canName.Data(),canName.Data(),800,800);
	canName = Form("Maximum_Face2_Side1");	
	CanF2S1 = new TCanvas(canName.Data(),canName.Data(),800,800);
	canName = Form("Maximum_Face2_Side2");	
	CanF2S2 = new TCanvas(canName.Data(),canName.Data(),800,800);
	
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

	for (Int_t ii=0; ii<numDet; ii++)
	{
		canName = Form("Energy_Det%d_Face1_Side1",ii+1);	
		CanFace1Side1[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Energy_Det%d_Face1_Side2",ii+1);
		CanFace1Side2[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Energy_Det%d_Face2_Side1",ii+1);
		CanFace2Side1[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		canName = Form("Energy_Det%d_Face2_Side2",ii+1);	
		CanFace2Side2[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);
		
		for (Int_t ll=0; ll < numStrip; ll++)
		{
			histName = Form("Energy_Det%d_Face1_Side1_Strip%d",ii+1,ll+1);					
			hEnergyFace1Side1[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("Energy_Det%d_Face1_Side2_Strip%d",ii+1,ll+1);					
			hEnergyFace1Side2[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("Energy_Det%d_Face2_Side1_Strip%d",ii+1,ll+1);					
			hEnergyFace2Side1[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));
			histName = Form("Energy_Det%d_Face2_Side2_Strip%d",ii+1,ll+1);					
			hEnergyFace2Side2[ii][ll] = (TH2F*)(file1->FindObjectAny(histName));

			if (ll==6) {color = 13;} 
			else if (ll==7) {color = 51;} 
			else if (ll==8) {color = 1;} 
			else if (ll==9) color = 2; 
			else if (ll==10) color = 3; 
			else if (ll==11) color = 4; 
			else if (ll==12) color = 5; 
			else if (ll==13) color = 6; 
			else if (ll==14) color = 7; 
			else if (ll==15) color = 8; 
			else if (ll==16) color = 9; 
			else if (ll==17) color = 13; 
			else if (ll==18) color = 51; 
			else if (ll==19) color = 1; 
			else if (ll==20) color = 2; 
			else if (ll==21) color = 3; 
			else if (ll==22) color = 4; 
			else if (ll==23) color = 5; 
			else if (ll==24) color = 6; 
			else if (ll==25) color = 7; 
			else if (ll==26) color = 8; 
			else if (ll==27) color = 9; 
			else if (ll==28) color = 13; 

			TString LatexLabel = Form("#color[%d]{#bullet} %d",color,ll+1);
			Label[ll] = new TLatex(0.9,1.0-0.03*ll,LatexLabel.Data());

			Label[ll]->SetNDC();
					
			TString titnom;
			if (hEnergyFace1Side1[ii][ll] != NULL)
			{
				if (hEnergyFace1Side1[ii][ll]->GetEntries() > 10000)
				{
					CanFace1Side1[ii]->cd();
					hEnergyFace1Side1[ii][ll]->Scale(1./hEnergyFace1Side1[ii][ll]->GetEntries());
					if (counter11==0) hEnergyFace1Side1[ii][ll]->Draw("L");
					else hEnergyFace1Side1[ii][ll]->Draw("HIST SAME");
					if (ii % 2 == 0) {hEnergyFace1Side1[ii][ll]->GetXaxis()->SetRangeUser(0,450000);}
					else {hEnergyFace1Side1[ii][ll]->GetXaxis()->SetRangeUser(0,20000);}
					hEnergyFace1Side1[ii][ll]->SetStats(0);
					hEnergyFace1Side1[ii][ll]->SetLineColor(color);
					hEnergyFace1Side1[ii][ll]->GetYaxis()->SetRangeUser(0,0.15);
					titnom = Form("Face 1 Side 1 Detector: %d",ii+1);
					hEnergyFace1Side1[ii][ll]->SetTitle(titnom.Data());
			
					Label[ll]->Draw();
		
					if (ii==0) GraphDet1Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					else if (ii==1) GraphDet2Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					else if (ii==2) GraphDet3Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					else if (ii==3) GraphDet4Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					else if (ii==4) GraphDet5Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					else if (ii==5) GraphDet6Face1Side1->SetPoint(counter11,ll+1,hEnergyFace1Side1[ii][ll]->GetMaximumBin());
					counter11++;
				}
			}
			if (hEnergyFace1Side2[ii][ll] != NULL)
			{
				if (hEnergyFace1Side2[ii][ll]->GetEntries() > 10000)
				{
					CanFace1Side2[ii]->cd();
					if (counter12==0) hEnergyFace1Side2[ii][ll]->Draw();
					else hEnergyFace1Side2[ii][ll]->Draw("same hist");
					if (ii %2 == 0) {hEnergyFace1Side2[ii][ll]->GetXaxis()->SetRangeUser(0,450000);}
					else {hEnergyFace1Side2[ii][ll]->GetXaxis()->SetRangeUser(0,20000);}
					hEnergyFace1Side2[ii][ll]->Scale(1./hEnergyFace1Side2[ii][ll]->GetEntries());
					hEnergyFace1Side2[ii][ll]->GetYaxis()->SetRangeUser(0,0.15);
					hEnergyFace1Side2[ii][ll]->SetStats(0);
					hEnergyFace1Side2[ii][ll]->SetLineColor(color);
					titnom = Form("Face 1 Side 2 Detector: %d",ii+1);
					hEnergyFace1Side2[ii][ll]->SetTitle(titnom.Data());
			
					Label[ll]->Draw();

					if (ii==0) GraphDet1Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					else if (ii==1) GraphDet2Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					else if (ii==2) GraphDet3Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					else if (ii==3) GraphDet4Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					else if (ii==4) GraphDet5Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					else if (ii==5) GraphDet6Face1Side2->SetPoint(counter12,ll+1,hEnergyFace1Side2[ii][ll]->GetMaximumBin());
					counter12++;
				}
			}
			if (hEnergyFace2Side1[ii][ll] != NULL)
			{
				if (hEnergyFace2Side1[ii][ll]->GetEntries() > 10000)
				{
					CanFace2Side1[ii]->cd();
					if (counter21==0) hEnergyFace2Side1[ii][ll]->Draw();
					else hEnergyFace2Side1[ii][ll]->Draw("same hist");
					if (ii %2 == 0) {hEnergyFace2Side1[ii][ll]->GetXaxis()->SetRangeUser(0,450000);}
					else {hEnergyFace2Side1[ii][ll]->GetXaxis()->SetRangeUser(0,20000);}
					hEnergyFace2Side1[ii][ll]->Scale(1./hEnergyFace2Side1[ii][ll]->GetEntries());
					hEnergyFace2Side1[ii][ll]->SetStats(0);
					hEnergyFace2Side1[ii][ll]->SetLineColor(color);
					hEnergyFace2Side1[ii][ll]->GetYaxis()->SetRangeUser(0,0.15);
					titnom = Form("Face 2 Side 1 Detector: %d",ii+1);
					hEnergyFace2Side1[ii][ll]->SetTitle(titnom.Data());
			
					Label[ll]->Draw();

					if (ii==0) GraphDet1Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					else if (ii==1) GraphDet2Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					else if (ii==2) GraphDet3Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					else if (ii==3) GraphDet4Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					else if (ii==4) GraphDet5Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					else if (ii==5) GraphDet6Face2Side1->SetPoint(counter21,ll+1,hEnergyFace2Side1[ii][ll]->GetMaximumBin());
					counter21++;
				}
			}
			if (hEnergyFace2Side2[ii][ll] != NULL)
			{
				if (hEnergyFace2Side2[ii][ll]->GetEntries() > 10000)
				{
					CanFace2Side2[ii]->cd();
					if (counter22==0) hEnergyFace2Side2[ii][ll]->Draw();
					else hEnergyFace2Side2[ii][ll]->Draw("same hist");
					if (ii %2 == 0) {hEnergyFace2Side2[ii][ll]->GetXaxis()->SetRangeUser(0,450000);}
					else {hEnergyFace2Side2[ii][ll]->GetXaxis()->SetRangeUser(0,20000);}
					hEnergyFace2Side2[ii][ll]->Scale(1./hEnergyFace2Side2[ii][ll]->GetEntries());
					hEnergyFace2Side2[ii][ll]->GetYaxis()->SetRangeUser(0,0.15);
					hEnergyFace2Side2[ii][ll]->SetStats(0);
					hEnergyFace2Side2[ii][ll]->SetLineColor(color);
					titnom = Form("Face 2 Side 2 Detector: %d",ii+1);
					hEnergyFace2Side2[ii][ll]->SetTitle(titnom.Data());
			
					Label[ll]->Draw();

					if (ii==0) GraphDet1Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					else if (ii==1) GraphDet2Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					else if (ii==2) GraphDet3Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					else if (ii==3) GraphDet4Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					else if (ii==4) GraphDet5Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					else if (ii==5) GraphDet6Face2Side2->SetPoint(counter22,ll+1,hEnergyFace2Side2[ii][ll]->GetMaximumBin());
					counter22++;
				}
			}
		}
		CanF1S1->cd();
		if (ii==0) {hF1S1->Draw();}
		GraphDet1Face1Side1->Draw("P");
		GraphDet1Face1Side1->SetMarkerStyle(20);
		GraphDet1Face1Side1->SetMarkerColor(1);
		GraphDet2Face1Side1->Draw("P");
		GraphDet2Face1Side1->SetMarkerStyle(20);
		GraphDet2Face1Side1->SetMarkerColor(2);
		GraphDet3Face1Side1->Draw("P");
		GraphDet3Face1Side1->SetMarkerStyle(20);
		GraphDet3Face1Side1->SetMarkerColor(3);
		GraphDet4Face1Side1->Draw("P");
		GraphDet4Face1Side1->SetMarkerStyle(20);
		GraphDet4Face1Side1->SetMarkerColor(4);
		GraphDet5Face1Side1->Draw("P");
		GraphDet5Face1Side1->SetMarkerStyle(20);
		GraphDet5Face1Side1->SetMarkerColor(5);
		GraphDet6Face1Side1->Draw("P");
		GraphDet6Face1Side1->SetMarkerStyle(20);
		GraphDet6Face1Side1->SetMarkerColor(6);
		CanF1S2->cd();
		if (ii==0) {hF1S2->Draw();}
		GraphDet1Face1Side2->Draw("P");
		GraphDet1Face1Side2->SetMarkerStyle(20);
		GraphDet1Face1Side2->SetMarkerColor(1);
		GraphDet2Face1Side2->Draw("P");
		GraphDet2Face1Side2->SetMarkerStyle(20);
		GraphDet2Face1Side2->SetMarkerColor(2);
		GraphDet3Face1Side2->Draw("P");
		GraphDet3Face1Side2->SetMarkerStyle(20);
		GraphDet3Face1Side2->SetMarkerColor(3);
		GraphDet4Face1Side2->Draw("P");
		GraphDet4Face1Side2->SetMarkerStyle(20);
		GraphDet4Face1Side2->SetMarkerColor(4);
		GraphDet5Face1Side2->Draw("P");
		GraphDet5Face1Side2->SetMarkerStyle(20);
		GraphDet5Face1Side2->SetMarkerColor(5);
		GraphDet6Face1Side2->Draw("P");
		GraphDet6Face1Side2->SetMarkerStyle(20);
		GraphDet6Face1Side2->SetMarkerColor(6);
		CanF2S1->cd();
		if (ii==0) {hF2S1->Draw();}
		GraphDet1Face2Side1->Draw("P");
		GraphDet1Face2Side1->SetMarkerStyle(20);
		GraphDet1Face2Side1->SetMarkerColor(1);
		GraphDet2Face2Side1->Draw("P");
		GraphDet2Face2Side1->SetMarkerStyle(20);
		GraphDet2Face2Side1->SetMarkerColor(2);
		GraphDet3Face2Side1->Draw("P");
		GraphDet3Face2Side1->SetMarkerStyle(20);
		GraphDet3Face2Side1->SetMarkerColor(3);
		GraphDet4Face2Side1->Draw("P");
		GraphDet4Face2Side1->SetMarkerStyle(20);
		GraphDet4Face2Side1->SetMarkerColor(4);
		GraphDet5Face2Side1->Draw("P");
		GraphDet5Face2Side1->SetMarkerStyle(20);
		GraphDet5Face2Side1->SetMarkerColor(5);
		GraphDet6Face2Side1->Draw("P");
		GraphDet6Face2Side1->SetMarkerStyle(20);
		GraphDet6Face2Side1->SetMarkerColor(6);
		CanF2S2->cd();
		if (ii==0) {hF2S2->Draw();}
		GraphDet1Face2Side2->Draw("P");
		GraphDet1Face2Side2->SetMarkerStyle(20);
		GraphDet1Face2Side2->SetMarkerColor(1);
		GraphDet2Face2Side2->Draw("P");
		GraphDet2Face2Side2->SetMarkerStyle(20);
		GraphDet2Face2Side2->SetMarkerColor(2);
		GraphDet3Face2Side2->Draw("P");
		GraphDet3Face2Side2->SetMarkerStyle(20);
		GraphDet3Face2Side2->SetMarkerColor(3);
		GraphDet4Face2Side2->Draw("P");
		GraphDet4Face2Side2->SetMarkerStyle(20);
		GraphDet4Face2Side2->SetMarkerColor(4);
		GraphDet5Face2Side2->Draw("P");
		GraphDet5Face2Side2->SetMarkerStyle(20);
		GraphDet5Face2Side2->SetMarkerColor(5);
		GraphDet6Face2Side2->Draw("P");
		GraphDet6Face2Side2->SetMarkerStyle(20);
		GraphDet6Face2Side2->SetMarkerColor(6);
	
		//CanFace1Side1[ii]->SaveAs(".png");
		//CanFace1Side2[ii]->SaveAs(".png");
		//CanFace2Side1[ii]->SaveAs(".png");
		//CanFace2Side2[ii]->SaveAs(".png");

	}
	CanF1S1->SaveAs(".png");
	CanF1S2->SaveAs(".png");
	CanF2S1->SaveAs(".png");
	CanF2S2->SaveAs(".png");
}

