/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 
/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements DOGP service service structures and functions
 *
 ****************************************************************************/

#ifndef __GATT_OVER_BREDR_AIR_INTERNAL_H__
#define __GATT_OVER_BREDR_AIR_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"
#include "gatt_over_bredr_air.h"

/************************************************
*   structures
*************************************************/

/**
 * structure for main cntx.
 */
typedef struct {
    bool                            is_real_connected; //After ACL link connected and first write request had come
    uint16_t                        notify_enabled;
    uint16_t                        conn_handle;
    uint16_t                        max_packet_size; 
    bt_bd_addr_t                    peer_addr; /**< Address information of the remote device. */
    uint8_t                         *receive_buffer;
    uint32_t                        receive_buffer_start;
    uint32_t                        receive_buffer_length;   
} gatt_over_bredr_air_cntx_t;

typedef struct {
    bool in_use;
    gatt_over_bredr_air_common_callback_t callback;
} gatt_over_bredr_air_callback_node_t;


bt_status_t gatt_over_bredr_air_event_callback_init(void);
gatt_over_bredr_air_cntx_t *gatt_over_bredr_air_get_cntx_by_handle(uint16_t conn_handle);
uint32_t gatt_over_bredr_air_tx_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
uint32_t gatt_over_bredr_air_rx_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);


#ifdef __cplusplus
}
#endif

#endif /**__GATT_OVER_BREDR_AIR_INTERNAL_H__*/

