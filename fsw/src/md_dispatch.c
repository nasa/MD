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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* Verify command packet length                                               */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

const MD_CmdHandlerTblRec_t MD_CmdHandlerTbl[] = {
    /* Message ID, Command Code, Msg Size */
    {MD_NOOP_CC, sizeof(MD_NoopCmd_t)},
    {MD_RESET_CNTRS_CC, sizeof(MD_ResetCountersCmd_t)},
    {MD_START_DWELL_CC, sizeof(MD_StartDwellCmd_t)},
    {MD_STOP_DWELL_CC, sizeof(MD_StopDwellCmd_t)},
    {MD_JAM_DWELL_CC, sizeof(MD_JamDwellCmd_t)},
#if MD_INTERFACE_SIGNATURE_OPTION == 1
    {MD_SET_SIGNATURE_CC, sizeof(MD_SetSignatureCmd_t)},
#endif
};

/******************************************************************************/
void MD_ExecRequest(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_MSG_FcnCode_t CommandCode  = 0;
    CFE_SB_MsgId_t    MessageID    = CFE_SB_INVALID_MSG_ID;
    int16             CmdIndx      = 0;
    size_t            ActualLength = 0;

    /* Extract message info */
    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);
    CFE_MSG_GetFcnCode(&BufPtr->Msg, &CommandCode);
    CFE_MSG_GetSize(&BufPtr->Msg, &ActualLength);

    /* Find index which will be used to access ExpectedLength data */
    CmdIndx = MD_SearchCmdHndlrTbl(CommandCode);

    if (CmdIndx < 0)
    {
        /*
         * If match wasn't found in command info structure,
         * issue an error event, increment error counter.
         */
        CFE_EVS_SendEvent(MD_CC_NOT_IN_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Command Code %d not found in MD_CmdHandlerTbl structure", CommandCode);
        MD_AppData.ErrCounter++;
    }
    else if (ActualLength != MD_CmdHandlerTbl[CmdIndx].ExpectedLength)
    {
        /*
         * If Command  Length is inconsistent with command type,
         * send error event, increment error count.
         */
        CFE_EVS_SendEvent(MD_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Cmd Msg with Bad length Rcvd: ID = 0x%08lX, CC = %d, Exp Len = %d, Len = %d",
                          (unsigned long)CFE_SB_MsgIdToValue(MessageID), CommandCode,
                          (int)MD_CmdHandlerTbl[CmdIndx].ExpectedLength, (int)ActualLength);

        MD_AppData.ErrCounter++;
    }
    else
    {
        /* Process command */
        switch (CommandCode)
        {
            case MD_NOOP_CC:
            default: /* For coverage, invalid already checked by MD_SearchCmdHndlrTbl */
                MD_NoopCmd((const MD_NoopCmd_t *) BufPtr);
                break;

            case MD_RESET_CNTRS_CC:

                MD_ResetCountersCmd((const MD_ResetCountersCmd_t *) BufPtr);
                break;

            case MD_START_DWELL_CC:

                MD_StartDwellCmd((const MD_StartDwellCmd_t *) BufPtr);
                break;

            case MD_STOP_DWELL_CC: /* Params: byte:table_ID */

                MD_StopDwellCmd((const MD_StopDwellCmd_t *) BufPtr);
                break;

            case MD_JAM_DWELL_CC: /* Params: table_ID, AddrIndex, address, */
                                  /* FieldLength, DwellDelay */
                MD_JamDwellCmd((const MD_JamDwellCmd_t *) BufPtr);
                break;

#if MD_INTERFACE_SIGNATURE_OPTION == 1
            case MD_SET_SIGNATURE_CC:
                MD_SetSignatureCmd((const MD_SetSignatureCmd_t *) BufPtr);
                break;
#endif
        } /* End Switch */
    }
}

/******************************************************************************/

void MD_ProcessCommandPacket(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t MessageID    = CFE_SB_INVALID_MSG_ID;
    size_t         ActualLength = 0;

    /* Process Executive Request */
    if (BufPtr != NULL)
    {
        CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

        switch (CFE_SB_MsgIdToValue(MessageID))
        {
            case MD_WAKEUP_MID:
                CFE_MSG_GetSize(&BufPtr->Msg, &ActualLength);
                if (ActualLength != sizeof(MD_Wakeup_t))
                {
                    CFE_EVS_SendEvent(MD_MSG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Msg with Bad length Rcvd: ID = 0x%08lX, Exp Len = %u, Len = %d",
                                      (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                                      (unsigned int)sizeof(MD_Wakeup_t), (int)ActualLength);
                }
                else
                {
                    MD_DwellLoop((const MD_Wakeup_t *) BufPtr);
                }
                break;

            case MD_CMD_MID:

                MD_ExecRequest(BufPtr);
                break;

            case MD_SEND_HK_MID:
                CFE_MSG_GetSize(&BufPtr->Msg, &ActualLength);
                if (ActualLength != sizeof(MD_SendHkCmd_t))
                {
                    CFE_EVS_SendEvent(MD_MSG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Msg with Bad length Rcvd: ID = 0x%08lX, Exp Len = %u, Len = %d",
                                      (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                                      (unsigned int)sizeof(MD_SendHkCmd_t), (int)ActualLength);
                }
                else
                {
                    MD_HkStatus((const MD_SendHkCmd_t *) BufPtr);
                }
                break;

            default:
                CFE_EVS_SendEvent(MD_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Msg with Invalid message ID Rcvd -- ID = 0x%08lX",
                                  (unsigned long)CFE_SB_MsgIdToValue(MessageID));
                break;
        }
    }
}

int16 MD_SearchCmdHndlrTbl(CFE_MSG_FcnCode_t CommandCode)
{
    int16 TblIndx = MD_BAD_CMD_CODE;
    int16 i       = 0;

    while (TblIndx == MD_BAD_CMD_CODE && i < sizeof(MD_CmdHandlerTbl) / sizeof(MD_CmdHandlerTbl[0]))
    {
        if (MD_CmdHandlerTbl[i].CmdCode == CommandCode)
        {
            TblIndx = i;
        }
        i++;
    }

    return TblIndx;
}