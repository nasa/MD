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
 *   Functions for processing individual CFS Memory Dwell commands
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "md_cmds.h"
#include "md_utils.h"
/* Need to include md_msg.h for command type definitions */
#include "md_msg.h"
#include <string.h>
#include "md_app.h"
#include "md_eventids.h"
#include "md_dwell_tbl.h"
#include "md_extern_typedefs.h"
#include "md_version.h"
#include "md_internal_cfg.h"

/* Global Data */
extern MD_AppData_t MD_AppData;

/******************************************************************************/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Noop command                                                    */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

CFE_Status_t MD_NoopCmd(const MD_NoopCmd_t *Msg)
{
    CFE_EVS_SendEvent(MD_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op command, Version %d.%d.%d.%d",
                        MD_MAJOR_VERSION, MD_MINOR_VERSION, MD_REVISION, MD_INTERNAL_MISSION_REV);

    MD_AppData.CmdCounter++;
    return CFE_SUCCESS;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Reset counters command                                          */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CFE_Status_t MD_ResetCountersCmd(const MD_ResetCountersCmd_t *Msg)
{
    CFE_EVS_SendEvent(MD_RESET_INF_EID, CFE_EVS_EventType_INFORMATION,
                        "Reset Counters Cmd Received");
    MD_AppData.CmdCounter = 0;
    MD_AppData.ErrCounter = 0;
    return CFE_SUCCESS;    
}

CFE_Status_t MD_StartDwellCmd(const MD_StartDwellCmd_t *Msg)
{
    int32              ErrorCount = 0;
    CFE_Status_t       Status;
    CFE_Status_t       TableUpdateStatus;
    int32              NumTblInMask = 0; /* Purely as info for event message */
    uint16             TableId      = 0;
    uint16             TableIndex;
    bool               AnyTablesInMask = false;

    Status = CFE_SUCCESS;

    /*  Preview tables specified by command:                   */
    /*  Check that there's at least one valid table specified. */
    for (TableId = 1; TableId <= MD_INTERFACE_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Msg->Payload.TableMask))
        {
            /* At least one valid Table Id is in Mask */
            AnyTablesInMask = true;
        }
    }

    /*
    ** Handle specified operation.
    ** If nominal, start each of the specified tables.
    ** If error case was encountered, issue error message.
    */
    if (AnyTablesInMask)
    { /* Handle Nominal Case */

        for (TableId = 1; TableId <= MD_INTERFACE_NUM_DWELL_TABLES; TableId++)
        {
            if (MD_TableIsInMask(TableId, Msg->Payload.TableMask))
            {
                NumTblInMask++;

                /* Setting Countdown to 1 causes a dwell packet to be issued */
                /* on first wakeup call received. */
                TableIndex                                         = TableId - 1;
                MD_AppData.MD_DwellTables[TableIndex].Enabled      = MD_Dwell_States_ENABLED;
                MD_AppData.MD_DwellTables[TableIndex].Countdown    = 1;
                MD_AppData.MD_DwellTables[TableIndex].CurrentEntry = 0;
                MD_AppData.MD_DwellTables[TableIndex].PktOffset    = 0;

                /* Change value in Table Services managed buffer */
                TableUpdateStatus = MD_UpdateTableEnabledField(TableIndex, MD_Dwell_States_ENABLED);
                if (TableUpdateStatus != CFE_SUCCESS)
                {
                    ErrorCount++;
                    Status = TableUpdateStatus;
                }

                /* If table contains a rate of zero, report that no processing will occur */
                if (MD_AppData.MD_DwellTables[TableIndex].Rate == 0)
                {
                    CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "Dwell Table %d is enabled with a delay of zero so no processing will occur",
                                      TableId);
                }
            }
        }

        if (ErrorCount == 0)
        {
            MD_AppData.CmdCounter++;

            CFE_EVS_SendEvent(MD_START_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Start Dwell Table command processed successfully for table mask 0x%04X",
                              Msg->Payload.TableMask);
        }
        else
        {
            MD_AppData.ErrCounter++;

            CFE_EVS_SendEvent(MD_START_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Start Dwell Table for mask 0x%04X failed for %d of %d tables", Msg->Payload.TableMask,
                              (int)ErrorCount, (int)NumTblInMask);
        }
    }
    else /* No valid table id's specified in mask */
    {
        MD_AppData.ErrCounter++;
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "%s command rejected because no tables were specified in table mask (0x%04X)", "Start Dwell",
                          Msg->Payload.TableMask);
    }
    return Status;
}

/******************************************************************************/

CFE_Status_t MD_StopDwellCmd(const MD_StopDwellCmd_t *Msg)
{
    int32              ErrorCount = 0;
    CFE_Status_t       Status;
    CFE_Status_t       TableUpdateStatus;
    int32              NumTblInMask = 0; /* Purely as info for event message */
    uint16             TableId      = 0;
    uint16             TableIndex;
    bool               AnyTablesInMask = false;

    Status = CFE_SUCCESS;

    for (TableId = 1; TableId <= MD_INTERFACE_NUM_DWELL_TABLES; TableId++)
    {
        if (MD_TableIsInMask(TableId, Msg->Payload.TableMask))
        {
            NumTblInMask++;
            TableIndex                                         = TableId - 1;
            MD_AppData.MD_DwellTables[TableIndex].Enabled      = MD_Dwell_States_DISABLED;
            MD_AppData.MD_DwellTables[TableIndex].Countdown    = 0;
            MD_AppData.MD_DwellTables[TableIndex].CurrentEntry = 0;
            MD_AppData.MD_DwellTables[TableIndex].PktOffset    = 0;

            AnyTablesInMask = true;

            /* Change value in Table Services managed buffer */
            TableUpdateStatus = MD_UpdateTableEnabledField(TableIndex, MD_Dwell_States_DISABLED);
            if (TableUpdateStatus != CFE_SUCCESS)
            {
                ErrorCount++;
                Status = TableUpdateStatus;
            }
        }
    }

    if (AnyTablesInMask)
    {
        if (ErrorCount == 0)
        {
            CFE_EVS_SendEvent(MD_STOP_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Stop Dwell Table command processed successfully for table mask 0x%04X",
                              Msg->Payload.TableMask);

            MD_AppData.CmdCounter++;
        }
        else
        {
            MD_AppData.ErrCounter++;

            CFE_EVS_SendEvent(MD_STOP_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Stop Dwell Table for mask 0x%04X failed for %d of %d tables", Msg->Payload.TableMask,
                              (int)ErrorCount, (int)NumTblInMask);
        }
    }
    else
    {
        CFE_EVS_SendEvent(MD_EMPTY_TBLMASK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "%s command rejected because no tables were specified in table mask (0x%04X)", "Stop Dwell",
                          Msg->Payload.TableMask);
        MD_AppData.ErrCounter++;
    }
    return Status;
}

/******************************************************************************/

CFE_Status_t MD_JamDwellCmd(const MD_JamDwellCmd_t *Msg)
{
    /* Local variables */
    CFE_Status_t            Status;
    bool                    AllInputsValid = true;
    cpuaddr                 ResolvedAddr   = 0;
    MD_DwellControlEntry_t *DwellEntryPtr; /* points to local task data */
    uint16                  EntryIndex;
    uint8                   TableIndex = 0;
    MD_SymAddr_t            NewDwellAddress;

    Status = CFE_SUCCESS;

    /* In case Dwell Address sym name isn't null terminated, do it now. */
    // Msg->Payload.DwellAddress.SymName[CFE_MISSION_MAX_PATH_LEN - 1] = '\0';

    /*
    **  Check that TableId and EntryId command arguments,
    **  which are used as array indexes, are valid.
    */
    if (!MD_ValidTableId(Msg->Payload.TableId))
    {
        CFE_EVS_SendEvent(MD_INVALID_JAM_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Jam Cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)", Msg->Payload.TableId,
                          MD_INTERFACE_NUM_DWELL_TABLES);

        AllInputsValid = false;
    }

    else if (!MD_ValidEntryId(Msg->Payload.EntryId))
    {
        CFE_EVS_SendEvent(MD_INVALID_ENTRY_ARG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Jam Cmd rejected due to invalid Entry Id arg = %d (Expect 1.. %d)", Msg->Payload.EntryId,
                          MD_INTERFACE_DWELL_TABLE_SIZE);

        AllInputsValid = false;
    }
    else
    {
        AllInputsValid = true;
    }

    /*
    **  If all inputs checked so far are valid, continue.
    */
    if (AllInputsValid == true)
    {
        TableIndex = Msg->Payload.TableId - 1;
        EntryIndex = Msg->Payload.EntryId - 1;

        DwellEntryPtr = (MD_DwellControlEntry_t *)&MD_AppData.MD_DwellTables[TableIndex].Entry[EntryIndex];

        if (Msg->Payload.FieldLength == 0)
        /*
        **  Jam a null entry.  Set all entry fields to zero.
        */
        {
            /* Assign local values */
            DwellEntryPtr->ResolvedAddress = 0;
            DwellEntryPtr->Length          = 0;
            DwellEntryPtr->Delay           = 0;

            /* Update Table Services buffer */
            NewDwellAddress.Offset     = 0;
            NewDwellAddress.SymName[0] = '\0';
            Status                     = MD_UpdateTableDwellEntry(TableIndex, EntryIndex, 0, 0, NewDwellAddress);

            /* Issue event */
            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(MD_JAM_NULL_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                                  "Successful Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d",
                                  Msg->Payload.TableId, Msg->Payload.EntryId);
            }
            else
            {
                CFE_EVS_SendEvent(MD_JAM_NULL_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Failed Jam of a Null Dwell Entry to Dwell Tbl#%d Entry #%d", Msg->Payload.TableId,
                                  Msg->Payload.EntryId);

                AllInputsValid = false;
            }
        }
        else
        /*
        **  Process non-null entry.
        */
        {
            /*
            **  Check that address and field length arguments pass all validity checks.
            */

            /* Resolve and Validate Dwell Address */
            if (MD_ResolveSymAddr(&Msg->Payload.DwellAddress, &ResolvedAddr) == false)
            {
                /* If DwellAddress argument couldn't be resolved, issue error event */
                CFE_EVS_SendEvent(MD_CANT_RESOLVE_JAM_ADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected because symbolic address '%s' couldn't be resolved",
                                  Msg->Payload.DwellAddress.SymName);
                AllInputsValid = false;
            }
            else if (!MD_ValidFieldLength(Msg->Payload.FieldLength))
            {
                CFE_EVS_SendEvent(MD_INVALID_LEN_ARG_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected due to invalid Field Length arg = %d (Expect 0,1,2,or 4)",
                                  Msg->Payload.FieldLength);
                AllInputsValid = false;
            }
            else if (!MD_ValidAddrRange(ResolvedAddr, Msg->Payload.FieldLength))
            {
                /* Issue event message that ResolvedAddr is invalid */
                CFE_EVS_SendEvent(MD_INVALID_JAM_ADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected because address 0x%08X is not in a valid range",
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
#if MD_INTERFACE_ENFORCE_DWORD_ALIGN == 0
            else if ((Jam->Payload.FieldLength == 4) &&
                     MD_Verify16Aligned(ResolvedAddr, (uint32)Jam->Payload.FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected because address 0x%08X is not 16-bit aligned", ResolvedAddr);
                AllInputsValid = false;
            }
#else
            else if ((Msg->Payload.FieldLength == 4) &&
                     MD_Verify32Aligned(ResolvedAddr, (uint32)Msg->Payload.FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_32BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected because address 0x%08X is not 32-bit aligned",
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
#endif
            else if ((Msg->Payload.FieldLength == 2) &&
                     MD_Verify16Aligned(ResolvedAddr, (uint32)Msg->Payload.FieldLength) != true)
            {
                CFE_EVS_SendEvent(MD_JAM_ADDR_NOT_16BIT_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Jam Cmd rejected because address 0x%08X is not 16-bit aligned",
                                  (unsigned int)ResolvedAddr);
                AllInputsValid = false;
            }
            else
            {
                /* All inputs are valid */
                AllInputsValid = true;
            }

            if (AllInputsValid == true)
            /*
            ** Perform Jam Operation : Copy Resolved DwellAddress, Length, and Delay to
            ** local control structure.
            */
            {
                /* Jam the new values into Application control structure */
                DwellEntryPtr->ResolvedAddress = ResolvedAddr;
                DwellEntryPtr->Length          = Msg->Payload.FieldLength;
                DwellEntryPtr->Delay           = Msg->Payload.DwellDelay;

                /* Update values in Table Services buffer */
                NewDwellAddress.Offset = Msg->Payload.DwellAddress.Offset;

                CFE_SB_MessageStringGet(NewDwellAddress.SymName, Msg->Payload.DwellAddress.SymName, "", 
                    sizeof(NewDwellAddress.SymName), sizeof(Msg->Payload.DwellAddress.SymName));

                Status = MD_UpdateTableDwellEntry(TableIndex, EntryIndex, Msg->Payload.FieldLength,
                                                  Msg->Payload.DwellDelay, NewDwellAddress);

                /* Issue event */
                if (Status == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(MD_JAM_DWELL_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "Successful Jam to Dwell Tbl#%d Entry #%d", Msg->Payload.TableId,
                                      Msg->Payload.EntryId);
                }
                else
                {
                    CFE_EVS_SendEvent(MD_JAM_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Failed Jam to Dwell Tbl#%d Entry #%d", Msg->Payload.TableId,
                                      Msg->Payload.EntryId);

                    AllInputsValid = false;
                }
            }

        } /* end else Process non-null entry */

    } /* end if AllInputsValid */

    /*
    **  Handle bookkeeping.
    */
    if (AllInputsValid == true)
    {
        MD_AppData.CmdCounter++;

        /* Update Dwell Table Control Info, including rate */
        MD_UpdateDwellControlInfo(TableIndex);

        /* If table contains a rate of zero, and it enabled report that no processing will occur */
        if ((MD_AppData.MD_DwellTables[TableIndex].Rate == 0) &&
            (MD_AppData.MD_DwellTables[TableIndex].Enabled == MD_Dwell_States_ENABLED))
        {
            CFE_EVS_SendEvent(MD_ZERO_RATE_CMD_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Dwell Table %d is enabled with a delay of zero so no processing will occur",
                              Msg->Payload.TableId);
        }
    }
    else
    {
        MD_AppData.ErrCounter++;
    }
    return Status;
}

/******************************************************************************/
#if MD_INTERFACE_SIGNATURE_OPTION == 1

CFE_Status_t MD_SetSignatureCmd(const MD_SetSignatureCmd_t *Msg)
{
    CFE_Status_t          Status;
    uint16                TblId        = 0;
    uint16                StringLength;

    Status = CFE_SUCCESS;

    TblId = Msg->Payload.TableId;

    /*
    **  Check for Null Termination of string
    */
    for (StringLength = 0; StringLength < MD_INTERFACE_SIGNATURE_FIELD_LENGTH; StringLength++)
    {
        if (Msg->Payload.Signature[StringLength] == '\0')
            break;
    }

    if (StringLength >= MD_INTERFACE_SIGNATURE_FIELD_LENGTH)
    {
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_LENGTH_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Set Signature cmd rejected due to invalid Signature length");

        MD_AppData.ErrCounter++;
    }

    /*
    ** Check for valid TableId argument
    */
    else if (!MD_ValidTableId(TblId))
    {
        CFE_EVS_SendEvent(MD_INVALID_SIGNATURE_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Set Signature cmd rejected due to invalid Tbl Id arg = %d (Expect 1.. %d)", TblId,
                          MD_INTERFACE_NUM_DWELL_TABLES);

        MD_AppData.ErrCounter++;
    }

    else

    /*
    **  Handle nominal case.
    */
    {
        /* Copy signature field to local dwell control structure */
        strncpy(MD_AppData.MD_DwellTables[TblId - 1].Signature, Msg->Payload.Signature,
                MD_INTERFACE_SIGNATURE_FIELD_LENGTH - 1);
        MD_AppData.MD_DwellTables[TblId - 1].Signature[MD_INTERFACE_SIGNATURE_FIELD_LENGTH - 1] = '\0';

        /* Update signature in Table Services buffer */
        Status = MD_UpdateTableSignature(TblId - 1, Msg->Payload.Signature);
        if (Status == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(MD_SET_SIGNATURE_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "Successfully set signature for Dwell Tbl#%d to '%s'", TblId,
                              Msg->Payload.Signature);

            MD_AppData.CmdCounter++;
        }
        else
        {
            CFE_EVS_SendEvent(MD_SET_SIGNATURE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Failed to set signature for Dwell Tbl#%d. Update returned 0x%08X", (int)TblId,
                              (unsigned int)Status);

            MD_AppData.ErrCounter++;
        }
    }
    return Status;
}

#endif
