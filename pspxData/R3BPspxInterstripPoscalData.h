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

#ifndef R3BPSPXINTERSTRIPPOSCALDATA_H
#define R3BPSPXINTERSTRIPPOSCALDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Cal level.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 * Modified Dec 2019 by M. Holl
 */

class R3BPspxInterstripPoscalData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxInterstripPoscalData(){}

    /** Standard Constructor **/
    R3BPspxInterstripPoscalData(int detector, int face, int strip, float energy1, float energy2, float time1, float time2);

    // Getters
    inline const int& GetDetector() const { return fDetector; }
    inline const int& GetFace() const { return fFace; }
    inline const int& GetStrip() const { return fStrip; }
    inline const float& GetEnergy1() const { return fEnergy1; }
    inline const float& GetEnergy2() const { return fEnergy2; }
    inline const float& GetTime1() const { return fTime1; }
    inline const float& GetTime2() const { return fTime2; }

  private:
    int fDetector{-1};  // Detector number, counting from 1
    int fFace{-1};      // Strip number, counting from 1
    int fStrip{-1};     // Strip number, counting from 1
    float fEnergy1{0}; // Energy from both sides of hit strip after corrected for the position.
    float fEnergy2{0}; // Energy from both sides of hit strip after corrected for the position.
    float fTime1{0};   // Hit time from both sides of hit strip. This value is corrected relative to the trigger time
    float fTime2{0};   // Hit time from both sides of hit strip. This value is corrected relative to the trigger time
    //For Time calibration, dependent on the FW used. For Nik's FW, time has to be recentered. For CALIFA FW, time subtracted relative to the trigger.


  public:
    ClassDef(R3BPspxInterstripPoscalData, 3)
};

#endif
