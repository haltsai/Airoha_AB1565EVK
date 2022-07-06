

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
 * File: app_va_xiaowei_multi_support.c
 *
 * Description: This file is used for multi voice assistant support.
 *
 */


#include "app_va_xiaowei_multi_support.h"
#include "multi_va_manager.h"
#include "multi_ble_adv_manager.h"
#include "app_va_xiaowei_activity.h"
#include "bt_customer_config.h"
#include "xiaowei.h"
#include "apps_debug.h"
#include "stdbool.h"
#include "bt_app_common.h"
#include "va_xiaowei_customer_config.h"

#ifdef MTK_VA_XIAOWEI_ENABLE

#define APP_VA_XIAOWEI_MULTI_PREFIX     "[VA_XIAOWEI] XIAOWEI_MULTI"

uint32_t va_xiaowei_gen_adv_data_info(multi_ble_adv_info_t *adv_info)
{
    if (adv_info == NULL) {
        return MULTI_BLE_ADV_NEED_GEN_ADV_PARAM | MULTI_BLE_ADV_NEED_GEN_ADV_DATA | MULTI_BLE_ADV_NEED_GEN_SCAN_RSP;
    }

    bt_bd_addr_t *local_addr;
#ifdef MTK_AWS_MCE_ENABLE
    /* To fix the BT name change after RHO. */
    if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()) {
        local_addr = bt_device_manager_aws_local_info_get_peer_address();
    } else
#endif
    {
        local_addr = bt_device_manager_get_local_address();
    }

    xiaowei_get_adv_data(VA_XIAOWEI_CUSTOMER_CONFIG_COMPANY_ID,
                         VA_XIAOWEI_CUSTOMER_CONFIG_PRODUCT_ID,
                         *local_addr,
                         adv_info->adv_data->advertising_data,
                         &adv_info->adv_data->advertising_data_length);

    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_MULTI_PREFIX", Advertisement data length : %d", 1, adv_info->adv_data->advertising_data_length);
    APPS_LOG_DUMP_I("xiaowei adv dump 2", adv_info->adv_data->advertising_data, adv_info->adv_data->advertising_data_length);

    char ble_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = { 0 };

    bt_customer_config_get_ble_device_name(ble_name);
    uint16_t len = 0;
    len = strlen(ble_name);

    if (len > 31 - 12) {
        len = 31 - 12;
    }
    adv_info->scan_rsp->scan_response_data[0] = len + 1;
    adv_info->scan_rsp->scan_response_data[1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    memcpy(&adv_info->scan_rsp->scan_response_data[2], ble_name, len);

    adv_info->scan_rsp->scan_response_data_length += (len + 2);

    /* Gen unique_id. */
    uint8_t default_unique_id[16] = {0};
    bt_bd_addr_t *edr_addr = bt_device_manager_get_local_address();
    bt_status_t ret = bt_app_common_generate_unique_id((const uint8_t *)edr_addr,
                                                       BT_BD_ADDR_LEN,
                                                       default_unique_id);
    if (BT_STATUS_SUCCESS != ret) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_MULTI_PREFIX" Xiaowei gen unique_id fail: %d", 1, ret);
    } else {
        adv_info->scan_rsp->scan_response_data[len + 2] = BT_APP_COMMON_UNIQUE_ID_MAX_LEN + 1;
        adv_info->scan_rsp->scan_response_data[len + 3] = BT_GAP_LE_AD_TYPE_MANUFACTURER_SPECIFIC;
        memcpy(&adv_info->scan_rsp->scan_response_data[len + 4], default_unique_id, BT_APP_COMMON_UNIQUE_ID_MAX_LEN);

        adv_info->scan_rsp->scan_response_data_length += (BT_APP_COMMON_UNIQUE_ID_MAX_LEN + 2);
    }

    return MULTI_BLE_ADV_NEED_GEN_ADV_PARAM;
}

multi_va_switch_off_return_t va_xiaowei_assistant_switched(bool selected)
{
    if (selected == false) {
        xiaowei_set_other_va_enabled(true);
    }
    return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
}

void va_xiaowei_null_init(bool selected)
{

}

void app_va_xiaowei_multi_support_register(void)
{
    multi_va_manager_callbacks_t callback;

    callback.on_get_ble_adv_data = va_xiaowei_gen_adv_data_info;
    callback.on_voice_assistant_type_switch = va_xiaowei_assistant_switched;
    callback.voice_assistant_initialize = va_xiaowei_null_init;

    multi_voice_assistant_manager_register_instance(MULTI_VA_TYPE_XIAOWEI, &callback);
}


#endif

