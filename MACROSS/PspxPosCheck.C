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
#include <TProfile2D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TAxis.h>


void PspxPosCheck(Int_t runId1, Int_t runId2, Int_t runId3, Int_t runId4)
{
	Int_t numDet = 6;
	Int_t numStrip = 32;

	TString cannom;
	TCanvas *CanPspxX[numDet];
	TCanvas *CanPspxY[numDet];

	TString histName;
	TProfile2D *hPosX[numDet];
	TProfile2D *hPosY[numDet];

	Double_t PosX1[6][32][32] = {{{0}}};
	Double_t PosX2[6][32][32] = {{{0}}};
	Double_t PosY1[6][32][32] = {{{0}}};
	Double_t PosY2[6][32][32] = {{{0}}};

	TString nom;
	for (int idet=0; idet<numDet; idet++)
	//for (int idet=0; idet<1; idet++)
	{
		nom = Form("FactorsX_Det%d",idet);
		CanPspxX[idet] = new TCanvas(nom.Data(),nom.Data(),800,800);
		hPosX[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		
		nom = Form("FactorsY_Det%d",idet);
		CanPspxY[idet] = new TCanvas(nom.Data(),nom.Data(),800,800);
		hPosY[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
	}

	Int_t Det=0, Face=0, StripFront=0, StripBack=0;
	Int_t Det1=0, Face1=0, StripFront1=0, StripBack1=0;
	Double_t ratio=0; 
	Double_t ratio1=0; 
	Char_t line[5000] = {0};
	Char_t line1[5000] = {0};

	//TString path = Form("/lustre/land/ajedele/s473/rootfiles/");
	TString path = Form("/u/ajedele/R3BRoot_oldest/macros/r3b/unpack/s473/pspx/new_andrea/");
	//TString filename = Form("Mapped2CalParGlobal.txt");
	TString filename = (Form("%sPspxMapped2CalPar%04d-%04d.par",path.Data(),runId1,runId2));
	ifstream infile(filename);
	
	while(!infile.eof()) 
	{
		infile.getline(line,1000);
		sscanf(line,"%d %d %d %d %lf", &Det, &Face, &StripFront, &StripBack, &ratio);
		//if (Det==0 && Face == 1) {printf("File1!! Det: %d, Face: %d, Strip: %d, Strip: %d, ratio: %lf\n",Det,Face,StripFront,StripBack,ratio);}
		if (Face == 0) {PosX1[Det][StripFront][StripBack]=ratio;}
		else if (Face == 1) {PosY1[Det][StripFront][StripBack]=ratio;}
	}
	infile.close();
	
	
	TString filename1 = (Form("%sPspxMapped2CalPar%04d-%04d.par",path.Data(),runId3,runId4));
	ifstream infile1(filename1);

	while(!infile1.eof()) 
	{
		infile1.getline(line1,1000);
		sscanf(line1,"%d %d %d %d %lf", &Det1, &Face1, &StripFront1, &StripBack1, &ratio1);
		//if (Det1==0 && Face1 == 1) {printf("File2!! Det: %d, Face: %d, Strip: %d, Strip: %d, ratio: %lf\n",Det1,Face1,StripFront1,StripBack1,ratio1);}
		if (Face1 == 0) {PosX2[Det1][StripFront1][StripBack1]=ratio1;}
		else if (Face1 == 1) {PosY2[Det1][StripFront1][StripBack1]=ratio1;}
	}
	infile1.close();

	Double_t factorX=0., factorY=0.;
	for (int idet=0; idet<numDet; idet++)
	{
		for (int istripfront=0; istripfront<numStrip; istripfront++)
		{
			for (int istripback=0; istripback<numStrip; istripback++)
			{
				//printf("Det: %d, StripFront: %d, StripBack: %d, RatioY1: %lf, RatioY2: %lf\n", idet+1, istripfront+1, istripback+1, PosY1[idet][istripfront][istripback], PosY2[idet][istripfront][istripback]);
				if (PosX1[idet][istripfront][istripback] != 0 && PosX2[idet][istripfront][istripback] != 0) 
				{
					factorX = PosX1[idet][istripfront][istripback]/PosX2[idet][istripfront][istripback];
					if (factorX < 0.98 || factorX > 1.02) printf("Det: %d, StripFront: %d, StripBack: %d, FactorX: %lf\n", idet+1, istripfront+1, istripback+1, factorX);
				}
				else {factorX = 0.;}
			
				if (PosY1[idet][istripfront][istripback] != 0 && PosY2[idet][istripfront][istripback] != 0) 
				{
					factorY = PosY1[idet][istripfront][istripback]/PosY2[idet][istripfront][istripback];
					if (factorY < 0.98 || factorY > 1.02) printf("Det: %d, StripFront: %d, StripBack: %d, FactorY: %lf\n", idet+1, istripfront+1, istripback+1, factorY);
				}
				else {factorY = 0.;}
			
				hPosX[idet]->Fill(istripfront+1,istripback+1,factorX);
				hPosY[idet]->Fill(istripfront+1,istripback+1,factorY);
				factorX = 0.; factorY = 0.;
			}
		}
		CanPspxX[idet]->cd();
		hPosX[idet]->GetZaxis()->SetRangeUser(0.95,1.05);
		hPosX[idet]->Draw("colz");
		CanPspxX[idet]->SaveAs(".png");
		
		CanPspxY[idet]->cd();
		hPosY[idet]->GetZaxis()->SetRangeUser(0.95,1.05);
		hPosY[idet]->Draw("colz");
		CanPspxY[idet]->SaveAs(".png");
			
		//CanPspxX[idet]->SaveAs(".png");
		//CanPspxY[idet]->SaveAs(".png");
	}
}

