
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
 * File: app_va_xiaoai_hfp_at_cmd.c
 *
 * Description: This file provides XiaoAI HFP AT CMD for XiaoAI activity.
 *
 */

#ifdef MTK_VA_XIAOAI_ENABLE

#include "app_va_xiaoai_hfp_at_cmd.h"

#include "apps_config_key_remapper.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "app_va_xiaoai_config.h"
#include "bt_app_common.h"
#include "bt_callback_manager.h"
#include "bt_device_manager.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_hf.h"
#include "FreeRTOS.h"

#include "xiaoai.h"
#include "app_va_xiaoai_device_config.h"

#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#endif
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

#define APP_VA_XIAOAI_HFP_TAG           "[XIAOAI_HF]"

#define APP_XIAOMI_HFP_ATCMD            "+XIAOMI: "
#define APP_XIAOMI_HFP_ATCMD_LEN        (strlen(APP_XIAOMI_HFP_ATCMD))
#define APP_XIAOMI_HFP_HEADER_LEN       10 // <FF><01><02><01><01/03>

#define XIAOAI_HFP_MORE_ATCMD_MAX_LEN   200

static uint32_t g_xiaoai_hfp_handle = 0;


typedef enum {
    XIAOAI_HFP_ATCMD_ATT_STATUS = 1,
    XIAOAI_HFP_ATCMD_ATT_CONFIG = 3
} xiaoai_hfp_miui_attribute;

typedef enum {
    XIAOAI_HFP_ATCMD_TYPE_REPORT = 0x00,
    XIAOAI_HFP_ATCMD_TYPE_RUN_INFO_CHANGED = 0x03,

    XIAOAI_HFP_ATCMD_TYPE_FLAG = 0x01,
    XIAOAI_HFP_ATCMD_TYPE_FEATURE = 0x20,
    XIAOAI_HFP_ATCMD_TYPE_ANC = 0x04,
    XIAOAI_HFP_ATCMD_TYPE_KEY = 0x05,
    XIAOAI_HFP_ATCMD_TYPE_VOICE = 0x06,
    XIAOAI_HFP_ATCMD_TYPE_EQ = 0x07,
    XIAOAI_HFP_ATCMD_TYPE_GAME_MODE = 0x08,
    XIAOAI_HFP_ATCMD_TYPE_ANTI_LOST = 0x0A
} xiaoai_hfp_atcmd_status_type;

typedef enum {
    XIAOAI_HFP_ATCMD_TYPE_CONFIG_NAME = 0x01,
    XIAOAI_HFP_ATCMD_TYPE_CONFIG_LEAUDIO = 0x04
} xiaoai_hfp_atcmd_config_type;

typedef enum {
    XIAOAI_KEY_ANC = 0,
    XIAOAI_KEY_VA,
    XIAOAI_KEY_GAME_MODE,
    XIAOAI_KEY_PREV_MUSIC,
    XIAOAI_KEY_NEXT_MUSIC,
    XIAOAI_KEY_VOL_UP,
    XIAOAI_KEY_VOL_DOWN,
    XIAOAI_KEY_PALY_PAUSE
} xiaoai_key_function;

static xiaoai_key_function app_va_xiaoai_key_click_function(bool left_earbud, airo_key_event_t key_event)
{
    xiaoai_key_function key_function = XIAOAI_KEY_ANC;
    apps_config_state_t mmi_state = apps_config_key_get_mmi_state();
    apps_config_key_action_t key_action = apps_config_key_get_mapper_action(
            DEVICE_KEY_POWER, key_event, mmi_state, left_earbud);
    if (key_action == KEY_VA_XIAOAI_START || key_action == KEY_VA_XIAOAI_START_NOTIFY
            || (key_action >= KEY_WAKE_UP_VOICE_ASSISTANT && key_action <= KEY_WAKE_UP_VOICE_ASSISTANT_NOTIFY)) {
        key_function = XIAOAI_KEY_VA;
    } else if (key_action == KEY_AVRCP_PLAY || key_action == KEY_AVRCP_PAUSE) {
        key_function = XIAOAI_KEY_PALY_PAUSE;
    } else if (key_action == KEY_AVRCP_BACKWARD) {
        key_function = XIAOAI_KEY_PREV_MUSIC;
    } else if (key_action == KEY_AVRCP_FORWARD) {
        key_function = XIAOAI_KEY_NEXT_MUSIC;
    } else if (key_action == KEY_VOICE_UP) {
        key_function = XIAOAI_KEY_VOL_UP;
    } else if (key_action == KEY_VOICE_DN) {
        key_function = XIAOAI_KEY_VOL_DOWN;
    } else if (key_action == KEY_PASS_THROUGH || key_action == KEY_ANC
            || key_action == KEY_SWITCH_ANC_AND_PASSTHROUGH) {
        key_function = XIAOAI_KEY_ANC;
    } else if (key_action == KEY_GAMEMODE_ON || key_action == KEY_GAMEMODE_OFF
            || key_action == KEY_GAMEMODE_TOGGLE) {
        key_function = XIAOAI_KEY_GAME_MODE;
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" double_click left=%d %d->%d",
                     3, left_earbud, key_action, key_function);
    return key_function;
}

typedef enum {
    XIAOAI_KEY_ANC_OFF_ON_PT = 0,  // OFF, ANC_ON, PT_ON
    XIAOAI_KEY_ANC_ON_PT,          // ANC_ON, PT_ON
    XIAOAI_KEY_ANC_OFF_ON,         // OFF, ANC_ON
    XIAOAI_KEY_ANC_OFF_PT,         // OFF, PT_ON
    XIAOAI_KEY_ANC_OFF             // OFF
} xiaoai_key_anc_function;

// Customer configure option: need to implement ANC switch Key
static xiaoai_key_anc_function app_va_xiaoai_key_anc_function(bool left_earbud)
{
    xiaoai_key_anc_function key_anc_function = 0;
    return key_anc_function;
}

static bool app_va_xiaoai_send_hfp_atcmd(uint8_t miui_attribute, uint8_t *data, uint8_t data_len)
{
    bool ret = FALSE;
    char *atcmd = NULL;
    char *param = NULL;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" send_hfp_atcmd role=%02X miui_attribute=%d data=0x%08X data_len=%d",
                     4, role, miui_attribute, data, data_len);

    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_HFP_TAG" send_hfp_atcmd, role fail", 0);
        goto exit;
    } else if ((miui_attribute != XIAOAI_HFP_ATCMD_ATT_STATUS
                && miui_attribute != XIAOAI_HFP_ATCMD_ATT_CONFIG)
                || data == NULL || data_len == 0) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_HFP_TAG" send_hfp_atcmd, parameter fail", 0);
        goto exit;
    }

    atcmd = (char *)pvPortMalloc(XIAOAI_HFP_MORE_ATCMD_MAX_LEN);
    param = (char *)pvPortMalloc(data_len * 2 + 1);
    if (atcmd == NULL || param == NULL) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_HFP_TAG" send_hfp_atcmd, malloc fail", 0);
        goto exit;
    }
    memset(atcmd, 0, XIAOAI_HFP_MORE_ATCMD_MAX_LEN);
    memset(param, 0, data_len * 2 + 1);

    for (int i = 0; i < data_len; i++) {
        char temp[3] = {0};
        snprintf(temp, 3, "%02X", data[i]);
        param[i * 2] = temp[0];
        param[i * 2 + 1] = temp[1];
    }

    // AT+XIAOMI=<FF> <01><02><01> <01/03> <length><type><data(LTV List)>... <FF>
    snprintf(atcmd, XIAOAI_HFP_MORE_ATCMD_MAX_LEN, "FF010201%02X%sFF", miui_attribute, param);

    int atcmd_len = strlen(atcmd);
    APPS_LOG_I("[XIAOAI_HF] HFP ATCMD=%s atcmd_len=%d\r\n", (char *)atcmd, atcmd_len);

    if (atcmd != NULL && atcmd_len > APP_XIAOMI_HFP_HEADER_LEN) {
        bt_status_t status = bt_sink_srv_hf_xiaomi_custom(atcmd, atcmd_len);
        ret = (status == BT_STATUS_SUCCESS);
    }

exit:
    if (atcmd != NULL) {
        vPortFree(atcmd);
    }
    if (param != NULL) {
        vPortFree(param);
    }
    return ret;
}

static void app_va_xiaoai_handle_get_status(const char *cmd)
{
    uint8_t len = 0;
    uint8_t type = 0;
    uint8_t flag = 0; // 0 - use current value, 1 - re-detect and use latest value
    int n = sscanf(cmd, "%02x%02x%02x", (unsigned int *)&len, (unsigned int *)&type, (unsigned int *)&flag);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_get_status, n=%d len=%d type=%d flag=%d",
                     4, n, len, type, flag);
    if (n == 3 && len == 2) {
        switch (type) {
            case 0: {
                // all
                app_va_xiaoai_hfp_miui_more_atcmd_report_all_status();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_FLAG: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_feature();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_ANC: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_anc();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_KEY: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_key();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_VOICE: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_voice();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_EQ: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_eq();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_GAME_MODE: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_game_mode();
                break;
            }
            case XIAOAI_HFP_ATCMD_TYPE_ANTI_LOST: {
                app_va_xiaoai_hfp_miui_more_atcmd_report_anti_lost();
                break;
            }
            default: {
                APPS_LOG_MSGID_E(APP_VA_XIAOAI_HFP_TAG" handle_get_status error", 0);
                break;
            }
        }
    }
}

static void app_va_xiaoai_handle_config_request(const char *cmd)
{
    uint8_t len = 0;
    uint8_t type = 0;
    int n = sscanf(cmd, "%02x%02x", (unsigned int *)&len, (unsigned int *)&type);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_config_request len=%d type=%d",
                     2, len, type);

    if (n == 2 && len >= 2) {
        if (type == XIAOAI_HFP_ATCMD_TYPE_CONFIG_NAME) {
            const char *name = cmd + 4;
            len = len - 1;  // <len><type><name_string>, <type> only 1 bytes
            // <len><type>=2 + 1 '\0'
            uint8_t *report_data = (uint8_t *)pvPortMalloc(2 + XIAOAI_BT_NAME_LEN + 1);
            if (report_data != NULL) {
                memset(report_data, 0, 2 + XIAOAI_BT_NAME_LEN + 1);
                uint8_t copy_len = (len >= XIAOAI_BT_NAME_LEN ? XIAOAI_BT_NAME_LEN : len);
                report_data[0] = copy_len + 1;
                report_data[1] = XIAOAI_HFP_ATCMD_TYPE_CONFIG_NAME;
                memcpy(report_data + 2, name, copy_len);
                bool ret = app_va_xiaoai_agent_set_device_name(report_data + 2, copy_len);
                if (ret) {
                    ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_CONFIG,
                                                       report_data, copy_len + 2);
                    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_set_name, send rsp ret=%d", 1, ret);
                }
                vPortFree(report_data);
            } else {
                APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_set_name, malloc fail", 0);
            }
        } else if (type == XIAOAI_HFP_ATCMD_TYPE_CONFIG_LEAUDIO) {
            uint8_t le_audio_switch = *(cmd + 5);
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_enable_leaudio, %d", 1, le_audio_switch);
            bool enable = (le_audio_switch == 1);
#ifdef AIR_LE_AUDIO_ENABLE
            if (enable) {
#ifdef AIR_MULTI_POINT_ENABLE
                // disconnect other edr if existing
                bt_bd_addr_t *bt_bd_addr = bt_hfp_get_bd_addr_by_handle(g_xiaoai_hfp_handle);
                const uint8_t *addr = (const uint8_t *)*bt_bd_addr;
                app_bt_emp_force_disable(addr);
#endif
            }
            app_le_audio_enable(enable);
#endif
            uint8_t report_data[3] = {0x02, XIAOAI_HFP_ATCMD_TYPE_CONFIG_LEAUDIO,
                                      le_audio_switch};
            bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_CONFIG,
                                                    report_data, 3);
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" handle_enable_leaudio, send rsp ret=%d", 1, ret);
        }
    }
}

bt_status_t app_va_xiaoai_hfp_callback(bt_msg_type_t event, bt_status_t status, void *param)
{
    switch (event) {
        case BT_HFP_CUSTOM_COMMAND_RESULT_IND: {
            bt_hfp_custom_command_result_ind_t *ind = (bt_hfp_custom_command_result_ind_t *)param;
            const char *atcmd = ind->result;
            // +XIAOMI: <FF><01><02><01><01/03>"data"<FF>
            if (status == BT_STATUS_SUCCESS && atcmd != NULL) {
                g_xiaoai_hfp_handle = ind->handle;
                int atcmd_len = strlen(atcmd) - APP_XIAOMI_HFP_ATCMD_LEN - APP_XIAOMI_HFP_HEADER_LEN;
                if (atcmd_len > 0
                        && strstr(atcmd, APP_XIAOMI_HFP_ATCMD) > 0) {
                    atcmd += APP_XIAOMI_HFP_ATCMD_LEN + APP_XIAOMI_HFP_HEADER_LEN - 2;
                    uint8_t type = 0;
                    sscanf(atcmd, "%02x", (unsigned int *)&type);
                    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" hfp_callback type=%d", 1, type);
                    // TODO <03><00>?
                    if (type == XIAOAI_HFP_ATCMD_ATT_STATUS) {
                        app_va_xiaoai_handle_get_status(atcmd + 2);
                    } else if (type == XIAOAI_HFP_ATCMD_ATT_CONFIG) {
                        app_va_xiaoai_handle_config_request(atcmd + 2);
                    }
                }
            } else {
                APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" hfp_callback error, status=0x%08X atcmd=%d",
                                 2, status, atcmd);
            }
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}





bool app_va_xiaoai_hfp_miui_basic_atcmd()
{
    bool ret = FALSE;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_HFP_TAG" hfp_miui_basic_atcmd, role fail", 0);
        return ret;
    }

    char *at_cmd = xiaoai_get_miui_at_cmd();
    if (at_cmd != NULL) {
        bt_status_t status = bt_sink_srv_hf_xiaomi_custom(at_cmd, strlen(at_cmd));
        vPortFree(at_cmd);
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" hfp_miui_basic_atcmd", 0);
        ret = (status == BT_STATUS_SUCCESS);
    }
    return ret;
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_feature()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" miui_more_atcmd_report_feature, not Agent", 0);
        return;
    }

    #define XIAOMI_HFP_MORE_ATCMD_REPORT_FEATURE_LEN             19
    uint8_t report_data[XIAOMI_HFP_MORE_ATCMD_REPORT_FEATURE_LEN] = {0};

    // bit0 - support upgrade (Yes)
    // bit1 - support anti_lost (Yes)
    // bit2 - support ear detection (Yes)
    // bit3 - support audio mode (No)      // Customer configure option: Audio mode?
    // bit4 - support single ear mode (No)
    // bit5 - support auto ack (Yes)
    // bit6 - support multi connect (Yes)
    // bit7 - support compactness (Yes)
    uint8_t support_feature_flag1 = 0xE7;

    // bit0 - support double click left headset (Yes)
    // bit1 - support double click right headset (Yes)
    // bit2 - support triple click left headset (Yes)
    // bit3 - support triple click right headset (Yes)
    // bit4 - support long press left headset (Yes)
    // bit5 - support long press right headset (Yes)
    // bit6 - support rename headset (Yes)
    // bit7 - support Reversed (No)
    uint8_t support_feature_flag2 = 0x7F;

    // bit0 - support ANC (Yes)
    // bit1 - support voiceprint recognition (Yes)
    // bit2 - support game mode (Yes)
    // bit3 - support voice command training (No)
    // bit4 - support equalizer (Yes)
    // bit5 - Reversed
    // bit6 - Reversed
    // bit7 - Reversed
    uint8_t support_feature_flag3 = 0x17;

    // pack report data
    report_data[0] = 0x12;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x04;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_FLAG;
    report_data[4] = support_feature_flag1;
    report_data[5] = support_feature_flag2;
    report_data[6] = support_feature_flag3;
    report_data[7] = 0x0B;
    report_data[8] = XIAOAI_HFP_ATCMD_TYPE_FEATURE;
    report_data[9] = 0x01;
    // 0 --- MIUI BLE ADV
    // 1 --- MIOT BLE ADV
    // 2 --- Fast Pair with same account
    report_data[10] = 0x01;
    report_data[11] = 0x01;
    report_data[12] = APP_VA_XIAOAI_MAJOR_ID;
    report_data[13] = (APP_VA_XIAOAI_MINOR_ID & 0xFF);
    report_data[14] = ((APP_VA_XIAOAI_MINOR_ID >> 8) & 0xFF);
    report_data[15] = (APP_VA_XIAOAI_VERSION & 0x0F);
    report_data[16] = ((APP_VA_XIAOAI_VERSION >> 4) & 0x0F);
    report_data[17] = ((APP_VA_XIAOAI_VERSION >> 8) & 0x0F);
    report_data[18] = ((APP_VA_XIAOAI_VERSION >> 12) & 0x0F);

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOMI_HFP_MORE_ATCMD_REPORT_FEATURE_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" miui_more_atcmd_report_feature ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_all_status()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_all_status, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_ALL_STATUS_LEN      (0x1B + 1)
    uint8_t report_data[XIAOAI_HFP_REPORT_ALL_STATUS_LEN] = {0};
    report_data[0] = 0x1B;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_RUN_INFO_CHANGED;
    uint8_t anc_state = app_va_xiaoai_get_anc_state_for_miui();
    uint8_t eq_state = app_va_xiaoai_get_eq_mode();
    uint8_t game_mode_state = app_va_xiaoai_get_game_mode_for_miui();
    uint8_t anti_lost_state = app_va_xiaoai_get_anti_lost_state();
    report_data[2] = 0x02;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_ANC;
    report_data[4] = anc_state;
    report_data[5] = 0x07;
    report_data[6] = XIAOAI_HFP_ATCMD_TYPE_KEY;
    report_data[7] = app_va_xiaoai_key_click_function(TRUE, AIRO_KEY_DOUBLE_CLICK);
    report_data[8] = app_va_xiaoai_key_click_function(FALSE, AIRO_KEY_DOUBLE_CLICK);
    report_data[9] = app_va_xiaoai_key_click_function(TRUE, AIRO_KEY_TRIPLE_CLICK);
    report_data[10] = app_va_xiaoai_key_click_function(FALSE, AIRO_KEY_TRIPLE_CLICK);
    report_data[11] = app_va_xiaoai_key_anc_function(TRUE);
    report_data[12] = app_va_xiaoai_key_anc_function(FALSE);
    report_data[13] = 0x05;
    report_data[14] = XIAOAI_HFP_ATCMD_TYPE_VOICE;
    report_data[15] = app_va_xiaoai_is_enable_wwe();
    report_data[16] = app_va_xiaoai_get_voice_recognition_state();
    report_data[17] = 0x1F;
    report_data[18] = 0;
    report_data[19] = 0x02;
    report_data[20] = XIAOAI_HFP_ATCMD_TYPE_EQ;
    report_data[21] = eq_state;
    report_data[22] = 0x02;
    report_data[23] = XIAOAI_HFP_ATCMD_TYPE_GAME_MODE;
    report_data[24] = game_mode_state;
    report_data[25] = 0x02;
    report_data[26] = XIAOAI_HFP_ATCMD_TYPE_ANTI_LOST;
    report_data[27] = anti_lost_state;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_ALL_STATUS_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_all_status ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_anc()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_anc, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_ANC_LEN      5
    uint8_t report_data[XIAOAI_HFP_REPORT_ANC_LEN] = {0};
    uint8_t anc_state = app_va_xiaoai_get_anc_state_for_miui();
    report_data[0] = 0x04;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x02;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_ANC;
    report_data[4] = anc_state;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_ANC_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_anc ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_key()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_key, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_KEY_LEN      10
    uint8_t report_data[XIAOAI_HFP_REPORT_KEY_LEN] = {0};
    report_data[0] = 0x09;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x07;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_KEY;
    report_data[4] = app_va_xiaoai_key_click_function(TRUE, AIRO_KEY_DOUBLE_CLICK);
    report_data[5] = app_va_xiaoai_key_click_function(FALSE, AIRO_KEY_DOUBLE_CLICK);
    report_data[6] = app_va_xiaoai_key_click_function(TRUE, AIRO_KEY_TRIPLE_CLICK);
    report_data[7] = app_va_xiaoai_key_click_function(FALSE, AIRO_KEY_TRIPLE_CLICK);
    report_data[8] = app_va_xiaoai_key_anc_function(TRUE);
    report_data[9] = app_va_xiaoai_key_anc_function(FALSE);

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_KEY_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_key ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_voice()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_voice, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_VOICE_LEN      8
    uint8_t report_data[XIAOAI_HFP_REPORT_VOICE_LEN] = {0};
    report_data[0] = 0x07;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x05;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_VOICE;
    report_data[4] = app_va_xiaoai_is_enable_wwe();
    report_data[5] = app_va_xiaoai_get_voice_recognition_state();
    // bit0 - support XiaoAI (Yes)
    // bit1 - support prev (Yes)
    // bit2 - support next (Yes)
    // bit3 - support volume up (Yes)
    // bit4 - support volume down (Yes)
    // bit5 - support reject call (No)
    // bit6 - support accept call (No)
    // bit7 - support 0
    report_data[6] = 0x1F;
    report_data[7] = 0;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_VOICE_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_voice ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_eq()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_eq, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_EQ_LEN      5
    uint8_t report_data[XIAOAI_HFP_REPORT_EQ_LEN] = {0};
    uint8_t eq_state = app_va_xiaoai_get_eq_mode();
    report_data[0] = 0x04;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x02;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_EQ;
    report_data[4] = eq_state;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_EQ_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_eq ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_game_mode()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_game_mode, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_GAME_MODE_LEN      5
    uint8_t report_data[XIAOAI_HFP_REPORT_GAME_MODE_LEN] = {0};
    uint8_t game_mode_state = app_va_xiaoai_get_game_mode_for_miui();
    report_data[0] = 0x04;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x02;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_GAME_MODE;
    report_data[4] = game_mode_state;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_GAME_MODE_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_game_mode ret=%d", 1, ret);
}

void app_va_xiaoai_hfp_miui_more_atcmd_report_anti_lost()
{
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_anti_lost, not Agent", 0);
        return;
    }

    #define XIAOAI_HFP_REPORT_ANTI_LOST_LEN      5
    uint8_t report_data[XIAOAI_HFP_REPORT_ANTI_LOST_LEN] = {0};
    uint8_t anti_lost_state = app_va_xiaoai_get_anti_lost_state();
    report_data[0] = 0x04;
    report_data[1] = XIAOAI_HFP_ATCMD_TYPE_REPORT;
    report_data[2] = 0x02;
    report_data[3] = XIAOAI_HFP_ATCMD_TYPE_ANTI_LOST;
    report_data[4] = anti_lost_state;

    bool ret = app_va_xiaoai_send_hfp_atcmd(XIAOAI_HFP_ATCMD_ATT_STATUS,
                                            report_data,
                                            XIAOAI_HFP_REPORT_ANTI_LOST_LEN);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" more_atcmd_report_anti_lost ret=%d", 1, ret);
}

void xiaoai_va_xiaoai_hfp_at_cmd_register(bool enable)
{
    bt_status_t status = BT_STATUS_FAIL;
    if (enable) {
        status = bt_callback_manager_register_callback(bt_callback_type_app_event,
                                    MODULE_MASK_HFP, (void *)app_va_xiaoai_hfp_callback);
    } else {
        status = bt_callback_manager_deregister_callback(bt_callback_type_app_event,
                                    (void *)app_va_xiaoai_hfp_callback);
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_HFP_TAG" register HFP enable=%d status=0x%08X",
                     2, enable, status);
}

#endif /* MTK_VA_XIAOAI_ENABLE */
