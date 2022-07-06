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

#ifdef MTK_AMA_ENABLE
#include "multi_va_manager.h"
#include "bt_customer_config.h"
#include "FreeRTOS.h"
#include "BtAma.h"
#include "apps_debug.h"
#include "bt_app_common.h"
#include "app_ama_activity.h"
#include "string.h"
#include "apps_events_event_group.h"

#define LOG_AMA_TAG     "[Multi_VA_AMA] "

uint32_t bt_ama_app_event_callback(Handler handler, uint16_t event_type, void *param, uint32_t len);

static AMA_HandlerData g_app_ama_handler = { bt_ama_app_event_callback };

/**
 * @brief      callback of AMA middleware.
 * @param[in]  handler, The same handler obtained when calling AMA_Target_StartService().
 * @param[in]  event_type, the event type, which is defined in AMA_MSG_E.
 * @param[in]  param, the pointer to the parameter.
 * @param[in]  len, the length of the parameter.
 */
uint32_t bt_ama_app_event_callback(Handler handler, uint16_t event_type, void *param, uint32_t len)
{
    void *event_param = NULL;
    APPS_LOG_MSGID_I(LOG_AMA_TAG"bt_ama_app_event_callback :%x, len = %x, event_type : %x", 3, param, len, event_type);
    if (event_type <= AMA_MSG_MAX && len) {
        event_param = pvPortMalloc(len);
        if (event_param) {
            memcpy(event_param, param, len);
            ui_shell_send_event(false,
                                EVENT_PRIORITY_MIDDLE,
                                EVENT_GROUP_UI_SHELL_BT_AMA,
                                event_type,
                                event_param,
                                len,
                                NULL,
                                0);
        }
    }
    return 0;
}

static void ama_voice_assistant_init(bool selected)
{
    APPS_LOG_MSGID_E(LOG_AMA_TAG"ama_voice_assistant_init, selected :%d", 1, selected);

    AMAStatus status = AMA_Target_StartService(&g_app_ama_handler, 1);//one link
    APPS_LOG_MSGID_I(LOG_AMA_TAG"ama_voice_assistant_init, status: %x", 1, status);

    AMA_Target_SetSelected(selected);

    /**
     * @brief Configure the AMA trigger mode
     *
     */

    uint8_t trigger_mode = 0x00;
    ama_trigger_model_wake_word_t wake_word = {0};

#ifdef AMA_TRIGGER_MODE_TTT_ENABLE
    trigger_mode |= AMA_TRIGGER_MODE_TAP;
#endif
#ifdef AMA_TRIGGER_MODE_PTT_ENABLE
    trigger_mode |= AMA_TRIGGER_MODE_PRESS_AND_HOLD;
#endif
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    trigger_mode |= AMA_TRIGGER_MODE_WAKE_WORD;
    memcpy(wake_word.wake_word, "alexa", strlen("alexa"));
#endif

    APPS_LOG_MSGID_I(LOG_AMA_TAG"ama_voice_assistant_init, Configure AMA trigger mode : 0x%02x", 1, trigger_mode);
    if ((trigger_mode & AMA_TRIGGER_MODE_WAKE_WORD) == AMA_TRIGGER_MODE_WAKE_WORD) {
        AMA_Target_ConfigureTriggerMode(trigger_mode, &wake_word, 1);
    } else {
        AMA_Target_ConfigureTriggerMode(trigger_mode, NULL, 0);
    }
}

static multi_va_switch_off_return_t ama_type_switch(bool selected)
{
    //TO do call api get connected or not
    APPS_LOG_MSGID_E(LOG_AMA_TAG"ama_type_switch, selected : %d", 1, selected);
    if (!selected) { // be switch other
        bool b = app_ama_multi_va_set_configuration(false);
        if (b == false) {
            return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
        } else {
            return MULTI_VA_SWITCH_OFF_WAIT_INACTIVE;
        }
    }
    return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
}

static uint32_t ama_on_get_ble_adv_data(multi_ble_adv_info_t *adv_info)
{
    uint16_t len;
    bt_status_t ret;
    uint8_t default_unique_id[16] = {0};

    uint8_t data_context[31] = {0};
    char ble_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = { 0 };

    if (adv_info->adv_data) {
        adv_info->adv_data->data_length = 0;
        AMA_Target_Build_Adv_Data((void *)data_context, &len);
        // APPS_LOG_DUMP_I("ama_adv_data1", data_context, len);
        memcpy(adv_info->adv_data->data, data_context, len);
        adv_info->adv_data->data_length = len;
        // APPS_LOG_DUMP_I("ama_adv_data2", adv_info->adv_data->data, adv_info->adv_data->data_length);
    }

    if (adv_info->scan_rsp) {
        adv_info->scan_rsp->data_length = 0;
        bt_customer_config_get_ble_device_name(ble_name);
        len = strlen(ble_name);

        if (len > 31 - 12) {
            len = 31 - 12;
        }
        adv_info->scan_rsp->data[0] = len + 1;
        adv_info->scan_rsp->data[1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;

        memcpy(&adv_info->scan_rsp->data[2], ble_name, len);
        adv_info->scan_rsp->data_length += (len + 2);

        // gen unique_id
        bt_bd_addr_t *edr_addr = bt_device_manager_get_local_address();
        ret = bt_app_common_generate_unique_id((const uint8_t *)edr_addr,
                                                BT_BD_ADDR_LEN,
                                                default_unique_id);
        if (BT_STATUS_SUCCESS != ret) {
            APPS_LOG_MSGID_E(LOG_AMA_TAG" AMA gen unique_id fail: %d", 1, ret);
        } else {
            adv_info->scan_rsp->data[len + 2] = BT_APP_COMMON_UNIQUE_ID_MAX_LEN + 1;
            adv_info->scan_rsp->data[len + 3] = BT_GAP_LE_AD_TYPE_MANUFACTURER_SPECIFIC;
            memcpy(adv_info->scan_rsp->data + len + 4, default_unique_id, BT_APP_COMMON_UNIQUE_ID_MAX_LEN);

            adv_info->scan_rsp->data_length += (BT_APP_COMMON_UNIQUE_ID_MAX_LEN + 2);
        }
    }

    return MULTI_BLE_ADV_NEED_GEN_ADV_PARAM;
}

static const multi_va_manager_callbacks_t multi_cb = {
    ama_voice_assistant_init,
    ama_type_switch,
    ama_on_get_ble_adv_data
};

void apps_register_ama_in_multi_voice(void)
{
    multi_voice_assistant_manager_register_instance(MULTI_VA_TYPE_AMA, &multi_cb);
}

void apps_ama_multi_va_set_inactive_done(void)
{
    multi_voice_assistant_manager_set_inactive_done(MULTI_VA_TYPE_AMA);
}

#endif
