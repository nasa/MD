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
 *   Specification for the CFS Memory Dwell command and telemetry
 *   messages.
 */
#ifndef MD_MSG_H
#define MD_MSG_H

#include <md_platform_cfg.h>
#include <cfe.h>
#include <md_msgdefs.h>

/************************************************************************
 * Type Definitions
 ************************************************************************/

/**
 * \defgroup cfsmdcmdstructs CFS Memory Dwell Command Structures
 * \{
 */

/**
 *  \brief Symbolic Address Type
 */
typedef struct
{
    cpuaddr Offset;               /**< \brief Optional offset that is used as the
                                     absolute address if the SymName string is NUL */
    char SymName[OS_MAX_SYM_LEN]; /**< \brief Symbol name string  */
} MD_SymAddr_t;

/**
 * \brief Generic "no arguments" command
 *
 * This command structure is used for commands that do not have any parameters.
 * This includes:
 * -# The Housekeeping Request Message
 * -# The Wakeup Message
 * -# The No-Op Command (For details, see #MD_NOOP_CC)
 * -# The Reset Counters Command (For details, see #MD_RESET_CNTRS_CC)
 */
typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< Command Header */
} MD_NoArgsCmd_t;

/**
 * \brief Start and Stop Dwell Commands
 *
 * For command details, see #MD_START_DWELL_CC and #MD_STOP_DWELL_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header; /**< \brief Command header */

    uint16 TableMask; /**< \brief 0x0001=TBL1 bit, 0x0002=TBL2 bit, 0x0004=TBL3 bit, etc. */
    uint16 Padding;   /**< \brief structure padding */
} MD_CmdStartStop_t;

/**
 * \brief Jam Dwell Command
 *
 * For command details, see #MD_JAM_DWELL_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header; /**< \brief Command header */

    uint16       TableId;      /**< \brief Table Id: 1..#MD_NUM_DWELL_TABLES */
    uint16       EntryId;      /**< \brief Address index: 1..#MD_DWELL_TABLE_SIZE  */
    uint16       FieldLength;  /**< \brief Length of Dwell Field : 0, 1, 2, or 4  */
    uint16       DwellDelay;   /**< \brief Dwell Delay (number of task wakeup calls before following dwell) */
    MD_SymAddr_t DwellAddress; /**< \brief Dwell Address in #MD_SymAddr_t format */
} MD_CmdJam_t;

#if MD_SIGNATURE_OPTION == 1

/**
 * \brief Set Signature Command
 *
 * For command details, see #MD_SET_SIGNATURE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header; /**< \brief Command Header */

    uint16 TableId;                              /**< \brief Table Id: 1..MD_NUM_DWELL_TABLES */
    uint16 Padding;                              /**< \brief Padding  */
    char   Signature[MD_SIGNATURE_FIELD_LENGTH]; /**< \brief Sigature */
} MD_CmdSetSignature_t;

#endif

/**\}*/

/**
 * \defgroup cfsmdtlm CFS Memory Dwell Telemetry
 * \{
 */

/**
 *  \brief Memory Dwell HK Telemetry format
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader; /**< \brief Telemetry header */

    uint8  InvalidCmdCntr;   /**< \brief Count of invalid commands received */
    uint8  ValidCmdCntr;     /**< \brief Count of valid commands received */
    uint16 DwellEnabledMask; /**< \brief Dwell table enable bitmask 0x0001=TBL1, etc */

    uint16 DwellTblAddrCount[MD_NUM_DWELL_TABLES]; /**< \brief Number of dwell addresses in table */
    uint16 NumWaitsPerPkt[MD_NUM_DWELL_TABLES];    /**< \brief Number of delay counts in table */
    uint16 ByteCount[MD_NUM_DWELL_TABLES];         /**< \brief Number of bytes of data specified by table */
    uint16 DwellPktOffset[MD_NUM_DWELL_TABLES];    /**< \brief Current write offset within dwell pkt data region */
    uint16 DwellTblEntry[MD_NUM_DWELL_TABLES];     /**< \brief Next dwell table entry to be processed  */

    uint16 Countdown[MD_NUM_DWELL_TABLES]; /**< \brief Current value of countdown timer  */
} MD_HkTlm_t;

/**
 * \brief Memory Dwell Housekeeping telemetry packet length
 */
#define MD_HK_TLM_LNGTH sizeof(MD_HkTlm_t)

/**
 *  \brief Memory Dwell Telemetry Packet format
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TlmHeader; /**< \brief Telemetry header */

    uint8  TableId;   /**< \brief TableId from 1 to #MD_NUM_DWELL_TABLES */
    uint8  AddrCount; /**< \brief Number of addresses being sent - 1..#MD_DWELL_TABLE_SIZE valid */
    uint16 ByteCount; /**< \brief Number of bytes of dwell data contained in packet */
    uint32 Rate;      /**< \brief Number of counts between packet sends*/

#if MD_SIGNATURE_OPTION == 1
    char Signature[MD_SIGNATURE_FIELD_LENGTH]; /**< \brief Signature */
#endif

    uint8 Data[MD_DWELL_TABLE_SIZE * 4]; /**< \brief Dwell data (can be variable size based on dfn) */
} MD_DwellPkt_t;

/**
 * \brief Memory Dwell Telemetry maximum packet size
 */
#define MD_DWELL_PKT_LNGTH (sizeof(MD_DwellPkt_t))

/**\}*/

#endif
