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
 *
 * Auto-Generated stub implementations for functions defined in md_utils header
 */

#include "md_utils.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ResolveSymAddr()
 * ----------------------------------------------------
 */
bool MD_ResolveSymAddr(const MD_SymAddr_t *SymAddr, cpuaddr *ResolvedAddr)
{
    UT_GenStub_SetupReturnBuffer(MD_ResolveSymAddr, bool);

    UT_GenStub_AddParam(MD_ResolveSymAddr, const MD_SymAddr_t *, SymAddr);
    UT_GenStub_AddParam(MD_ResolveSymAddr, cpuaddr *, ResolvedAddr);

    UT_GenStub_Execute(MD_ResolveSymAddr, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ResolveSymAddr, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_TableIsInMask()
 * ----------------------------------------------------
 */
bool MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
    UT_GenStub_SetupReturnBuffer(MD_TableIsInMask, bool);

    UT_GenStub_AddParam(MD_TableIsInMask, int16, TableId);
    UT_GenStub_AddParam(MD_TableIsInMask, uint16, TableMask);

    UT_GenStub_Execute(MD_TableIsInMask, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_TableIsInMask, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_UpdateDwellControlInfo()
 * ----------------------------------------------------
 */
void MD_UpdateDwellControlInfo(uint16 TableIndex)
{
    UT_GenStub_AddParam(MD_UpdateDwellControlInfo, uint16, TableIndex);

    UT_GenStub_Execute(MD_UpdateDwellControlInfo, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ValidAddrRange()
 * ----------------------------------------------------
 */
bool MD_ValidAddrRange(cpuaddr Addr, uint32 Size)
{
    UT_GenStub_SetupReturnBuffer(MD_ValidAddrRange, bool);

    UT_GenStub_AddParam(MD_ValidAddrRange, cpuaddr, Addr);
    UT_GenStub_AddParam(MD_ValidAddrRange, uint32, Size);

    UT_GenStub_Execute(MD_ValidAddrRange, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ValidAddrRange, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ValidEntryId()
 * ----------------------------------------------------
 */
bool MD_ValidEntryId(uint16 EntryId)
{
    UT_GenStub_SetupReturnBuffer(MD_ValidEntryId, bool);

    UT_GenStub_AddParam(MD_ValidEntryId, uint16, EntryId);

    UT_GenStub_Execute(MD_ValidEntryId, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ValidEntryId, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ValidFieldLength()
 * ----------------------------------------------------
 */
bool MD_ValidFieldLength(uint16 FieldLength)
{
    UT_GenStub_SetupReturnBuffer(MD_ValidFieldLength, bool);

    UT_GenStub_AddParam(MD_ValidFieldLength, uint16, FieldLength);

    UT_GenStub_Execute(MD_ValidFieldLength, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ValidFieldLength, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ValidTableId()
 * ----------------------------------------------------
 */
bool MD_ValidTableId(uint16 TableId)
{
    UT_GenStub_SetupReturnBuffer(MD_ValidTableId, bool);

    UT_GenStub_AddParam(MD_ValidTableId, uint16, TableId);

    UT_GenStub_Execute(MD_ValidTableId, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ValidTableId, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_Verify16Aligned()
 * ----------------------------------------------------
 */
bool MD_Verify16Aligned(cpuaddr Address, uint32 Size)
{
    UT_GenStub_SetupReturnBuffer(MD_Verify16Aligned, bool);

    UT_GenStub_AddParam(MD_Verify16Aligned, cpuaddr, Address);
    UT_GenStub_AddParam(MD_Verify16Aligned, uint32, Size);

    UT_GenStub_Execute(MD_Verify16Aligned, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_Verify16Aligned, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_Verify32Aligned()
 * ----------------------------------------------------
 */
bool MD_Verify32Aligned(cpuaddr Address, uint32 Size)
{
    UT_GenStub_SetupReturnBuffer(MD_Verify32Aligned, bool);

    UT_GenStub_AddParam(MD_Verify32Aligned, cpuaddr, Address);
    UT_GenStub_AddParam(MD_Verify32Aligned, uint32, Size);

    UT_GenStub_Execute(MD_Verify32Aligned, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_Verify32Aligned, bool);
}
