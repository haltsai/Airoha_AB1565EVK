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
#include "bt_device_manager_internal.h"
#include "bt_device_manager_test_mode.h"
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_utility.h"
#endif

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

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_cm_check_connect_request=_default_bt_cm_check_connect_request")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_cm_check_connect_request = default_bt_cm_check_connect_request
#else
#error "Unsupported Platform"
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
    bt_cm_memcpy(g_bt_sink_srv_gap_config.device_name, name, strlen((char *)name));
    bt_cmgr_report("[BT_CM] device name:%s", g_bt_sink_srv_gap_config.device_name);    
#endif
    return &g_bt_sink_srv_gap_config;
}

bt_gap_accept_connection_status_t bt_gap_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod)
{
    g_remote_cod = cod;
    bt_gap_config_t *cfg = (bt_gap_config_t *)bt_sink_srv_gap_config();
    bt_cmgr_report_id("[BT_CM] ACL accept_check, rmt_cod:0x%08x, local_cod:0x%08x", 2, cod, cfg->cod);
    bt_device_manager_remote_update_cod((void *)address, cod);
    if (BT_CM_POWER_STATE_ON != bt_cm_power_get_state() && BT_DEVICE_MANAGER_TEST_MODE_NONE == bt_device_manager_get_test_mode()) {
        return BT_GAP_REJECTED_DUE_TO_LIMITED_RESOURCES;
    }
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)address);
    if (NULL == device_p) {
        bt_bd_addr_t temp_addr = {0};
        if (NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, temp_addr))) {
            return BT_GAP_REJECTED_DUE_TO_LIMITED_RESOURCES;
        }
        bt_cm_memset(device_p, 0, sizeof(*device_p));
        bt_cm_memcpy(device_p->link_info.addr, address, sizeof(bt_bd_addr_t));
        device_p->flags |= BT_CM_REMOTE_FLAG_PASSIV_CONNECTING; 
    }
    if (bt_cm_check_connect_request(address,cod)) {
	    /* To avoid IOT issue. (Multiple remote device request connect at the same time), to disable the inquriry scan and page scan, before accept one remote connect request. */
	    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
        bt_ull_context_t* ull_ctx = bt_ull_get_context();
        if (BT_ULL_ROLE_SERVER == ull_ctx->ull_role) {
            return BT_GAP_ACCEPT_AS_MASTER;
        } else {
            return BT_GAP_ACCEPT_AS_SLAVE;
        }
#endif
#ifndef MTK_AWS_MCE_ENABLE
        if (cod == cfg->cod) {
            return BT_GAP_ACCEPT_AS_MASTER;
        } else
#endif
        {
            return BT_GAP_ACCEPT_AS_SLAVE;
        }
    } else {
        device_p->link_info.link_state = BT_CM_ACL_LINK_DISCONNECTING;
        device_p->flags &= ~(BT_CM_REMOTE_FLAG_PASSIV_CONNECTING);
        return BT_GAP_REJECTED_DUE_TO_LIMITED_RESOURCES;
    }
}



bool default_bt_cm_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod)
{
    return true;
}
