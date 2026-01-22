/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   Data structure definitions for Memory Dwell table loads.
 */
#ifndef DEFAULT_MD_TBLDEFS_H
#define DEFAULT_MD_TBLDEFS_H

/* ======== */
/* Includes */
/* ======== */

#include "cfe.h"
#include "md_extern_typedefs.h"

/************************************************************************
 * Type Definitions
 ************************************************************************/

/**
 *  \brief Memory Dwell structure for individual memory dwell specifications
 *
 *  \par Description
 *    To be valid, entry must either be a null entry
 *    (specified by a zero field length) or all of its
 *    address and length fields must pass various checks.
 *
 *    The following checks must be met for a valid non-null entry:
 *
 *    - Length fields must be 1, 2, or 4.
 *    - Delay can be any value in a uint16 field.
 *    - DwellAddress.SymName must be a null string or must be found in the Symbol Table.
 *    - The sum of DwellAddress.SymName plus DwellAddress.Offset must be in a valid range,
 *      as determined by OSAL routines.
 *    - If the Length field is 2, the sum of DwellAddress.SymName plus DwellAddress.Offset
 *      must be even.
 *    - If the Length field is 4, the sum of DwellAddress.SymName plus DwellAddress.Offset
 *      must be evenly divisable by 4.
 */
typedef struct
{
    uint16       Length;       /**< \brief Length of dwell field in bytes. 0 indicates null entry. */
    uint16       Delay;        /**< \brief Delay before following dwell sample in number of task wakeup calls */
    MD_SymAddr_t DwellAddress; /**< \brief Dwell Address in #MD_SymAddr_t format */
} MD_TableLoadEntry_t;

#endif /* DEFAULT_MD_TBLDEFS_H */