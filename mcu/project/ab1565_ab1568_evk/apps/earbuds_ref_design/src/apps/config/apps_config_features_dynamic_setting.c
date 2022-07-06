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
 * File: apps_config_features_dynamic_setting.c
 *
 * Description: This file define ATCI for APP features.
 *
 */

#include "atci.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_config_features_dynamic_setting.h"
#include "apps_debug.h"
#include "apps_customer_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 *  @brief This structure defines format of the static variables of the file.
 */
typedef struct _apps_config_features {
    uint8_t support_auto_rho;                   /* The flag if support auto rho. */
    bool no_connection_sleep_mode_disable_bt;   /* If it's true, only do BT OFF when power saving. */
} apps_config_features_t;

#if (defined(APPS_AUTO_TRIGGER_RHO) && defined(APPS_TRIGGER_RHO_BY_KEY)) || (defined(APPS_SLEEP_AFTER_NO_CONNECTION) && (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF))
/* Static variable. */
static apps_config_features_t s_apps_features = {
    .support_auto_rho = 1,
    .no_connection_sleep_mode_disable_bt = false
};
#endif

#if defined(APPS_AUTO_TRIGGER_RHO) && defined(APPS_TRIGGER_RHO_BY_KEY)
#define APP_DYNAMIC_FEATURE_NAME_AUTO_RHO           "AUTORHO"   /* The 2nd parameter of the ATCI, means set support_auto_rho. */

bool apps_config_features_is_auto_rho_enabled(void)
{
    return s_apps_features.support_auto_rho == 1;
}

#endif

#if defined(APPS_SLEEP_AFTER_NO_CONNECTION) && (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF)
#define APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE   "NOCONN_SLEEP"  /* The 2nd parameter of the ATCI, means set no_connection_sleep_mode_disable_bt. */

bool apps_config_features_is_no_connection_sleep_mode_bt_off(void)
{
    return s_apps_features.no_connection_sleep_mode_disable_bt == 1;
}
#endif

/* The flag means if current is MP test mode. */
static uint8_t s_mp_test_mode = 0;

bool apps_config_features_is_mp_test_mode(void)
{
    return (s_mp_test_mode != 0);
}

/**
 * @brief      The ATCI cmd handler, refer to at_cmd_hdlr_fp.
 * @param[in]  parse_cmd, The value is defined in #atci_parse_cmd_param_t. This parameter is given by the ATCI
 *             parser to indicate the input command data to be transferred to the command handler.
 * @return     ATCI_STATUS_OK means success, otherwise means fail.
 */
static atci_status_t _set_features_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *saveptr;
    char *param1 = NULL;
    char *param2 = NULL;
    char *param3 = NULL;
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    APPS_LOG_MSGID_I("_set_features_cmd_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            param1 = strtok_r(parse_cmd->string_ptr + parse_cmd->name_len + 1, ",", &saveptr);
            param2 = strtok_r(NULL, ",", &saveptr);
            /* AT+APPFEATURE=SET,<param2>,<param3> */
            if (strcmp(param1, "SET") == 0 && param2) {
                param3 = strtok_r(NULL, ",", &saveptr);
                if (param3) {
#if defined(APPS_AUTO_TRIGGER_RHO) && defined(APPS_TRIGGER_RHO_BY_KEY)
                    /* AT+APPFEATURE=SET,AUTORHO,<param3> */
                    if (strncmp(param2, APP_DYNAMIC_FEATURE_NAME_AUTO_RHO, strlen(APP_DYNAMIC_FEATURE_NAME_AUTO_RHO)) == 0) {
                        int32_t value = atoi(param3);
                        s_apps_features.support_auto_rho = value ? 1 : 0;
                        APPS_LOG_MSGID_I("set "APP_DYNAMIC_FEATURE_NAME_AUTO_RHO"to %d", 1, s_apps_features.support_auto_rho);
                        nvdm_write_data_item(NVDM_GROUP_APP_DYNAMIC_FEATURE, NVDM_APP_DYNAMIC_FEATURE_ITEM_AUTO_RHO,
                                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                             (uint8_t *)&s_apps_features,
                                             sizeof(s_apps_features));
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
#endif
#if defined(APPS_SLEEP_AFTER_NO_CONNECTION) && (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF)
                    /* AT+APPFEATURE=SET,NOCONN_SLEEP,<param3> */
                    if (strncmp(param2, APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE, strlen(APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE)) == 0) {
                        int32_t value = atoi(param3);
                        s_apps_features.no_connection_sleep_mode_disable_bt = value ? true : false;
                        APPS_LOG_MSGID_I("set"APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE"to %d", 1, s_apps_features.no_connection_sleep_mode_disable_bt);
                        nvdm_write_data_item(NVDM_GROUP_APP_DYNAMIC_FEATURE, NVDM_APP_DYNAMIC_FEATURE_ITEM_AUTO_RHO,
                                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                             (uint8_t *)&s_apps_features,
                                             sizeof(s_apps_features));
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
#endif
                }
                /* AT+APPFEATURE=GET,<param2> */
            } else if (strcmp(param1, "GET") == 0 && param2) {
#if defined(APPS_AUTO_TRIGGER_RHO) && defined(APPS_TRIGGER_RHO_BY_KEY)
                /* AT+APPFEATURE=GET,AUTORHO */
                if (strncmp(param2, APP_DYNAMIC_FEATURE_NAME_AUTO_RHO, strlen(APP_DYNAMIC_FEATURE_NAME_AUTO_RHO)) == 0) {
                    APPS_LOG_MSGID_I("get "APP_DYNAMIC_FEATURE_NAME_AUTO_RHO" = %d", 1, s_apps_features.support_auto_rho);
                    snprintf((char *)response.response_buf, sizeof(response.response_buf), "+%s:%d.\r\n", APP_DYNAMIC_FEATURE_NAME_AUTO_RHO,
                             s_apps_features.support_auto_rho);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#endif
#if defined(APPS_SLEEP_AFTER_NO_CONNECTION) && (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF)
                /* AT+APPFEATURE=GET,NOCONN_SLEEP */
                if (strncmp(param2, APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE, strlen(APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE)) == 0) {
                    APPS_LOG_MSGID_I("get "APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE" = %d", 1, s_apps_features.no_connection_sleep_mode_disable_bt);
                    snprintf((char *)response.response_buf, sizeof(response.response_buf), "+%s:%d.\r\n", APP_DYNAMIC_FEATURE_NAME_NO_CONNECTION_SLEEP_MODE,
                             s_apps_features.no_connection_sleep_mode_disable_bt);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#endif
            } else {
                APPS_LOG_W("param 1 = %s", param1);
            }
            break;
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/* The atci command table. */
static atci_cmd_hdlr_item_t dynamic_settings_atci_cmd[] = {
    {
        .command_head = "AT+APPFEATURE",    /**< Test Charger plugin/out */
        .command_hdlr = _set_features_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void apps_config_features_dynamic_setting_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(dynamic_settings_atci_cmd, sizeof(dynamic_settings_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I("atci_register_handler ret = %d", 1, ret);
#if defined(APPS_AUTO_TRIGGER_RHO) && defined(APPS_TRIGGER_RHO_BY_KEY)
    {
        uint32_t nvdm_size;
        nvdm_status_t nvdm_ret;
        nvdm_size = sizeof(s_apps_features);
        nvdm_ret = nvdm_read_data_item(NVDM_GROUP_APP_DYNAMIC_FEATURE, NVDM_APP_DYNAMIC_FEATURE_ITEM_AUTO_RHO,
                                       (uint8_t *)&s_apps_features, &nvdm_size);
        if (NVDM_STATUS_OK == nvdm_ret) {
            APPS_LOG_MSGID_I("Successed to read s_apps_features", 0);
        } else if (nvdm_ret == NVDM_STATUS_ITEM_NOT_FOUND) {
            APPS_LOG_MSGID_I("Cannot find data of s_apps_features", 0);
        } else {
            APPS_LOG_MSGID_E("Error to read s_apps_features", 0);
        }
    }
#endif
    {
        uint32_t nvkey_size = sizeof(s_mp_test_mode);
        nvkey_status_t nvkey_ret = nvkey_read_data(NVKEYID_ENABLE_MP_TEST_MODE, &s_mp_test_mode, &nvkey_size);
        if (NVKEY_STATUS_OK == nvkey_ret) {
            APPS_LOG_MSGID_I("Successed to read s_mp_test_mode : %d", 1, s_mp_test_mode);
        } else if (NVKEY_STATUS_ITEM_NOT_FOUND == nvkey_ret) {
            APPS_LOG_MSGID_I("Cannot find data of s_apps_features", 0);
        } else {
            APPS_LOG_MSGID_E("Error to read s_mp_test_mode", 0);
        }
    }
}
