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


#ifndef __LE_SINK_SRV_VOLUME_H__
#define __LE_SINK_SRV_VOLUME_H__

#include "bt_type.h"

#define BT_SINK_SRV_LE_VCS_ACTION_RELATIVE_VOLUME_DOWN                    (0x0001U)    /**< This action sends a request to set relative volume down. */
#define BT_SINK_SRV_LE_VCS_ACTION_RELATIVE_VOLUME_UP                      (0x0002U)    /**< This action sends a request to set relative volume up. */
#define BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_DOWN             (0x0003U)    /**< This action sends a request to set unmute relative volume down. */
#define BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_UP               (0x0004U)    /**< This action sends a request to set unmute relative volume up. */
#define BT_SINK_SRV_LE_VCS_ACTION_SET_ABSOLUTE_VOLUME                     (0x0005U)    /**< This action sends a request to set absolute volume with parameter #bt_sink_srv_le_set_absolute_volume_t. */
#define BT_SINK_SRV_LE_VCS_ACTION_UNMUTE                                  (0x0006U)    /**< This action sends a request to set unmute. */
#define BT_SINK_SRV_LE_VCS_ACTION_MUTE                                    (0x0007U)    /**< This action sends a request to set mute. */
#define BT_SINK_SRV_LE_ACTION_INVALID                                     (0xFFFF)     /**< Invalid sink srv LE VCP action. */
typedef uint16_t bt_sink_srv_le_vcp_action_t; /**< The type of the sink srv LE VCP action. */

/**
 * @brief Defines the volume step.
 */
#define BT_SINK_LE_VOLUME_VALUE_STEP    0x11    /**< The volume value step. */

/**
 * @brief Defines the stream type.
 */
#define BT_SINK_SRV_LE_STREAM_TYPE_IN     0x01    /**< Stream type in. */
#define BT_SINK_SRV_LE_STREAM_TYPE_OUT    0x02    /**< Stream type out. */
typedef uint8_t bt_sink_srv_le_stream_type_t;     /**< The type of stream. */

typedef struct {
    uint8_t volume;     /**< Volume.*/
} bt_sink_srv_le_set_absolute_volume_t;

typedef struct {
    uint8_t volume;     /**< Volume.*/
    bool mute;          /**< Mute.*/
} bt_sink_srv_le_volume_state_t;

bt_status_t bt_sink_srv_le_volume_init(void);

bt_status_t bt_sink_srv_le_volume_vcp_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

bt_status_t bt_sink_srv_le_volume_vcp_send_action(bt_handle_t handle, bt_sink_srv_le_vcp_action_t action, void *params);

bt_status_t bt_sink_srv_le_volume_set_volume(bt_sink_srv_le_stream_type_t type, uint8_t volume);

bt_status_t bt_sink_srv_le_volume_set_mute(bt_sink_srv_le_stream_type_t type, bool mute);

bt_sink_srv_le_volume_state_t bt_sink_srv_le_volume_get_state(bt_handle_t handle);

#endif  /* __LE_SINK_SRV_VOLUME_H__ */

