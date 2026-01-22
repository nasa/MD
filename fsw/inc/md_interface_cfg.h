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
#ifndef MD_INTERFACE_CFG_H
#define MD_INTERFACE_CFG_H

#include "md_interface_cfg_values.h"

/**
 * \defgroup cfsmdplatformcfg CFS Memory Dwell Platform Configuration
 * \{
 */

/**
 *  \brief Number of characters used in the Signature Field
 *
 *  \par Limits
 *       Signature field length needs to be a multiple of 4 so that
 *       dwell packet is a multiple of 4 bytes and no compiler padding
 *       will occur. Note that the final character of the signature string
 *       must be a null character, so the effective length of user definable
 *       characters is one less than the defined length. The length
 *       specified must therefore be at least 4.
 */
#define MD_INTERFACE_SIGNATURE_FIELD_LENGTH         MD_INTERFACE_CFGVAL(SIGNATURE_FIELD_LENGTH)
#define DEFAULT_MD_INTERFACE_SIGNATURE_FIELD_LENGTH 32

/**
 *  \brief Number of memory dwell tables.
 *
 *  \par Limits
 *       Acceptable values for this parameter are 1 to 16.
 */
#define MD_INTERFACE_NUM_DWELL_TABLES         MD_INTERFACE_CFGVAL(NUM_DWELL_TABLES)
#define DEFAULT_MD_INTERFACE_NUM_DWELL_TABLES 4

/**
 *  \brief Maximum number of dwell specifications (address/delay/length)
 *    in a Dwell Table.
 *
 *  \par Limits
 *       The maximum value for this parameter is limited by its effect
 *       on the MD_DwellTableLoad_t and associated data points, limiting
 *       it to 65535 points.
 */
#define MD_INTERFACE_DWELL_TABLE_SIZE         MD_INTERFACE_CFGVAL(DWELL_TABLE_SIZE)
#define DEFAULT_MD_INTERFACE_DWELL_TABLE_SIZE 25

/**
 *  \brief Option of whether 32 bit integers must be aligned to 32 bit
 *   boundaries.  1 indicates 'yes' (32-bit boundary alignment enforced),
 *   0 indicates 'no' (16-bit boundary alignment enforced).
 *
 *  \par Limits
 *       Value must be 0 or 1.
 */
#define MD_INTERFACE_ENFORCE_DWORD_ALIGN         MD_INTERFACE_CFGVAL(ENFORCE_DWORD_ALIGN)
#define DEFAULT_MD_INTERFACE_ENFORCE_DWORD_ALIGN 1

/**
 *  \brief Option of whether a signature field will be reserved in
 *     dwell packets.  1 indicates 'yes', 0 indicates 'no'.
 *
 *  \par Limits
 *       Value must be 0 or 1.
 */
#define MD_INTERFACE_SIGNATURE_OPTION         MD_INTERFACE_CFGVAL(SIGNATURE_OPTION)
#define DEFAULT_MD_INTERFACE_SIGNATURE_OPTION 1

/**
 * \}
 */

#endif /* MD_INTERFACE_CFG_H */