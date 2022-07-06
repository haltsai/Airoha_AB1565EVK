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

#ifndef __AUDIO_SINK_SRV_LINE_IN_H__
#define __AUDIO_SINK_SRV_LINE_IN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "hal_gpt.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_ami.h"
#include "audio_log.h"

#ifndef MAXIMUM
#define     MAXIMUM(A, B)                  (((A)>(B))?(A):(B))
#define     MINIMUM(A, B)                  (((A)<(B))?(A):(B))
#endif

#define AUDIO_SINK_SRV_SET_FLAG(FLAG, MASK) do { \
    (FLAG) |= (MASK); \
} while(0);

#define AUDIO_SINK_SRV_REMOVE_FLAG(FLAG, MASK) do { \
    (FLAG) &= ~(MASK); \
} while(0);

#define AUDIO_SINK_SRV_INVALID_AID                 (-1)

typedef audio_src_srv_state_t audio_sink_srv_line_in_state_t;

typedef enum {
    AUDIO_SINK_SRV_LINE_IN_STATUS_SUCCESS        =     0,    /**< The sink service status: success. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_FAIL           =    -1,    /**< The sink service status: fail. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_PENDING        =    -2,    /**< The sink service status: operation is pending. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_INVALID_PARAM  =    -3,    /**< The sink service status: invalid parameters. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_DB_NOT_FOUND   =    -4,    /**< The sink service status: database is not found. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_EVENT_STOP     =    -5,    /**< The sink service status: event stop looping. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_NO_REQUEST     =    -6,    /**< The sink service status: no request is found. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_LINK_EXIST     =    -7,    /**< The sink service status: link is already existed. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_MAX_LINK       =    -8,    /**< The sink service status: reach the max link number. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_NEED_RETRY     =    -9,    /**< The sink service status: the request need to be retried. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_REQUEST_EXIST  =    -10,   /**< The sink service status: the request is already existed. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_INVALID_STATUS =    -11,   /**< The sink service status: invalid status. */
    AUDIO_SINK_SRV_LINE_IN_STATUS_USER_CANCEL    =    -12    /**< The sink service status: user cancel the action. */
} audio_sink_srv_line_in_status_t;

typedef enum {
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_NONE = 0,
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_WAIT_CONN,
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_WAIT_DISCONN,
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_CODEC,
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_PREPARE_BUFFER,
    AUDIO_SINK_SRV_LINE_IN_TRANSIENT_STATE_CLEAR_CODEC,
} audio_sink_srv_line_in_transient_state_t;

/**< Deveice flag */
/* Stable flag */
typedef enum {
    AUDIO_SINK_SRV_FLAG_LINE_IN_INIT           =     (1 << 0),
    AUDIO_SINK_SRV_FLAG_LINE_IN_PLAY           =     (1 << 1),
    AUDIO_SINK_SRV_FLAG_LINE_IN_PLAYING        =     (1 << 2),
    AUDIO_SINK_SRV_FLAG_LINE_IN_INTERRUPT      =     (1 << 3),
    AUDIO_SINK_SRV_FLAG_LINE_IN_RECOVER        =     (1 << 4),
    AUDIO_SINK_SRV_FLAG_WAIT_DRV_PLAY          =     (1 << 5),
    AUDIO_SINK_SRV_FLAG_WAIT_DRV_STOP          =     (1 << 6),
    AUDIO_SINK_SRV_FLAG_WAIT_AMI_OPEN_CODEC    =     (1 << 7),
    AUDIO_SINK_SRV_FLAG_WAIT_LIST_SINK_PLAY    =     (1 << 8),
    AUDIO_SINK_SRV_FLAG_WAIT_SET_VOLUME        =     (1 << 9),
} audio_sink_srv_line_in_flag_t;

typedef enum {
    AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_IN       =     (0),
    AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_OUT      =     (1),
    AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_START        =     (2),
    AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_STOP         =     (3),
    AUDIO_SINK_SRV_LINE_IN_ACT_PAUSE                =     (4),
    AUDIO_SINK_SRV_LINE_IN_ACT_VOLUME_UP            =     (5),
    AUDIO_SINK_SRV_LINE_IN_ACT_VOLUME_DOWN          =     (6),
} audio_sink_srv_line_in_action_t;

/**< Operation flag */
typedef enum {
    AUDIO_SINK_SRV_LINE_IN_OP_CODEC_OPEN            =     (1 << 0),
    AUDIO_SINK_SRV_LINE_IN_OP_DRV_PLAY              =     (1 << 1),
    AUDIO_SINK_SRV_LINE_IN_OP_PLAY_IND              =     (1 << 2),
    AUDIO_SINK_SRV_LINE_IN_OP_PLAY_TRIGGER          =     (1 << 3),
    AUDIO_SINK_SRV_LINE_IN_OP_HF_INTERRUPT          =     (1 << 4),
} audio_sink_srv_line_in_op_t;

typedef enum {
    AUDIO_SINK_SRV_LINE_IN_EVT_UNAVAILABLE = 0,
    AUDIO_SINK_SRV_LINE_IN_EVT_READY,
    AUDIO_SINK_SRV_LINE_IN_EVT_START,
    AUDIO_SINK_SRV_LINE_IN_EVT_STOP,
    AUDIO_SINK_SRV_LINE_IN_EVT_PLAYING,
    AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_OPEN,
    AUDIO_SINK_SRV_LINE_IN_EVT_SUSPEND,
    AUDIO_SINK_SRV_LINE_IN_EVT_REJECT,
    AUDIO_SINK_SRV_LINE_IN_EVT_RECOVER,
    AUDIO_SINK_SRV_LINE_IN_EVT_RESUME,
    AUDIO_SINK_SRV_LINE_IN_EVT_PREPARE_FAIL,
    AUDIO_SINK_SRV_LINE_IN_EVT_CODEC_CLEAR,
} audio_sink_srv_line_in_event_t;

typedef enum {
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_7TH          =     (7),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_6TH          =     (6),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_5TH          =     (5),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_4TH          =     (4),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_3RD          =     (3),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_2ND          =     (2),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_1ST          =     (1),
    AUDIO_SINK_SRV_LINE_IN_ERR_SUCCESS_OK           =     (0),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_1ST             =     (-1),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_2ND             =     (-2),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_3RD             =     (-3),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_4TH             =     (-4),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_5TH             =     (-5),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_6TH             =     (-6),
    AUDIO_SINK_SRV_LINE_IN_ERR_FAIL_7TH             =     (-7),
} audio_sink_srv_line_in_err_t;

typedef void(* audio_sink_srv_line_in_callback_t)(uint32_t evt_id, void *param, void *user_data);

typedef struct {
    audio_sink_srv_line_in_state_t state;
    audio_sink_srv_line_in_state_t target_state;

    uint32_t flag;                                          /**< Device flag */
    uint32_t op;                                            /**< Operation flag */
    audio_src_srv_handle_t *handle;                         /**< Pseudo device handle */
    audio_sink_srv_am_line_in_codec_t codec;

    bt_sink_srv_am_media_handle_t med_handle;
} audio_sink_srv_line_in_device_t;

typedef struct {
    audio_sink_srv_line_in_state_t state;
    audio_sink_srv_line_in_device_t *run_dev;
    audio_sink_srv_line_in_device_t sink_dev;

    /* Audio manager ID */
    int8_t line_in_aid;
    /* volume level */
    uint8_t vol_lev;
} audio_sink_srv_line_in_context_t;

/**
 * @brief     This function is to control line_in handler.
 *
 * @param[in] action      APP user action: For now only support
 *                       "DEVICE_PLUG_IN", "DEVICE_PLUG_OUT", "TRIGGER_START", "TRIGGER_STOP".
 * @param[in] *param      Not use, assign NULL.
 * @return     AUDIO_SINK_SRV_LINE_IN_STATUS_SUCCESS on success
 * @sa         #audio_sink_srv_line_in_control_action_handler()
 */
audio_sink_srv_line_in_status_t audio_sink_srv_line_in_control_action_handler(audio_sink_srv_line_in_action_t action, void *param);

/**
 * @brief     This function is to set Line-IN parameters.
 *
 * @param[in] *line_in_cap    For setting line-in parameters including:
 *                            "linein_sample_rate", "in_audio_device", "out_audio_device".
 * @return     AUDIO_SINK_SRV_LINE_IN_STATUS_SUCCESS on success
 * @sa         #audio_sink_srv_line_in_set_param()
 */
audio_sink_srv_line_in_status_t audio_sink_srv_line_in_set_param(audio_sink_srv_am_line_in_codec_t *line_in_cap);

#ifdef __cplusplus
}
#endif

#endif

