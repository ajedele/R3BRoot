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
// -----                   R3BPspxMapped2Poscal               -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl	        -----
// ----------------------------------------------------------------

#ifndef R3BPSPXMAPPED2POSCAL_H
#define R3BPSPXMAPPED2POSCAL_H

#include "FairTask.h"
#include "R3BPspxInterstripPoscalData.h"
#include "R3BPspxPoscalData.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxPrecalPar;

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

class R3BPspxMapped2Poscal : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxMapped2Poscal();

    /** Standard Constructor **/
    R3BPspxMapped2Poscal(const char* name, int iVerbose);

    /** Destructor **/
    virtual ~R3BPspxMapped2Poscal();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t*);

    /** Virtual method FinishEvent **/
    virtual void Reset();

    /** Virtual method FinishEvent **/
    virtual void FinishEvent();

    /** Virtual method FinishTask **/
    virtual void FinishTask();

    //Method to specify experiment. Very important due to different FWs used
    /** Method for setting number of detectors. 
     * No need to specify number of sides or channels 
     * - that's fixed for the PSPs **/
    void SetNumExpt(int det) {fNumExpt = det;}
    void SetNumDet(int det) {fNumDet = det;}
    void SetNumStrips(int strip) {fNumStrips = strip;}
    void SetMaxEnergies(std::vector<double> maxenergies) {fMaxEnergies = maxenergies;}

    void SetParOutName(TString);

  private:
    R3BEventHeader* fHeader{};                 // do we need that?
    TClonesArray* fMapped{}; /**< Arrays holding input (Mapped) data */
    TClonesArray* fPoscal{}; /**< Arrays holding input (Mapped) data */
    TClonesArray* fInterstripPoscal{}; /**< Arrays holding input (Mapped) data */

    unsigned int fNumExpt;   //Experiment Number
    unsigned int fNumDet;    //Number of detectors 
    unsigned int fNumStrips; //Number of detectors 

    TString fParOutName;

    int counter_tot = 0;
    int counter_tot_tot = 0;
    int counter_events = 0;
    int counter_matched_bad = 0;
    int counter_mult_mismatch = 0;
    int counter_timing_bad = 0;

    std::vector<double> fMaxEnergies{6};

    bool ZombieAreAlive = kFALSE;

    double GetTimeCorr_s473(int time);
    double GetTimeCorr_s515(int time, int trigger);
    bool MatchedEvents(int index);
    int MultFace(int det, int face);
    int FacePartner(int ii);

    int fArray[10][2][2][32] = {};

    TH2F* hOverflow[10][2][2]{};
    TH1F* hEnergy[10][2][2][32]{};
    TH1F* hTime[10][2][2][32]{};
    TH1F* hEnergyTot[10][2][32]{};

    TH2F* hMult[10]{};
    TH2F* hMultCheck[10]{};
    TH2F* hMultInterstrip[10]{};
    TH1F* hTimeDiffPartner[10]{};
       
    TDirectory *savDir = gDirectory; 
    TDirectory *Mult = savDir->mkdir("Multiplicity"); 
    TDirectory *Overflow = savDir->mkdir("Overflow"); 
    TDirectory *Timing = savDir->mkdir("Timing"); 
    TDirectory *Energy = savDir->mkdir("Energy"); 
   
    std::ofstream outputfile;

    R3BPspxPoscalData* AddPoscalData(int det, int face, int strip, double energy1, double energy2, double time1, double time2, int mult1, bool mainstrip);
    R3BPspxInterstripPoscalData* AddInterstripPoscalData(int det, int face, int strip, double energy1, double energy2, double time1, double time2);
    bool IsGoodStrip(int det, int face, int strip);
    bool IsMainStrip(int det, int face, int strip);


  public:
    ClassDef(R3BPspxMapped2Poscal, 1)
};

#endif
