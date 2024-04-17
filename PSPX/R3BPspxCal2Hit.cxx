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
// -----                 R3BPspxCal2Hit              -----
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
#include "R3BPspxCal2Hit.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxHitPar.h"

#include "TClonesArray.h"
#include <iostream>
#include <fstream>
#include <limits>

//ROOT headers
#include "TF1.h" 
#include "TH1F.h" 
#include "TH2F.h" 
#include "TMath.h" 
#include "TProfile.h" 
#include "TRandom.h" 
#include "TFile.h"
#include "TGraphErrors.h"
#include "TGraph.h"
#include "TCanvas.h"

R3BPspxCal2Hit::R3BPspxCal2Hit()
    : FairTask("PspxCal2Hit", 1)
{
}

R3BPspxCal2Hit::R3BPspxCal2Hit(const char* name, int iVerbose)
    	: FairTask(name, iVerbose)
    	, fNumDet(0)
    	, fNumStrips(0)
    	, fParInName1("DefaultOutput")
    	, fParInName2("DefaultOutput")
    	, fParInName3("DefaultOutput")
    	, fParInName4("DefaultOutput")
{
}

R3BPspxCal2Hit::~R3BPspxCal2Hit()
{
	delete fHit;
	delete fCal;
}

InitStatus R3BPspxCal2Hit::Init()
{
	auto olddir = gDirectory; 
	ff = TFile::Open("psp.root","recreate");

	//FairRootManager
	FairRootManager* fMan = FairRootManager::Instance();
	if (!fMan) { LOG(error) << "R3BPspxCal2Hit::Init(). No FairRootManager found"; return kFATAL;}
	
	//Get Cal Data
	fCal = dynamic_cast<TClonesArray*>(fMan->GetObject("PspxCalData"));
	if (!fCal) {LOG(error) << "R3BPspxCal2Hit::Init(). No PspxCalData found."; return kFATAL;}
	
	// R3BEventHeader for trigger information, if needed!
	fHeader = dynamic_cast<R3BEventHeader*>(fMan->GetObject("R3BEventHeader"));
	
	//Container needs to be created in tcal/R3BCalContFact.cxx AND R3BCal needs
	//to be set as dependency in CMakelists.txt (in this case in the psp dir.)
	FairRuntimeDb* rtdb = FairRuntimeDb::instance();
	if (!rtdb) {return kFATAL;}
	
	if (!fNumDet)
	{
	    LOG(error) << "R3BPspxCal2Hit::Init(). No Detectors detected.";
	    return kFATAL;
	}
	
	if (!fNumStrips)
	{
	    LOG(error) << "R3BPspxCal2Hit::Init(). No Strips found.";
	    return kFATAL;
	}

	//Register the Hit TClonesArray for output
	fHit = new TClonesArray("R3BPspxHitData");
	fMan->Register("PspxHitData", "PSPX Hit", fHit, kTRUE);
	fHit->Clear();

	PositionEnergyCorrection(); 
	ChanneltoEnergyLossConversion(); 
	printf("\n\nEnd of init\n\n");
	ff->Close();
	olddir->cd();

	TString hName;
	for (int ii = 0; ii < fNumDet; ii++)
	{
		for (int jj = 0; jj < 2; jj++)
		{
			hName = Form("FinalEnergy_Det%d_Face%d",ii+1,jj+1);
			hFinalEnergy[ii][jj] = new TH1D(hName.Data(),hName.Data(),8000,0,800); 
			
			hName = Form("FinalTime_Det%d_Face%d",ii+1,jj+1);
			hFinalTime[ii][jj] = new TH1D(hName.Data(),hName.Data(),5000,0,10000); 
			
			for (int kk = 0; kk < fNumStrips; kk++)
			{
				hName = Form("FinalEnergy_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);
				hFinalEnergyStrip[ii][jj][kk] = new TH1D(hName.Data(),hName.Data(),8000,0,800); 
				
				hName = Form("FinalTime_Det%d_Face%d_Strip%d",ii+1,jj+1,kk+1);
				hFinalTimeStrip[ii][jj][kk] = new TH1D(hName.Data(),hName.Data(),5000,0,10000); 
			}
		}
	}

	return kSUCCESS;
}

InitStatus R3BPspxCal2Hit::ReInit() {return kSUCCESS;}

void R3BPspxCal2Hit::Exec(Option_t* option)
{
	int nHits = fCal->GetEntries();
	if (nHits > 0)
	{
		double PosCorrEnergy = 0;
		double FinalCorr = 0;
		double ZZ = 0;
		double pos = 0;
		double yoffset;
		for (int ii=0; ii<nHits; ii++)
		{
			auto cal = dynamic_cast<R3BPspxCalData*>(fCal->At(ii));
	
			double ECorr;
			if (posefits[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1][0]>0)
			{
				ECorr = cal->GetEnergy() - (posefits[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1][1] * cal->GetPosition() + posefits[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1][2] * cal->GetPosition() * cal->GetPosition());
			}
			else
			{
				ECorr = cal->GetEnergy() - (totalfits[cal->GetDetector()-1][cal->GetFace()-1][1] * cal->GetPosition() + totalfits[cal->GetDetector()-1][cal->GetFace()-1][2] * cal->GetPosition() * cal->GetPosition());
			}

			FinalCorr = (ECorr-LinearFit[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1][0])/LinearFit[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1][1];
			ZZ = FinalCorr * 50. / BB[fRunId];

			if (FinalCorr<800) hFinalEnergy[cal->GetDetector()-1][cal->GetFace()-1]->Fill(FinalCorr);
			hFinalTime[cal->GetDetector()-1][cal->GetFace()-1]->Fill(cal->GetTime());
			if (FinalCorr<1000) hFinalEnergyStrip[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1]->Fill(FinalCorr);
			hFinalTimeStrip[cal->GetDetector()-1][cal->GetFace()-1][cal->GetStrip()-1]->Fill(cal->GetTime());
			pos = cal->GetPosition()*9.57/2.;
		
			//printf("Det: %d, Face: %d, Strip: %d, FinalCorr: %lf, ZZ: %lf, pos: %lf\n", cal->GetDetector(), cal->GetFace(), cal->GetStrip(), FinalCorr, ZZ, pos);
			AddHitData(cal->GetDetector(), cal->GetFace(), cal->GetStrip(), FinalCorr, ZZ, pos, cal->GetTime(), cal->GetMult(), cal->IsMainStrip());
			
			PosCorrEnergy = 0;
			FinalCorr = 0;
			pos = 0;
		}
	}
}

void R3BPspxCal2Hit::FinishEvent() {fCal->Clear(); fHit->Clear();}

void R3BPspxCal2Hit::FinishTask()
{
	for (int ii=0; ii < fNumDet; ii++)
	{
		for(int jj=0; jj<2; jj++)
		{	
			if (hFinalEnergy[ii][jj]->GetEntries()>0) hFinalEnergy[ii][jj]->Write();
			if (hFinalTime[ii][jj]->GetEntries()>0)   hFinalTime[ii][jj]->Write();
			
			for (int kk=0; kk<fNumStrips; kk++)
			{
				if (hFinalEnergyStrip[ii][jj][kk]->GetEntries()>0) hFinalEnergyStrip[ii][jj][kk]->Write();
				if (hFinalTimeStrip[ii][jj][kk]->GetEntries()>0)   hFinalTimeStrip[ii][jj][kk]->Write();
			}
		}
	}
}

void R3BPspxCal2Hit::PositionEnergyCorrection()
{

	for (int ii = 0; ii< fNumDet; ii++) {
		for (int jj=0; jj<2; jj++) {
			for (int ipars = 0; ipars<4; ipars++) {
				for (int kk=0; kk<fNumStrips; kk++) {
					posefits[ii][jj][kk][ipars] = 0;
				}
				totalfits[ii][jj][ipars] = 0;
			}
		}
	}


	char line[1000]={0};
	int Det=0, Face=0, Strip=0;
	int countertotfits[6][2] = {{0}};
	double par[4] = {0};
	int entries = 0;

	TString filename = Form("%s",fParName.Data());
	std::ifstream infile(filename);
	if (infile.bad()) {printf("Zombie File1!!!\n");}
	printf("Input file: %s\n",fParName.Data());
	
	while(!infile.eof())
	{
		infile.getline(line,1000);
		sscanf(line,"%d %d %d %lf %lf %lf %d", &Det, &Face, &Strip, &par[0], &par[1], &par[2], &entries);
		totalfits[Det][Face][3] = totalfits[Det][Face][3] + (double)entries;
		//printf("%d %d %d %lf %lf %lf\n", Det, Face, Strip, par[0], par[1], par[2]);
		for (int ipar = 0; ipar<3; ipar++) 
		{
			posefits[Det][Face][Strip][ipar] = par[ipar];
			totalfits[Det][Face][ipar] = totalfits[Det][Face][ipar] + par[ipar]/100;
		}
		 countertotfits[Det][Face]++;
		if (Strip == 0) {printf("I is brokey!\n"); break;}
	}
	for (int idet=0; idet<6; idet++)
	{
		for (int iface=0; iface<2; iface++)
		{
			for (int ipar=0; ipar<3; ipar++)
			{
				totalfits[idet][iface][ipar] = totalfits[idet][iface][ipar] / totalfits[Det][Face][3] * 100;
			}
			totalfits[idet][iface][3] = totalfits[idet][iface][3] / (double)countertotfits[idet][iface];
		}
	}
}

void R3BPspxCal2Hit::ChanneltoEnergyLossConversion()
{
	outputfile.open(fParOutName.Data());

	for (int ii = 0; ii< fNumDet; ii++)
	{
		for (int jj=0; jj<2; jj++)
		{
			for (int irun = 0; irun<4; irun++)
			{
				for (int ipars = 0; ipars<4; ipars++) 
				{
					for (int kk=0; kk<fNumStrips; kk++)
					{
						GausFits[ii][jj][kk][irun][ipars] = 0;
					}
					TotalFits[ii][jj][irun][ipars] = 0;
				}
			}
		}
	}

	char line1[4][1000]={0};
	int Det1=0, Face1=0, Strip1=0;
	double pos1=0.;
	double totE1=0.;
	double totEerror1=0;
	double par1=0, par2=0, par3=0;
	int counter[6][2][4]={{{0}}};
	int entries=0;	
	
	for (int ii=0; ii<4; ii++)
	{
		//Input par files (in ascii form) into arrays to use	
		TString filename;
	        if (ii==0) filename= Form("%s",fParInName1.Data());
		else if (ii==1) filename= Form("%s",fParInName2.Data());
	        else if (ii==2) filename= Form("%s",fParInName3.Data());
	        else filename= Form("%s",fParInName4.Data());
		std::ifstream infile(filename);
		if (infile.bad()) {printf("Zombie File1!!!\n");}
		printf("Input file: %s\n",filename.Data());

		while(!infile.eof())
		{
			infile.getline(line1[ii],1000);
			sscanf(line1[ii],"%d %d %d %lf %lf %lf %d", &Det1, &Face1, &Strip1, &pos1, &totE1, &totEerror1, &entries);
			//printf("%d %d %d %lf %lf %lf %d\n", Det1, Face1, Strip1, pos1, totE1, totEerror1, entries);
			GausFits[Det1][Face1][Strip1][ii][0] = pos1;
			GausFits[Det1][Face1][Strip1][ii][1] = totE1;
			GausFits[Det1][Face1][Strip1][ii][2] = totEerror1;
			GausFits[Det1][Face1][Strip1][ii][3] = entries;
			TotalFits[Det1][Face1][ii][0] = TotalFits[Det1][Face1][ii][0] + pos1*entries;
			TotalFits[Det1][Face1][ii][1] = TotalFits[Det1][Face1][ii][1] + totE1/100.*entries;
			TotalFits[Det1][Face1][ii][2] = TotalFits[Det1][Face1][ii][2] + totE1*entries;
			TotalFits[Det1][Face1][ii][3] = TotalFits[Det1][Face1][ii][3] + entries;
			counter[Det1][Face1][ii]++;
			if (Strip1 == 0) {printf("I is brokey!\n"); break;}
			Det1=0; Face1=0; Strip1=0; pos1=0; totE1=0; totEerror1=0; entries=0;
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
				TotalFits[idet][iface][irun][3] = TotalFits[idet][iface][irun][3]/(double)counter[idet][iface][irun];
				//printf("%d %d %d %lf %lf %lf\n", idet, iface, irun, TotalFits[idet][iface][irun][0], TotalFits[idet][iface][irun][1], TotalFits[idet][iface][irun][2]);
			}
		}
	}

	

	//printf("\n\nNumDet: %d, NumStrips: %d\n\n\n",fNumDet,fNumStrips);
	for (int ii = 0; ii < fNumDet; ii++)
	{
		for (int jj = 0; jj < 2; jj++)
		{
			for (int kk = 0; kk < fNumStrips; kk++)
			{
				double minimum;
				double error = 0, energy = 0;
				std::vector<double> venergy{};
				std::vector<double> verror{};
				std::vector<double> vbb{};
	
				for (int irun=0; irun<4; irun++)
				{
					if (posefits[ii][jj][kk][0]>0 && GausFits[ii][jj][kk][irun][1]>0)
					{
						energy = GausFits[ii][jj][kk][irun][1] - (posefits[ii][jj][kk][1] * GausFits[ii][jj][kk][irun][0] + posefits[ii][jj][kk][2] * GausFits[ii][jj][kk][irun][0] * GausFits[ii][jj][kk][irun][0]);
						
						error = GausFits[ii][jj][kk][irun][2];
						
						venergy.push_back(energy);
						verror.push_back(error);
						vbb.push_back(BB[irun]);
					}

					else if (GausFits[ii][jj][kk][irun][0]>0)
					{
						energy = GausFits[ii][jj][kk][irun][1] - (totalfits[ii][jj][1] * GausFits[ii][jj][kk][irun][0] + totalfits[ii][jj][2] * GausFits[ii][jj][kk][irun][0] * GausFits[ii][jj][kk][irun][0]);
					
						error = GausFits[ii][jj][kk][irun][2];
						
						venergy.push_back(energy);
						verror.push_back(error);
						vbb.push_back(BB[irun]);
					}

				}

				TString nom = Form("ECalib_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
				if (venergy.size() > 1) 
				{	
					if (venergy.size()!=4) printf("I'm less than 4. Det: %d, Face: %d, Strip: %d, size: %lu\n",ii+1, jj+1, kk+1, venergy.size()); 
					int sizingup = venergy.size();
					double srenergy[sizingup];
					double srerror[sizingup];
					double srbb[sizingup];
				        for (int ithings = 0; ithings < sizingup; ithings++)
					{
						srenergy[ithings] = venergy[ithings];
						srerror[ithings]  = verror[ithings];
						srbb[ithings] = vbb[ithings];
					}
					tBetheBloch[ii][jj][kk] = new TGraphErrors(sizingup,srbb,srenergy,0,srerror);
					tBetheBloch[ii][jj][kk]->SetName(nom.Data());
					tBetheBloch[ii][jj][kk]->SetTitle(nom.Data());
					
					TF1 *linearfit = new TF1("BetheBlock", "[0]+[1]*(x)", BB[0], BB[3]);
					tBetheBloch[ii][jj][kk]->Fit(linearfit, "Q0NEC", "", BB[0], BB[3]);
					nom = Form("Fit_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					linearfit->SetName(nom.Data());
					linearfit->SetTitle(nom.Data());
					
					ff->WriteTObject(tBetheBloch[ii][jj][kk]);
					ff->WriteTObject(linearfit);
					
					outputfile<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<linearfit->GetParameter(0)<<"\t"<<linearfit->GetParameter(1)<<"\t"<<linearfit->GetChisquare()/3.<<std::endl;

					LinearFit[ii][jj][kk][0] = linearfit->GetParameter(0);
					LinearFit[ii][jj][kk][1] = linearfit->GetParameter(1);
					LinearFit[ii][jj][kk][2] = linearfit->GetChisquare()/3.;
					std::cout<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<LinearFit[ii][jj][kk][0]<<"\t"<<LinearFit[ii][jj][kk][1]<<std::endl;
					minimum = 0;
				
					venergy.clear();
					verror.clear();
					vbb.clear();	

					tBetheBloch[ii][jj][kk]->Delete();
				}
			
				if (tBetheBloch[ii][jj][kk] != NULL)
				{	
					for (int irun = 0; irun < 4; irun++)
					{	
						if (venergy.size() == 1)
						{
							double correction = venergy[0];
							double difference = 0;
							if (vbb[0] == BB[0]) difference = correction - TotalFits[ii][jj][0][1];
							else if (vbb[0] == BB[1]) difference = correction - TotalFits[ii][jj][1][1]; 
							else if (vbb[0] == BB[2]) difference = correction - TotalFits[ii][jj][2][1]; 
							else difference = correction - TotalFits[ii][jj][3][1]; 
							energy = TotalFits[ii][jj][irun][1] + difference - (totalfits[ii][jj][1] * TotalFits[ii][jj][irun][0] + totalfits[ii][jj][2] * TotalFits[ii][jj][irun][0] * TotalFits[ii][jj][irun][0]);
							
							error = TotalFits[ii][jj][irun][2];
							
							if (irun == 0)
							{
								venergy.clear();
								verror.clear();
								vbb.clear();	
							}
							
							venergy.push_back(energy);
							verror.push_back(error);
							vbb.push_back(BB[irun]);
						}	

						else
						{
							energy = TotalFits[ii][jj][irun][1] - (totalfits[ii][jj][1] * TotalFits[ii][jj][irun][0] + totalfits[ii][jj][2] * TotalFits[ii][jj][irun][0] * TotalFits[ii][jj][irun][0]);
							
							error = TotalFits[ii][jj][irun][2];
							
							venergy.push_back(energy);
							verror.push_back(error);
							vbb.push_back(BB[irun]);
						}
					}	
		
					int sizingup = venergy.size();
					double srenergy[sizingup];
					double srerror[sizingup];
					double srbb[sizingup];
		        		for (int ithings = 0; ithings < sizingup; ithings++)
					{
						srenergy[ithings] = venergy[ithings];
						srerror[ithings]  = verror[ithings];
						srbb[ithings] = vbb[ithings];
					}
					tBetheBloch[ii][jj][kk] = new TGraphErrors(sizingup,srbb,srenergy,0,srerror);
					tBetheBloch[ii][jj][kk]->SetName(nom.Data());
					tBetheBloch[ii][jj][kk]->SetTitle(nom.Data());
					
					TF1 *linearfit = new TF1("BetheBlock", "[0]+[1]*(x)", BB[0], BB[3]);
					tBetheBloch[ii][jj][kk]->Fit(linearfit, "Q0NEC", "", BB[0], BB[3]);
					nom = Form("Fit_Det%dFace%dStrip%d",ii+1,jj+1,kk+1);
					linearfit->SetName(nom.Data());
					linearfit->SetTitle(nom.Data());
					
					ff->WriteTObject(tBetheBloch[ii][jj][kk]);
					ff->WriteTObject(linearfit);
					
					outputfile<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<linearfit->GetParameter(0)<<"\t"<<linearfit->GetParameter(1)<<"\t"<<linearfit->GetChisquare()/3.<<std::endl;

					LinearFit[ii][jj][kk][0] = linearfit->GetParameter(0);
					LinearFit[ii][jj][kk][1] = linearfit->GetParameter(1);
					LinearFit[ii][jj][kk][2] = linearfit->GetChisquare()/3.;
					std::cout<<ii<<"\t"<<jj<<"\t"<<kk<<"\t"<<LinearFit[ii][jj][kk][0]<<"\t"<<LinearFit[ii][jj][kk][1]<<std::endl;
					minimum = 0;
		
					venergy.clear();
					verror.clear();
					vbb.clear();	

					tBetheBloch[ii][jj][kk]->Delete();
				}
			}
		}
	}
	outputfile.close();
}

R3BPspxHitData* R3BPspxCal2Hit::AddHitData(int det, int face, int strip, double energy, double charge, double pos, double time, int mult, bool mainstrip)
{
	TClonesArray& clref = *fHit;
	int size = clref.GetEntriesFast();
	return new (clref[size]) R3BPspxHitData(det, face, strip, energy, charge, pos, time, mult, mainstrip);
}	

void R3BPspxCal2Hit::SetRunTag(int runtag) {fRunId = runtag;}
void R3BPspxCal2Hit::SetNumDet(int det) {fNumDet = det;}
void R3BPspxCal2Hit::SetNumStrips(int strip) {fNumStrips = strip;}
void R3BPspxCal2Hit::SetParName(TString parname) {fParName = parname;}
void R3BPspxCal2Hit::SetParInName1(TString parinname1) {fParInName1 = parinname1;}
void R3BPspxCal2Hit::SetParInName2(TString parinname2) {fParInName2 = parinname2;}
void R3BPspxCal2Hit::SetParInName3(TString parinname3) {fParInName3 = parinname3;}
void R3BPspxCal2Hit::SetParInName4(TString parinname4) {fParInName4 = parinname4;}
void R3BPspxCal2Hit::SetParOutName(TString paroutname) {fParOutName = paroutname;}
void R3BPspxCal2Hit::SetParOutName2(TString paroutname2) {fParOutName2 = paroutname2;}

ClassImp(R3BPspxCal2Hit)
