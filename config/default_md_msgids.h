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
 *   Specification for the CFS Memory Dwell macro constants that can
 *   be configured from one mission to another
 */
#ifndef DEFAULT_MD_MSGIDS_H
#define DEFAULT_MD_MSGIDS_H

#include "cfe_core_api_base_msgids.h"
#include "md_msgid_values.h"

/**
 * \defgroup cfsmdcmdmid CFS Memory Dwell Command Message IDs
 * \{
 */

/** \brief Message Id for Memory Dwell's ground command      */
#define MD_CMD_MID CFE_PLATFORM_MD_CMD_MIDVAL(CMD)

/** \brief Message Id for Memory Dwell's 'Send Housekeeping' message  */
#define MD_SEND_HK_MID CFE_PLATFORM_MD_CMD_MIDVAL(SEND_HK)

/** \brief Message Id for Memory Dwell's wakeup message       */
#define MD_WAKEUP_MID CFE_PLATFORM_MD_CMD_MIDVAL(WAKEUP)

/**\}*/

/**
 * \defgroup cfsmdtlmmid CFS Memory Dwell Telemetry Message IDs
 * \{
 */

/** \brief Message Id for Memory Dwell's housekeeping pkt  */
#define MD_HK_TLM_MID CFE_PLATFORM_MD_TLM_MIDVAL(HK_TLM)

/** \brief Base Message Id for Memory Dwell's dwell packets. */
#define MD_DWELL_PKT_MID_BASE CFE_PLATFORM_MD_TLM_MIDVAL(DWELL_PKT_MID_BASE)

/**\}*/

#endif /* DEFAULT_MD_MSGIDS_H */
