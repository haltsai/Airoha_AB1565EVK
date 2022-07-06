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
 
#include "bt_sink_srv_call_audio.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_common.h"

#if defined(MTK_AVM_DIRECT)
#include "bt_avm.h"
#endif

#if defined(__AFE_HS_DC_CALIBRATION__)
    #define BT_SINK_SRV_CALL_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HANDSET
#else
    #define BT_SINK_SRV_CALL_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#endif
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    #define BT_SINK_SRV_CALL_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
    #define BT_SINK_SRV_CALL_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif

static bool g_sink_srv_call_audio_init;

static const uint16_t bt_sink_srv_call_tone_16ksr[] = {
#ifdef BT_SINK_ENABLE_CALL_LOCAL_RINGTONE
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6
#endif/*BT_SINK_ENABLE_CALL_LOCAL_RINGTONE*/
};

void bt_sink_srv_call_audio_init(void)
{
    if (!g_sink_srv_call_audio_init) {
        g_sink_srv_call_audio_init = true;
    #ifdef MTK_AUDIO_SYNC_ENABLE
        bt_sink_srv_register_sync_callback(BT_SINK_SRV_CALL_VOLUME_TYPE, bt_sink_srv_call_audio_sync_callback);
    #endif
    }
}

void bt_sink_srv_call_audio_deinit(void)
{
    if (g_sink_srv_call_audio_init) {
        g_sink_srv_call_audio_init = false;
    #ifdef MTK_AUDIO_SYNC_ENABLE
        bt_sink_srv_deregister_sync_callback(BT_SINK_SRV_CALL_VOLUME_TYPE);
    #endif
    }
}


void bt_sink_srv_call_audio_pcm_parameter_init(bt_sink_srv_call_audio_capability_t *audio_capability, bt_sink_srv_call_audio_volume_t out_volume)
{
    if (NULL != audio_capability) {
        bt_sink_srv_report_id("[CALL][AUDIO]Init PCM params, volume:%d", 1, out_volume);
        audio_capability->type = PCM;
        audio_capability->audio_stream_out.audio_device = BT_SINK_SRV_CALL_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t )out_volume;
        audio_capability->codec.pcm_format.stream.stream_sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        audio_capability->codec.pcm_format.stream.stream_channel = HAL_AUDIO_STEREO;
        audio_capability->codec.pcm_format.stream.buffer = (void *)bt_sink_srv_call_tone_16ksr;
        audio_capability->codec.pcm_format.stream.size = sizeof(bt_sink_srv_call_tone_16ksr);
        audio_capability->codec.pcm_format.in_out = STREAM_OUT;
        audio_capability->audio_path_type = HAL_AUDIO_PLAYBACK_MUSIC;
    }
}
 
void bt_sink_srv_call_audio_sco_parameter_init(
        bt_sink_srv_call_audio_capability_t *audio_capability,
        bt_hfp_audio_codec_type_t codec,
        bt_sink_srv_call_audio_volume_t out_volume)
{
    if (NULL != audio_capability) {
        audio_capability->type = HFP;
        audio_capability->codec.hfp_format.hfp_codec.type = codec;
        audio_capability->audio_stream_in.audio_device = BT_SINK_SRV_CALL_INPUT_DEVICE;
        audio_capability->audio_stream_in.audio_volume = AUD_VOL_IN_LEVEL0;
        audio_capability->audio_stream_out.audio_device = BT_SINK_SRV_CALL_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t )out_volume;
        bt_sink_srv_report_id("[CALL][AUDIO]Init HFP params, volume:%d, codec:0x%x, sco input device: 0x%x", 3, 
                            audio_capability->audio_stream_out.audio_volume,
                            audio_capability->codec.hfp_format.hfp_codec.type,
                            audio_capability->audio_stream_in.audio_device);
        bt_sink_srv_set_clock_offset_ptr_to_dsp((const bt_bd_addr_t *)&audio_capability->dev_addr);
    }
}
 
uint8_t bt_sink_srv_call_audio_volume_local_to_bt(bt_sink_srv_call_audio_volume_t local_volume)
{
    bt_sink_srv_assert(AUD_VOL_OUT_MAX >= local_volume);
    return (uint8_t)local_volume;
}
 
bt_sink_srv_call_audio_volume_t bt_sink_srv_call_audio_volume_bt_to_local(uint8_t bt_volume)
{
    if (bt_volume >= AUD_VOL_OUT_MAX)
    {
        bt_volume = (uint8_t)AUD_VOL_OUT_LEVEL15;
    }

    return (bt_sink_srv_call_audio_volume_t)bt_volume;
}

void bt_sink_srv_call_audio_set_out_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume)
{
    if (BT_SINK_SRV_CALL_AUDIO_INVALID_ID != audio_id) {
        #ifdef MTK_AUDIO_SYNC_ENABLE
        if (bt_aws_mce_srv_get_link_type() == BT_AWS_MCE_SRV_LINK_NORMAL) {
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                bt_sink_srv_call_audio_sync_data_t sync_data = {0};
                bt_sink_srv_get_sync_data_parameter_t request_param = {0};

                /* Fill sync data */
                sync_data.out_volume = volume;

                /* Fill request parameter */
                request_param.type = BT_SINK_SRV_CALL_VOLUME_TYPE;
                request_param.data = &sync_data;
                request_param.length = sizeof(bt_sink_srv_call_audio_sync_data_t);
                request_param.duration = BT_SINK_SRV_CALL_AUDIO_SYNC_DURATION;
                request_param.timeout_duration = BT_SINK_SRV_CALL_AUDIO_SYNC_TIMEOUT;

                bt_sink_srv_request_sync_gpt(&request_param);
            }
        } else {
            bt_sink_srv_ami_audio_set_volume(audio_id, volume, STREAM_OUT);
        }
    #else
        bt_sink_srv_ami_audio_set_volume(audio_id, volume, STREAM_OUT);
    #endif
    }
}

void bt_sink_srv_call_audio_set_out_start_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume)
{
#ifdef MTK_AUDIO_SYNC_ENABLE
    if (audio_id != BT_SINK_SRV_CALL_AUDIO_INVALID_ID) {
        bt_sink_srv_ami_audio_set_volume(audio_id, volume, STREAM_OUT);
    }
#endif
}

void bt_sink_srv_call_audio_set_in_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume)
{
    if (BT_SINK_SRV_CALL_AUDIO_INVALID_ID != audio_id) {
        bt_sink_srv_ami_audio_set_volume(audio_id, volume, STREAM_IN);
    }
}

#ifdef MTK_AUDIO_SYNC_ENABLE
void bt_sink_srv_call_audio_sync_out_volume(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_volume_t volume, uint32_t gpt_count)
{
    bt_sink_srv_am_audio_sync_capability_t sync_capability = {0};

    if (audio_id != BT_SINK_SRV_CALL_AUDIO_INVALID_ID) {
        sync_capability.sync_scenario_type = MCU2DSP_SYNC_REQUEST_HFP;
        sync_capability.sync_action_type = MCU2DSP_SYNC_REQUEST_SET_VOLUME;
        sync_capability.target_gpt_cnt = gpt_count;
        sync_capability.vol_out.vol_level = volume;
        sync_capability.vol_out.channel = MCU2DSP_SYNC_VOLUME_CHANNEL_DUAL;

        bt_sink_srv_ami_audio_request_sync(audio_id, &sync_capability);
    }
}
#endif

bt_sink_srv_call_audio_id_t bt_sink_srv_call_audio_codec_open(bt_sink_srv_call_audio_notify_cb callback)
{
    bt_sink_srv_call_audio_id_t audio_id = bt_sink_srv_ami_audio_open(AUD_HIGH, callback);
    bt_sink_srv_report_id("[CALL][AUDIO]Open audio id:0x%x", 1, audio_id);
    bt_sink_srv_assert(audio_id != BT_SINK_SRV_CALL_AUDIO_INVALID_ID);
    return audio_id;
}

bool bt_sink_srv_call_audio_codec_close(bt_sink_srv_call_audio_id_t audio_id)
{
    bool result = false;
    bt_sink_srv_am_result_t aud_ret = bt_sink_srv_ami_audio_close(audio_id);
    bt_sink_srv_report_id("[CALL][AUDIO]Close result:0x%x", 1, aud_ret);
    if (aud_ret == AUD_EXECUTION_SUCCESS) {
        result = true;
    }
    return result;
}

bool bt_sink_srv_call_audio_play(bt_sink_srv_call_audio_id_t audio_id, bt_sink_srv_call_audio_capability_t *audio_capability)
{
    bool result = false;
    bt_sink_srv_am_result_t aud_ret = bt_sink_srv_ami_audio_play(audio_id, audio_capability);
    bt_sink_srv_report_id("[CALL][AUDIO]Play, AM result:0x%x, type:%d, volume:%d", 3, 
        aud_ret, audio_capability->type, audio_capability->audio_stream_out.audio_volume);
    if (aud_ret == AUD_EXECUTION_SUCCESS) {
        result = true;
    }
    return result;
}

bool bt_sink_srv_call_audio_stop(bt_sink_srv_call_audio_id_t audio_id)
{
    bool result = false;
    bt_sink_srv_am_result_t aud_ret = bt_sink_srv_ami_audio_stop(audio_id);
    bt_sink_srv_report_id("[CALL][AUDIO]Stop result:0x%x", 1, aud_ret);
    if (aud_ret == AUD_EXECUTION_SUCCESS) {
        result = true;
    }
    return result;
}

bool bt_sink_srv_call_audio_continue_play(bt_sink_srv_call_audio_id_t audio_id, 
        void *buffer,
        uint32_t data_count)
{
    bool result = false;
    bt_sink_srv_am_result_t aud_ret = bt_sink_srv_ami_audio_continue_stream(audio_id, buffer, data_count);
    if (aud_ret == AUD_EXECUTION_SUCCESS) {
        result = true;
    }
    return result;
}

void* bt_sink_srv_call_audio_get_ring(uint32_t *length)
{
    bt_sink_srv_assert(length != NULL);
    *length = (uint32_t)sizeof(bt_sink_srv_call_tone_16ksr);
    return (void*)bt_sink_srv_call_tone_16ksr;
}

void bt_sink_srv_call_audio_side_tone_enable(void)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role == BT_AWS_MCE_ROLE_NONE ||
        role == BT_AWS_MCE_ROLE_AGENT ||
        role == BT_AWS_MCE_ROLE_PARTNER) {
        am_audio_side_tone_enable();
    }
}

void bt_sink_srv_call_audio_side_tone_disable(void)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role == BT_AWS_MCE_ROLE_NONE ||
        role == BT_AWS_MCE_ROLE_AGENT ||
        role == BT_AWS_MCE_ROLE_PARTNER) {
        am_audio_side_tone_disable();
   }
}

void bt_sink_srv_call_audio_init_play(uint32_t gap_handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_clock_t play_clk = {0};
    bt_sink_srv_report_id("[CALL][AUDIO]Initial sync, handle:0x%x", 1, gap_handle);
    if (gap_handle) {
        bt_avm_set_audio_tracking_time(gap_handle, BT_AVM_TYPE_CALL, &play_clk);
    }
#endif
}

#ifdef MTK_AUDIO_SYNC_ENABLE
void bt_sink_srv_call_audio_sync_callback(bt_sink_srv_sync_status_t status, bt_sink_srv_sync_callback_data_t *data)
{
    bt_sink_srv_report_id("[CALL][AUDIO]sync_callback, status:0x%x data:0x%x", 2, status, data);

    if (status == BT_SINK_SRV_SYNC_SUCCESS) {
        if ((data != NULL) && (data->type == BT_SINK_SRV_CALL_VOLUME_TYPE) &&
            (data->data != NULL) && (data->length == sizeof(bt_sink_srv_call_audio_sync_data_t))) {
            bt_sink_srv_call_pseudo_dev_t *pseudo_dev = NULL;
            bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
            bt_sink_srv_call_audio_sync_data_t *sync_data = (bt_sink_srv_call_audio_sync_data_t *)data->data;
            bt_sink_srv_report_id("[CALL][AUDIO]sync_callback, out_volume:0x%x gpt_count:0x%x", 2, sync_data->out_volume, data->gpt_count);

            switch (aws_role) {
                case BT_AWS_MCE_ROLE_AGENT: {
                    bt_sink_srv_hf_context_t *hf_context
                        = bt_sink_srv_hf_get_context_by_flag(BT_SINK_SRV_HF_FLAG_SCO_CREATED | BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);
                    if (hf_context != NULL) {
                        pseudo_dev = (bt_sink_srv_call_pseudo_dev_t *)hf_context->device;
                    }
                    break;
                }

                case BT_AWS_MCE_ROLE_PARTNER: {
                #ifdef MTK_AWS_MCE_ENABLE
                    bt_sink_srv_aws_mce_call_context_t *aws_context
                        = bt_sink_srv_aws_mce_call_get_context_by_sco_state(BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED);
                    if (aws_context != NULL) {
                        pseudo_dev = (bt_sink_srv_call_pseudo_dev_t *)aws_context->device;
                    }
                #endif
                    break;
                }

                default: {
                    bt_sink_srv_report_id("[CALL][AUDIO]sync_callback, unknown role!", 0);
                    break;
                }
            }

            if (pseudo_dev != NULL) {
                bt_sink_srv_call_audio_sync_out_volume(pseudo_dev->audio_id, sync_data->out_volume, data->gpt_count);
            } else {
                bt_sink_srv_report_id("[CALL][AUDIO]sync_callback, context not found!", 0);
            }
        } else {
            bt_sink_srv_report_id("[CALL][AUDIO]sync_callback, not call type!", 0);
        }
    }
}
#endif

