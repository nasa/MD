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

#ifndef DEFAULT_MD_MSGSTRUCT_H
#define DEFAULT_MD_MSGSTRUCT_H

/* ======== */
/* Includes */
/* ======== */

#include "md_interface_cfg.h"
#include "md_msgdefs.h"
#include "cfe.h"

/**
 *  \brief Memory Dwell HK Telemetry format
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    MD_HkTlm_Payload_t        Payload;
} MD_HkTlm_t;

/**
 * \brief Start Dwell Command
 *
 * For command details, see #MD_START_DWELL_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   Header; /**< \brief Command header */
    MD_CmdStartStop_Payload_t Payload;
} MD_StartDwellCmd_t;

/**
 * \brief Stop Dwell Command
 *
 * For command details, see #MD_STOP_DWELL_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t   Header; /**< \brief Command header */
    MD_CmdStartStop_Payload_t Payload;
} MD_StopDwellCmd_t;

/**
 *  \brief Memory Dwell Telemetry Packet format
 */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    MD_DwellPkt_Payload_t     Payload;
} MD_DwellPkt_t;

/**
 * \brief Jam Dwell Command
 *
 * For command details, see #MD_JAM_DWELL_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t Header; /**< \brief Command header */
    MD_CmdJam_Payload_t     Payload;
} MD_JamDwellCmd_t;

#if MD_INTERFACE_SIGNATURE_OPTION == 1

/**
 * \brief Set Signature Command
 *
 * For command details, see #MD_SET_SIGNATURE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t      Header; /**< \brief Command Header */
    MD_CmdSetSignature_Payload_t Payload;
} MD_SetSignatureCmd_t;

#endif

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< Command Header */
} MD_NoopCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< Command Header */
} MD_ResetCountersCmd_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< Command Header */
} MD_Wakeup_t;

typedef struct
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< Command Header */
} MD_SendHkCmd_t;

#endif /* DEFAULT_MD_MSGSTRUCT_H */