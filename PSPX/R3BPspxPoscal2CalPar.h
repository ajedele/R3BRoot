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
// -----                   R3BPspxPoscal2CalPar               -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl	        -----
// ----------------------------------------------------------------

#ifndef R3BPSPXPOSCAL2CALPAR_H
#define R3BPSPXPOSCAL2CALPAR_H

#include "FairTask.h"
#include "R3BPspxInterstripPoscalData.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TDirectory.h"
#include "TCanvas.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxCalPar;

/**
 * Class to convert poscal data to Cal data for PSPX detector data.
 * Thresholds are applied to signal from both sides of each strip
 * Signal from side 2 of each strip is multiplied by gain for position calibration
 * @author Ina Syndikus
 * @since March 13, 2016
 * Modified Dec 2019 by M.Holl
 * Modified April 2021 by J.L.Rodriguez
 * Modified July 2022 by A. Jedele
 */

class R3BPspxPoscal2CalPar : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxPoscal2CalPar();

    /** Standard Constructor **/
    R3BPspxPoscal2CalPar(const char* name, int iVerbose);

    /** Destructor **/
    virtual ~R3BPspxPoscal2CalPar();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t*);

    /** Virtual method FinishTask **/
    virtual void Reset();

    /** Virtual method GetPosParamters **/
    virtual void GetPosParameters();

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    //Method to specify experiment. Very important due to different FWs used
    void SetNumExpt(int fNumExpt);

    /** Method for setting number of detectors. 
     * No need to specify number of sides or channels 
     * - that's fixed for the PSPs **/
    void SetNumDet(int);
    void SetNumStrips(int);

    void SetParOutName(TString);
    void SetMaxEnergies(const std::vector<double> arg);
  private:
    R3BEventHeader* fHeader{};                 // do we need that?
    TClonesArray* fInterstripPoscal{}; /**< Arrays holding input (Mapped) data */

    unsigned int fNumDet = 0; /**Number of detectors */
    unsigned int fNumStrips = 0; /**Number of detectors */
    unsigned int fNumExpt = 0; /**Number of experiment */

    TString fParOutName = "";

    int counter_tot = 0; 
    int counter_tot_tot = 0; 
    int counter_events = 0; 
    int counter_interstrip = 0; 
    int counter_tot_interstrip = 0; 
    int counter_bad_interstrip = 0; 
    int counter_good_partner = 0; 
    int counter_bad_time_partner = 0; 
    int counter_bad_energy_partner = 0; 
    int counter_bad_energy = 0; 

    int timing_index=0;
    int posx_index=0;
    int posy_index=0;

    std::vector<double> fMaxEnergies{6};
    double GainFactors[6][2][32][32] = {{{{0}}}};

    Bool_t ZombieAreAlive = kFALSE;

    int InterstripEvent(int ii);
    int FacePartner(int ii, int jj);

    double GetStripPosition(int ii);
    double GetPosition(int ii, int jj);

    TH1D *hInterstrip[10][5][35];
    TH1D *hInterstripTot[10][5][35];
    TH1D *hInterstripTotBad[10][5][35];
    TH2D *hStrip1vsStrip2[10][5][35];

    TH2D *hHitMap[10];
    TH2D *hPosXdet[10];
    TH2D *hPosYdet[10];
    TH1D *hPosX[10][35][35];
    TH1D *hPosY[10][35][35];
    TH2D *hPosYbad[10][2][35];
    TH2D *hPosYbaddiff[10][35];
    TH2D *hPosYgood[10][2][35];
    TH2D *hPosYgooddiff[10][35];
    TH2D *hPosvsEX[10][35];
    TH2D *hPosvsEY[10][35];
    TH2D *hPosvsEXSide1[10][35];
    TH2D *hPosvsEXSide2[10][35];
    TH2D *hPosvsEYSide1[10][35];
    TH2D *hPosvsEYSide2[10][35];

    TDirectory *savDir = gDirectory; 
    TDirectory *Interstrip = savDir->mkdir("Interstrip"); 
    TDirectory *Position = savDir->mkdir("Position"); 
    TDirectory *PositionStrips = savDir->mkdir("PositionStrips"); 
    TDirectory *PositionStripsGood = savDir->mkdir("PositionStripsGood"); 
    TDirectory *PositionStripsBad = savDir->mkdir("PositionStripsBad"); 
    TDirectory *Energy = savDir->mkdir("Energy"); 
    TDirectory *EnergySingleSide = savDir->mkdir("EnergySingleSide"); 
   
    std::ofstream outputfile;

  public:
    ClassDef(R3BPspxPoscal2CalPar, 1)
};

#endif
