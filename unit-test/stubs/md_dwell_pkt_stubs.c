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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in md_dwell_pkt header
 */

#include "md_dwell_pkt.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for MD_DwellLoop()
 * ----------------------------------------------------
 */
CFE_Status_t MD_DwellLoop(const MD_Wakeup_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_DwellLoop, CFE_Status_t);

    UT_GenStub_AddParam(MD_DwellLoop, const MD_Wakeup_t *, Msg);

    UT_GenStub_Execute(MD_DwellLoop, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_DwellLoop, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_GetDwellData()
 * ----------------------------------------------------
 */
int32 MD_GetDwellData(uint16 TblIndex, uint16 EntryIndex)
{
    UT_GenStub_SetupReturnBuffer(MD_GetDwellData, int32);

    UT_GenStub_AddParam(MD_GetDwellData, uint16, TblIndex);
    UT_GenStub_AddParam(MD_GetDwellData, uint16, EntryIndex);

    UT_GenStub_Execute(MD_GetDwellData, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_GetDwellData, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_SendDwellPkt()
 * ----------------------------------------------------
 */
void MD_SendDwellPkt(uint16 TableIndex)
{
    UT_GenStub_AddParam(MD_SendDwellPkt, uint16, TableIndex);

    UT_GenStub_Execute(MD_SendDwellPkt, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_StartDwellStream()
 * ----------------------------------------------------
 */
void MD_StartDwellStream(uint16 TableIndex)
{
    UT_GenStub_AddParam(MD_StartDwellStream, uint16, TableIndex);

    UT_GenStub_Execute(MD_StartDwellStream, Basic, NULL);
}
