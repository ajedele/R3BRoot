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

#ifndef R3BPSPXHITDATA_H
#define R3BPSPXHITDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Hit level.
 * @author Ina Syndikus
 * @since April 2016
 * Modified Dec 2019 by M. Holl
 */

class R3BPspxHitData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxHitData(){}

    /** Standard Constructor Float_t time, **/
    R3BPspxHitData(int det, int face, int strip, double energy, double charge, double pos, double time, int mult, bool mainstrip);

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
    int fDetector = -1; // Detector Id
    int fFace = -1; // Detector Id
    int fStrip = -1; // Detector Id
    double fEnergy = 0; // Energy in MeV
    double fCharge = 0; // Charge in A 
    double fPosition = 0;  // Position in x cm in the range [-l/2:l/2]
    double fTime = 0; // Time in ns 
    int fMult = 0; // Multiplicity of side
    bool fMainStrip = 0;    // Position in x cm in the range [-l/2:l/2]

  public:
    ClassDef(R3BPspxHitData, 10)
};

#endif
