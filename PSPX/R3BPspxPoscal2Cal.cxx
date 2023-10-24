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

// ----------------------------------------------------------------
// -----                 R3BPspxPoscal2Cal              -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl          -----
// -----              Modified  Jul 2022  by A. Jedele        -----
// ----------------------------------------------------------------

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// PSP headers
#include "R3BEventHeader.h"
#include "R3BPspxPoscal2Cal.h"
#include "R3BPspxCalData.h"
#include "R3BPspxPoscalData.h"
#include "R3BPspxCalPar.h"

#include "TClonesArray.h"
#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <tuple>

//ROOT headers
#include "TH1F.h" 
#include "TH2F.h" 
#include "TMath.h" 
#include "TProfile.h" 
#include "TRandom.h" 
#include "TFile.h"

R3BPspxPoscal2Cal::R3BPspxPoscal2Cal()
    : FairTask("PspxPoscal2Cal", 1)
{
}

R3BPspxPoscal2Cal::R3BPspxPoscal2Cal(const char* name, int iVerbose)
    	: FairTask(name, iVerbose)
{
}

R3BPspxPoscal2Cal::~R3BPspxPoscal2Cal()
{
	delete fPoscal;
	delete fCal;
}

void R3BPspxPoscal2Cal::SetParContainers()
{
	//Parameter Container
	//Reading PspCalPar from FairRuntimeDB
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {LOG(error) << "FairRuntimeDb not found";}

}

InitStatus R3BPspxPoscal2Cal::Init()
{
	//FairRootManager
	FairRootManager* fMan = FairRootManager::Instance();
	if (!fMan) { LOG(error) << "R3BPspxPoscal2Cal::Init(). No FairRootManager found"; return kFATAL;}
	
	//Get Poscal Data
	fPoscal = dynamic_cast<TClonesArray*>(fMan->GetObject("PspxPoscalData"));
	if (!fPoscal) {LOG(error) << "R3BPspxPoscal2Cal::Init(). No PspxPoscalData found."; return kFATAL;}

	//R3BEventHeader for trigger information, if needed!
	fHeader = dynamic_cast<R3BEventHeader*>(fMan->GetObject("R3BEventHeader"));
	
	//Register Cal Level Data
	fCal = new TClonesArray("R3BPspxCalData");
	fMan->Register("PspxCalData","PSPX Cal", fCal, kTRUE);
	fCal->Clear();
	
	//Container needs to be created in tcal/R3BCalContFact.cxx AND R3BCal needs
	//to be set as dependency in CMakelists.txt (in this case in the psp dir.)
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {return kFATAL;}
	
	if (!fNumDet)
	{
	    LOG(error) << "R3BPspxPoscal2Cal::Init(). No Detectors detected.";
	    return kFATAL;
	}
	
	if (!fNumStrips)
	{
	    LOG(error) << "R3BPspxPoscal2Cal::Init(). No Strips found.";
	    return kFATAL;
	}


	printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	TString Name;
	for (int ii = 0; ii < fNumDet; ii++)
	{
    		for (int jj = 0; jj < 2; jj++)
    		{
			for (int kk = 0; kk < fNumStrips; kk++)
			{
				FitType[ii][jj][kk] = -1;
				for (int ll = 0; ll < 2; ll++)
				{
					GainFactors[ii][jj][kk][ll] = -1;
				}
				
				Name = Form("PosEnergy_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);
	    			hPosEnergy[ii][jj][kk] = new TH2F(Name.Data(), Name.Data(),200,-1.0,1.0,10000,0,1000000);
 
           			Name = Form("PosEnergyCorr_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);
	    			hPosEnergyCorr[ii][jj][kk] = new TH2F(Name.Data(), Name.Data(), 200,-1.0,1.0,10000,0,1000000);	
			}
           		Name = Form("Time_Det%d_Face%d",ii+1,jj+1);
	    		hTime[ii][jj] = new TH2F(Name.Data(), Name.Data(),32,1,33,4096,0,4096);
 
           		Name = Form("Energy_Det%d_Face%d",ii+1,jj+1);
	    		hEnergy[ii][jj] = new TH2F(Name.Data(), Name.Data(),32,1,33,10000,0,1000000);
 
           		Name = Form("EnergyCorr_Det%d_Face%d",ii+1,jj+1);
	    		hEnergyCorr[ii][jj] = new TH2F(Name.Data(), Name.Data(), 32,1,33,10000,0,1000000);
           			
        		Name = Form("PosRaw%dFace%d",ii+1,jj+1);
			hPosRaw[ii][jj] = new TH2F(Name.Data(), Name.Data(),330,0,33,1000,-1.0,1.0);
        		
        		Name = Form("Pos%d_Face%d", ii+1, jj+1);
			hPos[ii][jj] = new TH2F(Name.Data(), Name.Data(),330,0,33,1000,-1.0,1.0);
        		
        		Name = Form("PosCompare%d_Face%d", ii+1, jj+1);
			hPosCompare[ii][jj] = new TH2F(Name.Data(), Name.Data(),1000,-1.0,1.0,1000,-1.0,1.0);
		}
	}

	TString filename = Form("%s",fParInName.Data());
	std::ifstream infile(filename);
	if (infile.bad()) {printf("Zombie File!!!\n"); return kFATAL;}

	int Det=0, Face=0, Strip=0;
	double par[2] = {0};
	char line[1000];
	int countertan[6]={0}, fittype=-1;

	while(!infile.eof())
	{
		infile.getline(line,1000);
		sscanf(line,"%d %d %d %lf %lf %d", &Det, &Face, &Strip, &par[0], &par[1], &fittype);
		printf("%d %d %d %lf %lf %d\n",Det,Face,Strip,par[0],par[1], fittype);
		
		int ipar=0;
		if (par[1]!=0) {ipar = 2; countertan[Det]++;}
		else if (par[0]!=0) ipar = 1;
		else continue;
		
		FitType[Det][Face][Strip] = fittype;
		for (int ii = 0; ii < ipar; ii++) 
		{
			GainFactors[Det][Face][Strip][ii] = par[ii];
			if (ipar>0) totalpar[Det][ii] = totalpar[Det][ii] + par[ii]; 
		}
		//if (Det != 0) {printf("SScanf line. Det; %d, Face: %d, Strip: %d, StripBack: %d, ratio: %lf, GainFactor: %lf\n", Det, Face, Strip, StripBack, ratio, GainFactors[Det][Face][Strip][StripBack]);}
	}
	for (int idet = 0; idet < 6; idet++) 
	{
		for (int ii = 0; ii < 2; ii++) 
		{
			totalpar[idet][ii] = totalpar[idet][ii]/(double)countertan[idet];
		       printf("Totalpar[%d]: %lf\n",ii,totalpar[idet][ii]);	
		}
	}
	
	printf("Done with initialization\n\n");
	return kSUCCESS;
}

InitStatus R3BPspxPoscal2Cal::ReInit() {return kSUCCESS;}

void R3BPspxPoscal2Cal::Exec(Option_t* option)
{
	int nHits = fPoscal->GetEntries();
	if (nHits > 0)
	{
		for (int ii = 0; ii < nHits; ii++)
		{
			auto poscal = dynamic_cast<R3BPspxPoscalData*>(fPoscal->At(ii));
			double pos = (poscal->GetEnergy2() - poscal->GetEnergy1())/(poscal->GetEnergy1() + poscal->GetEnergy2());
			double posCorr;
			
			if (poscal->GetFace()==1 && FitType[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]==1) {posCorr = TMath::ATan(pos - GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][0])/GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][1];} 
			else if (poscal->GetFace()==1 && FitType[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]==0) {posCorr = (pos - GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][0])/GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][1];} 
			else if (poscal->GetFace()==1) {posCorr = TMath::ATan(pos - totalpar[poscal->GetDetector()-1][0])/totalpar[poscal->GetDetector()-1][1];} 
			
			else if (poscal->GetFace()==2 && FitType[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]==1) {posCorr = TMath::ATan(-pos - GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][0])/GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][1];} 
			else if (poscal->GetFace()==2 && FitType[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]==0) {posCorr = (-pos - GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][0])/GainFactors[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1][1];} 
			else {posCorr = TMath::ATan(-pos - totalpar[poscal->GetDetector()-1][0])/totalpar[poscal->GetDetector()-1][1];} 
		
			double corrE2;
		        if (poscal->GetFace()==1) corrE2 = poscal->GetEnergy1()*(1+posCorr)/(1-posCorr);	
			else corrE2 = poscal->GetEnergy1()*(1-posCorr)/(1+posCorr);	
			//printf("Energy2: %lf, CorrEnergy2: %lf\n", poscal->GetEnergy2(), corrE2);
			double tot = poscal->GetEnergy1() + corrE2;
			double avgtime = (poscal->GetTime1() + poscal->GetTime2())/2.;

			hEnergy[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(poscal->GetStrip(), poscal->GetEnergy1() + poscal->GetEnergy2());
			hEnergyCorr[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(poscal->GetStrip(), tot);
			hPosEnergy[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(pos, poscal->GetEnergy1() + poscal->GetEnergy2());
			hPosEnergyCorr[poscal->GetDetector()-1][poscal->GetFace()-1][poscal->GetStrip()-1]->Fill(posCorr, tot);
			hPosCompare[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(pos, posCorr);
			hPos[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(poscal->GetStrip(), posCorr);
			hPosRaw[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(poscal->GetStrip(), pos);
			hTime[poscal->GetDetector()-1][poscal->GetFace()-1]->Fill(poscal->GetStrip(), avgtime);

			AddCalData(poscal->GetDetector(), poscal->GetFace(), poscal->GetStrip(), tot, posCorr, avgtime, poscal->GetMult(), poscal->IsMainStrip());
			
			counter++;
		}
	}
}

void R3BPspxPoscal2Cal::Reset() {}

void R3BPspxPoscal2Cal::FinishEvent() 
{
	fCal->Clear();
}

void R3BPspxPoscal2Cal::FinishTask()
{
	for (int ii=0; ii < fNumDet; ii++)
	{
		for (int jj=0; jj < 2; jj++)
		{
			//poscal2calHisto->cd();
			//if(hTime[ii][jj]->GetEntries() > 0) {hTime[ii][jj]->Write();}
			//if(hEnergy[ii][jj]->GetEntries() > 0) {hEnergy[ii][jj]->Write();}
			//if(hEnergyCorr[ii][jj]->GetEntries() > 0) {hEnergyCorr[ii][jj]->Write();}
			//if(hPosCompare[ii][jj]->GetEntries() > 0) {hPosCompare[ii][jj]->Write();}
			//if(hPosRaw[ii][jj]->GetEntries() > 0) {hPosRaw[ii][jj]->Write();}
			//if(hPos[ii][jj]->GetEntries() > 0) {hPos[ii][jj]->Write();}
			//for (int kk=0; kk<fNumStrips; kk++)
			//{
			//	if(hPosEnergy[ii][jj][kk]->GetEntries() > 0) {hPosEnergy[ii][jj][kk]->Write();}
			//	if(hPosEnergyCorr[ii][jj][kk]->GetEntries() > 0) {hPosEnergyCorr[ii][jj][kk]->Write();}
			//}
		}
	}
	printf("counter_pairing: %d, totmult_counter: %d,  timing_counter%d, wee timing counter: %d\n", counter_pairing, totmult_counter, timing_counter,timing_small_counter);
}
			
R3BPspxCalData* R3BPspxPoscal2Cal::AddCalData(int det, int face, int strip, double totenergy, double pos, double avgtime, int mult, bool mainstrip)
{
	TClonesArray& clref = *fCal;
	int size = clref.GetEntriesFast();
	return new (clref[size]) R3BPspxCalData(det, face, strip, totenergy, pos, avgtime, mult, mainstrip);
}	

void R3BPspxPoscal2Cal::SetNumExpt(int expt) {fNumExpt = expt;}
void R3BPspxPoscal2Cal::SetNumDet(int det) {fNumDet = det;}
void R3BPspxPoscal2Cal::SetNumStrips(int strip) {fNumStrips = strip;}
void R3BPspxPoscal2Cal::SetParInName(TString parinname) {fParInName = parinname;}

ClassImp(R3BPspxPoscal2Cal)
