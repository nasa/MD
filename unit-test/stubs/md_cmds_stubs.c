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
 * Auto-Generated stub implementations for functions defined in md_cmds header
 */

#include "md_cmds.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for MD_JamDwellCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_JamDwellCmd(const MD_JamDwellCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_JamDwellCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_JamDwellCmd, const MD_JamDwellCmd_t *, Msg);

    UT_GenStub_Execute(MD_JamDwellCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_JamDwellCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_NoopCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_NoopCmd(const MD_NoopCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_NoopCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_NoopCmd, const MD_NoopCmd_t *, Msg);

    UT_GenStub_Execute(MD_NoopCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_NoopCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ResetCountersCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_ResetCountersCmd(const MD_ResetCountersCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_ResetCountersCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_ResetCountersCmd, const MD_ResetCountersCmd_t *, Msg);

    UT_GenStub_Execute(MD_ResetCountersCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ResetCountersCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_SetSignatureCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_SetSignatureCmd(const MD_SetSignatureCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_SetSignatureCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_SetSignatureCmd, const MD_SetSignatureCmd_t *, Msg);

    UT_GenStub_Execute(MD_SetSignatureCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_SetSignatureCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_StartDwellCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_StartDwellCmd(const MD_StartDwellCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_StartDwellCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_StartDwellCmd, const MD_StartDwellCmd_t *, Msg);

    UT_GenStub_Execute(MD_StartDwellCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_StartDwellCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_StopDwellCmd()
 * ----------------------------------------------------
 */
CFE_Status_t MD_StopDwellCmd(const MD_StopDwellCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_StopDwellCmd, CFE_Status_t);

    UT_GenStub_AddParam(MD_StopDwellCmd, const MD_StopDwellCmd_t *, Msg);

    UT_GenStub_Execute(MD_StopDwellCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_StopDwellCmd, CFE_Status_t);
}
