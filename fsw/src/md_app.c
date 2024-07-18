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
 *   CFS Memory Dwell Application top-level procedures.
 */

#include "cfe.h"
#include "md_app.h"
#include "md_events.h"
#include "md_tbldefs.h"
#include "md_verify.h"
#include "md_cmds.h"
#include <string.h>
#include "md_dwell_tbl.h"
#include "md_dwell_pkt.h"
#include "md_utils.h"
#include "md_perfids.h"
#include "md_version.h"
#include "md_extern_typedefs.h"

/* Constant Data */
const MD_CmdHandlerTblRec_t MD_CmdHandlerTbl[] = {
    /* Message ID, Command Code, Msg Size */
    {MD_NOOP_CC, sizeof(MD_NoArgsCmd_t)},
    {MD_RESET_CNTRS_CC, sizeof(MD_NoArgsCmd_t)},
    {MD_START_DWELL_CC, sizeof(MD_CmdStartStop_t)},
    {MD_STOP_DWELL_CC, sizeof(MD_CmdStartStop_t)},
    {MD_JAM_DWELL_CC, sizeof(MD_CmdJam_t)},
#if MD_SIGNATURE_OPTION == 1
    {MD_SET_SIGNATURE_CC, sizeof(MD_CmdSetSignature_t)},
#endif
};

MD_AppData_t MD_AppData;

void MD_AppMain(void)
{
    CFE_SB_MsgId_t   MessageID    = CFE_SB_INVALID_MSG_ID;
    CFE_Status_t     Status       = CFE_SUCCESS;
    uint8            TblIndex     = 0;
    CFE_SB_Buffer_t *BufPtr       = NULL;
    size_t           ActualLength = 0;

    /* Create the first Performance Log entry */
    CFE_ES_PerfLogEntry(MD_APPMAIN_PERF_ID);

    /* Initialize the task */
    Status = MD_AppInit();

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("MD:Application Init Failed,RC=%08x\n", (unsigned int)Status);
        MD_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    /* Loop Forever, or until ES exit command, or SB error  */
    while (CFE_ES_RunLoop(&MD_AppData.RunStatus) == true)
    {
        /* Copy any newly loaded tables */
        for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
        {
            MD_ManageDwellTable(TblIndex);

        } /* end for each table loop */

        CFE_ES_PerfLogExit(MD_APPMAIN_PERF_ID);

        /* Wait for next message */
        Status = CFE_SB_ReceiveBuffer(&BufPtr, MD_AppData.CmdPipe, MD_SB_TIMEOUT);
        if (Status == CFE_SB_TIME_OUT)
        {
            /* No action, but also no error */
        }
        else if (Status != CFE_SUCCESS)
        {
            /*
            ** Exit on pipe read error
            */
            CFE_EVS_SendEvent(MD_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "SB Pipe Read Error, App will exit. Pipe Return Status = %08x", (unsigned int)Status);

            MD_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

        CFE_ES_PerfLogEntry(MD_APPMAIN_PERF_ID);

        /* Process Executive Request */
        if ((Status == CFE_SUCCESS) && (BufPtr != NULL))
        {
            CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

            switch (CFE_SB_MsgIdToValue(MessageID))
            {
                case MD_WAKEUP_MID:
                    CFE_MSG_GetSize(&BufPtr->Msg, &ActualLength);
                    if (ActualLength != sizeof(MD_NoArgsCmd_t))
                    {
                        CFE_EVS_SendEvent(MD_MSG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                          "Msg with Bad length Rcvd: ID = 0x%08lX, Exp Len = %u, Len = %d",
                                          (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                                          (unsigned int)sizeof(MD_NoArgsCmd_t), (int)ActualLength);
                    }
                    else
                    {
                        MD_DwellLoop();
                    }
                    break;

                case MD_CMD_MID:

                    MD_ExecRequest(BufPtr);
                    break;

                case MD_SEND_HK_MID:
                    CFE_MSG_GetSize(&BufPtr->Msg, &ActualLength);
                    if (ActualLength != sizeof(MD_NoArgsCmd_t))
                    {
                        CFE_EVS_SendEvent(MD_MSG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                                          "Msg with Bad length Rcvd: ID = 0x%08lX, Exp Len = %u, Len = %d",
                                          (unsigned long)CFE_SB_MsgIdToValue(MessageID),
                                          (unsigned int)sizeof(MD_NoArgsCmd_t), (int)ActualLength);
                    }
                    else
                    {
                        MD_HkStatus();
                    }
                    break;

                default:
                    CFE_EVS_SendEvent(MD_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Msg with Invalid message ID Rcvd -- ID = 0x%08lX",
                                      (unsigned long)CFE_SB_MsgIdToValue(MessageID));
                    break;
            }
        }

    } /* end while forever loop */

    /*
    ** Performance Log Exit Stamp.
    */
    CFE_ES_PerfLogExit(MD_APPMAIN_PERF_ID);

    /*
    ** Exit the Application
    */
    CFE_ES_ExitApp(MD_AppData.RunStatus);
}

/******************************************************************************/

CFE_Status_t MD_AppInit(void)
{
    /*
    ** Locals
    */
    CFE_Status_t Status = CFE_SUCCESS;

    /* Zero out the global data structure */
    memset(&MD_AppData, 0, sizeof(MD_AppData));

    MD_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    /* Initialize local control structures */
    MD_InitControlStructures();

    /* Register for event services...*/
    Status = CFE_EVS_Register(NULL, 0, CFE_EVS_NO_FILTER);

    if (Status == CFE_SUCCESS)
    {
        /*
        ** Set up for Software Bus Services
        */
        Status = MD_InitSoftwareBusServices();
    }
    else
    {
        CFE_ES_WriteToSysLog("MD_APP:Call to CFE_EVS_Register Failed:RC=%d\n", (unsigned int)Status);
    }

    /*
    ** Register for Table Services
    */
    if (Status == CFE_SUCCESS)
    {
        Status = MD_InitTableServices();
    }

    /*
    ** Issue Event Message
    */
    if (Status == CFE_SUCCESS)
    {
        Status =
            CFE_EVS_SendEvent(MD_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "MD Initialized.  Version %d.%d.%d.%d",
                              MD_MAJOR_VERSION, MD_MINOR_VERSION, MD_REVISION, MD_MISSION_REV);
    }

    return Status;
}

/******************************************************************************/
void MD_InitControlStructures(void)
{
    uint16                   TblIndex;
    MD_DwellPacketControl_t *DwellControlPtr = NULL;

    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
    {
        DwellControlPtr = &MD_AppData.MD_DwellTables[TblIndex];

        memset(DwellControlPtr, 0, sizeof(*DwellControlPtr));

        /* Countdown must be set to 1 since it's decremented at the top of */
        /* the dwell loop. */
        DwellControlPtr->Countdown = 1;

#if MD_SIGNATURE_OPTION == 1
        strncpy(DwellControlPtr->Signature, "", MD_SIGNATURE_FIELD_LENGTH);
#endif

    } /* end for TblIndex loop */
}

/******************************************************************************/
CFE_Status_t MD_InitSoftwareBusServices(void)
{
    CFE_Status_t Status = CFE_SUCCESS;
    uint16       TblIndex;

    /*
    ** Initialize housekeeping telemetry packet (clear user data area)
    */
    CFE_MSG_Init(&MD_AppData.HkPkt.TlmHeader.Msg, CFE_SB_ValueToMsgId(MD_HK_TLM_MID), MD_HK_TLM_LNGTH);
    /*
    ** Initialize dwell packets (clear user data area)
    */
    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
    {
        CFE_MSG_Init(&MD_AppData.MD_DwellPkt[TblIndex].TlmHeader.Msg,
                     CFE_SB_ValueToMsgId(MD_DWELL_PKT_MID_BASE + TblIndex),
                     MD_DWELL_PKT_LNGTH); /* this is max pkt size */

#if MD_SIGNATURE_OPTION == 1
        MD_AppData.MD_DwellPkt[TblIndex].Payload.Signature[0] = '\0';
#endif
    }

    /*
    **  Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&MD_AppData.CmdPipe, MD_PIPE_DEPTH, MD_PIPE_NAME);

    if (Status == CFE_SUCCESS)
    {
        /*
         ** Subscribe to Housekeeping request commands
         */
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MD_SEND_HK_MID), MD_AppData.CmdPipe);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(MD_SUB_HK_ERR_EID, CFE_EVS_EventType_ERROR, "Failed to subscribe to HK requests  RC = %d",
                              (unsigned int)Status);
        }
    }
    else
    {
        CFE_EVS_SendEvent(MD_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, "Failed to create pipe.  RC = %d",
                          (unsigned int)Status);
    }

    /*
    ** Subscribe to MD ground command packets
    */
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MD_CMD_MID), MD_AppData.CmdPipe);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(MD_SUB_CMD_ERR_EID, CFE_EVS_EventType_ERROR, "Failed to subscribe to commands.  RC = %d",
                              (unsigned int)Status);
        }
    }

    /*
    ** Subscribe to MD wakeup packets
    */
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MD_WAKEUP_MID), MD_AppData.CmdPipe);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(MD_SUB_WAKEUP_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to subscribe to wakeup messages.  RC = %08x", (unsigned int)Status);
        }
    }

    return Status;
}

/******************************************************************************/

CFE_Status_t MD_InitTableServices(void)
{
    CFE_Status_t         Status           = CFE_SUCCESS;
    int32                GetAddressResult = 0;
    uint8                TblIndex;
    bool                 RecoveredValidTable = true; /* for current table */
    bool                 TableInitValidFlag  = true; /* for all tables so far*/
    MD_DwellTableLoad_t *MD_LoadTablePtr     = NULL;
    uint16               TblRecos            = 0; /* Number of Tables Recovered */
    uint16               TblInits            = 0; /* Number of Tables Initialized */
    char                 TblFileName[OS_MAX_PATH_LEN];

    /*
    ** For each table, load recovered data if available and valid.
    ** Otherwise, load initial data.
    */

    for (TblIndex = 0; (TblIndex < MD_NUM_DWELL_TABLES) && (TableInitValidFlag == true); TblIndex++)
    {
        RecoveredValidTable = false;

        /* Prepare Table Name */
        Status = snprintf(MD_AppData.MD_TableName[TblIndex], sizeof(MD_AppData.MD_TableName[TblIndex]), "%s%d",
                          MD_DWELL_TABLE_BASENAME, TblIndex + 1);
        if (Status < 0)
        {
            CFE_EVS_SendEvent(MD_INIT_TBL_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TableName could not be made. Err=0x%08X, Idx=%u", (unsigned int)Status,
                              (unsigned int)TblIndex);

            TableInitValidFlag = false;

            /* Advance to the next index */
            continue;
        }

        /* allows total of CFE_TBL_MAX_NAME_LENGTH characters to be copied */
        Status = snprintf(TblFileName, OS_MAX_PATH_LEN, MD_TBL_FILENAME_FORMAT, TblIndex + 1);

        if (Status < 0)
        {
            CFE_EVS_SendEvent(MD_INIT_TBL_FILENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                              "TblFileName could not be made. Err=0x%08X, Idx=%u", (unsigned int)Status,
                              (unsigned int)TblIndex);

            TableInitValidFlag = false;

            /* Advance to the next index */
            continue;
        }

        /* Register Dwell Table #tblnum */
        Status = CFE_TBL_Register(&MD_AppData.MD_TableHandle[TblIndex], /* Table Handle (to be returned) */
                                  MD_AppData.MD_TableName[TblIndex],    /*   Table Name */
                                  sizeof(MD_DwellTableLoad_t),          /* Size of Table being Registered */
                                  CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL,
                                  &MD_TableValidationFunc); /* Ptr to table validation function */

        /* Check for recovered table data */
        if (Status == CFE_TBL_INFO_RECOVERED_TBL)
        {
            /* Restore dwell table contents from CDS */

            GetAddressResult = CFE_TBL_GetAddress((void *)&MD_LoadTablePtr, MD_AppData.MD_TableHandle[TblIndex]);

            if (GetAddressResult != CFE_TBL_INFO_UPDATED)
            {
                CFE_EVS_SendEvent(MD_NO_TBL_COPY_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Didn't update MD tbl #%d due to unexpected CFE_TBL_GetAddress return: 0x%08X",
                                  TblIndex + 1, (unsigned int)GetAddressResult);
            }

            else /* GetAddressResult == CFE_TBL_INFO_UPDATED*/
            {
                if (MD_TableValidationFunc((void *)MD_LoadTablePtr) == CFE_SUCCESS)
                {
                    MD_CopyUpdatedTbl(MD_LoadTablePtr, TblIndex);

                    if (MD_LoadTablePtr->Enabled == MD_DWELL_STREAM_ENABLED)
                    {
                        MD_StartDwellStream((uint16)TblIndex);
                    }

                    RecoveredValidTable = true;
                    TblRecos++;
                    CFE_EVS_SendEvent(MD_RECOVERED_TBL_VALID_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "Recovered Dwell Table #%d is valid and has been copied to the MD App",
                                      TblIndex + 1);
                }
                else
                {
                    CFE_EVS_SendEvent(MD_RECOVERED_TBL_NOT_VALID_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "MD App will reinitialize Dwell Table #%d because recovered table is not valid",
                                      TblIndex + 1);
                }

            } /* end if(GetAddressResult != CFE_TBL_INFO_UPDATED) */

            /* Unlock Table */
            CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TblIndex]);

        } /* end if Register indicates recovered data */

        else if (Status == CFE_TBL_ERR_INVALID_SIZE)
        {
            CFE_EVS_SendEvent(MD_DWELL_TBL_TOO_LARGE_CRIT_EID, CFE_EVS_EventType_CRITICAL,
                              "Dwell Table(s) are too large to register: %u bytes, %d entries",
                              (unsigned int)MD_TBL_LOAD_LNGTH, MD_DWELL_TABLE_SIZE);
            TableInitValidFlag = false;
        }

        else if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(MD_TBL_REGISTER_CRIT_EID, CFE_EVS_EventType_CRITICAL,
                              "CFE_TBL_Register error %08x received for tbl#%u", (unsigned int)Status,
                              (unsigned int)TblIndex + 1);
            TableInitValidFlag = false;
        }
        else
        {
            /* Table is registered and valid */
            TableInitValidFlag = true;
        }

        /*
        ** Load initial values if needed
        */
        if ((RecoveredValidTable == false) && (TableInitValidFlag == true))
        {
            Status =
                CFE_TBL_Load(MD_AppData.MD_TableHandle[TblIndex], CFE_TBL_SRC_FILE, /*  following ptr is memory ptr */
                             (const void *)TblFileName);                            /* Pointer to data to be loaded */

            MD_AppData.MD_DwellTables[TblIndex].Enabled = MD_DWELL_STREAM_DISABLED;

            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("MD_APP: Error 0x%08X received loading tbl#%u\n", (unsigned int)Status,
                                     (unsigned int)TblIndex + 1);
                TableInitValidFlag = false;
            }
            else
            {
                TblInits++;
            }
        }

        if (TableInitValidFlag == true)
        {
            /* Update Dwell Table Control Info, including rate */
            MD_UpdateDwellControlInfo(TblIndex);
        }

    } /* end for loop */

    /* Output init and recovery event message */
    CFE_EVS_SendEvent(MD_TBL_INIT_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "Dwell Tables Recovered: %u, Dwell Tables Initialized: %u", TblRecos, TblInits);

    if (TableInitValidFlag == true)
    {
        return CFE_SUCCESS;
    }
    else
    {
        return Status;
    }
}

/******************************************************************************/
CFE_Status_t MD_ManageDwellTable(uint8 TblIndex)
{
    CFE_Status_t         Status           = CFE_SUCCESS;
    int32                GetAddressResult = 0;
    bool                 FinishedManaging = false;
    MD_DwellTableLoad_t *MD_LoadTablePtr  = 0;

    while (!FinishedManaging)
    {
        /* Determine if the table has a pending validation or update */
        Status = CFE_TBL_GetStatus(MD_AppData.MD_TableHandle[TblIndex]);

        if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
        {
            /* Validate the specified Table */
            Status = CFE_TBL_Validate(MD_AppData.MD_TableHandle[TblIndex]);

            if (Status != CFE_SUCCESS)
            {
                /* If an error occurred during Validate, */
                /* then do not perform any more managing */
                FinishedManaging = true;
            }
        }
        else if (Status == CFE_TBL_INFO_UPDATE_PENDING)
        {
            Status = CFE_TBL_Update(MD_AppData.MD_TableHandle[TblIndex]);

            /* If table was updated, copy contents to local structure */
            if (Status == CFE_SUCCESS)
            {
                GetAddressResult = CFE_TBL_GetAddress((void *)&MD_LoadTablePtr, MD_AppData.MD_TableHandle[TblIndex]);

                if (GetAddressResult == CFE_TBL_INFO_UPDATED)
                {
                    MD_CopyUpdatedTbl(MD_LoadTablePtr, TblIndex);

                    if (MD_LoadTablePtr->Enabled == MD_DWELL_STREAM_ENABLED)
                    {
                        MD_StartDwellStream((uint16)TblIndex);
                    }
                }
                else
                {
                    CFE_EVS_SendEvent(MD_NO_TBL_COPY_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Didn't update MD tbl #%d due to unexpected CFE_TBL_GetAddress return: %u",
                                      (unsigned int)TblIndex + 1, (unsigned int)GetAddressResult);
                }

                /* Unlock Table */
                CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TblIndex]);
            }

            /* After an Update, always assume we are done */
            /* and return Update Status */
            FinishedManaging = true;
        }
        else if ((Status & CFE_SEVERITY_BITMASK) == CFE_SEVERITY_ERROR)
        {
            CFE_EVS_SendEvent(MD_TBL_STATUS_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Received unexpected error %08x from CFE_TBL_GetStatus for tbl #%u", (unsigned int)Status,
                              (unsigned int)TblIndex + 1);
            FinishedManaging = true;
        }

        else
        {
            FinishedManaging = true;
        }
    }

    return Status;
}

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

                CFE_EVS_SendEvent(MD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op command, Version %d.%d.%d.%d",
                                  MD_MAJOR_VERSION, MD_MINOR_VERSION, MD_REVISION, MD_MISSION_REV);

                MD_AppData.CmdCounter++;
                break;

            case MD_RESET_CNTRS_CC:

                CFE_EVS_SendEvent(MD_RESET_INF_EID, CFE_EVS_EventType_DEBUG, "Reset Counters Cmd Received");
                MD_AppData.CmdCounter = 0;
                MD_AppData.ErrCounter = 0;
                break;

            case MD_START_DWELL_CC:

                MD_ProcessStartCmd(BufPtr);
                break;

            case MD_STOP_DWELL_CC: /* Params: byte:table_ID */

                MD_ProcessStopCmd(BufPtr);
                break;

            case MD_JAM_DWELL_CC: /* Params: table_ID, AddrIndex, address, */
                                  /* FieldLength, DwellDelay */
                MD_ProcessJamCmd(BufPtr);
                break;

#if MD_SIGNATURE_OPTION == 1
            case MD_SET_SIGNATURE_CC:
                MD_ProcessSignatureCmd(BufPtr);
                break;
#endif
        } /* End Switch */
    }
}

/******************************************************************************/
void MD_HkStatus()
{
    uint8                    TblIndex;
    uint16                   MemDwellEnableBits = 0;
    MD_HkTlm_t              *HkPktPtr           = NULL;
    MD_DwellPacketControl_t *ThisDwellTablePtr  = NULL;

    /* Assign pointer used as shorthand to access Housekeeping Packet fields */
    HkPktPtr = &MD_AppData.HkPkt;

    /* Assign values for command counters */

    HkPktPtr->Payload.ValidCmdCntr   = MD_AppData.CmdCounter;
    HkPktPtr->Payload.InvalidCmdCntr = MD_AppData.ErrCounter;

    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
    {
        /*
        **  Prepare the DwellEnabledMask showing which dwell streams are enabled
        */

        /*   The dwell enabled bits are set in the following bit pattern:
         0x0001 = table 1 enabled, 0x0002 = table 2 enabled
         0x0004 = table 3 enabled, 0x0008 = table 4 enabled,
         0x0010 = table 5 enabled, etc.
        */
        MemDwellEnableBits |= (MD_AppData.MD_DwellTables[TblIndex].Enabled << (TblIndex));

        /* Assign pointer used as shorthand to access current dwell tbl data */
        ThisDwellTablePtr = &MD_AppData.MD_DwellTables[TblIndex];

        /*
        **  Insert current values for each dwell stream
        */
        HkPktPtr->Payload.DwellTblAddrCount[TblIndex] = ThisDwellTablePtr->AddrCount;
        HkPktPtr->Payload.NumWaitsPerPkt[TblIndex]    = ThisDwellTablePtr->Rate;
        HkPktPtr->Payload.DwellPktOffset[TblIndex]    = ThisDwellTablePtr->PktOffset;
        HkPktPtr->Payload.ByteCount[TblIndex]         = ThisDwellTablePtr->DataSize;
        HkPktPtr->Payload.DwellTblEntry[TblIndex]     = ThisDwellTablePtr->CurrentEntry;
        HkPktPtr->Payload.Countdown[TblIndex]         = ThisDwellTablePtr->Countdown;
    }

    HkPktPtr->Payload.DwellEnabledMask = MemDwellEnableBits;

    /*
    ** Send housekeeping telemetry packet...
    */
    CFE_SB_TimeStampMsg(&HkPktPtr->TlmHeader.Msg);
    CFE_SB_TransmitMsg(&HkPktPtr->TlmHeader.Msg, true);
}

/******************************************************************************/

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
