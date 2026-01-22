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
 *   Specification for the CFS Memory Dwell ground commands.
 */
#ifndef MD_CMDS_H
#define MD_CMDS_H

/*************************************************************************
 * Includes
 *************************************************************************/
#include "cfe.h"
#include "md_msg.h"

/**
 * \brief Noop command
 *
 *  \par Description
 *       Processes a noop ground command.
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Msg Pointer to Noop Command
 *
 *  \sa #MD_NOOP_CC
 */
CFE_Status_t MD_NoopCmd(const MD_NoopCmd_t *Msg);

/**
 * \brief Reset counters command
 *
 *  \par Description
 *       Processes a reset counters ground command which will reset
 *       the following MD application counters to zero:
 *         - Command counter
 *         - Command error counter
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param[in] Msg Pointer to Reset Counters Command
 *
 *  \sa #MD_RESET_CNTRS_CC
 */
CFE_Status_t MD_ResetCountersCmd(const MD_ResetCountersCmd_t *Msg);

/**
 * \brief Process Memory Dwell Start Command
 *
 * \par Description
 *          Extract command arguments, take appropriate actions,
 *          issue event, and increment the command counter or
 *          error counter as appropriate.
 *
 * \par Assumptions, External Events, and Notes:
 *          Correct message length has been verified.
 *
 * \param[in] Msg Pointer to Start Dwell Command
 */
CFE_Status_t MD_StartDwellCmd(const MD_StartDwellCmd_t *Msg);

/**
 * \brief Process Memory Dwell Stop Command
 *
 * \par Description
 *          Extract command arguments, take appropriate actions,
 *          issue event, and increment the command counter or
 *          error counter as appropriate.
 *
 * \par Assumptions, External Events, and Notes:
 *          Correct message length has been verified.
 *
 * \param[in] Msg Pointer to Stop Dwell Command
 */
CFE_Status_t MD_StopDwellCmd(const MD_StopDwellCmd_t *Msg);

/**
 * \brief Process Memory Dwell Jam Command
 *
 * \par Description
 *          Extract command arguments, take appropriate actions,
 *          issue event, and increment the command counter or
 *          error counter as appropriate.
 *
 * \par Assumptions, External Events, and Notes:
 *          Correct message length has been verified.
 *
 * \param[in] Msg Pointer to Jam Dwell Command
 */
CFE_Status_t MD_JamDwellCmd(const MD_JamDwellCmd_t *Msg);

/**
 * \brief Process Set Signature Command
 *
 * \par Description
 *          Extract command arguments, take appropriate actions,
 *          issue event, and increment the command counter or
 *          error counter as appropriate.
 *
 * \par Assumptions, External Events, and Notes:
 *          Correct message length has been verified.
 *
 * \param[in] Msg Pointer to Set Signature Command
 */
CFE_Status_t MD_SetSignatureCmd(const MD_SetSignatureCmd_t *Msg);

#endif /* MD_CMDS_H */
