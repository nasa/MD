 /*************************************************************************
 ** File:
 **   $Id: md_testrunner.c 1.2 2017/05/22 14:56:30EDT mdeschu Exp  $
 **
 ** Purpose: 
 **   This file contains the unit test runner for the MD application.
 **
 ** References:
 **   Flight Software Branch C Coding Standard Version 1.2
 **   CFS Development Standards Document
 ** Notes:
 **
 *************************************************************************/

/*
 * Includes
 */

#include "uttest.h"
#include "md_app_test.h"
#include "md_cmds_test.h"
#include "md_dwell_pkt_test.h"
#include "md_dwell_tbl_test.h"
#include "md_utils_test.h"

/*
 * Function Definitions
 */

int main(void)
{   
    MD_App_Test_AddTestCases();
    MD_Cmds_Test_AddTestCases();
    MD_Dwell_Pkt_Test_AddTestCases();
    MD_Dwell_Tbl_Test_AddTestCases();
    MD_Utils_Test_AddTestCases();

    return(UtTest_Run());
} /* end main */


/************************/
/*  End of File Comment */
/************************/
