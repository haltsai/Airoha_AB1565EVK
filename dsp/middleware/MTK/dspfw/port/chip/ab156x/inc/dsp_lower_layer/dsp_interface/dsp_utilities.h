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
 
#ifndef _DSP_UTILITIES_H_
#define _DSP_UTILITIES_H_

#include "types.h"
#include "FreeRTOS.h"



#ifndef DSP_ALIGN2
#define     DSP_ALIGN2      ALIGN(2)
#endif
#ifndef DSP_ALIGN4
#define     DSP_ALIGN4      ALIGN(4)
#endif
#ifndef DSP_ALIGN8
#define     DSP_ALIGN8      ALIGN(8)
#endif
/**
 * DSP_WrapSubtraction
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U16 DSP_WrapSubtraction(U16 Minuend, U16 Subtrahend, U16 WrapSize)
{
    configASSERT((WrapSize >= Minuend) && (WrapSize >= Subtrahend));

    return (Minuend >= Subtrahend) ? (Minuend - Subtrahend) : (Minuend + WrapSize - Subtrahend);
}


/**
 * DSP_WrapSubtraction
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
STATIC INLINE U16 DSP_WrapAddition(U16 Augend, U16 Addend, U16 WrapSize)
{
    configASSERT((WrapSize >= Augend) && (WrapSize >= Addend));

    return (Augend + Addend >= WrapSize) ? (Augend + Addend - WrapSize) : (Augend + Addend);
}


/**
 * DSP_CheckBit
 *
 * Check Bit Field is Set in Variable
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 */
STATIC INLINE BOOL DSP_CheckBit(U32 Var, U32 Pos)
{
    return ((Var & (1 << Pos)) != 0);
}


/**
 * DSP_ByteSwap16
 *
 * Endian swap of 16-bit value
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 */
STATIC INLINE U16 DSP_ByteSwap16 (U16 value)
{
    U16 byte0 = value>>8;
    U16 byte1 = value<<8;

    return (byte0|byte1);
}


/**
 * DSP_ByteSwap32
 *
 * Endian swap of 32-bit value
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 */
STATIC INLINE U32 DSP_ByteSwap32 (U32 value)
{
    value = ((value<<8) & 0xFF00FF00) | ((value>>8) & 0x00FF00FF);

    U32 word0   = value<<16;
    U32 word1   = (value>>16) & 0x0000FFFF;

    return (word0|word1);
}


#endif /* _DSP_UTILITIES_H_ */

