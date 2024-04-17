/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_SYNC_ASYEOS_EXT_H101_SYNC_ASYEOS_H__
#define __GUARD_H101_SYNC_ASYEOS_EXT_H101_SYNC_ASYEOS_H__

#ifndef __CINT__
# include <stdint.h>
#else
/* For CINT (old version trouble with stdint.h): */
# ifndef uint32_t
typedef unsigned int uint32_t;
typedef          int  int32_t;
# endif
#endif
#ifndef EXT_STRUCT_CTRL
# define EXT_STRUCT_CTRL(x)
#endif

/********************************************************
 *
 * Plain structure (layout as ntuple/root file):
 */

typedef struct EXT_STR_h101_SYNC_ASYEOS_t
{
  /* RAW */
  uint32_t SYNC_CHECK_MASTER /* [0,65535] */;
  uint32_t SYNC_CHECK_MASTERRR /* [0,65535] */;
  uint32_t SYNC_CHECK_BUS /* [0,65535] */;
  uint32_t SYNC_CHECK_BUSRR /* [0,65535] */;
  uint32_t SYNC_CHECK_CHIMERA /* [0,65535] */;
  uint32_t SYNC_CHECK_CHIMERARR /* [0,65535] */;

} EXT_STR_h101_SYNC_ASYEOS;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_SYNC_ASYEOS_onion_t
{
  /* RAW */
  uint32_t SYNC_CHECK_MASTER;
  uint32_t SYNC_CHECK_MASTERRR;
  uint32_t SYNC_CHECK_BUS;
  uint32_t SYNC_CHECK_BUSRR;
  uint32_t SYNC_CHECK_CHIMERA;
  uint32_t SYNC_CHECK_CHIMERARR;

} EXT_STR_h101_SYNC_ASYEOS_onion;

/*******************************************************/

#define EXT_STR_h101_SYNC_ASYEOS_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MASTER,               UINT32,\
                    "SYNC_CHECK_MASTER",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_MASTERRR,             UINT32,\
                    "SYNC_CHECK_MASTERRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_BUS,                  UINT32,\
                    "SYNC_CHECK_BUS",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_BUSRR,                UINT32,\
                    "SYNC_CHECK_BUSRR",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_CHIMERA,              UINT32,\
                    "SYNC_CHECK_CHIMERA",65535,0/*flags*/); \
  EXT_STR_ITEM_INFO2_LIM(ok,si,offset,struct_t,printerr,\
                     SYNC_CHECK_CHIMERARR,            UINT32,\
                    "SYNC_CHECK_CHIMERARR",65535,0/*flags*/); \
  \
} while (0);
#endif/*__GUARD_H101_SYNC_ASYEOS_EXT_H101_SYNC_ASYEOS_H__*/

/*******************************************************/
