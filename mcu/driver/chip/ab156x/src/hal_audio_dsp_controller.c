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

#include <string.h>
#include "hal_audio.h"
#include <assert.h>
#if defined(HAL_AUDIO_MODULE_ENABLED)

//==== Include header files ====
#include "memory_attribute.h"
#include "hal_resource_assignment.h"
#include "hal_ccni.h"
#include "hal_ccni_config.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#include "hal_rtc_internal.h"
#include "hal_hw_semaphore.h"
#include "hal_nvic.h"

#include "assert.h"
#include "hal_log.h"

#include "hal_gpt.h"
#include "hal_core_status.h"
#include "hal_clock.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu.h"
#endif
#include "hal_dvfs.h"
#if defined(HAL_SLEEP_MANAGER_ENABLED)
    #include "hal_core_status.h"
    #include "hal_spm.h"
    #include "memory_map.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "avm_external.h"
#endif

#include "exception_handler.h"
#define UNUSED(x)  ((void)(x))
#define HAL_AUDIO_DEBUG
//#define UNIT_TEST

//==== Static variables ====
ATTR_SHARE_ZIDATA static n9_dsp_share_info_t n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_MAX];
ATTR_SHARE_ZIDATA static audio_share_buffer_t audio_share_buffer;
#ifdef MTK_BT_SPEAKER_ENABLE
ATTR_SHARE_ZIDATA static uint8_t fec_share_buffer[SHARE_BUFFER_FEC_SIZE];
#endif

ATTR_SHARE_ZIDATA static audio_dsp_a2dp_dl_play_en_param_t audio_play_en_info;

ATTR_SHARE_ZIDATA static n9_dsp_share_info_t audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_MAX];

#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
//#define SHARE_BUFFER_ANC_MONITOR_SIZE 5 //UU gain info: int32_t *5
ATTR_SHARE_ZIDATA static int32_t g_ANC_monitor_share_buffer[SHARE_BUFFER_ANC_MONITOR_INFO_COUNT];
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
static leaudio_share_buffer_t leaudio_share_buffer;
#endif

typedef struct {
    uint32_t                           flag;
    hal_audio_callback_t               callback[AUDIO_MESSAGE_TYPE_MAX];
    void                               *user_data[AUDIO_MESSAGE_TYPE_MAX];
    hal_bt_audio_dl_open_callback_t    bt_audio_dl_open_callback;
    hal_audio_notify_task_callback_t   audio_event_task_callback;
    hal_audio_task_ms_delay_function_t task_ms_delay_func;
} audio_isr_t;

static audiosys_clkmux_control audio_clkmux_status[AUDIO_CLKMUX_BLOCK_NUM_OF_CLKMUX_BLOCK];

/** @brief Define hires clock frequency. */
typedef enum {
    DL_HIRES_96KHZ      = 0,
    DL_HIRES_192KHZ     = 1,
} hires_clock_frequency_dl_t;

typedef enum {
    HWSRC_HIRES_96KHZ      = 0,
    HWSRC_HIRES_192KHZ     = 1,
} hires_clock_frequency_hwsrc_t;

typedef enum {
    INTERNAL_BUS_HIRES_96KHZ      = 0,
    INTERNAL_BUS_HIRES_192KHZ     = 1,
} hires_clock_frequency_internal_bus_t;

typedef struct {
    bool is_low_jitter_linein_playback_in;
    bool is_low_jitter_pure_linein_playback_in;
    bool is_low_jitter_linein_playback_out;
    bool is_low_jitter_pure_linein_playback_out;
    bool is_low_jitter_bt_codec_a2dp_out;
    bool is_low_jitter_hal_audio_notify_anc_afe_settings_out;
    bool is_low_jitter_mp3_codec_play_avm_out;
    bool is_low_jitter_usb_audio_playback_out;
} is_low_jitter_scenario_t;

typedef struct {
    uint32_t low_jitter_mode_sampling_rate_linein_playback_in;
    uint32_t low_jitter_mode_sampling_rate_pure_linein_playback_in;
    uint32_t low_jitter_mode_sampling_rate_linein_playback_out;
    uint32_t low_jitter_mode_sampling_rate_pure_linein_playback_out;
    uint32_t low_jitter_mode_sampling_rate_bt_codec_a2dp_out;
    uint32_t low_jitter_mode_sampling_rate_hal_audio_notify_anc_afe_settings_out;
    uint32_t low_jitter_mode_sampling_rate_mp3_codec_play_avm_out;
    uint32_t low_jitter_mode_sampling_rate_usb_audio_playback_out;
} low_jitter_mode_sampling_rate_scenario_t;

typedef struct {
    audio_message_queue_t dsp_msg_queue;

    bool waiting;
    bool waiting_VP;
    bool waiting_RECORD;
    bool waiting_A2DP;
    bool waiting_ESCO;
    bool waiting_Adapt_ANC;
    bool waiting_DAC_OFF;
    bool dsp_power_on;
    bool dac_hi_res_on;
    bool clk_mux_dac_192k_on;
    bool hi_res_on_dl;                                                  /**< Specifies the hires ON/OFF status of downlink hires.*/
    bool hi_res_on_ul;                                                  /**< Specifies the hires ON/OFF status of uplink hires.*/
    bool hi_res_on_hwsrc;                                               /**< Specifies the hires ON/OFF status of hwsrc.*/
    bool hi_res_on_internalbus;                                         /**< Specifies the hires ON/OFF status of internalbus.*/
    hires_clock_frequency_dl_t clk_mux_hires_dl;                        /**< Specifies the hires clock frequency of downlink hires.*/
    hires_clock_frequency_hwsrc_t clk_mux_hires_hwsrc;                  /**< Specifies the hires clock frequency of hwsrc.*/
    hires_clock_frequency_internal_bus_t clk_mux_hires_internal_bus;    /**< Specifies the hires clock frequency of internal bus.*/
    bool flag_vp;                                                       /**< Specifies the ON/OFF status of VP.*/
    bool flag_dac;                                                      /**< Specifies the hires ON/OFF status of dac user.*/
    bool flag_ul_apll;                                                  /**< Specifies the hires ON/OFF status of ul apll.*/
    bool flag_dl_apll;                                                  /**< Specifies the hires ON/OFF status of dl apll.*/
    bool hi_res_top_speed;
    uint32_t running;
    uint16_t dsp_notify;
    is_low_jitter_scenario_t is_low_jitter_scenario;
    low_jitter_mode_sampling_rate_scenario_t low_jitter_mode_sampling_rate_scenario;
} dsp_controller_t;

static dsp_controller_t dsp_controller;
static audio_isr_t audio_isr;
static audio_dsp_a2dp_dl_time_param_t audio_sync_time_info;
static uint32_t dsp2mcu_data;
static uint32_t dsp2mcu_AUDIO_DL_ACL_data;
audio_dsp_leakage_compensation_report_param_t leakage_compensation_info;
#ifdef AIR_SILENCE_DETECTION_ENABLE
audio_dsp_silence_detection_param_t silence_detection_info;
#endif

extern void hal_clock_set_running_flags(uint32_t clock_cg_opt_set,bool on_off);
extern unsigned char *bt_pka_get_esco_forwarder_addr(uint16_t size);

//==== Private API ====

//== Delay function ==
static void hal_audio_delay_ms(uint32_t ms_duration)
{
    if (audio_isr.task_ms_delay_func) {
        audio_isr.task_ms_delay_func(ms_duration);
    } else {
        hal_gpt_delay_ms(ms_duration);
    }
}

//== Audio Service related ==
static void hal_audio_service_callback(audio_message_element_t *p_msg)
{
    audio_message_type_t type;
    hal_audio_event_t event = HAL_AUDIO_EVENT_NONE;
    uint16_t message16, data16;

    message16 = p_msg->message16;
    data16 = p_msg->data16;
    type = (message16 & MSG_TYPE_BASE_MASK) >> MSG_TYPE_SHIFT_BIT;
    if (type >= AUDIO_MESSAGE_TYPE_MAX){
        log_hal_msgid_info("Hal_audio_service_callback_type_error", 0);
        assert(0);
        return;
    }

    switch (message16) {
        // Error report
        case MSG_DSP2MCU_BT_AUDIO_DL_ERROR:
        case MSG_DSP2MCU_BT_VOICE_UL_ERROR:
        case MSG_DSP2MCU_BT_VOICE_DL_ERROR:
        case MSG_DSP2MCU_PLAYBACK_ERROR:
        case MSG_DSP2MCU_RECORD_ERROR:
        case MSG_DSP2MCU_PROMPT_ERROR:
            if (data16 == DSP_ERROR_REPORT_END) {
                event = HAL_AUDIO_EVENT_END;
            } else {
                event = HAL_AUDIO_EVENT_ERROR;
            }
            break;
        // Ramp down ack
        case MSG_DSP2MCU_COMMON_SIDETONE_STOP_ACK:  //Not main stream type, need assign type
            type = AUDIO_MESSAGE_TYPE_SIDETONE;
            event = HAL_AUDIO_EVENT_END;
            break;
        // Data request
        case MSG_DSP2MCU_PLAYBACK_DATA_REQUEST:
        case MSG_DSP2MCU_PROMPT_DATA_REQUEST:
            event = HAL_AUDIO_EVENT_DATA_REQUEST;
            hal_audio_status_set_notify_flag(type, true);
            break;

        // Data notification
        case MSG_DSP2MCU_RECORD_DATA_NOTIFY:
            event = HAL_AUDIO_EVENT_DATA_NOTIFICATION;
            hal_audio_status_set_notify_flag(type, true);
            if (audio_isr.callback[type]) {
                void *user_data = audio_isr.user_data[type];
                #ifdef MTK_USER_TRIGGER_FF_ENABLE
//                log_hal_msgid_info("[user_trigger_ff]hal_audio_service_callback, p_msg->message16=0x%x, p_msg->data16=%d, p_msg->data32=%d", 3, p_msg->message16, p_msg->data16, p_msg->data32);
                if (user_data) {
                    *((uint32_t *)user_data) = ((uint32_t)(p_msg->data16) << 16) | (((uint32_t)p_msg->data32) & 0xFFFF);
//                    log_hal_msgid_info("[user_trigger_ff]*((uint32_t *)user_data) = 0x%x", 1, *((uint32_t *)user_data));
                }
                #endif
                audio_isr.callback[type](event, user_data);
                return;
            }
            break;

        case MSG_DSP2MCU_RECORD_WWE_VERSION:
            event = HAL_AUDIO_EVENT_WWE_VERSION;
            if (audio_isr.callback[type]) {
                //void *user_data = audio_isr.user_data[type];
                audio_isr.callback[type](event, (void *)p_msg->data32);
                return;
            }
            //hal_audio_status_set_notify_flag(type, true);
            break;

        case MSG_DSP2MCU_RECORD_WWD_NOTIFY:
            event = HAL_AUDIO_EVENT_WWD_NOTIFICATION;
            if (audio_isr.callback[type]) {
                //void *user_data = audio_isr.user_data[type];
                audio_isr.callback[type](event, (void *)p_msg->data32);
                return;
            }
            //hal_audio_status_set_notify_flag(type, true);
            break;

        case MSG_DSP2MCU_RECORD_DATA_ABORT_NOTIFY:
            event = HAL_AUDIO_EVENT_DATA_ABORT_NOTIFICATION;
            //hal_audio_status_set_notify_flag(type, true);
            break;

        case MSG_DSP2MCU_BT_AUDIO_DL_TIME_REPORT:
            event = HAL_AUDIO_EVENT_TIME_REPORT;
            memcpy(&audio_sync_time_info, (void *)p_msg->data32, sizeof(audio_dsp_a2dp_dl_time_param_t));
            break;

        case MSG_DSP2MCU_BT_AUDIO_DL_ALC_REQUEST:
            event = HAL_AUDIO_EVENT_ALC_REQUEST;
            dsp2mcu_AUDIO_DL_ACL_data = p_msg->data32;
            break;

        case MSG_DSP2MCU_BT_AUDIO_DL_LTCS_DATA_REPORT:
            event = HAL_AUDIO_EVENT_LTCS_REPORT;
            //log_hal_msgid_info("[HAL audio] LTCS debug: ASI Buf 0x%x \r\n", 1, (uint32_t)hal_audio_query_ltcs_asi_buf());
            //log_hal_msgid_info("[HAL audio] LTCS debug: MNGP Buf 0x%x \r\n", 1, (uint32_t)hal_audio_query_ltcs_min_gap_buf());
            break;

        case MSG_DSP2MCU_BT_AUDIO_DL_LTCS_TIMEOUT_REQUEST:
            event = HAL_AUDIO_EVENT_LTCS_TIMEOUT;
            break;

        case MSG_DSP2MCU_BT_AUDIO_DL_REINIT_REQUEST:
            dsp2mcu_data = p_msg->data32;
            event = HAL_AUDIO_EVENT_DL_REINIT_REQUEST;
            break;

        // Config
        case MSG_DSP2MCU_PROMPT_CONFIG_ACK:
            // To notify VP should be closed by APP
            event = HAL_AUDIO_EVENT_END;
            break;

        // Special case.
        // BT Audio DL open: async callback
        case MSG_DSP2MCU_BT_AUDIO_DL_OPEN_ACK:
            if (audio_isr.bt_audio_dl_open_callback) {
                audio_isr.bt_audio_dl_open_callback();
            }
            // Need not to notify, so we use return here.
            return;

        case MSG_DSP2MCU_COMMON_REQ_GET_AUDIO_FEATURE_PARAMETER:
            event = p_msg->data16;
            if (audio_isr.callback[type]) {
                void *user_data = audio_isr.user_data[type];
                *((uint32_t *)user_data) = p_msg->data32;
                audio_isr.callback[type](event, user_data);
            }
            return;
        case MSG_DSP2MCU_AVC_PARA_SEND:
            dsp2mcu_data = p_msg->data32;
            event = HAL_AUDIO_EVENT_HFP_PARA_SEND;
            break;
        case MSG_DSP2MCU_AUDIO_AMP:  //Not main stream type, need assign type
            type = AUDIO_MESSAGE_TYPE_AFE;
            if (data16) {
                event = HAL_AUDIO_EVENT_DATA_REQUEST;
            } else {
                event = HAL_AUDIO_EVENT_NONE;
            }
            break;
        #ifdef MTK_ANC_ENABLE
        case MSG_DSP2MCU_COMMON_ANC_STOP_ACK:
        case MSG_DSP2MCU_COMMON_ANC_START_DONE:
        case MSG_DSP2MCU_COMMON_ANC_SET_VOLUME_ACK:
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_SWITCH:
            type = AUDIO_MESSAGE_TYPE_ANC;
            if (audio_isr.callback[type]) {
                void *user_data = audio_isr.user_data[type];
                if (user_data != NULL) {
                *((uint32_t *)user_data) = p_msg->data32;
                }
                audio_isr.callback[type](event, (void *)(uint32_t)message16);
            }
            return;
        #endif
        #if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_ADAPTIVE:
            type = AUDIO_MESSAGE_TYPE_ADAPTIVE;
            if (audio_isr.callback[type]) {
                *(uint32_t *)(audio_isr.user_data[type]) = p_msg->data16;
                audio_isr.callback[type]((hal_audio_event_t)p_msg->data16, &p_msg->data32);
            }
            return;
        #endif
        #ifdef MTK_LEAKAGE_DETECTION_ENABLE
        case MSG_DSP2MCU_RECORD_LC_WZ_REPORT:
            log_hal_msgid_info("[RECORD_LC]cm4 received dsp report, data16:%d, data32:%d\r\n", 2, p_msg->data16, p_msg->data32);
            leakage_compensation_info.calibration_status = p_msg->data16;
            leakage_compensation_info.wz_set = (uint16_t)p_msg->data32;
            if(leakage_compensation_info.api_callback != NULL) {
                leakage_compensation_info.api_callback((uint16_t)p_msg->data32);
            }
            return;
        #endif
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_NOTIFY:
            event = HAL_AUDIO_EVENT_DATA_NOTIFICATION;
            type = AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER;
            if (audio_isr.callback[type]) {
                uint32_t user_data[2];
                user_data[0] = (uint32_t)data16;
                user_data[1] = (uint32_t)p_msg->data32;
                audio_isr.callback[type](event, (void *)user_data);
            }
            return;
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_DIRECT:
            event = HAL_AUDIO_EVENT_DATA_DIRECT;
            type = AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER;
            if (audio_isr.callback[type]) {
                uint32_t user_data[2];
                user_data[0] = (uint32_t)data16;
                user_data[1] = (uint32_t)p_msg->data32;
                audio_isr.callback[type](event, (void *)user_data);
            }
            return;
        #ifdef MTK_SLT_AUDIO_HW
        case MSG_DSP2MCU_COMMON_AUDIO_SLT_RESULT:
            type = AUDIO_MESSAGE_TYPE_COMMON;
            if (audio_isr.callback[type]) {
                void *user_data = audio_isr.user_data[type];
                if (user_data != NULL) {
                *((uint32_t *)user_data) = p_msg->data32;
                }
                audio_isr.callback[type](event, (void *)(uint32_t)message16);
            }
         return;
        #endif
        case MSG_DSP2MCU_AUDIO_SYNC_DONE: /* notify APP that sync action is already done*/
        {
            uint8_t scenario_id = data16>>8;
            uint8_t action_id = (uint8_t)data16;
            log_hal_msgid_info("[DSP SYNC] DSP2MCU ACK, scenario_type = %d, action_type = %d\r\n", 2, scenario_id, action_id);
            switch (scenario_id)
            {
            case 0: // HFP
                /* TODO */
                return;
            case 1: // A2DP
                /* TODO */
                if (action_id == 1) { /* a2dp sync stop */
                    type = AUDIO_MESSAGE_TYPE_BT_A2DP_DL;
                    event = HAL_AUDIO_EVENT_END;
                }
                break;
            case 2: // ANC
                /* TODO */
                return;
            case 3: // VP
                if (action_id == 0) { /* VP Start */
                    type = AUDIO_MESSAGE_TYPE_PROMPT;
                    event = HAL_AUDIO_EVENT_DATA_DIRECT;
                }
                break;
            default:
                return;
            }
            break;
        }
        case MSG_DSP2MCU_COMMON_UPDATE_AUDIO_NVDM_STATUS:
            event = HAL_AUDIO_EVENT_AUDIO_NVDM;
            type = AUDIO_MESSAGE_TYPE_AUDIO_NVDM;
            if (audio_isr.callback[type]) {
                uint32_t user_data;
                user_data = data16;
                audio_isr.callback[type](event, &user_data);
            }
            return;
        #ifdef AIR_SILENCE_DETECTION_ENABLE
        case MSG_DSP2MCU_BT_AUDIO_DL_SILENCE_DETECTION_FEEDBACK:
        {
            log_hal_msgid_info("[SD]APO silence status:%d",1,p_msg->data16);
            if(silence_detection_info.callback != NULL) {
                silence_detection_info.callback((bool)(p_msg->data16));
            }
            return;
        }
        #endif
        default:
            // Need not to notify, so we use return here.
            return;
    }

    if (audio_isr.callback[type]) {
        void *user_data = audio_isr.user_data[type];
        audio_isr.callback[type](event, user_data);
    }
}

//== Message Queue related ==
static void hal_audio_message_enqueue(audio_message_queue_t *p_queue, uint32_t msg16, uint32_t data16, uint32_t data32)
{
    audio_message_element_t *p_msg_element;

    // Check whether queue is full
    if ( ((p_queue->write_index + 1) & (AUDIO_MESSAGE_QUEUE_SIZE-1)) == p_queue->read_index ) {
        log_hal_msgid_error("[HAL audio]Message queue full\r\n", 0);
        log_hal_msgid_info("DSP_to_CM4_msg_queue_full", 0);
        assert(0);
    }

    // Enqueue
    p_msg_element = &p_queue->message[p_queue->write_index];
    p_msg_element->message16 = msg16;
    p_msg_element->data16 = data16;
    p_msg_element->data32 = data32;
    p_queue->write_index = (p_queue->write_index + 1) & (AUDIO_MESSAGE_QUEUE_SIZE-1);
}

static void hal_audio_message_dequeue(audio_message_queue_t *p_queue, audio_message_element_t *p_msg)
{
    audio_message_element_t *p_msg_element;

    // Check whether queue is empty
    if ( p_queue->write_index == p_queue->read_index ) {
        log_hal_msgid_error("[HAL audio]Message queue empty\r\n", 0);
        log_hal_msgid_info("DSP_to_CM4_msg_queue_empty", 0);
        assert(0);
    }

    // Dequeue
    p_msg_element = &p_queue->message[p_queue->read_index];
    p_msg->message16 = p_msg_element->message16;
    p_msg->data16 = p_msg_element->data16;
    p_msg->data32 = p_msg_element->data32;
    p_queue->read_index = (p_queue->read_index + 1) & (AUDIO_MESSAGE_QUEUE_SIZE-1);
}

void hal_audio_dsp_message_process(void)
{
    // For data notification, request and error, we have to callback
    // For the other message, we may skip it
    while (dsp_controller.dsp_msg_queue.read_index != dsp_controller.dsp_msg_queue.write_index) {
        audio_message_element_t msg;

        hal_audio_message_dequeue( &dsp_controller.dsp_msg_queue, &msg);

        hal_audio_service_callback(&msg);
    }
}

static void hal_audio_init_share_info_section(n9_dsp_share_info_t *p_info, uint32_t *p_buf_addr, uint32_t buf_byte_size)
{
    memset(p_info, 0, sizeof(n9_dsp_share_info_t));
    p_info->start_addr = (uint32_t)p_buf_addr;
    p_info->length     = buf_byte_size;
}

//== DSP power on ==
#if 0 //Mark this for fixing build warning.
#if defined(HAL_SLEEP_MANAGER_ENABLED)
static void hal_audio_dsp_power_on_check(hal_core_id_t id)
{
    hal_core_status_t state;

    do {
        state = hal_core_status_read(id);
        if ((state == HAL_CORE_ACTIVE) || (state == HAL_CORE_SLEEP)) {
            break;
        }
        hal_audio_delay_ms(2);
    } while (1);
}
#endif
#endif
static void hal_audio_dsp_power_on(void)
{
#if defined(HAL_SLEEP_MANAGER_ENABLED)
    // DSP power on
    spm_control_mtcmos(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_ENABLE);
    //pmu_enable_power(PMU_BUCK_VAUD18, PMU_ON);
    // DSP reset
    hal_dsp_core_reset(HAL_CORE_DSP0, DSP0_BASE);
//    hal_dsp_core_reset(HAL_CORE_DSP1, DSP1_BASE);

  #if 0 // will checked in CCNI
    // Wait for finish
    hal_audio_dsp_power_on_check(HAL_CORE_DSP0);
//    hal_audio_dsp_power_on_check(HAL_CORE_DSP1);
  #endif

#endif
}

static void hal_audio_dsp_power_off(void)
{
#if defined(HAL_SLEEP_MANAGER_ENABLED)
    spm_control_mtcmos(SPM_MTCMOS_DSP, SPM_MTCMOS_PWR_DISABLE);
    //pmu_enable_power(PMU_BUCK_VAUD18, PMU_OFF);
#endif
}

uint32_t data_addr;
void common_type_callback(hal_audio_event_t event, void *user_data) {

   uint32_t addr = hal_memview_infrasys_to_cm4(*(uint32_t *)user_data);
   UNUSED(addr);
   UNUSED(event);
   //log_hal_msgid_info("============================== user_data = 0x%x \n\r", 1, addr);

}
//==== Public API ====
uint32_t hal_audio_dsp2mcu_data_get(void)
{
    return dsp2mcu_data;
}

uint32_t hal_audio_dsp2mcu_AUDIO_DL_ACL_data_get(void)
{
    return dsp2mcu_AUDIO_DL_ACL_data;
}

void hal_audio_dsp_controller_init(void)
{
    // Fill share buffer information
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_AUDIO_DL], &audio_share_buffer.bt_audio_dl[0], SHARE_BUFFER_BT_AUDIO_DL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL], &audio_share_buffer.bt_voice_ul[0], SHARE_BUFFER_BT_VOICE_UL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL], &audio_share_buffer.bt_voice_dl[0], SHARE_BUFFER_BT_VOICE_DL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_PROMPT],      &audio_share_buffer.prompt[0],      SHARE_BUFFER_PROMPT_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_RECORD],      &audio_share_buffer.record[0],      SHARE_BUFFER_RECORD_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_RINGTONE],    &audio_share_buffer.ringtone[0],    SHARE_BUFFER_RINGTONE_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_NVKEY_PARAMETER],    &audio_share_buffer.nvkey_param[0],   SHARE_BUFFER_NVKEY_PARAMETER_SIZE);
    #ifdef AIR_BT_CODEC_BLE_ENABLED
    leaudio_share_buffer.ble_audio_ul = (uint32_t *)bt_pka_get_leaudio_AVM_addr(SHARE_BUFFER_BLE_AUDIO_UL_SIZE + SHARE_BUFFER_BLE_AUDIO_DL_SIZE + SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE + SHARE_BUFFER_BLE_AUDIO_SUB_DL_SIZE);
    leaudio_share_buffer.ble_audio_dl = &leaudio_share_buffer.ble_audio_ul[(SHARE_BUFFER_BLE_AUDIO_UL_SIZE+3)/4];
    leaudio_share_buffer.ble_audio_sub_ul = &leaudio_share_buffer.ble_audio_dl[(SHARE_BUFFER_BLE_AUDIO_DL_SIZE+3)/4];
    leaudio_share_buffer.ble_audio_sub_dl = &leaudio_share_buffer.ble_audio_sub_ul[(SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE+3)/4];

    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_UL], leaudio_share_buffer.ble_audio_ul, SHARE_BUFFER_BLE_AUDIO_UL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_DL], leaudio_share_buffer.ble_audio_dl, SHARE_BUFFER_BLE_AUDIO_DL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_UL], leaudio_share_buffer.ble_audio_sub_ul, SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE);
    hal_audio_init_share_info_section( &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_DL], leaudio_share_buffer.ble_audio_sub_dl, SHARE_BUFFER_BLE_AUDIO_SUB_DL_SIZE);
    #endif

    #if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined (AIR_WIRED_AUDIO_ENABLE)
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_RECEIVE_FROM_AIR],&audio_share_buffer.bt_audio_dl[0],        10*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_SEND_TO_AIR],&audio_share_buffer.bt_audio_dl[0]+10*1024/4,10*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_0],&audio_share_buffer.bt_audio_dl[0]+20*1024/4,5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_1],&audio_share_buffer.bt_audio_dl[0]+25*1024/4,5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_SEND_TO_MCU],&audio_share_buffer.bt_audio_dl[0]+30*1024/4,10*1024);
    #if defined (AIR_WIRED_AUDIO_ENABLE)
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_0],&audio_share_buffer.usb_in_0[0],SHARE_BUFFER_BT_AUDIO_USB_IN_SIZE);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_1],&audio_share_buffer.usb_in_1[0],SHARE_BUFFER_BT_AUDIO_USB_IN_SIZE);
    #endif
    #endif

    #if defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_0], &audio_share_buffer.bt_audio_dl[0]+0*1024/4,    5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_1], &audio_share_buffer.bt_audio_dl[0]+5*1024/4,    5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0],      &audio_share_buffer.bt_audio_dl[0]+10*1024/4,   5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1],      &audio_share_buffer.bt_audio_dl[0]+15*1024/4,   5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_0],&audio_share_buffer.bt_audio_dl[0]+20*1024/4,   5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_1],&audio_share_buffer.bt_audio_dl[0]+25*1024/4,   5*1024);
    hal_audio_init_share_info_section(&audio_transmitter_share_info[AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_SEND_TO_MCU],       &audio_share_buffer.bt_audio_dl[0]+30*1024/4,   5*1024);
    #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

    //Add common callback register here
    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_COMMON, common_type_callback, &data_addr);

    // Notify DSP the share buffer address
    //Wait for UT:    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_MEMORY, 0, (uint32_t)&n9_dsp_share_info, false);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_MEMORY, 0, (uint32_t)&n9_dsp_share_info, false);

    memset((void *)&audio_clkmux_status, 0, AUDIO_CLKMUX_BLOCK_NUM_OF_CLKMUX_BLOCK * sizeof(audiosys_clkmux_control));
}

void hal_audio_dsp_controller_deinit(void)
{
    dsp_controller.dsp_power_on = false;
    hal_audio_dsp_power_off();
}

void hal_audio_ccni_isr(hal_ccni_event_t event, void *msg)
{
    uint32_t *pMsg = (uint32_t *)msg;
    uint32_t msg1, msg2;
    uint32_t msg16, data16;
    uint32_t status;

    status = hal_ccni_mask_event(event);

    msg1 = pMsg[0];
    msg2 = pMsg[1];

    status = hal_ccni_clear_event(event);
    status = hal_ccni_unmask_event(event);
    UNUSED(status);
    msg16 = msg1>>16;
    data16 = msg1 & 0xFFFF;

#if defined(HAL_AUDIO_DEBUG)
    log_hal_msgid_info("[HAL audio] Receive msg %x, %x, %x \r\n", 3, (unsigned int)msg16, (unsigned int)data16, (unsigned int)msg2);
#endif

    // Check the message type, clear waiting flag
    switch (msg16) {
        case MSG_DSP2MCU_BT_AUDIO_DL_OPEN_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_CLOSE_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_START_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_STOP_ACK:
            dsp_controller.waiting_A2DP = false;
            break;
        case MSG_DSP2MCU_BT_VOICE_UL_OPEN_ACK:
        case MSG_DSP2MCU_BT_VOICE_UL_CLOSE_ACK:
        case MSG_DSP2MCU_BT_VOICE_UL_START_ACK:
        case MSG_DSP2MCU_BT_VOICE_UL_STOP_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_OPEN_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_CLOSE_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_START_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_STOP_ACK:
        case MSG_DSP2MCU_BT_VOICE_UL_SUSPEND_ACK:
        case MSG_DSP2MCU_BT_VOICE_UL_RESUME_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_SUSPEND_ACK:
        case MSG_DSP2MCU_BT_VOICE_DL_RESUME_ACK:
            dsp_controller.waiting_ESCO = false;
            break;
        case MSG_DSP2MCU_BLE_AUDIO_UL_OPEN_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_UL_CLOSE_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_UL_START_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_UL_STOP_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_OPEN_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_CLOSE_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_START_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_STOP_ACK:
        case MSG_DSP2MCU_PLAYBACK_OPEN_ACK:
        case MSG_DSP2MCU_PLAYBACK_CLOSE_ACK:
        case MSG_DSP2MCU_PLAYBACK_START_ACK:
        case MSG_DSP2MCU_PLAYBACK_STOP_ACK:
            dsp_controller.waiting = false;
            break;
        case MSG_DSP2MCU_RECORD_OPEN_ACK:
        case MSG_DSP2MCU_RECORD_CLOSE_ACK:
        case MSG_DSP2MCU_RECORD_START_ACK:
        case MSG_DSP2MCU_RECORD_STOP_ACK:
        case MSG_DSP2MCU_RECORD_LC_SET_PARAM_ACK:
            dsp_controller.waiting_RECORD = false;
            break;
        case MSG_DSP2MCU_PROMPT_OPEN_ACK:
        case MSG_DSP2MCU_PROMPT_CLOSE_ACK:
        case MSG_DSP2MCU_PROMPT_START_ACK:
        case MSG_DSP2MCU_PROMPT_STOP_ACK:
        case MSG_DSP2MCU_PROMPT_DUMMY_SOURCE_OPEN_ACK:
        case MSG_DSP2MCU_PROMPT_DUMMY_SOURCE_CLOSE_ACK:
        case MSG_DSP2MCU_PROMPT_DUMMY_SOURCE_START_ACK:
        case MSG_DSP2MCU_PROMPT_DUMMY_SOURCE_STOP_ACK:
            dsp_controller.waiting_VP = false;
            break;
        case MSG_DSP2MCU_LINEIN_PLAYBACK_OPEN_ACK:
        case MSG_DSP2MCU_LINEIN_PLAYBACK_CLOSE_ACK:
        case MSG_DSP2MCU_LINEIN_PLAYBACK_START_ACK:
        case MSG_DSP2MCU_LINEIN_PLAYBACK_STOP_ACK:
        case MSG_DSP2MCU_LINEIN_PLAYBACK_SUSPEND_ACK:
        case MSG_DSP2MCU_LINEIN_PLAYBACK_RESUME_ACK:
        case MSG_DSP2MCU_TRULY_LINEIN_PLAYBACK_OPEN_ACK:
        case MSG_DSP2MCU_TRULY_LINEIN_PLAYBACK_CLOSE_ACK:
        case MSG_DSP2MCU_COMMON_AUDIO_LOOPBACK_TEST_ACK:
        case MSG_DSP2MCU_COMMON_REQ_GET_REALTIME_REF_GAIN_ACK:
        case MSG_MCU2DSP_COMMON_REQ_GET_REALTIME_LIB_VERSION_ACK:
        case MSG_DSP2MCU_COMMON_AEC_NR_SET_PARAM_ACK:
        case MSG_DSP2MCU_COMMON_SIDETONE_START_ACK:
        //case MSG_DSP2MCU_COMMON_SIDETONE_STOP_ACK:  //Sidetone stop ack asynchronous. no wait
        case MSG_DSP2MCU_COMMON_ANC_START_ACK:
        case MSG_DSP2MCU_COMMON_ANC_SET_VOLUME_ACK:
        case MSG_DSP2MCU_COMMON_ANC_SET_PARAM_ACK:
        case MSG_DSP2MCU_COMMON_DC_COMPENSATION_START_ACK:
        case MSG_DSP2MCU_COMMON_DC_COMPENSATION_STOP_ACK:
        case MSG_DSP2MCU_COMMON_DUMMY_DSP_SHUTDOWN_ACK:
        case MSG_DSP2MCU_COMMON_PEQ_SET_PARAM_ACK:
        case MSG_DSP2MCU_COMMON_DEQ_SET_PARAM_ACK:
        case MSG_DSP2MCU_COMMON_SET_ALGORITHM_PARAM_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_SUSPEND_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_RESUME_ACK:
        case MSG_DSP2MCU_PLAYBACK_SUSPEND_ACK:
        case MSG_DSP2MCU_PLAYBACK_RESUME_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_SUSPEND_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_DL_RESUME_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_UL_SUSPEND_ACK:
        case MSG_DSP2MCU_BLE_AUDIO_UL_RESUME_ACK:
        case MSG_DSP2MCU_RECORD_SUSPEND_ACK:
        case MSG_DSP2MCU_RECORD_RESUME_ACK:
        case MSG_DSP2MCU_COMMON_SET_OUTPUT_VOLUME_PARAMETERS_ACK:
        case MSG_DSP2MCU_AUDIO_AMP_FORCE_CLOSE_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_OPEN_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_CLOSE_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_START_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_STOP_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_CONFIG_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_SUSPEND_ACK:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_RESUME_ACK:
        case MSG_DSP2MCU_AUDIO_SYNC_REQUEST_ACK:
        case MSG_DSP2MCU_COMMON_SET_DRIVER_PARAM_ACK:
            dsp_controller.waiting = false;
            break;
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_ADAPTIVE_GET_ACK:
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_ADAPTIVE_SET_ACK:
            dsp_controller.waiting_Adapt_ANC = false;
            break;
        case MSG_MCU2DSP_COMMON_POWER_OFF_DAC_IMMEDIATELY_ACK:
            dsp_controller.waiting_DAC_OFF = false;
            break;
    }

    // Decide whether we have to handle the message further.
    switch (msg16) {
        case MSG_DSP2MCU_BT_AUDIO_DL_SILENCE_DETECTION_FEEDBACK:
        case MSG_DSP2MCU_AUDIO_AMP:
        case MSG_DSP2MCU_BT_AUDIO_DL_OPEN_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_ERROR:
        case MSG_DSP2MCU_BT_VOICE_UL_ERROR:
        case MSG_DSP2MCU_BT_VOICE_DL_ERROR:
        case MSG_DSP2MCU_PLAYBACK_ERROR:
        case MSG_DSP2MCU_PLAYBACK_DATA_REQUEST:
        case MSG_DSP2MCU_RECORD_ERROR:
        case MSG_DSP2MCU_RECORD_DATA_NOTIFY:
        case MSG_DSP2MCU_RECORD_WWE_VERSION:
        case MSG_DSP2MCU_RECORD_WWD_NOTIFY:
        case MSG_DSP2MCU_RECORD_DATA_ABORT_NOTIFY:
        case MSG_DSP2MCU_PROMPT_ERROR:
        case MSG_DSP2MCU_PROMPT_DATA_REQUEST:
        case MSG_DSP2MCU_PROMPT_CONFIG_ACK:
        case MSG_DSP2MCU_COMMON_SIDETONE_STOP_ACK:
        case MSG_DSP2MCU_BT_AUDIO_DL_TIME_REPORT:
        case MSG_DSP2MCU_BT_AUDIO_DL_ALC_REQUEST:
        case MSG_DSP2MCU_BT_AUDIO_DL_LTCS_DATA_REPORT:
        case MSG_DSP2MCU_BT_AUDIO_DL_LTCS_TIMEOUT_REQUEST:
        case MSG_DSP2MCU_COMMON_REQ_GET_AUDIO_FEATURE_PARAMETER:
        case MSG_DSP2MCU_BT_AUDIO_DL_REINIT_REQUEST:
        case MSG_DSP2MCU_AVC_PARA_SEND:
        case MSG_DSP2MCU_COMMON_ANC_STOP_ACK:
        case MSG_DSP2MCU_COMMON_ANC_START_DONE:
        case MSG_DSP2MCU_COMMON_ANC_SET_VOLUME_ACK:
        case MSG_DSP2MCU_RECORD_LC_WZ_REPORT:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_NOTIFY:
        case MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_DIRECT:
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_SWITCH:
        case MSG_DSP2MCU_COMMON_AUDIO_ANC_ADAPTIVE:
        case MSG_DSP2MCU_AUDIO_SYNC_DONE:
        case MSG_DSP2MCU_COMMON_UPDATE_AUDIO_NVDM_STATUS:
            // Put into message queue
            hal_audio_message_enqueue( &dsp_controller.dsp_msg_queue, msg16, data16, msg2);

            if (audio_isr.audio_event_task_callback) {
                // Notify to task
                audio_isr.audio_event_task_callback();
            } else {
                // Process message directly
                hal_audio_dsp_message_process();
            }

            break;
    }
}

void hal_audio_put_message_via_ccni(uint16_t message, uint16_t data16, uint32_t data32)
{
    hal_ccni_status_t ret;
    hal_ccni_message_t ccni_msg;
    uint32_t i;

    // Power on DSP at the first use
    if (!dsp_controller.dsp_power_on) {
        dsp_controller.dsp_power_on = true;
        hal_audio_dsp_power_on();
        log_hal_msgid_info("dsp power on\n", 0);

        // Wait DSP boot ready at the first use
        for (i = 1 ; ; i++) {
            if((hal_core_status_read(HAL_CORE_DSP0) == HAL_CORE_ACTIVE) || (hal_core_status_read(HAL_CORE_DSP0) == HAL_CORE_SLEEP)) {
                log_hal_msgid_info("dsp ready\n", 0);
                break;
            }
#if !defined(HAL_AUDIO_DEBUG)
            assert(i < 100);
#else
            if ((i % 1000) == 0) {
                log_hal_msgid_info("Waiting msg(0x%x) CCNI_busy %ld \r\n", 2, message, i);
                platform_assert("DSP_no_remove_CCNI",__FILE__,__LINE__);
            }
#endif
            hal_audio_delay_ms(2);
        }
    }

    // Fill into ccni message
    ccni_msg.ccni_message[0] = (message<<16) | data16;
    ccni_msg.ccni_message[1] = data32;

    for (i = 1 ; (ret = hal_ccni_set_event(CCNI_CM4_TO_DSP0_RX_EVENT, &ccni_msg)) != HAL_CCNI_STATUS_OK ; i++) {
#if !defined(HAL_AUDIO_DEBUG)
        assert(i < 40);
#else
        if ((i % 1000) == 0) {
            log_hal_msgid_info("Send message waiting %d (%d)\r\n", 2, (int)i, ret);
            platform_assert("CM4_send_DSP_CCNI_timeout",__FILE__,__LINE__);
        }
#endif
        hal_audio_delay_ms(2);
    }

#if defined(HAL_AUDIO_DEBUG)
    log_hal_msgid_info("[HAL audio] Send msg %x %x, wait count %d \r\n", 3, (unsigned int)ccni_msg.ccni_message[0], (unsigned int)ccni_msg.ccni_message[1], (int)i);
#endif
}

void hal_audio_dsp_internalbus_clkmux_control(uint32_t device_rate, bool isEnable)
{
    if (isEnable == TRUE) {
        if (device_rate > 48000) {
            if (device_rate > 96000) {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
                log_hal_msgid_info("frequency is risen to 0.8V for bus hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_internal_bus = INTERNAL_BUS_HIRES_192KHZ;
            } else {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M, HAL_DVFS_LOCK);
                log_hal_msgid_info("frequency is risen to 0.7V for bus hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_internal_bus = INTERNAL_BUS_HIRES_96KHZ;
            }

            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount == 0) {
                log_hal_msgid_info("[hi-res]Enable INTERNAL_BUS", 0);
                clock_mux_sel(CLK_AUD_BUS_SEL, 1);/*  MPLL_D5, 124.8 MHz */
                dsp_controller.hi_res_on_internalbus = isEnable;
            }
            audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount++;
        }
    } else {
        if (dsp_controller.hi_res_on_internalbus == TRUE) {
            audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount--;
            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount == 0) {
                log_hal_msgid_info("[hi-res]Disable INTERNAL_BUS", 0);
                clock_mux_sel(CLK_AUD_BUS_SEL, 0);/*  F_FXO_CK, 26 MHz */
                dsp_controller.hi_res_on_internalbus = isEnable;
            }
            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount < 0) {
                log_hal_msgid_info("Error, [hi-res]Disable INTERNAL_BUS UserCount < 0", 0);
                audio_clkmux_status[AUDIO_CLKMUX_BLOCK_INTERNAL_BUS].mUserCount = 0;
            }

            if (dsp_controller.clk_mux_hires_internal_bus == INTERNAL_BUS_HIRES_192KHZ) {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                log_hal_msgid_info("frequency set back from bus hi-res 0.8V out", 0);
                #endif
                #endif
            } else {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M, HAL_DVFS_UNLOCK);
                log_hal_msgid_info("frequency set back from bus hi-res 0.7V out", 0);
                #endif
                #endif
            }
        }
    }
}

void hal_audio_dsp_hwsrc_clkmux_control(uint32_t device_rate, bool isEnable)
{
    if (isEnable == TRUE) {
        if (device_rate > 48000) {
            if (device_rate > 96000) {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
                log_hal_msgid_info("frequency is risen to 0.8V for hwsrc hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_hwsrc = HWSRC_HIRES_192KHZ;
            } else {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M , HAL_DVFS_LOCK);
                log_hal_msgid_info("frequency is risen to 0.7V for hwsrc hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_hwsrc = HWSRC_HIRES_96KHZ;
            }

            log_hal_msgid_info("[hi-res]Enable HWSRC", 0);
            if (device_rate > 96000) {
                clock_mux_sel(CLK_AUD_GPSRC_SEL, 2);/* MPLL_D3_D2, 104 MHz */
            } else {
                clock_mux_sel(CLK_AUD_GPSRC_SEL, 1);/* MPLL_D3_D04, 52 MHz */
            }
            dsp_controller.hi_res_on_hwsrc = isEnable;

        }
    } else {
        if (dsp_controller.hi_res_on_hwsrc == TRUE) {
            log_hal_msgid_info("[hi-res]Disable HWSRC", 0);
            clock_mux_sel(CLK_AUD_GPSRC_SEL, 0);/* F_FXO_CK, 26 MHz */
            dsp_controller.hi_res_on_hwsrc = isEnable;

            if (dsp_controller.clk_mux_hires_hwsrc == HWSRC_HIRES_192KHZ) {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                log_hal_msgid_info("frequency set back from hwsrc hi-res 0.8V out", 0);
                #endif
                #endif
            } else {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M , HAL_DVFS_UNLOCK);
                log_hal_msgid_info("frequency set back from hwsrc hi-res 0.7V out", 0);
                #endif
                #endif
            }
        }
    }
}

#define AUDIO_DL_CLKMUX_SCENARIO_NUM 7
static uint32_t gAudio_dl_clkmux_rate[AUDIO_DL_CLKMUX_SCENARIO_NUM];
static uint8_t hal_audio_dl_clkmux_type_convert_index(audio_message_type_t type)
{
    uint8_t index=0;
    switch(type)
    {
        case AUDIO_MESSAGE_TYPE_PROMPT:
            index = 0;
            break;
        case AUDIO_MESSAGE_TYPE_BT_AUDIO_DL:
            index = 1;
            break;
        case AUDIO_MESSAGE_TYPE_BT_VOICE_DL:
            index = 2;
            break;
        case AUDIO_MESSAGE_TYPE_ANC:
            index = 3;
            break;
        case AUDIO_MESSAGE_TYPE_LINEIN:
            index = 4;
            break;
        case AUDIO_MESSAGE_TYPE_PLAYBACK:
            index = 5;
            break;
        case AUDIO_MESSAGE_TYPE_SIDETONE:
            index = 6;
            break;
        default:
            log_hal_msgid_error("[ERROR] dl_clkmux type convert not support type.", 0);
            break;
    }
    return index;
}
bool g_main_dl_use_high_res = 0;
void hal_audio_dsp_dl_clkmux_control(audio_message_type_t type, hal_audio_device_t out_device, uint32_t device_rate, bool isEnable)
{
    if (isEnable == TRUE) {
        gAudio_dl_clkmux_rate[hal_audio_dl_clkmux_type_convert_index(type)] = device_rate;
    #if defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)||defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
        if (((out_device & HAL_AUDIO_DEVICE_DAC_DUAL)||(out_device == HAL_AUDIO_DEVICE_I2S_MASTER)) && (device_rate > 48000)) {
    #else
        if ((type != AUDIO_MESSAGE_TYPE_PROMPT) && ((out_device & HAL_AUDIO_DEVICE_DAC_DUAL)||(out_device == HAL_AUDIO_DEVICE_I2S_MASTER)) && (device_rate > 48000)) {
    #endif
            if (device_rate > 96000) {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
                log_hal_msgid_info("frequency is risen to 0.8V for dl hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_dl = DL_HIRES_192KHZ;
            } else {
                #if defined(HAL_DVFS_MODULE_ENABLED)
                #if defined(HAL_DVFS_416M_SOURCE)
                /* No need to lock DVFS */
                //hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M , HAL_DVFS_LOCK);
                //log_hal_msgid_info("frequency is risen to 0.7V for dl hi-res out rate : %d", 1, (int)device_rate);
                #endif
                #endif
                dsp_controller.clk_mux_hires_dl = DL_HIRES_96KHZ;
            }

            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount == 0) {
                // fs >=96k open hwsrc
                hal_audio_dsp_hwsrc_clkmux_control(device_rate, isEnable);
                log_hal_msgid_info("[hi-res]Enable DL", 0);
                clock_mux_sel(CLK_AUD_DLCK_SEL , 1); /* MPLL_D5, 124.8 MHz */  //modify for ab1568
                hal_clock_enable(HAL_CLOCK_CG_AUD_DWLINK );//modify for ab1568
                //printf("_hf_faud_dwlink_ck %d\r\n",hal_clock_get_freq_meter(_hf_faud_dwlink_ck,100));
                dsp_controller.hi_res_on_dl = isEnable;
            }
            audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount++;
            if((type == AUDIO_MESSAGE_TYPE_BT_AUDIO_DL) || (type == AUDIO_MESSAGE_TYPE_LINEIN) || (type == AUDIO_MESSAGE_TYPE_PLAYBACK) || (type == AUDIO_MESSAGE_TYPE_BT_VOICE_DL)|| (type == AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL)){
                g_main_dl_use_high_res = true;
            }
            dsp_controller.flag_dac = isEnable;
        }
    #if !defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ) && !defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
        if (type == AUDIO_MESSAGE_TYPE_PROMPT) {
            dsp_controller.flag_vp = isEnable;
        }
    #endif
    }else {
    #if !defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ) && !defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
        if (type == AUDIO_MESSAGE_TYPE_PROMPT) {
            dsp_controller.flag_vp = isEnable;
        }
    #endif
        if ((dsp_controller.hi_res_on_dl == TRUE) && (gAudio_dl_clkmux_rate[hal_audio_dl_clkmux_type_convert_index(type)] > 48000)) {
    #if !defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ) && !defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
            if (type != AUDIO_MESSAGE_TYPE_PROMPT) {
    #endif
                dsp_controller.flag_dac = isEnable;
    #if !defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ) && !defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
            }
    #endif
            //Close dl clkmux when vp and dac user all closed.
    #if defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)||defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
            if (dsp_controller.flag_dac == false) {
    #else
            if ((dsp_controller.flag_dac == false) && (dsp_controller.flag_vp == false)) {
    #endif
            #if defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
                if((type == AUDIO_MESSAGE_TYPE_BT_AUDIO_DL) || (type == AUDIO_MESSAGE_TYPE_LINEIN) || (type == AUDIO_MESSAGE_TYPE_PLAYBACK) || (type == AUDIO_MESSAGE_TYPE_BT_VOICE_DL)|| (type == AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL)){
                    if(g_main_dl_use_high_res){
                        audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount--;
                        g_main_dl_use_high_res = false;
                    }
                }else{
            #endif
                audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount--;
            #if defined(AIR_FIXED_SUB_DL_HIGH_RES_ENABLE)
                }
            #endif
                if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount == 0) {
                    // fs >=96k close hwsrc
                    hal_audio_dsp_hwsrc_clkmux_control(device_rate, isEnable);
                    log_hal_msgid_info("[hi-res]Disable DL", 0);
                    hal_clock_disable(HAL_CLOCK_CG_AUD_DWLINK);//modify for ab1568
                    dsp_controller.hi_res_on_dl = isEnable;
                }
                if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount < 0) {
                    log_hal_msgid_info("Error, [hi-res]Disable DL UserCount < 0", 0);
                    audio_clkmux_status[AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES].mUserCount = 0;
                }

                if (dsp_controller.clk_mux_hires_dl == DL_HIRES_192KHZ) {
                    #if defined(HAL_DVFS_MODULE_ENABLED)
                    #if defined(HAL_DVFS_416M_SOURCE)
                    hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                    log_hal_msgid_info("frequency set back from dl hi-res 0.8V out", 0);
                    #endif
                    #endif
                } else {
                    #if defined(HAL_DVFS_MODULE_ENABLED)
                    #if defined(HAL_DVFS_416M_SOURCE)
                    /* No need to lock DVFS */
                    //hal_dvfs_lock_control( HAL_DVFS_HALF_SPEED_52M , HAL_DVFS_UNLOCK);
                    //log_hal_msgid_info("frequency set back from dl hi-res 0.7V out", 0);
                    #endif
                    #endif
                }
            }
        }
    }
}

void hal_audio_dsp_ul_clkmux_control(hal_audio_device_t in_device, uint32_t device_rate, bool isEnable)
{
    if (isEnable == TRUE) {
        if (((in_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL)||(in_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL)||(in_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL)||(in_device == HAL_AUDIO_DEVICE_I2S_MASTER))
            && (device_rate > 48000)) {
            #if defined(HAL_DVFS_MODULE_ENABLED)
            #if defined(HAL_DVFS_416M_SOURCE)
            hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
            log_hal_msgid_info("frequency is risen to 0.8V for ul hi-res out rate : %d", 1, (int)device_rate);
            #endif
            #endif

            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount == 0) {
                log_hal_msgid_info("[hi-res]Enable UL", 0);
                clock_mux_sel(CLK_AUD_ULCK_SEL, 1); /* MPLL_D3, 208 MHz */ //modify for ab1568
                hal_clock_enable( HAL_CLOCK_CG_AUD_UPLINK);//modify for ab1568
                #ifdef MTK_CLOCK_METER_ENABLE
                //printf("_hf_faud_uplink_ck %d\r\n",hal_clock_get_freq_meter(_hf_faud_uplink_ck,100));
                #endif
                dsp_controller.hi_res_on_ul = isEnable;
            }
            audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount++;
        }
    } else {
        if (dsp_controller.hi_res_on_ul == TRUE) {
            audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount--;
            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount == 0) {
                log_hal_msgid_info("[hi-res]Disable UL", 0);
                hal_clock_disable(HAL_CLOCK_CG_AUD_UPLINK);//modify for ab1568
                dsp_controller.hi_res_on_ul = isEnable;
            }
            if (audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount < 0) {
                log_hal_msgid_info("Error, [hi-res]Disable UL UserCount < 0", 0);
                audio_clkmux_status[AUDIO_CLKMUX_BLOCK_UPLINK_HIRES].mUserCount = 0;
            }

            #if defined(HAL_DVFS_MODULE_ENABLED)
            #if defined(HAL_DVFS_416M_SOURCE)
            hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
            log_hal_msgid_info("frequency set back from ul hi-res 0.8V out", 0);
            #endif
            #endif
        }
    }
}

#if defined(HAL_AUDIO_SUPPORT_APLL)
void hal_audio_i2s_master_in_low_jitter_mode(low_jitter_mode_scenario_t scenario, hal_audio_device_t in_device, uint32_t scenario_device_rate, bool is_low_jitter, bool isEnable)
{
    bool open_close_apll = FALSE;
    uint32_t device_rate = 0;
    if (scenario == LOW_JITTER_MODE_LINEIN_PLAYBACK_IN){
        if (isEnable == TRUE){
            if (in_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_in = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_in = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_in;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_in = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_in == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_in;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_in = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN){
        if (isEnable == TRUE){
            if (in_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_in = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_in = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_in;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_in = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_in == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_in;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_in = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    if (device_rate == 0) {
        open_close_apll = FALSE;
    }
    if (open_close_apll == TRUE) {
        hal_audio_apll_enable(isEnable, device_rate);
    }
}
#endif

#if defined(HAL_AUDIO_SUPPORT_APLL)
void hal_audio_i2s_master_out_low_jitter_mode(low_jitter_mode_scenario_t scenario, hal_audio_device_t out_device, uint32_t scenario_device_rate, bool is_low_jitter, bool isEnable)
{
    bool open_close_apll = FALSE;
    uint32_t device_rate = 0;
    if (scenario == LOW_JITTER_MODE_LINEIN_PLAYBACK_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_out = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_linein_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_linein_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_out = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_pure_linein_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_pure_linein_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_BT_CODEC_A2DP_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_bt_codec_a2dp_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_bt_codec_a2dp_out = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_bt_codec_a2dp_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_bt_codec_a2dp_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_bt_codec_a2dp_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_bt_codec_a2dp_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_bt_codec_a2dp_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_HAL_AUDIO_NOTIFY_ANC_AFE_SETTINGS_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_hal_audio_notify_anc_afe_settings_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_hal_audio_notify_anc_afe_settings_out = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_hal_audio_notify_anc_afe_settings_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_hal_audio_notify_anc_afe_settings_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_hal_audio_notify_anc_afe_settings_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_hal_audio_notify_anc_afe_settings_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_hal_audio_notify_anc_afe_settings_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_MP3_CODEC_PLAY_AVM_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_mp3_codec_play_avm_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_mp3_codec_play_avm_out = scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_mp3_codec_play_avm_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_mp3_codec_play_avm_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_mp3_codec_play_avm_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_mp3_codec_play_avm_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_mp3_codec_play_avm_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    else if (scenario == LOW_JITTER_MODE_USB_AUDIO_PLAYBACK_OUT){
        if (isEnable == TRUE){
            if (out_device == HAL_AUDIO_DEVICE_I2S_MASTER && (is_low_jitter == TRUE || scenario_device_rate > 48000)) {
                dsp_controller.is_low_jitter_scenario.is_low_jitter_usb_audio_playback_out = TRUE;
                dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_usb_audio_playback_out= scenario_device_rate;
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_usb_audio_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_usb_audio_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        } else {
            if (dsp_controller.is_low_jitter_scenario.is_low_jitter_usb_audio_playback_out == TRUE) {
                device_rate = dsp_controller.low_jitter_mode_sampling_rate_scenario.low_jitter_mode_sampling_rate_usb_audio_playback_out;
                open_close_apll = TRUE;
            } else{
                dsp_controller.is_low_jitter_scenario.is_low_jitter_usb_audio_playback_out = FALSE;
                open_close_apll = FALSE;
            }
        }
    }
    if (device_rate == 0) {
        open_close_apll = FALSE;
    }
    if (open_close_apll == TRUE) {
        hal_audio_apll_enable(isEnable, device_rate);
    }
}
#endif

void hal_audio_dsp_controller_send_message(uint16_t message, uint16_t data16, uint32_t data32, bool wait)
{
    uint16_t i;
    uint8_t ID_waiting = 0;

#ifdef HAL_DVFS_MODULE_ENABLED
    uint16_t flag_start = 0;
    uint32_t freq_result;
    freq_result = hal_dvfs_get_cpu_frequency();
    if(freq_result < (uint32_t)156000)
    {
        if(MSG_MCU2DSP_BT_AUDIO_DL_START == message)
            flag_start = 1;
        else if (MSG_MCU2DSP_BT_VOICE_UL_START == message)
            flag_start = 1;
        else if (MSG_MCU2DSP_BT_VOICE_DL_START == message)
            flag_start = 1;
        else if (MSG_MCU2DSP_PLAYBACK_START == message)
            flag_start = 1;
        else if (MSG_MCU2DSP_PROMPT_START == message)
            flag_start = 1;
        else
            flag_start = 0;
    }
#endif /*HAL_DVFS_MODULE_ENABLED*/

#ifdef HAL_DVFS_MODULE_ENABLED
    if(flag_start)
    {
        #if defined(HAL_DVFS_416M_SOURCE)
        #if defined(HAL_DVFS_416M_SOURCE)
        hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_LOCK);
        //log_hal_msgid_info("frequency is risen to 1.3V", 0);
        #endif
        #endif
    }
#endif /*HAL_DVFS_MODULE_ENABLED*/

    if (wait) {
        switch (message) {
            case MSG_MCU2DSP_BT_AUDIO_DL_OPEN:
            case MSG_MCU2DSP_BT_AUDIO_DL_CLOSE:
            case MSG_MCU2DSP_BT_AUDIO_DL_START:
            case MSG_MCU2DSP_BT_AUDIO_DL_STOP:
                dsp_controller.waiting_A2DP = true;
                ID_waiting = 1<<3;
                break;
            case MSG_MCU2DSP_BT_VOICE_UL_OPEN:
            case MSG_MCU2DSP_BT_VOICE_UL_CLOSE:
            case MSG_MCU2DSP_BT_VOICE_UL_START:
            case MSG_MCU2DSP_BT_VOICE_UL_STOP:
            case MSG_MCU2DSP_BT_VOICE_DL_OPEN:
            case MSG_MCU2DSP_BT_VOICE_DL_CLOSE:
            case MSG_MCU2DSP_BT_VOICE_DL_START:
            case MSG_MCU2DSP_BT_VOICE_DL_STOP:
            case MSG_MCU2DSP_BT_VOICE_UL_SUSPEND:
            case MSG_MCU2DSP_BT_VOICE_UL_RESUME:
            case MSG_MCU2DSP_BT_VOICE_DL_SUSPEND:
            case MSG_MCU2DSP_BT_VOICE_DL_RESUME:
                dsp_controller.waiting_ESCO = true;
                ID_waiting = 1<<4;
                break;
            case MSG_MCU2DSP_RECORD_OPEN:
            case MSG_MCU2DSP_RECORD_CLOSE:
            case MSG_MCU2DSP_RECORD_START:
            case MSG_MCU2DSP_RECORD_STOP:
            case MSG_MCU2DSP_RECORD_LC_SET_PARAM:
                dsp_controller.waiting_RECORD = true;
                ID_waiting = 1<<0;
                break;
            case MSG_MCU2DSP_PROMPT_OPEN:
            case MSG_MCU2DSP_PROMPT_CLOSE:
            case MSG_MCU2DSP_PROMPT_START:
            case MSG_MCU2DSP_PROMPT_STOP:
            case MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_OPEN:
            case MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CLOSE:
            case MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_START:
            case MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_STOP:
                dsp_controller.waiting_VP = true;
                ID_waiting = 1<<1;
                break;
            case MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_GET:
            case MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_SET:
                dsp_controller.waiting_Adapt_ANC = true;
                ID_waiting = 1<<5;
                break;
            case MSG_MCU2DSP_COMMON_POWER_OFF_DAC_IMMEDIATELY:
                dsp_controller.waiting_DAC_OFF = true;
                ID_waiting = 1 << 6;
                break;
            default:
                dsp_controller.waiting = true;
                ID_waiting = 1<<2;
                break;
        }
    }

    hal_audio_put_message_via_ccni(message, data16, data32);

    if (wait) {
        for (i=0; ; i++) {
            if(ID_waiting == 0x04){
                if (dsp_controller.waiting == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            }else if(ID_waiting == 0x02){
                if (dsp_controller.waiting_VP == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            }else if(ID_waiting == 0x01){
                if (dsp_controller.waiting_RECORD == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            }else if(ID_waiting == 0x08){
                if (dsp_controller.waiting_A2DP == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            } else if(ID_waiting == 0x10){
                if (dsp_controller.waiting_ESCO == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            } else if(ID_waiting == 0x20){
                if (dsp_controller.waiting_Adapt_ANC == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            } else if (ID_waiting == (1 << 6)) {
                if (dsp_controller.waiting_DAC_OFF == false) {
#if defined(HAL_DVFS_MODULE_ENABLED)
#if defined(HAL_DVFS_416M_SOURCE)
                    if(flag_start)
                    {
                          hal_dvfs_lock_control( HAL_DVFS_FULL_SPEED_104M, HAL_DVFS_UNLOCK);
                          freq_result = hal_dvfs_get_cpu_frequency();
                          //log_hal_msgid_info("frequency is set back %d", 1, (int)freq_result);
                    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
#endif /*HAL_DVFS_416M_SOURCE*/
                    break;
                }
            }
#if !defined(HAL_AUDIO_DEBUG)
            assert(i < 40);
#else
            if ((i % 1000) == 0) {
                log_hal_msgid_info("[HAL audio] Wait msg(0x%x) ack %d \r\n", 2, message, (int)i);
                if(i == 1000){
                    log_hal_msgid_info("DSP_no_ack_response_to_CCNI", 0);
                    assert(0);
                    }
            }
#endif

            hal_audio_delay_ms(2);
        }

#if defined(HAL_AUDIO_DEBUG)
        log_hal_msgid_info("[HAL audio] Ack %x, wait count %d \r\n", 2, message, i);
#endif

    }

}

void *hal_audio_dsp_controller_put_paramter(const void *p_param_addr, uint32_t param_size, audio_message_type_t msg_type)
{
    // Check the size of parameter
    // Copy paramter to share buffer
    if( param_size > (SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE >> 2) ){
        log_hal_msgid_info("Hal_audio_put_param_over_size", 0);
        assert(0);
        return NULL;
    }

    uint32_t offset = 0;
    const uint32_t channel_index = (SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE >> 2) >> 2;
    if(msg_type == AUDIO_MESSAGE_TYPE_PROMPT){
        if(dsp_controller.waiting_VP == true){
        log_hal_msgid_info("Hal_audio_put_param_VP_busy", 0);
        assert(0);
        }
        offset = 1 * channel_index;
    }
    else if(msg_type == AUDIO_MESSAGE_TYPE_RECORD){
        if(dsp_controller.waiting_RECORD == true){
        log_hal_msgid_info("Hal_audio_put_param_RECORD_busy", 0);
        assert(0);
        }
        offset = 2 * channel_index;
    }
    else if(msg_type == AUDIO_MESSAGE_TYPE_BT_AUDIO_DL){
        if(dsp_controller.waiting_A2DP == true){
        log_hal_msgid_info("Hal_audio_put_param_A2DP_busy", 0);
        assert(0);
        }
        offset = 3 * channel_index;
    }
    else if(msg_type < AUDIO_MESSAGE_TYPE_MAX){
        if(dsp_controller.waiting == true){
        log_hal_msgid_info("Hal_audio_put_param_AM_busy", 0);
        assert(0);
        }
        offset = 0;
    }
    else {
        log_hal_msgid_info("Hal_audio_put_param_msg_type_error", 0);
        assert(0);
        return NULL;
    }

    memcpy(audio_share_buffer.mcu2dsp_param + offset, p_param_addr, param_size);
    return (void *)(audio_share_buffer.mcu2dsp_param + offset);
}

//== OS task related ==
void hal_audio_set_task_notification_callback(hal_audio_notify_task_callback_t callback)
{
    audio_isr.audio_event_task_callback = callback;
}

void hal_audio_set_task_ms_delay_function(hal_audio_task_ms_delay_function_t delay_func)
{
    audio_isr.task_ms_delay_func = delay_func;
}

//== Audio Service related ==
void hal_audio_service_hook_callback(audio_message_type_t type, hal_audio_callback_t callback, void *user_data)
{
    uint32_t savedmask;

    if (type >= AUDIO_MESSAGE_TYPE_MAX){
        log_hal_msgid_info("Hal_audio_service_hook_callback_error", 0);
        assert(0);
        return;
    }

    hal_nvic_save_and_set_interrupt_mask(&savedmask);

    audio_isr.flag |= (1<<type);
    audio_isr.callback[type] = callback;
    audio_isr.user_data[type] = user_data;

    hal_nvic_restore_interrupt_mask(savedmask);
}

void hal_audio_service_unhook_callback(audio_message_type_t type)
{
    uint32_t savedmask;

    if (type >= AUDIO_MESSAGE_TYPE_MAX){
        log_hal_msgid_info("Hal_audio_service_unhook_unkown_msg_type", 0);
        assert(0);
        return;
    }
    if ( !(audio_isr.flag & (1<<type)) ){
        log_hal_msgid_info("Hal_audio_service_unhook_flag_error", 0);
        assert(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&savedmask);

    audio_isr.flag &= ~(1<<type);
    audio_isr.callback[type] = NULL;
    audio_isr.user_data[type] = NULL;

    hal_nvic_restore_interrupt_mask(savedmask);
}


//== Hardware semaphore ==
#define MAX_HW_SEMA_RETRY_COUNT 100
#define HW_SEMAPHORE_AUDIO_CM4_DSP0_PLAYBACK 6

static void hal_audio_take_hw_semaphore(uint32_t *p_int_mask)
{
#ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED
    uint32_t take_count = 0;

    //hal_nvic_save_and_set_interrupt_mask(p_int_mask);

    while(++take_count)
    {
        hal_nvic_save_and_set_interrupt_mask(p_int_mask);    /*change for System Checking*/
        if (HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_take(HW_SEMAPHORE_AUDIO_CM4_DSP0_PLAYBACK))
            break;
        if (take_count > MAX_HW_SEMA_RETRY_COUNT)
        {
            hal_nvic_restore_interrupt_mask( *p_int_mask );

            //error handling
            log_hal_msgid_info("[Aud] Can not take HW Semaphore", 0);
            assert(0);
        }
        hal_nvic_restore_interrupt_mask( *p_int_mask );    /*change for System Checking*/
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) {
            hal_audio_delay_ms(2);
        } else {
            hal_gpt_delay_us(10);
        }
    }
#endif
}

static void hal_audio_give_hw_semaphore(uint32_t int_mask)
{
#ifdef HAL_HW_SEMAPHORE_MODULE_ENABLED
    if (HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_give(HW_SEMAPHORE_AUDIO_CM4_DSP0_PLAYBACK))
    {
        hal_nvic_restore_interrupt_mask( int_mask );
    } else {
        hal_nvic_restore_interrupt_mask( int_mask );

        //error handling
        log_hal_msgid_info("[Aud] Can not give HW Semaphore", 0);
        assert(0);
    }
#endif
}

//== Buffer management related ==
uint32_t hal_audio_buf_mgm_get_data_byte_count(n9_dsp_share_info_t *p_info)
{
    uint32_t read, write, data_byte_count;
    uint32_t int_mask;

    hal_audio_take_hw_semaphore( &int_mask );

    if (p_info->bBufferIsFull) {
        hal_audio_give_hw_semaphore(int_mask);
        return p_info->length;
    }

    read = p_info->read_offset;
    write = p_info->write_offset;

    if (write >= read) {
        data_byte_count = write - read;
    } else {
        data_byte_count = p_info->length - read + write;
    }

    hal_audio_give_hw_semaphore(int_mask);

    return data_byte_count;
}

uint32_t hal_audio_buf_mgm_get_free_byte_count(n9_dsp_share_info_t *p_info)
{
    uint32_t data_byte_count, free_byte_count;

    if (p_info->bBufferIsFull) {
        return 0;
    }

    data_byte_count = hal_audio_buf_mgm_get_data_byte_count(p_info);
    free_byte_count = p_info->length - data_byte_count;

    return free_byte_count;
}

void hal_audio_buf_mgm_get_free_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count)
{
    uint32_t read, write, segment;
    uint32_t int_mask;

    hal_audio_take_hw_semaphore( &int_mask );

    read = p_info->read_offset;
    write = p_info->write_offset;

    if (p_info->bBufferIsFull) {
        *pp_buffer = (uint8_t *)(p_info->start_addr + write);
        *p_byte_count = 0;
    } else {
        if (write >= read) {
            segment = p_info->length - write;
        } else {
            segment = read - write;
        }

        *pp_buffer = (uint8_t *)(p_info->start_addr + write);
        *p_byte_count = segment;
    }

    hal_audio_give_hw_semaphore(int_mask);
}

void hal_audio_buf_mgm_get_data_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count)
{
    uint32_t read, write, segment;
    uint32_t int_mask;

    hal_audio_take_hw_semaphore( &int_mask );

    read = p_info->read_offset;
    write = p_info->write_offset;
    if ((read == write) && (p_info->bBufferIsFull == true)) {
        segment = p_info->length - read;
    } else if (write >= read) {
        segment = write - read;
    } else {
        segment = p_info->length - read;
    }

    *pp_buffer = (uint8_t *)(p_info->start_addr + read);
    *p_byte_count = segment;

    hal_audio_give_hw_semaphore(int_mask);
}

void hal_audio_buf_mgm_get_write_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count)
{
    uint32_t write;
    uint32_t int_mask;

    hal_audio_take_hw_semaphore( &int_mask );

    write = p_info->write_offset;
    write += byte_count;
    if (write >= p_info->length) {
        write -= p_info->length;
    }
    p_info->write_offset = write;

    if ((p_info->write_offset == p_info->read_offset) && (byte_count)) {
        p_info->bBufferIsFull = 1;
    }

    hal_audio_give_hw_semaphore(int_mask);
}

void hal_audio_buf_mgm_get_read_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count)
{
    uint32_t read;
    uint32_t int_mask;

    hal_audio_take_hw_semaphore( &int_mask );

    read = p_info->read_offset;
    read += byte_count;
    if (read >= p_info->length) {
        read -= p_info->length;
    }
    p_info->read_offset = read;

    if((p_info->bBufferIsFull == 1) && (byte_count != 0))
    {
        p_info->bBufferIsFull = 0;
    }

    hal_audio_give_hw_semaphore(int_mask);
}

//== Share buffer ==

#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
uint32_t *hal_audio_query_ANC_monitor_share_info(void)
{
    return (uint32_t*)&g_ANC_monitor_share_buffer;
}
#endif

#ifdef MTK_BT_SPEAKER_ENABLE
uint8_t *hal_audio_query_fec_share_info(void)
{
    return (uint8_t *)&fec_share_buffer;
}
#endif

n9_dsp_share_info_t *hal_audio_query_bt_audio_dl_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_AUDIO_DL];
}

n9_dsp_share_info_t *hal_audio_query_bt_voice_ul_share_info(void)
{
#ifdef AVM_SHAEE_BUF_INFO
    hal_audio_set_forwarder_addr((avm_share_buf_info_t *)&n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL], FALSE);
#endif
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL];
}

n9_dsp_share_info_t *hal_audio_query_bt_voice_dl_share_info(void)
{
#ifdef AVM_SHAEE_BUF_INFO
    hal_audio_set_forwarder_addr((avm_share_buf_info_t *)&n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL], TRUE);
#endif
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL];
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
n9_dsp_share_info_t *hal_audio_query_ble_audio_ul_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_UL];
}

n9_dsp_share_info_t *hal_audio_query_ble_audio_dl_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_DL];
}

n9_dsp_share_info_t *hal_audio_query_ble_audio_sub_ul_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_UL];
}

n9_dsp_share_info_t *hal_audio_query_ble_audio_sub_dl_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_DL];
}

#endif

n9_dsp_share_info_t *hal_audio_query_playback_share_info(void)
{
    //ToDo: reuse BT audio DL buffer
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_AUDIO_DL];
}

n9_dsp_share_info_t *hal_audio_query_record_share_info(void)
{
    //ToDo: currently, there is not dedicated buffer for recording.
    //ToDo: reuse BT audio DL buffer
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_RECORD];
}

uint32_t *hal_audio_query_rcdc_share_info(void)
{
    return &audio_share_buffer.clk_info[0];
}

uint32_t *hal_audio_query_ull_rcdc_share_info(void)
{
    return &audio_share_buffer.ull_clk_info[0];
}

uint32_t *hal_audio_query_hfp_air_dump(void)
{
    return &audio_share_buffer.airdump[0];
}

n9_dsp_audio_sync_info_t *hal_audio_query_audio_sync_info(void)
{
    return (n9_dsp_audio_sync_info_t*)&audio_share_buffer.audio_sync_info[0];
}

uint32_t *hal_audio_afe_dl_buf_report(void)
{
    return (uint32_t *)&n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL].drift_comp_val;
}

n9_dsp_share_info_t *hal_audio_query_prompt_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_PROMPT];
}

n9_dsp_share_info_t *hal_audio_query_nvkey_parameter_share_info(void)
{
    return &n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_NVKEY_PARAMETER];
}

n9_dsp_share_info_t *hal_audio_query_audio_transmitter_share_info(audio_transmitter_share_info_index_t index)
{
    return &audio_transmitter_share_info[index];
}

uint32_t *hal_audio_query_share_info(audio_message_type_t type)
{
    switch (type) {
        case AUDIO_MESSAGE_TYPE_BT_AUDIO_DL:
            return (uint32_t *)hal_audio_query_bt_audio_dl_share_info();
#ifdef AIR_BT_CODEC_BLE_ENABLED
        case AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL:
            return (uint32_t *)hal_audio_query_ble_audio_dl_share_info();
        case AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL:
            return (uint32_t *)hal_audio_query_ble_audio_ul_share_info();
        case AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_DL:
            return (uint32_t *)hal_audio_query_ble_audio_sub_dl_share_info();
        case AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_UL:
            return (uint32_t *)hal_audio_query_ble_audio_sub_ul_share_info();
#endif
        case AUDIO_MESSAGE_TYPE_BT_VOICE_UL:
            return (uint32_t *)hal_audio_query_bt_voice_ul_share_info();
        case AUDIO_MESSAGE_TYPE_BT_VOICE_DL:
            return (uint32_t *)hal_audio_query_bt_voice_dl_share_info();
        case AUDIO_MESSAGE_TYPE_PLAYBACK:
            return (uint32_t *)hal_audio_query_playback_share_info();
        case AUDIO_MESSAGE_TYPE_RECORD:
            return (uint32_t *)hal_audio_query_record_share_info();
        case AUDIO_MESSAGE_TYPE_PROMPT:
            return (uint32_t *)hal_audio_query_prompt_share_info();
        case AUDIO_RESERVE_TYPE_QUERY_RCDC:
            return hal_audio_query_rcdc_share_info();
        case AUDIO_RESERVE_TYPE_ULL_QUERY_RCDC:
            return hal_audio_query_ull_rcdc_share_info();
#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
        case AUDIO_MESSAGE_TYPE_ANC_MONITOR_ADAPTIVE_ANC:
            return (uint32_t *)hal_audio_query_ANC_monitor_share_info();
#endif
        default:
            return NULL;

    }
}
#ifdef AVM_SHAEE_BUF_INFO


#define AVM_HEADER_LEN 12
void hal_audio_set_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize, uint32_t sink_latnecy)
{
    uint16_t i;
    p_info->ReadIndex  = 0;
    p_info->WriteIndex = 0;
    p_info->MemBlkSize = ((blksize + AVM_HEADER_LEN + 7)>>3)<<3;
    p_info->MemBlkNum  = (uint16_t)(buf_len/(p_info->MemBlkSize));
    p_info->SinkLatency = sink_latnecy;
    for (i = 0; i < p_info->MemBlkNum; i++)
    {
        memset((void *)(p_info->StartAddr + i*p_info->MemBlkSize),0,AVM_HEADER_LEN);
    }
}

void hal_audio_set_hfp_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize)
{
    p_info->ReadIndex  = 0;
    p_info->WriteIndex = 0;
    p_info->MemBlkSize = blksize;
    p_info->MemBlkNum  = (uint16_t)(buf_len/(p_info->MemBlkSize));
}

void hal_audio_set_forwarder_addr(avm_share_buf_info_t *p_info, bool isRx)
{
    if(isRx == TRUE){
        p_info->ForwarderAddr = (uint32_t)bt_pka_get_esco_forwarder_addr(SHARE_BUFFER_SYSRAM3_AUDIO_FORWARDER_BUF_SIZE);
    } else {
        p_info->ForwarderAddr = (uint32_t)bt_pka_get_esco_forwarder_addr(SHARE_BUFFER_SYSRAM3_AUDIO_FORWARDER_BUF_SIZE) + SHARE_BUFFER_SYSRAM3_RX_AUDIO_FORWARDER_BUF_SIZE;
    }
}

void hal_audio_set_gaming_mode_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize)
{
    p_info->ReadIndex  = 0;
    p_info->WriteIndex = 0;
    p_info->MemBlkSize = blksize;
    p_info->MemBlkNum  = (uint16_t)(buf_len/(p_info->MemBlkSize));
}
#endif

void hal_audio_set_audio_transmitter_share_info(n9_dsp_share_info_t *p_share_info, uint32_t buf_len, uint32_t max_payload_size){
    uint16_t blk_num, blk_size;
    blk_size =(max_payload_size + 4 + 3)/4*4;  //add 4 byte header, 4B align
    blk_num = buf_len / blk_size;
    p_share_info->next = blk_size + (blk_num << 16);
    p_share_info->length = blk_size * blk_num;
}

void hal_audio_a2dp_reset_share_info(n9_dsp_share_info_t *p_info )
{
    p_info->read_offset = 0;
    p_info->next = 0;
    p_info->sampling_rate = 0;
    //p_info->length = 0;
    p_info->notify_count = 0;
    p_info->drift_comp_val = 0;
    p_info->anchor_clk = 0;

    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_SYSRAM, 0, (uint32_t)&audio_share_buffer.nvkey_param[0], false);
}

void hal_audio_set_a2dp_play_en(uint32_t seq_no, uint32_t bt_clk)
{
    audio_play_en_info.sequence_number = seq_no;
    audio_play_en_info.bt_clock = bt_clk;
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_PLAY_EN_FROM_BTCON, 0, (uint32_t)&audio_play_en_info, false);
}

#ifdef AIR_BT_CODEC_BLE_ENABLED

void hal_audio_reset_le_audio_info(void)
{
    n9_dsp_share_info_t *p_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
    memset((void *)p_info->start_addr, 0, SHARE_BUFFER_BLE_AUDIO_DL_SIZE);
    p_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL);
    memset((void *)p_info->start_addr, 0, SHARE_BUFFER_BLE_AUDIO_UL_SIZE);
    p_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_UL);
    if (SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE != 0)
    {
        memset((void *)p_info->start_addr, 0, SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE);
    }
    p_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BLE_AUDIO_SUB_DL);
    memset((void *)p_info->start_addr, 0, SHARE_BUFFER_BLE_AUDIO_SUB_DL_SIZE);
}
#endif

void hal_audio_reset_share_info(n9_dsp_share_info_t *p_info)
{
    p_info->write_offset = 0;
    p_info->bBufferIsFull = 0;
    hal_audio_a2dp_reset_share_info(p_info);
}

void hal_audio_set_sysram(void)
{
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_SYSRAM, 0, (uint32_t)&audio_share_buffer.nvkey_param[0], false);
}

uint32_t *hal_audio_query_ltcs_asi_buf(void)
{
    return (uint32_t*)(4 * ((n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL].start_addr + 3) / 4));
}

uint32_t *hal_audio_query_ltcs_min_gap_buf(void)
{
    return (uint32_t*)(4 * ((n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL].start_addr + 4) / 4));
}

uint32_t *hal_audio_query_ltcs_anchor_info_buf(void)
{
    return &audio_share_buffer.tx_audio_forwarder_buf[0];
}

uint32_t *hal_audio_query_race_cmd_audio_buf(void)
{
    return &audio_share_buffer.race_cmd_audio_buf[0];
}

uint32_t *hal_audio_report_bitrate_buf(void)
{
    return (uint32_t*)(4 * ((n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL].start_addr + n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL].length - 8) / 4));
}

uint32_t *hal_audio_report_lostnum_buf(void)
{
    return &(n9_dsp_share_info[SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL].next);
}

/**
 * @ Write audio drift to DSP.
 * @ val : updated drift value.
 * @ Retval HAL_AUDIO_STATUS_OK if operation is successful, others if failed.
 */
hal_audio_status_t hal_audio_write_audio_drift_val(int32_t val)
{
    ltcs_anchor_info_t *p_info = (ltcs_anchor_info_t *)hal_audio_query_ltcs_anchor_info_buf();

    p_info->drift_comp_val = val;

    return HAL_AUDIO_STATUS_OK;
}

/**
 * @ Write audio anchor to DSP.
 * @ val : updated drift value.
 * @ Retval HAL_AUDIO_STATUS_OK if operation is successful, others if failed.
 */
hal_audio_status_t hal_audio_write_audio_anchor_clk(uint32_t val)
{
    ltcs_anchor_info_t *p_info = (ltcs_anchor_info_t *)hal_audio_query_ltcs_anchor_info_buf();

    p_info->anchor_clk = val;

    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_write_audio_asi_base(uint32_t val)
{
    ltcs_anchor_info_t *p_info = (ltcs_anchor_info_t *)hal_audio_query_ltcs_anchor_info_buf();

    p_info->asi_base = val;

    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_write_audio_asi_cur(uint32_t val)
{
    ltcs_anchor_info_t *p_info = (ltcs_anchor_info_t *)hal_audio_query_ltcs_anchor_info_buf();

    p_info->asi_cur = val;

    return HAL_AUDIO_STATUS_OK;
}

extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;

bool hal_audio_is_a2dp_only(audio_message_type_t type) {
    if(((type == AUDIO_MESSAGE_TYPE_BT_AUDIO_DL) || (type == AUDIO_MESSAGE_TYPE_AFE)) &&\
    ((0 == (dsp_controller.running & (~((1 << AUDIO_MESSAGE_TYPE_BT_AUDIO_DL) | (1 << AUDIO_MESSAGE_TYPE_AFE))))))) {
        return true;
    }
    return false;
}

//== Status control ==
void hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running)
{
    log_hal_msgid_info("[hal_audio_status_set_running_flag] typr=%d, dsp_enable=%d, enable=%d\r\n",3, type, dsp_controller.running, is_running);
    if (is_running) {
        if(dsp_controller.running ==0){
        switch(audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Mode_Sel)
        {
            //Class AB
            case 0x1:
                pmu_set_audio_enhance(PMU_OFF);
                break;
            //Class D
            case 0x2:
                //Earbuds set to 1.71V for mono cases, headset set to 1.8V for stereo cases
                #ifdef MTK_AWS_MCE_ENABLE
                pmu_set_vaud18_voltage(PMIC_VAUD18_1P71_V);
                #else
                pmu_set_vaud18_voltage(PMIC_VAUD18_1P80_V);
                #endif
                pmu_set_audio_enhance(PMU_OFF);
                break;
            //Class G
            case 0x0:
            default:
                pmu_set_vaud18_voltage(PMIC_VAUD18_1P80_V);
                pmu_set_audio_enhance(PMU_ON);
                break;
        }
            hal_rtc_switch_to_dcxo(HAL_RTC_CLOCK_USER_AUDIO, true);
            #if 1
            hal_clock_enable(HAL_CLOCK_CG_AUD_INTBUS);
            hal_clock_enable(HAL_CLOCK_CG_AUD_GPSRC);
            #endif
            //pmu_set_register_value(PMU_BUCK_VAUD18_OP_MODE, 0x3, 0, 0x3);
#ifdef HAL_SLEEP_MANAGER_ENABLED
            spm_control_mtcmos(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_ENABLE);
            /* force infra on,force xo,sys off keep 0,ignore dsp0/1 active,skip sfc/emi backup */
#ifdef HAL_AUDIO_POWER_SLIM_ENABLED
            if((type == AUDIO_MESSAGE_TYPE_ANC) || (type == AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                ||(type == AUDIO_MESSAGE_TYPE_LINEIN)
#endif
            ){
                /*SLIM POWER enable audio scenario*/
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, Lock_S4", 1, type);
                spm_audio_lowpower_setting(SPM_STATE4,SPM_ENABLE);
            } else {
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, Lock_S1", 1, type);
                spm_audio_lowpower_setting(SPM_STATE1,SPM_ENABLE);
            }
#else
            spm_audio_lowpower_setting(SPM_STATE1,SPM_ENABLE);
#endif
#endif
#ifdef HAL_PMU_MODULE_ENABLED
#ifndef AIR_AUDIO_EXT_DAC_ENABLE
            pmu_enable_power(PMU_BUCK_VAUD18, PMU_ON);
#endif
#endif
            hal_clock_set_running_flags(0x44001FC,true);
            //*((volatile uint32_t*)(0xA2030B20)) = 0x44001FC;    //clock CG on [TEMP, should call clock APIs]

            //pdn
            *((volatile uint32_t*)(0x70000000)) = 0xBFFDC338; // Default:0xB0FDC038  LowPowerSetting:0x1F0C8304;//0xBFFDC33C;    // pdn AUDIO_TOP_CON0
            *((volatile uint32_t*)(0x70000004)) = 0x709b000f; // pdn AUDIO_TOP_CON1
#ifdef HAL_AUDIO_POWER_SLIM_ENABLED
        } else {
            if(((type != AUDIO_MESSAGE_TYPE_ANC) && (type != AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                && (type != AUDIO_MESSAGE_TYPE_LINEIN)
#endif
                ) && (0 == (dsp_controller.running & (~( (1 << AUDIO_MESSAGE_TYPE_ANC)|(1 << AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                |(1 << AUDIO_MESSAGE_TYPE_LINEIN)
#endif
            ))))){
                /*MAIN POWER enable audio scenario*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, Lock_S1", 1, type);
                spm_audio_lowpower_setting(SPM_STATE1,SPM_ENABLE);
#endif
            } else if(((type == AUDIO_MESSAGE_TYPE_ANC) || (type == AUDIO_MESSAGE_TYPE_AFE)) && (0 == (dsp_controller.running & ((1 << AUDIO_MESSAGE_TYPE_ANC)|(1 << AUDIO_MESSAGE_TYPE_AFE))))){
                /*SLIM POWER enable audio scenario*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, Lock_S4", 1, type);
                spm_audio_lowpower_setting(SPM_STATE4,SPM_ENABLE);
#endif
            }
#endif
        }
        dsp_controller.running |= (1<<type);

#if 0
        if(hal_audio_is_a2dp_only(type)) {
            //unlock dsp sleep
            *((volatile uint32_t*)(0xA2110B04)) &= 0x7FFFFFFF; /* use bit31 */
        } else {
            //lock dsp sleep
            *((volatile uint32_t*)(0xA2110B04)) |= 0x80000000; /* use bit31 */
        }
#endif

    } else {
        if(dsp_controller.running == 0) {
            log_hal_msgid_info("[hal_audio_status_set_running_flag] Dummy set running flag flow\r\n",0);
            return;
        }
        dsp_controller.running &= ~(1<<type);
        if(dsp_controller.running ==0){
            #if 1
            hal_clock_disable(HAL_CLOCK_CG_AUD_INTBUS);
            hal_clock_disable(HAL_CLOCK_CG_AUD_GPSRC);
            pmu_set_audio_enhance(PMU_OFF);
            #endif
            //pmu_set_register_value(PMU_BUCK_VAUD18_OP_MODE, 0x3, 0, 0);
            hal_clock_set_running_flags(0x44001FC,false);
#ifdef HAL_PMU_MODULE_ENABLED
#ifndef AIR_AUDIO_EXT_DAC_ENABLE
            pmu_enable_power(PMU_BUCK_VAUD18, PMU_OFF);
#endif
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
            spm_control_mtcmos(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_DISABLE);
#ifndef HAL_AUDIO_POWER_SLIM_ENABLED
            spm_audio_lowpower_setting(SPM_STATE1,SPM_DISABLE);
#else
            if((type == AUDIO_MESSAGE_TYPE_ANC) || (type == AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                || (type == AUDIO_MESSAGE_TYPE_LINEIN)
#endif
            ){
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, UN_Lock_S4", 1, type);
                spm_audio_lowpower_setting(SPM_STATE4,SPM_DISABLE);
            } else {
                //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, UN_Lock_S1", 1, type);
                spm_audio_lowpower_setting(SPM_STATE1,SPM_DISABLE);
            }
#endif
#endif
            hal_rtc_switch_to_dcxo(HAL_RTC_CLOCK_USER_AUDIO, false);
#ifdef HAL_AUDIO_POWER_SLIM_ENABLED
        } else if(((type == AUDIO_MESSAGE_TYPE_ANC) || (type == AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                || (type == AUDIO_MESSAGE_TYPE_LINEIN)
#endif
            ) && (0 == (dsp_controller.running & ((1 << AUDIO_MESSAGE_TYPE_ANC)|(1 << AUDIO_MESSAGE_TYPE_AFE)
#ifdef HAL_PURE_LINEIN_PLAYBACK_ENABLE
                |(1 << AUDIO_MESSAGE_TYPE_LINEIN)
#endif
        )))){
#ifdef HAL_SLEEP_MANAGER_ENABLED
            /*SLIM POWER disable audio scenario*/
            //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, UN_Lock_S4", 1, type);
            spm_audio_lowpower_setting(SPM_STATE4,SPM_DISABLE);
#endif
        } else if(((type != AUDIO_MESSAGE_TYPE_ANC) && (type != AUDIO_MESSAGE_TYPE_AFE)) && (0 == (dsp_controller.running & (~( (1 << AUDIO_MESSAGE_TYPE_ANC)|(1 << AUDIO_MESSAGE_TYPE_AFE)))))){
            /*Other Audio POWER disable API*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
            //log_hal_msgid_info("[hal_audio_status_set_running_flag] type:%d, UN_Lock_S1", 1, type);
            spm_audio_lowpower_setting(SPM_STATE1,SPM_DISABLE);
#endif
#endif
        }
    }
}

void hal_audio_status_set_notify_flag(audio_message_type_t type, bool is_notify)
{
    if (is_notify) {
        dsp_controller.dsp_notify |= (1<<type);
    } else {
        dsp_controller.dsp_notify &= ~(1<<type);
    }
}

bool hal_audio_status_query_running_flag(audio_message_type_t type)
{
    if (dsp_controller.running & (1<<type)) {
        return true;
    } else {
        return false;
    }
}

bool hal_audio_status_query_running_flag_except(audio_message_type_t type)
{
    if (dsp_controller.running & (~(1<<type))) {
        return true;
    } else {
        return false;
    }
}

bool hal_audio_status_query_notify_flag(audio_message_type_t type)
{
    if (dsp_controller.dsp_notify & (1<<type)) {
        return true;
    } else {
        return false;
    }
}

uint16_t hal_audio_status_query_running_flag_value()
{
    return dsp_controller.running;
}

//== Data path related API (codec will use) ==
hal_audio_status_t hal_audio_write_stream_out_by_type(audio_message_type_t type, const void *buffer, uint32_t size)
{
    //ToDo: limit the scope -- treat it as local playback
    n9_dsp_share_info_t *p_info;
    uint32_t free_byte_count;
    hal_audio_status_t result = HAL_AUDIO_STATUS_OK;
    uint32_t i;
    uint8_t *p_source_buf = (uint8_t *)buffer;
    bool is_notify;
    uint16_t message_ack;

    // Check buffer
    if (buffer == NULL)
        return HAL_AUDIO_STATUS_ERROR;

    // According to type to get share info
    switch (type) {
        case AUDIO_MESSAGE_TYPE_PLAYBACK:
            p_info = hal_audio_query_playback_share_info();
            message_ack = MSG_MCU2DSP_PLAYBACK_DATA_REQUEST_ACK;
            break;
        case AUDIO_MESSAGE_TYPE_PROMPT:
            p_info = hal_audio_query_prompt_share_info();
            message_ack = MSG_MCU2DSP_PROMPT_DATA_REQUEST_ACK;
            break;
        default:
            return HAL_AUDIO_STATUS_ERROR;
    }

    // Check data amount
    free_byte_count = hal_audio_buf_mgm_get_free_byte_count(p_info);
    if (size > free_byte_count) {
        if(type != AUDIO_MESSAGE_TYPE_PLAYBACK)
            return HAL_AUDIO_STATUS_ERROR;
        else
            size = free_byte_count;
    }

    // When free space is enough
    for (i=0; (i<2) && size; i++) {
        uint8_t *p_dest_buf;
        uint32_t buf_size, segment;

        hal_audio_buf_mgm_get_free_buffer(p_info, &p_dest_buf, &buf_size);
        if (size >= buf_size) {
            segment = buf_size;
        } else {
            segment = size;
        }
        memcpy(p_dest_buf, p_source_buf, segment);
        hal_audio_buf_mgm_get_write_data_done(p_info, segment);
        p_source_buf += segment;
        size -= segment;
    }
#if 0//vp debug
    log_hal_msgid_info("[VPC]p_info W(0x%x) R(0x%x) IsFull(%d)\r\n", 3, p_info->start_addr+p_info->read_offset,p_info->start_addr+p_info->write_offset,p_info->bBufferIsFull);
#endif
    // Check status and notify DSP
    is_notify = hal_audio_status_query_notify_flag(type);
    if (is_notify) {
        hal_audio_status_set_notify_flag(type, false);
        hal_audio_dsp_controller_send_message(message_ack, 0, 0, false);
    }

    return result;
}


//== AM task related API ==
//ToDo
void hal_audio_AM_register_callback()
{


}


void hal_audio_am_register_a2dp_open_callback(hal_bt_audio_dl_open_callback_t callback)
{
    audio_isr.bt_audio_dl_open_callback = callback;
}


//== Speech related parameter
//ToDo: currently, I can't confirm the structure and size of parameter
void speech_update_common(const uint16_t *common)
{

}

void speech_update_nb_param(const uint16_t *param)
{

}

void speech_update_wb_param(const uint16_t *param)
{

}

void speech_update_nb_fir(const int16_t *in_coeff, const int16_t *out_coeff)
{

}

void speech_update_wb_fir(const int16_t *in_coeff, const int16_t *out_coeff)
{

}

int32_t audio_update_iir_design(const uint32_t *parameter)
{
    return 0;
}

//== Ring buffer opeartion ==
/*@brief     circular buffer(ring buffer) implemented by mirroring, which keep an extra bit to distinguish empty and full situation. */
uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count(p_info);
}

void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;
    if (write_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {
        *pp_buffer = buffer_pointer + write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;
    if (read_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {
        *pp_buffer = buffer_pointer + read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}

void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer >= buffer_end ? write_pointer - buffer_end : write_pointer;
    return;
}

void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer >= buffer_end ? read_pointer - buffer_end : read_pointer;
    return;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/*@brief     circular buffer(ring buffer) implemented by keeping one slot open. Full buffer has at most (size - 1) slots. */
uint32_t ring_buffer_get_data_byte_count_non_mirroring(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;

    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = buffer_byte_count - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count_non_mirroring(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count_non_mirroring(p_info);
}

void ring_buffer_get_write_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;     //buffer size
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count_non_mirroring(p_info) - 2;  //space two bytes(one word) empty for DSP operation
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;

    tail_byte_count = buffer_byte_count - write_pointer;
    *pp_buffer = buffer_pointer + write_pointer;
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count_non_mirroring(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;

    *pp_buffer = buffer_pointer + read_pointer;
    tail_byte_count = buffer_byte_count - read_pointer;
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}


void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer == buffer_byte_count ? write_pointer - buffer_byte_count : write_pointer;
    return;
}

void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer == buffer_byte_count ? read_pointer - buffer_byte_count : read_pointer;
    return;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

//== Audio sync ==
audio_dsp_a2dp_dl_time_param_t *hal_audio_a2dp_dl_get_time_report(void)
{
    return &audio_sync_time_info;
}


//======== Unit test code ========
#if defined(UNIT_TEST)

#define KH_TOTAL_PCM_THRESHOLE 64*1024

static uint16_t KH_Test_Buffer[10*1024];
static uint16_t KH_serial_number;
static uint32_t KH_total_count;
static bool KH_is_eof;
volatile static  bool KH_is_media_end;

static void hal_audio_unit_test_fill_data(void)
{
    uint32_t i, byte_count, sample_count;
    uint16_t *p_buf = KH_Test_Buffer;

    if (KH_is_eof) {
        // skip
        return;
    }

    // Query data count
    hal_audio_get_stream_out_sample_count(&byte_count);

    // Prepare data
    sample_count = byte_count/2;
    for (i=0; i<sample_count; i++) {
        *p_buf++ = KH_serial_number++;
    }

    // Write to HAL
    hal_audio_write_stream_out(KH_Test_Buffer, sample_count*2);

    KH_total_count += sample_count;

    log_hal_msgid_info("\r\n CM4 UT: total data %d \r\n", 1, KH_total_count);

    if (KH_total_count >= KH_TOTAL_PCM_THRESHOLE) {
        KH_is_eof = true;
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PLAYBACK_CONFIG, AUDIO_PLAYBACK_CONFIG_EOF, 1, false);
        log_hal_msgid_info("\r\n CM4 UT: send EOF \r\n", 0);
    }
}

static void hal_audio_unit_test_isr_handler(hal_audio_event_t event, void *data)
{
    switch (event) {
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            hal_audio_unit_test_fill_data();
            break;
        case HAL_AUDIO_EVENT_END:
            KH_is_media_end = true;
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PLAYBACK_ERROR_ACK, AUDIO_PLAYBACK_CONFIG_EOF, 1, false);
            break;
    }
}

void hal_audio_unit_test(void)
{

    //== PCM open & start
    // Set information
    hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_44_1KHZ);
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);

    // Hook callback
    hal_audio_register_stream_out_callback(hal_audio_unit_test_isr_handler, NULL);

    // Prebuffer
    hal_audio_unit_test_fill_data();

    // Start
    log_hal_msgid_info("\r\n CM4 UT: start stream out ++\r\n", 0);
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    log_hal_msgid_info("\r\n CM4 UT: start stream out --\r\n", 0);

    // Wait for data consume
    do {

    } while (!KH_is_media_end);

    // PCM stop & close
    log_hal_msgid_info("\r\n CM4 UT: stop stream out ++\r\n", 0);
    hal_audio_stop_stream_out();
    log_hal_msgid_info("\r\n CM4 UT: stop stream out --\r\n", 0);
}
#endif // defined(UNIT_TEST)

// for action sync mechanism
cm4_dsp_audio_sync_request_param_t *hal_audio_query_audio_sync_request_param(void)
{
    return (cm4_dsp_audio_sync_request_param_t*)&audio_share_buffer.audio_sync_request_parm[0];
}

#endif /* defined(HAL_AUDIO_MODULE_ENABLED) */
