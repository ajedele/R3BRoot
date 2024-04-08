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

// ------------------------------------------------------------------
// -----                  R3BLosTCal2Hit                        -----
// -----       Updated January 16th 2023 by N.Mozumdar          -----
// ----- Convert time calibrated data to hit level (single time) ----
// ------------------------------------------------------------------

#ifndef R3BLOSTCAL2HIT
#define R3BLOSTCAL2HIT

#include <map>

#include "FairTask.h"

#include "TArrayF.h"

class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;
class R3BLosHitPar;
class R3BTDCCyclicCorrector;

class R3BLosTCal2Hit : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosTCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosTCal2Hit(const char* name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosTCal2Hit();

    /**
     * Method for task initialization.
     * This function is called by the framework before
     * the event loop.
     * @return Initialization status. kSUCCESS, kERROR or kFATAL.
     */
    virtual InitStatus Init();

    /**
     * Method for re-initialization of parameter containers
     * in case the Run ID has changed.
     */
    virtual InitStatus ReInit();

    /**
     * Method for event loop implementation.
     * Is called by the framework every time a new event is read.
     * @param option an execution option.
     */
    virtual void Exec(Option_t* option);

    /**
     * A method for finish of processing of an event.
     * Is called by the framework for each event after executing
     * the tasks.
     */
    virtual void FinishEvent();

    inline void SetLosParamMCFD(double offsetX, double offsetY, double veffX, double veffY)
    {
        flosOffsetX = offsetX;
        flosOffsetY = offsetY;
        flosVeffX = veffX;
        flosVeffY = veffY;
    }

    inline void SetLosParamToT(double offsetXQ, double offsetYQ, double veffXQ, double veffYQ)
    {
        flosOffsetXQ = offsetXQ;
        flosOffsetYQ = offsetYQ;
        flosVeffXQ = veffXQ;
        flosVeffYQ = veffYQ;
    }

    inline void SetLosParamTAMEX(double offsetXT, double offsetYT, double veffXT, double veffYT)
    {
        flosOffsetXT = offsetXT;
        flosOffsetYT = offsetYT;
        flosVeffXT = veffXT;
        flosVeffYT = veffYT;
    }

    /**
     * Methods for setting input files
     */
    inline void SetLosInput(std::string const& walk_param_file, std::string const& tot_param_file)
    {
        fwalk_param_file = walk_param_file;
        ftot_param_file = tot_param_file;
    }

    /**
     * Method for setting the trigger value.
     * @param trigger 1 - physics, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat) { fTpat = tpat; }
    inline void SetNofDet(uint32_t nDets) { fNofDetectors = nDets; }

    /**
     * Method for walk calculation.
     */
    virtual double walk(int inum, double tot);

    /**
     * Method for saturation correction.
     */
    virtual double satu(int inum, double tot, double dt);

    virtual void SetParContainers();

    /** Method to select online mode **/
    void SetOnline(bool option) { fOnline = option; }

    void SetVFTXWindow(double window) { fWindowV = window; }

  private:
    void SetParameter();
    R3BLosHitPar* fLosHit_Par;        // Parameter container
    TClonesArray* fTCalItems;         /**< Array with Cal items - input data. */
    TClonesArray* fTCalTriggerItems;  /**< Array with Trigger Cal items - input data. */
    TClonesArray* fHitItems;          /**< Array with Hit items - output data. */
    R3BTDCCyclicCorrector* fCyclicCorrector;      /**< Array with Hit items - output data. */
    float fp0, fp1;
    TArrayF* fLEMatchParams;
    TArrayF* fTEMatchParams;

    bool fOnline; // Don't store data for online

    R3BEventHeader* header; /**< Event header. */

    // check for trigger
    int fTrigger; /**< Trigger value. */
    int fTpat;

    int fNumParamsTamexLE;
    int fNumParamsTamexTE;
    uint32_t fNofDetectors; /**< Number of detectors. */

    uint32_t fNofHitItems; /**< Number of hit items for cur event. */
    double fClockFreq; /**< Clock cycle in [ns]. */
    double fWindowV;   // VFTX coincidence window in ns
    double flosVeffX;
    double flosVeffY;
    double flosOffsetX;
    double flosOffsetY;
    double flosVeffXQ;
    double flosVeffYQ;
    double flosOffsetXQ;
    double flosOffsetYQ;
    double flosVeffXT;
    double flosVeffYT;
    double flosOffsetXT;
    double flosOffsetYT;
    double walk_par[16][11]{}; // Array containing walk parameters: x=PM, y=min,max,p0...p9; MCFD and TAMEX considered
    double tot_par[8][4]{};    // Array containing walk parameters: x=PM, y=p0...p3;
    std::string fwalk_param_file;
    std::string ftot_param_file;

    TClonesArray* fMapped; /**< Array with mapped data - input data. */

    int Icount = 0;

  public:
    ClassDef(R3BLosTCal2Hit, 1)
};

#endif
