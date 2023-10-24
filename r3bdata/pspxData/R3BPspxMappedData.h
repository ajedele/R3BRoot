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

#ifndef R3BPSPXMAPPEDDATA_H
#define R3BPSPXMAPPEDDATA_H

#include "TObject.h"

/**
 * Class containing PSPX detector data on Mapped level.
 * @author Ralf Plag, Ina Syndikus
 * @since January 2016
 * Modified Dec 2019 by M. Holl
 */

class R3BPspxMappedData : public TObject
{
  public:
    /** Default Constructor **/
    R3BPspxMappedData(){}

    /** Standard Constructor **/
    R3BPspxMappedData(int detector, int face, int side, int strip, int energy, int time, int trigger, Bool_t overflow);

    void SetValue(int, int, int, int, int, int, int, Bool_t);
    // Getters
    inline const int& GetDetector() const { return fDetector; }
    inline const int& GetFace() const { return fFace; }
    inline const int& GetSide() const { return fSide; }
    inline const int& GetStrip() const { return fStrip; }
    inline const int& GetEnergy() const { return fEnergy; }
    inline const int& GetTime() const { return fTime; }
    inline const int& GetTrigger() const { return fTrigger; }
    inline const int& IsOverflow() const { return fOverflow; }

  private:
    int fDetector = -1;  // Detector number, for s473 should be 3 
    int fFace = -1;  // Face number, counting from 1, Face = 1 is x and Face = 2 is y
    int fSide = -1;  // Channel number, counting from 1, one entry for each side of each strip
    int fStrip = -1;  // Channel number, counting from 1, one entry for each side of each strip
    int fEnergy = -1; // Energy/Collected charge, one entry for each side of each strip
    int fTime = -1; // Time for each hit, one entry for each side of each strip
    int fTrigger = -1; // Time for each hit, one entry for each side of each strip
    int fOverflow = -1; // Overflow flag -> negative FEBEX energies

  public:
    ClassDef(R3BPspxMappedData, 2)
};

#endif
