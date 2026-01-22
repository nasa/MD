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
 *   Specification for the CFS Memory Dwell command and telemetry
 *   message constant definitions.
 *
 */

#ifndef DEFAULT_MD_MSGDEFS_H
#define DEFAULT_MD_MSGDEFS_H

/* ======== */
/* Includes */
/* ======== */

#include "common_types.h"
#include "md_fcncodes.h"
#include "md_interface_cfg.h"
#include "cfe.h"
#include "md_extern_typedefs.h"

/* ======== */
/* Typedefs */
/* ======== */

/**
 * \defgroup cfsmdcmdstructs CFS Memory Dwell Command Structures
 * \{
 */

/**
 * \brief Start and Stop Dwell Commands Payload
 */
typedef struct
{
    uint16 TableMask; /**< \brief 0x0001=TBL1 bit, 0x0002=TBL2 bit, 0x0004=TBL3 bit, etc. */
    uint16 Padding;   /**< \brief structure padding */
} MD_CmdStartStop_Payload_t;

typedef struct
{
    uint8  TableId;   /**< \brief TableId from 1 to #MD_INTERFACE_NUM_DWELL_TABLES */
    uint8  AddrCount; /**< \brief Number of addresses being sent - 1..#MD_INTERFACE_DWELL_TABLE_SIZE valid */
    uint16 ByteCount; /**< \brief Number of bytes of dwell data contained in packet */
    uint32 Rate;      /**< \brief Number of counts between packet sends*/

#if MD_INTERFACE_SIGNATURE_OPTION == 1
    char Signature[MD_INTERFACE_SIGNATURE_FIELD_LENGTH]; /**< \brief Signature */
#endif

    uint8 Data[MD_INTERFACE_DWELL_TABLE_SIZE * 4]; /**< \brief Dwell data (can be variable size based on dfn) */
} MD_DwellPkt_Payload_t;

/**
 * \brief Jam Dwell Command Payload
 */
typedef struct
{
    uint16       TableId;      /**< \brief Table Id: 1..#MD_INTERFACE_NUM_DWELL_TABLES */
    uint16       EntryId;      /**< \brief Address index: 1..#MD_INTERFACE_DWELL_TABLE_SIZE  */
    uint16       FieldLength;  /**< \brief Length of Dwell Field : 0, 1, 2, or 4  */
    uint16       DwellDelay;   /**< \brief Dwell Delay (number of task wakeup calls before following dwell) */
    MD_SymAddr_t DwellAddress; /**< \brief Dwell Address in #MD_SymAddr_t format */
} MD_CmdJam_Payload_t;

#if MD_INTERFACE_SIGNATURE_OPTION == 1

/**
 * \brief Set Signature Command Payload
 */
typedef struct
{
    uint16 TableId;                              /**< \brief Table Id: 1..MD_INTERFACE_NUM_DWELL_TABLES */
    uint16 Padding;                              /**< \brief Padding  */
    char   Signature[MD_INTERFACE_SIGNATURE_FIELD_LENGTH]; /**< \brief Sigature */
} MD_CmdSetSignature_Payload_t;

#endif

/**
 *  \brief Memory Dwell HK Telemetry Payload
 */
typedef struct
{
    uint8  InvalidCmdCntr;   /**< \brief Count of invalid commands received */
    uint8  ValidCmdCntr;     /**< \brief Count of valid commands received */
    uint16 DwellEnabledMask; /**< \brief Dwell table enable bitmask 0x0001=TBL1, etc */

    uint16 DwellTblAddrCount[MD_INTERFACE_NUM_DWELL_TABLES]; /**< \brief Number of dwell addresses in table */
    uint16 NumWaitsPerPkt[MD_INTERFACE_NUM_DWELL_TABLES];    /**< \brief Number of delay counts in table */
    uint16 ByteCount[MD_INTERFACE_NUM_DWELL_TABLES];         /**< \brief Number of bytes of data specified by table */
    uint16 DwellPktOffset[MD_INTERFACE_NUM_DWELL_TABLES];    /**< \brief Current write offset within dwell pkt data region */
    uint16 DwellTblEntry[MD_INTERFACE_NUM_DWELL_TABLES];     /**< \brief Next dwell table entry to be processed  */

    uint16 Countdown[MD_INTERFACE_NUM_DWELL_TABLES]; /**< \brief Current value of countdown timer  */
} MD_HkTlm_Payload_t;

/**\}*/

#endif /* DEFAULT_MD_MSGDEFS_H */
