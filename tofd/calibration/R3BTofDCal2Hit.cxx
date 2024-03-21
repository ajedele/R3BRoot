/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------
// -----                 R3BTofDCal2Hit                   -----
// -----    Created 18/03/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#include "R3BTofDCal2Hit.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTCalEngine.h"
#include "R3BTDCCyclicCorrector.h"
#include "R3BTofDHitModulePar.h"
#include "R3BTofDHitPar.h"
#include "R3BTofDMappingPar.h"
#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include <FairRootManager.h>

#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THnSparse.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom3.h"

#include <iostream>
#include <numeric>
#include <sstream>
#include <string>

using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

namespace
{
	double c_range_ns = 2048 * 5;
	double c_bar_coincidence_ns = 20; // nanoseconds.
} // namespace

R3BTofDCal2Hit::R3BTofDCal2Hit()
	: R3BTofDCal2Hit("TofdCal2Hit", 1)
{
}

R3BTofDCal2Hit::R3BTofDCal2Hit(const char* name, int iVerbose)
	: FairTask(name, iVerbose)
	, fHitItems(new TClonesArray("R3BTofdHitData"))
{
}

R3BTofDCal2Hit::~R3BTofDCal2Hit()
{
	if (fTofdHisto)
	{
		if (fhNoTpat) delete fhNoTpat;
		for (int i = 0; i < fNofPlanes; i++)
		{
			if (fhQ[i])        delete fhQ[i];
			if (fhxy[i])       delete fhxy[i];
			if (fhQvsEvent[i]) delete fhQvsEvent[i];
			if (fhTdiff[i])    delete fhTdiff[i];
			if (fhTsync[i])    delete fhTsync[i];
			if (fhQ0Qt[i])     delete fhQ0Qt[i];
			if (fhTvsQ[i])     delete fhTvsQ[i];
			for (int j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
			{
				if (fhQvsPos[i][j]) delete fhQvsPos[i][j];
			}
		}
	}
	if (fHitItems) {delete fHitItems;}
}

void R3BTofDCal2Hit::SetParContainers()
{
	fMapPar = dynamic_cast<R3BTofDMappingPar*>(FairRuntimeDb::instance()->getContainer("tofdMappingPar"));
	R3BLOG_IF(warn, !fMapPar, "Could not get access to tofdMappingPar container");
	
	fHitPar = dynamic_cast<R3BTofDHitPar*>(FairRuntimeDb::instance()->getContainer("tofdHitPar"));
	if (!fHitPar)
	{
		R3BLOG(error, "Could not get access to tofdHitPar container");
		fNofHitPars = 0;
		return;
	}
}

void R3BTofDCal2Hit::SetParameter()
{
	R3BLOG_IF(info, fMapPar, "Nb of planes " << fMapPar->GetNbPlanes() << " and paddles " << fMapPar->GetNbPaddles());
	if (fHitPar) fNofHitPars = fHitPar->GetNumModulePar();
	else         fNofHitPars = 0;
	R3BLOG(info, "Parameters in the tofdHitPar container: " << fNofHitPars);
	return;
}

InitStatus R3BTofDCal2Hit::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    header = dynamic_cast<R3BEventHeader*>(mgr->GetObject("EventHeader."));
    R3BLOG_IF(fatal, NULL == header, "EventHeader. not found");

    fCalItems = dynamic_cast<TClonesArray*>(mgr->GetObject("TofdCal"));
    R3BLOG_IF(fatal, NULL == fCalItems, "TofdCal not found");

    fCalTriggerItems = dynamic_cast<TClonesArray*>(mgr->GetObject("TofdTriggerCal"));
    R3BLOG_IF(fatal, NULL == fCalTriggerItems, "TofdTriggerCal not found");

    maxevent = mgr->CheckMaxEventNo();

    // request storage of Hit data in output tree
    mgr->Register("TofdHit", "TofdHit data", fHitItems, !fOnline);

    SetParameter();

    if (fTofdHisto)
    {
        for (int i = 1; i <= fNofPlanes; i++)
            for (int j = 1; j <= N_TOFD_HIT_PADDLE_MAX; j++)
                CreateHistograms(i, j);
    }

    // Definition of a time stich object to correlate times coming from different systems
    fCyclicCorrector = new R3BTDCCyclicCorrector();

    return kSUCCESS;
}

InitStatus R3BTofDCal2Hit::ReInit()
{
	SetParContainers();
	SetParameter();
	return kSUCCESS;
}

void R3BTofDCal2Hit::Exec(Option_t* option)
{
    static uint32_t counter = 0;
    ++counter;

    // test for requested trigger (if possible)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    {
        wrongtrigger++;
        return;
    }
    // fTpat = 1-16; fTpat_bit = 0-15
    int fTpat_bit1 = fTpat1 - 1;
    int fTpat_bit2 = fTpat2 - 1;
    int tpatbin;
    int tpatsum = 0;

    if (header && fTpat1 >= 0 && fTpat2 >= 0)
    {
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            // LOG(debug)<<"tpatbin "tpatbin;
            if (tpatbin != 0 && (i < fTpat_bit1 || i > fTpat_bit2))
            {
                wrongtpat++;
                return;
            }
            if (tpatbin != 0)
            {
                //  fhTpat->Fill(i+1);
                LOG(debug) << "Accepted Tpat: " << i + 1;
            }
            tpatsum += tpatbin;
        }
        if (tpatsum < 1)
        {
            if (fhNoTpat)
                fhNoTpat->Fill(counter - lasttpatevent);

            lasttpatevent = counter;
            notpat++;
            // LOG(fatal)<<"No Tpat info";
            return;
        }
    }

    headertpat++;
    double timeP0 = 0.;
    double randx;

    Uint vmultihits[N_PLANE_MAX + 1][N_TOFD_HIT_PADDLE_MAX + 1];
    for (int i = 0; i <= fNofPlanes; i++)
    {
        for (int j = 0; j <= N_TOFD_HIT_PADDLE_MAX; j++)
        {
            vmultihits[i][j] = 0;
        }
    }
    struct hit
    {
        double charge;
        double time;
        double xpos;
        double ypos;
        int plane;
        int bar;
        double time_raw;
        double tof;
    };

    //    std::cout<<"new event!*************************************\n";

    std::vector<hit> event;

    int nHits = fCalItems->GetEntriesFast();
    LOG(debug) << "Leading and trailing edges in this event: " << nHits;
    if (nHits == 0)
        events_wo_tofd_hits++;

    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofdCalData*> top;
        std::vector<R3BTofdCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    // puts("Event");
    for (int ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = dynamic_cast<R3BTofdCalData*>(fCalItems->At(ihit));
        size_t idx = (hit->GetDetectorId() - 1) * fPaddlesPerPlane + (hit->GetBarId() - 1);

        // std::cout << "Hits: " << hit->GetDetectorId() << ' ' << hit->GetBarId() << ' ' << hit->GetSideId() << '  '
        //          << hit->GetTimeLeading_ns() << ' ' << hit->GetTimeTrailing_ns() << ' '
        //          << hit->GetTimeTrailing_ns() - hit->GetTimeLeading_ns() << '\n';

        auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
        auto& vec = 1 == hit->GetSideId() ? ret.first->second.bot : ret.first->second.top;
        vec.push_back(hit);
        events_in_cal_level++;
    }

    // Build trigger map.
    std::vector<R3BTofdCalData const*> trig_map;
    for (int i = 0; i < fCalTriggerItems->GetEntriesFast(); ++i)
    {
        auto trig = dynamic_cast<R3BTofdCalData const*>(fCalTriggerItems->At(i));
        if (trig_map.size() < trig->GetBarId())
        {
            trig_map.resize(trig->GetBarId());
        }
        trig_map.at(trig->GetBarId() - 1) = trig;
    }

    bool s_was_trig_missing = false;
    // Find coincident PMT hits.
    // std::cout << "Print:\n";
    for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
    {
        //    reset:
        // for (auto it2 = it->second.top.begin(); it->second.top.end() != it2; ++it2) {
        // std::cout << "Top: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        // }
        // for (auto it2 = it->second.bot.begin(); it->second.bot.end() != it2; ++it2) {
        // std::cout << "Bot: " << (*it2)->GetDetectorId() << ' ' << (*it2)->GetBarId() << ' ' <<
        // (*it2)->GetTimeLeading_ns() << '\n';
        // }
        auto const& top_vec = it->second.top;
        auto const& bot_vec = it->second.bot;
        size_t top_i = 0;
        size_t bot_i = 0;
        for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
        {
            auto top = top_vec.at(top_i);
            auto bot = bot_vec.at(bot_i);

            int top_trig_i = 0;
            int bot_trig_i = 0;
            if (fMapPar)
            {
                top_trig_i = fMapPar->GetTrigMap(top->GetDetectorId(), top->GetBarId(), top->GetSideId());
                bot_trig_i = fMapPar->GetTrigMap(bot->GetDetectorId(), bot->GetBarId(), bot->GetSideId());
            }

            double top_trig_ns = 0, bot_trig_ns = 0;
            if (top_trig_i < trig_map.size() && trig_map.at(top_trig_i) && bot_trig_i < trig_map.size() &&
                trig_map.at(bot_trig_i))
            {
                auto top_trig = trig_map.at(top_trig_i);
                auto bot_trig = trig_map.at(bot_trig_i);
                top_trig_ns = top_trig->GetTimeLeading_ns();
                bot_trig_ns = bot_trig->GetTimeLeading_ns();

                ++n1;
            }
            else
            {
                if (!s_was_trig_missing)
                {
                    R3BLOG(error, "Missing trigger information!");
                    R3BLOG(error, "Top: " << top->GetDetectorId() << ' ' << top->GetSideId() << ' ' << top->GetBarId());
                    R3BLOG(error, "Bot: " << bot->GetDetectorId() << ' ' << bot->GetSideId() << ' ' << bot->GetBarId());
                    s_was_trig_missing = true;
                }
                ++n2;
            }

            // Shift the cyclic difference window by half a window-length and move it back,
            // this way the trigger time will be at 0.
            auto top_ns = fCyclicCorrector->GetTAMEXTime(top->GetTimeLeading_ns() - top_trig_ns);
            auto bot_ns = fCyclicCorrector->GetTAMEXTime(bot->GetTimeLeading_ns() - bot_trig_ns);

            auto dt = top_ns - bot_ns;
            // Handle wrap-around.
            auto dt_mod = fmod(dt + c_range_ns, c_range_ns);
            if (dt < 0)
            {
                // We're only interested in the short time-differences, so we
                // want to move the upper part of the coarse counter range close
                // to the lower range, i.e. we cut the middle of the range and
                // glue zero and the largest values together.
                dt_mod -= c_range_ns;
            }
            // std::cout << top_i << ' ' << bot_i << ": " << top_ns << ' ' << bot_ns << " = " << dt << ' ' <<
            // std::abs(dt_mod) << '\n';
            if (std::abs(dt_mod) < c_bar_coincidence_ns)
            {
                inbarcoincidence++;
                // Hit!
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

                auto THit_raw = (bot->GetTimeLeading_ns() + top->GetTimeLeading_ns()) / 2.; // needed for TOF for ROLUs

                // std::cout<<"ToT: "<<top_tot << " "<<bot_tot<<"\n";

                // register multi hits
                vmultihits[iPlane][iBar] += 1;

                auto par = fHitPar->GetModuleParAt(iPlane, iBar);
                if (!par)
                {
                    R3BLOG(error, "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
                    continue;
                }

                // walk corrections
                if (par->GetPar1Walk() == 0. || par->GetPar2Walk() == 0. || par->GetPar3Walk() == 0. ||
                    par->GetPar4Walk() == 0. || par->GetPar5Walk() == 0.)
                {
                    R3BLOG(debug, "TofD walk correction not found");
                }
                else
                {
                    auto bot_ns_walk = bot_ns - walk(bot_tot,
                                                     par->GetPar1Walk(),
                                                     par->GetPar2Walk(),
                                                     par->GetPar3Walk(),
                                                     par->GetPar4Walk(),
                                                     par->GetPar5Walk());
                    auto top_ns_walk = top_ns - walk(top_tot,
                                                     par->GetPar1Walk(),
                                                     par->GetPar2Walk(),
                                                     par->GetPar3Walk(),
                                                     par->GetPar4Walk(),
                                                     par->GetPar5Walk());
                }

                // calculate tdiff
                auto tdiff = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2()));

                // calculate time of hit
                double THit = (bot_ns + top_ns) / 2. - par->GetSync();
                if (std::isnan(THit))
                {
                    R3BLOG(fatal, "TofD THit not found");
                }
                if (timeP0 == 0.)
                    timeP0 = THit;

                // calculate y-position
                auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();

                // calculate y-position from ToT
                auto posToT =
                    par->GetLambda() * log((top_tot * par->GetToTOffset2()) / (bot_tot * par->GetToTOffset1()));

                if (fTofdTotPos)
                {
                    pos = posToT;
                }

                Float_t paddle_width = 2.700;
                Float_t air_gap_paddles = 0.04;
                Float_t air_gap_layer = 5.;
                Float_t detector_width =
                    fPaddlesPerPlane * paddle_width + (fPaddlesPerPlane - 1) * air_gap_paddles + paddle_width;
                double xp = -1000.;
                // calculate x-position
                if (iPlane == 1 || iPlane == 3)
                {
                    xp = -detector_width / 2 + (paddle_width + air_gap_paddles) / 2 +
                         (iBar - 1) * (paddle_width + air_gap_paddles) +
                         gRandom->Uniform(-paddle_width / 2., paddle_width / 2.);
                }
                if (iPlane == 2 || iPlane == 4)
                {
                    xp = -detector_width / 2 + (paddle_width + air_gap_paddles) +
                         (iBar - 1) * (paddle_width + air_gap_paddles) +
                         gRandom->Uniform(-paddle_width / 2., paddle_width / 2.);
                }

                double para[4];
                double qb = 0.;
                if (fTofdQ > 0)
                {
                    if (fTofdTotPos)
                    {
                        // via pol3
                        para[0] = par->GetPola();
                        para[1] = par->GetPolb();
                        para[2] = par->GetPolc();
                        para[3] = par->GetPold();
                        qb = TMath::Sqrt(top_tot * bot_tot) /
                             (para[0] + para[1] * pos + para[2] * pow(pos, 2) + para[3] * pow(pos, 3));
                        qb = qb * fTofdQ;
                    }
                    else
                    {
                        // via double exponential:
                        para[0] = par->GetPar1a();
                        para[1] = par->GetPar1b();
                        para[2] = par->GetPar1c();
                        para[3] = par->GetPar1d();
                        auto q1 = bot_tot /
                                  (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                        para[0] = par->GetPar2a();
                        para[1] = par->GetPar2b();
                        para[2] = par->GetPar2c();
                        para[3] = par->GetPar2d();
                        auto q2 = top_tot /
                                  (para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3]);
                        q1 = q1 * fTofdQ;
                        q2 = q2 * fTofdQ;
                        qb = (q1 + q2) / 2.;
                    }
                }
                else
                {
                    qb = TMath::Sqrt(top_tot * bot_tot);
                }

                double parz[3];
                parz[0] = par->GetPar1za();
                parz[1] = par->GetPar1zb();
                parz[2] = par->GetPar1zc();

                if (parz[0] > 0 && parz[2] > 0)
                    LOG(debug) << "Charges in this event " << parz[0] * TMath::Power(qb, parz[2]) + parz[1] << " plane "
                               << iPlane << " ibar " << iBar;
                else
                    LOG(debug) << "Charges in this event " << qb << " plane " << iPlane << " ibar " << iBar;
                LOG(debug) << "Times in this event " << THit << " plane " << iPlane << " ibar " << iBar;
                if (iPlane == 1 || iPlane == 3)
                    LOG(debug) << "x in this event "
                               << -detector_width / 2 + (paddle_width + air_gap_paddles) / 2 +
                                      (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04
                               << " plane " << iPlane << " ibar " << iBar;
                if (iPlane == 2 || iPlane == 4)
                    LOG(debug) << "x in this event "
                               << -detector_width / 2 + (paddle_width + air_gap_paddles) +
                                      (iBar - 1) * (paddle_width + air_gap_paddles) - 0.04
                               << " plane " << iPlane << " ibar " << iBar;
                LOG(debug) << "y in this event " << pos << " plane " << iPlane << " ibar " << iBar << "\n";

                // Tof with respect LOS detector
                auto tof = fCyclicCorrector->GetTAMEXTime((bot_ns + top_ns) / 2. - header->GetTStart());
                // auto tof_corr = par->GetTofSyncOffset() + par->GetTofSyncSlope() * tof;
                auto tof_corr = tof - par->GetTofSyncOffset();

                // if (parz[1] > 0)
                // {
                event.push_back(
                    { parz[0] + parz[1] * qb + parz[2] * qb * qb, THit, xp, pos, iPlane, iBar, THit_raw, tof_corr });
                // }

                /* if (parz[0] > 0 && parz[2] > 0)
                 {
                     event.push_back(
                         { parz[0] * TMath::Power(qb, parz[2]) + parz[1], THit, xp, pos, iPlane, iBar, THit_raw, tof });
                 }
                 else
                 {
                     parz[0] = 1.;
                     parz[1] = 0.;
                     parz[2] = 1.;
                     event.push_back({ qb, THit, xp, pos, iPlane, iBar, THit_raw, tof });
                 }*/

                if (fTofdHisto)
                {
                    // fill control histograms
                    fhTsync[iPlane - 1]->Fill(iBar, THit);
                    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
                    fhQvsPos[iPlane - 1][iBar - 1]->Fill(pos, parz[0] * TMath::Power(qb, parz[2]) + parz[1]);
                    // fhQvsTHit[iPlane - 1][iBar - 1]->Fill(qb, THit);
                    // fhTvsTHit[iPlane - 1][iBar - 1]->Fill(dt_mod, THit);
                }

                for (int e = 0; e < event.size(); e++)
                {
                    LOG(debug) << event[e].charge << " " << event[e].time << " " << event[e].xpos << " "
                               << event[e].ypos << " " << event[e].plane << " " << event[e].bar;
                }

                ++top_i;
                ++bot_i;
            }
            else if (dt < 0 && dt > -c_range_ns / 2)
            {
                ++top_i;
                LOG(debug) << "Not in bar coincidence increase top counter";
            }
            else
            {
                ++bot_i;
                LOG(debug) << "Not in bar coincidence increase bot counter";
            }
        }
    }

    // Now all hits in this event are analyzed

    LOG(debug) << "Hits in this event: " << event.size();
    bool tArrU[event.size() + 1];
    for (int i = 0; i < (event.size() + 1); i++)
        tArrU[i] = kFALSE;

    for (int i = 1; i <= fNofPlanes; i++)
    {
        for (int j = 0; j < fPaddlesPerPlane + 1; j++)
        {
            if (vmultihits[i][j] > 1)
            {
                bars_with_multihit++;
                multihit += vmultihits[i][j] - 1;
                // if(vmultihits[i][j]>3)cout<<vmultihits[i][j] - 1<<"\n";
            }
        }
    }

    std::sort(event.begin(), event.end(), [](hit const& a, hit const& b) { return a.time < b.time; });
    // Now we have all hits in this event time sorted

    if (fTofdHisto)
    {
        LOG(debug) << "Charge Time xpos ypos plane bar";
        for (int hit = 0; hit < event.size(); hit++)
        {
            LOG(debug) << event[hit].charge << " " << event[hit].time << " " << event[hit].xpos << " "
                       << event[hit].ypos << " " << event[hit].plane << " " << event[hit].bar;
            // if (event[hit].plane == 2 && (event[hit].bar < 21 || event[hit].bar > 24)) fhTvsQ[event[hit].plane -
            // 1]->Fill(event[hit].time-event[0].time,event[hit].charge);
            if (event[hit].plane == 2 && (event[hit].bar == 18))
                fhTvsQ[event[hit].plane - 1]->Fill(event[hit].time - event[0].time, event[hit].charge);
        }
    }

    // Now we can analyze the hits in this event

    // select events with feasible times
    double hit_coinc = 20.; // coincidence window for hits in one event in ns. physics says max 250 ps
    double time0;
    for (int ihit = 0; ihit < event.size();)
    { // loop over all hits in this event
        LOG(debug) << "Set new coincidence window: " << event[ihit].plane << " " << event[ihit].bar << " "
                   << event[ihit].time << " " << event[ihit].charge;
        time0 = event[ihit].time;              // time of first hit in coincidence window
        double charge0 = event[ihit].charge; // charge of first hit in coincidence window
        int plane0 = event[ihit].plane;      // plane of first hit in coincidence window
        std::vector<double> goodcharge;
        std::vector<double> goodplane;
        std::vector<double> goodbar;
        struct goodhit
        {
            double goodq;
            double goodp;
            double goodb;
        };
        struct by_charge
        {
            bool operator()(goodhit const& a, goodhit const& b) const noexcept { return a.goodq < b.goodq; }
        };
        std::vector<goodhit> goodevents;

        while (event[ihit].time < time0 + hit_coinc)
        { // check if in coincidence window
            if (fTofdHisto)
            {
                if (event[ihit].plane == plane0 && charge0 != event[ihit].charge)
                {
                    fhQ0Qt[event[ihit].plane - 1]->Fill(charge0, event[ihit].charge);
                }
            }

            if ((event[ihit].charge > 5.5 && event[ihit].charge < 6.5) ||
                (event[ihit].charge > 1.5 && event[ihit].charge < 2.5))
            {
                goodcharge.push_back(event[ihit].charge);
                goodplane.push_back(event[ihit].plane);
                goodbar.push_back(event[ihit].bar);
            }

            LOG(debug) << "Hit in coincidence window: " << event[ihit].plane << " " << event[ihit].bar << " "
                       << event[ihit].time << " " << event[ihit].charge;

            ihit++;
            if (ihit >= event.size())
                break;
        }
        if (goodcharge.size() > 3)
        {
            if (goodcharge.size() == 4)
            {
                if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 14. &&
                    std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 18.)
                {
                    if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 6.)
                    {
                        LOG(debug) << "Found good pair 2 times in all planes";
                        for (int g = 0; g < goodcharge.size(); g++)
                        {
                            LOG(debug) << goodcharge.at(g);
                            LOG(debug) << goodplane.at(g);
                            LOG(debug) << goodbar.at(g);
                            goodevents.push_back({ goodcharge.at(g), goodplane.at(g), goodbar.at(g) });
                        }
                        goodpair++;
                        goodpair4++;
                    }
                    else
                    {
                        goodpair3++;
                    }
                }
            }
            else if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 14.)
                goodpair6++;
        }
        if (goodcharge.size() == 3)
        {
            if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 8.5 &&
                std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 15.5)
            {
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 4 ||
                    std::accumulate(goodplane.begin(), goodplane.end(), 0) == 5)
                {
                    LOG(debug) << "Found good pair at least once in all planes";
                    for (int g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(debug) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair5++;
                }
                else
                {
                    goodpair7++;
                }
            }
        }
        if (goodcharge.size() == 2)
        {
            if (std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) > 7. &&
                std::accumulate(goodcharge.begin(), goodcharge.end(), 0.0) < 9.)
            {
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 2. ||
                    std::accumulate(goodplane.begin(), goodplane.end(), 0) == 4.)
                {
                    LOG(debug) << "Found good pair in one plane";
                    for (int g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(debug) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair1++;
                }
                if (std::accumulate(goodplane.begin(), goodplane.end(), 0) == 3.)
                {
                    LOG(debug) << "Found good pair in different planes";
                    for (int g = 0; g < goodcharge.size(); g++)
                    {
                        LOG(debug) << goodcharge.at(g);
                    }
                    goodpair++;
                    goodpair2++;
                }
            }
        }
        std::sort(
            goodevents.begin(), goodevents.end(), [](goodhit const& a, goodhit const& b) { return a.goodq < b.goodq; });
        for (int g = 0; g < goodevents.size(); g++)
        {
            LOG(debug) << goodevents[g].goodq;
            LOG(debug) << goodevents[g].goodp;
            LOG(debug) << goodevents[g].goodb;
        }
    }

    if (fTofdHisto)
    {
        size_t ihit = 0;
        for (; ihit < event.size();)
        {                                                                          // loop over all hits
            fhQ[event[ihit].plane - 1]->Fill(event[ihit].bar, event[ihit].charge); // charge per plane
            fhQvsEvent[event[ihit].plane - 1]->Fill(fnEvents, event[ihit].charge); // charge vs event #
            fhxy[event[ihit].plane - 1]->Fill(event[ihit].bar, event[ihit].ypos);  // xy of plane
            ihit++;
        }
    }

    // store events
    for (int hit = 0; hit < event.size(); hit++)
    { // loop over hits
        if (tArrU[hit] == false)
        {
            eventstore++;
            tArrU[hit] = true;
            // store single hits
            singlehit++;
            new ((*fHitItems)[fHitItems->GetEntriesFast()]) R3BTofdHitData(event[hit].time,
                                                                           event[hit].xpos,
                                                                           event[hit].ypos,
                                                                           event[hit].charge,
                                                                           -1.,
                                                                           event[hit].charge,
                                                                           event[hit].plane,
                                                                           event[hit].bar,
                                                                           event[hit].time_raw,
                                                                           event[hit].tof);
        }
    }

    LOG(debug) << "Used up hits in this event:";
    for (int a = 0; a < event.size(); a++)
    {
        LOG(debug) << "Event " << a << " " << tArrU[a] << " ";
        if (tArrU[a] != true)
            LOG(fatal) << "Not all events analyzed!";
    }

    LOG(debug) << "------------------------------------------------------\n";
    fnEvents++;
}

void R3BTofDCal2Hit::CreateHistograms()
{
	double max_charge = 80.;
	TString strName;
	TString strName1;
	TString strName2;
	// create histograms if not already existing
        fhNoTpat = new TH1F("NoTpat", "NoTpat", 200, 0, 200);
        fhNoTpat->GetXaxis()->SetTitle("No Tpat event dist");
        
	for (int iPlane = 0; iPlane < fNofPlanes; iPlane++)
	{
		strName = Form("Time_Sync_Plane_%d", iPlane);
        	fhTsync[iPlane - 1] = new TH2F(strName.Data(), "", 50, 0, 50, 10000, -2000., 2000.);
        	fhTsync[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        	fhTsync[iPlane - 1]->GetYaxis()->SetTitle("THit in ns");

        	strName1 = Form("Time_Diff_Plane_%d", iPlane);
        	strName2 = Form("Time Diff Plane %d", iPlane);
        	fhTdiff[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), 50, 0, 50, 400, -8., 8.);
        	fhTdiff[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        	fhTdiff[iPlane - 1]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");

        	strName1 = Form("TvsQ_Plane_%d", iPlane);
        	strName2 = Form("Time Hit vs Q per event Plane %d", iPlane);
        	fhTvsQ[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), 10000, 0, 1000, 1000, 0., max_charge);
        	fhTvsQ[iPlane - 1]->GetXaxis()->SetTitle("Time between hits in ns");
        	fhTvsQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");

        	strName1 = Form("Q_Plane_%d", iPlane);
        	strName2 = Form("Q Plane %d", iPlane);
        	fhQ[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), 44, 0, 44, max_charge * 10, 0., max_charge);
        	fhQ[iPlane - 1]->GetYaxis()->SetTitle("Charge");
        	fhQ[iPlane - 1]->GetXaxis()->SetTitle("Bar #");
        	
		strName1 = Form("xy_Plane_%d", iPlane);
        	strName2 = Form("xy of Plane %d", iPlane);
        	fhxy[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), 46, 0, 46, 400, -100., 100.);
        	fhxy[iPlane - 1]->GetYaxis()->SetTitle("y-position in cm");
        	fhxy[iPlane - 1]->GetXaxis()->SetTitle("Bar #");

        	strName1 = Form("QvsEvent_Plane_%d", iPlane);
        	strName2 = Form("Charge vs Event # Plane %d ", iPlane);
        	fhQvsEvent[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), 2e5, 0, 2e9, max_charge * 10, 0., max_charge);
        	fhQvsEvent[iPlane - 1]->GetYaxis()->SetTitle("Charge");
        	fhQvsEvent[iPlane - 1]->GetXaxis()->SetTitle("Event #");
		
		strName1 = Form("QvsQt0_Plane_%d", iPlane);
		strName2 = Form("Q vs Q_time0 Plane %d ", iPlane);
		fhQ0Qt[iPlane - 1] = new TH2F(strName1.Data(), strName2.Data(), max_charge * 10, 0., max_charge, max_charge * 10, 0., max_charge);
		fhQ0Qt[iPlane - 1]->GetYaxis()->SetTitle("Charge particle i");
		fhQ0Qt[iPlane - 1]->GetXaxis()->SetTitle("Charge first particle");
        	
		for (int iBar = 0; iBar < fPaddlesPerPlane; iBar++)
		{
			strName = Form("Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
        		fhQvsPos[iPlane][iBar] = new TH2F(strName.Data(), "", 400, -100, 100, max_charge * 10, 0., max_charge);
        		fhQvsPos[iPlane][iBar]->GetYaxis()->SetTitle("Charge");
        		fhQvsPos[iPlane][iBar]->GetXaxis()->SetTitle("Position in cm");
			
			/*
    			strName = Form("Q_vs_THit_Plane_%d_Bar_%d", iPlane, iBar);
        		fhQvsTHit[iPlane][iBar] = new TH2F(strName, "", 1000, 0., max_charge, 1000, -10, 40);
        		fhQvsTHit[iPlane][iBar]->GetYaxis()->SetTitle("THit in ns");
        		fhQvsTHit[iPlane][iBar]->GetXaxis()->SetTitle("Charge");
			*/
		}
	}
}

void R3BTofDCal2Hit::FinishEvent()
{
    if (fHitItems)
    {
        fHitItems->Clear();
    }
}

void R3BTofDCal2Hit::FinishTask()
{
    if (fTofdHisto)
    {
	if (fhTpat) fhTpat->Write();
        if (fhNoTpat) fhNoTpat->Write();
        for (int i = 0; i < fNofPlanes; i++)
        {
            if (fhQ[i]) fhQ[i]->Write();
            if (fhxy[i]) fhxy[i]->Write();
            if (fhQvsEvent[i]) fhQvsEvent[i]->Write();
            if (fhTdiff[i]) fhTdiff[i]->Write();
            if (fhTsync[i]) fhTsync[i]->Write();
            if (fhQ0Qt[i])  fhQ0Qt[i]->Write();
            if (fhTvsQ[i])  fhTvsQ[i]->Write();
            for (int j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
            {
                // control histogram time particles
                if (fhQvsPos[i][j]) fhQvsPos[i][j]->Write();
                //if (fhQvsTHit[i][j]) fhQvsTHit[i][j]->Write();
            }
        }
    }

    std::cout << "\n\nSome statistics:" << std::endl;
    std::cout << "Total number of events in tree    " << maxevent << std::endl;
    std::cout << "Max Event analyzed                " << fnEvents + wrongtrigger + wrongtpat + notpat << std::endl;
    std::cout << "Events skipped due to trigger     " << wrongtrigger << std::endl;
    std::cout << "Events skipped due to tpat        " << wrongtpat << std::endl;
    std::cout << "Events with no tpat               " << notpat << std::endl;
    std::cout << "Events with correct header&tpat   " << headertpat << std::endl;
    std::cout << "Events without ToFd hits          " << events_wo_tofd_hits << std::endl;
    std::cout << "Events in cal level               " << events_in_cal_level << " leading and trailing edges" << std::endl;
    std::cout << "Hits in bar coincidence           " << inbarcoincidence;
    std::cout << " leading and trailing edges glued together" << std::endl;
    std::cout << "Could not match                   " << events_in_cal_level - inbarcoincidence << " edges" << std::endl;
    std::cout << "Bars with multihits               " << bars_with_multihit << std::endl;
    std::cout << "# Multihits in planes and bars    " << multihit << std::endl;
    std::cout << "Events stored                     " << eventstore << " <-> " << inbarcoincidence << " (Events in bar coincidence)" << std::endl;
    std::cout << "Events in single planes           " << singlehit << std::endl;
    //<< "Good events in total            " << eventstore << " <-> " << singlehit << " = singlehit \n";
    std::cout << "Really good events                " << goodpair4 << " 2 particles 2 times in 2 planes" << std::endl;
    std::cout << "Good events                       " << goodpair1 << " 2 particles in 1 plane" << std::endl;
    std::cout << "Good events                       " << goodpair2 << " 2 particles in different planes" << std::endl;
    std::cout << "Good events                       " << goodpair5 << " 2 particles at least once in 2 planes" << std::endl;
    std::cout << "Good events                       " << goodpair7 << " 3 in 2 planes" << std::endl;
    std::cout << "Good events                       " << goodpair3 << " 2 particles in 2 planes odd" << std::endl;
    std::cout << "Good events                       " << goodpair6 << " more than 2 particles in 2 planes" << std::endl;
    std::cout << "Good events in total              " << goodpair << std::endl;
}

double R3BTofDCal2Hit::walk(double Q, double par1, double par2, double par3, double par4, double par5)
{
    double y = -30.2 + par1 * TMath::Power(Q, par2) + par3 / Q + par4 * Q + par5 * Q * Q;
    return y;
}

ClassImp(R3BTofDCal2Hit)
