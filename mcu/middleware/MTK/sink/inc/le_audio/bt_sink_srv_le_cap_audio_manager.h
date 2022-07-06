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


#ifndef __BT_SINK_SRV_CAP_AUDIO_MANAGER_H__
#define __BT_SINK_SRV_CAP_AUDIO_MANAGER_H__

#include "bt_sink_srv_ami.h"

enum {
    CODEC_LC3,  /**< LC3 codec. */
    CODEC_NUM   /**< Maximum number of codec. */
};

enum {
    CHANNEL_MODE_DL_ONLY,       /**< Down link only. */
    CHANNEL_MODE_DL_UL_BOTH,    /**< Down link and up link both. */
    CHANNEL_MODE_UL_ONLY,       /**< Up link only. */
    CHANNEL_MODE_NUM            /**< Maximum number of channel mode. */
};

enum {
    CHANNEL_MONO,               /**< Mono. */
    CHANNEL_STEREO,             /**< Stereo. */
    CHANNEL_DUAL_MONO,        /**< Dual Mono, Major share info denote L channle. */
    CHANNEL_NUM                 /**< Maximum number of channel type. */
};

enum {
    SAMPLING_RATE_16K,          /**< 16 kHz. */
    SAMPLING_RATE_24K,          /**< 24 kHz. */
    SAMPLING_RATE_32K,          /**< 32 kHz. */
    SAMPLING_RATE_44_1K,        /**< 44100 Hz. */
    SAMPLING_RATE_48K,          /**< 48 kHz. */
    SAMPLING_RATE_NUM           /**< Maximum number of sampling rate. */
};

enum {
    FRAME_MS_7P5,   /**< use 7.5 ms codec frames. */
    FRAME_MS_10,    /**< use 10 ms codec frames. */
    FRAME_MS_NUM    /**< Maximum number of frame duration. */
};


enum
{
    CAP_AM_SUB_STATE_IDLE,
    CAP_AM_SUB_STATE_UNICAST_MUSIC_WAITING,         /*Stopping BIS, then play unicast music*/
    CAP_AM_SUB_STATE_UNICAST_CALL_WAITING,          /*Stopping BIS, then accept call*/
    CAP_AM_SUB_STATE_UNICAST_STREAM_PREPARING,      /*Preparing CIS*/
    CAP_AM_SUB_STATE_UNICAST_STREAMING,
    CAP_AM_SUB_STATE_BROADCAST_MUSIC_WAITING,       /*Stopping CIS, then play broadcast music*/
    CAP_AM_SUB_STATE_BROADCAST_STREAM_PREPARING,    /*Preparing BIS*/
    CAP_AM_SUB_STATE_BROADCAST_STREAMING,
};

typedef enum
{
    CAP_AM_UNICAST_CALL_MODE_0,
    CAP_AM_UNICAST_CALL_MODE_1,
    CAP_AM_UNICAST_MUSIC_MODE_0,
    CAP_AM_UNICAST_MUSIC_MODE_1,
    CAP_AM_BROADCAST_MUSIC_MODE,
    CAP_AM_MODE_NUM,
} bt_sink_srv_cap_am_mode;

/*typedef enum
{
    CAP_AM_DEINIT_REASON_CIS_DISCONNECT,
    CAP_AM_DEINIT_REASON_BIS_TERMINATE,
    CAP_AM_DEINIT_REASON_LE_DISCONNECT,
    CAP_AM_DEINIT_REASON_POWER_OFF,
}bt_sink_srv_cap_am_deinit_reason;*/

typedef enum
{
    UNICAST_MUSIC_MODE = 1,
    UNICAST_CALL_MODE,
    BROADCAST_MUSIC_MODE,
}bt_sink_srv_cap_streaming_mode;

/**
 * @brief                       This function is a Sink Service Audio Manager initialization API.
 * @param[in] mode              is channel mode.
 * @return                      none
 */
void bt_sink_srv_cap_am_init(void);

/**
 * @brief                       This function is a Sink Service Audio Manager deinitialization API.
 * @return                      none
 */
void bt_sink_srv_cap_am_deinit(void);

/**
 * @brief                       This function request Audio Manager to start streaming.
 * @param[in] channel_mode      is channel mode.
 * @return                      none
 */
void bt_sink_srv_cap_am_audio_start(bt_sink_srv_cap_am_mode mode);

/**
 * @brief                       This function request Audio Manager to stop streaming.
 * @return                      none
 */
void bt_sink_srv_cap_am_audio_stop(bt_sink_srv_cap_am_mode mode);

/**
 * @brief                       This function request Audio Manager to suspend streaming.
 * @return                      none
 */
void bt_sink_srv_cap_am_audio_suspend(bt_sink_srv_cap_am_mode mode);
/**
 * @brief                       This function request Audio Manager to restart streaming.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_am_audio_restart(void);

/**
 * @brief                       This function get Audio ID from Audio Manager.
 * @param[in] mode              is streaming mode.
 * @return                      Audio ID.
 */
int8_t bt_sink_srv_cap_am_get_aid(void);

/**
 * @brief                       This function disables AM waiting list.
 * @return                      none.
 */
void bt_sink_srv_cap_am_disable_waiting_list(void);

/**
 * @brief                       This function enables AM waiting list.
 * @return                      none.
 */
void bt_sink_srv_cap_am_enable_waiting_list(void);

/**
 * @brief                       This function gets current streaming mode.
 * @return                      streaming mode.
 */
bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_current_mode(void);


/**
 * @brief                       This function gets restarting streaming mode.
 * @return                      streaming mode.
 */
bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_restarting_mode(void);


#endif

