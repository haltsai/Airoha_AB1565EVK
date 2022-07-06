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
 
#ifndef _SINK_H
#define _SINK_H
/*!
 *@file   Sink.h
 *@brief  defines the api of sink interface
 *
 @verbatim
 @endverbatim
 */

#include "stream_config.h"
#include "sink_.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SINK_INVALID_SLACK (U32)0
#define SINK_INVALID_CLAIM (U32)0xffffffff


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



/**
 * @brief Report how many bytes can successfully be flush into the sink.
 *
 * @param sink The Sink to check.
 * @return Zero if the sink is not valid or sink do not have resource.
 */
U32 SinkSlack(SINK sink);

/**
 * @brief malloc the number of extra bytes in a sink.
 *
 * @param sink The sink to claim.
 * @param extra The number of bytes to malloc.
 * @return The offset of the claimed region if the claim was successful,
 *   0xFFFFFFFF otherwise.
 */
U32 SinkClaim(SINK sink, U32 extra);

/**
 * @brief write buffer from source address into sink
 *
 * @param sink The Sink to configure.
 * @param src_addr The source address to load the data.
 * @param length  The length to load from source and write into sink .
 *
 *  @return FALSE if the request could not be performed, TRUE otherwise.
 */
BOOL SinkWriteBuf(SINK sink, U8 *src_addr, U32 length);


/**
 * @brief Map the sink into the address map, returning a pointer,
 *  to the first byte in the sink, only the size of sinkclaim
 *  is avaliable
 *
 * @param sink The sink to map into the address map.
 * @return zero if the sink is invalid.
 */
U8* SinkMap(SINK sink);

/**
 * @brief Flush the indicated number of bytes out of the sink.
 *
 * @param sink The Sink to flush.
 * @param amount The number of bytes to flush.
 *
 * @return TRUE on success, or FALSE otherwise
 */
BOOL SinkFlush(SINK sink, U32 amount);

/**
 *  @brief Configure a particular sink.
 *  @param sink The Sink to configure.
 *  @param key The key to configure.
 *  @param value The value to write to 'key'
 *
 *  @return FALSE if the request could not be performed, TRUE otherwise.
 */
BOOL SinkConfigure(SINK sink, stream_config_type type, U32 value);

/**
 *  @brief Request to close the sink
 *  @param sink The sink to close
 *
 *  @return TRUE if the source could be closed, and FALSE otherwise.
 */
BOOL SinkClose(SINK sink);

/**
 *  @brief Request to alias two Sinks
 *  @param sink1 The first Sink to be aliased
 *  @param sink2 The second Sink to be aliased
 *
 *  @return TRUE if the sinks are aliased successfully, else FALSE.
 */
BOOL SinkAlias(SINK sink1, SINK sink2);

/**
 * @brief Find the SCO handle corresponding to a sink.
 * @param sink The Sink to get the handle for
 * @returns The handle, or 0 is the sink wasn't a SCO sink
 */
U32 SinkGetScoHandle(SINK sink);


/**
 * @brief Get the Bluetooth address from a sink.
 *
 * @param sink The Sink to fetch the Bluetooth address from.
 * @param taddr If the address is found it will be returned to the
 * location pointed at by this value.
 *
 * @return TRUE if such an address was found, FALSE otherwise.
 */
BOOL SinkGetBdAddr(SINK sink, VOID* taddr);


/**
 * @brief Return TRUE if a sink is valid, FALSE otherwise.
 *
 * @param sink The sink to check.
 */
BOOL SinkIsValid(SINK sink);



#endif
