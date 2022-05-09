
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
#include <sys/fcntl.h>
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

int32 MD_CMDS_TEST_CFE_TBL_GetAddressHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
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
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table command processed successfully for table mask 0x%%04X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent[2];
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 0;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_Buffer_t *)(&CmdPacket));

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

} /* end MD_ProcessStartCmd_Test_ZeroRate */

void MD_ProcessStartCmd_Test_Success(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table command processed successfully for table mask 0x%%04X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 1;

    MD_AppData.MD_DwellTables[0].Rate = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 1, "MD_AppData.MD_DwellTables[0].Countdown == 1");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_START_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStartCmd_Test_Success */

void MD_ProcessStartCmd_Test_EmptyTableMask(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s command rejected because no tables were specified in table mask (0x%%04X)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStartCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_EMPTY_TBLMASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStartCmd_Test_EmptyTableMask */

void MD_ProcessStartCmd_Test_NoUpdateTableEnabledField(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Start Dwell Table for mask 0x%%04X failed for %%d of %%d tables");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask               = 1;
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
    MD_ProcessStartCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_START_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStartCmd_Test_NoUpdateTableEnabledField  */

void MD_ProcessStopCmd_Test_Success(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Stop Dwell Table command processed successfully for table mask 0x%%04X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);

    /* Execute the function being tested */
    MD_ProcessStopCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 0, "MD_AppData.MD_DwellTables[0].Countdown == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_STOP_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStopCmd_Test_Success */

void MD_ProcessStopCmd_Test_EmptyTableMask(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "%%s command rejected because no tables were specified in table mask (0x%%04X)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 0;

    /* Execute the function being tested */
    MD_ProcessStopCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_EMPTY_TBLMASK_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStopCmd_Test_EmptyTableMask */

void MD_ProcessStopCmd_Test_NoUpdateTableEnabledField(void)
{
    MD_CmdStartStop_t CmdPacket;
    CFE_SB_MsgId_t    TestMsgId;
    int32             strCmpResult;
    char              ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Stop Dwell Table for mask 0x%%04X failed for %%d of %%d tables");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableMask = 1;

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableEnabledField */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    /* Only process one entry per table processing loop */
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_TableIsInMask), 3, false);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableEnabledField), 1, -1);

    /* Execute the function being tested */
    MD_ProcessStopCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_DISABLED,
                  "MD_AppData.MD_DwellTables[0].Enabled == MD_DWELL_STREAM_ENABLED");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Countdown == 0, "MD_AppData.MD_DwellTables[0].Countdown == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].CurrentEntry == 0, "MD_AppData.MD_DwellTables[0].CurrentEntry == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].PktOffset == 0, "MD_AppData.MD_DwellTables[0].PktOffset == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_STOP_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessStopCmd_Test_NoUpdateTableEnabledField  */

void MD_ProcessJamCmd_Test_InvalidJamTable(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Tbl Id arg = %%d (Expect 1.. %%d)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId = 99;
    CmdPacket.EntryId = 2;

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_JAM_TABLE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_InvalidJamTable */

void MD_ProcessJamCmd_Test_InvalidEntryArg(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Entry Id arg = %%d (Expect 1.. %%d)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId = 1;
    CmdPacket.EntryId = 99;

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_ENTRY_ARG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_InvalidEntryArg */

void MD_ProcessJamCmd_Test_SuccessNullZeroRate(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Successful Jam of a Null Dwell Entry to Dwell Tbl#%%d Entry #%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent[2];
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 0;

    MD_AppData.MD_DwellTables[0].Rate    = 1;
    MD_AppData.MD_DwellTables[0].Enabled = MD_DWELL_STREAM_ENABLED;

    /* Prevents segmentation fault in call to subfunction MD_UpdateDwellControlInfo */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook1, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, CFE_SUCCESS);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

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

} /* end MD_ProcessJamCmd_Test_SuccessNullZeroRate */

void MD_ProcessJamCmd_Test_NullTableDwell(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Failed Jam of a Null Dwell Entry to Dwell Tbl#%%d Entry #%%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 0;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == true" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, -1);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_NULL_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_NullTableDwell */

void MD_ProcessJamCmd_Test_NoUpdateTableDwell(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Failed Jam to Dwell Tbl#%%d Entry #%%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == true" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableDwellEntry), 1, -1);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 1,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 1");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_DWELL_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_NoUpdateTableDwell */

void MD_ProcessJamCmd_Test_CantResolveJamAddr(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because symbolic address '%%s' couldn't be resolved");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 1;

    strncpy(CmdPacket.DwellAddress.SymName, "address", 10);

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == FALSE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, false);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_CANT_RESOLVE_JAM_ADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_CantResolveJamAddr */

void MD_ProcessJamCmd_Test_InvalidLenArg(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected due to invalid Field Length arg = %%d (Expect 0,1,2,or 4)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_LEN_ARG_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_InvalidLenArg */

void MD_ProcessJamCmd_Test_InvalidJamAddr(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not in a valid range");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_JAM_ADDR_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_InvalidJamAddr */

#if MD_ENFORCE_DWORD_ALIGN == 0
void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 16-bit aligned");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_ADDR_NOT_16BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength4 */
#endif

#if MD_ENFORCE_DWORD_ALIGN == 1
void MD_ProcessJamCmd_Test_JamAddrNot32Bit(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 32-bit aligned");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_ADDR_NOT_32BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_JamAddrNot32Bit */
#endif

void MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 16-bit aligned");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 2;
    CmdPacket.DwellAddress.Offset = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_ADDR_NOT_16BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_JamAddrNot16BitFieldLength2 */

void MD_ProcessJamCmd_Test_JamAddrNot16BitNot32Aligned(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Jam Cmd rejected because address 0x%%08X is not 32-bit aligned");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFS_Verify32Aligned), 1, false);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_ADDR_NOT_32BIT_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_JamAddrNot16BitNot32Aligned */

void MD_ProcessJamCmd_Test_JamFieldLength4Addr32Aligned(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId             = 1;
    CmdPacket.EntryId             = 2;
    CmdPacket.FieldLength         = 4;
    CmdPacket.DwellAddress.Offset = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFS_Verify32Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_JamFieldLength4Addr32Aligned */

void MD_ProcessJamCmd_Test_SuccessNonNullZeroRate(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[2][CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString[0], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    snprintf(ExpectedEventString[1], CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Dwell Table %%d is enabled with a delay of zero so no processing will occur");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent[2];
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 2;
    CmdPacket.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook1, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFS_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

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

} /* end MD_ProcessJamCmd_Test_SuccessNonNullZeroRate */

void MD_ProcessJamCmd_Test_SuccessZeroRateStreamDisabled(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 2;
    CmdPacket.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFS_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 3,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_SuccessZeroRateStreamDisabled */

void MD_ProcessJamCmd_Test_SuccessRateNotZero(void)
{
    MD_CmdJam_t    CmdPacket;
    CFE_SB_MsgId_t TestMsgId;
    int32          strCmpResult;
    char           ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH, "Successful Jam to Dwell Tbl#%%d Entry #%%d");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    CmdPacket.TableId     = 1;
    CmdPacket.EntryId     = 2;
    CmdPacket.FieldLength = 2;
    CmdPacket.DwellDelay  = 3;

    MD_AppData.MD_DwellTables[0].Entry[1].Delay = 1;

    UT_SetHookFunction(UT_KEY(MD_UpdateDwellControlInfo), &MD_CMDS_TEST_MD_UpdateDwellControlInfoHook2, NULL);

    /* Set to satisfy condition "CFS_ResolveSymAddr(&Jam->DwellAddress,&ResolvedAddr) == TRUE" */
    UT_SetDeferredRetcode(UT_KEY(CFS_ResolveSymAddr), 1, true);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidEntryId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidFieldLength), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_ValidAddrRange), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFS_Verify16Aligned), 1, true);

    /* Execute the function being tested */
    MD_ProcessJamCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == '\0',
                  "CmdPacket.DwellAddress.SymName[OS_MAX_SYM_LEN - 1] == ''");

    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0,
                  "MD_AppData.MD_DwellTables[0].Entry[1].ResolvedAddress == 0");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Length == 2,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Length == 2");
    UtAssert_True(MD_AppData.MD_DwellTables[0].Entry[1].Delay == 3,
                  "MD_AppData.MD_DwellTables[0].Entry[1].Delay  == 3");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_JAM_DWELL_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessJamCmd_Test_SuccessRateNotZero */

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureLength(void)
{
    uint16               i;
    MD_CmdSetSignature_t CmdPacket;
    CFE_SB_MsgId_t       TestMsgId;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set Signature cmd rejected due to invalid Signature length");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    // CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), true);

    for (i = 0; i < MD_SIGNATURE_FIELD_LENGTH; i++)
    {
        CmdPacket.Signature[i] = 'x';
    }

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_SIGNATURE_LENGTH_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessSignatureCmd_Test_InvalidSignatureLength */
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_InvalidSignatureTable(void)
{
    MD_CmdSetSignature_t CmdPacket;
    CFE_SB_MsgId_t       TestMsgId;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Set Signature cmd rejected due to invalid Tbl Id arg = %%d (Expect 1.. %%d)");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    // CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), true);

    CmdPacket.TableId = 0;

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_INVALID_SIGNATURE_TABLE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessSignatureCmd_Test_InvalidSignatureTable */
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_Success(void)
{
    MD_CmdSetSignature_t CmdPacket;
    CFE_SB_MsgId_t       TestMsgId;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Successfully set signature for Dwell Tbl#%%d to '%%s'");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    // CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), true);

    CmdPacket.TableId = 1;
    strncpy(CmdPacket.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableSignature */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(strncmp(MD_AppData.MD_DwellTables[0].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
                  "strncmp(MD_AppData.MD_DwellTables[0].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(MD_AppData.CmdCounter == 1, "MD_AppData.CmdCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_SET_SIGNATURE_INF_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessSignatureCmd_Test_Success */
#endif

#if MD_SIGNATURE_OPTION == 1
void MD_ProcessSignatureCmd_Test_NoUpdateTableSignature(void)
{
    MD_CmdSetSignature_t CmdPacket;
    CFE_SB_MsgId_t       TestMsgId;
    int32                strCmpResult;
    char                 ExpectedEventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    snprintf(ExpectedEventString, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH,
             "Failed to set signature for Dwell Tbl#%%d. Update returned 0x%%08X");

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Utils_stub_reporter_hook, &context_CFE_EVS_SendEvent);

    TestMsgId = MD_CMD_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &TestMsgId, sizeof(TestMsgId), false);

    // CFE_SB_InitMsg (&CmdPacket, MD_CMD_MID, sizeof(MD_CmdSetSignature_t), true);

    CmdPacket.TableId = 1;
    strncpy(CmdPacket.Signature, "signature", MD_SIGNATURE_FIELD_LENGTH);

    /* Prevents segmentation fault in call to subfunction MD_UpdateTableSignature */
    UT_SetHookFunction(UT_KEY(CFE_TBL_GetAddress), &MD_CMDS_TEST_CFE_TBL_GetAddressHook, NULL);

    UT_SetDeferredRetcode(UT_KEY(MD_ValidTableId), 1, true);
    UT_SetDeferredRetcode(UT_KEY(MD_UpdateTableSignature), 1, true);

    /* Execute the function being tested */
    MD_ProcessSignatureCmd((CFE_SB_Buffer_t *)(&CmdPacket));

    /* Verify results */
    UtAssert_True(strncmp(MD_AppData.MD_DwellTables[0].Signature, "signature", MD_SIGNATURE_FIELD_LENGTH) == 0,
                  "strncmp(MD_AppData.MD_DwellTables[0].Signature, 'signature', MD_SIGNATURE_FIELD_LENGTH) == 0");

    UtAssert_True(MD_AppData.ErrCounter == 1, "MD_AppData.ErrCounter == 1");

    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventID, MD_SET_SIGNATURE_ERR_EID);
    UtAssert_INT32_EQ(context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

    strCmpResult = strncmp(ExpectedEventString, context_CFE_EVS_SendEvent.Spec, CFE_MISSION_EVS_MAX_MESSAGE_LENGTH);

    UtAssert_True(strCmpResult == 0, "Event string matched expected result, '%s'", context_CFE_EVS_SendEvent.Spec);

    call_count_CFE_EVS_SendEvent = UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    UtAssert_True(call_count_CFE_EVS_SendEvent == 1, "CFE_EVS_SendEvent was called %u time(s), expected 1",
                  call_count_CFE_EVS_SendEvent);

} /* end MD_ProcessSignatureCmd_Test_NoUpdateTableSignature */
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

} /* end UtTest_Setup */

/************************/
/*  End of File Comment */
/************************/
