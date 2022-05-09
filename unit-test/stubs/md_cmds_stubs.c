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

#include "md_msg.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void MD_ProcessStartCmd(CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ProcessStartCmd), BufPtr);
    UT_DEFAULT_IMPL(MD_ProcessStartCmd);
}

void MD_ProcessStopCmd(CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ProcessStopCmd), BufPtr);
    UT_DEFAULT_IMPL(MD_ProcessStopCmd);
}

void MD_ProcessJamCmd(CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ProcessJamCmd), BufPtr);
    UT_DEFAULT_IMPL(MD_ProcessJamCmd);
}

void MD_ProcessSignatureCmd(CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ProcessSignatureCmd), BufPtr);
    UT_DEFAULT_IMPL(MD_ProcessSignatureCmd);
}
