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

#ifndef R3BPSPXCALDATA_H
#define R3BPSPXCALDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Cal level.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 * Modified Dec 2019 by M. Holl
 */

class R3BPspxCalData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxCalData(){}

    /** Standard Constructor **/
    R3BPspxCalData(int detector, int face, int strip, double energy, double pos, double time, int mult, bool mainstrip);

    // Getters
    inline const int& GetDetector() const { return fDetector; }
    inline const int& GetFace() const { return fFace; }
    inline const int& GetStrip() const { return fStrip; }
    inline const double& GetEnergy() const { return fEnergy; }
    inline const double& GetPosition() const { return fPosition; }
    inline const double& GetTime() const { return fTime; }
    inline const int& GetMult() const { return fMult; }
    inline const bool& IsMainStrip() const { return fMainStrip; }

  private:
    int fDetector = -1; // Detector number, counting from 1
    int fFace = -1;    // Face number, counting from 1
    int fStrip = -1;    // Strip number, counting from 1
    double fEnergy = 0; // Energy from both sides of hit strip after corrected for the position.
    double fPosition = 0;   // Hit position from both sides of hit strip. Corrected E2
    double fTime = 0;   // Hit time from both sides of hit strip. This value is corrected relative to the trigger time for CALIFA FW and cyclic corrected for Niks FW
    int fMult = 0;   // Multiplicity of hits per face
    bool fMainStrip = -1; // If the strip has a value above 0.15*expected value - main peak

  public:
    ClassDef(R3BPspxCalData, 11)
};

#endif
