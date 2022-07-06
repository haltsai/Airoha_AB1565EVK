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
 

#include "bt_aws_mce_le_association.h"
#include "bt_aws_mce_le_association_internal.h"
#include "bt_gap_le.h"
#include "bt_hci.h"
#include "syslog.h"

#define BT_AWS_MCE_LE_ASSOCIATION_SERVICE_MAX_COUNT 2

typedef struct {
    bool is_used;
    bt_handle_t handle;
    bt_aws_mce_le_association_client_pair_ind_t client;
} bt_aws_mce_le_association_client_record_t;

typedef struct {
    uint8_t state;
    bt_aws_mce_le_association_client_record_t record[BT_AWS_MCE_LE_ASSOCIATION_SERVICE_MAX_COUNT];
} bt_aws_mce_le_association_service_context_t;

static uint32_t bt_aws_mce_le_association_agent_addr_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_key_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_client_addr_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_audio_lat_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_voice_lat_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_number_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_custom_data_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

static uint32_t bt_aws_mce_le_association_custom_data_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);



const bt_uuid_t ASSOCIATION_AGENT_ADDR_CHAR_UUID128 = {ASSOCIATION_AGENT_ADDR_CHAR_UUID};
const bt_uuid_t ASSOCIATION_KEY_CHAR_UUID128 = {ASSOCIATION_KEY_CHAR_UUID};
const bt_uuid_t ASSOCIATION_CLIENT_ADDR_CHAR_UUID128 = {ASSOCIATION_CLIENT_ADDR_CHAR_UUID};
const bt_uuid_t ASSOCIATION_AUDIO_LAT_CHAR_UUID128 = {ASSOCIATION_AUDIO_LAT_CHAR_UUID};
const bt_uuid_t ASSOCIATION_VOICE_LAT_CHAR_UUID128 = {ASSOCIATION_VOICE_LAT_CHAR_UUID};
const bt_uuid_t ASSOCIATION_NUMBER_CHAR_UUID128 = {ASSOCIATION_NUMBER_CHAR_UUID};
const bt_uuid_t ASSOCIATION_CUSTOM_READ_CHAR_UUID128 = {ASSOCIATION_CUSTOM_READ_CHAR_UUID};
const bt_uuid_t ASSOCIATION_CUSTOM_WRITE_CHAR_UUID128 = {ASSOCIATION_CUSTOM_WRITE_CHAR_UUID};


/* Primary Service Declaration */
BT_GATTS_NEW_PRIMARY_SERVICE_128(bt_aws_mce_le_association_primary_service, {ASSOCIATION_SERVICE_UUID});

/* Agent addr char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_agent_addr,
    BT_GATT_CHARC_PROP_READ, ASSOCIATION_AGENT_ADDR_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_AGENT_ADDR_CHAR_UUID});

#if 0
#define BT_GATTS_REC_PERM_READABLE_AUTHENTICATION BT_GATTS_REC_PERM_READABLE
#define BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION BT_GATTS_REC_PERM_WRITABLE
#endif
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_agent_addr, 
    ASSOCIATION_AGENT_ADDR_CHAR_UUID128, BT_GATTS_REC_PERM_READABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_agent_addr_read_callback);

/* Key char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_key,
    BT_GATT_CHARC_PROP_READ, ASSOCIATION_KEY_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_KEY_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_key, 
    ASSOCIATION_KEY_CHAR_UUID128, BT_GATTS_REC_PERM_READABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_key_read_callback);

/* Client addr char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_client_addr,
    BT_GATT_CHARC_PROP_WRITE, ASSOCIATION_CLIENT_ADDR_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_CLIENT_ADDR_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_client_addr, 
    ASSOCIATION_CLIENT_ADDR_CHAR_UUID128, BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_client_addr_write_callback);

/* Audio latency char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_audio_lat,
    BT_GATT_CHARC_PROP_WRITE, ASSOCIATION_AUDIO_LAT_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_AUDIO_LAT_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_audio_lat, 
    ASSOCIATION_AUDIO_LAT_CHAR_UUID128, BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_audio_lat_write_callback);

/* Voice latency char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_voice_lat,
    BT_GATT_CHARC_PROP_WRITE, ASSOCIATION_VOICE_LAT_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_VOICE_LAT_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_voice_lat, 
    ASSOCIATION_VOICE_LAT_CHAR_UUID128, BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_voice_lat_write_callback);

/* Number char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_number,
    BT_GATT_CHARC_PROP_READ, ASSOCIATION_NUMBER_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_NUMBER_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_number, 
    ASSOCIATION_NUMBER_CHAR_UUID128, BT_GATTS_REC_PERM_READABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_number_read_callback);

/* Custom read data char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_custom_read_data,
    BT_GATT_CHARC_PROP_READ, ASSOCIATION_CUSTOM_READ_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_CUSTOM_READ_CHAR_UUID});

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_custom_read_data, 
    ASSOCIATION_CUSTOM_READ_CHAR_UUID128, BT_GATTS_REC_PERM_READABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_custom_data_read_callback);

/* Custom write data char Declaration */
BT_GATTS_NEW_CHARC_128(bt_aws_mce_le_association_char4_custom_write_data,
    BT_GATT_CHARC_PROP_READ, ASSOCIATION_CUSTOM_WRITE_CHAR_VALUE_HANDLE, 
    {ASSOCIATION_CUSTOM_WRITE_CHAR_UUID});
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_aws_mce_le_association_custom_write_data, 
    ASSOCIATION_CUSTOM_WRITE_CHAR_UUID128, BT_GATTS_REC_PERM_WRITABLE_AUTHENTICATION, 
    bt_aws_mce_le_association_custom_data_write_callback);


static const bt_gatts_service_rec_t *bt_aws_mce_le_association_rec[] = {
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_primary_service,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_agent_addr,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_agent_addr,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_key,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_key,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_client_addr,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_client_addr,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_audio_lat,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_audio_lat,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_voice_lat,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_voice_lat,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_number,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_number,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_custom_read_data,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_custom_read_data,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_char4_custom_write_data,
    (const bt_gatts_service_rec_t*) &bt_aws_mce_le_association_custom_write_data,
    };


const bt_gatts_service_t bt_aws_mce_le_association_service = {
    .starting_handle = 0x0120,
    .ending_handle = 0x0130,
    .required_encryption_key_size = 7,
    .records = bt_aws_mce_le_association_rec
    };

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_aws_mce_le_association_service_get_assign_number=_default_bt_aws_mce_le_association_service_get_assign_number")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak bt_aws_mce_le_association_service_get_assign_number = default_bt_aws_mce_le_association_service_get_assign_number
#else
#error "Unsupported Platform"
#endif

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_aws_mce_le_association_service_get_assign_number=_default_bt_aws_mce_le_association_service_get_assign_number")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak bt_aws_mce_le_association_service_get_custom_data = default_bt_aws_mce_le_association_service_get_custom_data
#else
#error "Unsupported Platform"
#endif


static bt_aws_mce_le_association_agent_info_t g_bt_aws_mce_le_assocation_agent_info;
static bt_aws_mce_le_association_service_context_t g_bt_aws_mce_le_assocation_service_cntx;

static bt_aws_mce_le_association_client_record_t *bt_aws_mce_le_association_service_get_idle_cntx(void)
{

    uint32_t i;

    for (i = 0; i < BT_AWS_MCE_LE_ASSOCIATION_SERVICE_MAX_COUNT; i++) {
        if (g_bt_aws_mce_le_assocation_service_cntx.record[i].is_used == false) {
            return &g_bt_aws_mce_le_assocation_service_cntx.record[i];
        }
    }

    return NULL;

}

static bt_aws_mce_le_association_client_record_t *bt_aws_mce_le_association_service_get_cntx_by_handle(bt_handle_t handle)
{
    uint32_t i;

    for (i = 0; i < BT_AWS_MCE_LE_ASSOCIATION_SERVICE_MAX_COUNT; i++) {
        if (g_bt_aws_mce_le_assocation_service_cntx.record[i].is_used 
            && (g_bt_aws_mce_le_assocation_service_cntx.record[i].handle == handle)) {
            return &g_bt_aws_mce_le_assocation_service_cntx.record[i];
        }
    }

    return NULL;

}

static void bt_aws_mce_le_association_service_destory_record_by_handle(bt_aws_mce_le_association_client_record_t *record)
{
    if (record) {
        memset(record, 0, sizeof(bt_aws_mce_le_association_client_record_t));
    }

}

static uint32_t bt_aws_mce_le_association_agent_addr_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{

    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_READ) {

            uint32_t len = sizeof(bt_bd_addr_t);

            if (size == 0) {
                return len;
            } else {

                if (size < len) {
                    return 0;
                }

                memcpy(data, &g_bt_aws_mce_le_assocation_agent_info.address, len);
                return len;

            }

        } else {
            /* Wrongly operation */
        }

    } else {
        /*  Wronlgy operation */
    }

    return 0;

}


static uint32_t bt_aws_mce_le_association_key_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{

    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_READ) {

            /* 16 byte is the length of secret key */
            uint32_t len = 16;

            if (size == 0) {
                return len;
            } else {

                if (size < len) {
                    return 0;
                }

                memcpy(data, g_bt_aws_mce_le_assocation_agent_info.secret_key, len);
                return len;

            }

        } else {
            /* Wrongly operation */
        }

    } else {
        /*  Wronlgy operation */
    }

    return 0;

}

static uint32_t bt_aws_mce_le_association_client_addr_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_WRITE) {

            /* 16 byte is the length of secret key */
            uint32_t len = sizeof(bt_bd_addr_t);

            if (size != len) {
                return 0;
            } else {

                bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);

                if (record != NULL) {
                    /* Means previous record not clear. Reuse or disconnect. Considering  */
                } else {

                    record = bt_aws_mce_le_association_service_get_idle_cntx();

                }

                if (record == NULL) {
                    /* Error handler, disconnect or assert */

                } else {
                    uint16_t number;
                    /* get the assign number from application */
                    number = bt_aws_mce_le_association_service_get_assign_number();
                    record->is_used = true;
                    record->client.number = number;
                    record->handle = handle;
                    record->client.handle = handle;
                    memcpy(&record->client.info.address, data, len);
                    return len;
                }

            }
        } else {

        }
        /*  Wronlgy operation */
    }

    return 0;

}

static uint32_t bt_aws_mce_le_association_audio_lat_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_WRITE) {

            /* 16 byte is the length of secret key */
            uint32_t len = sizeof(uint16_t);

            if (size != len) {
                return 0;
            } else {

                bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);

                if (record == NULL) {
                    /* No address write, get the buffer*/
                    record = bt_aws_mce_le_association_service_get_idle_cntx();
                    if (record != NULL) {
                        record->is_used = true;
                        record->client.number = 0;
                        record->handle = handle;
                        record->client.handle = handle;
                    }
                }

                if (record == NULL) {
                    /* Error handler, disconnect or assert */

                } else {

                    memcpy(&record->client.info.audio_latency, data, len);
                    return len;
                }

            }
        } else {

        }
    }

    return 0;

}

static uint32_t bt_aws_mce_le_association_voice_lat_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_WRITE) {

            /* 16 byte is the length of secret key */
            uint32_t len = sizeof(uint16_t);

            if (size != len) {
                return 0;
            } else {

                bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);

                LOG_I(common, "voice_write(%x)", record);
                if (record == NULL) {
                    /* Flow have problem. */
                    return 0;

                }  else {

                    bt_aws_mce_le_association_client_pair_ind_t client;
                    client.handle = handle;
                    memcpy(&record->client.info.voice_latency, data, len);

                    memcpy(&client, &record->client, sizeof(bt_aws_mce_le_association_client_pair_ind_t));

                    /* Noitfy application pairing completed */

                    bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_CLIENT_PAIR_IND, BT_STATUS_SUCCESS, (void *)&client, sizeof(bt_aws_mce_le_association_client_pair_ind_t));

                    return len;
                }

            }
        } else {

            /*  Wronlgy operation */
        }
    }

    return 0;

}

static uint32_t bt_aws_mce_le_association_number_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{

    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_READ) {

            /* 16 byte is the length of secret key */
            uint32_t len = sizeof(uint16_t);

            if (size == 0) {
                return len;
            } else {

                if (size < len) {
                    return 0;
                }

                bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);
                
                if (record == NULL) {
                    return 0;
                }


                memcpy(data, &record->client.number, len);

                return len;

            }

        } else {
            /* Wrongly operation */
        }

    } else {
        /*  Wronlgy operation */
    }

    return 0;

}

uint16_t default_bt_aws_mce_le_association_service_get_assign_number()
{
    return 0;
}

void bt_aws_mce_le_association_service_info_set(bt_aws_mce_le_association_agent_info_t *agent)
{
    if (agent == NULL) {
        memset(&g_bt_aws_mce_le_assocation_agent_info, 0, sizeof(bt_aws_mce_le_association_agent_info_t));
    } else {

        memcpy(&g_bt_aws_mce_le_assocation_agent_info, agent, sizeof(bt_aws_mce_le_association_agent_info_t));

    }
}

bt_status_t bt_aws_mce_le_association_service_build_advertising_data(void *buffer, uint16_t *buffer_length, bt_firmware_type_t mode)
{
    uint8_t len;
    char adv_data[] = "AWS 1.0";
    uint8_t *buff = (uint8_t *)buffer;

    if (buffer == NULL || buffer_length == NULL) {
        return BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
    }
    len = sizeof(adv_data);
    *buff++ = len + 1;
    *buff++ = 0xFF;
    memcpy(buff, adv_data, len - 1);
    buff = buff + len - 1;

    if (mode  == BT_FIRMWARE_TYPE_SPEAKER || mode == BT_AWS_MCE_SRV_MODE_BROADCAST) {
        mode = 0x01;//BT_FIRMWARE_TYPE_SPEAKER
    }
    *buff = mode;/*add mode type*/
    *buffer_length = sizeof(adv_data) + 2;

    return BT_STATUS_SUCCESS;

}


bt_status_t bt_aws_mce_le_association_service_event_handler(bt_msg_type_t msg, bt_status_t status,void *buffer)
{
    switch (msg) {
        case BT_GAP_LE_DISCONNECT_IND:
        {

            bt_hci_evt_disconnect_complete_t *disconnect_complete = (bt_hci_evt_disconnect_complete_t*) buffer;
            bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(disconnect_complete->connection_handle);

            if (record) {
                bt_aws_mce_le_association_service_destory_record_by_handle(record);
            }

            break;

        }
    }
    return BT_STATUS_SUCCESS;

}

static uint32_t bt_aws_mce_le_association_custom_data_read_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(common, "custom_data_read_callback: handle (%x), data (%x), size (%x)", 3, handle, data, size);

    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_READ) {
            uint8_t len = BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN;

            if (size == 0) {

                return len;

            } else {
            
                if (size < len) {
                    return 0;
                }
                bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);
                
                if (record == NULL || data == NULL) {
                    return 0;
                }

                uint8_t temp_data[BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN];
                bt_aws_mce_le_association_custom_data_t custom_data;
                custom_data.data = temp_data;
                bool status = bt_aws_mce_le_association_service_get_custom_data(&custom_data);
                if (!status)
                {
                    return 0;
                }
                len = custom_data.len;
                LOG_MSGID_I(common, "custom_data_read_callback: len :%x, custom_data[%x,%x,%x,%x]", 5, len, custom_data.data[0], custom_data.data[1], custom_data.data[2], custom_data.data[3]);
                if (len > BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN) {
                    len = BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN;
                }
                memcpy(data, custom_data.data, len);

                return len;

            }

            //LOG_MSGID_I(common, "custom_data_read_callback: data[%x, %x, %x, %x]", 4,custom_data.data[0], custom_data.data[1], custom_data.data[2], custom_data.data[3]);


        } else {
            /* Wrongly operation */
        }

    } else {
        /*  Wronlgy operation */
    }

    return 0;
}

static uint32_t bt_aws_mce_le_association_custom_data_write_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{

    if (handle != BT_HANDLE_INVALID) {

        if (rw == BT_GATTS_CALLBACK_WRITE) {

            bt_aws_mce_le_association_client_record_t *record = bt_aws_mce_le_association_service_get_cntx_by_handle(handle);
            
            if (record == NULL) {
                return 0;
            }

            if (size > BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN) {//it should not be over 4
                size = BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN;
            }
            bt_aws_mce_le_association_write_custom_data_ind_t ind;
            uint8_t temp_data[BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN];
            ind.handle = handle;
            memcpy(temp_data, data, size);
            ind.info.data = temp_data;
            ind.info.len = size;
            LOG_I(common, "custom_data_write_callback: size(%x), data[%x, %x, %x, %x]", size, ind.info.data[0], ind.info.data[1], ind.info.data[2], ind.info.data[3]);

            bt_aws_mce_le_association_event_callback(BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_IND, BT_STATUS_SUCCESS, (void*)&ind, sizeof(bt_aws_mce_le_association_write_custom_data_ind_t));

            return size;

        } else {
            /* Wrongly operation */
        }

    } else {
        /*  Wronlgy operation */
    }

    return 0;
}

bool default_bt_aws_mce_le_association_service_get_custom_data(bt_aws_mce_le_association_custom_data_t* data)
{
    return true;
}

