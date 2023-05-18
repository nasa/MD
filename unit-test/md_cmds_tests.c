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

#include "md_cmds.h"
#include "md_dwell_tbl.h"
#include "md_utils.h"
#include "md_msg.h"
#include "md_msgdefs.h"
#include "md_events.h"
#include "md_version.h"
#include "md_test_utils.h"
#include "md_platform_cfg.h"
#include <unistd.h>
#include <stdlib.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* md_cmds_tests globals */
uint8 call_count_CFE_EVS_SendEvent;

/*
 * Function Definitions
 */

CFE_Status_t MD_CMDS_TEST_CFE_TBL_GetAddressHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                 const UT_StubContext_t *Context)
{
    void **TblPtr = (void **)Context->ArgPtr[0];

    *TblPtr = &MD_AppData.MD_DwellTables[0];

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    /* Return value doesn't matter in this case */
    return CFE_TBL_INFO_UPDATED;
}

int32 MD_CMDS_TEST_MD_UpdateDwellControlInfoHook1(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                  const UT_StubContext_t *Context)
{
    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    MD_AppData.MD_DwellTables[0].Rate = 0;

    /* Return value doesn't matter in this case */
    return 0;
}

int32 MD_CMDS_TEST_MD_UpdateDwellControlInfoHook2(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                  const UT_StubContext_t *Context)
{
    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_DISABLED;

    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Return value doesn't matter in this case */
    return 0;
}

void MD_ProcessStartCmd_Test_ZeroRate(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table command processed successfully for table mask 0x%%04X");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 0;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStartCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_ZERO_RATE_CMD_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_START_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStartCmd_Test_Success(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table command processed successfully for table mask 0x%%04X");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStartCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_START_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStartCmd_Test_EmptyTableMask(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s command rejected because no tables were specified in table mask (0x%%04X)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStartCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_EMPTY_TBLMASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStartCmd_Test_NoUpdateTableEnabledField(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table for mask 0x%%04X failed for %%d of %%d tables");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask  = 1;
    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableEnabledField), 1, -1);

    /* Execute the function being tested */
    MD_ProcessStartCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_START_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStopCmd_Test_Success(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Stop Dwell Table command processed successfully for table mask 0x%%04X");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStopCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 0, "MD_AppData.MD_DwellTables[0].Countdown == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_STOP_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStopCmd_Test_EmptyTableMask(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s command rejected because no tables were specified in table mask (0x%%04X)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStopCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_EMPTY_TBLMASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessStopCmd_Test_NoUpdateTableEnabledField(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Stop Dwell Table for mask 0x%%04X failed for %%d of %%d tables");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdStartStop.TableMask = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableEnabledField), 1, -1);

    /* Execute the function being tested */
    MD_ProcessStopCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 0, "MD_AppData.MD_DwellTables[0].Countdown == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_STOP_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_InvalidJamTable(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Tbl Id arg = %%d (Expect 1.. %%d)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId = 99;
    UT_CmdBuf.CmdJam.EntryId = 2;

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_JAM_TABLE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_InvalidEntryArg(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Entry Id arg = %%d (Expect 1.. %%d)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId = 1;
    UT_CmdBuf.CmdJam.EntryId = 99;

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_ENTRY_ARG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_SuccessNullZeroRate(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Successful Jam of a Null Dwell Entry to Dwell Tbl#%%d Entry #%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 0;

    MD_AppData.MD_DwellTables[0].Rate    = 1;
    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    /* Prevents segmentation fault in call to subfunction MD_UpdateDwellControlInfo */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook1, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, CFE_SUCCESS);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[1].Delay == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_NULL_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_ZERO_RATE_CMD_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_NullTableDwell(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Failed Jam of a Null Dwell Entry to Dwell Tbl#%%d Entry #%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 0;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == true" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, -1);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_NULL_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_NoUpdateTableDwell(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Failed Jam to Dwell Tbl#%%d Entry #%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == true" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, -1);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 1,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 1");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_CantResolveJamAddr(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because symbolic address '%%s' couldn't be resolved");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 1;

    strncpy(UT_CmdBuf.CmdJam.DwellAddress.SymName, "address", sizeof(UT_CmdBuf.CmdJam.DwellAddress.SymName) - 1);

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == FALSE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, false);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CANT_RESOLVE_JAM_ADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_InvalidLenArg(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Field Length arg = %%d (Expect 0,1,2,or 4)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_LEN_ARG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_InvalidJamAddr(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not in a valid range");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_JAM_ADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_ENFORCE_DWORD_ALIGN == 0
void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 16-bit aligned");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId             = 1;
    UT_CmdBuf.CmdJam.EntryId             = 2;
    UT_CmdBuf.CmdJam.FieldLength         = 4;
    UT_CmdBuf.CmdJam.DwellAddress.Offset = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_ADDR_NOT_16BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

#if MD_ENFORCE_DWORD_ALIGN == 1
void MD_ProcessJamCmd_Test_JamAddrNot32Bit(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 32-bit aligned");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId             = 1;
    UT_CmdBuf.CmdJam.EntryId             = 2;
    UT_CmdBuf.CmdJam.FieldLength         = 4;
    UT_CmdBuf.CmdJam.DwellAddress.Offset = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_ADDR_NOT_32BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 16-bit aligned");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId             = 1;
    UT_CmdBuf.CmdJam.EntryId             = 2;
    UT_CmdBuf.CmdJam.FieldLength         = 2;
    UT_CmdBuf.CmdJam.DwellAddress.Offset = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_ADDR_NOT_16BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_JamAddrNot16BitNot32Aligned(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 32-bit aligned");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId             = 1;
    UT_CmdBuf.CmdJam.EntryId             = 2;
    UT_CmdBuf.CmdJam.FieldLength         = 4;
    UT_CmdBuf.CmdJam.DwellAddress.Offset = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_Verify32Aligned), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_ADDR_NOT_32BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_JamFieldLength4Addr32Aligned(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId             = 1;
    UT_CmdBuf.CmdJam.EntryId             = 2;
    UT_CmdBuf.CmdJam.FieldLength         = 4;
    UT_CmdBuf.CmdJam.DwellAddress.Offset = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_Verify32Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_SuccessNonNullZeroRate(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 2;
    UT_CmdBuf.CmdJam.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook1, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 3,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_ZERO_RATE_CMD_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_SuccessZeroRateStreamDisabled(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 2;
    UT_CmdBuf.CmdJam.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 3,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ProcessJamCmd_Test_SuccessRateNotZero(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdJam.TableId     = 1;
    UT_CmdBuf.CmdJam.EntryId     = 2;
    UT_CmdBuf.CmdJam.FieldLength = 2;
    UT_CmdBuf.CmdJam.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook2, NULL);

    /* Set to satisfy condition "MD_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(MD_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 3,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureLength(void)
{
    uint16         i;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set Signature cmd rejected due to invalid Signature length");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    for (i = 0; i < sizeof(UT_CmdBuf.CmdSetSignature.Signature); i++)
    {
        UT_CmdBuf.CmdSetSignature.Signature[i] = 'x';
    }

    /* Execute the function being tested */
    MD_ProcessSignatureCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_SIGNATURE_LENGTH_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureTable(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set Signature cmd rejected due to invalid Tbl Id arg = %%d (Expect 1.. %%d)");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdSetSignature.TableId = 0;

    /* Execute the function being tested */
    MD_ProcessSignatureCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INVALID_SIGNATURE_TABLE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_Success(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Successfully set signature for Dwell Tbl#%%d to '%%s'");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdSetSignature.TableId = 1;
    strncpy(UT_CmdBuf.CmdSetSignature.Signature, "signature", sizeof(UT_CmdBuf.CmdSetSignature.Signature) - 1);

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableSignature */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);

    /* Execute the function being tested */
    MD_ProcessSignatureCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(strncmp(MD_AppData.MD_DwellTables[0].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
                  "strncmp(MD_AppData.MD_DwellTables[0].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_SET_SIGNATURE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_NoUpdateTableSignature(void)
{
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Failed to set signature for Dwell Tbl#%%d. Update returned 0x%%08X");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    UT_CmdBuf.CmdSetSignature.TableId = 1;
    strncpy(UT_CmdBuf.CmdSetSignature.Signature, "signature", sizeof(UT_CmdBuf.CmdSetSignature.Signature) - 1);

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableSignature */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableSignature), 1, true);

    /* Execute the function being tested */
    MD_ProcessSignatureCmd(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(strncmp(MD_AppData.MD_DwellTables[0].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
                  "strncmp(MD_AppData.MD_DwellTables[0].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_SET_SIGNATURE_ERR_EID);
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
    UtTest_Add(MD_ProcessStartCmd_Test_ZeroRate, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStartCmd_Test_ZeroRate");
    UtTest_Add(MD_ProcessStartCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStartCmd_Test_Success");
    UtTest_Add(MD_ProcessStartCmd_Test_EmptyTableMask, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessStartCmd_Test_EmptyTableMask");
    UtTest_Add(MD_ProcessStartCmd_Test_NoUpdateTableEnabledField, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessStartCmd_Test_NoUpdateTableEnabledField");

    UtTest_Add(MD_ProcessStopCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessStopCmd_Test_Success");
    UtTest_Add(MD_ProcessStopCmd_Test_EmptyTableMask, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessStopCmd_Test_EmptyTableMask");
    UtTest_Add(MD_ProcessStopCmd_Test_NoUpdateTableEnabledField, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessStopCmd_Test_NoUpdateTableEnabledField");

    UtTest_Add(MD_ProcessJamCmd_Test_InvalidJamTable, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_InvalidJamTable");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidEntryArg, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_InvalidEntryArg");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessNullZeroRate, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_SuccessNullZeroRate");
    UtTest_Add(MD_ProcessJamCmd_Test_NullTableDwell, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_NullTableDwell");
    UtTest_Add(MD_ProcessJamCmd_Test_NoUpdateTableDwell, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_SuccessNullZeroRate");
    UtTest_Add(MD_ProcessJamCmd_Test_CantResolveJamAddr, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_CantResolveJamAddr");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidLenArg, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_InvalidLenArg");
    UtTest_Add(MD_ProcessJamCmd_Test_InvalidJamAddr, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_InvalidJamAddr");

#if MD_ENFORCE_DWORD_ALIGN == 0
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4");
#endif

#if MD_ENFORCE_DWORD_ALIGN == 1
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot32Bit, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_JamAddrNot32Bit");
#endif

    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2");
    UtTest_Add(MD_ProcessJamCmd_Test_JamAddrNot16BitNot32Aligned, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_JamAddrNot16BitNot32Aligned");
    UtTest_Add(MD_ProcessJamCmd_Test_JamFieldLength4Addr32Aligned, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_JamFieldLength4Addr32Aligned");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessNonNullZeroRate, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_SuccessNonNullZeroRate");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessZeroRateStreamDisabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_SuccessZeroRateStreamDisabled");
    UtTest_Add(MD_ProcessJamCmd_Test_SuccessRateNotZero, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessJamCmd_Test_SuccessRateNotZero");
#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ProcessSignatureCmd_Test_InvalidSignatureLength, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessSignatureCmd_Test_InvalidSignatureLength");
    UtTest_Add(MD_ProcessSignatureCmd_Test_InvalidSignatureTable, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessSignatureCmd_Test_InvalidSignatureTable");
    UtTest_Add(MD_ProcessSignatureCmd_Test_Success, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessSignatureCmd_Test_Success");
    UtTest_Add(MD_ProcessSignatureCmd_Test_NoUpdateTableSignature, MD_Test_Setup, MD_Test_TearDown,
               "MD_ProcessSignatureCmd_Test_NoUpdateTableSignature");
#endif
}
