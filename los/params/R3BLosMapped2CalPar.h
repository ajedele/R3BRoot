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

// ----------------------------------------------------------------
// -----     Create time calib parameters for LOS via VFTX    -----
// -----      Adopted Feb 4th 2016 from Neuland by R. Plag    -----
// ----------------------------------------------------------------

#ifndef R3BLOSMAPPED2CALPAR_H
#define R3BLOSMAPPED2CALPAR_H

#include "FairTask.h"

class R3BTCalPar;
class TClonesArray;
class R3BEventHeader;
class R3BTCalEngine;

/**
 * An analysis task for TCAL calibration of NeuLAND Tamex data.
 * This class fills TDC distribution for each Photomultiplier
 * of the NeuLAND detector and calculates the calibration
 * parameters using the R3BTCalEngine.
 * @author D. Kresan
 * @since September 7, 2015
 */
class R3BLosMapped2CalPar : public FairTask
{

  public:
    /**
     * Default constructor.
     * Creates an instance of the task with default parameters.
     */
    R3BLosMapped2CalPar();

    /**
     * Standard constructor.
     * Creates an instance of the task.
     * @param name a name of the task.
     * @param iVerbose a verbosity level.
     */
    R3BLosMapped2CalPar(const char* name, int iVerbose = 1);

    /**
     * Destructor.
     * Frees the memory used by the object.
     */
    virtual ~R3BLosMapped2CalPar();

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

    /**
     * Method for setting the update rate for control histograms
     * @param rate an update rate value (events).
     */
    inline void SetUpdateRate(int rate) { fUpdateRate = rate; }

    /**
     * Method for setting minimum required statistics per module.
     * Only detector modules with number of entries in TDC
     * distribution greater than minimum statistics will be
     * calibrated.
     * @param minStats a value of minimum statistics required.
     */
    inline void SetMinStats(int minStats) { fMinStats = minStats; }

    /**
     * Method for selecting events with certain trigger value.
     * @param trigger 1 - onspill, 2 - offspill, -1 - all events.
     */
    inline void SetTrigger(int trigger) { fTrigger = trigger; }

    /**
     * Method for setting number of LOS detectors and channels.
     * @param nDets number of detectors.
     * @param nCh number of channels per detector (4+master trigger?)
     */
    inline void SetNofModules(int nDets, int nCh)
    {
        fNofDetectors = nDets;
        fNofChannels = nCh; // = 4 or 8 or 16
        fNofTypes = 3;
        fNofModules = nDets * nCh * 3; // 4 or 8 los signals *3 times per channel 12 or 24 in total
    }

  private:
    int fUpdateRate = 10000000; /**< An update rate. */
    int fMinStats = 1000;   /**< Minimum statistics required per module. */
    int fTrigger = -1;    /**< Trigger value. */
    int Icounts1 = 0;
    int Icounts2 = 0;
    int Icounts3 = 0;
    int Icount[16][3]{};
    int Icounttrig[16][3]{};

    int32_t fNofDetectors = 1; /**< Number of LOS detectors. */
    int32_t fNofChannels = 8;  /**< Number of channels per detector. */
    int32_t fNofTypes = 3; /**< Number of time-types per channel (VFTX, TAMEX leading/trailing). */
    int32_t fNofModules = 2;   /**< Total number of modules (=edges) to calibrate */

    int fNEvents = 0;        /**< Event counter. */
    R3BTCalPar* fCal_Par;  /**< Parameter container. */
    TClonesArray* fMapped; /**< Array with mapped data - input data. */
    TClonesArray* fMappedTriggerItems;
    R3BEventHeader* header; /**< Event header - input data. */

    R3BTCalEngine* fEngine; /**< Instance of the TCAL engine. */

  public:
    ClassDef(R3BLosMapped2CalPar, 1)
};

#endif
