/************************************************************************
 * NASA Docket No. GSC-18,922-1, and identified as “Core Flight
 * System (cFS) Memory Dwell Application Version 2.4.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
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
 *   Specification for functions used to populate and send Memory Dwell packets.
 */
#ifndef MD_DWELL_PKT_H
#define MD_DWELL_PKT_H

/************************************************************************
 * Includes
 ************************************************************************/

#include "common_types.h"

/************************************************************************
 * Type Definitions
 ************************************************************************/

/**
 *  \brief  MD enum used for representing error types of dwell address read
 */
enum MD_DwellAddrReadError
{
    /**
     * @brief CFE_PSP_MemRead8 error
     */
    ONE_BYTE_MEM_ADDR_READ_ERR = -1,

    /**
     * @brief CFE_PSP_MemRead16 error
     */
    TWO_BYTE_MEM_ADDR_READ_ERR = -2,

    /**
     * @brief CFE_PSP_MemRead32 error
     */
    FOUR_BYTE_MEM_ADDR_READ_ERR = -3,

    /**
     * @brief Default case - Invalid Dwell Address length
     */
    INVALID_DWELL_ADDR_LEN = -4
};

/************************************************************************
 * Exported Functions
 ************************************************************************/

/**
 * \brief Process Dwell Packets
 *
 * \par Description
 *      Look at each table.  If the table is enabled and
 *      its countdown counter times out, add dwell data to dwell packet until a
 *      packet delay is reached or the dwell packet is completed.
 *      If dwell packet is completed, send the packet and reset the counter.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 */
void MD_DwellLoop(void);

/**
 * \brief Read a Single Dwell Address
 *
 * \par Description
 *          Copy memory value from a single dwell address to a dwell packet.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] TblIndex identifies source dwell table (0..)
 * \param[in] EntryIndex identifies dwell entry within specified table (0..)
 *
 * \return Execution status, non-zero on failure
 * \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
int32 MD_GetDwellData(uint16 TblIndex, uint16 EntryIndex);

/**
 * \brief Send Memory Dwell Packet
 *
 * \par Description
 *          Send contents of memory pointed to by a table as a
 *          telemetry packet.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] TableIndex Identifies source dwell table for the dwell packet.
 */
void MD_SendDwellPkt(uint16 TableIndex);

/**
 * \brief Start Dwell Stream
 *
 * \par Description
 *          Initialize parameters used to control generation of dwell packets.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] TableIndex Identifies source dwell table for the dwell packet.
 */
void MD_StartDwellStream(uint16 TableIndex);

#endif
