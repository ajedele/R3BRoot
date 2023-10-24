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
// -----------------------------------------------------------------
// -----           R3BPspxMappedPar header file                -----
// -----           Created 16/05/12  by I.Syndikus             -----
// -----           Modified Dec 2019 by M. Holl                -----
// -----------------------------------------------------------------

#include "R3BPspxPoscalPar.h"

#include "FairDetParIo.h"
#include "FairLogger.h"
#include "FairParIo.h"
#include "FairParamList.h"
#include "TMath.h"
#include "TString.h"
#include "TArrayF.h"

// -------- Standard Constructor ----------------------------------------------
R3BPspxPoscalPar::R3BPspxPoscalPar(const char* name, const char* title, const char* context)
    : FairParGenericSet(name, title, context)
    , fNumExpt(-1)
    , fNumDet(-1)
    , fNumStrips(-1)
{
    fCalPosPar = new TArrayF(fNumDet * 2 * fNumStrips * fNumStrips); // number of detectors * number of faces * number of strips * *number of strips on other side  - pos cal par
}

// -------- Destructor --------------------------------------------------------
R3BPspxPoscalPar::~R3BPspxPoscalPar() 
{
    	clear();
	if (fCalPosPar) delete fCalPosPar;
}

// -------- Method clear ------------------------------------------------------
void R3BPspxPoscalPar::clear()
{
    status = kFALSE;
    resetInputVersions();
}

// -------- Method putParams --------------------------------------------------
void R3BPspxPoscalPar::putParams(FairParamList* list)
{
    LOG(info) << "I am in R3BPspxPoscalPar::putParams ";
    if (!list)
    { 	
	LOG(error) << "Could not find FairParamList)";    
	    return;
    }

    list->add("R3BPspxPoscalDetectors", fNumDet);
    list->add("R3BPspxPoscalStrips", fNumStrips);

    Int_t array_size = fNumDet * 2 * fNumStrips * fNumStrips;
    LOG(info) << "Array size: " << array_size;

    fCalPosPar->Set(array_size);
    list->add("R3BPspxPosPar", *fCalPosPar);
}

// -------- Method getParams --------------------------------------------------
Bool_t R3BPspxPoscalPar::getParams(FairParamList* list)
{
    LOG(info) << "I am in R3BPspxPoscalPar::getParams ";

    if (!list)
    { 	
	LOG(error) << "Could not find FairParamList)";    
	    return kFALSE;
    }

    if (!list->fill("R3BPspxPoscalDetectors", &fNumDet)) {return kFALSE;}
    if (!list->fill("R3BPspxPoscalStrips", &fNumStrips)) {return kFALSE;}

    // count all entries: lines with strip info (2 entries) + lines with detector info (3 entries)
    Int_t array_size = fNumDet * 2 * fNumStrips * fNumStrips;
    LOG(info) << "R3BPspxPoscalPosPar Array Size: " << array_size;

    fCalPosPar->Set(array_size);
    if (!(list->fill("R3BPspxPoscalPosPar", fCalPosPar)))
    {
        LOG(warning) << "Could not initialize R3BPspxPoscalPosPar";
        return kFALSE;
    }

    return kTRUE;
}

// -------- Print Params ------------------------------------------------------
void R3BPspxPoscalPar::printParams()
{

    LOG(info) << "R3BPspxPoscalPosPar::printParams";
    LOG(info) << "fNumDetectors: " << fNumDet;
    LOG(info) << "fNumStrips: " << fNumStrips;

    Int_t array_size = fNumDet * 2 * fNumStrips * fNumStrips;
    LOG(info) << "\n***********************\nArray Size: " << array_size << "\n***********************\n";
    LOG(info) << "fCalPosPar: ";
    //for (Int_t i = 0; i < array_size; i++)
    //{
    //    LOG(info) << i << " :" << fCalPosPar->GetAt(i);
    //}
}

ClassImp(R3BPspxPoscalPar)
