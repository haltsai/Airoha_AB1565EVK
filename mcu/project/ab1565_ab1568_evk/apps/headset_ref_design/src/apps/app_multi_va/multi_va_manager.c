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
 * File: multi_va_manager.c
 *
 * Description: This file provides ability to register voice assistant callback and control the connected, disconnected,
 * adding adv and switching flow.
 *
 */

#include "multi_va_manager.h"
#include "multi_va_event_id.h"
#include "apps_events_interaction_event.h"
#include "FreeRTOS.h"
#include "apps_debug.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_aws_sync_event.h"
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "apps_config_key_remapper.h"
#include "apps_customer_config.h"
#include "bt_device_manager.h"
#include <string.h>
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
#include "apps_race_cmd_co_sys_event.h"
#endif /* AIR_RACE_CO_SYS_ENABLE */

#define LOG_TAG     "[Multi_VA] "   /* Log tag. */

/**
 *  @brief This struct defines the format of a voice assistant item.
 */
typedef struct {
    multi_va_manager_callbacks_t *p_callbacks;  /**< the callback functions. */
} multi_va_register_item_t;

static uint8_t s_va_type;   /* Current voice assistant type. */
static uint8_t removing_va_type;    /* When remove voice assistant and wait it inactive done, the value is the removing type. */
static bool s_removing_from_other_app = false; /* remove pairing request from other device apps, ex. from homescreen */
static uint8_t s_va_connected = false;  /* The flag of voice assistant is connected. */
static bool s_rebooting = false;        /* The flag of voice assistant type is switched to another, need do reboot. */
static bool s_enable_adv = false;       /* When the flag is true, need add adv in multi_ble_adv_manager. */

/* The array is useful to record the registered voice assistant items. */
multi_va_register_item_t va_info_array[MULTI_VA_TYPE_MAX_NUM];

void multi_voice_assistant_manager_register_instance(
    multi_va_type_t va_type,
    const multi_va_manager_callbacks_t *p_callbacks)
{
    if (va_type < MULTI_VA_TYPE_MAX_NUM && p_callbacks) {
        va_info_array[va_type].p_callbacks = (multi_va_manager_callbacks_t *)pvPortMalloc(sizeof(multi_va_manager_callbacks_t));
        if (va_info_array[va_type].p_callbacks) {
            memcpy(va_info_array[va_type].p_callbacks, p_callbacks,
                   sizeof(multi_va_manager_callbacks_t));
        } else {
            APPS_LOG_MSGID_E(LOG_TAG"register_instance malloc fail", 0);
        }
    } else {
        APPS_LOG_MSGID_E(LOG_TAG"register_instance parameter error va_type: %d", 1, va_type);
    }
}

void multi_va_manager_start(void)
{
    uint32_t read_size = sizeof(s_va_type);
    nvkey_status_t read_status;

    s_va_type = MULTI_VA_TYPE_UNKNOWN;
    removing_va_type = MULTI_VA_TYPE_UNKNOWN;
    /* When system start, read va type from NVDM. */
    read_status = nvkey_read_data(NVKEYID_VA_SWITH, &s_va_type, &read_size);
    APPS_LOG_MSGID_I(LOG_TAG"start va_type is: %d, read_status = %d", 2, s_va_type, read_status);
    if (NVKEY_STATUS_OK != read_status) {
        s_va_type = APPS_DEFAULT_VA_TYPE;
        nvkey_status_t write_status = nvkey_write_data(NVKEYID_VA_SWITH, &s_va_type, sizeof(s_va_type));
        if (NVKEY_STATUS_OK != write_status) {
            APPS_LOG_MSGID_I(LOG_TAG", va_manager_start, Write NVKEYID_VA_SWITH failed %d, va type : %d", 2, write_status, s_va_type);
        }
    }

#ifdef MULTI_VA_SUPPORT_COMPETITION
    size_t i;
    for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
        /* If support MULTI_VA_SUPPORT_COMPETITION, add adv of all voice assistant item. */
        if (va_info_array[i].p_callbacks) {
            va_info_array[i].p_callbacks->voice_assistant_initialize(s_va_type == i);
        }
    }
#else
    /* Because not support MULTI_VA_SUPPORT_COMPETITION, only start one VA */
    if (s_va_type < MULTI_VA_TYPE_MAX_NUM && va_info_array[s_va_type].p_callbacks) {
        va_info_array[s_va_type].p_callbacks->voice_assistant_initialize(true);
    } else {
        APPS_LOG_MSGID_E(LOG_TAG"Cannot init the va type: %d", 1, s_va_type);
    }
#endif
    /* Initialize multi_ble_adv_manager when system start. */
    multi_ble_adv_manager_init();
#if defined(MULTI_VA_SUPPORT_COMPETITION) && defined(MTK_AMA_ENABLE) && defined(GSOUND_LIBRARY_ENABLE)
    multi_ble_adv_manager_set_support_multi_adv_data(MULTI_ADV_INSTANCE_DEFAULT);
#endif
}

multi_va_type_t multi_va_manager_get_current_va_type(void)
{
    return s_va_type;
}

bool multi_va_manager_get_current_va_connected(void)
{
    return s_va_connected;
}

#if defined(MTK_AWS_MCE_ENABLE) && defined(MULTI_VA_SUPPORT_COMPETITION)
void multi_va_manager_send_va_type_to_partner(void)
{
    bt_status_t send_ret = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                          MULTI_VA_EVENT_SYNC_VA_TYPE_TO_PARTNER, &s_va_type, sizeof(s_va_type));
    APPS_LOG_MSGID_I(LOG_TAG"send_va_type_to_partner(), type = %d", 1, s_va_type);
    if (BT_STATUS_SUCCESS != send_ret) {
        APPS_LOG_MSGID_W(LOG_TAG"send_va_type_to_partner(), fail = %x", 1, send_ret);
    }
}

void multi_va_manager_receive_va_change_from_agent(void *p_va_type)
{
    nvkey_status_t write_status;
    uint8_t va_type = *(uint8_t *)p_va_type;
    /* size_t i; */
    APPS_LOG_MSGID_I(LOG_TAG"_receive_va_change_from_agent(), type = %d, s_va_type = %d",
                     2, va_type, s_va_type);
    if (s_va_type != va_type) {
        if (s_va_type >= MULTI_VA_TYPE_MAX_NUM) {

        } else {
            /* Notify the voice assistant switch to false. */
            if (va_info_array[s_va_type].p_callbacks) {
                va_info_array[s_va_type].p_callbacks->on_voice_assistant_type_switch(false);
            }
        }

        /* Notify the voice assistant switch to true. */
        if (va_type < MULTI_VA_TYPE_MAX_NUM) {
            if (va_info_array[va_type].p_callbacks) {
                va_info_array[va_type].p_callbacks->on_voice_assistant_type_switch(true);
            }
        }

        /* Set the key remapping table when va type changed. */
        apps_config_key_remapper_set_va_type(s_va_type, va_type);
        s_va_type = va_type;
        write_status = nvkey_write_data(NVKEYID_VA_SWITH, &s_va_type, sizeof(s_va_type));
        if (NVKEY_STATUS_OK != write_status) {
            APPS_LOG_MSGID_I(LOG_TAG"_receive_va_change_from_agent(), Write NVKEYID_VA_SWITH failed %d", 1, write_status);
        }
    }
}

void multi_va_manager_receive_reboot_from_agent(void)
{
    APPS_LOG_MSGID_I(LOG_TAG"multi_va_manager_receive_reboot_from_agent(), type = %d",
                     1, s_va_type);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                        EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                        (void *)100, 0, NULL, 0);
}

void multi_va_manager_on_partner_detached(void)
{
    if (s_rebooting) {
        APPS_LOG_MSGID_I(LOG_TAG"multi_va_manager_partner_detached()", 0);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                            NULL, 0, NULL, 0);
    }
}
#endif

/**
 * @brief      This function do the reboot flow.
 */
static void multi_va_manager_reboot(void)
{
    s_rebooting = true;
#if defined(MTK_AWS_MCE_ENABLE) && defined(MULTI_VA_SUPPORT_COMPETITION)
    bt_status_t send_ret = BT_STATUS_SUCCESS;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    APPS_LOG_MSGID_I(LOG_TAG"multi_va_manager_reboot(), type = %d, role = %x",
                     2, s_va_type, role);
    if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
        /* Agent send aws data to notify partner reboot. */
        send_ret = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                  MULTI_VA_EVENT_SYNC_REBOOT, NULL, 0);
    } else if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        /* When set VA type from Smart phone, Partner should wait agent sending reboot request. */
        APPS_LOG_MSGID_I(LOG_TAG"multi_va_manager_reboot(), partner connected, just wait", 0);
    } else {
        send_ret = BT_STATUS_UNSUPPORTED;
        APPS_LOG_MSGID_I(LOG_TAG"multi_va_manager_reboot(), partner disconnected reboot now", 0);
    }

    if (BT_STATUS_SUCCESS != send_ret)
#endif
    {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) && defined(AIR_RACE_CO_SYS_ENABLE)
        APPS_LOG_MSGID_W(LOG_TAG"multi_va_manager_reboot, send reboot event to master to execute reboot action", 0);
        app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                                        NULL,
                                        0,
                                        false);
#else
        APPS_LOG_MSGID_W(LOG_TAG"multi_va_manager_reboot, directly_reboot", 0);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                            NULL, 0, NULL, 0);
#endif
    }
}

void multi_voice_assistant_manager_notify_va_connected(multi_va_type_t va_type)
{
#ifdef MULTI_VA_SUPPORT_COMPETITION
    nvkey_status_t write_status;
    APPS_LOG_MSGID_I(LOG_TAG"notify_va_connected, va_type: (%d)->(%d)", 2, s_va_type, va_type);
    if (s_va_type < MULTI_VA_TYPE_MAX_NUM && s_va_type != va_type) {
        /* s_va_type changed, it's va changed need reboot */
        multi_va_switch_off_return_t switch_off_result = MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
        if (va_info_array[s_va_type].p_callbacks) {
            switch_off_result =
                va_info_array[s_va_type].p_callbacks->on_voice_assistant_type_switch(false);
            APPS_LOG_MSGID_I(LOG_TAG"notify_va_connected(), switch off result : %d",
                             1, switch_off_result);
        }
        if (MULTI_VA_SWITCH_OFF_WAIT_INACTIVE == switch_off_result) {
            removing_va_type = s_va_type;
        }

        apps_config_key_remapper_set_va_type(s_va_type, va_type);
        s_va_type = va_type;
        write_status = nvkey_write_data(NVKEYID_VA_SWITH, &s_va_type, sizeof(s_va_type));
        if (NVKEY_STATUS_OK != write_status) {
            APPS_LOG_MSGID_E(LOG_TAG"notify_va_connected(), Write NVKEYID_VA_SWITH failed %d", 1, write_status);
        }
#if defined(MTK_AWS_MCE_ENABLE)
        /* Sync VA type to partner */
        multi_va_manager_send_va_type_to_partner();
#endif
        if (MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE == switch_off_result) {
            multi_va_manager_reboot();
        }

    } else {
        if (s_va_type >= MULTI_VA_TYPE_MAX_NUM && s_va_type != va_type) {
            apps_config_key_remapper_set_va_type(s_va_type, va_type);
            s_va_type = va_type;
            APPS_LOG_MSGID_I(LOG_TAG"notify_va_connected(), first time switch %d", 1, va_type);
            write_status = nvkey_write_data(NVKEYID_VA_SWITH, &s_va_type, sizeof(s_va_type));
            if (NVKEY_STATUS_OK != write_status) {
                APPS_LOG_MSGID_E(LOG_TAG"notify_va_connected(), Write NVKEYID_VA_SWITH failed %d", 1, write_status);
            }
#if defined(MTK_AWS_MCE_ENABLE)
            /* sync VA type to partner */
            multi_va_manager_send_va_type_to_partner();
#endif
            if (s_enable_adv) {
                if (va_info_array[va_type].p_callbacks) {
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[va_type].p_callbacks->on_get_ble_adv_data);
                    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
                }
            }
        } else if (s_va_type == va_type) {
            /* If VA type is not changed, remove adv. */
            APPS_LOG_MSGID_I(LOG_TAG"notify_va_connected(), va_type_not change", 0);
            if (s_enable_adv) {
                if (va_info_array[va_type].p_callbacks) {
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[va_type].p_callbacks->on_get_ble_adv_data);
                }
                multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
            }
        }
    }
#else
    if (s_va_type != va_type) {
        APPS_LOG_MSGID_E(LOG_TAG"notify_va_connected, not support MULTI_VA_SUPPORT_COMPETITION, but va type not correct: %d", 1, va_type);
    } else {
        APPS_LOG_MSGID_I(LOG_TAG"notify_va_connected, not support MULTI_VA_SUPPORT_COMPETITION, va_type = %d", 1, va_type);
        if (va_info_array[va_type].p_callbacks) {
            multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[va_type].p_callbacks->on_get_ble_adv_data);
            multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
        }
    }
#endif
    s_va_connected = true;
}

void multi_voice_assistant_manager_set_inactive_done(multi_va_type_t va_type)
{
    APPS_LOG_MSGID_I(LOG_TAG"set_inactive_done, va_type: %d, removing_va_type: %d, s_va_type: %d",
                     3, va_type, removing_va_type, s_va_type);
    if (removing_va_type == va_type) {
        removing_va_type = MULTI_VA_TYPE_UNKNOWN;
        if (s_removing_from_other_app) {
            if (s_va_type == va_type) {
                s_va_type = MULTI_VA_TYPE_UNKNOWN;
            }
            APPS_LOG_MSGID_I(LOG_TAG"remove pairing by other app, notify it", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_MULTI_VA_REMOVE_PAIRING_DONE,
                                NULL, 0, NULL, 0);
        } else {
#ifdef MULTI_VA_SUPPORT_COMPETITION
            /* reboot both side */
            multi_va_manager_reboot();
#endif
        }
    } else if (s_va_type == va_type) {
#if 0
#ifdef MULTI_VA_SUPPORT_COMPETITION
        /* size_t i; */
        nvkey_status_t write_status;
        if (s_enable_adv) {
            /*
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (va_info_array[i].p_callbacks) {
                    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT,
                            va_info_array[i].p_callbacks->on_get_ble_adv_data);
                }
            }
            */
            if (va_info_array[s_va_type].p_callbacks) {
                multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[s_va_type].p_callbacks->on_get_ble_adv_data);
            }
            multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
        }
        apps_config_key_remapper_set_va_type(s_va_type, MULTI_VA_TYPE_UNKNOWN);
        s_va_type = MULTI_VA_TYPE_UNKNOWN;
        write_status = nvkey_write_data(NVKEYID_VA_SWITH, &s_va_type, sizeof(s_va_type));
        if (NVKEY_STATUS_OK != write_status) {
            APPS_LOG_MSGID_I(LOG_TAG"_receive_va_change_from_agent(), Write NVKEYID_VA_SWITH failed %d", 1, write_status);
        }
#if defined(MTK_AWS_MCE_ENABLE) && defined(MULTI_VA_SUPPORT_COMPETITION)
        multi_va_manager_send_va_type_to_partner();
#endif
#endif
#endif
        APPS_LOG_MSGID_I(LOG_TAG"set_inactive_done, wait for disconnected and set adv", 0);

    }
}

void multi_voice_assistant_manager_notify_va_disconnected(multi_va_type_t va_type)
{
    if (s_va_type == va_type) {
        s_va_connected = false;
        APPS_LOG_MSGID_I(LOG_TAG"va_disconnect, need trigger BLE adv", 1, va_type);
        /* When VA disconnected, add the ble adv. */
        if (s_enable_adv) {
            /*
            size_t i;
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (va_type != i && va_info_array[i].p_callbacks) {
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[i].p_callbacks->on_get_ble_adv_data);
                }
            }
            */
            if (va_info_array[s_va_type].p_callbacks) {
                multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[s_va_type].p_callbacks->on_get_ble_adv_data, 1);
            }
            multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
        }
    } else {
        APPS_LOG_MSGID_I(LOG_TAG"va_disconnect, but s_va_type(%d) != va_type(%d)",
                         2, s_va_type, va_type);
    }
}

bool multi_voice_assistant_manager_enable_adv(bool enable)
{
    bool have_va_adv = false;
    APPS_LOG_MSGID_I(LOG_TAG"enable_adv %d -> %d, va_type: %x, connected : %d",
                     4, s_enable_adv, enable, s_va_type, s_va_connected);
    /* If the enable is not change, ignore. */
    if (!s_enable_adv && enable) {
#ifdef MULTI_VA_SUPPORT_COMPETITION
        if (s_va_connected) {
            size_t i;
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (s_va_type != i && va_info_array[i].p_callbacks) {
                    /* Add adv for every VA except the connected VA. */
                    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[i].p_callbacks->on_get_ble_adv_data, 1);
                    have_va_adv = true;
                }
            }
        } else {
            size_t i;
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (/*(s_va_type >= MULTI_VA_TYPE_MAX_NUM || s_va_type == i)
                        && */va_info_array[i].p_callbacks) {
                    /* Add adv for every VA. */
                    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[i].p_callbacks->on_get_ble_adv_data, 1);
                    have_va_adv = true;
                }
            }
        }
#else
        if (s_va_type < MULTI_VA_TYPE_MAX_NUM) {
            if (va_info_array[s_va_type].p_callbacks) {
                /* If not support MULTI_VA_SUPPORT_COMPETITION, add adv for current selected VA. */
                multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[s_va_type].p_callbacks->on_get_ble_adv_data, 1);
                have_va_adv = true;
            }
        }
#endif
    } else if (s_enable_adv && !enable) {
#ifdef MULTI_VA_SUPPORT_COMPETITION
        if (s_va_connected) {
            size_t i;
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (s_va_type != i && va_info_array[i].p_callbacks) {
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[i].p_callbacks->on_get_ble_adv_data);
                    have_va_adv = true;
                }
            }
        } else {
            size_t i;
            for (i = 0; i < MULTI_VA_TYPE_MAX_NUM; i++) {
                if (/*(s_va_type >= MULTI_VA_TYPE_MAX_NUM || s_va_type == i)
                        && */va_info_array[i].p_callbacks) {
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[i].p_callbacks->on_get_ble_adv_data);
                    have_va_adv = true;
                }
            }
        }
#else
        if (s_va_type < MULTI_VA_TYPE_MAX_NUM) {
            if (va_info_array[s_va_type].p_callbacks) {
                multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, va_info_array[s_va_type].p_callbacks->on_get_ble_adv_data);
                have_va_adv = true;
            }
        }
#endif
    }
    s_enable_adv = enable;

    return have_va_adv;
}

void multi_voice_assistant_manager_va_config_changed(void)
{
    uint8_t va_type = MULTI_VA_TYPE_UNKNOWN;    /* The value to read from NVDM. */
    uint32_t va_type_len = sizeof(s_va_type);   /* The length of the value to read from NVDM. */
    nvkey_status_t read_status = nvkey_read_data(NVKEYID_VA_SWITH, &va_type, &va_type_len);
    if (NVKEY_STATUS_OK != read_status || va_type_len != sizeof(s_va_type)) {
        APPS_LOG_MSGID_E(LOG_TAG"_set_va_type(), Read NVKEYID_VA_SWITH failed %d", 1, read_status);
    }
    if (va_type != s_va_type) {
        multi_va_switch_off_return_t switch_off_result = MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;

        APPS_LOG_MSGID_I(LOG_TAG"_set_va_type(), type = %d", 1, va_type);

#ifdef MULTI_VA_SUPPORT_COMPETITION
        if (s_va_type < MULTI_VA_TYPE_MAX_NUM) {
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
            {
                if (va_info_array[s_va_type].p_callbacks) {
                    switch_off_result =
                        va_info_array[s_va_type].p_callbacks->on_voice_assistant_type_switch(false);
                    APPS_LOG_MSGID_I(LOG_TAG"set_va_type(), switch off[%d] result : %d",
                                     2, s_va_type, switch_off_result);
                }
                if (MULTI_VA_SWITCH_OFF_WAIT_INACTIVE == switch_off_result) {
                    removing_va_type = s_va_type;
                }
            }
        } else {
            APPS_LOG_MSGID_I(LOG_TAG"set_va_type(), old type is unknown", 0);
        }
#endif
        s_va_type = va_type;
        if (MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE == switch_off_result) {
            multi_va_manager_reboot();
        }
    } else {
        APPS_LOG_MSGID_W(LOG_TAG"_set_va_type(), va_type not change %d", 1, va_type);
    }
}

multi_va_switch_off_return_t multi_voice_assistant_manager_va_remove_pairing(void)
{
    multi_va_switch_off_return_t ret = MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;

    APPS_LOG_MSGID_I(LOG_TAG"remove_pairing(), old type is %x, connected: %d", 2, s_va_type, s_va_connected);
    if (!s_va_connected) {
        /* If not connected, cannot notify to smart phone, so return immediately.*/
        return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
    }

    if (s_va_type < MULTI_VA_TYPE_MAX_NUM) {
#ifdef MTK_AWS_MCE_ENABLE
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
        {
            multi_va_switch_off_return_t switch_off_result;
            switch_off_result =
                va_info_array[s_va_type].p_callbacks->on_voice_assistant_type_switch(false);
            APPS_LOG_MSGID_I(LOG_TAG"set_va_type(), switch off[%d] result : %d",
                             2, s_va_type, switch_off_result);
            if (MULTI_VA_SWITCH_OFF_WAIT_INACTIVE == switch_off_result) {
                s_removing_from_other_app = true;
                removing_va_type = s_va_type;
            }
            ret = switch_off_result;
        }
    }
    return ret;
}