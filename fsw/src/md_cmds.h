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
 *   Specification for the CFS Memory Dwell ground commands.
 */
#ifndef MD_CMDS_H
#define MD_CMDS_H

/*************************************************************************
 * Includes
 *************************************************************************/

/* md_msg needs to be included for MD_SymAddr_t definition */
#include "md_msg.h"

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
 * \param[in] BufPtr Pointer to Software Bus buffer
 */
void MD_ProcessStartCmd(const CFE_SB_Buffer_t *BufPtr);

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
 * \param[in] BufPtr Pointer to Software Bus buffer
 */
void MD_ProcessStopCmd(const CFE_SB_Buffer_t *BufPtr);

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
 * \param[in] BufPtr Pointer to Software Bus buffer
 */
void MD_ProcessJamCmd(const CFE_SB_Buffer_t *BufPtr);

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
 * \param[in] BufPtr Pointer to Software Bus buffer
 */
void MD_ProcessSignatureCmd(const CFE_SB_Buffer_t *BufPtr);

#endif
