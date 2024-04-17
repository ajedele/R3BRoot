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

#pragma once

#include <TClonesArray.h>
#include "R3BReader.h"
#include "TClonesArray.h"
#include <Rtypes.h>

struct EXT_STR_h101_MUSLI_t;
typedef struct EXT_STR_h101_MUSLI_t EXT_STR_h101_MUSLI;
typedef struct EXT_STR_h101_MUSLI_onion_t EXT_STR_h101_MUSLI_onion;
class ext_data_struct_info;

class R3BMusliReader : public R3BReader
{
  public:
    // Standard constructor
    R3BMusliReader(EXT_STR_h101_MUSLI*, size_t);

    // Destructor
    virtual ~R3BMusliReader();

    // Setup structure information
    virtual bool Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual bool R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(bool option) { fOnline = option; }

  private:
    bool ReadData(EXT_STR_h101_MUSLI_onion*);

    /* Reader specific data structure from ucesb */
    EXT_STR_h101_MUSLI* fData;
    /* Data offset */
    size_t fOffset;
    // Don't store data for online
    bool fOnline;
    /* the structs of type R3BMusliMappedData Item */
    TClonesArray* fArray; /**< Output array. */

  public:
    ClassDefOverride(R3BMusliReader, 0);
};
