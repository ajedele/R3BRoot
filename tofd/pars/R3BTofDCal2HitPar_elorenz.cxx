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

#include "R3BTofDCal2HitPar_elorenz.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTofDHitModulePar.h"
#include "R3BTofDHitPar.h"
#include "R3BTofDMappingPar.h"
#include "R3BTofdCalData.h"
//#include "/u/ajedele/R3BRoot_fork/r3bdata/pspxData/R3BPspxHitData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMath.h"
#include "TProfile.h"
#include "TSpectrum.h"
#include "TSystem.h"
#include "TVirtualFitter.h"


#include <iostream>
#include <stdlib.h>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
	double c_range_ns = 2048. * 5.;
	double c_bar_coincidence_ns = 20.; // nanoseconds.
} // namespace

R3BTofDCal2HitPar_elorenz::R3BTofDCal2HitPar_elorenz()
	: R3BTofDCal2HitPar_elorenz("R3BTofDCal2HitPar_elorenz", 1)
{
}

R3BTofDCal2HitPar_elorenz::R3BTofDCal2HitPar_elorenz(const char* name, int iVerbose)
	: FairTask(name, iVerbose)
	, fNEvents(0)
	  , fParameter(1)
	  , fHistoFile("")
{
}

R3BTofDCal2HitPar_elorenz::~R3BTofDCal2HitPar_elorenz() {}

InitStatus R3BTofDCal2HitPar_elorenz::Init()
{
	R3BLOG(info, "");
	FairRootManager* rm = FairRootManager::Instance();
	if (!rm)
	{
		R3BLOG(fatal, "FairRootManager not found");
		return kFATAL;
	}

	fHeader = dynamic_cast<R3BEventHeader*>(rm->GetObject("EventHeader."));
	R3BLOG_IF(fatal, NULL == fHeader, "EventHeader. not found");

	fCalData = dynamic_cast<TClonesArray*>(rm->GetObject("TofdCal"));
	R3BLOG_IF(fatal, NULL == fCalData, "TofdCal not found");

	//fPspxHitData = dynamic_cast<TClonesArray*>(rm->GetObject("PspxHitData"));
	//R3BLOG_IF(fatal, NULL == fCalTriggerItems, "PspxHitData not found");

	fHitPar = dynamic_cast<R3BTofDHitPar*>(FairRuntimeDb::instance()->getContainer("tofdHitPar"));
	if (!fHitPar)
	{
		R3BLOG(error, "Could not get access to tofdHitPar container");
		return kFATAL;
	}

	SetHistogramsNull();
	CreateHistograms();


	printf("End of init\n");
	return kSUCCESS;
}

void R3BTofDCal2HitPar_elorenz::Exec(Option_t* option)
{
	if(fParameter == 5 || fParameter == -3 || fParameter==7) return;
	// test for requested trigger (if possible)
	if ((fTrigger >= 0) && (fHeader) && (fHeader->GetTrigger() != fTrigger))
		return;

	// fTpat = 1-16; fTpat_bit = 0-15
	int fTpat_bit = fTpat - 1;
	if (fTpat_bit >= 0)
	{
		int itpat = fHeader->GetTpat();
		int tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
		if ((fHeader) && (tpatvalue == 0))
			return;
	}

	// ToFD detector
	std::vector<std::vector<unsigned int>> multihits(fNofPlanes, std::vector<unsigned int>(fPaddlesPerPlane));
	for (int i = 0; i < fNofPlanes; ++i)
		for (int j = 0; j < fPaddlesPerPlane; ++j)
			multihits[i][j] = 0;

	unsigned int nHits = fCalData->GetEntriesFast();
	//unsigned int nHits_pspx = fPspxHitData->GetEntriesFast();

	// Organize cals into bars.
	struct Entry
	{
		std::vector<R3BTofdCalData*> top;
		std::vector<R3BTofdCalData*> bot;
	};
	std::map<size_t, Entry> bar_map;
	for (int ihit = 0; ihit < nHits; ihit++)
	{
		auto* hit = dynamic_cast<R3BTofdCalData*>(fCalData->At(ihit));
		size_t idx = (hit->GetDetectorId() - 1) * fPaddlesPerPlane + (hit->GetBarId() - 1);
		auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
		auto& vec = 1 == hit->GetSideId() ? ret.first->second.top : ret.first->second.bot;
		vec.push_back(hit);
	}

	//std::vector<std::tuple<int,int,double>> pspxvect;
	//for (int ihit = 0; ihit < nHits_pspx; ihit++)
	//{
	//	auto* hit = dynamic_cast<R3BPspxHitData*>(fPspxHitData->At(ihit));
	//	pspxvect.push_back(make_tuple(hit->GetDetector(), hit->GetFace(), hit->GetEnergy()));
	//}

	// Find coincident PMT hits.
	for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
	{
		auto const& top_vec = it->second.top;
		auto const& bot_vec = it->second.bot;
		size_t top_i = 0;
		size_t bot_i = 0;
		for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
		{
			auto top = top_vec.at(top_i);
			auto bot = bot_vec.at(bot_i);


			// Shift the cyclic difference window by half a window-length and move it back,
			auto top_ns =
				fmod(top->GetTimeLeading_ns() + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
			auto bot_ns =
				fmod(bot->GetTimeLeading_ns() + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;

			auto dt = top_ns - bot_ns;
			if (std::abs(dt) < c_bar_coincidence_ns)
			{
				// Hit
				int iPlane = top->GetDetectorId(); // 1..n
				int iBar = top->GetBarId();        // 1..n
				if (iPlane > fNofPlanes)             // this also errors for iDetector==0
				{
					R3BLOG(error, "More detectors than expected! Det: " << iPlane << " allowed are 1.." << fNofPlanes);
					continue;
				}
				if (iBar > fPaddlesPerPlane) // same here
				{
					R3BLOG(error, "More bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane);
					continue;
				}
				auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
				auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);
				// register multi hits
				multihits[iPlane - 1][iBar - 1] += 1;

				double Ctop_tot = 0./0.;
				double Cbot_tot = 0./0.;

				//First step - 
				//Energy calibration: calculates geometric mean of ToT1 and ToT2
				//Use y = center sweep run for this
				if (fParameter == 1)
				{
					// calculate tdiff
					auto tdiff = (bot_ns) - (top_ns);
					auto posToT = TMath::Log(top_tot / bot_tot);
					auto THit = (top_ns + bot_ns) / 2.;
					// fill control histograms
					fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->Fill(TMath::Log(top_tot), TMath::Log(bot_tot));
					if((sqrt(top_tot*bot_tot)>fToTMin && sqrt(top_tot*bot_tot)<fToTMax))
					{    
						fh_tofd_TotPm[iPlane - 1][0]->Fill(iBar, top_tot);
						fh_tofd_TotPm[iPlane - 1][0]->Fill(-iBar - 1, bot_tot);
						fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
						fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->Fill(posToT, sqrt(top_tot * bot_tot));
						fhTsync[iPlane - 1]->Fill(iBar, THit);
						fh1_walk_t[iPlane-1][iBar-1]->Fill(top_tot,top_ns);
						fh1_walk_b[iPlane-1][iBar-1]->Fill(bot_tot,bot_ns);
					}
				}

				else if (fTofdQ > 0. && fParameter > 1)
				{	
					auto par = fHitPar->GetModuleParAt(iPlane, iBar);
					if (!par)
					{
						R3BLOG(error,"Hit par not found, Plane: "<<top->GetDetectorId()<<", Bar: "<<top->GetBarId()); break;
					}

					// calculate tdiff
					auto tdiff = (bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2());
					Ctop_tot = top_tot  * par->GetToTOffset2();
					Cbot_tot = bot_tot  * par->GetToTOffset1();
					auto THit = (top_ns + bot_ns) / 2. - par->GetSync();

					// fill control histograms
					fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->Fill(TMath::Log(top_tot), TMath::Log(bot_tot));
					fh_tofd_TotPm[iPlane - 1][0]->Fill(iBar, top_tot);
					fh_tofd_TotPm[iPlane - 1][0]->Fill(-iBar - 1, bot_tot);
					fh_tofd_TotPm[iPlane - 1][1]->Fill(iBar, top_tot  * par->GetToTOffset2());
					fh_tofd_TotPm[iPlane - 1][1]->Fill(-iBar - 1, bot_tot * par->GetToTOffset1());

					double posToT=0.;
					if (fParameter == 2) posToT = TMath::Log((Ctop_tot) / (Cbot_tot));
					else posToT = par->GetLambda() * log((Ctop_tot) / (Cbot_tot));

					double pos = 0;
					if (fParameter == 3)
					{	
						pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();
						fhTot1vsPos[iPlane - 1][iBar - 1]->Fill(pos, Cbot_tot);
						fhTot2vsPos[iPlane - 1][iBar - 1]->Fill(pos, Ctop_tot);
					}

					if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax))
					{
						// Time differences of one paddle; offset  histo
						fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
						if (multihits[iPlane - 1][iBar - 1] < 2) fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->Fill(posToT, sqrt(Ctop_tot * Cbot_tot));
						// Time of hit
						fhTsync[iPlane - 1]->Fill(iBar, THit);
					}
				
					if (fParameter > 3 && fTofdZ == true)
					{
						LOG(debug) << "Prepare histo for quenching correction";
						double parf[4]{};
						parf[0] = par->GetPar1a();
						parf[1] = par->GetPar1b();
						parf[2] = par->GetPar1c();
						parf[3] = par->GetPar1d();


						double parq[4]{};
						parq[0] = par->GetPola();
						parq[1] = par->GetPolb();
						parq[2] = par->GetPolc();
						parq[3] = par->GetPold();

						// Calculate charge
						double qb = 0.;
						double qbx = 0.;
					
						if (fTofdSmiley)
						{	
							if(parq[0]==0. && parq[1]==0. && parq[2]==0. && parq[3]==0.) parq[0] = 1.;

							qb = TMath::Sqrt(Ctop_tot * Cbot_tot);
							qbx=TMath::Sqrt(Ctop_tot*Cbot_tot)/(parq[0]+parq[1]*posToT+parq[2]*pow(posToT,2)+parq[3]*pow(posToT,3));
							// theory says: dE ~ Z^2 but we see quenching -> just use linear and fit the rest
							qb = qb * fTofdQ / parq[0]; 
							qbx = qbx * fTofdQ;
						}
						else
						{
							// double exponential
							auto q1 = Cbot_tot/(parf[0] * (exp(-parf[1] * (pos + 100.)) + exp(-parf[2] * (pos + 100.))) + parf[3]*1.);
							parq[0] = par->GetPar2a();
							parq[1] = par->GetPar2b();
							parq[2] = par->GetPar2c();
							parq[3] = par->GetPar2d();
							
							for(int ii=0; ii<4; ii++) cout<<"parf["<<ii<<"] "<<parf[ii]<<" parq["<<ii<<"] "<<parq[ii]<<endl;
							
							auto q2 = Ctop_tot/(parq[0]*(exp(-parq[1]*(pos + 100.))+exp(-parq[2]*(pos+100.)))+parq[3]*1.);
							// theory says: dE ~ Z^2 but we see quenching -> just use linear and fit the rest
							q1 = q1 * fTofdQ / fMeanToT;
							q2 = q2 * fTofdQ / fMeanToT;
							cout<<"q1 "<<q1<<" q2 "<<q2<<endl;
							qb = (q1 + q2) / 2.;
						}

						if (multihits[iPlane - 1][iBar - 1] < 2)
						{
							if (par->GetPar1za() > 0) qbx = par->GetPar1za() + par->GetPar1za()*qbx + par->GetPar1za()*qbx*qbx;
							//if (bPspx == true)
							//{
							//	bool pspx_cond = false;
							//	double energy1 = 0, energy2 = 0, energy3 = 0, energy4 = 0;
							//	for (int ipspx = 0; ipspx < pspxvect.size(); ipspx++)
							//	{
							//		if (get<0>(pspxvect[ipspx])==4 && get<1>(pspxvect[ipspx])==1) energy1 = get<2>(pspxvect[ipspx]);
							//		if (get<0>(pspxvect[ipspx])==4 && get<1>(pspxvect[ipspx])==2) energy2 =         get<2>(pspxvect[ipspx]);
							//		if (get<0>(pspxvect[ipspx])==6 && get<1>(pspxvect[ipspx])==1) energy3 =         get<2>(pspxvect[ipspx]);
							//		if (get<0>(pspxvect[ipspx])==6 && get<1>(pspxvect[ipspx])==2) energy4 =         get<2>(pspxvect[ipspx]);
							//	}
							//	if (energy1>(fPspxEnergy-3) || energy4>(fPspxEnergy-3) || energy1>(fPspxEnergy-3) || energy3>(fPspxEnergy-3)) pspx_cond = true;
							//
							//	if (pspx_cond == false || (iBar<fBarMin || iBar>fBarMax))
							//	{
							//		fhQvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qb);
							//		fhQXvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qbx);
							//		fhQvsPos1D[iPlane - 1][iBar - 1]->Fill(qb);
							//		fhQXvsPos1D[iPlane - 1][iBar - 1]->Fill(qbx);
							//	}
							//	energy1 = 0;
							//	energy2 = 0;
							//	energy3 = 0;
							//	energy4 = 0;
							//}
							//else
							//{	
								fhQvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qb);
								fhQXvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qbx);
								fhQvsPos1D[iPlane - 1][iBar - 1]->Fill(qb);
								fhQXvsPos1D[iPlane - 1][iBar - 1]->Fill(qbx);
							//}
						}
					}
				}
				++top_i;
				++bot_i;
				fNEvents += 1;
			}
			else if (dt < 0 && dt > -c_range_ns / 2) {++top_i;}
			else {++bot_i;}
		}
	}
	//pspxvect.clear();
}

void R3BTofDCal2HitPar_elorenz::SetHistogramsNull()
{
	for (int iPlane=0; iPlane < fNofPlanes; iPlane++)
	{
		fhTdiff[iPlane] = NULL;
		fhTsync[iPlane] = NULL;
		for (int index = 0; index < 2; index++)
		{	
			fh_tofd_TotPm[iPlane][index] = NULL;
		}	
	
		for (int iBar=0; iBar < fPaddlesPerPlane; iBar++)
		{
			fhSqrtQvsPosToT[iPlane][iBar] = NULL;
			fhLogTot1vsLogTot2[iPlane][iBar] = NULL;
			fhQvsPos[iPlane][iBar] = NULL;
			fhQXvsPos[iPlane][iBar] = NULL;
			fhQvsPos1D[iPlane][iBar] = NULL;
			fhQXvsPos1D[iPlane][iBar] = NULL;
			fhTot1vsPos[iPlane][iBar] = NULL;
			fhTot2vsPos[iPlane][iBar] = NULL;
		}
	}
}

void R3BTofDCal2HitPar_elorenz::CreateHistograms()
{
	TString strName;
	double max_charge = fMaxQ;
	bool fWalk = true;

	for (int iPlane=0; iPlane < fNofPlanes; iPlane++)
	{
		printf("I'm inside CreateHistograms: Plane %d\n",iPlane);	

		strName = Form("Time_Diff_Plane_%d", iPlane+1);
		fhTdiff[iPlane] = new TH2D(strName.Data(), strName.Data(), 50, 0, 50, 4000, -20., 20.);
		fhTdiff[iPlane]->GetXaxis()->SetTitle("Bar ");
		fhTdiff[iPlane]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
		
		strName = Form("Time_Sync_Plane_%d", iPlane+1);
		fhTsync[iPlane] = new TH2D(strName.Data(), strName.Data(), 45, 0, 45, 4500, -1500, 3000.);
		fhTsync[iPlane]->GetXaxis()->SetTitle("Bar ");
		fhTsync[iPlane]->GetYaxis()->SetTitle("THit in ns");
		
		for (int index = 0; index < 2; index++)
		{	
			strName = Form("Tofd_ToT_plane_%d_%d", iPlane+1, index);
			fh_tofd_TotPm[iPlane][index] = new TH2D(strName.Data(), strName.Data(), 90, -45, 45, 500, 0., 500.);
			fh_tofd_TotPm[iPlane][index]->GetXaxis()->SetTitle("Bar ");
			fh_tofd_TotPm[iPlane][index]->GetYaxis()->SetTitle("ToT / ns");
		}	

		for (int iBar=0; iBar < fPaddlesPerPlane; iBar++)
		{
			strName = Form("SqrtQ_vs_PosToT_Plane_%d_Bar_%d", iPlane+1, iBar+1);
			if (fParameter < 3) fhSqrtQvsPosToT[iPlane][iBar] = new TH2D(strName.Data(),strName.Data(),2000,-2,2,150,50,200);
			else  fhSqrtQvsPosToT[iPlane][iBar] = new TH2D(strName.Data(),strName.Data(),12000,-60,60,150,50,200);
			fhSqrtQvsPosToT[iPlane][iBar]->GetYaxis()->SetTitle("sqrt(PM1*PM2)");
			fhSqrtQvsPosToT[iPlane][iBar]->GetXaxis()->SetTitle("Position from ToT in cm");
			
			strName = Form("Plane_%d_Bar_%d_LogToT1vsLogToT2", iPlane+1, iBar+1);
			fhLogTot1vsLogTot2[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 400, 2., 6., 400, 2., 6.);
			fhLogTot1vsLogTot2[iPlane][iBar]->GetXaxis()->SetTitle("Log(ToT) of PM2");
			fhLogTot1vsLogTot2[iPlane][iBar]->GetYaxis()->SetTitle("Log(ToT) of PM1");

			if (fParameter > 3)
			{	
				strName = Form("Q_vs_Pos_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhQvsPos[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 400, -100, 100, fTofdQ*2.5*100, 0., fTofdQ*2.5);
				fhQvsPos[iPlane][iBar]->GetYaxis()->SetTitle("Charge");
				fhQvsPos[iPlane][iBar]->GetXaxis()->SetTitle("Position in cm");
			
				strName = Form("QX_vs_Pos_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhQXvsPos[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 400, -100, 100, fTofdQ*2.5*100, 0., fTofdQ*2.5);
				fhQXvsPos[iPlane][iBar]->GetYaxis()->SetTitle("Charge");
				fhQXvsPos[iPlane][iBar]->GetXaxis()->SetTitle("Position in cm");
				
				strName = Form("Q_vs_Pos1D_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhQvsPos1D[iPlane][iBar] = new TH1D(strName.Data(), strName.Data(), fTofdQ*2.5*100, 0., fTofdQ*2.5);
				fhQvsPos1D[iPlane][iBar]->GetYaxis()->SetTitle("Charge");
				fhQvsPos1D[iPlane][iBar]->GetXaxis()->SetTitle("Position in cm");
			
				strName = Form("QX_vs_Pos1D_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhQXvsPos1D[iPlane][iBar] = new TH1D(strName.Data(), strName.Data(), fTofdQ*2.5*100, 0., fTofdQ*2.5);
				fhQXvsPos1D[iPlane][iBar]->GetYaxis()->SetTitle("Charge");
				fhQXvsPos1D[iPlane][iBar]->GetXaxis()->SetTitle("Position in cm");
			}

			if (fParameter == 1 && fWalk)
			{
				strName = Form("PMTtime_vs_PMTToT_t_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fh1_walk_t[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 400., 0., 200.,4000, -2000, 2000);
				fh1_walk_t[iPlane][iBar]->GetYaxis()->SetTitle("PMT time ns");
				fh1_walk_t[iPlane][iBar]->GetXaxis()->SetTitle("ToT of PMT");

				strName = Form("PMTtime_vs_PMTToT_b_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fh1_walk_b[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 400., 0., 200.,4000, -2000, 2000);
				fh1_walk_b[iPlane][iBar]->GetYaxis()->SetTitle("PMT time ns");
				fh1_walk_b[iPlane][iBar]->GetXaxis()->SetTitle("ToT of PMT");
			}

			if (std::abs(fParameter) == 3)
			{
				strName = Form("Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhTot1vsPos[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 200, -100, 100, 300, 0., 300.);
				fhTot1vsPos[iPlane][iBar]->GetXaxis()->SetTitle("Pos in cm");
				fhTot1vsPos[iPlane][iBar]->GetYaxis()->SetTitle("ToT of PM1 in ns");

				strName = Form("Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane+1, iBar+1);
				fhTot2vsPos[iPlane][iBar] = new TH2D(strName.Data(), strName.Data(), 200, -100, 100, 300, 0., 300.);
				fhTot2vsPos[iPlane][iBar]->GetXaxis()->SetTitle("Pos in cm");
				fhTot2vsPos[iPlane][iBar]->GetYaxis()->SetTitle("ToT of PM2 in ns");
			}
		}
		printf("I'm inside CreateHistograms: Plane %d\n",iPlane+1);	
	}
}

void R3BTofDCal2HitPar_elorenz::calcOffset()
{
	TCanvas* cOffset = new TCanvas("cOffset", "cOffset", 10, 10, 1000, 900);
	cOffset->Divide(2, 2);
	R3BTofDHitModulePar* mpar;
	for (int i = 0; i < fNofPlanes; i++)
	{
		if (fhTdiff[i]->GetEntries() > fMinStats)
		{
			LOG(warning) << "Found histo Time_Diff_Plane_" << i + 1;
			for (int j = 0; j < fPaddlesPerPlane; j++)
			{
				int s=0;
				mpar = new R3BTofDHitModulePar();
				double offset = 0.;
				cOffset->cd(i + 1);
				fhTdiff[i]->Draw("colz");
				TH1D* histo_py = (TH1D*)fhTdiff[i]->ProjectionY("histo_py", j + 2, j + 2, "");
				histo_py->Rebin(5);
				int binmax = histo_py->GetMaximumBin();
				double Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.3, Max + 0.3);
				histo_py->Fit("fgaus", "QR0");
				offset = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
				LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Offset  " << offset;
				mpar->SetPlane(i + 1);
				mpar->SetPaddle(j + 1);
				mpar->SetOffset1(-offset / 2.);
				mpar->SetOffset2(offset / 2.);
				fHitPar->AddModulePar(mpar);
				//cin>>s;
			}
			fhTdiff[i]->Write();
		}
	}
	fHitPar->setChanged();
}
void R3BTofDCal2HitPar_elorenz::calcToTOffset(double totLow, double totHigh)
{
	TCanvas* cToTOffset1 = new TCanvas("cToTOffset1", "cToTOffset1", 10, 10, 1000, 900);
	cToTOffset1->Divide(1, 2);
	TCanvas* cToTOffset2 = new TCanvas("cToTOffset2", "cToTOffset2", 10, 10, 1000, 900);
	cToTOffset2->Divide(1, 2);
	double fitrange = 25.;
	if(fParameter == -1) fitrange = 10.;
	for (int iplane = 0; iplane < fNofPlanes; iplane++)
	{	
		R3BTofDHitModulePar* par;
		for (int ibar = 0; ibar < (fPaddlesPerPlane); ibar++)
		{	
			double offset = 0.;	

			if(ibar<44) par = fHitPar->GetModuleParAt(iplane + 1, 1 + ibar);
			else par = fHitPar->GetModuleParAt(iplane + 1, ibar - fPaddlesPerPlane - 2);
			if (fhSqrtQvsPosToT[iplane][ibar]->GetEntries() > fMinStats)
			{
				LOG(info) << "Found histo SqrtQ_vs_PosToT_Plane_" << iplane + 1 << "_Bar_" << ibar + 1;
				cToTOffset1->cd(1);
				fhSqrtQvsPosToT[iplane][ibar]->Draw("colz");
			
				auto histo_py = (TH2D*)fhSqrtQvsPosToT[iplane][ibar]->ProjectionX("histo_py", totLow, totHigh, "");	
				cToTOffset1->cd(2);
				histo_py->Draw();
				int binmax = histo_py->GetMaximumBin();
				double Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.2, Max + 0.2); 
				histo_py->Fit("fgaus", "QR0");
				offset = fgaus->GetParameter(1);
				fgaus->Draw("SAME");

				histo_py->SetAxisRange(Max - .5, Max + .5, "X");
				fhSqrtQvsPosToT[iplane][ibar]->SetAxisRange(Max - .5, Max + .5, "X");
				fhSqrtQvsPosToT[iplane][ibar]->SetAxisRange(totLow, totHigh, "Y");
				cToTOffset1->Update();
				delete fgaus;
				fhSqrtQvsPosToT[iplane][ibar]->Write();
			}
			LOG(warning) << " Plane  " << iplane + 1 << " Bar " << ibar + 1 << " ToT Offset  " << offset << "\n";
			LOG(info) <<"TOP " << 1. / sqrt(exp(offset)) << " BOT " <<sqrt(exp(offset))<< "\n";
			par->SetToTOffset1(sqrt(exp(offset)));
			par->SetToTOffset2(1. / sqrt(exp(offset)));
		}
	}
	fHitPar->setChanged();
}

void R3BTofDCal2HitPar_elorenz::calcSync()
{
	TCanvas* cSync = new TCanvas("cSync", "cSync", 10, 10, 1000, 900);
	cSync->Divide(2, 2);
	for (int iplane = 0; iplane < fNofPlanes; iplane++)
	{
		if (fhTsync[iplane]->GetEntries() > fMinStats)
		{
			LOG(info) << "Found histo Time_Sync_Plane_" << iplane + 1;
			for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
			{
				cSync->cd(iplane + 1);
				fhTsync[iplane]->Draw("colz");
				auto* histo_py = (TH1D*)fhTsync[iplane]->ProjectionY("histo_py", ibar + 2, ibar + 2, "");
				R3BTofDHitModulePar* par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);
				int binmax = histo_py->GetMaximumBin();
				double Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				double MaxEntry = histo_py->GetBinContent(binmax);
				TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 10., Max + 10.);
				fgaus->SetParameters(MaxEntry, Max, 20);
				histo_py->Fit("fgaus", "QR0");
				double sync = fgaus->GetParameter(1); 
				par->SetSync(sync);
				LOG(info) << " Plane  " << iplane + 1 << " Bar " << ibar + 1 << " Sync  " << sync;
			}
			fhTsync[iplane]->Write();
		}
	}
	fHitPar->setChanged();
}

void R3BTofDCal2HitPar_elorenz::calcVeff()
{
	TCanvas* cVeff = new TCanvas("cVeff", "cVeff", 10, 10, 1000, 900);
	cVeff->Divide(2, 2);
	for (int i = 0; i < fNofPlanes; i++)
	{
		for (int j = 0; j < fPaddlesPerPlane; j++)
		{
			double max = 0.;
			double max1 = 0.;
			double veff = 7.;
			double veff1 = 7.;
			if (fhTdiff[i]->GetEntries() > fMinStats)
			{
				LOG(info) << "Found histo Time_Diff_Plane_" << i + 1;
				auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
				if (!par)
				{
					LOG(warning) << "Hit par not found, Plane: " << i + 1 << ", Bar: " << j + 1;
					continue;
				}
				cVeff->cd(i + 1);
				TH1D* histo_py = (TH1D*)fhTdiff[i]->ProjectionY("histo_py", j + 2, j + 2, "");
				int binmax = histo_py->GetMaximumBin();
				max = histo_py->GetXaxis()->GetBinCenter(binmax);
				double maxEntry = histo_py->GetBinContent(binmax);
				auto* fgaus = new TF1("fgaus", "gaus(0)", max - 0.3, max + 0.3); /// TODO: find best range
				fgaus->SetParameters(maxEntry, max, 20);
				histo_py->Fit("fgaus", "QR0");
				double offset1 = par->GetOffset1();
				double offset2 = par->GetOffset2();
				printf("Offset1: %lf\n Offset2:%lf\n", offset1, offset2);
				max = fgaus->GetParameter(1); 
				veff = fTofdY / max; // effective speed of light in [cm/ns] 
				LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " offset  " << par->GetOffset1();
				LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " max  " << max;
				LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " veff  " << veff;
				par->SetVeff(veff);
			}
		}
	}
	fHitPar->setChanged();
}

void R3BTofDCal2HitPar_elorenz::calcLambda(double totLow, double totHigh)
{
	TCanvas* cToTOffset = new TCanvas("cLambda", "cLambda", 10, 10, 1000, 900);
	cToTOffset->Divide(1, 2);
	for (int iplane = 0; iplane < fNofPlanes; iplane++)
	{
		for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
		{
			double offset = 0.;
			auto par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);

			if (fhSqrtQvsPosToT[iplane][ibar]->GetEntries() > fMinStats)
			{
				LOG(info) << "Found histo SqrtQ_vs_PosToT_Plane_" << iplane + 1 << "_Bar_" << ibar + 1;
				cToTOffset->cd(1);
				fhSqrtQvsPosToT[iplane][ibar]->Draw("colz");
				auto* histo_py = (TH2D*)fhSqrtQvsPosToT[iplane][ibar]->ProjectionX("histo_py", totLow, totHigh, "");
				
				cToTOffset->cd(2);
				histo_py->Draw();
				
				int binmax = histo_py->GetMaximumBin();
				double Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.6, Max + 0.6);
				histo_py->Fit("fgaus", "QR0");
				offset = fgaus->GetParameter(1);
				
				fgaus->Draw("SAME");
				histo_py->SetAxisRange(Max - .5, Max + .5, "X");
				fhSqrtQvsPosToT[iplane][ibar]->SetAxisRange(Max - .5, Max + .5, "X");
				fhSqrtQvsPosToT[iplane][ibar]->SetAxisRange(totLow, totHigh, "Y");
				cToTOffset->Update();
				delete fgaus; delete histo_py;
			}
			else {LOG(error) << "Missing histo plane " << iplane + 1 << " bar " << ibar + 1;}
			double lambda;
		        lambda = fTofdY / offset;
			LOG(info)<<" Plane  "<<iplane + 1<<" Bar "<<ibar + 1<<" ToT Offset  "<<offset<<" Lambda "<<lambda<< "\n";
			par->SetLambda(lambda);
		}
	}
	fHitPar->setChanged();
}

// This fits the smiley: Sqrt(q1*q2) returns position dependent charge, fit twice w/ pol3
void R3BTofDCal2HitPar_elorenz::smiley(TH2D* histo, double min, double max, double* para)
{
	cout<<"Enter smiley function"<<endl;
	int nn = 0;
	double fitrange = 5.;
Redo:	
	double *yy = (double*)malloc(1000);
	double *xx = (double*)malloc(1000);

	for (int j = 0; j <= 3; j++) {para[j] = 0;}

	TGraph* gr1 = new TGraph();
	TGraph* gr2 = new TGraph();
	TCanvas* cfit_smiley = new TCanvas("cfit_smiley", "fit smiley", 150, 150, 1200, 1200);
	cfit_smiley->Clear();
	cfit_smiley->Divide(1, 4);
	
	cfit_smiley->cd(1);
	TH2D* histo1 = (TH2D*)histo->Clone();
	histo1->Draw("colz");
	
	TH2D* histo2 = (TH2D*)histo->Clone();
	TH2D* histo3 = (TH2D*)histo->Clone();
	histo2->RebinX(50);
	histo2->RebinX(25);
	histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
	histo3->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
	cfit_smiley->cd(2);
	histo2->Draw("colz");
	
	std::cout << "Searching for points to fit...\n";
	for (int ibins = 1; ibins < histo2->GetNbinsX(); ibins++)
	{
		std::cout<<"Bin "<<ibins<<" of "<<histo2->GetNbinsX()<<" with cut: "<<fTofdTotLow<<" < sqrt(q1*q2) <"<<fTofdTotHigh<<"\n";
		cfit_smiley->cd(2);
		//TLine* line = new TLine(histo2->GetXaxis()->GetBinCenter(ibins), fTofdTotLow, histo2->GetXaxis()->GetBinCenter(ibins), fTofdTotHigh);
		//line->SetLineColor(kRed); line->SetLineWidth(2.); line->Draw();
		
		cfit_smiley->cd(3);
		TH1D* histo_py = (TH1D*)histo2->ProjectionY("histo_py", ibins, ibins, "");
		if (histo_py->GetEntries() < fMinEntries) continue;
		else if (histo_py->GetEntries() < 2500) histo_py->Rebin(5);
		histo_py->Draw();
		//cfit_smiley->Update();

		xx[nn] = histo2->GetXaxis()->GetBinCenter(ibins);
		double y_check=histo_py->GetXaxis()->GetBinCenter(histo_py->GetMaximumBin());
		
		if ((xx[nn]<min||xx[nn]>max)||(y_check<fTofdTotLow||y_check>fTofdTotHigh)) {delete histo_py; continue;}

		TF1* fgaus = new TF1("fgausX", "gaus(0)", y_check - fitrange, y_check + fitrange);
		fgaus->SetParameter(1, y_check);
		fgaus->SetParLimits(1, y_check - (fitrange*1.25), y_check + (fitrange*1.25));
		histo_py->Fit(fgaus, "QR");
		yy[nn] = fgaus->GetParameter(1);
		fgaus->Draw();
		cout<<"y "<<y_check<<" y_new "<<yy[nn]<<endl;
		nn++;
		/*delete line;*/ delete fgaus; delete histo_py;
	}
	gr1 = new TGraph(nn, xx, yy);
	gr1->SetTitle("Points found for fitting; x position in cm; sqrt(tot1*tot2)");
	gr1->Draw("A*");
	
	std::cout << "Start fitting\n";
	TF1* f1 = new TF1("f1", "pol3", min, max);
	f1->SetLineColor(2);
	gr1->Fit("f1", "Q", "", min, max);
	for (int j = 0; j <= 3; j++)
	{
		para[j] = f1->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	
	// fit again but with more information and better cuts
	std::cout << "Fit again with more information\n";
	nn = 0;
	cfit_smiley->cd(4);
	for (int ibin = 1; ibin < histo2->GetNbinsX(); ibin++)
	{
		double pos = histo2->GetXaxis()->GetBinCenter(ibin);
		double ymean = f1->Eval(pos);
		histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
		histo2->Draw("colz");

		TH1D* histo_py = (TH1D*)histo2->ProjectionY("histo_py", ibin, ibin, "");
		histo_py->Draw();
		if (histo_py->GetEntries() < fMinEntries) continue;
		if (histo_py->GetEntries() < 2500) histo_py->Rebin(5);
		xx[nn] = histo2->GetXaxis()->GetBinCenter(ibin);
		double y_check = histo_py->GetXaxis()->GetBinCenter(histo_py->GetMaximumBin());

		if ((xx[nn]<min||xx[nn]>max)||(y_check<fTofdTotLow||y_check>fTofdTotHigh)) {delete histo_py; continue;}
		
		TF1* fgaus2 = new TF1("fgaus2", "gaus(0)", y_check - (fitrange), y_check + (fitrange));
		fgaus2->SetParameter(1, y_check);
		fgaus2->SetParLimits(1,y_check - (fitrange*1.25),y_check + (fitrange*1.25));
		histo_py->Fit(fgaus2, "QR");
		yy[nn] = fgaus2->GetParameter(1);
		cout<<"fine y "<<y_check<<" y_new "<<yy[nn]<<endl;
		fgaus2->Draw();
		nn++;
		delete histo_py;
	}
	gr2 = new TGraph(nn, xx, yy);
	gr2->SetTitle("More information;x position in cm;sqrt(q1*q2)");
	gr2->Draw("A*");
	f1->DrawCopy("SAME");

	TF1* f2 = new TF1("f2", "pol3", min, max);
	f2->SetParameters(para[0], para[1], para[2], para[3]);
	f2->SetLineColor(3);
	gr2->Fit("f2", "0Q", "", min, max);
	f2->Draw("SAME");
	std::cout << "Will write:\n";
	for (int jj = 0; jj <= 3; jj++)
	{
		para[jj] = f2->GetParameter(jj);
		std::cout << "Parameter"<<jj<<": " << para[jj] << "\n";
	}
	
	histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
	auto legend = new TLegend(.9, 0.7, .99, 0.9);
	legend->AddEntry("f1", "First Fit", "l");
	legend->AddEntry("f2", "Second Fit", "l");
	legend->Draw();
	cfit_smiley->Update();
	
	TString cannom = histo->GetTitle();
	cfit_smiley->SaveAs(Form("%s.png",cannom.Data()));
	gPad->WaitPrimitive();
	gSystem->Sleep(10);
	char cCheck = 'y';
	while(!(cCheck == 'y' || cCheck == 'n')){
		cout<< "Continue? (y/n) ";
		cin >> cCheck;
	}
	cout<<"1"<<endl;
	delete histo1; delete histo2; delete gr1; delete gr2; delete f1; delete f2; delete cfit_smiley;
	free(xx);
	free(yy);
	nn=0;
	if(cCheck == 'n') {goto Redo;}
	cout<<"2"<<endl;
}

void R3BTofDCal2HitPar_elorenz::doubleExp(TH2D* histo, double min, double max, double* para)
{
	// This fits the exponential decay of the light in a paddle. The 2 PMTs are fit with the same function but one
	// side will deliver negative attenuation parameters and the other positive.
	double yy[1000], xx[1000];
	int nn = 0;
	for (int j = 0; j <= 3; j++) {para[j] = 0;}
	TGraph* gr1 = new TGraph();
	TGraph* gr2 = new TGraph();
	TCanvas* cfit_exp = new TCanvas("cfit_exp", "fit exponential", 100, 100, 800, 800);
	cfit_exp->Clear();
	cfit_exp->Divide(1, 3);
	cfit_exp->cd(1);
	TH2D* histo1 = (TH2D*)histo->Clone();
	TH2D* histo2 = (TH2D*)histo->Clone();
	histo1->Draw("colz");
	cfit_exp->cd(2);
	for (int ii = 1; ii < histo1->GetNbinsX() - 1; ii++)
	{
		TH1D* histo_py = (TH1D*)histo1->ProjectionY("histo_py", ii, ii, "");
		histo_py->Draw();
		xx[nn] = histo1->GetXaxis()->GetBinCenter(ii);
		int binmax = histo_py->GetMaximumBin();
		yy[nn] = histo_py->GetXaxis()->GetBinCenter(binmax);
		if (std::abs(xx[nn] > 40.) || yy[nn] < 50.)
		{
			delete histo_py;
			continue;
		}
		if (histo_py->GetMaximum() > 5) nn++;
		delete histo_py;
	}
	gr1 = new TGraph(nn, xx, yy);
	gr1->Draw("A*");
	TF1* f1 = new TF1("f1", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
	//f1->SetParameters(520., 0.001, 17234, -485.);
	f1->SetLineColor(2);
	gr1->Fit("f1", "", "", min, max);
	for (int j = 0; j <= 3; j++)
	{
		para[j] = f1->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	// fit again but with more information and better cuts
	nn = 0;
	cfit_exp->cd(3);
	for (int ii = 1; ii < histo2->GetNbinsX(); ii++)
	{
		double pos = histo2->GetXaxis()->GetBinCenter(ii);
		double ymean = para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3];
		histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
		histo2->Draw("colz");
		TH1D* histo_py = (TH1D*)histo2->ProjectionY("histo_py", ii, ii, "");
		histo_py->Draw();
		xx[nn] = histo2->GetXaxis()->GetBinCenter(ii);
		int binmax = histo_py->GetMaximumBin();
		yy[nn] = histo_py->GetXaxis()->GetBinCenter(binmax);
		if (histo_py->GetMaximum() > 2) nn++;
		delete histo_py;
	}
	gr2 = new TGraph(nn, xx, yy);
	gr2->Draw("A*");
	TF1* f2 = new TF1("f2", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
	f2->SetParameters(para[0], para[1], para[2], para[3]);
	f2->SetLineColor(2);
	gr2->Fit("f2", "", "", min, max);
	for (int j = 0; j <= 3; j++)
	{
		para[j] = f2->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	cfit_exp->Update();
	gPad->WaitPrimitive();
	gSystem->Sleep(3000);
	delete gr1; delete gr2; delete f1; delete f2;
}

int R3BTofDCal2HitPar_elorenz::zcorr(TH2D* histo, double min, double max, double* pars, int pl, int b, int index)
{
	if (histo->GetEntries() < 10)
	{
		R3BLOG(warning, "Nb of events below 10 with "<< histo->GetEntries() << " entries for histo with index" << index);
	}

	double par[3000] = { 0 };
	double xx[3000] = { 0 };
	TString strName = Form("canvas_%d", index);
	TCanvas* c1 = new TCanvas(strName.Data(), "", 100, 100, 800, 800);
	c1->Divide(1, 3);
	c1->cd(1);
	auto* h = (TH2D*)histo->Clone();
	h->Draw("colz");
	h->RebinY(10);
	h->GetXaxis()->SetRangeUser(-40, 40);
	h->GetYaxis()->SetRangeUser(min, max);
	
	//Projection of charge axis
	auto* h1 = h->ProjectionY("p_y", 1, 2000);
	c1->cd(2);
	h1->Draw();

	//Use TSpectrum to find the peak candidates
	double threshhold = 0.12;
	int sigma = 1;
	double sqrtf = 1.;
	int nPeaks = fNbZPeaks;
dofitagain:
	TSpectrum* spec = new TSpectrum(nPeaks);
	int nfound = spec->Search(h1, sigma, "nobackground", threshhold);
	std::cout << "Found " << nfound << " candidate peaks to fit\n";

	if (nfound == 0)
	{
		delete spec; delete c1; delete h; delete h1;
		return -1;
	}

	c1->Update();
	// Eliminate background peaks
	int mPeaks = 0;
	double* xpeaks = spec->GetPositionX();
	for (int ipeaks = 0; ipeaks <= nfound; ipeaks++)
	{
		Float_t xp = xpeaks[ipeaks];
		int bin = h1->GetXaxis()->FindBin(xp);
		Float_t yp = h1->GetBinContent(bin);
		if (yp - sqrtf * TMath::Sqrt(yp) < 1.){
			cout<<"jump "<<yp - sqrtf * TMath::Sqrt(yp)<<endl;
			continue;
		}
		par[2 * mPeaks] = yp;
		par[2 * mPeaks + 1] = xp;
		mPeaks++;
	}

	double peaks[mPeaks];
	for (int ii = 0; ii < mPeaks; ii++)
	{
		// printf("Found peak @ %f\n",xpeaks[i]);
		peaks[ii] = par[2 * ii + 1];
	}
	c1->Update();

	if (mPeaks < 2 && 0)
	{
		pars[0] = 0.;
		//pars[1] = fMaxQ / peaks[0];
		pars[1] = 8. / peaks[0];
		pars[2] = 0.;

		delete spec; delete c1; delete h; 	delete h1;
		return -1;
	}
	cout<<"mPeaks "<<mPeaks<<endl;
	// select useful peaks
	sort(peaks, peaks + mPeaks);
	double zpeaks[3000] = { 0 };
	string doagain = "y";
	int nfp;
	//dofitagain:
	int z = 0;
	do
	{
		string peakscheck = "x";
		std::cout << "Peaks ok? (y/n/c/goback) ";
		std::cin >> peakscheck;
		if(peakscheck == "n"){
			delete spec;
			std::cout << "New sigma: ";
			std::cin >> sigma;
			std::cout << "New threshhold: ";
			std::cin >> threshhold;
			std::cout << "New Sqrt factor: ";
			std::cin >> sqrtf;
			std::cout << "New # of peaks: ";
			std::cin >> nPeaks;
			goto dofitagain;
		}
		if(peakscheck == "goback"){
			int Z_corr=0;
			std::cout << "How many bars? ";
			std::cin >> Z_corr;
			return Z_corr;
		}
		while(peakscheck == 'c'){
			cout<< "We see "<<mPeaks<<" peaks. Continue? (y/n) ";
			cin >> peakscheck;
		}

		nfp = 0;
		for (int i = 0; i < mPeaks; i++)
		{
			std::cout << "Peak @ " << peaks[i];
			while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
			{
				std::cout << "That's not a number;";
				std::cin.clear();
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
			if (z == 0)
				continue;
			xx[nfp] = (double)z;
			zpeaks[nfp] = peaks[i];
			// std::cout<<"z real " << xx[nfp] << " z found " << zpeaks[nfp] <<"\n";
			nfp++;
		}
		doagain = 'n';
		//		    std::cout << "Do again? (y/n) ";
		//		    std::cin >> doagain;
	} while (doagain != "n");

	if (mPeaks < 2)
	{
		pars[0] = 0.;
		pars[1] = z / peaks[0];
		pars[2] = 0.;

		delete spec; delete c1; delete h; delete h1;
		return -1;
	}

	// fit charge axis
	std::cout << "Selected " << nfp << " useful peaks to fit\nStart fitting...\n";
	c1->cd(3)->Clear();
	c1->Update();
	c1->cd(3);
	auto gr1 = new TGraph(nfp, zpeaks, xx);
	gr1->SetMarkerColor(4);
	gr1->SetMarkerSize(1.5);
	gr1->SetMarkerStyle(20);
	gr1->Draw("AP");

	// TF1* fitz = new TF1("fitz", "[0]*TMath::Power(x,[2])+[1]", min, max);
	if (fZfitType != "pol1" && fZfitType != "pol2")
	{
		R3BLOG(error, "Fit " << fZfitType << " is not allowed, use pol1 or pol2 ");
		return -1;
	}
	auto fitz = new TF1("fitz", fZfitType, min, max);
	fitz->SetLineColor(2);
	fitz->SetLineWidth(2);
	fitz->SetLineStyle(1);
	gr1->Fit("fitz", "Q");
	fitz->Draw("lsame");
	c1->Update();
	// write parameters
	std::cout << "Is OK? (y/n) ";
	std::cin >> doagain;
	if (doagain == "n"){
		std::cout << "New sigma: ";
		std::cin >> sigma;
		std::cout << "New threshhold: ";
		std::cin >> threshhold;
		delete spec;
		goto dofitagain;
	}

	int nbpars = 2;
	if (fZfitType == "pol2")
		nbpars = 3;

	for (int j = 0; j < nbpars; j++)
	{
		pars[j] = fitz->GetParameter(j);
		std::cout<<Form("par%i= ",j)<<pars[j]<<"\n";
	}
	c1->Write();
	delete spec; delete h; delete h1; delete gr1; delete c1; delete fitz;

	return -1;
}

void R3BTofDCal2HitPar_elorenz::FinishTask()
{	
	if (fParameter == 1)
	{
		// Determine time offset of the 2 PMTs of one paddle. This procedure
		// assumes a sweep run in the middle of the ToF wall horizontally.
		// Since all paddles are mounted vertically one can determine the offset.
		// Half of the offset is added to PM1 and half to PM2.
		LOG(info) << "Calling function calcOffset";
		calcOffset();
		// Determine ToT offset between top and bottom PMT
		LOG(info) << "Calling function calcToTOffset";
		calcToTOffset(fTofdTotLow, fTofdTotHigh);
		// Determine sync offset between paddles
		LOG(info) << "Calling function calcSync";
		calcSync();
		LOG(error) << "Call walk correction before next step!";
	}
	else if (fParameter == 2)
	{
		// Determine effective speed of light in [cm/s] for each paddle
		LOG(info) << "Calling function calcVeff";
		calcVeff();
		// Determine light attenuation lambda for each paddle
		LOG(info) << "Calling function calcLambda";
		calcLambda(fTofdTotLow, fTofdTotHigh);

		for(int iplane = 0; iplane < fNofPlanes; iplane++)
		{
			fhTdiff[iplane]->Write();
			for (int jj = 0; jj < 2; jj++) fh_tofd_TotPm[iplane][jj]->Write();
			for(int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
			{
				fhSqrtQvsPosToT[iplane][ibar]->Write();
			}
		}
	}
	else if (fParameter == 3)
	{
		// calculation of position dependend charge
		for(int iplane=0; iplane<fNofPlanes; iplane++){
			for(int ibar=0; ibar<fPaddlesPerPlane; ibar++){
				fhSqrtQvsPosToT[iplane][ibar]->Write();
				fhTot2vsPos[iplane][ibar]->Write();
				fhTot1vsPos[iplane][ibar]->Write();
			}
		}
	}

	//The point of this step is to 
	if(fParameter == -3)
	{
		if (fTofdSmiley)
		{
			LOG(info) << "Calling function smiley";
			//TFile* histofilename = TFile::Open(fHistoFile);
			printf("Inputfile: %s\n", fHistoFile.Data());
			TFile* histofilename = new TFile(fHistoFile.Data());
			double para2[4]{};
			double min2 = -40.; // -40 effective bar length
			double max2 = 40.;  // 40 effective bar length = 80 cm
			// we will use 50 here for some fit safety margin
			for (int iplane = 0; iplane < fNofPlanes; iplane++)
			{
				for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
				{
					TString histnom = Form("SqrtQ_vs_PosToT_Plane_%d_Bar_%d", iplane+1, ibar+1);
					TH2D *histo = (TH2D*)histofilename->FindObjectAny(histnom.Data());
					if (histo == NULL) continue;
					if (histo->GetEntries() > fMinStats)
					{
						for (int index = 0; index < 4; index++) para2[index] = 0.;
						LOG(info) << "Calling Plane " << iplane + 1 << " Bar " << ibar + 1;
						auto par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);
						smiley(histo, min2, max2, para2);

						cout<<"Plane "<<iplane+1<<" Paddle "<<ibar+1<<" ";
						for(int k = 0; k<4; k++){cout<<"para"<<k<<" "<<para2[k]<<endl;}
						cout<<"3"<<endl;

						if(para2[0]== 0. && para2[1]== 0. && para2[2]== 0. && para2[3]== 0.){
							para2[0] = 1.;
							cout<<"Not enough statistics for smiley correction. Set par0 = 1, rest 0"<<endl;
						}
						par->SetPola(para2[0]);
						par->SetPolb(para2[1]);
						par->SetPolc(para2[2]);
						par->SetPold(para2[3]);
						cout<<"4"<<endl;
					}
				}
			}
			fHitPar->setChanged();
		}
		else
		{
			LOG(info) << "Calling function doubleExp";
			printf("Inputfile: %s\n", fHistoFile.Data());
			TFile* histofilename = new TFile(fHistoFile.Data());
			double para[4];
			for (int ii = 0; ii < 4; ii++) para[ii] = 0.;
			double min = -40.; // effective bar length
			double max = 40.;  // effective bar length = 80 cm

			for (int iplane = 0; iplane < fNofPlanes; iplane++)
			{
				for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
				{
					TString histnom;
				        histnom	= Form("Tot1_vs_Pos_Plane_%d_Bar_%d", iplane+1, ibar+1);
					TH2D *histo1 = (TH2D*)histofilename->FindObjectAny(histnom.Data());
					histnom = Form("Tot1_vs_Pos_Plane_%d_Bar_%d", iplane+1, ibar+1);
					TH2D *histo2 = (TH2D*)histofilename->FindObjectAny(histnom.Data());
					cout<<"Plane "<<iplane+1<<" Paddle "<<ibar+1<<" Entries " <<histo1->GetEntries()<<endl;
					if (histo1 == NULL || histo2 == NULL) continue;
					if (histo1->GetEntries() > fMinStats)
					{
						auto par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);
						doubleExp(fhTot1vsPos[iplane][ibar], min, max, para);
						par->SetPar1a(para[0]);
						par->SetPar1b(para[1]);
						par->SetPar1c(para[2]);
						par->SetPar1d(para[3]);
						fhTot1vsPos[iplane][ibar]->Write();
					}
					if (histo2->GetEntries() > fMinStats)
					{
						auto par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);
						doubleExp(fhTot2vsPos[iplane][ibar], min, max, para);
						par->SetPar2a(para[0]);
						par->SetPar2b(para[1]);
						par->SetPar2c(para[2]);
						par->SetPar2d(para[3]);
						fhTot2vsPos[iplane][ibar]->Write();
					}
				}
			}
			fHitPar->setChanged();
		}
	}

	if (fParameter == 4)
	{
		//Write Histograms for Charge correction.
		//Call fParameter = 5 to read these Histogramms.
		for (int iplane = 0; iplane < fNofPlanes; iplane++)
		{
			for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
			{
				if  (fhQvsPos[iplane][ibar]->GetEntries() > fMinStats) {fhQvsPos[iplane][ibar]->Write();}
				if (fhQXvsPos[iplane][ibar]->GetEntries() > fMinStats) {fhQXvsPos[iplane][ibar]->Write();}
				if  (fhQvsPos1D[iplane][ibar]->GetEntries() > fMinStats) {fhQvsPos1D[iplane][ibar]->Write();}
				if (fhQXvsPos1D[iplane][ibar]->GetEntries() > fMinStats) {fhQXvsPos1D[iplane][ibar]->Write();}
				printf("Inside writing\n");
			}
		}
	}

	if(fParameter == 5)
	{
		// Z correction for each plane
		LOG(warning) << "Calling function zcorr";
		TFile* histofilename = new TFile(fHistoFile.Data());
		double para[8]{};
		double pars[3]{};
		int min = fMinQ, max = fMaxQ; // select range for peak search
		cout<<"1"<<endl;
		
		for (int iplane = 0; iplane < fNofPlanes; iplane++)
		{
			cout<<"2"<<endl;
			for (int ibar = 0; ibar < fPaddlesPerPlane; ibar++)
			//for (int ibar = fBarMin; ibar < fBarMax; ibar++)
			{
				cout<<"3 "<<ibar<<endl;
				TString histnom = Form("QX_vs_Pos_Plane_%d_Bar_%d", iplane+1, ibar+1);
				TH2D *histo = (TH2D*)histofilename->FindObjectAny(histnom.Data());
				if (histo == NULL) continue;
				if (histo->GetEntries() > fMinStats)
				{
					cout<<"4"<<endl;
					auto par = fHitPar->GetModuleParAt(iplane + 1, ibar + 1);
					std::cout << "Calling Plane: " << iplane + 1 << " Bar " << ibar + 1 << "\n";
					int index = iplane * fPaddlesPerPlane + ibar;
					int Z_corr = zcorr((TH2D*)histofilename->Get(Form("QX_vs_Pos_Plane_%i_Bar_%i", iplane+1, ibar+1)), min, max, pars, iplane, ibar, index);
					std::cout << "Write parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
					cout<<"5"<<endl;

					if(Z_corr > 0) ibar = ibar - Z_corr; //zcorr should return a -1, if it doesn't it's a problem

					par->SetPar1za(pars[0]);
					par->SetPar1zb(pars[1]);
					par->SetPar1zc(pars[2]);
				}
			}
		}
	}
}

double R3BTofDCal2HitPar_elorenz::walk(double QQ, double par1, double par2, double par3, double par4, double par5)
{
	double yy = 0;
	yy = -30.2 + par1 * TMath::Power(QQ, par2) + par3 / QQ + par4 * QQ + par5 * QQ * QQ;
	return yy;
}

double R3BTofDCal2HitPar_elorenz::saturation(double xx)
{
	Double_t kor;
	Int_t voltage = 600;
	if (voltage == 600)
	{
		if (xx < 173) xx = 0;
		else if (xx > 208) xx = 208;

		kor = -1.73665e+03 + 2.82009e+01 * xx - 1.53846e-01 * (xx * xx) + 2.82425e-04 * (xx * xx * xx);
	}
	if (voltage == 500)
	{
		if (xx < 95.5) {kor = 0.;}
		else if (xx > 124) {kor = 1.08 * xx - 112.44;}
		else {kor = 643.257 - 16.7823 * xx + 0.139822 * (xx * xx) - 0.000362154 * (xx * xx * xx);}
	}
	if (voltage == 700)
	{
	    if (xx < 198) {kor = 0.;}
	    else if (xx > 298) {kor = 0.21 * xx - 45.54;}
	    else
	    {
	        kor = -19067 + 383.93 * xx - 3.05794 * (xx * xx) + 0.0120429 * (xx * xx * xx) - 2.34619e-05 * (xx * xx * xx * xx) +
	              1.81076e-08 * (xx * xx * xx * xx * xx);
	    }
	}
	return kor;
}

ClassImp(R3BTofDCal2HitPar_elorenz);
