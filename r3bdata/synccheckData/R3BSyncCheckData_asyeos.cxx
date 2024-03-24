/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BSyncCheckData_asyeos.h"

R3BSyncCheckData_asyeos::R3BSyncCheckData_asyeos()
{
}

R3BSyncCheckData_asyeos::R3BSyncCheckData_asyeos(uint32_t master,
                                   uint32_t masterref,
                                   uint32_t bus,
                                   uint32_t chimera)
    : fMaster(master)
    , fMasterRef(masterref)
    , fBus(bus)
    , fChimera(chimera)
{
}

ClassImp(R3BSyncCheckData_asyeos);
