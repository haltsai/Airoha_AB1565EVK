/* Copyright Statement:
 *
 * (C) 2019 Airoha Technology Corp. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifdef AIR_LE_AUDIO_ENABLE
#include "app_ccp.h"

#include "ble_ccp_discovery.h"
#include "bt_gattc_discovery.h"

#include "FreeRTOS.h"

#include "bt_le_audio_msglog.h"
/**************************************************************************************************
* Define
**************************************************************************************************/
typedef struct {
    bt_gattc_discovery_characteristic_t charc[BLE_TBS_MAX_CHARC_NUMBER];
    bt_gattc_discovery_descriptor_t descrp[BLE_TBS_MAX_CHARC_NUMBER];
} app_ccp_discovery_charc_t;

/**************************************************************************************************
* Variable
**************************************************************************************************/
static bt_gattc_discovery_service_t g_ccp_service;
static app_ccp_discovery_charc_t g_ccp_charc;
static app_ccp_callback_t g_ccp_callback = NULL;
/**************************************************************************************************
* Prototype
**************************************************************************************************/
extern bool bt_le_audio_sink_is_link_valid(bt_handle_t handle);

/**************************************************************************************************
* Static function
**************************************************************************************************/
static void app_ccp_set_attribute_callback(void)
{
    LE_AUDIO_MSGLOG_I("[APP][CCP] set attribute done", 0);
    bt_gattc_discovery_event_response();
}

static bool app_ccp_discovery_callback(bt_gattc_discovery_event_t *event, bool is_gtbs)
{
    bt_gattc_discovery_characteristic_t *p_found = NULL;
    ble_ccp_characteristic_t *p_charc_list = NULL;
    ble_ccp_characteristic_t *p_charc = NULL;
    ble_ccp_set_service_attribute_parameter_t param;
    bt_status_t status;
    uint8_t i = 0;

    if (NULL == event) {
        return false;
    }

    if ((false == bt_le_audio_sink_is_link_valid(event->conn_handle)) ||
        ((BT_GATTC_DISCOVERY_EVENT_FAIL == event->event_type) && (!event->last_instance))) {
        return false;
    }

    LE_AUDIO_MSGLOG_I("[APP][CCP] app_ccp_discovery_callback, is_gtbs:%x charc_num:%d", 2, is_gtbs, g_ccp_service.char_count_found);

    memset(&param, 0, sizeof(ble_ccp_set_service_attribute_parameter_t));

    if (BT_GATTC_DISCOVERY_EVENT_COMPLETE == event->event_type) {
	    uint8_t charc_num = 0;
        charc_num = g_ccp_service.char_count_found;

        if (0 != charc_num) {

            /* 1. Malloc TBS charc list */
            if (NULL != (p_charc_list = (ble_ccp_characteristic_t*)pvPortMalloc(sizeof(ble_ccp_characteristic_t)*charc_num))) {

                /* 2. Fill TBS charc list */
                for (i = 0; i < charc_num; i++) {

                    p_found = &g_ccp_service.charateristics[i];
                    p_charc = &p_charc_list[i];

                    p_charc->uuid = p_found->char_uuid.uuid.uuid16;
                    p_charc->value_handle = p_found->value_handle;
                    p_charc->desc_handle = 0;

                    if (p_found->descr_count_found) {
                        p_charc->desc_handle = p_found->descriptor[0].handle;
                    }
                }

                param.charc = p_charc_list;
            }
        }

        param.start_handle = g_ccp_service.start_handle;
        param.end_handle = g_ccp_service.end_handle;
        param.charc_num = charc_num;

    }

    param.is_gtbs = is_gtbs;
    param.is_complete = event->last_instance;
    param.callback = app_ccp_set_attribute_callback;

    status = ble_ccp_set_service_attribute(event->conn_handle, &param);
    LE_AUDIO_MSGLOG_I("[APP][CCP] app_ccp_discovery_callback, status:%x ", 1, status);

    if (p_charc_list) {
        vPortFree(p_charc_list);
    }

    if (is_gtbs) {
        g_ccp_callback(event->conn_handle, (0 != g_ccp_service.char_count_found) ? true : false);
    }

    if (BT_STATUS_SUCCESS != status) {
        return false;
    }

    return true;
}

static void app_ccp_discover_gtbs_callback(bt_gattc_discovery_event_t *event)
{
    if (!app_ccp_discovery_callback(event, true)) {
        bt_gattc_discovery_event_response();
    }
}

static void app_ccp_discover_tbs_callback(bt_gattc_discovery_event_t *event)
{
    if (!app_ccp_discovery_callback(event, false)) {
        bt_gattc_discovery_event_response();
    }
}

/**************************************************************************************************
* Public function
**************************************************************************************************/
bt_status_t app_ccp_init(app_ccp_callback_t callback)
{
    uint8_t i = 0;

    ble_uuid_t service_uuid = {
        .type = BLE_UUID_TYPE_16BIT,
        .uuid.uuid16 = BT_SIG_UUID16_GENERIC_TELEPHONE_BEARER_SERVICE,
    };

    if (NULL != g_ccp_callback) {
        return BT_STATUS_FAIL;
    }

    g_ccp_callback = callback;

    memset(&g_ccp_charc, 0, sizeof(app_ccp_discovery_charc_t));

    for (i = 0; i < BLE_TBS_MAX_CHARC_NUMBER; i++) {
        g_ccp_charc.charc[i].descriptor_count = 1;
        g_ccp_charc.charc[i].descriptor = &g_ccp_charc.descrp[i];
    }

    memset(&g_ccp_service, 0, sizeof(bt_gattc_discovery_service_t));
    g_ccp_service.characteristic_count = BLE_TBS_MAX_CHARC_NUMBER;
    g_ccp_service.charateristics = g_ccp_charc.charc;

    bt_gattc_discovery_service_register(&service_uuid, &g_ccp_service, app_ccp_discover_gtbs_callback);

    service_uuid.uuid.uuid16 = BT_SIG_UUID16_TELEPHONE_BEARER_SERVICE;
    bt_gattc_discovery_service_register(&service_uuid, &g_ccp_service, app_ccp_discover_tbs_callback);

    return BT_STATUS_SUCCESS;
}

#endif
