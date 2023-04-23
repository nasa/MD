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
#include "common_types.h"
#include "md_platform_cfg.h"
#include "cfe_mission_cfg.h"
#include "md_msgids.h"
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

CFE_Status_t MD_AppInit(void)
{
    return UT_DEFAULT_IMPL(MD_AppInit);
}

void MD_InitControlStructures(void)
{
    UT_DEFAULT_IMPL(MD_InitControlStructures);
}

CFE_Status_t MD_InitSoftwareBusServices(void)
{
    return UT_DEFAULT_IMPL(MD_InitSoftwareBusServices);
}

CFE_Status_t MD_InitTableServices(void)
{
    return UT_DEFAULT_IMPL(MD_InitTableServices);
}

CFE_Status_t MD_ManageDwellTable(uint8 TblIndex)
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

int16 MD_SearchCmdHndlrTbl(CFE_MSG_FcnCode_t CommandCode)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_SearchCmdHndlrTbl), CommandCode);
    return UT_DEFAULT_IMPL(MD_SearchCmdHndlrTbl);
}
