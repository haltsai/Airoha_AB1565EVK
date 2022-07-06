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
#ifndef __LEAKAGE_DETECTION_CONTROL_H__
#define __LEAKAGE_DETECTION_CONTROL_H__

#include "syslog.h"
#ifdef MTK_ANC_V2
  #include "anc_control_api.h"
#else
  #include "anc_control.h"
#endif
#include "bt_sink_srv_ami.h"

#ifdef AIR_UL_FIX_SAMPLING_RATE_48K
#define LD_FRAME_SIZE    (720)
#else
#define LD_FRAME_SIZE    (240)
#endif

#ifndef MTK_ANC_V2
typedef anc_control_result_t audio_anc_control_result_t;
typedef anc_control_event_t  audio_anc_control_event_t;
#define AUDIO_ANC_CONTROL_EXECUTION_SUCCESS ANC_CONTROL_EXECUTION_SUCCESS
#define AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED ANC_CONTROL_EXECUTION_NOT_ALLOWED
#else
/** @brief The local device of the target role. */
#define TO_ITSELF    (1)
/** @brief The other device of the target role. */
#define TO_THEOTHER  (2)
/** @brief The both device of the target role. */
#define TO_BOTH      (3)

#define CB_LEVEL_ALL             (0)

#define LD_SYNC_LATENCY         (500)
#define LD_SYNC_LATENCY_UNDER_SPECIAL_LINK (60)
#define LD_MAX_CALLBACK_NUM    (3)
/** @brief The all level of callback service level. */
#define AUDIO_LEAKAGE_DETECTION_CONTROL_CB_LEVEL_ALL             (0)
/** @brief The success only level of callback service level. */
#define AUDIO_LEAKAGE_DETECTIONs_CONTROL_CB_LEVEL_SUCCESS_ONLY    (1)


#endif

#define LD_STATUS_PASS                 (1) //from lib
#define LD_STATUS_FAIL_CASE_1          (2) //from lib
#define LD_STATUS_FAIL_CASE_2          (3) //from lib
#define LD_STATUS_TIMEOUT              (4)
#define LD_STATUS_TERMINATE            (5) //terminated by higher priority job
#define LD_STATUS_ABSENT               (6) //for partner not attached to agent


/** @brief Leakage detection execution result. */
typedef enum {
    AUDIO_LEAKAGE_DETECTION_EXECUTION_NONE        = -2,  /**< The process result was in initail status.   */
    AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL        = -1,  /**< The process result was fail.   */
    AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS     =  0,  /**< The process result was successful.   */
    AUDIO_LEAKAGE_DETECTION_EXECUTION_CANCELLED   =  1,  /**< The process result was cancelled.   */
    AUDIO_LEAKAGE_DETECTION_EXECUTION_NOT_ALLOWED =  2,  /**< The process result was not allowed.   */
} audio_anc_leakage_detection_execution_t;

/** @brief Leakage detection control event values. */
typedef enum {
    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_RACE_CH_ID     = 1 << 0,  /**< The event to sync race ch id with partner.   */
    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE     = 1 << 1,
    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START          = 1 << 2,
    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP           = 1 << 3,
    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE   = 1 << 4,
} audio_anc_leakage_detection_control_event_t;

typedef enum {
    LD_DIRECT = 0,
    LD_SYNC,
} audio_anc_leakage_detection_sync_mode_t;

/** @brief whole settings structure of the sync control. */
typedef struct audio_anc_leakage_detection_control_sync_para_s {
    uint8_t target_device;       /**< Target device in anc control flow.   */
    uint32_t extend_param;                /**< Extend param */
} audio_anc_leakage_detection_sync_para_t;

typedef struct {
    uint16_t   sync_time;         //the duration that agent/partner wait for anc/pass-thru on/off/set vol synchronization.
    bt_clock_t sync_time_bt_clock;//the Bt clock of trigger that agent/partner wait for anc/pass-thru on/off/set vol synchronization.

    /* for anc sync timer */
    audio_anc_leakage_detection_control_event_t     event_id;
    uint8_t                                         timer_available;
    audio_anc_leakage_detection_sync_para_t         arg;
} audio_anc_leakage_detection_sync_ctrl_t;

typedef struct aws_mce_report_LD_param_s
{
    uint32_t                                event_id;
    uint32_t                                ch_info;
    bt_clock_t                              sync_bt_clock;
    bool                                    is_sync;
    audio_anc_leakage_detection_sync_para_t arg;
} aws_mce_report_LD_param_t;

typedef struct {
    S16 ld_thrd;
    U16 RXIN_TXREF_DELAY;
    S16 DIGITAL_GAIN;
} anc_leakage_compensation_parameters_nvdm_t;

typedef struct {
    anc_leakage_compensation_parameters_nvdm_t nvdm_param;
    uint32_t report_thd;
    uint32_t no_response_thd;
} anc_leakage_compensation_parameters_mcu2dsp_t;

typedef void (*anc_leakage_compensation_callback_t)(uint16_t leakage_status);
typedef void (*audio_anc_leakage_detection_control_callback_t)(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_execution_t result);
typedef uint8_t audio_anc_leakage_detection_control_callback_level_t;

#ifdef MTK_ANC_V2
typedef struct {
    audio_anc_leakage_detection_control_callback_t       callback_list[LD_MAX_CALLBACK_NUM];
    audio_anc_leakage_detection_control_event_t          event_mask[LD_MAX_CALLBACK_NUM];
    audio_anc_leakage_detection_control_callback_level_t cb_level[LD_MAX_CALLBACK_NUM];
} audio_anc_leakage_detection_callback_service_t;

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_control_register_callback(audio_anc_leakage_detection_control_callback_t callback, audio_anc_leakage_detection_control_event_t event_mask, audio_anc_leakage_detection_control_callback_level_t level);
#endif
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_send_aws_mce_param(audio_anc_leakage_detection_sync_mode_t setting_mode, audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg, uint32_t ch_info);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_control(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_sync_control(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg);
void audio_anc_set_leakage_compensation_parameters_nvdm(anc_leakage_compensation_parameters_nvdm_t *leakage_compensation_param_ptr);
int32_t audio_anc_read_leakage_compensation_parameters_nvdm(void);
void audio_anc_leakage_compensation_CCNI_callback(hal_audio_event_t event, void *data);
void audio_anc_leakage_compensation_AM_notify_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm);
void audio_anc_leakage_compensation_cb(uint16_t leakage_status);
void audio_anc_leakage_compensation_start(anc_leakage_compensation_callback_t callback);
void audio_anc_leakage_compensation_stop(void);
void audio_anc_leakage_compensation_set_status(bool status);
bool audio_anc_leakage_compensation_get_status(void);
void audio_anc_leakage_compensation_terminate(void);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_mute_music(bool is_mute);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_prepare(anc_leakage_compensation_callback_t callback);
void audio_anc_leakage_detection_init(void);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_start(anc_leakage_compensation_callback_t callback);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_stop(void);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_resume_dl(void);
void audio_anc_leakage_detection_register_vp_start_callback(anc_leakage_compensation_callback_t callback);
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_resume_anc(void);
void audio_anc_leakage_detection_send_aws_mce_race_ch_id(uint8_t race_ch_id);
void audio_anc_leakage_detection_set_mute_status(bool mute);
bool audio_anc_leakage_detection_get_mute_status(void);
void audio_anc_leakage_detection_set_duration(uint32_t report_thd, uint32_t no_response_thd);

#endif

