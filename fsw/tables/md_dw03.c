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

/**
 * @file
 *  The CFS Memory Dwell (MD) Example Dwell Table 3
 */

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "md_tbldefs.h"
#include "md_extern_typedefs.h"
#include "cfe_tbl_filedef.h"
#include "md_platform_cfg.h"

MD_DwellTableLoad_t MD_Default_Dwell3_Tbl = {
    /* Enabled State */ MD_DWELL_STREAM_DISABLED,
#if MD_SIGNATURE_OPTION == 1
    /* Signature     */ "Default Table 3",
#endif
    /* Entry    Length    Delay    Offset           SymName     */
    /*   1 */
    {
        {0, 0, {0, ""}},
        /*   2 */ {0, 0, {0, ""}},
        /*   3 */ {0, 0, {0, ""}},
        /*   4 */ {0, 0, {0, ""}},
        /*   5 */ {0, 0, {0, ""}},
        /*   6 */ {0, 0, {0, ""}},
        /*   7 */ {0, 0, {0, ""}},
        /*   8 */ {0, 0, {0, ""}},
        /*   9 */ {0, 0, {0, ""}},
        /*  10 */ {0, 0, {0, ""}},
        /*  11 */ {0, 0, {0, ""}},
        /*  12 */ {0, 0, {0, ""}},
        /*  13 */ {0, 0, {0, ""}},
        /*  14 */ {0, 0, {0, ""}},
        /*  15 */ {0, 0, {0, ""}},
        /*  16 */ {0, 0, {0, ""}},
        /*  17 */ {0, 0, {0, ""}},
        /*  18 */ {0, 0, {0, ""}},
        /*  19 */ {0, 0, {0, ""}},
        /*  20 */ {0, 0, {0, ""}},
        /*  21 */ {0, 0, {0, ""}},
        /*  22 */ {0, 0, {0, ""}},
        /*  23 */ {0, 0, {0, ""}},
        /*  24 */ {0, 0, {0, ""}},
        /*  25 */ {0, 0, {0, ""}},
    }};

CFE_TBL_FILEDEF(MD_Default_Dwell3_Tbl, MD.DWELL_TABLE3, MD Dwell Table 3, md_dw03.tbl)
