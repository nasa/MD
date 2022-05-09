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

#include "common_types.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void MD_DwellLoop(void)
{
    UT_DEFAULT_IMPL(MD_DwellLoop);
}

int32 MD_GetDwellData(uint16 TblIndex, uint16 EntryIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_GetDwellData), TblIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_GetDwellData), EntryIndex);
    return UT_DEFAULT_IMPL(MD_GetDwellData);
}

void MD_SendDwellPkt(uint16 TableIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_SendDwellPkt), TableIndex);
    UT_DEFAULT_IMPL(MD_SendDwellPkt);
}

void MD_StartDwellStream(uint16 TableIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_StartDwellStream), TableIndex);
    UT_DEFAULT_IMPL(MD_StartDwellStream);
}
