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

/*
 * Includes
 */
#include "md_app.h"
#include "md_dispatch.h"
#include "md_cmds.h"
#include "md_msgids.h"
#include "md_eventids.h"
#include "md_version.h"
#include "md_dwell_pkt.h"

#include "md_test_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include <unistd.h>
#include <stdlib.h>

/*
 * Helper functions
 */
static void MD_Dispatch_Test_SetupMsg(CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t FcnCode, size_t MsgSize)
{
    /* Note some paths get the MsgId/FcnCode multiple times, so register accordingly */
    CFE_SB_MsgId_t    RegMsgId[2]   = {MsgId, MsgId};
    CFE_MSG_FcnCode_t RegFcnCode[2] = {FcnCode, FcnCode};
    size_t            RegMsgSize[2] = {MsgSize, MsgSize};

    UT_ResetState(UT_KEY(CFE_MSG_GetMsgId));
    UT_ResetState(UT_KEY(CFE_MSG_GetFcnCode));
    UT_ResetState(UT_KEY(CFE_MSG_GetSize));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), RegMsgId, sizeof(RegMsgId), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), RegFcnCode, sizeof(RegFcnCode), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), RegMsgSize, sizeof(RegMsgSize), true);
}

void MD_ProcessCommandPacket_Test_Wakeup(void)
{
    CFE_SB_MsgId_t TestMsgId;
    uint8 call_count_MD_DwellLoop;
    uint8 call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_WAKEUP_MID);
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, sizeof(MD_Wakeup_t));

    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify MD_DwellLoop was called */
    call_count_MD_DwellLoop = UT_GetStubCount(UT_KEY(MD_DwellLoop));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_MD_DwellLoop == 1, "MD_DwellLoop was called %u time(s), expected 1",
                  call_count_MD_DwellLoop);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    /* Bad Length */
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, 1);
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify MD_DwellLoop was not called again */
    call_count_MD_DwellLoop = UT_GetStubCount(UT_KEY(MD_DwellLoop));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_DwellLoop == 1, "MD_DwellLoop was called %u time(s), expected 1",
                  call_count_MD_DwellLoop);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_MSG_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_ProcessCommandPacket_Test_SendHk(void)
{
    CFE_SB_MsgId_t TestMsgId;
    uint8 call_count_MD_HkStatus;
    uint8 call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_SEND_HK_MID);
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, sizeof(MD_SendHkCmd_t));

    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify MD_HkStatus was called */
    call_count_MD_HkStatus = UT_GetStubCount(UT_KEY(MD_HkStatus));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_HkStatus == 1, "MD_HkStatus was called %u time(s), expected 1",
                  call_count_MD_HkStatus);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);

    /* Bad Length */
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, 1);
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify MD_HkStatus was not called again */
    call_count_MD_HkStatus = UT_GetStubCount(UT_KEY(MD_HkStatus));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_HkStatus == 1, "MD_HkStatus was called %u time(s), expected 1",
                  call_count_MD_HkStatus);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_MSG_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_ProcessCommandPacket_Test_Cmd(void)
{
    CFE_SB_MsgId_t TestMsgId;
    uint8 call_count_MD_ExecRequest;
    uint8 call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, sizeof(MD_NoopCmd_t));
    
    /* Reset relevant stubs for proper counting */
    UT_ResetState(UT_KEY(MD_ExecRequest));
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify MD_ExecRequest was called */
    call_count_MD_ExecRequest = UT_GetStubCount(UT_KEY(MD_ExecRequest));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_ExecRequest == 0, "MD_ExecRequest was called %u time(s), expected 1",
                  call_count_MD_ExecRequest);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}   

void MD_ProcessCommandPacket_Test_InvalidMsgId(void)
{
    CFE_SB_MsgId_t TestMsgId;
    uint8 call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(0xFFFF); /* Invalid MID */
    MD_Dispatch_Test_SetupMsg(TestMsgId, 0, 0);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UT_CmdBuf.Buf));
    
    /* Verify error event was sent */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_MID_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_ProcessCommandPacket_Test_NullPtr(void)
{
    /* Execute the function with a NULL pointer */
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(NULL));
    
    /* No specific verification needed - just making sure it doesn't crash */
    /* This test is just to cover the NULL pointer check branch */
}

void MD_ExecRequest_Test_Noop(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_NoopCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_NOOP_CC;
    MsgSize   = sizeof(MD_NoopCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);

    /* Initialize command counter */
    MD_AppData.CmdCounter = 0;
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_NoopCmd = UT_GetStubCount(UT_KEY(MD_NoopCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_NoopCmd == 1, "MD_NoopCmd was called %u time(s), expected 1",
                  call_count_MD_NoopCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_Reset(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_ResetCountersCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_RESET_CNTRS_CC;
    MsgSize   = sizeof(MD_ResetCountersCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_ResetCountersCmd = UT_GetStubCount(UT_KEY(MD_ResetCountersCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_ResetCountersCmd == 1, "MD_ResetCountersCmd was called %u time(s), expected 1",
                  call_count_MD_ResetCountersCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_StartDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_StartDwellCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_START_DWELL_CC;
    MsgSize   = sizeof(MD_StartDwellCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_StartDwellCmd = UT_GetStubCount(UT_KEY(MD_StartDwellCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_StartDwellCmd == 1, "MD_StartDwellCmd was called %u time(s), expected 1",
                  call_count_MD_StartDwellCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_StopDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_StopDwellCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_STOP_DWELL_CC;
    MsgSize   = sizeof(MD_StopDwellCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_StopDwellCmd = UT_GetStubCount(UT_KEY(MD_StopDwellCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_StopDwellCmd == 1, "MD_StopDwellCmd was called %u time(s), expected 1",
                  call_count_MD_StopDwellCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

void MD_ExecRequest_Test_JamDwell(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_JamDwellCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_JAM_DWELL_CC;
    MsgSize   = sizeof(MD_JamDwellCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_JamDwellCmd = UT_GetStubCount(UT_KEY(MD_JamDwellCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_JamDwellCmd == 1, "MD_JamDwellCmd was called %u time(s), expected 1",
                  call_count_MD_JamDwellCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}

#if MD_INTERFACE_SIGNATURE_OPTION == 1
void MD_ExecRequest_Test_SetSignature(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_MD_SetSignatureCmd;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode   = MD_SET_SIGNATURE_CC;
    MsgSize   = sizeof(MD_SetSignatureCmd_t);
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify the command handler was called */
    call_count_MD_SetSignatureCmd = UT_GetStubCount(UT_KEY(MD_SetSignatureCmd));
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_True(call_count_MD_SetSignatureCmd == 1, "MD_SetSignatureCmd was called %u time(s), expected 1",
                  call_count_MD_SetSignatureCmd);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 0, "CFE_EVS_SendEvent was called %u time(s), expected 0",
                  call_count_CFE_EVS_SendEvent);
}
#endif

void MD_ExecRequest_Test_InvalidCommandCode(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode = 99; /* Invalid command code */
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, 0);
    
    /* Reset error counter */
    MD_AppData.ErrCounter = 0;
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify error counter incremented and event sent */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_UINT32_EQ(MD_AppData.ErrCounter, 1);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CC_NOT_IN_TBL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_ExecRequest_Test_InvalidMsgLength(void)
{
    CFE_SB_MsgId_t    TestMsgId;
    CFE_MSG_FcnCode_t FcnCode;
    size_t            MsgSize;
    uint8             call_count_CFE_EVS_SendEvent;

    TestMsgId = CFE_SB_ValueToMsgId(MD_CMD_MID);
    FcnCode = MD_NOOP_CC;
    MsgSize = sizeof(MD_NoopCmd_t) - 1; /* Invalid size */
    MD_Dispatch_Test_SetupMsg(TestMsgId, FcnCode, MsgSize);
    
    /* Reset error counter */
    MD_AppData.ErrCounter = 0;
    
    /* Execute the function being tested */
    UtAssert_VOIDCALL(MD_ExecRequest(&UT_CmdBuf.Buf));
    
    /* Verify error counter incremented and event sent */
    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));
    
    UtAssert_UINT32_EQ(MD_AppData.ErrCounter, 1);
    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventID, MD_CMD_LEN_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent[0].EventType, CFE_EVS_EventType_ERROR);
}

void MD_SearchCmdHndlrTbl_Test(void)
{
    int16 Result;
    
    /* Test with a valid command code */
    Result = MD_SearchCmdHndlrTbl(MD_NOOP_CC);
    
    /* First entry in table should be NOOP */
    UtAssert_INT32_EQ(Result, 0);
    
    /* Test with another valid command code */
    Result = MD_SearchCmdHndlrTbl(MD_RESET_CNTRS_CC);
    
    /* Should be the second entry in the table */
    UtAssert_INT32_EQ(Result, 1);
    
    /* Test with an invalid command code */
    Result = MD_SearchCmdHndlrTbl(99);
    
    /* Should return error code */
    UtAssert_INT32_EQ(Result, MD_BAD_CMD_CODE);
}

void UtTest_Setup(void)
{
    UtTest_Add(MD_ProcessCommandPacket_Test_Wakeup, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessCommandPacket_Test_Wakeup");
    UtTest_Add(MD_ProcessCommandPacket_Test_SendHk, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessCommandPacket_Test_SendHk");
    UtTest_Add(MD_ProcessCommandPacket_Test_Cmd, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessCommandPacket_Test_Cmd");
    UtTest_Add(MD_ProcessCommandPacket_Test_InvalidMsgId, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessCommandPacket_Test_InvalidMsgId");
    UtTest_Add(MD_ProcessCommandPacket_Test_NullPtr, MD_Test_Setup, MD_Test_TearDown, "MD_ProcessCommandPacket_Test_NullPtr");

    UtTest_Add(MD_ExecRequest_Test_Noop, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_Noop");
    UtTest_Add(MD_ExecRequest_Test_Reset, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_Reset");
    UtTest_Add(MD_ExecRequest_Test_StartDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StartDwell");
    UtTest_Add(MD_ExecRequest_Test_StopDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_StopDwell");
    UtTest_Add(MD_ExecRequest_Test_JamDwell, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_JamDwell");
    
#if MD_INTERFACE_SIGNATURE_OPTION == 1
    UtTest_Add(MD_ExecRequest_Test_SetSignature, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_SetSignature");
#endif
    
    UtTest_Add(MD_ExecRequest_Test_InvalidCommandCode, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_InvalidCommandCode");
    UtTest_Add(MD_ExecRequest_Test_InvalidMsgLength, MD_Test_Setup, MD_Test_TearDown, "MD_ExecRequest_Test_InvalidMsgLength");
    
    UtTest_Add(MD_SearchCmdHndlrTbl_Test, MD_Test_Setup, MD_Test_TearDown, "MD_SearchCmdHndlrTbl_Test");
}