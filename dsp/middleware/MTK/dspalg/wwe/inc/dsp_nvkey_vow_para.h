/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#include "types.h"

#ifndef __DSP_NVKEY_VOW_PARA_H__
#define __DSP_NVKEY_VOW_PARA_H__

#define ENDIAN_RVRS(A)                  (A)

#define VOW_SNR_THRESHOLD_0303 (0x00)
#define VOW_SNR_THRESHOLD_1313 (0x01)
#define VOW_SNR_THRESHOLD_2323 (0x02)
#define VOW_SNR_THRESHOLD_3333 (0x03)
#define VOW_SNR_THRESHOLD_4343 (0x04)
#define VOW_SNR_THRESHOLD_5353 (0x05)
#define VOW_SNR_THRESHOLD_6363 (0x06)
#define VOW_SNR_THRESHOLD_7373 (0x07)

#define VOW_NOISE_IGNORE_BITS_FFFF0000 (0x01)
#define VOW_NOISE_IGNORE_BITS_FFF00000 (0x02)
#define VOW_NOISE_IGNORE_BITS_FF000000 (0x03)
#define VOW_NOISE_IGNORE_BITS_F0000000 (0x04)

typedef struct
{
    /*NVkey_0 0x00:0x0303 0x01:0x1313
              0x02:0x2323 0x03:0x3333
              0x04:0x4343 0x05:0x5353
              0x06:0x6363 0x07:0x7373*/
    uint8_t snr_threshold;
    uint8_t noise_ignore_bits; /*NVkey_1 0x01:0xFFFF0000 0x02:0xFFF00000 0x03:0xFF000000 0x04:0xF0000000 */
    uint8_t alpha_rise;        /*NVkey_3 0x01 - 0x0F */
    uint8_t enable;            /*NVkey_4 0x00:diable 0x01:enable*/
    uint16_t main_mic;         /*NVkey_5 vow use main mic type*/
    uint16_t ref_mic;          /*NVkey_6 vow use ref mic type*/
}DSP_NVKEY_VOW_PARA;

#endif

