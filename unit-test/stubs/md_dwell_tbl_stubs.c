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
 * Auto-Generated stub implementations for functions defined in md_dwell_tbl header
 */

#include "md_dwell_tbl.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for MD_CheckTableEntries()
 * ----------------------------------------------------
 */
int32 MD_CheckTableEntries(MD_DwellTableLoad_t *TblPtr, uint16 *ErrorEntryArg)
{
    UT_GenStub_SetupReturnBuffer(MD_CheckTableEntries, int32);

    UT_GenStub_AddParam(MD_CheckTableEntries, MD_DwellTableLoad_t *, TblPtr);
    UT_GenStub_AddParam(MD_CheckTableEntries, uint16 *, ErrorEntryArg);

    UT_GenStub_Execute(MD_CheckTableEntries, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_CheckTableEntries, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_CopyUpdatedTbl()
 * ----------------------------------------------------
 */
void MD_CopyUpdatedTbl(MD_DwellTableLoad_t *MD_LoadTablePtr, uint8 TblIndex)
{
    UT_GenStub_AddParam(MD_CopyUpdatedTbl, MD_DwellTableLoad_t *, MD_LoadTablePtr);
    UT_GenStub_AddParam(MD_CopyUpdatedTbl, uint8, TblIndex);

    UT_GenStub_Execute(MD_CopyUpdatedTbl, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ReadDwellTable()
 * ----------------------------------------------------
 */
CFE_Status_t MD_ReadDwellTable(const MD_DwellTableLoad_t *TblPtr, uint16 *ActiveAddrCountPtr, uint16 *SizePtr,
                               uint32 *RatePtr)
{
    UT_GenStub_SetupReturnBuffer(MD_ReadDwellTable, CFE_Status_t);

    UT_GenStub_AddParam(MD_ReadDwellTable, const MD_DwellTableLoad_t *, TblPtr);
    UT_GenStub_AddParam(MD_ReadDwellTable, uint16 *, ActiveAddrCountPtr);
    UT_GenStub_AddParam(MD_ReadDwellTable, uint16 *, SizePtr);
    UT_GenStub_AddParam(MD_ReadDwellTable, uint32 *, RatePtr);

    UT_GenStub_Execute(MD_ReadDwellTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ReadDwellTable, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_TableValidationFunc()
 * ----------------------------------------------------
 */
int32 MD_TableValidationFunc(void *TblPtr)
{
    UT_GenStub_SetupReturnBuffer(MD_TableValidationFunc, int32);

    UT_GenStub_AddParam(MD_TableValidationFunc, void *, TblPtr);

    UT_GenStub_Execute(MD_TableValidationFunc, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_TableValidationFunc, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_UpdateTableDwellEntry()
 * ----------------------------------------------------
 */
CFE_Status_t MD_UpdateTableDwellEntry(uint16 TableIndex, uint16 EntryIndex, uint16 NewLength, uint16 NewDelay,
                                      MD_SymAddr_t NewDwellAddress)
{
    UT_GenStub_SetupReturnBuffer(MD_UpdateTableDwellEntry, CFE_Status_t);

    UT_GenStub_AddParam(MD_UpdateTableDwellEntry, uint16, TableIndex);
    UT_GenStub_AddParam(MD_UpdateTableDwellEntry, uint16, EntryIndex);
    UT_GenStub_AddParam(MD_UpdateTableDwellEntry, uint16, NewLength);
    UT_GenStub_AddParam(MD_UpdateTableDwellEntry, uint16, NewDelay);
    UT_GenStub_AddParam(MD_UpdateTableDwellEntry, MD_SymAddr_t, NewDwellAddress);

    UT_GenStub_Execute(MD_UpdateTableDwellEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_UpdateTableDwellEntry, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_UpdateTableEnabledField()
 * ----------------------------------------------------
 */
CFE_Status_t MD_UpdateTableEnabledField(uint16 TableIndex, uint16 FieldValue)
{
    UT_GenStub_SetupReturnBuffer(MD_UpdateTableEnabledField, CFE_Status_t);

    UT_GenStub_AddParam(MD_UpdateTableEnabledField, uint16, TableIndex);
    UT_GenStub_AddParam(MD_UpdateTableEnabledField, uint16, FieldValue);

    UT_GenStub_Execute(MD_UpdateTableEnabledField, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_UpdateTableEnabledField, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_UpdateTableSignature()
 * ----------------------------------------------------
 */
CFE_Status_t MD_UpdateTableSignature(uint16 TableIndex, const char NewSignature[MD_INTERFACE_SIGNATURE_FIELD_LENGTH])
{
    UT_GenStub_SetupReturnBuffer(MD_UpdateTableSignature, CFE_Status_t);

    UT_GenStub_AddParam(MD_UpdateTableSignature, uint16, TableIndex);

    UT_GenStub_Execute(MD_UpdateTableSignature, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_UpdateTableSignature, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for MD_ValidTableEntry()
 * ----------------------------------------------------
 */
int32 MD_ValidTableEntry(MD_TableLoadEntry_t *TblEntryPtr)
{
    UT_GenStub_SetupReturnBuffer(MD_ValidTableEntry, int32);

    UT_GenStub_AddParam(MD_ValidTableEntry, MD_TableLoadEntry_t *, TblEntryPtr);

    UT_GenStub_Execute(MD_ValidTableEntry, Basic, NULL);

    return UT_GenStub_GetReturnValue(MD_ValidTableEntry, int32);
}
