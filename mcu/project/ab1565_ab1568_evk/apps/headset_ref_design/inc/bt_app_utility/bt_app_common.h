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

#ifndef __BT_APP_COMMON_H__
#define __BT_APP_COMMON_H__


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"
#include "ui_shell_activity.h"
#include "bt_hci.h"
#include "bt_sink_srv.h"
#include "bt_gap_le_service.h"
#include "syslog.h"

#ifdef MTK_BLE_IAS
#define __BLE_FMP__
#endif
#ifdef MTK_BLE_BAS
#define __BLE_BAS__
#endif
//#define __BLE_ANCS__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define BT_APP_COMMON_UNIQUE_ID_MAX_LEN     (8)

void bt_app_common_init(void);

bt_status_t bt_app_common_generate_unique_id(const uint8_t *input,
                                             size_t ilen,
                                             uint8_t unique_id[16]);

void bt_app_common_cmd_disable_advertising(void);

void bt_app_common_generate_device_name(void);

typedef enum {
    BT_APP_COMMON_BLE_ADV_STOPPED   = 0,
    BT_APP_COMMON_BLE_ADV_STARTED,
    BT_APP_COMMON_BLE_ADV_STOPING,
    BT_APP_COMMON_BLE_ADV_STARTING,
    BT_APP_COMMON_BLE_ADV_UPDATING,
} bt_app_common_ble_adv_status_t;

bt_status_t bt_app_common_generate_default_adv_data(
    bt_hci_le_set_ext_advertising_parameters_t *adv_param,
    bt_gap_le_set_ext_advertising_data_t *adv_data,
    bt_gap_le_set_ext_scan_response_data_t *scan_data,
    uint8_t *unique_id,
    uint8_t unique_id_len);

bt_status_t bt_app_common_advtising_start(uint16_t adv_interval_min,
                                          uint16_t adv_interval_max,
                                          uint8_t *unique_id,
                                          uint8_t unique_id_len);

bt_status_t bt_app_common_advertising_start_ex(
    const bt_hci_cmd_le_set_advertising_parameters_t *adv_param,
    const bt_hci_cmd_le_set_advertising_data_t *adv_data,
    const bt_hci_cmd_le_set_scan_response_data_t *scan_rsp);

bt_status_t bt_app_common_advertising_start_ex_2(
    bool update_adv_param,
    const bt_hci_cmd_le_set_advertising_parameters_t *adv_param,
    const bt_hci_cmd_le_set_advertising_data_t *adv_data,
    const bt_hci_cmd_le_set_scan_response_data_t *scan_rsp);

bt_status_t bt_app_common_ble_adv_timer_start(void);

bt_status_t bt_app_common_start_ble_adv_with_default_interval(void);

bt_status_t bt_app_common_advtising_stop(void);

bt_status_t bt_app_common_trigger_increase_ble_adv(void);

bt_app_common_ble_adv_status_t bt_app_common_get_advertising_status(void);

bool bt_app_common_get_power_on_status(void);

void bt_app_common_on_bt_sink_event_callback(bt_sink_srv_event_t event_id,
                                             void *param, uint32_t param_len);


const bt_bd_addr_t *bt_app_common_get_aws_peer_random_address(void);

bt_status_t bt_app_common_store_local_random_address(bt_bd_addr_t *addr);

void bt_app_common_generate_random_address(bt_bd_addr_t addr);

bool bt_app_common_sink_event_proc(uint32_t event_id, void *extra_data, size_t data_len);
bool bt_app_common_gsound_event_proc(uint32_t event_id, void *extra_data, size_t data_len);

bool bt_app_common_cm_event_proc(uint32_t event_id, void *extra_data, size_t data_len);

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
typedef enum {
    BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE,
    BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT,
    BT_APP_COMMON_ULL_LATENCY_MODULE_MAX
} bt_app_common_ull_latency_module_t;

/**
 *  @brief This enum defines the supported retry count, current support 2 ~ 6 and 18. The latency equal retry count * 2.5ms + 15ms.
 */
typedef enum {
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_2 = 2,     /* Latency is 20ms. */
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_3,         /* Latency is 22.5ms. */
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_4,         /* Latency is 25ms. */
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_5,         /* Latency is 27.5ms. */
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_6,         /* Latency is 30ms. */
    BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_18 = 18,   /* Some customer need the latency is 60ms. */
} bt_app_common_ull_stream_retry_count_t;

#define BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_DISCOVER   (BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_18)
#define BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING (BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_18)
#define BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK (BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_4)
#define BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP    (BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_18)

void bt_app_common_pre_set_ultra_low_latency_retry_count(bt_app_common_ull_latency_module_t module, bt_app_common_ull_stream_retry_count_t retry_count);

void bt_app_common_apply_ultra_low_latency_retry_count(void);
#endif

uint8_t *bt_app_common_get_ble_local_irk(void);

#ifdef __cplusplus
}
#endif

#endif /* __BT_APP_COMMON_H__ */

