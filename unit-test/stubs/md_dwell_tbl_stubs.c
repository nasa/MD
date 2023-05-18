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

#include "cfe.h"
#include "md_tbldefs.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

int32 MD_TableValidationFunc(void *TblPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_TableValidationFunc), TblPtr);
    return UT_DEFAULT_IMPL(MD_TableValidationFunc);
}

void MD_CopyUpdatedTbl(MD_DwellTableLoad_t *MD_LoadTablePtr, uint8 TblIndex)
{
    UT_Stub_RegisterContext(UT_KEY(MD_CopyUpdatedTbl), MD_LoadTablePtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_CopyUpdatedTbl), TblIndex);
    UT_DEFAULT_IMPL(MD_CopyUpdatedTbl);
}

CFE_Status_t MD_UpdateTableEnabledField(uint16 TableIndex, uint16 FieldValue)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableEnabledField), TableIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableEnabledField), FieldValue);
    return UT_DEFAULT_IMPL(MD_UpdateTableEnabledField);
}

CFE_Status_t MD_UpdateTableDwellEntry(uint16 TableIndex, uint16 EntryIndex, uint16 NewLength, uint16 NewDelay,
                                      MD_SymAddr_t NewDwellAddress)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), TableIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), EntryIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewLength);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewDelay);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewDwellAddress);
    return UT_DEFAULT_IMPL(MD_UpdateTableDwellEntry);
}

CFE_Status_t MD_UpdateTableSignature(uint16 TableIndex, char NewSignature[MD_SIGNATURE_FIELD_LENGTH])
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableSignature), TableIndex);
    UT_Stub_RegisterContext(UT_KEY(MD_UpdateTableSignature), NewSignature);
    return UT_DEFAULT_IMPL(MD_UpdateTableSignature);
}

CFE_Status_t MD_ReadDwellTable(MD_DwellTableLoad_t *TblPtr, uint16 *ActiveAddrCountPtr, uint16 *SizePtr,
                               uint32 *RatePtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ReadDwellTable), TblPtr);
    UT_Stub_RegisterContext(UT_KEY(MD_ReadDwellTable), ActiveAddrCountPtr);
    UT_Stub_RegisterContext(UT_KEY(MD_ReadDwellTable), SizePtr);
    UT_Stub_RegisterContext(UT_KEY(MD_ReadDwellTable), RatePtr);
    return UT_DEFAULT_IMPL(MD_ReadDwellTable);
}

int32 MD_ValidTableEntry(MD_TableLoadEntry_t *TblEntryPtr)
{
    UT_Stub_RegisterContext(UT_KEY(MD_ValidTableEntry), TblEntryPtr);
    return UT_DEFAULT_IMPL(MD_ValidTableEntry);
}

int32 MD_CheckTableEntries(MD_DwellTableLoad_t *TblPtr, uint16 *ErrorEntryArg)
{
    UT_Stub_RegisterContext(UT_KEY(MD_CheckTableEntries), TblPtr);
    UT_Stub_RegisterContext(UT_KEY(MD_CheckTableEntries), ErrorEntryArg);
    return UT_DEFAULT_IMPL(MD_CheckTableEntries);
}
