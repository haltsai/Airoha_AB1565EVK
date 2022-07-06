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

#ifndef __BT_SINK_SRV_AWS_MCE_CALL_H__
#define __BT_SINK_SRV_AWS_MCE_CALL_H__

#include "bt_sink_srv_aws_mce.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_AWS_MCE_CALL_INFO_LENGTH        4
#define BT_SINK_SRV_AWS_MCE_CALL_INFO_RETRY_COUNT   5
#define BT_SINK_SRV_AWS_MCE_CALL_INFO_RETRY_TIMEOUT 200
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
#define BT_SINK_SRV_AWS_MCE_CALL_HF_SWITCH          1
#endif

#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && defined(AIR_MULTI_POINT_ENABLE)
#define BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT
#endif

typedef uint8_t bt_sink_srv_aws_mce_call_packet_type_t;
#define BT_SINK_SRV_AWS_MCE_CALL_PACKET_ACTION 0

typedef uint8_t bt_sink_srv_aws_mce_sco_state;
#define BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED  0
#define BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED  1
#define BT_SINK_SRV_AWS_MCE_SCO_STATE_ACTIVE  2

typedef uint8_t bt_sink_srv_aws_mce_call_info_mask_t;
#define BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE       0x01
#define BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_SCO_STATUS  0x02
#define BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME      0x04
#define BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND    0x08
#define BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_ALL         0xFF

/*AWS MCE call state machine*/
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE             0x00
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_INCOMING         0x01
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_OUTGOING         0x02
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_ACTIVE           0x03
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_INCOMING     0x04
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_TWC_OUTGOING     0x05
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_ACTIVE      0x06
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_HELD_REMAINING   0x07
#define BT_SINK_SRV_AWS_MCE_CALL_STATE_MULTIPARTY       0x08
typedef uint8_t bt_sink_srv_aws_mce_call_state_t;

typedef enum {
    BT_SINK_SRV_AWS_MCE_CALL_FLAG_NONE                = 0x0000,
    BT_SINK_SRV_AWS_MCE_CALL_FLAG_SUSPEND             = 0x0001
} bt_sink_srv_aws_mce_call_flag_t;

typedef struct {
    bt_sink_srv_aws_mce_call_state_t call_state;
    bt_sink_srv_aws_mce_sco_state sco_state;
    uint8_t volume;
    uint8_t is_ring;
} bt_sink_srv_aws_mce_call_info_t;

typedef struct {
    bt_sink_srv_aws_mce_call_info_mask_t mask;
    bt_sink_srv_aws_mce_call_info_t data;
} bt_sink_srv_aws_mce_call_update_info_t;

typedef struct {
    uint32_t aws_handle;
    bt_sink_srv_aws_mce_call_info_t call_info;
    void* device;
#ifdef MTK_BT_SPEAKER_ENABLE
    uint32_t info_retry_counter;
#endif
    bt_sink_srv_aws_mce_call_flag_t flag;
} bt_sink_srv_aws_mce_call_context_t;

typedef struct {
    uint8_t packet_type;
    bt_sink_srv_action_t action;
    uint8_t action_param[1];
} bt_sink_srv_aws_mce_call_action_t;
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
typedef struct {
    uint8_t  packet_type;
    bool     switch_value;
} bt_sink_srv_aws_mce_call_hf_switch_t;
#endif

bt_status_t bt_sink_srv_aws_mce_call_action_handler(bt_sink_srv_action_t action, void *param);
bt_sink_srv_aws_mce_call_state_t bt_sink_srv_aws_mce_call_transfer_hf_call_state(bt_sink_srv_state_t hf_call);
void bt_sink_srv_aws_mce_call_send_call_info(bt_bd_addr_t *remote_address, bt_sink_srv_aws_mce_call_update_info_t* call_info);
bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_sco_state(bt_sink_srv_aws_mce_sco_state state);
uint32_t bt_sink_srv_aws_mce_call_get_speaker_volume(bt_sink_srv_aws_mce_call_context_t *context);
bt_sink_srv_aws_mce_call_context_t *bt_sink_srv_aws_mce_call_get_context_by_address(const bt_bd_addr_t *address);
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
bt_status_t bt_sink_srv_aws_mce_call_sync_hf_switch(bool value);
#endif
#ifdef BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT
void bt_sink_srv_aws_mce_call_update_agent(void);
void bt_sink_srv_aws_mce_call_update_partner(void);
#endif /* BT_SINK_SRV_AWS_MCE_CALL_IN_MULTIPOINT */

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_AWS_MCE_CALL_H__ */
