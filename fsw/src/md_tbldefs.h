/************************************************************************
** File:
**   $Id: md_tbldefs.h 1.3 2017/05/22 14:56:36EDT mdeschu Exp  $
**
**  Copyright (c) 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** Purpose: 
**   Data structure definitions for Memory Dwell table loads.
**
** Notes:
**
** 
*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _md_tbldefs_h_
#define _md_tbldefs_h_

/*
** Required header files...
*/
#include "md_platform_cfg.h"
#include "cfe.h"
/* cfs_utils.h needed for CFS_SymAddr_t */
#include "cfs_utils.h"

/************************************************************************
** Type Definitions
*************************************************************************/

/* 
**  \brief Memory Dwell structure for individual memory dwell specifications
**
**  \par Description
**          To be valid, entry must either be a null entry
**          (specified by a zero field length) or all of its
**          address and length fields must pass various checks.
**
**          The following checks must be met for a valid non-null entry:
**
**          * Length fields must be 1, 2, or 4.
**          * Delay can be any value in a uint16 field.
**          * DwellAddress.SymName must be a null string or must be found in the Symbol Table.
**          * The sum of DwellAddress.SymName plus DwellAddress.Offset must be in a valid range,
**          as determined by OSAL routines.
**          * If the Length field is 2, the sum of DwellAddress.SymName plus DwellAddress.Offset
**          must be even.
**          * If the Length field is 4, the sum of DwellAddress.SymName plus DwellAddress.Offset
**          must be evenly divisable by 4.
*/    
typedef struct
{
	uint16          Length;       /**< \brief  Length of dwell  field in bytes.  0 indicates null entry. */
	uint16          Delay;        /**< \brief  Delay before following dwell sample in terms of number of task wakeup calls */
    CFS_SymAddr_t   DwellAddress; /**< \brief  Dwell Address in #CFS_SymAddr_t format */
} MD_TableLoadEntry_t;

#define MD_TBL_LOAD_ENTRY_LNGTH sizeof(MD_TableLoadEntry_t)

/** 
**  \brief Memory Dwell Table Load structure
**
**  \par Description
**          To be valid, each of the Entry structures must be valid.  See #MD_TableLoadEntry_t for details.
**          Tables will be processed beginning with the first entry if it is non-null 
**          and continuing until the first null entry is reached.
**          Note that non-null entries may follow a terminator entry; however they will not be processed.
**
**          In order to be processed, all of the following must be true: 
**          # There are one or more non-null entries beginning with the first entry of the table.
**          # The sum of individual entry delays, beginning with the first entry and up until the 
**            terminator entry or the end of the table, must be non-zero.
**          # The table's Enabled field must be set to TRUE.  This is initially set in the load, and
**            is controlled with the #MD_START_DWELL_CC and #MD_STOP_DWELL_CC commands.
*/    
typedef struct
{
    uint32 Enabled; /**< \brief Is table is enabled for dwell?  Valid values are MD_DWELL_STREAM_DISABLED, MD_DWELL_STREAM_ENABLED */
#if MD_SIGNATURE_OPTION == 1  
    char Signature[MD_SIGNATURE_FIELD_LENGTH];
#endif
    MD_TableLoadEntry_t Entry[MD_DWELL_TABLE_SIZE];  /**< \brief Array of individual memory dwell specifications */
} MD_DwellTableLoad_t;

#define MD_TBL_LOAD_LNGTH        sizeof(MD_DwellTableLoad_t)



/*************************************************************************/

#endif /* _md_tbldefs_h_ */

/************************/
/*  End of File Comment */
/************************/
