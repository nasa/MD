
/*
 * Includes
 */

#include "md_msg.h"
#include "cfe.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

bool MD_TableIsInMask(int16 TableId, uint16 TableMask)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_TableIsInMask), TableId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_TableIsInMask), TableMask);
    return UT_DEFAULT_IMPL(MD_TableIsInMask) != 0;
}

void MD_UpdateDwellControlInfo(uint16 TableIndex)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_UpdateDwellControlInfo), TableIndex);
    UT_DEFAULT_IMPL(MD_UpdateDwellControlInfo);
}

bool MD_ValidEntryId(uint16 EntryId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidEntryId), EntryId);
    return UT_DEFAULT_IMPL(MD_ValidEntryId) != 0;
}

bool MD_ValidAddrRange(cpuaddr Addr, uint32 Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidAddrRange), Addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidAddrRange), Size);
    return UT_DEFAULT_IMPL(MD_ValidAddrRange) != 0;
}

bool MD_ValidTableId(uint16 TableId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidTableId), TableId);
    return UT_DEFAULT_IMPL(MD_ValidTableId) != 0;
}

bool MD_ValidFieldLength(uint16 FieldLength)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(MD_ValidFieldLength), FieldLength);
    return UT_DEFAULT_IMPL(MD_ValidFieldLength) != 0;
}
