
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

int32 MD_UpdateTableEnabledField(uint16 TableIndex, uint16 FieldValue)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableEnabledField), TableIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableEnabledField), FieldValue);
    return UT_DEFAULT_IMPL(MD_UpdateTableEnabledField);
}

int32 MD_UpdateTableDwellEntry(uint16 TableIndex, uint16 EntryIndex, uint16 NewLength, uint16 NewDelay,
                               CFS_SymAddr_t NewDwellAddress)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), TableIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), EntryIndex);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewLength);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewDelay);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableDwellEntry), NewDwellAddress);
    return UT_DEFAULT_IMPL(MD_UpdateTableDwellEntry);
}

int32 MD_UpdateTableSignature(uint16 TableIndex, char NewSignature[MD_SIGNATURE_FIELD_LENGTH])
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateTableSignature), TableIndex);
    UT_Stub_RegisterContext(UT_KEY(MD_UpdateTableSignature), NewSignature);
    return UT_DEFAULT_IMPL(MD_UpdateTableSignature);
}

int32 MD_ReadDwellTable(MD_DwellTableLoad_t *TblPtr, uint16 *ActiveAddrCountPtr, uint16 *SizePtr, uint32 *RatePtr)
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
