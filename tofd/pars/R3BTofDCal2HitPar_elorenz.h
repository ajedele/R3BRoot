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

// ------------------------------------------------------------
// -----               R3BTofDCal2HitPar_elorenz                  -----
// -----    Created 20/04/22 by J.L. Rodriguez-Sanchez    -----
// ------------------------------------------------------------

#ifndef R3BTofDCal2HitPar_elorenz_H
#define R3BTofDCal2HitPar_elorenz_H 1

#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

#include <map>

#include "FairTask.h"
#include "TObject.h"
#include <string.h>
#include "TDirectory.h"

class R3BTofDHitPar;
class TClonesArray;
class R3BEventHeader;
class R3BTofDMappingPar;
class TH1D;
class TH2D;

class R3BTofDCal2HitPar_elorenz : public FairTask
{
	public:
		/**
		 * Default constructor.
		 */
		R3BTofDCal2HitPar_elorenz();

		/**
		 * Standard constructor.
		 * Creates an instance of the task.
		 * @param name a name of the task.
		 * @param iVerbose a verbosity level.
		 */
		R3BTofDCal2HitPar_elorenz(const char* name, int iVerbose = 1);

		/**
		 * Destructor.
		 * Frees the memory used by the object.
		 */
		virtual ~R3BTofDCal2HitPar_elorenz();

		/**
		 * Method for task initialization.
		 * This function is called by the framework before
		 * the event loop.
		 * @return Initialization status. kSUCCESS, kERROR or kFATAL.
		 */
		virtual InitStatus Init();

		/**
		 * Method for event loop implementation.
		 * Is called by the framework every time a new event is read.
		 * @param option an execution option.
		 */
		virtual void Exec(Option_t* option);

		/**
		 * Method for finish of the task execution.
		 * Is called by the framework after processing the event loop.
		 */
		virtual void FinishTask();

		void SetParameterOption(int opt) { fParameter = opt; }

		/**
		 * Method for setting minimum required statistics per module.
		 * Only detector modules with number of entries in TDC
		 * distribution greater than minimum statistics will be
		 * calibrated.
		 * @param minStats a value of minimum statistics required.
		 */
		void SetMinStats(int minStats) { fMinStats = minStats; }
		void SetMinEntries(int minEntries) { fMinEntries = minEntries; }
		
		void SetOutofRange(int outofRange) { fOutofRange = outofRange; }

		/**
		 * Method for selecting events with certain trigger value.
		 * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
		 */
		void SetTrigger(int trigger) { fTrigger = trigger; }
		void SetTpat(int tpat) { fTpat = tpat; }

		/**
		 * Method for setting number of LOS detectors and channels.
		 * @param nDets number of detectors.
		 * @param nCh number of channels per detector (4+master trigger?)
		 */
		void SetNofModules(int nDets, int nCh)
		{
			fNofPlanes = nDets;
			fPaddlesPerPlane = nCh;
			fNofModules = nDets * nCh * 1; // 1 edge per channel
		}
		/**
		 * Method for setting the y-position of a horizonzal sweep run for calibration of effective velocity of light
		 */
		void SetTofdPos(double Y) { fTofdY = Y; }

		/**
		 * Method for setting the nuclear charge of main beam
		 */
		void SetTofdQ(double Q) { fTofdQ = Q; }

		/**
		 * Method for setting the max. charge for histograms
		 */
		void SetMaxQ(double Q) { fMaxQ = Q; }
		void SetMinQ(double Q) { fMinQ = Q; }

		void SetBarMax(double bar) { fBarMax = bar; }
		void SetBarMin(double bar) { fBarMin = bar; }

		/**
		 * Method for setting the nb of Z peaks to be calibrated
		 */
		void SetNbZPeaks(int nb) { fNbZPeaks = nb; }

		/**
		 * Method for setting charge correction
		 */
		void SetTofdZ(Bool_t Z) { fTofdZ = Z; }

		/**
		 * Method for setting the mean ToF
		 */
		void SetMeanToT(double tot) { fMeanToT = tot; }

		/**
		 * Method for setting the fit for the charge
		 * pol1
		 + pol2
		 */
		void SetChargeFit(TString type) { fZfitType = type; }

		/**
		 * Method for using smiley or double exponential charge correction
		 */
		void SetTofdSmiley(Bool_t Smiley) { fTofdSmiley = Smiley; }

		/**
		 * Method for setting the lower range of ToT for offset calibration
		 */
		void SetTofdTotLow(double TotLow) { fTofdTotLow = TotLow; }

		/**
		 * Method for setting the upper range of ToT for offset calibration
		 */
		void SetTofdTotHigh(double TotHigh) { fTofdTotHigh = TotHigh; }

		void SetTotRange(double totmin, double totmax) {fToTMin = totmin; fToTMax = totmax;}

		void SetPspx(bool pspx) {bPspx = pspx;}
		void SetPspxEnergy(double pspx_energy) {fPspxEnergy = pspx_energy;}

		inline void ReadHistoFile(TString file) { fHistoFile = file; }

	private:
		/**
		 * Method for creating histograms.
		 */
		void SetHistogramsNull();
		void CreateHistograms();
		void WriteHistograms();
		void calcZoffset();
		void calcOffset();
		void calcSync();
		void calcVeff();
		void calcLambda(double, double);

		void smiley(TH2D* histo, double min, double max, double* para);

		void doubleExp(TH2D* histo, double min, double max, double* para);

		/**
		 * Method for calculation of z correction.
		 */
		int zcorr(TH2D* histo, double min, double max, double*, int, int, int index);

		/**
		 * Method for calculation of ToT offset.
		 */
		void calcToTOffset(double, double);

		/**
		 * new Method for walk calculation.
		 */
		double walk(double Q, double par1, double par2, double par3, double par4, double par5);

		/**
		 * Method for calculation of saturation.
		 */
		double saturation(double x);

		int fParameter = -1;
		int fUpdateRate = 100000; /* An update rate. */
		int fMinStats = 100;   /* Minimum statistics required per module. */
		int fMinEntries = 100;   /* Minimum statistics required per module. */
		int fTrigger = -1;    /* Trigger value. */
		int fTpat = -1;
		int fOutofRange = 140;
		TString fZfitType = "pol1";
		unsigned int fNofPlanes = 4;       /* Number of planes. */
		unsigned int fPaddlesPerPlane = 44; /* Number of bars per plane. */
		unsigned int fNofModules = fNofPlanes * fPaddlesPerPlane;      /* Total number of modules (=edges) to calibrate */

		unsigned int fNEvents = 0; /* Event counter. */
		R3BTofDHitPar* fHitPar{};         /* Parameter container. */
		TClonesArray* fCalData{};         /* Array with mapped data - input data. */
		TClonesArray* fCalTriggerItems{}; /* Array with trigger Cal items - input data. */
		R3BEventHeader* fHeader{};        /* Event header  */
		TClonesArray* fPspxHitData{};     /* Array with mapped data - input data. */
		double fTofdY = 0.;
		double fTofdQ = 0.;
		double fMinQ = 10;
		double fMaxQ = 50;
		double fBarMin = 25;
		double fBarMax = 29;
		double fToTMin = 50;
		double fToTMax = 200;
		int fNbZPeaks = 2;
		bool fTofdSmiley = true;
		bool fTofdZ = false;
		double fMeanToT = 120;
		unsigned int maxevent = 0;
		double fTofdTotLow=0.;
		double fTofdTotHigh=200.;
		bool bPspx = false;
		double fPspxEnergy = 0;
		TString fHistoFile;

		// Arrays of control histograms
		TH2D* fh_tofd_TotPm[N_TOFD_HIT_PLANE_MAX][3]{};
		TH2D* fh_tofd_TotPm1[N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fh_tofd_TotPm2[N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhTdiff[N_TOFD_HIT_PLANE_MAX]{};
		TH2D* fhTsync[N_TOFD_HIT_PLANE_MAX]{};
		TH2D* fhLogTot1vsLogTot2[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhSqrtQvsPosToT[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhQvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhQXvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH1D* fhQvsPos1D[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH1D* fhQXvsPos1D[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhTot1vsTot2[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhTot1vsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhTot2vsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhlos[N_TOFD_HIT_PLANE_MAX]{};
		TH2D* fhlost[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fhtott[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fh1_walk_t[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fh1_walk_b[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
		TH2D* fZoffset[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};

	public:
		ClassDef(R3BTofDCal2HitPar_elorenz, 1)
};

#endif /* R3BTofDCal2HitPar_elorenz_H */
