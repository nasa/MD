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
 *   Specification for the CFS Memory Dwell macro constants that can
 *   be configured from one mission to another
 */
#ifndef MD_MSGIDS_H
#define MD_MSGIDS_H

/**
 * \defgroup cfsmdcmdmid CFS Memory Dwell Command Message IDs
 * \{
 */

#define MD_CMD_MID     0x1890 /**< \brief Message Id for Memory Dwell's ground command */
#define MD_SEND_HK_MID 0x1891 /**< \brief Message Id for Memory Dwell's 'Send Housekeeping' message */
#define MD_WAKEUP_MID  0x1892 /**< \brief Message Id for Memory Dwell's wakeup message */

/**\}*/

/**
 * \defgroup cfsmdtlmmid CFS Memory Dwell Telemetry Message IDs
 * \{
 */

#define MD_HK_TLM_MID         0x0890 /**< \brief Message Id for Memory Dwell's housekeeping pkt */
#define MD_DWELL_PKT_MID_BASE 0x0891 /**< \brief Base Message Id for Memory Dwell's dwell packets. */

/**\}*/

#endif
