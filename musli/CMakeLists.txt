##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

# Create a library called "libR3BMusli" which includes the source files given 
# in the array. The extension is already found. Any number of sources could be 
# listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES 
${SYSTEM_INCLUDE_DIRECTORIES} 
${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
#put here all directories where header files are located
${R3BROOT_SOURCE_DIR}/r3bbase
${R3BROOT_SOURCE_DIR}/mwpc
${R3BROOT_SOURCE_DIR}/mwpc/mwpc0
${R3BROOT_SOURCE_DIR}/mwpc/mwpc1
${R3BROOT_SOURCE_DIR}/musli
${R3BROOT_SOURCE_DIR}/musli/online
${R3BROOT_SOURCE_DIR}/musli/pars
${R3BROOT_SOURCE_DIR}/musli/calibration
${R3BROOT_SOURCE_DIR}/r3bdata
${R3BROOT_SOURCE_DIR}/r3bdata/musliData
${R3BROOT_SOURCE_DIR}/r3bdata/frsData
${R3BROOT_SOURCE_DIR}/r3bdata/mwpcData
${R3BROOT_SOURCE_DIR}/tracking
)

include_directories( ${INCLUDE_DIRECTORIES})
include_directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES ${ROOT_LIBRARY_DIR} ${FAIRROOT_LIBRARY_DIR} )

link_directories( ${LINK_DIRECTORIES})

file(GLOB SRCS 
./pars/*.cxx
./calibration/*.cxx
./online/*.cxx
)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h HEADERS "${SRCS}")

set(LINKDEF MusliLinkDef.h)
set(LIBRARY_NAME R3BMusli)
set(DEPENDENCIES
          R3BBase R3BData R3BTracking)

GENERATE_LIBRARY()
