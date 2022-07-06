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

#include "bt_hfp_codec_internal.h"

#if !defined(MTK_AVM_DIRECT)
    #include "cvsd_codec.h"
    #include "msbc_codec.h"
    #include "plc.h"
    #include "hal_audio_enhancement.h"
    #include "hal_audio_internal_afe.h"
    #include "hal_audio_ext.h"
#endif

#if defined(MTK_AVM_DIRECT)
    #include "bt_sink_srv_ami.h"
    #include "audio_nvdm_common.h"
    #include "hal_dvfs_internal.h"
#endif

#include "hal_gpt.h"
#include "hal_audio.h"
#include "task_def.h"
#include "FreeRTOS.h"

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#include "external_dsp_driver.h"
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "audio_mixer.h"
#endif

#include "audio_nvdm_coef.h"

#ifdef MTK_AIRDUMP_EN
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_dsprealtime.h"
#endif
#endif

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#include "bt_sink_srv_ami.h"
#if defined(MTK_ANC_ENABLE) && defined(MTK_VOICE_ANC_EQ)
#ifdef MTK_ANC_V2
  #include "anc_control_api.h"
#else
  #include "anc_control.h"
#endif
#endif

#include "hal_dvfs_internal.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#if (PRODUCT_VERSION == 2822)
#define HFP_DVFS_DEFAULT_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_MAX_SPEED HAL_DVFS_HIGH_SPEED_208M
#define HFP_DVFS_ECNR_DUAL_MIC_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_INEAR_ENHANCEMENT_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED HAL_DVFS_FULL_SPEED_104M
#define HFP_DVFS_3RD_PARTY_NR_SPEED HAL_DVFS_FULL_SPEED_104M
#define HFP_DVFS_3RD_PARTY_NR_INEAR_SPEED HAL_DVFS_FULL_SPEED_104M
#elif(PRODUCT_VERSION == 1565)
#define HFP_DVFS_DEFAULT_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_MAX_SPEED HAL_DVFS_HIGH_SPEED_208M
#define HFP_DVFS_ECNR_DUAL_MIC_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_INEAR_ENHANCEMENT_SPEED HAL_DVFS_HALF_SPEED_52M
#define HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED HAL_DVFS_FULL_SPEED_104M
#define HFP_DVFS_3RD_PARTY_NR_SPEED HAL_DVFS_FULL_SPEED_104M
#define HFP_DVFS_3RD_PARTY_NR_INEAR_SPEED HAL_DVFS_FULL_SPEED_104M
#else
#define HFP_DVFS_DEFAULT_SPEED DVFS_26M_SPEED
#define HFP_DVFS_MAX_SPEED DVFS_156M_SPEED
#define HFP_DVFS_ECNR_DUAL_MIC_SPEED DVFS_39M_SPEED
#define HFP_DVFS_INEAR_ENHANCEMENT_SPEED DVFS_39M_SPEED
#define HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED DVFS_78M_SPEED
#define HFP_DVFS_3RD_PARTY_NR_SPEED DVFS_78M_SPEED
#define HFP_DVFS_3RD_PARTY_NR_INEAR_SPEED DVFS_78M_SPEED
#endif
#endif

typedef struct {
    uint8_t packet_length;
    uint8_t packet_number_per_interrupt;
    bool two_bytes_dummy_between_packet_flag;
} bt_hfp_packet_info_t;

/*******************************************************************************\
| LPK_UL_TO_HP feature                                                                    |
\*******************************************************************************/
//#define LBK_UL_TO_HP_ENABLED
#if defined LBK_UL_TO_HP_ENABLED
uint16_t audio_pcm2way_hp_buffer[320] = {0};
#endif
/*******************************************************************************\
| ROM Tables                                                                    |
\*******************************************************************************/

static const uint32_t bt_hfp_rx_packet_valid_mask_table[BT_HFP_PKT_TYPE_IDX_MAX][BT_HFP_HW_SRAM_RX_PKT_CNT] = {
    {0x1 << 0, 0x1 << 0, 0x1 << 1, 0x1 << 1, 0x1 <<  2, 0x1 <<  2}, /* 60 Bytes */
    {0x1 << 0, 0x1 << 1, 0x1 << 2, 0x1 << 3, 0x1 <<  4, 0x1 <<  5}, /* 30 Bytes */
    {0x3 << 0, 0x3 << 1, 0x3 << 3, 0x3 << 4, 0x3 <<  6, 0x3 <<  7}, /* 20 Bytes */
    {0x7 << 0, 0x7 << 3, 0x7 << 6, 0x7 << 9, 0x7 << 12, 0x7 << 15}  /* 10 Bytes */
};

static const uint8_t bt_hfp_msbc_header[4] = {0x08, 0x38, 0xc8, 0xf8};

/*******************************************************************************\
| Global Variables                                                              |
\*******************************************************************************/

static bool bt_hfp_codec_is_running = false;
static bt_hfp_codec_internal_handle_t *bt_hfp_codec_internal_handle = NULL;
static QueueHandle_t bt_codec_queue_handle = NULL;
static uint32_t bt_codec_queue_reg_num = 0;
static bt_codec_queue_event_id_t bt_codec_queue_id_array[MAX_BT_CODEC_FUNCTIONS];
static bt_codec_callback_t bt_codec_queue_handler[MAX_BT_CODEC_FUNCTIONS];
#ifdef HAL_DVFS_MODULE_ENABLED
static dvfs_frequency_t g_hfp_dvfs_clk = HFP_DVFS_DEFAULT_SPEED;
#endif
/* #define BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
uint32_t bt_hfp_rx_cnt = 0;
uint32_t p2w_tx_cnt = 0;
uint32_t p2w_rx_cnt = 0;
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

#define CVSD_TONE_BUFFER_SIZE_IN_BYTES  320
#define MSBC_TONE_BUFFER_SIZE_IN_BYTES  640

#define DL_ONLY TRUE
#define UL_ONLY TRUE
#ifdef AIR_HFP_DNN_PATH_ENABLE
#define UL_DNN_EN TRUE
#endif

#ifdef AVM_SHAEE_BUF_INFO
#define MSBC_SHARE_BUF_FRAME_SIZE 60
#define CVSD_SHARE_BUF_FRAME_SIZE 120
#define HFP_SHARE_BUF_PARTTERN_TOTAL_SIZE 480
#endif

log_create_module(BT_HFP_CODEC, PRINT_LEVEL_INFO);
#define TASK_LOG_MSGID_E(fmt, arg...)     //LOG_MSGID_E(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define TASK_LOG_MSGID_W(fmt, arg...)     //LOG_MSGID_W(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define TASK_LOG_MSGID_I(fmt, arg...)     //LOG_MSGID_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define TASK_LOG_MSGID_CTRL(fmt, arg...)  LOG_MSGID_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_E(fmt, arg...)     //LOG_MSGID_E(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_W(fmt, arg...)     LOG_MSGID_W(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_I(fmt, arg...)     //LOG_MSGID_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)
#define LISR_LOG_MSGID_CTRL(fmt, arg...)  //LOG_MSGID_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg)

#define RETURN_NULL_IF_FALSE(expr)          do { if (!(expr)) { return NULL;                        } } while(0)
#define RETURN_MEDIA_ERROR_IF_FALSE(expr)   do { if (!(expr)) { return BT_CODEC_MEDIA_STATUS_ERROR; } } while(0)
#define RETURN_IF_FALSE(expr)               do { if (!(expr)) { return;                             } } while(0)

/*******************************************************************************\
| Private Functions                                                             |
\*******************************************************************************/

#if defined(MTK_BT_HFP_CODEC_DEBUG)
#include "ff.h"
#include "bt_hci_log.h"
#include "hal_audio_internal_vm.h"
static FATFS fatfs;
static FIL fdst_hfp_downlink_stream_out_pcm;
static FIL fdst_hfp_downlink_esco_raw_data;
uint32_t bt_hfp_codec_debug_flag_global = 0;
bool bt_hfp_codec_debug_is_open = false;

#define DEBUG_ARRAY_SIZE (1600 * 2 * 100)   // we allocate an array that can audio data. 1600 * 2 can store 0.1s in 16000 sampling rate.
typedef struct {
    uint8_t debug_array[DEBUG_ARRAY_SIZE];
    uint32_t debug_array_write_index;
} bt_hfp_codec_debug_array_t;

static bt_hfp_codec_debug_array_t bt_hfp_codec_debug_array;


static int32_t bt_hfp_codec_get_debug_array_space_size(void)
{
    return DEBUG_ARRAY_SIZE - bt_hfp_codec_debug_array.debug_array_write_index;
}

static void bt_hfp_codec_save_debug_data_to_debug_array_if_full_write_to_sdcard(FIL *fil_handle, uint8_t *debug_data, uint32_t debug_data_length)
{
    FRESULT f_res;

    if ((bt_hfp_codec_get_debug_array_space_size() - (int32_t)debug_data_length) >= 0) {
        //TASK_LOG_CTRL("bt_hfp_codec_get_debug_array_space_size()=%d, DEBUG_ARRAY_SIZE=%d, debug_array_write_index=%d, debug_data_length=%d\r\n", bt_hfp_codec_get_debug_array_space_size(), DEBUG_ARRAY_SIZE, bt_hfp_codec_debug_array.debug_array_write_index, debug_data_length);
        memcpy((bt_hfp_codec_debug_array.debug_array + bt_hfp_codec_debug_array.debug_array_write_index), debug_data, debug_data_length);
        bt_hfp_codec_debug_array.debug_array_write_index += debug_data_length;

    } else {    // debug array is full, write to sd card first and then save it to debug arrary
        //TASK_LOG_CTRL("debug_array_space_size not enough. debug_array_space_size=%d, DEBUG_ARRAY_SIZE=%d, debug_array_write_index=%d, debug_data_length=%d\r\n", bt_hfp_codec_get_debug_array_space_size(), DEBUG_ARRAY_SIZE, bt_hfp_codec_debug_array.debug_array_write_index, debug_data_length);
        uint32_t write_byte_count = bt_hfp_codec_debug_array.debug_array_write_index;
        UINT length_written = 0;

        f_res = f_write(fil_handle, bt_hfp_codec_debug_array.debug_array, write_byte_count, &length_written);

        if (f_res == FR_OK) {
            TASK_LOG_MSGID_CTRL("write ok, write_byte_count=%d, length_written=%d, *debug_data=%x, *(debug_data+1)=%x, *(debug_data+2)=%x\r\n",5, write_byte_count, length_written, *debug_data, *(debug_data+1), *(debug_data+2));

            bt_hfp_codec_debug_array.debug_array_write_index = 0;
            memcpy((bt_hfp_codec_debug_array.debug_array + bt_hfp_codec_debug_array.debug_array_write_index), debug_data, debug_data_length);
            bt_hfp_codec_debug_array.debug_array_write_index += debug_data_length;
        } else {
            //TASK_LOG_CTRL("write fail, throw away some data. f_res=%d, length_written=%d", f_res, length_written);
            bt_hfp_codec_debug_array.debug_array_write_index = 0;
        }
    }
}
#endif

#if !defined(MTK_AVM_DIRECT)
static uint32_t bt_codec_get_interrupt_time (bool is_isr)
{
    /* Check interrupt interval */
    uint32_t curr_cnt = 0;
    hal_gpt_status_t result = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    if (HAL_GPT_STATUS_OK != result) {
        if (is_isr) {
            LISR_LOG_MSGID_I("gpt result %d\r\n",1, result);
        } else {
            TASK_LOG_MSGID_I("gpt result %d\r\n",1, result);
        }
    }
    return curr_cnt;
}
#endif

#ifdef AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE
SemaphoreHandle_t g_xSemaphore_hfp_nvkey_change_notify;
static int g_call_mode = 0;
void hfp_dsp_nvkey_replace(bt_hfp_audio_codec_type_t hfp_codec_type,DSP_FEATURE_TYPE_LIST *dsp_feature_list)
{
    /* g_call_mode must be < BT_CODEC_HFP_MODE_MAX */
    configASSERT(g_call_mode < BT_CODEC_HFP_MODE_MAX);

    /*config the nvkey base on g_call_mode*/
    if (hfp_codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        dsp_feature_list[0] = FUNC_VOICE_NB_MODE0 + g_call_mode;
    }else if (hfp_codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        dsp_feature_list[0] = FUNC_VOICE_WB_MODE0 + g_call_mode;
    }
    TASK_LOG_MSGID_CTRL("[CALL mode] feature[0]=%d", 1, dsp_feature_list[0]);
}

void audio_voice_change_mode_notify_callback(audio_nvdm_user_t user, audio_nvdm_status_t status, void *user_data)
{
    switch( status ){
        case AUDIO_NVDM_STATUS_PREV_CHANGE:
        case AUDIO_NVDM_STATUS_CHANGE:
            break;
        /*The audio nvkey can be update now*/
        case AUDIO_NVDM_STATUS_POST_CHANGE:
            if (hal_nvic_query_exception_number() == HAL_NVIC_NOT_EXCEPTION) {
                xSemaphoreGive(g_xSemaphore_hfp_nvkey_change_notify);
            }else {
                xSemaphoreGiveFromISR(g_xSemaphore_hfp_nvkey_change_notify,NULL);
            }
            break;
        default:
            break;
    }
}

bt_codec_media_status_t bt_codec_hfp_set_mode(int mode)
 {
    static bool set_busy_flg = false;

    if (set_busy_flg == true) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    set_busy_flg = true;

    if(mode >= BT_CODEC_HFP_MODE_MAX) {
        TASK_LOG_MSGID_CTRL("[CALL mode] mode=%d, it should be 0..%d", 2, mode, BT_CODEC_HFP_MODE_MAX-1);
        set_busy_flg = false;
        return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }

    TASK_LOG_MSGID_CTRL("[CALL mode] call mode=%d", 1, mode);

    if(g_call_mode == mode) {
        TASK_LOG_MSGID_CTRL("[CALL mode] call mode is not changed, do nothing", 0);
        set_busy_flg = false;
        return BT_CODEC_MEDIA_STATUS_OK;
    }

    /* update call mode */
    g_call_mode = mode;

    if(bt_sink_srv_ami_get_current_scenario() == HFP){
        /* Call mode exist.*/
        bt_sink_srv_am_result_t am_status;
        TASK_LOG_MSGID_CTRL("[CALL mode] HFP resume start", 0);
        am_status = am_audio_set_resume();
        if (am_status != AUD_EXECUTION_SUCCESS){
            TASK_LOG_MSGID_CTRL("[CALL mode] HFP resume fail", 0);
            set_busy_flg = false;
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }else{
            /*wait for audio nvkey sync to DSP*/
            if (pdTRUE != xSemaphoreTake(g_xSemaphore_hfp_nvkey_change_notify,1000 / portTICK_PERIOD_MS)) {
                TASK_LOG_MSGID_CTRL("[CALL mode] HFP resume fail,sem timeout", 0);
                set_busy_flg = false;
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }else {
                TASK_LOG_MSGID_CTRL("[CALL mode] HFP resume success", 0);
                set_busy_flg = false;
                return BT_CODEC_MEDIA_STATUS_OK;
            }
        }
    }else{
        set_busy_flg = false;
        return BT_CODEC_MEDIA_STATUS_OK;
    }
}
#endif

static bt_codec_media_status_t bt_hfp_cvsd_open (bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
#if defined(MTK_AVM_DIRECT)
    mcu2dsp_open_param_t open_param;
    void *p_param_share;
    sysram_status_t     status;
    // DL+UL
    DSP_FEATURE_TYPE_LIST AudioFeatureList_NBeSCO[2] =
    {
        FUNC_TX_NR,
        FUNC_END,
    };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
    if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
        AudioFeatureList_NBeSCO[0] = FUNC_NB_BOOM_MIC;
        TASK_LOG_MSGID_CTRL("[HFP][open] it's boom mic: %d", 1, mic_cur_type);
    }
#endif
#if defined(MTK_ANC_ENABLE) && defined(MTK_VOICE_ANC_EQ)
#ifndef MTK_ANC_V2
    uint8_t enable;
    uint8_t hybrid_enable;
    anc_get_status(&enable, NULL, &hybrid_enable);
    if ((enable > 0) && (hybrid_enable > 0)) {
        AudioFeatureList_NBeSCO[0] = FUNC_VOICE_NB_ANC;
    }
#endif
#endif

#if defined(MTK_ANC_ENABLE) && defined(AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE)
    /*config the nvkey base on ANC mode*/
    hfp_dsp_nvkey_replace(BT_HFP_CODEC_TYPE_CVSD,AudioFeatureList_NBeSCO);
#endif

    /* reset share buffer before put parameters*/
    audio_nvdm_reset_sysram();
    status = audio_nvdm_set_feature(2, AudioFeatureList_NBeSCO);
    if (status != NVDM_STATUS_NAT_OK){
        TASK_LOG_MSGID_CTRL("CVSD_open is failed to set parameters to share memory - err(%d)\r\n", 1, status);
        configASSERT(0);
    }
#ifdef MTK_ANC_ENABLE
    audio_set_anc_compensate(HFP, 0, NULL);
    //audio_set_anc_compensate_phase2(HFP, 0);
#endif


    // hfp dvfs setting
    hal_audio_set_dvfs_clk(AU_DSP_VOICE, &g_hfp_dvfs_clk);
    TASK_LOG_MSGID_CTRL("[HFP][open] hal_audio_set_dvfs_clk: %d", 1, g_hfp_dvfs_clk);
    if ((gDspEcnrAlg_MicType == ECNR_DUAL_MIC) && (g_hfp_dvfs_clk < HFP_DVFS_ECNR_DUAL_MIC_SPEED)){
       g_hfp_dvfs_clk = HFP_DVFS_ECNR_DUAL_MIC_SPEED;
       TASK_LOG_MSGID_CTRL("[HFP][open] ECNR dual mic algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
    }

#if defined(MTK_INEAR_ENHANCEMENT)
    if(g_hfp_dvfs_clk < HFP_DVFS_INEAR_ENHANCEMENT_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_INEAR_ENHANCEMENT_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] INEAR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#elif defined(MTK_DUALMIC_INEAR)
    if(g_hfp_dvfs_clk < HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] Dualmic INEAR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#elif defined(MTK_3RD_PARTY_NR)
    if(g_hfp_dvfs_clk < HFP_DVFS_3RD_PARTY_NR_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_3RD_PARTY_NR_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] 3rd Party NR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#endif

#if defined(AIR_HFP_DNN_PATH_ENABLE)
    if(g_hfp_dvfs_clk < HFP_DVFS_MAX_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_MAX_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] DNN path enable, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#endif

   if((g_hfp_dvfs_clk > HFP_DVFS_DEFAULT_SPEED) && (g_hfp_dvfs_clk <= HFP_DVFS_MAX_SPEED)){
       hal_dvfs_lock_control(g_hfp_dvfs_clk, HAL_DVFS_LOCK);
       TASK_LOG_MSGID_CTRL("[HFP][open] dvfs lock to step: %d", 1, g_hfp_dvfs_clk);
   }

    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

    open_param.param.stream_in  = STREAM_IN_HFP;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.hfp.codec_type     = BT_HFP_CODEC_TYPE_CVSD;
    open_param.stream_in_param.hfp.bt_inf_address = bt_sink_srv_ami_get_bt_inf_address();
    #ifdef AVM_SHAEE_BUF_INFO
    open_param.stream_in_param.hfp.p_share_info   = (avm_share_buf_info_t *)hal_audio_query_bt_voice_dl_share_info();
    #else
    open_param.stream_in_param.hfp.p_share_info   = hal_audio_query_bt_voice_dl_share_info();
    #endif
    open_param.stream_in_param.hfp.clk_info_address   = hal_audio_query_rcdc_share_info();
#ifdef MTK_AIRDUMP_EN
    open_param.stream_in_param.hfp.p_air_dump_buf   = hal_audio_query_hfp_air_dump();
#endif
#ifndef AVM_SHAEE_BUF_INFO
    hal_audio_reset_share_info(open_param.stream_in_param.hfp.p_share_info);
    TASK_LOG_MSGID_CTRL("mcuDL Codec:%d, shareP:%X\r\n", 2, open_param.stream_in_param.hfp.codec_type, open_param.stream_in_param.hfp.p_share_info->start_addr);
#else
    hal_audio_set_hfp_avm_info(open_param.stream_in_param.hfp.p_share_info, HFP_SHARE_BUF_PARTTERN_TOTAL_SIZE, CVSD_SHARE_BUF_FRAME_SIZE);
    TASK_LOG_MSGID_CTRL("mcuDL Codec type:%d, shareP:0x%x, rx forwarder:0x%x\r\n", 2, open_param.stream_in_param.hfp.codec_type, open_param.stream_in_param.hfp.p_share_info->StartAddr, open_param.stream_in_param.hfp.p_share_info->ForwarderAddr);
#endif

#if 0
    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
    open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }
#else
    hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param.stream_out_param);
    TASK_LOG_MSGID_I("CVSD_open out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
    open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
    open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate   = 16000;
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate   = 16000;
#endif
    open_param.stream_out_param.afe.irq_period      = 15;
    open_param.stream_out_param.afe.frame_size      = 240;
    open_param.stream_out_param.afe.frame_number    = 6;
    open_param.stream_out_param.afe.hw_gain         = true;
#ifdef ENABLE_HWSRC_CLKSKEW
    #if (PRODUCT_VERSION == 1565)
        #if defined AIR_HWSRC_TX_TRACKING_ENABLE || defined AIR_HWSRC_RX_TRACKING_ENABLE
        open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
        #else
        open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;
        #endif
    #else
    open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
    #endif
    TASK_LOG_MSGID_CTRL("[HFP][MCU] open_param.stream_out_param.afe.clkskew_mode:%d", 1, open_param.stream_out_param.afe.clkskew_mode);
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_out_param.afe.enable_ul_dnn   = true;
#endif
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, HAL_AUDIO_SAMPLING_RATE_16KHZ, true);
#endif
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_DL);
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_VOICE_DL, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, TRUE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_OPEN, AUDIO_DSP_CODEC_TYPE_CVSD, (uint32_t)p_param_share, true);

///////// UL DNN ///////////////////////////////////////////////////////////////////////////////////////////

#ifdef AIR_HFP_DNN_PATH_ENABLE
#if (UL_DNN_EN == TRUE)
    open_param.param.stream_in  = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param);
    open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_R;

    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM_SUB;
    //open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE; //AFE_PCM_FORMAT_S32_LE
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_in_param.afe.sampling_rate   = 16000;
    open_param.stream_in_param.afe.irq_period      = 15;
    open_param.stream_in_param.afe.frame_size      = 240;
    open_param.stream_in_param.afe.frame_number    = 6;
    open_param.stream_in_param.afe.hw_gain         = false;
    open_param.stream_in_param.afe.enable_ul_dnn   = true;

    hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param.stream_out_param);
    open_param.stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_R;
    open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM3;
    open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE; //AFE_PCM_FORMAT_S32_LE
    open_param.stream_out_param.afe.stream_out_sampling_rate   = 16000;
    open_param.stream_out_param.afe.sampling_rate   = 16000;
    open_param.stream_out_param.afe.irq_period      = 15;
    open_param.stream_out_param.afe.frame_size      = 240;
    open_param.stream_out_param.afe.frame_number    = 4;
    open_param.stream_out_param.afe.hw_gain         = false;
    open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
    open_param.stream_out_param.afe.enable_ul_dnn   = true;

    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_OPEN, AUDIO_DSP_CODEC_TYPE_DNN, (uint32_t)p_param_share, true);
#endif
#endif

/////// UL /////////////////////////////////////////////////////////////////////////////////

    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

    open_param.param.stream_in  = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_HFP;

    open_param.stream_out_param.hfp.codec_type  = BT_HFP_CODEC_TYPE_CVSD;

#ifndef AVM_SHAEE_BUF_INFO
    open_param.stream_out_param.hfp.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_bt_voice_ul_share_info();
    hal_audio_reset_share_info(open_param.stream_out_param.hfp.p_share_info);
    TASK_LOG_MSGID_CTRL("mcuUL Codec:%d, shareP:%X\r\n",2, open_param.stream_out_param.hfp.codec_type, open_param.stream_out_param.hfp.p_share_info->start_addr);
#else
    open_param.stream_out_param.hfp.p_share_info = (avm_share_buf_info_t *)hal_audio_query_bt_voice_ul_share_info();
    hal_audio_set_hfp_avm_info(open_param.stream_out_param.hfp.p_share_info, HFP_SHARE_BUF_PARTTERN_TOTAL_SIZE, CVSD_SHARE_BUF_FRAME_SIZE);
    TASK_LOG_MSGID_CTRL("mcuUL Codec type:%d, shareP:0x%x, tx forwarder:0x%x\r\n", 2, open_param.stream_out_param.hfp.codec_type, open_param.stream_out_param.hfp.p_share_info->StartAddr, open_param.stream_out_param.hfp.p_share_info->ForwarderAddr);
#endif
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_HFP_AVC, am_hfp_ndvc_callback);
#endif

#if 0
    if(((*((volatile uint32_t*)(0xA2120B04)) >> 5)&0x01) == 1) {
        //AMIC mono L
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
    } else if(((*((volatile uint32_t*)(0xA2120B04)) >> 6)&0x01) == 1) {
        //AMIC mono R
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_R;
    } else {
        //default AMIC mono L
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
    }

    open_param.stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;//HAL_AUDIO_MEM3 to enable echo referencr;
    open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    }
#else
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param);

    #ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_L;
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1;
    #else
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;//HAL_AUDIO_MEM3 to enable echo referencr;
    #endif
    TASK_LOG_MSGID_I("CVSD_open in_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_in_param.afe.audio_device, open_param.stream_in_param.afe.stream_channel, open_param.stream_in_param.afe.audio_interface);
#endif
    #ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
    #else
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    #endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K)
    open_param.stream_in_param.afe.sampling_rate   = 48000;
    open_param.stream_in_param.afe.frame_size      = 240 * 3;
#else
    open_param.stream_in_param.afe.sampling_rate   = 16000;
    open_param.stream_in_param.afe.frame_size      = 240;
#endif
    open_param.stream_in_param.afe.irq_period      = 15;
    open_param.stream_in_param.afe.frame_number    = 3;
    open_param.stream_in_param.afe.hw_gain         = false;
#ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.enable_ul_dnn   = true;
#endif
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_SLAVE){
        //for esco UL Downsample
        // open_param.stream_in_param.afe.with_upwdown_sampler     = true;
        // open_param.stream_in_param.afe.audio_path_input_rate    = open_param.stream_out_param.afe.sampling_rate;
        // open_param.stream_in_param.afe.audio_path_output_rate   = open_param.stream_in_param.afe.sampling_rate;
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        open_param.stream_in_param.afe.sampling_rate   = 16000;
        open_param.stream_in_param.afe.stream_out_sampling_rate   = 16000;
        #endif
        #ifdef AIR_DUAL_CHIP_I2S_ENABLE
        open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1;//remove HAL_AUDIO_MEM3 to enable echo reference from i2s slave;
        #endif
    }
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER){
        //for esco UL Downsample
        open_param.stream_in_param.afe.with_upwdown_sampler     = true;
        open_param.stream_in_param.afe.audio_path_input_rate    = HAL_AUDIO_FIXED_AFE_SAMPLE_RATE;
        open_param.stream_in_param.afe.audio_path_output_rate   = open_param.stream_in_param.afe.sampling_rate;
    }

#if (UL_ONLY == TRUE)
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_OPEN, AUDIO_DSP_CODEC_TYPE_CVSD, (uint32_t)p_param_share, true);
#endif

#else
    uint32_t tx_buffer_size = 0;
    uint32_t rx_buffer_size = 0;
    uint8_t *tx_buffer_pointer;
    uint8_t *rx_buffer_pointer;
    uint32_t encoder_buffer_size           = cvsd_encoder_get_buffer_size();
    uint32_t src_up_sampling_buffer_size   = cvsd_src_up_sampling_get_buffer_size();
    uint32_t decoder_buffer_size           = cvsd_decoder_get_buffer_size();
    uint32_t src_down_sampling_buffer_size = cvsd_src_down_sampling_get_buffer_size();
    uint32_t plc_buffer_size               = plc_get_buffer_size();
    {   /* Force buffer size to be 4-byte-aligned */
        encoder_buffer_size           = FOUR_BYTE_ALIGNED(encoder_buffer_size);
        src_up_sampling_buffer_size   = FOUR_BYTE_ALIGNED(src_up_sampling_buffer_size);
        decoder_buffer_size           = FOUR_BYTE_ALIGNED(decoder_buffer_size);
        src_down_sampling_buffer_size = FOUR_BYTE_ALIGNED(src_down_sampling_buffer_size);
        plc_buffer_size               = FOUR_BYTE_ALIGNED(plc_buffer_size);
    }
    {   /* Record CVSD related SWIP internal buffer sizes */
        p_tx_info->encoder.internal_buffer_size           = encoder_buffer_size;
        p_tx_info->src_up_sampling.internal_buffer_size   = src_up_sampling_buffer_size;
        p_rx_info->decoder.internal_buffer_size           = decoder_buffer_size;
        p_rx_info->src_down_sampling.internal_buffer_size = src_down_sampling_buffer_size;
        p_rx_info->plc.internal_buffer_size               = plc_buffer_size;
    }
    {   /* Sum up to total internal buffer size */
        tx_buffer_size += encoder_buffer_size;
        tx_buffer_size += src_up_sampling_buffer_size;
        tx_buffer_size += BT_HFP_TX_BITSTREAM_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET;
        tx_buffer_size += BT_HFP_TX_PCM_RING_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
        rx_buffer_size += decoder_buffer_size;
        rx_buffer_size += src_down_sampling_buffer_size;
        rx_buffer_size += plc_buffer_size;
        rx_buffer_size += BT_HFP_RX_BITSTREAM_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET;
        rx_buffer_size += BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
    }
    {   /* Allocate total internal buffer */
        tx_buffer_pointer = (uint8_t *)pvPortMalloc(tx_buffer_size);
        if (tx_buffer_pointer == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_cvsd_open: tx_buffer_pointer allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        p_tx_info->buffer_size    = tx_buffer_size;
        p_tx_info->buffer_pointer = tx_buffer_pointer;

        rx_buffer_pointer = (uint8_t *)pvPortMalloc(rx_buffer_size);
        if (rx_buffer_pointer == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_cvsd_open: rx_buffer_pointer allocate fail\r\n", 0);
            vPortFree(p_tx_info->buffer_pointer);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        p_rx_info->buffer_size    = rx_buffer_size;
        p_rx_info->buffer_pointer = rx_buffer_pointer;
    }
    {   /* Assign internal buffer to each SWIP */
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->encoder.handle          , encoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->src_up_sampling.handle  , src_up_sampling_buffer_size);
        ASSIGN_U8_TX_2D_BUFFER_UPDATE_POINTER (tx_buffer_pointer, p_tx_info->pkt_buf                 , BT_HFP_TX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_8khz_buf            , BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_64khz_buf           , BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tmp_buf             , BT_HFP_TX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (tx_buffer_pointer, p_tx_info->pcm_ring_buf            , BT_HFP_TX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tx_tmp_buf          , BT_HFP_DSP_FRAME_SIZE);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->decoder.handle          , decoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->src_down_sampling.handle, src_down_sampling_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->plc.handle              , plc_buffer_size);
        ASSIGN_U8_RX_2D_BUFFER_UPDATE_POINTER (rx_buffer_pointer, p_rx_info->pkt_buf                 , BT_HFP_RX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_64khz_buf           , BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_8khz_buf            , BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_tmp_buf             , BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pcm_ring_buf            , BT_HFP_RX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_rx_tmp_buf          , BT_HFP_DSP_FRAME_SIZE);
    }
#endif

    return BT_CODEC_MEDIA_STATUS_OK;
}

#if !defined(MTK_AVM_DIRECT)
static void bt_hfp_cvsd_close(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    vPortFree(p_tx_info->buffer_pointer);
    vPortFree(p_rx_info->buffer_pointer);
    p_tx_info->buffer_pointer = NULL;
    p_rx_info->buffer_pointer = NULL;
    return;
}
#endif

static bt_codec_media_status_t bt_hfp_msbc_open(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
#if defined(MTK_AVM_DIRECT)
    // Notify to do dynamic download.
    mcu2dsp_open_param_t open_param;
    void *p_param_share;
    sysram_status_t     status;
    // DL+UL
    DSP_FEATURE_TYPE_LIST AudioFeatureList_WBeSCO[2] =
    {
        FUNC_RX_NR,
        FUNC_END,
    };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
    if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
        AudioFeatureList_WBeSCO[0] = FUNC_WB_BOOM_MIC;
        TASK_LOG_MSGID_CTRL("[HFP][open] it's boom mic: %d", 1, mic_cur_type);
    }
#endif
#if defined(MTK_ANC_ENABLE) && defined(MTK_VOICE_ANC_EQ)
#ifndef MTK_ANC_V2
    uint8_t enable;
    uint8_t hybrid_enable;
    anc_get_status(&enable, NULL, &hybrid_enable);
    if ((enable > 0) && (hybrid_enable > 0)) {
        AudioFeatureList_WBeSCO[0] = FUNC_VOICE_WB_ANC;
    }
#endif
#endif

#if defined(MTK_ANC_ENABLE) && defined(AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE)
    /*config the nvkey base on ANC mode*/
    hfp_dsp_nvkey_replace(BT_HFP_CODEC_TYPE_MSBC,AudioFeatureList_WBeSCO);
#endif

    /* reset share buffer before put parameters*/
    audio_nvdm_reset_sysram();
    status = audio_nvdm_set_feature(2, AudioFeatureList_WBeSCO);
    if (status != NVDM_STATUS_NAT_OK){
        TASK_LOG_MSGID_CTRL("MSBC_open is failed to set parameters to share memory - err(%d)\r\n", 1, status);
        configASSERT(0);
    }
/////////// Downlink /////////////////////////////////////////////////////////////////////////////////////////
#ifdef MTK_ANC_ENABLE
    audio_set_anc_compensate(HFP, 0, NULL);
    //audio_set_anc_compensate_phase2(HFP, 0);
#endif

    // hfp dvfs setting
    hal_audio_set_dvfs_clk(AU_DSP_VOICE, &g_hfp_dvfs_clk);
    TASK_LOG_MSGID_CTRL("[HFP][open] hal_audio_set_dvfs_clk: %d", 1, g_hfp_dvfs_clk);
    if ((gDspEcnrAlg_MicType == ECNR_DUAL_MIC) && (g_hfp_dvfs_clk < HFP_DVFS_ECNR_DUAL_MIC_SPEED)){
       g_hfp_dvfs_clk = HFP_DVFS_ECNR_DUAL_MIC_SPEED;
       TASK_LOG_MSGID_CTRL("[HFP][open] ECNR dual mic algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
    }

#if defined(MTK_INEAR_ENHANCEMENT)
    if(g_hfp_dvfs_clk < HFP_DVFS_INEAR_ENHANCEMENT_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_INEAR_ENHANCEMENT_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] INEAR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#elif defined(MTK_DUALMIC_INEAR)
    if(g_hfp_dvfs_clk < HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_DUAL_MIC_INEAR_ENHANCEMENT_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] Dualmic INEAR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#elif defined(MTK_3RD_PARTY_NR)
    if(g_hfp_dvfs_clk < HFP_DVFS_3RD_PARTY_NR_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_3RD_PARTY_NR_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] 3rd Party NR algorithm, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#endif

#if defined(AIR_HFP_DNN_PATH_ENABLE)
    if(g_hfp_dvfs_clk < HFP_DVFS_MAX_SPEED){
       g_hfp_dvfs_clk = HFP_DVFS_MAX_SPEED;
    }
    TASK_LOG_MSGID_CTRL("[HFP][open] DNN path enable, dvfs lock to: %d", 1, g_hfp_dvfs_clk);
#endif

   if((g_hfp_dvfs_clk > HFP_DVFS_DEFAULT_SPEED) && (g_hfp_dvfs_clk <= HFP_DVFS_MAX_SPEED)){
       hal_dvfs_lock_control(g_hfp_dvfs_clk, HAL_DVFS_LOCK);
       TASK_LOG_MSGID_CTRL("[HFP][open] dvfs lock to step: %d", 1, g_hfp_dvfs_clk);
   }

    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

    open_param.param.stream_in  = STREAM_IN_HFP;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.hfp.codec_type     = BT_HFP_CODEC_TYPE_MSBC;
    #ifdef AVM_SHAEE_BUF_INFO
    open_param.stream_in_param.hfp.p_share_info   = (avm_share_buf_info_t *)(hal_audio_query_bt_voice_dl_share_info());
    #else
    open_param.stream_in_param.hfp.p_share_info   = hal_audio_query_bt_voice_dl_share_info();
    #endif
    open_param.stream_in_param.hfp.bt_inf_address = bt_sink_srv_ami_get_bt_inf_address();
    open_param.stream_in_param.hfp.clk_info_address   = hal_audio_query_rcdc_share_info();
#ifdef MTK_AIRDUMP_EN
    open_param.stream_in_param.hfp.p_air_dump_buf   = hal_audio_query_hfp_air_dump();
#endif

#ifndef AVM_SHAEE_BUF_INFO
    hal_audio_reset_share_info(open_param.stream_in_param.hfp.p_share_info);
    TASK_LOG_MSGID_CTRL("mcuDL Codec:%d, shareP:%X\r\n", 2, open_param.stream_in_param.hfp.codec_type, open_param.stream_in_param.hfp.p_share_info->start_addr);
#else
    hal_audio_set_hfp_avm_info(open_param.stream_in_param.hfp.p_share_info, HFP_SHARE_BUF_PARTTERN_TOTAL_SIZE, MSBC_SHARE_BUF_FRAME_SIZE);
    TASK_LOG_MSGID_CTRL("mcuDL Codec type:%d, shareP:0x%x, rx forwarder:0x%x\r\n", 2, open_param.stream_in_param.hfp.codec_type, open_param.stream_in_param.hfp.p_share_info->StartAddr, open_param.stream_in_param.hfp.p_share_info->ForwarderAddr);
#endif


#if 0
    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
    open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }
#else
    hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param.stream_out_param);
    TASK_LOG_MSGID_I("MSBC_open out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
    open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
    open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate   = 16000;
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
    open_param.stream_out_param.afe.sampling_rate   = 16000;
#endif
    open_param.stream_out_param.afe.irq_period      = 15;
    open_param.stream_out_param.afe.frame_size      = 240;
    open_param.stream_out_param.afe.frame_number    = 4;
    open_param.stream_out_param.afe.hw_gain         = true;
#ifdef ENABLE_HWSRC_CLKSKEW
    #if (PRODUCT_VERSION == 1565)
        #if defined AIR_HWSRC_TX_TRACKING_ENABLE || defined AIR_HWSRC_RX_TRACKING_ENABLE
        open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
        #else
        open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;
        #endif
    #else
    open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
    #endif
    TASK_LOG_MSGID_CTRL("[HFP][MCU] open_param.stream_out_param.afe.clkskew_mode:%d", 1, open_param.stream_out_param.afe.clkskew_mode);
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_out_param.afe.enable_ul_dnn   = true;
#endif
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, HAL_AUDIO_SAMPLING_RATE_16KHZ, true);
#endif

#if (DL_ONLY == TRUE)
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_DL);
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_VOICE_DL, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, TRUE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_OPEN, AUDIO_DSP_CODEC_TYPE_MSBC, (uint32_t)p_param_share, true);
#endif

///////// UL DNN ///////////////////////////////////////////////////////////////////////////////////////////

#ifdef AIR_HFP_DNN_PATH_ENABLE
#if (UL_DNN_EN == TRUE)
    open_param.param.stream_in  = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_AFE;
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param);
    open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_R;

    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM_SUB;
    //open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE; //AFE_PCM_FORMAT_S32_LE
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
    open_param.stream_in_param.afe.sampling_rate   = 16000;
    open_param.stream_in_param.afe.irq_period      = 15;
    open_param.stream_in_param.afe.frame_size      = 240;
    open_param.stream_in_param.afe.frame_number    = 6;
    open_param.stream_in_param.afe.hw_gain         = false;
    open_param.stream_in_param.afe.enable_ul_dnn   = true;

    hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param.stream_out_param);
    open_param.stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_R;
    open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM3;
    open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE; //AFE_PCM_FORMAT_S32_LE
    open_param.stream_out_param.afe.stream_out_sampling_rate   = 16000;
    open_param.stream_out_param.afe.sampling_rate   = 16000;
    open_param.stream_out_param.afe.irq_period      = 15;
    open_param.stream_out_param.afe.frame_size      = 240;
    open_param.stream_out_param.afe.frame_number    = 4;
    open_param.stream_out_param.afe.hw_gain         = false;
    open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
    open_param.stream_out_param.afe.enable_ul_dnn   = true;

    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_OPEN, AUDIO_DSP_CODEC_TYPE_DNN, (uint32_t)p_param_share, true);
#endif
#endif

/////////// Uplink /////////////////////////////////////////////////////////////////////////////////////////
    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

    open_param.param.stream_in  = STREAM_IN_AFE;
    open_param.param.stream_out = STREAM_OUT_HFP;

    open_param.stream_out_param.hfp.codec_type  = BT_HFP_CODEC_TYPE_MSBC;
#ifndef AVM_SHAEE_BUF_INFO
    open_param.stream_out_param.hfp.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_bt_voice_ul_share_info();
    hal_audio_reset_share_info(open_param.stream_out_param.hfp.p_share_info);
    TASK_LOG_MSGID_CTRL("mcuUL Codec:%d, shareP:%X\r\n",2, open_param.stream_out_param.hfp.codec_type, open_param.stream_out_param.hfp.p_share_info->start_addr);
#else
    open_param.stream_out_param.hfp.p_share_info = (avm_share_buf_info_t *)hal_audio_query_bt_voice_ul_share_info();
    hal_audio_set_hfp_avm_info(open_param.stream_out_param.hfp.p_share_info, HFP_SHARE_BUF_PARTTERN_TOTAL_SIZE, MSBC_SHARE_BUF_FRAME_SIZE);
    TASK_LOG_MSGID_CTRL("mcuUL Codec type:%d, shareP:0x%x, tx forwarder:0x%x\r\n", 2, open_param.stream_out_param.hfp.codec_type, open_param.stream_out_param.hfp.p_share_info->StartAddr, open_param.stream_out_param.hfp.p_share_info->ForwarderAddr);
#endif

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_HFP_AVC, am_hfp_ndvc_callback);
#endif
#if 0
    if(((*((volatile uint32_t*)(0xA2120B04)) >> 5)&0x01) == 1) {
        //AMIC mono L
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
    } else if(((*((volatile uint32_t*)(0xA2120B04)) >> 6)&0x01) == 1) {
        //AMIC mono R
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_R;
    } else {
        //default AMIC mono L
        open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_MAIN_MIC_L;
    }

    open_param.stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;//HAL_AUDIO_MEM3 to enable echo referencr
    open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param.stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    }
#else
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param);

    #ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER_L;
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1;
    #else
    open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;//HAL_AUDIO_MEM3 to enable echo referencr;
    #endif
    TASK_LOG_MSGID_CTRL("MSBC_open in_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_in_param.afe.audio_device, open_param.stream_in_param.afe.stream_channel, open_param.stream_in_param.afe.audio_interface);
#endif
    #ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
    #else
    open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    #endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K)
    open_param.stream_in_param.afe.sampling_rate   = 48000;
    open_param.stream_in_param.afe.frame_size      = 240 * 3;
#else
    open_param.stream_in_param.afe.sampling_rate   = 16000;
    open_param.stream_in_param.afe.frame_size      = 240;
#endif
    open_param.stream_in_param.afe.irq_period      = 15;
    open_param.stream_in_param.afe.frame_number    = 3;
    open_param.stream_in_param.afe.hw_gain         = false;
#ifdef AIR_HFP_DNN_PATH_ENABLE
    open_param.stream_in_param.afe.enable_ul_dnn   = true;
#endif
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_SLAVE){
        //for esco UL Downsample
        // open_param.stream_in_param.afe.with_upwdown_sampler     = true;
        // open_param.stream_in_param.afe.audio_path_input_rate    = open_param.stream_out_param.afe.sampling_rate;
        // open_param.stream_in_param.afe.audio_path_output_rate   = open_param.stream_in_param.afe.sampling_rate;
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        open_param.stream_in_param.afe.sampling_rate   = 16000;
        open_param.stream_in_param.afe.stream_out_sampling_rate   = 16000;
        #endif
        #ifdef AIR_DUAL_CHIP_I2S_ENABLE
        open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1;//remove HAL_AUDIO_MEM3 to enable echo reference from i2s slave;
        #endif
    }
    if(open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER){
        //for esco UL Downsample
        open_param.stream_in_param.afe.with_upwdown_sampler     = true;
        open_param.stream_in_param.afe.audio_path_input_rate    = HAL_AUDIO_FIXED_AFE_SAMPLE_RATE;
        open_param.stream_in_param.afe.audio_path_output_rate   = open_param.stream_in_param.afe.sampling_rate;
    }

#if (UL_ONLY == TRUE)
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_OPEN, AUDIO_DSP_CODEC_TYPE_MSBC, (uint32_t)p_param_share, true);
#endif

#else /*defined(MTK_AVM_DIRECT)*/
    uint32_t tx_buffer_size = 0;
    uint32_t rx_buffer_size = 0;
    uint8_t *tx_buffer_pointer;
    uint8_t *rx_buffer_pointer;
    uint32_t encoder_buffer_size = msbc_encoder_get_buffer_size();
    uint32_t decoder_buffer_size = msbc_decoder_get_buffer_size();
    uint32_t plc_buffer_size     = plc_get_buffer_size();
    {   /* Force buffer size to be 4-byte-aligned */
        encoder_buffer_size = FOUR_BYTE_ALIGNED(encoder_buffer_size);
        decoder_buffer_size = FOUR_BYTE_ALIGNED(decoder_buffer_size);
        plc_buffer_size     = FOUR_BYTE_ALIGNED(plc_buffer_size);
    }
    {   /* Record CVSD related SWIP internal buffer sizes */
        p_tx_info->encoder.internal_buffer_size = encoder_buffer_size;
        p_rx_info->decoder.internal_buffer_size = decoder_buffer_size;
        p_rx_info->plc.internal_buffer_size     = plc_buffer_size;
    }
    {   /* Sum up to total internal buffer size */
        tx_buffer_size += encoder_buffer_size;
        tx_buffer_size += BT_HFP_TX_BITSTREAM_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
        tx_buffer_size += BT_HFP_TX_PCM_RING_BUFFER_SIZE;
        tx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
        rx_buffer_size += decoder_buffer_size;
        rx_buffer_size += plc_buffer_size;
        rx_buffer_size += BT_HFP_RX_BITSTREAM_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_RX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME;
        rx_buffer_size += BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
        rx_buffer_size += BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        rx_buffer_size += BT_HFP_DSP_FRAME_SIZE;
    }
    {   /* Allocate total internal buffer */
        tx_buffer_pointer = (uint8_t *)pvPortMalloc(tx_buffer_size);
        if (tx_buffer_pointer == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_msbc_open: tx_buffer_pointer allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        p_tx_info->buffer_size    = tx_buffer_size;
        p_tx_info->buffer_pointer = tx_buffer_pointer;

        rx_buffer_pointer = (uint8_t *)pvPortMalloc(rx_buffer_size);
        if (rx_buffer_pointer == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_msbc_open: rx_buffer_pointer allocate fail\r\n", 0);
            vPortFree(p_tx_info->buffer_pointer);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        p_rx_info->buffer_size    = rx_buffer_size;
        p_rx_info->buffer_pointer = rx_buffer_pointer;
    }
    {   /* Assign internal buffer to each SWIP */
        ASSIGN_HANDLE_UPDATE_POINTER          (tx_buffer_pointer, p_tx_info->encoder.handle, encoder_buffer_size);
        ASSIGN_U8_TX_2D_BUFFER_UPDATE_POINTER (tx_buffer_pointer, p_tx_info->pkt_buf       , BT_HFP_TX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_16khz_buf , BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (tx_buffer_pointer, p_tx_info->pcm_ring_buf  , BT_HFP_TX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (tx_buffer_pointer, p_tx_info->pcm_tx_tmp_buf, BT_HFP_DSP_FRAME_SIZE);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->decoder.handle, decoder_buffer_size);
        ASSIGN_HANDLE_UPDATE_POINTER          (rx_buffer_pointer, p_rx_info->plc.handle    , plc_buffer_size);
        ASSIGN_U8_RX_2D_BUFFER_UPDATE_POINTER (rx_buffer_pointer, p_rx_info->pkt_buf       , BT_HFP_RX_BITSTREAM_BUFFER_SIZE);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pkt_tmp_buf   , BT_HFP_RX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME);
        ASSIGN_S16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_16khz_buf , BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
        ASSIGN_U8_BUFFER_UPDATE_POINTER       (rx_buffer_pointer, p_rx_info->pcm_ring_buf  , BT_HFP_RX_PCM_RING_BUFFER_SIZE);
        ASSIGN_U16_BUFFER_UPDATE_POINTER      (rx_buffer_pointer, p_rx_info->pcm_rx_tmp_buf, BT_HFP_DSP_FRAME_SIZE);
    }
#endif /*defined(MTK_AVM_DIRECT)*/

    return BT_CODEC_MEDIA_STATUS_OK;
}

#if !defined(MTK_AVM_DIRECT)
static void bt_hfp_msbc_close(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    vPortFree(p_tx_info->buffer_pointer);
    vPortFree(p_rx_info->buffer_pointer);
    p_tx_info->buffer_pointer = NULL;
    p_rx_info->buffer_pointer = NULL;
    return;
}

static uint32_t bt_hfp_get_packet_type_index(uint32_t pkt_type)
{
    uint32_t type_idx;
    if (pkt_type == BT_HFP_PKT_LEN_60) {
        type_idx = 0;
    } else if (pkt_type == BT_HFP_PKT_LEN_30) {
        type_idx = 1;
    } else if (pkt_type == BT_HFP_PKT_LEN_20) {
        type_idx = 2;
    } else if (pkt_type == BT_HFP_PKT_LEN_10) {
        type_idx = 3;
    } else {
        type_idx = BT_HFP_PKT_TYPE_IDX_MAX;
    }
    return type_idx;
}

static bt_hfp_packet_info_t bt_hfp_get_packet_info_from_packet_type(uint32_t bt_hw_ctrl)
{
    bt_hfp_packet_info_t bt_hfp_packet_info;
    uint32_t pkt_type    = (bt_hw_ctrl >> 18) & 0x7;
    if (pkt_type > BT_HFP_PKT_LEN_20) {
        TASK_LOG_MSGID_E("bt_hfp_read_from_bt: pkt_type(%d) out of range",1, pkt_type);
        pkt_type = BT_HFP_PKT_LEN_60;   // When pkt_type out of range, we force set to BT_HFP_PKT_LEN_60, but we have to find the root cause
    }

    bt_hfp_packet_length_t bt_hfp_packet_length = (bt_hfp_packet_length_t)pkt_type;

    switch (bt_hfp_packet_length) {
        case BT_HFP_PKT_LEN_30:
            bt_hfp_packet_info.packet_length = 30;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = true;
            bt_hfp_packet_info.packet_number_per_interrupt = 6;

            break;
        case BT_HFP_PKT_LEN_60:
            // packet length meet four bytes aligned
            bt_hfp_packet_info.packet_length = 60;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = false;
            bt_hfp_packet_info.packet_number_per_interrupt = 3;

            break;

        case BT_HFP_PKT_LEN_90:
            bt_hfp_packet_info.packet_length = 90;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = true;
            bt_hfp_packet_info.packet_number_per_interrupt = 2;

            break;
        case BT_HFP_PKT_LEN_120:
            // TODO: no this case now. We just force let the same with BT_HFP_PKT_LEN_60
            bt_hfp_packet_info.packet_length = 60;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = false;
            bt_hfp_packet_info.packet_number_per_interrupt = 3;

            break;

        case BT_HFP_PKT_LEN_10:
            bt_hfp_packet_info.packet_length = 10;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = true;
            bt_hfp_packet_info.packet_number_per_interrupt = 18;

            break;

        case BT_HFP_PKT_LEN_20:
            // packet length meet four bytes aligned
            bt_hfp_packet_info.packet_length = 20;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = false;
            bt_hfp_packet_info.packet_number_per_interrupt = 9;

            break;
#if 0
        default:
            // can't be here, just set default value as BT_HFP_PKT_LEN_60
            bt_hfp_packet_info.packet_length = 60;
            bt_hfp_packet_info.two_bytes_dummy_between_packet_flag = false;
            bt_hfp_packet_info.packet_number_per_interrupt = 3;

            break;
#endif
    }

    return bt_hfp_packet_info;

}

static void bt_hfp_read_from_shared_memory(bt_hfp_shared_memory_information_t *p_mem_info, uint8_t *p_dst, uint32_t bt_hw_ctrl)
{
    /* Copy from shared memory to temp buffer */
    uint32_t offset = *(p_mem_info->p_offset_r);
    volatile uint8_t *p_src = (volatile uint8_t *)(p_mem_info->base_address + offset);
    uint8_t *p_dst_temp = p_dst;

    bt_hfp_packet_info_t bt_hfp_packet_info;
    uint8_t i = 0;
    uint8_t j = 0;

    bt_hfp_packet_info = bt_hfp_get_packet_info_from_packet_type(bt_hw_ctrl);

    for (i = 0; i < bt_hfp_packet_info.packet_number_per_interrupt; i++) {
        for (j = 0; j < bt_hfp_packet_info.packet_length; j++){
            *p_dst_temp++ = *p_src++;
        }

        if (bt_hfp_packet_info.two_bytes_dummy_between_packet_flag) {
            p_src++;
            p_src++;
        }
    }

#if defined(MTK_BT_HFP_CODEC_DEBUG)
    if (bt_hfp_codec_debug_is_open && (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA)) {
        //uint32_t curr_cnt = 0;
        //uint32_t curr_cnt2 = 0;

        //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);

        //                                                  esco log
        //                                             hci_packet_header                                        |
        // HCI type    |  Connection handle |  Packet status flag | reserved  |  Data length | Data
        // Byte 0       |                                      Byte 1,2,3                                        | Byte 4~63
        // SCO= 0x03 |
        //     [0:7]     |           [8:19]        |          [20:21]       |  [22:23]   |     [24:31]    |60 bytes
        // HCI type = 0x03 ( SCO type )
        // Connection handle = 0x0ff , ( magic number)
        // Packet flag  : if valid = 2¡¦b00 , if invalid = 2¡¦b01
        uint32_t hci_packet_header = 0;
        const uint8_t hci_packet_header_length = 4;
        const uint8_t esco_log_length = hci_packet_header_length + bt_hfp_packet_info.packet_length;
        uint8_t esco_log[esco_log_length];

        hci_packet_header = 0x03 | (0x0ff << 8);    // HCI type and connection handle
        hci_packet_header |= bt_hfp_packet_info.packet_length << 24;    // data length


        p_dst_temp = p_dst;

        for (i = 0; i < bt_hfp_packet_info.packet_number_per_interrupt; i++) {
            hci_packet_header |= ((bt_hw_ctrl & (1 << i)) ==0) ? (0 << 20) : (1 << 20);     // packet status flag
            memcpy(esco_log, &hci_packet_header, hci_packet_header_length);
            memcpy(&esco_log[hci_packet_header_length], p_dst_temp, bt_hfp_packet_info.packet_length);

            p_dst_temp += bt_hfp_packet_info.packet_length;


            bt_hfp_codec_save_debug_data_to_debug_array_if_full_write_to_sdcard(&fdst_hfp_downlink_esco_raw_data, esco_log, esco_log_length);

            //bt_hci_log(0, esco_log, esco_log_length);

        }

        //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt2);
        //TASK_LOG_CTRL("SAVE_DOWNLINK_ESCO_RAW_DATA time cost= %d\r\n", (curr_cnt2-curr_cnt));

    }

#endif
    return;
}

static void bt_hfp_write_to_shared_memory (bt_hfp_shared_memory_information_t *p_mem_info, uint8_t *p_src, uint32_t bt_hw_ctrl)
{
    /* Copy from temp buffer to shared memory  */
    uint32_t offset = *(p_mem_info->p_offset_w);
    volatile uint8_t *p_dst = (volatile uint8_t *)(p_mem_info->base_address + offset);

    bt_hfp_packet_info_t bt_hfp_packet_info;
    uint8_t i = 0;
    uint8_t j = 0;

    bt_hfp_packet_info = bt_hfp_get_packet_info_from_packet_type(bt_hw_ctrl);

    for (i = 0; i < bt_hfp_packet_info.packet_number_per_interrupt; i++) {
        for (j = 0; j < bt_hfp_packet_info.packet_length; j++){
            *p_dst++ = *p_src++;
        }

        if (bt_hfp_packet_info.two_bytes_dummy_between_packet_flag) {
            p_dst++;
            p_dst++;
        }
    }

    return;
}

static void bt_hfp_fill_pattern_to_shared_memory (bt_hfp_shared_memory_information_t *p_mem_info, uint8_t pattern, uint32_t bt_hw_ctrl)
{
    /* Fill shared memory with pattern */
    uint32_t offset = *(p_mem_info->p_offset_w);
    volatile uint8_t *p_dst = (volatile uint8_t *)(p_mem_info->base_address + offset);

    bt_hfp_packet_info_t bt_hfp_packet_info;
    uint8_t i = 0;
    uint8_t j = 0;

    bt_hfp_packet_info = bt_hfp_get_packet_info_from_packet_type(bt_hw_ctrl);

    for (i = 0; i < bt_hfp_packet_info.packet_number_per_interrupt; i++) {
        for (j = 0; j < bt_hfp_packet_info.packet_length; j++){
            *p_dst++ = pattern;
        }

        if (bt_hfp_packet_info.two_bytes_dummy_between_packet_flag) {
            *p_dst++;
            *p_dst++;
        }
    }

    return;
}

static void bt_hfp_read_from_bt(bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_rx_information_t *p_rx_info, uint32_t *p_tmp_buf, uint32_t mask_array[BT_HFP_HW_SRAM_RX_PKT_CNT], uint32_t bt_hw_ctrl)
{
    bt_hfp_read_from_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
    {   /* Copy from temp buffer to decoder input buffer & Check if packet is valid */
        uint32_t blk_idx;
        uint8_t *p_src                                              = (uint8_t *)p_tmp_buf;
        uint32_t rx_pkt_w                                           = p_rx_info->pkt_w;
        uint8_t  (*rx_pkt_buf)[BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET] = p_rx_info->pkt_buf;
        uint32_t *rx_pkt_valid                                      = p_rx_info->pkt_valid;
        for (blk_idx = 0; blk_idx < BT_HFP_HW_SRAM_RX_PKT_CNT; blk_idx++) {
            uint32_t pkt_idx = rx_pkt_w & BT_HFP_RX_PACKET_MASK;
            {   /* Copy from temp buffer to decoder input buffer */
                uint8_t *p_dst = rx_pkt_buf[pkt_idx];
                memcpy(p_dst, p_src, BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET);
                p_src += BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET;
            }
            {   /* Check if packet is valid */
                uint32_t tab_val = mask_array[blk_idx];
                uint32_t reg_val = bt_hw_ctrl & tab_val;
                rx_pkt_valid[pkt_idx] = reg_val == tab_val ? BT_HFP_TRUE : BT_HFP_FALSE;
            }
            rx_pkt_w++;
        }
        p_rx_info->pkt_w = rx_pkt_w;
    }
    return;
}

static void bt_hfp_write_to_bt(bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_tx_information_t *p_tx_info, uint32_t *p_tmp_buf, uint32_t bt_hw_ctrl)
{
    {   /* Copy from encoder output buffer to temp buffer */
        uint32_t blk_idx;
        uint8_t *p_dst                                              = (uint8_t *)p_tmp_buf;
        uint32_t tx_pkt_r                                           = p_tx_info->pkt_r;
        uint8_t  (*tx_pkt_buf)[BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET] = p_tx_info->pkt_buf;
        for (blk_idx = 0; blk_idx < BT_HFP_HW_SRAM_TX_PKT_CNT; blk_idx++) {
            uint32_t pkt_idx = tx_pkt_r & BT_HFP_TX_PACKET_MASK;
            uint8_t *p_src   = tx_pkt_buf[pkt_idx];
            memcpy(p_dst, p_src, BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET);
            p_dst += BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET;
            tx_pkt_r++;
        }
        p_tx_info->pkt_r = tx_pkt_r;
    }

    bt_hfp_write_to_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
    return;
}

static void bt_codec_read_from_rx_buffer (bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_rx_information_t *p_rx_info, uint32_t *p_tmp_buf, uint32_t *mask_array, uint32_t bt_hw_ctrl)
{
    uint32_t w_pkt_cnt        = p_rx_info->pkt_w;
    uint32_t r_pkt_cnt        = p_rx_info->pkt_r;
    uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
    uint32_t rx_overflow;
    if (remained_pkt_cnt > 6) {
        rx_overflow = BT_HFP_TRUE;
    } else if (remained_pkt_cnt == 0) {
        rx_overflow = BT_HFP_FALSE;
    } else {
        rx_overflow = p_rx_info->pkt_overflow;
    }
    if (rx_overflow) {
        LISR_LOG_MSGID_W("BT Rx OF\r\n", 0);
    } else {
        bt_hfp_read_from_bt(p_mem_info, p_rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
    }
    p_rx_info->pkt_overflow = rx_overflow;
    return;
}

static void bt_codec_write_to_tx_buffer (bt_hfp_shared_memory_information_t *p_mem_info, bt_hfp_speech_tx_information_t *p_tx_info, uint32_t *p_tmp_buf, bt_hfp_audio_codec_type_t codec_type, uint32_t bt_hw_ctrl)
{
    uint32_t w_pkt_cnt          = p_tx_info->pkt_w;
    uint32_t r_pkt_cnt          = p_tx_info->pkt_r;
    uint32_t remained_pkt_cnt   = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
    uint32_t data_byte_cnt      = p_tx_info->pcm_w; /* PCM buffer, unit: byte */
    uint32_t threshold_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 40 : 80; /* 2.5 ms for CVSD 8kHz PCM buffer & mSBC 16kHz PCM buffer */
    uint32_t tx_underflow;
    if (remained_pkt_cnt < 3) {
        tx_underflow = BT_HFP_TRUE;
    } else if (remained_pkt_cnt >= 6 || (remained_pkt_cnt == 5 && data_byte_cnt >= threshold_byte_cnt)) {
        tx_underflow = BT_HFP_FALSE;    /* [6 x 7.5 ms] or [5 x 7.5 ms + 2.5 ms] */
    } else {
        tx_underflow = p_tx_info->pkt_underflow;
    }
    if (tx_underflow) {
        uint8_t pattern = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 0x55 : 0x00;
        bt_hfp_fill_pattern_to_shared_memory (p_mem_info, pattern, bt_hw_ctrl);
        LISR_LOG_MSGID_W("BT Tx UF\r\n", 0);
    } else {
        bt_hfp_write_to_bt(p_mem_info, p_tx_info, p_tmp_buf, bt_hw_ctrl);
    }
    p_tx_info->pkt_underflow = tx_underflow;
    return;
}
#endif

void bt_codec_event_send_from_isr (bt_codec_queue_event_id_t id, void *parameter)
{
    static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    bt_codec_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(bt_codec_queue_handle, &event, &xHigherPriorityTaskWoken) != pdPASS) {
        LISR_LOG_MSGID_W("queue not pass %d, queue stuck amount=%d\r\n",2, id, uxQueueMessagesWaitingFromISR(bt_codec_queue_handle));
        return;
    }
    if (xHigherPriorityTaskWoken != pdFALSE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    return;
}

void bt_codec_event_register_callback (bt_codec_queue_event_id_t reg_id, bt_codec_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
        if (bt_codec_queue_id_array[id_idx] == BT_CODEC_QUEUE_EVENT_NONE) {
            bt_codec_queue_id_array[id_idx] = reg_id;
            bt_codec_queue_handler[id_idx]  = callback;
            bt_codec_queue_reg_num++;
            break;
        }
    }
    return;
}

void bt_codec_event_deregister_callback (bt_codec_queue_event_id_t dereg_id)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
        if (bt_codec_queue_id_array[id_idx] == dereg_id) {
            bt_codec_queue_id_array[id_idx] = BT_CODEC_QUEUE_EVENT_NONE;
            bt_codec_queue_reg_num--;
            break;
        }
    }
    return;
}

#if !defined(MTK_AVM_DIRECT)
static void bt_hfp_tx_cvsd_encode(bt_hfp_speech_tx_information_t *p_tx_info)
{
    /* 8kHz speech Tx buffer --> 64kHz temp PCM buffer --> CVSD bitstream buffer */
    ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
    uint32_t data_8k_byte_cnt   = p_tx_info->pcm_w;
    uint32_t space_8k_byte_cnt  = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET - data_8k_byte_cnt;
    uint32_t b_pcm_empty        = BT_HFP_FALSE; /* 8kHz speech Tx buffer flag */
    uint32_t b_cvsd_full        = BT_HFP_FALSE; /* CVSD Tx bitstream buffer flag */
    TASK_LOG_MSGID_I("+cvsd_encode\r\n", 0);
    while (!b_cvsd_full && !b_pcm_empty) {
        uint32_t data_64k_byte_cnt  = 0;
        uint32_t space_64k_byte_cnt = BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
        {   /* Move 8kHz speech Tx ring buffer data to 8kHz temp PCM buffer */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (data_8k_byte_cnt < BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) {
                    uint8_t *p_read_buf    = NULL;
                    uint32_t read_byte_cnt = 0;
                    ring_buffer_get_read_information(p_tx_ring, &p_read_buf, &read_byte_cnt);
                    if (read_byte_cnt > 0) {
                        uint8_t *p_ou_base         = (uint8_t *)p_tx_info->pcm_8khz_buf;
                        int16_t *p_in_buf          = (int16_t *)p_read_buf;
                        int16_t *p_ou_buf          = (int16_t *)(p_ou_base + data_8k_byte_cnt);
                        uint32_t consumed_byte_cnt = MINIMUM(read_byte_cnt >> 1, space_8k_byte_cnt >> 1) << 1;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        data_8k_byte_cnt  += consumed_byte_cnt;
                        space_8k_byte_cnt -= consumed_byte_cnt;
                        ring_buffer_read_done(p_tx_ring, consumed_byte_cnt);
                    } else {    /* 8kHz speech Tx buffer is empty */
                        b_pcm_empty = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
        if (data_8k_byte_cnt >= BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) { /* 8kHz temp PCM buffer --> 64kHz temp PCM buffer */
            void *handle               = p_tx_info->src_up_sampling.handle;
            int16_t *p_in_buf          = p_tx_info->pcm_8khz_buf;
            int16_t *p_ou_buf          = p_tx_info->pcm_64khz_buf;
            int16_t *p_tmp_buf         = p_tx_info->pcm_tmp_buf;
            uint32_t in_byte_cnt       = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
            uint32_t ou_byte_cnt       = in_byte_cnt << 3;
            int32_t result;
            result = cvsd_src_up_sampling_process (handle, p_tmp_buf, p_in_buf, p_ou_buf, in_byte_cnt);
            if (result < 0) {   /* CVSD SRC up sampling error */
                TASK_LOG_MSGID_E("CVSD up %d\r\n",1, result);
                return;
            }
            data_64k_byte_cnt  += ou_byte_cnt;
            space_64k_byte_cnt -= ou_byte_cnt;
            data_8k_byte_cnt    = 0;
            space_8k_byte_cnt   = BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
        }
        if (!b_pcm_empty && data_64k_byte_cnt >= BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET) { /* 64kHz temp PCM buffer --> CVSD bitstream buffer */
            uint32_t w_pkt_cnt        = p_tx_info->pkt_w;
            uint32_t r_pkt_cnt        = p_tx_info->pkt_r;
            uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
            TASK_LOG_MSGID_I(" cvsd_encode, w_pkt_cnt %d, r_pkt_cnt %d\r\n",2, w_pkt_cnt, r_pkt_cnt);
            if (remained_pkt_cnt < BT_HFP_TX_PACKET_NUM) {  /* Encode */
                void    *handle      = p_tx_info->encoder.handle;
                int16_t *p_in_buf    = (int16_t *)p_tx_info->pcm_64khz_buf;
                uint8_t *p_ou_buf    = (uint8_t *)p_tx_info->pkt_buf[w_pkt_cnt & BT_HFP_TX_PACKET_MASK];
                uint32_t in_byte_cnt = BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET;
                uint32_t ou_byte_cnt = BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET;
                int32_t result;
                result = cvsd_encode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                if (result < 0) {   /* CVSD encode error */
                    TASK_LOG_MSGID_E("CVSD en %d\r\n",1, result);
                    return;
                }
                w_pkt_cnt++;
            } else {    /* CVSD Tx bitstream buffer is full */
                b_cvsd_full = BT_HFP_TRUE;
            }
            p_tx_info->pkt_w = w_pkt_cnt;
        }
    }
    p_tx_info->pcm_w = data_8k_byte_cnt;
    TASK_LOG_MSGID_I("-cvsd_encode\r\n", 0);
    return;
}

static void bt_hfp_rx_cvsd_decode(bt_hfp_speech_rx_information_t *p_rx_info, uint32_t control_reg, uint32_t isr_time)
{
    /* CVSD bitstream buffer --> 64kHz temp PCM buffer --> 8kHz temp PCM buffer --> 8kHz speech Rx buffer */
    ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
    uint32_t space_byte_cnt = p_rx_info->pcm_r;                                                 /* 8kHz temp PCM buffer space byte count */
    uint32_t data_byte_cnt  = BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET - space_byte_cnt;  /* 8kHz temp PCM buffer data byte count */
    uint32_t b_cvsd_empty   = BT_HFP_FALSE;
    uint32_t b_pcm_full     = BT_HFP_FALSE;
    uint32_t num_bad        = 0;
    uint32_t num_good       = 0;
    uint32_t num_headfail   = 0;
    TASK_LOG_MSGID_I("+cvsd_decode\r\n", 0);
    while (!b_cvsd_empty && !b_pcm_full) {
        if (space_byte_cnt >= BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) { /* 8kHz temp PCM buffer is empty, decode new CVSD bitstream */
            uint32_t w_pkt_cnt = p_rx_info->pkt_w;
            uint32_t r_pkt_cnt = p_rx_info->pkt_r;
            TASK_LOG_MSGID_I(" cvsd_decode, w_pkt_cnt %d, r_pkt_cnt %d\r\n",2, w_pkt_cnt, r_pkt_cnt);
            if (w_pkt_cnt != r_pkt_cnt) {   /* Decode 30-byte CVSD bitstream --> 480-byte 64kHz PCM --> 60-byte 8kHz PCM */
                uint32_t valid_flag = p_rx_info->pkt_valid[r_pkt_cnt & BT_HFP_RX_PACKET_MASK];
                plc_runtime_parameter_t param;
                if (valid_flag) {   /* Packet Valid */
                    {   /* Decode 3.75 ms (CVSD bitstream buffer --> 64kHz temp PCM buffer) */
                        void    *handle      = p_rx_info->decoder.handle;
                        uint8_t *p_in_buf    = p_rx_info->pkt_buf[r_pkt_cnt & BT_HFP_RX_PACKET_MASK];
                        int16_t *p_ou_buf    = p_rx_info->pcm_64khz_buf;
                        uint32_t in_byte_cnt = BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET; /* 30 bytes */
                        uint32_t ou_byte_cnt = BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET; /* 240 samples @ 64kHz (= 3.75 ms) */
                        int32_t  result;
                        result = cvsd_decode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                        if (result < 0) {   /* CVSD decode error */
                            TASK_LOG_MSGID_E("CVSD de %d\r\n",1, result);
                            return;
                        }
                    }
                    {   /* Downsample 3.75 ms (64kHz temp PCM buffer --> 8kHz temp PCM buffer) */
                        int16_t *p_in_buf    = p_rx_info->pcm_64khz_buf;
                        int16_t *p_ou_buf    = p_rx_info->pcm_8khz_buf;
                        int16_t *p_tmp_buf   = p_rx_info->pcm_tmp_buf;
                        void    *handle      = p_rx_info->src_down_sampling.handle;
                        uint32_t in_byte_cnt = BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET; /* 240 samples @ 64kHz (= 3.75 ms) */
                        int32_t result;
                        result = cvsd_src_down_sampling_process (handle, p_tmp_buf, p_in_buf, p_ou_buf, in_byte_cnt);
                        if (result < 0) {   /* CVSD SRC down sampling error */
                            TASK_LOG_MSGID_E("CVSD dw %d\r\n",1, result);
                            return;
                        }
                    }
                    param.bad_frame_indicator  = 0;
                    param.bt_ev3_half_bad_flag = 0;
                    num_good++;
                } else {            /* Packet Invalid */
                    int16_t *p_ou_buf = p_rx_info->pcm_8khz_buf;
                    memset(p_ou_buf, 0, BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET);
                    param.bad_frame_indicator  = 1;
                    param.bt_ev3_half_bad_flag = 0;
                    p_rx_info->pkt_lost_cnt++;
                    num_bad++;
                    TASK_LOG_MSGID_CTRL("cvsd lost %d\r\n",1, p_rx_info->pkt_lost_cnt);
                }
                {   /* Apply PLC */
                    int16_t *p_ou_buf    = p_rx_info->pcm_8khz_buf;
                    void    *handle      = p_rx_info->plc.handle;
                    int32_t result;
                    result = plc_process (handle, p_ou_buf, &param);
                    if (result < 0) {   /* CVSD PLC error */
                        TASK_LOG_MSGID_CTRL("CVSD plc fail: %d\r\n",1, result);
                    }
                }
                r_pkt_cnt++;
                space_byte_cnt = 0;
                data_byte_cnt = BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET;
            } else {    /* CVSD Rx bitsream buffer is empty */
                b_cvsd_empty = BT_HFP_TRUE;
            }
            p_rx_info->pkt_r = r_pkt_cnt;
        }
        if (!b_cvsd_empty) {    /* Move at most twice for ring buffer usage (8kHz temp PCM buffer --> 8kHz speech Rx buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (space_byte_cnt < BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET) {
                    uint8_t *p_ou_buf    = NULL;
                    uint32_t ou_byte_cnt = 0;
                    ring_buffer_get_write_information(p_rx_ring, &p_ou_buf, &ou_byte_cnt);
                    if (ou_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(data_byte_cnt, ou_byte_cnt);
                        uint8_t *p_in_base         = (uint8_t *)p_rx_info->pcm_8khz_buf;
                        uint8_t *p_in_buf          = p_in_base + space_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        space_byte_cnt += consumed_byte_cnt;
                        data_byte_cnt  -= consumed_byte_cnt;
                        ring_buffer_write_done (p_rx_ring, consumed_byte_cnt);
                    } else {    /* 8kHz speech Rx buffer is full */
                        b_pcm_full = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
    }
    {
        uint32_t acc_bad      = p_rx_info->acc_bad;
        uint32_t acc_good     = p_rx_info->acc_good;
        uint32_t acc_headfail = p_rx_info->acc_headfail;
        acc_bad      += num_bad;
        acc_good     += num_good;
        acc_headfail += num_headfail;
        p_rx_info->acc_bad      = acc_bad;
        p_rx_info->acc_good     = acc_good;
        p_rx_info->acc_headfail = acc_headfail;
        if (num_bad > 0) {
            TASK_LOG_MSGID_W("CVSD %d %d %d %d %d %x %d %d\r\n",8, isr_time, acc_bad, acc_good, num_bad, num_good, control_reg, acc_headfail, num_headfail);
        }
    }
    p_rx_info->pcm_r = space_byte_cnt;
    TASK_LOG_MSGID_I("-cvsd_decode\r\n", 0);
    return;
}

static void bt_hfp_tx_msbc_encode(bt_hfp_speech_tx_information_t *p_tx_info)
{
    /* 16kHz speech Tx buffer --> mSBC bitstream buffer */
    ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
    uint32_t data_byte_cnt  = p_tx_info->pcm_w;
    uint32_t space_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME - data_byte_cnt;
    uint32_t b_pcm_empty    = BT_HFP_FALSE; /* 16kHz speech Tx buffer flag */
    uint32_t b_msbc_full    = BT_HFP_FALSE; /* mSBC Tx bitstream buffer flag */
    TASK_LOG_MSGID_I("+msbc_encode\r\n", 0);
    while (!b_msbc_full && !b_pcm_empty) {
        {   /* Move at most twice for ring buffer usage (16kHz speech Tx buffer --> 16kHz temp PCM buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (data_byte_cnt < BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {
                    uint8_t *p_in_buf    = NULL;
                    uint32_t in_byte_cnt = 0;
                    ring_buffer_get_read_information(p_tx_ring, &p_in_buf, &in_byte_cnt);
                    if (in_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(space_byte_cnt, in_byte_cnt);
                        uint8_t *p_ou_base         = (uint8_t *)p_tx_info->pcm_16khz_buf;
                        uint8_t *p_ou_buf          = p_ou_base + data_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        data_byte_cnt  += consumed_byte_cnt;
                        space_byte_cnt -= consumed_byte_cnt;
                        ring_buffer_read_done (p_tx_ring, consumed_byte_cnt);
                    } else {    /* 16kHz speech Tx buffer is empty */
                        b_pcm_empty = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
        if (!b_pcm_empty && data_byte_cnt >= BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {  /* 16kHz temp PCM buffer --> mSBC bitstream buffer */
            uint32_t w_pkt_cnt        = p_tx_info->pkt_w;
            uint32_t r_pkt_cnt        = p_tx_info->pkt_r;
            uint32_t remained_pkt_cnt = REMAINED_PKT_CNT(w_pkt_cnt, r_pkt_cnt);
            if (remained_pkt_cnt < BT_HFP_TX_PACKET_NUM) {  /* Encode */
                void    *handle      = p_tx_info->encoder.handle;
                uint32_t w_pkt_idx   = w_pkt_cnt & BT_HFP_TX_PACKET_MASK;
                int16_t *p_in_buf    = (int16_t *)p_tx_info->pcm_16khz_buf;
                uint8_t *p_ou_buf    = (uint8_t *)&p_tx_info->pkt_buf[w_pkt_idx][2];
                uint32_t in_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
                uint32_t ou_byte_cnt = BT_HFP_TX_MSBC_BITSTREAM_SIZE_PER_FRAME;
                int32_t result;
                result = msbc_encode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                if (result < 0) {   /* mSBC encode error */
                    TASK_LOG_MSGID_E("mSBC en %d\r\n",1, result);
                    return;
                }
                p_tx_info->pkt_buf[w_pkt_idx][ 0] = 0x01;
                p_tx_info->pkt_buf[w_pkt_idx][ 1] = bt_hfp_msbc_header[w_pkt_idx & 0x3];
                p_tx_info->pkt_buf[w_pkt_idx][59] = 0x00;
                data_byte_cnt  = 0;
                space_byte_cnt = BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
                w_pkt_cnt++;
            } else {    /* mSBC Tx bitstream buffer is full */
                b_msbc_full = BT_HFP_TRUE;
            }
            p_tx_info->pkt_w = w_pkt_cnt;
        }
    }
    p_tx_info->pcm_w = data_byte_cnt;
    TASK_LOG_MSGID_I("-msbc_encode\r\n", 0);
    return;
}

static void bt_hfp_rx_msbc_decode(bt_hfp_speech_rx_information_t *p_rx_info, uint32_t control_reg, uint32_t isr_time)
{
    /* mSBC bitstream buffer --> 16kHz temp PCM buffer --> 16kHz speech Rx buffer */
    ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
    uint32_t space_byte_cnt = p_rx_info->pcm_r;                                                 /* 16kHz temp PCM buffer space byte count */
    uint32_t data_byte_cnt  = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME - space_byte_cnt;  /* 16kHz temp PCM buffer data byte count */
    uint32_t b_msbc_empty   = BT_HFP_FALSE;
    uint32_t b_pcm_full     = BT_HFP_FALSE;
    uint32_t num_bad        = 0;
    uint32_t num_good       = 0;
    uint32_t num_headfail   = 0;
    TASK_LOG_MSGID_I("+msbc_decode\r\n", 0);
    while (!b_msbc_empty && !b_pcm_full) {
        if (space_byte_cnt >= BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) { /* 16kHz temp PCM buffer is empty, decode new CVSD bitstream */
            uint32_t w_pkt_cnt = p_rx_info->pkt_w;
            uint32_t r_pkt_cnt = p_rx_info->pkt_r;
            if (w_pkt_cnt >= r_pkt_cnt + 2) {   /* Decode 57-byte mSBC bitstream --> 240-byte 16kHz PCM */
                uint32_t pkt_1_idx    = (r_pkt_cnt + 0) & BT_HFP_RX_PACKET_MASK;
                uint32_t pkt_2_idx    = (r_pkt_cnt + 1) & BT_HFP_RX_PACKET_MASK;
                uint8_t *p_in_buf_1   = p_rx_info->pkt_buf[pkt_1_idx];
                uint8_t *p_in_buf_2   = p_rx_info->pkt_buf[pkt_2_idx];
                uint32_t valid_flag_1 = p_rx_info->pkt_valid[pkt_1_idx];
                uint32_t valid_flag_2 = p_rx_info->pkt_valid[pkt_2_idx];
                uint32_t header_check = p_in_buf_1[0] == 0x01 && (p_in_buf_1[1] & 0x0F) == 0x08 ? BT_HFP_TRUE : BT_HFP_FALSE;
                plc_runtime_parameter_t param;
                if (header_check && valid_flag_1 && valid_flag_2) { /* Good Frame or 2nd Half Bad */
                    {   /* Collect 57-byte mSBC bitstream from two 30-byte Rx packets */
                        uint8_t *p_ou_buf   = p_rx_info->pkt_tmp_buf;
                        memcpy(p_ou_buf +  0, p_in_buf_1 + 2, 28);
                        memcpy(p_ou_buf + 28, p_in_buf_2 + 0, 29); /* total 57 bytes */
                    }
                    {   /* Decode 7.5 ms (mSBC bitstream buffer --> 64kHz temp PCM buffer) */
                        void    *handle      = p_rx_info->decoder.handle;
                        uint8_t *p_in_buf    = p_rx_info->pkt_tmp_buf;
                        int16_t *p_ou_buf    = p_rx_info->pcm_16khz_buf;
                        uint32_t in_byte_cnt = BT_HFP_RX_MSBC_BITSTREAM_SIZE_PER_FRAME;         /* 57 bytes */
                        uint32_t ou_byte_cnt = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;  /* 120 samples @ 16kHz (= 7.5 ms) */
                        int32_t  result      = msbc_decode_process (handle, p_in_buf, &in_byte_cnt, p_ou_buf, &ou_byte_cnt);
                        if (result < 0) {   /* mSBC decode error */
                            memset(p_ou_buf, 0, BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
                            TASK_LOG_MSGID_E("mSBC de %d\r\n",1, result);
                        }
                    }
                    param.bad_frame_indicator  = 0;
                    param.bt_ev3_half_bad_flag = 0;
                    num_good++;
                } else {    /* Bad Frame */
                    int16_t *p_ou_buf = p_rx_info->pcm_16khz_buf;
                    memset(p_ou_buf, 0, BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME);
                    param.bad_frame_indicator  = 1;
                    param.bt_ev3_half_bad_flag = 0;
                    p_rx_info->pkt_lost_cnt++;
                    if (valid_flag_1 && valid_flag_2) {
                        num_headfail++;
                    }
                    num_bad++;
                    TASK_LOG_MSGID_CTRL("mSBC lost %d\r\n",1, p_rx_info->pkt_lost_cnt);
                }
                {   /* Apply PLC */
                    int16_t *p_ou_buf    = p_rx_info->pcm_16khz_buf;
                    void    *handle      = p_rx_info->plc.handle;
                    int32_t result;
                    result = plc_process (handle, p_ou_buf, &param);
                    if (result < 0) {   /* mSBC PLC error */
                        TASK_LOG_MSGID_CTRL("mSBC plc fail:%d\r\n",1, result);
                    }
                }
                r_pkt_cnt += 2;
                space_byte_cnt = 0;
                data_byte_cnt = BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME;
            } else {    /* mSBC Rx bitsream buffer is empty */
                b_msbc_empty = BT_HFP_TRUE;
            }
            p_rx_info->pkt_r = r_pkt_cnt;
        }
        if (!b_msbc_empty) {    /* Move at most twice for ring buffer usage (16kHz temp PCM buffer --> 16kHz speech Rx buffer) */
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                if (space_byte_cnt < BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME) {
                    uint8_t *p_ou_buf    = NULL;
                    uint32_t ou_byte_cnt = 0;
                    ring_buffer_get_write_information(p_rx_ring, &p_ou_buf, &ou_byte_cnt);
                    if (ou_byte_cnt > 0) {
                        uint32_t consumed_byte_cnt = MINIMUM(data_byte_cnt, ou_byte_cnt);
                        uint8_t *p_in_base         = (uint8_t *)p_rx_info->pcm_16khz_buf;
                        uint8_t *p_in_buf          = p_in_base + space_byte_cnt;
                        memcpy(p_ou_buf, p_in_buf, consumed_byte_cnt);
                        space_byte_cnt += consumed_byte_cnt;
                        data_byte_cnt  -= consumed_byte_cnt;
                        ring_buffer_write_done (p_rx_ring, consumed_byte_cnt);
                    } else {    /* 16kHz speech Rx buffer is full */
                        b_pcm_full = BT_HFP_TRUE;
                        break;
                    }
                }
            }
        }
    }
    {
        uint32_t acc_bad      = p_rx_info->acc_bad;
        uint32_t acc_good     = p_rx_info->acc_good;
        uint32_t acc_headfail = p_rx_info->acc_headfail;
        acc_bad      += num_bad;
        acc_good     += num_good;
        acc_headfail += num_headfail;
        p_rx_info->acc_bad      = acc_bad;
        p_rx_info->acc_good     = acc_good;
        p_rx_info->acc_headfail = acc_headfail;
        if (num_bad > 0) {
            TASK_LOG_MSGID_W("mSBC %d %d %d %d %d %x %d %d\r\n",8, isr_time, acc_bad, acc_good, num_bad, num_good, control_reg, acc_headfail, num_headfail);
        }
    }
    p_rx_info->pcm_r = space_byte_cnt;
    TASK_LOG_MSGID_I("-msbc_decode\r\n", 0);
    return;
}

static void bt_hfp_codec_loopback_process(ring_buffer_information_t *p_tx_ring, ring_buffer_information_t *p_rx_ring)
{
    uint32_t loop_idx;
    TASK_LOG_MSGID_I("+codec_loopback\r\n", 0);
    for (loop_idx = 0; loop_idx < 4; loop_idx++) {
        uint8_t *p_in_buf = NULL;
        uint8_t *p_ou_buf = NULL;
        uint32_t byte_cnt_in = 0;
        uint32_t byte_cnt_ou = 0;
        uint32_t byte_cnt_moved;
        ring_buffer_get_read_information  (p_rx_ring, &p_in_buf, &byte_cnt_in);
        ring_buffer_get_write_information (p_tx_ring, &p_ou_buf, &byte_cnt_ou);
        byte_cnt_moved = MINIMUM(byte_cnt_in, byte_cnt_ou);
        memcpy(p_ou_buf, p_in_buf, byte_cnt_moved);
        ring_buffer_write_done (p_tx_ring, byte_cnt_moved);
        ring_buffer_read_done  (p_rx_ring, byte_cnt_moved);
    }
    TASK_LOG_MSGID_I("-codec_loopback\r\n", 0);
    return;
}

static void bt_hfp_codec_tx_process_handler(void *parameter)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    TASK_LOG_MSGID_I("+codec_tx\r\n", 0);
    {   /* BT Encode */
        bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
            bt_hfp_tx_cvsd_encode(p_tx_info);
        } else {
            bt_hfp_tx_msbc_encode(p_tx_info);
        }
    }
    TASK_LOG_MSGID_I("-codec_tx\r\n", 0);
    return;
}

static void bt_hfp_codec_rx_process_handler(void *parameter)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    TASK_LOG_MSGID_I("+codec_rx\r\n", 0);
    {   /* BT Decode */
        bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_mode_t mode = p_info->mode;
        uint32_t control_reg = p_info->control_reg;
        uint32_t isr_time    = p_info->isr_time;
        if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
            bt_hfp_rx_cvsd_decode(p_rx_info, control_reg, isr_time);
        } else {
            bt_hfp_rx_msbc_decode(p_rx_info, control_reg, isr_time);
        }
        if (mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
            bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
            bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
            ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
            ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
            bt_hfp_codec_loopback_process(p_tx_ring, p_rx_ring);
            bt_hfp_codec_tx_process_handler(NULL);
        }
    }
    TASK_LOG_MSGID_I("-codec_rx\r\n", 0);
    return;
}

bt_codec_media_status_t bt_codec_hfp_set_customer_enhanced_stream_in_data(void *buffer, uint32_t sample_count)
{
    uint32_t write_byte_count = 0;
    uint8_t *p_source = (uint8_t *)buffer;


    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        write_byte_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        write_byte_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_set_customer_enhanced_stream_in_data: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    write_byte_count = (sample_count < write_byte_count) ? sample_count : write_byte_count;

    memcpy(bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data, p_source, write_byte_count);

    bt_hfp_codec_internal_handle->customer_enhancement_callback.waiting = false;

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_get_customer_enhanced_stream_in_sample_count(uint32_t *sample_count)
{
    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        *sample_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        *sample_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_get_customer_enhanced_stream_in_sample_count: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}
static void bt_hfp_dsp_tx_process_handler(void *parameter)
{
    if (bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function != NULL) {
        if (!bt_hfp_codec_internal_handle->customer_enhancement_callback.waiting){
            bt_hfp_codec_internal_handle->customer_enhancement_callback.waiting = true;
            bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function(bt_hfp_codec_internal_handle->customer_enhancement_callback.data);
        }

        if (bt_hfp_codec_internal_handle->customer_enhancement_callback.waiting){
            TASK_LOG_MSGID_CTRL("bt_hfp_dsp_tx_process_handler, warring: please set stream in data first\r\n", 0);
        }

        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_speech_tx_information_t *p_tx_info    = &p_info->tx_info;
        ring_buffer_information_t      *p_tx_ring    = &p_tx_info->pcm_ring_info;
        bt_hfp_audio_codec_type_t       codec_type   = p_info->codec_type;
        uint32_t                        frm_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;

        uint8_t *p_tmp_buf = (uint8_t *)bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data;
        uint8_t *p_pcm_buf = NULL;
        uint32_t write_byte_cnt = 0;
        uint32_t loop_idx;

        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            ring_buffer_get_write_information(p_tx_ring, &p_pcm_buf, &write_byte_cnt);
            write_byte_cnt = MINIMUM(write_byte_cnt, frm_byte_cnt);
            memcpy(p_pcm_buf, p_tmp_buf, write_byte_cnt);
            p_tmp_buf += write_byte_cnt;
            frm_byte_cnt -= write_byte_cnt;
            ring_buffer_write_done(p_tx_ring, write_byte_cnt);
        }
    }
    bt_hfp_codec_tx_process_handler(parameter);
    return;
}

static void bt_hfp_dsp_rx_process_handler(void *parameter)
{
    return;
}
#endif

#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
static void initialize_interrupt_interval (void)
{
    bt_hfp_rx_cnt = 0;
    p2w_tx_cnt = 0;
    p2w_rx_cnt = 0;
    return;
}

static void print_interrupt_interval (uint32_t *p_counter, const char *p_description)
{
    /* Check interrupt interval */
    uint32_t prev_cnt = *p_counter;
    uint32_t curr_cnt = 0;
    hal_gpt_status_t result = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
    if (HAL_GPT_STATUS_OK != result) {
        LOG_I("EXCEPTION, [%s] gpt result = %d\r\n", p_description, result);
    } else if (prev_cnt == 0) {
        LOG_I("[%s]first time\r\n", p_description);
    } else {
        uint32_t interval = curr_cnt >= prev_cnt ? curr_cnt - prev_cnt : 0xffffffff - prev_cnt + curr_cnt;
        LOG_I("[%s]interval = %d\r\n", p_description, interval);
    }
    *p_counter = curr_cnt;
    return;
}
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */

static uint32_t bt_hfp_check_state (bt_codec_state_t ref_state)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    uint32_t result = BT_HFP_FALSE;
    if (p_info != NULL) {
        bt_media_handle_t *handle = &p_info->handle;
        bt_codec_state_t cur_state = handle->state;
        if (cur_state == ref_state) {
            result = BT_HFP_TRUE;
        }
    }
    return result;
}

#if !defined(MTK_AVM_DIRECT)
static void bt_hfp_bt_interrupt_callback_internal (bt_hfp_codec_internal_handle_t *p_info)
{
    {   /* Check & Exchange */
        bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
        volatile uint32_t *p_control = p_mem_info->p_control;
        uint32_t bt_hw_ctrl  = *p_control;
        uint32_t pkt_type    = (bt_hw_ctrl >> 18) & 0x7;
        uint32_t type_idx    = bt_hfp_get_packet_type_index(pkt_type);
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        {   /* Check information */
            if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
                LISR_LOG_MSGID_W("BT ISR not PLAY, %u\r\n",1, bt_codec_get_interrupt_time(true));
                return;
            }
            if ((bt_hw_ctrl & BT_HFP_HW_INTERRUPT) == 0) {
                LISR_LOG_MSGID_I("bt interrupt re-entry\r\n", 0);
                return;
            }
            if (type_idx >= BT_HFP_PKT_TYPE_IDX_MAX) {
                LISR_LOG_MSGID_E("EXCEPTION, type_idx %d\r\n",1, type_idx);
                return;
            }
            if (codec_type != BT_HFP_CODEC_TYPE_CVSD && codec_type != BT_HFP_CODEC_TYPE_MSBC) {
                LISR_LOG_MSGID_E("EXCEPTION, codec_type %d\r\n",1, codec_type);
                return;
            }
        }
        LISR_LOG_MSGID_I("+bt interrupt\r\n", 0);
        p_info->isr_time = bt_codec_get_interrupt_time(true);
        p_info->control_reg = bt_hw_ctrl;
        {   /* Exchange bitstream with BT shared buffer */
            uint32_t *mask_array = (uint32_t *)&bt_hfp_rx_packet_valid_mask_table[type_idx];
            uint32_t *p_tmp_buf  = p_info->pkt_tmp_buf;
            bt_hfp_mode_t mode = p_info->mode;
            if (mode == BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC) {
                LISR_LOG_MSGID_I("+loopback_without_codec\r\n", 0);
                bt_hfp_read_from_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
                bt_hfp_write_to_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
                LISR_LOG_MSGID_I("-loopback_without_codec\r\n", 0);
                return;
            }
            bt_codec_read_from_rx_buffer (p_mem_info, &p_info->rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
            bt_codec_write_to_tx_buffer  (p_mem_info, &p_info->tx_info, p_tmp_buf, codec_type, bt_hw_ctrl);
        }
        {
            bt_hfp_mode_t mode = p_info->mode;
            if (mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
                LISR_LOG_MSGID_I("+loopback_with_codec\r\n", 0);
                bt_hfp_codec_rx_process_handler(NULL);
                LISR_LOG_MSGID_I("-loopback_with_codec\r\n", 0);
                return;
            } else {
                bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS, NULL);
            }
        }
        LISR_LOG_MSGID_I("-bt interrupt\r\n", 0);
    }
    return;
}

static void bt_hfp_bt_interrupt_callback (void *user_data)
{
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&bt_hfp_rx_cnt, "BT Interrupt");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    {
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_bt_interrupt_callback_internal(p_info);
        *BT_HFP_HW_REG_CONTROL &= ~BT_HFP_HW_CTRL_MASK;
        p_info->eint_status = hal_eint_unmask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            LISR_LOG_MSGID_E(" play, EXCEPTION, unmask eint status %d\r\n",1, p_info->eint_status);
        }
    }
    return;
}
bt_codec_media_status_t bt_codec_hfp_read_stream_in(void *buffer, uint32_t sample_count)
{
    uint32_t read_byte_count = 0;
    uint8_t *p_dst = (uint8_t *)buffer;


    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        read_byte_count = (afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) ? CVSD_TONE_BUFFER_SIZE_IN_BYTES * 2 : CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        read_byte_count = (afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) ? MSBC_TONE_BUFFER_SIZE_IN_BYTES * 2 : MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_read_stream_in: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    read_byte_count = (sample_count < read_byte_count) ? sample_count : read_byte_count;

    memcpy(p_dst, bt_hfp_codec_internal_handle->stream_in_data, read_byte_count);

    bt_hfp_codec_internal_handle->get_stream_in_callback.waiting = false;

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_get_stream_in_sample_count(uint32_t *sample_count)
{
    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        *sample_count = (afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) ? CVSD_TONE_BUFFER_SIZE_IN_BYTES * 2 : CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        *sample_count = (afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) ? MSBC_TONE_BUFFER_SIZE_IN_BYTES * 2 : MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_get_stream_in_sample_count: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}

static void bt_hfp_codec_pcm2way_stream_in_callback (void)
{
    uint32_t isr_time = bt_codec_get_interrupt_time(true);
    isr_time = isr_time;    // just for IAR build pass
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&p2w_tx_cnt, "P2W Tx");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
        LISR_LOG_MSGID_W("P2W Tx not PLAY, %d\r\n",1, isr_time);
    } else {
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_speech_tx_information_t *p_tx_info    = &p_info->tx_info;
        ring_buffer_information_t      *p_tx_ring    = &p_tx_info->pcm_ring_info;
        bt_hfp_audio_codec_type_t              codec_type   = p_info->codec_type;
        uint32_t                        frm_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;
        uint16_t                       *p_tmp_base   = p_tx_info->pcm_tx_tmp_buf;
        LISR_LOG_MSGID_I("+dsp_tx\r\n", 0);
        {   /* Check Tx PCM ring buffer space byte count */
            uint32_t space_byte_cnt = ring_buffer_get_space_byte_count(p_tx_ring);
            if (space_byte_cnt >= frm_byte_cnt) {
                p_tx_info->pcm_overflow = BT_HFP_FALSE;
            } else {    /* Tx PCM ring buffer overflow */
                LISR_LOG_MSGID_W("DSP Tx OF, %d\r\n",1, isr_time);
                p_tx_info->pcm_overflow = BT_HFP_TRUE;
                bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, NULL);
                return;
            }
        }
        {   /* Get PCM data from stream in & write to Tx PCM ring buffer */
            uint8_t *p_tmp_buf = (uint8_t *)p_tmp_base;
            uint8_t *p_pcm_buf = NULL;
            uint32_t write_byte_cnt = 0;
            uint32_t loop_idx;
            audio_pcm2way_get_from_stream_in(p_tmp_base);
#if defined LBK_UL_TO_HP_ENABLED
            memcpy (audio_pcm2way_hp_buffer, p_tmp_base, 320 * 2);
#endif
            {   /* If register get stream in callback */
                if (p_info->get_stream_in_callback.callback_function){
                    if (!p_info->get_stream_in_callback.waiting) {
                        p_info->get_stream_in_callback.waiting = true;
                        if(afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
                            uint16_t *p_second_mic_temp_buf = (uint16_t *)(bt_hfp_codec_internal_handle->stream_in_data+frm_byte_cnt);
                            audio_pcm2way_get_from_stream_in_second_mic(p_second_mic_temp_buf);
                            uint32_t k = 0;
                            uint32_t l = 0;
                            for(k = 0; k < frm_byte_cnt*2/4; k++){
                                *(bt_hfp_codec_internal_handle->stream_in_data+k*4+0) = *(p_tmp_buf+l);
                                *(bt_hfp_codec_internal_handle->stream_in_data+k*4+1) = *(p_tmp_buf+l+1);
                                *(bt_hfp_codec_internal_handle->stream_in_data+k*4+2) = *(bt_hfp_codec_internal_handle->stream_in_data+frm_byte_cnt+l);
                                *(bt_hfp_codec_internal_handle->stream_in_data+k*4+3) = *(bt_hfp_codec_internal_handle->stream_in_data+frm_byte_cnt+l+1);
                                l += 2;
                            }
                        } else {
                            memcpy(bt_hfp_codec_internal_handle->stream_in_data, p_tmp_buf, frm_byte_cnt);
                        }

                        p_info->get_stream_in_callback.callback_function(BT_CODEC_MEDIA_DATA_NOTIFICATION, p_info->get_stream_in_callback.data);
                    }
                }
            }

            if (bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function == NULL) {
                for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                    ring_buffer_get_write_information(p_tx_ring, &p_pcm_buf, &write_byte_cnt);
                    write_byte_cnt = MINIMUM(write_byte_cnt, frm_byte_cnt);
                    memcpy(p_pcm_buf, p_tmp_buf, write_byte_cnt);
                    p_tmp_buf += write_byte_cnt;
                    frm_byte_cnt -= write_byte_cnt;
                    ring_buffer_write_done(p_tx_ring, write_byte_cnt);
                }
            }
        }
        LISR_LOG_MSGID_I("-dsp_tx\r\n", 0);
        bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, NULL);
    }
    return;
}

static void bt_hfp_codec_mix_stream_out_method_weighted_mean(uint16_t *origin_stream_out_buffer, uint16_t *mix_data_buffer, uint32_t buffer_size_in_word)
{
    uint32_t i = 0;
    int16_t temp_int16 = 0;
    int16_t temp_int16_2 = 0;
    int16_t temp_tone = 0;
    for(i = 0; i < buffer_size_in_word; i++){
        temp_int16 = (int16_t)*(origin_stream_out_buffer+i);
        temp_int16_2 = (int16_t)*(mix_data_buffer+i);
        temp_tone = (int16_t)(((int32_t)temp_int16 + (int32_t)temp_int16_2)/2);

        *(origin_stream_out_buffer+i) = (uint16_t)temp_tone;
    }
}

static void bt_hfp_codec_mix_stream_out(bt_codec_media_hfp_mix_stream_out_method_t mix_method, uint16_t *origin_stream_out_buffer, uint16_t *mix_data_buffer, uint32_t buffer_size_in_word)
{
    uint8_t *p_dst = (uint8_t *)origin_stream_out_buffer;
    uint8_t *p_source = (uint8_t *)mix_data_buffer;

    switch(mix_method) {
        case BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_NO_MIX:
            // do nothing
            break;

        case BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_REPLACE:
            memcpy(p_dst, p_source, buffer_size_in_word*2);
            break;

        case BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_WEIGHTED_MEAN:
            bt_hfp_codec_mix_stream_out_method_weighted_mean(origin_stream_out_buffer, mix_data_buffer, buffer_size_in_word);
            break;

        default:
            // do nothing
            break;
    }
}

bt_codec_media_status_t bt_codec_hfp_read_stream_out(void *buffer, uint32_t sample_count)
{
    uint32_t read_byte_count = 0;
    uint8_t *p_dst = (uint8_t *)buffer;


    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        read_byte_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        read_byte_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_read_stream_out: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    read_byte_count = (sample_count < read_byte_count) ? sample_count : read_byte_count;

    memcpy(p_dst, bt_hfp_codec_internal_handle->stream_out_data, read_byte_count);

    bt_hfp_codec_internal_handle->get_stream_out_callback.waiting = false;

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_get_stream_out_sample_count(uint32_t *sample_count)
{
    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        *sample_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        *sample_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_get_stream_out_sample_count: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}

#if (defined MTK_EXTERNAL_DSP_ENABLE) && (defined MTK_NDVC_ENABLE)
extern bool ndvc_at_test;
#endif

#if defined LBK_UL_TO_HP_ENABLED
static void bt_hfp_codec_pcm2way_stream_out_callback (void)
{
    audio_pcm2way_put_to_stream_out(audio_pcm2way_hp_buffer);
}
#else

static void bt_hfp_codec_pcm2way_stream_out_callback (void)
{
    uint32_t isr_time = bt_codec_get_interrupt_time(true);
    isr_time = isr_time;    // just for IAR build pass
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    print_interrupt_interval (&p2w_rx_cnt, "P2W Rx");
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
        LISR_LOG_MSGID_W("P2W Rx not PLAY, %d\r\n",1, isr_time);
    } else {
        bt_hfp_codec_internal_handle_t *p_info       = bt_hfp_codec_internal_handle;
        bt_hfp_speech_rx_information_t *p_rx_info    = &p_info->rx_info;
        ring_buffer_information_t      *p_rx_ring    = &p_rx_info->pcm_ring_info;
        bt_hfp_audio_codec_type_t              codec_type   = p_info->codec_type;
        uint32_t                        frm_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;
        uint16_t                       *p_tmp_base   = p_rx_info->pcm_rx_tmp_buf;
        LISR_LOG_MSGID_I("+dsp_rx\r\n", 0);
        {   /* Check Rx PCM ring buffer data byte count */
            uint32_t data_byte_cnt = ring_buffer_get_data_byte_count(p_rx_ring);
            if (data_byte_cnt >= frm_byte_cnt) {
                p_rx_info->pcm_underflow = BT_HFP_FALSE;
            } else {    /* Rx PCM ring buffer underflow */
                LISR_LOG_MSGID_W("DSP Rx UF, %d\r\n",1, isr_time);
                p_rx_info->pcm_underflow = BT_HFP_TRUE;
                memset(p_tmp_base, 0, BT_HFP_DSP_FRAME_SIZE);
                audio_pcm2way_put_to_stream_out(p_tmp_base);

#if defined(MTK_BT_HFP_CODEC_DEBUG)
                if (bt_hfp_codec_debug_is_open && (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM)) {
                    //uint32_t curr_cnt = 0;
                    //uint32_t curr_cnt2 = 0;
                    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);

                    uint32_t stream_out_pcm_byte_count = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;
                    bt_hfp_codec_save_debug_data_to_debug_array_if_full_write_to_sdcard(&fdst_hfp_downlink_stream_out_pcm, (uint8_t *)p_tmp_base, stream_out_pcm_byte_count);

                    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt2);
                    //TASK_LOG_CTRL("SAVE_DOWNLINK_STREAM_OUT_PCM time cost= %d\r\n", (curr_cnt2-curr_cnt));
                }
#endif
                return;
            }
        }
        {   /* Get PCM data from Rx PCM ring buffer & write to stream out */
            uint8_t *p_tmp_buf = (uint8_t *)p_tmp_base;
            uint8_t *p_pcm_buf = NULL;
            uint32_t read_byte_cnt = 0;
            uint32_t loop_idx;

            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                ring_buffer_get_read_information(p_rx_ring, &p_pcm_buf, &read_byte_cnt);
                read_byte_cnt = MINIMUM(read_byte_cnt, frm_byte_cnt);
                memcpy(p_tmp_buf, p_pcm_buf, read_byte_cnt);
                p_tmp_buf += read_byte_cnt;
                frm_byte_cnt -= read_byte_cnt;
                ring_buffer_read_done(p_rx_ring, read_byte_cnt);
            }

            {   /* If register mix stream out callback */
                if (p_info->mix_stream_out_callback.callback_function){
                    if (!p_info->mix_stream_out_callback.waiting) {
                        p_info->mix_stream_out_callback.waiting = true;

                        uint32_t mix_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;

                        uint16_t *p_tmp_mix_data_base = (uint16_t *)bt_hfp_codec_internal_handle->mix_stream_out_data;

                        bt_hfp_codec_mix_stream_out(bt_hfp_codec_internal_handle->mix_stream_out_method, p_tmp_base, p_tmp_mix_data_base, mix_byte_cnt/2);

                        p_info->mix_stream_out_callback.callback_function(BT_CODEC_MEDIA_REQUEST, p_info->mix_stream_out_callback.data);

                    }
                }
            }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
            {
                uint32_t mix_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;

                audio_mixer_p2w_isr((uint8_t *)p_tmp_base, mix_byte_cnt);
            }
#endif
            {   /* If register get stream out callback */
                if (p_info->get_stream_out_callback.callback_function){
                    if (!p_info->get_stream_out_callback.waiting) {
                        p_info->get_stream_out_callback.waiting = true;

                        uint32_t get_byte_cnt = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;

                        memcpy(bt_hfp_codec_internal_handle->stream_out_data, (uint8_t *)p_tmp_base, get_byte_cnt);

                        p_info->get_stream_out_callback.callback_function(BT_CODEC_MEDIA_DATA_NOTIFICATION, p_info->get_stream_out_callback.data);

                    }
                }
            }

            audio_pcm2way_put_to_stream_out(p_tmp_base);

#if defined(MTK_BT_HFP_CODEC_DEBUG)
                if (bt_hfp_codec_debug_is_open && (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM)) {
                    //uint32_t curr_cnt = 0;
                    //uint32_t curr_cnt2 = 0;

                    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);


                    uint32_t stream_out_pcm_byte_count = codec_type == BT_HFP_CODEC_TYPE_CVSD ? 320 : 640;
                    bt_hfp_codec_save_debug_data_to_debug_array_if_full_write_to_sdcard(&fdst_hfp_downlink_stream_out_pcm, (uint8_t *)p_tmp_base, stream_out_pcm_byte_count);

                    //hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt2);
                    //TASK_LOG_CTRL("SAVE_DOWNLINK_STREAM_OUT_PCM time cost2= %d\r\n", (curr_cnt2-curr_cnt));
                }
#endif
        }

#if (defined MTK_EXTERNAL_DSP_ENABLE) && (defined MTK_NDVC_ENABLE)
        if(!ndvc_at_test)
        {
            uint16_t noise_db = external_dsp_read_dsp_spl_value();
            uint16_t noise_idx = spe_ndvc_uplink_noise_index_map(noise_db);
            spe_ndvc_noise_setting(noise_idx);
            TASK_LOG_MSGID_I("\r\n[HFP]NDVC Test noise_dB=%d, index=%d \r\n",2, noise_db, noise_idx);
        }
#endif
        LISR_LOG_MSGID_I("-dsp_rx\r\n", 0);
        bt_codec_event_send_from_isr(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS, NULL);
    }
    return;
}
#endif
#endif //MTK_AVM_DIRECT

#if defined(MTK_AVM_DIRECT)
static void bt_hfp_isr_handler(hal_audio_event_t event, void *data)
{
    bt_hfp_codec_internal_handle_t *internal_handle = (bt_hfp_codec_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
    if (event == HAL_AUDIO_EVENT_HFP_PARA_SEND) {
        handle->handler(handle, BT_CODEC_MEDIA_HFP_AVC_PARA_SEND);
    }
}

static bt_codec_media_status_t bt_hfp_play(bt_media_handle_t *handle)
{
    bt_hfp_codec_internal_handle_t *internal_handle = (bt_hfp_codec_internal_handle_t *)handle;
    bool aws_flag = internal_handle->aws_flag;

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_VOICE_DL, bt_hfp_isr_handler, handle);
    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_VOICE_UL, bt_hfp_isr_handler, handle);
    #if defined(MTK_AVM_DIRECT)
        mcu2dsp_start_param_t start_param;
        void *p_param_share;

        // Collect parameters
        start_param.param.stream_in     = STREAM_IN_HFP;
        start_param.param.stream_out    = STREAM_OUT_AFE;

        start_param.stream_out_param.afe.aws_flag   =  aws_flag;

        p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_DL);
        #if (DL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_START, 0, (uint32_t)p_param_share, true);
        #endif
    #else
        #if (DL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_START, 0, 0, true);
        #endif
    #endif

    #if defined(MTK_AVM_DIRECT)
        // Collect parameters
        start_param.param.stream_in     = STREAM_IN_AFE;
        start_param.param.stream_out    = STREAM_OUT_HFP;

        start_param.stream_in_param.afe.aws_flag   =  aws_flag;

        p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_UL);
        #if (UL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_START, 0, (uint32_t)p_param_share, true);
        #endif
    #else
        #if (UL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_START, 0, 0, true);
        #endif
    #endif

#ifdef AIR_HFP_DNN_PATH_ENABLE
#if (UL_DNN_EN == TRUE)
        start_param.param.stream_in     = STREAM_IN_AFE;
        start_param.param.stream_out    = STREAM_OUT_AFE;
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_START, AUDIO_DSP_CODEC_TYPE_DNN, 0, true);
#endif
#endif

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
        ami_execute_vendor_se(EVENT_AFTER_HFP_START);
#endif
#endif

    handle->state = BT_CODEC_STATE_PLAY;

    return BT_CODEC_MEDIA_STATUS_OK;
}
#else
static bt_codec_media_status_t bt_hfp_play(bt_media_handle_t *handle)
{
#if defined(BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL)
    initialize_interrupt_interval();
#endif  /* BT_HFP_CODEC_CHECK_INTERRUPT_INTERVAL */
    TASK_LOG_MSGID_CTRL("+play, %ld\r\n",1, bt_codec_get_interrupt_time(false));
    {   /* PLAY */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
            TASK_LOG_MSGID_E("play not ready\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        /******************[TAIL] Check State *******************************************************/
        /******************[HEAD] Initialize Codec Library ******************************************/
        {
            bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
            bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
            void **en_handle = &p_tx_info->encoder.handle;
            void **de_handle = &p_rx_info->decoder.handle;
            void **pl_handle = &p_rx_info->plc.handle;
            uint8_t *en_int_buf = (uint8_t *)(*en_handle);
            uint8_t *de_int_buf = (uint8_t *)(*de_handle);
            uint8_t *pl_int_buf = (uint8_t *)(*pl_handle);
            plc_initial_parameter_t param;
            if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
                void **up_handle = &p_tx_info->src_up_sampling.handle;
                void **dw_handle = &p_rx_info->src_down_sampling.handle;
                uint8_t *up_int_buf = (uint8_t *)(*up_handle);
                uint8_t *dw_int_buf = (uint8_t *)(*dw_handle);
                param.sampling_rate = 8000;
                cvsd_encode_init            (en_handle, en_int_buf);
                cvsd_src_up_sampling_init   (up_handle, up_int_buf);
                cvsd_decode_init            (de_handle, de_int_buf);
                cvsd_src_down_sampling_init (dw_handle, dw_int_buf);
            } else {
                param.sampling_rate = 16000;
                msbc_encode_init (en_handle, en_int_buf);
                msbc_decode_init (de_handle, de_int_buf);
            }
            {
                uint32_t buf_size = p_rx_info->plc.internal_buffer_size;
                memset(pl_int_buf, 0, buf_size);
                plc_init (pl_handle, pl_int_buf, &param);
            }
        }
        /******************[TAIL] Initialize Codec Library ******************************************/
        /******************[HEAD] Hook BT Interrupt Handler *****************************************/
        bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS, bt_hfp_codec_rx_process_handler);
        /******************[TAIL] Hook BT Interrupt Handler *****************************************/
        /******************[HEAD] Set PLAY State ****************************************************/
        handle->state = BT_CODEC_STATE_PLAY;
        /******************[TAIL] Set PLAY State ****************************************************/
        /******************[HEAD] Unmask BT Interrupt ***********************************************/
        p_info->eint_status = hal_eint_unmask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            TASK_LOG_MSGID_E("play unmask %d\r\n",1, p_info->eint_status);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        /******************[TAIL] Unmask BT Interrupt ***********************************************/
        /******************[HEAD] PCM2WAY Start *****************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            audio_pcm2way_band_t band;
            audio_pcm2way_format_t format;
            audio_pcm2way_config_t config;
            if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
                band   = PCM2WAY_NARROWBAND;
                format = PCM2WAY_FORMAT_NORMAL;
            } else {
                band   = PCM2WAY_WIDEBAND;
                format = PCM2WAY_FORMAT_WB_NORMAL;
            }
            config.stream_in_d2m_on                = BT_HFP_TRUE;
            config.stream_in_is_after_enhancement  = BT_HFP_TRUE;
            config.stream_out_m2d_on               = BT_HFP_TRUE;
            config.stream_out_is_after_enhancement = BT_HFP_FALSE;
            config.band                            = band;
            config.format                          = format;
            bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS, bt_hfp_dsp_tx_process_handler);
            bt_codec_event_register_callback(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS, bt_hfp_dsp_rx_process_handler);
            TASK_LOG_MSGID_CTRL(" play, p2w\r\n", 0);
            {
                uint16_t device = afe_audio_get_output_device();
                hal_speech_device_mode_t enh_mode;
                if (device == HAL_AUDIO_DEVICE_HEADSET || device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
                    enh_mode = SPH_MODE_BT_EARPHONE;
                } else {
                    enh_mode = SPH_MODE_BT_LOUDSPEAKER;
                }

                if (bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function == NULL) {
                    speech_set_mode(enh_mode);
                }
            }
#ifdef MTK_EXTERNAL_DSP_ENABLE
#ifdef MTK_AUTOMOTIVE_SUPPORT
            external_dsp_status_t status = external_dsp_switch_automotive_mode(EXTERNAL_DSP_AUTO_BT_MODE);
#else
            hal_audio_set_extended_out_device(HAL_AUDIO_EXT_DEVICE_I2S_OUT);
#endif
#endif
            {
                if (bt_hfp_codec_internal_handle->device_path == BT_CODEC_HFP_DEVICE_PATH_MASTER_I2S){
                    TASK_LOG_MSGID_CTRL("bt_hfp_play: uplink fifo from i2s\r\n", 0);
                    audio_pcm2way_set_dsp_uplink_fifo(I2S_FIFO);
                } else{
                    // TODO: although default is ul FIFO, but we may set clearly
                }

            }
            audio_pcm2way_start(bt_hfp_codec_pcm2way_stream_out_callback, bt_hfp_codec_pcm2way_stream_in_callback, &config);
#if defined(MTK_EXTERNAL_DSP_ENABLE) && defined(MTK_AUTOMOTIVE_SUPPORT)
            external_dsp_clk_sw_config(true);
            external_dsp_echo_ref_sw_config(true);
#endif

            if (bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function == NULL) {
                speech_set_enhancement(true);
            }
        }
        /******************[TAIL] PCM2WAY Start *****************************************************/
    }
    TASK_LOG_MSGID_CTRL("-play, %ld\r\n",1, bt_codec_get_interrupt_time(false));
    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif

#if defined(MTK_AVM_DIRECT)
static bt_codec_media_status_t bt_hfp_stop(bt_media_handle_t *handle)
{
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_BEFORE_HFP_STOP);
#endif
#endif
#if (DL_ONLY == TRUE)
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_STOP, 0, 0, true);
#endif
#if (UL_ONLY == TRUE)
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_STOP, 0, 0, true);
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
#if (UL_DNN_EN == TRUE)
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_STOP, AUDIO_DSP_CODEC_TYPE_DNN, 0, true);
#endif
#endif /*AIR_HFP_DNN_PATH_ENABLE*/
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_VOICE_DL);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_VOICE_UL);

    handle->state = BT_CODEC_STATE_STOP;
#ifdef HAL_DVFS_MODULE_ENABLED
    if(g_hfp_dvfs_clk != HFP_DVFS_DEFAULT_SPEED){
        hal_dvfs_lock_control(g_hfp_dvfs_clk, HAL_DVFS_UNLOCK);
        TASK_LOG_MSGID_CTRL("[HFP][stop] dvfs unlock from:%d", 1, g_hfp_dvfs_clk);
    }
#endif
    return BT_CODEC_MEDIA_STATUS_OK;
}
#else
static bt_codec_media_status_t bt_hfp_stop(bt_media_handle_t *handle)
{
    TASK_LOG_MSGID_CTRL("+stop, %ld\r\n",1, bt_codec_get_interrupt_time(false));
    {   /* STOP */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
            TASK_LOG_MSGID_E("stop not play\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        /******************[TAIL] Check State *******************************************************/
        /******************[HEAD] Set STOP State ****************************************************/
        handle->state = BT_CODEC_STATE_STOP;
        /******************[TAIL] Set STOP State ****************************************************/
        /******************[HEAD] PCM2WAY Stop ******************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            speech_set_enhancement(false);
  #ifdef MTK_EXTERNAL_DSP_ENABLE
  #ifdef MTK_AUTOMOTIVE_SUPPORT
            external_dsp_clk_sw_config(false);
            external_dsp_echo_ref_sw_config(false);
            external_dsp_switch_automotive_mode(EXTERNAL_DSP_AUTO_VR_MODE);
  #else
            hal_audio_set_extended_out_device(HAL_AUDIO_EXT_DEVICE_NONE);
  #endif
  #endif
            audio_pcm2way_stop();
            bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS);
            bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS);
            TASK_LOG_MSGID_CTRL(" stop, p2w\r\n", 0);
        }
        /******************[TAIL] PCM2WAY Stop ******************************************************/
        /******************[HEAD] Mask BT Interrupt *************************************************/
        p_info->eint_status = hal_eint_mask(p_info->eint_number);
        if (p_info->eint_status != HAL_EINT_STATUS_OK) {
            TASK_LOG_MSGID_E("stop mask %d\r\n",1, p_info->eint_status);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        /******************[TAIL] Mask BT Interrupt *************************************************/
        /******************[HEAD] Unhook BT Interrupt Handler ***************************************/
        bt_codec_event_deregister_callback(BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS);
        /******************[TAIL] Unhook BT Interrupt Handler ***************************************/
    }
    TASK_LOG_MSGID_CTRL("-stop, %ld\r\n",1, bt_codec_get_interrupt_time(false));
    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif

/* Should not use this API */
static bt_codec_media_status_t bt_hfp_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    TASK_LOG_MSGID_E("process\r\n", 0);
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

static void bt_set_aws_flag(bt_media_handle_t *handle, bool is_aws)
{
#if defined(MTK_AVM_DIRECT)
    bt_hfp_codec_internal_handle_t *internal_handle = (bt_hfp_codec_internal_handle_t *)handle;

    TASK_LOG_MSGID_CTRL("hfp codec set aws flag:%d\r\n",1, is_aws);
    internal_handle->aws_flag = is_aws;
#endif
}

static void bt_trigger_mic(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    if(handle) {
        TASK_LOG_MSGID_CTRL("hfp codec Trigger Mic On\r\n", 0);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_PLAY_EN, 0, 0, 0);
    }
#endif
}

static void bt_side_tone_enable(bt_media_handle_t *handle)
{
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        TASK_LOG_MSGID_CTRL("[HFP Codec]SideTone Enable No Handle\n", 0);
        return;
    }

    TASK_LOG_MSGID_CTRL("[HFP Codec]SideTone Enable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain                            = 600;
    sidetone.sample_rate                     = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_START, 0, (uint32_t)p_param_share, true);
}

static void bt_side_tone_disable(bt_media_handle_t *handle)
{
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        TASK_LOG_MSGID_CTRL("[HFP Codec]SideTone Disable No Handle\n", 0);
        return;
    }

    TASK_LOG_MSGID_CTRL("[HFP Codec]SideTone Disable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain                            = 0;
    sidetone.sample_rate                     = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_STOP, 0, (uint32_t)p_param_share, true);
}

static bt_media_handle_t *bt_hfp_open_codec_internal(bt_hfp_audio_codec_type_t codec_type, bt_hfp_mode_t mode)
{
    bt_hfp_codec_internal_handle_t *p_info;
    bt_media_handle_t *handle;
    TASK_LOG_MSGID_CTRL("+open\r\n", 0);
    {   /* Allocate Internal Handle */
        bt_hfp_speech_tx_information_t *p_tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info;
        uint32_t *pkt_tmp_buf;
        p_info = (bt_hfp_codec_internal_handle_t *)pvPortMalloc(sizeof(bt_hfp_codec_internal_handle_t));
        if (p_info == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_open_codec_internal: p_info allocate fail\r\n", 0);
            return NULL;
        }

        pkt_tmp_buf = (uint32_t *)pvPortMalloc(BT_HFP_HW_SRAM_SIZE);
        if (pkt_tmp_buf == NULL) {
            TASK_LOG_MSGID_CTRL("bt_hfp_open_codec_internal: pkt_tmp_buf allocate fail\r\n", 0);
            vPortFree(p_info);
            return NULL;
        }

        memset(p_info, 0, sizeof(bt_hfp_codec_internal_handle_t));
        handle = &p_info->handle;
        handle->play    = bt_hfp_play;
        handle->stop    = bt_hfp_stop;
        handle->process = bt_hfp_process;
        handle->set_aws_flag = bt_set_aws_flag;
        handle->trigger_mic = bt_trigger_mic;
        handle->side_tone_enable = bt_side_tone_enable;
        handle->side_tone_disable = bt_side_tone_disable;
        bt_hfp_codec_internal_handle = p_info;
        bt_hfp_codec_is_running = true;
        p_info->codec_type  = codec_type;
        p_info->mode        = mode;
        p_info->pkt_tmp_buf = pkt_tmp_buf;
        p_tx_info = &p_info->tx_info;
        p_rx_info = &p_info->rx_info;
#if !defined(MTK_AVM_DIRECT)
        bt_hfp_set_shared_memory_information(BTSRAM_BASE, BT_HFP_HW_REG_PACKET_R, BT_HFP_HW_REG_PACKET_W, BT_HFP_HW_REG_CONTROL);
#endif
        if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
            audio_nvdm_update_status(AUDIO_NVDM_USER_HFP, AUDIO_NVDM_STATUS_CHANGE);
            /* Codec Setting */
            if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
                if (bt_hfp_cvsd_open(p_tx_info, p_rx_info) == BT_CODEC_MEDIA_STATUS_ERROR) {
                    TASK_LOG_MSGID_CTRL("bt_hfp_open_codec_internal: bt_hfp_cvsd_open fail\r\n", 0);
                    vPortFree(p_info->pkt_tmp_buf);
                    vPortFree(p_info);
                    return NULL;
                }
            } else {
                if (bt_hfp_msbc_open(p_tx_info, p_rx_info) == BT_CODEC_MEDIA_STATUS_ERROR) {
                    TASK_LOG_MSGID_CTRL("bt_hfp_open_codec_internal: bt_hfp_msbc_open fail\r\n", 0);
                    vPortFree(p_info->pkt_tmp_buf);
                    vPortFree(p_info);
                    return NULL;
                }
            }
        }
        {   /* Set ring buffer */
            ring_buffer_information_t *p_tx_ring = &p_tx_info->pcm_ring_info;
            ring_buffer_information_t *p_rx_ring = &p_rx_info->pcm_ring_info;
            p_tx_ring->buffer_base_pointer = p_tx_info->pcm_ring_buf;
            p_rx_ring->buffer_base_pointer = p_rx_info->pcm_ring_buf;
            p_tx_ring->buffer_byte_count = BT_HFP_TX_PCM_RING_BUFFER_SIZE;
            p_rx_ring->buffer_byte_count = BT_HFP_RX_PCM_RING_BUFFER_SIZE;
        }
    }

#if !defined(MTK_AVM_DIRECT)
    {   /* EINT Settings */
        hal_eint_number_t eint_num = HAL_EINT_BTSYS;
        hal_eint_status_t status;
        p_info->eint_number = eint_num;
        {   /* EINT Mask */
            status = hal_eint_mask(eint_num);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
        {   /* EINT Init */
            hal_eint_config_t *p_config = &p_info->eint_config;
            p_config->trigger_mode = HAL_EINT_LEVEL_LOW;
            p_config->debounce_time = 3;    /* Unit: ms */
            status = hal_eint_init(eint_num, p_config);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
        {   /* EINT Callback */
            hal_eint_callback_t callback = bt_hfp_bt_interrupt_callback;
            void *user_data = NULL;
            p_info->eint_callback = callback;
            status = hal_eint_register_callback(eint_num, callback, user_data);
            p_info->eint_status = status;
            RETURN_NULL_IF_FALSE(status == HAL_EINT_STATUS_OK);
        }
    }
#endif

    handle->state = BT_CODEC_STATE_READY;
    TASK_LOG_MSGID_CTRL("-open\r\n", 0);
    return handle;
}

static bt_codec_media_status_t bt_hfp_close_codec_internal(void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    bt_media_handle_t *handle;
    bt_codec_state_t state;
    bt_hfp_mode_t mode;
    {   /* Basic Settings & Check */
        RETURN_MEDIA_ERROR_IF_FALSE(p_info != NULL);
        handle = &p_info->handle;
        state = handle->state;
        mode = p_info->mode;
        RETURN_MEDIA_ERROR_IF_FALSE(state == BT_CODEC_STATE_STOP);
    }
    TASK_LOG_MSGID_CTRL("+close\r\n", 0);
    if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
#if defined(MTK_AVM_DIRECT)
        // Notify to release dynamic download.
#if (UL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_CLOSE, 0, 0, true);
#endif
#if (DL_ONLY == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_CLOSE, 0, 0, true);
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_VOICE_DL, 0, 0, FALSE);
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
#if (UL_DNN_EN == TRUE)
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_CLOSE, AUDIO_DSP_CODEC_TYPE_DNN, 0, true);
#endif
#endif /*AIR_HFP_DNN_PATH_ENABLE*/
#else
        /* Codec Settings */
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
        RETURN_MEDIA_ERROR_IF_FALSE(codec_type == BT_HFP_CODEC_TYPE_CVSD || codec_type == BT_HFP_CODEC_TYPE_MSBC);
        if (codec_type == BT_HFP_CODEC_TYPE_CVSD) {
            bt_hfp_cvsd_close(p_tx_info, p_rx_info);
        } else {
            bt_hfp_msbc_close(p_tx_info, p_rx_info);
        }
#endif
    }

#if !defined(MTK_AVM_DIRECT)
    {   /* EINT Deinit */
        hal_eint_number_t eint_num = p_info->eint_number;
        hal_eint_status_t status = hal_eint_deinit(eint_num);
        p_info->eint_status = status;
        RETURN_MEDIA_ERROR_IF_FALSE(status == HAL_EINT_STATUS_OK);
        p_info->eint_number = HAL_EINT_NUMBER_0;
    }
#endif

    handle->state = BT_CODEC_STATE_IDLE;
    vPortFree(p_info->pkt_tmp_buf);
    vPortFree(p_info);
    bt_hfp_codec_is_running = false;
    TASK_LOG_MSGID_CTRL("-close\r\n", 0);
    return BT_CODEC_MEDIA_STATUS_OK;
}

#if !defined(MTK_AVM_DIRECT)
void bsp_bt_codec_task_main(void *arg)
{
    bt_codec_queue_event_t event;
    bt_codec_queue_handle = xQueueCreate(BT_CODEC_QUEUE_SIZE, sizeof(bt_codec_queue_event_t));
    {   /* Initialize queue registration */
        uint32_t id_idx;
        for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
            bt_codec_queue_id_array[id_idx] = BT_CODEC_QUEUE_EVENT_NONE;
        }
    }
    while (1) {
        if (xQueueReceive(bt_codec_queue_handle, &event, portMAX_DELAY)) {
            bt_codec_queue_event_id_t rece_id = event.id;
            uint32_t id_idx;
            for (id_idx = 0; id_idx < MAX_BT_CODEC_FUNCTIONS; id_idx++) {
                if (bt_codec_queue_id_array[id_idx] == rece_id) {
                    bt_codec_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}
#endif

/*******************************************************************************\
| Public Functions                                                              |
\*******************************************************************************/

void bt_codec_task_create(void)
{
#if !defined(MTK_AVM_DIRECT)
    xTaskCreate(bsp_bt_codec_task_main, BT_CODEC_TASK_NAME, BT_CODEC_TASK_STACKSIZE/((uint32_t)sizeof(StackType_t)), NULL, BT_CODEC_TASK_PRIO, NULL);
#endif
    return;
}

bt_codec_media_status_t bt_codec_hfp_unregister_customer_enhancement_function_callback(void)
{
    if (bt_hfp_codec_internal_handle->get_stream_out_callback.callback_function == NULL){
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_customer_enhancement_function_callback: customer enhancement callback was never registered. Just return\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_OK;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_customer_enhancement_function_callback++\r\n",0);

    bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function = NULL;
    bt_hfp_codec_internal_handle->customer_enhancement_callback.data = NULL;

    vPortFree(bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data);

    bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data = NULL;

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_customer_enhancement_function_callback--\r\n",0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_register_customer_enhancement_function_callback(bt_codec_hfp_customer_enhancement_function_callback_t callback, void *user_data)
{
    if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback: bt codec state not BT_CODEC_STATE_READY\r\n",0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback++\r\n",0);

    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data = pvPortMalloc(CVSD_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data == NULL){
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback: CVSD customer_enhanced_stream_in_data allocate fail\r\n",0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC){
        bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data = pvPortMalloc(MSBC_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->customer_enhanced_stream_in_data == NULL){
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback: MSBC customer_enhanced_stream_in_data allocate fail\r\n",0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: wrong codec_type\r\n",0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }


    if (bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function == NULL){
        bt_hfp_codec_internal_handle->customer_enhancement_callback.callback_function = callback;
        bt_hfp_codec_internal_handle->customer_enhancement_callback.data = user_data;
        bt_hfp_codec_internal_handle->customer_enhancement_callback.waiting = false;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback: customer enhancement callback already registered\r\n",0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_customer_enhancement_function_callback--\r\n",0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_mix_stream_out(bt_codec_media_hfp_mix_stream_out_method_t mix_method, void *buffer, uint32_t sample_count)
{
    uint32_t write_byte_count = 0;
    uint8_t *p_source = (uint8_t *)buffer;


    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        write_byte_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        write_byte_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_mix_stream_out: wrong codec_type\r\n",0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }


    bt_hfp_codec_internal_handle->mix_stream_out_method = mix_method;

    write_byte_count = (sample_count < write_byte_count) ? sample_count : write_byte_count;

    memcpy(bt_hfp_codec_internal_handle->mix_stream_out_data, p_source, write_byte_count);

    bt_hfp_codec_internal_handle->mix_stream_out_callback.waiting = false;

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_get_mix_stream_out_sample_count(uint32_t *sample_count)
{
    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        *sample_count = CVSD_TONE_BUFFER_SIZE_IN_BYTES;
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC) {
        *sample_count = MSBC_TONE_BUFFER_SIZE_IN_BYTES;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_get_mix_stream_out_sample_count: wrong codec_type\r\n",0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}


bt_codec_media_status_t bt_codec_hfp_unregister_mix_stream_out_data_callback(void)
{
    if (bt_hfp_codec_internal_handle->mix_stream_out_callback.callback_function == NULL){
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_mix_stream_out_data_callback: mix stream out data callback was never registered. Just return\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_OK;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_mix_stream_out_data_callback++\r\n", 0);

    bt_hfp_codec_internal_handle->mix_stream_out_callback.callback_function = NULL;
    bt_hfp_codec_internal_handle->mix_stream_out_callback.data = NULL;


    vPortFree(bt_hfp_codec_internal_handle->mix_stream_out_data);

    bt_hfp_codec_internal_handle->mix_stream_out_data = NULL;

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_mix_stream_out_data_callback--\r\n", 0);

    return BT_CODEC_MEDIA_STATUS_OK;
}


bt_codec_media_status_t bt_codec_hfp_register_mix_stream_out_data_callback(bt_codec_hfp_mix_stream_out_data_callback_t callback, void *user_data)
{
    if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: bt codec state not BT_CODEC_STATE_READY\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback++\r\n", 0);

    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        bt_hfp_codec_internal_handle->mix_stream_out_data = pvPortMalloc(CVSD_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->mix_stream_out_data == NULL){
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: CVSD mix_stream_out_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC){
        bt_hfp_codec_internal_handle->mix_stream_out_data = pvPortMalloc(MSBC_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->mix_stream_out_data == NULL){
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: MSBC mix_stream_out_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (bt_hfp_codec_internal_handle->mix_stream_out_callback.callback_function == NULL){
        bt_hfp_codec_internal_handle->mix_stream_out_callback.callback_function = callback;
        bt_hfp_codec_internal_handle->mix_stream_out_callback.data = user_data;
        bt_hfp_codec_internal_handle->mix_stream_out_callback.waiting = false;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_mix_stream_out_data_callback: mix stream out data callback already registered\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    bt_hfp_codec_internal_handle->mix_stream_out_callback.waiting = false;

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback--\r\n", 0);
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_unregister_get_stream_out_data_callback(void)
{
    if (bt_hfp_codec_internal_handle->get_stream_out_callback.callback_function == NULL){
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_out_data_callback: get stream out data callback was never registered. Just return\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_OK;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_out_data_callback++\r\n", 0);

    bt_hfp_codec_internal_handle->get_stream_out_callback.callback_function = NULL;
    bt_hfp_codec_internal_handle->get_stream_out_callback.data = NULL;


    vPortFree(bt_hfp_codec_internal_handle->stream_out_data);
    bt_hfp_codec_internal_handle->stream_out_data = NULL;

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_out_data_callback--\r\n", 0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_register_get_stream_out_data_callback(bt_codec_hfp_get_stream_out_data_callback_t callback, void *user_data)
{
    if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback: bt codec state not BT_CODEC_STATE_READY\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback++\r\n", 0);

    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        bt_hfp_codec_internal_handle->stream_out_data = pvPortMalloc(CVSD_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->stream_out_data == NULL) {
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback: CVSD stream_out_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC){
        bt_hfp_codec_internal_handle->stream_out_data = pvPortMalloc(MSBC_TONE_BUFFER_SIZE_IN_BYTES);
        if (bt_hfp_codec_internal_handle->stream_out_data == NULL) {
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback: MSBC stream_out_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (bt_hfp_codec_internal_handle->get_stream_out_callback.callback_function == NULL){
        bt_hfp_codec_internal_handle->get_stream_out_callback.callback_function = callback;
        bt_hfp_codec_internal_handle->get_stream_out_callback.data = user_data;
        bt_hfp_codec_internal_handle->get_stream_out_callback.waiting = false;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback: get stream out data callback already registered\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_out_data_callback--\r\n", 0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_hfp_unregister_get_stream_in_data_callback(void)
{
    if (bt_hfp_codec_internal_handle->get_stream_in_callback.callback_function == NULL){
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_in_data_callback: get stream in data callback was never registered. Just return\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_OK;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_in_data_callback++\r\n", 0);

    bt_hfp_codec_internal_handle->get_stream_in_callback.callback_function = NULL;
    bt_hfp_codec_internal_handle->get_stream_in_callback.data = NULL;


    vPortFree(bt_hfp_codec_internal_handle->stream_in_data);
    bt_hfp_codec_internal_handle->stream_in_data = NULL;

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_unregister_get_stream_in_data_callback--\r\n", 0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

#if !defined(MTK_AVM_DIRECT)
bt_codec_media_status_t bt_codec_hfp_register_get_stream_in_data_callback(bt_codec_hfp_get_stream_in_data_callback_t callback, void *user_data)
{
    if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback: bt codec state not BT_CODEC_STATE_READY\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback++\r\n", 0);

    if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        if(afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            bt_hfp_codec_internal_handle->stream_in_data = pvPortMalloc(CVSD_TONE_BUFFER_SIZE_IN_BYTES * 2);
        } else {
            bt_hfp_codec_internal_handle->stream_in_data = pvPortMalloc(CVSD_TONE_BUFFER_SIZE_IN_BYTES);
        }

        if (bt_hfp_codec_internal_handle->stream_in_data == NULL) {
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback: CVSD stream_in_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_MSBC){
        if(afe_audio_get_input_device() == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            bt_hfp_codec_internal_handle->stream_in_data = pvPortMalloc(MSBC_TONE_BUFFER_SIZE_IN_BYTES * 2);
        } else {
            bt_hfp_codec_internal_handle->stream_in_data = pvPortMalloc(MSBC_TONE_BUFFER_SIZE_IN_BYTES);
        }

        if (bt_hfp_codec_internal_handle->stream_in_data == NULL) {
            TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback: MSBC stream_in_data allocate fail\r\n", 0);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback: wrong codec_type\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (bt_hfp_codec_internal_handle->get_stream_in_callback.callback_function == NULL){
        bt_hfp_codec_internal_handle->get_stream_in_callback.callback_function = callback;
        bt_hfp_codec_internal_handle->get_stream_in_callback.data = user_data;
        bt_hfp_codec_internal_handle->get_stream_in_callback.waiting = false;
    } else {
        TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback: get stream in data callback already registered\r\n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    TASK_LOG_MSGID_CTRL("bt_codec_hfp_register_get_stream_in_data_callback--\r\n", 0);
    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif

bt_codec_media_status_t bt_codec_hfp_set_device_path(bt_codec_hfp_device_path_t device_path)
{
    bt_hfp_codec_internal_handle->device_path = device_path;
    TASK_LOG_MSGID_CTRL("bt_codec_hfp_set_device_path: device_path=%d\r\n",1, device_path);
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_codec_hfp_audio_t *param)
{
    bt_hfp_audio_codec_type_t codec_type;
    bt_hfp_mode_t mode;
    bt_media_handle_t *handle;
    RETURN_NULL_IF_FALSE(param != NULL);
    codec_type = param->type;
    TASK_LOG_MSGID_CTRL("codec type:%d\r\n", 1, codec_type);
    RETURN_NULL_IF_FALSE(codec_type == BT_HFP_CODEC_TYPE_CVSD || codec_type == BT_HFP_CODEC_TYPE_MSBC);
    mode = BT_HFP_MODE_SPEECH;
    TASK_LOG_MSGID_CTRL("open_codec\r\n", 0);
    handle = bt_hfp_open_codec_internal(codec_type, mode);
    handle->handler = callback;
#ifdef MTK_EXTERNAL_DSP_ENABLE
#ifndef MTK_AUTOMOTIVE_SUPPORT
    external_dsp_activate(true);
#endif
#endif
    return handle;
}

bt_codec_media_status_t bt_codec_hfp_close (bt_media_handle_t *handle)
{
    bt_codec_media_status_t status;
    TASK_LOG_MSGID_CTRL("close_codec\r\n", 0);
#ifdef RACE_DSP_REALTIME_CMD_ENABLE
#ifdef MTK_AIRDUMP_EN
    airdump_timer_delete();
#endif
#endif
    status = bt_hfp_close_codec_internal();

#ifdef MTK_EXTERNAL_DSP_ENABLE
#ifndef MTK_AUTOMOTIVE_SUPPORT
    external_dsp_activate(false);
#endif
#endif
    return status;
}

#if defined(MTK_BT_HFP_CODEC_DEBUG)
void bt_hfp_codec_debug_open(bt_hfp_codec_debug_flag_t bt_hfp_codec_debug_flag, bt_hfp_codec_debug_save_or_print_method_t bt_hfp_codec_debug_save_or_print_method)
{
    FRESULT f_res;

    TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_open++ \n", 0);

    if ((bt_hfp_codec_debug_flag == BT_HFP_CODEC_DEBUG_FLAG_NONE) || (bt_hfp_codec_debug_is_open == true)) {
        TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_flag = BT_HFP_CODEC_DEBUG_FLAG_NONE or bt_hfp_codec_debug_is_open == true, just return\r\n", 0);
        return;
    }

    bt_hfp_codec_debug_flag_global = bt_hfp_codec_debug_flag;

    memset(&bt_hfp_codec_debug_array, 0, sizeof(bt_hfp_codec_debug_array));


    if (((bt_hfp_codec_debug_flag & (BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA | BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM | BT_HFP_CODEC_DEBUG_FLAG_VM_LOG)) != 0)) {
        if (((bt_hfp_codec_debug_flag & BT_HFP_CODEC_DEBUG_FLAG_VM_LOG) != 0)) {
            // since now f_mount can't f_mount twice and there's f_mount in audio_vm_Start(),
            // so we XOR this flag with others

            TASK_LOG_MSGID_CTRL("Start save VM log. Note, it will omit other flags \r\n", 0);

            if (bt_hfp_codec_debug_save_or_print_method == BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD) {
                audio_vm_Start(VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE);
            } else if (bt_hfp_codec_debug_save_or_print_method == BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT) {
                audio_vm_Start(VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT);
            } else {
                TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_save_or_print_method: no this method (%d)\r\n",1, bt_hfp_codec_debug_save_or_print_method);
                return;
            }

            bt_hfp_codec_debug_flag_global = BT_HFP_CODEC_DEBUG_FLAG_VM_LOG;
        } else {
            bt_hfp_codec_debug_flag_global = (bt_hfp_codec_debug_flag & ~BT_HFP_CODEC_DEBUG_FLAG_VM_LOG);  // since now f_mount can't f_mount twice and there's f_mount in audio_vm_Start(), so we XOR flag with BT_HFP_CODEC_DEBUG_FLAG_VM_LOG

            if (f_mount(&fatfs, _T("0:"), 1) == FR_OK) {
                TASK_LOG_MSGID_CTRL("fmount ok \n", 0);

                if (bt_hfp_codec_debug_flag & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM) {
                    f_res = f_open(&fdst_hfp_downlink_stream_out_pcm, _T("SD:/out.pcm"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
                    if (f_res == FR_OK) {
                        TASK_LOG_MSGID_CTRL("fopen ok: out.pcm\r\n", 0);
                    } else {
                        TASK_LOG_MSGID_CTRL("fopen fail: out.pcm, f_res=%d\r\n",1, f_res);
                    }
                }

                if (bt_hfp_codec_debug_flag & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA) {
                    f_res = f_open(&fdst_hfp_downlink_esco_raw_data, _T("SD:/esco.txt"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
                    if (f_res == FR_OK) {
                        TASK_LOG_MSGID_CTRL("fopen ok: esco.txt, hfp_downlink_esco_raw_data\r\n", 0);
                    } else {
                        TASK_LOG_MSGID_CTRL("fopen fail: esco.txt, hfp_downlink_esco_raw_data, f_res=%d\r\n",1, f_res);
                    }
                }
            } else {
                TASK_LOG_MSGID_CTRL("fmount fail, just return\r\n", 0);
                return;
            }
        }
    }else {
        TASK_LOG_MSGID_CTRL("no such a flag bt_hfp_codec_debug_flag=%d (BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA | BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM)=%d\n",2, bt_hfp_codec_debug_flag, (BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA | BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM));
    }

    bt_hfp_codec_debug_is_open = true;

    TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_open-- \n", 0);
}
void bt_hfp_codec_debug_close(void)
{
    TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_close++ \n", 0);
    FRESULT f_res;
    uint32_t write_byte_count = bt_hfp_codec_debug_array.debug_array_write_index;
    UINT length_written = 0;

    bt_hfp_codec_debug_is_open = false;

    if (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM) {
        f_res = f_write(&fdst_hfp_downlink_stream_out_pcm, bt_hfp_codec_debug_array.debug_array, write_byte_count, &length_written);
        if (f_res == FR_OK) {
            TASK_LOG_MSGID_CTRL("write ok, write_byte_count=%d, length_written=%d\r\n",2, write_byte_count, length_written);
        } else {
            TASK_LOG_MSGID_CTRL("write fail: out.pcm, hfp_downlink_esco_raw_data, f_res=%d\r\n",1, f_res);
        }

        if (f_close(&fdst_hfp_downlink_stream_out_pcm) == FR_OK) {
            TASK_LOG_MSGID_CTRL("fclose ok: out.pcm\r\n", 0);
        } else {
            TASK_LOG_MSGID_CTRL("fclose error: out.pcm\r\n", 0);
        }

        f_mount(NULL, _T("0:"), 1);
    }

    if (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA) {
        f_res = f_write(&fdst_hfp_downlink_esco_raw_data, bt_hfp_codec_debug_array.debug_array, write_byte_count, &length_written);
        if (f_res == FR_OK) {
            TASK_LOG_MSGID_CTRL("write ok, write_byte_count=%d, length_written=%d\r\n",2, write_byte_count, length_written);
        } else {
            TASK_LOG_MSGID_CTRL("write fail: esco.txt, hfp_downlink_esco_raw_data, f_res=%d\r\n",1, f_res);
        }

        if (f_close(&fdst_hfp_downlink_esco_raw_data) == FR_OK) {
            TASK_LOG_MSGID_CTRL("fclose ok: esco.txt, hfp_downlink_esco_raw_data\r\n", 0);
        } else {
            TASK_LOG_MSGID_CTRL("fclose error: esco.txt, hfp_downlink_esco_raw_data\r\n", 0);
        }

        f_mount(NULL, _T("0:"), 1);
    }

    if (bt_hfp_codec_debug_flag_global & BT_HFP_CODEC_DEBUG_FLAG_VM_LOG) {
        TASK_LOG_MSGID_CTRL("Stop vm log\r\n", 0);
        audio_vm_Stop();
    }

    memset(&bt_hfp_codec_debug_array, 0, sizeof(bt_hfp_codec_debug_array));

    TASK_LOG_MSGID_CTRL("bt_hfp_codec_debug_close-- \n", 0);
}
#endif
bt_media_handle_t *bt_hf_open_loopback(const bt_hf_loopback_t *param)
{
    bt_hfp_audio_codec_type_t codec_type;
    bt_hf_loopback_mode_t loopback_mode;
    bt_hfp_mode_t mode;
    RETURN_NULL_IF_FALSE(param != NULL);
    codec_type = param->type;
    RETURN_NULL_IF_FALSE(codec_type == BT_HFP_CODEC_TYPE_CVSD || codec_type == BT_HFP_CODEC_TYPE_MSBC);
    loopback_mode = param->mode;
    RETURN_NULL_IF_FALSE(loopback_mode == BT_HF_LOOPBACK_WITHOUT_CODEC || loopback_mode == BT_HF_LOOPBACK_WITH_CODEC);
    if (loopback_mode == BT_HF_LOOPBACK_WITHOUT_CODEC) {
        mode = BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC;
    } else {
        mode = BT_HFP_MODE_LOOPBACK_WITH_CODEC;
    }
    TASK_LOG_MSGID_CTRL("open_loopback\r\n", 0);
    return bt_hfp_open_codec_internal(codec_type, mode);
}

bt_codec_media_status_t bt_hf_close_loopback(void)
{
    TASK_LOG_MSGID_CTRL("close_loopback\r\n", 0);
    return bt_hfp_close_codec_internal();
}

void bt_hfp_set_shared_memory_information (uint32_t base_address, volatile uint32_t *p_offset_r, volatile uint32_t *p_offset_w, volatile uint32_t *p_control)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
    p_mem_info->base_address = base_address;
    p_mem_info->p_offset_r   = p_offset_r;
    p_mem_info->p_offset_w   = p_offset_w;
    p_mem_info->p_control    = p_control;
    return;
}

#if !defined(MTK_AVM_DIRECT)
void bt_hfp_ut_process (void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    RETURN_IF_FALSE(p_info != NULL);
    {   /* Exchange bitstream with BT */
        bt_hfp_shared_memory_information_t *p_mem_info = &p_info->mem_info;
        volatile uint32_t *p_control = p_mem_info->p_control;
        uint32_t bt_hw_ctrl  = *p_control;
        uint32_t pkt_type    = (bt_hw_ctrl >> 18) & 0x7;
        uint32_t type_idx    = bt_hfp_get_packet_type_index(pkt_type);
        uint32_t *mask_array = (uint32_t *)&bt_hfp_rx_packet_valid_mask_table[type_idx];
        uint32_t *p_tmp_buf  = p_info->pkt_tmp_buf;
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_mode_t mode = p_info->mode;
        if (mode == BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC) {
            bt_hfp_read_from_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
            bt_hfp_write_to_shared_memory(p_mem_info, (uint8_t *)p_tmp_buf, bt_hw_ctrl);
            *p_control &= ~BT_HFP_HW_CTRL_MASK;
            return;
        }
        bt_codec_read_from_rx_buffer(p_mem_info, &p_info->rx_info, p_tmp_buf, mask_array, bt_hw_ctrl);
        bt_codec_write_to_tx_buffer(p_mem_info, &p_info->tx_info, p_tmp_buf, codec_type, bt_hw_ctrl);
        *p_control &= ~BT_HFP_HW_CTRL_MASK;
        bt_hfp_codec_rx_process_handler(NULL);
    }
    return;
}
#endif

bool bt_hfp_codec_query_is_running(void)
{
    return bt_hfp_codec_is_running;
}

hal_audio_sampling_rate_t bt_hfp_codec_query_sampling_rate(void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;

    if (p_info->codec_type == BT_HFP_CODEC_TYPE_CVSD) {
        return HAL_AUDIO_SAMPLING_RATE_8KHZ;
    } else {
        return HAL_AUDIO_SAMPLING_RATE_16KHZ;
    }
}

hal_audio_channel_number_t bt_hfp_codec_query_channel_number(void)
{
    return HAL_AUDIO_MONO;
}

