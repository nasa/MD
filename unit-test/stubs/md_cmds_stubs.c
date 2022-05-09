
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
