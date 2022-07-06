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

#include "apps_config_event_list.h"
#include "apps_aws_sync_event.h"
#include "apps_config_vp_manager.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include "apps_events_battery_event.h"
#include "apps_events_event_group.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"

#include "app_ama_activity.h"
#include "app_ama_audio.h"
#include "app_voice_prompt.h"
#include "apps_config_vp_index_list.h"
#include "multi_va_manager.h"
#include "bt_connection_manager.h"
#include "app_rho_idle_activity.h"
#include "bt_sink_srv_music.h"
#include "app_bt_takeover_service.h"

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
#include "va_model_manager.h"
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif /* MTK_ANC_ENABLE */

/* Alexa APP Launch Feature. */
#if defined(AMA_IAP2_VIA_MUX_ENABLE) && defined(MTK_MUX_ENABLE)
#include "mux_iap2.h"
#endif

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
#include "app_adaptive_anc_idle_activity.h"
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_service.h"
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */

#ifdef MTK_AMA_ENABLE

typedef enum {
    APP_AMA_UI_SHELL_EVENT_ID_HANDLE_ANC_CHANGE                 = 0x0100,
    APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE         = 0x0101,
    APP_AMA_UI_SHELL_EVENT_ID_MAX
} app_ama_ui_shell_event_id_t;

#ifdef MTK_VA_MODEL_MANAGER_ENABLE

uint8_t  wwe_language[VA_MODEL_MANAGER_MODEL_LANGUAGE_LENGTH] = {0};

#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

#ifdef MTK_ANC_ENABLE
#define APP_AMA_TARGET_ANC_NONE        AUDIO_ANC_CONTROL_TYPE_DUMMY
#define APP_AMA_TARGET_ANC_DISABLE     (-1)

typedef struct {
    int32_t                             target_anc_type;
    uint8_t                             info_anc_enabled;
    audio_anc_control_filter_id_t       info_anc_filter_id;
    audio_anc_control_type_t            info_anc_type;
    int16_t                             info_anc_runtime_gain;
    uint8_t                             info_anc_support_hybrid;
    audio_anc_control_misc_t            info_anc_control_misc;
    bool                                info_anc_init_done;
} app_ama_anc_info_t;

/**
 * @brief The ANC feateure information.
 *
 */
app_ama_anc_info_t      app_ama_anc_info;
#endif /* MTK_ANC_ENABLE */

static void app_ama_media_control_handler(AMA_ISSUE_MEDIA_CONTROL_IND_T *ind);
static void app_ama_activity_notify_state_change(AMA_VALUE_TYPE_E type, AMA_STATE_FEATURE_E feature, uint32_t value);
static bool app_ama_is_ull_upstreaming();

extern bt_status_t bt_sink_srv_set_must_play_tone_flag(bt_bd_addr_t *sp_addr, bt_sink_srv_notification_voice_t type, bool is_open);

ama_context_t ama_activity_context = {0};

/**
 * @brief Need configure the WWE voice data address according to the WWE mode
 * Need change by model manager.
 */
uint32_t wwe_flash_address = 0x00;
uint32_t wwe_flash_length = 0x00;

bool app_ama_is_valid_address(uint8_t *addr)
{
    uint8_t temp_addr[BT_BD_ADDR_LEN] = {0};
    memset(temp_addr, 0, sizeof(uint8_t) * BT_BD_ADDR_LEN);
    if (memcmp(temp_addr, addr, BT_BD_ADDR_LEN) == 0) {
        return false;
    }
    return true;
}

typedef struct {
    char            target_locale[8];
    char            expected_locale[8];
} local_mapping_t;

/**
 * @brief If wish to change (add, remove, modify) the locale mapping rule, please modify this table.
 *
 */
static const local_mapping_t local_mapping[] = {
    {"de-DE", "de-DE"},
    {"en-AU", "en-AU"},
    {"en-US", "en-CA"},
    {"en-GB", "en-GB"},
    {"en-IN", "en-IN"},
    {"en-US", "en-US"},
    {"es-ES", "es-ES"},
    {"es-MX", "es-MX"},
    {"es-ES", "es-US"}, // mapping es-US to be es-ES to improve FRR performance
    {"fr-CA", "fr-CA"},
    {"fr-FR", "fr-FR"},
    {"it-IT", "it-IT"},
    {"ja-JP", "ja-JP"},
    {"pt-BR", "pt-BR"},
    {"en-IN", "hi-IN"},
};

#define LOCAL_MAPPING_TABLE_LEN     (sizeof(local_mapping) / sizeof(local_mapping_t))
/**
 * @brief
 *
 */
char *app_ama_match_locale(char *language)
{
    uint8_t index = 0;
    for (index = 0; index < LOCAL_MAPPING_TABLE_LEN; index ++) {
        if (strcmp(language, local_mapping[index].expected_locale) == 0) {
            return (char *)(local_mapping[index].target_locale);
        }
    }
    return NULL;
}

void app_ama_handle_get_locales_ind(AMA_GET_LOCALES_IND_T *get_locales_ind)
{
    if (get_locales_ind == NULL) {
        return;
    }
#ifdef MTK_VA_MODEL_MANAGER_ENABLE
    AMA_GetLocaleInfoRsp_T locale_rsp = { 0 };
    AMA_LOCALE_NAME_T *supported_locale_list;
    va_model_manager_model_info_t cur_model;

    unsigned char model_count = 0;
    unsigned char model_index = 0;

    va_model_manager_error_code_t err_code = va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_AMA, &cur_model);
    va_model_manager_model_info_t *model_list = va_model_manager_list_models(VA_MODEL_MANAGER_VA_TYPE_AMA, &model_count);

    if (model_list == NULL || model_count == 0) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_handle_get_locales_ind, model list is NULL (%x) or empty (%d)", 2, model_list, model_count);
        AMA_Target_GetLocalesResponse(&(get_locales_ind->bdAddr), NULL, AMA_ERROR_CODE_UNSUPPORTED);
        if (model_list != NULL) {
            vPortFree(model_list);
            model_list = NULL;
        }
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_get_locales_ind, current configured model count : %d", 1, model_count);

    supported_locale_list = (AMA_LOCALE_NAME_T *)pvPortMalloc(sizeof(AMA_LOCALE_NAME_T) * model_count);
    if (supported_locale_list == NULL) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_handle_get_locales_ind, Failed to allocate memory for supported model list", 0);
        AMA_Target_GetLocalesResponse(&(get_locales_ind->bdAddr), NULL, AMA_ERROR_CODE_UNSUPPORTED);

        vPortFree(model_list);
        model_list = NULL;
        return;
    }
    memset(supported_locale_list, 0, sizeof(AMA_LOCALE_NAME_T) * model_count);

    for (model_index = 0; model_index < model_count; model_index ++) {
        if (strlen((char*)(model_list[model_index].model_language)) > (sizeof(AMA_LOCALE_NAME_T) - 1)) {
            memcpy((void*)(supported_locale_list[model_index].name), (void*)(model_list[model_index].model_language), (sizeof(AMA_LOCALE_NAME_T) - 1));
        } else {
            memcpy((void*)(supported_locale_list[model_index].name), (void*)(model_list[model_index].model_language), strlen((char*)(model_list[model_index].model_language)));
        }
    }

    locale_rsp.supported_locale_info = supported_locale_list;
    locale_rsp.supported_locale_count = model_count;

    // locale_rsp.current_locale.locale_id = AMA_INVALID_LOCALE_ID;

    if (err_code == VA_MODEL_MANAGER_ERROR_CODE_OK) {
        if (strlen((char*)(cur_model.model_language)) > (sizeof(AMA_LOCALE_NAME_T) - 1)) {
            memcpy((void*)(locale_rsp.current_locale.name), (void*)(cur_model.model_language), (sizeof(AMA_LOCALE_NAME_T) - 1));
        } else {
            memcpy((void*)(locale_rsp.current_locale.name), (void*)(cur_model.model_language), strlen((char*)(cur_model.model_language)));
        }
    }
    // APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_get_locales_ind, Current configure locale ID : %d", 1, locale_rsp.current_locale.locale_id);

    AMAStatus rsp_status = AMA_Target_GetLocalesResponse(&(get_locales_ind->bdAddr), &locale_rsp, AMA_ERROR_CODE_SUCCESS);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_get_locales_ind, Response GetLocales result : %d", 1, rsp_status);

    vPortFree(supported_locale_list);
    supported_locale_list = NULL;

    vPortFree(model_list);
    model_list = NULL;

#else /* MTK_VA_MODEL_MANAGER_ENABLE */
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_get_locales_ind, not define MTK_VA_MODEL_MANAGER_ENABLE", 0);
    AMA_Target_GetLocalesResponse(&(get_locales_ind->bdAddr), NULL, AMA_ERROR_CODE_UNSUPPORTED);
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */
}

void app_ama_handle_set_locale_ind(AMA_SET_LOCALE_IND_T *set_locale_ind)
{
    if (set_locale_ind == NULL) {
        return;
    }
    if (strlen(set_locale_ind->locale.name) == 0) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, invalid language to configure, length is 0", 0);
        return;
    }
#ifdef MTK_VA_MODEL_MANAGER_ENABLE
    va_model_manager_model_info_t load_info;
    unsigned char language[VA_MODEL_MANAGER_MODEL_LANGUAGE_LENGTH] = {0};
    va_model_manager_error_code_t load_err = VA_MODEL_MANAGER_ERROR_CODE_OK;
    va_model_manager_error_code_t err = VA_MODEL_MANAGER_ERROR_CODE_OK;
    AMA_ERROR_CODE_E resp_status = AMA_ERROR_CODE_SUCCESS;
    // APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_handle_set_locale_ind, locale id : 0x%02x", 1, set_locale_ind->locale_id);

    char *target_locale = app_ama_match_locale(set_locale_ind->locale.name);
    if (target_locale == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, failed to mapping locale to the expected locale. 0x%02x %02x %02x %02x %02x %02x", 6,
                        set_locale_ind->locale.name[0], set_locale_ind->locale.name[1], set_locale_ind->locale.name[2],
                        set_locale_ind->locale.name[3], set_locale_ind->locale.name[4], set_locale_ind->locale.name[5]);
        AMA_Target_SetLocaleResponse(&(set_locale_ind->bdAddr), AMA_ERROR_CODE_INVALID);
        return;
    }

    memcpy(language, target_locale, strlen(target_locale));

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, The language to configure, 0x%02x %02x %02x %02x %02x %02x %02x %02x", 8,
                                language[0], language[1], language[2], language[3], language[4], language[5], language[6], language[7]);

    load_err = va_model_manager_load(VA_MODEL_MANAGER_VA_TYPE_AMA, language, &load_info);
    err = va_model_manager_load_finish(VA_MODEL_MANAGER_VA_TYPE_AMA, language);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, load result : %d, load finish result : %d", 2, load_err, err);

    if (load_err != VA_MODEL_MANAGER_ERROR_CODE_OK) {
        resp_status = AMA_ERROR_CODE_UNSUPPORTED;
    } else {
        if (load_info.model_flash_address == 0x00 || load_info.model_length == 0x00) {
            resp_status = AMA_ERROR_CODE_UNSUPPORTED;
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, flash address (0x%08x), length (0x%08x) error",
                                2, load_info.model_flash_address, load_info.model_length);
        } else {
            resp_status = AMA_ERROR_CODE_SUCCESS;
            wwe_flash_address = load_info.model_flash_address;
            wwe_flash_length = load_info.model_length;
            memcpy(wwe_language, language, VA_MODEL_MANAGER_MODEL_LANGUAGE_LENGTH);
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, Succeed to load model with address : 0x%08x (0x%08x), length : 0x%08x (0x%08x)", 4,
                                    load_info.model_flash_address, wwe_flash_address, load_info.model_length, wwe_flash_length);
            /**
             * @brief If current is not sending audio data, and current is connected
             * Need restart the audio recorder with the new configured language model.
             * If current is AMA mode.
             */
            multi_va_type_t type = multi_va_manager_get_current_va_type();
            APPS_LOG_MSGID_I(APP_AMA_ACTI", [Multi_AI] SetLocale, current va_type : %x", 1, type);
            if (type == MULTI_VA_TYPE_AMA) {
                bool suspend_by_ull = app_ama_is_ull_upstreaming();
                bool recorder_already_suspend = ama_audio_is_suspended();

                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, suspend by ULL : %d, already suspended : %d", 2, suspend_by_ull, recorder_already_suspend);
                if ((suspend_by_ull == false) && (recorder_already_suspend == false)) {
                    bool audio_busy = ama_audio_is_busy();
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", SetLocale, audio_busy : %d, wake_word_detection_enable : %d, model address : 0x%x, model length : 0x%x", 4,
                                        audio_busy,
                                        ama_activity_context.context_wake_word_detection_enable,
                                        wwe_flash_address,
                                        wwe_flash_length);
                    if ((audio_busy == false)
                            && (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == true)
                            && (ama_activity_context.context_wake_word_detection_enable == true)
                            && (wwe_flash_address != 0x00)
                            && (wwe_flash_length != 0x00)) {
                        ama_audio_restart(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
                    }
                }
            }
        }
    }
    AMA_Target_SetLocaleResponse(&(set_locale_ind->bdAddr), resp_status);
#else /* MTK_VA_MODEL_MANAGER_ENABLE */
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_set_locale_ind, not define MTK_VA_MODEL_MANAGER_ENABLE", 0);
    AMA_Target_SetLocaleResponse(&(set_locale_ind->bdAddr), AMA_ERROR_CODE_UNSUPPORTED);
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */
}

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
static bool start_speech_with_wwd(bool from_agent, uint32_t stop_index)
{
    AMA_START_SPEECH_T *startSpeechParam = NULL;

    if (stop_index <= AMA_AUDIO_WWD_START_INDEX) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start recognition failed, Stop index error : %d", 1, stop_index);
        return false;
    }

    if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start recognition failed, The address is INVALID", 0);
        return false;
    }

#if 0
    if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_IDLE) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start recognition failed, Current speech state is not IDLE : %d", 1, ama_activity_context.context_recognition_state);
        // ama_audio_stop();
        /**
         * @brief Fix issue : BTA-7506
         * That when the start speech failed, need restart the WWE for next round.
         */
        ama_audio_restart(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
        return false;
    }
#endif

    startSpeechParam = (AMA_START_SPEECH_T *)pvPortMalloc(sizeof(AMA_START_SPEECH_T));
    if (startSpeechParam == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start recognition failed, cause allocate memory for start speech parameter failed", 0);
        return false;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start speech with stop index : %d", 1, stop_index);

    if (from_agent == true) {
        apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    }

    startSpeechParam->audioProfile = AMA_AUDIO_PROFILE_NEAR_FIELD;
    startSpeechParam->audioFormat = AMA_AUDIO_FORMAT_OPUS_16KHZ_32KBPS_CBR_0_20MS;
    startSpeechParam->audioSource = AMA_AUDIO_SOURCE_STREAM;
    startSpeechParam->type = AMA_SPEECH_INITIATOR_TYPE_WAKEWORD;
    startSpeechParam->suppressEndpointEarcon = 0;

#ifdef AIR_AMA_SIDETONE_ENABLE
    /**
     * @brief If enable side tone feature, need play local VP to replace the A2DP start tone.
     * So suppress the start earcon from Alexa APP.
     */
    startSpeechParam->suppressStartEarcon = 1;
#else
    startSpeechParam->suppressStartEarcon = 0;
#endif /* AIR_AMA_SIDETONE_ENABLE */

    startSpeechParam->wake_word.startIndexInSamples = AMA_AUDIO_WWD_START_INDEX;
    startSpeechParam->wake_word.endIndexInSamples = stop_index;
    startSpeechParam->wake_word.metadataLength = 0;
    startSpeechParam->wake_word.nearMiss = false;

    ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_WAKEWORD;
    ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_STARTING;

    APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start speech : address 0x%x:%x:%x:%x:%x:%x", 6,
                        ama_activity_context.context_addr.addr[0], ama_activity_context.context_addr.addr[1], ama_activity_context.context_addr.addr[2],
                        ama_activity_context.context_addr.addr[3], ama_activity_context.context_addr.addr[4], ama_activity_context.context_addr.addr[5]);

    AMAStatus status = AMA_Target_StartSpeech(&(ama_activity_context.context_addr), startSpeechParam);

    vPortFree(startSpeechParam);
    startSpeechParam = NULL;

    APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] start_speech_with_wwd, Start speech result : %d", 1, status);

    if (status == AMA_STATUS_OK) {
        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_RECORDING;

        /**
         * @brief For multi-link
         */
        bt_sink_srv_set_must_play_tone_flag(&(ama_activity_context.context_addr.addr),
                                            BT_SINK_SRV_AMA_NOTIFICATION_VOICE,
                                            true);
        return true;
    }
    ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
    return false;
}
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

static bool ama_wwd_trigger_handler(uint32_t stop_index)
{
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, The address is INVALID", 0);
        return false;
    }

    if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_IDLE
        && ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_THINKING
        && ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_SPEAKING) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, Current is BUSY state : %d", 1, ama_activity_context.context_recognition_state);
        return false;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, Stop index : %d", 1, stop_index);

    if (stop_index <= AMA_AUDIO_WWD_START_INDEX) {
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
        apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);

        bt_status_t status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_KEY,
                                                            KEY_AMA_START,
                                                            (void *)&stop_index,
                                                            sizeof(uint32_t));
        if (BT_STATUS_SUCCESS != status) {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, Partner send KEY_AMA_START aws to agent failed", 0);
            return false;
        } else {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, Partner send KEY_AMA_START aws to agent success", 0);
            return true;
        }
    } else
#endif /* MTK_AWS_MCE_ENABLE */
    {
        if (start_speech_with_wwd(true, stop_index) == false) {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", [WWD] ama_wwd_trigger_handler, Start speech failed, restart recorder, wake_word_enable : %d, model address : 0x%x, model length : 0x%x", 3,
                                ama_activity_context.context_wake_word_detection_enable,
                                wwe_flash_address, wwe_flash_length);

            if ((ama_activity_context.context_wake_word_detection_enable == true)
                && (wwe_flash_address != 0x00)
                && (wwe_flash_length != 0x00)) {
                // ama_audio_restart(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
            }
            return false;
        } else {
            return true;
        }
    }
#else /* AMA_TRIGGER_MODE_WWD_ENABLE */
    return true;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
}

static void app_ama_activity_start_wwe()
{
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
#ifndef MTK_VA_MODEL_MANAGER_ENABLE
    wwe_flash_address = 0x00;
    wwe_flash_length = 0x00;
#else /* MTK_VA_MODEL_MANAGER_ENABLE */
    va_model_manager_model_info_t model_info;
    va_model_manager_error_code_t err_code = va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_AMA, &model_info);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_start_wwe, Get current running model result : 0x%02x", 1, err_code);
    if (err_code == VA_MODEL_MANAGER_ERROR_CODE_OK && model_info.model_flash_address != 0 && model_info.model_length != 0) {
        wwe_flash_address = model_info.model_flash_address;
        wwe_flash_length = model_info.model_length;
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_start_wwe, current running flash address : 0x%08x, length : 0x%08x", 2, wwe_flash_address, wwe_flash_length);
    } else {
        assert(false);
    }
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */
    bool suspend_by_ull = app_ama_is_ull_upstreaming();
    bool recorder_already_suspend = ama_audio_is_suspended();

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_start_wwe, suspend by ULL : %d, already suspended : %d", 2, suspend_by_ull, recorder_already_suspend);
    if ((suspend_by_ull == true)
            || (recorder_already_suspend == true)) {
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_start_wwe, wake_word_detection_enable : %d, model address : 0x%x, length : 0x%x",
                    3, ama_activity_context.context_wake_word_detection_enable, wwe_flash_address, wwe_flash_length);
    if ((ama_activity_context.context_wake_word_detection_enable == true)
            && (wwe_flash_address != 0x00)
            && (wwe_flash_length != 0x00)) {
        /**
         * @brief Configure the WWD triggered callback
         */
        ama_audio_set_wwd_trigger_callback(ama_wwd_trigger_handler);
        /**
         * @brief Start recorder to trigger WWE, and detect the key word.
         */
        ama_audio_start(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
    }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
}

#ifdef MTK_ANC_ENABLE

/**
 * @brief Mapping from ANC/passthrough level set by Alexa to runtime_gain.
 *
 * @param level: 0 ~ 100
 * @retun gain: -500 ~ 500
 */
static int32_t app_ama_activity_anc_level_to_gain(int32_t level)
{
    int32_t gain;

    if (level < 0) {
        gain = -500;
    } else if (level > 100) {
        gain = 500;
    } else {
        gain = (level - 50) * 10;
    }
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_anc_level_to_gain, level(%d) -> gain(%d)", 2, level, gain);
    return gain;
}

/**
 * @brief Mapping from runtime_gain to ANC/passthrough level set by Alexa.
 *
 * @param gain: -500 ~ 500
 * @retun level: 0 ~ 100
 */
static int32_t app_ama_activity_anc_gain_to_level(int32_t gain)
{
    int32_t level;

    if (gain < -500) {
        level = 0;
    } else if (gain > 500) {
        level = 100;
    } else {
        level = (gain + 500) / 10;
    }
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_anc_gain_to_level, gain(%d) -> level(%d)", 2, gain, level);
    return level;
}

/**
 * @brief The Audio ANC monitor callback handler.
 *
 * @param event_id
 * @param result
 */
static void app_ama_activity_audio_anc_control_callback_handler(audio_anc_control_event_t event_id, audio_anc_control_result_t result)
{
    if (AUDIO_ANC_CONTROL_EXECUTION_SUCCESS == result) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_audio_anc_control_callback_handler, ANC Status changed", 0);
        ui_shell_send_event(false,
                            EVENT_PRIORITY_MIDDLE,
                            EVENT_GROUP_UI_SHELL_BT_AMA,
                            APP_AMA_UI_SHELL_EVENT_ID_HANDLE_ANC_CHANGE,
                            NULL, 0, NULL, 0);
    }
}
#endif /* MTK_ANC_ENABLE */

/**
 * @brief Handle the audio ACN change, need notify SmartPhone that the ACN state changed.
 *
 */
static void app_ama_activity_handle_audio_anc_change()
{
#ifdef MTK_ANC_ENABLE
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_misc_t        anc_misc;
    uint8_t                         anc_enabled = 0;
    int16_t                         anc_runtime_gain = 0;
    uint8_t                         anc_hybrid_enabled = 0;

    audio_anc_control_get_status(&anc_enabled,
                                    &anc_filter_id,
                                    &anc_type,
                                    &anc_runtime_gain,
                                    &anc_hybrid_enabled,
                                    &anc_misc);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_handle_audio_anc_change, Origin ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d), target (%d)",
                        6, app_ama_anc_info.info_anc_enabled, app_ama_anc_info.info_anc_filter_id, app_ama_anc_info.info_anc_type,
                        app_ama_anc_info.info_anc_support_hybrid, app_ama_anc_info.info_anc_runtime_gain, app_ama_anc_info.target_anc_type);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_handle_audio_anc_change, New ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d)",
                        5, anc_enabled, anc_filter_id, anc_type, anc_hybrid_enabled, anc_runtime_gain);

    if ((anc_enabled && (anc_type == app_ama_anc_info.target_anc_type)) ||
            ((!anc_enabled) && (app_ama_anc_info.target_anc_type == APP_AMA_TARGET_ANC_DISABLE))) {
        /* ANC status change match target. */
        app_ama_anc_info.target_anc_type = APP_AMA_TARGET_ANC_NONE;
    } else if (app_ama_anc_info.target_anc_type != APP_AMA_TARGET_ANC_NONE) {
        /* ANC status not match target, it's temp state, don't report. */
        return;
    }

    if ((anc_enabled != app_ama_anc_info.info_anc_enabled) || (anc_type != app_ama_anc_info.info_anc_type)) {
        if (anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, (anc_enabled == true ? true : false));
            if (anc_enabled && app_ama_anc_info.info_anc_enabled) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, false);
            }
        }
        if (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, (anc_enabled == true ? true : false));
            if (anc_enabled && app_ama_anc_info.info_anc_enabled) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, false);
            }
        }
    }
    if ((anc_runtime_gain != app_ama_anc_info.info_anc_runtime_gain) || (anc_enabled != app_ama_anc_info.info_anc_enabled) || (anc_type != app_ama_anc_info.info_anc_type)) {
        if (anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            app_ama_activity_notify_state_change(AMA_VALUE_TYPE_INTEGER, AMA_STATE_FEATURE_ACTIVE_NC_LEVEL, app_ama_activity_anc_gain_to_level(anc_runtime_gain));
        }
#if 0
        if (anc_type == AUDIO_ANC_CONTROL_TYPE_FB) {
            app_ama_activity_notify_state_change(AMA_VALUE_TYPE_INTEGER, AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL, app_ama_activity_anc_gain_to_level(anc_runtime_gain));
        }
#endif
        if (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
            app_ama_activity_notify_state_change(AMA_VALUE_TYPE_INTEGER, AMA_STATE_FEATURE_PASSTHROUGH_LEVEL, app_ama_activity_anc_gain_to_level(anc_runtime_gain));
        }
    }

    app_ama_anc_info.info_anc_enabled = anc_enabled;
    app_ama_anc_info.info_anc_filter_id = anc_filter_id;
    app_ama_anc_info.info_anc_type = anc_type;
    app_ama_anc_info.info_anc_support_hybrid = anc_hybrid_enabled;
    app_ama_anc_info.info_anc_runtime_gain = anc_runtime_gain;
    app_ama_anc_info.info_anc_control_misc.extend_use_parameters = anc_misc.extend_use_parameters;
#endif /* MTK_ANC_ENABLE */
}

static void app_ama_activity_register_anc_callback()
{
#ifdef MTK_ANC_ENABLE
    audio_anc_control_event_t event_mask = AUDIO_ANC_CONTROL_EVENT_ON | AUDIO_ANC_CONTROL_EVENT_OFF | AUDIO_ANC_CONTROL_EVENT_FORCE_OFF | AUDIO_ANC_CONTROL_EVENT_SET_REG;
    audio_anc_control_register_callback(app_ama_activity_audio_anc_control_callback_handler, event_mask, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
#endif /* MTK_ANC_ENABLE */
}

static void app_ama_activity_deregister_anc_callback()
{
#ifdef MTK_ANC_ENABLE
    audio_anc_control_deregister_callback(app_ama_activity_audio_anc_control_callback_handler);
#endif /* MTK_ANC_ENABLE */
}

static void app_ama_activity_init_anc()
{
#ifdef MTK_ANC_ENABLE
    if (app_ama_anc_info.info_anc_init_done) {
        return;
    }
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_init_anc, Start to get ANC current configuration and register callback", 0);
    memset(&app_ama_anc_info, 0, sizeof(app_ama_anc_info_t));
    audio_anc_control_get_status(&app_ama_anc_info.info_anc_enabled,
                                    &app_ama_anc_info.info_anc_filter_id,
                                    &app_ama_anc_info.info_anc_type,
                                    &app_ama_anc_info.info_anc_runtime_gain,
                                    &app_ama_anc_info.info_anc_support_hybrid,
                                    &app_ama_anc_info.info_anc_control_misc);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_init_anc, ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d)",
                        5, app_ama_anc_info.info_anc_enabled, app_ama_anc_info.info_anc_filter_id,
                        app_ama_anc_info.info_anc_type, app_ama_anc_info.info_anc_support_hybrid, app_ama_anc_info.info_anc_runtime_gain);

    app_ama_anc_info.target_anc_type = APP_AMA_TARGET_ANC_NONE;
    app_ama_activity_register_anc_callback();
    app_ama_anc_info.info_anc_init_done = true;
#endif /* MTK_ANC_ENABLE */
}

/**
 * @brief Handle the audio recorder stopped by other application event
 *
 * @return true
 * @return false
 */
static bool audio_recorder_stopped_callback_handler(void)
{
    multi_va_type_t type = multi_va_manager_get_current_va_type();
    if (type != MULTI_VA_TYPE_AMA) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", audio_recorder_stopped_callback_handler, Current is not AMA (%d)", 1, type);
        return false;
    }

    /**
     * @brief Need check the address is valid or not.
     *
     */
    if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", audio_recorder_stopped_callback_handler, Stop speech failed, cause address is INVALID", 0);
        return false;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", audio_recorder_stopped_callback_handler, Current recognition state : %d", 1, ama_activity_context.context_recognition_state);
    if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_IDLE) {
        /**
         * @brief When the recorder stopped by other application (HFP/eSCO/LD)
         * Need sent the stop speech command to SP to stop current executing flow.
         */
        AMAStatus status = AMA_Target_StopSpeech(&(ama_activity_context.context_addr));
        APPS_LOG_MSGID_I(APP_AMA_ACTI", audio_recorder_stopped_callback_handler, Stop speech result :%d", 1, status);
        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
    }

    return true;
}

static void audio_recorder_released_callback_handler(void)
{
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
    if (app_adaptive_anc_get_state() != APP_ADAPTIVE_ANC_IDLE) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", audio_recorder_released_callback_handler, recorder is using by adaptive ANC, cannot start AMA WWE", 0);
        return;
    }
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

    ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
    ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
    /**
     * @brief If the audio recorder has been release by other application
     * And current support WWD feature, need start the audio recorder to detect the key word.
     */
    multi_va_type_t type = multi_va_manager_get_current_va_type();

    APPS_LOG_MSGID_I(APP_AMA_ACTI", audio_recorder_released_callback_handler, recorder released handler, current_va_type : %d, enabled : %d, address : 0x%08x, length : 0x%08x",
                        4, type, ama_activity_context.context_wake_word_detection_enable, wwe_flash_address, wwe_flash_length);

    if ((type == MULTI_VA_TYPE_AMA)
            && (wwe_flash_address != 0x00)
            && (wwe_flash_length != 0x00)
            && (ama_activity_context.context_wake_word_detection_enable == true)) {
        ama_audio_start(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
    }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
}


static bool app_ama_bt_takeover_handler(const bt_bd_addr_t remote_addr)
{
    bool ret = true;

    //if(memcmp(&(ama_activity_context.context_addr), remote_addr, sizeof(bt_bd_addr_t)) == 0) {
    if (ama_activity_context.context_recognition_state > AMA_ACTIVITY_RECOGNITION_STATE_IDLE
        && ama_activity_context.context_recognition_state < AMA_ACTIVITY_RECOGNITION_STATE_SPEAKING) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", cur addr takeover not allow, state %d", 1, ama_activity_context.context_recognition_state);
        ret = false;
    }

    return ret;
}

/**
 * @brief When AMA connected, checking the A2DP/HFP state and notify to Alexa APP.
 * This change will fix issue, that when AMA connected, the trigger tone out from the Smart phone.
 */
static void app_ama_activity_state_checking()
{
    bt_cm_profile_service_state_t a2dp_profile_state = bt_cm_get_profile_service_state(ama_activity_context.context_addr.addr, BT_CM_PROFILE_SERVICE_A2DP_SINK);
    bt_cm_profile_service_state_t hfp_profile_state = bt_cm_get_profile_service_state(ama_activity_context.context_addr.addr, BT_CM_PROFILE_SERVICE_HFP);

    if (a2dp_profile_state == BT_CM_PROFILE_SERVICE_STATE_CONNECTED) {
        app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_A2DP_CONNECTED, true);
    } else {
        app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_A2DP_CONNECTED, false);
    }

    if (hfp_profile_state == BT_CM_PROFILE_SERVICE_STATE_CONNECTED) {
        app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_HFP_CONNECTED, true);
    } else {
        app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_HFP_CONNECTED, false);
    }
}

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true; // UI shell internal event must process by this activity, so default is true
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", create", 0);

            if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
                assert(false);
            }

            ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
            ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
            ama_activity_context.context_avrcp_override = false;

            /**
             * @brief Default configure wake word detection to be enabled.
             *
             */
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            ama_activity_context.context_wake_word_detection_enable = true;
#else
            ama_activity_context.context_wake_word_detection_enable = false;
#endif

            multi_va_type_t type = multi_va_manager_get_current_va_type();
            APPS_LOG_MSGID_I(APP_AMA_ACTI", [Multi_AI] on_create_type: %x", 1, type);
            if (type == MULTI_VA_TYPE_AMA) {
                multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_AMA);

                /**
                 * @brief Init the audio recorder for WWD enable.
                 */
                ama_audio_init(audio_recorder_stopped_callback_handler, audio_recorder_released_callback_handler);

                /**
                 * @brief Start audio recorder for wake word detection.
                 *
                 */
                app_ama_activity_start_wwe();

                /**
                 * @brief Init ANC information and register callback to monitor the ANC change.
                 *
                 */
                app_ama_activity_init_anc();

                /**
                 * @brief Check the A2DP and HFP is connected or not and notify state to Alexa APP.
                 */
                app_ama_activity_state_checking();
            }
            /* Init takeover service user. */
            app_bt_takeover_service_user_register(APP_BT_TAKEOVER_ID_AMA, app_ama_bt_takeover_handler);
        }
        break;

        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", destroy", 0);

            /**
             * @brief Fix issue
             * When the AMA connected, and disconnected in a short time, the recorder is not start,
             * So the stop execution in the disconnect indication is not useful.
             * Make sure the audio recorder execute stop operation.
             */
            ama_audio_stop();
            memset(ama_activity_context.context_addr.addr, 0, BT_BD_ADDR_LEN);
            memset(&ama_activity_context, 0, sizeof(ama_context_t));

            wwe_flash_address = 0x00;
            wwe_flash_length = 0x00;
#ifdef MTK_ANC_ENABLE
            app_ama_activity_deregister_anc_callback();
            memset(&app_ama_anc_info, 0, sizeof(app_ama_anc_info_t));
#endif /* MTK_ANC_ENABLE */
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static AMAStatus app_ama_start_speech_with_tap_or_hold(bool long_press)
{

    multi_va_type_t current_type = multi_va_manager_get_current_va_type();
    if (current_type != MULTI_VA_TYPE_AMA) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, Current VA type is not AMA (%d)", 1, current_type);
        return AMA_STATUS_ERROR;
    }

    if (ama_audio_is_stopped_by_ld() == true) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, recorder already stopped by LD and need waiting for LD finished.", 0);
        /**
         * @brief Maybe need add the VP to notify user that LD is ongoing
         * cannot trigger AMA.
         */
        return AMA_STATUS_ERROR;
    }

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
    if (app_adaptive_anc_get_state() != APP_ADAPTIVE_ANC_IDLE) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, recorder is using by adaptive ANC, cannot trigger AMA", 0);
        return AMA_STATUS_ERROR;
    }
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, long press ? %d", 1, long_press);

    if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_IDLE
        && ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_THINKING
        && ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_SPEAKING) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, Start recognition failed, Current BUSY state : %d", 1, ama_activity_context.context_recognition_state);
        return AMA_STATUS_ERROR;
    }

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    if (long_press == false) {
        /**
         * @brief If current support WWD feature, need stop recorder firstly
         *
         */
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, WWD enabled, stop audio recorder firstly", 0);
        ama_audio_stop();
    }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

    AMA_START_SPEECH_T startSpeechParam;

    if (long_press == true) {
        startSpeechParam.audioProfile = AMA_AUDIO_PROFILE_CLOSE_TALK;
        startSpeechParam.type = AMA_SPEECH_INITIATOR_TYPE_PRESS_AND_HOLD;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_PRESS_AND_HOLD;
    } else {
        startSpeechParam.audioProfile = AMA_AUDIO_PROFILE_NEAR_FIELD;
        startSpeechParam.type = AMA_SPEECH_INITIATOR_TYPE_TAP;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_TAP;
    }

    startSpeechParam.audioFormat = AMA_AUDIO_FORMAT_OPUS_16KHZ_32KBPS_CBR_0_20MS;
    startSpeechParam.audioSource = AMA_AUDIO_SOURCE_STREAM;
    startSpeechParam.suppressEndpointEarcon = 0;
#ifdef AIR_AMA_SIDETONE_ENABLE
    /**
     * @brief If enable side tone feature, need play local VP to replace the A2DP start tone.
     * So suppress the start earcon from Alexa APP.
     */
    startSpeechParam.suppressStartEarcon = 1;
#else
    startSpeechParam.suppressStartEarcon = 0;
#endif /* AIR_AMA_SIDETONE_ENABLE */

    ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_STARTING;

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, address : 0x%x:%x:%x:%x:%x:%x", 6,
                        ama_activity_context.context_addr.addr[0], ama_activity_context.context_addr.addr[1], ama_activity_context.context_addr.addr[2],
                        ama_activity_context.context_addr.addr[3], ama_activity_context.context_addr.addr[4], ama_activity_context.context_addr.addr[5]);

    AMAStatus status = AMA_Target_StartSpeech(&(ama_activity_context.context_addr), &startSpeechParam);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, Start recognition result : 0x%x", 1, status);

    if (status != AMA_STATUS_OK) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, Start recognition failed, start WWE again", 0);
        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_start_speech_with_tap_or_hold, Start recognition failed, wake_word_enabled : %d, model address : 0x%x, model length : 0x%x", 3,
                            ama_activity_context.context_wake_word_detection_enable,
                            wwe_flash_address, wwe_flash_length);

        if ((ama_activity_context.context_wake_word_detection_enable == true)
                && (wwe_flash_address != 0x00)
                && (wwe_flash_length != 0x00)) {
            ama_audio_start(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
        }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        return status;
    }

    /**
     * @brief For multi-link
     */
    bt_sink_srv_set_must_play_tone_flag(&(ama_activity_context.context_addr.addr),
                                        BT_SINK_SRV_AMA_NOTIFICATION_VOICE,
                                        true);

    return status;
}

static void app_ama_handle_long_press_trigger_press()
{
    AMAStatus status = app_ama_start_speech_with_tap_or_hold(true);
    if (status == AMA_STATUS_OK) {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_press, Restart audio recorder", 0);
        ama_audio_restart(WWE_MODE_NONE, 0, 0);
#else
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_press, Start audio recorder", 0);
        ama_audio_start(WWE_MODE_NONE, 0, 0);
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        ama_activity_context.context_ptt_recorder_stopped = false;
    }
}

static void app_ama_handle_long_press_trigger_release()
{
    AMAStatus status = AMA_STATUS_OK;
    static uint8_t retry_count = 0;

    // if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_RECORDING) {
    //     APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_release, Current is not recording state : %d",
    //                 1, ama_activity_context.context_recognition_state);
    //     return;
    // }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_release, Start speech result : %d, recorder stopped : %d",
                    2, ama_activity_context.context_ptt_start_speech_result,
                    ama_activity_context.context_ptt_recorder_stopped);

    if (ama_activity_context.context_ptt_start_speech_result == false) {
        return;
    }

    if (ama_activity_context.context_ptt_recorder_stopped == false) {
        ama_audio_stop();
        ama_activity_context.context_ptt_recorder_stopped = true;
    }

    unsigned int voice_tx_queue_count = AMA_Target_GetVoiceQueueCount(&(ama_activity_context.context_addr));
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_release, current tx queue count : %d, retry_count : %d",
                        2, voice_tx_queue_count, retry_count);

    if (voice_tx_queue_count == 0 || retry_count == 20) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_BT_AMA, APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE);
        status = AMA_Target_EndpointSpeech(&(ama_activity_context.context_addr));

        retry_count = 0;
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_release, End point speech result : %d", 1, status);
    } else {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_long_press_trigger_release, Send long press release event with 10ms delay", 0);
        retry_count ++;

        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_BT_AMA, APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE);
        ui_shell_send_event(false,
                            EVENT_PRIORITY_MIDDLE,
                            EVENT_GROUP_UI_SHELL_BT_AMA,
                            APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE,
                            NULL,
                            0,
                            NULL,
                            100);
    }

}

static bool app_ama_is_ull_upstreaming()
{
    /**
     * @brief Fix issue that when ULL upstreaming, cannot start AMA
     */
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    bt_ull_streaming_info_t info = {0};
    bt_status_t ret = BT_STATUS_FAIL;

    bt_ull_streaming_t streaming = {
        .streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE,
        .port = 0,
    };
    ret = bt_ull_get_streaming_info(streaming, &info);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_is_ull_upstreaming, result : 0x%x, playing : %d", 2, ret, info.is_playing);
    if (BT_STATUS_SUCCESS == ret) {
        return info.is_playing;
    }
    return false;
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
    return false;
}

static bool app_ama_handle_key_event(apps_config_key_action_t key_event)
{
    if ((key_event != KEY_AMA_START) && (key_event != KEY_AMA_START_NOTIFY)
        && (key_event != KEY_AMA_LONG_PRESS_TRIGGER_START) && (key_event != KEY_AMA_LONG_PRESS_TRIGGER_STOP)) {
        return false;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_key_event, key event : 0x%04x", 1, key_event);

    bool suspend_by_ull = app_ama_is_ull_upstreaming();
    bool recorder_already_suspend = ama_audio_is_suspended();

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_key_event, suspend by ULL : %d, already suspended : %d", 2, suspend_by_ull, recorder_already_suspend);

    if ((suspend_by_ull == true)
            || (recorder_already_suspend == true)) {
        return false;
    }

    if (key_event == KEY_AMA_START_NOTIFY || key_event == KEY_AMA_LONG_PRESS_TRIGGER_START) {
        // if (ama_audio_is_stopped_by_ld() == true) {
        //     APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_key_event, recorder already stopped by LD and need waiting for LD finished.", 0);
        //     /**
        //      * @brief Maybe need add the VP to notify user that LD is ongoing
        //      * cannot trigger AMA.
        //      */
        //     return false;
        // }

        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_key_event, Key start notify Execution", 0);
        apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);

        if (key_event == KEY_AMA_START_NOTIFY) {
            return true;
        }
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
        uint32_t dummy_value = 0;
        /**
         * @brief Send the dummy data to agent which to indicate the event from key or WWD
         *
         */
        bt_status_t send_result = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_KEY, key_event, &dummy_value, sizeof(uint32_t));
        if (BT_STATUS_SUCCESS != send_result) {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", _proc_key_event_group, Partner send key (0x%04x) to agent failed", 1, key_event);
        } else {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", _proc_key_event_group, Partner send key (0x%04x) to agent success", 1, key_event);
        }
        return true;
    }
#endif /* MTK_AWS_MCE_ENABLE */

    switch (key_event) {
        case KEY_AMA_START: {
            app_ama_start_speech_with_tap_or_hold(false);
        }
        break;
        case KEY_AMA_LONG_PRESS_TRIGGER_START: {
            app_ama_handle_long_press_trigger_press();
        }
        break;
        case KEY_AMA_LONG_PRESS_TRIGGER_STOP: {
            app_ama_handle_long_press_trigger_release();
        }
        break;
    }
    return true;
}

static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    uint16_t key_id = *(uint16_t *)extra_data;
    return app_ama_handle_key_event(key_id);
}

void app_ama_get_state_ind_handler(AMA_GET_STATE_IND_T * ind)
{
    if (ind == NULL) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_get_state_ind_handler, ind is NULL", 0);
        return;
    }

    if (app_ama_is_valid_address(ind->bdAddr.addr) == false) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_get_state_ind_handler, Address is INVALID", 0);
        return;
    }

    AMA_STATE_T state = {0};
    AMA_ERROR_CODE_E errCode = AMA_ERROR_CODE_SUCCESS;
#ifdef MTK_ANC_ENABLE
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_misc_t        anc_misc;
    uint8_t                         anc_enabled = 0;
    int16_t                         anc_runtime_gain = 0;
    uint8_t                         anc_hybrid_enabled = 0;

    audio_anc_control_get_status(&anc_enabled, &anc_filter_id, &anc_type, &anc_runtime_gain, &anc_hybrid_enabled, &anc_misc);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d)",
                        5, anc_enabled, anc_filter_id, anc_type, anc_hybrid_enabled, anc_runtime_gain);
#endif /* MTK_ANC_ENABLE */

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, feature:0x%x", 1, ind->feature);

    state.feature = ind->feature;

    switch (ind->feature) {
        case AMA_STATE_FEATURE_BT_DISCOVERABLE: { /* 0x134 */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = 1;
        }
        break;
        case AMA_STATE_FEATURE_A2DP_ENABLED: /* 0x130 */
        case AMA_STATE_FEATURE_HFP_ENABLED: /* 0x131 */
        case AMA_STATE_FEATURE_A2DP_ACTIVE: /* 0x135 */
        case AMA_STATE_FEATURE_HFP_ACTIVE: {/* 0x136 */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = 1;
            break;
        }
        case AMA_STATE_FEATURE_A2DP_CONNECTED: { /* 0x132 */
            bt_cm_profile_service_state_t profile_state = bt_cm_get_profile_service_state(ama_activity_context.context_addr.addr, BT_CM_PROFILE_SERVICE_A2DP_SINK);

            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = (profile_state == BT_CM_PROFILE_SERVICE_STATE_CONNECTED ?  true : false);
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, A2DP : 0x%x", 1, state.value.boolean);
            break;
        }
        case AMA_STATE_FEATURE_HFP_CONNECTED: { /* 0x133 */
            bt_cm_profile_service_state_t profile_state = bt_cm_get_profile_service_state(ama_activity_context.context_addr.addr, BT_CM_PROFILE_SERVICE_HFP);

            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = (profile_state == BT_CM_PROFILE_SERVICE_STATE_CONNECTED ? true : false);
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, HFP : 0x%x", 1, state.value.boolean);
            break;
        }
#if 0
        case AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL: { /* 0x209 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL (0x209)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_INTEGER;
            state.value.integer = 0;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;

            if (anc_enabled == 0) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL, ANC current disabled", 0);
            } else {
                errCode = AMA_ERROR_CODE_INTERNAL;

                if (anc_type == AUDIO_ANC_CONTROL_TYPE_FB) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL, Get PB ANC level : %d",
                                        1, anc_runtime_gain);
                    state.valueType = AMA_VALUE_TYPE_INTEGER;
                    state.value.integer = app_ama_activity_anc_gain_to_level(anc_runtime_gain);
                    errCode = AMA_ERROR_CODE_SUCCESS;
                } else {
                    errCode = AMA_ERROR_CODE_INTERNAL;
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL, Current is not FB ANC : %d",
                                        1, anc_type);
                }
            }
#else /* MTK_ANC_ENABLE */
            state.valueType = AMA_VALUE_TYPE_INTEGER;
            state.value.integer = 0;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;
#endif
        case AMA_STATE_FEATURE_ANC_PASSTHROUGH_OFF: { /* 0x209 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_ANC_PASSTHROUGH_OFF (0x209)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            if (anc_enabled) {
                state.value.boolean = true;
            } else {
                state.value.boolean = false;
            }
#else /* MTK_ANC_ENABLE */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = false;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
            break;
        }
        case AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED: { /* 0x210 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED (0x210)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            if (anc_enabled && (anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                state.value.boolean = true;
            } else {
                state.value.boolean = false;
            }
#else /* MTK_ANC_ENABLE */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = false;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PASSTHROUGH_ENABLED: { /* 0x211 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_PASSTHROUGH_ENABLED (0x211)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            if (anc_enabled && (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                state.value.boolean = true;
            } else {
                state.value.boolean = false;
            }
#else /* MTK_ANC_ENABLE */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = false;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_BASS: { /* 0x450 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_BASS (0x450)", 0);
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_MID: { /* 0x452 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_MID (0x451)", 0);
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_TREBLE: { /* 0x452 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_TREBLE (0x452)", 0);
        }
        break;

        case AMA_STATE_FEATURE_ACTIVE_NC_LEVEL: { /* 0x205 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_ACTIVE_NC_LEVEL (0x205)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_INTEGER;
            state.value.integer = 0;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;

            if (anc_enabled == 0) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NC_LEVEL, ANC current disabled", 0);
            } else {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NC_LEVEL, Get ANC level : %d",
                                    1, anc_runtime_gain);
                errCode = AMA_ERROR_CODE_SUCCESS;
                state.value.integer = anc_runtime_gain;
            }
#else /* MTK_ANC_ENABLE */
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
            state.valueType = AMA_VALUE_TYPE_INTEGER;
            state.value.integer = 0;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PASSTHROUGH_LEVEL: { /* 0x206 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_PASSTHROUGH_LEVEL (0x212)", 0);
#ifdef MTK_ANC_ENABLE
            state.valueType = AMA_VALUE_TYPE_INTEGER;
            state.value.integer = 0;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;

            if (anc_enabled == 0) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_LEVEL, Passthrough current disabled", 0);
            } else {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_LEVEL, Get ANC Passthrough level : %d",
                                    1, anc_runtime_gain);
                if (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
                    errCode = AMA_ERROR_CODE_SUCCESS;
                    state.value.integer = app_ama_activity_anc_gain_to_level(anc_runtime_gain);
                } else {
                    errCode = AMA_ERROR_CODE_INTERNAL;
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_LEVEL, Current is not Passthrough mode : %d",
                                        1, anc_type);
                }
            }
#else /* MTK_ANC_ENABLE */
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = false;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PRIVACY_MODE_ENABLED: { /* 0x204 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_PRIVACY_MODE_ENABLED (0x204)", 0);
        }
        break;

        case AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED: { /* 0x352 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_get_state_ind_handler, received AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED (0x352)", 0);
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = true;
#else
            state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            state.value.boolean = false;
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
#endif
        }
        break;

        default: {
            errCode = AMA_ERROR_CODE_UNSUPPORTED;
            // state.valueType = AMA_VALUE_TYPE_BOOLEAN;
            // state.value.integer = 0;
            break;
        }
    }
    APPS_LOG_MSGID_I(APP_AMA_ACTI",app_ama_get_state_ind_handler, response error code : %d, feature : 0x%x, value : %d",
                        3, errCode, ind->feature, state.value.integer);
    AMA_Target_GetStateResponse(&ind->bdAddr, &state, errCode);
}

static void app_ama_media_control_handler(AMA_ISSUE_MEDIA_CONTROL_IND_T *ind)
{
    AMAStatus status;
    bt_sink_srv_action_t action = 0;

    if (ind == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_media_control_handler, ind is NULL", 0);
        return;
    }
    if (app_ama_is_valid_address(ind->bdAddr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_media_control_handler, Address is INVALID", 0);
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_media_control_handler, override: %x, control: %x", 2, ama_activity_context.context_avrcp_override, ind->control);
    if (ama_activity_context.context_avrcp_override) {
        if (ind->control >= AMA_MEDIA_CONTROL_PLAY && ind->control <= AMA_MEDIA_CONTROL_PLAY_PAUSE) {
            status = AMA_Target_IssueMediaControl(&(ind->bdAddr), ind->control);

            AMA_Target_IssueMediaControlResponse(&ind->bdAddr, AMA_ERROR_CODE_SUCCESS);

        } else {
            status = AMA_Target_IssueMediaControlResponse(&ind->bdAddr, AMA_ERROR_CODE_UNKNOWN);
        }

        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_media_control_handler, status: %x", 1, status);
        return;
    }
    switch (ind->control) {
        case AMA_MEDIA_CONTROL_PLAY: {
            action = BT_SINK_SRV_ACTION_PLAY;
            break;
        }
        case AMA_MEDIA_CONTROL_PAUSE: {
            action = BT_SINK_SRV_ACTION_PAUSE;
            break;
        }
        case AMA_MEDIA_CONTROL_NEXT: {
            action = BT_SINK_SRV_ACTION_NEXT_TRACK;
            break;
        }
        case AMA_MEDIA_CONTROL_PREVIOUS: {
            action = BT_SINK_SRV_ACTION_PREV_TRACK;
            break;
        }
        default: {
            break;
        }
    }

    bt_status_t result = bt_sink_srv_send_action(action, NULL);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_media_control_handler, Send action result : 0x%x", result);

    AMA_Target_IssueMediaControlResponse(&(ama_activity_context.context_addr), AMA_ERROR_CODE_SUCCESS);
}

static void app_ama_set_state_ind_handler(AMA_SET_STATE_IND_T *ind)
{
    AMAStatus status;
    if (ind == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, ind is NULL", 0);
        return;
    }

    if (app_ama_is_valid_address(ind->bdAddr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, Address is INVALID", 0);
        return;
    }

    AMA_ERROR_CODE_E error_code = AMA_ERROR_CODE_SUCCESS;

#ifdef MTK_ANC_ENABLE
    bool                            anc_ret = FALSE;
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_misc_t        anc_misc;
    uint8_t                         anc_enabled = 0;
    int16_t                         anc_runtime_gain = 0;
    uint8_t                         anc_hybrid_enabled = 0;

    audio_anc_control_get_status(&anc_enabled, &anc_filter_id, &anc_type, &anc_runtime_gain, &anc_hybrid_enabled, &anc_misc);

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d)",
                        5, anc_enabled, anc_filter_id, anc_type, anc_hybrid_enabled, anc_runtime_gain);
#endif /* MTK_ANC_ENABLE */

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler : feature(0x%x), value:(0x%x)", 2, ind->state.feature, ind->state.value.boolean);

    switch (ind->state.feature) {
        case AMA_STATE_FEATURE_AVRCP_OVERRIDE: { /* 0x400 */
            ama_activity_context.context_avrcp_override = ind->state.value.boolean;
        }
        break;

        case AMA_STATE_FEATURE_DEVICE_NETWORK_STATUS: { /* 0x203 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_DEVICE_NETWORK_STATUS (0x203) value type : %d, value : %d",
                        2, ind->state.valueType, ind->state.value.integer);
        }
        break;
#if 0
        case AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL: { /* 0x209 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL (0x209) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
            if (anc_enabled == 0) {
                error_code = AMA_ERROR_CODE_INVALID;
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_FEEDBACK_ACTIVE_NOISE_CANCELLATION_LEVEL, ANC Disabled", 0);
            } else {
                error_code = AMA_ERROR_CODE_INTERNAL;
                if ((anc_enabled == 1)
                    && ((anc_type == AUDIO_ANC_CONTROL_TYPE_FB) || (app_ama_anc_info.target_anc_type == AUDIO_ANC_CONTROL_TYPE_FB))
                    && (ind->state.value.integer != anc_runtime_gain)) {
                    if (ind->state.value.integer > 0 && ind->state.value.integer < 100) {
                        anc_ret = app_anc_service_set_runtime_gain(AUDIO_ANC_CONTROL_TYPE_FB, app_ama_activity_anc_level_to_gain(ind->state.value.integer));
                        if (anc_ret) {
                            error_code = AMA_ERROR_CODE_SUCCESS;
                        }
                    } else {
                        error_code = AMA_ERROR_CODE_INVALID;
                    }
                }
            }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;
#endif
        case AMA_STATE_FEATURE_ANC_PASSTHROUGH_OFF: { /* 0x209 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_ANC_PASSTHROUGH_OFF (0x209) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
            if (ind->state.value.boolean == false && (app_ama_anc_info.target_anc_type != APP_AMA_TARGET_ANC_DISABLE)) {
                app_ama_anc_info.target_anc_type = APP_AMA_TARGET_ANC_DISABLE;
                anc_ret = app_anc_service_disable();
            } else {
                anc_ret = true;
            }

            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ANC_PASSTHROUGH_OFF, Operate (%d) ANC result : %d",
                                2, ind->state.value.boolean, anc_ret);

            if (!anc_ret) {
                error_code = AMA_ERROR_CODE_INTERNAL;
            }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
            break;
        }
        case AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED: { /* 0x210 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED (0x210) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, Current ANC enabled : %d, configure enable : %d",
                                2, anc_enabled, ind->state.value.boolean);

            if (ind->state.value.boolean == true) {
                audio_anc_control_type_t target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
                audio_anc_control_filter_id_t target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
                if (anc_hybrid_enabled) {
                    target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
                } else {
                    target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
                }
                app_ama_anc_info.target_anc_type = target_anc_type;
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, enable ANC", 0);
                anc_ret = app_anc_service_enable(target_filter_id, target_anc_type, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL);
            } else {
                if ((app_ama_anc_info.info_anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) && (app_ama_anc_info.target_anc_type != AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, disable ANC", 0);
                    app_ama_anc_info.target_anc_type = APP_AMA_TARGET_ANC_DISABLE;
                    anc_ret = app_anc_service_disable();
                } else {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, Current is not ANC mode, ignore", 0);
                }
            }

            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_ACTIVE_NOISE_CANCELLATION_ENABLED, Operate (%d) ANC result : %d",
                                2, ind->state.value.boolean, anc_ret);

            if (!anc_ret) {
                error_code = AMA_ERROR_CODE_INTERNAL;
            }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PASSTHROUGH_ENABLED: { /* 0x211 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_PASSTHROUGH_ENABLED (0x211) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
                if (ind->state.value.boolean == true) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, enable Passthrough", 0);
                    audio_anc_control_filter_id_t target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
                    app_ama_anc_info.target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
                    anc_ret = app_anc_service_enable(target_filter_id, AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL);
                } else {
                    if ((app_ama_anc_info.info_anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) && (app_ama_anc_info.target_anc_type >= AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, disable Passthrough", 0);
                        app_ama_anc_info.target_anc_type = APP_AMA_TARGET_ANC_DISABLE;
                        anc_ret = app_anc_service_disable();
                    } else {
                        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, Current is not PASSTHROUGH mode, ignore", 0);
                    }
                }
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_PASSTHROUGH_ENABLED, Operate (%d) passthrough result : %d",
                                    2, ind->state.value.boolean, anc_ret);

                if (!anc_ret) {
                    error_code = AMA_ERROR_CODE_INTERNAL;
                }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_BASS: { /* 0x450 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_BASS (0x450) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_MID: { /* 0x451 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_MID (0x451) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
        }
        break;

        case AMA_STATE_FEATURE_EQUALIZER_TREBLE: { /* 0x452 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_EQUALIZER_TREBLE (0x452) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
        }
        break;

        case AMA_STATE_FEATURE_ACTIVE_NC_LEVEL: { /* 0x205 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_ACTIVE_NC_LEVEL (0x205) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
            error_code = AMA_ERROR_CODE_INTERNAL;

            if ((anc_enabled == 1)
                && ((anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) || (app_ama_anc_info.target_anc_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF))
                && (anc_runtime_gain != ind->state.value.integer)) {
                if (ind->state.value.integer <= 100) {
                    anc_ret = app_anc_service_set_runtime_gain(anc_type, app_ama_activity_anc_level_to_gain(ind->state.value.integer));
                    if (anc_ret) {
                        error_code = AMA_ERROR_CODE_SUCCESS;
                    }
                } else {
                    error_code = AMA_ERROR_CODE_INVALID;
                }
            }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PASSTHROUGH_LEVEL: { /* 0x206 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_PASSTHROUGH_LEVEL (0x212) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef MTK_ANC_ENABLE
            error_code = AMA_ERROR_CODE_INTERNAL;

            if ((anc_enabled == 1)
                && ((anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) || (app_ama_anc_info.target_anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF))
                && (anc_runtime_gain != ind->state.value.integer)) {
                if (ind->state.value.integer <= 100) {
                    anc_ret = app_anc_service_set_runtime_gain(anc_type, app_ama_activity_anc_level_to_gain(ind->state.value.integer));
                    if (anc_ret) {
                        error_code = AMA_ERROR_CODE_SUCCESS;
                    }
                } else {
                    error_code = AMA_ERROR_CODE_INVALID;
                }
            }
#else /* MTK_ANC_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* MTK_ANC_ENABLE */
        }
        break;

        case AMA_STATE_FEATURE_PRIVACY_MODE_ENABLED: { /* 0x204 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_PRIVACY_MODE_ENABLED (0x204) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
        }
        break;

        case AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED: { /* 0x352 */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, received AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED (0x352) value type : %d, value : %d",
                                2, ind->state.valueType, ind->state.value.integer);
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            if (ind->state.value.boolean == false) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED, configure wake_word_detection_enable to be false", 0);
                ama_activity_context.context_wake_word_detection_enable = false;
                if (ama_audio_is_busy() == false) {
                    ama_audio_stop();
                } else {
                    error_code = AMA_ERROR_CODE_BUSY;
                }
            } else {
                bool suspend_by_ull = app_ama_is_ull_upstreaming();
                bool recorder_already_suspend = ama_audio_is_suspended();

                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, suspend by ULL : %d, already suspended : %d", 2, suspend_by_ull, recorder_already_suspend);
                if ((suspend_by_ull == true)
                        || (recorder_already_suspend == true)) {
                    return;
                }

                if (wwe_flash_address != 0x00 && wwe_flash_length != 0x00 && multi_va_manager_get_current_va_type() == MULTI_VA_TYPE_AMA) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED, configure wake_word_detection_enable to be true", 0);
                    ama_activity_context.context_wake_word_detection_enable = true;
                    bool audio_busy = ama_audio_is_busy();
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, AMA_STATE_FEATURE_WAKE_WORD_DETECTION_ENABLED, audio_busy : %d, wake_word_enabled : %d, model address : 0x%x, model length : 0x%x", 4,
                                        audio_busy, ama_activity_context.context_wake_word_detection_enable,
                                        wwe_flash_address, wwe_flash_length);
                    if ((audio_busy == false)
                            && (ama_activity_context.context_wake_word_detection_enable == true)
                            && (wwe_flash_address != 0x00)
                            && (wwe_flash_length != 0x00)) {
                        ama_audio_restart(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
                    }
                } else {
                    error_code = AMA_ERROR_CODE_INVALID;
                }
            }
#else /* AMA_TRIGGER_MODE_WWD_ENABLE */
            error_code = AMA_ERROR_CODE_UNSUPPORTED;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        }
        break;

        default:
            status = AMA_Target_SetStateResponse(&ind->bdAddr, AMA_ERROR_CODE_UNSUPPORTED);
            return;
    }

    status = AMA_Target_SetStateResponse(&(ama_activity_context.context_addr), error_code);
    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_set_state_ind_handler, Response error code : %d, response result : 0x%x", 2, error_code, status);
}

/**
 * @brief Forward the AT CMD to the Smart Phone, which smart phone should execute the
 * corresponding action.
 *
 * @param ind The forward at cmd indication which contains the command.
 */
void app_ama_forward_at_cmd(AMA_FORWARD_AT_COMMAND_IND_T *ind)
{
    if (ind == NULL) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, ind is NULL", 0);
        return;
    }

    if (app_ama_is_valid_address(ind->bdAddr.addr) == false) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, Address is invalid", 0);
        return;
    }

    if (strlen((char *)(ind->command)) == 0) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, AT CMD is EMPTY", 0);
        AMA_Target_ForwardATCommandResponse(&(ind->bdAddr), AMA_ERROR_CODE_INVALID);
        return;
    }

    if (bt_cm_get_profile_service_state(ind->bdAddr.addr, BT_CM_PROFILE_SERVICE_HFP) != BT_CM_PROFILE_SERVICE_STATE_CONNECTED) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, HFP profile is not connected, 0x%x",
                            1, bt_cm_get_profile_service_state(ind->bdAddr.addr, BT_CM_PROFILE_SERVICE_HFP));
        AMA_Target_ForwardATCommandResponse(&(ind->bdAddr), AMA_ERROR_CODE_INTERNAL);
        return;
    }

    bt_status_t send_status = BT_STATUS_SUCCESS;

    if (memcmp(ind->command, "ATD", strlen((char *)("ATD"))) == 0) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, Dial the phone number ongoing", 0);

        /*
         * The command from Alexa app like : ATD+86xxxxx\r
         * Remove the ATD and \r from the command, which HFP will add it.
         * */
        uint8_t x_number[64] = {0};

        if ((strlen((char *)(ind->command)) - strlen((char *)("ATD")) - 1) < 64) {
            memcpy(x_number, ind->command + strlen((char *)("ATD")), (strlen((char *)(ind->command)) - strlen((char *)("ATD")) - 1));

            send_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_DIAL_NUMBER, (void*)x_number);
        }
    }
    if (memcmp(ind->command, "AT+BLDN", strlen((char *)("AT+BLDN"))) == 0) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, Dial last number", 0);
        send_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_DIAL_LAST, NULL);
    }

    if (send_status == BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, Execute HFP AT CMD succeed", 0);
        AMA_Target_ForwardATCommandResponse(&(ind->bdAddr), AMA_ERROR_CODE_SUCCESS);
    } else {
        APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_forward_at_cmd, Execute HFP AT CMD failed with error : 0x%x", 1, send_status);
        AMA_Target_ForwardATCommandResponse(&(ind->bdAddr), AMA_ERROR_CODE_INTERNAL);
    }
}

void app_ama_activity_notify_state_change(AMA_VALUE_TYPE_E type, AMA_STATE_FEATURE_E feature, uint32_t value)
{
    if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_notify_state_change, Address is INVALID", 0);
        return;
    }

    AMA_STATE_T state;
    state.valueType = type;
    state.feature = feature;

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_notify_state_change, value type : %d, feature : 0x%x, value : %d", 3, type, feature, value);

    if (state.valueType == AMA_VALUE_TYPE_BOOLEAN) {
        state.value.boolean = (value != 0 ? true : false);
    } else {
        state.value.integer = value;
    }
    AMA_Target_SynchronizeState(&(ama_activity_context.context_addr), &state);
}

#if 0
bool app_ama_activity_proc_bt_event(ui_shell_activity_t *self,
                                    uint32_t event_id,
                                    void *extra_data,
                                    size_t data_len)
{
    bool ret = true;

    multi_va_type_t type = multi_va_manager_get_current_va_type();
    if (type != MULTI_VA_TYPE_AMA) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_proc_bt_event, Current is not AMA VA (%d), ignore", 1, type);
        return false;
    }

    /**
     * @brief For audio recorder operation
     * 0x01: Need start audio recorder
     * 0x02: Need stop audio recorder
     * 0x00: invalid state.
     */
    static uint8_t switch_state = 0x00;
    uint8_t temp_switch_state = 0x00;

    temp_switch_state = switch_state;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
            if (event != NULL) {
                bt_sink_srv_state_t curr_state = event->state_change.current;
                bt_sink_srv_state_t pre_state = event->state_change.previous;

                APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_SINK_SRV_EVENT_STATE_CHANGE] prev: 0x%x, current: 0x%x", 2, pre_state, curr_state);
                if ((pre_state >= BT_SINK_SRV_STATE_INCOMING) && (pre_state != curr_state)
                    && (curr_state < BT_SINK_SRV_STATE_INCOMING)) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_SINK_SRV_EVENT_STATE_CHANGE] HFP is finished, need start the wake word engine", 0);
                    switch_state = 0x01;
                } else if ((curr_state >= BT_SINK_SRV_STATE_INCOMING)
                            && (pre_state < BT_SINK_SRV_STATE_INCOMING)) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_SINK_SRV_EVENT_STATE_CHANGE] HFP is started, need stop wake word engine", 0);
                    switch_state = 0x02;
                } else {
                    switch_state = 0x00;
                }
            }
            ret = false;
        }
        break;

        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            /**
             * @brief Fix issue : BTA-8632
             * When the MIC has been used by eSCO which trigger Siri, should stop/start operation
             */
            bt_sink_srv_sco_state_update_t *esco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
            if (esco_state != NULL) {
                if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE] eSCO connected, need stop wake word engine", 0);
                    switch_state = 0x02;
                } else if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE] eSCO disconnected, need start wake word engine", 0);
                    switch_state = 0x01;
                } else {
                    switch_state = 0x00;
                }
            }
            ret = false;
        }
        break;

        default:
            break;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_proc_bt_event, switch_state = 0x%02x, temp_switch_state : 0x%02x", 2, switch_state, temp_switch_state);

    /**
     * @brief If the switch state not changed, ignore.
     * If the switch state is invalid state, ignore.
     */
    if ((switch_state == temp_switch_state) || (switch_state == 0x00)) {
        return false;
    }

    if (switch_state == 0x01) {
        /**
         * @brief Fix issue : BTA-8652
         * If current is speaking state, when call action happen, the speech state not changed to be IDLE from Alexa.
         * So when call end, cannot trigger Alexa again.
         */
        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        /**
         * @brief When HFP/eSCO dismissed or disconnected, need restart the audio recorder.
         *
         */
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_proc_bt_event, wake_word_enabled : %d, model address : 0x%x, model length : 0x%x", 3,
                        ama_activity_context.context_wake_word_detection_enable,
                        wwe_flash_address, wwe_flash_length);
        if ((ama_activity_context.context_wake_word_detection_enable == true)
                && (wwe_flash_address != 0x00)
                && (wwe_flash_length != 0x00)) {
            ama_audio_start(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
        }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        switch_state = 0x00;
    } else if (switch_state == 0x02) {
        /**
         * @brief When HFP/eSCO created or connected, need stop the audio recorder.
         *
         */
        ama_audio_stop();

        if (ama_activity_context.context_recognition_state != AMA_ACTIVITY_RECOGNITION_STATE_IDLE) {
            /**
             * @brief When the recorder stopped by other application (HFP/eSCO/LD)
             * Need sent the stop speech command to SP to stop current executing flow.
             */
            AMAStatus status = AMA_Target_StopSpeech(&(ama_activity_context.context_addr));
            APPS_LOG_MSGID_I(APP_AMA_ACTI", audio_recorder_stopped_callback_handler, Stop speech result :%d", 1, status);
        }

        /**
         * @brief Fix issue : BTA-8652
         * If current is speaking state, when call action happen, the speech state not changed to be IDLE from Alexa.
         * So when call end, cannot trigger Alexa again.
         */
        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
        ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
    }

    return ret;
}
#endif

void app_ama_handle_stop_recording()
{
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    bool suspend_by_ull = app_ama_is_ull_upstreaming();
    bool recorder_already_suspend = ama_audio_is_suspended();

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_stop_recording, suspend by ULL : %d, already suspended : %d", 2, suspend_by_ull, recorder_already_suspend);
    if ((suspend_by_ull == true)
            || (recorder_already_suspend == true)) {
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_handle_stop_recording, wake_word_enable : %d, model address : 0x%x, length : 0x%x", 3,
                    ama_activity_context.context_wake_word_detection_enable, wwe_flash_address, wwe_flash_length);
    if ((ama_activity_context.context_wake_word_detection_enable == true)
            && (wwe_flash_address != 0x00)
            && (wwe_flash_length != 0x00)) {
        ama_audio_restart(WWE_MODE_AMA, wwe_flash_address, wwe_flash_length);
    } else
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
    {
        if (((ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_PRESS_AND_HOLD)
                && (ama_activity_context.context_ptt_recorder_stopped == false))
                || (ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_TAP)) {
            ama_audio_stop();
        }
    }

    if (ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_PRESS_AND_HOLD) {
        ama_activity_context.context_ptt_recorder_stopped = true;
        ama_activity_context.context_ptt_start_speech_result = false;
    }

    ama_activity_context.context_trigger_mode = AMA_SPEECH_INITIATOR_TYPE_NONE;
    ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
}

bool app_ama_activity_proc_ama_event(ui_shell_activity_t *self,
                                        uint32_t event_id,
                                        void *extra_data,
                                        size_t data_len)
{
    AMAStatus status;
    bool ret = true;

    switch (event_id) {
        /* CONNECTION */
        case AMA_START_SERVICE_CFM: {
            AMA_START_SERVICE_CFM_T *start_cnf = (AMA_START_SERVICE_CFM_T *)extra_data;
            if (start_cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_START_SERVICE_CFM, Confirm result : 0x%x", 1, start_cnf->status);
            }
            break;
        }
        case AMA_STOP_SERVICE_CFM: {
            AMA_STOP_SERVICE_CFM_T *stop_cnf = (AMA_STOP_SERVICE_CFM_T *)extra_data;
            if (stop_cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_STOP_SERVICE_CFM, Confirm result : 0x%x", 1, stop_cnf->status);
            }
            break;
        }
        case AMA_DISCONNECT_IND: {
            AMA_DISCONNECT_IND_T *ind = (AMA_DISCONNECT_IND_T *)extra_data;
            if (ind) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_IND, status: 0x%x", 1, ind->status);
                if (memcmp(&ind->bdAddr, ama_activity_context.context_addr.addr, sizeof(BD_ADDR_T)) == 0) {
                    /**
                     * @brief Stop the audio recorder.
                     *
                     */
                    ama_audio_stop();
                    /**
                     * @brief Notify the multi VA that AMA disconnected.
                     *
                     */
                    multi_voice_assistant_manager_notify_va_disconnected(MULTI_VA_TYPE_AMA);

                    /**
                     * @brief Finish activity
                     *
                     */
                    ui_shell_finish_activity(self, self);

                } else {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI" AMA_DISCONNECT_IND, ind->addr: 0x%x:%x:%x:%x:%x:%x", 6,
                                        ind->bdAddr.addr[0], ind->bdAddr.addr[1], ind->bdAddr.addr[2],
                                        ind->bdAddr.addr[3], ind->bdAddr.addr[4], ind->bdAddr.addr[5]);
                    APPS_LOG_MSGID_I(APP_AMA_ACTI" AMA_DISCONNECT_IND, record_addr: 0x%x:%x:%x:%x:%x:%x", 6,
                                        ama_activity_context.context_addr.addr[0], ama_activity_context.context_addr.addr[1], ama_activity_context.context_addr.addr[2],
                                        ama_activity_context.context_addr.addr[3], ama_activity_context.context_addr.addr[4], ama_activity_context.context_addr.addr[5]);
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_IND, The bdAddr is not match, ignore", 0);
                }
            }
        }
        break;
        case AMA_NOTIFY_DEVICE_CONFIG_IND: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_NOTIFY_DEVICE_CONFIG_IND", 0);
            multi_voice_assistant_manager_set_inactive_done(MULTI_VA_TYPE_AMA);
            break;
        }
        case AMA_DISCONNECT_CFM: {
            AMA_DISCONNECT_CFM_T *cnf = (AMA_DISCONNECT_CFM_T *)extra_data;
            if (cnf) {
                if (memcmp(&cnf->bdAddr, ama_activity_context.context_addr.addr, sizeof(BD_ADDR_T)) == 0) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_CFM", 0);
                    /**
                     * @brief Stop the audio recorder.
                     *
                     */
                    ama_audio_stop();
                    /**
                     * @brief Notify the multi VA that AMA disconnected.
                     *
                     */
                    multi_voice_assistant_manager_notify_va_disconnected(MULTI_VA_TYPE_AMA);
                    /**
                     * @brief Finish activity
                     *
                     */
                    ui_shell_finish_activity(self, self);
                } else {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_CFM, cnf->addr: 0x%x:%x:%x:%x:%x:%x", 6,
                                        cnf->bdAddr.addr[0], cnf->bdAddr.addr[1], cnf->bdAddr.addr[2],
                                        cnf->bdAddr.addr[3], cnf->bdAddr.addr[4], cnf->bdAddr.addr[5]);
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_CFM, record_addr: 0x%x:%x:%x:%x:%x:%x", 6,
                                        ama_activity_context.context_addr.addr[0], ama_activity_context.context_addr.addr[1], ama_activity_context.context_addr.addr[2],
                                        ama_activity_context.context_addr.addr[3], ama_activity_context.context_addr.addr[4], ama_activity_context.context_addr.addr[5]);
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_DISCONNECT_CFM, The bdAddr is not match, ignore", 0);
                }
            }
        }
        break;
        /* SPEECH */
        case AMA_START_SPEECH_CFM: {
            AMA_START_SPEECH_CFM_T *cnf = (AMA_START_SPEECH_CFM_T *)extra_data;
            if (cnf == NULL) {
                break;
            }
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_START_SPEECH_CFM, err_code: 0x%x, trigger mode : %d", 2, cnf->errorCode, ama_activity_context.context_trigger_mode);
            if (cnf->errorCode == AMA_ERROR_CODE_SUCCESS) {

                if (ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_NONE) {
                    /**
                     * @brief For the start_speech_cfm with succeed message is after stop_speech_ind message.
                     * When stop_speech_ind received, will modify trigger mode to be none.
                     * Do not change the recognition state to be recording for next running. otherwise the next start speech should be failed
                     */
                    break;
                }

#ifdef AIR_AMA_SIDETONE_ENABLE
                /**
                 * @brief Play local VP to replace the start earcon from Alexa APP via A2DP
                 */
                apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
#endif /* AIR_AMA_SIDETONE_ENABLE */

                if (ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_TAP) {
                    ama_audio_start(WWE_MODE_NONE, 0, 0);
                }
                ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_RECORDING;

                if (ama_activity_context.context_trigger_mode == AMA_SPEECH_INITIATOR_TYPE_PRESS_AND_HOLD) {
                    ama_activity_context.context_ptt_start_speech_result = true;
                }
            } else {
                /**
                 * @brief Fix issue that when start speech failed, need restart recorder for next WWD
                 * or stop recorder (for TTT or PTT mode)
                 */
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_START_SPEECH_CFM, Start speech failed, execute stop recording", 0);
                app_ama_handle_stop_recording();
            }
        }
        break;
        case AMA_PROVIDE_SPEECH_IND: {
            AMA_PROVIDE_SPEECH_IND_T *ind = (AMA_PROVIDE_SPEECH_IND_T *)extra_data;
            if (ind != NULL) {

#ifdef AIR_AMA_SIDETONE_ENABLE
                /**
                 * @brief Play local VP to replace the start earcon from Alexa APP via A2DP
                 */
                apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
#endif /* AIR_AMA_SIDETONE_ENABLE */

                AMA_PROVIDE_SPEECH_T speech_param;
                speech_param.audioFormat = AMA_AUDIO_FORMAT_OPUS_16KHZ_32KBPS_CBR_0_20MS;
                speech_param.audioProfile = AMA_AUDIO_PROFILE_NEAR_FIELD;
                speech_param.audioSource = AMA_AUDIO_SOURCE_STREAM;

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
                if (app_adaptive_anc_get_state() != APP_ADAPTIVE_ANC_IDLE) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_PROVIDE_SPEECH_IND, recorder is using by adaptive ANC, cannot start recorder for AMA", 0);
                    AMAStatus status = AMA_Target_StopSpeech(&(ama_activity_context.context_addr));
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_PROVIDE_SPEECH_IND, Stop speech result :%d", 1, status);
                    break;
                }
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

                ama_audio_restart(WWE_MODE_NONE, 0, 0);
                ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_RECORDING;

                /**
                 * @brief For multi-link
                 */
                bt_sink_srv_set_must_play_tone_flag(&(ama_activity_context.context_addr.addr),
                                                    BT_SINK_SRV_AMA_NOTIFICATION_VOICE,
                                                    true);

                AMAStatus status = AMA_Target_ProvideSpeechResponse(&ind->bdAddr, &speech_param, AMA_ERROR_CODE_SUCCESS);
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_PROVIDE_SPEECH_IND, Response result : 0x%x", 1, status);
            }
        }
        break;
        case AMA_STOP_SPEECH_IND: {
            AMA_STOP_SPEECH_IND_T *ind = (AMA_STOP_SPEECH_IND_T *)extra_data;
            if (ind) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_STOP_SPEECH_IND , wake_word_detection_enable : %d",
                                    1, ama_activity_context.context_wake_word_detection_enable);
                app_ama_handle_stop_recording();

                status = AMA_Target_StopSpeechResponse(&ind->bdAddr);
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_STOP_SPEECH_IND, Response result : 0x%x", 1, status);
            }
        }
        break;
        case AMA_STOP_SPEECH_CFM: {
            AMA_STOP_SPEECH_CFM_T *cnf = ((AMA_STOP_SPEECH_CFM_T *)extra_data);
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_STOP_SPEECH_CFM, wake_word_detection_enable : %d",
                                    1, ama_activity_context.context_wake_word_detection_enable);
                app_ama_handle_stop_recording();
            }
        }
        break;
        case AMA_ENDPOINT_SPEECH_IND: {
            AMA_ENDPOINT_SPEECH_IND_T *ind = ((AMA_ENDPOINT_SPEECH_IND_T *)extra_data);
            if (ind == NULL) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_ENDPOINT_SPEECH_IND, there has some error, ind is NULL", 0);
                break;
            }
            if (memcmp(&ind->bdAddr, ama_activity_context.context_addr.addr, sizeof(BD_ADDR_T)) == 0) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_ENDPOINT_SPEECH_IND, wake_word_detection_enable : %d",
                                    1, ama_activity_context.context_wake_word_detection_enable);
                app_ama_handle_stop_recording();

                status = AMA_Target_EndpointSpeechResponse(&ind->bdAddr);
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_ENDPOINT_SPEECH_IND, Response result : 0x%x", 1, status);
            }
        }
        break;
        case AMA_ENDPOINT_SPEECH_CFM: {
            AMA_ENDPOINT_SPEECH_CFM_T *cnf = (AMA_ENDPOINT_SPEECH_CFM_T *)extra_data;
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI",AMA_ENDPOINT_SPEECH_CFM: error code : %x", 1, cnf->errorCode);
                app_ama_handle_stop_recording();
            }
        }
        break;
        case AMA_SPEECH_STATE_IND: {
            AMA_SPEECH_STATE_IND_T *ind = (AMA_SPEECH_STATE_IND_T *)extra_data;

            if (ind != NULL) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_SPEECH_STATE_IND, New Speech State : 0x%x", 1, ind->state);
                switch (ind->state) {
                    case AMA_SPEECH_STATE_IDLE:
                        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_IDLE;
                        /**
                         * @brief For multi-link
                         * When recognition is done, need set to be false.
                         */
                        bt_sink_srv_set_must_play_tone_flag(&(ama_activity_context.context_addr.addr),
                                                            BT_SINK_SRV_AMA_NOTIFICATION_VOICE,
                                                            false);
                        break;
                    case AMA_SPEECH_STATE_LISTENING:
                        break;
                    case AMA_SPEECH_STATE_PROCESSING:
                        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_THINKING;
                        break;
                    case AMA_SPEECH_STATE_SPEAKING:
                        ama_activity_context.context_recognition_state = AMA_ACTIVITY_RECOGNITION_STATE_SPEAKING;
                        break;
                }
            }
        }
        break;
        /* STATE */
        case AMA_GET_STATE_CFM: {
            AMA_GET_STATE_CFM_T *cnf = (AMA_GET_STATE_CFM_T *)extra_data;
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_GET_STATE_CFM, erro_code: 0x%x, feature : 0x%x, value: 0x%x, value_type : 0x%x",
                                    4, cnf->errorCode, cnf->state.feature, cnf->state.value, cnf->state.valueType);
            }
        }
        break;
        case AMA_GET_STATE_IND: {
            app_ama_get_state_ind_handler((AMA_GET_STATE_IND_T *)extra_data);
        }
        break;
        case AMA_SET_STATE_IND: {
            app_ama_set_state_ind_handler((AMA_SET_STATE_IND_T *)extra_data);
        }
        break;
        case AMA_SYNCHRONIZE_STATE_CFM: {
            AMA_SYNCHRONIZE_STATE_CFM_T *cnf = ((AMA_SYNCHRONIZE_STATE_CFM_T *)extra_data);
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_SYNCHRONIZE_STATE_CFM, error_code : 0x%x", 1, cnf->errorCode);
            }
            break;
        }
        case AMA_SYNCHRONIZE_STATE_IND: {
            AMA_SYNCHRONIZE_STATE_IND_T *ind = (AMA_SYNCHRONIZE_STATE_IND_T *)extra_data;
            AMA_Target_SynchronizeStateResponse(&ind->bdAddr, AMA_ERROR_CODE_SUCCESS);
        }
        break;
        /* MEDIA */
        case AMA_ISSUE_MEDIA_CONTROL_CFM: {
            AMA_ISSUE_MEDIA_CONTROL_CFM_T *cnf = (AMA_ISSUE_MEDIA_CONTROL_CFM_T *)extra_data;
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_ISSUE_MEDIA_CONTROL_CFM, error_code : 0x%x", 1, cnf->errorCode);
            }
        }
        break;
        case AMA_ISSUE_MEDIA_CONTROL_IND: {
            app_ama_media_control_handler((AMA_ISSUE_MEDIA_CONTROL_IND_T *)extra_data);
        }
        break;
        /* Cellular Calling */
        case AMA_FORWARD_AT_COMMAND_IND: {
            //app_AmaForwardATCmdIndHandler((AMA_FORWARD_AT_COMMAND_IND_T *)extra_data);
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_FORWARD_AT_COMMAND_IND", 0);
            app_ama_forward_at_cmd((AMA_FORWARD_AT_COMMAND_IND_T *)extra_data);
        }
        break;
        case AMA_INCOMING_CALL_CFM: {
            //app_AmaIncomingCallCfmHandler((AMA_INCOMING_CALL_CFM_T *)extra_data);
        }
        break;
        /* MISC IND*/
        case AMA_SWITCH_TRANSPORT_IND: {
            AMA_SWITCH_TRANSPORT_IND_T *ind = (AMA_SWITCH_TRANSPORT_IND_T *)extra_data;
            if (ind) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_SWITCH_TRANSPORT_IND, old_addr %x, %x, %x,%x,%x,%x", 6,
                                    ind->bdAddr.addr[0], ind->bdAddr.addr[1], ind->bdAddr.addr[2],
                                    ind->bdAddr.addr[3], ind->bdAddr.addr[4], ind->bdAddr.addr[5]);

                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_SWITCH_TRANSPORT_IND, local_addr %x, %x, %x,%x,%x,%x", 6,
                                    ama_activity_context.context_addr.addr[0], ama_activity_context.context_addr.addr[1], ama_activity_context.context_addr.addr[2],
                                    ama_activity_context.context_addr.addr[3], ama_activity_context.context_addr.addr[4], ama_activity_context.context_addr.addr[5]);

                if (memcmp(&(ind->bdAddr), ama_activity_context.context_addr.addr, sizeof(BD_ADDR_T)) == 0) {
                    memcpy(ama_activity_context.context_addr.addr, &(ind->newBdAddr), sizeof(BD_ADDR_T));
                }
            }
        }
        break;

        case AMA_UPDATE_DEVICE_INFORMATION_IND: {
            //app_AmaUpdateDeviceInfoIndHandler((AMA_UPDATE_DEVICE_INFORMATION_IND_T *)extra_data);
        }
        break;
        case AMA_OVERRIDE_ASSISTANT_IND: {
            AMA_OVERRIDE_ASSISTANT_IND_T *ind = (AMA_OVERRIDE_ASSISTANT_IND_T *)extra_data;
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_OVERRIDE_ASSISTANT_IND, error code : %d", 1, ind->errorCode);
            if (ama_activity_context.context_start_setup == true) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_OVERRIDE_ASSISTANT_IND, is start_setup, wait setup complete", 0);
                break;
            }
            if (ind->errorCode == AMA_ERROR_CODE_SUCCESS) {

                /**
                 * @brief Notify multi VA that AMA connected to switch VA.
                 */
                multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_AMA);

                multi_va_type_t type = multi_va_manager_get_current_va_type();
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_OVERRIDE_ASSISTANT_IND cur_va_type: %x", 1, type);
                /**
                 * @brief When override assistant happen, means user connect AMA from the
                 * Alexa APP, means user switch VA to AMA, so need start WWE.
                 */
                if (type == MULTI_VA_TYPE_AMA) {
                    /**
                     * @brief Init the audio recorder for WWD enable.
                     */
                    ama_audio_init(audio_recorder_stopped_callback_handler, audio_recorder_released_callback_handler);

                    /**
                     * @brief Start audio recorder for wake word detection.
                     *
                     */
                    app_ama_activity_start_wwe();

                    /**
                     * @brief Init ANC information and register callback to monitor the ANC change.
                     *
                     */
                    app_ama_activity_init_anc();

                    /**
                     * @brief Check the A2DP and HFP is connected or not and notify state to Alexa APP.
                     */
                    app_ama_activity_state_checking();
                }
            }
        }
        break;
        /* MISC CFM*/
        case AMA_GET_CENTRAL_INFORMATION_CFM: {
            AMA_GET_CENTRAL_INFORMATION_CFM_T *cnf = (AMA_GET_CENTRAL_INFORMATION_CFM_T *)extra_data;
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_GET_CENTRAL_INFORMATION_CFM, error_code : 0x%x, paltom : 0x%x", 2, cnf->errorCode, cnf->platform);
            }
        }
        break;
        case AMA_OVERRIDE_ASSISTANT_CFM: {
            AMA_OVERRIDE_ASSISTANT_CFM_T *cnf = ((AMA_OVERRIDE_ASSISTANT_CFM_T *)extra_data);
            if (cnf) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_OVERRIDE_ASSISTANT_CFM, error_code : 0x%x", 1, cnf->errorCode);
                multi_voice_assistant_manager_set_inactive_done(MULTI_VA_TYPE_AMA);
            }
        }
        break;
        case AMA_START_SETUP_IND: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_START_SETUP_IND", 0);
            ama_activity_context.context_start_setup = true;
        }
        break;
        case AMA_COMPLETE_SETUP_IND: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_COMPLETE_SETUP_IND", 0);
            ama_activity_context.context_start_setup = false;

            multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_AMA);

            ama_audio_init(audio_recorder_stopped_callback_handler, audio_recorder_released_callback_handler);

            /**
             * @brief Start wake word engine.
             *
             */
            app_ama_activity_start_wwe();

            /**
             * @brief Init ANC information and register callback to monitor the ANC change.
             *
             */
            app_ama_activity_init_anc();

            /**
             * @brief Check the A2DP and HFP is connected or not and notify state to Alexa APP.
             */
            app_ama_activity_state_checking();
        }
        break;
        case AMA_FACTORY_RESET_CFM: {
            //app_AmaFactoryResetCfmHandler((AMA_FACTORY_RESET_CFM_T *)msg);
        }
        break;
        /** GET_LOCALE_IND */
        case AMA_GET_LOCALES_IND: {
            AMA_GET_LOCALES_IND_T *get_locale = (AMA_GET_LOCALES_IND_T *)extra_data;
            if (get_locale == NULL) {
                APPS_LOG_MSGID_E(APP_AMA_ACTI", AMA_GET_LOCALES_IND, get_locale is NULL", 0);
                return true;
            }
            APPS_LOG_MSGID_E(APP_AMA_ACTI", AMA_GET_LOCALES_IND enter", 0);
            app_ama_handle_get_locales_ind(get_locale);
        }
        break;
        /** SET_LOCALE_IND */
        case AMA_SET_LOCALE_IND: {
            AMA_SET_LOCALE_IND_T *locale = (AMA_SET_LOCALE_IND_T *)extra_data;
            if (locale == NULL) {
                APPS_LOG_MSGID_E(APP_AMA_ACTI", AMA_SET_LOCALE_IND, locale is NULL", 0);
                return true;
            }
            app_ama_handle_set_locale_ind(locale);
        }
        break;

        /** GetDeviceFeatures */
        case AMA_GET_DEVICE_FEATURES_IND: {
            AMA_GET_DEVICE_FEATURES_IND_T *device_features = (AMA_GET_DEVICE_FEATURES_IND_T *)extra_data;
            if (device_features == NULL) {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_GET_DEVICE_FEATURES_IND, device_features is NULL", 0);
                return true;
            }

            APPS_LOG_MSGID_I(APP_AMA_ACTI", AMA_GET_DEVICE_FEATURES_IND, get device features", 0);

            /**
             * @brief TODO need implemented by customer according to the configuration
             * Currently all are enabled.
             */
            AMA_GetDeviceFeaturesRsp_t rsp_features = {0};
            rsp_features.error_code = AMA_ERROR_CODE_SUCCESS;
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
            rsp_features.battery_enabled = 1;
#endif /* MTK_BATTERY_MANAGEMENT_ENABLE */

#ifdef MTK_ANC_ENABLE
            rsp_features.anc_enabled = 1;
            rsp_features.passthrough_enabled = 1;
#endif /* MTK_ANC_ENABLE */
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            rsp_features.wake_word_enabled = 1;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
            rsp_features.privacy_mode_enabled = 0;
            rsp_features.equalizer_enabled = 0;

            AMA_Target_ResponseDeviceFeatures(&(device_features->bdAddr), &rsp_features);
        }
        break;

        /* Launch 3rd-party APP by using iap2_mux API with app_id. */
        case AMA_LAUNCH_APP_IND: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", launch 3rd-party APP", 0);
#if defined(AMA_IAP2_VIA_MUX_ENABLE) && defined(MTK_MUX_ENABLE)
            AMA_LAUNCH_APP_IND_T *ind = (AMA_LAUNCH_APP_IND_T *)extra_data;
            mux_iap2_send_app_launch_request((uint8_t *)ind->app_id, FALSE);
#endif
        }
        break;

        case APP_AMA_UI_SHELL_EVENT_ID_HANDLE_ANC_CHANGE: {
            /**
             * @brief Fix issue : BTA-9391
             * Handle the ANC change in the UI shell task.
             */
            APPS_LOG_MSGID_I(APP_AMA_ACTI", APP_AMA_UI_SHELL_EVENT_ID_HANDLE_ANC_CHANGE", 0);
            app_ama_activity_handle_audio_anc_change();
        }
        break;

        case APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE: {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", APP_AMA_UI_SHELL_EVENT_ID_HANDLE_LONG_PRESS_RELEASE", 0);
            app_ama_handle_long_press_trigger_release();
        }
        break;

        default:
            break;
    }
    return ret;
}

bool app_ama_activity_process_bt_cm_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (remote_update == NULL) {
                APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_activity_process_bt_cm_proc, remote_update null", 0);
                return false;
            }

            if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == false) {
                APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_activity_process_bt_cm_proc, The address is INVALID", 0);
                return false;
            }

            /**
             * @brief Fix issue that if the update address is not the AMA connected address, just ignore the event.
             */
            if (memcmp(ama_activity_context.context_addr.addr, remote_update->address, BT_BD_ADDR_LEN) != 0) {
                APPS_LOG_MSGID_E(APP_AMA_ACTI", app_ama_activity_process_bt_cm_proc, NOT the AMA connected address", 6,
                        remote_update->address[0], remote_update->address[1], remote_update->address[2],
                        remote_update->address[3], remote_update->address[4], remote_update->address[5]);
                return false;
            }

            bool pre_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) & remote_update->pre_connected_service) > 0);
            if (!pre_hfp_conn) {
                pre_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP) & remote_update->pre_connected_service) > 0);
            }
            bool pre_a2dp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->pre_connected_service) > 0);
            bool cur_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) & remote_update->connected_service) > 0;
            if (!cur_hfp_conn) {
                cur_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP) & remote_update->connected_service) > 0);
            }
            bool cur_a2dp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->connected_service) > 0);

            APPS_LOG_MSGID_I(APP_AMA_ACTI", bt_info_update acl=%d->%d srv=0x%08X->0x%08X hfp=%d->%d a2dp=%d->%d", 8,
                                remote_update->pre_acl_state, remote_update->acl_state,
                                remote_update->pre_connected_service, remote_update->connected_service,
                                pre_hfp_conn, cur_hfp_conn, pre_a2dp_conn, cur_a2dp_conn);

            if (!cur_hfp_conn && !cur_a2dp_conn && (pre_hfp_conn || pre_a2dp_conn)) {
                /**
                 * @brief If A2DP/HFP disconnected, disconnect AMA link
                 * For bug fix.
                 * If not disconnected AMA, the AMA should keep connected, cannot pass the Amazon certification.
                 */
                APPS_LOG_MSGID_I(APP_AMA_ACTI", [BT_CM_EVENT_REMOTE_INFO_UPDATE] A2DP and HFP disconnected, Disconnect AMA link", 0);
                AMA_Target_DisconnectRequest(&(ama_activity_context.context_addr));
                return false;
            }

            if (!pre_a2dp_conn && cur_a2dp_conn) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_A2DP_CONNECTED, true);
            } else if (pre_a2dp_conn && !cur_a2dp_conn) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_A2DP_CONNECTED, false);
            }

            if (!pre_hfp_conn && cur_hfp_conn) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_HFP_CONNECTED, true);
            } else if (pre_hfp_conn && !cur_hfp_conn) {
                app_ama_activity_notify_state_change(AMA_VALUE_TYPE_BOOLEAN, AMA_STATE_FEATURE_HFP_CONNECTED, false);
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
bool app_ama_activity_process_aws_data_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION && role == BT_AWS_MCE_ROLE_AGENT) {

        uint32_t event_group;
        uint32_t action;

        void *stop_index_p = NULL;
        uint32_t stop_index = 0;
        uint32_t stop_index_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action, &stop_index_p, &stop_index_len);

        if (event_group != EVENT_GROUP_UI_SHELL_KEY) {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_process_aws_data_proc, Not key event, 0x%04x", 1, event_group);
            return false;
        }

        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_process_aws_data_proc, Received key event : 0x%04x, stop_index : 0x%x",
                            2, action, stop_index_p);

        if (action == KEY_AMA_START) {
            if (stop_index_p != NULL && stop_index_len == sizeof(uint32_t)) {
                stop_index = (uint32_t)stop_index_p;

                if (stop_index == 0) {
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_process_aws_data_proc, Key trigger", 0);
                    return app_ama_handle_key_event(action);
                } else {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
                    APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_process_aws_data_proc, WWD trigger stop_index : %d", 1, stop_index);
                    start_speech_with_wwd(false, stop_index);
                    return true;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
                }
            } else {
                APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_activity_process_aws_data_proc, KEY_AMA_START key trigger on partner", 0);
                return app_ama_handle_key_event(action);
            }
        } else {
            return app_ama_handle_key_event(action);
        }
        return true;
    }
    return false;
}
#endif

/**
 * @brief Set the AMA selected by multi-VA model or not.
 *
 * @param selected The AMA selected or not. true means selected, false means not selected.
 * @return
 */
bool app_ama_multi_va_set_configuration(bool selected)
{
    if (selected == false) {
        if (app_ama_is_valid_address(ama_activity_context.context_addr.addr) == true) {
            AMA_Target_SetSelected(false);
            AMA_Target_SetDeviceConfiguration(&(ama_activity_context.context_addr), true, true);
            AMA_Target_DisconnectRequest(&(ama_activity_context.context_addr));
            return true;
        } else {
            APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_multi_va_set_configuration, Address is INVALID", 0);
            return false;
        }
    }
    return false;
}

bool app_ama_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    // APPS_LOG_MSGID_I(APP_AMA_ACTI", event_group : %x, id : %x", 2, event_group, event_id);
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#if 0
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            app_ama_activity_proc_bt_event(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_ama_activity_process_bt_cm_proc(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_AMA: {
            ret = app_ama_activity_proc_ama_event(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            app_ama_activity_process_aws_data_proc(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }

    // APPS_LOG_MSGID_I(APP_AMA_ACTI", ret : %x", 1, ret);
    return ret;
}

void app_ama_set_connected_address(uint8_t *src_addr)
{
    if (src_addr == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI ", app_ama_set_connected_address, Source address is NULL", 0);
        return;
    }
    if (app_ama_is_valid_address(src_addr) == false) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI ", app_ama_set_connected_address, The source address is INVALID", 0);
        return;
    }
    memcpy(ama_activity_context.context_addr.addr, src_addr, BT_BD_ADDR_LEN);
}

void app_ama_reset_connected_address()
{
    memset(ama_activity_context.context_addr.addr, 0, BT_BD_ADDR_LEN);
}

bool app_ama_is_connected()
{
    return app_ama_is_valid_address(ama_activity_context.context_addr.addr);
}

void app_ama_configure_wwe_parameter(uint8_t *language)
{
    if (language == NULL) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI ", app_ama_configure_wwe_parameter, language is NULL", 0);
        return;
    }
    if (strlen((char *)language) == 0) {
        APPS_LOG_MSGID_E(APP_AMA_ACTI ", app_ama_configure_wwe_parameter, language is empty", 0);
        return;
    }

    APPS_LOG_MSGID_E(APP_AMA_ACTI ", app_ama_configure_wwe_parameter, Configured language is : 0x%02x %02x %02x %02x %02x", 5,
                        language[0], language[1], language[2], language[3], language[4]);

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
    va_model_manager_error_code_t err = VA_MODEL_MANAGER_ERROR_CODE_OK;
    va_model_manager_model_info_t load_info;

    char *target_locale = app_ama_match_locale((char *)language);
    if (target_locale == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_ACTI", app_ama_configure_wwe_parameter, failed to mapping locale to the expected locale. 0x%02x %02x %02x %02x %02x %02x", 6,
                        language[0], language[1], language[2], language[3], language[4], language[5]);
        return;
    }

    memset(wwe_language, 0, sizeof(uint8_t) * VA_MODEL_MANAGER_MODEL_LANGUAGE_LENGTH);

    err = va_model_manager_load(VA_MODEL_MANAGER_VA_TYPE_AMA, (uint8_t *)target_locale, &load_info);
    err = va_model_manager_load_finish(VA_MODEL_MANAGER_VA_TYPE_AMA, (uint8_t *)target_locale);
    APPS_LOG_MSGID_E(APP_AMA_ACTI ", app_ama_configure_wwe_parameter, Load language result : 0x%x, address : 0x%08x, length 0x%08x", 3,
                        err, load_info.model_flash_address, load_info.model_length);
    if (load_info.model_flash_address != 0x00 && load_info.model_length != 0x00) {
        if (strlen(target_locale) > (sizeof(AMA_LOCALE_NAME_T) - 1)) {
            memcpy(wwe_language, target_locale, (sizeof(AMA_LOCALE_NAME_T) - 1));
        } else {
            memcpy(wwe_language, target_locale, strlen(target_locale));
        }

        wwe_flash_address = load_info.model_flash_address;
        wwe_flash_length = load_info.model_length;
    }
#endif
}

uint32_t app_ama_get_mtu()
{
    if (app_ama_is_connected() == false) {
        return 0;
    }

    return AMA_Target_GetMtu(&(ama_activity_context.context_addr));
}


#endif

