
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
 * File: app_va_xiaowei_device_control.c
 *
 * Description: This file implement the xiaowei device control features
 *
 */

#include "app_va_xiaowei_device_control.h"
#include "apps_debug.h"
#include "app_anc_service.h"

#ifdef MTK_VA_XIAOWEI_ENABLE

#define APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX        "[VA_XIAOWEI] VA_XIAOWEI_GENERA_CONFIG"

#define APP_VA_XIAOWEI_ANC_STEP         ((AUDIO_ANC_CONTROL_RAMP_GAIN_MAX - AUDIO_ANC_CONTROL_RAMP_GAIN_MIN) / (XIAOWEI_GENERAL_CONFIG_ANC_MAX_VALUE - XIAOWEI_GENERAL_CONFIG_ANC_MIN_VALUE))
// #define APP_VA_XIAOWEI_ANC_STEP         ((AUDIO_ANC_CONTROL_RAMP_GAIN_MAX - AUDIO_ANC_CONTROL_RAMP_GAIN_MIN) / (XIAOWEI_GENERAL_CONFIG_ANC_MAX_VALUE - XIAOWEI_GENERAL_CONFIG_ANC_MIN_VALUE))

bool static app_va_xiaowei_device_control_anc_operate(xiaowei_payload_general_config_request_t *config);
bool static app_va_xiaowei_device_control_eq_operate(xiaowei_payload_general_config_request_t *config);

bool static app_va_xiaowei_device_control_anc_operate(xiaowei_payload_general_config_request_t *config)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [ANC] operate feature : 0x%04x", 1, config->req_feature);

#ifdef MTK_ANC_ENABLE
    audio_anc_control_filter_id_t   anc_filter_id = AUDIO_ANC_CONTROL_FILTER_ID_NONE;
    audio_anc_control_type_t        anc_type = AUDIO_ANC_CONTROL_TYPE_DUMMY;
    audio_anc_control_misc_t        anc_control_misc;
    uint8_t                         anc_enabled = 0;
    int16_t                         anc_runtime_gain = 0;
    uint8_t                         anc_hybrid_enabled = 0;

    audio_anc_control_get_status(&anc_enabled, &anc_filter_id, &anc_type, &anc_runtime_gain, &anc_hybrid_enabled, &anc_control_misc);
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", ANC Information, enabled (%d), filter_id (%d), anc_type (%d), anc_hybrid_enabled (%d), gain (%d)",
                        5, anc_enabled, anc_filter_id, anc_type, anc_hybrid_enabled, anc_runtime_gain);

    switch (config->req_feature) {
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_REPORT: {
            return true;
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_GAIN_INCREASE: {
            if (anc_enabled == false) {
                return false;
            }
            if ((anc_type != AUDIO_ANC_CONTROL_TYPE_HYBRID)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FF)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FB)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Type is not ANC, ignore it", 0);
                return false;
            }

            if ((anc_runtime_gain + APP_VA_XIAOWEI_ANC_STEP) > AUDIO_ANC_CONTROL_RAMP_GAIN_MAX) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Set gain to be max value", 0);
                if (app_anc_service_set_runtime_gain(anc_type, AUDIO_ANC_CONTROL_RAMP_GAIN_MAX) == true) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Set gain to be max value succeed", 0);
                    return true;
                }
                APPS_LOG_MSGID_E(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Set gain to be max value failed", 0);
                return false;
            }
            anc_runtime_gain += APP_VA_XIAOWEI_ANC_STEP;
            if (app_anc_service_set_runtime_gain(anc_type, anc_runtime_gain) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Set gain succeed with value : %d", 1, anc_runtime_gain);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [INCREASE ANC GAIN] Set gain failed with value : %d", 1, anc_runtime_gain);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_GAIN_DECREASE: {
            if (anc_enabled == false) {
                return false;
            }
            if ((anc_type != AUDIO_ANC_CONTROL_TYPE_HYBRID)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FF)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FB)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Type is not ANC, ignore it", 0);
                return false;
            }

            if ((anc_runtime_gain - APP_VA_XIAOWEI_ANC_STEP) < AUDIO_ANC_CONTROL_RAMP_GAIN_MIN) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Set gain to be min value", 0);
                if (app_anc_service_set_runtime_gain(anc_type, AUDIO_ANC_CONTROL_RAMP_GAIN_MIN) == true) {
                    APPS_LOG_MSGID_E(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Set gain to be min value succeed", 0);
                    return true;
                }
                APPS_LOG_MSGID_E(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Set gain to be min value failed", 0);
                return false;
            }
            anc_runtime_gain -= APP_VA_XIAOWEI_ANC_STEP;
            if (app_anc_service_set_runtime_gain(anc_type, anc_runtime_gain) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Set gain succeed with value : %d", 1, anc_runtime_gain);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DECREASE ANC GAIN] Set gain failed with value : %d", 1, anc_runtime_gain);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_GAIN_SET: {
            if (anc_enabled == false) {
                return false;
            }
            if ((anc_type != AUDIO_ANC_CONTROL_TYPE_HYBRID)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FF)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FB)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [SET ANC GAIN] Type is not ANC, ignore it", 0);
                return false;
            }

            anc_runtime_gain = (AUDIO_ANC_CONTROL_RAMP_GAIN_MIN + (config->req_payload[0] * APP_VA_XIAOWEI_ANC_STEP));

            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [SET ANC GAIN] Request gain : %d, target gain : %d", 2,
                                config->req_payload[0], anc_runtime_gain);

            if (app_anc_service_set_runtime_gain(anc_type, anc_runtime_gain) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [SET ANC GAIN] Set gain succeed with value : %d", 1, anc_runtime_gain);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [SET ANC GAIN] Set gain failed with value : %d", 1, anc_runtime_gain);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_ON: {
            if ((anc_enabled == true)
                    && (
                        (anc_type == AUDIO_ANC_CONTROL_TYPE_HYBRID)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_FF)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_FB)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)
                    )) {
                return true;
            }
            audio_anc_control_type_t target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
            if (anc_hybrid_enabled) {
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
            } else {
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
            }
            if (app_anc_service_enable(AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT, target_anc_type, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [ENABLE ANC] Enable ANC succeed", 0);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [ENABLE ANC] Enable ANC failed", 0);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_OFF: {
            if ((anc_enabled == true)
                    && (
                        (anc_type == AUDIO_ANC_CONTROL_TYPE_HYBRID)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_FF)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_FB)
                        || (anc_type == AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)
                    )) {
                if (app_anc_service_disable() == true) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DISABLE ANC] Disable ANC succeed", 0);
                    return true;
                }
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [DISABLE ANC] Enable ANC failed", 0);
            }
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_GAIN_MAX: {
            if (anc_enabled == false) {
                return false;
            }
            if ((anc_type != AUDIO_ANC_CONTROL_TYPE_HYBRID)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FF)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FB)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MAX ANC GAIN] Type is not ANC, ignore it", 0);
                return false;
            }
            if (app_anc_service_set_runtime_gain(anc_type, AUDIO_ANC_CONTROL_RAMP_GAIN_MAX) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MAX ANC GAIN] Set MAX gain succeed", 0);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MAX ANC GAIN] Set MAX gain failed", 0);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_NC_GAIN_MIN: {
            if (anc_enabled == false) {
                return false;
            }
            if ((anc_type != AUDIO_ANC_CONTROL_TYPE_HYBRID)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FF)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_FB)
                    && (anc_type != AUDIO_ANC_CONTROL_TYPE_USER_DEFINED)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MIN ANC GAIN] Type is not ANC, ignore it", 0);
                return false;
            }
            if (app_anc_service_set_runtime_gain(anc_type, AUDIO_ANC_CONTROL_RAMP_GAIN_MIN) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MIN ANC GAIN] Set MIN gain succeed", 0);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [MIN ANC GAIN] Set MIN gain failed", 0);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_CONVERSATION_MODE_ON: {
            if ((anc_enabled == true) && (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [CONVERSATION ON] Already conversation mode", 0);
                return true;
            }
            if (app_anc_service_enable(AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT, AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL) == true) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [CONVERSATION ON] Turn on conversation mode succeed", 0);
                return true;
            }
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [CONVERSATION ON] Turn on conversation mode failed", 0);
        }
        break;
        case XIAOWEI_GENERAL_CONFIG_FEATURE_CONVERSATION_MODE_OFF: {
            if ((anc_enabled == true) && (anc_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF)) {
                if (app_anc_service_disable() == true) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [CONVERSATION OFF] Turn off conversation mode succeed", 0);
                    return true;
                }
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", [CONVERSATION OFF] Turn off conversation mode failed", 0);
            }
        }
        break;
        default : {
            return false;
        }
    }

    return false;
#else
    return false;
#endif /* MTK_ANC_ENABLE */
}

bool static app_va_xiaowei_device_control_eq_operate(xiaowei_payload_general_config_request_t *config)
{
    return false;
}

bool app_va_xiaowei_device_control_handler(xiaowei_payload_general_config_request_t *config)
{
    if (config == NULL) {
        return false;
    }

    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_GENERAL_CONFIG_PREFIX", device control hander, feature : 0x%04x, length : %d",
                        2, config->req_feature, config->req_payload_length);

    uint8_t index = 0;
    if (config->req_feature >= XIAOWEI_GENERAL_CONFIG_FEATURE_NC
        && config->req_feature <= XIAOWEI_GENERAL_CONFIG_FEATURE_CONVERSATION_MODE_OFF) {
        return app_va_xiaowei_device_control_anc_operate(config);
    }

    if (config->req_feature >= XIAOWEI_GENERAL_CONFIG_FEATURE_EQ
        && config->req_feature <= XIAOWEI_GENERAL_CONFIG_FEATURE_EQ_SET) {
        return app_va_xiaowei_device_control_eq_operate(config);
    }

    return false;
}

#endif /* MTK_VA_XIAOWEI_ENABLE */
