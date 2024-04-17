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

#ifndef R3BPSPXPOSCALDATA_H
#define R3BPSPXPOSCALDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Cal level.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 * Modified Dec 2019 by M. Holl
 */

class R3BPspxPoscalData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxPoscalData(){}

    /** Standard Constructor **/
    R3BPspxPoscalData(int detector, int face, int strip, double energy1, double energy2, double time1, double time2, int mult, bool mainstrip);

    // Getters
    inline const int& GetDetector() const { return fDetector; }
    inline const int& GetFace() const { return fFace; }
    inline const int& GetStrip() const { return fStrip; }
    inline const double& GetEnergy1() const { return fEnergy1; }
    inline const double& GetEnergy2() const { return fEnergy2; }
    inline const double& GetTime1() const { return fTime1; }
    inline const double& GetTime2() const { return fTime2; }
    inline const int& GetMult() const { return fMult; }
    inline const bool& IsMainStrip() const { return fMainStrip; }

  private:
    int fDetector = -1;  // Detector number, counting from 1
    int fFace = -1;      // Strip number, counting from 1
    int fStrip = -1;     // Strip number, counting from 1
    double fEnergy1 = 0; // Energy from both sides of hit strip after corrected for the position.
    double fEnergy2 = 0; // Energy from both sides of hit strip after corrected for the position.
    double fTime1 = 0;   // Hit time from each side of hit strip. This value is corrected relative to the trigger time
    double fTime2 = 0;   // Hit time from each side of hit strip. This value is corrected relative to the trigger time
    //For Time calibration, dependent on the FW used. For Nik's FW, time has to be recentered. For CALIFA FW, time subtracted relative to the trigger.
    int fMult = -1;      //Multiplicity of the  side
    bool fMainStrip = -1;      //Multiplicity of the  side


  public:
    ClassDef(R3BPspxPoscalData, 4)
};

#endif
