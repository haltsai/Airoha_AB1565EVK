/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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


/**
 * File: apps_events_audio_event.c
 *
 * Description: This file defines callback of audio and send events to APPs
 *
 */

#include "ui_shell_manager.h"
#include "apps_events_audio_event.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_sink_srv_ami.h"
#include "apps_debug.h"

#define LOG_TAG    "[AUDIO_EVENT]"

#ifdef AIR_SILENCE_DETECTION_ENABLE
static bool s_silence_detect_has_active_audio = false; /* The silence detect has active audio. */

static void app_events_audio_event_silence_detect_callback(bool silence_flag)
{
    APPS_LOG_MSGID_I(LOG_TAG"Received silence detect flag %d -> %d", 2, !s_silence_detect_has_active_audio, silence_flag);
    s_silence_detect_has_active_audio = !silence_flag;
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_SILENCE_DETECT_CHANGE, NULL, 0, NULL, 0);
}

bool app_events_audio_event_get_silence_detect_flag(void)
{
    return !s_silence_detect_has_active_audio;
}
#endif

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
static void apps_events_audio_ami_callback(vendor_se_event_t event, void *arg)
{
    ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_AMI_VENDOR,
                        event, NULL, 0, NULL, 0);
}
#endif

void apps_events_audio_event_init(void)
{
#ifdef AIR_SILENCE_DETECTION_ENABLE
    audio_silence_detection_callback_register(app_events_audio_event_silence_detect_callback);
#endif

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    int32_t se_id = 0;
    bt_sink_srv_am_result_t ret;

    se_id = ami_get_vendor_se_id();
    ret = ami_register_vendor_se(se_id, apps_events_audio_ami_callback);
    APPS_LOG_MSGID_I(LOG_TAG"am init: %d", 1, ret);
#endif
}
