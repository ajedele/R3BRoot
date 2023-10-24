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


void PspxC2HCal()
{
	int runId = 187;
	int fNumDet = 6;
	int fNumStrips = 32;

	double GausFits[10][4][35][4][4] = {{{{{0}}}}};
	double TotalFits[10][4][4][4] = {{{{0}}}};

	ofstream outputfile;
	outputfile.open("PspXCal2HitFitPar.par");

	TString cnom;
	TCanvas *CanPspx[fNumDet][4];
	TGraphErrors *tBetheBloch[10][4][35];

	char line[4][1000]={{0}};
	int Det=0, Face=0, Strip=0, entries=0;
	double totE=0., pos=0., totEerror = 0.;
	int gcounter[6][4][4] = {{{0}}};

	for (int iset = 0; iset<4; iset++)
	{
		TString fParInName;
		if (iset == 0) fParInName = Form("PspXCal2HitPar0184-0193.par");
		else if (iset == 1) fParInName = Form("PspXCal2HitPar0211-0212.par");
		else if (iset == 2) fParInName = Form("PspXCal2HitPar0220.par");
		else fParInName = Form("PspXCal2HitPar0229.par");

		TString filename = Form("%s",fParInName.Data());
		std::ifstream infile(filename);
		if (infile.bad()) {printf("Zombie File1!!!\n");}
		printf("Input file: %s\n",fParInName.Data());

		while(!infile.eof())
		{
			infile.getline(line[iset],1000);
			sscanf(line[iset],"%d %d %d %lf %lf %lf %d", &Det, &Face, &Strip, &pos, &totE, &totEerror, &entries);
			GausFits[Det][Face][Strip][iset][0] = pos;
			GausFits[Det][Face][Strip][iset][1] = totE;
			GausFits[Det][Face][Strip][iset][2] = totEerror;
                        GausFits[Det][Face][Strip][iset][3] = entries;
			TotalFits[Det][Face][iset][0] = TotalFits[Det][Face][iset][0] + pos*entries;
                        TotalFits[Det][Face][iset][1] = TotalFits[Det][Face][iset][1] + totE/100.*entries;
                        TotalFits[Det][Face][iset][2] = TotalFits[Det][Face][iset][2] + totEerror*entries;
                        TotalFits[Det][Face][iset][3] = TotalFits[Det][Face][iset][3] + entries;
			//if (Det1 == 0) {printf("SScanf line. Det; %d, Face: %d, Strip: %d, gaus: %lf, gainerror: %lf\n", Det1, Face1, StripFront1, gaus1, gausserror1);}
			gcounter[Det][Face][iset] += 1;
		}
	}

	for (int idet=0; idet<fNumDet; idet++)
        {
                for (int iface=0; iface<2; iface++)
                {
                        for (int irun=0; irun<4; irun++)
                        {
                                TotalFits[idet][iface][irun][0] = TotalFits[idet][iface][irun][0]/TotalFits[idet][iface][irun][3];
                                TotalFits[idet][iface][irun][1] = TotalFits[idet][iface][irun][1]/TotalFits[idet][iface][irun][3]*100;
                                TotalFits[idet][iface][irun][2] = TotalFits[idet][iface][irun][2]/TotalFits[idet][iface][irun][3];
                                TotalFits[idet][iface][irun][3] = TotalFits[idet][iface][irun][3]/(double)gcounter[idet][iface][irun];
                                printf("%d %d %d %lf %lf %lf\n", idet, iface, irun, TotalFits[idet][iface][irun][0], TotalFits[idet][iface][irun][1], TotalFits[idet][iface][irun][2]);
                        }
                }
        }

	//value from Bethe Bloch from ATIMA
	//double max[6] = {350000,20000,500000,30000,500000,30000};
	//double min[6] = {200000,10000,300000,15000,220000,15000};

	int counter = 0;
	printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	for (int ii = 0; ii < fNumDet; ii++)
	//for (int ii = 0; ii < 1; ii++)
	{
    		for (int jj = 0; jj < 2; jj++)
    		{
			cnom = Form("ECalib_Det%dFace%d",ii+1,jj+1);
			CanPspx[ii][jj] = new TCanvas(cnom.Data(),cnom.Data(),800,800);
    			for (int kk = 0; kk < fNumStrips; kk++)
    			{
				double BB[4] = {449.082 + 0.5*(kk-10), 394.773 + 0.5*(kk-10), 373.066 + 0.5*(kk-10), 351.553 + 0.5*(kk-10)};
				
				int color = kk % 8;
				if (kk%8 == 0) color=9;;
				int yy = kk*5 + 280;
				int xx;
				if (ii%2 == 0) xx=300000;
				if (ii%2 == 1) xx=20000;
				TLatex *label = new TLatex(xx,yy,Form("#color[%d]{#bullet}Strip: %d",color,kk+1));
				label->SetNDC();
				if (GausFits[ii][jj][kk][0][1] > 0 && GausFits[ii][jj][kk][1][1] > 0 && GausFits[ii][jj][kk][2][1] > 0 && GausFits[ii][jj][kk][3][1] > 0)
				{
					double GF[4] = {GausFits[ii][jj][kk][0][1], GausFits[ii][jj][kk][1][1], GausFits[ii][jj][kk][2][1], GausFits[ii][jj][kk][3][1]};
					double GE[4] = {GausFits[ii][jj][kk][0][2], GausFits[ii][jj][kk][1][2], GausFits[ii][jj][kk][2][2], GausFits[ii][jj][kk][3][2]};
					tBetheBloch[ii][jj][kk] = new TGraphErrors(4,BB,GF,0,GE);
					TString nom = Form("ECalib_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					tBetheBloch[ii][jj][kk]->SetName(nom.Data());
					tBetheBloch[ii][jj][kk]->SetTitle(nom.Data());
					//printf("Det: %d, Face: %d, Strip1: %d, Strip2: %d\nBB: %lf\t%lf\t%lf\t%lf\nGaus: %lf\t%lf\t%lf\t%lf\n",ii,jj,kk,BB[0],BB[1],BB[2],BB[3],GF[0],GF[1],GF[2],GF[3]);
					//printf("Det: %d, Face: %d, Strip1: %d, Strip2: %d\n\nGaus: %lf\t%lf\t%lf\t%lf\n",ii,jj,kk,GF[0],GF[1],GF[2],GF[3]);
					
					TF1 *linearfit = new TF1("BetheBlock", "[0]+[1]*(x)", 300, 500);
					tBetheBloch[ii][jj][kk]->Fit(linearfit, "0NECQW", "", 300, 500);
					nom = Form("Fit_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					linearfit->SetName(nom.Data());
					if (linearfit->GetParameter(1)>0.01 || linearfit->GetParameter(1)<0.005) printf("Fit Parameter: %lf + %lf * x\n", linearfit->GetParameter(0),linearfit->GetParameter(1));
					
					outputfile << ii << "\t" << 0 << "\t" << kk+1 << "\t" << "\t" << linearfit->GetParameter(0) << "\t" << linearfit->GetParameter(1) << std::endl;
					
					if (counter == 0) tBetheBloch[ii][jj][kk]->Draw("AP");
					else tBetheBloch[ii][jj][kk]->Draw("sameP");
					tBetheBloch[ii][jj][kk]->SetTitle("");
					tBetheBloch[ii][jj][kk]->SetMarkerColor(color);
					tBetheBloch[ii][jj][kk]->SetLineColor(color);
					tBetheBloch[ii][jj][kk]->SetMarkerStyle(20);
					tBetheBloch[ii][jj][kk]->SetLineWidth(2);
					linearfit->Draw("same");
					linearfit->SetLineColor(color);
					counter++;
				}
				else if (GausFits[ii][jj][kk][0][1] > 0 && (GausFits[ii][jj][kk][1][1] > 0 || GausFits[ii][jj][kk][2][1] > 0) && GausFits[ii][jj][kk][3][1] > 0)
				{
					double gausfit = ((GausFits[ii][jj][kk][1][1] > 0) ? GausFits[ii][jj][kk][1][1] : GausFits[ii][jj][kk][2][1]);
					double bb      = ((GausFits[ii][jj][kk][1][1] > 0) ? 394.773 : 373.066);
					printf("\n3 fits found. GausFit2: %lf, GausFit3: %lf, gausfit: %lf, bb %lf\n\n",GausFits[ii][jj][kk][1][1],GausFits[ii][jj][kk][2][1],gausfit,bb);
					double BBBB[3] = {449.082, bb, 351.553};
					double GF[3] = {GausFits[ii][jj][kk][0][1], gausfit, GausFits[ii][jj][kk][3][1]};
					double GE[3] = {GausFits[ii][jj][kk][0][2], gausfit, GausFits[ii][jj][kk][3][2]};
					tBetheBloch[ii][jj][kk] = new TGraphErrors(3,BBBB,GF,0,GE);
					TString nom = Form("ECalib_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					tBetheBloch[ii][jj][kk]->SetName(nom.Data());
					tBetheBloch[ii][jj][kk]->SetTitle(nom.Data());
					printf("Det: %d, Face: %d, Strip1: %d, \nGaus: %lf\t%lf\t%lf\n",ii,jj,kk,GF[0],GF[1],GF[2]);
					
					TF1 *linearfit = new TF1("BetheBlock", "[0]+[1]*(x)", 300, 500);
					tBetheBloch[ii][jj][kk]->Fit(linearfit, "0NECQW", "", 300, 500);
					nom = Form("Fit_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					linearfit->SetName(nom.Data());
					printf("Fit Parameter: %lf + %lf * x\n", linearfit->GetParameter(0),linearfit->GetParameter(1));
		
					outputfile << ii << "\t" << 1 << "\t" << kk+1 << "\t" << "\t" << linearfit->GetParameter(0) << "\t" << linearfit->GetParameter(1) << std::endl;
					
					if (counter == 0) tBetheBloch[ii][jj][kk]->Draw("AP");
					else tBetheBloch[ii][jj][kk]->Draw("sameP");
					tBetheBloch[ii][jj][kk]->SetMarkerColor(color);
					tBetheBloch[ii][jj][kk]->SetLineColor(color);
					tBetheBloch[ii][jj][kk]->SetMarkerStyle(20);
					tBetheBloch[ii][jj][kk]->SetLineWidth(2);
					linearfit->Draw("same");
					linearfit->SetLineColor(color);
					counter++;
				}
				else continue;
				label->Draw();
			}
			counter = 0;
			CanPspx[ii][jj]->SaveAs(".png");
		}
	}
	
	outputfile.close();
}

