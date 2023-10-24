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
// -----                   R3BPspxCal2HitPar               -----
// -----            Created  13-03-2017 by I. Syndikus        -----
// -----              Modified  Dec 2019  by M. Holl	        -----
// ----------------------------------------------------------------

#ifndef R3BPSPXCAL2HITPAR_H
#define R3BPSPXCAL2HITPAR_H

#include "FairTask.h"
#include "R3BPspxCalData.h"
#include "TGraphErrors.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TDirectory.h"
#include "TCanvas.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxHitPar;

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

class R3BPspxCal2HitPar : public FairTask
{
  public:
    /** Default Constructor **/
    R3BPspxCal2HitPar();

    /** Standard Constructor **/
    R3BPspxCal2HitPar(const char* name, int iVerbose = 1);

    /** Destructor **/
    virtual ~R3BPspxCal2HitPar();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t*);

    /** Virtual method FinishTask **/
    virtual void Reset();

    /** Virtual method FinishTask **/
    virtual void FinishTask();


    /** Virtual method GetEnergyCorrection **/
    void GetEnergyCorrection();

    /** Method for setting number of detectors. 
     * No need to specify number of sides or channels 
     * - that's fixed for the PSPs **/
    void SetNumCalib(int calib) {fNumCalib = calib;}
    void SetNumDet(int det) {fNumDet = det;}
    void SetNumStrips(int strip) {fNumStrips = strip;}
    void SetCalibPar(int calibpar) {fCalibPar = calibpar;}
    void SetMaxStrip(std::vector<double> maxstrip) {fMaxStrip = maxstrip;}
    void SetMaxEnergy(std::vector<double> maxenergies) {fMaxEnergy = maxenergies;}

    void SetParOutName(TString parout) {fParOutName = parout;}
    void SetParOutName1(TString parout1) {fParOutName1 = parout1;}


  private:
    R3BEventHeader* fHeader{}; 
    TClonesArray* fCal{};    

    int counter_tot = 0;
    int counter_events = 0;

    unsigned int fNumCalib = 0; /** Number of detectors */
    unsigned int fNumDet = 0; /** Number of detectors */
    unsigned int fNumStrips = 0; /** Number of detectors */
    unsigned int fCalibPar = 0;

    TString fParOutName = "";
    TString fParOutName1 = "";

    std::vector<double> fMaxStrip{12};
    std::vector<double> fMaxEnergy{100};

    TH2F* RawEnergy[6][2][32]{};
      
    TCanvas *GausFitCan[6]{}; 

    std::ofstream outputfile;
    std::ofstream outputfile1;

  public:
    ClassDef(R3BPspxCal2HitPar, 2)
};

#endif
