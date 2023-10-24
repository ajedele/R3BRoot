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


void PspxM2CCheck(int runstart, int runend, int runstart1, int runend1, int runstart2, int runend2, int runstart3, int runend3, int runstart4, int runend4)
{
	Int_t numDet = 6;
	Int_t numStrip = 32;

	TString cannom;
	TCanvas *CanPspxX[numDet];
	TCanvas *CanPspxY[numDet];

	TString histName;
	TProfile2D *hPosX[numDet];
	TProfile2D *hPosY[numDet];
	TProfile2D *hPosX1[numDet];
	TProfile2D *hPosY1[numDet];

	TProfile2D *hPosDiffX[numDet];
	TProfile2D *hPosDiffY[numDet];

	TString nom;
	for (int idet=0; idet<numDet; idet++)
	{
		nom = Form("CorrFactorsX_Det%d",idet);
		CanPspxX[idet] = new TCanvas(nom.Data(),nom.Data(),800,800);
		hPosX[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		nom = Form("CorrFactorsX1_Det%d",idet);
		hPosX1[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		nom = Form("CorrFactorsDiffX_Det%d",idet);
		hPosDiffX[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		
		nom = Form("CorrFactorsY_Det%d",idet);
		CanPspxY[idet] = new TCanvas(nom.Data(),nom.Data(),800,800);
		hPosY[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		nom = Form("CorrFactorsY1_Det%d",idet);
		hPosY1[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
		nom = Form("CorrFactorsDiffY_Det%d",idet);
		hPosDiffY[idet] = new TProfile2D(nom.Data(),nom.Data(),32,0,32,32,0,32);
	}
	double ratios[6][2][32][32][5]={0};

	int Det=0, Face=0, StripFront=0, StripBack=0;
	double ratio=0; double pos=0;
	char line[1000];
	TString filename = Form("Poscal2CalPar%04d-%04d.txt",runstart,runend);
	ifstream infile(filename);

	int Det1=0, Face1=0, StripFront1=0, StripBack1=0;
	double ratio1=0; double pos1=0;
	char line1[1000];
	TString filename1 = Form("Poscal2CalPar%04d-%04d.txt",runstart1,runend1);
	ifstream infile1(filename1);

	int Det2=0, Face2=0, StripFront2=0, StripBack2=0;
	double ratio2=0; double pos2=0;
	char line2[1000];
	TString filename2 = Form("Poscal2CalPar%04d-%04d.txt",runstart2,runend2);
	ifstream infile2(filename2);

	int Det3=0, Face3=0, StripFront3=0, StripBack3=0;
	double ratio3=0; double pos3=0;
	char line3[1000];
	TString filename3 = Form("Poscal2CalPar%04d-%04d.txt",runstart3,runend3);
	ifstream infile3(filename3);

	int Det4=0, Face4=0, StripFront4=0, StripBack4=0;
	double ratio4=0; double pos4=0;
	char line4[1000];
	TString filename4 = Form("Poscal2CalPar%04d-%04d.txt",runstart4,runend4);
	ifstream infile4(filename4);

	while(!infile.eof()) 
	{
		infile.getline(line,1000);
		sscanf(line,"%d %d %d %d %lf %lf",&Det, &Face, &StripFront, &StripBack, &pos, &ratio);
		printf("Det: %d, Face: %d, Strip: %d, Strip: %d, pos: %lf, ratio: %lf\n",Det,Face,StripFront,StripBack,pos,ratio);
		if (Face == 1) {hPosX[Det]->Fill(StripFront,StripBack,ratio);}
		else if (Face == 0) {hPosY[Det]->Fill(StripFront,StripBack,ratio);}
		ratios[Det][Face][StripFront][StripBack][0]=ratio;
	}
	while(!infile1.eof()) 
	{
		infile1.getline(line1,1000);
		sscanf(line1,"%d %d %d %d %lf %lf",&Det1, &Face1, &StripFront1, &StripBack1, &pos1, &ratio1);
		printf("Det: %d, Face: %d, Strip: %d, Strip: %d, pos: %lf, ratio: %lf\n",Det1,Face1,StripFront1,StripBack1,pos1,ratio1);
		if (Face1 == 1) {hPosX1[Det1]->Fill(StripFront1,StripBack1,ratio1);}
		else if (Face1 == 0) {hPosY1[Det1]->Fill(StripFront1,StripBack1,ratio1);}
		ratios[Det1][Face1][StripFront1][StripBack1][1]=ratio1;
	}
	while(!infile2.eof()) 
	{
		infile2.getline(line2,1000);
		sscanf(line2,"%d %d %d %d %lf %lf",&Det2, &Face2, &StripFront2, &StripBack2, &pos2, &ratio2);
		printf("Det: %d, Face: %d, Strip: %d, Strip: %d, pos: %lf, ratio: %lf\n",Det2,Face2,StripFront2,StripBack2,pos2,ratio2);
		//if (Face2 == 1) {hPosX2[Det2]->Fill(StripFront2,StripBack2,ratio2);}
		//else if (Face2 == 0) {hPosY2[Det2]->Fill(StripFront2,StripBack2,ratio2);}
		ratios[Det2][Face2][StripFront2][StripBack2][2]=ratio2;
	}
	while(!infile3.eof()) 
	{
		infile3.getline(line3,1000);
		sscanf(line3,"%d %d %d %d %lf %lf",&Det3, &Face3, &StripFront3, &StripBack3, &pos3, &ratio3);
		printf("Det: %d, Face: %d, Strip: %d, Strip: %d, pos: %lf, ratio: %lf\n",Det3,Face3,StripFront3,StripBack3,pos3,ratio3);
		//if (Face3 == 1) {hPosX3[Det3]->Fill(StripFront3,StripBack3,ratio3);}
		//else if (Face3 == 0) {hPosY3[Det3]->Fill(StripFront3,StripBack3,ratio3);}
		ratios[Det3][Face3][StripFront3][StripBack3][3]=ratio3;
	}
	while(!infile4.eof()) 
	{
		infile4.getline(line4,1000);
		sscanf(line4,"%d %d %d %d %lf %lf",&Det4, &Face4, &StripFront4, &StripBack4, &pos4, &ratio4);
		printf("Det: %d, Face: %d, Strip: %d, Strip: %d, pos: %lf, ratio: %lf\n",Det4,Face4,StripFront4,StripBack4,pos4,ratio4);
		//if (Face4 == 1) {hPosX4[Det4]->Fill(StripFront4,StripBack4,ratio4);}
		//else if (Face4 == 0) {hPosY4[Det4]->Fill(StripFront4,StripBack4,ratio4);}
		ratios[Det4][Face4][StripFront4][StripBack4][4]=ratio4;
	}
	for (int idet=0; idet<numDet; idet++)
	{
		CanPspxX[idet]->cd();
		hPosX[idet]->GetZaxis()->SetRangeUser(0.90,1.20);
		hPosX[idet]->Draw("colz");
		
		CanPspxY[idet]->cd();
		hPosY[idet]->GetZaxis()->SetRangeUser(0.90,1.20);
		hPosY[idet]->Draw("colz");
			
		//CanPspxX[idet]->SaveAs(".png");
		//CanPspxY[idet]->SaveAs(".png");
	}
	for (int idet=0; idet<numDet; idet++)
	{
		for (int iface=0; iface<2; iface++)
		{
			for (int istripfront=0; istripfront<numStrip; istripfront++)
			{
				for (int istripback=0; istripback<numStrip; istripback++)
				{
					//if (ratios[idet][iface][istripfront][istripback][0]!=0 && ratios[idet][iface][istripfront][istripback][1]!=0)	
					//{
					//	if (iface==0) hPosDiffY[idet]->Fill(istripfront,istripback,ratios[idet][iface][istripfront][istripback][0]/ratios[idet][iface][istripfront][istripback][1]);
					//	if (iface==1) hPosDiffX[idet]->Fill(istripfront,istripback,ratios[idet][iface][istripfront][istripback][0]/ratios[idet][iface][istripfront][istripback][1]);
					//}
					for (int index=0; index<3; index++)
					{
						if (ratios[idet][iface][istripfront][istripback][0]==0 && ratios[idet][iface][istripfront][istripback][index+1]!=0) printf("In not in 194-201. I'm in: %d and my id is : %d\t: %d\t: %d\t :%d\n",index+1, idet, iface, istripfront, istripback);
					}
				}
			}
		}
		//CanPspxX[idet]->cd();
		//hPosDiffX[idet]->GetZaxis()->SetRangeUser(0.95,1.05);
		//hPosDiffX[idet]->Draw("colz");
		//
		//CanPspxY[idet]->cd();
		//hPosDiffY[idet]->GetZaxis()->SetRangeUser(0.95,1.05);
		//hPosDiffY[idet]->Draw("colz");
		//	
		//CanPspxX[idet]->SaveAs(".png");
		//CanPspxY[idet]->SaveAs(".png");
	}	
}

