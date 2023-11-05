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

#pragma once

#define VFTX_CLOCK_MHZ 200 
#define N_TOFD_HIT_PLANE_MAX 4
#define N_TOFD_HIT_PADDLE_MAX 44

#include <FairTask.h>
#include <THnSparse.h>
#include <memory>

class TClonesArray;
class R3BTofDHitPar;
class R3BEventHeader;
class R3BTofDMappingPar;
class R3BTofDHitPar;
class R3BTDCCyclicCorrector;
class TH1F;
class TH2F;

/**
 * An analysis task to apply HIT calibration for ToFd.
 * it also applies walk corrections and Z-calibration
 */
class R3BTofDCal2Hit : public FairTask
{
  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BTofDCal2Hit();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BTofDCal2Hit(const char* name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BTofDCal2Hit();

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

    /**
     * Method for finish of the task execution.
     * Is called by the framework after processing the event loop.
     */
    virtual void FinishTask();

    virtual void SetParContainers();

    virtual void CreateHistograms();

    /**
     * Method for setting the nuclear charge of main beam
     */
    inline void SetTofdQ(double Q) { fTofdQ = Q; }

    /**
     * Method for setting histograms
     */
    inline void SetTofdHisto() { fTofdHisto = kTRUE; }
    /**
     * Method for setting y calculation via ToT instead of tdiff
     */
    inline void SetTofdTotPos(bool ToTy) { fTofdTotPos = ToTy; }

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }
    inline void SetTpat(int tpat1, int tpat2)
    {
        fTpat1 = tpat1;
        fTpat2 = tpat2;
    }

    /**
     * Methods for setting number of planes and paddles
     */
    inline void SetNofModules(int planes, int bars)
    {
        fNofPlanes = planes;
        fPaddlesPerPlane = bars;
    }

    // Method to setup online mode
    void SetOnline(bool option) { fOnline = option; }

  private:
    void SetParameter();

    /**
     * Method for walk calculation.
     */
    double walk(double Q, double par1, double par2, double par3, double par4, double par5);

    R3BCoarseTimeStitch* fTimeStitch{};
    R3BEventHeader* header{}; /**< Event header - input data. */
    R3BTofDHitPar* fHitPar{}; /**< Hit parameter container. */
    R3BTofDMappingPar* fMapPar{};

    bool fOnline = false;
    TClonesArray* fCalItems{};                                       /**< Array with Cal items - input data. */
    TClonesArray* fCalTriggerItems{};                                /**< Array with trigger Cal items - input data. */
    TClonesArray* fHitItems; 	                                     /**< Array with Hit items - output data. */
    uint32_t fNofHitPars = 0;                                          /**< Number of modules in parameter file. */
    double fClockFreq = (1. / VFTX_CLOCK_MHZ * 1000);              /**< Clock cycle in [ns]. */
    int fTrigger = -1;                                             /**< Trigger value. */
    int fTpat1 = -1;
    int fTpat2 = -1;
    double fTofdQ = 0;
    bool fTofdHisto = false;
    bool fTofdTotPos = false;
    uint32_t fnEvents = 0;
    uint32_t lasttpatevent = 0;
    uint32_t fNofPlanes = 4;
    uint32_t fPaddlesPerPlane = 44; /**< Number of paddles per plane. */
    uint32_t maxevent = 0;
    uint32_t wrongtrigger = 0;
    uint32_t wrongtpat = 0;
    uint32_t notpat = 0;
    uint32_t headertpat = 0;
    uint32_t events_in_cal_level = 0;
    uint32_t inbarcoincidence = 0;
    uint32_t eventstore = 0;
    uint32_t singlehit = 0;
    uint32_t multihit = 0;
    uint32_t bars_with_multihit = 0;
    uint32_t events_wo_tofd_hits = 0;
    uint32_t goodpair = 0;
    uint32_t goodpair4 = 0;
    uint32_t goodpair3 = 0;
    uint32_t goodpair1 = 0;
    uint32_t goodpair2 = 0;
    uint32_t goodpair5 = 0;
    uint32_t goodpair6 = 0;
    uint32_t goodpair7 = 0;

    // arrays of control histograms
    TH1F* fhTpat{};
    TH1F* fhNoTpat{};
    TH2F* fhQvsPos[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
    // TH2F* fhQvsTHit[N_TOFD_HIT_PLANE_MAX][N_TOFD_HIT_PADDLE_MAX]{};
    TH2F* fhQ[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhxy[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhQvsEvent[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhTdiff[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhTsync[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhQ0Qt[N_TOFD_HIT_PLANE_MAX]{};
    TH2F* fhTvsQ[N_TOFD_HIT_PLANE_MAX]{};

  public:
    ClassDef(R3BTofDCal2Hit, 1)
};
