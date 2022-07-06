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
 * File: app_preproc_activity.c
 *
 * Description: This activity is used to pre-process events before other activities.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_preproc_activity.h"
#include "apps_config_led_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_app_common.h"
#include "apps_debug.h"
#include "apps_config_key_remapper.h"
#include "apps_config_vp_manager.h"
#include "apps_events_key_event.h"
#include "app_preproc_sys_pwr.h"
#include "app_bt_state_service.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "apps_race_cmd_event.h"
#endif
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_aws_sync_event.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "bt_device_manager.h"
#include "apps_control_touch_key_status.h"
#ifdef AIR_RACE_SCRIPT_ENABLE
#include "race_cmd_script.h"
#endif
extern void app_detachable_mic_det_init();
extern void app_switch_mic();

static bool _proc_ui_shell_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = true;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            app_detachable_mic_det_init();
#endif
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I("app_preproc_activity destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I("app_preproc_activity resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I("app_preproc_activity pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I("app_preproc_activity refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I("app_preproc_activity result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool pre_proc_key_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    if (extra_data) {
        uint8_t key_id;
        airo_key_event_t key_event;

#ifdef AIR_RACE_SCRIPT_ENABLE
        race_script_key_notify();
#endif

        /* The pointer is from p_key_action in apps_events_key_event.c, content is same as s_press_from_power_on. */
        uint16_t *p_key_action = (uint16_t *)extra_data;
        if (INVALID_KEY_EVENT_ID == event_id) {
            /* Key event from CMD, not real key. */
            APPS_LOG_MSGID_I("Receive CMD key event, action: %04x", 1, *p_key_action);
            return false;
        }
        /* The key is from power on, ignore it. */
        if (*p_key_action) {
            APPS_LOG_MSGID_I("The key pressed from power on, do special %04x", 1, event_id);
            return true;
        }
        /* The extra_data in the key event is valid key_action. */
        app_event_key_event_decode(&key_id, &key_event, event_id);
        *p_key_action = apps_config_key_event_remapper_map_action(key_id, key_event);

        if (key_event == AIRO_KEY_PRESS) {
            apps_config_set_vp_preempt(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_HIGH, NULL);
        }

        /* Control captouch for bisto*/
#if defined(GSOUND_LIBRARY_ENABLE) && defined(MTK_RACE_CMD_ENABLE)
        uint8_t temp_touch_key_status = 0;
        temp_touch_key_status = apps_get_touch_control_status();
        APPS_LOG_MSGID_I("App_pre_pro get touch_key_status=0x%02X", 1, temp_touch_key_status);
        if (((0 == temp_touch_key_status) || (0xFF == temp_touch_key_status))
                && ((DEVICE_KEY_A == key_id) || (DEVICE_KEY_B == key_id) || (DEVICE_KEY_C == key_id) || (DEVICE_KEY_D == key_id))) {
            APPS_LOG_MSGID_I("App_pre_pro touch key is disable: 0x02X, key_id=%x, key_event=%x",
                    3, temp_touch_key_status, key_id, key_event);
            if ((AIRO_KEY_SHORT_CLICK == key_event) || (AIRO_KEY_DOUBLE_CLICK == key_event) || (AIRO_KEY_TRIPLE_CLICK == key_event)) {
                apps_config_set_vp(VP_INDEX_FAILED, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            return true;
        } else {
            APPS_LOG_MSGID_I("App_pre_pro touch key is enable: touch_key_status=%x, key_id=%x, key_event=%x.",
                    3, temp_touch_key_status, key_id, key_event);
        }
#endif
    }
    return false;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
static bool s_rotary_1_control_mix = false;
static bool pre_proc_rotary_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    if (extra_data) {
        bsp_rotary_encoder_port_t port;
        bsp_rotary_encoder_event_t event;
        uint32_t rotary_data;
        uint16_t *p_key_action = (uint16_t *)extra_data;

        app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);
        if (BSP_ROTARY_ENCODER_0 == port) {
            /* ENCODER_0 is for volume change.*/
            if (BSP_ROTARY_ENCODER_EVENT_CW == event) {
                *p_key_action = KEY_VOICE_UP;
            } else {
                *p_key_action = KEY_VOICE_DN;
            }
        } else if (BSP_ROTARY_ENCODER_1 == port && s_rotary_1_control_mix) {
            /* ENCODER_0 is for mix ratio and side tone. */
            if (BSP_ROTARY_ENCODER_EVENT_CW == event) {
                *p_key_action = KEY_AUDIO_MIX_RATIO_CHAT_ADD;
            } else {
                *p_key_action = KEY_AUDIO_MIX_RATIO_GAME_ADD;
            }
        } else if (BSP_ROTARY_ENCODER_1 == port) {
            if (BSP_ROTARY_ENCODER_EVENT_CW == event) {
                *p_key_action = KEY_AUDIO_SIDE_TONE_VOLUME_UP;
            } else {
                *p_key_action = KEY_AUDIO_SIDE_TONE_VOLUME_DOWN;
            }
        } else {
            *p_key_action = KEY_ACTION_INVALID;
        }
    } else {
        ret = true;
    }
    return ret;
}
#endif

static bool pre_proc_app_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        /* Increase BLE ADV interval and restart advertise, see bt_app_common_ble_adv_timer_hdlr in bt_app_common.c. */
        case APPS_EVENTS_INTERACTION_INCREASE_BLE_ADV_INTERVAL:
            bt_app_common_trigger_increase_ble_adv();
            ret = true;
            break;
        /* Reload key_remaper when NVKEY changed, see bt_race_reload_nvkey_event_callback in apps_events_bt_event.c. */
        case APPS_EVENTS_INTERACTION_RELOAD_KEY_ACTION_FROM_NVKEY:
            apps_config_key_remaper_init_configurable_table();
            ret = true;
            break;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
        case APPS_EVENTS_INTERACTION_SWITCH_MIC:
            app_switch_mic();
            break;
#endif
        case APPS_EVENTS_INTERACTION_PLAY_VP: {
            uint16_t vp_index = (uint16_t)(uint32_t)extra_data;
            apps_config_set_vp(vp_index, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            ret = true;
            break;
        }
#ifdef AIR_TILE_ENABLE /* Test CMD with tile features. */
        case APPS_EVENTS_INTERACTION_TEST_BLE_ADV: {
            bool on_off = (bool)extra_data;
            apps_events_test_event_ble_adv_start(on_off);
            ret = true;

            break;
        }
#endif
        case APPS_EVENTS_INTERACTION_SET_LED: {
            typedef struct {
                uint8_t type;
                uint8_t index;
                uint16_t timeout;
            } __attribute__((packed)) LED_DATA;
            LED_DATA *led_data = (LED_DATA *)(&extra_data);
            APPS_LOG_MSGID_I("app_preproc APPS_EVENTS_INTERACTION_SET_LED type %d, index %d, timeout %d", 3,
                    led_data->type, led_data->index, led_data->timeout);
            if (led_data->type == 0x01) {
                /* Foreground. */
                apps_config_set_foreground_led_pattern(led_data->index, led_data->timeout, false);
            } else if (led_data->type == 0x02) {
                /* Background. */
                apps_config_set_background_led_pattern(led_data->index, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGHEST);
            }
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}


static bool pre_proc_led_manager_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        /* Check and disable foreground LED when timeout, see _led_fg_time_out_callback in apps_config_led_manager.c. */
        case APPS_EVENTS_LED_FG_PATTERN_TIMEOUT:
            apps_config_check_foreground_led_pattern();
            ret = true;
            break;
#ifdef MTK_AWS_MCE_ENABLE
        /* Sync LED pattern info from Agent or Partner, then handle in UI Shell task, see app_led_sync_callback in apps_config_led_manager.c. */
        case APPS_EVENTS_LED_SYNC_LED_PATTERN:
            app_config_led_sync(extra_data);
            ret = true;
            break;
#endif
        default:
            break;
    }

    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_pre_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action,
                &p_extra_data, &extra_data_len);
        APPS_LOG_MSGID_I("App_pre_pro Received action=%x to sync touch key status, role=%x, event_group=%x",
                3, action, role, event_group);
        if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION && action == APPS_EVENTS_INTERACTION_SYNC_TOUCH_KEY_STATUS) {
            if (BT_AWS_MCE_ROLE_PARTNER == role) {
                uint8_t touch_key_status = *(uint8_t *)p_extra_data;
                APPS_LOG_MSGID_I("App_pre_pro partner receive touch_key_status=%x", 1, touch_key_status);
                nvkey_status_t status = 0;
                if (NVKEY_STATUS_OK != (status = nvkey_write_data(NVKEYID_APP_RACE_CMD_TOUCH_KEY_ENABLE,
                        (uint8_t *)&touch_key_status, sizeof(uint8_t)))) {
                    APPS_LOG_MSGID_I("App_pre_pro partner write touch_key_status into nvkey failed : 0x%x", 1, status);
                    return ret;
                } else {
                    APPS_LOG_MSGID_I("App_pre_pro partner write touch_key_status into nvkey success. ", 0);
                    ret = true;
                }
            }
        }
    }
    return ret;
}
#endif

bool app_preproc_activity_proc(
    ui_shell_activity_t *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    /* APPS_LOG_MSGID_I("pre-proc receive event_group=%d event_id=0x%08x", 2, event_group, event_id); */

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell APP interaction events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = pre_proc_app_interaction_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell key events. */
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = pre_proc_key_event_proc(self, event_id, extra_data, data_len);
            break;
#ifdef AIR_ROTARY_ENCODER_ENABLE
        /* UI Shell rotary encoder events. */
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            ret = pre_proc_rotary_event_proc(self, event_id, extra_data, data_len);
            break;
        }
#endif
        /* UI Shell BT Sink events, see bt_app_common.c. */
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            ret = bt_app_common_sink_event_proc(event_id, extra_data, data_len);
            break;
        /* UI Shell BT CM events, see bt_app_common.c. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = bt_app_common_cm_event_proc(event_id, extra_data, data_len);
            break;
        /* UI Shell GSound events, see bt_app_common.c. */
        case EVENT_GROUP_UI_SHELL_GSOUND:
            ret = bt_app_common_gsound_event_proc(event_id, extra_data, data_len);
            break;
        /* UI Shell LED manager events. */
        case EVENT_GROUP_UI_SHELL_LED_MANAGER:
            ret = pre_proc_led_manager_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell system_power events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM_POWER:
            //ret = sys_pwr_component_event_proc(self, event_id, extra_data, data_len);
            break;
#if (defined(MTK_AWS_MCE_ENABLE) && defined(MTK_RACE_CMD_ENABLE))
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            /* Handle the touch key state sync from the agent side. */
            ret = app_pre_proc_aws_data(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }
    if (!ret) {
        /* Handle again if ret is not TRUE, see app_bt_state_service.c. */
        ret = app_bt_state_service_process_events(event_group, event_id, extra_data, data_len);
    }
    return ret;
}
