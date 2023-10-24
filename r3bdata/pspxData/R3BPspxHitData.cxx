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

#include "R3BPspxHitData.h"

R3BPspxHitData::R3BPspxHitData(int detector, int face, int strip, double energy, double charge, double pos, double time, int mult, bool mainstrip)
    : fDetector(detector)
    , fFace(face)
    , fStrip(strip)
    , fEnergy(energy)
    , fCharge(charge)
    , fPosition(pos)
    , fTime(time)
    , fMult(mult)
    , fMainStrip(mainstrip)
{
}

ClassImp(R3BPspxHitData)
