/********************************************************
 *
 * Structure for ext_data_fetch_event() filling.
 *
 * Do not edit - automatically generated.
 */

#ifndef __GUARD_H101_TPAT_EXT_H101_TPAT_H__
#define __GUARD_H101_TPAT_EXT_H101_TPAT_H__

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

typedef struct EXT_STR_h101_TPAT_t
{
  /* RAW */
  uint32_t TPAT /* [0,170] */;
  uint32_t TPATv[170 EXT_STRUCT_CTRL(TPAT)] /* [0,65535] */;

} EXT_STR_h101_TPAT;

/********************************************************
 *
 * Structure with multiple levels of arrays (partially)
 * recovered (recommended):
 */

typedef struct EXT_STR_h101_TPAT_onion_t
{
  /* RAW */
  uint32_t TPAT;
  uint32_t TPATv[170 /* TPAT */];

} EXT_STR_h101_TPAT_onion;

/*******************************************************/

#define EXT_STR_h101_TPAT_ITEMS_INFO(ok,si,offset,struct_t,printerr) do { \
  ok = 1; \
  /* RAW */ \
  EXT_STR_ITEM_INFO_LIM(ok,si,offset,struct_t,printerr,\
                     TPAT,                            UINT32,\
                    "TPAT",170); \
  EXT_STR_ITEM_INFO_ZZP(ok,si,offset,struct_t,printerr,\
                     TPATv,                           UINT32,\
                    "TPATv",                           "TPAT"); \
  \
} while (0);
#endif/*__GUARD_H101_TPAT_EXT_H101_TPAT_H__*/

/*******************************************************/
