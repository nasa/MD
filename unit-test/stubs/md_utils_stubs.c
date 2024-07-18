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

/*
 * Includes
 */

#include "md_msg.h"
#include "cfe.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

bool MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_TableIsInMask), TableId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_TableIsInMask), TableMask);
    return UT_DEFAULT_IMPL(MD_TableIsInMask) != 0;
}

void MD_UpdateDwellControlInfo(uint16 TableIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateDwellControlInfo), TableIndex);
    UT_DEFAULT_IMPL(MD_UpdateDwellControlInfo);
}

bool MD_ValidEntryId(uint16 EntryId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidEntryId), EntryId);
    return UT_DEFAULT_IMPL(MD_ValidEntryId) != 0;
}

bool MD_ValidAddrRange(cpuaddr Addr, size_t Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidAddrRange), Addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidAddrRange), Size);
    return UT_DEFAULT_IMPL(MD_ValidAddrRange) != 0;
}

bool MD_ValidTableId(uint16 TableId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidTableId), TableId);
    return UT_DEFAULT_IMPL(MD_ValidTableId) != 0;
}

bool MD_ValidFieldLength(uint16 FieldLength)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidFieldLength), FieldLength);
    return UT_DEFAULT_IMPL(MD_ValidFieldLength) != 0;
}

bool MD_Verify32Aligned(cpuaddr Address, size_t Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_Verify32Aligned), Address);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_Verify32Aligned), Size);
    return UT_DEFAULT_IMPL(MD_Verify32Aligned) != 0;
}

bool MD_Verify16Aligned(cpuaddr Address, size_t Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_Verify16Aligned), Address);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_Verify16Aligned), Size);
    return UT_DEFAULT_IMPL(MD_Verify16Aligned) != 0;
}

bool MD_ResolveSymAddr(MD_SymAddr_t *SymAddr, cpuaddr *ResolvedAddr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ResolveSymAddr), SymAddr);
    UT_Stub_RegisterContext(UT_KEY(MD_ResolveSymAddr), ResolvedAddr);
    return UT_DEFAULT_IMPL(MD_ResolveSymAddr) != 0;
}
