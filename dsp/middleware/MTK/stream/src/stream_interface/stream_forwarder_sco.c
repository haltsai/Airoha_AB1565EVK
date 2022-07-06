/* Copyright Statement:
 *
 * (C) 2014  Airoha Technology Corp. All rights reserved.
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
#include "types.h"
#include "dsp_memory.h"
#include "stream_audio.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "transform.h"
#include "stream_n9sco.h"
#include "dsp_audio_msg.h"
#include "voice_plc_interface.h"
//-drivers
#include "audio_config.h"
#include "dsp_audio_process.h"
#include "dsp_memory.h"
#include "dsp_share_memory.h"
#include "dsp_temp.h"
#include "dsp_buffer.h"

#include "sfr_bt.h"
#include "timers.h"

#include "dsp_dump.h"
#include "audio_nvdm_common.h"
#include "bt_interface.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define DSP_FORWARD_BUFFER_SIZE                 (120)
#define DSP_SCO_INBAND_INFORMATION              (20)

#ifdef AIR_HFP_SYNC_STOP_ENABLE
#define ESCO_SYNC_STOP_DELAY_ACK_TIME           (2)     /* Unit of 15ms */
#endif
// for debug esco dl
#define AIR_ESCO_DL_FULL_DEBUG_ENABLE (0)

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Stream_n9sco_Config_Ptr N9SCO_setting;
U16 escoseqn;
static TimerHandle_t rx_for_timer = NULL;
static TimerHandle_t tx_for_timer = NULL;
static bool g_tx_first_mSBC_part_flag = true;
static bool g_rx_first_mSBC_part_flag = true;
static int32_t g_tx_mSBC_ev3_flag = -1;
static int32_t g_rx_mSBC_ev3_flag = -1;
static int32_t g_sco_pkt_type = -1;
static int32_t g_sco_frame_per_pkt_num = -1;
static int32_t g_sco_frame_id = -1;
static int32_t g_rx_fwd_irq_time = -1;
static int32_t g_pattern_framesize = -1;
static U32 prev_status = -1;
static uint32_t g_esco_afe_previous_writeoffset;
static bool g_esco_ul_stream_ready_check;
static bool g_esco_dl_ul_process_active;
static bool g_esco_ul_stream_ready_check;
#ifdef AIR_HFP_SYNC_START_ENABLE
static uint32_t g_esco_controller_sync_rg;
#endif
#ifdef AIR_HFP_SYNC_STOP_ENABLE
static int32_t g_esco_dl_sync_stop_detect;
#endif

////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef AIR_HFP_SYNC_STOP_ENABLE
extern void dsp_sync_callback_hfp(dsp_sync_request_action_id_t request_action_id, void *user_data);
#endif
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);
VOID N9ScoRx_update_from_share_information(SOURCE source);
VOID N9ScoRx_update_readoffset_share_information( SOURCE source,U32 ReadOffset);
VOID N9ScoRx_update_writeoffset_share_information(SOURCE source,U32 WriteOffset);
VOID N9ScoTx_update_from_share_information(SINK sink);
VOID N9ScoTx_update_from_share_information_forwarder(SINK sink);
VOID N9ScoTx_update_readoffset_share_information(SINK sink,U32 ReadOffset);
VOID N9ScoTx_update_writeoffset_share_information(SINK sink,U32 WriteOffset);
EXTERN bool afe_audio_device_ready(SOURCE_TYPE source_type,SINK_TYPE sink_type);

#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"

#define SW_GAIN_MUTE_VALUE (-(96 * 100))

sw_gain_port_t *g_call_sw_gain_port = NULL;
volatile int32_t g_call_target_gain;

void Call_UL_SW_Gain_Init(SINK sink)
{
    uint32_t channel_number;
    sw_gain_config_t default_config;
    DSP_STREAMING_PARA_PTR ul_stream;

    default_config.resolution = RESOLUTION_16BIT;
    default_config.target_gain = g_call_target_gain;
    default_config.up_step = 1;
    default_config.up_samples_per_step = 2;
    default_config.down_step = -1;
    default_config.down_samples_per_step = 2;
    if (g_call_sw_gain_port != NULL)
    {
        DSP_MW_LOG_E("Call mode sw gain init without deinit", 0);
        configASSERT(0);
    }
    g_call_sw_gain_port = stream_function_sw_gain_get_port(sink);

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));
    stream_function_sw_gain_init(g_call_sw_gain_port, channel_number, &default_config);
}

void Call_UL_SW_Gain_Deinit(void)
{
    stream_function_sw_gain_deinit(g_call_sw_gain_port);
    g_call_sw_gain_port = NULL;
}

void Call_UL_SW_Gain_Set(int32_t target_gain)
{
    uint32_t i, channel_number;
    DSP_STREAMING_PARA_PTR ul_stream;
    volatile SINK sink;

    g_call_target_gain = target_gain; 

    if (g_call_sw_gain_port == NULL) {
        return;
    }
    sink = g_call_sw_gain_port->owner;
    if ((sink == NULL) || (sink->transform == NULL)) {
        return;
    }

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));
    for (i = 0; i < channel_number; i++) {
        stream_function_sw_gain_configure_gain_target(g_call_sw_gain_port, 1 + i, g_call_target_gain);
    }
}

void Call_UL_SW_Gain_Mute_control(bool mute)
{
    uint32_t i, channel_number;
    DSP_STREAMING_PARA_PTR ul_stream;
    volatile SINK sink;

    if (g_call_sw_gain_port == NULL) {
        return;
    }
    sink = g_call_sw_gain_port->owner;
    if ((sink == NULL) || (sink->transform == NULL)) {
        return;
    }

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));
    for (i = 0; i < channel_number; i++) {
        if (mute == true) {
            stream_function_sw_gain_configure_gain_target(g_call_sw_gain_port, 1 + i, SW_GAIN_MUTE_VALUE);
        } else {
            stream_function_sw_gain_configure_gain_target(g_call_sw_gain_port, 1 + i, g_call_target_gain);
        }
    }
    DSP_MW_LOG_E("Mute flow success", 0);
}
#endif

#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
#include "src_fixed_ratio_interface.h"

static src_fixed_ratio_port_t *g_esco_ul_src_fixed_ratio_port;

static uint32_t internal_fs_converter(stream_samplerate_t fs)
{
    switch(fs) {
        case FS_RATE_44_1K:
            return 44100;

        case FS_RATE_8K:
        case FS_RATE_16K:
        case FS_RATE_24K:
        case FS_RATE_32K:
        case FS_RATE_48K:
            return fs * 1000;

        default:
            DSP_MW_LOG_E("[eSCO] sample rate is not supported!", 0);
            OS_ASSERT(FALSE);
            return fs;
    }
}

void SCO_UL_Fix_Sample_Rate_Init(void)
{
    uint32_t channel_number;
    DSP_STREAMING_PARA_PTR ul_stream;
    src_fixed_ratio_config_t smp_config;
    volatile SINK sink = Sink_blks[SINK_TYPE_N9SCO];

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));

    smp_config.channel_number = channel_number;
    smp_config.in_sampling_rate = internal_fs_converter((stream_samplerate_t)(ul_stream->callback.EntryPara.in_sampling_rate));
    smp_config.out_sampling_rate = internal_fs_converter((stream_samplerate_t)(ul_stream->callback.EntryPara.codec_out_sampling_rate));
    smp_config.resolution = ul_stream->callback.EntryPara.resolution.feature_res;
    smp_config.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
    smp_config.cvt_num = 1;
    g_esco_ul_src_fixed_ratio_port = stream_function_src_fixed_ratio_get_port(sink);
    stream_function_src_fixed_ratio_init(g_esco_ul_src_fixed_ratio_port, &smp_config);

    DSP_MW_LOG_I("[eSCO] src_fixed_ratio_init: channel_number %d, in_sampling_rate %d, out_sampling_rate %d, resolution %d", 4,
                smp_config.channel_number, smp_config.in_sampling_rate, smp_config.out_sampling_rate, smp_config.resolution);
}

void SCO_UL_Fix_Sample_Rate_Deinit(void)
{
    if (g_esco_ul_src_fixed_ratio_port) {
        stream_function_src_fixed_ratio_deinit(g_esco_ul_src_fixed_ratio_port);
        g_esco_ul_src_fixed_ratio_port = NULL;
    }
}
#endif

/**
 * Source_Sco_Audio_Fwd_Ctrl
 *
 * Function to enable/disable Audio Forwarder.
 *
 * param :ctrl - enable/disable Audio Forwarder.
 *
 */
hal_nvic_status_t Sco_Audio_Fwd_Ctrl(fowarder_ctrl forwarder_en, fowarder_type forwarder_type)
{
    hal_nvic_status_t ret = HAL_NVIC_STATUS_OK;

    if(forwarder_type == RX_FORWARDER) {
        g_rx_first_mSBC_part_flag = true;
        g_rx_mSBC_ev3_flag = -1;
        g_sco_pkt_type = SCO_PKT_NULL;
        g_sco_frame_per_pkt_num = 1;
        g_sco_frame_id = 1;
        g_rx_fwd_irq_time = 0;
        prev_status = -1;
        if (forwarder_en == ENABLE_FORWARDER){
            hal_nvic_disable_irq(BT_AURX_IRQn);
            ret = hal_nvic_register_isr_handler(BT_AURX_IRQn, (hal_nvic_isr_t)Sco_RX_IntrHandler);
            if (ret != HAL_NVIC_STATUS_OK) {
                DSP_MW_LOG_W("[Rx FWD] registerd callback handler fail!", 0);
                return ret;
            }
            ret = hal_nvic_enable_irq(BT_AURX_IRQn);
            SCO_Rx_Intr_Ctrl(FALSE);
            SCO_Rx_Reset_Disconnect_Status();
            DSP_MW_LOG_I("[Rx FWD] registerd callback handler done!", 0);
        } else {
            ret = hal_nvic_disable_irq(BT_AURX_IRQn);
            if (ret != HAL_NVIC_STATUS_OK) {
                DSP_MW_LOG_W("[Rx FWD] un-registerd callback handler fail!", 0);
                return ret;
            }
            SCO_Rx_Intr_Ctrl(FALSE);
            DSP_MW_LOG_I("[Rx FWD] un-registerd callback handler done!", 0);
        }
    } else if(forwarder_type == TX_FORWARDER) {
        g_tx_first_mSBC_part_flag = true;
        g_tx_mSBC_ev3_flag = -1;
        if (forwarder_en == ENABLE_FORWARDER) {
            hal_nvic_disable_irq(BT_AUTX_IRQn);
            ret = hal_nvic_register_isr_handler(BT_AUTX_IRQn, (hal_nvic_isr_t)Sco_TX_IntrHandler);
            if (ret != HAL_NVIC_STATUS_OK) {
                DSP_MW_LOG_W("[Tx FWD] registerd callback handler fail!", 0);
                return ret;
            }
            ret = hal_nvic_enable_irq(BT_AUTX_IRQn);
            SCO_Tx_Intr_Ctrl(FALSE);
            DSP_MW_LOG_I("[Tx FWD] registerd callback handler done!", 0);

        } else {
            ret = hal_nvic_disable_irq(BT_AUTX_IRQn);
            if (ret != HAL_NVIC_STATUS_OK) {
                DSP_MW_LOG_W("[Tx FWD] un-registerd callback handler fail!", 0);
                return ret;
            }
            SCO_Tx_Intr_Ctrl(FALSE);
            SCO_Tx_Buf_Ctrl(FALSE);
            DSP_MW_LOG_I("[Tx FWD] un-registerd callback handler done!", 0);
        }
    } else {
        DSP_MW_LOG_W("No this kind of forwarder!", 0);
        ret = HAL_NVIC_STATUS_ERROR;
    }

    return ret;
}

static void rx_forwarder_timer_callback(TimerHandle_t xTimer)
{
    UNUSED(xTimer);
    DSP_MW_LOG_I("Rx IRQ Handler", 0);
    MCE_LatchSrcTiming();
}

static void tx_forwarder_timer_callback(TimerHandle_t xTimer)
{
    UNUSED(xTimer);
    DSP_MW_LOG_I("Tx IRQ Handler", 0);
    MCE_LatchSrcTiming();
}

void Forwarder_IRQ_init(BOOL isRx)
{
    DSP_MW_LOG_I("Forwarder Init", 0);
    rBb->rClkCtl.rNativeClkCtl = 1;
    MCE_LatchSrcTiming();

    if(isRx == true)
    {
        if(rx_for_timer == NULL)
        {
            rx_for_timer = xTimerCreate("RX_FORWARDER_TIMER", pdMS_TO_TICKS(1000), pdTRUE, 0, rx_forwarder_timer_callback);
            if(!rx_for_timer) {
                DSP_MW_LOG_I("rx forwarder create timer FAIL", 0);
            } else {
                DSP_MW_LOG_I("rx forwarder create timer PASS", 0);
                xTimerStart(rx_for_timer, 0);
            }
        }

    } else {
        if(tx_for_timer == NULL)
        {
            tx_for_timer = xTimerCreate("RX_FORWARDER_TIMER", pdMS_TO_TICKS(1000), pdTRUE, 0, tx_forwarder_timer_callback);
            if(!tx_for_timer) {
                DSP_MW_LOG_I("tx forwarder create timer FAIL", 0);
            } else {
                DSP_MW_LOG_I("tx forwarder create timer PASS", 0);
                xTimerStart(tx_for_timer, 0);
            }
        }
    }
}


U16 Get_RX_FWD_Pattern_Size(VOID)
{
    U16 pattern_framesize; /*mSBC:decode in size; CVSD:decode out size*/

    if((gDspAlgParameter.EscoMode.Rx == mSBC) && (rBb->rAudioCtl.rRxAirMode == 0x3)){
        if((rBb->rAudioCtl.rRxDataLen != 60) && (rBb->rAudioCtl.rRxDataLen != 30)) {
            DSP_MW_LOG_I("[RX FWD] mSBC strange rRxDataLen:%d", 1, rBb->rAudioCtl.rRxDataLen);
            return 0;
        } else {
            pattern_framesize = rBb->rAudioCtl.rRxDataLen;
            //DSP_MW_LOG_I("[RX FWD] mSBC pattern_framesize:%d", 1, pattern_framesize);
        }
    } else if ((gDspAlgParameter.EscoMode.Rx == CVSD) && (rBb->rAudioCtl.rRxAirMode == 0x2)) {
        if((rBb->rAudioCtl.rRxDataLen != 60) && (rBb->rAudioCtl.rRxDataLen != 30) && (rBb->rAudioCtl.rRxDataLen != 20) && (rBb->rAudioCtl.rRxDataLen != 10)) {
            DSP_MW_LOG_I("[RX FWD] CVSD strange rRxDataLen:%d", 1, rBb->rAudioCtl.rRxDataLen);
            return 0;
        } else {
            pattern_framesize = rBb->rAudioCtl.rRxDataLen*2;
            //DSP_MW_LOG_I("[RX FWD] CVSD pattern_framesize:%d", 1, pattern_framesize);
        }
    } else {
        DSP_MW_LOG_I("[RX FWD] Codec type error, host codec:%d, controller codec: %d", 2, gDspAlgParameter.EscoMode.Rx, rBb->rAudioCtl.rRxAirMode);
        return 0;
    }

    return pattern_framesize;
}

U16 Get_TX_FWD_Pattern_Size(VOID)
{
    U16 pattern_framesize; /*mSBC:decode in size; CVSD:decode out size*/

    if((gDspAlgParameter.EscoMode.Rx == mSBC) && (rBb->rAudioCtl.rTxAirMode == 0x3)){
        if((rBb->rAudioCtl.rTxDataLen != 60) && (rBb->rAudioCtl.rRxDataLen != 30)) {
            DSP_MW_LOG_I("[TX FWD] mSBC strange rRxDataLen:%d", 1, rBb->rAudioCtl.rTxDataLen);
            return 0;
        } else {
            pattern_framesize = rBb->rAudioCtl.rRxDataLen;
        }
    } else if ((gDspAlgParameter.EscoMode.Rx == CVSD) && (rBb->rAudioCtl.rTxAirMode == 0x2)) {
        if((rBb->rAudioCtl.rTxDataLen != 120) && (rBb->rAudioCtl.rTxDataLen != 60) && (rBb->rAudioCtl.rTxDataLen != 30) && (rBb->rAudioCtl.rTxDataLen != 20)) {
            DSP_MW_LOG_I("[TX FWD] CVSD strange rRxDataLen:%d", 1, rBb->rAudioCtl.rTxDataLen);
            return 0;
        } else {
            pattern_framesize = rBb->rAudioCtl.rTxDataLen*2;
        }
    } else {
        DSP_MW_LOG_I("[TX FWD] Codec type error, host codec:%d, controller codec: %d", 2, gDspAlgParameter.EscoMode.Rx, rBb->rAudioCtl.rTxAirMode);
        return 0;
    }

    return pattern_framesize;
}

VOID Get_RX_FWD_Pkt_Type(VOID)
{
    U16 RxDataLen = rBb->rAudioCtl.rRxDataLen; /* decode in data len*/

    switch(RxDataLen)
    {
        case SCO_PKT_2EV3_LEN:
            g_sco_pkt_type = SCO_PKT_2EV3;
            g_sco_frame_per_pkt_num = 1;
            break;
        case SCO_PKT_EV3_LEN:
            g_sco_pkt_type = SCO_PKT_EV3;
            g_sco_frame_per_pkt_num = 2;
            break;
        case SCO_PKT_HV2_LEN:
            g_sco_pkt_type = SCO_PKT_HV2;
            g_sco_frame_per_pkt_num = 3;
            break;
        case SCO_PKT_HV1_LEN:
            g_sco_pkt_type = SCO_PKT_HV1;
            g_sco_frame_per_pkt_num = 6;
            break;
        default:
            g_sco_pkt_type = SCO_PKT_2EV3;
            g_sco_frame_per_pkt_num = 1;
            return;
    }
}

static volatile bool g_debug_forwarder_assert_flag;
bool g_ignore_next_drop_flag = false; // avoid double drop in dav task
static volatile uint32_t g_rx_fwd_running_count;
extern bool ClkSkewMode_g;
/**
 * Sco_RX_IntrHandler
 *
 * Function ISR when Rx done.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID Sco_RX_IntrHandler(VOID)
{
    VOICE_RX_SINGLE_PKT_STRU_PTR_t inbaud_info_1, inbaud_info_2;
    volatile SOURCE source = Source_blks[SOURCE_TYPE_N9SCO];
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    AUDIO_PARAMETER *sink_param = &sink->param.audio;
    N9SCO_PARAMETER *src_param = &source->param.n9sco;
    U16 pattern_framesize; /*mSBC:decode in size; CVSD:decode out size*/
    /* self-recover from deviation of L&R */
    uint32_t package_max     = 0;
    uint32_t package_num_w   = 0;
    uint32_t package_num_r   = 0;
    uint32_t differ_value    = 0;
    uint32_t avm_buf_next_ro = 0;
    uint32_t esco_pcm_length = 0;
    uint32_t differ_afe      = 0;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    DSP_STREAMING_PARA_PTR ul_stream = NULL;
    volatile SOURCE ul_source = Source_blks[SOURCE_TYPE_AUDIO];
    volatile SINK ul_sink = Sink_blks[SINK_TYPE_N9SCO];

    if (g_debug_forwarder_assert_flag == true) {
        assert(0);
        return;
    }

    /* If disconnect is detected, need to disable Rx forwader IRQ and indicate all frame as lost package */
    if (SCO_Rx_Check_Disconnect_Status() == true) {
        DSP_MW_LOG_W("[RX FWD] detect controller disconnect", 0);
        g_debug_forwarder_assert_flag = true;
        SCO_Rx_Reset_Disconnect_Status();
        SCO_Rx_Intr_HW_Handler();
        hal_nvic_disable_irq(BT_AURX_IRQn);
        SCO_Rx_Intr_Ctrl(false);
        SCO_Tx_Intr_HW_Handler();
        hal_nvic_disable_irq(BT_AUTX_IRQn);
        SCO_Tx_Intr_Ctrl(false);
        // note: mute volume in order to avoid pop noise
        hal_ccni_message_t msg;
        msg.ccni_message[0] = 0;//For DL1
        msg.ccni_message[1] = (0xD120)|(HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX<<16); //-120dB
        DSP_GC_SetOutputVolume(msg, NULL);
        DSP_MW_LOG_W("[RX FWD] disconnect, set the volume(MIN) to avoid pop noise in DL1", 0);
        return;
    }
    /* Wait Rx stream init done */
    callback_ptr = DSP_Callback_Get(source, sink);
    if (callback_ptr == NULL) {
        DSP_MW_LOG_W("[RX FWD] warning callback empty", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
    if ((src_param->rx_forwarder_en == TRUE) && (callback_ptr->Status != CALLBACK_SUSPEND)) {
        DSP_MW_LOG_W("[RX FWD] Warning: eSCO DL Start has been delayed by dl init for a little time, Status = %d", 1, callback_ptr->Status);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
    /* Wait Tx stream init done */
    if ((ul_source == NULL) || (ul_sink == NULL)) {
        DSP_MW_LOG_W("[RX FWD] warning ul source or sink empty", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
    callback_ptr = DSP_Callback_Get(ul_source, ul_sink);
    if (callback_ptr == NULL) {
        DSP_MW_LOG_W("[RX FWD] warning ul callback empty", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
    ul_stream = DSP_Streaming_Get(ul_source, ul_sink);
    if ((g_esco_ul_stream_ready_check == false) && (ul_stream->streamingStatus != STREAMING_START) && (callback_ptr->Status != CALLBACK_SUSPEND)) {
        DSP_MW_LOG_W("[RX FWD] Warning: eSCO DL Start has been delayed by ul init for a little time, Status = %d", 1, callback_ptr->Status);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
    if((g_esco_ul_stream_ready_check == false) && !afe_audio_device_ready(SOURCE_TYPE_N9SCO,SINK_TYPE_AUDIO)){
        DSP_MW_LOG_W("[RX FWD] warning afe device not ready",0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }

    g_esco_ul_stream_ready_check = true;

#ifdef AIR_HFP_SYNC_START_ENABLE
    /* Notice Controller the DSP eSCO flow is ready now */
    if (g_esco_controller_sync_rg != rBb->_reserved_dword_904h[1]) {
        DSP_MW_LOG_I("[RX FWD] update _reserved_dword_904h[1] 0x%08x", 1, rBb->_reserved_dword_904h[1]);
        g_esco_controller_sync_rg = rBb->_reserved_dword_904h[1];
    }

    SCO_Set_DSP_Ready_Status();

    /* Wait for the sync start event from the controller side */
    if (SCO_Check_Sync_Start_Status() == false) {
        DSP_MW_LOG_I("[RX FWD] wait sync start flag", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }
#endif

    if ((source == NULL) || (sink == NULL) || (source->transform == NULL) || (sink->transform == NULL)) {
        DSP_MW_LOG_I("[RX FWD] source or sink == NULL || source->transform or sink->transform == NULL", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    } else {
        N9ScoRx_update_from_share_information(source);
    }

    /* Check rx fwd irq interval */
    U32 rx_forwarder_gpt_time;
    U32 interval_gpt_time = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &rx_forwarder_gpt_time);
    if (g_rx_fwd_irq_time != 0) {
        hal_gpt_get_duration_count(g_rx_fwd_irq_time, rx_forwarder_gpt_time, &interval_gpt_time);
        uint32_t spec_interval = (rBb->rAudioCtl.rRxDataLen / 10) * 1250; // 10 -> 1.25ms, 20 -> 2.5ms, 30 -> 3.75ms, 60 -> 7.5ms
        if (interval_gpt_time < 0x10) {
            DSP_MW_LOG_W("[RX FWD] interval too close: %d, %d", 2, g_rx_fwd_irq_time, rx_forwarder_gpt_time);
            g_rx_fwd_irq_time = rx_forwarder_gpt_time;
            SCO_Rx_Intr_HW_Handler();
            return;
        } else if ((interval_gpt_time > (spec_interval + 500)) || (interval_gpt_time < (spec_interval - 500))) {
            DSP_MW_LOG_W("[RX FWD] interval is not stable in [%d]us: %d", 2, spec_interval, rx_forwarder_gpt_time-g_rx_fwd_irq_time);
            g_rx_fwd_irq_time = rx_forwarder_gpt_time;
        } else {
            g_rx_fwd_irq_time = rx_forwarder_gpt_time;
        }
    } else {
        g_rx_fwd_irq_time = rx_forwarder_gpt_time;
    }

    /* Check afe dl irq status*/
    if ( (sink_param->irq_exist == false) && (src_param->rx_forwarder_en == false) ) {
        DSP_MW_LOG_I("[RX FWD] irq not exist, rx_forwarder_time:%d, nclk_now:0x%x, [PlayEn_RG] nclk:0x%x, intraclk:0x%x, 0x%x", 5, rx_forwarder_gpt_time, rBb->rClkCtl.rNativeClock, AFE_READ(0xB0000204), AFE_READ(0xB0000208), AFE_READ(0xB0000200) );
    }

    /* Get Forwarder Parttern FrameSize */
    pattern_framesize = Get_RX_FWD_Pattern_Size();
    if(pattern_framesize == 0){
        DSP_MW_LOG_I("[RX FWD] pattern_framesize == 0", 0);
        SCO_Rx_Intr_HW_Handler();
        return;
    }

    /* Get Packety Type */
    if(g_sco_pkt_type == SCO_PKT_NULL ){
        Get_RX_FWD_Pkt_Type();
        g_pattern_framesize = pattern_framesize;
        DSP_MW_LOG_I("[RX FWD] g_sco_pkt_type == %d", 1, g_sco_pkt_type);
    }

    U8* fd_packet_ptr; //inlcude inband info + pattern
    U8* fd_pattern_ptr; // include only pattern
    U8* avm_buf_ptr; // put pattern
    U8* avm_info_ptr; // put info
    U32 avm_buf_len = source->streamBuffer.AVMBufferInfo.MemBlkSize*source->streamBuffer.AVMBufferInfo.MemBlkNum;
    U32 avm_buf_next_wo;
    U32 first_irq_avm_buf_wo = source->streamBuffer.AVMBufferInfo.MemBlkSize*N9SCO_setting->N9Sco_source.Process_Frame_Num;

    fd_packet_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.ForwarderAddr + SCO_Rx_Status()*(DSP_FORWARD_BUFFER_SIZE + DSP_SCO_INBAND_INFORMATION));
    fd_pattern_ptr = (U8 *)(fd_packet_ptr + DSP_SCO_INBAND_INFORMATION);
    avm_buf_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.StartAddr + (U32)source->streamBuffer.AVMBufferInfo.WriteIndex);
    avm_info_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.StartAddr + avm_buf_len + DSP_SCO_INBAND_INFORMATION*(((U32)source->streamBuffer.AVMBufferInfo.WriteIndex)/(pattern_framesize*g_sco_pkt_type)) );

    //*** Memcpy Inband Info ***//
    if(g_sco_pkt_type != SCO_PKT_2EV3) {
        if(g_sco_frame_id == 1) {
            DSP_D2C_BufferCopy(avm_info_ptr, fd_packet_ptr, DSP_SCO_INBAND_INFORMATION, (VOID *)(source->streamBuffer.AVMBufferInfo.StartAddr+avm_buf_len), DSP_SCO_INBAND_INFORMATION*source->streamBuffer.AVMBufferInfo.MemBlkNum);
        } else {
            inbaud_info_1 = (VOICE_RX_SINGLE_PKT_STRU_PTR_t)avm_info_ptr;
            inbaud_info_2 = (VOICE_RX_SINGLE_PKT_STRU_PTR_t)fd_packet_ptr;
            inbaud_info_1->InbandInfo.CrcErr |= inbaud_info_2->InbandInfo.CrcErr;
            inbaud_info_1->InbandInfo.HecErr |= inbaud_info_2->InbandInfo.HecErr;
            inbaud_info_1->InbandInfo.RxEd &= inbaud_info_2->InbandInfo.RxEd;
        }
        g_sco_frame_id++;
        if(g_sco_frame_id > g_sco_frame_per_pkt_num) {
            g_sco_frame_id = 1;
        }
    } else {
        DSP_D2C_BufferCopy(avm_info_ptr, fd_packet_ptr, DSP_SCO_INBAND_INFORMATION, (VOID *)(source->streamBuffer.AVMBufferInfo.StartAddr+avm_buf_len), DSP_SCO_INBAND_INFORMATION*source->streamBuffer.AVMBufferInfo.MemBlkNum);
    }
    g_rx_fwd_running_count++; // coumpute rx_fwd running times
    //*** Memcpy Pattern ***//
    DSP_D2C_BufferCopy(avm_buf_ptr, fd_pattern_ptr, pattern_framesize, (VOID *)source->streamBuffer.AVMBufferInfo.StartAddr, (U16)avm_buf_len);
    //DSP_MW_LOG_I("[RX FWD] rx_status:%d, RxED:%d, HEC:%d, CRC:%d, data: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x",10, SCO_Rx_Status(), ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)fd_packet_ptr)->InbandInfo.RxEd, ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)fd_packet_ptr)->InbandInfo.HecErr, ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)fd_packet_ptr)->InbandInfo.CrcErr,*(fd_pattern_ptr+1),*(fd_pattern_ptr+2),*(fd_pattern_ptr+3),*(fd_pattern_ptr+4),*(fd_pattern_ptr+5),*(fd_pattern_ptr+6));


    //*** For pkt lost debug: Accumulate Lost Pkt Num ***//
#ifdef DEBUG_HFP_PLK_LOST
    src_param->forwarder_pkt_num ++;
    if(Voice_PLC_CheckInfoValid((VOICE_RX_SINGLE_PKT_STRU_PTR_t)avm_info_ptr) == FALSE){
        src_param->lost_pkt_num++;
    }
    if(src_param->forwarder_pkt_num == 200){
        DSP_MW_LOG_I("lost packet: %d per %d pkt",1,src_param->lost_pkt_num, src_param->forwarder_pkt_num);
        src_param->lost_pkt_num = 0;
        src_param->forwarder_pkt_num = 0;
    }
#endif

    //*** Clean Fwd Pattern & Info ***//
    memset(fd_pattern_ptr, 0, pattern_framesize);
    Voice_PLC_CleanInfo((VOICE_RX_SINGLE_PKT_STRU_PTR_t)fd_packet_ptr);


    //*** Update AVM buffer writeoffset ***//
    avm_buf_next_wo = ((U32)source->streamBuffer.AVMBufferInfo.WriteIndex + (U32)pattern_framesize)%avm_buf_len;
    if(prev_status == SCO_Rx_Status()) {
        DSP_MW_LOG_I("[RX FWD] prev status:%d == now status:%d", 2, prev_status, SCO_Rx_Status());
        SCO_Rx_Intr_HW_Handler();
        prev_status = SCO_Rx_Status();
        return;
    } else {
        if(avm_buf_next_wo == source->streamBuffer.AVMBufferInfo.ReadIndex)
        {
            DSP_MW_LOG_I("[RX FWD] WO == RO %d %d, status:%d", 3, avm_buf_next_wo, source->streamBuffer.AVMBufferInfo.ReadIndex, SCO_Rx_Status());
            //U32 ro = (source->streamBuffer.AVMBufferInfo.ReadIndex + pattern_framesize)%avm_buf_len;
            //N9ScoRx_update_readoffset_share_information(source, ro);
        } else {
            //DSP_MW_LOG_I("[RX FWD] WO != RO %d %d, status:%d", 3, avm_buf_next_wo, source->streamBuffer.AVMBufferInfo.ReadIndex, SCO_Rx_Status());
        }
        N9ScoRx_update_writeoffset_share_information(source, avm_buf_next_wo);
        /* Check the position offset of WPTR/RPTR for both AVM buffer and AFE buffer */
        package_num_w = avm_buf_next_wo / pattern_framesize; /* Write package number */
        if (g_rx_fwd_running_count % (uint32_t)(g_sco_pkt_type * 2) == 0) {
            /* Check and fix the WPTR of sink AFE buffer */
            if (g_esco_afe_previous_writeoffset != 0xFFFFFFFF) {
                buffer_info = &sink->streamBuffer.BufferInfo;
                if (buffer_info->WriteOffset >= g_esco_afe_previous_writeoffset) {
                    differ_afe = buffer_info->WriteOffset - g_esco_afe_previous_writeoffset;
                } else {
                    differ_afe = buffer_info->WriteOffset + buffer_info->length - g_esco_afe_previous_writeoffset;
                }
                /* Need to consider the impact of clock skew process, which may increase or decrease the esco_pcm_length */
                if (sink->param.audio.AfeBlkControl.u4asrcflag) {
                    esco_pcm_length = sink->param.audio.channel_num * sink->param.audio.src_rate * sink->param.audio.period * sink->param.audio.format_bytes / 1000;
                } else {
                    esco_pcm_length = sink->param.audio.channel_num * sink->param.audio.rate * sink->param.audio.period * sink->param.audio.format_bytes / 1000;
                }
                if ((differ_afe >= (esco_pcm_length + 100)) || (differ_afe <= (esco_pcm_length - 100))) {
                    buffer_info->WriteOffset = (g_esco_afe_previous_writeoffset + esco_pcm_length) % buffer_info->length;
                    // enable src1 to avoid hwsrc underflow
#ifdef ENABLE_HWSRC_CLKSKEW
                    if (ClkSkewMode_g == CLK_SKEW_V2) {
                        if (afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK)== false) {
                            afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, true);
                            DSP_MW_LOG_I("[RX FWD] enable src1 to avoid HWSRC UNDERFLOW", 0);
                        }
                    } else {
                        hal_audio_set_value_parameter_t set_value_parameter;
                        set_value_parameter.set_current_offset.pure_agent_with_src = sink->param.audio.mem_handle.pure_agent_with_src;
                        set_value_parameter.set_current_offset.memory_select = sink->param.audio.mem_handle.memory_select;
                        set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
                        hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);
                    }
#else
                    hal_audio_set_value_parameter_t set_value_parameter;
                    set_value_parameter.set_current_offset.pure_agent_with_src = sink->param.audio.mem_handle.pure_agent_with_src;
                    set_value_parameter.set_current_offset.memory_select = sink->param.audio.mem_handle.memory_select;
                    set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];//(uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
                    hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);
#endif
                    DSP_MW_LOG_I("[RX FWD] sink AFE buffer wptr fixed", 0);
                    g_ignore_next_drop_flag = true;
                }
                /* update pre wo */
                g_esco_afe_previous_writeoffset = buffer_info->WriteOffset;
            }
            /* Check and fix the RPTR of source AVM buffer */
            package_max   = avm_buf_len / pattern_framesize;
            package_num_r = source->streamBuffer.AVMBufferInfo.ReadIndex / pattern_framesize; // Read package number
            if (package_num_w <= package_num_r) {
                differ_value = package_max - package_num_r + package_num_w;
            } else {
                differ_value = package_num_w - package_num_r;
            }
            if (differ_value != N9SCO_setting->N9Sco_source.Process_Frame_Num) {
                avm_buf_next_ro = ((avm_buf_next_wo + avm_buf_len) - N9SCO_setting->N9Sco_source.Process_Frame_Num * pattern_framesize) % avm_buf_len;
                N9ScoRx_update_readoffset_share_information(source, avm_buf_next_ro);
                DSP_MW_LOG_I("[RX FWD] source AVM buffer rptr fixed", 0);
            }
        #if AIR_ESCO_DL_FULL_DEBUG_ENABLE
            DSP_MW_LOG_I("[RX FWD] [Error fix] WO [%d] RO[%d], Fix RO[%d], differ [%d], buffer_wo[%d] differ_afe[%d] esco_pcm_length[%d]", 7, avm_buf_next_wo, package_num_r * pattern_framesize,
                                        source->param.n9sco.share_info_base_addr->ReadIndex, differ_value, buffer_info->WriteOffset, differ_afe, esco_pcm_length);
        #endif
        }
    }
    prev_status = SCO_Rx_Status();


    //*** Trigger 1st DL AFE IRQ ***//
    if ( (sink_param->irq_exist == FALSE) && (src_param->rx_forwarder_en == TRUE) && (avm_buf_next_wo == first_irq_avm_buf_wo))
    {
        if (source->transform->Handler != NULL)
        {
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
            hal_gpio_toggle_pin(HAL_GPIO_23);
#endif
            /* Save the initial write offset of AFE sink buffer here when trigger DL stream process */
            g_esco_afe_previous_writeoffset = buffer_info->WriteOffset;

            DSP_MW_LOG_I("[RX FWD] AirMode:%d, PktDataLen:%d", 2, rBb->rAudioCtl.rRxAirMode, rBb->rAudioCtl.rRxDataLen);
            U32 play_en_bt_clk = SCO_Rx_AncClk(); // next rx fwd anchor time

            U32 play_en_nclk;
            U16 play_en_intra_clk;
            MCE_TransBT2NativeClk(play_en_bt_clk, 1250, &play_en_nclk, &play_en_intra_clk,BT_CLK_Offset);
            hal_audio_afe_set_play_en(play_en_nclk, (U32)play_en_intra_clk);
            DSP_MW_LOG_I("[RX FWD] PhsOffset:0x%x, ClkOffset:0x%x, nclk_now:0x%x, [PlayEn_RG] play_en_nclk: 0x%x, play_en_intra: 0x%x", 5, rBb->rAudioCtl.rRxPhsOffset, rBb->rAudioCtl.rRxClkOffset, rBb->rClkCtl.rNativeClock, play_en_nclk, play_en_intra_clk);
            DSP_MW_LOG_I("[RX FWD] rPicoClock = 0x%x, bt_clk = 0x%x", 2, rBb->rClkCtl.rPicoClock, play_en_bt_clk);
            //*** Clean FWD IRQ ***//
            //SCO_Rx_Intr_HW_Handler();
            src_param->rx_forwarder_en = FALSE;

            U32 rx_first_forwarder_time;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &rx_first_forwarder_time);
            DSP_MW_LOG_I("[RX FWD] First Rx Forwarder IRQ resume DAVT, gpt_time: %d, nclk_now:0x%x, [PlayEn_RG] nclk:0x%x, intraclk:0x%x, 0x%x", 5, rx_first_forwarder_time, rBb->rClkCtl.rNativeClock, AFE_READ(0xB0000204), AFE_READ(0xB0000208), AFE_READ(0xB0000200));
            source->transform->Handler(source,sink);
            xTaskResumeFromISR((TaskHandle_t)DAV_TASK_ID);
            portYIELD_FROM_ISR(pdTRUE); // Force to do context switch, or task may not be trigger.
        }
    }else {
        //*** Clean FWD IRQ ***//
        //SCO_Rx_Intr_HW_Handler();
    }
    SCO_Rx_Intr_HW_Handler();
#if AIR_ESCO_DL_FULL_DEBUG_ENABLE
    DSP_MW_LOG_I("[RX FWD] AFE WO = [%d] RO = [%d] g_rx_fwd_running_count %d", 3, buffer_info->WriteOffset, buffer_info->ReadOffset, g_rx_fwd_running_count);
#endif
}

/**
 * Sco_TX_IntrHandler
 *
 * Function ISR when Rx done.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID Sco_TX_IntrHandler(VOID)
{
    volatile SINK sink = Sink_blks[SINK_TYPE_N9SCO];
    U16 pattern_framesize;  /*mSBC:encode out size; CVSD:encode in size*/

    if (sink == NULL)
    {
        DSP_MW_LOG_I("[TX FWD] sink == NULL", 0);
        SCO_Tx_Intr_HW_Handler();
        return;
    }
    N9ScoTx_update_from_share_information_forwarder(sink);

    if(sink->param.n9sco.tx_forwarder_en == TRUE)
    {
        U32 tx_first_forwarder_time;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tx_first_forwarder_time);
        DSP_MW_LOG_I("[TX FWD] First Tx forwarder, gpt_time: %d, rTxDataLen: %d", 2, tx_first_forwarder_time, rBb->rAudioCtl.rTxDataLen);
        sink->param.n9sco.tx_forwarder_en = FALSE;
    }

    /* Get Forwarder Parttern FrameSize */
    pattern_framesize = Get_RX_FWD_Pattern_Size();
    if(pattern_framesize == 0){
        DSP_MW_LOG_I("[TX FWD] pattern_framesize == 0", 0);
        SCO_Tx_Intr_HW_Handler();
        return;
    }

    U8* fd_packet_ptr; //forwarder pattern ptr
    U8* avm_buf_ptr; // avm pattern
    U32 avm_buf_len = (sink->streamBuffer.AVMBufferInfo.MemBlkSize)*(sink->streamBuffer.AVMBufferInfo.MemBlkNum);
    U32 avm_buf_next_ro;

    fd_packet_ptr = (U8 *)(sink->streamBuffer.AVMBufferInfo.ForwarderAddr + SCO_Tx_Status()*DSP_FORWARD_BUFFER_SIZE);
    avm_buf_ptr = (U8 *)(sink->streamBuffer.AVMBufferInfo.StartAddr + (U32)sink->streamBuffer.AVMBufferInfo.ReadIndex);
    if(sink->streamBuffer.AVMBufferInfo.ReadIndex == sink->streamBuffer.AVMBufferInfo.WriteIndex) {
        DSP_MW_LOG_I("[TX FWD] RO == WO, %d %d", 2, sink->streamBuffer.AVMBufferInfo.ReadIndex, sink->streamBuffer.AVMBufferInfo.WriteIndex);
        SCO_Tx_Intr_HW_Handler();
        return;
    }
    memcpy(fd_packet_ptr, avm_buf_ptr, pattern_framesize);

    SCO_Tx_Intr_HW_Handler();

    avm_buf_next_ro = ((U32)sink->streamBuffer.AVMBufferInfo.ReadIndex + (U32)pattern_framesize)%avm_buf_len;
    N9ScoTx_update_readoffset_share_information(sink, avm_buf_next_ro);

}

/**
 * SCO_Rx_Intr_HW_Handler
 *
 * Function Rx HW interrupt Handler.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Rx_Intr_HW_Handler(VOID)
{
    rBb->rAuRxIntFlag = 1;
    rBb->rAuRxIntFlag = 1;
    rBb->rAuRxIntFlag = 1;
}

VOID SCO_Tx_Intr_HW_Handler(VOID)
{
    rBb->rAuTxIntFlag = 1;
}

/**
 * SCO_Rx_Intr_Ctrl
 *
 * Function to report Rx status.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
U32 SCO_Rx_Status (void)
{
    return rBb->rAudioCtl.rRxDstAddrSelCurrSw;
}

U32 SCO_Tx_Status (void)
{
    return rBb->rAudioCtl.rTxSrcAddrSelNxtSw;
}

bool SCO_Rx_Check_Disconnect_Status(void)
{
    if ((rBb->_reserved_dword_904h[1] & (1 << 0)) != 0) {
        return true;
    } else {
        return false;
    }
}

void SCO_Rx_Reset_Disconnect_Status(void)
{
    rBb->_reserved_dword_904h[1] &= ~(1 << 0);
}

#ifdef AIR_HFP_SYNC_START_ENABLE
bool SCO_Check_Sync_Start_Status(VOID)
{
    if ((rBb->_reserved_dword_904h[1] & (1 << 2)) != 0) {
        return true;
    } else {
        return false;
    }
}

VOID SCO_Set_DSP_Ready_Status(VOID)
{
    rBb->_reserved_dword_904h[1] |= (1 << 1);
}
#endif

#ifdef AIR_HFP_SYNC_STOP_ENABLE
bool SCO_Check_Sync_Stop_Status(VOID)
{
    if ((rBb->_reserved_dword_904h[1] & (1 << 3)) != 0) {
        return true;
    } else {
        return false;
    }
}

VOID SCO_Reset_Sync_Stop_Status(VOID)
{
    rBb->_reserved_dword_904h[1] &= ~(1 << 3);
}
#endif

U32 SCO_Rx_AncClk (void)
{
    return rBb->rAudioCtl.rRxAncClk; //unit:312.5us
}

U32 SCO_RX_FWD_IntrTime(void)
{
    return (rBb->rAudioCtl.rRxIntrTmr)/625; //rRxIntrTmer unit:0.5us, return unit:312.5us
}

ATTR_TEXT_IN_IRAM VOID N9ScoRx_update_from_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(source->streamBuffer.AVMBufferInfo), source->param.n9sco.share_info_base_addr, 40);/* share info fix 40 byte */
    source->streamBuffer.AVMBufferInfo.StartAddr = hal_memview_cm4_to_dsp0(source->streamBuffer.AVMBufferInfo.StartAddr);
    source->streamBuffer.AVMBufferInfo.ForwarderAddr = hal_memview_cm4_to_dsp0(source->streamBuffer.AVMBufferInfo.ForwarderAddr);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9ScoRx_update_writeoffset_share_information(SOURCE source,U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();
    source->param.n9sco.share_info_base_addr->WriteIndex = (U16)WriteOffset;
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9ScoRx_update_readoffset_share_information( SOURCE source,U32 ReadOffset)
{
    StreamDSP_HWSemaphoreTake();
    source->param.n9sco.share_info_base_addr->ReadIndex = (U16)ReadOffset;
#ifdef PT_bBufferIsFull_ready
    source->param.n9sco.share_info_base_addr->bBufferIsFull = FALSE;
#endif
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM VOID N9ScoTx_update_from_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(sink->streamBuffer.AVMBufferInfo), sink->param.n9sco.share_info_base_addr, 40);/* share info fix 40 byte */
    sink->streamBuffer.AVMBufferInfo.StartAddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.StartAddr);
    //sink->streamBuffer.AVMBufferInfo.ForwarderAddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.ForwarderAddr);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM VOID N9ScoTx_update_from_share_information_forwarder(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(sink->streamBuffer.AVMBufferInfo), sink->param.n9sco.share_info_base_addr, 40);/* share info fix 40 byte */
    sink->streamBuffer.AVMBufferInfo.StartAddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.StartAddr);
    sink->streamBuffer.AVMBufferInfo.ForwarderAddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.ForwarderAddr);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9ScoTx_update_readoffset_share_information(SINK sink,U32 ReadOffset)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9sco.share_info_base_addr->ReadIndex = (U16)ReadOffset;
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9ScoTx_update_writeoffset_share_information(SINK sink,U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9sco.share_info_base_addr->WriteIndex = (U16)WriteOffset;
#ifdef PT_bufferfull
    if (WriteOffset == sink->param.n9sco.share_info_base_addr->ReadIndex)
    {
        sink->param.n9sco.share_info_base_addr->bBufferIsFull = 1;
    }
#endif
    StreamDSP_HWSemaphoreGive();
}
static VOID N9Sco_Reset_Sinkoffset_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9sco.share_info_base_addr->WriteIndex = 0;
    sink->param.n9sco.share_info_base_addr->ReadIndex  = 0;
#ifdef PT_bufferfull
    sink->param.n9sco.share_info_base_addr->bBufferIsFull = FALSE;
#endif
    StreamDSP_HWSemaphoreGive();
}


VOID N9SCO_Default_setting_init(VOID)
{
       if (N9SCO_setting != NULL)
       {return;}
       N9SCO_setting = pvPortMalloc(sizeof(Stream_n9sco_Config_t));//for rtos
       memset(N9SCO_setting,0,sizeof(Stream_n9sco_Config_t));

       N9SCO_setting->N9Sco_source.Process_Frame_Num       = 2;

       N9SCO_setting->N9Sco_sink.Process_Frame_Num         = 2;
       N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt        = 0;
}




/**
 * SinkSlackSco
 *
 * Function to know the remain buffer size of SCO sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 U32 SinkSlackSco(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    U32 writeOffset = sink->streamBuffer.AVMBufferInfo.WriteIndex;
    U32 readOffset  = sink->streamBuffer.AVMBufferInfo.ReadIndex;
    U32 sharebuflen = sink->streamBuffer.AVMBufferInfo.MemBlkSize * sink->streamBuffer.AVMBufferInfo.MemBlkNum;
    U32 ProcessFrameLen = (N9SCO_setting->N9Sco_source.Process_Frame_Num)*(sink->streamBuffer.AVMBufferInfo.MemBlkSize);
    U32 RemainBuf = (readOffset >= writeOffset) ?(sharebuflen + writeOffset - readOffset) : (readOffset - writeOffset - readOffset);
    //printf("SinkSlackN9Sco process_data_length : %d\r\n", sink->param.n9sco.process_data_length);
#ifdef PT_bufferfull
    if ((sink->streamBuffer.ShareBufferInfo.bBufferIsFull != 1)&&(RemainBuf >= ProcessFrameLen))
#else
    if (RemainBuf >= ProcessFrameLen)
#endif
    {
        return sink->param.n9sco.process_data_length;
    }
    else
    {
        return sink->param.n9sco.process_data_length;
    }
}


/**
 * SinkClaimSco
 *
 * Function to ask the buffer to write data into SCO sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
U32 SinkClaimSco(SINK sink, U32 extra)
{
    N9ScoTx_update_from_share_information(sink);
    U32 sharebuflen = sink->streamBuffer.AVMBufferInfo.MemBlkSize * sink->streamBuffer.AVMBufferInfo.MemBlkNum;
    U32 writeOffset = sink->streamBuffer.AVMBufferInfo.WriteIndex;
    U32 readOffset  = sink->streamBuffer.AVMBufferInfo.ReadIndex;
    U32 RemainBuf = (readOffset >= writeOffset) ? (sharebuflen - writeOffset + readOffset) : (writeOffset - readOffset);
#ifdef PT_bufferfull
    if((extra != 0)&&((sink->streamBuffer.ShareBufferInfo.bBufferIsFull != 1)&&(RemainBuf > extra)) && (sink->transform == NULL))
#else
    if((extra != 0)&&(RemainBuf > extra)&&(sink->transform == NULL))
#endif
    {
        return 0;
    }
    else
    {
        return SINK_INVALID_CLAIM;
    }
}

/**
 * SinkMapSco
 *
 * Function to read the decoded data in SCO sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
U8* SinkMapSco(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    //memcpy(MapAddr,sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.ReadOffset, sink->streamBuffer.ShareBufferInfo.length - sink->streamBuffer.ShareBufferInfo.ReadOffset);
    if (sink->streamBuffer.AVMBufferInfo.ReadIndex != 0)
    {
        memcpy(MapAddr + sink->streamBuffer.AVMBufferInfo.ReadIndex,&(sink->streamBuffer.AVMBufferInfo.StartAddr), sink->streamBuffer.AVMBufferInfo.ReadIndex);
    }

    return MapAddr;

}

/**
 * SinkFlushSco
 *
 * Function to read the decoded data in SCO sink.
 *
 * param :amount - The amount of data written into sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
*/
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SinkFlushSco(SINK sink,U32 amount)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);

    N9ScoTx_update_from_share_information(sink);
    U16 sharebuflen = sink->streamBuffer.AVMBufferInfo.MemBlkSize * sink->streamBuffer.AVMBufferInfo.MemBlkNum;
    U16 framesize = sink->streamBuffer.AVMBufferInfo.MemBlkSize;
    if ((SinkSlackSco(sink) == 0)||(amount != sink->param.n9sco.process_data_length))
    {
        DSP_MW_LOG_W("sinkflush, amount:%d != process_data_length:%d", 2, amount, sink->param.n9sco.process_data_length);
        return FALSE;
    }
    else
    {
        sink->streamBuffer.AVMBufferInfo.WriteIndex = (sink->streamBuffer.AVMBufferInfo.WriteIndex + N9SCO_setting->N9Sco_sink.Process_Frame_Num*framesize)%(sharebuflen);
    }

    #if !DL_TRIGGER_UL
    if (sink->param.n9sco.IsFirstIRQ == TRUE)
    {
        U32 gpt_timer,relative_delay;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        relative_delay = (gpt_timer - sink->param.n9sco.ul_play_gpt)%15000;
        /* send CCNI data transmit to N9 */
        if ((relative_delay < 8*1000)||(relative_delay > 12*1000)) //First flush time over 6 ms + 15 ms IRQ interval
        {
            U32 delay_time = (relative_delay < 8*1000) ? (8*1000 - relative_delay) : ((15+8)*1000 - relative_delay);
            hal_gpt_delay_us(delay_time);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
            DSP_MW_LOG_I("eSCO UL flush abnormal, delay :%d GTP_N :%d, GPT_P :%d\r\n",3, delay_time,gpt_timer,sink->param.n9sco.ul_play_gpt);
        }
        else
        {
            DSP_MW_LOG_I("eSCO UL flush time GTP_N :%d, GPT_P :%d\r\n",2, gpt_timer,sink->param.n9sco.ul_play_gpt);
        }
        #warning "Trigger N9SCO Tx Interrupt handler, Function Name:N9Sco_TX_IntrHandler"
        /* send CCNI data transmit to N9 */
        hal_ccni_message_t msg;
        memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
        msg.ccni_message[0] = MSG_DSP2N9_UL_START << 16;
        aud_msg_tx_handler(msg, 0, FALSE);
        sink->param.n9sco.IsFirstIRQ = FALSE;
    }
    #else
    /*if (((sink->streamBuffer.AVMBufferInfo.ReadIndex + N9SCO_setting->N9Sco_sink.Process_Frame_Num*N9SCO_setting->N9Sco_sink.Frame_Size)%sharebuflen) == sink->streamBuffer.AVMBufferInfo.WriteIndex)
    {
        //DSP_MW_LOG_I("eSCO UL Ro abnormal, cnt: %d %d %d",3,N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt,sink->streamBuffer.ShareBufferInfo.WriteOffset,sink->streamBuffer.ShareBufferInfo.ReadOffset);
        if(++N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt >= ESCO_UL_ERROR_DETECT_THD)
        {
            DSP_MW_LOG_I("eSCO UL trigger ro/wo error handle cnt:%d",1,N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt);
            sink->streamBuffer.AVMBufferInfo.WriteIndex = (sink->streamBuffer.AVMBufferInfo.WriteIndex + N9SCO_setting->N9Sco_sink.Frame_Size)%(sharebuflen);
        }
    }
    else
    {
        N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt = 0;
    }*/
    #endif


    N9ScoTx_update_writeoffset_share_information(sink,sink->streamBuffer.AVMBufferInfo.WriteIndex);
    //DSP_MW_LOG_I("[SinkFlushN9Sco] eSCO, wo:%d, ro:%d", 2, sink->streamBuffer.AVMBufferInfo.WriteIndex, sink->streamBuffer.AVMBufferInfo.ReadIndex);
    if (sink->param.n9sco.tx_forwarder_en == TRUE)
    {
        DSP_MW_LOG_I("[SinkFlushN9Sco] eSCO UL is first IRQ, wo:%d, ro:%d, sink->param.n9sco.tx_forwarder_en:%d", 3, sink->streamBuffer.AVMBufferInfo.WriteIndex, sink->streamBuffer.AVMBufferInfo.ReadIndex, sink->param.n9sco.tx_forwarder_en);
        SCO_Tx_Intr_HW_Handler();
        SCO_Tx_Intr_Ctrl(TRUE);
        SCO_Tx_Buf_Ctrl(TRUE);
    }
    hal_nvic_restore_interrupt_mask(mask);

    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) {
        if (g_esco_dl_ul_process_active == false) {
            audio_nvdm_update_status(AUDIO_NVDM_USER_HFP, AUDIO_NVDM_STATUS_POST_CHANGE);
            g_esco_dl_ul_process_active = true;
        }
        if ((sink->param.n9sco.ul_reinit == true) && (Source_blks[SOURCE_TYPE_N9SCO] != NULL) && (Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.dl_reinit == true)) {
            Source_blks[SOURCE_TYPE_N9SCO]->param.n9sco.dl_reinit = false;
            sink->param.n9sco.ul_reinit = false;
            audio_nvdm_update_status(AUDIO_NVDM_USER_HFP, AUDIO_NVDM_STATUS_POST_CHANGE);
        }
    }

    return TRUE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SinkBufferWriteSco (SINK sink, U8 *src_addr, U32 length)
{
    U16 i;
    U8* write_ptr;
    N9ScoTx_update_from_share_information(sink);
    U16 sharebuflen = sink->streamBuffer.AVMBufferInfo.MemBlkSize * sink->streamBuffer.AVMBufferInfo.MemBlkNum;
    U16 framesize = sink->streamBuffer.AVMBufferInfo.MemBlkSize;

    if (sink->param.n9sco.process_data_length != length)
    {
        return FALSE;
    }

    for (i = 0 ; i < N9SCO_setting->N9Sco_sink.Process_Frame_Num ; i++)
    {
        write_ptr = (U8*)(sink->streamBuffer.AVMBufferInfo.StartAddr + sink->streamBuffer.AVMBufferInfo.WriteIndex);
        memcpy(write_ptr, src_addr + (U32)(i*framesize) , framesize);
        sink->streamBuffer.AVMBufferInfo.WriteIndex = (sink->streamBuffer.AVMBufferInfo.WriteIndex + framesize)%(sharebuflen);
    }
    return TRUE;

}



/**
 * Sink_Sco_Buffer_Ctrl
 *
 * Function to enable/disable SCO buffer.
 *
 * param :ctrl - enable/disable SCO buffer.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID Sink_N9Sco_Buffer_Init(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    N9Sco_Reset_Sinkoffset_share_information(sink);

    // Prefill 1 frame in Sink Buffer
    if(gDspAlgParameter.EscoMode.Tx == mSBC)
    {
        N9ScoTx_update_writeoffset_share_information(sink, 0);
    }else if(gDspAlgParameter.EscoMode.Tx == CVSD)
    {
        N9ScoTx_update_writeoffset_share_information(sink, 0);
    }

}


/**
 * SinkCloseSco
 *
 * Function to shutdown SCO sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
BOOL SinkCloseSco(SINK sink)
{
    sink->param.n9sco.process_data_length = 0;
    Sco_Audio_Fwd_Ctrl(DISABLE_FORWARDER, TX_FORWARDER);

#ifdef AIR_SOFTWARE_GAIN_ENABLE
    Call_UL_SW_Gain_Deinit();
#endif

#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
    SCO_UL_Fix_Sample_Rate_Deinit();
#endif

    return TRUE;
}



/**
 * SinkInitSco
 *
 * Function to initialize SCO sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SinkInitN9Sco(SINK sink)
{
    /* buffer init */
    N9SCO_Default_setting_init();
    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    Sink_N9Sco_Buffer_Init(sink);
    SCO_Tx_Forwarder_Buf_Init(sink);
    Sco_Audio_Fwd_Ctrl(ENABLE_FORWARDER, TX_FORWARDER);

    sink->param.n9sco.process_data_length = N9SCO_setting->N9Sco_sink.Process_Frame_Num*(sink->streamBuffer.AVMBufferInfo.MemBlkSize);
    sink->param.n9sco.tx_forwarder_en = FALSE;
    //DSP_MW_LOG_I("eSCO UL process_data_length : %d %d %d %d\r\n", 4, sink->param.n9sco.process_data_length, N9SCO_setting->N9Sco_sink.Process_Frame_Num, sink->streamBuffer.AVMBufferInfo.MemBlkSize, sink->streamBuffer.AVMBufferInfo.MemBlkNum);

    /* interface init */
    sink->sif.SinkSlack       = SinkSlackSco;
    sink->sif.SinkClaim       = SinkClaimSco;
    sink->sif.SinkMap         = SinkMapSco;
    sink->sif.SinkFlush       = SinkFlushSco;
    sink->sif.SinkClose       = SinkCloseSco;
    sink->sif.SinkWriteBuf    = SinkBufferWriteSco;

    sink->param.n9sco.IsFirstIRQ = TRUE;
    escoseqn = 0;
    sink->param.n9sco.ul_reinit = false;
}


/**
 * SourceSizeSco
 *
 * Function to report remaining Source buffer size.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM U32 SourceSizeSco(SOURCE source)
{
    N9ScoRx_update_from_share_information(source);
    U32 writeOffset = source->streamBuffer.AVMBufferInfo.WriteIndex;
    U32 readOffset  = source->streamBuffer.AVMBufferInfo.ReadIndex;
    U32 avmbufLen = source->streamBuffer.AVMBufferInfo.MemBlkSize * source->streamBuffer.AVMBufferInfo.MemBlkNum;
    U32 processFrameLen = (N9SCO_setting->N9Sco_source.Process_Frame_Num)*source->streamBuffer.AVMBufferInfo.MemBlkSize;
    U32 dataRemainLen = (readOffset > writeOffset) ? (avmbufLen - readOffset + writeOffset) : (writeOffset - readOffset);

    if ((dataRemainLen >= processFrameLen)||(source->param.n9sco.write_offset_advance != 0))
    {
        //DSP_MW_LOG_I("SourceSize dataRemainLen: %d Ro:%d Wo:%d, avmbufLen:%d", 4,  dataRemainLen, readOffset, writeOffset, avmbufLen);
        return source->param.n9sco.process_data_length;
    }
    else
    {
        DSP_MW_LOG_I("SourceSize = 0, dataRemainLen:%d Ro:%d Wo:%d", 3, dataRemainLen, readOffset, writeOffset);
        return 0;
    }
}


/**
 * SourceMapSco
 *
 * Function to  read the received data in SCO source.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
U8* SourceMapSco(SOURCE source)
{
    N9ScoRx_update_from_share_information(source);
    //memcpy(MapAddr,source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset, source->streamBuffer.ShareBufferInfo.length - source->streamBuffer.ShareBufferInfo.ReadOffset);
    if (source->streamBuffer.AVMBufferInfo.ReadIndex != 0)
    {
        memcpy(MapAddr + source->streamBuffer.AVMBufferInfo.ReadIndex,&(source->streamBuffer.AVMBufferInfo.StartAddr), source->streamBuffer.AVMBufferInfo.ReadIndex);
    }

    return MapAddr;
}

/**
 * SourceDropSco
 *
 * Function to drop the data in SCO sink.
 *
 * param :amount - The amount of data to drop in sink.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID SourceDropSco(SOURCE source, U32 amount)
{
    U16 i;
    U8* write_ptr;

    N9ScoRx_update_from_share_information(source);
    U32 sharebuflen = source->streamBuffer.AVMBufferInfo.MemBlkSize * source->streamBuffer.AVMBufferInfo.MemBlkNum;
    U16 framesize = source->streamBuffer.AVMBufferInfo.MemBlkSize;

    if (amount != source->param.n9sco.process_data_length)
    {
        DSP_MW_LOG_I("Source Drop, false amount: %d, length:%d", 2, amount, source->param.n9sco.process_data_length);
        return;
    }
    else
    {

        for (i = 0 ; i < N9SCO_setting->N9Sco_source.Process_Frame_Num ; i++)
        {
            write_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.StartAddr + (source->streamBuffer.AVMBufferInfo.ReadIndex + i*framesize)%sharebuflen);
            //*(write_ptr + framesize - STATE_LEN) = SCO_PKT_FREE;
        }
        source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex + N9SCO_setting->N9Sco_source.Process_Frame_Num*framesize)%(sharebuflen);
        N9ScoRx_update_readoffset_share_information(source,source->streamBuffer.AVMBufferInfo.ReadIndex);

        /*if(source->streamBuffer.AVMBufferInfo.ReadIndex != source->streamBuffer.AVMBufferInfo.WriteIndex){
            printf("[DSP] Update stream buf Ro:%d, now_Wo:%d", source->streamBuffer.AVMBufferInfo.ReadIndex, source->streamBuffer.AVMBufferInfo.WriteIndex);
        }*/
    }
    source->param.n9sco.write_offset_advance = 0;


    #if DL_TRIGGER_UL
    if  (((Sink_blks[SINK_TYPE_N9SCO] != NULL) && (Sink_blks[SINK_TYPE_N9SCO]->param.n9sco.IsFirstIRQ == TRUE))&& ((source->transform != NULL)&&(source->param.n9sco.dl_enable_ul == FALSE)))
    {
        volatile SINK eSCO_sink = Sink_blks[SINK_TYPE_N9SCO];
        U32 relative_delay,delay_thd;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(eSCO_sink->param.n9sco.ul_play_gpt));
        relative_delay = ((eSCO_sink->param.n9sco.ul_play_gpt - source->param.n9sco.ul_play_gpt)*1000)>>5;
        delay_thd = ((6 + ((relative_delay>>10)/(15*4)))%15); //extend THD every 15*4 ms
        if ((((relative_delay>>10)%15) < delay_thd)&&(eSCO_sink->transform != NULL))
        {
            eSCO_sink->param.n9sco.IsFirstIRQ = FALSE;
            eSCO_sink->param.n9sco.tx_forwarder_en = TRUE;
            DSP_MW_LOG_I("eSCO UL sync dl, DL time:%d thd :%d GTP_N :%d, GPT_P :%d, tx_forwarder_en:%d\r\n", 5, relative_delay, delay_thd,(eSCO_sink->param.n9sco.ul_play_gpt),source->param.n9sco.ul_play_gpt, eSCO_sink->param.n9sco.tx_forwarder_en);
            hal_gpt_delay_us(delay_thd*1000 - ((relative_delay*1000>>10)%15000));

            hal_audio_trigger_start_parameter_t start_parameter;
            start_parameter.memory_select = eSCO_sink->transform->source->param.audio.mem_handle.memory_select;
            start_parameter.enable = true;
            DSP_MW_LOG_I("UL SET TRIGGER MEM audio.rate %d audio.count %d\r\n", 2, eSCO_sink->transform->source->param.audio.rate, eSCO_sink->transform->source->param.audio.count);
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);

            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(eSCO_sink->param.n9sco.ul_play_gpt));
            DSP_MW_LOG_I("eSCO UL start from DL drop, delay :%d GTP_N :%d",2, (delay_thd - relative_delay),eSCO_sink->param.n9sco.ul_play_gpt);
        }
        else
        {
            N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt = 0;
            SinkFlushSco(eSCO_sink,eSCO_sink->param.n9sco.process_data_length);
            DSP_MW_LOG_I("eSCO UL start from DL drop too late, delay :%d GTP_N :%d, GPT_P :%d\r\n",3, relative_delay,(eSCO_sink->param.n9sco.ul_play_gpt),source->param.n9sco.ul_play_gpt);
        }
    }
    #endif

    escoseqn += 2;
}

/**
 * SourceConfigureSco
 *
 * Function to configure SCO source.
 *
 * param :type - The configure type.
 *
 * param :value - The configure value.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
BOOL SourceConfigureSco(SOURCE source, stream_config_type type, U32 value)
{

    switch (type)
    {
        case SCO_SOURCE_WO_ADVANCE:
            source->param.n9sco.write_offset_advance = value;
            break;
        default:
            //printf("Wrong configure type");
            return FALSE;
            break;
    }

    return TRUE;
}


/**
 * SourceReadBufSco
 *
 * Function to read data from SCO source.
 *
 * param :dst_addr - The destination buffer to write data into.
 *
 * param :length -The leng of data to read.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SourceReadBufSco(SOURCE source, U8* dst_addr, U32 length)
{
    U16 i;
    U8* write_ptr;
    U8* info_ptr;

    N9ScoRx_update_from_share_information(source);
    U16 sharebuflen = source->streamBuffer.AVMBufferInfo.MemBlkSize * source->streamBuffer.AVMBufferInfo.MemBlkNum;
    U16 framesize = source->streamBuffer.AVMBufferInfo.MemBlkSize;

    if (source->param.n9sco.process_data_length != length)
    {
        return FALSE;
    }

#ifdef AIR_HFP_SYNC_STOP_ENABLE
    if (g_esco_dl_sync_stop_detect < 0) {
        if (SCO_Check_Sync_Stop_Status() == true) {
            /* If the Rx fowarder has been stopped, we need to mute the output */
            g_esco_dl_sync_stop_detect = 0;
            dsp_sync_callback_hfp(SUBMSG_MCU2DSP_SYNC_STOP, NULL);
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
            hal_gpio_toggle_pin(HAL_GPIO_23);
#endif
            DSP_MW_LOG_I("[Source Read] detect sync stop event", 0);
        }
    } else {
        if (++g_esco_dl_sync_stop_detect == ESCO_SYNC_STOP_DELAY_ACK_TIME) {
            /* Delay some time for the voice to ramp down to mute */
            SCO_Reset_Sync_Stop_Status();
            DSP_MW_LOG_I("[Source Read] ack sync stop event", 0);
        }
    }
#endif

    for (i = 0 ; i < N9SCO_setting->N9Sco_source.Process_Frame_Num ; i++)
    {
        VOICE_RX_INBAND_INFO_t RxPacketInfo;

        write_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.StartAddr + source->streamBuffer.AVMBufferInfo.ReadIndex);
        info_ptr = (U8 *)(source->streamBuffer.AVMBufferInfo.StartAddr + (U32)sharebuflen + DSP_SCO_INBAND_INFORMATION*(((U32)source->streamBuffer.AVMBufferInfo.ReadIndex)/(U32)framesize));
        RxPacketInfo = (((VOICE_RX_SINGLE_PKT_STRU_PTR_t)info_ptr)->InbandInfo);
        //DSP_MW_LOG_I("[Source Read] info_ptr RxEd:%d, HEC:%d, CRC:%d, data: 0x%x 0x%x 0x%x 0x%x 0x%x", 8, RxPacketInfo.RxEd, RxPacketInfo.HecErr, RxPacketInfo.CrcErr, *(write_ptr+1) ,*(write_ptr+2) ,*(write_ptr+3),*(write_ptr+4),*(write_ptr+5));
#ifdef AIR_HFP_SYNC_STOP_ENABLE
        /* Mark frame as packet expire status when sync stop is detect */
        if (g_esco_dl_sync_stop_detect >= 0) {
            ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)info_ptr)->InbandInfo.CrcErr = 1;
            ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)info_ptr)->InbandInfo.HecErr = 1;
            ((VOICE_RX_SINGLE_PKT_STRU_PTR_t)info_ptr)->InbandInfo.RxEd = 0;
        }
#endif
        if(Voice_PLC_CheckInfoValid((VOICE_RX_SINGLE_PKT_STRU_PTR_t)info_ptr) == FALSE)
        {
            DSP_MW_LOG_I("[Source Read] meet packet expired %d", 1, escoseqn + i);
            Voice_PLC_CheckAndFillZeroResponse((S16 *)(dst_addr + i*framesize), gDspAlgParameter.EscoMode.Rx);
        }else
        {
            memcpy(dst_addr + i*framesize, write_ptr, framesize);
        }
#ifdef AIR_HFP_SYNC_STOP_ENABLE
        /* Mark PLC status as good frame status when sync stop is detect */
        if (g_esco_dl_sync_stop_detect >= 0) {
            RxPacketInfo.CrcErr = 0;
            RxPacketInfo.HecErr = 0;
            RxPacketInfo.RxEd = 1;
        }
#endif
        Voice_PLC_UpdateInbandInfo(&RxPacketInfo,sizeof(VOICE_RX_INBAND_INFO_t),i);
        source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex + framesize)%(sharebuflen);
    }

    return TRUE;
}

/**
 * SourceCloseSco
 *
 * Function to shutdown SCO source.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
BOOL SourceCloseSco(SOURCE source)
{
    source->param.n9sco.process_data_length = 0;
    Sco_Audio_Fwd_Ctrl(DISABLE_FORWARDER, RX_FORWARDER);
    return TRUE;
}

/**
 * SourceInitSco
 *
 * Function to initialize SCO source.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SourceInitN9Sco(SOURCE source)
{
    /* buffer init */
    N9SCO_Default_setting_init();
    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    Source_N9Sco_Buffer_Init(source);
    Sco_Audio_Fwd_Ctrl(ENABLE_FORWARDER, RX_FORWARDER); // register rx forwarder irq handler

    source->param.n9sco.process_data_length = (N9SCO_setting->N9Sco_source.Process_Frame_Num)*(source->streamBuffer.AVMBufferInfo.MemBlkSize);
    //DSP_MW_LOG_I("source->param.n9sco.process_data_length:%d %d %d %d\r\n", 4, source->param.n9sco.process_data_length, (N9SCO_setting->N9Sco_source.Process_Frame_Num)*(source->streamBuffer.AVMBufferInfo.MemBlkSize), (source->streamBuffer.AVMBufferInfo.MemBlkSize), (source->streamBuffer.AVMBufferInfo.MemBlkNum));


    /* interface init */
    source->sif.SourceSize        = SourceSizeSco;
    source->sif.SourceReadBuf     = SourceReadBufSco;
    source->sif.SourceMap         = SourceMapSco;
    source->sif.SourceConfigure   = SourceConfigureSco;
    source->sif.SourceDrop        = SourceDropSco;
    source->sif.SourceClose       = SourceCloseSco;

    /* Enable Interrupt */
    source->param.n9sco.IsFirstIRQ = TRUE;
    source->param.n9sco.dl_enable_ul = TRUE;
    source->param.n9sco.write_offset_advance = 0;
#ifdef DEBUG_HFP_PLK_LOST
    source->param.n9sco.lost_pkt_num = 0;
    source->param.n9sco.forwarder_pkt_num = 0;
#endif

    g_debug_forwarder_assert_flag = false;
    g_esco_afe_previous_writeoffset = 0xFFFFFFFF;
    g_esco_ul_stream_ready_check = false;
#ifdef AIR_HFP_SYNC_START_ENABLE
    g_esco_controller_sync_rg = 0xFFFFFFFF;
#endif
#ifdef AIR_HFP_SYNC_STOP_ENABLE
    g_esco_dl_sync_stop_detect = -1;
#endif
    g_ignore_next_drop_flag = false;
    g_rx_fwd_running_count = 0;
    source->param.n9sco.dl_reinit = false;
    g_esco_dl_ul_process_active = false;
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
    hal_gpio_init(HAL_GPIO_23);
    hal_pinmux_set_function(HAL_GPIO_23, 0);
    hal_gpio_set_direction(HAL_GPIO_23, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_23, HAL_GPIO_DATA_LOW);
#endif
}


/**
 * Source_Sco_Buffer_Ctrl
 *
 * Function to enable/disable SCO buffer.
 *
 * param :ctrl - enable/disable SCO buffer.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID Source_N9Sco_Buffer_Init(SOURCE source)
{
    N9ScoRx_update_from_share_information(source);
    N9ScoRx_update_readoffset_share_information(source,0);
    N9ScoRx_update_writeoffset_share_information(source,0);
}


/**
 * SCO_Rx_Intr_Ctrl
 *
 * Function to enable/disable Rx interrupt.
 *
 * param :ctrl - enable/disable Audio Forwarder.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Rx_Intr_Ctrl(BOOL ctrl)
{
    rBb->rAuRxIntMask = ctrl;
}

/**
 * SCO_Rx_Buf_Ctrl
 *
 * Function to enable/disable Rx interrupt.
 *
 * param :ctrl - enable/disable Audio Forwarder.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Rx_Buf_Ctrl(BOOL ctrl)
{
    rBb->rAudioCtl.rRxBufRdy = ctrl;
}


/**
 * SCO_Tx_HW_Init
 *
 * Function to initialize SCO HW.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Tx_Forwarder_Buf_Init(SINK sink)
{
    memset((void *)hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.ForwarderAddr), 0 , DSP_FORWARD_BUFFER_SIZE*2);

    //SCO_Tx_Buf_Ctrl(TRUE);
}


/**
 * SCO_Tx_Intr_Ctrl
 *
 * Function to enable/disable SCO Tx interrupt.
 *
 * param :ctrl - enable/disable SCO Tx interrupt.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Tx_Intr_Ctrl(BOOL ctrl)
{
    rBb->rAuTxIntMask = ctrl;
}

/**
 * SCO_Tx_Buf_Ctrl
 *
 * Function to enable/disable SCO Tx interrupt.
 *
 * param :ctrl - enable/disable SCO Tx interrupt.
 *
 * @Author : MachiWu <MachiWu@airoha.com.tw>
 */
VOID SCO_Tx_Buf_Ctrl(BOOL ctrl)
{
    rBb->rAudioCtl.rTxBufRdy = ctrl;
}


