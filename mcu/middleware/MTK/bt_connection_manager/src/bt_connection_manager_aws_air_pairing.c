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

#include "bt_system.h"
#include "bt_sink_srv.h"
#include "bt_gap_le.h"
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager.h"
#include "bt_os_layer_api.h"
#include "hal_trng.h"
#include "bt_sink_srv_ami.h"
#include "bt_aws_mce_srv.h"
#include "bt_callback_manager.h"
#include "bt_aws_mce_role_recovery.h"
#include "bt_device_manager_power.h"


#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_aws_mce_srv_event_callback=_default_bt_aws_mce_srv_event_callback")
#pragma comment(linker, "/alternatename:_bt_aws_mce_srv_air_pairing_get_aws_role=_default_bt_aws_mce_srv_air_pairing_get_aws_role")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_aws_mce_srv_event_callback = default_bt_aws_mce_srv_event_callback
#pragma weak bt_aws_mce_srv_air_pairing_get_aws_role = default_bt_aws_mce_srv_air_pairing_get_aws_role
#else
#error "Unsupported Platform"
#endif

#define BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED         (0x01)
#define BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INITIATED       (0x02)
#define BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING      (0x04)
#define BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_COMPLETE        (0x08)
#define BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_FAIL            (0x10)
typedef uint8_t bt_aws_mce_srv_air_pairing_flag_t;

typedef struct {
    bt_aws_mce_srv_air_pairing_flag_t   flags;
    bt_aws_mce_srv_air_pairing_t        cnt;
    bt_key_t                            random_aws_key;
} bt_aws_mce_srv_air_pairing_cnt_t;

static bt_aws_mce_srv_air_pairing_cnt_t g_aws_mce_air_pairing_cnt;
static bt_status_t  bt_aws_mce_srv_air_pairing_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer);

static void         bt_aws_mce_srv_air_pairing_timeout_callback(void *params);

static void         default_bt_aws_mce_srv_event_callback(bt_aws_mce_srv_event_t event_id, void *params, uint32_t params_len)
{
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][E] APP not listen AWS mce event !!!", 0);
}

static bt_aws_mce_role_t   
                    default_bt_aws_mce_srv_air_pairing_get_aws_role(const bt_bd_addr_t *remote_addr)
{
    bt_bd_addr_t *local_addr = bt_device_manager_aws_local_info_get_fixed_address();
    if (NULL != local_addr && NULL != remote_addr) {
        for (int32_t i = sizeof(bt_bd_addr_t); i > 0; i--) {
            if((*local_addr)[i - 1] > (*remote_addr)[i - 1]) {
                return BT_AWS_MCE_ROLE_AGENT;
            } else if ((*local_addr)[i - 1] < (*remote_addr)[i - 1]) {
                return BT_AWS_MCE_ROLE_PARTNER;
            }
        }
        bt_cm_assert(0 && "local and remote address are all same");
    } else {
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING][E] Addr is null,  local addr 0x%x, remote addr 0x%x", 2, local_addr, remote_addr);
    }
    return BT_AWS_MCE_ROLE_NONE;
}

static bt_status_t bt_aws_mce_srv_air_pairing_bt_reset_callback(bt_cm_power_reset_progress_t type, void *user_data)
{
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][E] Air pairing reset type %d", 1, type);
    if (BT_CM_POWER_RESET_PROGRESS_MEDIUM == type) {
        if (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED) {
            uint8_t peer_addr[6] = {0};
            bt_bd_addr_t *aws_fixed_addr = bt_device_manager_aws_local_info_get_fixed_address();
            bt_set_local_public_address((void *)aws_fixed_addr);
            bt_device_manager_aws_local_info_store_local_address(aws_fixed_addr);
            bt_device_manager_aws_local_info_store_peer_address((bt_bd_addr_t *)peer_addr);
        } else {
            if (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_FAIL) {
                /* Recover aws infor from nvdm. */
                bt_device_manager_aws_local_info_init();
                bt_bd_addr_t *public_addr = bt_device_manager_get_local_address();
                bt_set_local_public_address((void *)public_addr);
                g_aws_mce_air_pairing_cnt.flags = 0;
            }
        }
    } else if (BT_CM_POWER_RESET_PROGRESS_COMPLETE == type) {
        if (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED) {
            bt_hci_iac_lap_t iac = 0x9E8B11;
            g_aws_mce_air_pairing_cnt.flags |= BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INITIATED;
            bt_callback_manager_register_callback(bt_callback_type_app_event,
                (uint32_t)MODULE_MASK_GAP, (void *)bt_aws_mce_srv_air_pairing_event_handle);
            bt_gap_write_inquiry_access_code(&iac, 1);
            bt_cm_timer_start(BT_SINK_SRV_CM_TERMINATE_AIR_PAIRING_TIMER_ID,
                g_aws_mce_air_pairing_cnt.cnt.duration * 1000, bt_aws_mce_srv_air_pairing_timeout_callback, (void *)0);
        } else {
            bt_aws_mce_srv_air_pairing_complete_ind_t air_pairing_ind;
            bt_cm_memset(&air_pairing_ind, 0, sizeof(air_pairing_ind));
            air_pairing_ind.result = (bool)user_data;
            air_pairing_ind.cur_aws_role = bt_device_manager_aws_local_info_get_role();
            bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE, &air_pairing_ind, sizeof(air_pairing_ind));
            bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE, &air_pairing_ind, sizeof(air_pairing_ind));
        }
    }
    return BT_STATUS_SUCCESS;
}

static void         bt_aws_mce_srv_air_pairing_timeout_callback(void *params)
{
    uint32_t data = (uint32_t)params;
    bt_hci_iac_lap_t iac = BT_HCI_IAC_LAP_TYPE_GIAC;
    g_aws_mce_air_pairing_cnt.flags = 0;
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
    bt_gap_write_inquiry_access_code(&iac, 1);
    if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()){
        bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
    }

    if (0 == data) {
        bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE, bt_aws_mce_srv_air_pairing_bt_reset_callback, (void *)false);
    } else {
        bt_device_manager_aws_local_info_update();
        bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE, bt_aws_mce_srv_air_pairing_bt_reset_callback, (void *)true);
    }
}

static void         bt_aws_mce_srv_air_pairing_success(bt_bd_addr_t *remote_addr, uint8_t *remote_key)
{
    bt_aws_mce_role_t aws_role = bt_aws_mce_srv_air_pairing_get_aws_role((const bt_bd_addr_t *)remote_addr);
    bt_device_manager_aws_local_info_store_peer_address(remote_addr);
    bt_device_manager_aws_local_info_store_role(aws_role);
    if (BT_AWS_MCE_ROLE_PARTNER == aws_role) {
        bt_device_manager_aws_local_info_store_key((void *)remote_key);
    } else if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        bt_device_manager_aws_local_info_store_key(&(g_aws_mce_air_pairing_cnt.random_aws_key));
    }
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Air pairing success current role: 0x%x", 1, aws_role);
    bt_cm_timer_stop(BT_SINK_SRV_CM_TERMINATE_AIR_PAIRING_TIMER_ID);
    bt_cm_timer_start(BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_ID,
        BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_DUR, bt_aws_mce_srv_air_pairing_timeout_callback, (void *)1);
}

static void         bt_aws_mce_srv_air_pairing_generate_aws_secret_key(bt_key_t *aws_key)
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

static void         bt_aws_mce_srv_air_pairing_inq_ind_handle(bt_gap_inquiry_ind_t *inq_ret)
{
    if (NULL == inq_ret || NULL == inq_ret->rssi) {
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING][E] Inquiry indicate buffer is NULL !!!", 0);
        return;
    }
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Recevice rssi %d, need rssi %d flags %d", 3,
        *(int8_t *)(inq_ret->rssi), g_aws_mce_air_pairing_cnt.cnt.rssi_threshold, g_aws_mce_air_pairing_cnt.flags);
    if ((*(int8_t *)(inq_ret->rssi) > g_aws_mce_air_pairing_cnt.cnt.rssi_threshold) &&
        (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING)) {
        bt_os_layer_aes_buffer_t decrypted_data;
        bt_os_layer_aes_buffer_t plain_text;
        bt_os_layer_aes_buffer_t aes_key;
        uint8_t air_pairing_length = sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info) + sizeof(bt_key_t);
        uint8_t air_pairing_rsp[air_pairing_length];

        decrypted_data.buffer = (uint8_t *)&air_pairing_rsp;
        decrypted_data.length = air_pairing_length;
        plain_text.buffer = (uint8_t *)&(inq_ret->eir[4]);
        plain_text.length = air_pairing_length;
        aes_key.buffer = g_aws_mce_air_pairing_cnt.cnt.air_pairing_key;
        aes_key.length = sizeof(bt_key_t);
        bt_os_layer_aes_decrypt(&decrypted_data, &plain_text, &aes_key);
        if (0 == memcmp((uint8_t *)decrypted_data.buffer, (uint8_t *)g_aws_mce_air_pairing_cnt.cnt.air_pairing_info,
            sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info))) {
            g_aws_mce_air_pairing_cnt.flags |= BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_COMPLETE;
            bt_gap_cancel_inquiry();
            bt_aws_mce_srv_air_pairing_success((bt_bd_addr_t *)inq_ret->address,
                (uint8_t *)decrypted_data.buffer + sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info));
        }
    }
}

static void         bt_aws_mce_srv_air_pairing_set_eir_cnf_handle(void)
{
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    /* 0.625 ms * 36(0x24) = 22.5 ms */
    uint16_t interval = 0x24;
    /* 0.625 ms * 18(0x12) = 11.25 ms */
    uint16_t window = 0x12;
    uint32_t random_seed;
    audio_channel_t audio_ch = g_aws_mce_air_pairing_cnt.cnt.audio_ch;
    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] generate random inquiry init fail", 0);
    } else {
        ret = hal_trng_get_generated_random_number(&random_seed);
        if (HAL_TRNG_STATUS_OK != ret) {
            bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] generate random inquiry window error", 0);
        } else {
            if (AUDIO_CHANNEL_L == audio_ch) {
                /* interval max: 0.625 ms * 80(0x50) = 50 ms */
                /* interval min: 0.625 ms * 36(0x24) = 22.5 ms */
                interval = (random_seed & (0x50 - 0x24)) + 0x24;
                bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] left interval %d", 1, interval);
            } else {
                /* interval max: 0.625 ms * 160(0xA0) = 100 ms */
                /* interval min: 0.625 ms * 96(0x60) = 60 ms */
                interval = 0x60 + (random_seed & (0x40 - 0x24)) + 0x24;
                bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] right interval %d", 1, interval);
            }
        }
    }
    hal_trng_deinit();
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] generate random inquiry scan interval: %d, window: %d", 2, interval, window);
    bt_gap_write_inquiry_scan_activity(interval, window);
    bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY);
}

static void         bt_aws_mce_srv_air_pairing_write_iac_cnf(void)
{
    bt_os_layer_aes_buffer_t encryped_key;
    bt_os_layer_aes_buffer_t plain_text;
    bt_os_layer_aes_buffer_t aes_key;
    uint8_t air_info_length = sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info) + sizeof(bt_key_t);
    uint8_t air_info[air_info_length];
    uint8_t eir_data[air_info_length + 4];
    memset(air_info, 0, sizeof(air_info));
    memset(eir_data, 0, sizeof(eir_data));
    memcpy(&air_info, &g_aws_mce_air_pairing_cnt.cnt.air_pairing_info, sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info));
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] air info 1 0x%x", 1, *(uint32_t *)air_info);
    /* Random Generate aws key */
    bt_aws_mce_srv_air_pairing_generate_aws_secret_key(&(g_aws_mce_air_pairing_cnt.random_aws_key));
    bt_hci_enable_t filter_enable = BT_HCI_DISABLE;
    encryped_key.buffer = eir_data + 4;
    encryped_key.length = sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info) + sizeof(bt_key_t);
    plain_text.buffer = air_info;
    plain_text.length = sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info) + sizeof(bt_key_t);
    memcpy((uint8_t *)(plain_text.buffer + sizeof(g_aws_mce_air_pairing_cnt.cnt.air_pairing_info)),
        (uint8_t *)&(g_aws_mce_air_pairing_cnt.random_aws_key), sizeof(bt_key_t));
    aes_key.buffer = g_aws_mce_air_pairing_cnt.cnt.air_pairing_key;
    aes_key.length = sizeof(bt_key_t);
    bt_os_layer_aes_encrypt(&encryped_key, &plain_text, &aes_key);
    eir_data[0] = (sizeof(eir_data) - 1);
    eir_data[1] = 0xFF;
    eir_data[2] = 0x94;
    eir_data[3] = 0x00;
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING] eir data info 2 0x%x", 1, *(uint32_t *)(eir_data + 4));
    bt_hci_send_vendor_cmd(0xFDC9, &filter_enable, sizeof(filter_enable));
    bt_gap_set_extended_inquiry_response(eir_data, sizeof(eir_data));
}

static bt_status_t  bt_aws_mce_srv_air_pairing_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    if (!(g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INITIATED)) {
        return result;
    }
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING] msg:0x%x, status:0x%x", 2, msg, status);
    switch (msg) {
    case BT_GAP_SET_SCAN_MODE_CNF: {
        if (status == BT_STATUS_SUCCESS) {
            uint8_t inquiry_duration = ((g_aws_mce_air_pairing_cnt.cnt.duration - 4) * 100) / 128;
            bt_gap_inquiry_ext(inquiry_duration, 0, 0x9E8B11);
        } else {
            bt_aws_mce_srv_air_pairing_stop();
        }
    }
        break;
    case BT_GAP_INQUIRY_CNF: {
        if (status == BT_STATUS_SUCCESS) {
            g_aws_mce_air_pairing_cnt.flags |= BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING;
        } else {
            bt_aws_mce_srv_air_pairing_stop();
        }
    }
        break;
    case BT_GAP_INQUIRY_IND: {
        bt_aws_mce_srv_air_pairing_inq_ind_handle((bt_gap_inquiry_ind_t *)buffer);
    }
        break;
    case BT_GAP_SET_EIR_CNF: {
        if (BT_STATUS_SUCCESS == status) {
            bt_aws_mce_srv_air_pairing_set_eir_cnf_handle();
        } else {
            bt_aws_mce_srv_air_pairing_stop();
        }
    }
        break;
    case BT_GAP_WRITE_INQUIRY_ACCESS_CODE_CNF: {
        if (BT_STATUS_SUCCESS == status) {
            bt_aws_mce_srv_air_pairing_write_iac_cnf();
        } else {
            bt_aws_mce_srv_air_pairing_stop();
        }
    }
        break;
    case BT_GAP_INQUIRY_COMPLETE_IND: {
        g_aws_mce_air_pairing_cnt.flags &= (~BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING);
        bt_aws_mce_srv_air_pairing_stop();
    }
        break;
    case BT_GAP_CANCEL_INQUIRY_CNF: {
        g_aws_mce_air_pairing_cnt.flags &= (~BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING);
        if (!(g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_COMPLETE)) {
            bt_aws_mce_srv_air_pairing_stop();
        } else {
            g_aws_mce_air_pairing_cnt.flags &= (~BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_COMPLETE);
        }
    }
        break;
    default:
        bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Unexcepted msg:%x", 1, msg);
        break;
    }
    return result;
}

bt_status_t         bt_aws_mce_srv_air_pairing_start(const bt_aws_mce_srv_air_pairing_t *param)
{
    if (NULL == param) {
        return BT_CM_STATUS_INVALID_PARAM;
    }
    if (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED) {
        return BT_CM_STATUS_PENDING;
    }
    g_aws_mce_air_pairing_cnt.flags |= BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED;
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Air pairing start duration %d, default role 0x%02X, rssi %d info 0x%x", 4,
        param->duration, param->default_role, param->rssi_threshold, *(uint32_t*)(param->air_pairing_info));
    bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_STARTED, NULL, 0);
    bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_STARTED, NULL, 0);
    bt_cm_memcpy(&(g_aws_mce_air_pairing_cnt.cnt), param, sizeof(*param));
    bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_START, bt_aws_mce_srv_air_pairing_bt_reset_callback, NULL);
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_aws_mce_srv_air_pairing_stop()
{
    bt_cmgr_report_id("[BT_CM][AIR_PAIRING][I] Cancel air pairing cur flags 0x%02x", 1, g_aws_mce_air_pairing_cnt.flags);
    if (!(g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_STARTED)) {
        return BT_CM_STATUS_INVALID_STATUS;
    }
    bt_cm_timer_stop(BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_ID);
    bt_cm_timer_stop(BT_SINK_SRV_CM_TERMINATE_AIR_PAIRING_TIMER_ID);
    if (g_aws_mce_air_pairing_cnt.flags & BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_INQUIRYING) {
        bt_gap_cancel_inquiry();
    } else {
        bt_hci_iac_lap_t iac = BT_HCI_IAC_LAP_TYPE_GIAC;
        bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
        bt_gap_write_inquiry_access_code(&iac, 1);
        g_aws_mce_air_pairing_cnt.flags = BT_AWS_MCE_SRV_AIR_PAIRING_FLAG_FAIL;
        /* Recover aws infor from nvdm. */
        bt_device_manager_aws_local_info_init();
        bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_aws_mce_srv_air_pairing_event_handle);
        bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE, bt_aws_mce_srv_air_pairing_bt_reset_callback, NULL);
    }
    return BT_STATUS_SUCCESS;
}


