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
 * Auto-Generated stub implementations for functions defined in md_app header
 */

#include "md_app.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for MD_AppInit()
 * ----------------------------------------------------
 */
CFE_Status_t MD_AppInit(void)
{
    UT_GenStub_SetupReturnBuffer(MD_AppInit, CFE_Status_t);

    UT_GenStub_Execute(MD_AppInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_AppInit, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_AppMain()
 * ----------------------------------------------------
 */
void MD_AppMain(void)
{

    UT_GenStub_Execute(MD_AppMain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_HkStatus()
 * ----------------------------------------------------
 */
CFE_Status_t MD_HkStatus(const MD_SendHkCmd_t *Msg)
{
    UT_GenStub_SetupReturnBuffer(MD_HkStatus, CFE_Status_t);

    UT_GenStub_AddParam(MD_HkStatus, const MD_SendHkCmd_t *, Msg);

    UT_GenStub_Execute(MD_HkStatus, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_HkStatus, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_InitControlStructures()
 * ----------------------------------------------------
 */
void MD_InitControlStructures(void)
{

    UT_GenStub_Execute(MD_InitControlStructures, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_InitSoftwareBusServices()
 * ----------------------------------------------------
 */
CFE_Status_t MD_InitSoftwareBusServices(void)
{
    UT_GenStub_SetupReturnBuffer(MD_InitSoftwareBusServices, CFE_Status_t);

    UT_GenStub_Execute(MD_InitSoftwareBusServices, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_InitSoftwareBusServices, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_InitTableServices()
 * ----------------------------------------------------
 */
CFE_Status_t MD_InitTableServices(void)
{
    UT_GenStub_SetupReturnBuffer(MD_InitTableServices, CFE_Status_t);

    UT_GenStub_Execute(MD_InitTableServices, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_InitTableServices, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ManageDwellTable()
 * ----------------------------------------------------
 */
CFE_Status_t MD_ManageDwellTable(uint8 TblIndex)
{
    UT_GenStub_SetupReturnBuffer(MD_ManageDwellTable, CFE_Status_t);

    UT_GenStub_AddParam(MD_ManageDwellTable, uint8, TblIndex);

    UT_GenStub_Execute(MD_ManageDwellTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ManageDwellTable, CFE_Status_t);
}
