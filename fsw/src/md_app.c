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
 *   CFS Memory Dwell Application top-level procedures.
 */

#include "cfe.h"
#include "md_tblstruct.h"
#include "md_app.h"
#include "md_eventids.h"
#include "md_extern_typedefs.h"
#include "md_verify.h"
#include "md_cmds.h"
#include "md_dispatch.h"
#include <string.h>
#include "md_dwell_tbl.h"
#include "md_dwell_pkt.h"
#include "md_utils.h"
#include "md_perfids.h"
#include "md_version.h"

MD_AppData_t MD_AppData;

void MD_AppMain(void)
{
    CFE_Status_t     Status       = CFE_SUCCESS;
    uint8            TblIndex     = 0;
    CFE_SB_Buffer_t *BufPtr       = NULL;

    MD_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

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
        for (TblIndex = 0; TblIndex < MD_INTERFACE_NUM_DWELL_TABLES; TblIndex++)
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

        /* Process message if received successfully */
        if (Status == CFE_SUCCESS)
        {
            MD_ProcessCommandPacket(BufPtr);
        }

    }/* end while forever loop */

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

    MD_AppData.CmdCounter = 0;
    MD_AppData.ErrCounter = 0;

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
                              MD_MAJOR_VERSION, MD_MINOR_VERSION, MD_REVISION, MD_INTERNAL_MISSION_REV);
    }

    return Status;
}

/******************************************************************************/
void MD_InitControlStructures(void)
{
    uint16                   TblIndex;
    MD_DwellPacketControl_t *DwellControlPtr = NULL;

    for (TblIndex = 0; TblIndex < MD_INTERFACE_NUM_DWELL_TABLES; TblIndex++)
    {
        DwellControlPtr = &MD_AppData.MD_DwellTables[TblIndex];

        memset(DwellControlPtr, 0, sizeof(*DwellControlPtr));

        /* Countdown must be set to 1 since it's decremented at the top of */
        /* the dwell loop. */
        DwellControlPtr->Countdown = 1;

#if MD_INTERFACE_SIGNATURE_OPTION == 1
        strncpy(DwellControlPtr->Signature, "", MD_INTERFACE_SIGNATURE_FIELD_LENGTH);
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
    CFE_MSG_Init(CFE_MSG_PTR(MD_AppData.HkPkt.TelemetryHeader), CFE_SB_ValueToMsgId(MD_HK_TLM_MID), sizeof(MD_HkTlm_t));
    /*
    ** Initialize dwell packets (clear user data area)
    */
    for (TblIndex = 0; TblIndex < MD_INTERFACE_NUM_DWELL_TABLES; TblIndex++)
    {
        CFE_MSG_Init(CFE_MSG_PTR(MD_AppData.MD_DwellPkt[TblIndex].TelemetryHeader),
                     CFE_SB_ValueToMsgId(MD_DWELL_PKT_MID_BASE + TblIndex),
                     sizeof(MD_DwellPkt_t)); /* this is max pkt size */

#if MD_INTERFACE_SIGNATURE_OPTION == 1
        MD_AppData.MD_DwellPkt[TblIndex].Payload.Signature[0] = '\0';
#endif
    }

    /*
    **  Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&MD_AppData.CmdPipe, MD_INTERNAL_PIPE_DEPTH, MD_INTERNAL_PIPE_NAME);

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

    for (TblIndex = 0; (TblIndex < MD_INTERFACE_NUM_DWELL_TABLES) && (TableInitValidFlag == true); TblIndex++)
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
        Status = snprintf(TblFileName, OS_MAX_PATH_LEN, MD_INTERNAL_TBL_FILENAME_FORMAT, TblIndex + 1);

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

                    if (MD_LoadTablePtr->Enabled == MD_Dwell_States_ENABLED)
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
                              (unsigned int)sizeof(MD_DwellTableLoad_t), MD_INTERFACE_DWELL_TABLE_SIZE);
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

            MD_AppData.MD_DwellTables[TblIndex].Enabled = MD_Dwell_States_DISABLED;

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

                    if (MD_LoadTablePtr->Enabled == MD_Dwell_States_ENABLED)
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
CFE_Status_t MD_HkStatus(const MD_SendHkCmd_t *Msg)
{
    uint8                    TblIndex;
    uint16                   MemDwellEnableBits = 0;
    MD_HkTlm_t *             HkPktPtr           = NULL;
    MD_DwellPacketControl_t *ThisDwellTablePtr  = NULL;

    /* Assign pointer used as shorthand to access Housekeeping Packet fields */
    HkPktPtr = &MD_AppData.HkPkt;

    /* Assign values for command counters */

    HkPktPtr->Payload.ValidCmdCntr   = MD_AppData.CmdCounter;
    HkPktPtr->Payload.InvalidCmdCntr = MD_AppData.ErrCounter;

    for (TblIndex = 0; TblIndex < MD_INTERFACE_NUM_DWELL_TABLES; TblIndex++)
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
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(HkPktPtr->TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(HkPktPtr->TelemetryHeader), true);

    return CFE_SUCCESS;
}
