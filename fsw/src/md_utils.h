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
 *   Specification for the CFS Memory Dwell utility functions.
 */
#ifndef MD_UTILS_H
#define MD_UTILS_H

/*************************************************************************
 * Includes
 *************************************************************************/

#include "md_msg.h" /* For MD_SymAddr_t */
#include "cfe.h"

/**
 * \brief  Determine if specified TableId is contained in argument mask.
 *
 * \par Description
 *          Determines whether specified Table Id is contained in argument mask.
 *
 * \param[in] TableId    identifies dwell table  (1..#MD_NUM_DWELL_TABLES)
 * \param[in] TableMask  Mask representing current status of all dwell tables.
 *
 * \return Boolean table in mask result
 * \retval true  Table is in argument mask
 * \retval false Table is not in argument mask
 */
bool MD_TableIsInMask(int16 TableId, uint16 TableMask);

/**
 * \brief Update Dwell Table Control Info
 *
 * \par Description
 *          Updates the control structure used by the application for
 *          dwell packet processing with address count, data size, and rate.
 *
 * \par Assumptions, External Events, and Notes:
 *          A zero value for length in a dwell table entry
 *    represents the end of the active portion of a dwell table.
 *
 * \param[in] TableIndex identifies dwell control structure  (0..#MD_NUM_DWELL_TABLES-1)
 */
void MD_UpdateDwellControlInfo(uint16 TableIndex);

/**
 * \brief Validate Entry Index
 *
 * \par Description
 *        Checks for valid value (1..MD_DWELL_TABLE_SIZE ) for entry id
 *        specified in Jam command.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] EntryId  EntryId (starting at one) for dwell control structure entry.
 *
 * \return Boolean entry validation result
 * \retval true  Entry is valid
 * \retval false Entry is not valid
 */
bool MD_ValidEntryId(uint16 EntryId);

/**
 * \brief Validate Dwell Address
 *
 * \par Description
 *        This function validates that the memory range as specified by the
 *        input address and size is valid for reading.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in] Addr  Dwell address.
 * \param[in] Size Size, in bytes, of field to be read.
 *
 * \return Boolean address is valid result
 * \retval true  Address is valid
 * \retval false Address is not valid
 */
bool MD_ValidAddrRange(cpuaddr Addr, size_t Size);

/**
 * \brief Validate Table ID
 *
 * \par Description
 *        Check valid range for TableId argument used in several
 *        Memory Dwell commands.
 *        Valid range is 1..#MD_NUM_DWELL_TABLES.
 *
 * \par Assumptions, External Events, and Notes:
 *        Note that this value will be internally converted to
 *        0..(#MD_NUM_DWELL_TABLES-1) for indexing into arrays.
 *
 * \param[in] TableId  Table ID.
 *
 * \return Boolean table ID valid result
 * \retval true  Table ID valid
 * \retval false Table ID not valid
 */
bool MD_ValidTableId(uint16 TableId);

/**
 * \brief Validate Field Length
 *
 * \par Description
 *        Check valid range for dwell field length.
 *
 * \par Assumptions, External Events, and Notes:
 *   Valid values for dwell field length are 0, 1, 2, and 4.
 *   0 corresponds to a null entry in Dwell Table.
 *
 * \param[in] FieldLength  Length of field, in bytes, to be copied for dwell.
 *
 * \return Boolean field length valid result
 * \retval true  Field length valid
 * \retval false Field length not valid
 */
bool MD_ValidFieldLength(uint16 FieldLength);

/**
 * \brief Verify 32 bit alignment
 *
 *  \par Description
 *       This routine will check an address and data size argument pair
 *       for correct 32 bit alignment
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   Address   The address to check for proper alignment
 *  \param [in]   Size      The size in bytes to check for proper
 *                          alignment
 *
 *  \return Boolean 32-bit alignment result
 *  \retval true  Both Address and Size 32-bit aligned
 *  \retval false Either Address or Size are not 32-bit aligned
 *
 *  \sa #MD_Verify16Aligned
 */
bool MD_Verify32Aligned(cpuaddr Address, size_t Size);

/**
 * \brief Verify 16 bit alignment
 *
 *  \par Description
 *       This routine will check an address and data size argument pair
 *       for correct 16 bit alignment
 *
 *  \par Assumptions, External Events, and Notes:
 *       None
 *
 *  \param [in]   Address   The address to check for proper alignment
 *  \param [in]   Size      The size in bytes to check for proper
 *                          alignment
 *
 *  \return Boolean 16-bit alignment result
 *  \retval true  Both Address and Size 16-bit aligned
 *  \retval false Either Address or Size are not 16-bit aligned
 *
 *  \sa #MD_Verify32Aligned
 */
bool MD_Verify16Aligned(cpuaddr Address, size_t Size);

/**
 * \brief Resolve symbolic address
 *
 *  \par Description
 *       This routine will resolve a symbol name and optional address
 *       offset to an absolute address
 *
 *  \par Assumptions, External Events, and Notes:
 *       If the symbol name is a NUL (empty) string, then the offset
 *       becomes the absolute address
 *
 *  \param [in]   SymAddr          A #MD_SymAddr_t pointer that holds
 *                                 the symbol name and optional offset
 *  \param [out]  ResolvedAddr     The fully resolved address. Only valid
 *                                 if the return value is TRUE
 *
 *  \return Boolean symbolic address resolved result
 *  \retval true  Symbolic address was resolved
 *  \retval false Symbolic address was not resolved
 *
 *  \sa #OS_SymbolLookup
 */
bool MD_ResolveSymAddr(MD_SymAddr_t *SymAddr, cpuaddr *ResolvedAddr);

#endif
