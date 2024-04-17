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
#include <TF1.h>

void TofdZCompare(int RunId)
{
	Int_t numPlane = 4;
	Int_t numBar = 44;
	Int_t color = 0;
	Int_t counter11=0; Int_t counter12=0; Int_t counter21=0; Int_t counter22=0;
	TLatex *Label[numBar];
	TLatex *LabelBar[numPlane];

	TString canName;
	TCanvas *CanPlane[4];
	TCanvas *CanBar[44];

	TString histName;
	TH1D *ZHisto[numPlane][numBar];

	TH2D *Background = new TH2D("","",21,35,55,100,0,0.06);

	TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TFile *file1 = new TFile(Form("%sPspxTofdHit_%04d_ZHisto.root",path.Data(),RunId));

	int counter = 0;
	for (Int_t ii=0; ii<numPlane; ii++)
	{
		canName = Form("Zdist_Plane%d",ii+1);	
		CanPlane[ii] = new TCanvas(canName.Data(),canName.Data(),800,800);

		Background->Draw();
		Background->SetStats(0);

		for (Int_t ll=12; ll < 33; ll++)
		{
			if (ii==0)
			{	
				canName = Form("Zdist_Bar%d",ll+1);	
				CanBar[ll] = new TCanvas(canName.Data(),canName.Data(),800,800);
			
				Background->Draw();
				Background->SetStats(0);
			}

			histName = Form("QX_vs_Pos1D_Plane_%d_Bar_%d",ii+1,ll+1);					
			ZHisto[ii][ll] = (TH1D*)(file1->FindObjectAny(histName));

			if (ll==12) color = 9; 
			else if (ll==13) color = 13; 
			else if (ll==14) {color = 51;} 
			else if (ll==15) {color = 1;} 
			else if (ll==16) color = 2; 
			else if (ll==17) color = 3; 
			else if (ll==18) color = 4; 
			else if (ll==19) color = 6; 
			else if (ll==20) color = 7; 
			else if (ll==21) color = 8; 
			else if (ll==22) color = 9; 
			else if (ll==23) color = 13; 
			else if (ll==24) color = 51; 
			else if (ll==25) color = 1; 
			else if (ll==26) color = 2; 
			else if (ll==27) color = 3; 
			else if (ll==28) color = 4; 
			else if (ll==29) color = 6; 
			else if (ll==30) color = 7; 
			else if (ll==31) color = 8; 
			else if (ll==32) color = 9; 
			else if (ll==33) color = 13; 

			TString LatexLabel = Form("#color[%d]{#bullet} %d",color,ll+1);
			Label[ll] = new TLatex(0.9,1.0-0.03*ll,LatexLabel.Data());
			Label[ll]->SetNDC();
					
			TString LatexLabelBar = Form("#color[%d]{#bullet} %d",ii+1,ii+1);
			LabelBar[ii] = new TLatex(0.9,0.7-0.1*ii,LatexLabelBar.Data());
			LabelBar[ii]->SetNDC();
					
			TString titnom;
			if (ZHisto[ii][ll] != NULL)
			{
				printf("Hello. I am Plane %d, Bar: %d, counter: %d\n",ii+1,ll+1,counter);
				ZHisto[ii][ll]->Rebin(10);
				ZHisto[ii][ll]->GetXaxis()->SetRangeUser(35,55);
			
				ZHisto[ii][ll]->Scale(1.0/ZHisto[ii][ll]->GetEntries());
				ZHisto[ii][ll]->SetLineColor(color);
				ZHisto[ii][ll]->SetStats(0);
				
				titnom = Form("Bar: %d",ll+1);
				ZHisto[ii][ll]->SetTitle(titnom.Data());
				Background->SetTitle(titnom.Data());
			
				CanPlane[ii]->cd();
				ZHisto[ii][ll]->Draw("SAME HIST");
				Label[ll]->Draw();
				CanBar[ll]->cd();
				ZHisto[ii][ll]->SetLineColor(ii+1);
				ZHisto[ii][ll]->Draw("SAME HIST");
				LabelBar[ii]->Draw();
				
				counter++;
			}
		}
		counter = 0;
	}
	for (int ll = 13; ll < 32; ll++) CanBar[ll]->SaveAs(".png");
}

