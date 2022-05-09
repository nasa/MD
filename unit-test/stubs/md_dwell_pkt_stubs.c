
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
