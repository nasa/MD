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
 *   Implementation of MD dispatch functions
 */

/*
** Include Files:
*/
#include "md_app.h"
#include "md_dispatch.h"
#include "md_cmds.h"
#include "md_dwell_pkt.h"
#include "md_eventids.h"
#include "md_msgids.h"
#include "md_msg.h"
#include "md_fcncodes.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

bool MD_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              IsValidLength;
    size_t            ActualLength;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t FcnCode;

    IsValidLength = true;
    ActualLength  = 0;
    MsgId         = CFE_SB_INVALID_MSG_ID;
    FcnCode       = 0;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(MD_CMD_LEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid Msg length: ID = 0x%X, CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                          (unsigned int)FcnCode,
                          (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        IsValidLength = false;
        MD_AppData.ErrCounter++;
    }

    return IsValidLength;
}

void MD_ProcessGroundCommand(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_MSG_FcnCode_t CommandCode = 0;

    CFE_MSG_GetFcnCode(&BufPtr->Msg, &CommandCode);
    switch (CommandCode)
    {
        case MD_NOOP_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_NoopCmd_t)))
            {
                MD_NoopCmd((MD_NoopCmd_t *)BufPtr);
            }
            break;
        case MD_RESET_CNTRS_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_ResetCountersCmd_t)))
            {
                MD_ResetCountersCmd((MD_ResetCountersCmd_t *)BufPtr);
            }
            break;
        case MD_START_DWELL_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_StartDwellCmd_t)))
            {
                MD_StartDwellCmd((MD_StartDwellCmd_t *)BufPtr);
            }
            break;
        case MD_STOP_DWELL_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_StopDwellCmd_t)))
            {
                MD_StopDwellCmd((MD_StopDwellCmd_t *)BufPtr);
            }
            break;
        case MD_JAM_DWELL_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_JamDwellCmd_t)))
            {
                MD_JamDwellCmd((MD_JamDwellCmd_t *)BufPtr);
            }
            break;

#if MD_INTERFACE_SIGNATURE_OPTION == 1
        case MD_SET_SIGNATURE_CC:
            if (MD_VerifyCmdLength(&BufPtr->Msg, sizeof(MD_SetSignatureCmd_t)))
            {
                MD_SetSignatureCmd((MD_SetSignatureCmd_t *)BufPtr);
            }
            break;
#endif

        default:
            MD_AppData.ErrCounter++;
            CFE_EVS_SendEvent(MD_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code %d", CommandCode);
            break;
    }
}

void MD_TaskPipe(const CFE_SB_Buffer_t *BufPtr)
{
    static CFE_SB_MsgId_t CMD_MID     = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SEND_HK_MID = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t WAKE_UP_MID = CFE_SB_MSGID_RESERVED;

    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    /* cache the local MID Values here, this avoids repeat lookups */
    if (!CFE_SB_IsValidMsgId(CMD_MID))
    {
        CMD_MID     = CFE_SB_ValueToMsgId(MD_CMD_MID);
        SEND_HK_MID = CFE_SB_ValueToMsgId(MD_SEND_HK_MID);
        WAKE_UP_MID = CFE_SB_ValueToMsgId(MD_WAKEUP_MID);
    }

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MsgId);

    /* Process all SB messages */
    if (CFE_SB_MsgId_Equal(MsgId, WAKE_UP_MID))
    {
        /* Periodic Wakeup */
        (void)MD_DwellLoop((const MD_Wakeup_t *)BufPtr);
    }
    else if (CFE_SB_MsgId_Equal(MsgId, SEND_HK_MID))
    {
        /* Housekeeping request */
        (void)MD_SendHkCmd((const MD_SendHkCmd_t *)BufPtr);
    }
    else if (CFE_SB_MsgId_Equal(MsgId, CMD_MID))
    {
        /* Ground command */
        MD_ProcessGroundCommand(BufPtr);
    }
    else
    {
        MD_AppData.ErrCounter++;
        CFE_EVS_SendEvent(MD_MID_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid command pipe message ID: 0x%08lX",
                          (unsigned long)CFE_SB_MsgIdToValue(MsgId));
    }
}
