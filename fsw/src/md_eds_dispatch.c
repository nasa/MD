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
 *   CFS Memory Dwell Application EDS-based message dispatch functions
 */

/*
** Include Files:
*/
#include "md_app.h"
#include "md_dispatch.h"
#include "md_cmds.h"
#include "md_eventids.h"
#include "md_extern_typedefs.h"
#include "md_dwell_pkt.h"
#include "md_msgstruct.h"

#include "md_eds_dispatcher.h"
#include "md_eds_dictionary.h"

/*
 * Define a lookup table for MD app command codes
 */
/* clang-format off */
static const EdsDispatchTable_EdsComponent_MD_Application_CFE_SB_Telecommand_t MD_TC_DISPATCH_TABLE = 
{
    .CMD =
    {
        .NoopCmd_indication          = MD_NoopCmd,
        .ResetCountersCmd_indication = MD_ResetCountersCmd,
        .StartDwellCmd_indication    = MD_StartDwellCmd,
        .StopDwellCmd_indication     = MD_StopDwellCmd,
        .JamDwellCmd_indication      = MD_JamDwellCmd,
#if MD_INTERFACE_SIGNATURE_OPTION == 1
        .SetSignatureCmd_indication  = MD_SetSignatureCmd
#endif
    },
    .SEND_HK =
    {
        .indication = MD_HkStatus
    },
    .WAKEUP =
    {
        .indication = MD_DwellLoop
    }
};
/* clang-format on */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the MD        */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void MD_ProcessCommandPacket(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_Status_t      Status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    Status = EdsDispatch_EdsComponent_MD_Application_Telecommand(BufPtr, &MD_TC_DISPATCH_TABLE);

    if (Status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&BufPtr->Msg, &MsgId);
        CFE_MSG_GetSize(&BufPtr->Msg, &MsgSize);
        CFE_MSG_GetFcnCode(&BufPtr->Msg, &MsgFc);
        ++MD_AppData.ErrCounter;

        if (Status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(MD_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MD: invalid command packet,MID = 0x%x", (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        }
        else if (Status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_EVS_SendEvent(MD_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid Msg length: ID = 0x%X,  CC = %u, Len = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)MsgFc, (unsigned int)MsgSize);
        }
        else
        {
            CFE_EVS_SendEvent(MD_CC_NOT_IN_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                              "MD: Invalid ground command code: CC = %d", (int)MsgFc);
        }
    }
}