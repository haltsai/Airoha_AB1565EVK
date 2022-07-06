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
 * File: ble_ancs_app.c
 *
 * Description: The file is ANCS app demo code.
 *
 * Note: See doc/AB1565_AB1568_Headset_Reference_Design_User_Guide.pdf for more detail.
 *
 */
#include "ble_ancs_app.h"
#include <string.h>
#include "bt_debug.h"
#include "bt_ancs_common.h"
#include "syslog.h"
#include "atci.h"
ble_ancs_app_cntx_t ancs_app_cntx;                   /* ANCS app context. */
ble_ancs_app_cntx_t *p_ancs_app = &ancs_app_cntx;    /* ANCS proxy flow ON flag. */

log_create_module(ANCS_APP, PRINT_LEVEL_INFO);

/**
 * @brief         Deal with the newly received notificaitons.
 * @param[in] notif    Pointer to notificaiton source.
 * @return    none.
 */
void ble_ancs_deal_with_new_notification(ble_ancs_event_notification_t *notif)
{
    /* Local Variables. */
    atci_response_t *rsp = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    /* Code Body */
    if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
        /* Remove EventID value. */
        p_ancs_app->notif_counter--;
    } else if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED) {
        /* Add EventID value. */
        p_ancs_app->notif_counter++;
    }
    if (rsp == NULL) {
        LOG_MSGID_I(ANCS_APP, "show notification, alloc memory fail\r\n", 0);
        return;
    }
    snprintf((char *)rsp->response_buf, sizeof(rsp->response_buf), "New notification, type = %d, counter = %d\n", notif->event_id, (int)p_ancs_app->notif_counter);
    rsp->response_len = strlen((char *)rsp->response_buf);
    atci_send_response(rsp);
    vPortFree(rsp);
}

/**
 * @brief         Send events from ancs service to ancs task.
 * @param[in] ancs_evt    Pointer to event.
 * @return    none.
 */
void ble_ancs_app_event_callback(ble_ancs_event_t *ancs_evt)
{
    switch (ancs_evt->evt_type) {
        case BLE_ANCS_EVENT_CONNECTED:
            /* BLE connected event. */
            if (ancs_evt->result == BT_STATUS_SUCCESS) {
                p_ancs_app->connection_handle = ancs_evt->connection_handle;
                ble_ancs_enable_data_source(ancs_evt->connection_handle);
                p_ancs_app->status = ANCS_APP_STATUS_ENABLE_DATA_SOURCE;
            } else {
                LOG_MSGID_I(ANCS_APP, "Discover service failed!\r\n", 0);
            }
            break;
        case BLE_ANCS_EVENT_REQUEST_COMPLETED: {
            /* BT_GATTC_WRITE_CHARC response event. */
            if (ancs_evt->result == BT_STATUS_SUCCESS) {
                if (p_ancs_app->status == ANCS_APP_STATUS_ENABLE_DATA_SOURCE) {
                    /* if notification source is not enabled, enable it. */
                    ble_ancs_enable_notification_source(ancs_evt->connection_handle);
                    p_ancs_app->status = ANCS_APP_STATUS_ENABLE_NOTIF_SOURCE;
                } else {
                    p_ancs_app->status = ANCS_APP_STATUS_NONE;
                }
            } else {
                LOG_MSGID_I(ANCS_APP, "Enable data source/notification source failed! result = %x\r\n", 1, ancs_evt->result);
                p_ancs_app->status = ANCS_APP_STATUS_NONE;
            }
        }
        break;
        case BLE_ANCS_EVENT_IOS_NOTIFICATION: {
            /* If you need ,get notification attribute. */
            ble_ancs_deal_with_new_notification(&ancs_evt->data.notification);
        }
        break;
        case BLE_ANCS_EVENT_DISCONNECTED: {
            /* BLE disconnected event. */
            p_ancs_app->notif_counter = 0;
            LOG_MSGID_I(ANCS_APP, "ANCS_APP disconnected!\r\n", 0);
        }
        break;
        default:
            break;
    }
}

/**
 * @brief         Initialize the context p_ancs_app and ancs service.
 * @param[in] none
 * @return    none
 */
void ble_ancs_app_init(void)
{
    LOG_MSGID_I(ANCS_APP, "[ANCSapp]ble_ancs_app_init\r\n", 0);
    memset(p_ancs_app, 0, sizeof(ble_ancs_app_cntx_t));
    ble_ancs_start(ble_ancs_app_event_callback);
}
