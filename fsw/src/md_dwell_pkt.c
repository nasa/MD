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
 *   Functions used to populate and send Memory Dwell packets.
 */

/*************************************************************************
** Includes
*************************************************************************/
#include "md_dwell_pkt.h"
#include "md_utils.h"
#include "md_app.h"
#include "md_events.h"
#include <string.h>
#include "md_extern_typedefs.h"

extern MD_AppData_t MD_AppData;

/******************************************************************************/

void MD_DwellLoop(void)
{
    int32                    Result;
    uint16                   TblIndex;
    uint16                   EntryIndex;
    uint16                   NumDwellAddresses;
    MD_DwellPacketControl_t *TblPtr = NULL;

    /* Check each dwell table */
    for (TblIndex = 0; TblIndex < MD_NUM_DWELL_TABLES; TblIndex++)
    {
        TblPtr            = &MD_AppData.MD_DwellTables[TblIndex];
        NumDwellAddresses = TblPtr->AddrCount;

        /* Process enabled dwell tables */
        if ((TblPtr->Enabled == MD_DWELL_STREAM_ENABLED) && (TblPtr->Rate > 0))
        {
            /*
            ** Handle special case that dwell pkt is already full because
            ** pkt size was shortened after data had been written to the pkt.
            */

            if (TblPtr->CurrentEntry >= NumDwellAddresses)
            {
                MD_SendDwellPkt(TblIndex);

                /* Initialize CurrentEntry index */
                TblPtr->CurrentEntry = 0;
                TblPtr->PktOffset    = 0;
                TblPtr->Countdown    = TblPtr->Entry[NumDwellAddresses - 1].Delay;
            }

            else
            /*
            ** Handle nominal processing
            */
            {
                /* Decrement counter */
                TblPtr->Countdown--;

                /* Check if it's time to collect data */
                while (TblPtr->Countdown == 0)
                {
                    EntryIndex = TblPtr->CurrentEntry;

                    /* Read data for next address and write it to dwell pkt */
                    Result = MD_GetDwellData(TblIndex, EntryIndex);

                    if (Result != CFE_SUCCESS)
                    {
                        /* Send error event message */
                        CFE_EVS_SendEvent(MD_DWELL_LOOP_GET_DWELL_DATA_ERR_EID, CFE_EVS_EventType_ERROR,
                                          "Dwell Table failed to read entry %d in table %d ", EntryIndex, TblIndex);
                        /* Don't exit here yet, still need to increment counters or send the packet */
                    }

                    /* Check if the dwell pkt is now full */
                    if (EntryIndex == NumDwellAddresses - 1)

                    /* Case:  Just filled last active entry of dwell table */
                    {
                        /*
                        ** Send dwell packet
                        */

                        MD_SendDwellPkt(TblIndex);

                        /*
                        ** Assign control values to cause dwell processing to
                        ** continue at beginning of dwell control structure.
                        */

                        /* Reset countdown timer based on current Delay field */
                        TblPtr->Countdown = TblPtr->Entry[EntryIndex].Delay;

                        /* Initialize CurrentEntry index */
                        TblPtr->CurrentEntry = 0;
                        TblPtr->PktOffset    = 0;
                    }

                    else
                    /* Case: There are more addresses to read for current pkt.*/
                    {
                        /*
                        ** Assign control values to cause dwell processing to
                        ** continue at next entry in dwell control structure.
                        */

                        /* Reset countdown timer based on current Delay field */
                        TblPtr->Countdown = TblPtr->Entry[EntryIndex].Delay;

                        /* Increment CurrentEntry index */
                        TblPtr->CurrentEntry++;
                    }

                } /* end while Countdown == 0 */

            } /* end else handle nominal processing */

        } /* end if current dwell stream enabled */

    } /* end for each dwell table */
}

/******************************************************************************/

int32 MD_GetDwellData(uint16 TblIndex, uint16 EntryIndex)
{
    uint8                    NumBytes   = 0; /* Num of bytes to read */
    uint32                   MemReadVal = 0; /* 1-, 2-, or 4-byte value */
    MD_DwellPacketControl_t *TblPtr     = &MD_AppData.MD_DwellTables[TblIndex];
    int32                    Status     = CFE_SUCCESS;
    cpuaddr                  DwellAddress; /* dwell address */

    /* How many bytes to read?*/
    NumBytes = TblPtr->Entry[EntryIndex].Length;

    /* fetch data pointed to by this address */
    DwellAddress = TblPtr->Entry[EntryIndex].ResolvedAddress;

    switch (NumBytes)
    {
        case 1:
            if (CFE_PSP_MemRead8(DwellAddress, (uint8 *)&MemReadVal) != CFE_PSP_SUCCESS)
            {
                Status = ONE_BYTE_MEM_ADDR_READ_ERR;
            }
            break;
        case 2:
            if (CFE_PSP_MemRead16(DwellAddress, (uint16 *)&MemReadVal) != CFE_PSP_SUCCESS)
            {
                Status = TWO_BYTE_MEM_ADDR_READ_ERR;
            }
            break;
        case 4:
            if (CFE_PSP_MemRead32(DwellAddress, &MemReadVal) != CFE_PSP_SUCCESS)
            {
                Status = FOUR_BYTE_MEM_ADDR_READ_ERR;
            }
            break;
        default:
            /* Invalid dwell length */
            /* Shouldn't ever get here unless length value was corrupted. */
            Status = INVALID_DWELL_ADDR_LEN;
            break;
    }

    /* If value was read successfully, copy value to dwell packet. */
    /* Wouldn't want to copy, if say, there was an invalid length & we */
    /* didn't read. */
    if (Status == CFE_SUCCESS)
    {
        memcpy(&MD_AppData.MD_DwellPkt[TblIndex].Payload.Data[TblPtr->PktOffset], &MemReadVal, NumBytes);
    }

    /* Update write location in dwell packet */
    TblPtr->PktOffset += NumBytes;

    return Status;
}

/******************************************************************************/

void MD_SendDwellPkt(uint16 TableIndex)
{
    uint16 DwellPktSize = 0; /* Dwell Packet Size, in bytes */

    /* Assign pointers to structures */
    MD_DwellPacketControl_t *TblPtr = &MD_AppData.MD_DwellTables[TableIndex];
    MD_DwellPkt_t *          PktPtr = &MD_AppData.MD_DwellPkt[TableIndex];

    /*
    ** Assign packet fields.
    */
    PktPtr->Payload.TableId   = TableIndex + 1;
    PktPtr->Payload.AddrCount = TblPtr->AddrCount;
    PktPtr->Payload.Rate      = TblPtr->Rate;
#if MD_SIGNATURE_OPTION == 1
    strncpy(PktPtr->Payload.Signature, TblPtr->Signature, MD_SIGNATURE_FIELD_LENGTH - 1);
    /* Make sure string is null-terminated. */
    PktPtr->Payload.Signature[MD_SIGNATURE_FIELD_LENGTH - 1] = '\0';
#endif
    PktPtr->Payload.ByteCount = TblPtr->DataSize;

    /*
    ** Set packet length in header.
    */

    DwellPktSize = MD_DWELL_PKT_LNGTH - MD_DWELL_TABLE_SIZE * 4 + TblPtr->DataSize;

    CFE_MSG_SetSize(CFE_MSG_PTR(PktPtr->TelemetryHeader), DwellPktSize);

    /*
    ** Send dwell telemetry packet.
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(PktPtr->TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(PktPtr->TelemetryHeader), true);
}

/******************************************************************************/

void MD_StartDwellStream(uint16 TableIndex)
{
    MD_AppData.MD_DwellTables[TableIndex].Countdown    = 1;
    MD_AppData.MD_DwellTables[TableIndex].CurrentEntry = 0;
    MD_AppData.MD_DwellTables[TableIndex].PktOffset    = 0;
}
