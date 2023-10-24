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

#include "R3BPspxInterstripPoscalData.h"

R3BPspxInterstripPoscalData::R3BPspxInterstripPoscalData(int detector, int face, int strip, float energy1, float energy2, float time1, float time2)
    : fDetector(detector)
    , fFace(face)
    , fStrip(strip)
    , fEnergy1(energy1)
    , fEnergy2(energy2)
    , fTime1(time1)
    , fTime2(time2)
{
}

ClassImp(R3BPspxInterstripPoscalData)
