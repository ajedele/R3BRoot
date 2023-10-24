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

#ifndef R3BPSPXREADER_H
#define R3BPSPXREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSPX_t;
typedef struct EXT_STR_h101_PSPX_t EXT_STR_h101_PSPX;
typedef struct EXT_STR_h101_PSPX_onion_t EXT_STR_h101_PSPX_onion;
class ext_data_struct_info;

class FairLogger;
class TClonesArray;

/**
 * Class to unpack (with ucesb) to Mapped data for PSPX detector data.
 * This includes: Checking for error messages.
 * @author Ralf Plag (?), Bastian Loeher(?), Ina Syndikus
 * Modified by M. Holl, Dec 2019
 * Modified by A. Jedele, July 2023
 */

class R3BPspxReader : public R3BReader
{
  public:
    /** Standard Constructor **/
    R3BPspxReader(EXT_STR_h101_PSPX*, size_t);
    /** Destructor **/
    ~R3BPspxReader();

    // Setup structure information
    virtual bool Init(ext_data_struct_info*) override;
    
    // Read data from full event structure
    virtual bool R3BRead() override;
    
    // Reset the output array 
    virtual void Reset() override; 

    // Accessor to select online mode
    void SetOnline(bool option) { fOnline = option; }


  private:
    EXT_STR_h101_PSPX* fData; /**< Reader specific data structure from ucesb */
    unsigned int fOffset;          /**< Data Offset */
    FairLogger* fLogger;     /**< FairLogger */
    // Don't store data for online
    bool fOnline;
    TClonesArray* fArray; /**< Array holding output (Mapped) data */

  public:
    ClassDefOverride(R3BPspxReader, 0);
};

#endif
