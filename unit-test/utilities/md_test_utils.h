/************************************************************************
 * NASA Docket No. GSC-18,922-1, and identified as “Core Flight
 * System (cFS) Memory Dwell Application Version 2.4.0”
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
 *   This file contains the function prototypes and global variables for
 *   the unit test utilities for the MD application.
 */
#ifndef MD_TEST_UTILS_H
#define MD_TEST_UTILS_H

#include "md_app.h"
#include "md_platform_cfg.h"
#include "utstubs.h"
#include "cfe_msgids.h"

extern MD_AppData_t MD_AppData;

/*
 * Global context structures
 */
typedef struct
{
    uint16 EventID;
    uint16 EventType;
    char   Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
} CFE_EVS_SendEvent_context_t;

typedef struct
{
    char Spec[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
} CFE_ES_WriteToSysLog_context_t;

extern CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent[];
extern CFE_ES_WriteToSysLog_context_t context_CFE_ES_WriteToSysLog;

/* Command buffer typedef for any handler */
typedef union
{
    CFE_SB_Buffer_t   Buf;
    MD_NoArgsCmd_t    NoArgsCmd;
    MD_CmdStartStop_t CmdStartStop;
    MD_CmdJam_t       CmdJam;
#if MD_SIGNATURE_OPTION == 1
    MD_CmdSetSignature_t CmdSetSignature;
#endif
} UT_CmdBuf_t;

extern UT_CmdBuf_t UT_CmdBuf;

/* Unit test ids */
#define MD_UT_MID_1 CFE_SB_ValueToMsgId(CFE_PLATFORM_TLM_MID_BASE + 1)

/*
 * Function Definitions
 */

void MD_Test_Setup(void);
void MD_Test_TearDown(void);

#endif
