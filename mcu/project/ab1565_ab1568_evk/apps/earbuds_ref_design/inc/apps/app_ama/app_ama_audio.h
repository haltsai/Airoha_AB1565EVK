
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

#ifndef __APP_AMA_AUD_H__
#define __APP_AMA_AUD_H__

#include <stdio.h>
#include "Ama_Hal.h"
#include "syslog.h"
#include "record_control.h"
#include "apps_debug.h"
#include "Ama_Interface.h"
#include "bt_sink_srv.h"

#ifdef MTK_AMA_ENABLE

#define APP_AMA_AUDIO "[AMA]app_ama_audio"

/**
 * @brief The WWD start index which required in the AMA spec.
 *
 */
#define AMA_AUDIO_WWD_START_INDEX                       8000

/**
 * @brief When audio recorder stopped by other application
 *
 */
typedef bool (*on_recorder_stop_record)(void);

/**
 * @brief When audio recorder has been release by other application
 *
 */
typedef void (*on_recorder_released)(void);

/**
 * @brief When the wake word detected.
 *
 */
typedef bool (*on_wwd_trigger)(uint32_t stop_index);

void ama_audio_set_wwd_trigger_callback(on_wwd_trigger trigger_callback);

void ama_audio_restart(wwe_mode_t wwe_mode, uint32_t wwe_flash_address, uint32_t wwe_length);

void ama_audio_init(on_recorder_stop_record callback, on_recorder_released released_callback);

void ama_audio_start(wwe_mode_t wwe_mode, uint32_t wwe_flash_address, uint32_t wwe_length);

void ama_audio_stop();

bool ama_audio_is_busy();

/**
 * @brief Audio recorder has been stopped by LD and cannot start AMA while LD ongoing
 *
 */
bool ama_audio_is_stopped_by_ld();

bool ama_audio_is_suspended();

#endif
#endif



