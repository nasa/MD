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

#include "md_app.h"
#include "md_dwell_tbl.h"
#include "md_dwell_pkt.h"
#include "md_msg.h"
#include "md_msgdefs.h"
#include "md_events.h"
#include "md_version.h"
#include "md_test_utils.h"
#include <unistd.h>
#include <stdlib.h>

#include "stub_stdio.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* md_app_tests globals */
uint8               call_count_CFE_EVS_SendEvent;
MD_DwellTableLoad_t MD_DWELL_TBL_TEST_GlobalLoadTable;

CFE_Status_t MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                      const UT_StubContext_t *Context)
{
    void **TblPtr                             = (void **)Context->ArgPtr[0];
    MD_DWELL_TBL_TEST_GlobalLoadTable.Enabled = MD_DWELL_STREAM_ENABLED;
    *TblPtr                                   = &MD_DWELL_TBL_TEST_GlobalLoadTable;

    return CFE_SUCCESS;
}

void MD_AppMain_Test_AppInitError(void)
{
    int32 strCmpResult;
    char  ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "MD:Application Init Failed,RC=%%d\n");

    /* Set to make MD_AppInit return -1, in order to generate log message "Application Init Failed" */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR, "MD_AppData.RunStatus == CFE_ES_APP_ERROR");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    /* Generates 1 log message we don't care about in this test */
    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void MD_AppMain_Test_RcvMsgError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "SB Pipe Read Error, App will exit. Pipe Return Status = %%d");

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to generate error message MD_PIPE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, -1);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR,
                  "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_ERROR");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_PIPE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_RcvMsgTimeout(void)
{
    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to generate error message MD_PIPE_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_TIME_OUT);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    /* Generates 1 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_RcvMsgNullBuffer(void)
{
    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SUCCESS);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Generates 2 event messages we don't care about in this test */
    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_WakeupNominal(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    MD_NoArgsCmd_t  Packet;
    MD_NoArgsCmd_t *TempBuf = &Packet;

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to satisfy subsequent condition "Result == CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SUCCESS);

    /* Set to provide a non-null buffer pointer */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);

    TestMsgId = CFE_SB_ValueToMsgId(MD_WAKEUP_MID);
    MsgSize   = sizeof(MD_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    /* Generates 2 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    uint8 call_count_MD_DwellLoop = UT_GetStubCount(UT_KEY(MD_DwellLoop));
    UtAssert_INT32_EQ(call_count_MD_DwellLoop, 1);
}

void MD_AppMain_Test_WakeupLengthError(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_NoArgsCmd_t  Packet;
    MD_NoArgsCmd_t *TempBuf = &Packet;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Msg with Bad length Rcvd: ID = 0x%%08lX, Exp Len = %%u, Len = %%d");

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to provide a non-null buffer pointer */

    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);

    TestMsgId = CFE_SB_ValueToMsgId(MD_WAKEUP_MID);
    MsgSize   = sizeof(MD_NoArgsCmd_t) + 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    /* Generates 1 event message we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_MSG_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);
}

void MD_AppMain_Test_CmdNominal(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    MD_NoArgsCmd_t    Packet;
    MD_NoArgsCmd_t *  TempBuf = &Packet;

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to provide a non-null buffer pointer */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = 0;
    MsgSize   = sizeof(MD_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    /* Generates 3 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_SendHkNominal(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    MD_NoArgsCmd_t  Packet;
    MD_NoArgsCmd_t *TempBuf = &Packet;

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to provide a non-null buffer pointer */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);

    TestMsgId = CFE_SB_ValueToMsgId(MD_SEND_HK_MID);
    MsgSize   = sizeof(MD_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    /* Generates 3 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_SendHkLengthError(void)
{
    CFE_SB_MsgId_t  TestMsgId;
    size_t          MsgSize;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_NoArgsCmd_t  Packet;
    MD_NoArgsCmd_t *TempBuf = &Packet;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Msg with Bad length Rcvd: ID = 0x%%08lX, Exp Len = %%u, Len = %%d");

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to provide a non-null buffer pointer */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);

    TestMsgId = CFE_SB_ValueToMsgId(MD_SEND_HK_MID);
    MsgSize   = 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_MSG_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppMain_Test_InvalidMessageID(void)
{
    CFE_SB_MsgId_t  TestMsgId = MD_UT_MID_1;
    int32           strCmpResult;
    char            ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_NoArgsCmd_t  Packet;
    MD_NoArgsCmd_t *TempBuf = &Packet;

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Msg with Invalid message ID Rcvd -- ID = 0x%%08lX");

    /* Set to prevent unintended error messages */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Set to make loop execute exactly once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    /* Set to provide a non-null buffer pointer */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &TempBuf, sizeof(TempBuf), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    /* Execute the function being tested */
    MD_AppMain();

    /* Verify results */
    UtAssert_True(MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN, "MD_AppData.RunStatus == CFE_ES_RunStatus_APP_RUN");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventID, MD_MID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[2].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[2].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[2].Spec);

    /* Generates 2 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 3, "CFE_EVS_SendEvent was called %u time(s), expected 3",
                  call_count_CFE_EVS_SendEvent);
}

void MD_AppInit_Test_Nominal(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "MD Initialized.  Version %%d.%%d.%%d.%%d");

    MD_AppData.CmdCounter = 1;
    MD_AppData.ErrCounter = 1;

    /* Execute the function being tested */
    Result = MD_AppInit();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    /* Generates 2 event messages we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    UtAssert_INT32_EQ(MD_AppData.CmdCounter, 0);
    UtAssert_INT32_EQ(MD_AppData.ErrCounter, 0);
}

void MD_AppInit_Test_EvsRegisterNotSuccess(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD_APP:Call to CFE_EVS_Register Failed:RC=%%d\n");

    /* Set to make MD_AppInit return -1, in order to generate log message "Application Init Failed" */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);

    /* Execute the function being tested */
    Result = MD_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void MD_AppInit_Test_InitSoftwareBusServicesNotSuccess(void)
{
    CFE_Status_t Result;

    /* Set to make MD_InitSoftwareBusServices return -1 */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);

    /* Execute the function being tested */
    Result = MD_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Generates 1 event message we don't care about in this test */
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    /* Generates 1 system log message we don't care about in this test */
}

void MD_AppInit_Test_InitTableServicesNotSuccess(void)
{
    CFE_Status_t Result;

    /* Set to make MD_InitTableServices return -1 */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Execute the function being tested */
    Result = MD_AppInit();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates 1 event message we don't care about in this test */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitControlStructures_Test(void)
{
    /* Execute the function being tested */
    MD_InitControlStructures();

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == 0, "MD_AppData.MD_DwellTables[0].Enabled == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].AddrCount == 0, "MD_AppData.MD_DwellTables[0].AddrCount == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Rate == 0, "MD_AppData.MD_DwellTables[0].Rate == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].DataSize == 0, "MD_AppData.MD_DwellTables[0].DataSize == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Filler == 0, "MD_AppData.MD_DwellTables[0].Filler == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Enabled == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 1,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Filler == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Filler == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Enabled == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 1,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Filler == 0,
                  "MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Filler == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[0].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[0].Length == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[0].Delay == 0, "MD_AppData.MD_DwellTables[0].Entry[0].Delay == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[0].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[0].ResolvedAddress == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Length == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Delay == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].Delay == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE / 2].ResolvedAddress == 0");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Length == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Delay == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].Delay == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[MD_DWELL_TABLE_SIZE - 1].ResolvedAddress == 0");

#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(strncmp(MD_AppData.MD_DwellTables[0].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0,
                  "strncmp(MD_AppData.MD_DwellTables[0].Signature, "
                  ", MD_SIGNATURE_FIELD_LENGTH) == 0");
    UtAssert_True(
        strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0,
        "strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Signature, "
        ", MD_SIGNATURE_FIELD_LENGTH) == 0");
    UtAssert_True(
        strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Signature, "", MD_SIGNATURE_FIELD_LENGTH) == 0,
        "strncmp(MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Signature, "
        ", MD_SIGNATURE_FIELD_LENGTH) == 0");
#endif

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitSoftwareBusServices_Test_Nominal(void)
{
    CFE_Status_t Result;

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();

    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES / 2].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == ''");
#endif

    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitSoftwareBusServices_Test_CreatePipeError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Failed to create pipe.  RC = %%d");

    /* Set to generate error message "MD_APP: Error creating cmd pipe" */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();

    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES / 2].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == ''");
#endif

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CREATE_PIPE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitSoftwareBusServices_Test_SubscribeHkError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Failed to subscribe to HK requests  RC = %%d");

    /* Set to generate error message MD_SUB_HK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();

    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES / 2].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == ''");
#endif

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_SUB_HK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitSoftwareBusServices_Test_SubscribeCmdError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Failed to subscribe to commands.  RC = %%d");

    /* Set to generate error message MD_SUB_HK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();

    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES / 2].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == ''");
#endif

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_SUB_CMD_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitSoftwareBusServices_Test_SubscribeWakeupError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Failed to subscribe to wakeup messages.  RC = %%d");

    /* Set to generate error message MD_SUB_HK_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 3, -1);

    /* Execute the function being tested */
    Result = MD_InitSoftwareBusServices();

    /* Verify results */
#if MD_SIGNATURE_OPTION == 1
    UtAssert_True(MD_AppData.MD_DwellPkt[0].Signature[0] == '\0', "MD_AppData.MD_DwellPkt[0].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES / 2].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES/2].Signature[0] == ''");
    UtAssert_True(MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == '\0',
                  "MD_AppData.MD_DwellPkt[MD_NUM_DWELL_TABLES - 1].Signature[0] == ''");
#endif

    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_SUB_WAKEUP_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_GetAddressErrorAndLoadError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char         ExpectedSysLogString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Didn't update MD tbl #%%d due to unexpected CFE_TBL_GetAddress return: 0x%%08X");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    snprintf(ExpectedSysLogString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD_APP: Error 0x%%08X received loading tbl#%%d\n");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Set to generate error message "MD_APP: Error 0x%08X received loading tbl#%d" (and to terminate loop after first
     * run) */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Load), 1, -1);

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    UtAssert_True(MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_NO_TBL_COPY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    strCmpResult = strncmp(ExpectedSysLogString, context_CFE_ES_WriteToSysLog.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Sys Log string matched expected result, '%s'", context_CFE_ES_WriteToSysLog.Spec);
}

void MD_InitTableServices_Test_TblRecoveredValidThenTblInits(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[4][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Set MD_LoadTablePtr = &MD_DWELL_TBL_TEST_GlobalLoadTable */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_DWELL_TBL_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_TblRecoveredNotValid(void)
{
    CFE_Status_t         Result;
    int32                strCmpResult;
    char                 ExpectedEventString[4][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    memset(&LoadTbl, 0, sizeof(LoadTbl));

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recovered Dwell Table #%%d is valid and has been copied to the MD App");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED", to
       prevent a core dump by assigning MD_LoadTablePtr, and to make MD_TableValidateionFunc() return non-success */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_DISABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);
    UT_SetDefaultReturnValue(UT_KEY(MD_TableValidationFunc), CFE_SUCCESS);

    /* Set to reach line "TblInits++" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_RECOVERED_TBL_VALID_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_DwellStreamEnabled(void)
{
    CFE_Status_t         Result;
    int32                strCmpResult;
    char                 ExpectedEventString[4][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Recovered Dwell Table #%%d is valid and has been copied to the MD App");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED", to
       prevent a core dump by assigning MD_LoadTablePtr, and to make MD_TableValidateionFunc() return non-success */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    LoadTblPtr->Enabled = MD_DWELL_STREAM_ENABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);
    UT_SetDefaultReturnValue(UT_KEY(MD_TableValidationFunc), CFE_SUCCESS);

    /* Set to reach line "TblInits++" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_RECOVERED_TBL_VALID_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_TblNotRecovered(void)
{
    CFE_Status_t         Result;
    int32                strCmpResult;
    char                 ExpectedEventString[4][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "MD App will reinitialize Dwell Table #%%d because recovered table is not valid");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_RECOVERED_TBL" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_INFO_RECOVERED_TBL);

    /* Set to fail condition "GetAddressResult != CFE_TBL_INFO_UPDATED", to
       prevent a core dump by assigning MD_LoadTablePtr, and to make MD_TableValidateionFunc() return non-success */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    LoadTblPtr->Enabled = MD_DWELL_STREAM_ENABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);
    UT_SetDefaultReturnValue(UT_KEY(MD_TableValidationFunc), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(MD_TableValidationFunc), 1, -1);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_True(MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[1].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[2].Enabled == MD_DWELL_STREAM_DISABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[3].Enabled == MD_DWELL_STREAM_DISABLED");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_RECOVERED_TBL_NOT_VALID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);
}

void MD_InitTableServices_Test_TblTooLarge(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table(s) are too large to register: %%u bytes, %%d entries");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_ERR_INVALID_SIZE" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, CFE_TBL_ERR_INVALID_SIZE);

    /* Set to reach line "TblInits++" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == CFE_TBL_ERR_INVALID_SIZE, "Result == CFE_TBL_ERR_INVALID_SIZE");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_DWELL_TBL_TOO_LARGE_CRIT_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_CRITICAL);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_TblRegisterCriticalError(void)
{
    CFE_Status_t Result;
    int32        strCmpResult;
    char         ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "CFE_TBL_Register error %%d received for tbl#%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to satisfy condition "Status != CFE_SUCCESS" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Register), 1, -1);

    /* Set to reach line "TblInits++" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), CFE_SUCCESS);

    /* Execute the function being tested */
    Result = MD_InitTableServices();

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    /* Generates 1 event message we don't care about in this test */
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_TBL_REGISTER_CRIT_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_CRITICAL);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 2, "CFE_EVS_SendEvent was called %u time(s), expected 2",
                  call_count_CFE_EVS_SendEvent);
}

void MD_InitTableServices_Test_TblNameError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "TableName could not be made. Err=0x%%08X, Idx=%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to generate snprintf error MD_INIT_TBL_NAME_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(stub_snprintf), 1, -1);

    /* Execute the function being tested */
    /* Verify results */
    UtAssert_UINT32_EQ(MD_InitTableServices(), -1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INIT_TBL_NAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
}

void MD_InitTableServices_Test_TblFileNameError(void)
{
    int32 strCmpResult;
    char  ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "TblFileName could not be made. Err=0x%%08X, Idx=%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Tables Recovered: %%d, Dwell Tables Initialized: %%d");

    /* Set to generate snprintf MD_INIT_TBL_FILENAME_ERR_EID error */
    UT_SetDeferredRetcode(UT_KEY(stub_snprintf), 2, -1);

    /* Execute the function being tested */
    /* Verify results */
    UtAssert_UINT32_EQ(MD_InitTableServices(), -1);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_INIT_TBL_FILENAME_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult =
        strncmp(ExpectedEventString[0], context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventID, MD_TBL_INIT_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult =
        strncmp(ExpectedEventString[1], context_CFE_EVS_SendEvent[1].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[1].Spec);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
}

void MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail(void)
{
    CFE_Status_t Result;
    uint8        TblIndex = 0;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_VALIDATION_PENDING" */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetStatus), CFE_TBL_INFO_VALIDATION_PENDING);

    /* Set to satisfy condition "Status != CFE_SUCCESS" on the 2nd loop */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Validate), 2, -1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled(void)
{
    CFE_Status_t         Result;
    uint8                TblIndex = 0;
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, CFE_TBL_INFO_UPDATE_PENDING);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_ReleaseAddress), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    LoadTblPtr->Enabled = MD_DWELL_STREAM_ENABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    uint8 call_count_MD_StartDwellStream = UT_GetStubCount(UT_KEY(MD_StartDwellStream));
    UtAssert_INT32_EQ(call_count_MD_StartDwellStream, 1);
}

void MD_ManageDwellTable_Test_UpdatePendingDwellStreamDisabled(void)
{
    CFE_Status_t         Result;
    uint8                TblIndex = 0;
    MD_DwellTableLoad_t  LoadTbl;
    MD_DwellTableLoad_t *LoadTblPtr = &LoadTbl;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, CFE_TBL_INFO_UPDATE_PENDING);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_ReleaseAddress), CFE_SUCCESS);

    /* Set to satisfy condition "MD_LoadTablePtr->Enabled == MD_DWELL_STREAM_DISABLED" and to prevent a core dump by
     * assigning MD_LoadTablePtr */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    LoadTblPtr->Enabled = MD_DWELL_STREAM_DISABLED;

    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &LoadTblPtr, sizeof(LoadTblPtr), false);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    uint8 call_count_MD_StartDwellStream = UT_GetStubCount(UT_KEY(MD_StartDwellStream));
    UtAssert_INT32_EQ(call_count_MD_StartDwellStream, 0);
}

void MD_ManageDwellTable_Test_TblNotUpdated(void)
{
    CFE_Status_t Result;
    uint8        TblIndex = 0;

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, CFE_TBL_INFO_UPDATE_PENDING);

    /* Set to satisfy condition "MD_LoadTablePtr->Enabled == MD_DWELL_STREAM_ENABLED" and to prevent a core dump by
     * assigning MD_LoadTablePtr */
    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;
    UT_SetDataBuffer(UT_KEY(CFE_TBL_GetAddress), &MD_AppData.MD_DwellTables[0], sizeof(MD_AppData.MD_DwellTables[0]),
                     false);

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_Update), 1, -1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == -1, "Result == -1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ManageDwellTable_Test_UpdatePendingTblCopyError(void)
{
    CFE_Status_t Result;
    uint8        TblIndex = 0;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Didn't update MD tbl #%%d due to unexpected CFE_TBL_GetAddress return: %%d");

    /* Set to satisfy condition "Status == CFE_TBL_INFO_UPDATE_PENDING" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, CFE_TBL_INFO_UPDATE_PENDING);

    /* Set to generate error message MD_NO_TBL_COPY_ERR_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetAddress), 1, -1);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == CFE_SUCCESS, "Result == CFE_SUCCESS");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_NO_TBL_COPY_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ManageDwellTable_Test_TblStatusErr(void)
{
    CFE_Status_t Result;
    uint8        TblIndex = 0;
    int32        strCmpResult;
    char         ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Received unexpected error %%d from CFE_TBL_GetStatus for tbl #%%d");

    /* Set to satisfy condition "(Status & CFE_SEVERITY_BITMASK) == CFE_SEVERITY_ERROR" */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, CFE_SEVERITY_BITMASK);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == CFE_SEVERITY_BITMASK, "Result == CFE_SEVERITY_BITMASK");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_TBL_STATUS_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ManageDwellTable_Test_OtherStatus(void)
{
    CFE_Status_t Result;
    uint8        TblIndex = 0;

    /* Set to reach final else-case */
    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_GetStatus), 1, 99);

    /* Execute the function being tested */
    Result = MD_ManageDwellTable(TblIndex);

    /* Verify results */
    UtAssert_True(Result == 99, "Result == 99");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_SearchCmdHndlrTblError(void)
{
    CFE_SB_MsgId_t    TestMsgId = MD_UT_MID_1;
    CFE_MSG_FcnCode_t FcnCode   = -1;
    size_t            MsgSize   = 0;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_UINT32_EQ(MD_AppData.ErrCounter, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CC_NOT_IN_TBL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_ExecRequest_Test_CmdLengthError(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Cmd Msg with Bad length Rcvd: ID = 0x%%08lX, CC = %%d, Exp Len = %%d, Len = %%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = 0;
    /* Set to generate error message MD_CMD_LEN_ERR_EID */
    MsgSize = 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_Noop(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "No-op command, Version %%d.%%d.%%d.%%d");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_NOOP_CC;
    MsgSize   = sizeof(MD_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_NOOP_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_ResetCounters(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Reset Counters Cmd Received");

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_RESET_CNTRS_CC;
    MsgSize   = sizeof(MD_NoArgsCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    UtAssert_True(MD_AppData.CmdCounter == 0, "MD_AppData.CmdCounter == 0");
    UtAssert_True(MD_AppData.ErrCounter == 0, "MD_AppData.ErrCounter == 0");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_RESET_CNTRS_DBG_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_DEBUG);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent[0].Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent[0].Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_StartDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_START_DWELL_CC;
    MsgSize   = sizeof(MD_CmdStartStop_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_StopDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_STOP_DWELL_CC;
    MsgSize   = sizeof(MD_CmdStartStop_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_JamDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_JAM_DWELL_CC;
    MsgSize   = sizeof(MD_CmdJam_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_SIGNATURE_OPTION == 1
void MD_ExecRequest_Test_SetSignature(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_SET_SIGNATURE_CC;
    MsgSize   = sizeof(MD_CmdSetSignature_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    /* Execute the function being tested */
    MD_ExecRequest(&UT_CmdBuf.Buf);

    /* Verify results */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}
#endif

void MD_HkStatus_Test(void)
{
    MD_AppData.CmdCounter = 1;
    MD_AppData.ErrCounter = 2;

    MD_AppData.MD_DwellTables[0].Enabled = 1;

    MD_AppData.MD_DwellTables[0].AddrCount    = 1;
    MD_AppData.MD_DwellTables[0].Rate         = 2;
    MD_AppData.MD_DwellTables[0].PktOffset    = 3;
    MD_AppData.MD_DwellTables[0].DataSize     = 4;
    MD_AppData.MD_DwellTables[0].CurrentEntry = 5;
    MD_AppData.MD_DwellTables[0].Countdown    = 6;

    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].AddrCount    = 7;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Rate         = 8;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].PktOffset    = 9;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].DataSize     = 10;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].CurrentEntry = 11;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES / 2].Countdown    = 12;

    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].AddrCount    = 13;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Rate         = 14;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].PktOffset    = 15;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].DataSize     = 16;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].CurrentEntry = 17;
    MD_AppData.MD_DwellTables[MD_NUM_DWELL_TABLES - 1].Countdown    = 18;

    /* Execute the function being tested */
    MD_HkStatus();

    /* Verify results */
    UtAssert_True(MD_AppData.HkPkt.ValidCmdCntr == 1, "MD_AppData.HkPkt.ValidCmdCntr == 1");
    UtAssert_True(MD_AppData.HkPkt.InvalidCmdCntr == 2, "MD_AppData.HkPkt.InvalidCmdCntr == 2");

    UtAssert_True(MD_AppData.HkPkt.DwellTblAddrCount[0] == 1, "MD_AppData.HkPkt.DwellTblAddrCount[0] == 1");
    UtAssert_True(MD_AppData.HkPkt.NumWaitsPerPkt[0] == 2, "MD_AppData.HkPkt.NumWaitsPerPkt[0]    == 2");
    UtAssert_True(MD_AppData.HkPkt.DwellPktOffset[0] == 3, "MD_AppData.HkPkt.DwellPktOffset[0]    == 3");
    UtAssert_True(MD_AppData.HkPkt.ByteCount[0] == 4, "MD_AppData.HkPkt.ByteCount[0]         == 4");
    UtAssert_True(MD_AppData.HkPkt.DwellTblEntry[0] == 5, "MD_AppData.HkPkt.DwellTblEntry[0]     == 5");
    UtAssert_True(MD_AppData.HkPkt.Countdown[0] == 6, "MD_AppData.HkPkt.Countdown[0]         == 6");

    UtAssert_True(MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES / 2] == 7,
                  "MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES / 2] == 7");
    UtAssert_True(MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES / 2] == 8,
                  "MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES / 2]    == 8");
    UtAssert_True(MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES / 2] == 9,
                  "MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES / 2]    == 9");
    UtAssert_True(MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES / 2] == 10,
                  "MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES / 2]         == 10");
    UtAssert_True(MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES / 2] == 11,
                  "MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES / 2]     == 11");
    UtAssert_True(MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES / 2] == 12,
                  "MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES / 2]         == 12");

    UtAssert_True(MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES - 1] == 13,
                  "MD_AppData.HkPkt.DwellTblAddrCount[MD_NUM_DWELL_TABLES - 1] == 13");
    UtAssert_True(MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES - 1] == 14,
                  "MD_AppData.HkPkt.NumWaitsPerPkt[MD_NUM_DWELL_TABLES - 1]    == 14");
    UtAssert_True(MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES - 1] == 15,
                  "MD_AppData.HkPkt.DwellPktOffset[MD_NUM_DWELL_TABLES - 1]    == 15");
    UtAssert_True(MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES - 1] == 16,
                  "MD_AppData.HkPkt.ByteCount[MD_NUM_DWELL_TABLES - 1]         == 16");
    UtAssert_True(MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES - 1] == 17,
                  "MD_AppData.HkPkt.DwellTblEntry[MD_NUM_DWELL_TABLES - 1]     == 17");
    UtAssert_True(MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES - 1] == 18,
                  "MD_AppData.HkPkt.Countdown[MD_NUM_DWELL_TABLES - 1]         == 18");

    UtAssert_True(MD_AppData.HkPkt.DwellEnabledMask == 1, "MD_AppData.HkPkt.DwellEnabledMask == 1");

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void UtTest_Setup(void)
{
    UtTest_Add(MD_AppMain_Test_AppInitError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_AppInitError");
    UtTest_Add(MD_AppMain_Test_RcvMsgError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_RcvMsgError");
    UtTest_Add(MD_AppMain_Test_RcvMsgTimeout, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_RcvMsgTimeout");
    UtTest_Add(MD_AppMain_Test_RcvMsgNullBuffer, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_RcvMsgNullBuffer");
    UtTest_Add(MD_AppMain_Test_WakeupNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_WakeupNominal");
    UtTest_Add(MD_AppMain_Test_WakeupLengthError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_WakeupLengthError");
    UtTest_Add(MD_AppMain_Test_CmdNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_CmdNominal");
    UtTest_Add(MD_AppMain_Test_SendHkNominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_SendHkNominal");
    UtTest_Add(MD_AppMain_Test_SendHkLengthError, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_SendHkLengthError");
    UtTest_Add(MD_AppMain_Test_InvalidMessageID, MD_Test_Setup, MD_Test_TearDown, "MD_AppMain_Test_InvalidMessageID");

    UtTest_Add(MD_AppInit_Test_Nominal, MD_Test_Setup, MD_Test_TearDown, "MD_AppInit_Test_Nominal");
    UtTest_Add(MD_AppInit_Test_EvsRegisterNotSuccess, MD_Test_Setup, MD_Test_TearDown,
               "MD_AppInit_Test_EvsRegisterNotSuccess");
    UtTest_Add(MD_AppInit_Test_InitSoftwareBusServicesNotSuccess, MD_Test_Setup, MD_Test_TearDown,
               "MD_AppInit_Test_InitSoftwareBusServicesNotSuccess");
    UtTest_Add(MD_AppInit_Test_InitTableServicesNotSuccess, MD_Test_Setup, MD_Test_TearDown,
               "MD_AppInit_Test_InitTableServicesNotSuccess");

    UtTest_Add(MD_InitControlStructures_Test, MD_Test_Setup, MD_Test_TearDown, "MD_InitControlStructures_Test");

    UtTest_Add(MD_InitSoftwareBusServices_Test_Nominal, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitSoftwareBusServices_Test_Nominal");
    UtTest_Add(MD_InitSoftwareBusServices_Test_CreatePipeError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitSoftwareBusServices_Test_CreatePipeError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeHkError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitSoftwareBusServices_Test_SubscribeHkError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeCmdError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitSoftwareBusServices_Test_SubscribeCmdError");
    UtTest_Add(MD_InitSoftwareBusServices_Test_SubscribeWakeupError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitSoftwareBusServices_Test_SubscribeWakeupError");

    UtTest_Add(MD_InitTableServices_Test_GetAddressErrorAndLoadError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_GetAddressErrorAndLoadError");
    UtTest_Add(MD_InitTableServices_Test_TblRecoveredValidThenTblInits, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblRecoveredValidThenTblInits");
    UtTest_Add(MD_InitTableServices_Test_TblRecoveredNotValid, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblRecoveredNotValid");
    UtTest_Add(MD_InitTableServices_Test_DwellStreamEnabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_DwellStreamEnabled");
    UtTest_Add(MD_InitTableServices_Test_TblNotRecovered, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblNotRecovered");
    UtTest_Add(MD_InitTableServices_Test_TblTooLarge, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblTooLarge");
    UtTest_Add(MD_InitTableServices_Test_TblRegisterCriticalError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblRegisterCriticalError");
    UtTest_Add(MD_InitTableServices_Test_TblNameError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblNameError");
    UtTest_Add(MD_InitTableServices_Test_TblFileNameError, MD_Test_Setup, MD_Test_TearDown,
               "MD_InitTableServices_Test_TblFileNameError");

    UtTest_Add(MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_ValidationPendingSucceedThenFail");
    UtTest_Add(MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_UpdatePendingDwellStreamEnabled");
    UtTest_Add(MD_ManageDwellTable_Test_UpdatePendingDwellStreamDisabled, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_UpdatePendingDwellStreamDisabled");
    UtTest_Add(MD_ManageDwellTable_Test_TblNotUpdated, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_TblNotUpdated");
    UtTest_Add(MD_ManageDwellTable_Test_UpdatePendingTblCopyError, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_UpdatePendingTblCopyError");
    UtTest_Add(MD_ManageDwellTable_Test_TblStatusErr, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_TblStatusErr");
    UtTest_Add(MD_ManageDwellTable_Test_OtherStatus, MD_Test_Setup, MD_Test_TearDown,
               "MD_ManageDwellTable_Test_OtherStatus");

    UtTest_Add(MD_ExecRequest_Test_SearchCmdHndlrTblError, MD_Test_Setup, MD_Test_TearDown,
               "MD_ExecRequest_Test_SearchCmdHndlrTblError");
    UtTest_Add(MD_ExecRequest_Test_CmdLengthError, MD_Test_Setup, MD_Test_TearDown,
               "MD_ExecRequest_Test_CmdLengthError");
    UtTest_Add(MD_ExecRequest_Test_Noop, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_Noop");
    UtTest_Add(MD_ExecRequest_Test_ResetCounters, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_ResetCounters");
    UtTest_Add(MD_ExecRequest_Test_StartDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StartDwell");
    UtTest_Add(MD_ExecRequest_Test_StopDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StopDwell");
    UtTest_Add(MD_ExecRequest_Test_JamDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_JamDwell");

#if MD_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ExecRequest_Test_SetSignature, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_SetSignature");
#endif

    UtTest_Add(MD_HkStatus_Test, MD_Test_Setup, MD_Test_TearDown, "MD_HkStatus_Test");
}
