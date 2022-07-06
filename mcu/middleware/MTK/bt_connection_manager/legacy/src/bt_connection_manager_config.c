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


#include "bt_connection_manager.h"
#include "bt_connection_manager_utils.h"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_connection_manager_event_callback=_default_bt_connection_manager_event_callback")
#pragma comment(linker, "/alternatename:_bt_connection_manager_air_pairing_get_aws_role=_default_bt_connection_manager_air_pairing_get_aws_role")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_link_loss_reconnect_params=_default_bt_sink_srv_get_link_loss_reconnect_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_power_on_reconnect_params=_default_bt_sink_srv_get_power_on_reconnect_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_visibility_params=_default_bt_sink_srv_get_visibility_params")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_eir=_default_bt_sink_srv_get_eir")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_role_switch_retry_times=_default_bt_sink_srv_get_role_switch_retry_times")
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_page_timeout_paramters=_default_bt_sink_srv_get_page_timeout_paramters")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_connection_manager_event_callback = default_bt_connection_manager_event_callback
#pragma weak bt_connection_manager_air_pairing_get_aws_role = default_bt_connection_manager_air_pairing_get_aws_role
#pragma weak bt_sink_srv_get_link_loss_reconnect_params = default_bt_sink_srv_get_link_loss_reconnect_params
#pragma weak bt_sink_srv_get_power_on_reconnect_params = default_bt_sink_srv_get_power_on_reconnect_params
#pragma weak bt_sink_srv_get_visibility_params = default_bt_sink_srv_get_visibility_params
#pragma weak bt_sink_srv_get_eir = default_bt_sink_srv_get_eir
#pragma weak bt_sink_srv_get_role_switch_retry_times = default_bt_sink_srv_get_role_switch_retry_times
#pragma weak bt_sink_srv_get_page_timeout_paramters = default_bt_sink_srv_get_page_timeout_paramters
#else
#error "Unsupported Platform"
#endif

bt_connection_manager_features_config_t bt_connection_manager_features_config;

const static bt_sink_srv_feature_reconnect_params_t link_loss_reconnect_params = {
    .attampts = 3
};

void default_bt_connection_manager_event_callback(bt_connection_manager_event_t event_id, void *params, uint32_t params_len)
{
    bt_cmgr_report_id("[BT_CM]Not find event handler event_id:%d, param:0x%x", 2, event_id, params);
}

bt_aws_mce_role_t default_bt_connection_manager_air_pairing_get_aws_role(bt_bd_addr_t *remote_addr)
{
    bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
    if (NULL != local_addr && NULL != remote_addr) {
        int32_t i = 0;
        while (i < sizeof(bt_bd_addr_t)) {
            if((*local_addr)[i] > (*remote_addr)[i]) {
                return BT_AWS_MCE_ROLE_AGENT;
            } else if ((*local_addr)[i] < (*remote_addr)[i]) {
                return BT_AWS_MCE_ROLE_PARTNER;
            }
            i++;
        }
        bt_connection_manager_assert(0 && "local and remote address are all same");
    } else {
        bt_cmgr_report_id("[BT_CM][E]Addr is null,  local addr %p, remote addr %p", 2, local_addr, remote_addr);
    }
    return BT_AWS_MCE_ROLE_NONE;
}

const bt_sink_srv_feature_reconnect_params_t *  default_bt_sink_srv_get_link_loss_reconnect_params(void)
{
    return &link_loss_reconnect_params;
}

const static bt_sink_srv_feature_reconnect_params_t power_on_reconnect_params = {
    .attampts = 3
};
const bt_sink_srv_feature_reconnect_params_t *  default_bt_sink_srv_get_power_on_reconnect_params(void)
{
    return &power_on_reconnect_params;
}

const static bt_sink_srv_feature_visibility_params_t visibility_params = {
    .visibility_duration = 90000,
    .power_on_be_visible_once = false
};
const bt_sink_srv_feature_visibility_params_t * default_bt_sink_srv_get_visibility_params(void)
{
    return &visibility_params;
}

uint8_t default_bt_sink_srv_get_role_switch_retry_times(void)
{
    return 0xFF;
}

uint16_t default_bt_sink_srv_get_page_timeout_paramters(void)
{
    return 0x5dc0; /* 0x5dc0 * 0.625 msec = 15 sec */
}

const static uint8_t default_eir_data[16] = {0x1D,0x23,0xBB,0x1B,0x00,0x00,0x10,0x00,0x30,0x00,0x50,0x80,0x5F,0x9B,0x34,0xFA};
const static bt_sink_srv_eir_information_t eir_params = {
    .uuid_128bit = (const uint8_t*)&default_eir_data,
    .rssi = 0    
};

const bt_sink_srv_eir_information_t* default_bt_sink_srv_get_eir(void)
{
    return &eir_params;
}


#include "bt_gap.h"
#ifdef MTK_NVDM_ENABLE
#define __QA_TEST__
#endif
// database constants
#define BT_SINK_SRV_CM_DB_NAME  "BT_Headset_Demo"
#define BT_SINK_SRV_CM_DB_COD 0x240404
#define BT_SINK_SRV_CM_DB_IO    BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT
#ifdef __QA_TEST__
#include "nvdm.h"
#endif
#ifdef __QA_TEST__
static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#else
const static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#endif

uint32_t g_remote_cod;
const bt_gap_config_t *bt_sink_srv_gap_config(void)
{
#ifdef __QA_TEST__
    uint8_t name[30] = {0};

    //Change BT local name to BT_Head_xxx for QA test (xxx is BT addr)
    bt_bd_addr_t* local_addr = bt_connection_manager_device_local_info_get_local_address();
    snprintf((char *)name, sizeof(name), "BT_Head_%.2X%.2X%.2X%.2X%.2X%.2X",
    (*local_addr)[5],(*local_addr)[4],(*local_addr)[3],(*local_addr)[2],(*local_addr)[1],(*local_addr)[0]);
    bt_connection_manager_memcpy(g_bt_sink_srv_gap_config.device_name, name, strlen((char *)name));
    bt_cmgr_report("[BT_CM] device name:%s", g_bt_sink_srv_gap_config.device_name);    
#endif
    return &g_bt_sink_srv_gap_config;
}

void bt_connection_manager_config_features(bt_connection_manager_features_config_t *features)
{
    bt_connection_manager_memcpy(&bt_connection_manager_features_config, features, sizeof(bt_connection_manager_features_config_t));
}

const bt_connection_manager_features_config_t *bt_connection_manager_get_features_config(void)
{
    return &bt_connection_manager_features_config;
}

bt_gap_accept_connection_status_t bt_gap_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod)
{
    g_remote_cod = cod;

    bt_gap_config_t *cfg = (bt_gap_config_t *)bt_sink_srv_gap_config();
    bt_cmgr_report_id("[BT_CM] ACL accept_check, rmt_cod:0x%08x, local_cod:0x%08x", 2, cod, cfg->cod);
#ifndef MTK_AWS_MCE_ENABLE
    if (cod == cfg->cod) {
        return BT_GAP_ACCEPT_AS_MASTER;
    } else 
#endif
    {
        return BT_GAP_ACCEPT_AS_SLAVE;    
    }
}

