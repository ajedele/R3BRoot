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


void PspxC2HParCheck()
{
	int numDet = 6;
	int numStrip = 32;

	TCanvas *CanPspx[numDet][2];

	TString path = Form("/u/ajedele/R3BRoot_temp/macros/r3b/unpack/s473/");
	TFile *file1 = new TFile(Form("%spsp.root",path.Data()));

	int entries = 0;
	int counter = 0;

	for (int ii = 0; ii < numDet; ii++)
	{
		double range;
        	if (ii%2==0) range = 20000;
		else range = 2000;
		for (int jj = 0; jj < 2; jj++)
		{
			TString cannom = Form("EnergyCorrectionDet%dFace%d",ii+1,jj+1);
			CanPspx[ii][jj] = new TCanvas(cannom.Data(),cannom.Data(),500,500);

			for (int kk=0; kk<numStrip; kk++)
			{
				TGraphErrors* ECalib = (TGraphErrors*)file1->Get(Form("ECalib_Det%dFace%dStrip%d",ii+1,jj+1,kk+1));
				TF1* Fit = (TF1*)file1->Get(Form("Fit_Det%dFace%dStrip%d",ii+1,jj+1,kk+1));
				if (ECalib != NULL) 
				{
					printf("Det: %d, Face: %d, Strip: %d\n",ii,jj,kk);
					
					if (counter == 0) ECalib->Draw("AP");
					else ECalib->Draw("same P");
					ECalib->SetLineWidth(7-counter);
					ECalib->SetLineColor(counter+1);
					
					Fit->Draw("same L");
					Fit->SetLineWidth(7-counter);
					Fit->SetLineColor(counter+1);
					
					counter++;
				}
			}
			counter = 0;
		}
	}
}

