 /*************************************************************************
 ** File:
 **   $Id: md_test_utils.c 1.2 2017/05/22 14:56:37EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains unit test utilities for the md application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 *************************************************************************/

#include "md_test_utils.h"
#include "md_tbldefs.h"

/*
 * Function Definitions
 */

void MD_Test_Setup(void)
{
    /* initialize test environment to default state for every test */

    CFE_PSP_MemSet(&MD_AppData, 0, sizeof(MD_AppData_t));

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
} /* end MD_Test_Setup */

void MD_Test_TearDown(void)
{
    /* cleanup test environment */
} /* end MD_Test_TearDown */

/************************/
/*  End of File Comment */
/************************/
