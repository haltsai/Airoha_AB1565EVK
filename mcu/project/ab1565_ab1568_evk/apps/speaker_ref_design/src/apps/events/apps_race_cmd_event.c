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


#include "apps_race_cmd_event.h"

#include "apps_config_key_remapper.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "bt_device_manager.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_aws_sync_event.h"
#include "app_home_screen_idle_activity.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_music_utils.h"
#endif
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd.h"
#include "race_cmd_relay_cmd.h"
#include "race_noti.h"
#include "race_bt.h"
#endif

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif

#include "apps_control_touch_key_status.h"

extern void vPortFree(void*);
extern void *pvPortMalloc(size_t);

#define LOG_TAG     "[apps_race_event]"

#define RACE_ID_APPLICATION_BEGIN 0x2C10
#define RACE_ID_APPLICATION_END 0x2CCF

#define RACE_ID_APP_POWER_SAVING_SET_CFG 0x2C15
#define RACE_ID_APP_POWER_SAVING_GET_CFG 0x2C16

#define RACE_ID_APP_IN_EAR_SET_MUSIC_CFG 0x2C17
#define RACE_ID_APP_IN_EAR_GET_MUSIC_CFG 0x2C18

#ifdef MTK_RACE_CMD_ENABLE
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
static void *apps_race_cmd_power_saving(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t enable_flag;
        uint16_t timeout;
        uint16_t silence_detect_enable;
        uint16_t silence_detect_timeout;
    } PACKED power_saving_cfg_set_req;

    typedef struct {
        uint8_t result;
    } PACKED RSP;

    typedef struct {
        uint8_t result;
        uint16_t enable_flag;
        uint16_t timeout;
        uint16_t silence_detect_enable;
        uint16_t silence_detect_timeout;
    } PACKED QUERY_RSP;

    void *pEvt = NULL;

    switch (p_race_package->hdr.id) {
        /* Nnable music control */
        case RACE_ID_APP_POWER_SAVING_SET_CFG: {
            power_saving_cfg_set_req *pcmd = (power_saving_cfg_set_req *)p_race_package;
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_POWER_SAVING_SET_CFG,
                                         sizeof(RSP),
                                         channel_id);
            if (pEvt) {
                ((RSP *)pEvt)->result = RACE_ERRCODE_FAIL;
                if (pcmd == NULL) {
                    return pEvt;
                }

                /* sync msg to partner. */
#ifdef MTK_AWS_MCE_ENABLE
                RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    ret = bt_send_aws_mce_race_cmd_data(p_race_package, pcmd->Hdr.length + (uint32_t) & (((RACE_COMMON_HDR_STRU *)0)->id), channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner failed,", 0);
                        ((RSP *)pEvt)->result = ret;
                        return pEvt;
                    }
                }
#endif
                uint16_t data_len = pcmd->Hdr.length - 2;
                app_power_saving_cfg cfg;
                memset(&cfg, 0, sizeof(cfg));
                if (data_len >= 2 * sizeof(uint16_t)) {
                    cfg.power_saving_enable = pcmd->enable_flag;
                    cfg.timeout = pcmd->timeout;
                } else {
                    return pEvt;
                }
                if (data_len >= 4 * sizeof(uint16_t)) {
                    /* Old version not support silence detect. so only 2 type.*/
                    cfg.silence_detect_enable = pcmd->silence_detect_enable;
                    cfg.silence_detect_timeout = pcmd->silence_detect_timeout;
                }
                /* Notify app the cfg updated. */
                APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, en: %d, t:%d, silence_en: %d, t: %d, len:%d", 5, cfg.power_saving_enable, cfg.timeout, cfg.silence_detect_enable, cfg.silence_detect_timeout, length);
                if ((pcmd->enable_flag && pcmd->timeout < 1) || (cfg.silence_detect_enable && cfg.silence_detect_timeout < 1)) {
                    return pEvt;
                }
                int32_t r = app_power_save_utils_set_cfg(&cfg);
                if (r == 0) {
                    app_power_save_utils_cfg_updated_notify();
                    ((RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                } else {
                    return pEvt;
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
        /* Get the music control state */
        case RACE_ID_APP_POWER_SAVING_GET_CFG: {
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_POWER_SAVING_GET_CFG,
                                         sizeof(QUERY_RSP),
                                         channel_id);
            if (pEvt) {
                app_power_saving_cfg *cfg = app_power_saving_get_cfg();
                ((QUERY_RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                ((QUERY_RSP *)pEvt)->enable_flag = cfg->power_saving_enable;
                ((QUERY_RSP *)pEvt)->timeout = cfg->timeout;
                ((QUERY_RSP *)pEvt)->silence_detect_enable = cfg->silence_detect_enable;
                ((QUERY_RSP *)pEvt)->silence_detect_timeout = cfg->silence_detect_timeout;
                APPS_LOG_MSGID_I(LOG_TAG"get power saving cfg, en: %d, t:%d", 2, cfg->power_saving_enable, cfg->timeout);
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
    }

    return pEvt;
}
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
static void* apps_race_cmd_in_ear(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    typedef struct{
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t req[1];
    }PACKED in_ear_cfg_set_req;

    typedef struct {
        uint8_t result;
    } PACKED RSP;

    typedef struct {
        uint8_t result;
        uint8_t enable;
    } PACKED QUERY_RSP;

    void* pEvt = NULL;

    switch(p_race_package->hdr.id) {
        /* Nnable music control */
        case RACE_ID_APP_IN_EAR_SET_MUSIC_CFG:{
            in_ear_cfg_set_req* pcmd = (in_ear_cfg_set_req*)p_race_package;
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                            RACE_TYPE_RESPONSE,
                                            RACE_ID_APP_IN_EAR_SET_MUSIC_CFG,
                                            sizeof(RSP),
                                            channel_id);
            if (pEvt) {
                ((RSP*)pEvt)->result = RACE_ERRCODE_FAIL;
                /* sync msg to partner. */
                #ifdef MTK_AWS_MCE_ENABLE
                RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    ret = bt_send_aws_mce_race_cmd_data(p_race_package, sizeof(RACE_COMMON_HDR_STRU) + 1, channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        APPS_LOG_MSGID_I(LOG_TAG"set music ctrl, sync to partner failed,", 0);
                        ((RSP*)pEvt)->result = ret;
                        return pEvt;
                    }
                }
                #endif
                /* Notify app the cfg updated. */
                uint8_t* p_data = (uint8_t *)pvPortMalloc(sizeof(uint8_t));
                if (p_data != NULL) {
                    ui_shell_status_t status = UI_SHELL_STATUS_OK;
                    *p_data = pcmd->req[0];
                    status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_IN_EAR_CFG_UPDATE,
                                        p_data, sizeof(uint8_t), NULL, 0);
                    if (status != UI_SHELL_STATUS_OK) {
                        vPortFree(p_data);
                        return pEvt;
                    }
                    ((RSP*)pEvt)->result = RACE_ERRCODE_SUCCESS;
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }

            break;
        }
        /* Get the music control state */
        case RACE_ID_APP_IN_EAR_GET_MUSIC_CFG:{
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                            RACE_TYPE_RESPONSE,
                                            RACE_ID_APP_IN_EAR_GET_MUSIC_CFG,
                                            sizeof(QUERY_RSP),
                                            channel_id);
            if (pEvt) {
                ((QUERY_RSP*)pEvt)->result = RACE_ERRCODE_SUCCESS;
                uint8_t temp_music_in_ear_cfg = app_music_get_in_ear_control_state();
                uint8_t enabled = temp_music_in_ear_cfg ? 1 : 0;
                ((QUERY_RSP*)pEvt)->enable = enabled;
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
    }

    return pEvt;
}
#endif
#endif
void app_race_send_notify(uint16_t cfg_type, int8_t *data, uint32_t len) {
    uint8_t channel_id;
    RACE_ERRCODE race_notify_ret;
    uint32_t serial_port;
    serial_port_dev_t dev;
    bt_handle_t handle;

    typedef struct
    {
        uint16_t config_type;
        uint8_t data[1];
    }PACKED NOTI;

    handle = race_bt_get_ble_conn_hdl();
    APPS_LOG_MSGID_I(LOG_TAG" send notiy, type:%d, data:0x%x, len: %d.", 3, cfg_type, data, len);

    if (handle != BT_HANDLE_INVALID) {
        dev = SERIAL_PORT_DEV_BT_LE;
    } else {
        dev = SERIAL_PORT_DEV_BT_SPP;
    }

    serial_port = race_get_serial_port_handle(dev);

    channel_id = (uint8_t)race_get_channel_id_by_port_handle(serial_port);
    NOTI *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND_WITHOUT_RSP,
            (uint16_t)RACE_SET_APP_COMMON_CONFIG,
            (uint16_t)(sizeof(uint16_t) + len),
            channel_id);
    if (noti == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG"claim packet failed.", 0);
        return;
    }

    noti->config_type = cfg_type;
    memcpy(&noti->data[0], data, len);

    race_notify_ret = race_noti_send(noti, channel_id, false);
    APPS_LOG_MSGID_I(LOG_TAG"notify data resule: %d.", 1, race_notify_ret);
    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
        RACE_FreePacket((void *)noti);
    }
}

void app_race_notify_mmi_state(uint32_t mmi_state)
{
    /* If connected ULL dongle, need notify to dongle->HID->PC. */
    uint8_t dev[2] = { 0 };
    uint32_t i;
    uint32_t dev_count = 0;
    uint8_t channel_id;
    RACE_ERRCODE race_notify_ret;

    typedef struct
    {
        uint32_t mmi_state;
    } PACKED NOTI;
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT
        || bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_NONE)
#endif
    {
        bt_bd_addr_t addr_list[1];
        uint32_t list_num = 1;
        list_num = bt_sink_srv_cm_get_connected_device_list(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL), addr_list, list_num);
        if (list_num > 0 &&
            BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AIR) & bt_cm_get_connected_profile_services(addr_list[0])) {
            dev[dev_count] = RACE_SERIAL_PORT_TYPE_SPP;
            dev_count++;
            APPS_LOG_MSGID_I(LOG_TAG"Add SPP in notify mmi state port", 0);
        }
    }
#endif
#if defined(APPS_USB_AUDIO_SUPPORT) && defined(MTK_USB_AUDIO_HID_ENABLE)
    dev[dev_count] = RACE_SERIAL_PORT_TYPE_USB;
    dev_count++;
    APPS_LOG_MSGID_I(LOG_TAG"Add USB in notify mmi state port", 0);
#endif
    for (i = 0; i < dev_count; i++) {
        channel_id = race_get_channel_id_by_port_type(dev[i]);
        NOTI *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND_WITHOUT_RSP,
            (uint16_t)RACE_GET_MMI_STATE,
            (uint16_t)(sizeof(NOTI)),
            channel_id);
        if (noti == NULL) {
            APPS_LOG_MSGID_E(LOG_TAG"notify_mmi_state[%d] claim packet failed", 1, i);
            return;
        }
        noti->mmi_state = mmi_state;
        race_notify_ret = race_noti_send(noti, channel_id, false);
        APPS_LOG_MSGID_I(LOG_TAG"notify_mmi_state resule: %d.", 1, race_notify_ret);
        if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
            RACE_FreePacket((void *)noti);
        }
    }
}

static void* apps_race_cmd_handler(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    void *pEvt = NULL;
    APPS_LOG_MSGID_I(LOG_TAG"apps_race_cmd_handler, type[0x%X], id[0x%X]", 2, p_race_package->hdr.type, p_race_package->hdr.id);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    if (p_race_package->hdr.type == RACE_TYPE_COMMAND ||
        p_race_package->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {
        switch (p_race_package->hdr.id)
        {
            case RACE_GET_MMI_STATE: {
                typedef struct {
                    uint8_t mmi_state;
                } PACKED RSP;
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                            RACE_TYPE_RESPONSE,
                            RACE_GET_MMI_STATE,
                            sizeof(RSP),
                            channel_id);
                if (pEvt) {
                    ((RSP *)pEvt)->mmi_state = apps_config_key_get_mmi_state();
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                }
                break;
            }
            case RACE_PLAY_VOICE_PROMPT: {
                typedef struct {
                    uint8_t status;
                } PACKED RSP;
                typedef struct
                {
                    RACE_COMMON_HDR_STRU cmdhdr;
                    uint16_t vp_index;
                } PACKED CMD;
                CMD *cmd = (CMD *)p_race_package;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_PLAY_VP,
                                    (void *)(uint32_t)(cmd->vp_index), 0, NULL, 0);
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                            RACE_TYPE_RESPONSE,
                            RACE_PLAY_VOICE_PROMPT,
                            sizeof(RSP),
                            channel_id);
                if (pEvt) {
                    ((RSP *)pEvt)->status = 0;
                }  else {
                    APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                }
                break;
            }

            case RACE_SET_LED_PATTERN: {
                typedef struct {
                    uint8_t status;
                } PACKED RSP;
                typedef struct
                {
                    RACE_COMMON_HDR_STRU cmdhdr;
                    uint32_t data;
                } PACKED CMD;
                CMD *cmd = (CMD *)p_race_package;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_SET_LED,
                                    (void *)(uint32_t)(cmd->data), 0, NULL, 0);
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                            RACE_TYPE_RESPONSE,
                            RACE_SET_LED_PATTERN,
                            sizeof(RSP),
                            channel_id);
                if (pEvt) {
                    ((RSP *)pEvt)->status = 0;
                }
                break;
            }
            case RACE_SET_APP_COMMON_CONFIG: {
                typedef struct {
                        uint8_t status;
                        uint16_t config_type;
                } PACKED RSP;
                typedef struct
                {
                        RACE_COMMON_HDR_STRU cmdhdr;
                        uint16_t config_type;
                        uint8_t config_data[0];
                } PACKED CMD;
                CMD* cmd = (CMD *)p_race_package;
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                        RACE_TYPE_RESPONSE,
                        RACE_SET_APP_COMMON_CONFIG,
                        sizeof(RSP),
                        channel_id);
                uint16_t data_len = cmd->cmdhdr.length - 4;
                APPS_LOG_MSGID_I(LOG_TAG" race set app common config: handler_id=0x%04X, config_type=0x%04X, data_len=%d",
                                 3, p_race_package->hdr.id, cmd->config_type, data_len);
                switch (cmd->config_type) {
                    case APPS_RACE_CMD_SET_TOUCH_KEY_ENABLE: {

                        void *p_data = &(cmd->config_data);
                        if (pEvt) {
#ifdef MTK_AWS_MCE_ENABLE
                            if ((BT_AWS_MCE_ROLE_AGENT == role) || (BT_AWS_MCE_ROLE_NONE == role)) {
#endif
                                APPS_LOG_MSGID_I(LOG_TAG" set touch key enable: 0x%02X", 1, *(uint8_t *)p_data);
                                uint8_t ret = apps_set_touch_control_enable(*(uint8_t *)p_data, false);
                                if (1 == ret) {
                                    ((RSP *)pEvt)->status = 0;
                                    ((RSP *)pEvt)->config_type = APPS_RACE_CMD_SET_TOUCH_KEY_ENABLE;
                                } else {
                                    ((RSP *)pEvt)->status = 1;
                                    ((RSP *)pEvt)->config_type = APPS_RACE_CMD_SET_TOUCH_KEY_ENABLE;
                                }
#ifdef MTK_AWS_MCE_ENABLE
                            }
#endif
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                        }
                        break;
                    }
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    case APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG: {
                        void *p_data = &(cmd->config_data);
                        if (pEvt) {
                            ((RSP *)pEvt)->status = RACE_ERRCODE_FAIL;
                            ((RSP *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG;
                            /* sync msg to partner. */
#ifdef MTK_AWS_MCE_ENABLE
                            RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                            if (role == BT_AWS_MCE_ROLE_AGENT) {
                                ret = bt_send_aws_mce_race_cmd_data(p_race_package, p_race_package->hdr.length + (uint32_t) & (((RACE_COMMON_HDR_STRU *)0)->id), channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                                if (RACE_ERRCODE_SUCCESS != ret) {
                                    APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner failed. ", 0);
                                    ((RSP *)pEvt)->status = ret;
                                    break;
                                }
                                APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner success. ", 0);
                            }
#endif
                            /* Notify app the cfg updated. */
                            app_power_saving_cfg cfg;
                            memset(&cfg, 0, sizeof(cfg));
                            if (data_len >= 2 * sizeof(uint16_t)) {
                                cfg.power_saving_enable = *(uint16_t *)p_data;
                                cfg.timeout = *((uint16_t *)p_data + 1);
                            }
                            if (data_len >= 4 * sizeof(uint16_t)) {
                                cfg.silence_detect_enable = *((uint16_t *)p_data + 2);
                                cfg.silence_detect_timeout = *((uint16_t *)p_data + 3);
                            }
                            APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg, en: %d, t:%d, en_silence:%d, t:%d, len:%d",
                                             5, cfg.power_saving_enable, cfg.timeout, cfg.silence_detect_enable, cfg.silence_detect_timeout, length);

                            if ((cfg.power_saving_enable && cfg.timeout < 1) || (cfg.silence_detect_enable && cfg.silence_detect_timeout < 1)) {
                                break;
                            }

                            int32_t r = app_power_save_utils_set_cfg(&cfg);
                            if (r == 0) {
                                APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg success. ", 0);
                                app_power_save_utils_cfg_updated_notify();
                                ((RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            } else {
                                APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg fail. ", 0);
                                ((RSP *)pEvt)->status = RACE_ERRCODE_FAIL;
                                break;
                            }
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                        }
                        break;
                    }
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                    /* Enable music in ear control */
                    case APPS_RACE_CMD_SET_IN_EAR_MUSIC_CFG: {
                        void *p_data = &(cmd->config_data);
                        if (pEvt) {
                            ((RSP*)pEvt)->status = RACE_ERRCODE_FAIL;
                            ((RSP*)pEvt)->config_type = APPS_RACE_CMD_SET_IN_EAR_MUSIC_CFG;
    #ifdef MTK_AWS_MCE_ENABLE
                            RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                            if (role == BT_AWS_MCE_ROLE_AGENT) {
                                ret = bt_send_aws_mce_race_cmd_data(p_race_package, sizeof(RACE_COMMON_HDR_STRU) + 3, channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                                if (RACE_ERRCODE_SUCCESS != ret) {
                                    APPS_LOG_MSGID_I(LOG_TAG"set music in ear ctrl, sync to partner failed. ", 0);
                                    ((RSP*)pEvt)->status = ret;
                                    break;
                                }
                                APPS_LOG_MSGID_I(LOG_TAG"set music in ear ctrl, sync to partner success. ", 0);
                            }
    #endif
                            /* Notify app the cfg updated. */
                            uint8_t* p_noti = (uint8_t *)pvPortMalloc(sizeof(uint8_t));
                            if (p_noti != NULL) {
                                ui_shell_status_t status = UI_SHELL_STATUS_OK;
                                *p_noti = *(uint8_t *)p_data;
                                status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_IN_EAR_CFG_UPDATE,
                                        p_noti, sizeof(uint8_t), NULL, 0);
                                if (status != UI_SHELL_STATUS_OK) {
                                    APPS_LOG_MSGID_I(LOG_TAG" notify music app the in ear cfg updated fail.", 0);
                                    vPortFree(p_noti);
                                    break;
                                }
                                APPS_LOG_MSGID_I(LOG_TAG" notify music app the in ear cfg updated success.", 0);
                                ((RSP*)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            } else {
                                APPS_LOG_MSGID_I(LOG_TAG" notify music app the in ear cfg updated malloc failed.", 0);
                            }
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                        }
                        break;
                    }
#endif
                    default:
                        break;
                }
                break;
            }
            case RACE_GET_APP_COMMON_CONFIG: {
                typedef struct
                {
                        RACE_COMMON_HDR_STRU cmdhdr;
                        uint16_t config_type;
                } PACKED CMD;
                CMD* cmd = (CMD *)p_race_package;
                APPS_LOG_MSGID_I(LOG_TAG" race get app common config: handler_id=0x%04X, config_type=0x%04X",
                        2, p_race_package->hdr.id, cmd->config_type);
                switch (cmd->config_type) {
                    case APPS_RACE_CMD_GET_TOUCH_KEY_ENABLE: {
                        typedef struct {
                                uint8_t status;
                                uint16_t config_type;
                                uint8_t enable;
                        } PACKED QUERY_RSP;
                        pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                RACE_TYPE_RESPONSE,
                                RACE_GET_APP_COMMON_CONFIG,
                                sizeof(QUERY_RSP),
                                channel_id);
                        if (pEvt != NULL) {
                            ((QUERY_RSP *)pEvt)->config_type = APPS_RACE_CMD_GET_TOUCH_KEY_ENABLE;
                            uint8_t ret = apps_get_touch_control_status();
                            if (0xFF == ret) {
                                ((QUERY_RSP *)pEvt)->status = 0xFF;
                                ((QUERY_RSP *)pEvt)->enable = 0xFF;
                                APPS_LOG_MSGID_I(LOG_TAG" get touch key status: 0x%02X", 1, ((QUERY_RSP *)pEvt)->status);
                            } else {
                                ((QUERY_RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                                ((QUERY_RSP *)pEvt)->enable = ret;
                                APPS_LOG_MSGID_I(LOG_TAG" get touch key status: 0x%02X", 1, ((QUERY_RSP *)pEvt)->status);
                            }
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" get touch key status pevt return NULL", 0);
                        }
                        break;
                    }
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    case APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG: {
                        typedef struct {
                            uint8_t status;
                            uint16_t config_type;
                            uint16_t enable_flag;
                            uint16_t timeout;
                            uint16_t silence_detect_enable;
                            uint16_t silence_detect_timeout;
                        } PACKED QUERY_RSP;
                        pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                                     RACE_TYPE_RESPONSE,
                                                     RACE_GET_APP_COMMON_CONFIG,
                                                     sizeof(QUERY_RSP),
                                                     channel_id);
                        if (pEvt != NULL) {
                            ((QUERY_RSP *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG;
                            app_power_saving_cfg *cfg = app_power_saving_get_cfg();
                            ((QUERY_RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            ((QUERY_RSP *)pEvt)->enable_flag = cfg->power_saving_enable;
                            ((QUERY_RSP *)pEvt)->timeout = cfg->timeout;
                            ((QUERY_RSP *)pEvt)->silence_detect_enable = cfg->silence_detect_enable;
                            ((QUERY_RSP *)pEvt)->silence_detect_timeout = cfg->silence_detect_timeout;
                            APPS_LOG_MSGID_I(LOG_TAG"get power saving cfg, en: %d, t:%d, en_silence: %d, t:%d", 4, cfg->power_saving_enable, cfg->timeout, cfg->silence_detect_enable, cfg->silence_detect_timeout);
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" get power saving pevt return NULL", 0);
                        }
                        break;
                    }
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                    case APPS_RACE_CMD_GET_IN_EAR_MUSIC_CFG: {
                        typedef struct {
                                uint8_t status;
                                uint16_t config_type;
                                uint8_t enable;
                        } PACKED QUERY_RSP;
                        pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                RACE_TYPE_RESPONSE,
                                RACE_GET_APP_COMMON_CONFIG,
                                sizeof(QUERY_RSP),
                                channel_id);
                        if (pEvt != NULL) {
                        ((QUERY_RSP*)pEvt)->config_type = APPS_RACE_CMD_GET_IN_EAR_MUSIC_CFG;
                        ((QUERY_RSP*)pEvt)->status = RACE_ERRCODE_SUCCESS;
                        uint8_t temp_music_in_ear_cfg = app_music_get_in_ear_control_state();
                        uint8_t enabled = temp_music_in_ear_cfg ? 1 : 0;
                        ((QUERY_RSP*)pEvt)->enable = enabled;
                        APPS_LOG_MSGID_I(LOG_TAG"get music in ear cfg, status=%d, enable=%d", 2, ((QUERY_RSP*)pEvt)->status, enabled);
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" get music in ear cfg pevt return NULL", 0);
                        }
                        break;
                    }
#endif
                    default:
                        break;
                }

                break;
            }

            /* In-ear app set or get cfg. */
            /* In-ear app set or get cfg. */
            #ifdef MTK_RACE_CMD_ENABLE
            #ifdef MTK_IN_EAR_FEATURE_ENABLE
            case RACE_ID_APP_IN_EAR_SET_MUSIC_CFG:
            case RACE_ID_APP_IN_EAR_GET_MUSIC_CFG:
                return apps_race_cmd_in_ear(p_race_package, length, channel_id);
            #endif
            /* Power saving */
            #ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            case RACE_ID_APP_POWER_SAVING_SET_CFG:
            case RACE_ID_APP_POWER_SAVING_GET_CFG:
                return apps_race_cmd_power_saving(p_race_package, length, channel_id);
            #endif
            #endif

            default:
                break;
        }
    }
    return pEvt;
}

void apps_race_cmd_event_init(void)
{
#if defined(MTK_RACE_CMD_ENABLE)
    RACE_HANDLER handler = {
        .id_start = RACE_ID_APPLICATION_BEGIN,
        .id_end = RACE_ID_APPLICATION_END,
        .handler = apps_race_cmd_handler
    };
    race_status_t ret = RACE_Register_Handler(&handler);
    APPS_LOG_MSGID_I(LOG_TAG" app race register fail: ret=%d", 1, ret);
#endif
}
