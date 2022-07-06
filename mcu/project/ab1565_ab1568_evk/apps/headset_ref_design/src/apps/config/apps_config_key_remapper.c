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
 * File: apps_config_key_remapper.c
 *
 * Description: This file support API to get key action. It also supply some other API to assist the target.
 *
 */

#include <stddef.h>

#include "apps_config_key_remapper.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_sink_srv_ami.h"
#include "hal_pmu.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "apps_race_cmd_event.h"
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "apps_race_cmd_co_sys_event.h"
#include "apps_events_event_group.h"
#endif

#include <string.h>

#define LOG_TAG "[Key_remap] "      /* Log tag */
static apps_config_state_t s_mmi_state = APP_BT_OFF;    /* Recored current mmsi state. */


static void _init_default_configurable_table(void);

/** @brief
 * This enum defines serialized key events of airo_key_event_t, because airo_key_event_t is not suitable to read array.
 */
typedef enum {
    APPS_CONFIG_KEY_RELEASE = 0,            /**< serialized value of AIRO_KEY_RELEASE */
    APPS_CONFIG_KEY_SHORT_CLICK,            /**< serialized value of AIRO_KEY_SHORT_CLICK */
    APPS_CONFIG_KEY_DOUBLE_CLICK,           /**< serialized value of AIRO_KEY_DOUBLE_CLICK */
    APPS_CONFIG_KEY_TRIPLE_CLICK,           /**< serialized value of AIRO_KEY_TRIPLE_CLICK */
    APPS_CONFIG_KEY_LONG_PRESS_1,           /**< serialized value of AIRO_KEY_LONG_PRESS_1 */
    APPS_CONFIG_KEY_LONG_PRESS_2,           /**< serialized value of AIRO_KEY_LONG_PRESS_2 */
    APPS_CONFIG_KEY_LONG_PRESS_3,           /**< serialized value of AIRO_KEY_LONG_PRESS_3 */
    APPS_CONFIG_KEY_SLONG,                  /**< serialized value of AIRO_KEY_SLONG */
    APPS_CONFIG_KEY_REPEAT,                 /**< serialized value of AIRO_KEY_REPEAT */
    APPS_CONFIG_KEY_PRESS,                  /**< serialized value of AIRO_KEY_PRESS */
    APPS_CONFIG_KEY_LONG_PRESS_RELEASE_1,   /**< serialized value of AIRO_KEY_LONG_PRESS_RELEASE_1 */
    APPS_CONFIG_KEY_LONG_PRESS_RELEASE_2,   /**< serialized value of AIRO_KEY_LONG_PRESS_RELEASE_2 */
    APPS_CONFIG_KEY_LONG_PRESS_RELEASE_3,   /**< serialized value of AIRO_KEY_LONG_PRESS_RELEASE_3 */
    APPS_CONFIG_KEY_SLONG_RELEASE,          /**< serialized value of AIRO_KEY_SLONG_RELEASE */
    APPS_CONFIG_KEY_DLONG,                  /**< serialized value of AIRO_KEY_DLONG */
    APPS_CONFIG_KEY_DLONG_RELEASE,          /**< serialized value of AIRO_KEY_DLONG_RELEASE */
    APPS_CONFIG_KEY_INVALID,                /**< serialized value of AIRO_KEY_INVALID */
} apps_config_serialized_event_t;

static apps_config_configurable_table_t *s_configurable_table = NULL;   /* Pointer to configurable table. */
static uint8_t s_configurable_table_len = 0;                            /* Length of configurable table. */
static bool s_configurable_table_ready = false;                         /* If the flag is true, do not re-init table. */

/* Add table in a customerized folder */
#include "customerized_key_config.c"

/** @brief
 * This structure defines the item of key mapping table.
 */
typedef struct {
    uint32_t map_size;                          /* Size of the mapping table item. */
    const apps_config_key_event_map_t *key_map; /* Pointer to the mapping table item. */
} apps_config_key_event_map_list_t;

/* Initialize the mapping table, if this is an AWS project, the variable is for right side. */
const static apps_config_key_event_map_list_t const s_map_list[APPS_CONFIG_KEY_INVALID] = {
    {
        sizeof(temp_key_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_release_configs
    },
    {
        sizeof(temp_key_short_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_short_click_configs
    },
    {
        sizeof(temp_key_double_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_double_click_configs
    },
    {
        sizeof(temp_key_triple_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_triple_click_configs
    },
    {
        sizeof(temp_key_long_press1_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press1_configs
    },
    {
        sizeof(temp_key_long_press2_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press2_configs
    },
    {
        sizeof(temp_key_long_press3_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press3_configs
    },
    {
        sizeof(temp_key_slong_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_slong_configs
    },
    {
        sizeof(temp_key_repeat_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_repeat_configs
    },
    {
        sizeof(temp_key_press_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_press_configs
    },
    {
        sizeof(temp_key_long_press_release1_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press_release1_configs
    },
    {
        sizeof(temp_key_long_press_release2_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press_release2_configs
    },
    {
        sizeof(temp_key_long_press_release3_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_long_press_release3_configs
    },
    {
        sizeof(temp_key_slong_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_slong_release_configs
    },
    {
        sizeof(temp_key_dlong_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_dlong_configs
    },
    {
        sizeof(temp_key_dlong_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_key_dlong_release_configs
    },
};


#ifdef MTK_AWS_MCE_ENABLE
/* If it's AWS project, the left earbuds use this map. */
const static apps_config_key_event_map_list_t const s_left_map_list[APPS_CONFIG_KEY_INVALID] = {
    {
        sizeof(temp_left_key_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_release_configs
    },
    {
        sizeof(temp_left_key_short_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_short_click_configs
    },
    {
        sizeof(temp_left_key_double_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_double_click_configs
    },
    {
        sizeof(temp_left_key_triple_click_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_triple_click_configs
    },
    {
        sizeof(temp_left_key_long_press1_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press1_configs
    },
    {
        sizeof(temp_left_key_long_press2_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press2_configs
    },
    {
        sizeof(temp_left_key_long_press3_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press3_configs
    },
    {
        sizeof(temp_left_key_slong_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_slong_configs
    },
    {
        sizeof(temp_left_key_repeat_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_repeat_configs
    },
    {
        sizeof(temp_left_key_press_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_press_configs
    },
    {
        sizeof(temp_left_key_long_press_release1_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press_release1_configs
    },
    {
        sizeof(temp_left_key_long_press_release2_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press_release2_configs
    },
    {
        sizeof(temp_left_key_long_press_release3_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_long_press_release3_configs
    },
    {
        sizeof(temp_left_key_slong_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_slong_release_configs
    },
    {
        sizeof(temp_left_key_dlong_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_dlong_configs
    },
    {
        sizeof(temp_left_key_dlong_release_configs) / sizeof(apps_config_key_event_map_t),
        temp_left_key_dlong_release_configs
    },
};
#endif

apps_config_key_action_t apps_config_key_event_remapper_map_action_in_temp_state(
    uint8_t key_id,
    airo_key_event_t key_event,
    apps_config_state_t temp_mmi_state)
{
    apps_config_key_action_t action_id = KEY_ACTION_INVALID;
    size_t i;
    apps_config_serialized_event_t serialized_event_id = APPS_CONFIG_KEY_INVALID;
    const apps_config_key_event_map_list_t *mapped_list = NULL;
    audio_channel_t channel = 0;
#ifdef MTK_AWS_MCE_ENABLE
    channel = ami_get_audio_channel();
#endif

    /* Convert the key event from airo_key_event_t to apps_config_serialized_event_t. */
    switch (key_event) {
        case AIRO_KEY_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_RELEASE;
            break;
        case AIRO_KEY_SHORT_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_SHORT_CLICK;
            break;
        case AIRO_KEY_DOUBLE_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_DOUBLE_CLICK;
            break;
        case AIRO_KEY_TRIPLE_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_TRIPLE_CLICK;
            break;
        case AIRO_KEY_LONG_PRESS_1:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_1;
            break;
        case AIRO_KEY_LONG_PRESS_2:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_2;
            break;
        case AIRO_KEY_LONG_PRESS_3:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_3;
            break;
        case AIRO_KEY_SLONG:
            serialized_event_id = APPS_CONFIG_KEY_SLONG;
            break;
        case AIRO_KEY_REPEAT:
            serialized_event_id = APPS_CONFIG_KEY_REPEAT;
            break;
        case AIRO_KEY_PRESS:
            serialized_event_id = APPS_CONFIG_KEY_PRESS;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_1:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_1;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_2:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_2;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_3:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_3;
            break;
        case AIRO_KEY_SLONG_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_SLONG_RELEASE;
            break;
        case AIRO_KEY_DLONG:
            serialized_event_id = APPS_CONFIG_KEY_DLONG;
            break;
        case AIRO_KEY_DLONG_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_DLONG_RELEASE;
            break;
        case AIRO_KEY_INVALID:
            serialized_event_id = APPS_CONFIG_KEY_INVALID;
            break;
        default:
            break;
    }

    if (serialized_event_id < APPS_CONFIG_KEY_INVALID) {
        /* If the configurable table have not been initialized from system on, initilize it. */
        if (!s_configurable_table && !s_configurable_table_ready) {
            _init_default_configurable_table();
        }
        /* The configurable table have higher priority than the hard code table. */
        if (s_configurable_table) {
            for (i = 0; i < s_configurable_table_len; i++) {
                if (serialized_event_id == s_configurable_table[i].key_event
                    && key_id == s_configurable_table[i].key_id
                    && ((1 << temp_mmi_state) & s_configurable_table[i].supported_states)) {
                    action_id = s_configurable_table[i].app_key_event;
                    APPS_LOG_MSGID_I(LOG_TAG"Current used key_id from configuration_table = 0x%x, key_event = 0x%x, state = %d, action_id = 0x%x, channel = %d", 5, key_id, key_event, temp_mmi_state, action_id, channel);
                    return action_id;
                }
            }
        }

#if MTK_AWS_MCE_ENABLE
        /* When device is left, use the left mapping tables */
        if (AUDIO_CHANNEL_L == channel) {
            mapped_list = &s_left_map_list[serialized_event_id];
        } else
#endif
        {
            /* Use the default(right side) mapping tables */
            mapped_list = &s_map_list[serialized_event_id];
        }
        /* For debug log
        APPS_LOG_MSGID_I(LOG_TAG"Current used key map [%d]", 1, serialized_event_id);
        for (j = 0; j < s_map_list[serialized_event_id].map_size; j++) {
            APPS_LOG_MSGID_I(LOG_TAG"%d, key_id = %x,  app_key_event = %x, states = %x", 4, j, s_map_list[serialized_event_id].key_map[j].key_id,
            s_map_list[serialized_event_id].key_map[j].app_key_event,
            s_map_list[serialized_event_id].key_map[j].supported_states);
        }
        */

        for (i = 0; i < mapped_list->map_size && mapped_list->key_map; i++) {
            if (key_id == mapped_list->key_map[i].key_id
                && ((1 << temp_mmi_state) & mapped_list->key_map[i].supported_states)) {
                action_id = mapped_list->key_map[i].app_key_event;
                break;
            }
        }
    }
    APPS_LOG_MSGID_I(LOG_TAG"Current used key_id = 0x%x, key_event = 0x%x, state = %d, action_id = 0x%x, channel = %d", 5, key_id, key_event, temp_mmi_state, action_id, channel);

    return action_id;
}

apps_config_key_action_t apps_config_key_event_remapper_map_action(
    uint8_t key_id,
    airo_key_event_t key_event)
{
    return apps_config_key_event_remapper_map_action_in_temp_state(key_id, key_event, s_mmi_state);
}

void apps_config_key_set_mmi_state(apps_config_state_t state)
{
#ifdef MTK_RACE_CMD_ENABLE
    if (s_mmi_state != state) {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_MMI_STATE,
                                        &state, sizeof(uint8_t), false);
#endif
        app_race_notify_mmi_state(state);
    }
#endif
    s_mmi_state = state;
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] apps_config_key_set_mmi_state = %d", 1, s_mmi_state);
}

apps_config_state_t apps_config_key_get_mmi_state(void)
{
    return s_mmi_state;
}

/**
 * @brief      Get the target key mapping table by VA type.
 * @param[in]  va_type, the VA type.
 * @param[out] p_target_table, the pointer to the mapping table.
 * @param[out] p_table_size, the pointer to the mapping table size.
 */
static void apps_config_key_get_va_key_table(multi_va_type_t va_type,
                                            const apps_config_configurable_table_t **p_target_table,
                                            uint32_t *p_table_size)
{
    *p_target_table = NULL;
    *p_table_size = 0;
#if MTK_AWS_MCE_ENABLE
    audio_channel_t channel = ami_get_audio_channel();
#endif
    switch (va_type) {
#ifdef MTK_AMA_ENABLE
        case MULTI_VA_TYPE_AMA:
            *p_target_table = ama_configurable_table;
            *p_table_size = sizeof(ama_configurable_table);
            break;
#endif
#ifdef GSOUND_LIBRARY_ENABLE
        case MULTI_VA_TYPE_GSOUND:
            *p_target_table = gsound_configurable_table;
            *p_table_size = sizeof(gsound_configurable_table);
            break;
#endif
#ifdef MTK_VA_XIAOWEI_ENABLE
        case MULTI_VA_TYPE_XIAOWEI:
            *p_target_table = xiaowei_configurable_table;
            *p_table_size = sizeof(xiaowei_configurable_table);
            break;
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
        case MULTI_VA_TYPE_XIAOAI:
            *p_target_table = xiaoai_configurable_table;
            *p_table_size = sizeof(xiaoai_configurable_table);
            break;
#endif
        case MULTI_VA_TYPE_UNKNOWN:
#if MTK_AWS_MCE_ENABLE
            if (AUDIO_CHANNEL_L == channel) {
                *p_target_table = left_configurable_table;
                *p_table_size = sizeof(left_configurable_table);
            } else if (AUDIO_CHANNEL_R == channel) {
                *p_target_table = default_configurable_table;
                *p_table_size = sizeof(default_configurable_table);
            }
#else
            *p_target_table = default_configurable_table;
            *p_table_size = sizeof(default_configurable_table);
#endif
            break;
        default:
            APPS_LOG_MSGID_E(LOG_TAG"get_va_key_table, param error : %x", 1, va_type);
            break;
    }
}

/**
 * @brief      Ready nvkey to get configurable table, if the nvkey is empty, write nvkey to hardcode value.
 */
static void _init_default_configurable_table(void)
{
    const apps_config_configurable_table_t *hard_code_table = NULL; /* The pointer to hardcode configurable table. */
    uint32_t hard_code_table_size = 0;
    const apps_config_configurable_table_t *va_table = NULL;
    uint32_t va_table_size = 0;
    nvkey_status_t nvkey_ret;
    /* When not support COMPETITION, need init key by VA type. */
    apps_config_key_get_va_key_table(multi_va_manager_get_current_va_type(), &va_table, &va_table_size);
#if MTK_AWS_MCE_ENABLE
    const apps_config_configurable_table_t *peer_code_table = NULL; /* Hard code table of peer side. */
    uint32_t peer_code_table_size = 0;
    audio_channel_t channel = ami_get_audio_channel();
    /* There are 2 hard code configurable tables -- default_configurable_table and left_configurable_table.
        Set the pointer depends on current device is left or right. */
    if (AUDIO_CHANNEL_L == channel) {
        hard_code_table = va_table;
        hard_code_table_size = va_table_size;
        peer_code_table = default_configurable_table;
        peer_code_table_size = sizeof(default_configurable_table);
    } else if (AUDIO_CHANNEL_R == channel) {
        hard_code_table = default_configurable_table;
        hard_code_table_size = sizeof(default_configurable_table);
        peer_code_table = va_table;
        peer_code_table_size = va_table_size;
    } else {
        /* Maybe user forget to set the channel. */
        APPS_LOG_MSGID_E(LOG_TAG"_init_default_configurable_table current audio channel not correct", 0);
    }
#else
    /* If not support AWS, use default configurable table. */
    hard_code_table = va_table;
    hard_code_table_size = va_table_size;
#endif
    if (hard_code_table) {
        if (hard_code_table_size) {
            s_configurable_table = (apps_config_configurable_table_t *)pvPortMalloc(hard_code_table_size);
            if (s_configurable_table) {
                memcpy(s_configurable_table, hard_code_table, hard_code_table_size);
                s_configurable_table_len = hard_code_table_size / sizeof(apps_config_configurable_table_t);
                nvkey_ret = nvkey_write_data(NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE,
                                             (uint8_t *)s_configurable_table,
                                             hard_code_table_size);
                APPS_LOG_MSGID_E(LOG_TAG"_init_default_configurable_table init ret = %d, size = %d",
                                 2, nvkey_ret, s_configurable_table_len);
            }
        }
        s_configurable_table_ready = true;
    }
#if MTK_AWS_MCE_ENABLE
    if (peer_code_table && peer_code_table_size) {
        nvkey_ret = nvkey_write_data(NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE,
                                     (uint8_t *)peer_code_table,
                                     peer_code_table_size);
        APPS_LOG_MSGID_E(LOG_TAG"_init_peer_configurable_table init ret = %d, size = %d",
                         2, nvkey_ret, peer_code_table_size);
    }
#endif
}

void apps_config_key_remaper_init_configurable_table(void)
{
    uint32_t size = 0;
    bool push_to_work = false;
    uint32_t i;
    nvkey_status_t ret = nvkey_data_item_length(NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE, &size);

    APPS_LOG_MSGID_I(LOG_TAG"apps_config_key_remaper_init_configurable_table", 0);
    if (NVKEY_STATUS_ITEM_NOT_FOUND == ret || size < sizeof(apps_config_configurable_table_t)) {
        /* When read nvkey failed, intialize configurable table from hardcode. */
        _init_default_configurable_table();
    } else if (NVKEY_STATUS_OK == ret) {
        if (s_configurable_table
            && size != s_configurable_table_len * sizeof(apps_config_configurable_table_t)) {
            vPortFree(s_configurable_table);
            s_configurable_table = NULL;
        }
        if (!s_configurable_table) {
            s_configurable_table = (apps_config_configurable_table_t *)pvPortMalloc(size);
        }
        s_configurable_table_len = 0;
        if (s_configurable_table) {
            ret = nvkey_read_data(NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE,
                                  (uint8_t *)s_configurable_table, &size);
            if (NVKEY_STATUS_OK == ret) {
                s_configurable_table_len = size / (sizeof(apps_config_configurable_table_t));
                APPS_LOG_MSGID_I(LOG_TAG"apps_config_key_remaper_init_configurable_table success, s_configurable_table_len = %d",
                                 1, s_configurable_table_len);
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"apps_config_key_remaper_init_configurable_table, read data fail : %d", 1, ret);
                vPortFree(s_configurable_table);
                s_configurable_table = NULL;
            }
        } else {
            APPS_LOG_MSGID_E(LOG_TAG"apps_config_key_remaper_init_configurable_table, malloc fail", 0);
        }
    } else {
        APPS_LOG_MSGID_W(LOG_TAG"apps_config_key_remaper_init_configurable_table, read size ret = %d", 1, ret);
        if (s_configurable_table) {
            vPortFree(s_configurable_table);
            s_configurable_table = NULL;
            s_configurable_table_len = 0;
        }
    }

#if defined(AIR_SLIDE_POWER_KEY_ENABLE)
    push_to_work = true;
#else
    /* Special code. Because GSOUND need push to talk, must disable HW power off when press power key. */
    for (i = 0; i < s_configurable_table_len; i++) {
        if (APPS_CONFIG_KEY_LONG_PRESS_3 == s_configurable_table[i].key_event
            && DEVICE_KEY_POWER == s_configurable_table[i].key_id
            && KEY_ACTION_INVALID == s_configurable_table[i].app_key_event) {
            push_to_work = true;
            break;
        }
    }
#endif
    if (push_to_work) {
        /* Disable long press power key to do HW power off. */
        pmu_pwrkey_enable(PMU_OFF);
    } else {
        /* Enable long press power key to do HW power off. */
        pmu_pwrkey_enable(PMU_ON);
    }
}

#if defined(MTK_AWS_MCE_ENABLE) && defined(MULTI_VA_SUPPORT_COMPETITION)
/**
 * @brief      Check the configurable table, return the VA mode if the table contains key actions for this VA mode.
 * @param[in]  table, the pointer to the configurable table.
 * @param[in]  table_len, The length of the table..
 * @return     What type of VA mode.
 */
static multi_va_type_t apps_config_key_remapper_get_key_table_va_type(apps_config_configurable_table_t *table, uint32_t table_len)
{
    uint32_t i;
    multi_va_type_t table_va_type = MULTI_VA_TYPE_UNKNOWN;
    /* If there is any key action in the table is used to control one kind of VA, the mode is this VA type. */
    for (i = 0; i < table_len; i++) {
        switch (table[i].app_key_event) {
#ifdef GSOUND_LIBRARY_ENABLE
            case KEY_GSOUND_PRESS:
            case KEY_GSOUND_RELEASE:
            case KEY_GSOUND_NOTIFY:
            case KEY_GSOUND_VOICE_QUERY:
            case KEY_GSOUND_CANCEL:
                table_va_type = MULTI_VA_TYPE_GSOUND;
                break;
#endif
#ifdef MTK_AMA_ENABLE
            case KEY_AMA_START:
            case KEY_AMA_START_NOTIFY:
            case KEY_AMA_STOP:
            case KEY_AMA_MEDIA_CONTROL:
            case KEY_AMA_LONG_PRESS_TRIGGER_START:
            case KEY_AMA_LONG_PRESS_TRIGGER_STOP:
                table_va_type = MULTI_VA_TYPE_AMA;
                break;
#endif
#ifdef MTK_VA_XIAOWEI_ENABLE
            case KEY_VA_XIAOWEI_START:
            case KEY_VA_XIAOWEI_START_NOTIFY:
            case KEY_VA_XIAOWEI_STOP_PLAY:
            case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START:
            case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP:
                table_va_type = MULTI_VA_TYPE_XIAOWEI;
                break;
#endif
            default:
                break;
        }
        if (MULTI_VA_TYPE_UNKNOWN != table_va_type) {
            APPS_LOG_MSGID_I(LOG_TAG"keyAction type is: %x, action:0x%x", 2,
                             table_va_type, table[i].app_key_event);
            break;
        }
    }
    return table_va_type;
}
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
void apps_config_key_remapper_dual_chip_sync_key_table(void)
{
    app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, 
                                   APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_SYNC_KEY_TABLE,
                                   s_configurable_table,
                                   s_configurable_table_len * sizeof(apps_config_configurable_table_t),
                                   false);
}
#endif

void apps_config_key_remapper_set_configurable_key_table(const apps_config_configurable_table_t *table, uint32_t data_size, bool peer_table)
{
    nvkey_status_t ret;
    const apps_config_configurable_table_t *target_table;
    apps_config_configurable_table_t temp_table[1];
    if (table == NULL || data_size == 0) {
        memset(temp_table, 0, sizeof(temp_table));
        data_size = sizeof(temp_table);
        target_table = temp_table;
    } else {
        target_table = table;
    }
    ret = nvkey_write_data(peer_table ? NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE : NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE,
                           (uint8_t *)target_table, data_size);
    APPS_LOG_MSGID_I(LOG_TAG"set_configurable_key_table, table : %x, size: %d, peer:%d", 3, target_table, data_size, peer_table);
    if (NVKEY_STATUS_OK == ret) {
        apps_config_key_remaper_init_configurable_table();
    } else {
        APPS_LOG_MSGID_E(LOG_TAG"set_configurable_key_table, write nvkey fail : %d", 1, ret);
    }
}

#ifdef MULTI_VA_SUPPORT_COMPETITION
void apps_config_key_remapper_set_va_type(multi_va_type_t old_va_type,  multi_va_type_t new_va_type)
{
    const apps_config_configurable_table_t *target_table = NULL;
#if MTK_AWS_MCE_ENABLE
    nvkey_status_t ret;
#endif
    uint32_t table_size = 0;
    bool need_update_self = true;
    APPS_LOG_MSGID_E(LOG_TAG"set_va_type, %x->%x", 2, old_va_type, new_va_type);
#if MTK_AWS_MCE_ENABLE
    bool need_update_peer = true;
    apps_config_configurable_table_t *temp_table;
    multi_va_type_t key_type_self = MULTI_VA_TYPE_UNKNOWN;
    multi_va_type_t key_type_peer = new_va_type;
    audio_channel_t channel = ami_get_audio_channel();
    ret = nvkey_data_item_length(NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE, &table_size);
    if (NVKEY_STATUS_OK == ret && table_size > 0) {
        temp_table = (apps_config_configurable_table_t *)pvPortMalloc(table_size);
        if (temp_table) {
            ret = nvkey_read_data(NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE, (uint8_t *)temp_table, &table_size);
            if (NVKEY_STATUS_OK == ret) {
                key_type_peer = apps_config_key_remapper_get_key_table_va_type(
                                    temp_table,
                                    table_size / sizeof(apps_config_configurable_table_t));
            }
            vPortFree(temp_table);
        }
    }
    key_type_self = apps_config_key_remapper_get_key_table_va_type(
                        s_configurable_table,
                        s_configurable_table_len);

    // When current key_type is not VA type, not need to change
    if (key_type_self != old_va_type || MULTI_VA_TYPE_UNKNOWN == old_va_type) {
        need_update_self = false;
    }
    if (key_type_peer != old_va_type || MULTI_VA_TYPE_UNKNOWN == old_va_type) {
        need_update_peer = false;
    }
    if (new_va_type != MULTI_VA_TYPE_UNKNOWN && !need_update_self && !need_update_peer) {
        // When new va is active, at least set LEFT key table.
        if (AUDIO_CHANNEL_L == channel) {
            need_update_self = true;
        } else if (AUDIO_CHANNEL_R == channel) {
            need_update_peer = true;
        } else {
            APPS_LOG_MSGID_E(LOG_TAG"audio channel is not set, cannot set VA key table", 0);
        }
    }
    APPS_LOG_MSGID_I(LOG_TAG"audio channel %x, key_table: %x,%x, need update: %x,%x", 5,
                     channel, key_type_self, key_type_peer, need_update_self, need_update_peer);

#endif
    apps_config_key_get_va_key_table(new_va_type, &target_table, &table_size);

    if (target_table && need_update_self) {
        apps_config_key_remapper_set_configurable_key_table(target_table, table_size, false);
    }
#if MTK_AWS_MCE_ENABLE
    if (target_table && need_update_peer) {
        if (MULTI_VA_TYPE_UNKNOWN == new_va_type) {
            target_table = AUDIO_CHANNEL_L == channel ? default_configurable_table : left_configurable_table;
            table_size = (AUDIO_CHANNEL_L == channel ? sizeof(default_configurable_table) : sizeof(left_configurable_table));
        }
        ret = nvkey_write_data(NVKEYID_PEER_CONFIGURABLE_KEY_ACTION_TABLE, (uint8_t *)target_table, table_size);
        APPS_LOG_MSGID_E(LOG_TAG"set_va_type, peer table : %x, size: %d", 2, target_table, table_size);
        if (NVKEY_STATUS_OK == ret) {
            apps_config_key_remaper_init_configurable_table();
        } else {
            APPS_LOG_MSGID_E(LOG_TAG"set_va_type, write peer nvkey fail : %d", 1, ret);
        }
    }
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    apps_config_key_remapper_dual_chip_sync_key_table();
#endif
}
#endif

#ifdef MTK_VA_XIAOAI_ENABLE
apps_config_key_action_t apps_config_key_get_mapper_action(
    uint8_t key_id,
    airo_key_event_t key_event,
    apps_config_state_t mmi_state,
    bool left_earbud)
{
    apps_config_key_action_t action_id = KEY_ACTION_INVALID;
    size_t i;
    apps_config_serialized_event_t serialized_event_id = APPS_CONFIG_KEY_INVALID;
    const apps_config_key_event_map_list_t *mapped_list = NULL;

    /* Convert the key event from airo_key_event_t to apps_config_serialized_event_t. */
    switch (key_event) {
        case AIRO_KEY_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_RELEASE;
            break;
        case AIRO_KEY_SHORT_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_SHORT_CLICK;
            break;
        case AIRO_KEY_DOUBLE_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_DOUBLE_CLICK;
            break;
        case AIRO_KEY_TRIPLE_CLICK:
            serialized_event_id = APPS_CONFIG_KEY_TRIPLE_CLICK;
            break;
        case AIRO_KEY_LONG_PRESS_1:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_1;
            break;
        case AIRO_KEY_LONG_PRESS_2:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_2;
            break;
        case AIRO_KEY_LONG_PRESS_3:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_3;
            break;
        case AIRO_KEY_SLONG:
            serialized_event_id = APPS_CONFIG_KEY_SLONG;
            break;
        case AIRO_KEY_REPEAT:
            serialized_event_id = APPS_CONFIG_KEY_REPEAT;
            break;
        case AIRO_KEY_PRESS:
            serialized_event_id = APPS_CONFIG_KEY_PRESS;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_1:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_1;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_2:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_2;
            break;
        case AIRO_KEY_LONG_PRESS_RELEASE_3:
            serialized_event_id = APPS_CONFIG_KEY_LONG_PRESS_RELEASE_3;
            break;
        case AIRO_KEY_SLONG_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_SLONG_RELEASE;
            break;
        case AIRO_KEY_DLONG:
            serialized_event_id = APPS_CONFIG_KEY_DLONG;
            break;
        case AIRO_KEY_DLONG_RELEASE:
            serialized_event_id = APPS_CONFIG_KEY_DLONG_RELEASE;
            break;
        case AIRO_KEY_INVALID:
            serialized_event_id = APPS_CONFIG_KEY_INVALID;
            break;
        default:
            break;
    }

    if (serialized_event_id < APPS_CONFIG_KEY_INVALID) {
        /* The configurable table have higher priority than the hard code table. */
        if (s_configurable_table) {
            for (i = 0; i < s_configurable_table_len; i++) {
                if (serialized_event_id == s_configurable_table[i].key_event
                    && key_id == s_configurable_table[i].key_id
                    && ((1 << mmi_state) & s_configurable_table[i].supported_states)) {
                    action_id = s_configurable_table[i].app_key_event;
                    APPS_LOG_MSGID_I(LOG_TAG" [XIAOAI] [left=%d] configurable_table key_id=0x%x key_event=0x%x mmi_state=%d -> action_id=0x%04X",
                                    5, left_earbud, key_id, key_event, mmi_state, action_id);
                    return action_id;
                }
            }
        }

        if (left_earbud) {
            mapped_list = &s_left_map_list[serialized_event_id];
        } else {
            mapped_list = &s_map_list[serialized_event_id];
        }

        for (i = 0; i < mapped_list->map_size && mapped_list->key_map; i++) {
            if (key_id == mapped_list->key_map[i].key_id
                && ((1 << mmi_state) & mapped_list->key_map[i].supported_states)) {
                action_id = mapped_list->key_map[i].app_key_event;
                break;
            }
        }
    }
    APPS_LOG_MSGID_I(LOG_TAG" [XIAOAI] [left=%d] key_id=0x%x key_event=0x%x mmi_state=%d -> action_id=0x%04X",
                    5, left_earbud, key_id, key_event, mmi_state, action_id);
    return action_id;
}
#endif
