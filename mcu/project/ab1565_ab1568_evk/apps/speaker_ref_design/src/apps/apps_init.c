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

/**
 * File: apps_init.c
 *
 * Description: This file is used to init APP modules.
 *
 */

#include "apps_events_key_event.h"
#include "apps_events_battery_event.h"
#include "apps_events_bt_event.h"
#include "apps_race_cmd_event.h"
#include "apps_bt_aws_mce_le_association.h"
#include "app_battery_idle_activity.h"
#include "app_home_screen_idle_activity.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "ui_shell_manager.h"
#include "app_hfp_idle_activity.h"
#include "app_music_idle_activity.h"
#include "apps_config_features_dynamic_setting.h"
#include "apps_config_led_manager.h"
#ifdef MTK_FOTA_ENABLE
#include "app_fota_idle_activity.h"
#endif
#include "app_power_save_idle_activity.h"
#include "app_preproc_activity.h"

#include "app_multi_va_idle_activity.h"
#include "app_speaker_mode_idle_activity.h"

#include "app_fm_idle_activity.h"
#include "bt_app_common.h"
#ifdef __BT_FAST_PAIR_ENABLE__
#include "app_fast_pair.h"
#endif
#include "app_bt_conn_componet_in_homescreen.h"
#include "multi_va_manager.h"
#include "apps_debug.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "app_led_control.h"
#include "app_voice_prompt.h"
#ifdef MTK_AMA_ENABLE
#include "app_ama_audio.h"
#include "app_ama_idle_activity.h"
#include "app_ama_multi_va.h"

#endif
#include "apps_config_key_remapper.h"
#include "apps_state_report.h"

#ifdef GSOUND_LIBRARY_ENABLE
#include "app_gsound_idle_activity.h"
#include "app_gsound_multi_va.h"
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_idle_activity.h"
#endif
#include "bt_customer_config.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_idle_activity.h"
#endif

#if defined(MTK_SMART_CHARGER_ENABLE)
extern void DrvCharger_SmartCase_Init(void);
#endif

#ifdef MTK_VA_XIAOWEI_ENABLE
#include "app_va_xiaowei_multi_support.h"
#include "app_va_xiaowei_activity.h"
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
#include "app_va_xiaoai_activity.h"
#include "app_va_xiaoai_ble_adv.h"
#endif

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
#include "app_leakage_detection_idle_activity.h"
#endif

#ifdef APPS_LINE_IN_SUPPORT
#include "app_line_in_idle_activity.h"
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "app_ull_idle_activity.h"
#endif

#ifdef APPS_USB_AUDIO_SUPPORT
#include "app_usb_audio_idle_activity.h"
#endif

#include "apps_control_touch_key_status.h"
#include "app_bt_takeover_service.h"

static void apps_init_events_senders(void)
{
    apps_event_key_event_init();
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
    apps_events_battery_event_init();
#endif
    apps_events_bt_event_init();
#ifdef CCASE_ENABLE
    apps_events_charger_case_event_init();
#endif
#ifdef MTK_RACE_CMD_ENABLE
    bt_race_app_event_init();
    apps_race_cmd_event_init();
#endif
#ifdef MTK_AWS_MCE_ENABLE
    app_aws_report_event_init();
#endif
#ifdef AIR_TILE_ENABLE
    apps_events_test_event_init();
#endif
}

static void apps_configs_module_init(void)
{
    apps_config_led_manager_init();
}

static void apps_init_applications(void)
{
    ui_shell_set_pre_proc_func(app_preproc_activity_proc);
    ui_shell_start_activity(NULL, app_event_state_report_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);

    ui_shell_start_activity(NULL, app_multi_va_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    ui_shell_start_activity(NULL, app_ull_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
    /* Register all idle activities here. The last registered activity is the foreground activity. */
#ifdef GSOUND_LIBRARY_ENABLE
    ui_shell_start_activity(NULL, app_gsound_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#if defined(MTK_SMART_CHARGER_ENABLE)
    DrvCharger_SmartCase_Init();
    ui_shell_start_activity(NULL, app_smcharger_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#elif defined(MTK_BATTERY_MANAGEMENT_ENABLE)
    ui_shell_start_activity(NULL, app_battery_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    ui_shell_start_activity(NULL, app_rho_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
    ui_shell_start_activity(NULL, app_hfp_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
    ui_shell_start_activity(NULL, app_music_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
    ui_shell_start_activity(NULL, app_speaker_mode_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#ifdef MTK_FOTA_ENABLE
    ui_shell_start_activity(NULL, app_fota_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef CCASE_ENABLE
    ui_shell_start_activity(NULL, app_charger_case_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef RACE_FIND_ME_ENABLE
    ui_shell_start_activity(NULL, app_find_me_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef __BT_FAST_PAIR_ENABLE__
    ui_shell_start_activity(NULL, app_fast_pair_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
    ui_shell_start_activity(NULL, app_power_save_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
    ui_shell_start_activity(NULL, app_in_ear_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
    ui_shell_start_activity(NULL, app_home_screen_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_TOP, NULL, 0);
#ifdef MTK_AMA_ENABLE
    ui_shell_start_activity(NULL, app_ama_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
    /* APPS_LINE_IN_SUPPORT for headset project */
#ifdef APPS_LINE_IN_SUPPORT
    ui_shell_start_activity(NULL, app_line_in_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
    /* APPS_USB_AUDIO_SUPPORT for headset project */
#ifdef APPS_USB_AUDIO_SUPPORT
    ui_shell_start_activity(NULL, app_usb_audio_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef MTK_VA_XIAOWEI_ENABLE
    ui_shell_start_activity(NULL, app_va_xiaowei_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
    ui_shell_start_activity(NULL, app_va_xiaoai_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    ui_shell_start_activity(NULL, app_leakage_detection_idle_activity_proc, ACTIVITY_PRIORITY_IDLE_BACKGROUND, NULL, 0);
#endif
}

static void apps_init_multi_va(void)
{
#ifdef MTK_AMA_ENABLE
    apps_register_ama_in_multi_voice();
#endif
#ifdef GSOUND_LIBRARY_ENABLE
    app_gsound_multi_va_register();
#endif
#ifdef MTK_VA_XIAOWEI_ENABLE
    app_va_xiaowei_multi_support_register();
#endif
#ifdef MTK_VA_XIAOAI_MULTI_VA_ENABLE
    app_va_xiaoai_multi_support_register();
#endif
    multi_va_manager_start();
}

void apps_init(void)
{
    /* init LED module. */
#if LED_ENABLE
    app_led_control_init();
#endif
    /* update feature setting. */
    apps_config_features_dynamic_setting_init();
    /* init APP event sender, such as key event, battery event, bt event etc. */
    apps_init_events_senders();
    /* init APP config module, only led manager. */
    apps_configs_module_init();
    /* init speaker association. */
    bt_aws_mce_le_association_app_init();
    /* All VA register to multi VA manager and start. */
    apps_init_multi_va();
    /* Init BT takeover service. */
    app_bt_takeover_service_init();
    /* Init all APP activities. */
    apps_init_applications();
    /* Init Key remaper table. */
    apps_config_key_remaper_init_configurable_table();
    /* Init touch key status. */
    apps_init_touch_key_status();
    app_voice_prompt_init();
    /* Start UI shell. */
    ui_shell_start();
}
