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

#include "FairLogger.h"
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "ext_data_struct_info.hh"
#include <iostream>

#include "R3BLogger.h"
#include "R3BPspxReader.h"
#include "R3BPspxMappedData.h"

extern "C"
{
#include "ext_data_client.h"
#include "ext_h101_pspx.h"
}

#define LENGTH(x) (sizeof x / sizeof *x)
#define MAX_PSPX_DETECTORS (sizeof data->PSPX / sizeof data->PSPX[0])

R3BPspxReader::R3BPspxReader(EXT_STR_h101_PSPX* data, size_t offset)
    : R3BReader("R3BPspxReader")
    , fData(data)
    , fOffset(offset)
    , fOnline(kFALSE)
    , fArray(new TClonesArray("R3BPspxMappedData")) // number of faces of detectors
{
}

R3BPspxReader::~R3BPspxReader()
{
    if (fArray) {delete fArray;}
}

/**
 * Initialize output data. Read input data.
 */
bool R3BPspxReader::Init(ext_data_struct_info* a_struct_info)
{
    int ok;
    R3BLOG(info, "R3BPspxReader::Init");
    EXT_STR_h101_PSPX_ITEMS_INFO(ok, *a_struct_info, fOffset, EXT_STR_h101_PSPX, 0);

    if (!ok)
    {
        R3BLOG(error, "Failed to setup structure information.");
        return kFALSE;
    }

    //Register output data array in tree 
    FairRootManager::Instance()->Register("PspxMapped", "PSPX", fArray, kTRUE);
    Reset();
    memset(fData, 0, sizeof *fData);

    // Register output array in tree
    EXT_STR_h101_PSPX_onion* data = (EXT_STR_h101_PSPX_onion*)fData;
    for (int dd = 0; dd < MAX_PSPX_DETECTORS; dd++)
    {
        for (int ff = 0; ff < 2; ff++)
        {
		for (int ss = 0; ss < 2; ss++)
		{
			data->PSPX[dd].F[ff].S[ss].E = 0;
			data->PSPX[dd].F[ff].S[ss].T = 0;
              }
        }
    }
    return kTRUE;
}

/**
 * Does the unpacking to Mapped level. It is called for every event.
 * Converts plain raw data to multi-dimensional array.
 * Ignores energies with an error message.
 */
bool R3BPspxReader::R3BRead()
{
    EXT_STR_h101_PSPX_onion* data = (EXT_STR_h101_PSPX_onion*)fData;

    // loop over all detectors
    for (int32_t det = 0; det < MAX_PSPX_DETECTORS; det++)
    {
        // loop over faces
        for (int32_t face = 0; face < 2; face++)
        {
            // loop over strip sides
            for (int32_t side = 0; side < 2; side++)
            {
                auto const& dfs = data->PSPX[det].F[face].S[side];
                uint32_t numHitsE = dfs.E;
                // loop over hits
                for (uint32_t i = 0; i < numHitsE; i++)
                {
      		    int32_t strip     = dfs.EI[i]; // counting from 1 to max number of channels for an detector
                    int32_t energy    = dfs.Ev[i];
                    int32_t timestrip = dfs.TI[i];
                    int32_t time      = dfs.Tv[i];
                    int32_t trigger;
		    bool overflow;
		    if (dfs.EI[i]<16) trigger = dfs.TT[0];
                    else trigger= dfs.TT[1];
		    if (((det==0 || det==1) && face==1) || ((det==2 || det==3) && face==0) || ((det==4 || det==5) && face==1)) {energy = energy*-1;} //FEBEX collects negative energies for the p-side of the detector (hence the side requirement)
                    if (energy < 0) overflow = kTRUE; //negative energy is overflow -> bad 
		    else overflow = kFALSE; //negative energy is overflow -> bad 

                    new ((*fArray)[fArray->GetEntriesFast()])
                    R3BPspxMappedData(det+1, face+1, side+1, strip, energy, time, trigger, overflow);
		}
            }
        }
    }
    return kTRUE;
}

void R3BPspxReader::Reset()
{
    if (fArray) fArray->Clear();
}

ClassImp(R3BPspxReader)
