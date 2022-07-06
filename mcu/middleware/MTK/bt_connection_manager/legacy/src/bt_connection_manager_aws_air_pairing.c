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


#include "bt_sink_srv.h"
#include "bt_gap_le.h"
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager.h"
#include "bt_os_layer_api.h"
#include "hal_trng.h"
#include "bt_sink_srv_ami.h"

static bool g_bt_connection_manager_start_air_pairing = false;
static bool g_bt_connection_manager_inquiry_stop = true;
static bt_sink_srv_start_air_pairing_t g_air_pairing_cnt = {0};
static bt_key_t random_aws_key = {0};

static void bt_connection_manager_air_pairing_timeout_callback(void *params)
{
    uint32_t data = (uint32_t)params;
    bt_connection_manager_air_pairing_complete_ind_t air_pairing_ind = {0};
    g_bt_connection_manager_start_air_pairing = false;
    if (0 == data) {
        air_pairing_ind.result = false;
    } else {
        air_pairing_ind.result = true;
        bt_device_manager_aws_local_info_update();
    }
    air_pairing_ind.cur_aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_AIR_PAIRING_COMPLETE, &air_pairing_ind, sizeof(air_pairing_ind));
    bt_connection_manager_power_reset(false);
}

static void bt_connection_manager_aws_air_pairing_success(bt_bd_addr_t *remote_addr, uint8_t *remote_key)
{
    bt_aws_mce_role_t aws_role = bt_connection_manager_air_pairing_get_aws_role(remote_addr);
    bt_connection_manager_device_local_info_store_peer_aws_address(remote_addr);
    bt_connection_manager_device_local_info_store_aws_role(aws_role);

    if (BT_AWS_MCE_ROLE_PARTNER == aws_role) {
        bt_key_t aws_key = {0};
        memcpy(aws_key, remote_key, sizeof(bt_key_t));
        bt_connection_manager_device_local_info_store_aws_key(&aws_key);
    } else if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        bt_connection_manager_device_local_info_store_aws_key(&random_aws_key);
    }
    bt_connection_manager_timer_start(BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_ID,
        BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_DUR, bt_connection_manager_air_pairing_timeout_callback, (void *)1);
}

static void bt_connection_manager_generate_aws_secret_key(bt_key_t *aws_key)
{
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    int8_t i;
    uint32_t random_seed;

    bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key", 0);
    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key--error 1", 0);
    }
    for (i = 0; i < 30; ++i) {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key--error 2", 0);
        }
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key--trn: 0x%x", 1, random_seed);
    }

    for (i = 0; i < sizeof(bt_key_t); i += 4) {
        /* randomly generate aws secret key */
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key--error 3: i = %d", 1, i);
        }
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate_random_aws_secret_key--trn: 0x%x", 1, random_seed);
        (*aws_key)[i + 0] = random_seed & 0xFF;
        (*aws_key)[i + 1] = (random_seed >> 8) & 0xFF;
        (*aws_key)[i + 2] = (random_seed >> 16) & 0xFF;
        (*aws_key)[i + 3] = (random_seed >> 24) & 0xFF;
    }
    hal_trng_deinit();
}

bt_status_t bt_connection_manager_aws_air_pairing_power_on_check()
{
    if (true == g_bt_connection_manager_start_air_pairing) {
        bt_hci_iac_lap_t iac = 0x9E8B11;
	    bt_gap_write_inquiry_access_code(&iac, 1);
        bt_connection_manager_timer_start(BT_SINK_SRV_CM_TERMINATE_AIR_PAIRING_TIMER_ID,
            g_air_pairing_cnt.duration * 1000, bt_connection_manager_air_pairing_timeout_callback, (void *)0);
        return BT_STATUS_SUCCESS;
    }
    return BT_CONNECTION_MANAGER_STATUS_FAIL;
}

bt_status_t bt_connection_manager_aws_air_pairing_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    switch (msg) {
    case BT_GAP_SET_SCAN_MODE_CNF: {
        if (true == g_bt_connection_manager_start_air_pairing) {
            if (status == BT_STATUS_SUCCESS) {
                uint8_t inquiry_duration = ((g_air_pairing_cnt.duration - 4) * 100) / 128;
                bt_gap_inquiry_ext(inquiry_duration, 0, 0x9E8B11);
                g_bt_connection_manager_inquiry_stop = false;
            } else {
                g_bt_connection_manager_start_air_pairing = false;
                bt_connection_manager_power_reset(false);
            }
        } else {
            result = BT_CONNECTION_MANAGER_STATUS_FAIL;
        }
    }
        break;
    case BT_GAP_INQUIRY_IND: {
        if (BT_STATUS_SUCCESS == status && true == g_bt_connection_manager_start_air_pairing) {
            bt_gap_inquiry_ind_t *inq_ret = (bt_gap_inquiry_ind_t *)buffer;
            if (NULL != inq_ret->rssi && (*(int8_t *)(inq_ret->rssi) > g_air_pairing_cnt.rssi_threshold) &&
                false == g_bt_connection_manager_inquiry_stop) {
                    bt_os_layer_aes_buffer_t decrypted_data;
                    bt_os_layer_aes_buffer_t plain_text;
                    bt_os_layer_aes_buffer_t aes_key;
                    uint8_t air_pairing_length = sizeof(g_air_pairing_cnt.air_pairing_info) + sizeof(bt_key_t);
                    uint8_t air_pairing_rsp[air_pairing_length];

                    decrypted_data.buffer = (uint8_t *)&air_pairing_rsp;
                    decrypted_data.length = air_pairing_length;
                    plain_text.buffer = (uint8_t *)&(inq_ret->eir[4]);
                    plain_text.length = air_pairing_length;
                    aes_key.buffer = g_air_pairing_cnt.air_pairing_key;
                    aes_key.length = sizeof(bt_key_t);
                    bt_os_layer_aes_decrypt(&decrypted_data, &plain_text, &aes_key);
                    if (0 == memcmp((uint8_t *)decrypted_data.buffer, (uint8_t *)g_air_pairing_cnt.air_pairing_info, sizeof(g_air_pairing_cnt.air_pairing_info))) {
                        bt_gap_cancel_inquiry();
                        bt_connection_manager_aws_air_pairing_success((bt_bd_addr_t *)inq_ret->address, (uint8_t *)decrypted_data.buffer + sizeof(g_air_pairing_cnt.air_pairing_info));
                        g_bt_connection_manager_inquiry_stop = true;
                    }
            }
        }
    }
        break;
    case BT_GAP_SET_EIR_CNF: {
        if (true == g_bt_connection_manager_start_air_pairing) {
            if (BT_STATUS_SUCCESS == status) {
                hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
                /* 0.625 ms * 36(0x24) = 22.5 ms */
                uint16_t interval = 0x24;
                /* 0.625 ms * 18(0x12) = 11.25 ms */
                uint16_t window = 0x12;
                uint32_t random_seed;
                audio_channel_t audio_ch = g_air_pairing_cnt.audio_ch;
                ret = hal_trng_init();
                if (HAL_TRNG_STATUS_OK != ret) {
                    bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate random inquiry init fail", 0);
                } else {
                    ret = hal_trng_get_generated_random_number(&random_seed);
                    if (HAL_TRNG_STATUS_OK != ret) {
                        bt_cmgr_report_id("[BT_CM][AIR_PAIRING]generate random inquiry window error", 0);
                    } else {
                        if (AUDIO_CHANNEL_L == audio_ch) {
                            /* interval max: 0.625 ms * 80(0x50) = 50 ms */
                            /* interval min: 0.625 ms * 36(0x24) = 22.5 ms */
                            interval = (random_seed & (0x50 - 0x24)) + 0x24;
                            bt_cmgr_report_id("[BT_CM][AIR_PAIRING] left interval %d", 1, interval);
                        } else {
                            /* interval max: 0.625 ms * 160(0xA0) = 100 ms */
                            /* interval min: 0.625 ms * 96(0x60) = 60 ms */
                            interval = 0x60 + (random_seed & (0x40 - 0x24)) + 0x24;
                            bt_cmgr_report_id("[BT_CM][AIR_PAIRING] right interval %d", 1, interval);
                        }
                    }
                }
                hal_trng_deinit();
                bt_cmgr_report_id("[BT_CM][AIR_PAIRING] generate random inquiry scan interval: %d, window: %d", 2, interval, window);
                bt_gap_write_inquiry_scan_activity(interval, window);
                bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY);
            } else {
                g_bt_connection_manager_start_air_pairing = false;
                bt_connection_manager_power_reset(false);
            }
        } else {
            result = BT_CONNECTION_MANAGER_STATUS_FAIL;
        }
    }
        break;
    case BT_GAP_WRITE_INQUIRY_ACCESS_CODE_CNF: {
        if (true == g_bt_connection_manager_start_air_pairing) {
            if (BT_STATUS_SUCCESS == status) {
                bt_os_layer_aes_buffer_t encryped_key;
                bt_os_layer_aes_buffer_t plain_text;
                bt_os_layer_aes_buffer_t aes_key;
                uint8_t air_info_length = sizeof(g_air_pairing_cnt.air_pairing_info) + sizeof(bt_key_t);
                uint8_t air_info[air_info_length];
                uint8_t eir_data[air_info_length + 4];
                memset(air_info, 0, sizeof(air_info));
                memset(eir_data, 0, sizeof(eir_data));

                memcpy(&air_info, &g_air_pairing_cnt.air_pairing_info[0], sizeof(g_air_pairing_cnt.air_pairing_info));
                /* Random Generate aws key */
                bt_connection_manager_generate_aws_secret_key(&random_aws_key);
                bt_hci_enable_t filter_enable = BT_HCI_DISABLE;
                extern bt_status_t bt_hci_send_vendor_cmd(uint16_t cmd_code, uint8_t *param, uint8_t param_length);
                encryped_key.buffer = eir_data + 4;
                encryped_key.length = sizeof(g_air_pairing_cnt.air_pairing_info) + sizeof(bt_key_t);
                plain_text.buffer = air_info;
                plain_text.length = sizeof(g_air_pairing_cnt.air_pairing_info) + sizeof(bt_key_t);
                memcpy((uint8_t *)(plain_text.buffer + sizeof(g_air_pairing_cnt.air_pairing_info)), (uint8_t *)random_aws_key, sizeof(bt_key_t));
                aes_key.buffer = g_air_pairing_cnt.air_pairing_key;
                aes_key.length = sizeof(bt_key_t);
                bt_os_layer_aes_encrypt(&encryped_key, &plain_text, &aes_key);
                eir_data[0] = (sizeof(eir_data) - 1);
                eir_data[1] = 0xFF;
                eir_data[2] = 0x94;
                eir_data[3] = 0x00;
                bt_hci_send_vendor_cmd(0xFDC9, &filter_enable, sizeof(filter_enable));
                bt_gap_set_extended_inquiry_response(eir_data, sizeof(eir_data));
            } else {
                g_bt_connection_manager_start_air_pairing = false;
                bt_connection_manager_power_reset(false);
            }
        }
    }
        break;
    default:
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Unexcepted msg:%x", 1, msg);
        break;
    }
    return result;
}

bt_status_t bt_connection_manager_aws_air_pairing_start(void *param)
{
    bt_sink_srv_start_air_pairing_t *air_cnt = (bt_sink_srv_start_air_pairing_t *)param;
    if (NULL == param) {
        return BT_CONNECTION_MANAGER_STATUS_FAIL;
    }

    g_bt_connection_manager_start_air_pairing = true;
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Start duration %d, default role 0x%02X, rssi %d", 3,
        air_cnt->duration, air_cnt->default_role, air_cnt->rssi_threshold);
    bt_connection_manager_memcpy(&g_air_pairing_cnt, air_cnt, sizeof(*air_cnt));
    bt_connection_manager_power_reset(false);

    return BT_STATUS_SUCCESS;
}

bt_status_t bt_connection_manager_aws_air_pairing_cancel(void)
{
    bt_connection_manager_air_pairing_complete_ind_t air_pairing_ind = {0};
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Cancel air pairing cur state %d", 1, g_bt_connection_manager_start_air_pairing);
    air_pairing_ind.result = false;
    air_pairing_ind.cur_aws_role = bt_connection_manager_device_local_info_get_aws_role();
    bt_connection_manager_event_callback(BT_CONNECTION_MANAGER_EVENT_AIR_PAIRING_COMPLETE, &air_pairing_ind, sizeof(air_pairing_ind));
    if (true == g_bt_connection_manager_start_air_pairing) {
        g_bt_connection_manager_start_air_pairing = false;
        bt_connection_manager_timer_stop(BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_ID);
        bt_connection_manager_power_reset(false);
    }

    return BT_STATUS_SUCCESS;
}

bool bt_connection_manager_aws_pair_pairing_is_ongoing()
{
    return g_bt_connection_manager_start_air_pairing;
}



