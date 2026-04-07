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
 *   Specification for MD dispatch functions
 */

#ifndef MD_DISPATCH_H
#define MD_DISPATCH_H

#include "cfe.h"

/**
 * \brief Verify command message length
 *
 *  \par Description
 *       This routine will check if the actual length of a software bus
 *       command message matches the expected length and send an
 *       error event message if a mismatch occurs
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   MsgPtr           Pointer to message
 *  \param [in]   ExpectedLength   The expected length of the message
 *                                 based upon the command code
 *
 *  \return Boolean length validation status
 *  \retval true  Length matches expected
 *  \retval false Length does not match expected
 */
bool MD_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

/**
 * \brief Processes a single software bus command pipe message. Checks
 *        the message and command IDs and calls the appropriate routine
 *        to handle the command.
 * \param[in] BufPtr Pointer to Software Bus buffer
 */
void MD_ProcessGroundCommand(const CFE_SB_Buffer_t *BufPtr);

/**
 * \brief Route a message/packet to the command processing function or to
 *        the housekeeping request function
 * \param[in] BufPtr SB buffer pointer to be routed
 */
void MD_TaskPipe(const CFE_SB_Buffer_t *BufPtr);

#endif /* MD_DISPATCH_H */