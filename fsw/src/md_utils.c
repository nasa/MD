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
 *   Utility functions used for processing CFS Memory Dwell commands
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_utils.h"
#include "md_app.h"
#include <string.h>
extern MD_AppData_t MD_AppData;

/******************************************************************************/

bool MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
    uint16 LocalMask = TableMask;
    bool   Status    = false;

    /* Shift TableId - 1 times */
    if (TableId - 1)
    {
        LocalMask = LocalMask >> (TableId - 1);
    }

    /* If result is odd, */
    /* then table is in mask. */
    if ((LocalMask & (uint16)1) == (uint16)1)
        Status = true;

    return Status;
}

/******************************************************************************/

void MD_UpdateDwellControlInfo(uint16 TableIndex)
{
    /* Initialize local data */
    uint16                   EntryIndex          = 0;
    uint16                   NumDwellAddresses   = 0;
    uint16                   NumDwellDataBytes   = 0;
    uint32                   NumDwellDelayCounts = 0;
    MD_DwellPacketControl_t *TblPtr              = &MD_AppData.MD_DwellTables[TableIndex];

    /*
    ** Sum Address Count, Data Size, and Rate from Dwell Entries
    */
    while ((EntryIndex < MD_DWELL_TABLE_SIZE) && (TblPtr->Entry[EntryIndex].Length != 0))
    {
        NumDwellAddresses++;
        NumDwellDataBytes += TblPtr->Entry[EntryIndex].Length;
        NumDwellDelayCounts += TblPtr->Entry[EntryIndex].Delay;
        EntryIndex++;
    }

    /* Copy totals to dwell control structure. */
    TblPtr->AddrCount = NumDwellAddresses;
    TblPtr->DataSize  = NumDwellDataBytes;
    TblPtr->Rate      = NumDwellDelayCounts;
}

/******************************************************************************/

/*
**  Data Validation Functions
*/
/******************************************************************************/
bool MD_ValidEntryId(uint16 EntryId)
{
    bool IsValid = false;

    if ((EntryId >= 1) && (EntryId <= MD_DWELL_TABLE_SIZE))
    {
        /* validate  value (1..MD_DWELL_TABLE_SIZE ) */
        IsValid = true;
    }

    return IsValid;
}

/******************************************************************************/

bool MD_ValidAddrRange(cpuaddr Addr, size_t Size)
{
    bool IsValid = false;

    if (CFE_PSP_MemValidateRange(Addr, Size, CFE_PSP_MEM_ANY) == OS_SUCCESS)
    {
        IsValid = true;
    }

    return IsValid;
}

/******************************************************************************/
bool MD_ValidTableId(uint16 TableId)
{
    bool IsValid = false;

    if ((TableId >= 1) && (TableId <= MD_NUM_DWELL_TABLES))
    {
        IsValid = true;
    }

    return IsValid;
}

/******************************************************************************/

bool MD_ValidFieldLength(uint16 FieldLength)
{
    bool IsValid = false;

    if ((FieldLength == 0) || (FieldLength == 1) || (FieldLength == 2) || (FieldLength == 4))
    {
        IsValid = true;
    }

    return IsValid;
}

/******************************************************************************/

bool MD_Verify32Aligned(cpuaddr Address, size_t Size)
{
    bool IsAligned;

    if (Address % sizeof(uint32) != 0)
    {
        IsAligned = false;
    }
    else if (Size % sizeof(uint32) != 0)
    {
        IsAligned = false;
    }
    else
    {
        IsAligned = true;
    }

    return IsAligned;
}

/******************************************************************************/

bool MD_Verify16Aligned(cpuaddr Address, size_t Size)
{
    bool IsAligned;

    if (Address % sizeof(uint16) != 0)
    {
        IsAligned = false;
    }
    else if (Size % sizeof(uint16) != 0)
    {
        IsAligned = false;
    }
    else
    {
        IsAligned = true;
    }

    return IsAligned;
}

/******************************************************************************/

bool MD_ResolveSymAddr(MD_SymAddr_t *SymAddr, cpuaddr *ResolvedAddr)
{
    bool  Valid;
    int32 OS_Status;

    /*
    ** NUL terminate the very end of the symbol name string array as a
    ** safety measure
    */
    SymAddr->SymName[OS_MAX_SYM_LEN - 1] = '\0';

    /*
    ** If the symbol name string is a nul string
    ** we use the offset as the absolute address
    */
    if (strlen(SymAddr->SymName) == 0)
    {
        *ResolvedAddr = SymAddr->Offset;
        Valid         = true;
    }
    else
    {
        /*
        ** If symbol name is not an empty string look it up
        ** using the OSAL API and add the offset if it succeeds
        */
        OS_Status = OS_SymbolLookup(ResolvedAddr, SymAddr->SymName);
        if (OS_Status == OS_SUCCESS)
        {
            *ResolvedAddr += SymAddr->Offset;
            Valid = true;
        }
        else
            Valid = false;
    }
    return Valid;
}
