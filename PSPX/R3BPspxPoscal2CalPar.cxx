/*****************************************************************************
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
// -----                    R3BPspxPoscal2CalPar                      -----
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
#include "R3BPspxPoscal2CalPar.h"
#include "R3BPspxInterstripPoscalData.h"
#include "R3BPspxContFact.h"

#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <stdio.h>

//ROOT headers
#include "TMath.h"
#include "TLatex.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include <TMath.h>
#include "TRandom.h"
#include "TDirectory.h"
#include "TProfile.h"
#include "TCanvas.h"

//R3BPspsxPoscal2CalPar: Default Constructor ---------------------------------------
R3BPspxPoscal2CalPar::R3BPspxPoscal2CalPar()
    : FairTask("PspxPoscal2CalPar", 1)
{
}

//R3BPspsxPoscal2CalPar: Standard Constructor ---------------------------------------
R3BPspxPoscal2CalPar::R3BPspxPoscal2CalPar(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

//Virtual R3BPspsxPoscal2CalPar: Destructor ----------------------------------------
R3BPspxPoscal2CalPar::~R3BPspxPoscal2CalPar()
{
    LOG(info) << "R3BPspxPoscal2CalPar: Delete instance";
    delete fInterstripPoscal;
}

//----Public method Init -----------------------------------------------------------
InitStatus R3BPspxPoscal2CalPar::Init()
{
	LOG(info) << "R3BPspxPoscal2CalPar: Init" ;
	
	//FairRootManager
	FairRootManager* fMan = FairRootManager::Instance();
	if (!fMan) { LOG(error) << "R3BPspxPoscal2CalPar::Init(). No FairRootManager found"; return kFATAL;}
	
	//Get Poscal Data
	fInterstripPoscal = dynamic_cast<TClonesArray*>(fMan->GetObject("PspxInterstripPoscalData"));
	if (!fInterstripPoscal) {LOG(error) << "R3BPspxPoscal2CalPar::Init(). No PspxInterstripPoscalData found."; return kFATAL;}

	fHeader = dynamic_cast<R3BEventHeader*>(fMan->GetObject("R3BEventHeader"));
	
	//Container needs to be created in tcal/R3BCalContFact.cxx AND R3BCal needs
	//to be set as dependency in CMakelists.txt (in this case in the psp dir.)
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {return kFATAL;}
	
	if (!fNumDet)
	{
	    LOG(error) << "R3BPspxPoscal2CalPar::Init(). No Detectors detected.";
	    return kFATAL;
	}
	
	if (!fNumStrips)
	{
	    LOG(error) << "R3BPspxPoscal2CalPar::Init(). No Strips found.";
	    return kFATAL;
	}

	outputfile.open(fParOutName.Data());
	printf("\nInside outputfile: %s\n",fParOutName.Data());

	//Define TGraph for the fits and other info
	//4 Parameters per strip per detectors: 2 sides and 2 faces
	printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	TString Name;
	for (int ii = 0; ii < fNumDet; ii++)
	{
		for (int ll = 0; ll<fNumStrips; ll++)
    		{
    			for (int jj = 0; jj < 2; jj++)
    			{
           			Name = Form("Interstrip_Det%d_Face%d_Strip%d",ii+1,jj+1,ll+1);
	    			hInterstrip[ii][jj][ll] = new TH1D(Name.Data(), Name.Data(), 1000,0,1000000);
           			Name = Form("InterstripTot_Det%d_Face%d_Strip%d",ii+1,jj+1,ll+1);
	    			hInterstripTot[ii][jj][ll] = new TH1D(Name.Data(), Name.Data(), 1000,0,1000000);
           			Name = Form("InterstripTotBad_Det%d_Face%d_Strip%d",ii+1,jj+1,ll+1);
	    			hInterstripTotBad[ii][jj][ll] = new TH1D(Name.Data(), Name.Data(), 1000,0,1000000);
				Name = Form("PosYGoodDet%dSide%dStrip%d",ii+1,jj+1,ll+1);                    
				hPosYgood[ii][jj][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
				Name = Form("PosYBadDet%dSide%dStrip%d",ii+1,jj+1,ll+1);                         
				hPosYbad[ii][jj][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			}
			
			for (int kk=0; kk < fNumStrips; kk++)
			{	
				Name = Form("PosX_Det%dStripFront%dStripBack%d",ii+1,ll+1,kk+1);
				hPosX[ii][ll][kk] = new TH1D(Name.Data(),"",800,-0.9,0.9);
				Name = Form("PosY_Det%dStripFront%dStripBack%d",ii+1,ll+1,kk+1);
				hPosY[ii][ll][kk] = new TH1D(Name.Data(),"",800,-0.9,0.9);
			}

			Name = Form("PosYGoodDiff_Det%dStrip%d",ii+1,ll+1); 
			hPosYgooddiff[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			Name = Form("PosYBadDiff_Det%dStrip%d",ii+1,ll+1);
			hPosYbaddiff[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			
			Name = Form("PosvsEX_Det%dStrip%d",ii+1,ll+1);
			hPosvsEX[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			Name = Form("PosvsEY_Det%dStrip%d",ii+1,ll+1);                   
			hPosvsEY[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			
			Name = Form("PosvsEX_Det%dSide1Strip%d",ii+1,ll+1);
			hPosvsEXSide1[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			Name = Form("PosvsEX_Det%dSide2Strip%d",ii+1,ll+1);
			hPosvsEXSide2[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			Name = Form("PosvsEY_Det%dSide1Strip%d",ii+1,ll+1);
			hPosvsEYSide1[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
			Name = Form("PosvsEY_Det%dSide2Strip%d",ii+1,ll+1);
			hPosvsEYSide2[ii][ll] = new TH2D(Name.Data(),"",80,-0.9,0.9,900,0,900000);
	    	}
		Name = Form("PosX_Det%d",ii+1);
		hPosXdet[ii] = new TH2D(Name.Data(),"",80,-0.9,0.9,800,-0.9,0.9);
		Name = Form("PosY_Det%d",ii+1);
		hPosYdet[ii] = new TH2D(Name.Data(),"",80,-0.9,0.9,800,-0.9,0.9);
		
		Name = Form("HitMap_Det%d",ii+1);
		hHitMap[ii] = new TH2D(Name.Data(),"",32,1,33,32,1,33);
		
	}	
	return kSUCCESS;
}

//----Public method ReInit -----------------------------------------------------------
InitStatus R3BPspxPoscal2CalPar::ReInit() { return kSUCCESS; }

void R3BPspxPoscal2CalPar::Exec(Option_t* option)
{
    /**
     * Gets the parameters for the conversion from Poscal to Cal level. It is called for every mult1=2 and mult2=1 and vice versa event.
     * The parameters are energy, timing and position.
     * Energy: thresholds found and applied
     * Timing: determines if the timing difference between each side of the strip is approx. the same
     * Sets beam peak to 0
     * Position: use interstrip events to determine the position stretching parameters
     *
     * Face 1 is strips in the x-direction (resitive splitting gives y-position)
     * Face 2 is strips in the y-direction (resitive splitting gives x-position)
     *
     * Face 1 is biased
     */
	counter_tot++;
	int nHits = fInterstripPoscal->GetEntries();
       	counter_events++; 

	for (int ii=0; ii<nHits; ii++)
	{
		if (ZombieAreAlive == kTRUE) {break;}

		counter_tot_tot++;
		auto poscal = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(ii));

		//Interstrip Events - used for the position calibration for s473, s444 and s515
		int interstrip_index = InterstripEvent(ii);
		if ((interstrip_index) != -1) 
		{
			//Find info of the otherface detector
			int index_otherface = FacePartner(ii, interstrip_index);
			if (index_otherface != -1)
			{
				auto poscalotherface = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(index_otherface));	

		       		//Get info info for each side of the neuighboring strip 
		       		Bool_t WrongEnergy = kFALSE;
		       		
		       		auto poscalneighbor = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(interstrip_index));
		       		
		       		//Total_Energy of 4 signals. Should equal approx. beam. If double beam, then it's 2 neighboring hits, not interstrip event
		       		int totEnergy = poscal->GetEnergy1() + poscal->GetEnergy2() + poscalneighbor->GetEnergy1() + poscalneighbor->GetEnergy2();
		       		int TotEnergy = poscalotherface->GetEnergy1() + poscalotherface->GetEnergy2();
		       		if ((totEnergy > 1.5*fMaxEnergies[poscal->GetDetector()-1] || totEnergy < fMaxEnergies[poscal->GetDetector()-1]*0.30) || (TotEnergy > 1.4*fMaxEnergies[poscal->GetDetector()-1] || TotEnergy < fMaxEnergies[poscal->GetDetector()-1]*0.80)) {WrongEnergy = kTRUE; counter_bad_energy++;}  

		       		//Important to get calibration params for each strip on left and right side
		       		if (poscal->GetFace()==2) 
		       		{
		       			double PosY = GetStripPosition(poscal->GetStrip());
		       			double posy = GetPosition(poscalotherface->GetEnergy2(),poscalotherface->GetEnergy1());
		       			if (WrongEnergy == kFALSE)
		       			{
		       				counter_interstrip++; 
		       				hHitMap[(poscal->GetDetector()-1)]->Fill(poscal->GetStrip(),poscalotherface->GetStrip());
		       				hPosYdet[(poscal->GetDetector()-1)]->Fill(PosY,posy);
		       				hPosY[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)][(poscal->GetStrip()-1)]->Fill(posy);
		       				if (posy > 0.1+PosY) 
						{
							hPosYbad[(poscal->GetDetector()-1)][0][(poscalotherface->GetStrip()-1)]->Fill(PosY,poscalotherface->GetEnergy1());
							hPosYbad[(poscal->GetDetector()-1)][1][(poscalotherface->GetStrip()-1)]->Fill(PosY,poscalotherface->GetEnergy2());
							hPosYbaddiff[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosY,(poscalotherface->GetEnergy1()-poscalotherface->GetEnergy2()));
						}
						else 
						{
							hPosYgood[(poscal->GetDetector()-1)][0][(poscalotherface->GetStrip()-1)]->Fill(PosY,poscalotherface->GetEnergy1());
							hPosYgood[(poscal->GetDetector()-1)][1][(poscalotherface->GetStrip()-1)]->Fill(PosY,poscalotherface->GetEnergy2());
							hPosYgooddiff[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosY,(poscalotherface->GetEnergy1()-poscalotherface->GetEnergy2()));
						}
		       				hPosvsEY[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosY,(poscalotherface->GetEnergy1()+poscalotherface->GetEnergy2()));
		       				hPosvsEYSide1[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosY,(poscalotherface->GetEnergy1()));
		       				hPosvsEYSide2[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosY,(poscalotherface->GetEnergy2()));
		       				
						hInterstrip[poscal->GetDetector()-1][poscal->GetFace()-1][poscalotherface->GetStrip()-1]->Fill(poscalotherface->GetEnergy1()+poscalotherface->GetEnergy2());
		       				hInterstripTot[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(totEnergy);
		       			}
	    	       			else hInterstripTotBad[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(totEnergy);
		       		} 
				
				else if (poscal->GetFace()==1) 
		       		{
					double PosX = GetStripPosition(poscal->GetStrip());
					double posx = GetPosition(poscalotherface->GetEnergy2(),poscalotherface->GetEnergy1());

		       			if (WrongEnergy == kFALSE)
		       			{
		       				counter_interstrip++; 
		       				hHitMap[(poscal->GetDetector()-1)]->Fill(poscalotherface->GetStrip(),poscal->GetStrip());
		       				hPosXdet[(poscal->GetDetector()-1)]->Fill(PosX,posx);
		       				hPosX[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)][(poscal->GetStrip()-1)]->Fill(posx);
		       				hPosvsEX[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosX,(poscalotherface->GetEnergy1()+poscalotherface->GetEnergy2()));
		       				hPosvsEXSide1[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosX,(poscalotherface->GetEnergy1()));
		       				hPosvsEXSide2[(poscal->GetDetector()-1)][(poscalotherface->GetStrip()-1)]->Fill(PosX,(poscalotherface->GetEnergy2()));
						
						hInterstrip[poscal->GetDetector()-1][poscal->GetFace()-1][poscalotherface->GetStrip()-1]->Fill(poscalotherface->GetEnergy1()+poscalotherface->GetEnergy2());
		       				hInterstripTot[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(totEnergy);
		       			}
	    	       			else hInterstripTotBad[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(totEnergy);
		       		}
	       		}
		}
	}
	fInterstripPoscal->Clear();
}

void R3BPspxPoscal2CalPar::FinishEvent() {}

void R3BPspxPoscal2CalPar::Reset() {}

double fitf(double *x, double *par)
{
        double fitval = par[0] + par[1]*TMath::Tan(x[0]);
        return fitval;
}

void R3BPspxPoscal2CalPar::GetPosParameters() 
{
	TString Name;
	TCanvas *CanPspxX[6];
	TCanvas *CanPspxY[6];
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

	for (int ii=0; ii<fNumDet; ii++)
        {
		Name = Form("CanX_Det%d",ii+1);
		CanPspxX[ii] = new TCanvas(Name.Data(),Name.Data(),500,500);
		Name = Form("CanY_Det%d",ii+1);
		CanPspxY[ii] = new TCanvas(Name.Data(),Name.Data(),500,500);
		
		CanPspxX[ii]->cd();
                BackgroundX->Draw();
		CanPspxY[ii]->cd();
                BackgroundY->Draw();
                
		for (int ll=0; ll < fNumStrips; ll++)
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
                        for (int kk=0; kk < fNumStrips; kk++)
                        {
                                //x-position calc from face 1 signals and is (E2-E1)/(E1+E2)
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
                                                if (offset < 0.10)
                                                {
                                                	double ratio = (1-YY)/(1+YY);
                                                	double constant = ratio*(posXX+1)/(1-posXX);
                                                	graph1->SetPoint(countX, posXX, YY);
                                                	graph1->SetPointError(countX, 0, Error);
                                                	countX++;
						} 
                                                YY=0; posXX=0; offset=0;
                                        }
                                }

 				//y-position calc from face 2 signals and is (E1-E2)/(E1+E2)
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

			//outputfile<<"Inside outputfile"<<std::endl;

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
                                        outputfile<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f12->GetParameter(0)<<"\t"<<f12->GetParameter(1)<<"\t"<<0<<std::endl;
                                }
                        }
                        if ((graph1->GetN()>1 && graph1->GetN()<4) || (graph1->GetN()>3 && fitstatus1 < 0))
                        {
                                TF1 *f11= new TF1("f11","[0]+[1]*x",-0.5,0.5);
                                f11->SetParLimits(1,0.8,1.2);
                                graph1->Fit(f11,"Q");
                                f11->SetLineColor(color);
                                f11->Draw("same");
                                parameter11 = f11->GetParameter(1);
                                if (parameter11 > 0.85 || parameter11 < 1.15) outputfile<<ii<<"\t"<<0<<"\t"<<ll<<"\t"<<f11->GetParameter(0)<<"\t"<<f11->GetParameter(1)<<"\t"<<1<<std::endl;
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
 TFitResultPtr fitinfo = graph2->Fit(f22, "");
                                fitstatus2 = fitinfo;
                                if (fitstatus2 >= 0)
                                {
                                        f22->Draw("same");
                                        outputfile<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f22->GetParameter(0)<<"\t"<<f22->GetParameter(1)<<"\t"<<0<<std::endl;
                                }
                        }
                        if ((graph2->GetN()>1 && graph2->GetN()<4) || (graph2->GetN()>3 && fitstatus2 < 0))
                        {
                                TF1 *f21 = new TF1("f21","[0]+[1]*x",-0.5,0.5);
                                f21->SetParLimits(1,0.8,1.2);
                                graph2->Fit(f21,"Q");
                                f21->SetLineColor(color);
                                f21->Draw("same");
                                double parameter21 = f21->GetParameter(1);
                                if (parameter21 > 0.85 || parameter21 < 1.15) outputfile<<ii<<"\t"<<1<<"\t"<<ll<<"\t"<<f21->GetParameter(0)<<"\t"<<f21->GetParameter(1)<<"\t"<<1<<std::endl;
                        }
                        fitstatus1 = 10;
                        fitstatus2 = 10;
                }
        }
}

void R3BPspxPoscal2CalPar::FinishTask() 
{
	GetPosParameters();
	for (int ii=0; ii < fNumDet; ii++)
	{
		Position->cd();
		if (hPosXdet[ii]->GetEntries()>0) {hPosXdet[ii]->Write();}
		if (hPosYdet[ii]->GetEntries()>0) {hPosYdet[ii]->Write();}
		if (hHitMap[ii]->GetEntries()>0) {hHitMap[ii]->Write();}

		Interstrip->cd();
       		for (int ll=0; ll < fNumStrips; ll++)
       		{
			for (int jj=0; jj < 2; jj++)
			{
				Interstrip->cd();
       				if (hInterstrip[ii][jj][ll]->GetEntries()>0) {hInterstrip[ii][jj][ll]->Write();}
       				if (hInterstripTot[ii][jj][ll]->GetEntries()>0) {hInterstripTot[ii][jj][ll]->Write();}
				if (hInterstripTotBad[ii][jj][ll]->GetEntries()>0) {hInterstripTotBad[ii][jj][ll]->Write();}
				PositionStripsGood->cd();
       				if (hPosYgood[ii][jj][ll]->GetEntries()>0) {hPosYgood[ii][jj][ll]->Write();}
				PositionStripsBad->cd();
       				if (hPosYbad[ii][jj][ll]->GetEntries()>0) {hPosYbad[ii][jj][ll]->Write();}
			}
			PositionStrips->cd();
			for (int kk=0; kk < fNumStrips; kk++)
			{
       				if (hPosX[ii][ll][kk]->GetEntries()>0) {hPosX[ii][ll][kk]->Write();}
       				if (hPosY[ii][ll][kk]->GetEntries()>0) {hPosY[ii][ll][kk]->Write();}
			}
			PositionStripsGood->cd();
       			if (hPosYgooddiff[ii][ll]->GetEntries()>0) {hPosYgooddiff[ii][ll]->Write();}
			PositionStripsBad->cd();
       			if (hPosYbaddiff[ii][ll]->GetEntries()>0) {hPosYbaddiff[ii][ll]->Write();}
       			Energy->cd();
			if (hPosvsEX[ii][ll]->GetEntries()>0) {hPosvsEX[ii][ll]->Write();}
       			if (hPosvsEY[ii][ll]->GetEntries()>0) {hPosvsEY[ii][ll]->Write();}
       			EnergySingleSide->cd();
       			if (hPosvsEXSide1[ii][ll]->GetEntries()>0) {hPosvsEXSide1[ii][ll]->Write();}
       			if (hPosvsEXSide2[ii][ll]->GetEntries()>0) {hPosvsEXSide2[ii][ll]->Write();}
       			if (hPosvsEYSide1[ii][ll]->GetEntries()>0) {hPosvsEYSide1[ii][ll]->Write();}
       			if (hPosvsEYSide2[ii][ll]->GetEntries()>0) {hPosvsEYSide2[ii][ll]->Write();}
       		}
	}
	printf("total events: %d, tot_tot events: %d, events of interest: %d, interstrip: %d, bad_interstrip: %d, tot_interstrip: %d, bad_energy_partner: %d, bad_time_partner: %d, good_partner: %d, bad energy: %d\n",counter_tot, counter_tot_tot, counter_events, counter_interstrip, counter_bad_interstrip, counter_tot_interstrip, counter_bad_time_partner, counter_bad_energy_partner, counter_good_partner, counter_bad_energy);
	outputfile.close();
}

int R3BPspxPoscal2CalPar::FacePartner(int index, int index2) 
{
	int nHits = fInterstripPoscal->GetEntries();
	int faceindex = -1;
       	int Time=-1000; int Time_partner=-9000;

	for (int jj = 0; jj < nHits; jj++)
	{
		if (jj != index && jj != index2)
		{
			auto poscal = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(index));
			auto poscalpartner = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(jj));
	
			if (poscal->GetFace() != poscalpartner->GetFace())
			{
	 			int TotalEnergy = poscalpartner->GetEnergy1() + poscalpartner->GetEnergy2();
				if (TotalEnergy > 0.25*fMaxEnergies[poscal->GetDetector()-1]) 
				{
					Time_partner = (poscalpartner->GetTime1() + poscalpartner->GetTime2())/2.; 
					Time = (poscal->GetTime1() + poscal->GetTime2())/2.;
	 				if (std::abs(Time - Time_partner) < 10) 
					{
						faceindex = jj; 
	 					counter_good_partner++; 
					}
					else {counter_bad_time_partner++;}
	 			}
				else {counter_bad_energy_partner++;}
			}
		}
	}
	return faceindex;
}

//Find events with mult = 2 on 1 side and mult = 1 on other side. Make sure these events are neighbors. Make sure time difference is acceptable. Output is the index of the neighboring strip, same side 
int R3BPspxPoscal2CalPar::InterstripEvent(int index) 
{
	auto poscal = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(index));
	int index_IS = -1;

	for (int ii=0; ii<fInterstripPoscal->GetEntries(); ii++)
	{
		if (ii != index)
		{
			auto poscalneighbor = dynamic_cast<R3BPspxInterstripPoscalData*>(fInterstripPoscal->At(ii));
	
			if (poscal->GetFace() == poscalneighbor->GetFace())
			{
				if ((poscalneighbor->GetStrip()-poscal->GetStrip())==1) 
				{
					counter_tot_interstrip++;
					double timestrip1 = (poscal->GetTime1() + poscal->GetTime2())/2.;
					double timestrip2 = (poscalneighbor->GetTime1() + poscalneighbor->GetTime2())/2.;
					if (std::abs(timestrip1-timestrip2) < 15) {index_IS = ii;}
					else {counter_bad_interstrip++;}
				} 
			}
		}
	}
	return index_IS; 
}

double R3BPspxPoscal2CalPar::GetPosition(int Energy1, int Energy2) 
{
	return ((double)Energy1-(double)Energy2)/((double)Energy1+(double)Energy2);
}

double R3BPspxPoscal2CalPar::GetStripPosition(int Strip) 
{
	return (Strip-16.)/16.;
}

void R3BPspxPoscal2CalPar::SetNumDet(int det) {fNumDet = det;} 
void R3BPspxPoscal2CalPar::SetNumStrips(int strip) {fNumStrips = strip;}
void R3BPspxPoscal2CalPar::SetNumExpt(int expt) {fNumExpt = expt;} 
void R3BPspxPoscal2CalPar::SetParOutName(TString paroutname) {fParOutName = paroutname;}
void R3BPspxPoscal2CalPar::SetMaxEnergies(std::vector<double> maxenergies) {fMaxEnergies = maxenergies;}

ClassImp(R3BPspxPoscal2CalPar)
