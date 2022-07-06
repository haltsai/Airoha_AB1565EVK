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


/* Includes ------------------------------------------------------------------*/

#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#include "hal_audio_cm4_dsp_message.h"
#include "syslog.h"
#ifdef MTK_AUDIO_DUMP_ENABLE
#include "audio_dump.h"
#endif
#include "usbaudio_drv.h"
#include "usb_audio_playback.h"
#include "usb_audio_control_internal.h"
#include "usb_audio_control.h"
#include "usb_audio_device.h"

log_create_module(usb_audio, PRINT_LEVEL_INFO);
#define USB_PLAYBACK_USE_MSGID_LOG
#ifdef USB_PLAYBACK_USE_MSGID_LOG
#define USB_PLAYBACK_LOG_E(fmt,arg...)   LOG_MSGID_E(usb_audio, "USB_PLAYBACK: "fmt,##arg)
#define USB_PLAYBACK_LOG_W(fmt,arg...)   LOG_MSGID_W(usb_audio, "USB_PLAYBACK: "fmt,##arg)
#define USB_PLAYBACK_LOG_I(fmt,arg...)   LOG_MSGID_I(usb_audio ,"USB_PLAYBACK: "fmt,##arg)
#else
#define USB_PLAYBACK_LOG_E(fmt,arg...)   LOG_E(usb_audio, "USB_PLAYBACK: "fmt,##arg)
#define USB_PLAYBACK_LOG_W(fmt,arg...)   LOG_W(usb_audio, "USB_PLAYBACK: "fmt,##arg)
#define USB_PLAYBACK_LOG_I(fmt,arg...)   LOG_I(usb_audio ,"USB_PLAYBACK: "fmt,##arg)
#endif

static uint8_t s_data_request_flag = false;

static usb_audio_playback_state_t usb_audio_status = USB_AUDIO_STATE_IDLE;
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
static void usb_audio_rx_cb(void);
#endif

#define MAX_VOL_LEVEL 16

//DG:-55~-15
//AG:4
uint32_t volume_gain_table[MAX_VOL_LEVEL][2] = {
    {0xFFFFFC18, 0x044C},
    {0xFFFFFCE0, 0x044C},
    {0xFFFFFDA8, 0x044C},
    {0xFFFFFE70, 0x044C},
    {0xFFFFFF38, 0x044C},
    {0x00000000, 0x044C},
    {0x00000000, 0x0384},
    {0xFFFFF768, 0x0000},
    {0xFFFFF8F8, 0x0000},
    {0xFFFFFA88, 0x0000},
    {0xFFFFFC18, 0x0000},
    {0xFFFFFDA8, 0x0000},
    {0xFFFFFF38, 0x0000},
    {0x00000000, 0x00C8},
    {0x00000000, 0x00C8},
    {0x00000000, 0x00C8}
};


static void usb_audio_isr_handler(hal_audio_event_t event, void *data)
{
    switch (event) {
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]usb_audio_isr_handler", 0);
            s_data_request_flag = true;
            #if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            usb_audio_rx_cb();
            #endif
            s_data_request_flag = false;
            break;

        case HAL_AUDIO_EVENT_END:
            break;
    }
}

static usb_audio_result_t usb_audio_playback_open(void)
{
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PLAYBACK;
    mcu2dsp_audio_msg_t open_msg = MSG_MCU2DSP_PLAYBACK_OPEN;
    void *p_param_share;

    bool is_running = hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK);
    if (is_running) {
        // Reentry: don't allow multiple playback
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]Re-entry\r\n", 0);
    } else {
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, true);
    }

    mcu2dsp_open_param_t open_param;
    memset(&open_param, 0, sizeof(open_param));

    open_param.param.stream_in  = STREAM_IN_PLAYBACK;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_16;
    open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
    open_param.stream_in_param.playback.channel_number = HAL_AUDIO_STEREO;
    open_param.stream_in_param.playback.codec_type = 0;  //KH: should use AUDIO_DSP_CODEC_TYPE_PCM
    open_param.stream_in_param.playback.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);
    hal_audio_reset_share_info(open_param.stream_in_param.playback.p_share_info);
    open_param.stream_in_param.playback.p_share_info->length = SHARE_BUFFER_BT_AUDIO_DL_SIZE;
    memset((void *)open_param.stream_in_param.playback.p_share_info->start_addr, 0, open_param.stream_in_param.playback.p_share_info->length);
    open_param.stream_in_param.playback.p_share_info->bBufferIsFull = true;

    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
    open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
    open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_out_param.afe.sampling_rate   = 48000;
    open_param.stream_out_param.afe.irq_period      = 10;
    open_param.stream_out_param.afe.frame_size      = 512;
    open_param.stream_out_param.afe.frame_number    = 4;
    open_param.stream_out_param.afe.hw_gain         = true;
    hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
    open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), msg_type);

#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_USB_AUDIO_PLAYBACK_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PLAYBACK, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, TRUE);
    hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);
    hal_audio_register_stream_out_callback(usb_audio_isr_handler, NULL);

    usb_audio_status = USB_AUDIO_STATE_READY;

    return USB_AUDIO_EXECUTION_SUCCESS;
}

static usb_audio_result_t usb_audio_playback_start(void)
{
    mcu2dsp_start_param_t start_param = {{0}};
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PLAYBACK;
    mcu2dsp_audio_msg_t start_msg = MSG_MCU2DSP_PLAYBACK_START;

    start_param.param.stream_in     = STREAM_IN_PLAYBACK;
    start_param.param.stream_out    = STREAM_OUT_AFE;
    start_param.stream_out_param.afe.aws_flag   =  false;
    void *p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_start_param_t), msg_type);
    hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);

    usb_audio_status = USB_AUDIO_STATE_PLAY;

    return USB_AUDIO_EXECUTION_SUCCESS;
}

static usb_audio_result_t usb_audio_playback_stop(void)
{
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PLAYBACK;
    mcu2dsp_audio_msg_t stop_msg = MSG_MCU2DSP_PLAYBACK_STOP;

    hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
    hal_audio_service_unhook_callback(msg_type);

    usb_audio_status = USB_AUDIO_STATE_STOP;

    return USB_AUDIO_EXECUTION_SUCCESS;
}

static usb_audio_result_t usb_audio_playback_close(void)
{
    mcu2dsp_audio_msg_t close_msg = MSG_MCU2DSP_PLAYBACK_CLOSE;

    hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Close i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_USB_AUDIO_PLAYBACK_OUT, 0, 0, 0, false);
#endif
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PLAYBACK, 0, 0, false);
    if (hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK)) {
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, false);
    }

    usb_audio_status = USB_AUDIO_STATE_IDLE;

    return USB_AUDIO_EXECUTION_SUCCESS;
}

#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
static void usb_audio_rx_cb(void)
{
    static uint32_t previous_count;
    uint32_t gpt_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    if((gpt_count - previous_count) > 2000) {
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]usb_audio_rx_cb duration = %d", 1, gpt_count - previous_count);
    }

    previous_count = gpt_count;

    //USB_PLAYBACK_LOG_I("[USB_PLAYBACK_CB]22_usb_audio_rx_cb", 0);
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PLAYBACK;
    n9_dsp_share_info_t *p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);

    for (uint32_t i = 0; i < 2; i++) {
        uint32_t source_data_count = USB_Audio_Rx_Buffer_Get_Bytes(0);
        void *p_source_buf = USB_Audio_Rx_Buffer_Get_Read_Address(0);
        if (s_data_request_flag == true) {
            USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]00_source_data_count = 0x%08x", 1, USB_Audio_Get_Len_Received_Data(0));
        }
        if (source_data_count == 0) {
            break;
        }
#if 1
        uint32_t free_space_count = hal_audio_buf_mgm_get_free_byte_count(p_dsp_info);
        if (s_data_request_flag == true) {
            USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]free_space_count = 0x%08x", 1, free_space_count);
        }
        if (source_data_count > free_space_count) {
            source_data_count = free_space_count;
            USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]share buffer full!!free_space_count = 0x%08x", 1, free_space_count);
        }
        //USB_PLAYBACK_LOG_I("[USB_PLAYBACK_DEBUG]11_source_data_count = 0x%08x", 1, source_data_count);
        hal_audio_write_stream_out_by_type(msg_type, p_source_buf, source_data_count);
        //LOG_AUDIO_DUMP(p_source_buf, source_data_count, VOICE_TX_MIC_3);
#else
        LOG_AUDIO_DUMP(p_source_buf, source_data_count, VOICE_TX_MIC_3);
#endif
        USB_Audio_Rx_Buffer_Drop_Bytes(0, source_data_count);
    }
}
#endif

usb_audio_result_t audio_usb_audio_playback_open(void)
{
    if (usb_audio_status != USB_AUDIO_STATE_IDLE) {
        USB_PLAYBACK_LOG_E("[USB_PLAYBACK_CTL]Open error: USB should be idle before open\r\n", 0);
        return USB_AUDIO_EXECUTION_FAIL;
    } else {
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_CTL]Open success\r\n", 0);
    }
    return usb_audio_playback_open();
}

usb_audio_result_t audio_usb_audio_playback_start(void)
{
    if (usb_audio_status != USB_AUDIO_STATE_READY) {
        USB_PLAYBACK_LOG_E("[USB_PLAYBACK_CTL]Start error: USB should be ready before start\r\n", 0);
        return USB_AUDIO_EXECUTION_FAIL;
    } else {
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_CTL]Start success\r\n", 0);
    }
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
    USB_Audio_Register_Rx_Callback(0, usb_audio_rx_cb);
#endif
    return usb_audio_playback_start();
}

usb_audio_result_t audio_usb_audio_playback_stop(void)
{
    if (usb_audio_status != USB_AUDIO_STATE_PLAY) {
        USB_PLAYBACK_LOG_E("[USB_PLAYBACK_CTL]Stop error: USB should be play before stop\r\n", 0);
        return USB_AUDIO_EXECUTION_FAIL;
    } else {
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_CTL]Stop success\r\n", 0);
    }
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
    USB_Audio_Register_Rx_Callback(0, NULL);
#endif
    return usb_audio_playback_stop();
}

usb_audio_result_t audio_usb_audio_playback_close(void)
{
    if (usb_audio_status != USB_AUDIO_STATE_STOP) {
        USB_PLAYBACK_LOG_E("[USB_PLAYBACK_CTL]Close error: USB should be stop before close\r\n", 0);
        return USB_AUDIO_EXECUTION_FAIL;
    } else {
        USB_PLAYBACK_LOG_I("[USB_PLAYBACK_CTL]Close success\r\n", 0);
    }
    return usb_audio_playback_close();
}

