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
 *   Functions used for validating and copying CFS Memory Dwell Tables.
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "md_dwell_tbl.h"
#include "md_utils.h"
/* Need md_app.h for MD_DwellTableLoad_t defn */
#include "md_app.h"
#include "md_events.h"
#include <string.h>
#include "md_tbldefs.h"
#include "md_extern_typedefs.h"

extern MD_AppData_t MD_AppData;

/******************************************************************************/
int32 MD_TableValidationFunc(void *TblPtr)
{
    uint16               ActiveEntryCount   = 0;
    uint16               Size               = 0;
    uint32               Rate               = 0;
    int32                Status             = CFE_SUCCESS; /* Initialize to valid table */
    MD_DwellTableLoad_t *LocalTblPtr        = (MD_DwellTableLoad_t *)TblPtr;
    uint16               TblErrorEntryIndex = 0; /* Zero-based entry number for error; */
                                                 /* valid if there is an error.        */

    if (TblPtr == NULL)
    {
        Status = MD_INVALID_ADDR_ERROR;

        CFE_EVS_SendEvent(MD_TBL_VAL_NULL_PTR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Dwell Table rejected because of null table pointer");

        return Status;
    }

#if MD_SIGNATURE_OPTION == 1

    uint16 StringLength;

    /*
    **  Check for Null Termination of string
    */
    for (StringLength = 0; StringLength < MD_SIGNATURE_FIELD_LENGTH; StringLength++)
    {
        if (LocalTblPtr->Signature[StringLength] == '\0')
            break;
    }

#endif

    /* Check that Enabled flag has valid value. */
    if ((LocalTblPtr->Enabled != MD_DWELL_STREAM_DISABLED) && (LocalTblPtr->Enabled != MD_DWELL_STREAM_ENABLED))
    {
        CFE_EVS_SendEvent(MD_TBL_ENA_FLAG_EID, CFE_EVS_EventType_ERROR,
                          "Dwell Table rejected because value of enable flag (%d) is invalid",
                          (int)LocalTblPtr->Enabled);
        Status = MD_TBL_ENA_FLAG_ERROR;
    }

#if MD_SIGNATURE_OPTION == 1

    else if (StringLength >= MD_SIGNATURE_FIELD_LENGTH)
    {
        CFE_EVS_SendEvent(MD_TBL_SIG_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Dwell Table rejected because Signature length was invalid");

        Status = MD_SIG_LEN_TBL_ERROR;
    }

#endif

    else
    {
        /* Read Dwell Table to get entry count, byte size, and dwell rate */
        MD_ReadDwellTable(LocalTblPtr, &ActiveEntryCount, &Size, &Rate);

        /* Validate entry contents */
        Status = MD_CheckTableEntries(LocalTblPtr, &TblErrorEntryIndex);

        if (Status == CFE_SUCCESS)
        {
            /* Allow ground to uplink a table with 0 delay, but if the table is enabled, report that the table will not
             * be processed */
            if ((LocalTblPtr->Enabled == MD_DWELL_STREAM_ENABLED) && (Rate == 0))
            {
                CFE_EVS_SendEvent(
                    MD_ZERO_RATE_TBL_INF_EID, CFE_EVS_EventType_INFORMATION,
                    "Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)");
            }
        }
        else if (Status == MD_RESOLVE_ERROR)
        {
            CFE_EVS_SendEvent(
                MD_RESOLVE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d couldn't be resolved",
                LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.SymName,
                (unsigned int)LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.Offset, TblErrorEntryIndex + 1);
        }
        else if (Status == MD_INVALID_ADDR_ERROR)
        {
            CFE_EVS_SendEvent(
                MD_RANGE_ERR_EID, CFE_EVS_EventType_ERROR,
                "Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d was out of range",
                LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.SymName,
                (unsigned int)LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.Offset, TblErrorEntryIndex + 1);
        }
        else if (Status == MD_INVALID_LEN_ERROR)
        {
            CFE_EVS_SendEvent(MD_TBL_HAS_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Dwell Table rejected because length (%d) in entry #%d was invalid",
                              LocalTblPtr->Entry[TblErrorEntryIndex].Length, TblErrorEntryIndex + 1);
        }
        else /* Status == MD_NOT_ALIGNED_ERROR is only remaining option */
        {
            CFE_EVS_SendEvent(MD_TBL_ALIGN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Dwell Table rejected because address (sym='%s'/offset=0x%08X) in entry #%d not properly "
                              "aligned for %d-byte dwell",
                              LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.SymName,
                              (unsigned int)LocalTblPtr->Entry[TblErrorEntryIndex].DwellAddress.Offset,
                              TblErrorEntryIndex + 1, LocalTblPtr->Entry[TblErrorEntryIndex].Length);
        }

    } /* end else MD_ReadDwellTable */

    return Status;
}

/******************************************************************************/
CFE_Status_t MD_ReadDwellTable(const MD_DwellTableLoad_t *TblPtr, uint16 *ActiveAddrCountPtr, uint16 *SizePtr,
                               uint32 *RatePtr)
{
    /* parameters cannot be NULL - checked by calling function */

    uint16 EntryId;
    *ActiveAddrCountPtr = 0;
    *SizePtr            = 0;
    *RatePtr            = 0;

    for (EntryId = 0; (EntryId < MD_DWELL_TABLE_SIZE) && (TblPtr->Entry[EntryId].Length != 0); EntryId++)
    {
        /* *ActiveAddrCountPtr++; */
        (*ActiveAddrCountPtr)++;
        *SizePtr += TblPtr->Entry[EntryId].Length;
        *RatePtr += TblPtr->Entry[EntryId].Delay;
    } /* end while */

    return CFE_SUCCESS;
}
/******************************************************************************/
int32 MD_CheckTableEntries(MD_DwellTableLoad_t *TblPtr, uint16 *ErrorEntryArg)
{
    int32  Status;
    int32  FirstErrorCode = CFE_SUCCESS;
    uint16 EntryIndex;
    int32  FirstBadIndex = -1;

    int32 GoodCount   = 0;
    int32 BadCount    = 0;
    int32 UnusedCount = 0;

    *ErrorEntryArg = 0;

    /* parameters cannot be NULL - checked by calling function */

    /*
    **   Check each Dwell Table entry for valid address range
    */
    for (EntryIndex = 0; EntryIndex < MD_DWELL_TABLE_SIZE; EntryIndex++)
    {
        Status = MD_ValidTableEntry(&TblPtr->Entry[EntryIndex]);

        if (Status == CFE_SUCCESS)
        {
            if (TblPtr->Entry[EntryIndex].Length == 0)
            {
                /*
                ** Stop reading through the table when the entry length is
                ** zero, remaining entries count as unused
                */
                break;
            }
            else
            {
                GoodCount++;
            }
        }
        else
        {
            /* Status != CFE_SUCCESS */
            BadCount++;
            if (FirstBadIndex == -1)
            {
                FirstBadIndex  = EntryIndex;
                *ErrorEntryArg = FirstBadIndex;
                FirstErrorCode = Status;
                /* Keep counting good,bad,unused, don't exit immediately */

                CFE_EVS_SendEvent(MD_TBL_ENTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Table entry %d failed with status 0x%08X", EntryIndex, Status);
            }
        }
    }

    UnusedCount = MD_DWELL_TABLE_SIZE - EntryIndex;

    /*
    ** Generate informational event with error totals
    */
    CFE_EVS_SendEvent(MD_DWELL_TBL_INF_EID, CFE_EVS_EventType_INFORMATION,
                      "MD Dwell Tbl verify results: good = %d, bad = %d, unused = %d", (int)GoodCount, (int)BadCount,
                      (int)UnusedCount);

    return FirstErrorCode;
}

/******************************************************************************/
int32 MD_ValidTableEntry(MD_TableLoadEntry_t *TblEntryPtr)
{
    int32   Status       = MD_ERROR;
    uint16  DwellLength  = 0;
    cpuaddr ResolvedAddr = 0;

    DwellLength = TblEntryPtr->Length;

    if (DwellLength == 0)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        if (MD_ResolveSymAddr(&TblEntryPtr->DwellAddress, &ResolvedAddr) != true)
        { /* Symbol was non-null AND was not in Symbol Table */
            Status = MD_RESOLVE_ERROR;
        }

        else if (MD_ValidAddrRange(ResolvedAddr, (uint32)DwellLength) != true)
        { /* Address is in invalid range  */
            Status = MD_INVALID_ADDR_ERROR;
        }
        else if (MD_ValidFieldLength(DwellLength) != true)
        {
            Status = MD_INVALID_LEN_ERROR;
        }
#if MD_ENFORCE_DWORD_ALIGN == 0
        else if ((DwellLength == 4) && MD_Verify16Aligned(ResolvedAddr, (uint32)DwellLength) != true)
        {
            Status = MD_NOT_ALIGNED_ERROR;
        }
#else
        else if ((DwellLength == 4) && MD_Verify32Aligned(ResolvedAddr, (uint32)DwellLength) != true)
        {
            Status = MD_NOT_ALIGNED_ERROR;
        }
#endif
        else if ((DwellLength == 2) && MD_Verify16Aligned(ResolvedAddr, (uint32)DwellLength) != true)
        {
            Status = MD_NOT_ALIGNED_ERROR;
        }
        else
        {
            Status = CFE_SUCCESS;
        }

    } /* end else */

    return Status;
}

/******************************************************************************/
void MD_CopyUpdatedTbl(MD_DwellTableLoad_t *MD_LoadTablePtr, uint8 TblIndex)
{
    uint8                    EntryIndex;
    cpuaddr                  ResolvedAddr       = 0;
    MD_TableLoadEntry_t *    ThisLoadEntry      = NULL;
    MD_DwellPacketControl_t *LocalControlStruct = &MD_AppData.MD_DwellTables[TblIndex];

    /* Null check on MD_LoadTablePtr not necessary - table passed validation */

    /* Copy 'Enabled' field from load structure to internal control structure. */
    LocalControlStruct->Enabled = MD_LoadTablePtr->Enabled;

#if MD_SIGNATURE_OPTION == 1
    /* Copy 'Signature' field from load structure to internal control structure. */
    strncpy(LocalControlStruct->Signature, MD_LoadTablePtr->Signature, MD_SIGNATURE_FIELD_LENGTH - 1);

    /* Ensure that resulting string is null-terminated */
    LocalControlStruct->Signature[MD_SIGNATURE_FIELD_LENGTH - 1] = '\0';
#endif

    /* For each row in the table load, */
    /* copy length, delay, and address fields from load structure to */
    /* internal control structure. */
    for (EntryIndex = 0; EntryIndex < MD_DWELL_TABLE_SIZE; EntryIndex++)
    {
        /* Get ResolvedAddr & insert in local control structure */

        ThisLoadEntry = &MD_LoadTablePtr->Entry[EntryIndex];

        MD_ResolveSymAddr(&ThisLoadEntry->DwellAddress, &ResolvedAddr);

        MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].ResolvedAddress = ResolvedAddr;
        MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Length          = ThisLoadEntry->Length;
        MD_AppData.MD_DwellTables[TblIndex].Entry[EntryIndex].Delay           = ThisLoadEntry->Delay;

    } /* end for loop */

    /* Update Dwell Table Control Info, used to process dwell packets */
    MD_UpdateDwellControlInfo((uint16)TblIndex);
}

/******************************************************************************/
CFE_Status_t MD_UpdateTableEnabledField(uint16 TableIndex, uint16 FieldValue)
{
    CFE_Status_t         Status          = CFE_SUCCESS;
    MD_DwellTableLoad_t *MD_LoadTablePtr = NULL;

    Status = CFE_TBL_GetAddress((void *)&MD_LoadTablePtr, MD_AppData.MD_TableHandle[TableIndex]);

    if ((Status != CFE_SUCCESS) && (Status != CFE_TBL_INFO_UPDATED))
    {
        CFE_EVS_SendEvent(MD_UPDATE_TBL_EN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "MD_UpdateTableEnabledField, TableIndex %d: CFE_TBL_GetAddress Returned 0x%08x", TableIndex,
                          Status);
    }
    else
    {
        MD_LoadTablePtr->Enabled = FieldValue;

        CFE_TBL_Modified(MD_AppData.MD_TableHandle[TableIndex]);

        CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TableIndex]);

        Status = CFE_SUCCESS;
    }

    return Status;
}

/******************************************************************************/

CFE_Status_t MD_UpdateTableDwellEntry(uint16 TableIndex, uint16 EntryIndex, uint16 NewLength, uint16 NewDelay,
                                      MD_SymAddr_t NewDwellAddress)
{
    CFE_Status_t         Status          = CFE_SUCCESS;
    MD_DwellTableLoad_t *MD_LoadTablePtr = NULL;
    MD_TableLoadEntry_t *EntryPtr        = NULL;

    /* Get pointer to Table */
    Status = CFE_TBL_GetAddress((void *)&MD_LoadTablePtr, MD_AppData.MD_TableHandle[TableIndex]);

    if ((Status != CFE_SUCCESS) && (Status != CFE_TBL_INFO_UPDATED))
    {
        CFE_EVS_SendEvent(MD_UPDATE_TBL_DWELL_ERR_EID, CFE_EVS_EventType_ERROR,
                          "MD_UpdateTableDwellEntry, TableIndex %d: CFE_TBL_GetAddress Returned 0x%08x", TableIndex,
                          Status);
    }
    else
    {
        /* Get pointer to specific entry */
        EntryPtr = &MD_LoadTablePtr->Entry[EntryIndex];

        /* Copy new numerical values to Table Services buffer */
        EntryPtr->Length              = NewLength;
        EntryPtr->Delay               = NewDelay;
        EntryPtr->DwellAddress.Offset = NewDwellAddress.Offset;

        /* Copy symbol name to Table Services buffer */
        strncpy(EntryPtr->DwellAddress.SymName, NewDwellAddress.SymName, OS_MAX_SYM_LEN - 1);

        /* Ensure string is null terminated. */
        EntryPtr->DwellAddress.SymName[OS_MAX_SYM_LEN - 1] = '\0';

        /* Notify Table Services that buffer was modified */
        CFE_TBL_Modified(MD_AppData.MD_TableHandle[TableIndex]);

        /* Release access to Table Services buffer */
        CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TableIndex]);

        Status = CFE_SUCCESS;
    }

    return Status;
}

/******************************************************************************/
#if MD_SIGNATURE_OPTION == 1

CFE_Status_t MD_UpdateTableSignature(uint16 TableIndex, char NewSignature[MD_SIGNATURE_FIELD_LENGTH])
{
    CFE_Status_t         Status          = CFE_SUCCESS;
    MD_DwellTableLoad_t *MD_LoadTablePtr = NULL;

    /* Get pointer to Table */
    Status = CFE_TBL_GetAddress((void *)&MD_LoadTablePtr, MD_AppData.MD_TableHandle[TableIndex]);

    if ((Status != CFE_SUCCESS) && (Status != CFE_TBL_INFO_UPDATED))
    {
        CFE_EVS_SendEvent(MD_UPDATE_TBL_SIG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "MD_UpdateTableSignature, TableIndex %d: CFE_TBL_GetAddress Returned 0x%08x", TableIndex,
                          Status);
    }
    else
    {
        /* Copy Signature to dwell structure */
        strncpy(MD_LoadTablePtr->Signature, NewSignature, MD_SIGNATURE_FIELD_LENGTH - 1);

        MD_LoadTablePtr->Signature[MD_SIGNATURE_FIELD_LENGTH - 1] = '\0';

        /* Notify Table Services that buffer was modified */
        CFE_TBL_Modified(MD_AppData.MD_TableHandle[TableIndex]);

        /* Release access to Table Services buffer */
        CFE_TBL_ReleaseAddress(MD_AppData.MD_TableHandle[TableIndex]);

        Status = CFE_SUCCESS;
    }

    return Status;
}

#endif
