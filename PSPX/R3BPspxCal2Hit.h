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
// -----                   R3BPspxCal2Hit               -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl	        -----
// ----------------------------------------------------------------

#ifndef R3BPSPXCAL2HIT_H
#define R3BPSPXCAL2HIT_H

#include "FairTask.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxHitPar.h"
#include "TGraph.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

#include "TGraphErrors.h"

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

class R3BPspxCal2Hit : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxCal2Hit();

    /** Standard Constructor **/
    R3BPspxCal2Hit(const char* name, int iVerbose);

    /** Destructor **/
    virtual ~R3BPspxCal2Hit();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option);

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    void ChanneltoEnergyLossConversion();
    void PositionEnergyCorrection();

    /** Method for setting number of detectors. 
     * No need to specify number of sides or channels 
     * - that's fixed for the PSPs **/
    void SetRunTag(int);
    void SetNumDet(int);
    void SetNumStrips(int);

    void SetParName(TString);
    void SetParInName1(TString);
    void SetParInName2(TString);
    void SetParInName3(TString);
    void SetParInName4(TString);
    void SetParOutName(TString);
    void SetParOutName2(TString);

  private:
    R3BEventHeader* fHeader;                
    TClonesArray* fCal{}; 
    TClonesArray* fHit{}; 

    TFile *ff{};

    double EnergyMatch[12] = {300000,230000,17000,13000,460000,340000,26000,19000,450000,330000,25000,18500};
    int maxenergy[6] = {300000, 18000, 450000, 26000, 450000, 25000};

    unsigned int fRunId;    
    unsigned int fNumDet;    
    unsigned int fNumStrips; 

    double Beta(double gamma);
    double Gamma(double energy);

    TString fParName;
    TString fParInName1;
    TString fParInName2;
    TString fParInName3;
    TString fParInName4;
    TString fParOutName;
    TString fParOutName2;

    int fIndex = 0;
    Bool_t ZombieAreAlive = kFALSE;

    //value from Bethe Bloch from ATIMA
    double BB[4] = {449.082, 394.773, 373.066, 351.553};
    
    double totalfits[10][4][4]  = {{{0}}};
    
    double posefits[10][4][35][3]  = {{{{0}}}};

    double GausFits[10][4][35][4][4]  = {{{{{0}}}}};

    double TotalFits[10][4][4][4]  = {{{{0}}}};

    double LinearFit[10][4][35][3] = {{{{0}}}};
    double AlignmentPar[10][4][35][3] = {{{{0}}}};

    TH1D* hFinalEnergy[10][2];
    TH1D* hFinalTime[10][2];
    TH1D* hFinalEnergyStrip[10][2][35];
    TH1D* hFinalTimeStrip[10][2][35];

    std::ifstream inputfile;
    std::ifstream inputfile1;
    std::ifstream inputfile2;
    std::ifstream inputfile3;
    std::ifstream inputfile4;
    std::ofstream outputfile;

    TGraphErrors *tBetheBloch[10][4][35];

    R3BPspxHitData* AddHitData(int det, int face, int strip, double energy, double charge, double pos, double time, int mult, bool mainstrip);

  public:
    ClassDef(R3BPspxCal2Hit, 1)
};

#endif
