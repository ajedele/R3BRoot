/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------------
// -----                    R3BPspxCal2HitPar                      -----
// -----              Created  Jul 2022 by A. Jedele                  -----
// -----      Adopted from script by M. Holl, S. Storck                 -----
// -----  Purpose of script is to generate the position calibration   -----
// -----        and the timing calibration (new addition)             -----
// ------------------------------------------------------------------------

// Fair headers
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "FairRunAna.h"
#include "FairRootManager.h"

// PSP headers
#include "R3BEventHeader.h"
#include "R3BPspxCal2HitPar.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitPar.h"

#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <stdio.h>

//ROOT headers
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLatex.h"

//R3BPspsxCal2HitPar: Default Constructor ---------------------------------------
R3BPspxCal2HitPar::R3BPspxCal2HitPar()
    : FairTask("PspxCal2HitPar", 1)
{
}

//R3BPspsxCal2HitPar: Standard Constructor ---------------------------------------
R3BPspxCal2HitPar::R3BPspxCal2HitPar(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

//Virtual R3BPspsxCal2HitPar: Destructor ----------------------------------------
R3BPspxCal2HitPar::~R3BPspxCal2HitPar()
{
    LOG(info) << "R3BPspxCal2HitPar: Delete instance";
    delete fCal;
}

//----Public method Init -----------------------------------------------------------
InitStatus R3BPspxCal2HitPar::Init()
{
	LOG(info) << "R3BPspxCal2HitPar: Init" ;
	
	//FairRootManager
	FairRootManager* fMan = FairRootManager::Instance();
	if (!fMan) { LOG(error) << "R3BPspxCal2HitPar::Init(). No FairRootManager found"; return kFATAL;}
	
	//Get Cal Data
	fCal = dynamic_cast<TClonesArray*>(fMan->GetObject("PspxCalData"));
	if (!fCal) {LOG(error) << "R3BPspxCal2HitPar::Init(). No PspxCalData found."; return kFATAL;}
	
	// R3BEventHeader for trigger information, if needed!
	fHeader = dynamic_cast<R3BEventHeader*>(fMan->GetObject("R3BEventHeader"));
	
	//Container needs to be created in tcal/R3BCalContFact.cxx AND R3BCal needs
	//to be set as dependency in CMakelists.txt (in this case in the psp dir.)
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {return kFATAL;}
	
	if (!fNumDet)
	{
	    LOG(error) << "R3BPspxCal2HitPar::Init(). No Detectors detected.";
	    return kFATAL;
	}
	
	if (!fNumStrips)
	{
	    LOG(error) << "R3BPspxCal2HitPar::Init(). No Strips found.";
	    return kFATAL;
	}

	outputfile.open(fParOutName.Data());
	printf("ParOut: %s\n",fParOutName.Data());

	outputfile1.open(fParOutName1.Data());
	printf("ParOut: %s\n",fParOutName1.Data());

	//Define TGraph for the fits and other info
	//4 Parameters per strip per detectors: 2 sides and 2 faces
	printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	TString Name;
	for (int ii = 0; ii < fNumDet; ii++)
	{
		for  (int jj = 0; jj<2; jj++)
		{
			for  (int kk = 0; kk<fNumStrips; kk++)
			{
				Name = Form("RawEnergy_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);
				if (ii%2==0) RawEnergy[ii][jj][kk] = new TH2F(Name.Data(),Name.Data(),100,-1.0,1.0,40000,0,4000000);
				else RawEnergy[ii][jj][kk] = new TH2F(Name.Data(),Name.Data(),100,-1.0,1.0,10000,0,100000);
			}
		}
	}
	
	printf("End of Init\n");
	return kSUCCESS;

	printf("Entries = %d\n",fCal->GetEntriesFast());
}

//----Public method ReInit -----------------------------------------------------------
InitStatus R3BPspxCal2HitPar::ReInit() { return kSUCCESS; }

void R3BPspxCal2HitPar::Exec(Option_t* option)
{
	Reset();
    /**
     * Gets the parameters for the conversion from Cal to Hit level. It is called for every event.
     * The parameters are energy and timing.
     * Energy: 2 step calibration: 1st step is to align all the corrected energies. 2nd is to find the offset and slope using various different energies
     * Timing: calibration was done in the 1st step
     * Sets beam peak to 3x10^6
     *
     * Face 1 is strips in the x-direction (resitive splitting gives y-position)
     * Face 2 is strips in the y-direction (resitive splitting gives x-position)
     *
     * Side 1 is left, Side 2 is right (look beam-downstream)
     * Side 1 is up, Side 2 is down (look beam-downstream)
     *
     * Face 1 is biased
     */

	counter_tot++;

	int nHits = fCal->GetEntries();
	if (nHits > 0) 
	{
	       	counter_events++; /*PRINTF("entries: %d\n",fCal->GetEntries());*/

		//Start of the actual calibration
		for (int ii=0; ii<nHits; ii++)
		{
        		auto cal = dynamic_cast<R3BPspxCalData*>(fCal->At(ii));

			if (cal->GetMult() == 1)
			{
				RawEnergy[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1]->Fill(cal->GetPosition(), cal->GetEnergy());
			}
		}
	}
	fCal->Clear();
}


void R3BPspxCal2HitPar::Reset() {}


void R3BPspxCal2HitPar::FinishTask() 
{
	printf("R3BPspxCal2Hit::FinishTask()\n");	
	GetEnergyCorrection();
	for (int ii=0; ii < fNumDet; ii++)
	{
		for (int jj=0; jj < 2; jj++)
		{
			for (int ll=0; ll < fNumStrips; ll++)
			{
				if(RawEnergy[ii][jj][ll]->GetEntries()>0) {RawEnergy[ii][jj][ll]->Write();}	
			}
		}
	}
	outputfile.close();
	printf("Counters. Total: %d, Events: %d\n", counter_tot, counter_events);
}

void R3BPspxCal2HitPar::GetEnergyCorrection() 
{
	TString cnom;
        TCanvas *CanPspx[10][2][35];

	//if numExpt == 444
	//double maxenergy[10] = {380000., 22000., 230000., 13000., 270000., 15000.};
	// numExpt == 473
	//double maxenergy[4][6] = {{300000., 18000., 450000., 26000., 450000., 25000.},
	//        {260000., 14000., 390000., 23000., 380000., 21000.},
	//        {240000., 13000., 370000., 21000., 350000., 20000.},
	//        {230000., 12000., 340000., 19000., 330000., 18000.}};
	
	for (int ii = 0; ii < fNumDet; ii++)
	{
		double range;
		if (ii%2==0) range = 25000;
		else range = 1500;
		for (int jj = 0; jj < 2; jj++)
		{
			for (int kk=0; kk<fNumStrips; kk++)
			{
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
							if (max>0.8*fMaxEnergy[ii] && max<1.2*fMaxEnergy[ii])
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
						
						TFitResultPtr fitinfo = tgm->Fit(QuadFit,"");
						int fitstatus = fitinfo;
						if (fitstatus >= 0) {outputfile<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<QuadFit->GetParameter(0)<<"\t"<<QuadFit->GetParameter(1)<<"\t"<<QuadFit->GetParameter(2)<<"\t"<<entries<<std::endl;}
						
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

	printf("Max Strip: %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %lf\n", fMaxStrip[0], fMaxStrip[1], fMaxStrip[2], fMaxStrip[3], fMaxStrip[4], fMaxStrip[5], fMaxStrip[6], fMaxStrip[7], fMaxStrip[8], fMaxStrip[9], fMaxStrip[10], fMaxStrip[11]);

	for (int ii = 0; ii < fNumDet; ii++)
	{
		double range;
		if (ii%2==0) range = 20000;
		else range = 2000;
		for (int jj = 0; jj < 2; jj++)
		{
			for (int kk=0; kk<fNumStrips; kk++)
			{
				if (RawEnergy[ii][jj][kk] != NULL)
				{
					RawEnergy[ii][jj][kk]->Rebin2D(2);
					if (ii%2==0) RawEnergy[ii][jj][kk]->GetYaxis()->SetRangeUser(200000,700000);
					else if (ii%2==1) RawEnergy[ii][jj][kk]->GetYaxis()->SetRangeUser(10000,40000);
					int entries = 0;
					double totalmax = 0;
					double totalpos = 0;
					double totalmaxerror = 0;
					
					TProfile *prof = RawEnergy[ii][jj][kk]->ProfileX();
					//printf("entries: %d\n",prof->GetNbinsX());
					double totalmaxpos = prof->GetNbinsX()/2. + prof->GetNbinsX()/2 * ((fMaxStrip[2*ii+jj] - 16.)/16.);
					int maxpos = (int)totalmaxpos;
					
					TH1D * proj = RawEnergy[ii][jj][kk]->ProjectionY("proj", maxpos+1, maxpos+1, "");
					int binmax = proj->GetMaximumBin();
					double max = proj->GetXaxis()->GetBinCenter(binmax);
					
					printf("Det: %d, Face: %d, Strip: %d, max: %lf, fMaxEnergy: %lf, entries: %f\n",ii,jj,kk,max, fMaxEnergy[fCalibPar*fNumDet + ii], proj->GetEntries());
					if (max<1.2*fMaxEnergy[fCalibPar*fNumDet+ii] && max>0.8*fMaxEnergy[fCalibPar*fNumDet+ii] && proj->GetEntries()>1000)
					{
						TString cannom = Form("EnergyCorrectionDet%dFace%dStrip%d",ii+1,jj+1,kk+1);
						CanPspx[ii][jj][kk] = new TCanvas(cannom.Data(),cannom.Data(),500,500);
						
						totalmax = max; entries = proj->GetEntries(); totalpos = prof->GetBinCenter(maxpos);
						totalmaxerror = proj->GetStdDev();
						
						RawEnergy[ii][jj][kk]->Draw("colz");
						TGraphErrors *gr1 = new TGraphErrors();
						gr1->SetPoint(0, totalpos, totalmax);
						gr1->SetPointError(0, 0, totalmaxerror);
						gr1->Draw("same P");
						gr1->SetLineWidth(3);
						gr1->SetMarkerStyle(20);
					}
					if (totalmax > 0) outputfile1<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<totalpos<<"\t"<<totalmax<<"\t"<<totalmaxerror<<"\t"<<entries<<std::endl;
					if (totalmax > 0) printf("Det: %d, Face: %d, Strip: %d totalpos: %lf max value: %lf error: %lf entries: %d\n",ii,jj,kk,totalpos,totalmax, totalmaxerror,entries);
					
					totalmax=0; entries=0; totalpos=0; totalmaxpos=0; totalmaxerror=0; max=0; binmax=0; maxpos=0;
				}
			}
		}
		outputfile1.close();
	}
}

ClassImp(R3BPspxCal2HitPar)
