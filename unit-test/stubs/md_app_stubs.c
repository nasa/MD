
/*
 * Includes
 */

#include "md_app.h"
#include "common_types.h"
#include "md_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "md_msgids.h"
#include "cfs_utils.h"
#include "md_msg.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

MD_AppData_t MD_AppData;

void MD_AppMain(void)
{
    UT_DEFAULT_IMPL(MD_AppMain);
}

int32 MD_AppInit(void)
{
    return UT_DEFAULT_IMPL(MD_AppInit);
}

void MD_InitControlStructures(void)
{
    UT_DEFAULT_IMPL(MD_InitControlStructures);
}

int32 MD_InitSoftwareBusServices(void)
{
    return UT_DEFAULT_IMPL(MD_InitSoftwareBusServices);
}

int32 MD_InitTableServices(void)
{
    return UT_DEFAULT_IMPL(MD_InitTableServices);
}

int32 MD_ManageDwellTable(uint8 TblIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ManageDwellTable), TblIndex);
    return UT_DEFAULT_IMPL(MD_ManageDwellTable);
}

void MD_ExecRequest(const CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ExecRequest), BufPtr);
    UT_DEFAULT_IMPL(MD_ExecRequest);
}

void MD_HkStatus(void)
{
    UT_DEFAULT_IMPL(MD_HkStatus);
}

int16 MD_SearchCmdHndlrTbl(CFE_SB_MsgId_t MessageID, CFE_MSG_FcnCode_t CommandCode)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_SearchCmdHndlrTbl), MessageID);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_SearchCmdHndlrTbl), CommandCode);
    return UT_DEFAULT_IMPL(MD_SearchCmdHndlrTbl);
}
