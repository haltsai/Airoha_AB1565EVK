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
 * File: app_gsound_multi_va.c
 *
 * Description: This file is the activity to handle the action from multi-VA module.
 */


#ifdef GSOUND_LIBRARY_ENABLE
#include "app_gsound_multi_va.h"
#include "app_gsound_service.h"
#include "gsound_service.h"
#include "gsound_port_interface.h"
#include "multi_va_manager.h"
#include "bt_customer_config.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "bt_app_common.h"
#include "gsound_debug.h"

#define TAG "[GS][APP][VA] "

/* Timer */
#define GSOUND_TIMER_NAME           "gsound_timer"
#define GSOUND_TIMER_ID             0
#define GSOUND_TIMER_INTERVAL_MS    (1000*portTICK_PERIOD_MS)

/**************************************************************************************************
* Prototype
**************************************************************************************************/
/**
 * @brief Get GSound ADV and response data.
 * param[in] adv_info, ADV info to be filled.
 */
static uint32_t app_gsound_multi_va_get_adv_data(multi_ble_adv_info_t *adv_info);

/**
 * @brief Notify whether GSound has been selected to current VA mode or not.
 * param[in] selected, true: GSound has been selected, false: Gsound has been removed by multi-VA module.
 */
static multi_va_switch_off_return_t app_gsound_multi_va_switch(bool selected);

/**
 * @brief Initialize GSound service.
 * param[in] selected, true: Enable GSound, false: Disable GSound.
 */
static void app_gsound_multi_va_init(bool selected);

/**************************************************************************************************
* Variable
**************************************************************************************************/
/**
 * @brief Callback function, callback from multi-VA module.
 */
static const multi_va_manager_callbacks_t gsound_cb = {
    app_gsound_multi_va_init,
    app_gsound_multi_va_switch,
    app_gsound_multi_va_get_adv_data
};

static TimerHandle_t gsound_timer = NULL;

/**************************************************************************************************
* Static function
**************************************************************************************************/
static void app_gsound_multi_va_delay_timeout_cb(TimerHandle_t xTimer)
{
    GSOUND_LOG_I(TAG"Connected Notify! (delay timeout)", 0);
    multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_GSOUND);
}

static void app_gsound_multi_va_start_delay_timer(void)
{
    if (gsound_timer == NULL) {
        gsound_timer = xTimerCreate(GSOUND_TIMER_NAME, GSOUND_TIMER_INTERVAL_MS, pdFALSE, GSOUND_TIMER_ID, app_gsound_multi_va_delay_timeout_cb);
    }

    if (gsound_timer) {

        GSOUND_LOG_I(TAG"Connected Notify - delay timer start!", 0);
        if (pdPASS == xTimerStart(gsound_timer, 0)) {
            return;
        }
    }

    GSOUND_LOG_I(TAG"Connected Notify! (delay timer start fail)", 0);
    multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_GSOUND);
}

static uint32_t app_gsound_multi_va_get_adv_data(multi_ble_adv_info_t *adv_info)
{
    char le_device_name[34] = {0};
    uint8_t *adv_data = NULL;
    uint8_t len = 0;
    bt_status_t ret;
    uint8_t default_unique_id[16] = {0};

    if (adv_info->adv_data) {
        /* [ADV DATA] AD_TYPE_FLAG */
        adv_info->adv_data->data_length = 31;
        adv_data = adv_info->adv_data->data;
        adv_data[0] = 2;
        adv_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
        adv_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;

        // Gen unique_id
        bt_bd_addr_t *edr_addr = bt_device_manager_get_local_address();
        ret = bt_app_common_generate_unique_id((const uint8_t *)edr_addr,
                                               BT_BD_ADDR_LEN,
                                               default_unique_id);
        if (BT_STATUS_SUCCESS != ret) {
            GSOUND_LOG_I(TAG"Gen unique_id fail = %d", 1, ret);
            adv_info->adv_data->data_length = 3;
        } else {
            adv_data[3] = BT_APP_COMMON_UNIQUE_ID_MAX_LEN + 1;
            adv_data[4] = BT_GAP_LE_AD_TYPE_MANUFACTURER_SPECIFIC;
            memcpy(&(adv_data[5]), default_unique_id, BT_APP_COMMON_UNIQUE_ID_MAX_LEN);
            adv_info->adv_data->data_length = 3 + 2 + BT_APP_COMMON_UNIQUE_ID_MAX_LEN;
        }
    }

    if (adv_info->scan_rsp) {
        /* [SCAN DATA] AD_TYPE_NAME_COMPLETE */
        bt_customer_config_get_ble_device_name(le_device_name);
        len = strlen(le_device_name);
        if (len > BT_HCI_LE_ADVERTISING_DATA_LENGTH_MAXIMUM - 2) {
            len = BT_HCI_LE_ADVERTISING_DATA_LENGTH_MAXIMUM - 2;
        }
        adv_info->scan_rsp->data_length = 2 + len;
        adv_data = adv_info->scan_rsp->data;
        adv_data[0] = len + 1;
        adv_data[1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
        memcpy(&adv_data[2], le_device_name, len);
    }

    return MULTI_BLE_ADV_NEED_GEN_ADV_PARAM;
}

multi_va_switch_off_return_t app_gsound_multi_va_switch(bool selected)
{
    GSOUND_LOG_I(TAG"Switch! (select=0x%X)", 1, selected);

    if (selected) {
#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()) {
            app_gsound_enable();
        }
#endif

        return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
    }

    /* Disable GSound and wait for app_gsound_multi_va_notify_disconnected(gsound_disable = true) */
    app_gsound_disable();

    return MULTI_VA_SWITCH_OFF_WAIT_INACTIVE;
}

void app_gsound_multi_va_init(bool selected)
{
    app_gsound_init(selected);
}

bool gsound_ble_common_handle_check(bt_bd_addr_t* addr)
{
    bt_bd_addr_t random_addr;
    bt_gap_le_advertising_handle_t handle;
    multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_DEFAULT,
                                                    &random_addr,
                                                    &handle);
    if (memcmp(&random_addr, addr, sizeof(bt_bd_addr_t)) == 0) {
        return true;
    } else {
        return false;
    }
}

/**************************************************************************************************
* Public function
**************************************************************************************************/
void app_gsound_multi_va_notify_connected(void)
{
    multi_va_type_t va_type;

    va_type = multi_va_manager_get_current_va_type();

    if ((va_type != MULTI_VA_TYPE_UNKNOWN) && (va_type != MULTI_VA_TYPE_GSOUND)) {
        /* VA switching. Start delay timer for waiting phone config complete */
        app_gsound_multi_va_start_delay_timer();
        return;
    }

    GSOUND_LOG_I(TAG"Connected Notify!", 0);
    multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_GSOUND);
}

void app_gsound_multi_va_notify_disconnected(bool gsound_disable)
{
    GSOUND_LOG_I(TAG"Disconnected Notify! gsound_disable=0x%X", 1, gsound_disable);

    if (gsound_disable) {
        multi_voice_assistant_manager_set_inactive_done(MULTI_VA_TYPE_GSOUND);
    }

    multi_voice_assistant_manager_notify_va_disconnected(MULTI_VA_TYPE_GSOUND);
}

void app_gsound_multi_va_register(void)
{
    GSOUND_LOG_I(TAG"Register!", 0);
    multi_voice_assistant_manager_register_instance(MULTI_VA_TYPE_GSOUND, &gsound_cb);
}

#endif

