/*
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
#ifdef AIR_LE_AUDIO_ENABLE

#include "app_mps.h"
#include "app_le_audio.h"
//#include "app_smcharger_utils.h"

#include "bt_gap_le.h"
#include "bt_gatts.h"
#include "bt_sink_srv_ami.h"

#include "bt_le_audio_msglog.h"

/************************************************
*   Macro and Structure
*************************************************/
/* Attribute handle */
#define MPS_START_HANDLE                (0xA501)
#define MPS_VALUE_HANDLE_BATTERY_LEVEL  (0xA503)
#define MPS_VALUE_HANDLE_CONTROL_POINT  (0xA506)
#define MPS_END_HANDLE                  (0xA507)

/* UUID_16 */
#define MPS_SERVICE_UUID                0x2454
#define MPS_UUID_BATTERY_LEVEL          0x2455
#define MPS_UUID_CONTROL_POINT          0x2456

/* Battery level */
#define MPS_BATTERY_LEVEL_DATA_LEN          3       /* |Left ear battery(1 octet) | Right ear battery (1 octet) | Box battery (1 octet)| */
#define MPS_BATTERY_LEVEL_DATA_OFFSET_L     0
#define MPS_BATTERY_LEVEL_DATA_OFFSET_R     1
#define MPS_BATTERY_LEVEL_DATA_OFFSET_BOX   2
#define MPS_BATTERY_LEVEL_SMART_CHARGER_VALUE_INVALID   0xFF

#define MPS_GATTS_CCCD_SIZE                 2
#define MPS_GATTS_NOTI_HEADER_SIZE          5
#define MPS_GATTS_NOTI_DATA_HEADER_SIZE     3
#define MPS_GATTS_NOTI_ENABLE               1

typedef struct {
    uint16_t battery_level_cccd;
    uint16_t control_point_cccd;
} app_mps_cccd_t;

/************************************************
*   Global
*************************************************/
/* UUID_128 */
static const bt_uuid_t MPS_UUID128_BATTERY_LEVEL = BT_UUID_INIT_WITH_UUID16(MPS_UUID_BATTERY_LEVEL);
static const bt_uuid_t MPS_UUID128_CONTROL_POINT = BT_UUID_INIT_WITH_UUID16(MPS_UUID_CONTROL_POINT);

/* Charc_data */
static uint8_t g_mps_battery_level[MPS_BATTERY_LEVEL_DATA_LEN]= {0};

static uint8_t g_mps_channel_local = MPS_BATTERY_LEVEL_DATA_OFFSET_L;
static uint8_t g_mps_channel_peer = MPS_BATTERY_LEVEL_DATA_OFFSET_R;

static app_mps_cccd_t g_mps_cccd[APP_LE_AUDIO_MAX_LINK_NUM];

/************************************************
* Prototype
*************************************************/
static uint32_t app_mps_callback_battery_level(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t app_mps_client_config_callback_battery_level(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t app_mps_callback_control_point(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t app_mps_client_config_callback_control_point(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

/************************************************
*   SERVICE TABLE
*************************************************/
BT_GATTS_NEW_PRIMARY_SERVICE_16(ble_mps_primary_service, MPS_SERVICE_UUID);

/* Battery Level */
BT_GATTS_NEW_CHARC_16(ble_mps_charc_battery_level,
                        BT_GATT_CHARC_PROP_READ|BT_GATT_CHARC_PROP_NOTIFY,
                        MPS_VALUE_HANDLE_BATTERY_LEVEL,
                        MPS_UUID_BATTERY_LEVEL);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_mps_charc_value_battery_level,
                                    MPS_UUID128_BATTERY_LEVEL,
                                    BT_GATTS_REC_PERM_READABLE_ENCRYPTION,
                                    app_mps_callback_battery_level);
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(ble_mps_client_config_battery_level,
                                    BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE_ENCRYPTION,
                                    app_mps_client_config_callback_battery_level);

/* Control Point */
BT_GATTS_NEW_CHARC_16(ble_mps_charc_control_point,
                        BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP|BT_GATT_CHARC_PROP_WRITE|BT_GATT_CHARC_PROP_NOTIFY,
                        MPS_VALUE_HANDLE_CONTROL_POINT,
                        MPS_UUID_CONTROL_POINT);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_mps_charc_value_control_point,
                                    MPS_UUID128_CONTROL_POINT,
                                    BT_GATTS_REC_PERM_WRITABLE_ENCRYPTION,
                                    app_mps_callback_control_point);
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(ble_mps_client_config_control_point,
                                    BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE_ENCRYPTION,
                                    app_mps_client_config_callback_control_point);

static const bt_gatts_service_rec_t *ble_mps_service_rec[] = {
    (const bt_gatts_service_rec_t*) &ble_mps_primary_service,
    (const bt_gatts_service_rec_t*) &ble_mps_charc_battery_level,
    (const bt_gatts_service_rec_t*) &ble_mps_charc_value_battery_level,
    (const bt_gatts_service_rec_t*) &ble_mps_client_config_battery_level,
    (const bt_gatts_service_rec_t*) &ble_mps_charc_control_point,
    (const bt_gatts_service_rec_t*) &ble_mps_charc_value_control_point,
    (const bt_gatts_service_rec_t*) &ble_mps_client_config_control_point,
};

const bt_gatts_service_t ble_mps_service = {
    .starting_handle = MPS_START_HANDLE,
    .ending_handle = MPS_END_HANDLE,
    .required_encryption_key_size = 0,
    .records = ble_mps_service_rec,
};

/************************************************
* Functions
*************************************************/
static uint32_t app_mps_callback_battery_level(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (BT_HANDLE_INVALID == handle) {
        return 0;
    }

    if (BT_GATTS_CALLBACK_READ != rw) {
        return 0;
    }

    if (NULL != data) {
        memcpy((uint8_t *)data, g_mps_battery_level, MPS_BATTERY_LEVEL_DATA_LEN);
    }

    return MPS_BATTERY_LEVEL_DATA_LEN;
}

static uint32_t app_mps_client_config_callback_battery_level(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint8_t idx;

    if ((BT_HANDLE_INVALID == handle) || (APP_LE_AUDIO_MAX_LINK_NUM == (idx = app_le_audio_get_link_index(handle)))) {
        return 0;
    }

    switch (rw) {
        case BT_GATTS_CALLBACK_WRITE: {
            if ((MPS_GATTS_CCCD_SIZE == size) && (NULL != data)) {
                g_mps_cccd[idx].battery_level_cccd = *(uint16_t *)data;
                LE_AUDIO_MSGLOG_I("[APP][MPS] w battery_level cccd, size:%x cccd:%x idx:%x", 3, size, g_mps_cccd[idx].battery_level_cccd, idx);
                app_mps_send_battery_level_notification(handle);
                return MPS_GATTS_CCCD_SIZE;
            }
            break;
        }

        case BT_GATTS_CALLBACK_READ: {
            if ((MPS_GATTS_CCCD_SIZE == size) && (NULL != data)) {
                uint16_t *buf = (uint16_t *)data;
                *buf = g_mps_cccd[idx].battery_level_cccd;
                LE_AUDIO_MSGLOG_I("[APP][MPS] r battery_level cccd, size:%x cccd:%x idx:%x", 3, size, g_mps_cccd[idx].battery_level_cccd, idx);
            }
            return MPS_GATTS_CCCD_SIZE;
        }

        default:
            break;
    }

    return 0;
}

static uint32_t app_mps_callback_control_point(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (BT_HANDLE_INVALID == handle) {
        return 0;
    }

    if (BT_GATTS_CALLBACK_WRITE != rw) {
        return 0;
    }

    uint8_t *ptr = (uint8_t *)data;
    LE_AUDIO_MSGLOG_I("[APP][MPS] w control_point, size:%x %x %x %x", 4, size, ptr[0], ptr[1], ptr[2]);
    return size;
}

static uint32_t app_mps_client_config_callback_control_point(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint8_t idx;

    if ((BT_HANDLE_INVALID == handle) || (APP_LE_AUDIO_MAX_LINK_NUM == (idx = app_le_audio_get_link_index(handle)))) {
        return 0;
    }

    switch (rw) {
        case BT_GATTS_CALLBACK_WRITE: {
            if ((MPS_GATTS_CCCD_SIZE == size) && (NULL != data)) {
                g_mps_cccd[idx].control_point_cccd= *(uint16_t *)data;
                LE_AUDIO_MSGLOG_I("[APP][MPS] w control_point cccd, size:%x cccd:%x idx:%x", 3, size, g_mps_cccd[idx].control_point_cccd, idx);
                return MPS_GATTS_CCCD_SIZE;
            }
            break;
        }

        case BT_GATTS_CALLBACK_READ: {
            if ((MPS_GATTS_CCCD_SIZE == size) && (NULL != data)) {
                uint16_t *buf = (uint16_t *)data;
                *buf = g_mps_cccd[idx].control_point_cccd;
                LE_AUDIO_MSGLOG_I("[APP][MPS] r control_point cccd, size:%x cccd:%x idx:%x", 3, size, g_mps_cccd[idx].control_point_cccd, idx);
            }
            return MPS_GATTS_CCCD_SIZE;
        }

        default:
            break;
    }

    return 0;

}

uint8_t app_mps_get_battery_level_by_channel(app_mps_channel_t channel)
{
    switch (channel) {
        case APP_MPS_CHANNEL_LOCAL: {
            return g_mps_battery_level[g_mps_channel_local];
        }
        case APP_MPS_CHANNEL_PEER: {
            return g_mps_battery_level[g_mps_channel_peer];
        }
        case APP_MPS_CHANNEL_BOX: {
            return g_mps_battery_level[MPS_BATTERY_LEVEL_DATA_OFFSET_BOX];
        }
        default:
            break;
    }
    return MPS_BATTERY_LEVEL_VALUE_MIN;
}

void app_mps_set_battery_level_by_channel(app_mps_channel_t channel, app_mps_battery_level_t battery_level)
{
    switch (channel) {
        case APP_MPS_CHANNEL_LOCAL: {
            g_mps_battery_level[g_mps_channel_local] = battery_level;
            break;
        }
        case APP_MPS_CHANNEL_PEER: {
            g_mps_battery_level[g_mps_channel_peer] = battery_level;
            break;
        }
        case APP_MPS_CHANNEL_BOX: {
            g_mps_battery_level[MPS_BATTERY_LEVEL_DATA_OFFSET_BOX] = battery_level;
            break;
        }
        default:
            break;
    }
}

void app_mps_set_battery_level(void)
{
    g_mps_battery_level[g_mps_channel_local] = MPS_BATTERY_LEVEL_VALUE_MAX;
    g_mps_battery_level[g_mps_channel_peer] = MPS_BATTERY_LEVEL_VALUE_MIN;
    g_mps_battery_level[MPS_BATTERY_LEVEL_DATA_OFFSET_BOX] = MPS_BATTERY_LEVEL_VALUE_MAX;

    LE_AUDIO_MSGLOG_I("[APP][MPS] set_battery_level, local:%x peer:%x box:%x", 3, g_mps_battery_level[g_mps_channel_local], g_mps_battery_level[g_mps_channel_peer], g_mps_battery_level[MPS_BATTERY_LEVEL_DATA_OFFSET_BOX]);
}

void app_mps_send_battery_level_notification(bt_handle_t handle)
{
    bt_gattc_charc_value_notification_indication_t *noti;
    bt_status_t ret = BT_STATUS_FAIL;
    uint8_t buf[MPS_GATTS_NOTI_HEADER_SIZE+MPS_BATTERY_LEVEL_DATA_LEN] = {0};
    uint8_t idx;

    if (APP_LE_AUDIO_MAX_LINK_NUM == (idx = app_le_audio_get_link_index(handle))) {
        return;
    }

    if (g_mps_cccd[idx].battery_level_cccd & MPS_GATTS_NOTI_ENABLE) {
        noti = (bt_gattc_charc_value_notification_indication_t*) buf;
        noti->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        noti->att_req.handle = MPS_VALUE_HANDLE_BATTERY_LEVEL;
        memcpy(noti->att_req.attribute_value, g_mps_battery_level, MPS_BATTERY_LEVEL_DATA_LEN);
        noti->attribute_value_length = MPS_GATTS_NOTI_DATA_HEADER_SIZE + MPS_BATTERY_LEVEL_DATA_LEN;

        ret = bt_gatts_send_charc_value_notification_indication(handle, noti);
        LE_AUDIO_MSGLOG_I("[APP][MPS] battery_level_noti, handle:%x ret:%x", 2, handle, ret);
    }
}

void app_mps_init(void)
{
    audio_channel_t channel = 0;

    /* Get local audio channel */
    channel = ami_get_audio_channel();
    LE_AUDIO_MSGLOG_I("[APP][MPS] app_mps_init, channel:%x", 1, channel);

    switch (channel) {
        case AUDIO_CHANNEL_L:    /* Local: L */
            g_mps_channel_local = MPS_BATTERY_LEVEL_DATA_OFFSET_L;
            g_mps_channel_peer = MPS_BATTERY_LEVEL_DATA_OFFSET_R;
            break;
        case AUDIO_CHANNEL_R:    /* Local: R */
            g_mps_channel_local = MPS_BATTERY_LEVEL_DATA_OFFSET_R;
            g_mps_channel_peer = MPS_BATTERY_LEVEL_DATA_OFFSET_L;
            break;
        default:
            break;
    }
    LE_AUDIO_MSGLOG_I("[APP][MPS] app_mps_init, g_mps_channel_local:%x g_mps_channel_peer:%x", 2, g_mps_channel_local, g_mps_channel_peer);

    /* Get battery level */
    app_mps_set_battery_level();

    memset(g_mps_cccd, 0, sizeof(app_mps_cccd_t)*APP_LE_AUDIO_MAX_LINK_NUM);
}

#endif  /* AIR_LE_AUDIO_ENABLE */

