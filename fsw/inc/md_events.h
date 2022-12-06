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
 *  The CFS Memory Dwell (MD) Application event id header file
 */
#ifndef MD_EVENTS_H
#define MD_EVENTS_H

/**
 * \defgroup cfsmdevents CFS Memory Dwell Event IDs
 * \{
 */

/**
 * \brief MD Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued upon successful completion of task initialization.
 */
#define MD_INIT_INF_EID 1

/**
 * \brief MD Read Software Bus Pipe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued following error return from #CFE_SB_ReceiveBuffer call.
 */
#define MD_PIPE_ERR_EID 2

/**
 * \brief MD Dwell Table Recovered Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued upon successful recovery of a Dwell Table.
 */
#define MD_RECOVERED_TBL_VALID_INF_EID 3

/**
 * \brief MD Dwell Table Recovery Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Issued when a Dwell Table is recovered and found to be invalid.
 */
#define MD_RECOVERED_TBL_NOT_VALID_ERR_EID 4

/**
 * \brief MD Dwell Table Registration Too Large Event ID
 *
 *  \par Type: CRITICAL
 *
 *  \par Cause:
 *
 *  Issued when a #CFE_TBL_ERR_INVALID_SIZE error message is received from #CFE_TBL_Register call.
 *  Load structure can be reduced by reducing #MD_DWELL_TABLE_SIZE, number of entries per Dwell Table.
 */
#define MD_DWELL_TBL_TOO_LARGE_CRIT_EID 5

/**
 * \brief MD Dwell Table Registration Failed Event ID
 *
 *  \par Type: CRITICAL
 *
 *  \par Cause:
 *
 *  Issued when an error message, other than #CFE_TBL_ERR_INVALID_SIZE, is received from #CFE_TBL_Register call.
 */
#define MD_TBL_REGISTER_CRIT_EID 6

/**
 * \brief MD Dwell Table Initialization Complete Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued at the end of Table Initialization, specifying how many tables were recovered and how many initialized.
 */
#define MD_TBL_INIT_INF_EID 7

/**
 * \brief MD No-op Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued upon receipt of a Memory Dwell no-op command.
 */
#define MD_NOOP_INF_EID 10

/**
 * \brief MD Reset Counters Command Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  Issued upon receipt of a Memory Dwell Reset Counters command.
 */
#define MD_RESET_CNTRS_DBG_EID 11

/**
 * \brief MD Start Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued upon receipt of a Memory Dwell Start command.
 *  Upon receipt of this command, the specified tables are started for processing.
 */
#define MD_START_DWELL_INF_EID 12

/**
 * \brief MD Stop Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Issued upon receipt of a Memory Dwell Start command.
 *  Upon receipt of this command, the specified tables are stopped.
 */
#define MD_STOP_DWELL_INF_EID 13

/**
 * \brief MD Start Command No Tables Specified Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  None of the valid Table Ids (1..#MD_NUM_DWELL_TABLES) are contained in
 *  the table mask argument for the Start Dwell or Stop Dwell command.
 */
#define MD_EMPTY_TBLMASK_ERR_EID 14

/**
 * \brief MD Message ID Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued if the Memory Dwell task receives a message
 *  with an unrecognized Message ID.
 */
#define MD_MID_ERR_EID 15

/**
 * \brief MD Command Code Not In Table Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued when a command for the Memory Dwell task is
 *  received with a function code which is not listed in the internal
 *  MD_CmdHandlerTbl structure, which is used to associate an expected
 *  length for the command.
 */
#define MD_CC_NOT_IN_TBL_ERR_EID 16

/**
 * \brief MD Dwell Table Get Status Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued on receipt of an unexpected error message from
 *  CFE_TBL_GetStatus.  Normal processing continues; no special action is taken.
 */
#define MD_TBL_STATUS_ERR_EID 20

/**
 * \brief MD Command Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued when the Memory Dwell task receives a command
 *  which has a length that is inconsistent with the expected length
 *  for its command code.
 */
#define MD_CMD_LEN_ERR_EID 21

/**
 * \brief MD Message Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued when the Memory Dwell task receives a message
 *  which has a length that is inconsistent with the expected length
 *  for its message id.
 */
#define MD_MSG_LEN_ERR_EID 22

/**
 * \brief MD Jam To Dwell Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event is issued for a successful jam operation.
 */
#define MD_JAM_DWELL_INF_EID 30

/**
 * \brief MD Jam Null Dwell Entry Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event is issued for a jam operation in which a null dwell entry is specified.
 *  A null entry is specified when the input dwell length is zero.
 *  All dwell fields (address, length, and delay) will be set to zero in this case.
 */
#define MD_JAM_NULL_DWELL_INF_EID 31

/**
 * \brief MD Jam Command Table Id Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when a Jam Dwell Command is received
 *  with an invalid value for the table id argument.
 *  Values in the range 1..#MD_NUM_DWELL_TABLES are expected.
 */
#define MD_INVALID_JAM_TABLE_ERR_EID 32

/**
 * \brief MD Jam Command Entry Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when a Jam Dwell Command is received
 *  with an invalid value for the entry id argument.
 *  Values in the range 1..#MD_DWELL_TABLE_SIZE are expected.
 */
#define MD_INVALID_ENTRY_ARG_ERR_EID 33

/**
 * \brief MD Jam Command Field Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when a Jam Dwell Command is received
 *  with an invalid value for the field length argument.
 */
#define MD_INVALID_LEN_ARG_ERR_EID 34

/**
 * \brief MD Jam Command Symbolic Address Resolution Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when symbolic address passed in Jam command
 *  couldn't be resolved by use of the on-board Symbol Table.
 */
#define MD_CANT_RESOLVE_JAM_ADDR_ERR_EID 35

/**
 * \brief MD Jam Command Address Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The resolved address (numerical value for symbol + offset) and
 *  field length specified by the Jam command were found to specify a dwell
 *  be outside valid ranges.
 */
#define MD_INVALID_JAM_ADDR_ERR_EID 36

/**
 * \brief MD Jam Command Address Not 32-bit Aligned Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The Jam command specified a 4-byte read, and the resolved address
 *  (numerical value for symbol + offset) is not on a 4-byte boundary.
 */
#define MD_JAM_ADDR_NOT_32BIT_ERR_EID 37

/**
 * \brief MD Jam Command Address Not 16-bit Aligned Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The Jam command specified a 2-byte read, and the resolved address
 *  (numerical value for symbol + offset) is not on a 2-byte boundary.
 */
#define MD_JAM_ADDR_NOT_16BIT_ERR_EID 38

/**
 * \brief MD Dwell Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued after the following sequence occurs:
 *   - #CFE_TBL_GetStatus returned #CFE_TBL_INFO_UPDATE_PENDING
 *   - #CFE_TBL_Update returned #CFE_SUCCESS, a call is made to
 *   - #CFE_TBL_GetAddress returned something other than #CFE_TBL_INFO_UPDATED.
 *
 *  When this happens, the newly loaded table contents are _not_ copied
 *  to MD task structures.
 */
#define MD_NO_TBL_COPY_ERR_EID 39

/**
 * \brief MD Dwell Table Enabled Rate Is Zero Event ID
 *
 *  \par Type: INFORMATIONAL
 *
 *  \par Cause:
 *
 *   - The calculated rate, the total of delays for all active entries equals zero
 *   - The table is enabled
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_ZERO_RATE_TBL_INF_EID 40

/**
 * \brief MD Dwell Table Table Symbol Address Unresolved Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The specified symbol wasn't found in the system symbol table.
 *  This could be either because there is no symbol table, or because
 *  the symbol isn't present in an existing symbol table.
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_RESOLVE_ERR_EID 41

/**
 * \brief MD Dwell Table Address Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The specified address was not in allowable memory ranges.
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_RANGE_ERR_EID 42

/**
 * \brief MD Dwell Table Entry Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The dwell table contains an invalid value for a dwell length.
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_TBL_HAS_LEN_ERR_EID 43

/**
 * \brief MD Dwell Table Enable Flag Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The dwell table's enable value was neither zero nor one.
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_TBL_ENA_FLAG_EID 44

/**
 * \brief MD Dwell Table Entry Alignment Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Either a 4-byte dwell was specified and address is not 4-byte aligned, or
 *  a 2-byte dwell was specified and address is not 2-byte aligned.
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_TBL_ALIGN_ERR_EID 45

/**
 * \brief MD Set Signature Command Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  A 'Set Signature' command was received and processed nominally to
 *  associate a signature with the specified dwell table.  All dwell packets
 *  derived from that dwell table will include the specified signature string.
 */
#define MD_SET_SIGNATURE_INF_EID 46

/**
 * \brief MD Set Signature Command Table ID Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when a Set Signature Command is received
 *  with an invalid value for the table id argument.
 *  Values in the range 1..#MD_NUM_DWELL_TABLES are expected.
 */
#define MD_INVALID_SIGNATURE_TABLE_ERR_EID 47

/**
 * \brief MD Set Signature Command Signature Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This error event is issued when a Set Signature Command is received
 *  with a string not containing null termination within the allowable length.
 */
#define MD_INVALID_SIGNATURE_LENGTH_ERR_EID 49

/**
 * \brief MD Dwell Table Signature Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  The dwell table contains an invalid Signature (not null terminated).
 *
 *  If this load was initiated via ground command, the Table Id will be known
 *  from the command.  If this load was recovered after a reset, a subsequent
 *  event message will identify the Table Id.
 */
#define MD_TBL_SIG_LEN_ERR_EID 50

/**
 * \brief MD Dwell Table Enabled With Zero Delay Event ID
 *
 *  \par Type: INFORMATIONAL
 *
 *  \par Cause:
 *
 *  - The calculated rate, the total of delays for all active entries, current equals zero
 *  - The table is currently enabled
 *
 *  If the command either changes the delay values in the table (such that the total delay is 0)
 *  while the table is enabled, or if the table is enabled while the total delay value is 0, this
 *  event will be sent.
 */
#define MD_ZERO_RATE_CMD_INF_EID 51

/**
 * \brief MD Dwell Table Verification Results Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  This event message is issued when a table validation has been
 *  completed for memory dwell table load
 */
#define MD_DWELL_TBL_INF_EID 52

/**
 * \brief MD Dwell Table Update Table Enabled Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired in
 *  the #MD_UpdateTableEnabledField function.  This event is issued when the
 *  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or
 *  CFE_TBL_INFO_UPDATED.
 */
#define MD_UPDATE_TBL_EN_ERR_EID 53

/**
 * \brief MD Dwell Table Update Dwell Entry Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired in
 *  the #MD_UpdateTableDwellEntry function.  This event is issued when the
 *  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or
 *  CFE_TBL_INFO_UPDATED.
 */
#define MD_UPDATE_TBL_DWELL_ERR_EID 54

/**
 * \brief MD Dwell Table Update Table Signature Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired in
 *  the MD_UpdateTableSignature function.  This event is issued when the
 *  #CFE_TBL_GetAddress function returns a value other than CFE_SUCCESS or
 *  CFE_TBL_INFO_UPDATED.
 */
#define MD_UPDATE_TBL_SIG_ERR_EID 55

/**
 * \brief MD Process Start Command Dwell Table Mask Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired for
 *  one of the tables being started with the #MD_ProcessStartCmd.  In addition
 *  to this summary message, a #MD_UPDATE_TBL_EN_ERR_EID event is issed for each
 *  failure.
 */
#define MD_START_DWELL_ERR_EID 56

/**
 * \brief MD Process Stop Command Dwell Table Mask Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired for
 *  one of the tables being started with the #MD_ProcessStopCmd.  In addition
 *  to this summary message, a #MD_UPDATE_TBL_EN_ERR_EID event is issed for each
 *  failure.
 */
#define MD_STOP_DWELL_ERR_EID 57

/**
 * \brief MD Process Signature Command Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table address cannot be acquired
 *  for the table specified in the #MD_ProcessSignatureCmd.
 */
#define MD_SET_SIGNATURE_ERR_EID 58

/**
 * \brief MD Jam To Dwell Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued for a failed jam operation.
 */
#define MD_JAM_DWELL_ERR_EID 59

/**
 * \brief MD Jam Null To Dwell Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event is issued for a failed jam operation in which a null dwell entry is
 *  specified.  A null entry is specified when the input dwell length is zero.
 *  All dwell fields (address, length, and delay) will be set to zero in this case.
 */
#define MD_JAM_NULL_DWELL_ERR_EID 60

/**
 * \brief MD Table Name Generation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued if the snprintf call used to create the
 *  TableName in #MD_InitTableServices function fails.
 */
#define MD_INIT_TBL_NAME_ERR_EID 61

/**
 * \brief MD Table Filename Generation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued if the snprintf call used to create the
 *  TblFileName in #MD_InitTableServices function fails.
 */

#define MD_INIT_TBL_FILENAME_ERR_EID 62

/**
 * \brief MD Dwell Table Validation Null Pointer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the table pointer passed to
 *  #MD_TableValidationFunc is null.
 */
#define MD_TBL_VAL_NULL_PTR_ERR_EID 63

/**
 * \brief MD Dwell Table Address Read Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when the PSP returns an error when attempting
 *  to read the memory address of an entry
 */
#define MD_DWELL_LOOP_GET_DWELL_DATA_ERR_EID 70

/**
 * \brief MD Create Pipe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when MD cannot create the software bus pipe.
 */
#define MD_CREATE_PIPE_ERR_EID 71

/**
 * \brief MD Housekeeping Subscribe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when MD cannot subscribe to housekeeping
 *  requests.
 */
#define MD_SUB_HK_ERR_EID 72

/**
 * \brief MD Command Subscribe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when MD cannot subscribe to commands.
 */
#define MD_SUB_CMD_ERR_EID 73

/**
 * \brief MD Wakeup Message Subscribe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when MD cannot subscribe to wakeup
 *  messages.
 */
#define MD_SUB_WAKEUP_ERR_EID 74

/**
 * \brief MD Dwell Table Validation Entry Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  This event message is issued when a dwell table entry is determined
 *  to be invalid.
 *  The Table entry field specifies the index of the invalid entry.
 *  The status field specifies the error generated when validating that
 *  entry.
 */
#define MD_TBL_ENTRY_ERR_EID 75

/**\}*/

#endif
