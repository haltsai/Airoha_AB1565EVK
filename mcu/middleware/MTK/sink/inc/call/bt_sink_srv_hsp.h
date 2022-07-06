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
 
#ifndef __BT_SINK_SRV_HSP_H__
#define __BT_SINK_SRV_HSP_H__

#include "bt_sink_srv.h"
#include "bt_hsp.h"
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_HSP_LINK_NUM    0x03
#define BT_SINK_SRV_HSP_CMD_LENGTH  12
#define BT_SINK_SRV_TIMER_DURATION_HS_WAIT_RING_IND        (5500)
#define BT_SINK_SRV_HSP_SWITCH_LINK 0x80


#define BT_SINK_SRV_HSP_FLAG_NONE      (0x00)
#define BT_SINK_SRV_HSP_FLAG_RINGING   (0x01)
typedef uint8_t bt_sink_srv_hsp_flag_t;     /**<The hsp link flag. */

typedef struct {
    uint32_t handle;
    bt_bd_addr_t address;
    bt_sink_srv_hsp_flag_t flag;
    bt_sink_srv_sco_connection_state_t sco_state;
    void *device;
    bool is_used;
} bt_sink_srv_hsp_context_t;

bt_status_t bt_sink_srv_hsp_connect(bt_bd_addr_t *address);
bool bt_sink_srv_hsp_check_is_connected(bt_bd_addr_t *address);

bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_alloc_free_context(bt_bd_addr_t *address);
bt_sink_srv_hsp_context_t *bt_sink_srv_hsp_get_context_by_address(bt_bd_addr_t *address);
bool bt_sink_srv_hsp_check_is_connected_by_context(bt_sink_srv_hsp_context_t *context);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_HSP_H__ */

