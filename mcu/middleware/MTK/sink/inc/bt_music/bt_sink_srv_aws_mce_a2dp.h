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
 
#ifndef __BT_SINK_SRV_AWS_MCE_A2DP_H__
#define __BT_SINK_SRV_AWS_MCE_A2DP_H__

#include "bt_sink_srv_music.h"

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
#include <stdint.h>
#include "bt_a2dp.h"
#include "audio_src_srv.h"
#include "bt_sink_srv.h"


#if defined(__AFE_HS_DC_CALIBRATION__)
#define BT_SINK_SRV_AWS_A2DP_OUTPUT_DEVICE          (HAL_AUDIO_DEVICE_HANDSET)
#else
#define BT_SINK_SRV_AWS_A2DP_OUTPUT_DEVICE          (HAL_AUDIO_DEVICE_HEADSET)
#endif

#define BT_SINK_SRV_AWS_A2DP_VOL_DEF_LEV            (AUD_VOL_OUT_LEVEL6)

#define BT_SINK_SRV_AWS_A2DP_MAGIC_CODE             (0xa0)

#define BT_SINK_SRV_AWS_A2DP_VOL_CHANGE_UP          (0xFF)
#define BT_SINK_SRV_AWS_A2DP_VOL_CHANGE_DOWN        (0xFE)


#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_CONNECTION                (1 << 0)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_DISCONNECTION             (1 << 1)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_START_STREAMING           (1 << 2)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_SUSPEND_STREAMING         (1 << 3)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_UPDATE_BASE               (1 << 4)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_SYNC                  (1 << 5)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_PLAY_IND                  (1 << 6)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_UP                    (1 << 7)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_DOWN                  (1 << 8)
#define BT_SINK_SRV_AWS_MCE_A2DP_EVT_REINIT                    (1 << 9)


#define BT_SINK_SRV_AWS_MCE_A2DP_MAGIC_CODE_1                  (0xbaba)   /**< A2DP connection */

#define BT_SINK_SRV_AWS_MCE_A2DP_MAGIC_CODE_2                  (0xbabb)   /**< A2DP suspend */

#define BT_SINK_SRV_AWS_MCE_A2DP_MAGIC_CODE_3                  (0xbabc)   /**< A2DP start */

#define BT_SINK_SRV_AWS_MCE_A2DP_MAGIC_CODE_4                  (0xbabe)   /**< Sync volume*/


#define BT_SINK_SRV_AWS_MCE_A2DP_FEC_LOOP_COUNT                (100)

/*mce a2dp flag*/
#define BT_SINK_SRV_AWS_MCE_A2DP_REINIT_FLAG                    (1 << 0)
#define BT_SINK_SRV_AWS_MCE_A2DP_ALC_FLAG                       (1 << 1)

/**
 * @brief This structure defines sink a2dp state.
 */
typedef enum {
    BT_SINK_SRV_A2DP_STATE_NONE,
    BT_SINK_SRV_A2DP_STATE_READY,
    BT_SINK_SRV_A2DP_STATE_PLAY,
    BT_SINK_SRV_A2DP_UP_BASE = 0XFF,
} bt_sink_srv_a2dp_state_t;


typedef struct {
    uint32_t handle;
    uint8_t *data;
    uint16_t length;
} bt_sink_srv_aws_mce_a2dp_if_data_ind_t;

typedef struct {
    uint8_t action_type;
    uint8_t action_length;
    uint8_t action_value;
    uint8_t data[1];
} bt_sink_srv_aws_mce_a2dp_action_t;

typedef struct {
    bt_sink_srv_action_t avrcp_action;
    bt_sink_srv_avrcp_operation_state_t operation_state;
} bt_sink_srv_aws_mce_avrcp_pass_through_action_t;

#ifdef __cplusplus
extern "C" {
#endif

bt_status_t bt_sink_srv_aws_mce_a2dp_action_handler(bt_sink_srv_action_t action, void *param);

bt_status_t bt_sink_srv_aws_mce_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

void bt_sink_srv_aws_mce_a2dp_init();

void bt_sink_srv_aws_mce_a2dp_play(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_mce_a2dp_stop(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_mce_a2dp_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);

void bt_sink_srv_aws_mce_a2dp_reject(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_mce_a2dp_exception(audio_src_srv_handle_t *handle, int32_t event, void *param);

void bt_sink_srv_aws_mce_a2dp_create_pse_handle(void);

void bt_sink_srv_aws_mce_a2dp_destroy_pse_handle(void);

void bt_sink_srv_aws_mce_a2dp_drv_play(void *param);

bt_status_t bt_sink_srv_music_aws_mce_a2dp_action_handler(bt_sink_srv_action_t action, void *param);

void bt_sink_srv_aws_mce_a2dp_send_eir(uint32_t event, void *param);

bt_status_t bt_sink_srv_music_aws_a2dp_action_handler(bt_sink_srv_action_t action, void 
*param);

void bt_sink_srv_aws_mce_trigger_agent_reinitial_sync(uint8_t reinit_type);

void bt_sink_srv_aws_mce_a2dp_reset_n_flag(void);

#ifdef __cplusplus
}
#endif

#endif /* __BT_AWS_MCE_A2DP_SUPPORT__ */

#endif /* __BT_SINK_SRV_AWS_MCE_A2DP_H__ */

