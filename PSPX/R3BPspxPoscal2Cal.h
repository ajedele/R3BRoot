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
// -----                   R3BPspxPoscal2Cal               -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl	        -----
// ----------------------------------------------------------------

#ifndef R3BPSPXPOSCAL2CAL_H
#define R3BPSPXPOSCAL2CAL_H
#define _GLIBCXX_ASSERTIONS 1

#include "FairTask.h"
#include "R3BPspxPoscalData.h"
#include "R3BPspxCalData.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"
#include "boost/multi_array.hpp"

class TClonesArray;
class R3BEventHeader;
class R3BPspxCalPar;

/**
 * Class to convert Mapped data to Cal data for PSPX detector data.
 * Thresholds are applied to signal from both sides of each strip
 * Signal from side 2 of each strip is multiplied by gain for position calibration
 * @author Ina Syndikus
 * @since March 13, 2016
 * Modified Dec 2019 by M.Holl
 * Modified April 2021 by J.L.Rodriguez
 * Modified July 2022 by A. Jedele
 */

class R3BPspxPoscal2Cal : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxPoscal2Cal();

    /** Standard Constructor **/
    R3BPspxPoscal2Cal(const char* name, int iVerbose);

    /** Destructor **/
    virtual ~R3BPspxPoscal2Cal();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method FinishTask **/
    virtual void SetParContainers();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method FinishTask **/
    virtual void Reset();

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();


    /** Method for setting number of detectors. 
     * No need to specify number of sides or channels 
     * - that's fixed for the PSPs **/
    void SetNumExpt(int);
    void SetNumDet(int);
    void SetNumStrips(int);

    void SetParInName(TString);

  private:
    R3BEventHeader* fHeader{};                 
    TClonesArray* fPoscal{}; /**< Arrays holding output (Cal) data */
    TClonesArray* fCal{}; /**< Arrays holding output (Cal) data */

    R3BPspxCalPar* fCalPar; /**< Parameter instance holding thresholds and gains for position correction */

    int fNumExpt = 0; /**Number of detectors */
    int fNumDet = 0; /**Number of detectors */
    int fNumStrips = 0; /**Number of detectors */

    TString fParInName = "";
    
    int maxenergy[6] = {300000, 18000, 450000, 26000, 450000, 25000};

    Bool_t ZombieAreAlive = kFALSE;

    double GainFactors[6][2][32][2] = {{{{0.}}}};
    int FitType[6][2][32] = {{{0}}};
    double totalpar[6][4] = {0};

    TH2F* hPos[10][2];
    TH2F* hPosRaw[10][2];
    TH2F* hPosCompare[10][2];
    TH2F* hTime[10][2];
    TH2F* hEnergy[10][2];
    TH2F* hEnergyCorr[10][2];
    TH2F* hPosEnergy[10][2][32];
    TH2F* hPosEnergyCorr[10][2][32];

    int counter=0; int counter_total=0;
    int counter_mult = 0; int counter_mult_pairing = 0; int counter_pairing = 0;
    int timing_counter = 0; int timing_small_counter = 0; int totmult_counter=0;

    TDirectory *savDir = gDirectory; 
    TDirectory *poscal2calHisto = savDir->mkdir("Poscal2CalHistos"); 
 
    R3BPspxCalData* AddCalData(int det, int face, int strip, double energy, double pos, double time, int mult, bool mainstrip);

  public:
    ClassDef(R3BPspxPoscal2Cal, 2)
};

#endif
