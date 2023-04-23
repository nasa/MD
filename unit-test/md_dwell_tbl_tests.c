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

#include "md_dwell_tbl.h"
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

/* md_dwell_tbl_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

MD_DwellTableLoad_t MD_DWELL_TBL_TEST_GlobalLoadTable;

void MD_TableValidationFunc_Test_InvalidEnableFlag(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because value of enable flag (%%d) is invalid");

    Table.Enabled = 99;

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_TBL_ENA_FLAG_ERROR, "Result == MD_TBL_ENA_FLAG_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_TBL_ENA_FLAG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_SIGNATURE_OPTION == 1
void MD_TableValidationFunc_Test_InvalidSignatureLength(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because Signature length was invalid");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;
    memset(Table.Signature, 'x', sizeof(Table.Signature));

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_SIG_LEN_TBL_ERROR, "Result == MD_SIG_LEN_TBL_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_TBL_SIG_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

void MD_TableValidationFunc_Test_ResolveError(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because address (sym='%%s'/offset=0x%%08X) in entry #%%d couldn't be resolved");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].Length              = 1;
    Table.Entry[0].DwellAddress.Offset = 0;

    /* Set to make MD_CheckTableEntries return MD_RESOLVE_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), false);
    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_RESOLVE_ERROR, "Result == MD_RESOLVE_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_RESOLVE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 message we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_InvalidAddress(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because address (sym='%%s'/offset=0x%%08X) in entry #%%d was out of range");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;
    Table.Entry[0].Length              = 1;

    /* Set to make MD_CheckTableEntries return MD_RESOLVE_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), false);

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    /* Set to make MD_CheckTableEntries return MD_INVALID_ADDR_ERROR */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_RANGE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 1 message we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_NullPtr(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because of null table pointer");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(NULL);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_TBL_VAL_NULL_PTR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    /* Generates 1 message we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_InvalidLength(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because length (%%d) in entry #%%d was invalid");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 0;
    Table.Entry[0].Length              = 5;

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_LEN_ERROR, "Result == MD_INVALID_LEN_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_TBL_HAS_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 messages we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_NotAligned(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table rejected because address (sym='%%s'/offset=0x%%08X) in entry #%%d not properly aligned for "
             "%%d-byte dwell");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    Table.Entry[0].DwellAddress.Offset = 1;
    Table.Entry[0].Length              = 2;

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_TBL_ALIGN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 messages we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_ZeroRate(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              i;
    int32               strCmpResult;
    char                ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table is enabled but no processing will occur for table being loaded (rate is zero)");

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length              = 1;
        Table.Entry[i].Delay               = 0;
    }

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_ZERO_RATE_TBL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    /* Generates 1 message we don't care about */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_SuccessStreamDisabled(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              i;

    Table.Enabled = MD_DWELL_STREAM_DISABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length              = 1;
        Table.Entry[i].Delay               = 1;
    }

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Generates 1 message we don't care about here */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_TableValidationFunc_Test_Success(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              i;

    Table.Enabled = MD_DWELL_STREAM_ENABLED;

#if MD_SIGNATURE_OPTION == 1
    strncpy(Table.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    strncpy(Table.Entry[0].DwellAddress.SymName, "symname", 10);

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length              = 1;
        Table.Entry[i].Delay               = 1;
    }

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    /* Execute the function being tested */
    Result = MD_TableValidationFunc(&Table);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    /* Generates 1 message we don't care about here */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ReadDwellTable_Test(void)
{
    CFE_Status_t        Result;
    MD_DwellTableLoad_t Table;
    uint16              i;
    uint16              ActiveAddrCount = 0;
    uint16              Size            = 0;
    uint32              Rate            = 0;

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].Length = 1;
        Table.Entry[i].Delay  = 1;
    }

    /* Execute the function being tested */
    Result = MD_ReadDwellTable(&Table, &ActiveAddrCount, &Size, &Rate);

    /* Verify results */
    UtAssert_True(ActiveAddrCount == MD_DWELL_TABLE_SIZE, "ActiveAddrCount == MD_DWELL_TABLE_SIZE");
    UtAssert_True(Size == MD_DWELL_TABLE_SIZE, "Size == MD_DWELL_TABLE_SIZE");
    UtAssert_True(Rate == MD_DWELL_TABLE_SIZE, "Rate == MD_DWELL_TABLE_SIZE");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_CheckTableEntries_Test_Error(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              ErrorEntryArg = 0;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD Dwell Tbl verify results: good = %%d, bad = %%d, unused = %%d");

    Table.Entry[0].Length              = 2;
    Table.Entry[1].Length              = 2;
    Table.Entry[2].Length              = 2;
    Table.Entry[0].DwellAddress.Offset = 0;
    Table.Entry[1].DwellAddress.Offset = 0;
    Table.Entry[2].DwellAddress.Offset = 0;
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_Verify16Aligned), true);

    /* Allow first entry to succeed and next 2 to fail */
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 2, false);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, false);

    /* Execute the function being tested */
    Result = MD_CheckTableEntries(&Table, &ErrorEntryArg);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_DWELL_TBL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);
    UtAssert_INT32_EQ(ErrorEntryArg, 1);
    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_CheckTableEntries_Test_MultiError(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              ErrorEntryArg = 0;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    memset(&Table, 0, sizeof(Table));

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD Dwell Tbl verify results: good = %%d, bad = %%d, unused = %%d");

    Table.Entry[0].Length = 5;

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), false);

    /* Execute the function being tested */
    Result = MD_CheckTableEntries(&Table, &ErrorEntryArg);

    /* Verify results */
    UtAssert_True(ErrorEntryArg == 0, "ErrorEntryArg == 0");

    UtAssert_True(Result == MD_RESOLVE_ERROR, "Result == MD_RESOLVE_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_DWELL_TBL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_CheckTableEntries_Test_Success(void)
{
    int32               Result;
    MD_DwellTableLoad_t Table;
    uint16              ErrorEntryArg = 0;
    uint16              i;
    int32               strCmpResult;
    char                ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD Dwell Tbl verify results: good = %%d, bad = %%d, unused = %%d");

    for (i = 0; i < MD_DWELL_TABLE_SIZE; i++)
    {
        Table.Entry[i].DwellAddress.Offset = 0;
        Table.Entry[i].Length              = 1;
        Table.Entry[i].Delay               = 1;
    }

    /* Test the 'unused' entry count */
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].DwellAddress.Offset = 0;
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].Length              = 0;
    Table.Entry[MD_DWELL_TABLE_SIZE - 1].Delay               = 1;

    /* Set to make MD_CheckTableEntries return SUCCESS */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    /* Execute the function being tested */
    Result = MD_CheckTableEntries(&Table, &ErrorEntryArg);

    /* Verify results */
    UtAssert_True(ErrorEntryArg == 0, "ErrorEntryArg == 0");

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_DWELL_TBL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_SuccessDwellLengthZero(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length = 0;

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_ResolveError(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length = 1;

    /* Set to make MD_ResolveSymAddr return MD_RESOLVE_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), false);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_RESOLVE_ERROR, "Result == MD_RESOLVE_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_InvalidAddress(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length = 1;

    /* Set to make MD_ValidAddrRange return MD_INVALID_ADDR_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_ADDR_ERROR, "Result == MD_INVALID_ADDR_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_InvalidLength(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length = -1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), false);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_INVALID_LEN_ERROR, "Result == MD_INVALID_LEN_ERROR 0x%x", Result);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_SIGNATURE_OPTION == 1
void MD_ValidTableEntry_Test_NotAligned16DwellLength4(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 4;
    Entry.DwellAddress.Offset = 1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_Aligned32(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 4;
    Entry.DwellAddress.Offset = 1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    UT_SetDeferredRetcode(UT_KEY(MD_Verify32Aligned), 1, true);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_NotAligned32(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 4;
    Entry.DwellAddress.Offset = 1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);

    UT_SetDeferredRetcode(UT_KEY(MD_Verify32Aligned), 1, false);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}
#endif

#if MD_SIGNATURE_OPTION == 0
void MD_ValidTableEntry_Test_NotAligned32(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 4;
    Entry.DwellAddress.Offset = 1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), false);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}
#endif

void MD_ValidTableEntry_Test_NotAligned16DwellLength2(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 2;
    Entry.DwellAddress.Offset = 1;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_Verify16Aligned), false);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == MD_NOT_ALIGNED_ERROR, "Result == MD_NOT_ALIGNED_ERROR 0x%x", Result);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ValidTableEntry_Test_ElseSuccess(void)
{
    int32               Result;
    MD_TableLoadEntry_t Entry;

    Entry.Length              = 2;
    Entry.DwellAddress.Offset = 0;

    UT_SetDefaultReturnValue(UT_KEY(MD_ResolveSymAddr), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidAddrRange), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_ValidFieldLength), true);
    UT_SetDefaultReturnValue(UT_KEY(MD_Verify16Aligned), true);

    /* Execute the function being tested */
    Result = MD_ValidTableEntry(&Entry);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_CopyUpdatedTbl_Test(void)
{
    MD_DwellTableLoad_t LoadTable;
    uint8               TblIndex = 0;

    LoadTable.Enabled = 1;

#if MD_SIGNATURE_OPTION == 1
    strncpy(LoadTable.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);
#endif

    /* Execute the function being tested */
    MD_CopyUpdatedTbl(&LoadTable, TblIndex);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].Enabled == 1, "MD_AppData.MD_DwellTables[TblIndex].Enabled == 1");

#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(
        strncmp(MD_AppData.MD_DwellTables[TblIndex].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
        "strncmp(MD_AppData.MD_DwellTables[TblIndex].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[TblIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == '\0',
                  "MD_AppData.MD_DwellTables[TblIndex].Signature[MD_SIGNATURE_FIELD_LENGTH - 1] == ''");
#endif

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableEnabledField_Test_DwellStreamEnabled(void)
{
    uint16               TableIndex = 0;
    uint16               FieldValue = MD_DWELL_STREAM_ENABLED;
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    /* UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook2, NULL); */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);
    LoadTblPtr->Enabled = MD_DWELL_STREAM_DISABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableEnabledField(TableIndex, FieldValue);

    /* Verify results */
    UtAssert_True(LoadTblPtr->Enabled == MD_DWELL_STREAM_ENABLED, "LoadTblPtr->Enabled == MD_DWELL_STREAM_ENABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableEnabledField_Test_DwellStreamDisabled(void)
{
    uint16               TableIndex = 0;
    uint16               FieldValue = MD_DWELL_STREAM_DISABLED;
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);
    LoadTblPtr->Enabled = MD_DWELL_STREAM_ENABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableEnabledField(TableIndex, FieldValue);

    /* Verify results */
    UtAssert_True(LoadTblPtr->Enabled == MD_DWELL_STREAM_DISABLED, "LoadTblPtr->Enabled == MD_DWELL_STREAM_DISABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableEnabledField_Test_Error(void)
{
    uint16 TableIndex = 0;
    uint16 FieldValue = MD_DWELL_STREAM_DISABLED;
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD_UpdateTableEnabledField, TableIndex %%d: CFE_TBL_GetAddress Returned 0x%%08x");

    /* Set to make CFE_TBL_GetAddress != CFE_SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    MD_UpdateTableEnabledField(TableIndex, FieldValue);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_UPDATE_TBL_EN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableDwellEntry_Test(void)
{
    uint16       TableIndex = 0;
    uint16       EntryIndex = 0;
    uint16       NewLength  = 1;
    uint16       NewDelay   = 1;
    MD_SymAddr_t NewDwellAddress;

    NewDwellAddress.Offset = 1;

    strncpy(NewDwellAddress.SymName, "symname", 10);

    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableDwellEntry(TableIndex, EntryIndex, NewLength, NewDelay, NewDwellAddress);

    /* Verify results */
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].Length == 1, "LoadTblPtr->Entry[EntryIndex].Length == 1");
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].Delay == 1, "LoadTblPtr->Entry[EntryIndex].Delay == 1");
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].DwellAddress.Offset == 1,
                  "LoadTablePtr->Entry[EntryIndex].DwellAddress.Offset == 1");

    UtAssert_True(strncmp(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName, "symname", 10) == 0,
                  "strncmp(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName, 'symname', 10) == 0");

    UtAssert_True(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableDwellEntry_Test_Updated(void)
{
    uint16       TableIndex = 0;
    uint16       EntryIndex = 0;
    uint16       NewLength  = 1;
    uint16       NewDelay   = 1;
    MD_SymAddr_t NewDwellAddress;

    NewDwellAddress.Offset = 1;

    strncpy(NewDwellAddress.SymName, "symname", 10);

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableDwellEntry(TableIndex, EntryIndex, NewLength, NewDelay, NewDwellAddress);

    /* Verify results */
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].Length == 1, "LoadTblPtr->Entry[EntryIndex].Length == 1");
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].Delay == 1, "LoadTblPtr->Entry[EntryIndex].Delay == 1");
    UtAssert_True(LoadTblPtr->Entry[EntryIndex].DwellAddress.Offset == 1,
                  "LoadTablePtr->Entry[EntryIndex].DwellAddress.Offset == 1");

    UtAssert_True(strncmp(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName, "symname", 10) == 0,
                  "strncmp(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName, 'symname', 10) == 0");

    UtAssert_True(LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "LoadTblPtr->Entry[EntryIndex].DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableDwellEntry_Test_Error(void)
{
    uint16       TableIndex = 0;
    uint16       EntryIndex = 0;
    uint16       NewLength  = 1;
    uint16       NewDelay   = 1;
    MD_SymAddr_t NewDwellAddress;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD_UpdateTableDwellEntry, TableIndex %%d: CFE_TBL_GetAddress Returned 0x%%08x");

    NewDwellAddress.Offset = 1;

    strncpy(NewDwellAddress.SymName, "symname", 10);

    /* Set to make CFE_TBL_GetAddress != CFE_SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    MD_UpdateTableDwellEntry(TableIndex, EntryIndex, NewLength, NewDelay, NewDwellAddress);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_UPDATE_TBL_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_SIGNATURE_OPTION == 1
void MD_UpdateTableSignature_Test(void)
{
    uint16 TableIndex     = 0;
    char   newsignature[] = "newsignature";

    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableSignature(TableIndex, newsignature);

    /* Verify results */
    UtAssert_True(strncmp(LoadTblPtr->Signature, "newsignature", 15) == 0,
                  "strncmp(LoadTblPtr->Signature, 'newsignature', 15) == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableSignature_Test_Updated(void)
{
    uint16 TableIndex     = 0;
    char   newsignature[] = "newsignature";

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    MD_UpdateTableSignature(TableIndex, newsignature);

    /* Verify results */
    UtAssert_True(strncmp(LoadTblPtr->Signature, "newsignature", 15) == 0,
                  "strncmp(LoadTblPtr->Signature, 'newsignature', 15) == 0");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_UpdateTableSignature_Test_Error(void)
{
    uint16 TableIndex     = 0;
    char   newsignature[] = "newsignature";
    int32  strCmpResult;
    char   ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD_UpdateTableSignature, TableIndex %%d: CFE_TBL_GetAddress Returned 0x%%08x");

    /* Set to make CFE_TBL_GetAddress != CFE_SUCCESS */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    MD_UpdateTableSignature(TableIndex, newsignature);

    /* Verify results */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_UPDATE_TBL_SIG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#endif

void UtTest_Setup(void)
{
    UtTest_Add(MD_TableValidationFunc_Test_InvalidEnableFlag, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_InvalidEnableFlag");
#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_TableValidationFunc_Test_InvalidSignatureLength, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_InvalidSignatureLength");
#endif

    UtTest_Add(MD_TableValidationFunc_Test_ResolveError, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_ResolveError");
    UtTest_Add(MD_TableValidationFunc_Test_InvalidAddress, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_InvalidAddress");
    UtTest_Add(MD_TableValidationFunc_Test_NullPtr, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_NullPtr");
    UtTest_Add(MD_TableValidationFunc_Test_InvalidLength, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_InvalidLength");
    UtTest_Add(MD_TableValidationFunc_Test_NotAligned, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_NotAligned");
    UtTest_Add(MD_TableValidationFunc_Test_ZeroRate, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_ZeroRate");
    UtTest_Add(MD_TableValidationFunc_Test_SuccessStreamDisabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_SuccessStreamDisabled");
    UtTest_Add(MD_TableValidationFunc_Test_Success, MD_Test_Setup, MD_Test_TearDown,
               "MD_TableValidationFunc_Test_Success");
    UtTest_Add(MD_ReadDwellTable_Test, MD_Test_Setup, MD_Test_TearDown, "MD_ReadDwellTable_Test");

    UtTest_Add(MD_CheckTableEntries_Test_Error, MD_Test_Setup, MD_Test_TearDown, "MD_CheckTableEntries_Test_Error");
    UtTest_Add(MD_CheckTableEntries_Test_MultiError, MD_Test_Setup, MD_Test_TearDown,
               "MD_CheckTableEntries_Test_MultiError");
    UtTest_Add(MD_CheckTableEntries_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_CheckTableEntries_Test_Success");

    UtTest_Add(MD_ValidTableEntry_Test_SuccessDwellLengthZero, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_SuccessDwellLengthZero");
    UtTest_Add(MD_ValidTableEntry_Test_ResolveError, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_ResolveError");
    UtTest_Add(MD_ValidTableEntry_Test_InvalidAddress, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_InvalidAddress");
    UtTest_Add(MD_ValidTableEntry_Test_InvalidLength, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_InvalidLength");

#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned16DwellLength4, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_NotAligned16DwellLength4");
    UtTest_Add(MD_ValidTableEntry_Test_Aligned32, MD_Test_Setup, MD_Test_TearDown, "MD_ValidTableEntry_Test_Aligned32");
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned32, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_NotAligned32");
    UtTest_Add(MD_ValidTableEntry_Test_NotAligned16DwellLength2, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_NotAligned16DwellLength2");
#endif

    UtTest_Add(MD_ValidTableEntry_Test_ElseSuccess, MD_Test_Setup, MD_Test_TearDown,
               "MD_ValidTableEntry_Test_ElseSuccess");

    UtTest_Add(MD_CopyUpdatedTbl_Test, MD_Test_Setup, MD_Test_TearDown, "MD_CopyUpdatedTbl_Test");

    UtTest_Add(MD_UpdateTableEnabledField_Test_DwellStreamEnabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableEnabledField_Test_DwellStreamEnabled");
    UtTest_Add(MD_UpdateTableEnabledField_Test_DwellStreamDisabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableEnabledField_Test_DwellStreamDisabled");
    UtTest_Add(MD_UpdateTableEnabledField_Test_Error, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableEnabledField_Test_Error");

    UtTest_Add(MD_UpdateTableDwellEntry_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableDwellEntry_Test");
    UtTest_Add(MD_UpdateTableDwellEntry_Test_Updated, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableDwellEntry_Test_Updated");
    UtTest_Add(MD_UpdateTableDwellEntry_Test_Error, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableDwellEntry_Test_Error");

#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_UpdateTableSignature_Test, MD_Test_Setup, MD_Test_TearDown, "MD_UpdateTableSignature_Test");
    UtTest_Add(MD_UpdateTableSignature_Test_Updated, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableSignature_Test_Updated");
    UtTest_Add(MD_UpdateTableSignature_Test_Error, MD_Test_Setup, MD_Test_TearDown,
               "MD_UpdateTableSignature_Test_Error");
#endif
}
