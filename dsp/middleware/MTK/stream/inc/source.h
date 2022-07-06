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
 
#ifndef _SOURCE_H
#define _SOURCE_H


/*!
 *@file   Source.h
 *@brief  defines the api of source interface
 *
 @verbatim
 @endverbatim
 */

//-
#include "stream_config.h"
#include "source_.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Reports the number of bytes available in the source.
 *
 * @param source The source to fetch the size of.
 * @return Zero if the source is invalid.
 */
U32 SourceSize(SOURCE source);


/**
 * @brief copy data in source buffer to map, only number of SourceSize report is valid
 *  data in map become invalid if another sourcemap is called
 *
 * @param source The source to map into the address map.
 *
 * @return address of map, zero if the source is invalid.
 */
U8* SourceMap(SOURCE source);

/**
 * @brief drop data in front of source buffer
 *
 * @param source The Source to drop the data from.
 * @param amount The number of bytes to drop.
 */
VOID SourceDrop(SOURCE source, U32 amount);


/**
 * @brief Configure a specific source.
 *
 * @param source The source to configure.
 * @param key The key to configure.
 * @param value The value to write to 'key'
 *
 * @return FALSE if the request could not be performed, TRUE otherwise.
 */
BOOL SourceConfigure(SOURCE source, stream_config_type type, U32 value);

/**
 * @brief Request to close the source
 * @param source The source to close
 *
 * @return TRUE if the source could be closed, and FALSE
 * otherwise.
 */
BOOL SourceClose(SOURCE source);


/**
 * @brief Return TRUE if a source is valid, FALSE otherwise.
 *
 * @param source The source to check.
 * @return TRUE stands for valid , FALSE otherwise
 */
BOOL SourceIsValid(SOURCE source);

/**
 * @brief clean buffer in source
 *
 * @param source  the source to clean
 */
VOID SourceEmpty(SOURCE source);

/**
 * @brief read buffer in source into destination
 *
 * @param source The source to read.
 * @param dst_addr The destination address to store the data.
 * @param length  The length to read from source and store into destination.
 */
BOOL SourceReadBuf(SOURCE source, U8 *dst_addr, U32 length);



#endif
