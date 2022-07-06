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

#ifdef MTK_WWE_ENABLE

#include "dsp_feature_interface.h"
#include "dsp_share_memory.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_audio_process.h"
#include "audio_nvdm_common.h"
#include "dsp_sdk.h"
#include "dsp_dump.h"
#include "wwe_interface.h"
#include "stream_audio_driver.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_msg.h"
#include "hotword_dsp_multi_bank_api.h"
#include "gsound_target_model_format.h"
#include "FreeRTOS.h"
#include "preloader_pisplit.h"
#ifdef MTK_WWE_USE_PIC
#include "wwe_portable.h"
#endif

//#define WWE_STATE_DEBUG
//#define WWE_COMMON_DEBUG
//#define WWE_NVKEY_DEBUG
//#define WWE_PREROLL_DEBUG
#define WWE_DETECT_ONCE

volatile wwe_mode_t g_wwe_mode = WWE_MODE_AMA;
//Preroll will modify codec output size, so record the frame size to a global instead of run time get from void *para
volatile uint32_t g_wwe_frame_size = 0;
static volatile uint16_t g_wwe_skip_frame_size = 0;
static volatile uint16_t g_wwe_skip_frame_size_from_nvkey = 0;
static uint32_t g_wwe_ama_sample_cnt = 0;

volatile bool is_hwvad_enable = false;

#ifdef WWE_PREROLL_DEBUG
#include "dsp_scenario.h"
extern CONNECTION_IF record_if;
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
volatile mcu2dsp_vad_param_p g_mcu2dsp_vad_param = NULL;

static const uint8_t db_1mic_hex[] = {
//#include "db_1mic.bin.hex"
};
static const uint8_t db_2mic_hex[] = {
//#include "db_2mic.bin.hex"
};

void *s_memory_handle = NULL;

void *p_comm_nvkey = NULL;
void *p_nvkey = NULL;
void *p_wwe_dbase = NULL;
uint32_t s_wwe_dbase_length = 0;
char *engineBuffer = NULL;

typedef enum {
    WWE_WORKING_MODE_VAD = 0,
    WWE_WORKING_MODE_CONTINIOUS = 1
} wwe_woking_mode_t;

void wwe_get_db_nvkey(void **p_wwe_dbase, uint32_t *wwe_dbase_length, void **p_comm_nvkey, void **p_nvkey, int mic_num, wwe_woking_mode_t wwe_woking_mode)
{
#ifdef WWE_NVKEY_DEBUG

    extern DSP_NVKEY_VAD_COMM NvKey_COMM;
    extern DSP_NVKEY_VAD_PARA NvKey_1M_CON;
    extern DSP_NVKEY_VAD_PARA NvKey_1M_VAD;
    extern DSP_NVKEY_VAD_PARA NvKey_2M_CON;
    extern DSP_NVKEY_VAD_PARA NvKey_2M_VAD;

    if (memcmp(&g_mcu2dsp_vad_param->vad_nvkey_common, &NvKey_COMM, sizeof(DSP_NVKEY_VAD_COMM))) {
        DSP_MW_LOG_E("[VAD_NVKEY]NvKey_COMM mismatch!", 0);
        uint16_t *key_p = (uint16_t *)&g_mcu2dsp_vad_param->vad_nvkey_common;
        for (uint32_t i = 0; i < (sizeof(DSP_NVKEY_VAD_COMM) >> 1); i++) {
            DSP_MW_LOG_I("[VAD_NVKEY]vad_nvkey_common[%d] = 0x%04x", 2, i, *key_p);
            key_p++;
        }
    } else {
        DSP_MW_LOG_I("[VAD_NVKEY]NvKey_COMM pass!", 0);
    }

    if (memcmp(&g_mcu2dsp_vad_param->vad_nvkey_1mic_v_mode, &NvKey_1M_VAD, sizeof(DSP_NVKEY_VAD_PARA))) {
        DSP_MW_LOG_E("[VAD_NVKEY]NvKey_1M_VAD mismatch!", 0);
        uint16_t *key_p = (uint16_t *)&g_mcu2dsp_vad_param->vad_nvkey_1mic_v_mode;
        for (uint32_t i = 0; i < (sizeof(DSP_NVKEY_VAD_PARA) >> 1); i++) {
            DSP_MW_LOG_I("[VAD_NVKEY]vad_nvkey_1mic_v_mode[%d] = 0x%04x", 2, i, *key_p);
            key_p++;
        }
    } else {
        DSP_MW_LOG_I("[VAD_NVKEY]NvKey_1M_VAD pass", 0);
    }
    if (memcmp(&g_mcu2dsp_vad_param->vad_nvkey_1mic_c_mode, &NvKey_1M_CON, sizeof(DSP_NVKEY_VAD_PARA))) {
        DSP_MW_LOG_E("[VAD_NVKEY]NvKey_1M_CON mismatch!", 0);
        uint16_t *key_p = (uint16_t *)&g_mcu2dsp_vad_param->vad_nvkey_1mic_c_mode;
        for (uint32_t i = 0; i < (sizeof(DSP_NVKEY_VAD_PARA) >> 1); i++) {
            DSP_MW_LOG_I("[VAD_NVKEY]vad_nvkey_1mic_c_mode[%d] = 0x%04x", 2, i, *key_p);
            key_p++;
        }
    } else {
        DSP_MW_LOG_I("[VAD_NVKEY]NvKey_1M_CON pass", 0);
    }
    if (memcmp(&g_mcu2dsp_vad_param->vad_nvkey_2mic_v_mode, &NvKey_2M_VAD, sizeof(DSP_NVKEY_VAD_PARA))) {
        DSP_MW_LOG_E("[VAD_NVKEY]NvKey_2M_VAD mismatch!", 0);
        uint16_t *key_p = (uint16_t *)&g_mcu2dsp_vad_param->vad_nvkey_2mic_v_mode;
        for (uint32_t i = 0; i < (sizeof(DSP_NVKEY_VAD_PARA) >> 1); i++) {
            DSP_MW_LOG_I("[VAD_NVKEY]vad_nvkey_2mic_v_mode[%d] = 0x%04x", 2, i, *key_p);
            key_p++;
        }
    } else {
        DSP_MW_LOG_I("[VAD_NVKEY]NvKey_2M_VAD pass", 0);
    }
    if (memcmp(&g_mcu2dsp_vad_param->vad_nvkey_2mic_c_mode, &NvKey_2M_CON, sizeof(DSP_NVKEY_VAD_PARA))) {
        DSP_MW_LOG_E("[VAD_NVKEY]NvKey_2M_CON mismatch!", 0);
        uint16_t *key_p = (uint16_t *)&g_mcu2dsp_vad_param->vad_nvkey_2mic_c_mode;
        for (uint32_t i = 0; i < (sizeof(DSP_NVKEY_VAD_PARA) >> 1); i++) {
            DSP_MW_LOG_I("[VAD_NVKEY]vad_nvkey_2mic_c_mode[%d] = 0x%04x", 2, i, *key_p);
            key_p++;
        }
    } else {
        DSP_MW_LOG_I("[VAD_NVKEY]NvKey_2M_CON pass", 0);
    }

#endif

    *p_comm_nvkey = &g_mcu2dsp_vad_param->vad_nvkey_common;
    if (mic_num == 1) {
        *p_nvkey = (wwe_woking_mode == WWE_WORKING_MODE_VAD) ? (void *)&g_mcu2dsp_vad_param->vad_nvkey_1mic_v_mode : (void *)&g_mcu2dsp_vad_param->vad_nvkey_1mic_c_mode;
    } else {
        *p_nvkey = (wwe_woking_mode == WWE_WORKING_MODE_VAD) ? (void *)&g_mcu2dsp_vad_param->vad_nvkey_2mic_v_mode : (void *)&g_mcu2dsp_vad_param->vad_nvkey_2mic_c_mode;
    }

    *p_wwe_dbase = (void *)(g_mcu2dsp_vad_param->language_mode_address);
    *wwe_dbase_length = g_mcu2dsp_vad_param->language_mode_length;

    DSP_MW_LOG_I("[VAD_NVKEY]lauguage_model_address = 0x%08x, language_model_length = 0x%08x", 2, *p_wwe_dbase, *wwe_dbase_length);

    g_wwe_skip_frame_size_from_nvkey = ((DSP_NVKEY_VAD_COMM *)(*p_comm_nvkey))->skip_frame_num;

    DSP_MW_LOG_I("[VAD_NVKEY]g_wwe_skip_frame_size_from_nvkey = 0x%04x", 1, g_wwe_skip_frame_size_from_nvkey);

    switch (g_wwe_mode) {
        case WWE_MODE_AMA:

            break;

        case WWE_MODE_GSOUND:

            break;

        case WWE_MODE_VENDOR1:
            if (mic_num == 1) {
                *p_wwe_dbase = (void *)&db_1mic_hex;
            } else {
                *p_wwe_dbase = (void *)&db_2mic_hex;
            }

            break;
        case WWE_MODE_NONE:
        case WWE_MODE_MAX:

            break;
    }
}

void wwe_language_model_load(void **model_address, void *model_src, uint32_t model_length)
{
    DSP_MW_LOG_I("[WWE][LANG_MODEL]Load language model", 0);
    void *p_model_address = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE , model_length);
    //static uint8_t p_model_address[56 * 1024];
    if (p_model_address == NULL) {
        DSP_MW_LOG_I("[WWE][LANG_MODEL]DSP can not allocate enough memory(0x%08x) for language model", 1, model_length);
        configASSERT(0);
    } else {
        memcpy(p_model_address, model_src, model_length);
        *model_address = p_model_address;
        DSP_MW_LOG_I("[WWE][LANG_MODEL]lauguage_model_address = 0x%08x, language_model_length = 0x%08x", 2, p_model_address, model_length);
    }
}

void wwe_language_model_unload(void **model_address)
{
    if (*model_address != NULL) {
        DSP_MW_LOG_I("[WWE][LANG_MODEL]Unload language model", 0);
        preloader_pisplit_free_memory(*model_address);
        *model_address = NULL;
    } else {
        DSP_MW_LOG_I("[WWE][LANG_MODEL]Pointer NULL, no need to unload", 0);
    }
}

void wwe_buffer_allocate(void **buffer_address, uint32_t buffer_length)
{
    void *p_buffer_address = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE , buffer_length);
    if (p_buffer_address == NULL) {
        DSP_MW_LOG_I("[WWE][BUFFER]DSP can not allocate enough memory(0x%08x)", 1, buffer_length);
        configASSERT(0);
    } else {
        DSP_MW_LOG_I("[WWE][BUFFER]Allocate: buffer address = 0x%08x, buffer_length = 0x%08x", 2, (int)p_buffer_address, buffer_length);
        *buffer_address = p_buffer_address;
    }
}

void wwe_buffer_free(void **buffer_address)
{
    if (*buffer_address != NULL) {
        DSP_MW_LOG_I("[WWE][BUFFER]Free: buffer address = 0x%08x", 1, (int)*buffer_address);
        preloader_pisplit_free_memory(*buffer_address);
        *buffer_address = NULL;
    } else {
        DSP_MW_LOG_I("[WWE][BUFFER]Pointer NULL, no need to free", 0);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//Define for pre-proc-wwe
#define SWVAD_OFF                      0
#define SWVAD_ON                       1

//Define for env
#define ENV_INIT                       0
#define ENV_NOISY                      1
#define ENV_SILENT                     2

//Define for wwe
#define WWD_NEED_MORE_SAMPLE           2
#define WWD_WORKING                    1
#define WWD_SUCCESS                    0
#define WWD_FAIL                      -1

/////////////////////////////////////////////////////////////////////////////////////////////////
static short is_noisy = ENV_INIT;
static void *g_p_wwe_pp_mem_ext = NULL;

volatile bool g_is_wwe_success = false;
volatile wwe_state_t g_wwe_state = WWE_STATE_NONE;
static volatile uint16_t g_wwe_wkword_frame_number = 0;
static volatile int g_wwe_process_ret = 0;
volatile void *g_p_wwe_extra_preroll_buffer = NULL;

/////////////////////////////////////////////////////////////////////////////////////////////////
volatile bool is_wwe_time_out = false;
#define MTK_HWVAD_ENABLE
#ifdef MTK_HWVAD_ENABLE

volatile uint32_t g_wwe_sw_timer_handler = 0;
#define WWE_TIMEOUT_DUATION   10000

static void wwe_sw_timer_callback(void *user_data)
{
    UNUSED(user_data);
    DSP_MW_LOG_I("[WWE][TIMER]Timer time out", 0);
    is_wwe_time_out = true;
}

void wwe_sw_timer_start(void)
{
    hal_gpt_status_t gpt_status;
    if (g_wwe_sw_timer_handler == 0) {
        gpt_status = hal_gpt_sw_get_timer((uint32_t *)&g_wwe_sw_timer_handler);
        if (HAL_GPT_STATUS_OK != gpt_status) {
            DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_get_timer fail, fail id = %d", 1, gpt_status);
        } else {
            DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_get_timer pass", 0);
        }
    }
    is_wwe_time_out = false;
    gpt_status = hal_gpt_sw_start_timer_ms(g_wwe_sw_timer_handler, WWE_TIMEOUT_DUATION, wwe_sw_timer_callback, NULL);
    if (HAL_GPT_STATUS_OK != gpt_status) {
        DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_start_timer_ms fail, fail id = %d", 1, gpt_status);
    } else {
        DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_start_timer_ms pass", 0);
    }
}

void wwe_sw_timer_stop(void)
{
    hal_gpt_status_t gpt_status;
    uint32_t remaining_time_ms;
    if (g_wwe_sw_timer_handler == 0) {
        DSP_MW_LOG_I("[WWE][TIMER]wwe_sw_timer_stop fail, invalid g_wwe_sw_timer_handler!", 0);
    } else {
        hal_gpt_sw_get_remaining_time_ms(g_wwe_sw_timer_handler, &remaining_time_ms);
        if ((remaining_time_ms != 0) && (is_wwe_time_out == false)) {
            gpt_status = hal_gpt_sw_stop_timer_ms(g_wwe_sw_timer_handler);
            if (HAL_GPT_STATUS_OK != gpt_status) {
                DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_stop_timer_ms fail, fail id = %d", 1, gpt_status);
            } else {
                DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_stop_timer_ms pass", 0);
            }
        } else {
            is_wwe_time_out = false;
            DSP_MW_LOG_I("[WWE][TIMER]Timeout or stopped already, no need to stop", 0);
        }
    }
}

void wwe_sw_timer_free(void)
{
    hal_gpt_status_t gpt_status;
    is_wwe_time_out = false;
    if (g_wwe_sw_timer_handler == 0) {
        DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_free_timer fail, invalid g_wwe_sw_timer_handler!", 0);
    } else {
        gpt_status = hal_gpt_sw_free_timer(g_wwe_sw_timer_handler);
        if (HAL_GPT_STATUS_OK != gpt_status) {
            DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_free_timer fail, fail id = %d", 1, gpt_status);
        } else {
            DSP_MW_LOG_I("[WWE][TIMER]hal_gpt_sw_free_timer pass", 0);
            g_wwe_sw_timer_handler = 0;
        }
    }
}

VOID wwe_hwvad_resume(void)
{
    if (g_wwe_state == WWE_STATE_HWVAD) {
        g_wwe_state = WWE_STATE_LBF;
//#ifdef WWE_STATE_DEBUG
        DSP_MW_LOG_I("[WWE][STATE]HWVAD==>LBF", 0);
//#endif
        g_wwe_skip_frame_size = g_wwe_skip_frame_size_from_nvkey;
        DSP_MW_LOG_I("[WWE][STATE]g_wwe_skip_frame_size = 0x%04x", 1, g_wwe_skip_frame_size);

        wwe_sw_timer_start();
    }
    return;
}

static VOID wwe_hwvad_enable(void)
{
    hwvad_enable();
    return;
}

#endif


static VOID wwe_notify_cm4(bool is_detected)
{
    hal_ccni_message_t msg;

    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));

    uint16_t total_frame_size = vad_preroll_get_data_length() - (FORWARD_FRAME_NUMBER * (g_wwe_frame_size * 2));
    uint16_t wkword_frame_size = g_wwe_wkword_frame_number * (g_wwe_frame_size * 2);

    uint16_t total_frame_number = total_frame_size / (g_wwe_frame_size * 2);

    if (is_detected == true) {
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]g_wwe_wkword_frame_number = %d", 1, g_wwe_wkword_frame_number);
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]total_frame_number = %d", 1, total_frame_number);
#ifdef WWE_PREROLL_DEBUG
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]sink_slack = %d", 1, record_if.sink->sif.SinkSlack(record_if.sink));
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]sink_length = %d", 1, record_if.sink->streamBuffer.ShareBufferInfo.length);
#endif

         /*check if the preroll buffer have overflow ,will lead to drop data*/
        if (wkword_frame_size > total_frame_size) {
            DSP_MW_LOG_E("[WWE][wwe_notify_cm4] wkword_frame_size is larger than total_frame_size!!!!",0);
            //configASSERT(0);
        }
    }

    msg.ccni_message[0] = (is_detected == true) ? (MSG_DSP2MCU_RECORD_WWD_NOTIFY << 16) : (MSG_DSP2MCU_RECORD_DATA_ABORT_NOTIFY << 16);
    msg.ccni_message[1] = ((total_frame_size << 16) | wkword_frame_size);

    /* Set wake-word frame number to 0 */
    g_wwe_wkword_frame_number = 0;

    if (AUDIO_MSG_STATUS_OK != aud_msg_tx_handler(msg, 0, FALSE)) {
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]wwe_notify_cm4 Fail!", 0);
    } else {
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4]wwe_notify_cm4 Success!", 0);
    }
}

static VOID wwe_notify_cm4_wwe_version(uint32_t wwe_version)
{
    hal_ccni_message_t msg;

    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));

    DSP_MW_LOG_I("[WWE][wwe_notify_cm4_wwe_version]wwe_version = %d", 1, wwe_version);

    msg.ccni_message[0] = MSG_DSP2MCU_RECORD_WWE_VERSION << 16;
    msg.ccni_message[1] = wwe_version;

    if (AUDIO_MSG_STATUS_OK != aud_msg_tx_handler(msg, 0, FALSE)) {
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4_wwe_version]wwe_notify_cm4 Fail!", 0);
    } else {
        DSP_MW_LOG_I("[WWE][wwe_notify_cm4_wwe_version]wwe_notify_cm4 Success!", 0);
    }
}

static uint32_t wwe_get_channel_number(void *para)
{
#ifndef AIR_DUAL_CHIP_I2S_ENABLE
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t channel_number = stream_function_get_channel_number(para);
    if (stream_ptr->source->type == SOURCE_TYPE_AUDIO) {
        if (stream_ptr->source->param.audio.echo_reference) {
            channel_number--;
        }
    }
    return channel_number;
#else
    return 1;
#endif
}

uint32_t wwe_get_frame_size(void *para)
{
    return (stream_function_get_output_resolution(para) == RESOLUTION_16BIT) ? \
           stream_function_get_output_size(para) / sizeof(S16) : stream_function_get_output_size(para) / sizeof(S32);
}

bool IS_A2DP_Start(void)
{
    SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    SOURCE source = Source_blks[SOURCE_TYPE_A2DP];

    if ((sink != NULL) && (source != NULL)) {
        if ((sink->transform != NULL) && (source->transform != NULL) && (sink->transform == source->transform)) {
            return TRUE;
        }
    }
    return FALSE;
}

bool stream_function_wwe_preprocessing_initialize(void *para)
{
    int ret;

    /* Get channel number */
    uint32_t channel_number = wwe_get_channel_number(para);
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]channel_number = 0x%x", 1, channel_number);

    /* Get frame size */
    g_wwe_frame_size = wwe_get_frame_size(para);
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
    g_wwe_frame_size /= 3;
#endif
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]g_wwe_frame_size = %d", 1, g_wwe_frame_size);

    /* Workaround for first init of FW and 3rd init of FW */
    if ((g_wwe_frame_size == 0) || (g_wwe_state != WWE_STATE_NONE)) {
        return 0;
    }

#if defined(MTK_WWE_AMA_ENABLE) || defined(MTK_WWE_GSOUND_ENABLE)
    void *p_wwe_pp_mem_ext = NULL;

    /* Get and Show WWE Version */
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]LBF Version = %d", 1, (int)get_wwe_version());

    /* Check and get working memory address */
    int memsize = get_wwe_preprocessing_memsize(channel_number);
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]memsize = 0x%x", 1, memsize);
    wwe_buffer_allocate((void *)&p_wwe_pp_mem_ext,memsize);
    g_p_wwe_pp_mem_ext = p_wwe_pp_mem_ext;
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]addr[p_wwe_pp_mem_ext] = 0x%x", 1, (uint32_t)p_wwe_pp_mem_ext);

    /* Get data base and nvkey */
    wwe_get_db_nvkey(&p_wwe_dbase, &s_wwe_dbase_length, &p_comm_nvkey, &p_nvkey, channel_number, WWE_WORKING_MODE_CONTINIOUS);

#ifndef MTK_WWE_AMA_LM_IN_DRAM_ENABLE
    //bypass AMA language mode loading to DRAM, use language model on flash directly
    if(g_wwe_mode != WWE_MODE_AMA) {
        wwe_language_model_load(&p_wwe_dbase, p_wwe_dbase, s_wwe_dbase_length);
    }
#else
        wwe_language_model_load(&p_wwe_dbase, p_wwe_dbase, s_wwe_dbase_length);
#endif
    /* Init for LBF */
    ret = WWE_PreProcessing_Init(p_wwe_pp_mem_ext, p_comm_nvkey, p_nvkey, channel_number);

    /*allocate the wwe extra dynamic preroll buffer*/
    wwe_buffer_allocate((void *)&g_p_wwe_extra_preroll_buffer, PREROLL_BUFFER_SIZE);

    /* Reset pre-roll buffer */
    vad_preroll_reset(para);

    /* Set wake-word frame number to 0 */
    g_wwe_wkword_frame_number = 0;

    /* Set wwe success flag to false */
    g_is_wwe_success = false;

    DSP_MW_LOG_I("[WWE][PREPROC_INIT]ret = %d", 1, ret);
#else
    DSP_MW_LOG_I("[WWE][PREPROC_INIT]No LBF, bypass PREPROC_INIT", 0);
#endif

    return 0;
}

/* Need put libary to IRAM and modify CPU frequency in CM4 side before MIPS evaluation */

//#define WWE_MIPS_TEST
#ifdef WWE_MIPS_TEST
#define CPU_SPEED 104
#define TEST_COUNT 100
#endif
#include "dsp_scenario.h"
extern CONNECTION_IF record_if;
uint32_t slt_time[3] = {0};
uint32_t wwe_enter_cnt=0,wwe_error_cnt=0,last_wwe_error_cnt=0,last_slt_time=0;

bool stream_function_wwe_preprocessing_process(void *para)
{
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &slt_time[1]);
    slt_time[0] = slt_time[1] - slt_time[2];
    slt_time[2] = slt_time[1];
    int ret;

    if (g_wwe_state != WWE_STATE_LBF) {
        return 0;
    }

    /* Get channel number */
    uint32_t channel_number = wwe_get_channel_number(para);

    /* Get working memory address */
    void *p_wwe_pp_mem_ext = g_p_wwe_pp_mem_ext;

    /* Get pcm buffer address for mic1 & mic 2 */
    short *mic_buf_1 = stream_function_get_inout_buffer(para, 1);
    short *mic_buf_2 = channel_number > 1 ? stream_function_get_inout_buffer(para, 2) : NULL;
    //DSP_MW_LOG_I("[WWE][PREPROC]addr[mic_buf_1] = %x, addr[mic_buf_2] = %x", 2, (uint32_t)mic_buf_1, (uint32_t)mic_buf_2);

    /* Get frame size */
    uint32_t wwe_frame_size = wwe_get_frame_size(para);

    /* Update frame size for 32 bit to 16 bit cases, the real frame size can be get in process flow other than init flow */
    if((wwe_frame_size != 0) && (g_wwe_frame_size != wwe_frame_size)) {
        g_wwe_frame_size = wwe_frame_size;
        DSP_MW_LOG_I("[WWE][PREPROC]update the frame size: g_wwe_frame_size = %d", 1, g_wwe_frame_size);
    }
#ifdef WWE_COMMON_DEBUG
    DSP_MW_LOG_I("[WWE][PREPROC]wwe_frame_size = %d", 1, wwe_frame_size);
    DSP_MW_LOG_I("[WWE][PREPROC]sink_size = %d", 1, (U32)SinkSlack(DSP_STREAMING_GET_FROM_PRAR(para)->sink));
#endif

#if defined(MTK_WWE_AMA_ENABLE) || defined(MTK_WWE_GSOUND_ENABLE)

    /* Indicates dummy callback after WWE success */
    if ((wwe_frame_size == 0) && (true == g_is_wwe_success)) {
        if (PREROLL_STATUS_OK == vad_preroll_extra_buffer_read((U8 *)mic_buf_1, g_wwe_frame_size * 2)) {
            stream_function_modify_output_size(para, g_wwe_frame_size * 2);
            stream_function_reenter_stream(para);
        }  else if (PREROLL_STATUS_OK == vad_preroll_read_data((U8 *)mic_buf_1, g_wwe_frame_size * 2)) {
            stream_function_modify_output_size(para, g_wwe_frame_size * 2);
            stream_function_reenter_stream(para);
        }
        return 0;
    } else {
#ifdef WWE_MIPS_TEST
        static uint32_t average_duration_lbf = 0;
        static uint32_t test_times_lbf = 0;
        uint32_t before_count, after_count;
        uint32_t duration;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &before_count);
#endif
        /* WWE preprocessing for LBF */
        ret = WWE_PreProcessing(mic_buf_1, mic_buf_2, NULL, g_wwe_frame_size * 2, p_wwe_pp_mem_ext, 1, mic_buf_1, &is_noisy);

#ifdef WWE_MIPS_TEST
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &after_count);
        duration = after_count - before_count;
        DSP_MW_LOG_I("lbf_duration = %d", 1, duration);
        average_duration_lbf += duration;
        test_times_lbf ++;

        if ((test_times_lbf % TEST_COUNT) == 0) {
            average_duration_lbf = average_duration_lbf / TEST_COUNT;
            DSP_MW_LOG_I("average_duration_lbf = %d", 1, average_duration_lbf);
            printf("LBF_MIPS = %f", (float)average_duration_lbf / 1000 * CPU_SPEED / (g_wwe_frame_size / 16));
            average_duration_lbf = 0;
            test_times_lbf = 0;
        }
#endif

#ifdef WWE_COMMON_DEBUG
        DSP_MW_LOG_I("[WWE][PREPROC]is_noisy = %d", 1, (int32_t)is_noisy);
#endif

        /* Skip some frames if mic resume from HWVAD mode */
        if (0 != g_wwe_skip_frame_size) {
            memset(mic_buf_1, 0, g_wwe_frame_size * 2);
            g_wwe_skip_frame_size--;
#ifdef WWE_COMMON_DEBUG
            DSP_MW_LOG_I("[WWE][PREPROC]g_wwe_skip_frame_size = 0x%04x", 1, g_wwe_skip_frame_size);
#endif
        }
#ifdef WWE_PREROLL_DEBUG
        if(true == g_is_wwe_success) {
            memset(mic_buf_1, 0x5A, g_wwe_frame_size * 2);
        }
#endif
        /* Pre-roll buffer update */
        vad_preroll_write_data((U8 *)mic_buf_1, g_wwe_frame_size * 2);

        /* If WWE success and pre-roll buffer still have data, re-enter stream */
        if (true == g_is_wwe_success) {
            if (vad_preroll_get_data_length() >= g_wwe_frame_size * 2) {
                stream_function_reenter_stream(para);
            }
#ifdef WWE_COMMON_DEBUG
            DSP_MW_LOG_I("[WWE][PREPROC]vad_preroll_get_data_length = %d", 1, vad_preroll_get_data_length());
#endif
            return 0;
        }
    }

    wwe_enter_cnt++;
    if(g_wwe_mode==WWE_MODE_AMA) {
        if(slt_time[0]>15000 && last_slt_time>15000) {
            wwe_error_cnt++;
            DSP_MW_LOG_I("DEBUG, wwe_enter_cnt=%d and last_slt_time=%d, slt_time=%d",3,wwe_enter_cnt,last_slt_time,slt_time[0]);
            DSP_MW_LOG_I("DEBUG ERROR, VUL_CUR:0x%x, U_BASE:0x%x, U_END:0x%x, DL1_CUR:0x%x, D_BASE:0x%x, D_END:0x%x, WR:%d, RD:%d, SleepSetting:0x%x @@@@", 9, AFE_GET_REG(AFE_VUL_CUR), AFE_GET_REG(AFE_VUL_BASE), AFE_GET_REG(AFE_VUL_END), AFE_GET_REG(AFE_DL1_CUR), AFE_GET_REG(AFE_DL1_BASE), AFE_GET_REG(AFE_DL1_END), record_if.source->streamBuffer.BufferInfo.WriteOffset, record_if.source->streamBuffer.BufferInfo.ReadOffset, *(volatile uint32_t*)(0xA2110000+ 0x03B0) );
            DSP_MW_LOG_I("DEBUG ERROR, 0x0014:0x%x, 0x03F8:0x%x, 0x03D0:0x%x, 0x03D4:0x%x, 0x03D8:0x%x, 0x1408:0x%x", 6, AFE_GET_REG(AFE_DAC_CON1), AFE_GET_REG(AFE_MEMIF_HD_MODE), AFE_GET_REG(AFE_MEMIF_MINLEN), AFE_GET_REG(AFE_MEMIF_MAXLEN), AFE_GET_REG(AFE_MEMIF_PBUF_SIZE), AFE_GET_REG(AFE_I2S_SLV_ENGEN_CON2));
            DSP_MW_LOG_I("DEBUG ERROR, slt_t=%d, now_t=%d, wwe_enter_cnt=%d, wwe_error_cnt=%d", 4,slt_time[0],slt_time[1],wwe_enter_cnt,wwe_error_cnt);
            if(wwe_error_cnt > 6) {
                DSP_MW_LOG_I("DEBUG, happen issueA",0);
                wwe_error_cnt = 0;
                hal_audio_device_force_off_delay_timer();
                wwe_notify_cm4(0);
            }
        } else {
            wwe_error_cnt = 0;
        }
    }
    last_slt_time = slt_time[0];
    if ((false == g_is_wwe_success) && (!IS_A2DP_Start()) && (is_hwvad_enable) && (ENV_SILENT == is_noisy) && ((g_wwe_process_ret < 0) || (is_wwe_time_out == true))) {
#ifdef MTK_HWVAD_ENABLE
//#ifdef WWE_STATE_DEBUG
        DSP_MW_LOG_I("[WWE][STATE]LBF==>HWVAD", 0);
//#endif
        g_wwe_state = WWE_STATE_HWVAD;
        g_wwe_wkword_frame_number = 0;
        //wwe_sw_timer_stop();
        vad_preroll_reset(para);
        //wwe_notify_cm4(false);
        wwe_hwvad_enable();
#else   /* MTK_HWVAD_ENABLE not defined */
#ifdef WWE_STATE_DEBUG
        DSP_MW_LOG_I("[WWE][STATE]HWVAD(Fake)==>WWD", 0);
#endif
        g_wwe_state = WWE_STATE_WWD;
#endif
    } else {
#ifdef WWE_STATE_DEBUG
        DSP_MW_LOG_I("[WWE][STATE]LBF==>WWD", 0);
#endif

#ifdef MTK_HWVAD_ENABLE
        /* Restart timer if can not enter HWVAD mode */
        if(is_wwe_time_out == true) {
            wwe_sw_timer_start();
        }
#endif
        g_wwe_state = WWE_STATE_WWD;
    }

#else
    DSP_MW_LOG_I("[WWE][PREPROC]No LBF, bypass PREPROC", 0);
#endif

    return 0;
}

#ifdef MTK_WWE_AMA_ENABLE

#include "pryon_lite_PRL1000.h"
void *g_para;

static void loadWakewordModel(PryonLiteWakewordConfig *config)
{
    // In order to detect keywords, the decoder uses a model which defines the parameters,
    // neural network weights, classifiers, etc that are used at runtime to process the audio
    // and give detection results.

    // Each model is packaged in two formats:
    // 1. A .bin file that can be loaded from disk (via fopen, fread, etc)
    // 2. A .cpp file that can be hard-coded at compile time

    config->sizeofModel = s_wwe_dbase_length; // example value, will be the size of the binary model byte array
    config->model = p_wwe_dbase; // pointer to model in memory
}

// engine handle
static PryonLiteV2Handle sHandle = {0};
static bool is_amz_wwe_quit = false;
// VAD event handler
static void vadEventHandler(PryonLiteV2Handle *handle, const PryonLiteVadEvent *vadEvent)
{
    UNUSED(handle);
    DSP_MW_LOG_I("[AMZ]VAD state %d\n", 1, (int) vadEvent->vadState);
}
// Wakeword event handler
static void wakewordEventHandler(PryonLiteV2Handle *handle, const PryonLiteWakewordResult *wwEvent)
{
    UNUSED(handle);
    DSP_MW_LOG_I("########[WWE][STATE][RESULT][AMA]WWD(Success)==>LBF########", 0);
    printf("[AMZ]Detected wakeword '%s'\n", wwEvent->keyword);
    //DSP_MW_LOG_I("[AMZ]endSampleIndex = %d, beginSampleIndex = %d\n", 2, wwEvent->endSampleIndex, wwEvent->beginSampleIndex);
    printf("[AMZ]endSampleIndex = %d(Actual endSampleIndex = %d, delta = %d frames), beginSampleIndex = %d\n",\
    (uint32_t)wwEvent->endSampleIndex,g_wwe_ama_sample_cnt * g_wwe_frame_size, g_wwe_ama_sample_cnt-(uint32_t)wwEvent->endSampleIndex/g_wwe_frame_size, (uint32_t)wwEvent->beginSampleIndex);
#ifdef MTK_HWVAD_ENABLE
    if (is_hwvad_enable) {
        wwe_sw_timer_stop();
    }
#ifndef WWE_DETECT_ONCE
    if (is_hwvad_enable) {
        wwe_sw_timer_start();
    }
#endif
#endif
    //g_wwe_wkword_frame_number = (wwEvent->endSampleIndex - wwEvent->beginSampleIndex) / g_wwe_frame_size;
    g_wwe_wkword_frame_number = g_wwe_ama_sample_cnt - (wwEvent->beginSampleIndex / g_wwe_frame_size);
    wwe_notify_cm4(true);
#ifdef WWE_DETECT_ONCE
    stream_function_reenter_stream(g_para);
    g_is_wwe_success = true;
#endif
}

static void handleEvent(PryonLiteV2Handle *handle, const PryonLiteV2Event *event)
{
    if (event->vadEvent != NULL) {
        vadEventHandler(handle, event->vadEvent);
    }
    if (event->wwEvent != NULL) {
        wakewordEventHandler(handle, event->wwEvent);
    }
}

#endif

bool stream_function_wwe_processing_initialize(void *para)
{
    int ret;
    UNUSED(ret);
    int wwe_version = 0;

    /* Get channel number */
    uint32_t channel_number = wwe_get_channel_number(para);
    DSP_MW_LOG_I("[WWE][PROC_INIT]channel_number = 0x%x", 1, channel_number);

    /* Get frame size */
    g_wwe_frame_size = wwe_get_frame_size(para);
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
    g_wwe_frame_size /= 3;
#endif
    DSP_MW_LOG_I("[WWE][PROC_INIT]g_wwe_frame_size = %d", 1, g_wwe_frame_size);

    /* Workaround for first init of FW and 3rd init of FW */
    if ((g_wwe_frame_size == 0) || (g_wwe_state != WWE_STATE_NONE)) {
        return 0;
    }

    switch (g_wwe_mode) {
#ifdef MTK_WWE_AMA_ENABLE
        case WWE_MODE_AMA: {
            g_para = para;

            //char *engineBuffer = stream_function_get_working_buffer(para);

            PryonLiteV2Config engineConfig = {0};
            PryonLiteV2EventConfig engineEventConfig = {0, 0};
            PryonLiteV2ConfigAttributes configAttributes = {0, 0, 0, {0, 0}};

            // Wakeword detector configuration
            PryonLiteWakewordConfig wakewordConfig = PryonLiteWakewordConfig_Default;
            loadWakewordModel(&wakewordConfig);
            wakewordConfig.detectThreshold = 500; // default threshold
            wakewordConfig.useVad = 1;  // enable voice activity detector
            engineConfig.ww = &wakewordConfig;

            engineEventConfig.enableVadEvent = true;
            engineEventConfig.enableWwEvent = true;

            PryonLiteStatus status = PryonLite_GetConfigAttributes(&engineConfig, &engineEventConfig, &configAttributes);

            if (status.publicCode != PRYON_LITE_ERROR_OK) {
                DSP_MW_LOG_I("[WWE][PROC_INIT][AMZ]PryonLite_GetConfigAttributes Fail, Fail_ID = %d", 1, status.publicCode);
                configASSERT(0);
                return 1;
            }

            DSP_MW_LOG_I("[WWE][PROC_INIT][AMZ] configAttributes.requiredMem = 0x%08x", 1,  configAttributes.requiredMem);

            wwe_buffer_allocate((void *)&engineBuffer, configAttributes.requiredMem);

            status = PryonLite_Initialize(&engineConfig, &sHandle, handleEvent, &engineEventConfig, engineBuffer, configAttributes.requiredMem);

            g_wwe_ama_sample_cnt = 0;

            if (status.publicCode != PRYON_LITE_ERROR_OK) {
                DSP_MW_LOG_I("[WWE][PROC_INIT][AMZ]PryonLite_Initialize Fail, Fail_ID = %d", 1, status.publicCode);
                configASSERT(0);
                return 1;
            }

            // Set detection threshold for all keywords (this function can be called any time after decoder initialization)
            int detectionThreshold = 500;
            status = PryonLiteWakeword_SetDetectionThreshold(sHandle.ww, NULL, detectionThreshold);
            if (status.publicCode != PRYON_LITE_ERROR_OK) {
                DSP_MW_LOG_I("[WWE][PROC_INIT][AMZ]PryonLiteWakeword_SetDetectionThreshold Fail, Fail_ID = %d", 1, status.publicCode);
                configASSERT(0);
                return 1;
            }

            is_amz_wwe_quit = false;
            break;
        }
#endif
#ifdef MTK_WWE_GSOUND_ENABLE
        case WWE_MODE_GSOUND: {

            void *memory_banks[] = {
                NULL,
                NULL
            };

#if 0
            static const uint8_t lauguage_model_gsound[] = {
#include "cmn_hans_new.ota.hex"
            };

            if (memcmp(lauguage_model_gsound, p_wwe_dbase, sizeof(lauguage_model_gsound))) {
                DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]LANG_MODEL Mismatch!!!!!", 0);
            } else {
                DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]LANG_MODEL match!!!!!", 0);
            }
#endif


            memory_banks[0] = (void *)GSOUND_HOTWORD_RW_BASE(p_wwe_dbase);
            memory_banks[1] = (void *)GSOUND_HOTWORD_RO_BASE(p_wwe_dbase);

            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GSOUND_HOTWORD_RW_BASE = 0x%08x, GSOUND_HOTWORD_RW_LENGTH = 0x%08x", 2, GSOUND_HOTWORD_RW_BASE(p_wwe_dbase), GSOUND_HOTWORD_RW_LENGTH(p_wwe_dbase));
            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GSOUND_HOTWORD_RO_BASE = 0x%08x, GSOUND_HOTWORD_RO_LENGTH = 0x%08x", 2, GSOUND_HOTWORD_RO_BASE(p_wwe_dbase), GSOUND_HOTWORD_RO_LENGTH(p_wwe_dbase));
            wwe_version = GoogleHotwordVersion();
            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GoogleHotwordVersion = %d", 1, wwe_version);
            wwe_notify_cm4_wwe_version(wwe_version);
            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]kGoogleHotwordRequiredDataAlignment = %d", 1, kGoogleHotwordRequiredDataAlignment);
            //DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GoogleHotwordDspGetMaximumAudioPreambleMs = %d", 1, GoogleHotwordDspGetMaximumAudioPreambleMs());
            //ret = GoogleHotwordDspInit(p_wwe_dbase);
            //DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GoogleHotwordDspInit: ret = %d", 1, ret);

            s_memory_handle = GoogleHotwordDspMultiBankInit((void *)&memory_banks, 2);
            GoogleHotwordDspMultiBankReset(s_memory_handle);
            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]s_memory_handle = %d", 1, (int32_t)s_memory_handle);

            DSP_MW_LOG_I("[WWE][PROC_INIT][BISTO]GoogleHotwordDspMultiBankGetMaximumAudioPreambleMs = %d", 1, GoogleHotwordDspMultiBankGetMaximumAudioPreambleMs(s_memory_handle));

            break;
        }
#endif
#ifdef MTK_WWE_VENDOR1_ENABLE
        case WWE_MODE_VENDOR1: {
            /* Get and Show WWE Version */
            wwe_version = get_wwe_version();
            DSP_MW_LOG_I("[WWE][PROC_INIT][VENDOR1]WWE Version = %d", 1, wwe_version);
            wwe_notify_cm4_wwe_version(wwe_version);
            int memsize = get_wwe_memsize(channel_number);
            DSP_MW_LOG_I("[WWE][PROC_INIT][VENDOR1]memsize = 0x%x", 1, memsize);
            if (memsize > WWE_PROC_MEMSIZE) {
                DSP_MW_LOG_I("[WWE][PROC_INIT][VENDOR1]Working memory size is not enough!", 0);
                configASSERT(0);
            }
            void *p_wwe_mem_ext = stream_function_get_working_buffer(para);
            DSP_MW_LOG_I("[WWE][PROC_INIT][VENDOR1]addr[p_wwe_mem_ext] = 0x%x", 1, (uint32_t)p_wwe_mem_ext);

            /* Init for WWE */
            ret = WWE_Init(p_wwe_mem_ext, p_wwe_dbase, p_comm_nvkey, p_nvkey, channel_number);
            DSP_MW_LOG_I("[WWE][PROC_INIT][VENDOR1]ret = %d", 1, ret);

            break;
        }
#endif
        default: {
            DSP_MW_LOG_I("[WWE][PROC_INIT]UNKNOWN WWE MODE(%d)", 1, g_wwe_mode);
            configASSERT(0);
            break;
        }
    }

    /* Set wwe to LBF state */
    g_wwe_state = WWE_STATE_LBF;

    return 0;
}

bool stream_function_wwe_processing_process(void *para)
{

    if (g_wwe_state != WWE_STATE_WWD) {
        return 0;
    }

    /* Get working memory address */
    void *p_wwe_mem_ext = stream_function_get_working_buffer(para);
    UNUSED(p_wwe_mem_ext);

    uint32_t read_address;
    if (PREROLL_STATUS_OK != vad_preroll_forward_data(&read_address, FORWARD_FRAME_NUMBER * g_wwe_frame_size * 2)) {
        DSP_MW_LOG_I("[WWE][PROC]Forward Preroll Data Fail", 0);
        configASSERT(0);
    }
#ifdef WWE_MIPS_TEST
    static uint32_t average_duration_wwe = 0;
    static uint32_t test_times_wwe = 0;
    uint32_t before_count, after_count;
    uint32_t duration;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &before_count);
#endif
    switch (g_wwe_mode) {
#ifdef MTK_WWE_AMA_ENABLE
        case WWE_MODE_AMA: {
            if (is_amz_wwe_quit == false) {
                //uint32_t int_mask;
                //hal_nvic_save_and_set_interrupt_mask(&int_mask);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)read_address, (U32)(g_wwe_frame_size*2), SOURCE_IN5);
#endif
                PryonLiteStatus status = PryonLite_PushAudioSamples(&sHandle, (short *)read_address, g_wwe_frame_size);
                g_wwe_ama_sample_cnt++;
                //hal_nvic_restore_interrupt_mask(int_mask);

                if (status.publicCode == PRYON_LITE_ERROR_OK) {
#ifdef WWE_STATE_DEBUG
                    DSP_MW_LOG_I("[WWE][STATE][AMA]WWD(Working)==>LBF", 0);
#endif
                } else {
                    DSP_MW_LOG_I("********[WWE][STATE][RESULT][AMA]WWD(Fail)==>LBF********", 0);
                    DSP_MW_LOG_I("[WWE][PROC]Fail ID = %d", 1, status.publicCode);
                    g_wwe_wkword_frame_number = 0;
                }
            }
            break;
        }
#endif
#ifdef MTK_WWE_GSOUND_ENABLE
        case WWE_MODE_GSOUND: {
            int preamble_length_ms = 0;

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)read_address, (U32)(g_wwe_frame_size*2), SOURCE_IN5);
#endif
            //g_wwe_process_ret = GoogleHotwordDspProcess((short *)read_address, g_wwe_frame_size, &preamble_length_ms);
            g_wwe_process_ret = GoogleHotwordDspMultiBankProcess((short *)read_address, g_wwe_frame_size, &preamble_length_ms, s_memory_handle);

            if (g_wwe_process_ret == 1) {
                DSP_MW_LOG_I("########[WWE][STATE][RESULT][BISTO]WWD(Success)==>LBF########", 0);
                g_wwe_wkword_frame_number = (preamble_length_ms << 4) / g_wwe_frame_size;
                wwe_notify_cm4(true);
#ifdef WWE_DETECT_ONCE
                stream_function_reenter_stream(para);
                g_is_wwe_success = true;
#endif
                //GoogleHotwordDspReset();
                GoogleHotwordDspMultiBankReset(s_memory_handle);
            } else {
#ifdef WWE_STATE_DEBUG
                //DSP_MW_LOG_I("********[WWE][STATE][RESULT][BISTO]WWD(Fail)==>LBF********", 0);
                DSP_MW_LOG_I("[WWE][PROC][BISTO]Fail ID = %d", 1, g_wwe_process_ret);
#endif
            }
            break;
        }
#endif
#ifdef MTK_WWE_VENDOR1_ENABLE
        case WWE_MODE_VENDOR1: {
            /* The expected value of g_wwe_wkword_frame_number should be 58~104 */
            g_wwe_process_ret = WWE_Processing((short *)read_address, NULL, NULL, g_wwe_frame_size * 2, p_wwe_mem_ext, g_p_wwe_pp_mem_ext, (short *)&g_wwe_wkword_frame_number);

#ifdef WWE_COMMON_DEBUG
            DSP_MW_LOG_I("[WWE][PROC][VENDOR1]g_wwe_process_ret = %d", 1, g_wwe_process_ret);
#endif

            //TODO: debug dump for analysis
#if 0
            void *p_rd_dumpbuf = NULL;
            int dump_length = WWE_Debug(g_p_wwe_pp_mem_ext, p_wwe_mem_ext, &p_rd_dumpbuf);
            DSP_MW_LOG_I("[WWE][PROC]dump_length = %d", 1, dump_length);
            LOG_AUDIO_DUMP((U8 *)p_rd_dumpbuf, dump_length, SOURCE_IN4);
#endif

            switch (g_wwe_process_ret) {
                case WWD_WORKING:
#ifdef WWE_STATE_DEBUG
                    DSP_MW_LOG_I("[WWE][STATE][VENDOR1]WWD(Working)==>LBF", 0);
#endif
                    break;

                case WWD_NEED_MORE_SAMPLE:
#ifdef WWE_STATE_DEBUG
                    DSP_MW_LOG_I("[WWE][STATE][VENDOR1]WWD(Need more sample)==>LBF", 0);
#endif
                    break;

                case WWD_SUCCESS:
                    DSP_MW_LOG_I("########[WWE][STATE][RESULT][VENDOR1]WWD(Success)==>LBF########", 0);
                    wwe_notify_cm4(true);
#ifdef WWE_DETECT_ONCE
                    stream_function_reenter_stream(para);
                    g_is_wwe_success = true;
#endif
                    break;

                default:
#ifdef WWE_STATE_DEBUG
                    DSP_MW_LOG_I("********[WWE][STATE][RESULT][VENDOR1]WWD(Fail)==>LBF********", 0);
                    DSP_MW_LOG_I("[WWE][PROC]Fail ID = %d", 1, g_wwe_process_ret);
                    g_wwe_wkword_frame_number = 0;
#endif
                    break;
            }

            break;
        }
#endif

        case WWE_MODE_NONE:
        default: {
            DSP_MW_LOG_I("[WWE][PROC_INIT]UNKNOWN WWE MODE(%d)", 1, g_wwe_mode);
            configASSERT(0);
            break;
        }
    }

#ifdef WWE_MIPS_TEST
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &after_count);
    duration = after_count - before_count;
    DSP_MW_LOG_I("wwe_duration = %d", 1, duration);
    average_duration_wwe += duration;
    test_times_wwe ++;

    if ((test_times_wwe % TEST_COUNT) == 0) {
        average_duration_wwe = average_duration_wwe / TEST_COUNT;
        DSP_MW_LOG_I("average_duration_wwe = %d", 1, average_duration_wwe);
        printf("WWE_MIPS = %f", (float)average_duration_wwe / 1000 * CPU_SPEED / (g_wwe_frame_size / 16));
        average_duration_wwe = 0;
        test_times_wwe = 0;
        //DSP_MW_LOG_I("cpu_frequency = %d", 1, hal_dvfs_get_cpu_frequency());
    }
#endif

    /* Swith to LBF mode */
    g_wwe_state = WWE_STATE_LBF;

    return 0;
}

mcu2dsp_vad_param_p new_mcu2dsp_vad_param_p = NULL;
VOID wwe_processing_init(void)
{
    if(g_wwe_mode == WWE_MODE_NONE) {
        return;
    }
    wwe_buffer_allocate((void *)&new_mcu2dsp_vad_param_p, sizeof(mcu2dsp_vad_param_t));
    memcpy(new_mcu2dsp_vad_param_p, g_mcu2dsp_vad_param, sizeof(mcu2dsp_vad_param_t));
    g_mcu2dsp_vad_param = new_mcu2dsp_vad_param_p;

    hwvad_config_t hwvad_config;
    hwvad_config.hwvad_vow_para = g_mcu2dsp_vad_param->vow_setting;
#ifdef WWE_MIPS_TEST
    is_hwvad_enable = false;
#else
    is_hwvad_enable = hwvad_config.hwvad_vow_para.enable;
#endif
    DSP_MW_LOG_I("[WWE][PROC_INIT]is_hwvad_enable = %d", 1, is_hwvad_enable);
#ifdef MTK_WWE_AMA_ENABLE
#ifdef MTK_HWVAD_ENABLE
    if (g_wwe_mode == WWE_MODE_AMA) {
        if (is_hwvad_enable) {
            hwvad_init(hwvad_config);
            wwe_sw_timer_start();
        }
    }
#endif
#endif
}

void wwe_hwvad_deinit(void)
{
    if(g_wwe_mode == WWE_MODE_NONE) {
        return;
    }
#ifdef MTK_WWE_AMA_ENABLE
    if (g_wwe_mode == WWE_MODE_AMA) {
#ifdef MTK_HWVAD_ENABLE
        if (is_hwvad_enable) {
            hwvad_disable();
            hwvad_deinit();
        }
#endif
    }
#endif
}

VOID wwe_processing_deinit(void)
{
    if(g_wwe_mode == WWE_MODE_NONE) {
        return;
    }
#ifdef MTK_WWE_AMA_ENABLE
    if (g_wwe_mode == WWE_MODE_AMA) {
#ifdef MTK_HWVAD_ENABLE
        if (is_hwvad_enable) {
            wwe_sw_timer_stop();
            wwe_sw_timer_free();
        }
#endif
        PryonLiteStatus status = PryonLite_Destroy(&sHandle);
        if (status.publicCode != PRYON_LITE_ERROR_OK) {
            DSP_MW_LOG_I("[WWE][PROC_DEINIT][AMZ]PryonLite_Destroy Fail, Fail_ID = %d", 1, status.publicCode);
        } else {
            DSP_MW_LOG_I("[WWE][PROC_DEINIT][AMZ]PryonLite_Destroy Success", 0);
        }
        wwe_buffer_free((void *)&engineBuffer);
        is_amz_wwe_quit = true;
    }
#endif
#ifndef MTK_WWE_AMA_LM_IN_DRAM_ENABLE
    if(g_wwe_mode != WWE_MODE_AMA) {
        wwe_language_model_unload(&p_wwe_dbase);
    }
#else
    wwe_language_model_unload(&p_wwe_dbase);
#endif
    wwe_buffer_free((void *)&new_mcu2dsp_vad_param_p);
    wwe_buffer_free((void *)&g_p_wwe_pp_mem_ext); /*free LBF malloc buffer*/
    wwe_buffer_free((void *)&g_p_wwe_extra_preroll_buffer);

    g_wwe_state = WWE_STATE_NONE;
}

#endif

