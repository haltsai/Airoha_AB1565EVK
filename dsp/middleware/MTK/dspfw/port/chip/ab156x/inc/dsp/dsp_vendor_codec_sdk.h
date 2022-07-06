/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
#ifndef _DSP_VENDOR_CODEC_SDK_H_
#define _DSP_VENDOR_CODEC_SDK_H_





#ifdef MTK_BT_A2DP_VENDOR_ENABLE
#ifndef MTK_BT_A2DP_VENDOR_1_ENABLE
#include "vendor_decoder_proting.h"
#else
#define  STREAM_CODEC_DECODER_VENDOR_INITIALIZE NULL
#define  STREAM_CODEC_DECODER_VENDOR_PROCESS    NULL
#define  VENDOR_HANDLE_SIZE                     0
#define  VENDOR_CODEC_OUT_SIZE                  0xFF
#define  vendor_library_load                    NULL
#define  vendor_library_unload                  NULL
#endif
#else
#define  STREAM_CODEC_DECODER_VENDOR_INITIALIZE NULL
#define  STREAM_CODEC_DECODER_VENDOR_PROCESS    NULL
#define  VENDOR_HANDLE_SIZE                     0
#define  VENDOR_CODEC_OUT_SIZE                  0xFF
#define  vendor_library_load                    NULL
#define  vendor_library_unload                  NULL
#endif

#ifdef MTK_BT_A2DP_VENDOR_1_ENABLE
#include "vendor_1_decoder_proting.h"
#else
#define  STREAM_CODEC_DECODER_VENDOR_1_INITIALIZE NULL
#define  STREAM_CODEC_DECODER_VENDOR_1_PROCESS    NULL
#define  VENDOR_1_HANDLE_SIZE                     0
#define  VENDOR_1_CODEC_OUT_SIZE                  0xFF
#define  vendor_1_library_load                    NULL
#define  vendor_1_library_unload                  NULL
#endif

#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
#include "vendor_2_decoder_proting.h"
#else
#define  STREAM_CODEC_DECODER_VENDOR_2_INITIALIZE NULL
#define  STREAM_CODEC_DECODER_VENDOR_2_PROCESS    NULL
#define  VENDOR_2_HANDLE_SIZE                     0
#define  VENDOR_2_CODEC_OUT_SIZE                  0xFF
#define  vendor_2_library_load                    NULL
#define  vendor_2_library_unload                  NULL
#endif

#ifdef MTK_BT_A2DP_VENDOR_3_ENABLE
#include "vendor_3_decoder_proting.h"
#else
#define  STREAM_CODEC_DECODER_VENDOR_3_INITIALIZE NULL
#define  STREAM_CODEC_DECODER_VENDOR_3_PROCESS    NULL
#define  VENDOR_3_HANDLE_SIZE                     0
#define  VENDOR_3_CODEC_OUT_SIZE                  0xFF
#define  vendor_3_library_load                    NULL
#define  vendor_3_library_unload                  NULL
#endif

#endif
