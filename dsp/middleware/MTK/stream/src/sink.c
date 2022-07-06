/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

/*!
 *@file   Sink.c
 *@brief  defines the api of sink interface
 *
 @verbatim
 @endverbatim
 */

//-
#include "sink_inter.h"
#include "sink.h"
#include "stream.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
SINK Sink_blks[SINK_TYPE_MAX];


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Report how many bytes can successfully be flush into the sink.
 *
 * @param sink The Sink to check.
 * @return Zero if the sink is not valid or sink do not have resource.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SinkSlack(SINK sink)
{
    U32 slack = SINK_INVALID_SLACK;

    if (sink)
    {
        slack = sink->sif.SinkSlack(sink);
    }

    return slack;
}

/**
 * @brief malloc the number of extra bytes in a sink.
 *
 * @param sink The sink to claim.
 * @param extra The number of bytes to malloc.
 * @return The offset of the claimed region if the claim was successful,
 *   0xFFFFFFFF otherwise.
 */
U32 SinkClaim(SINK sink, U32 extra)
{
    U32 offset = SINK_INVALID_CLAIM;

    if (sink)
    {
        offset = sink->sif.SinkClaim(sink, extra);
    }

    return offset;
}

/**
 * @brief write buffer from source address into sink
 *
 * @param sink The Sink to configure.
 * @param src_addr The source address to load the data.
 * @param length  The length to load from source and write into sink .
 *
 *  @return FALSE if the request could not be performed, TRUE otherwise.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SinkWriteBuf(SINK sink, U8 *src_addr, U32 length)
{
    BOOL result = FALSE;
    if (sink)
    {
        result = sink->sif.SinkWriteBuf(sink, src_addr, length);
    }
    return result;
}


/**
 * @brief Map the sink into the address map, returning a pointer,
 *  to the first byte in the sink, only the size of sinkclaim
 *  is avaliable
 *
 * @param sink The sink to map into the address map.
 * @return zero if the sink is invalid.
 */
U8* SinkMap(SINK sink)
{
    U8* Map = NULL;

    if (sink)
    {
        Map = sink->sif.SinkMap(sink);
    }

    return Map;
}

/**
 * @brief Flush the indicated number of bytes out of the sink.
 *
 * @param sink The Sink to flush.
 * @param amount The number of bytes to flush.
 *
 * @return TRUE on success, or FALSE otherwise
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SinkFlush(SINK sink, U32 amount)
{
    BOOL result = FALSE;

    if (sink != NULL)
    {
        result = sink->sif.SinkFlush(sink, amount);
    }

    return result;
}

/**
 *  @brief Configure a particular sink.
 *  @param sink The Sink to configure.
 *  @param key The key to configure.
 *  @param value The value to write to 'key'
 *
 *  @return FALSE if the request could not be performed, TRUE otherwise.
 */
BOOL SinkConfigure(SINK sink, stream_config_type type, U32 value)
{
    BOOL result = FALSE;

    if (sink)
    {
        result = sink->sif.SinkConfigure(sink, type, value);
    }

    return result;
}

/**
 *  @brief Request to close the sink
 *  @param sink The sink to close
 *
 *  @return TRUE if the source could be closed, and FALSE otherwise.
 */
BOOL SinkClose(SINK sink)
{
    BOOL result = TRUE;

    if (sink)
    {
        if (sink->transform != NULL)
        {
            StreamDisconnect(sink->transform);
        }
        if(sink->sif.SinkClose != NULL)
        {
            result = sink->sif.SinkClose(sink);
        }
        if (result)
        {
            Sink_blks[sink->type] = NULL;
            vPortFree(sink);
        }
    }
    return result;

}

/**
 *  @brief Request to alias two Sinks
 *  @param sink1 The first Sink to be aliased
 *  @param sink2 The second Sink to be aliased
 *
 *  @return TRUE if the sinks are aliased successfully, else FALSE.
 */
BOOL SinkAlias(SINK sink, SINK sink2)
{
    if (sink == NULL && sink2 == NULL)
    {
        return FALSE;
    }

    return FALSE;
}


/**
 * @brief Find the SCO handle corresponding to a sink.
 * @param sink The Sink to get the handle for
 * @returns The handle, or 0 is the sink wasn't a SCO sink
 */
U32 SinkGetScoHandle(SINK sink)
{
    if ((sink != NULL))
    {
        return 0;
    } else {
        return 1;
    }
}


/**
 * @brief Get the Bluetooth address from a sink.
 *
 * @param sink The Sink to fetch the Bluetooth address from.
 * @param taddr If the address is found it will be returned to the
 * location pointed at by this value.
 *
 * @return TRUE if such an address was found, FALSE otherwise.
 */
BOOL SinkGetBdAddr(SINK sink, VOID* taddr)
{
    UNUSED(sink);
    UNUSED(taddr);
    return 0;
}




/**
 * @brief Return TRUE if a sink is valid, FALSE otherwise.
 *
 * @param sink The sink to check.
 */
BOOL SinkIsValid(SINK sink)
{
    BOOL result = FALSE;
    if (sink)
    {
        if(sink == Sink_blks[sink->type])
        {
            result = TRUE;
        }
    }
    return result;
}


