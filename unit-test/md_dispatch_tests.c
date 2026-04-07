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

void MD_VerifyCmdLength_Nominal(void)
{
    bool              Result;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t FcnCode;

    /* Set up values for test */
    ExpectedLen = sizeof(MD_NoopCmd_t);
    MsgId       = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    FcnCode     = MD_NOOP_CC;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Run function under test */
    Result = MD_VerifyCmdLength(NULL, sizeof(MD_NoopCmd_t));

    /* Evaluate run */
    UtAssert_BOOL_TRUE(Result);

    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void MD_VerifyCmdLength_InvalidSize(void)
{
    bool              Result;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t FcnCode;

    /* Set up values for test */
    ExpectedLen = sizeof(MD_NoopCmd_t) + 1;
    MsgId       = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    FcnCode     = MD_NOOP_CC;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Run function under test */
    Result = MD_VerifyCmdLength(NULL, sizeof(MD_NoopCmd_t));

    /* Evaluate run */
    UtAssert_BOOL_FALSE(Result);

    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    MD_Test_Verify_Event(0,
                         MD_CMD_LEN_ERR_EID,
                         CFE_EVS_EventType_ERROR,
                         "Invalid Msg length: ID = 0x%X, CC = %u, Len = %u, Expected = %u");
}

void MD_ProcessGroundCommand_NoopCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_NoopCmd() */
    CommandCode = MD_NOOP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_NoopCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_NoopCmd, 1);
}

void MD_ProcessGroundCommand_NoopCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_NoopCmd() */
    CommandCode = MD_NOOP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);

    UtAssert_STUB_COUNT(MD_NoopCmd, 0);
}

void MD_ProcessGroundCommand_ResetCountersCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_ResetCountersCmd() */
    CommandCode = MD_RESET_CNTRS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_ResetCountersCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_ResetCountersCmd, 1);
}

void MD_ProcessGroundCommand_ResetCountersCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_ResetCountersCmd() */
    CommandCode = MD_RESET_CNTRS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);

    UtAssert_STUB_COUNT(MD_ResetCountersCmd, 0);
}

void MD_ProcessGroundCommand_StartDwellCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_StartDwellCmd() */
    CommandCode = MD_START_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_StartDwellCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_StartDwellCmd, 1);
}

void MD_ProcessGroundCommand_StartDwellCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_StartDwellCmd() */
    CommandCode = MD_START_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);
}

void MD_ProcessGroundCommand_StopDwellCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_StopDwellCmd() */
    CommandCode = MD_STOP_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_StopDwellCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_StopDwellCmd, 1);
}

void MD_ProcessGroundCommand_StopDwellCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_StopDwellCmd() */
    CommandCode = MD_STOP_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);
}

void MD_ProcessGroundCommand_JamDwellCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_JamDwellCmd() */
    CommandCode = MD_JAM_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_JamDwellCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_JamDwellCmd, 1);
}

void MD_ProcessGroundCommand_JamDwellCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_JamDwellCmd() */
    CommandCode = MD_JAM_DWELL_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);
}

#if MD_INTERFACE_SIGNATURE_OPTION == 1

void MD_ProcessGroundCommand_SetSignatureCmd(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_SetSignatureCmd() */
    CommandCode = MD_SET_SIGNATURE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_SetSignatureCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 0);

    UtAssert_STUB_COUNT(MD_SetSignatureCmd, 1);
}

void MD_ProcessGroundCommand_SetSignatureCmdErr(void)
{
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;
    CFE_SB_MsgId_t    MsgId;

    /* Provide a message ID for test */
    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Set up to run MD_SetSignatureCmd() */
    CommandCode = MD_SET_SIGNATURE_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to fail command length verification */
    ExpectedLen = 0;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);
}

#endif

void MD_ProcessGroundCommand_Err(void)
{
    CFE_MSG_FcnCode_t CommandCode;

    /* Set up to fail the CommandCode switch-case */
    CommandCode = 200;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Run the function under test */
    UtAssert_VOIDCALL(MD_ProcessGroundCommand(NULL));

    /* Evaluate run */
    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);

    MD_Test_Verify_Event(0, MD_CC_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid ground command code %d");
}

void MD_TaskPipe_Test_Wakeup(void)
{
    CFE_SB_MsgId_t MsgId;

    /* Set up to run wake up command */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_MsgId_Equal), true);

    MsgId = CFE_SB_MSGID_C(MD_WAKEUP_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_TaskPipe(NULL));

    /* Evaluate run */
    UtAssert_STUB_COUNT(MD_DwellLoop, 1);
    UtAssert_STUB_COUNT(MD_SendHkCmd, 0);
    UtAssert_STUB_COUNT(MD_ProcessGroundCommand, 0);
}

void MD_TaskPipe_Test_SendHk(void)
{
    CFE_SB_MsgId_t MsgId;

    /* Set up to run Send Housekeeping command */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_MsgId_Equal), 1, true);

    MsgId = CFE_SB_MSGID_C(MD_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_TaskPipe(NULL));

    /* Evaluate run */
    UtAssert_STUB_COUNT(MD_DwellLoop, 0);
    UtAssert_STUB_COUNT(MD_SendHkCmd, 1);
    UtAssert_STUB_COUNT(MD_ProcessGroundCommand, 0);
}

void MD_TaskPipe_Test_Cmd(void)
{
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_FcnCode_t CommandCode;
    size_t            ExpectedLen;

    /* Set up to run MD_NoopCmd() */
    CommandCode = MD_NOOP_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &CommandCode, sizeof(CFE_MSG_FcnCode_t), false);

    /* Set up to pass command length verification */
    ExpectedLen = sizeof(MD_NoopCmd_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &ExpectedLen, sizeof(size_t), false);

    /* Set up to run Send Housekeeping command */
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_MsgId_Equal), 2, true);

    MsgId = CFE_SB_MSGID_C(MD_CMD_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_TaskPipe(NULL));

    /* Evaluate run */
    UtAssert_STUB_COUNT(MD_DwellLoop, 0);
    UtAssert_STUB_COUNT(MD_SendHkCmd, 0);
    UtAssert_STUB_COUNT(MD_NoopCmd, 1);
}

void MD_TaskPipe_Test_InvalidMsgId(void)
{
    CFE_SB_MsgId_t MsgId;

    /* Set up to run Send Housekeeping command */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_MsgId_Equal), false);

    MsgId = CFE_SB_MSGID_C(CFE_MISSION_MD_CMD_TOPICID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(CFE_SB_MsgId_t), false);

    /* Run function under test */
    UtAssert_VOIDCALL(MD_TaskPipe(NULL));

    /* Evaluate run */
    UtAssert_STUB_COUNT(MD_DwellLoop, 0);
    UtAssert_STUB_COUNT(MD_SendHkCmd, 0);
    UtAssert_STUB_COUNT(MD_ProcessGroundCommand, 0);

    UtAssert_EQ(uint8, MD_AppData.CommandCounter, 0);
    UtAssert_EQ(uint8, MD_AppData.CommandErrorCounter, 1);

    MD_Test_Verify_Event(0, MD_MID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid command pipe message ID: 0x%08lX");
}

void UtTest_Setup(void)
{
    ADD_TEST(MD_VerifyCmdLength_Nominal);
    ADD_TEST(MD_VerifyCmdLength_InvalidSize);
    ADD_TEST(MD_ProcessGroundCommand_NoopCmd);
    ADD_TEST(MD_ProcessGroundCommand_NoopCmdErr);
    ADD_TEST(MD_ProcessGroundCommand_ResetCountersCmd);
    ADD_TEST(MD_ProcessGroundCommand_ResetCountersCmdErr);
    ADD_TEST(MD_ProcessGroundCommand_StartDwellCmd);
    ADD_TEST(MD_ProcessGroundCommand_StartDwellCmdErr);
    ADD_TEST(MD_ProcessGroundCommand_StopDwellCmd);
    ADD_TEST(MD_ProcessGroundCommand_StopDwellCmdErr);
    ADD_TEST(MD_ProcessGroundCommand_JamDwellCmd);
    ADD_TEST(MD_ProcessGroundCommand_JamDwellCmdErr);

#if MD_INTERFACE_SIGNATURE_OPTION == 1
    ADD_TEST(MD_ProcessGroundCommand_SetSignatureCmd);
    ADD_TEST(MD_ProcessGroundCommand_SetSignatureCmdErr);
#endif

    ADD_TEST(MD_ProcessGroundCommand_Err);
    ADD_TEST(MD_TaskPipe_Test_Wakeup);
    ADD_TEST(MD_TaskPipe_Test_SendHk);
    ADD_TEST(MD_TaskPipe_Test_Cmd);
    ADD_TEST(MD_TaskPipe_Test_InvalidMsgId);
}
