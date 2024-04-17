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

#ifndef R3BSyncCheckData_asyeos_H
#define R3BSyncCheckData_asyeos_H 1

#include "R3BLogger.h"
#include "TObject.h"
#include <Rtypes.h>

class R3BSyncCheckData_asyeos : public TObject
{
  public:
    R3BSyncCheckData_asyeos();
    R3BSyncCheckData_asyeos(uint32_t master,
                     uint32_t masterref,
                     uint32_t bus,
                     uint32_t chimera);

    virtual ~R3BSyncCheckData_asyeos() {}

    inline const uint32_t& GetMaster() const { return fMaster; }
    inline const uint32_t& GetMasterRef() const { return fMasterRef; }
    inline const uint32_t& GetBus() const { return fBus; }
    inline const uint32_t& GetChimera() const { return fChimera; }

  protected:
    // following the order from ext file
    uint32_t fMaster = 0;
    uint32_t fMasterRef = 0;
    uint32_t fBus = 0;
    uint32_t fChimera = 0;

  public:
    ClassDef(R3BSyncCheckData_asyeos, 0)
};

#endif /* R3BSyncCheckData_asyeos_H */
