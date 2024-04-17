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
// -----                    R3BPspxMapped2Poscal                      -----
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
#include "R3BPspxMapped2Poscal.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxPoscalData.h"
#include "R3BPspxInterstripPoscalData.h"
#include "R3BPspxPoscalPar.h"
#include "R3BPspxContFact.h"

#include <bits/stdc++.h>
#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <stdio.h>

//ROOT headers
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include "TDirectory.h"
#include "TProfile.h"

//R3BPspsxMapped2Poscal: Default Constructor ---------------------------------------
R3BPspxMapped2Poscal::R3BPspxMapped2Poscal()
    : FairTask("PspxMapped2Poscal", 1)
{
}

//R3BPspsxMapped2Poscal: Standard Constructor ---------------------------------------
R3BPspxMapped2Poscal::R3BPspxMapped2Poscal(const char* name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

//Virtual R3BPspsxMapped2Poscal: Destructor ----------------------------------------
R3BPspxMapped2Poscal::~R3BPspxMapped2Poscal()
{
    LOG(info) << "R3BPspxMapped2Poscal: Delete instance";
    delete fPoscal;
    delete fInterstripPoscal;
}

//----Public method Init -----------------------------------------------------------
InitStatus R3BPspxMapped2Poscal::Init()
{
	LOG(info) << "R3BPspxMapped2Poscal: Init" ;
	
	//FairRootManager
	FairRootManager* fMan = FairRootManager::Instance();
	if (!fMan) { LOG(error) << "R3BPspxMapped2Poscal::Init(). No FairRootManager found"; return kFATAL;}
	
	//Get Mapped Data
	fMapped = dynamic_cast<TClonesArray*>(fMan->GetObject("PspxMapped"));
	if (!fMapped) {LOG(error) << "R3BPspxMapped2Poscal::Init(). No PspxMappedData found."; return kFATAL;}

	fHeader = dynamic_cast<R3BEventHeader*>(fMan->GetObject("R3BEventHeader"));
	
	//Register Poscal Level Data
	fPoscal = new TClonesArray("R3BPspxPoscalData");
	fMan->Register("PspxPoscalData","PSPX Poscal", fPoscal, kTRUE);
	fPoscal->Clear();
	
	//Register Interstrip Poscal Level Data
	fInterstripPoscal = new TClonesArray("R3BPspxInterstripPoscalData");
	fMan->Register("PspxInterstripPoscalData","PSPX InterstripPoscal", fInterstripPoscal, kTRUE);
	fInterstripPoscal->Clear();
	
	//Container needs to be created in tcal/R3BCalContFact.cxx AND R3BCal needs
	//to be set as dependency in CMakelists.txt (in this case in the psp dir.)
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {return kFATAL;}
	
	if (!fNumDet)
	{
	    LOG(error) << "R3BPspxMapped2Poscal::Init(). No Detectors detected.";
	    return kFATAL;
	}
	
	if (!fNumStrips)
	{
	    LOG(error) << "R3BPspxMapped2Poscal::Init(). No Strips found.";
	    return kFATAL;
	}
	

	//Define TH1/TH2 for the fits and other info
	//4 Parameters per strip per detectors: 2 sides and 2 faces
	printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	TString Name;
	for (int ii = 0; ii < fNumDet; ii++)
	{
    		for (int jj = 0; jj < 2; jj++)
    		{
    			for (int kk = 0; kk < 2; kk++)
    			{
	   			for  (int ll = 0; ll<fNumStrips; ll++)
    				{
					Timing->cd();
           				Name = Form("Time_Det%d_Face%d_Side%d_Strip%d",ii+1,jj+1,kk+1,ll+1);
	    				hTime[ii][jj][kk][ll] = new TH1F(Name.Data(), Name.Data(), 5000,-5000,5000);
					Energy->cd();
           				Name = Form("Energy_Det%d_Face%d_Side%d_Strip%d",ii+1,jj+1,kk+1,ll+1);
	    				hEnergy[ii][jj][kk][ll] = new TH1F(Name.Data(), Name.Data(), 10000,0,1000000);
 
 
           				if (kk == 0)
					{
						Energy->cd();
           					Name = Form("EnergyTot_Det%d_Face%d_Strip%d",ii+1,jj+1,ll+1);
	    					hEnergyTot[ii][jj][ll] = new TH1F(Name.Data(), Name.Data(), 10000,0,1000000);
 
					}
				}
				Overflow->cd();
           			Name = Form("Overflow_Det%d_Face%d_Side%d",ii+1,jj+1,kk+1);
	    			hOverflow[ii][jj][kk] = new TH2F(Name.Data(), Name.Data(),32,0,32,10000,-1000000,0);
			}
	    	}
		Name = Form("TimeDiffPartner_Det%d",ii+1);
		hTimeDiffPartner[ii] = new TH1F(Name.Data(),Name.Data(),1000,-500,500);
		
		Name = Form("Mult_Det%d",ii+1);
		hMult[ii] = new TH2F(Name.Data(),Name.Data(),50,0,50,50,0,50);
		
		Name = Form("MultInterstrip_Det%d",ii+1);
		hMultInterstrip[ii] = new TH2F(Name.Data(),Name.Data(),50,0,50,50,0,50);
	}	
	return kSUCCESS;
}

//----Public method ReInit -----------------------------------------------------------
InitStatus R3BPspxMapped2Poscal::ReInit() { return kSUCCESS; }

void R3BPspxMapped2Poscal::Exec(Option_t* option)
{
    /**
     * Gets the parameters for the conversion from Mapped to Cal level. It is called for every event.
     * The parameters are energy, timing and position.
     * Energy: thresholds found and applied
     * Timing: determines if the timing difference between each side of the strip is approx. the same
     * Sets beam peak to 0
     * Position: use interstrip events to determine the position stretching parameters
     *
     * Face 1 is strips in the x-direction (resitive splitting gives y-position)
     * Face 2 is strips in the y-direction (resitive splitting gives x-position)
     *
     * Side 1 is left, Side 2 is right (look beam-downstream)
     * Side 1 is up, Side 2 is down (look beam-downstream)
     *
     * Face 1 is biased
     */

	for (int ii=0; ii<fNumDet; ii++)
	{
		for (int jj=0; jj<2; jj++)
		{
			for (int kk=0; kk<2; kk++)
			{
				for (int ll=0; ll<fNumStrips; ll++)
				{
					fArray[ii][jj][kk][ll] = -1;
				}
			}
		}

	}

	counter_tot++;

	int nHits = fMapped->GetEntries();
	counter_events++; 

	//Fill index Array for all hits - used to calculate multiplicity, find partners, identify interstrip events
	for (int ii = 0; ii < nHits; ii++)
	{
		auto mapped = dynamic_cast<R3BPspxMappedData*>(fMapped->At(ii));
		if (mapped == NULL) {ZombieAreAlive = kTRUE; printf("I'm am Zombie\n"); break;}  // removes bad events where -nan is filled into the fMapped array
		if (mapped->IsOverflow() == kTRUE) {fArray[mapped->GetDetector()-1][mapped->GetFace()-1][mapped->GetSide()-1][mapped->GetStrip()-1] = -1;}
		else fArray[mapped->GetDetector()-1][mapped->GetFace()-1][mapped->GetSide()-1][mapped->GetStrip()-1] = ii; 
	}

	for (int ii=0; ii<nHits; ii++)
	{
        	if (ZombieAreAlive == kTRUE) {break;}

		counter_tot_tot++;
        	auto mapped = dynamic_cast<R3BPspxMappedData*>(fMapped->At(ii));
		if (mapped->IsOverflow() == kTRUE) {continue;}
		if (mapped->GetSide()-1 == 0)
		{
    			if (MatchedEvents(ii) == kFALSE) {counter_matched_bad++; continue;}
        		//printf("Event Number: %d, Det: %d, Face: %d, Side: %d, Strip: %d, nHits: %d, Index: %d\n",counter_tot,mapped->GetDetector(),mapped->GetFace(),mapped->GetSide(),mapped->GetStrip(),nHits,fArray[mapped->GetDetector()-1][mapped->GetFace()-1][mapped->GetSide()-1][mapped->GetStrip()-1]);

			//Used to get the array index for the opposite side and opposite face of a detector
			auto mapped2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[mapped->GetDetector()-1][mapped->GetFace()-1][1][mapped->GetStrip()-1]));	

			double time_corr1=0; double time_corr2=0;

			//need to add code so this is just for s473+s444 (Nik's FW)
        		if (fNumExpt == 473 || fNumExpt == 444)
			{
				time_corr1 = GetTimeCorr_s473(mapped->GetTime());
        			time_corr2 = GetTimeCorr_s473(mapped2->GetTime());
			}

			//How to get the timing for s515 (CALIFA FW) 
        		if (fNumExpt == 515)
			{
				time_corr1 = GetTimeCorr_s515(mapped->GetTime(),mapped->GetTrigger());
        			time_corr2 = GetTimeCorr_s515(mapped2->GetTime(),mapped->GetTrigger());
			}

       			if (IsGoodStrip(mapped->GetDetector()-1, mapped->GetFace()-1, mapped->GetStrip()-1))
       			{
				int OtherFace = -1;
				if (mapped->GetFace() == 1) OtherFace=2;
				else OtherFace = 1;	
				
				int mult1 = MultFace(mapped->GetDetector()-1,mapped->GetFace()-1);
				int mult2  = MultFace(mapped->GetDetector()-1,OtherFace-1);

				//Multiplicity requirement: Mult x-face has to equal mult y-face - timing and energy calibration needs this requirement
				if ((mult1 == 1) && (mult2 == 1)) 
				{
					//printf("Det: %d, Face: %d, Side: %d, Strip:%d, Timing: %lf, Timing2: %lf\n",mapped->GetDetector()-1,mapped->GetFace()-1,mapped->GetSide()-1,mapped->GetStrip()-1,time_corr1,time_corr2);
					int facepartnerindex = FacePartner(ii);
					if (facepartnerindex != -1)
					{
						hTime[(mapped->GetDetector()-1)][mapped->GetFace()-1][0][mapped->GetStrip()-1]->Fill(time_corr1);
						hTime[(mapped2->GetDetector()-1)][mapped2->GetFace()-1][1][mapped2->GetStrip()-1]->Fill(time_corr2);
						hEnergy[(mapped->GetDetector()-1)][mapped->GetFace()-1][0][mapped->GetStrip()-1]->Fill(mapped->GetEnergy());
						hEnergy[(mapped2->GetDetector()-1)][mapped2->GetFace()-1][1][mapped2->GetStrip()-1]->Fill(mapped2->GetEnergy());
						hEnergyTot[(mapped->GetDetector()-1)][mapped->GetFace()-1][mapped->GetStrip()-1]->Fill(mapped->GetEnergy() + mapped2->GetEnergy());
					}
				}
				else {counter_mult_mismatch++;}

				if (mapped->GetFace()-1 == 0) hMult[mapped->GetDetector()-1]->Fill(mult1,mult2);
					
				if (mult1 > 0)
				{
					//printf("Det: %d, Face: %d, Strip: %d, Energy: %lf, Energy: %lf, time1: %lf, time2: %lf, mult: %d, mult: %d\n",mapped->GetDetector(),mapped->GetFace(),mapped->GetStrip(),(double)mapped->GetEnergy(),(double)mapped2->GetEnergy(),(double)time_corr1,(double)time_corr2,multfront, multback);
					AddPoscalData(mapped->GetDetector(),mapped->GetFace(),mapped->GetStrip(),(double)mapped->GetEnergy(),(double)mapped2->GetEnergy(),(double)time_corr1,(double)time_corr2,mult1, IsMainStrip(mapped->GetDetector()-1, mapped->GetFace()-1, mapped->GetStrip()-1));
				}
				if ((mult1==2 && mult2==1) || (mult1==1 && mult2==2))
				{
					if (IsMainStrip(mapped->GetDetector()-1, mapped->GetFace()-1, mapped->GetStrip()-1)==kTRUE)
					{
						AddInterstripPoscalData(mapped->GetDetector(),mapped->GetFace(),mapped->GetStrip(),(double)mapped->GetEnergy(),(double)mapped2->GetEnergy(),(double)time_corr1,(double)time_corr2);
						hMultInterstrip[mapped->GetDetector()-1]->Fill(mult1,mult2);
					}	
				}	
       			}
       			else {counter_timing_bad++;}
		}
	}
}

void R3BPspxMapped2Poscal::Reset() {}

void R3BPspxMapped2Poscal::FinishEvent() 
{
	for (int ii=0; ii<fNumDet; ii++)
	{
		for (int jj=0; jj<2; jj++)
		{
			for (int kk=0; kk<2; kk++)
			{
				for (int ll=0; ll<fNumStrips; ll++)
				{
					fArray[ii][jj][kk][ll]=-1;
				}
			}
		}
	}
	fMapped->Clear();
	fPoscal->Clear();
	fInterstripPoscal->Clear();
}

void R3BPspxMapped2Poscal::FinishTask() 
{
	for (int ii=0; ii < fNumDet; ii++)
	{
		Mult->cd();
		if (hMult[ii]->GetEntries()>0) {hMult[ii]->Write();}
		if (hMultInterstrip[ii]->GetEntries()>0) {hMultInterstrip[ii]->Write();}

		for (int jj=0; jj < 2; jj++)
		{
			for (int kk=0; kk < 2; kk++)
			{
				for (int ll=0; ll < fNumStrips; ll++)
				{
					Timing->cd();
					if (hTime[ii][jj][kk][ll]->GetEntries()>0) {hTime[ii][jj][kk][ll]->Write();}
					Energy->cd();
					if(hEnergy[ii][jj][kk][ll]->GetEntries()>0) {hEnergy[ii][jj][kk][ll]->Write();}
					if (kk == 0)
					{
						if (hEnergyTot[ii][jj][ll]->GetEntries()>0) {hEnergyTot[ii][jj][ll]->Write();}
					}
				}
				Overflow->cd();
				if (hOverflow[ii][jj][kk]->GetEntries()>0) {hOverflow[ii][jj][kk]->Write();}
			}
		}
	}
        printf("total events: %d, tot_tot events: %d, events of interest: %d, bad matched: %d, mulit mismatch: %d,bad timing: %d\n",counter_tot, counter_tot_tot,counter_events, counter_matched_bad, counter_mult_mismatch, counter_timing_bad);
	outputfile.close();
}

double R3BPspxMapped2Poscal::GetTimeCorr_s473(int time)
{
	//corrects for the timing offset
	int corr_time = time - 2047;
	if (corr_time < 0) {corr_time = 4095 + corr_time;}
	return corr_time;
}

double R3BPspxMapped2Poscal::GetTimeCorr_s515(int time, int trigger)
{
	//corrects for the timing offset
	int corr_time = time - trigger;
	return corr_time;
}

Bool_t R3BPspxMapped2Poscal::MatchedEvents(int index) 
{
	int index2 = -1, index3 = -1, index4 = -1;
	auto mapped = dynamic_cast<R3BPspxMappedData*>(fMapped->At(index));

        //printf("Event Number: %d, Det: %d, Face: %d, Side: %d, Strip: %d, Index: %d\n",counter_tot,mapped->GetDetector(),mapped->GetFace(),mapped->GetSide(),mapped->GetStrip(), fArray[mapped->GetDetector()-1][mapped->GetFace()-1][mapped->GetSide()-1][mapped->GetStrip()-1]);
	if (fMapped->GetEntries() < 4) {return kFALSE;}

	if (fArray[mapped->GetDetector()-1][mapped->GetFace()-1][1][mapped->GetStrip()-1] != -1) {index2 = 1;}
	else return kFALSE;
	for (int jj = 0; jj < fNumStrips; jj++)
	{
		int OtherFace = -1;
		if (mapped->GetFace() == 1) OtherFace=2;
		else OtherFace = 1;	
		if (fArray[mapped->GetDetector()-1][OtherFace-1][0][jj] != -1 && fArray[mapped->GetDetector()-1][OtherFace-1][1][jj] != -1) 
		{
			index3 = 1; index4 = 1; return kTRUE;
		}
	}
        //printf("Det: %d, Face: %d, Side: %d, Strip: %d, Index: %d\n",mapped->GetDetector()-1,1,1,mapped->GetStrip(), fArray[mapped->GetDetector()-1][1][mapped->GetSide()-1][mapped->GetStrip()-1]);
	if (index2 == 1 && index3 == 1 && index4 == 1) {return kTRUE;}
	else return kFALSE;
}

bool R3BPspxMapped2Poscal::IsGoodStrip(int det, int face, int strip)
{
	if (fArray[det][face][0][strip] != -1 && fArray[det][face][1][strip] != -1) 
	{
		auto mapped1 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][0][strip]));
		auto mapped2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][1][strip]));
		int mapped_time1 = GetTimeCorr_s473(mapped1->GetTime());
		int mapped_time2 = GetTimeCorr_s473(mapped2->GetTime());
		if (std::abs(mapped_time1-mapped_time2)<10) {return kTRUE;}
	}
	return kFALSE;
}

bool R3BPspxMapped2Poscal::IsMainStrip(int det, int face, int strip)
{
	if (IsGoodStrip(det, face, strip) == kTRUE)
	{
		auto mapped1 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][0][strip]));
		auto mapped2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][1][strip]));
		if ((mapped1->GetEnergy() + mapped2->GetEnergy()) > 0.20*fMaxEnergies[mapped1->GetDetector()-1]) {return kTRUE;}
	}
	return kFALSE;
}

int R3BPspxMapped2Poscal::MultFace(int det, int face)
{
	int mult = 0;
	for (int ii = 0; ii < fNumStrips; ii++)
	{
		if (fArray[det][face][0][ii] != -1 && fArray[det][face][1][ii] != -1)
		{
			auto mapped1 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][0][ii]));
			auto mapped2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[det][face][1][ii]));

			int totE = mapped1->GetEnergy() + mapped2->GetEnergy();

			if (totE > 0.20 * fMaxEnergies[mapped1->GetDetector()-1]) {mult+=IsGoodStrip(det, face, mapped1->GetStrip()-1);}
			//printf("Det: %d, face: %d, strip: %d, totE: %d\n",det,face,mapped1->GetStrip(), totE);
		}
	}
	return mult;
}

int R3BPspxMapped2Poscal::FacePartner(int index) 
{
	auto mapped = dynamic_cast<R3BPspxMappedData*>(fMapped->At(index));
	auto mapped2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[mapped->GetDetector()-1][mapped->GetFace()-1][1][mapped->GetStrip()-1]));

	int Time=-1000; int Time_partner=-9000;
	int faceindex=-1;

	for (int jj = 0; jj < fNumStrips; jj++)
	{
		int OtherFace = -1;
		if (mapped->GetFace() == 1) OtherFace=2;
		else OtherFace = 1;
		int mintemp = 100000000;	
		if (fArray[mapped->GetDetector()-1][OtherFace-1][0][jj] != -1 && fArray[mapped->GetDetector()-1][OtherFace-1][1][jj] != -1) 
		{
			auto mappedpartner = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[mapped->GetDetector()-1][OtherFace-1][0][jj]));
			auto mappedpartner2 = dynamic_cast<R3BPspxMappedData*>(fMapped->At(fArray[mapped->GetDetector()-1][OtherFace-1][1][jj]));

			int TotalEnergy = mappedpartner->GetEnergy() + mappedpartner2->GetEnergy();
			int totE = mapped->GetEnergy() + mapped2->GetEnergy();
			int minE = std::abs(totE-TotalEnergy);
	
			if (TotalEnergy > 0.1*fMaxEnergies[mapped->GetDetector()-1] && minE < mintemp && minE/TotalEnergy < 0.4) {

				Time_partner = (GetTimeCorr_s473(mappedpartner->GetTime()) + GetTimeCorr_s473(mappedpartner2->GetTime()))/2.; 
				Time = (GetTimeCorr_s473(mapped->GetTime()) + GetTimeCorr_s473(mapped2->GetTime()))/2.; 
				if (std::abs(Time - Time_partner) < 10)
				{	
					faceindex = fArray[mapped->GetDetector()-1][OtherFace-1][mapped->GetSide()-1][jj]; 
				}
				mintemp = minE;
			}
		}
	}
	return faceindex;
}

R3BPspxPoscalData* R3BPspxMapped2Poscal::AddPoscalData(int det, int face, int strip, double energy1, double energy2, double time1, double time2, int mult1, bool mainstrip)
{
	TClonesArray& clref = *fPoscal;
	int size = clref.GetEntriesFast();
	//printf("Inside TClonesArray\nSize: %d, Det: %d, Face: %d, Strip: %d, Energy1: %lf, Energy2: %lf, Time1: %lf, Time2: %lf, Mult: %d, Mult: %d\n", size, det, face, strip, energy1, energy2, time1, time2, mult1, mult2);
	return new (clref[size]) R3BPspxPoscalData(det, face, strip, energy1, energy2, time1, time2, mult1, mainstrip);
}	

R3BPspxInterstripPoscalData* R3BPspxMapped2Poscal::AddInterstripPoscalData(int det, int face, int strip, double energy1, double energy2, double time1, double time2)
{
	TClonesArray& clref = *fInterstripPoscal;
	int size = clref.GetEntriesFast();
	//printf("Inside TClonesArray\nSize: %d, Det: %d, Face: %d, Strip: %d, Energy1: %lf, Energy2: %lf, Time1: %lf, Time2: %lf, Mult: %d\n", size, det, face, strip, energy1, energy2, time1, time2, mult);
	return new (clref[size]) R3BPspxInterstripPoscalData(det, face, strip, energy1, energy2, time1, time2);
}	

ClassImp(R3BPspxMapped2Poscal)
