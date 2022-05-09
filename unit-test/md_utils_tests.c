
/*
 * Includes
 */

#include "md_utils.h"
#include "md_msg.h"
#include "md_msgdefs.h"
#include "md_events.h"
#include "md_version.h"
#include "md_test_utils.h"
#include <sys/fcntl.h>
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

} /* end MD_TableIsInMask_Test_ShiftOddResult */

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

} /* end MD_TableIsInMask_Test_ShiftEvenResult */

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

} /* end MD_TableIsInMask_Test_TableNotInMask */

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

} /* end MD_UpdateDwellControlInfo_Test */

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

} /* end MD_UpdateDwellControlInfo_TestAllTableEntries */

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

} /* end MD_ValidEntryId_Test_Valid */

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

} /* end MD_ValidEntryId_Test_Invalid */

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

} /* end MD_ValidEntryId_Test_RangeError */

void MD_ValidAddrRange_Test_Valid(void)
{
    bool    Result;
    cpuaddr Addr = 1;
    uint32  Size = 1;

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == true, "Result == true");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ValidAddrRange_Test_Valid */

void MD_ValidAddrRange_Test_Invalid(void)
{
    bool    Result;
    cpuaddr Addr = 1;
    uint32  Size = 1;

    /* Set to reach "IsValid = false" */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = MD_ValidAddrRange(Addr, Size);

    /* Verify results */
    UtAssert_True(Result == false, "Result == false");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ValidAddrRange_Test_Invalid */

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

} /* end MD_ValidTableId_Test_Valid */

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

} /* end MD_ValidTableId_Test_Invalid */

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

} /* end MD_ValidTableId_Test_RangeError */

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

} /* end MD_ValidFieldLength_Test_ValidFieldLength0 */

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

} /* end MD_ValidFieldLength_Test_ValidFieldLength1 */

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

} /* end MD_ValidFieldLength_Test_ValidFieldLength2 */

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

} /* end MD_ValidFieldLength_Test_ValidFieldLength4 */

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

} /* end MD_ValidFieldLength_Test_Invalid */

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

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
