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

/* MD testing includes */
#include "md_test_utils.h"
#include "md_dispatch.h"
#include "md_eds_dispatcher.h"
#include "md_cmds.h"
#include "md_msgids.h"
#include "md_eventids.h"
#include "md_app.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void md_dispatch_tests_Setup(void)
{
    MD_Test_Setup();
}

void md_dispatch_tests_Teardown(void)
{
    MD_Test_TearDown();
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_MD_ProcessCommandPacket(void)
{
    /*
     * Test Case For:
     * void MD_ProcessCommandPacket
     */
    CFE_SB_Buffer_t UtBuf;

    UT_SetDeferredRetcode(UT_KEY(CFE_EDSMSG_Dispatch), 1, CFE_SUCCESS);

    memset(&UtBuf, 0, sizeof(UtBuf));
    UtAssert_VOIDCALL(MD_ProcessCommandPacket(&UtBuf));
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    UtTest_Add(Test_MD_ProcessCommandPacket, md_dispatch_tests_Setup, md_dispatch_tests_Teardown, "Test_MD_ProcessCommandPacket");
}