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
 *   This file is maintained for backward compatibility.
 *   All table structures are now defined in md_tbldefs.h
 */
#ifndef DEFAULT_MD_TBLSTRUCT_H
#define DEFAULT_MD_TBLSTRUCT_H

#include "md_tbldefs.h"
#include "md_interface_cfg.h"
#include "md_extern_typedefs.h"

/**
 *  \brief Memory Dwell Table Load structure
 *
 *  \par Description
 *    To be valid, each of the Entry structures must be valid.  See #MD_TableLoadEntry_t for details.
 *    Tables will be processed beginning with the first entry if it is non-null
 *    and continuing until the first null entry is reached.
 *    Note that non-null entries may follow a terminator entry; however they will not be processed.
 *
 *    In order to be processed, all of the following must be true:
 *    - There are one or more non-null entries beginning with the first entry of the table.
 *    - The sum of individual entry delays, beginning with the first entry and up until the
 *      terminator entry or the end of the table, must be non-zero.
 *    - The table's Enabled field must be set to TRUE.  This is initially set in the load, and
 *      is controlled with the #MD_START_DWELL_CC and #MD_STOP_DWELL_CC commands.
 */
typedef struct
{
    MD_Dwell_States Enabled; /**< \brief Table enable flag: #MD_Dwell_States_DISABLED, #MD_Dwell_States_ENABLED */

#if MD_INTERFACE_SIGNATURE_OPTION == 1
    char Signature[MD_INTERFACE_SIGNATURE_FIELD_LENGTH]; /**< \brief Signature */
#endif

    MD_TableLoadEntry_t Entry[MD_INTERFACE_DWELL_TABLE_SIZE]; /**< \brief Array of individual memory dwell entries */
} MD_DwellTableLoad_t;

#endif /* DEFAULT_MD_TBLSTRUCT_H */
