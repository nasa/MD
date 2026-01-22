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
 *   Specification for the CFS Memory Dwell (MD) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_MD_FCNCODE_VALUES_H
#define DEFAULT_MD_FCNCODE_VALUES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define MD_CCVAL(x) MD_FunctionCode_##x

enum MD_FunctionCode_
{
    MD_FunctionCode_NOOP           = 0,
    MD_FunctionCode_RESET_COUNTERS = 1,
    MD_FunctionCode_START_DWELL    = 2,
    MD_FunctionCode_STOP_DWELL     = 3,
    MD_FunctionCode_JAM_DWELL      = 4,
    MD_FunctionCode_SET_SIGNATURE  = 5,
};

#endif /* DEFAULT_MD_FCNCODE_VALUES_H */