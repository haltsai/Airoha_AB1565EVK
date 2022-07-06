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
 * File: app_gsound_service.c
 *
 * Description: This file is to handle the GSound implementation functions.
 */


#ifdef GSOUND_LIBRARY_ENABLE
#include "bt_callback_manager.h"
#if defined MTK_GSOUND_AIR_SPP_ENABLE
#include "gsound_spp_air_interface.h"
#endif
#include "FreeRTOS.h"
#include "app_gsound_idle_activity.h"
#include "app_gsound_service.h"
#include "gsound_port_interface.h"
#include "gsound_service.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "ble_ancs_app.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "multi_va_manager.h"
#include "nvdm.h"
#include "nvkey_id_list.h"
#include "nvkey.h"
#include "nvdm_id_list.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "gsound_device_action.pb.h"
#include "app_gsound_idle_activity.h"
#include "bt_sink_srv_ami.h"
#include "bt_device_manager.h"
#include "hal_feature_config.h"
#include "apps_race_cmd_event.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#ifdef MTK_ANC_ENABLE
#include "anc_control_api.h"
#endif
#include "gsound_debug.h"

#define TAG "[GS][APP][SRV] "

/**************************************************************************************************
* Define
**************************************************************************************************/
#define APP_GSOUND_VP_SYNC_TIME         100

#define APP_GSOUND_CHANNEL_CONTROL      0x01 /* GSound connection channel for control */
#define APP_GSOUND_CHANNEL_AUDIO        0x02 /* GSound connection channel for audio */
#define APP_GSOUND_CHANNEL_BITMASK      0x03

#define GSOUND_INITIAL_STATE_DISABLED   0
#define GSOUND_INITIAL_STATE_ENABLED    1

#define MODULE_SN_LENGTH_SIZE           5
#define VER_LENGTH_SIZE                 1

#define DEVICE_ACTION_STATE_KEY_BATTERY "battery_details"
#define DEVICE_ACTION_STATE_KEY_ALL "all"

/**
 * @brief The data in NVRAM of GSound.
 */
typedef struct {
    uint32_t module_id;
    uint8_t  serial_num_length;
    uint8_t  serial_num[1];
    uint8_t  version_length;
    uint8_t  version[1];
} __attribute__((packed)) app_gsound_info_nvkey_t;

typedef struct {
    uint8_t * volatile key_all;
    uint8_t * volatile key_battery;
} app_gsound_state_key_t;

/**************************************************************************************************
* Prototype
**************************************************************************************************/
/**
 * @brief Callback by Libgsound, to notify GSound has been enabled.
 */
static void callback_gsound_enabled(void);

/**
 * @brief Callback by Libgsound, to notify GSound has been disabled.
 */
static void callback_gsound_disabled(void);

/**
 * @brief Clear GSound channel.
 */
static void app_gsound_reset_parameter(void);

/**
 * @brief GSound's VP index mapping to device's VP index.
 * param[in] clip, GSound's VP index to be played.
 */
static uint32_t app_gsound_map_vp_index(uint8_t clip);

/**
 * @brief Write GSound initial state which will be read at the next time power on.
 * param[in] state, initial state of GSound.
 */
static void app_gsound_write_initial_state(uint8_t state);

/**************************************************************************************************
* Variable
**************************************************************************************************/
static GSoundServiceObserver observer = {
    callback_gsound_enabled,
    callback_gsound_disabled
};

static GSoundServiceConfig gsound_config;          /* The parameter of GSoundServiceInit, to notify Libgsound the initial state of GSound. */
static bool gsound_ignore_false_alarm = true;      /* Used to ignore the first callback_gsound_enabled or callback_gsound_disabled. Because they will be called right after we give initial state of GSound to Libgsound but the connection was not established at that time. */
static uint8_t gsound_on_connected_channel = 0;    /* To record the control or audio channel has been connected. */
static uint8_t gsound_on_disconnected_channel = 0; /* To record the control or audio channel has been disconnected. */
static uint8_t app_gsound_enable_state = APP_GSOUND_DISABLED;
static volatile app_gsound_state_key_t g_app_gsound_device_action_state_key = {NULL, NULL};
#ifdef MTK_ANC_ENABLE
static audio_anc_control_type_t g_app_gsound_anc_type;
#endif

/**************************************************************************************************
* Prototype
**************************************************************************************************/
static void app_gsound_device_action_noise_cancellation(double volume);
static void app_gsound_device_action_ambient_mode(double volume);
static void app_gsound_device_action_turn_off(bool on_off);
static void app_gsound_device_action_touch_control(bool on_off);
static int32_t app_gsound_device_action_get_noise_cancellation_level(void);
static int32_t app_gsound_device_action_get_ambient_mode_level(void);
static bool app_gsound_device_action_is_touch_control_enabled(void);

/**************************************************************************************************
* Static function
**************************************************************************************************/
static void callback_gsound_enabled(void)
{
    GSOUND_LOG_I(TAG"GSound Enabled cb, false_alarm=0x%X", 1, gsound_ignore_false_alarm);

    app_gsound_enable_state = APP_GSOUND_ENABLED;

    if (gsound_ignore_false_alarm) {
        /* While GSound init (gsound_enabled = true), callback_gsound_enabled() will be executed. Ignore the 1st callback. */
        gsound_ignore_false_alarm = false;
        return;
    }

    app_gsound_write_initial_state(GSOUND_INITIAL_STATE_ENABLED);
    app_gsound_reset_parameter();

#ifdef MTK_AWS_MCE_ENABLE
    if (BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role()) {
        return;
    }
#endif

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                        APPS_EVENTS_INTERACTION_GSOUND_ENABLED,
                        NULL, 0, NULL, 0);
}

static void callback_gsound_disabled(void)
{
    GSOUND_LOG_I(TAG"GSound Disabled cb, false_alarm=0x%X", 1, gsound_ignore_false_alarm);

    app_gsound_enable_state = APP_GSOUND_DISABLED;

    if (gsound_ignore_false_alarm) {
        /* While GSound init (gsound_enabled = false), callback_gsound_disabled() will be executed. Ignore the 1st callback. */
        gsound_ignore_false_alarm = false;
        return;
    }

    app_gsound_write_initial_state(GSOUND_INITIAL_STATE_DISABLED);
    app_gsound_reset_parameter();

#ifdef MTK_AWS_MCE_ENABLE
    if (BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role()) {
        return;
    }
#endif

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                        APPS_EVENTS_INTERACTION_GSOUND_DISABLED,
                        NULL, 0, NULL, 0);
}

void gsound_bt_on_connected_callback(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    if ((MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type()) ||
        (BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role()))
#else
    if (MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type())
#endif
    {
        return;
    }

    GSOUND_LOG_I(TAG"BT Connected cb", 0);

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                        APPS_EVENTS_INTERACTION_GSOUND_CONNECTED,
                        NULL, 0, NULL, 0);
}

void gsound_bt_on_disconnected_callback(uint8_t channel)
{
#ifdef MTK_AWS_MCE_ENABLE
    if ((MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type()) ||
        (BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role()))
#else
    if (MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type())
#endif
    {
        return;
    }

    GSOUND_LOG_I(TAG"BT Disconnected cb channel:0x%X", 1, gsound_on_disconnected_channel);

    gsound_on_disconnected_channel |= ((channel == GSOUND_CHANNEL_CONTROL) ? APP_GSOUND_CHANNEL_CONTROL : APP_GSOUND_CHANNEL_AUDIO);

    if (gsound_on_disconnected_channel == APP_GSOUND_CHANNEL_BITMASK) {
        gsound_on_disconnected_channel = 0;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                            APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED,
                            NULL, 0, NULL, 0);
    }
}

void gsound_ble_on_connected_callback(uint8_t channel)
{
#ifdef MTK_AWS_MCE_ENABLE
    if ((MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type()) ||
        (BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role()))
#else
    if (MULTI_VA_TYPE_GSOUND != multi_va_manager_get_current_va_type())
#endif
    {
        return;
    }

    GSOUND_LOG_I(TAG"BLE Connected cb channel:0x%X", 1, gsound_on_connected_channel);

    gsound_on_connected_channel |= ((channel == GSOUND_CHANNEL_CONTROL) ? APP_GSOUND_CHANNEL_CONTROL : APP_GSOUND_CHANNEL_AUDIO);

    if (gsound_on_connected_channel == APP_GSOUND_CHANNEL_BITMASK) {
        gsound_on_connected_channel = 0;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                            APPS_EVENTS_INTERACTION_GSOUND_CONNECTED,
                            NULL, 0, NULL, 0);
    }
}

void gsound_ble_on_disconnected_callback(uint8_t channel)
{
    gsound_bt_on_disconnected_callback(channel);
}

void gsound_rho_send_prepare_ready(void)
{
    GSOUND_LOG_I(TAG"gsound_rho_send_prepare_ready", 0);
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_GSOUND,
                        APPS_EVENTS_INTERACTION_GSOUND_RHO_PREPARE_READY,
                        NULL, 0, NULL, 0);
#endif
}

static void app_gsound_reset_parameter(void)
{
    gsound_on_connected_channel = 0;
    gsound_on_disconnected_channel = 0;
}

static uint32_t app_gsound_map_vp_index(uint8_t clip)
{
    uint32_t vp_index = 0xFFFFFFFF;
    switch (clip) {
        //case GSOUND_PORT_AUDIO_OUT_CLIP_PREPARE_MIC_OPEN:
        case GSOUND_PORT_AUDIO_OUT_CLIP_FETCH:
        case GSOUND_PORT_AUDIO_OUT_CLIP_PTT_QUERY:
        case GSOUND_PORT_AUDIO_OUT_CLIP_REMOTE_MIC_OPEN:
            vp_index = VP_INDEX_BISTO_MIC_OPEN;
            break;
        case GSOUND_PORT_AUDIO_OUT_CLIP_QUERY_INTERRUPTED:
        case GSOUND_PORT_AUDIO_OUT_CLIP_PTT_REJECTED:
        case GSOUND_PORT_AUDIO_OUT_CLIP_REMOTE_MIC_CLOSE:
        case GSOUND_PORT_AUDIO_OUT_CLIP_PTT_MIC_CLOSE:
            vp_index = VP_INDEX_BISTO_MIC_CLOSE;
            break;
        case GSOUND_PORT_AUDIO_OUT_CLIP_GSOUND_NC:
            vp_index = VP_INDEX_BISTO_MIC_NOT_CONNECTED;
            break;
        default:
            break;
    }
    return vp_index;
}

static bool app_gsound_read_initial_state(bool selected)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    uint32_t size = 1;
    uint8_t initial_state = GSOUND_INITIAL_STATE_DISABLED;
    bool update_nvdm = false;
#endif

    /* GSound default initial state: Disable */
    /* [selected]-->true: GSound has been chosen before last power off. */

#ifdef MTK_NVDM_ENABLE
    status = nvdm_read_data_item(NVDM_GROUP_GSOUND, NVDM_GSOUND_ITEM_INIT_STATE, (uint8_t *)&initial_state, &size);

    GSOUND_LOG_I(TAG"NVDM Read status=0x%X, initial_state=0x%X", 2, status, initial_state);

    if (selected) {
        if ((status == NVDM_STATUS_ITEM_NOT_FOUND) ||
            ((status == NVDM_STATUS_OK) && (initial_state == GSOUND_INITIAL_STATE_DISABLED))) {
            initial_state = GSOUND_INITIAL_STATE_ENABLED;
            update_nvdm = true;
        }

    } else {
        if ((status == NVDM_STATUS_OK) && (initial_state == GSOUND_INITIAL_STATE_ENABLED)) {
            initial_state = GSOUND_INITIAL_STATE_DISABLED;
            update_nvdm = true;
        }
    }

    if (update_nvdm) {
        app_gsound_write_initial_state(initial_state);
    }
#endif

    GSOUND_LOG_I(TAG"Init! (selected=0x%X)", 1, selected);

    return selected;
}

static void app_gsound_write_initial_state(uint8_t initial_state)
{
#ifdef MTK_NVDM_ENABLE
    nvdm_status_t status;
    uint32_t size = 1;
    status = nvdm_write_data_item(NVDM_GROUP_GSOUND, NVDM_GSOUND_ITEM_INIT_STATE, NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                  (uint8_t *)&initial_state, size);

    if (status != NVDM_STATUS_OK) {
        GSOUND_LOG_E(TAG"NVDM Write error=0x%X", 1, status);
    }
#endif
}

/**************************************************************************************************
* Public function
**************************************************************************************************/
void gsound_port_ota_reboot(void)
{
    GSOUND_LOG_I(TAG"gsound_port_ota_reboot", 0);

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_REQUEST_REBOOT, NULL, 0,
                        NULL, 0);
}

// return 0 means VP enable success, else fail
uint32_t gsound_port_audio_out_start(uint8_t clip)
{
    uint32_t result = 0;
    uint32_t vp_index;

    GSOUND_LOG_I(TAG"gsound_port_audio_out_start=%d", 1, clip);

    if ((vp_index = app_gsound_map_vp_index(clip)) != 0xFFFFFFFF) {
        GSOUND_LOG_I(TAG"app_gsound_map_vp_index=0x%X", 1, vp_index);
        apps_config_set_vp(app_gsound_map_vp_index(clip), true, APP_GSOUND_VP_SYNC_TIME, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    }

    return result;
}

void app_gsound_enable(void)
{
    GSoundStatus status;

    status = GSoundServiceEnable();

    if(status == GSOUND_STATUS_OK)
    {
        app_gsound_enable_state = APP_GSOUND_ENABLING;
    }

    GSOUND_LOG_I(TAG"GSound Enable, status=0x%X", 1, status);
}

void app_gsound_disable(void)
{
    GSoundStatus status;

    status = GSoundServiceDisable();

    if(status == GSOUND_STATUS_OK)
    {
        app_gsound_enable_state = APP_GSOUND_DISABLING;
    }

    GSOUND_LOG_I(TAG"GSound Disable, status=0x%X", 1, status);
}

bt_status_t gsound_port_info_get_serial(uint8_t *serial_num, uint32_t max_len)
{
    uint32_t nvkey_size = 0;
    uint8_t *sn = NULL, sn_length;
    app_gsound_info_nvkey_t *data;
    nvkey_status_t result = NVKEY_STATUS_ERROR;

    if (nvkey_data_item_length(NVKEYID_APP_GSOUND_INFO, &nvkey_size) != NVKEY_STATUS_OK) {
        GSOUND_LOG_E(TAG"get_serial length error", 0);
        return BT_STATUS_FAIL;
    }

    if ((data = pvPortMalloc(nvkey_size)) != NULL && (result = nvkey_read_data(NVKEYID_APP_GSOUND_INFO, (uint8_t *)data, &nvkey_size)) == NVKEY_STATUS_OK) {
        sn_length = data->serial_num_length;
        sn = (uint8_t *)data;

        GSOUND_LOG_I(TAG"gsound_port_info_get_serial=0x%X,0x%X", 2, data->module_id, sn_length);

        memcpy(serial_num, &sn[0 + MODULE_SN_LENGTH_SIZE], (sn_length > max_len) ? max_len : sn_length);
    }

    if (data != NULL) {
        vPortFree(data);
    }

    return (result == NVKEY_STATUS_OK) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t gsound_port_info_get_app_version(uint8_t *version, uint32_t max_len)
{
    uint32_t nvkey_size = 0;
    uint8_t *ver = NULL, ver_length, sn_length;
    app_gsound_info_nvkey_t *data;
    nvkey_status_t result = NVKEY_STATUS_ERROR;

    if (nvkey_data_item_length(NVKEYID_APP_GSOUND_INFO, &nvkey_size) != NVKEY_STATUS_OK) {
        GSOUND_LOG_I(TAG"get_app_version length error", 0);
        return BT_STATUS_FAIL;
    }

    if ((data = pvPortMalloc(nvkey_size)) != NULL && (result = nvkey_read_data(NVKEYID_APP_GSOUND_INFO, (uint8_t *)data, &nvkey_size)) == NVKEY_STATUS_OK) {
        sn_length = data->serial_num_length;
        ver = (uint8_t *)data;
        ver_length = ver[0 + MODULE_SN_LENGTH_SIZE + sn_length];
        GSOUND_LOG_I(TAG"gsound_port_info_get_app_version:0x%X, 0x%X, 0x%X", 3, data->module_id, sn_length, ver_length);

        memcpy(version, &ver[0 + MODULE_SN_LENGTH_SIZE + sn_length + VER_LENGTH_SIZE], (ver_length > max_len) ? max_len : ver_length);

    }

    if (data != NULL) {
        vPortFree(data);
    }

    return (result == NVKEY_STATUS_OK) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

bt_status_t gsound_port_info_get_device_id(uint32_t *device_id)
{
    uint32_t nvkey_size = 0;
    app_gsound_info_nvkey_t *data;
    nvkey_status_t result = NVKEY_STATUS_ERROR;

    if (nvkey_data_item_length(NVKEYID_APP_GSOUND_INFO, &nvkey_size) != NVKEY_STATUS_OK) {
        GSOUND_LOG_E(TAG"get_device_id length error", 0);
        return BT_STATUS_FAIL;
    }

    if ((data = pvPortMalloc(nvkey_size)) != NULL && (result = nvkey_read_data(NVKEYID_APP_GSOUND_INFO, (uint8_t *)data, &nvkey_size)) == NVKEY_STATUS_OK) {
        *device_id = data->module_id;
        GSOUND_LOG_I(TAG"gsound_port_info_get_device_id=0x%X", 1, data->module_id);
    }

    if (data != NULL) {
        vPortFree(data);
    }

    return (result == NVKEY_STATUS_OK) ? BT_STATUS_SUCCESS : BT_STATUS_FAIL;
}

void app_gsound_init(bool va_selected)
{
#ifdef MTK_GSOUND_AIR_SPP_ENABLE
    bt_gsound_spp_air_main();
#endif

    gsound_config.gsound_enabled = app_gsound_read_initial_state(va_selected);
    GSoundServiceInit(GSOUND_BUILD_ID, &gsound_config, &observer);

    gsound_port_register_callback();
}

uint8_t app_gsound_get_enable_state(void)
{
    return app_gsound_enable_state;
}

#include "memory_map.h"
#ifndef LM_BASE
#define LM_BASE 0x1233
#endif

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
#include "va_model_manager.h"
#endif

uint32_t gsound_get_language_model_length(void)
{
#ifndef MTK_VA_MODEL_MANAGER_ENABLE
    uint32_t len = 0xB9A0;
    GSOUND_LOG_I(TAG"gsound_get_language_mode_length=0x%X", 1, len);
    return len;
#else
    va_model_manager_model_info_t model_info = {0};
    va_model_manager_error_code_t error = va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_GVA, &model_info);
    if (error != VA_MODEL_MANAGER_ERROR_CODE_OK) {
        GSOUND_LOG_I(TAG"gsound_get_language_mode_length, failed to get current running model = %d", 1, error);
        return 0;
    }
    return model_info.model_length;
#endif
}
uint32_t gsound_get_language_model_address(void)
{
#ifndef MTK_VA_MODEL_MANAGER_ENABLE
    GSOUND_LOG_I(TAG"gsound_get_language_mode_address=0x%X", 1, LM_BASE + 0x1000);
    return LM_BASE + 0x1000;
#else
    va_model_manager_model_info_t model_info = {0};
    va_model_manager_error_code_t error = va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_GVA, &model_info);
    if (error != VA_MODEL_MANAGER_ERROR_CODE_OK) {
        GSOUND_LOG_E(TAG"gsound_get_language_mode_length, failed to get current running model = %d", 1, error);
        return 0;
    }
    GSOUND_LOG_I(TAG"gsound_get_language_mode_address=0x%X", 1, model_info.model_flash_address);
    return model_info.model_flash_address;
#endif
}

#ifdef MTK_ANC_ENABLE
int32_t app_gsound_device_action_get_anc_level(audio_anc_control_type_t type)
{
    uint8_t anc_enable;
    audio_anc_control_filter_id_t anc_current_filter_id;
    audio_anc_control_type_t anc_current_type;
    int16_t anc_runtime_gain;
    uint8_t support_hybrid_enable;
    audio_anc_control_misc_t control_misc;

    audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, &support_hybrid_enable, &control_misc);
    GSOUND_LOG_I(TAG"get_anc_status. input_type=%d, enable=%d, filterId=%d, type=%d, gain=%d dB, hybridEnable=%d, sub_state=%d", 7,
        type, anc_enable, anc_current_filter_id, anc_current_type, anc_runtime_gain, support_hybrid_enable, control_misc.sub_state);

    if(control_misc.sub_state == AUDIO_ANC_CONTROL_SUB_STATE_WAITING_ENABLE ||
        (anc_enable && control_misc.sub_state != AUDIO_ANC_CONTROL_SUB_STATE_WAITING_DISABLE))
    {
        return (type == anc_current_type) ? 1 : 0;
    }
    else
    {
        return 0;
    }
}

void app_gsound_device_action_turn_on_noise_cancellation(void)
{
    uint8_t anc_enable;
    audio_anc_control_filter_id_t anc_current_filter_id;
    audio_anc_control_type_t anc_current_type;
    int16_t anc_runtime_gain;
    uint8_t support_hybrid_enable;

    audio_anc_control_result_t         control_ret;
    audio_anc_control_filter_id_t      target_filter_id;
    audio_anc_control_type_t           target_anc_type;

    audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, &support_hybrid_enable, NULL);

    if(support_hybrid_enable){
        target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
    }else{
        target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
    }

    g_app_gsound_anc_type = target_anc_type;

    target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
    control_ret = audio_anc_control_set_runtime_gain(AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, target_anc_type); // If user wants to change runtime gain, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN can skip this action.
    control_ret = audio_anc_control_enable(target_filter_id, target_anc_type, NULL);
    GSOUND_LOG_I(TAG"turn_on_noise_cancellation status=%d", 1, control_ret);
}

void app_gsound_device_action_turn_off_noise_cancellation_ambient_mode(void)
{
    audio_anc_control_result_t control_ret;
    uint8_t anc_enable;
    audio_anc_control_filter_id_t anc_current_filter_id;
    audio_anc_control_type_t anc_current_type;
    int16_t anc_runtime_gain;

    audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, NULL, NULL);
    control_ret = audio_anc_control_set_status_into_flash(anc_enable, anc_current_filter_id, anc_current_type, anc_runtime_gain, NULL);
    control_ret = audio_anc_control_disable(NULL);
    GSOUND_LOG_I(TAG"turn_off_noise_cancellation_ambient_mode status=%d", 1, control_ret);
}

void app_gsound_device_action_turn_on_ambient_mode(void)
{
    audio_anc_control_result_t control_ret;
    audio_anc_control_filter_id_t target_filter_id;
    audio_anc_control_type_t target_anc_type;

    target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
    target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
    control_ret = audio_anc_control_set_runtime_gain(AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, target_anc_type); // If user wants to change runtime gain, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN can skip this action.
    control_ret = audio_anc_control_enable(target_filter_id, target_anc_type, NULL);
    GSOUND_LOG_I(TAG"turn_on_ambient_mode status=%d", 1, control_ret);
}
#endif

/**************************************************************************************************
* Public function (Device action)
**************************************************************************************************/
static bool decode_battery(pb_istream_t *stream, const pb_field_t *field, void **arg) //to check encoded data
{
    BatteryDetails *bat = (BatteryDetails *)(*arg);

    if (!nano_pb_decode(stream, BatteryDetails_fields, bat)) {
        GSOUND_LOG_TEXT(TAG"decode_battery error=%s", PB_GET_ERROR(stream));
        return false;
    }

    *arg = bat + 1;
    return true;
}

void decode_check(const uint8_t *device_actions, int device_actions_size) //to check encoded data
{
    BatteryDetails bat[3];
    CurrentDeviceState state_key_all = CurrentDeviceState_init_default;
    pb_istream_t stream = nano_pb_istream_from_buffer(device_actions, device_actions_size);

    GSOUND_LOG_I(TAG"decode_check=0x%X", 1, device_actions_size);


    state_key_all.battery_details.funcs.decode = decode_battery;
    state_key_all.battery_details.arg = bat;

    if (!(nano_pb_decode(&stream, CurrentDeviceState_fields, &state_key_all))) {
        GSOUND_LOG_TEXT(TAG"decode_check error = %s", PB_GET_ERROR(&stream));
        return;
    }

    GSOUND_LOG_I(TAG"bat 1: %d, %d, %d", 3, bat[0].remaining_battery_percentage, bat[0].connected, bat[0].device_type);
    GSOUND_LOG_I(TAG"bat 2: %d, %d, %d", 3, bat[1].remaining_battery_percentage, bat[1].connected, bat[1].device_type);
    GSOUND_LOG_I(TAG"bat 3: %d, %d, %d", 3, bat[2].remaining_battery_percentage, bat[2].connected, bat[2].device_type);
    GSOUND_LOG_I(TAG"noise = %d, ambient = %d, touch = %d", 3,
               state_key_all.noise_cancellation_level, state_key_all.ambient_mode_level, state_key_all.touch_control_enabled);
}

static bool app_gsound_device_action_battery_encode(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
    uint8_t i;
    BatteryDetails *bat = (BatteryDetails *)*arg;

#ifdef MTK_AWS_MCE_ENABLE
    for (i = 0; i < 2; i++)
#else
    i = 0;
#endif
    {
        if (!nano_pb_encode_tag_for_field(stream, field)) {
            GSOUND_LOG_TEXT(TAG"device_action_battery_encode tag error %d: %s", i, PB_GET_ERROR(stream));
            return false;
        }

        if (!nano_pb_encode_submessage(stream, BatteryDetails_fields, bat + i)) {
            GSOUND_LOG_TEXT(TAG"device_action_battery_encode error %d: %s", i, PB_GET_ERROR(stream));
            return false;
        }
    }

    GSOUND_LOG_I(TAG"device_action_battery_encode success", 0);
    return true;
}

static uint8_t *app_gsound_device_action_pb_encode(uint32_t *buf_len)
{
    uint8_t *buf;
    pb_ostream_t stream = PB_OSTREAM_SIZING;
    pb_ostream_t stream_dummy = PB_OSTREAM_SIZING;
    CurrentDeviceState state_key = CurrentDeviceState_init_default;

    bool is_right = (AUDIO_CHANNEL_R == ami_get_audio_channel() ? true : false);
    uint32_t bat_local_percent = app_gsound_get_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT);
#ifdef MTK_AWS_MCE_ENABLE
    uint32_t bat_partner_percent = app_gsound_get_battery_info(APP_GSOUND_BATTERY_INFO_PARTNER_PERCENT);
    bt_aws_mce_srv_link_type_t aws_status = bt_aws_mce_srv_get_link_type();
#endif

    // 1. set value
#ifdef MTK_AWS_MCE_ENABLE
    BatteryDetails bat[2] = {
        {
            (is_right) ? bat_partner_percent : bat_local_percent,
            (is_right) ? (aws_status != BT_AWS_MCE_SRV_LINK_NONE) : true,
            BatteryDetails_DeviceType_LEFT_EAR_BUD
        },

        {
            (is_right) ? bat_local_percent : bat_partner_percent,
            (is_right) ? true : (aws_status != BT_AWS_MCE_SRV_LINK_NONE),
            BatteryDetails_DeviceType_RIGHT_EAR_BUD
        },
        //{89, true, BatteryDetails_DeviceType_EAR_BUD_CASE},
    };
#else
    BatteryDetails bat[1] = {
        {bat_local_percent, true, BatteryDetails_DeviceType_SINGLE_BATTERY_DEVICE},
    };
#endif
    GSOUND_LOG_I(TAG"bat_local_percent=%d", 1, bat_local_percent);

    state_key.battery_details.funcs.encode = app_gsound_device_action_battery_encode;
    state_key.battery_details.arg = bat;
    state_key.noise_cancellation_level = app_gsound_device_action_get_noise_cancellation_level();
    state_key.ambient_mode_level = app_gsound_device_action_get_ambient_mode_level();
    #ifdef GSOUND_HOTWORD_ENABLE
    state_key.touch_control_enabled = app_gsound_device_action_is_touch_control_enabled();
    #else
    state_key.touch_control_enabled = true;
    #endif

    // 2. get encoded length
    if (!nano_pb_encode(&stream_dummy, CurrentDeviceState_fields, &state_key)) {
        GSOUND_LOG_I(TAG"device_action_get_encode_length error", 0);
        return NULL;
    }
    *buf_len = stream_dummy.bytes_written;

    // 3. allocate buffer memory
    if (!(buf = pvPortMalloc(*buf_len))) {
        GSOUND_LOG_I(TAG"device_action_pb_encode error no memory. buf_len=%d", 1, buf_len);
        return NULL;
    }

    // 4. set buffer to stream and encode
    stream = nano_pb_ostream_from_buffer(buf, *buf_len);
    if (!nano_pb_encode(&stream, CurrentDeviceState_fields, &state_key)) {
        GSOUND_LOG_TEXT(TAG"device_action_pb_encode error=%s", PB_GET_ERROR(&stream));
        vPortFree(buf);
        return NULL;
    }

    GSOUND_LOG_I(TAG"device_action_pb_encode success buf_len=%d", 1, *buf_len);
    //decode_check(buf, *buf_len); //test to check the encode is correct

    return buf;
}

bt_status_t gsound_port_device_action_state_update(void)
{
    uint8_t *state_key_all;
    uint32_t state_len_all;

    GSOUND_LOG_I(TAG"gsound_port_device_action_state_update=0x%X", 1, g_app_gsound_device_action_state_key.key_all);

    if (g_app_gsound_device_action_state_key.key_all == NULL && (state_key_all = app_gsound_device_action_pb_encode(&state_len_all))) {
        g_app_gsound_device_action_state_key.key_all = state_key_all;
        if (BT_STATUS_SUCCESS == gsound_port_device_action_send_state_key(DEVICE_ACTION_STATE_KEY_ALL, strlen(DEVICE_ACTION_STATE_KEY_ALL), state_key_all, state_len_all)) {
            ;
        } else {
            g_app_gsound_device_action_state_key.key_all = NULL;
            vPortFree(state_key_all);
            return BT_STATUS_FAIL;
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t gsound_port_device_action_state_comsume(uint8_t *action_state_key)
{
    if (!strcmp(action_state_key, DEVICE_ACTION_STATE_KEY_ALL)) {
        GSOUND_LOG_I(TAG"gsound_port_device_action_state_comsume: all, 0x%X", 1, g_app_gsound_device_action_state_key.key_all);
        if (g_app_gsound_device_action_state_key.key_all) {
            vPortFree(g_app_gsound_device_action_state_key.key_all);
            g_app_gsound_device_action_state_key.key_all = NULL;
        }
    }

    return BT_STATUS_SUCCESS;
}

bool app_gsound_device_action_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    char *str = (char *)(*arg);

    GSOUND_LOG_I(TAG"app_gsound_device_action_decode_string", 0);

    if (!nano_pb_read(stream, str, stream->bytes_left)) {
        GSOUND_LOG_TEXT(TAG"device_action_decode_string error=%s", PB_GET_ERROR(stream));
        return false;
    }

    GSOUND_LOG_TEXT(TAG"device_action_decode_string success=%s", str);

    return true;
}

bool app_gsound_device_action_decode_param(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    Execution_ParamsEntry *param = (Execution_ParamsEntry *)(*arg);
    char key[20];

    GSOUND_LOG_I(TAG"app_gsound_device_action_decode_param", 0);

    param->key.funcs.decode = app_gsound_device_action_decode_string;
    param->key.arg = key;

    if (!nano_pb_decode(stream, Execution_ParamsEntry_fields, param)) {
        GSOUND_LOG_TEXT(TAG"device_action_decode_param error=%s", PB_GET_ERROR(stream));
        return false;
    }

    GSOUND_LOG_I(TAG"device_action_decode_param success kind=%d", 1, param->value.which_kind);

    return true;
}

static void app_gsound_device_action_command_handler(char *command, Execution_ParamsEntry *param)
{
    if(!strcmp(command, "action.devices.commands.headphone.UpdateNoiseCancellation"))
    {
        uint8_t *ptr = (uint8_t *)&(param->value.kind.number_value);

        GSOUND_LOG_I(TAG"device_action_command_handler NoiseCancellation: 0x%X,%X,%X,%X,%X,%X,%X,%X", 8,
            ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);

        app_gsound_device_action_noise_cancellation(param->value.kind.number_value);
    }
    else if(!strcmp(command, "action.devices.commands.headphone.UpdateAmbientMode"))
    {
        uint8_t *ptr = (uint8_t *)&(param->value.kind.number_value);

        GSOUND_LOG_I(TAG"device_action_command_handler AmbientMode: 0x%X,%X,%X,%X,%X,%X,%X,%X", 8,
            ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5], ptr[6], ptr[7]);

        app_gsound_device_action_ambient_mode(param->value.kind.number_value);
    }
    else if(!strcmp(command, "action.devices.commands.OnOff"))
    {
        GSOUND_LOG_I(TAG"device_action_command_handler OnOff = 0x%X", 1, param->value.kind.bool_value);
        app_gsound_device_action_turn_off(param->value.kind.bool_value);
    }
    else if(!strcmp(command, "action.devices.commands.UpdateTouchControl"))
    {
        GSOUND_LOG_I(TAG"device_action_command_handler TouchControl = 0x%X", 1, param->value.kind.bool_value);
        app_gsound_device_action_touch_control(param->value.kind.bool_value);
    }
}

bt_status_t gsound_port_device_action_notify(const uint8_t *device_actions, int device_actions_size)
{
    char command[60] = {0};
    Execution execution = Execution_init_zero;
    Execution_ParamsEntry param = Execution_ParamsEntry_init_zero;
    pb_istream_t stream = nano_pb_istream_from_buffer(device_actions, device_actions_size);

    GSOUND_LOG_I(TAG"gsound_port_device_action_notify = 0x%X", 1, device_actions_size);

    execution.command.funcs.decode = app_gsound_device_action_decode_string;
    execution.command.arg = command;
    execution.params.funcs.decode = app_gsound_device_action_decode_param;
    execution.params.arg = &param;

    if (!nano_pb_decode(&stream, Execution_fields, &execution)) {
        GSOUND_LOG_TEXT(TAG"gsound_port_device_action_notify decode error=%s, device_actions_size=%d", PB_GET_ERROR(&stream), device_actions_size);
        return BT_STATUS_FAIL;
    }

    GSOUND_LOG_I(TAG"gsound_port_device_action_notify decode OK. device_actions_size=%d", 1, device_actions_size);

    app_gsound_device_action_command_handler(command, &param);

    return BT_STATUS_SUCCESS;
}

bool app_gsound_is_push_to_talk_enabled(void)
{
    bool ptt_enabled;
    bool suspend_by_ld = gsound_port_audio_is_suspend_by_ld();

    ptt_enabled = (!suspend_by_ld);
    if(!ptt_enabled)
    {
        GSOUND_LOG_I(TAG"push_to_talk disabled. LD=%d", 1, suspend_by_ld);
    }

    return ptt_enabled;
}


/**************************************************************************************************
* Device actions control functions (Customers can implement by theirself)
**************************************************************************************************/
static void app_gsound_device_action_noise_cancellation(double volume)
{
    /*
       Implementation: Control noise cancellation and set it's level
       volume: 0: Close, >0: Open with volume, -1: Open with the last time volume
    */
#ifdef MTK_ANC_ENABLE
    if(volume != 0)
    {
        app_gsound_device_action_turn_off_noise_cancellation_ambient_mode();
        app_gsound_device_action_turn_on_noise_cancellation();
    }
    else
    {
        app_gsound_device_action_turn_off_noise_cancellation_ambient_mode();
    }
#endif
    /* Update the status to GSound immediately */
    gsound_port_device_action_state_update();
}

static void app_gsound_device_action_ambient_mode(double volume)
{
    /*
       Implementation: Control ambient mode and set it's level
       volume: 0: Close, >0: Open with volume, -1: Open with the last time volume
    */
#ifdef MTK_ANC_ENABLE
    if(volume != 0)
    {
        app_gsound_device_action_turn_off_noise_cancellation_ambient_mode();
        app_gsound_device_action_turn_on_ambient_mode();
    }
    else
    {
        app_gsound_device_action_turn_off_noise_cancellation_ambient_mode();
    }
#endif
    /* Update the status to GSound immediately */
    gsound_port_device_action_state_update();
}

static void app_gsound_device_action_turn_off(bool on_off)
{
    /*
       Implementation: Trun off the device
       on_off: Should be only false
    */
    if(!on_off)
    {
#if defined(MTK_AWS_MCE_ENABLE)
        uint32_t status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_POWER_OFF_SYNC);
        GSOUND_LOG_I(TAG"app_gsound_device_action_turn_off send aws status=0x%X", 1, status);
#endif
        bool *need_rho = (bool*)pvPortMalloc(sizeof(bool));
        if (need_rho == NULL) {
            GSOUND_LOG_E(TAG"malloc failed", 0);
            return;
        }
        *need_rho = false;
        ui_shell_status_t sta = ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF, (void *)need_rho, sizeof(bool),
                NULL, 0);
        if (sta != UI_SHELL_STATUS_OK) {
            vPortFree(need_rho);
            GSOUND_LOG_E(TAG"send event failed", 0);
        }

    }
}
static void app_gsound_device_action_touch_control(bool on_off)
{
    /*
       Implementation: Control touch control
       on_off: true: enable (device can be triggered by button)
               false: disable (device can only be triggered by hotword)
    */

    if(on_off == true)
    {
        uint8_t status = apps_set_touch_control_enable(true, true);
        GSOUND_LOG_I(TAG"set touch control on=%d", 1, status);

    }
    else
    {
        uint8_t status = apps_set_touch_control_enable(false, true);
        GSOUND_LOG_I(TAG"set touch control off=%d", 1, status);
    }

    /* Update the status to GSound immediately */
    gsound_port_device_action_state_update();
}

static int32_t app_gsound_device_action_get_noise_cancellation_level(void)
{
    /* Implementation: Return noise cancellation level */
#ifdef MTK_ANC_ENABLE
    int32_t level = app_gsound_device_action_get_anc_level(g_app_gsound_anc_type);
    GSOUND_LOG_I(TAG"get_noise_cancellation_level. level=%d", 1, level);
    return level;
#else
    return 0;
#endif
}

static int32_t app_gsound_device_action_get_ambient_mode_level(void)
{
    /* Implementation: Return ambient mode level */
#ifdef MTK_ANC_ENABLE
    int32_t level = app_gsound_device_action_get_anc_level(AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF);
    GSOUND_LOG_I(TAG"get_ambient_mode_level. level=%d", 1, level);
    return level;
#else
    return 0;
#endif
}

static bool app_gsound_device_action_is_touch_control_enabled(void)
{
    /* Implementation: Return
       true: if touch control is enabled (device can be triggered by button)
       false: if touch control is disabled (device can only be triggered by hotword)
    */
    uint8_t enable = apps_get_touch_control_status();
    GSOUND_LOG_I(TAG"is_touch_control_enabled=%d", 1, enable); //app key get touch status
    return enable;
}

#endif

