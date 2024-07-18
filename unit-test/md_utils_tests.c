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

#include "md_utils.h"
#include "md_msg.h"
#include "md_msgdefs.h"
#include "md_events.h"
#include "md_version.h"
#include "md_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* md_utils_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

void MD_TableIsInMask_Test_ShiftOddResult(void)
{
    bool   Result;
    int16  TableId   = 2;
    uint16 TableMask = 2;

    /* Execute the function being tested */
    Result = MD_TableIsInMask(TableId, TableMask);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableIsInMask_Test_ShiftEvenResult(void)
{
    bool   Result;
    int16  TableId   = 2;
    uint16 TableMask = 4;

    /* Execute the function being tested */
    Result = MD_TableIsInMask(TableId, TableMask);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableIsInMask_Test_TableNotInMask(void)
{
    bool   Result;
    int16  TableId   = 1;
    uint16 TableMask = 2;

    /* Execute the function being tested */
    Result = MD_TableIsInMask(TableId, TableMask);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateDwellControlInfo_Test(void)
{
    uint16 TableIndex = 1;

    MD_AppData.MD_DwellTables[TableIndex].Entry[0].Length = 1;
    MD_AppData.MD_DwellTables[TableIndex].Entry[0].Delay  = 2;

    /* Execute the function being tested */
    MD_UpdateDwellControlInfo(TableIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].AddrCount == 1,
                  "MD_AppData.MD_DwellTables[TableIndex].AddrCount == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].DataSize == 1,
                  "MD_AppData.MD_DwellTables[TableIndex].DataSize  == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].Rate == 2,
                  "MD_AppData.MD_DwellTables[TableIndex].Rate      == 2");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateDwellControlInfo_TestAllTableEntries(void)
{
    uint16 TableIndex = 1;
    int    i;

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        MD_AppData.MD_DwellTables[TableIndex].Entry[i].Length = 1;
        MD_AppData.MD_DwellTables[TableIndex].Entry[i].Delay  = 2;
    }

    /* Execute the function being tested */
    MD_UpdateDwellControlInfo(TableIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].AddrCount == 25,
                  "MD_AppData.MD_DwellTables[TableIndex].AddrCount == 25");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].DataSize == 25,
                  "MD_AppData.MD_DwellTables[TableIndex].DataSize  == 25");
    UtAssert_True(MD_AppData.MD_DwellTables[TableIndex].Rate == 50,
                  "MD_AppData.MD_DwellTables[TableIndex].Rate      == 50");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidEntryId_Test_Valid(void)
{
    bool   Result;
    uint16 EntryId = 1;

    /* Execute the function being tested */
    Result = MD_ValidEntryId(EntryId);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidEntryId_Test_Invalid(void)
{
    bool   Result;
    uint16 EntryId = 0;

    /* Execute the function being tested */
    Result = MD_ValidEntryId(EntryId);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidEntryId_Test_RangeError(void)
{
    bool   Result;
    uint16 EntryId = MD_DWELL_TABLE_SIZE + 1;

    /* Execute the function being tested */
    Result = MD_ValidEntryId(EntryId);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidAddrRange_Test_Valid(void)
{
    bool    Result;
    cpuaddr Addr = 1;
    size_t  Size = 1;

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidAddrRange_Test_Invalid(void)
{
    bool    Result;
    cpuaddr Addr = 1;
    size_t  Size = 1;

    /* Set to reach "IsValid = false" */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableId_Test_Valid(void)
{
    bool   Result;
    uint16 TableId = 1;

    /* Execute the function being tested */
    Result = MD_ValidTableId(TableId);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableId_Test_Invalid(void)
{
    bool   Result;
    uint16 TableId = 0;

    /* Execute the function being tested */
    Result = MD_ValidTableId(TableId);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableId_Test_RangeError(void)
{
    bool   Result;
    uint16 TableId = MD_NUM_DWELL_TABLES + 1;

    /* Execute the function being tested */
    Result = MD_ValidTableId(TableId);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidFieldLength_Test_ValidFieldLength0(void)
{
    bool   Result;
    uint16 FieldLength = 0;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidFieldLength_Test_ValidFieldLength1(void)
{
    bool   Result;
    uint16 FieldLength = 1;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidFieldLength_Test_ValidFieldLength2(void)
{
    bool   Result;
    uint16 FieldLength = 2;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidFieldLength_Test_ValidFieldLength4(void)
{
    bool   Result;
    uint16 FieldLength = 4;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidFieldLength_Test_Invalid(void)
{
    bool   Result;
    uint16 FieldLength = 5;

    /* Execute the function being tested */
    Result = MD_ValidFieldLength(FieldLength);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_Verify32Aligned_Test(void)
{
    bool    Result;
    cpuaddr Addr;
    size_t  Size;

    Addr = 0; /* address is aligned */
    Size = 4; /* size is aligned */

    /* Execute the function being tested */
    Result = MD_Verify32Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    Addr = 0; /* address is aligned */
    Size = 1; /* size is not aligned */

    /* Execute the function being tested */
    Result = MD_Verify32Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    Addr = 1; /* address is not aligned */
    Size = 0; /* size is aligned */

    /* Execute the function being tested */
    Result = MD_Verify32Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");
}

void MD_Verify16Aligned_Test(void)
{
    bool    Result;
    cpuaddr Addr;
    size_t  Size;

    Addr = 0; /* address is aligned */
    Size = 4; /* size is aligned */

    /* Execute the function being tested */
    Result = MD_Verify16Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    Addr = 0; /* address is aligned */
    Size = 1; /* size is not aligned */

    /* Execute the function being tested */
    Result = MD_Verify16Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    Addr = 1; /* address is not aligned */
    Size = 0; /* size is aligned */

    /* Execute the function being tested */
    Result = MD_Verify16Aligned(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");
}

void MD_ResolveSymAddr_Test(void)
{
    MD_SymAddr_t SymAddr;
    cpuaddr      ResolvedAddr = 0;
    bool         Result;

    memset(&SymAddr, 0, sizeof(SymAddr));

    SymAddr.Offset = 99;

    /* Execute the function being tested */
    Result = MD_ResolveSymAddr(&SymAddr, &ResolvedAddr);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(ResolvedAddr == SymAddr.Offset, "ResolvedAddr == SymAddr.Offset");

    ResolvedAddr = 0;
    strncpy(SymAddr.SymName, "symname", OS_MAX_PATH_LEN);

    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &ResolvedAddr, sizeof(ResolvedAddr), false);
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);

    /* Execute the function being tested */
    Result = MD_ResolveSymAddr(&SymAddr, &ResolvedAddr);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");
    UtAssert_True(ResolvedAddr == SymAddr.Offset, "ResolvedAddr == SymAddr.Offset");

    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), -1);

    /* Execute the function being tested */
    Result = MD_ResolveSymAddr(&SymAddr, &ResolvedAddr);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");
}

void UtTest_Setup(void)
{
    UtTest_Add(MD_TableIsInMask_Test_ShiftOddResult, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableIsInMask_Test_ShiftOddResult");
    UtTest_Add(MD_TableIsInMask_Test_ShiftEvenResult, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableIsInMask_Test_ShiftEvenResult");
    UtTest_Add(MD_TableIsInMask_Test_TableNotInMask, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableIsInMask_Test_TableNotInMask");

    UtTest_Add(MD_UpdateDwellControlInfo_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateDwellControlInfo_Test");
    UtTest_Add(MD_UpdateDwellControlInfo_TestAllTableEntries, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateDwellControlInfo_TestAllTableEntries");

    UtTest_Add(MD_ValidEntryId_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidEntryId_Test_Valid");
    UtTest_Add(MD_ValidEntryId_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidEntryId_Test_Invalid");
    UtTest_Add(MD_ValidEntryId_Test_RangeError, MD_Test_Setup, MD_Test_TearDown, "MD_ValidEntryId_Test_RangeError");

    UtTest_Add(MD_ValidAddrRange_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidAddrRange_Test_Valid");
    UtTest_Add(MD_ValidAddrRange_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidAddrRange_Test_Invalid");

    UtTest_Add(MD_ValidTableId_Test_Valid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableId_Test_Valid");
    UtTest_Add(MD_ValidTableId_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableId_Test_Invalid");
    UtTest_Add(MD_ValidTableId_Test_RangeError, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableId_Test_RangeError");

    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength0, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidFieldLength_Test_ValidFieldLength0");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength1, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidFieldLength_Test_ValidFieldLength1");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength2, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidFieldLength_Test_ValidFieldLength2");
    UtTest_Add(MD_ValidFieldLength_Test_ValidFieldLength4, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidFieldLength_Test_ValidFieldLength4");
    UtTest_Add(MD_ValidFieldLength_Test_Invalid, MD_Test_Setup, MD_Test_TearDown, "MD_ValidFieldLength_Test_Invalid");

    UtTest_Add(MD_Verify32Aligned_Test, MD_Test_Setup, MD_Test_TearDown, "MD_Verify32Aligned_Test");
    UtTest_Add(MD_Verify16Aligned_Test, MD_Test_Setup, MD_Test_TearDown, "MD_Verify16Aligned_Test");

    UtTest_Add(MD_ResolveSymAddr_Test, MD_Test_Setup, MD_Test_TearDown, "MD_ResolveSymAddr_Test");
}
