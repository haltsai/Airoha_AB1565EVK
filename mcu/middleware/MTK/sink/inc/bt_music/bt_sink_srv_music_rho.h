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
 
#ifndef __BT_SINK_SRV_MUSIC_RHO_H__
#define __BT_SINK_SRV_MUSIC_RHO_H__

#include "bt_sink_srv_music.h"
#include "bt_device_manager_internal.h"

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
#define BT_SINK_SRV_MUSIC_RHO_WAITING_LIST_FLAG     (1<<0) /**< Fix the issue for GVA-9584*/
#define BT_SINK_SRV_MUSIC_RHO_START_ON_NEW_AGENT    (1<<1) /**< Fix the issue for GWICLUBPRO-103*/
#define BT_SINK_SRV_MUSIC_RHO_EMPTY_DEVICE_FLAG     (1<<2)

typedef struct {
    uint32_t flag;                                          /**< Device flag */
    uint32_t op;                                            /**< Operation flag */
    uint32_t avrcp_volume;                                  /**< avrcp volume*/
    uint16_t conn_bit;
    bool vol_rsp_flag;
    uint8_t vol;
    uint8_t a2dp_status;
    uint8_t avrcp_status;
    uint8_t last_play_pause_action;
    uint8_t rho_temp_flag;
    bt_device_manager_db_remote_pnp_info_t device_id;
#ifdef  AIR_MULTI_POINT_ENABLE
    bt_a2dp_codec_capability_t codec_cap;
#endif
} bt_sink_srv_music_change_context_t;

#ifdef  AIR_MULTI_POINT_ENABLE
typedef struct {
    bt_bd_addr_t playing_addr;
} bt_sink_srv_music_common_exchange_context_t;
#endif


#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
bt_status_t bt_sink_srv_music_role_handover_service_init(void);
#endif
//bool bt_sink_srv_music_role_handover_is_allowed(void);
//uint8_t bt_sink_srv_music_role_handover_get_length(const bt_bd_addr_t* addr);
//void bt_sink_srv_music_role_handover_get_data(uint8_t *data, const bt_bd_addr_t* addr);
//void bt_sink_srv_music_role_handover(void *profile_data, uint8_t *data);

#endif

