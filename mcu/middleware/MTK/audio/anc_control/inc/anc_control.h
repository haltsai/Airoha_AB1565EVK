/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#ifndef __ANC_CONTROL_H__
#define __ANC_CONTROL_H__

#ifdef MTK_ANC_ENABLE
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hal_audio.h"
#include "anc_control_api.h"

#include "bt_sink_srv.h"



#define PACKED  __attribute__((packed))

#ifdef __cplusplus
extern "C" {
#endif

#define ANC_SYNC_LATENCY        (500)
#define ANC_SYNC_LATENCY_UNDER_SPECIAL_LINK (60)
#ifdef MTK_POST_PEQ_DEFAULT_ON
#define ANC_STOP_LATENCY        (80)  //for A2DP post PEQ
#else
#define ANC_STOP_LATENCY        (0)
#endif

#define ANC_FILTER_TYPE_MASK    (0xF)
#define ANC_FF_ONLY_BIT_MASK    (1 << 7)
#define ANC_FB_ONLY_BIT_MASK    (1 << 6)
#define MACRO_ANC_CHK_PASSTHRU(x) ((((x) & ANC_FILTER_TYPE_MASK) >= FILTER_PASSTHRU_BEGIN) ? true : false)

#define ANC_UNASSIGNED_GAIN     (0x7FFF)
#define ANC_MAX_CALLBACK_NUM    (4)


#define TO_BOTH     (0)
#define TO_AGENT    (1)
#define TO_PARTNER  (2)
typedef uint8_t anc_to_role_t;

#define CB_LEVEL_ALL             (0)
#define CB_LEVEL_SUCCESS_ONLY    (1)
typedef uint8_t anc_callback_level_t;

typedef enum {
    ANC_DIRECT = 0,
    ANC_SYNC,
} anc_setting_mode_t;

typedef enum {
    FILTER_1 = 1,
    FILTER_2,
    FILTER_3,
    FILTER_4,
    FILTER_PASSTHRU_BEGIN,
    FILTER_5 = FILTER_PASSTHRU_BEGIN,
    FILTER_6,
    FILTER_7,

    FF_ONLY_FILTER_1 = ANC_FF_ONLY_BIT_MASK | FILTER_1,
    FF_ONLY_FILTER_2,
    FF_ONLY_FILTER_3,
    FF_ONLY_FILTER_4,

    FB_ONLY_FILTER_1 = ANC_FB_ONLY_BIT_MASK | FILTER_1,
    FB_ONLY_FILTER_2,
    FB_ONLY_FILTER_3,
    FB_ONLY_FILTER_4,
} anc_filter_type_t;

typedef enum {
    ANC_CONTROL_EVENT_ON                = 1 << 0,
    ANC_CONTROL_EVENT_OFF               = 1 << 1,
    ANC_CONTROL_EVENT_FORCE_OFF         = 1 << 2,
    ANC_CONTROL_EVENT_SET_VOLUME        = 1 << 3,
    ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME= 1 << 4,
    ANC_CONTROL_EVENT_READ_VOLUME_NVDM  = 1 << 5,
    ANC_CONTROL_EVENT_WRITE_VOLUME_NVDM = 1 << 6,
    ANC_CONTROL_EVENT_SET_ATTACH_FUNC   = 1 << 7,
    ANC_CONTROL_EVENT_SET_SYNC_TIME     = 1 << 8,
    ANC_CONTROL_EVENT_HOWLING           = 1 << 9,
    ANC_CONTROL_EVENT_LD_START          = 1 << 10,
    ANC_CONTROL_EVENT_LD_STOP           = 1 << 11,
    ANC_CONTROL_EVENT_LD_DL_RESUME      = 1 << 12,
} anc_control_event_t;

typedef enum {
    ANC_CONFIG_RAMP_DLY = 1,
    ANC_CONFIG_ADC_SWAP,
    ANC_CONFIG_RAMP_STEP,
    ANC_CONFIG_RAMP_CONFIG_GAIN,
    ANC_CONFIG_PWR_DET_ENABLE,
    ANC_CONFIG_SMOOTH,
    ANC_CONFIG_HIGH_THRESHOLD_IN_DBFS,
    ANC_CONFIG_LOWER_BOUND,
    ANC_CONFIG_RAMP_UP_DLY,
    ANC_CONFIG_RAMP_UP_STEP,
    ANC_CONFIG_RAMP_DOWN_DLY,
    ANC_CONFIG_RAMP_DOWN_STEP,
    ANC_CONFIG_OVF_CHECK_ENABLE,
    ANC_CONFIG_OVF_THRESHOLD,
    ANC_CONFIG_RESTORE_NVDM_PARAM,
    ANC_CONFIG_REDUCE_GAIN_STEP,
} anc_config_cmd_t;

#define ANC_RAMP_DLY_16US   (0)
#define ANC_RAMP_DLY_32US   (1)
#define ANC_RAMP_DLY_64US   (2)
#define ANC_RAMP_DLY_128US  (3)
typedef uint8_t anc_ramp_dly_t;

#define ANC_RAMP_STEP_00125DB   (0)
#define ANC_RAMP_STEP_0025DB    (1)
#define ANC_RAMP_STEP_005DB     (2)
#define ANC_RAMP_STEP_01DB      (3)
#define ANC_RAMP_STEP_02DB      (4)
#define ANC_RAMP_STEP_04DB      (5)
#define ANC_RAMP_STEP_08DB      (6)
#define ANC_RAMP_STEP_16DB      (7)
#define ANC_RAMP_STEP_32DB      (8)
#define ANC_RAMP_STEP_64DB      (9)
typedef uint8_t anc_ramp_step_t;


typedef void (*anc_control_callback_t)(anc_control_event_t event_id, anc_control_result_t result);

typedef struct anc_fwd_iir_s {
    uint32_t  iir_filter_coef[64];       // [b,a] format
    uint16_t  iir_filter_coef_B00;
    uint8_t   shift_a;
    uint8_t   shift_b;
    int16_t   filter_unique_gain;
} PACKED anc_fwd_iir_t;

typedef struct anc_hybrid_cap_s {
    uint16_t chip_id;
    uint8_t efuse_dis_hybrid;
    uint8_t proj_dis_hybrid;
} anc_hybrid_cap_t;

typedef struct anc_sw_gain_s {
    union {
        struct {
            short  gain_index_l;
            short  gain_index_r;
        };
        uint32_t val;
    };
} anc_sw_gain_t;

typedef struct {
    anc_ramp_dly_t  ramp_dly_sel;           //ramp switch delay when enable/disable anc or pass-through
    uint8_t         adc_swap;               //adc channel swap or not in anc hw module.
    anc_ramp_step_t ramp_step;              //ramp gain step when enable/disable anc or pass-through
    int16_t         ramp_config_gain;       //default gain for ramp gain
    uint8_t         reserved0;

    /* for anc howling detection */
    uint8_t         power_detect_enable;    //for enable power detector or not.
    uint8_t         detect_smooth;          //for power calculate smooth when detect
    uint8_t         attack_smooth;          //for power calculate smooth when attack (reserved)
    uint8_t         release_smooth;         //for power calculate smooth when release (reserved)
    int16_t         low_threshold_in_dbfs;  //the default value for power detect low level (reserved)
    int16_t         high_threshold_in_dbfs; //the default value for power detect high level
    int16_t         lower_bound_in_dbfs;    //the maxmum gain attacked for howling
    anc_ramp_dly_t  ramp_up_dly_sel;        //ramp switch delay when release gain
    anc_ramp_step_t ramp_up_step;           //ramp gain step when release gain
    anc_ramp_dly_t  ramp_down_dly_sel;      //ramp switch delay when attack gain
    anc_ramp_step_t ramp_down_step;         //ramp gain step when release gain
    uint16_t        reduce_gain_step;       //reduce sw gain how many dBFS every time to reduce sw gain for howling
    uint16_t        enlarge_gain_step;      //release sw gain how many dBFS every time to release sw gain for howling recovery
    uint32_t        check_irq_period_in_us; //release sw gain once every 100ms if there is no howling interrupts come

    /* for anc overflow */
    uint8_t         reserved3;
    uint8_t         overflow_check_enable;  //for enable overflow chek or not
    int16_t         overflow_threshold;     //for overflow threshold offset of 0dBFS
    uint32_t        reset_delay_in_us;      //after how long to re-enable ANC back

    uint8_t         howling_tolerance;      //trigger howling event to turn off ANC after howling xx sec.
} PACKED anc_nvdm_param_t;

typedef struct {
    uint8_t enable;
    uint8_t suspend;
    uint8_t backup_enable;
    //anc_suspend_t suspend;      //to indicate current state and original enable state.
    uint8_t attach_enable;      //to enable anc attach function for partner side.
    uint16_t sync_time;         //the duration that agent/partner wait for anc/pass-thru on/off/set vol synchronization.
    bt_clock_t sync_time_bt_clock;//the Bt clock of trigger that agent/partner wait for anc/pass-thru on/off/set vol synchronization.

    /* for anc sync timer */
    anc_control_event_t     event;
    union {
        anc_filter_type_t   filter_type;        //for ANC_CONTROL_EVENT_ON, hide runtime_gain param inside.
        anc_sw_gain_t       sw_gain;            //for ANC_CONTROL_EVENT_SET_VOLUME
        int16_t             user_runtime_gain;  //for ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME
        uint32_t            val;
    } arg;
    anc_control_callback_t  callback;
    uint8_t timer_available;

    /* for async anc stop ack */
    uint8_t waiting_stop_ack;
    anc_control_callback_t  anc_off_callback;

    /* for dsp massage u32data */
    uint32_t dsp_message_cb_data;

    /* for queue anc on request*/
    anc_filter_type_t request_filter_type;
    int16_t request_runtime_gain;
    anc_control_callback_t  anc_on_callback;

    /* for user run-time parameters */
    anc_filter_type_t cur_filter_type;
    int16_t anc_runtime_gain;
    int16_t passthru_runtime_gain;
    uint8_t hybrid_enable;

    /* for anc howling detection */
    anc_nvdm_param_t nvdm_param;

    /* for anc stop delay timer */
    uint8_t stop_dly_timer_available;

#ifdef MTK_DEQ_ENABLE
    /* for deq */
    uint8_t deq_enable;
    uint8_t deq_misc_param;
    uint8_t deq_delay_samples;
    int16_t deq_digital_gain;
#endif
} anc_control_t;

typedef struct {
    anc_control_callback_t callback_list[ANC_MAX_CALLBACK_NUM];
    anc_control_event_t event_mask[ANC_MAX_CALLBACK_NUM];
    anc_callback_level_t cb_level[ANC_MAX_CALLBACK_NUM];
} anc_callback_service_t;

typedef struct {
    uint8_t anc_enable;
    uint8_t anc_filter_type;
    int16_t anc_runtime_gain;
    int16_t passthru_runtime_gain;
    uint8_t anc_suspend;
} anc_nvdm_misc_param_t;


typedef struct aws_mce_report_anc_param_s
{
    uint32_t event_id;
    uint32_t arg;
    uint32_t ch_info;

    bt_clock_t sync_bt_clock;
    bool     is_sync;
} aws_mce_report_anc_param_t;

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#define USER_TRIGGER_FF_LATENCY        (1000)
#define USER_TRIGGER_FF_REPORT_LENGTH  (301)

#define ANC_K_STATUS_RUNNING 2
#define ANC_K_STATUS_DONE 3
#define ANC_K_STATUS_ABORT 4
#define ANC_K_STATUS_SMALL_VOLUME 5
#define ANC_K_STATUS_Compare 6
#define ANC_K_STATUS_KEEP_DEFAULT 7
#define ANC_K_STATUS_UPDATE 8


typedef void (*anc_user_trigger_ff_callback_t)(int32_t Cal_status);

typedef struct {
    anc_user_trigger_ff_callback_t api_callback;
    uint8_t enable;
    uint8_t anc_enable;
    uint16_t dsp_msg_cb_data;
    uint32_t anc_runtime_info;
    int32_t report_array[USER_TRIGGER_FF_REPORT_LENGTH];
    anc_fwd_iir_t *cmp_ori_filter;
    anc_fwd_iir_t *cmp_new_filter;
} anc_user_trigger_ff_param_t;

typedef struct {
    int32_t Sz_length;
    int32_t utff_sz_coef_array_B[101];
    int32_t utff_sz_coef_array_A[101];
}PACKED anc_user_trigger_adaptive_ff_sz_coef_nvdm_t;

typedef struct {
    int16_t FF_Noise_thd;
    int16_t FB_Noise_thd;
    int16_t stepsize;
    int16_t Shaping_switch;
    uint16_t time_end;
} PACKED anc_user_trigger_adaptive_ff_param_nvdm_t;
#endif
#endif




void at_anc_callback(anc_control_event_t event_id, anc_control_result_t result);
void anc_save_misc_param(void);
int32_t anc_on(anc_filter_type_t anc_filter_type, int16_t anc_runtime_gain, anc_control_callback_t callback, uint8_t from_nvdm);
void anc_dsp_message_callback(hal_audio_event_t event, void *user_data);
int32_t anc_off(anc_control_callback_t callback, anc_filter_type_t request_filter_type, uint8_t to_nvdm);
int32_t anc_set_volume(anc_sw_gain_t *anc_sw_gain);
int32_t anc_set_runtime_volume(int16_t anc_runtime_gain);
int32_t anc_read_volume_nvdm(anc_sw_gain_t *anc_sw_gain);
int32_t anc_write_volume_nvdm(anc_sw_gain_t *anc_sw_gain);
int32_t anc_write_filter_volume_nvdm(anc_filter_type_t anc_filter_type, int16_t fiter_uniqu_gain);
uint32_t anc_get_hybrid_capability(anc_hybrid_cap_t *hybrid_cap);
void anc_get_status(uint8_t *enable, uint32_t *runtime_info, uint8_t *hybrid_enable);
int16_t anc_get_runtime_gain(uint8_t get_anc_gain);
void anc_get_attach_enable(uint8_t *enable);
void anc_set_attach_enable(uint8_t enable);
void anc_get_backup_status(uint8_t *enable);
#ifdef MTK_DEQ_ENABLE
uint32_t anc_get_deq_param(void);
void anc_set_deq_param(uint8_t delay, int16_t gain, uint8_t phase_inverse);
uint8_t anc_get_deq_enable(void);
void anc_set_deq_enable(uint8_t deq_enable);
#endif
#ifdef MTK_AWS_MCE_ENABLE
anc_control_result_t anc_sync_control(anc_control_event_t event, uint32_t arg, anc_control_callback_t callback);
#endif
uint16_t anc_get_sync_time(void);
void anc_set_sync_time(uint16_t sync_time);
void anc_config(anc_config_cmd_t cmd, uint32_t *arg);
void anc_init(void);
anc_control_result_t anc_send_aws_mce_param(anc_setting_mode_t setting_mode, anc_control_event_t event_id, uint32_t arg, uint32_t ch_info);
anc_control_result_t audio_anc_enable(anc_filter_type_t anc_filter_type, int16_t anc_runtime_gain, anc_control_callback_t callback);
anc_control_result_t audio_anc_disable(anc_control_callback_t callback);
anc_control_result_t audio_anc_suspend(anc_control_callback_t callback, bool power_off);
anc_control_result_t audio_anc_set_volume(anc_sw_gain_t anc_sw_gain, anc_to_role_t to_role);
anc_control_result_t audio_anc_set_runtime_volume(int16_t anc_runtime_gain);
anc_control_result_t audio_anc_read_volume_nvdm(anc_sw_gain_t *anc_sw_gain, anc_to_role_t to_role);
anc_control_result_t audio_anc_write_volume_nvdm(anc_sw_gain_t *anc_sw_gain, anc_to_role_t to_role);
anc_control_result_t audio_anc_resume(anc_control_callback_t callback, bool power_on);
anc_control_result_t audio_anc_control(anc_control_event_t event_id, uint32_t arg, anc_control_callback_t callback);
anc_control_result_t audio_anc_register_callback(anc_control_callback_t callback, anc_control_event_t event_mask, anc_callback_level_t level);
anc_control_result_t audio_anc_deregister_callback(anc_control_callback_t callback);
anc_control_result_t audio_anc_backup_while_suspend(uint8_t anc_enable, uint8_t anc_filter_type, int16_t anc_runtime_gain);
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
void audio_anc_user_trigger_ff_CCNI_demo_callback(hal_audio_event_t event, void *data);
int32_t audio_anc_user_trigger_ff_write_filter_nvdm(anc_filter_type_t anc_filter_type, anc_fwd_iir_t *filter);
int32_t audio_anc_user_trigger_ff_read_filter_nvdm(anc_filter_type_t anc_filter_type);
int32_t audio_anc_user_trigger_ff_read_Sz_coef_nvdm(void);
void audio_anc_user_trigger_ff_write_Sz_coef_share_buff(void);
void audio_anc_user_trigger_ff_cb(int32_t Cal_status);
void audio_anc_user_trigger_ff_start(anc_user_trigger_ff_callback_t callback);
void audio_anc_user_trigger_ff_stop(void);
void audio_anc_user_trigger_ff_receive_filter(anc_user_trigger_ff_callback_t callback, uint8_t *filter_ptr, uint32_t filter_length);
bool audio_anc_user_trigger_ff_get_status(void);
void audio_anc_user_trigger_ff_recover_anc(int32_t Cal_status);
void audio_anc_user_trigger_ff_terminate(void);
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif  /*MTK_ANC_ENABLE*/
#endif  /*__ANC_CONTROL_H__*/
