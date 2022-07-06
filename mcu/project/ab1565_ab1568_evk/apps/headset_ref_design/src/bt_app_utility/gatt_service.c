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
 * File: gatt_service.c
 *
 * Description: The file is GATT service definition.
 *
 */
#include <string.h>
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#ifdef MTK_BLE_IAS
#include "ble_ias.h"
#endif
#ifdef __BLE_BAS__
#include "ble_bas.h"
#endif
#ifdef __BT_FAST_PAIR_ENABLE__
#include "bt_fast_pair.h"
#endif
#include "bt_gatts_service.h"

extern const bt_gatts_service_t **bt_get_gatt_server(void);
#if _MSC_VER >= 1500
#pragma comment(linker,"/alternatename:_bt_get_gatt_server=_bt_gatt_service_get_table")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
#pragma weak bt_get_gatt_server = bt_gatt_service_get_table
#else
#error "Unsupported Platform"
#endif
/*
Declare every record here
service collects all bt_gatts_service_rec_t
IMPORTAMT: handle:0x0000 is reserved, please start your handle from 0x0001
GAP 0x0001
*/

#ifdef MTK_BLE_SMTCN_ENABLE
extern const bt_gatts_service_t bt_if_dtp_service;
#endif
/*
extern const bt_gatts_service_t bt_if_gatt_service;
extern const bt_gatts_service_t bt_if_gap_service;
extern const bt_gatts_service_t bt_if_dogp_service;
extern const bt_gatts_service_t ble_bas_service;
*/
#ifdef MTK_BLE_IAS
extern const bt_gatts_service_t ble_ias_service;
#endif

#ifdef MTK_PORT_SERVICE_BT_ENABLE
extern const bt_gatts_service_t ble_air_service;
#endif

extern const bt_gatts_service_t ble_dis_service;

#ifdef MTK_AMA_ENABLE
extern const bt_gatts_service_t ble_ama_service;
#endif

#ifdef MTK_VA_XIAOWEI_ENABLE
extern const bt_gatts_service_t xiaowei_ble_service;
#endif

#ifdef MTK_VA_XIAOAI_ENABLE
extern const bt_gatts_service_t xiaoai_ble_service;
#endif

#ifdef AIR_LE_AUDIO_ENABLE
extern const bt_gatts_service_t ble_vcs_service;
extern const bt_gatts_service_t ble_vocs_service_channel_1;
#ifdef AIR_LE_AUDIO_HEADSET_ENABLE
extern const bt_gatts_service_t ble_vocs_service_channel_2;
#endif
extern const bt_gatts_service_t ble_aics_service;
extern const bt_gatts_service_t ble_mics_service;
extern const bt_gatts_service_t ble_csis_service;
extern const bt_gatts_service_t ble_cas_service;
extern const bt_gatts_service_t ble_ascs_service;
extern const bt_gatts_service_t ble_pacs_service;
extern const bt_gatts_service_t ble_bass_service;
extern const bt_gatts_service_t ble_tmas_service;
#endif

/**< gatt server collects all service. */
const bt_gatts_service_t *bt_if_clm_gatt_server[] = {
    &bt_if_gap_service, /**< handle range: 0x0001 to 0x0009. */
    &bt_if_gatt_service,/**< handle range: 0x0011 to 0x0015. */
#ifdef MTK_BLE_SMTCN_ENABLE
    &bt_if_dtp_service, //0x0014-0x0017
#endif
    /*&bt_if_dogp_service,*/ /**< handle range: 0x0020 to 0x0025. */
#ifdef __BLE_BAS__
    &ble_bas_service,/**< handle range: 0x0031 to 0x0034. */
#endif
#ifdef MTK_BLE_IAS
    &ble_ias_service,/**< handle range: 0x0040 to 0x0042. */
#endif
#ifdef MTK_PORT_SERVICE_BT_ENABLE
    &ble_air_service,/**< handle range: 0x0051 to 0x0056. */
#endif
    &ble_dis_service,/**< handle range: 0x0060 to 0x0072. */
#ifdef MTK_AMA_ENABLE
    &ble_ama_service,/**handle range: 0x00D0 to 0x00D5. */
#endif
#ifdef MTK_VA_XIAOWEI_ENABLE
    &xiaowei_ble_service,
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
    &xiaoai_ble_service,            /**< handle range: 0x00F0 to 0x00F5. */
#endif
#ifdef __BT_FAST_PAIR_ENABLE__
    &bt_fast_pair_service,/**< handle range: 0x0100 to 0x013F. */
#endif
#ifdef AIR_LE_AUDIO_ENABLE
    &ble_ascs_service,   /**< handle range: 0x1103 to 0x110F. */
    &ble_pacs_service,   /**< handle range: 0x1200 to 0x1212. */
    &ble_vcs_service,    /**< handle range: 0x1301 to 0x1309. */
    &ble_vocs_service_channel_1, /**< handle range: 0x2001 to 0x200C. */
#ifdef AIR_LE_AUDIO_HEADSET_ENABLE
    &ble_vocs_service_channel_2, /**< handle range: 0x3001 to 0x300C. */
#endif
    &ble_aics_service,   /**< handle range: 0x4001 to 0x4010. */
    &ble_mics_service,   /**< handle range: 0x5001 to 0x5004. */
    &ble_csis_service,   /**< handle range: 0x6001 to 0x600A. */
    &ble_cas_service,    /**< handle range: 0x7001 to 0x7002. */
    &ble_bass_service,   /**< handle range: 0xA201 to 0xA209. */
    &ble_tmas_service,   /**< handle range: 0xA301 to 0xA303. */
#endif
    NULL
};

const bt_gatts_service_t **bt_gatts_get_server_by_handle(bt_handle_t connection_handle)
{
    return bt_get_gatt_server();
}

const bt_gatts_service_t **bt_gatt_service_get_table(void)
{
    return bt_if_clm_gatt_server;
}
