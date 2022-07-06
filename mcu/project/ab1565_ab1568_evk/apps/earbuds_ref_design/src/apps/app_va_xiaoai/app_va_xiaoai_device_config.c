
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

/**
 * File: app_va_xiaoai_device_config.c
 *
 * Description: This file provides Device Config Feature for XiaoAI activity.
 *
 */

#ifdef MTK_VA_XIAOAI_ENABLE

#include "app_va_xiaoai_device_config.h"

#include "apps_aws_sync_event.h"
#include "apps_config_key_remapper.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "app_va_xiaoai_ble_adv.h"
#include "app_va_xiaoai_hfp_at_cmd.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "ui_shell_manager.h"

#include "xiaoai.h"

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_utils.h"
#endif
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_hfp_utils.h"
#include "app_music_utils.h"
#include "app_in_ear_utils.h"
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
#include "leakage_detection_control.h"
#endif
#include "app_fm_activity.h"
#include "bt_sink_srv_music.h"



#define APP_VA_XIAOAI_DC_TAG           "[XIAOAI_DC]"

#define XIAOAI_MMA_SET_DEVICE_INFO_OPCODE           0x08
#define XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE         0xF2
#define XIAOAI_MMA_GET_DEVICE_INFO_OPCODE           0xF3
#define XIAOAI_MMA_NOTIFY_DEVICE_INFO_OPCODE        0xF4

extern bool xiaoai_app_aws_is_connected();
static void app_va_xiaoai_update_saving_config();

typedef struct {
    uint8_t wwe_enable: 1;           // 0 - OFF, 1 - ON
    uint8_t eq_mode: 3;              // 0~6
    uint8_t left_anc_mode: 4;        // bit0 - Disable, bit1 - Enable, bit2 - Passthrough
    uint8_t right_anc_mode: 4;       // bit0 - Disable, bit1 - Enable, bit2 - Passthrough
    uint8_t anc_state: 2;            // 0 - Disable, 1 - Enable, 2 - Passthrough
    uint8_t anc_level: 2;            // anc_level - 0/1/2/3, passthrough - 0/1
    uint8_t pt_level: 2;
    uint8_t audio_mode: 2;           // audio_mode, 0 - normal, 1 - free listen, 2 - high quality
    uint8_t chat_mode;               // listen_free_mode: 0 - close, 0~0xFF open with timer, 0xFF open always
} PACKED xiaoai_saving_config_t;

static xiaoai_saving_config_t g_xiaoai_saving_config = {0};
static uint8_t                g_xiaoai_anc_target_level = 0;

typedef enum {
    XIAOAI_AUDIO_MODE_NORMAL = 0,
    XIAOAI_AUDIO_MODE_FREE_LISTEN,
    XIAOAI_AUDIO_MODE_HIGH_QUALITY
} xiaoai_auduo_mode_t;

static bool app_va_xiaoai_get_phone_addr(bt_bd_addr_t *bt_addr)
{
    bt_bd_addr_t addr_list[1] = {{0}};
    uint32_t num = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), addr_list, 1);
    uint8_t *addr = addr_list[0];
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" get_phone_addr, count=%d %02X:%02X:%02X:%02X:%02X:%02X",
                     7, num, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    if (num != 0) {
        memcpy(bt_addr, addr_list[0], BT_BD_ADDR_LEN);
    }
    return (num > 0);
}

static void app_va_xiaoai_device_config_reply(uint8_t opcode, uint8_t status, uint8_t app_type, uint8_t opcode_sn)
{
    bool ret = xiaoai_send_mma_rsp(opcode, app_type, status, opcode_sn, NULL, 0);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" device_config reply, opcode=%02X status=%d ret=%d ",
                     3, opcode, status, ret);
}

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
typedef enum {
    XIAOAI_LEAKAGE_UNKOWN = 0,
    XIAOAI_LEAKAGE_GOOD,
    XIAOAI_LEAKAGE_BAD,
    XIAOAI_LEAKAGE_CAN_CHECK = 3,
    XIAOAI_LEAKAGE_CANNOT_CHECK = 9
} xiaoai_leakage_check_result_t;

typedef enum {
    XIAOAI_LEAKAGE_STATUS_NONE = 0,
    XIAOAI_LEAKAGE_STATUS_AGENT_DONE = 1,
    XIAOAI_LEAKAGE_STATUS_PARTNER_DONE = 2,
    XIAOAI_LEAKAGE_STATUS_BOTH_DONE = 3
} xiaoai_leakage_check_status_t;

typedef struct {
    uint8_t    status;
    uint8_t    left_result;
    uint8_t    right_result;
} xiaoai_leakage_result;

static xiaoai_leakage_result g_xiaoai_leakage_result = {0};
static TimerHandle_t         g_xiaoai_leakage_timer;
static bool                  g_xiaoai_ld_ongoing = FALSE;

static void xiaoai_leakage_agent_check_result()
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" leakage_check_result, status=%d left=%d right=%d",
                     3, g_xiaoai_leakage_result.status, g_xiaoai_leakage_result.left_result,
                     g_xiaoai_leakage_result.right_result);
    if (g_xiaoai_leakage_result.status == XIAOAI_LEAKAGE_STATUS_BOTH_DONE) {
        uint16_t result = ((g_xiaoai_leakage_result.right_result << 8)
                                | g_xiaoai_leakage_result.left_result);
        app_va_xiaoai_notify_device_config(XIAOAI_DEVICE_CONFIG_HEADSET_FIT,
                                           (uint8_t *)&result, sizeof(uint16_t));

        // Agent stop leakage_detection VP (only malloc 1 byte... due to seal check APP)
        bool *play_flag = (bool *)pvPortMalloc(sizeof(bool));
        if (play_flag == NULL) {
            return;
        }
        *play_flag = FALSE;
        ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                       APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER,
                                                       play_flag, sizeof(bool), NULL, 0);
        if (UI_SHELL_STATUS_OK != status) {
            vPortFree(play_flag);
        }
        // Agent resume music & ANC (sync to partner)
        audio_anc_leakage_detection_resume_dl();

        g_xiaoai_ld_ongoing = FALSE;
    }
}

static void xiaoai_leakage_detection_timer_cb(TimerHandle_t xTimer)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" leakage_detection timer callback", 0);
    app_va_xiaoai_agent_handle_partner_leakage_result(XIAOAI_LEAKAGE_UNKOWN);
}

static void xiaoai_leakage_detection_callback(uint16_t leakage_result)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [%02X] leakage_detection callback status=%d",
                     2, role, leakage_result);
    if (leakage_result == LD_STATUS_PASS) {
        leakage_result = XIAOAI_LEAKAGE_GOOD;
    } else if (leakage_result == LD_STATUS_FAIL_CASE_1 || leakage_result == LD_STATUS_FAIL_CASE_2) {
        leakage_result = XIAOAI_LEAKAGE_BAD;
    } else {
        leakage_result = XIAOAI_LEAKAGE_UNKOWN;
    }

    // stop detection
    audio_anc_leakage_compensation_stop();
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        bool agent_is_left = (ami_get_audio_channel() == AUDIO_CHANNEL_L);
        if (agent_is_left) {
            g_xiaoai_leakage_result.left_result = leakage_result;
        } else {
            g_xiaoai_leakage_result.right_result = leakage_result;
        }
        g_xiaoai_leakage_result.status |= XIAOAI_LEAKAGE_STATUS_AGENT_DONE;
        xiaoai_leakage_agent_check_result();
        if (g_xiaoai_leakage_result.status != XIAOAI_LEAKAGE_STATUS_BOTH_DONE) {
            if (g_xiaoai_leakage_timer == NULL) {
                g_xiaoai_leakage_timer = xTimerCreate("LDTmr",
                                                       2000 / portTICK_PERIOD_MS,
                                                       pdFALSE,
                                                       0,
                                                       xiaoai_leakage_detection_timer_cb);
            }
            if (g_xiaoai_leakage_timer != NULL) {
                if (xTimerStart(g_xiaoai_leakage_timer, 0) == pdPASS) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] leakage_detection timer start", 0);
                } else {
                    APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [Agent] leakage_detection timer start fail", 0);
                }
            }
        }
    } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
        bt_status_t status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_XIAOAI,
                                                            XIAOAI_EVENT_SYNC_LEAKAGE_RESULT,
                                                            (uint8_t *)&leakage_result,
                                                            sizeof(uint8_t));
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Partner] aws_send status=0x%08X", 1, status);
    }
}

static bool app_va_xiaoai_start_leakage_detection()
{
    bool ret = FALSE;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
    if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" leakage_detection fail, not agent role %02X",
                         1, role);
        return ret;
    } else if (!xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" leakage_detection fail, aws_not_connected", 0);
        return ret;
    } else if (bt_sink_state >= BT_SINK_SRV_STATE_INCOMING) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" leakage_detection fail, HFP ongoing %d",
                         1, bt_sink_state);
        return ret;
    }
#ifdef MTK_IN_EAR_FEATURE_ENABLE
    app_in_ear_state_t in_ear_state = app_in_ear_get_state();
    if (in_ear_state != APP_IN_EAR_STA_BOTH_IN) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" leakage_detection fail, not both_in_ear %d",
                         1, in_ear_state);
        return ret;
    }
#endif

    g_xiaoai_leakage_result.status = 0;
    g_xiaoai_leakage_result.left_result = XIAOAI_LEAKAGE_UNKOWN;
    g_xiaoai_leakage_result.right_result = XIAOAI_LEAKAGE_UNKOWN;
    audio_anc_leakage_detection_execution_t anc_ret = audio_anc_leakage_detection_prepare(xiaoai_leakage_detection_callback);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" leakage_detection, anc_ret=%d",
                     1, anc_ret);
    return (anc_ret == AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS);
}

void app_va_xiaoai_notify_leakage_detectable(bool detectable)
{
    uint16_t result = 0;
    if (detectable) {
        result = ((XIAOAI_LEAKAGE_CAN_CHECK << 8) | XIAOAI_LEAKAGE_CAN_CHECK);
    } else {
        result = ((XIAOAI_LEAKAGE_CANNOT_CHECK << 8) | XIAOAI_LEAKAGE_CANNOT_CHECK);
    }
    app_va_xiaoai_notify_device_config(XIAOAI_DEVICE_CONFIG_HEADSET_FIT,
                                       (uint8_t *)&result, sizeof(uint16_t));
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" notify_leakage_detectable %d",
                     1, detectable);
}

void app_va_xiaoai_agent_handle_partner_leakage_result(uint8_t leakage_result)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] leakage handle partner, leakage_result=%d",
                     1, leakage_result);
    if (g_xiaoai_leakage_timer != NULL
            && xTimerIsTimerActive(g_xiaoai_leakage_timer)) {
        xTimerStop(g_xiaoai_leakage_timer, 0);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] leakage handle partner, stop timer", 0);
    }

    bool agent_is_left = (ami_get_audio_channel() == AUDIO_CHANNEL_L);
    if (agent_is_left) {
        g_xiaoai_leakage_result.right_result = leakage_result;
    } else {
        g_xiaoai_leakage_result.left_result = leakage_result;
    }
    g_xiaoai_leakage_result.status |= XIAOAI_LEAKAGE_STATUS_PARTNER_DONE;
    xiaoai_leakage_agent_check_result();
}

bool app_va_xiaoai_is_ld_ongoing()
{
    return g_xiaoai_ld_ongoing;
}
#endif



typedef enum {
    XIAOAI_ANC_SET_STATE_SUCCESS = 0,
    XIAOAI_ANC_SET_STATE_IGNORE,
    XIAOAI_ANC_SET_STATE_FAIL
} xiaoai_anc_set_state_result;

#ifdef MTK_ANC_ENABLE
typedef enum {
    XIAOAI_ANC_DISABLE = 0,         /* Disable ANC. */
    XIAOAI_ANC_ENABLE,              /* Enable ANC. */
    XIAOAI_ANC_ENABLE_PASSTHROUGH,  /* Enable passthrough. */
    XIAOAI_ANC_ENABLE_ANTI_WIND     /* Anti-Wind noise = FB + default filter_id. */
} xiaoai_anc_action;

typedef enum {
    XIAOAI_ANC_STATE_DISABLE = 0,
    XIAOAI_ANC_STATE_ENABLE,
    XIAOAI_ANC_STATE_PASSTHROUGH,
    XIAOAI_ANC_STATE_ANTI_WIND
} xiaoai_anc_state;

static bool app_va_xiaoai_apply_anc_key(bool need_sync, uint8_t left_anc_mode, uint8_t right_anc_mode)
{
    bool is_left = (ami_get_audio_channel() == AUDIO_CHANNEL_L);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" update_anc_key, need_sync=%d is_left=%d left=%d right=%d",
                     4, need_sync, is_left, left_anc_mode, right_anc_mode);
    if (need_sync && !xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" update_anc_key fail, not aws connected", 0);
        return FALSE;
    }
    if (is_left) {
        /* Customer configure option: Use left_anc_mode to config Key ANC switch mode. */
    } else {
        /* Customer configure option: Use right_anc_mode to config Key ANC switch mode. */
    }
    return TRUE;
}
#endif

static uint8_t app_va_xiaoai_get_anc_state()
{
#ifdef MTK_ANC_ENABLE
    uint8_t                         anc_enabled = 0;
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_get_status(&anc_enabled, &anc_filter_id, &anc_type, NULL, NULL, NULL);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" get ANC, enabled=%d filter_id=%d anc_type=%d",
                     3, anc_enabled, anc_filter_id, anc_type);

    uint8_t anc_state = XIAOAI_ANC_STATE_DISABLE;
    if (anc_enabled == 1) {
        if (anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            if (anc_type == AUDIO_ANC_CONTROL_TYPE_FB
                    && anc_filter_id == AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT) {
                anc_state = XIAOAI_ANC_STATE_ANTI_WIND;
            } else {
                anc_state = XIAOAI_ANC_STATE_ENABLE;
            }
        } else if (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            anc_state = XIAOAI_ANC_STATE_PASSTHROUGH;
        }
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" get_anc_state %d", 1, anc_state);
    return anc_state;
#else
    return 0;
#endif
}

uint8_t app_va_xiaoai_get_anc_state_for_miui()
{
#ifdef MTK_ANC_ENABLE
    uint8_t anc_state = app_va_xiaoai_get_anc_state();
    if (anc_state == XIAOAI_ANC_STATE_ENABLE || anc_state == XIAOAI_ANC_STATE_ANTI_WIND) {
        anc_state = 2;
    } else if (anc_state == XIAOAI_ANC_STATE_PASSTHROUGH) {
        anc_state = 1;
    }
    return anc_state;
#else
    return 0;
#endif
}

static xiaoai_anc_set_state_result app_va_xiaoai_set_anc_state(uint8_t target_anc_state, uint8_t level)
{
    bool anc_ret = FALSE;
#ifdef MTK_ANC_ENABLE
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, target_anc_state=%d", 1, target_anc_state);
    uint8_t anc_enabled = 0;
    audio_anc_control_filter_id_t anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_misc_t anc_misc;
    int16_t anc_runtime_gain = 0;
    uint8_t anc_hybrid_enabled = 0;
    audio_anc_control_get_status(&anc_enabled, &anc_filter_id, &anc_type,
                                 &anc_runtime_gain, &anc_hybrid_enabled, &anc_misc);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, enabled=%d filter_id=%d anc_type=%d anc_hybrid_enabled=%d gain=%d",
            5, anc_enabled, anc_filter_id, anc_type, anc_hybrid_enabled, anc_runtime_gain);

    if (target_anc_state == XIAOAI_ANC_DISABLE && anc_enabled == 0) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, ignore DISABLE action", 0);
        return XIAOAI_ANC_SET_STATE_IGNORE;
    } else if ((target_anc_state == XIAOAI_ANC_ENABLE
            || target_anc_state == XIAOAI_ANC_ENABLE_PASSTHROUGH
            || target_anc_state == XIAOAI_ANC_ENABLE_ANTI_WIND)
            && anc_enabled == 1) {
        if (target_anc_state == XIAOAI_ANC_ENABLE
                && ((anc_type == AUDIO_ANC_CONTROL_TYPE_FB
                        && anc_filter_id != AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT)
                        || anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)
                && g_xiaoai_saving_config.anc_level == level) {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, ignore ENABLE action", 0);
            return XIAOAI_ANC_SET_STATE_IGNORE;
        } else if (target_anc_state == XIAOAI_ANC_ENABLE_PASSTHROUGH
                && anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF
                && g_xiaoai_saving_config.pt_level == level) {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, ignore ENABLE_PASSTHROUGH action", 0);
            return XIAOAI_ANC_SET_STATE_IGNORE;
        } else if (target_anc_state == XIAOAI_ANC_ENABLE_ANTI_WIND
                && anc_type == AUDIO_ANC_CONTROL_TYPE_FB
                && anc_filter_id == AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT
                && g_xiaoai_saving_config.anc_level == level) {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, ignore ENABLE_ANTI_WIND action", 0);
            return XIAOAI_ANC_SET_STATE_IGNORE;
        }
    }

    audio_anc_control_filter_id_t target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
    int16_t target_runtime_gain = AUDIO_ANC_CONTROL_UNASSIGNED_GAIN;
    if (target_anc_state == XIAOAI_ANC_ENABLE) {
        /* Customer configure option: set your filter/runtime_gain according to the target_anc_state/level. */
        if (level == 0) {
            target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
        } else if (level == 1) {
            target_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_ANC_2;
        } else if (level == 2) {
            target_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_ANC_3;
        } else if (level == 3) {
            target_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_ANC_4;
        }
    } else if (target_anc_state == XIAOAI_ANC_ENABLE_PASSTHROUGH) {
        target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
        if (level == 1) {
            target_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_PASS_THRU_2;
        }
    } else if (target_anc_state == XIAOAI_ANC_DISABLE) {
        // ignore level
    }

    if (target_anc_state == XIAOAI_ANC_DISABLE) {
        anc_ret = app_anc_service_disable();
    } else if (target_anc_state == XIAOAI_ANC_ENABLE) {
        audio_anc_control_type_t target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
        if (anc_hybrid_enabled) {
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
        } else {
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
        }
        anc_ret = app_anc_service_enable(target_filter_id, target_anc_type,
                                         target_runtime_gain, NULL);
    } else if (target_anc_state == XIAOAI_ANC_ENABLE_PASSTHROUGH) {
        anc_ret = app_anc_service_enable(target_filter_id,
                                         AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF,
                                         target_runtime_gain, NULL);
    } else if (target_anc_state == XIAOAI_ANC_ENABLE_ANTI_WIND) {
        anc_ret = app_anc_service_enable(target_filter_id,
                                         AUDIO_ANC_CONTROL_TYPE_FB,
                                         target_runtime_gain, NULL);
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_anc_state, target_anc_state=%d level=%d anc_ret=%d",
                     3, target_anc_state, level, anc_ret);
#endif
    return (anc_ret ? XIAOAI_ANC_SET_STATE_SUCCESS : XIAOAI_ANC_SET_STATE_FAIL);
}

static void app_va_xiaoai_agent_handle_anc_level(uint8_t f4_anc_state, uint8_t anc_target_level)
{
#ifdef MTK_ANC_ENABLE
    g_xiaoai_saving_config.anc_state = f4_anc_state;
    if (f4_anc_state == XIAOAI_ANC_STATE_ENABLE) {
        g_xiaoai_saving_config.anc_level = anc_target_level;
    } else if (f4_anc_state == XIAOAI_ANC_STATE_PASSTHROUGH) {
        g_xiaoai_saving_config.pt_level = anc_target_level;
    }
    // Save and notify to partner
    app_va_xiaoai_update_saving_config();

    // 0xF4 Notify
    uint16_t f4_anc_result = 0;
    if (f4_anc_state == XIAOAI_ANC_STATE_ENABLE) {
        f4_anc_result = ((g_xiaoai_saving_config.anc_level << 8) | g_xiaoai_saving_config.anc_state);
    } else if (f4_anc_state == XIAOAI_ANC_STATE_PASSTHROUGH) {
        f4_anc_result = ((g_xiaoai_saving_config.pt_level << 8) | g_xiaoai_saving_config.anc_state);
    } else {
        f4_anc_result = 0;
    }
    app_va_xiaoai_notify_device_config(XIAOAI_DEVICE_CONFIG_SELECT_ANC_LEVEL,
                                       (uint8_t *)&f4_anc_result, sizeof(uint16_t));
#endif
}

void app_va_xiaoai_handle_anc_event(bool on_event, void *extra_data)
{
#ifdef MTK_ANC_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    uint8_t new_anc_state = XIAOAI_ANC_STATE_DISABLE;
    if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" ANC EVENT, not Agent role", 0);
        return;
    }

    if (on_event) {
        app_anc_srv_result_t *anc_result = (app_anc_srv_result_t *)extra_data;
        new_anc_state = XIAOAI_ANC_STATE_ENABLE;
        if (anc_result->cur_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            new_anc_state = XIAOAI_ANC_STATE_PASSTHROUGH;
        } else if (anc_result->cur_type == AUDIO_ANC_CONTROL_TYPE_FB
                && anc_result->cur_filter_id == AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT) {
            new_anc_state = XIAOAI_ANC_STATE_ANTI_WIND;
        }
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" ANC EVENT ON type=%d filter=%d -> state=%d",
                         3, anc_result->cur_type, anc_result->cur_filter_id, new_anc_state);
    } else {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" ANC EVENT OFF", 0);
    }
    xiaoai_notify_anc_status(new_anc_state);
    app_va_xiaoai_hfp_miui_more_atcmd_report_anc();

    uint8_t f4_anc_state = new_anc_state;
    if (new_anc_state == XIAOAI_ANC_STATE_ANTI_WIND) {
        f4_anc_state = XIAOAI_ANC_STATE_ENABLE;
    }

    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" ANC EVENT, %d anc_state=%d->%d anc_target_level=%d",
                     4, new_anc_state, g_xiaoai_saving_config.anc_state,
                     f4_anc_state, g_xiaoai_anc_target_level);
    app_va_xiaoai_agent_handle_anc_level(f4_anc_state, g_xiaoai_anc_target_level);
#endif
}

static uint8_t app_va_xiaoai_get_game_mode()
{
    uint8_t result = 0;
    bt_bd_addr_t bt_addr = {0};
    bool ret = app_va_xiaoai_get_phone_addr(&bt_addr);
    if (ret) {
        bt_sink_srv_music_mode_t mode = bt_sink_srv_music_get_mode(&bt_addr);
        result = (mode == BT_SINK_SRV_MUSIC_GAME_MODE ? 1 : 0);
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" get_game_mode, result=%d", 1, result);
    return result;
}

uint8_t app_va_xiaoai_get_game_mode_for_miui()
{
    /* Customer configure option: not enable/disable game_mode. */
    // 0 - low_latency for all
    // 1 - auto-control low_latency via game center
    return 0;
}

static bool app_va_xiaoai_set_game_mode(bool enable)
{
    bt_bd_addr_t bt_addr;
    bool ret = app_va_xiaoai_get_phone_addr(&bt_addr);
    if (ret) {
        bt_sink_srv_music_mode_t mode = BT_SINK_SRV_MUSIC_NORMAL_MODE;
        if (enable) {
            mode = BT_SINK_SRV_MUSIC_GAME_MODE;
        }
        bt_status_t status = bt_sink_srv_music_set_mode(&bt_addr, mode);
        ret = (status == BT_STATUS_SUCCESS);
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" set_game_mode, ret=%d enable=%d",
                     2, ret, enable);
    return ret;
}

bool app_va_xiaoai_agent_set_device_name(uint8_t *name, uint8_t len)
{
    bool ret = FALSE;
    // APPS_LOG_I("[XIAOAI_DC] set_device_name name=%s\r\n", name);
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
    if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" leakage_detection fail, not agent role %02X",
                         1, role);
        return ret;
    } else if (name == NULL || len == 0) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] set_device_name, name=0x%08X len=%d",
                         2, name, len);
        return ret;
    } else if (!xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [Agent] set_device_name, aws_not_connected", 0);
        return ret;
    } else if (bt_sink_state >= BT_SINK_SRV_STATE_INCOMING) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [Agent] set_device_name, HFP ongoing %d",
                         1, bt_sink_state);
        return ret;
    }

    if (len > XIAOAI_BT_NAME_LEN) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] set_device_name, len slim=%d->%d",
                         2, len, XIAOAI_BT_NAME_LEN);
        len = XIAOAI_BT_NAME_LEN;
    }

    nvkey_status_t status = nvkey_write_data(NVKEYID_APP_DEVICE_NAME_USER_DEFINED,
                                             name, len);
    if (status == NVKEY_STATUS_OK) {
        bt_status_t status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_XIAOAI,
                                                            XIAOAI_EVENT_SYNC_DEVICE_NAME,
                                                            name, len);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] set_device_name, status=0x%08X",
                         1, status);
        if (status == BT_STATUS_SUCCESS) {
            ui_shell_status_t ui_status = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                                              EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                              APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                                                              NULL, 0, NULL, 1000);
            ret = (ui_status == UI_SHELL_STATUS_OK);
        }
    }
    return ret;
}

void app_va_xiaoai_partner_set_device_name(uint8_t *name, uint8_t len)
{
    if (name == NULL || len == 0) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [Partner] set_device_name, name=0x%08X len=%d",
                         2, name, len);
        return;
    }
    nvkey_status_t nvkey_status = nvkey_write_data(NVKEYID_APP_DEVICE_NAME_USER_DEFINED,
                                             name, len);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Partner] set_device_name, nvkey_status=%d len=%d",
                     2, nvkey_status, len);
    if (nvkey_status == NVKEY_STATUS_OK) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                            NULL, 0, NULL, 0);
    }
}

static uint8_t* app_va_xiaoai_get_device_name()
{
    uint8_t *buf = (uint8_t *)pvPortMalloc(XIAOAI_BT_NAME_LEN + 1);
    if (buf == NULL) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" get_device_name, malloc fail", 0);
        return NULL;
    }
    memset(buf, 0, XIAOAI_BT_NAME_LEN + 1);

    uint32_t len = XIAOAI_BT_NAME_LEN;
    nvkey_status_t status = nvkey_read_data(NVKEYID_APP_DEVICE_NAME_USER_DEFINED,
                                            buf, &len);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" get_device_name, status=%d len=%d",
                     2, status, len);
    if (status != NVKEY_STATUS_OK) {
        buf = NULL;
    }
    return buf;
}

bool app_va_xiaoai_is_enable_wwe()
{
    uint8_t wwe_state = 0;
#ifdef MTK_XIAOAI_WWE_ENABLE
    wwe_state = g_xiaoai_saving_config.wwe_enable;
#endif
    return (wwe_state == 1);
}

#ifdef MTK_XIAOAI_WWE_ENABLE
static bool app_va_xiaoai_enable_wwe(bool enable)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [%02X] set_wwe_state, bt_sink_state=%d enable=%d",
                     3, role, bt_sink_state, enable);
    if (g_xiaoai_saving_config.wwe_enable == enable) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [%02X] set_wwe_state, ignore same enable=%d",
                         2, role, enable);
        return TRUE;
    } else if (!xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [%02X] set_wwe_state, aws_not_connected",
                         1, role);
        return FALSE;
    } else if (bt_sink_state >= BT_SINK_SRV_STATE_INCOMING) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" [%02X] set_wwe_state, HFP ongoing %d",
                         2, role, bt_sink_state);
        return FALSE;
    }
    if (enable) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_XIAOAI,
                            XIAOAI_EVENT_START_WWE_ACTION,
                            NULL, 0, NULL, 0);
    } else {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_XIAOAI,
                            XIAOAI_EVENT_STOP_WWE_ACTION,
                            NULL, 0, NULL, 0);
    }
    g_xiaoai_saving_config.wwe_enable = enable;
    app_va_xiaoai_update_saving_config();
    return TRUE;
}
#endif

static bool app_va_xiaoai_apply_audio_mode(bool need_sync, uint8_t audio_mode)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [%2X] apply_audio_mode, need_sync=%d %d->%d",
                     4, role, need_sync, g_xiaoai_saving_config.audio_mode, audio_mode);
    if (g_xiaoai_saving_config.audio_mode == audio_mode) {
        return TRUE;
    } else if (need_sync && !xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" apply_audio_mode fail, not aws connected", 0);
        return FALSE;
    }

    if (audio_mode == XIAOAI_AUDIO_MODE_NORMAL) {

    } else if (audio_mode == XIAOAI_AUDIO_MODE_FREE_LISTEN) {

    } else if (audio_mode == XIAOAI_AUDIO_MODE_HIGH_QUALITY) {

    }
    return TRUE;
}

static bool app_va_xiaoai_apply_chat_mode(bool need_sync, uint8_t chat_mode)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [%2X] apply_chat_mode, need_sync=%d %d->%d",
                     4, role, need_sync, g_xiaoai_saving_config.chat_mode, chat_mode);
    if (g_xiaoai_saving_config.chat_mode == chat_mode) {
        return TRUE;
    } else if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" apply_chat_mode fail, not Agent", 0);
        return FALSE;
    } else if (need_sync && !xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" apply_chat_mode fail, not aws connected", 0);
        return FALSE;
    }

    if (chat_mode == 0) {
        // close
    } else if (chat_mode > 0 && chat_mode < 0xFF) {
        // open with timer
    } else if (chat_mode == 0xFF) {
        // open always
    }
    return TRUE;
}

#ifdef MTK_PEQ_ENABLE

#include "race_cmd_dsprealtime.h"

typedef enum {
    XIAOAI_EQ_MODE_DEFAULT = 0,
    XIAOAI_EQ_MODE_HUMAN,
    XIAOAI_EQ_MODE_ROCK,
    XIAOAI_EQ_MODE_CLASSICAL,
    XIAOAI_EQ_MODE_POPULAR,
    XIAOAI_EQ_MODE_BASS,
    XIAOAI_EQ_MODE_TREBLE
} xiaoai_eq_mode_t;

static bool app_va_xiaoai_apply_eq_mode(uint8_t eq_mode)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" apply_eq_mode [%02X] %d->%d",
                     3, role, g_xiaoai_saving_config.eq_mode, eq_mode);
    if (g_xiaoai_saving_config.eq_mode == eq_mode) {
        return TRUE;
    } else if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" apply_eq_mode fail, not Agent role", 0);
        return FALSE;
    } else if (!xiaoai_app_aws_is_connected()) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" apply_eq_mode fail, not aws connected", 0);
        return FALSE;
    }

    // phase_id: 0 - pre-peq, 1 - post_peq
    // setting_mode: 0:PEQ_DIRECT (only agent)  1:PEQ_SYNC (agent sync to partner).
    // target_bt_clk: sync time
    // enable: 0 - disable, 1 - enable peq
    // sound_mode: index of ped/sound_mode
    // audio_path_id: am_feature_type_t, AM_A2DP_PEQ
    // return 0 - success
    //    uint32_t race_dsprt_peq_change_mode_data(uint8_t phase_id, uint8_t setting_mode,
    //            uint32_t target_bt_clk, uint8_t enable,
    //            uint8_t sound_mode, am_feature_type_t audio_path_id)

    uint8_t setting_mode = 0;
    bt_clock_t target_bt_clk = {0};
    race_dsprt_peq_get_target_bt_clk(role, &setting_mode, &target_bt_clk);
    uint32_t result = race_dsprt_peq_change_mode_data(0, setting_mode, target_bt_clk.nclk,
                                                      TRUE, eq_mode, AM_A2DP_PEQ);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" apply_eq_mode, setting_mode=%d eq_mode=%d result=%d",
                     3, setting_mode, eq_mode, result);
    return (result == 0);
}
#endif

static void app_va_xiaoai_print_saving_config(xiaoai_saving_config_t *config)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" print_saving_config, wwe=%d eq_mode=%d anc=%d,%d,%d,%d,%d audio_mode=%d",
                     8, config->wwe_enable, config->eq_mode, config->left_anc_mode, config->right_anc_mode,
                     config->anc_state, config->anc_level, config->pt_level, config->audio_mode);
}

static void app_va_xiaoai_update_saving_config()
{
    uint32_t size = sizeof(xiaoai_saving_config_t);
    nvkey_status_t nvkey_status = nvkey_write_data(NVKEYID_XIAOAI_KEY_CONFIG_INFO, (const uint8_t *)&g_xiaoai_saving_config, size);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] update_saving_config, nvkey_status=%d size=%d",
                     2, nvkey_status, size);
    app_va_xiaoai_print_saving_config(&g_xiaoai_saving_config);
    if (nvkey_status == NVKEY_STATUS_OK) {
        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_XIAOAI,
                                                               XIAOAI_EVENT_SYNC_SAVING_CONFIG,
                                                               (uint8_t *)&g_xiaoai_saving_config,
                                                               size);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Agent] update_saving_config, bt_status=0x%08X",
                         1, bt_status);
    }
}

void app_va_xiaoai_partner_handle_saving_config(uint8_t *data)
{
    if (data != NULL) {
        xiaoai_saving_config_t *config = (xiaoai_saving_config_t *)data;
        uint32_t size = sizeof(xiaoai_saving_config_t);
        nvkey_status_t nvkey_status = nvkey_write_data(NVKEYID_XIAOAI_KEY_CONFIG_INFO, (const uint8_t *)config, size);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [Partner] handle_saving_config, nvkey_status=%d size=%d",
                         2, nvkey_status, size);
        app_va_xiaoai_print_saving_config(&g_xiaoai_saving_config);
        app_va_xiaoai_print_saving_config(config);

        if (g_xiaoai_saving_config.left_anc_mode != config->left_anc_mode
                || g_xiaoai_saving_config.right_anc_mode != config->right_anc_mode) {
            app_va_xiaoai_apply_anc_key(FALSE, config->left_anc_mode, config->right_anc_mode);
        }
        if (g_xiaoai_saving_config.audio_mode != config->audio_mode) {
            app_va_xiaoai_apply_audio_mode(FALSE, config->audio_mode);
        }
        g_xiaoai_saving_config = *config;
    }
}

uint8_t app_va_xiaoai_get_voice_recognition_state()
{
    return FALSE;
}

uint8_t app_va_xiaoai_get_eq_mode()
{
    return g_xiaoai_saving_config.eq_mode;
}

uint8_t app_va_xiaoai_get_anti_lost_state()
{
    uint8_t result = 0;

    bool agent_is_left = (ami_get_audio_channel() == AUDIO_CHANNEL_L);
#ifdef MTK_SMART_CHARGER_ENABLE
    app_smcharger_in_out_t own = app_smcharger_is_charging();
    app_smcharger_in_out_t peer = app_smcharger_peer_is_charging();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" Anti-lost agent_is_left=%d own=%d peer=%d",
                     3, agent_is_left, own, peer);
    if (agent_is_left) {
        result = (peer == APP_SMCHARGER_IN);
        result |= ((own == APP_SMCHARGER_IN) << 1);
    } else {
        result = (own == APP_SMCHARGER_IN);
        result |= ((peer == APP_SMCHARGER_IN) << 1);
    }
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
    app_in_ear_state_t in_ear_state = app_in_ear_get_state();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" Anti-lost in_ear_state=%d", 1, in_ear_state);
    if (in_ear_state == APP_IN_EAR_STA_BOTH_IN) {
        result |= (0x3 << 2);
    } else if (in_ear_state == APP_IN_EAR_STA_AIN_POUT) {
        if (agent_is_left) {
            result |= (0x2 << 2);
        } else {
            result |= (0x1 << 2);
        }
    } else if (in_ear_state == APP_IN_EAR_STA_AOUT_PIN) {
        if (agent_is_left) {
            result |= (0x1 << 2);
        } else {
            result |= (0x2 << 2);
        }
    }
#endif
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" Anti-lost result=%d", 1, result);
    return result;
}

void app_va_xiaoai_handle_chat_event(bool detect_chat)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" detect_chat %d", 1, detect_chat);
}



void app_va_xiaoai_set_device_config(void *param)
{
    xiaoai_device_config_t *config_param = (xiaoai_device_config_t *)param;
    uint8_t type = config_param->type;
    uint32_t value = config_param->value;
    void *data = config_param->data;
    uint32_t length = config_param->length;
    uint8_t app_type = config_param->app_type;
    uint8_t opcode_sn = config_param->opcode_sn;
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" device_config SET, type=%d value=%d(0x%04X) app_type=%d opcode_sn=%02X",
                     5, type, value, value, app_type, opcode_sn);

    xiaoai_mma_rsp_status rsp_status = XIAOAI_MMA_RSP_STATUS_SUCCESS;
    uint8_t opcode = 0;
    switch (type) {
        /* 0x08 bit0 - ATTR_DEVICE_POWER_MODE. */
        case XIAOAI_DEVICE_CONFIG_POWER_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
            break;
        }
        /* 0x08 bit1 - ATTR_DEVICE_FUNC_KEY. */
        case XIAOAI_DEVICE_CONFIG_FUNC_KEY: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
            break;
        }
        /* 0x08 bit2 - ATTR_DEVICE_HOTWORD. */
        case XIAOAI_DEVICE_CONFIG_HOTWORD: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
#ifdef MTK_XIAOAI_WWE_ENABLE
            bool ret = app_va_xiaoai_enable_wwe((value == 1));
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0x08 bit3 - ATTR_DEVICE_SUP_POWER_SAVE_NEW. */
        case XIAOAI_DEVICE_CONFIG_POWER_SAVE_NEW: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
            break;
        }
        /* 0x08 bit4 - ATTR_DEVICE_ANC_MODE. */
        case XIAOAI_DEVICE_CONFIG_ANC_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
#ifdef MTK_ANC_ENABLE
            uint8_t level = 0;
            if (value == XIAOAI_ANC_STATE_ENABLE
                    || value == XIAOAI_ANC_STATE_ANTI_WIND) {
                level = g_xiaoai_saving_config.anc_level;
            } else if (value == XIAOAI_ANC_STATE_PASSTHROUGH) {
                level = g_xiaoai_saving_config.pt_level;
            }
            xiaoai_anc_set_state_result ret = app_va_xiaoai_set_anc_state(value, level);
            if (ret == XIAOAI_ANC_SET_STATE_SUCCESS) {
                // Wait ANC_EVENT to update anc/pt level
                g_xiaoai_anc_target_level = level;
                rsp_status = XIAOAI_MMA_RSP_STATUS_SUCCESS;
            } else if (ret == XIAOAI_ANC_SET_STATE_IGNORE) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_SUCCESS;
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0x08 bit5 - ATTR_DEVICE_GAME_MODE. */
        case XIAOAI_DEVICE_CONFIG_GAME_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
            bool ret = app_va_xiaoai_set_game_mode(value);
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
            break;
        }
        /* 0x08 bit6 - ATTR_DEVICE_AUTO_PLAY. */
        case XIAOAI_DEVICE_CONFIG_AUTO_PLAY: {
            opcode = XIAOAI_MMA_SET_DEVICE_INFO_OPCODE;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            bool ret = app_music_set_in_ear_control((value == 0 ? APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME : APP_MUSIC_IN_EAR_DISABLE),
                                                    TRUE);
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }

        /* 0xF2 - 0001. */
        case XIAOAI_DEVICE_CONFIG_AUDIO_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            bool ret = app_va_xiaoai_apply_audio_mode(TRUE, value);
            if (ret) {
                g_xiaoai_saving_config.audio_mode = value;
                app_va_xiaoai_update_saving_config();
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
            break;
        }
        /* 0xF2 - 0002. */
        case XIAOAI_DEVICE_CONFIG_CUSTOM_KEY: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
            // app_va_xiaoai_hfp_miui_more_atcmd_report_key();
            break;
        }
        /* 0xF2 - 0003. */
        case XIAOAI_DEVICE_CONFIG_AUTO_ACCEPT_INCOMING_CALL: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            bool ret = app_hfp_set_auto_accept_incoming_call((value == 1));
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 0004. */
        case XIAOAI_DEVICE_CONFIG_MULTI_POINT_ENABLE: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef AIR_MULTI_POINT_ENABLE
            bool ret = app_bt_emp_enable((value == 1));
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 0005. */
        case XIAOAI_DEVICE_CONFIG_START_FIT_DETECTION: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
            bool ret = app_va_xiaoai_start_leakage_detection();
            if (ret) {
                g_xiaoai_ld_ongoing = TRUE;
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 0006. */
        case XIAOAI_DEVICE_CONFIG_HEADSET_FIT: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NO_RSP;
            break;
        }
        /* 0xF2 - 0007. */
        case XIAOAI_DEVICE_CONFIG_EQ_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef MTK_PEQ_ENABLE
            bool ret = app_va_xiaoai_apply_eq_mode(value);
            if (ret) {
                g_xiaoai_saving_config.eq_mode = value;
                /* ToDo MIUI HFP ATCMD EQ enum not match MMA eq_mode. */
                // app_va_xiaoai_hfp_miui_more_atcmd_report_eq();
                app_va_xiaoai_update_saving_config();
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 0008. */
        case XIAOAI_DEVICE_CONFIG_DEVICE_NAME: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            bool ret = app_va_xiaoai_agent_set_device_name(data, length);
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
            break;
        }
        /* 0xF2 - 0009. */
        case XIAOAI_DEVICE_CONFIG_FIND_ME: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            uint8_t fm_enable = (uint8_t)((value >> 8) & 0xFF);
            uint8_t fm_config = (uint8_t)(value & 0xFF);
#ifdef RACE_FIND_ME_ENABLE
            bool ret = app_find_me_do_find_me_action((bool)fm_enable, (app_find_me_config_t)fm_config);
#else
            bool ret = FALSE;
#endif
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" device_config SET, find_me %d %d",
                             2, fm_enable, fm_config);
            if (!ret) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
            break;
        }
        /* 0xF2 - 000A. */
        case XIAOAI_DEVICE_CONFIG_SWICH_ANC_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef MTK_ANC_ENABLE
            uint8_t left_anc_mode = ((value >> 8) & 0xFF);
            uint8_t right_anc_mode = (value & 0xFF);
            bool ret = app_va_xiaoai_apply_anc_key(TRUE, left_anc_mode, right_anc_mode);
            if (ret) {
                g_xiaoai_saving_config.left_anc_mode = left_anc_mode;
                g_xiaoai_saving_config.right_anc_mode = right_anc_mode;
                app_va_xiaoai_update_saving_config();
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 000B. */
        case XIAOAI_DEVICE_CONFIG_SELECT_ANC_LEVEL: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
#ifdef MTK_ANC_ENABLE
            uint8_t anc_state = ((value >> 8) & 0xFF);
            uint8_t level = (value & 0xFF);
            xiaoai_anc_set_state_result ret = app_va_xiaoai_set_anc_state(anc_state, level);
            if (ret == XIAOAI_ANC_SET_STATE_SUCCESS) {
                // Wait ANC_EVENT to update anc/pt level
                g_xiaoai_anc_target_level = level;
            } else if (ret == XIAOAI_ANC_SET_STATE_IGNORE) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_SUCCESS;
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
#else
            rsp_status = XIAOAI_MMA_RSP_STATUS_NOT_SUPPORT;
#endif
            break;
        }
        /* 0xF2 - 000C. */
        case XIAOAI_DEVICE_CONFIG_ANTI_LOST: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_NO_RSP;
            break;
        }
        /* 0xF2 - 000D. */
        case XIAOAI_DEVICE_CONFIG_CHAT_FREE_MODE: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            uint8_t chat_mode = 0;
            uint8_t chat_enable = ((value >> 8) & 0xFF);
            uint8_t chat_timer = (value & 0xFF);
            if (chat_enable == 0) {
                chat_mode = 0;
            } else if (chat_timer == 0) {
                rsp_status = XIAOAI_MMA_RSP_STATUS_PARAM_ERROR;
                break;
            } else if (chat_timer > 0) {
                chat_mode = chat_timer;
            }
            bool ret = app_va_xiaoai_apply_chat_mode(TRUE, chat_mode);
            if (ret) {
                g_xiaoai_saving_config.chat_mode = chat_mode;
                app_va_xiaoai_update_saving_config();
            } else {
                rsp_status = XIAOAI_MMA_RSP_STATUS_FAIL;
            }
            break;
        }
        /* 0xF2 - 000E. */
        case XIAOAI_DEVICE_CONFIG_HOST_APP_PACKAGE_NAME: {
            opcode = XIAOAI_MMA_SET_DEVICE_CONFIG_OPCODE;
            rsp_status = XIAOAI_MMA_RSP_STATUS_SUCCESS;
            if (data != NULL && length > 0) {
                APPS_LOG_I("[XIAOAI_DC] Host APP package name=%s\r\n", (char *)data);
            }
            break;
        }
        default:
            break;
    }
    app_va_xiaoai_device_config_reply(opcode, rsp_status, app_type, opcode_sn);
}

uint32_t app_va_xiaoai_get_device_config(uint8_t type)
{
    uint32_t result = 0;
    switch (type) {
        /* 0x09 bit4 - ATTR_GET_POWER_MODE. */
        case XIAOAI_DEVICE_CONFIG_POWER_MODE: {
            // only support normal_mode
            result = 0;
            break;
        }
        /* 0x09 bit9 - ATTR_TYPE_GET_ANC_STATUS. */
        case XIAOAI_DEVICE_CONFIG_ANC_MODE: {
#ifdef MTK_ANC_ENABLE
            result = app_va_xiaoai_get_anc_state();
#endif
            break;
        }
        /* 0x08 bit5 - ATTR_DEVICE_GAME_MODE. */
        case XIAOAI_DEVICE_CONFIG_GAME_MODE: {
            result = app_va_xiaoai_get_game_mode();
            break;
        }
        /* 0x08 bit6 - ATTR_DEVICE_AUTO_PLAY. */
        case XIAOAI_DEVICE_CONFIG_AUTO_PLAY: {
            // 0 - auto_play_when_in_ear default, 1 - not
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            uint8_t ret = app_music_get_in_ear_control_state();
            if (ret == APP_MUSIC_IN_EAR_DISABLE || ret == APP_MUSIC_IN_EAR_ONLY_AUTO_PAUSE) {
                result = 1;
            } else if (ret == APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME ) {
                result = 0;
            }
#else
            result = 1;
#endif
            break;
        }

        /* 0xF3 - 0001. */
        case XIAOAI_DEVICE_CONFIG_AUDIO_MODE: {
            result = g_xiaoai_saving_config.audio_mode;
            break;
        }
        /* 0xF3 - 0002. */
        case XIAOAI_DEVICE_CONFIG_CUSTOM_KEY: {
            // TODO
            break;
        }
        /* 0xF3 - 0003. */
        case XIAOAI_DEVICE_CONFIG_AUTO_ACCEPT_INCOMING_CALL: {
            // 0 - OFF, 1 - ON, -1 - not support
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            result = (app_hfp_is_auto_accept_incoming_call() == APP_HFP_AUTO_ACCEPT_ENABLE);
#else
            result = -1;
#endif
            break;
        }
        /* 0xF3 - 0004. */
        case XIAOAI_DEVICE_CONFIG_MULTI_POINT_ENABLE: {
#ifdef AIR_MULTI_POINT_ENABLE
            result = app_bt_emp_is_enable();
#else
            result = -1;
#endif
            break;
        }
        /* 0xF3 - 0005 N/A. */
        case XIAOAI_DEVICE_CONFIG_START_FIT_DETECTION: {
            result = 0;
            break;
        }
        /* 0xF3 - 0006. */
        case XIAOAI_DEVICE_CONFIG_HEADSET_FIT: {
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
            result = ((g_xiaoai_leakage_result.left_result << 8)
                            | g_xiaoai_leakage_result.right_result);
#endif
            break;
        }
        /* 0xF3 - 0007. */
        case XIAOAI_DEVICE_CONFIG_EQ_MODE: {
            result = app_va_xiaoai_get_eq_mode();
            break;
        }
        /* 0xF3 - 0008. */
        case XIAOAI_DEVICE_CONFIG_DEVICE_NAME: {
            uint8_t* name = app_va_xiaoai_get_device_name();
            if (name != NULL) {
                result = (uint32_t)name;
            }
            break;
        }
        /* 0xF3 - 0009 N/A. */
        case XIAOAI_DEVICE_CONFIG_FIND_ME: {
            result = 0;
            break;
        }
        /* 0xF3 - 000A. */
        case XIAOAI_DEVICE_CONFIG_SWICH_ANC_MODE: {
#ifdef MTK_ANC_ENABLE
            result = ((g_xiaoai_saving_config.left_anc_mode << 8)
                            | g_xiaoai_saving_config.right_anc_mode);
#endif
            break;
        }
        /* 0xF3 - 000B. */
        case XIAOAI_DEVICE_CONFIG_SELECT_ANC_LEVEL: {
#ifdef MTK_ANC_ENABLE
            result = app_va_xiaoai_get_anc_state();
            if (result == XIAOAI_ANC_STATE_ENABLE
                    || result == XIAOAI_ANC_STATE_ANTI_WIND) {
                result = ((g_xiaoai_saving_config.anc_state << 8) | g_xiaoai_saving_config.anc_level);
            } else if (result == XIAOAI_ANC_STATE_PASSTHROUGH) {
                result = ((g_xiaoai_saving_config.anc_state << 8) | g_xiaoai_saving_config.pt_level);
            } else {
                result = 0;
            }
#endif
            break;
        }
        /* 0xF3 - 000C. */
        case XIAOAI_DEVICE_CONFIG_ANTI_LOST: {
            result = app_va_xiaoai_get_anti_lost_state();
            break;
        }
        /* 0xF3 - 000D N/A. */
        case XIAOAI_DEVICE_CONFIG_CHAT_FREE_MODE: {
            if (g_xiaoai_saving_config.chat_mode == 0) {
                result = 0;
            } else {
                result = ((1 << 8) | g_xiaoai_saving_config.chat_mode);
            }
            break;
        }
        /* 0xF3 - 000E N/A. */
        case XIAOAI_DEVICE_CONFIG_HOST_APP_PACKAGE_NAME: {
            result = 0;
            break;
        }
        default:
            break;
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" device_config GET, type=%d result=%d(0x%04X)",
                     3, type, result, result);
    return result;
}

void app_va_xiaoai_notify_device_config(uint8_t type, uint8_t *data, uint8_t len)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" device_config Notify, not agent role", 0);
        return;
    }

    uint16_t f4_type = 0;
    uint8_t f4_len = 2 + len;
    if (type == XIAOAI_DEVICE_CONFIG_HEADSET_FIT) {
        f4_type = 0x0006;
    } else if (type == XIAOAI_DEVICE_CONFIG_EQ_MODE) {
        f4_type = 0x0007;
    } else if (type == XIAOAI_DEVICE_CONFIG_SWICH_ANC_MODE) {
        f4_type = 0x000A;
    } else if (type == XIAOAI_DEVICE_CONFIG_SELECT_ANC_LEVEL) {
        f4_type = 0x000B;
    } else if (type == XIAOAI_DEVICE_CONFIG_ANTI_LOST) {
        f4_type = 0x000C;
    } else {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" device_config Notify, not_support type=%d",
                         1, type);
        return;
    }

    uint8_t *buf = (uint8_t *)pvPortMalloc(1 + f4_len);
    if (buf == NULL) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_DC_TAG" device_config Notify, malloc fail", 0);
        return;
    }

    buf[0] = f4_len;
    buf[1] = (uint8_t)(f4_type >> 8);
    buf[2] = (uint8_t)(f4_type & 0xFF);
    memcpy(buf + 3, data, len);

    bool ret = xiaoai_send_mma_cmd(XIAOAI_MMA_NOTIFY_DEVICE_INFO_OPCODE, buf, 1 + f4_len);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" device_config Notify, ret=%d type=%d f4_len=%d %02X:%02X:%02X:%02X:%02X",
                     8, ret, type, f4_len, buf[0], buf[1], buf[2], buf[3], buf[4]);
    vPortFree(buf);
}

void app_va_xiaoai_init_device_config()
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    xiaoai_saving_config_t config = {0};
    uint32_t size = sizeof(xiaoai_saving_config_t);
    nvkey_status_t status = nvkey_read_data(NVKEYID_XIAOAI_KEY_CONFIG_INFO, (uint8_t *)&config, &size);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" [%02X] init, read status=%d", 1, status);
    if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
        g_xiaoai_saving_config.wwe_enable = 0;
        g_xiaoai_saving_config.eq_mode = 0;
        g_xiaoai_saving_config.left_anc_mode = 0x7;     // bit 111
        g_xiaoai_saving_config.right_anc_mode = 0x7;
        g_xiaoai_saving_config.anc_state = 0;
        g_xiaoai_saving_config.anc_level = 0;
        g_xiaoai_saving_config.pt_level = 0;
        g_xiaoai_saving_config.audio_mode = XIAOAI_AUDIO_MODE_NORMAL;
        g_xiaoai_saving_config.chat_mode = 0;

        size = sizeof(xiaoai_saving_config_t);
        status = nvkey_write_data(NVKEYID_XIAOAI_KEY_CONFIG_INFO, (const uint8_t *)&g_xiaoai_saving_config, size);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" init, write status=%d", 1, status);
    } else if (status == NVKEY_STATUS_OK) {
        bool ret = FALSE;
        app_va_xiaoai_print_saving_config(&config);
#ifdef MTK_XIAOAI_WWE_ENABLE
        if (config.wwe_enable) {
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_XIAOAI,
                                XIAOAI_EVENT_START_WWE_ACTION,
                                NULL, 0, NULL, 0);
            g_xiaoai_saving_config.wwe_enable = config.wwe_enable;
        }
#endif
#ifdef MTK_ANC_ENABLE
        ret = app_va_xiaoai_apply_anc_key(FALSE, config.left_anc_mode, config.right_anc_mode);
        if (ret) {
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_DC_TAG" init, update_anc_key PASS", 0);
            g_xiaoai_saving_config.left_anc_mode = config.left_anc_mode;
            g_xiaoai_saving_config.right_anc_mode = config.right_anc_mode;
        }
        g_xiaoai_saving_config.anc_level = config.anc_level;
        g_xiaoai_saving_config.pt_level = config.pt_level;
#endif
        ret = app_va_xiaoai_apply_audio_mode(FALSE, config.audio_mode);
        if (ret) {
            g_xiaoai_saving_config.audio_mode = config.audio_mode;
        }
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            ret = app_va_xiaoai_apply_chat_mode(FALSE, config.chat_mode);
            if (ret) {
                g_xiaoai_saving_config.chat_mode = config.chat_mode;
            }
        } else {
            g_xiaoai_saving_config.chat_mode = config.chat_mode;
        }
        g_xiaoai_saving_config.eq_mode = config.eq_mode;
    }
}

#endif /* MTK_VA_XIAOAI_ENABLE */
