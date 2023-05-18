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
 *   Functions used for validating and copying CFS Memory Dwell Tables.
 */
#ifndef MD_DWELL_TBL_H
#define MD_DWELL_TBL_H

/*************************************************************************
 * Includes
 *************************************************************************/

#include "cfe.h"
#include "md_tbldefs.h"

/**
 * \brief Dwell Table Validation Function
 *
 * \par Description
 *  This function indicates whether the Dwell Table corresponding to the
 *  input table pointer is valid.
 *
 * \par Assumptions, External Events, and Notes:
 *  This function gets registered with Table Services as a callback function
 *  for validating Dwell Tables that are loaded from the ground, and so must
 *  be in accordance with the call signature specified by Table Services.
 *  In addition, the function is used by Memory Dwell to validate Dwell Tables
 *  that have been recovered from a Critical Data Store.
 *
 * \param[in] TblPtr Table pointer
 *
 * \return Table validation response
 * \retval #CFE_SUCCESS            \copydoc CFE_SUCCESS
 * \retval #MD_TBL_ENA_FLAG_ERROR  \copydoc MD_TBL_ENA_FLAG_ERROR
 * \retval #MD_ZERO_RATE_TBL_ERROR \copydoc MD_ZERO_RATE_TBL_ERROR
 * \retval #MD_RESOLVE_ERROR       \copydoc MD_RESOLVE_ERROR
 * \retval #MD_INVALID_ADDR_ERROR  \copydoc MD_INVALID_ADDR_ERROR
 * \retval #MD_INVALID_LEN_ERROR   \copydoc MD_INVALID_LEN_ERROR
 * \retval #MD_NOT_ALIGNED_ERROR   \copydoc MD_NOT_ALIGNED_ERROR
 */
int32 MD_TableValidationFunc(void *TblPtr);

/**
 * \brief Generate internal data structures based on Dwell Table Load.
 *
 * \par Description
 *          Copies Enabled field.
 *          Copies Signature field.
 *          For each dwell table entry, copies field length, and delay value.
 *          Evaluates and saves resolved dwell address for each dwell entry.
 *          Evaluates and saves additional summary data based on entry contents.
 *
 * \par Assumptions, External Events, and Notes:
 *          Dwell table contents have been validated before reaching this point.
 *          Run when a table is loaded by command, or when a table is recovered
 *          on start up.
 *
 * \param[in] MD_LoadTablePtr Pointer to Table Services buffer.
 * \param[in] TblIndex An identifier specifying which dwell table is to be
 *             copied.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
 */
void MD_CopyUpdatedTbl(MD_DwellTableLoad_t *MD_LoadTablePtr, uint8 TblIndex);

/**
 * \brief Update Dwell Table's Enabled Field.
 *
 * \par Description
 *          Update Dwell Table's Enabled Field.
 *
 * \par Assumptions, External Events, and Notes:
 *          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
 *          FieldValue is MD_DWELL_STREAM_ENABLED or MD_DWELL_STREAM_DISABLED.
 *
 * \param[in] TableIndex An identifier specifying which dwell table is to be
 *             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
 * \param[in] FieldValue New value for Enabled field.
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t MD_UpdateTableEnabledField(uint16 TableIndex, uint16 FieldValue);

/**
 * \brief Update Values for a Dwell Table Entry.
 *
 * \par Description
 *          Update Values for a Dwell Table Entry.
 *
 * \par Assumptions, External Events, and Notes:
 *          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
 *          EntryIndex is in [0..MD_DWELL_TABLE_SIZE-1] range.
 *          NewLength is 0, 1, 2, or 4.
 *          NewDwellAddress is a valid dwell address.
 *
 * \param[in] TableIndex An identifier specifying which dwell table is to be
 *             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
 * \param[in] EntryIndex An identifier specifying which entry is to be
 *             modified.  Internal values [0..MD_DWELL_TABLE_SIZE-1] are used.
 * \param[in] NewLength         Number of bytes to be read.
 * \param[in] NewDelay          Number of counts before next dwell.
 * \param[in] NewDwellAddress   Memory address to be dwelled on.
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t MD_UpdateTableDwellEntry(uint16 TableIndex, uint16 EntryIndex, uint16 NewLength, uint16 NewDelay,
                                      MD_SymAddr_t NewDwellAddress);

#if MD_SIGNATURE_OPTION == 1

/**
 * \brief Update Dwell Table Signature.
 *
 * \par Description
 *          Update Dwell Table Signature.
 *
 * \par Assumptions, External Events, and Notes:
 *          TableIndex is in [0..MD_NUM_DWELL_TABLES-1] range.
 *
 * \param[in] TableIndex An identifier specifying which dwell table is to be
 *             modified.  Internal values [0..MD_NUM_DWELL_TABLES-1] are used.
 * \param[in] NewSignature   New Dwell Table signature.
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t MD_UpdateTableSignature(uint16 TableIndex, char NewSignature[MD_SIGNATURE_FIELD_LENGTH]);

#endif

/**
 * \brief Read Dwell Table to extract address count, byte size, and rate.
 *
 * \par Description
 *          Read active entries and count number of dwell addresses,
 *          number of bytes to be dwelled on, and number of wakeup calls
 *          between sending of dwell packets.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] TblPtr Table pointer
 * \param[out] *ActiveAddrCountPtr Number of addresses to be sampled.
 * \param[out] *SizePtr            Size, in bytes, of data to be sampled.
 * \param[out] *RatePtr            Rate, in number of wakeup calls, between
 *
 *  \return Execution status, see \ref CFEReturnCodes
 *  \retval #CFE_SUCCESS \copybrief CFE_SUCCESS
 */
CFE_Status_t MD_ReadDwellTable(const MD_DwellTableLoad_t *TblPtr, uint16 *ActiveAddrCountPtr, uint16 *SizePtr,
                               uint32 *RatePtr);

/**
 * \brief Validate dwell table entry.
 *
 * \par Description
 *          Validates whether specified dwell table entry is valid.
 *
 * \par Assumptions, External Events, and Notes:
 *          To be valid, entry must either be a null entry
 *          (specified by a zero field length) or all of its
 *          address and length fields must pass various checks.
 *
 * \param[in] TblEntryPtr Entry pointer
 *
 *
 * \return Dwell table entry validation response
 * \retval #CFE_SUCCESS           \copydoc CFE_SUCCESS
 * \retval #MD_RESOLVE_ERROR      \copydoc MD_RESOLVE_ERROR
 * \retval #MD_INVALID_ADDR_ERROR \copydoc MD_INVALID_ADDR_ERROR
 * \retval #MD_INVALID_LEN_ERROR  \copydoc MD_INVALID_LEN_ERROR
 * \retval #MD_NOT_ALIGNED_ERROR  \copydoc MD_NOT_ALIGNED_ERROR
 */
int32 MD_ValidTableEntry(MD_TableLoadEntry_t *TblEntryPtr);

/**
 * \brief Validate dwell entries in specified Dwell Table.
 *
 * \par Description
 *          Validate memory ranges for dwell address and field length,
 *          and validate field length size for entries in specified Dwell Table.
 *
 * \par Assumptions, External Events, and Notes:
 *          For table to be valid, each entry must be a null entry
 *          (specified by a zero field length) or the entry's address
 *          and length field must pass various checks.
 *
 * \param[in] TblPtr Table pointer
 * \param[out] *ErrorEntryArg  Entry number (0..) of first detected error, if any.
 *
 * \return Check table entry response
 * \retval #CFE_SUCCESS           \copydoc CFE_SUCCESS
 * \retval #MD_RESOLVE_ERROR      \copydoc MD_RESOLVE_ERROR
 * \retval #MD_INVALID_ADDR_ERROR \copydoc MD_INVALID_ADDR_ERROR
 * \retval #MD_INVALID_LEN_ERROR  \copydoc MD_INVALID_LEN_ERROR
 * \retval #MD_NOT_ALIGNED_ERROR  \copydoc MD_NOT_ALIGNED_ERROR
 */
int32 MD_CheckTableEntries(MD_DwellTableLoad_t *TblPtr, uint16 *ErrorEntryArg);

#endif
