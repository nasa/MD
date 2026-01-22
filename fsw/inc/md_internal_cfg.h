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
#ifndef MD_INTERNAL_CFG_H
#define MD_INTERNAL_CFG_H

#include "md_internal_cfg_values.h"

/**
 * \brief Command Pipe Name
 *
 *  \par Description:
 *       Software bus name for the command pipe
 *
 *  \par Limits:
 *       The string must be no longer than #OS_MAX_API_NAME (including terminator).
 */
#define MD_INTERNAL_PIPE_NAME          MD_INTERNAL_CFGVAL(PIPE_NAME)
#define DEFAULT_MD_INTERNAL_PIPE_NAME "MD_CMD_PIPE"

/**
 * \brief Command Pipe Depth
 *
 *  \par Description:
 *       Maximum number of messages that will be allowed in the
 *       command pipe at one time. Used during initialization
 *       in the call to #CFE_SB_CreatePipe
 *
 *  \par Limits:
 *       This parameter can't be larger than an unsigned 16 bit
 *       integer (65535).
 */
#define MD_INTERNAL_PIPE_DEPTH         MD_INTERNAL_CFGVAL(PIPE_DEPTH)
#define DEFAULT_MD_INTERNAL_PIPE_DEPTH 50

/**
 *  \brief Memory Dwell Base Filename
 *
 *  \par Description:
 *       Default base name and location for Memory Dwell filenames
 *       AppInit will append 01,02,03, up to number of tables.
 *
 *  \par Limits:
 *       This string shouldn't be longer than #OS_MAX_PATH_LEN for the
 *       target platform in question
 */
#define MD_INTERNAL_TBL_FILENAME_FORMAT         MD_INTERNAL_CFGVAL(TBL_FILENAME_FORMAT)
#define DEFAULT_MD_INTERNAL_TBL_FILENAME_FORMAT "/cf/md_dw%02d.tbl"

/**
 * \brief Mission specific version number for MD application
 *
 *  \par Description:
 *       An application version number consists of four parts:
 *       major version number, minor version number, revision
 *       number and mission specific revision number. The mission
 *       specific revision number is defined here and the other
 *       parts are defined in "md_version.h".
 *
 *  \par Limits:
 *       Must be defined as a numeric value that is greater than
 *       or equal to zero.
 */
#define MD_INTERNAL_MISSION_REV         MD_INTERNAL_CFGVAL(MISSION_REV)
#define DEFAULT_MD_INTERNAL_MISSION_REV 0

#endif /* MD_INTERNAL_CFG_H */