/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "scenario_ble_audio.h"
#include "audio_transmitter_playback_port.h"
#include "usbaudio_drv.h"
#include "bt_sink_srv_ami.h"

/* Private define ------------------------------------------------------------*/
#define USB_RX_PORT_TOTAL 2

#define DONGLE_PAYLOAD_SIZE_USB_RX_PCM 192    //1ms for 48K/16bit/Stereo
#define DONGLE_PAYLOAD_SIZE_ENCODED 155 //10ms/124Kbps for 48K/16bit/Mono/LC3

#define USB_TX_PORT_TOTAL 1

#define DONGLE_PAYLOAD_SIZE_USB_TX_PCM_MAX 192    //1ms for 48K/16bit/Stereo
#define DONGLE_PAYLOAD_SIZE_USB_TX_PCM 128    //1ms for 32K/16bit/Stereo
#define DONGLE_PAYLOAD_SIZE_DECODED 80 //10ms/64Kbps for 32K/16bit/Mono/LC3

#define DONGLE_USB_TX_SEND_CCNI_FRAMES 3

#define BLE_AUDIO_DONGLE_DEBUG_LANTENCY             1
#define BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG       0
#define BLE_AUDIO_DONGLE_VOICE_PATH_DEBUG_LOG       0

#define BT_AVM_SHARE_BUFFER_SIZE            (5*1024)

#define GAIN_COMPENSATION_STEP 10

/* Private typedef -----------------------------------------------------------*/
typedef struct  {
    U16 DataOffset; /* offset of payload */
    U16 _reserved_word_02h;
    U32 TimeStamp; /* this CIS/BIS link's CLK, Unit:312.5us */
    U16 ConnEvtCnt; /* event count seem on airlog, for debug propose */
    U8 SampleSeq;  /* Sameple sequence of this SDU interval Ex:0,1,2... */
    U8 _reserved_byte_0Bh; /* valid packet: 0x01, invalid packet 0x00 */
    U8 PduHdrLo;
    U8 _reserved_byte_0Dh;
    U8 PduLen ; /* payload size */
    U8 _reserved_byte_0Fh;
    U16 DataLen;
    U16 _reserved_word_12h;
    U32 _reserved_long_0;
    U32 _reserved_long_1;
} LE_AUDIO_HEADER;

typedef struct
{
#if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
    uint16_t latency_debug_enable;
    int16_t latency_debug_last_sample;
    uint16_t latency_debug_last_level;
#endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
    uint8_t usb_first_in_flag;
    uint8_t stream_is_started;
    uint32_t previous_gpt_count;
    audio_transmitter_block_header_t usb_stream_header;
    uint32_t period;
    uint32_t frame_size;
    audio_dsp_codec_type_t usb_type;
    audio_codec_param_t usb_param;
} ble_audio_dongle_usb_handle_t;

typedef struct {
    uint32_t vol_gain_1;
    uint32_t vol_gain_2;
} vol_gain_t;

typedef struct {
    uint8_t scenario_type;
    uint8_t scenario_sub_id;
} dl_mixer_param_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static ble_audio_dongle_usb_handle_t usb_stream_rx_handle[USB_RX_PORT_TOTAL];
static ble_audio_dongle_usb_handle_t usb_stream_tx_handle[USB_TX_PORT_TOTAL];
static uint8_t all_zero_buffer[DONGLE_PAYLOAD_SIZE_USB_TX_PCM_MAX];
static uint32_t dl_stream_status = 0;
static uint32_t ul_stream_status = 0;
const static int16_t gain_compensation_table[GAIN_COMPENSATION_STEP + 1] =
{
    /*
    Ratio |    db    | Compensation
    0%    |  -60db   | 0xE890
    10%   |  -20db   | 0xF830
    20%   | -13.98db | 0xFA8B
    30%   | -10.46db | 0xFBEB
    40%   |  -7.96db | 0xFCE5
    50%   |  -6.02db | 0xFDA6
    60%   |  -4.44db | 0xFE45
    70%   |  -3.1db  | 0xFECB
    80%   |  -1.94db | 0xFF3F
    90%   |  -0.92db | 0xFFA5
    100%  |     0db  | 0
    */
    0xE890,
    0xF830,
    0xFA8B,
    0xFBEB,
    0xFCE5,
    0xFDA6,
    0xFE45,
    0xFECB,
    0xFF3F,
    0xFFA5,
    0x0
};

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint32_t usb_audio_get_frame_size(audio_dsp_codec_type_t *usb_type, audio_codec_param_t *usb_param)
{
    uint32_t frame_size = 0;
    uint32_t samples;
    uint32_t channel_num;
    uint32_t resolution_size;

    if (*usb_type == AUDIO_DSP_CODEC_TYPE_PCM)
    {
        frame_size = 1;

        switch (usb_param->pcm.sample_rate)
        {
            case 44100:
                samples = 44;
                break;

            case 16000:
            case 32000:
            case 48000:
                samples = usb_param->pcm.sample_rate/1000;
                break;

            default:
                configASSERT(0);
                break;
        }

        switch (usb_param->pcm.channel_mode)
        {
            case 1:
                channel_num = 1;
                break;

            case 2:
                channel_num = 2;
                break;

            default:
                configASSERT(0);
                break;
        }

        switch (usb_param->pcm.format)
        {
            case AFE_PCM_FORMAT_S16_LE:
                resolution_size = 2;
                break;

            default:
                configASSERT(0);
                break;
        }
    }
    else
    {
        frame_size = 0;
    }

    frame_size = frame_size * samples * channel_num * resolution_size;

    return frame_size;
}

static uint32_t ble_audio_codec_get_frame_size(audio_dsp_codec_type_t *codec_type, audio_codec_param_t *codec_param)
{
    uint32_t frame_size = 0;

    if (*codec_type != AUDIO_DSP_CODEC_TYPE_LC3)
    {
        configASSERT(0);
    }

    frame_size = codec_param->lc3.bit_rate * codec_param->lc3.frame_interval / 8 / 1000 / 1000;

    return frame_size;
}

static void usb_audio_rx_cb_ble_audio_dongle_0(void)
{
    uint32_t gpt_count, duration_count;
    uint32_t available_data_size;
    n9_dsp_share_info_t *p_dsp_info;
    uint32_t buf_size;
    uint8_t *p_source_buf;

    /* get current gpt count */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
    TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]usb_audio_rx_cb_ble_audio_dongle_0 callback = %u", 1, gpt_count);
    #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */

    /* get share buffer info */
    p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_0);

    if (usb_stream_rx_handle[0].usb_first_in_flag == 0)
    {
        /* this is the first irq, we need to drop all relict USB data */
        available_data_size = USB_Audio_Get_Len_Received_Data(0);
        USB_Audio_Rx_Buffer_Drop_Bytes(0, available_data_size);
        usb_stream_rx_handle[0].usb_first_in_flag = 1;
        usb_stream_rx_handle[0].previous_gpt_count = gpt_count;

        TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]usb_audio_rx_cb_ble_audio_dongle_0 callback first time = %u", 1, gpt_count);

        #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
        TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]share buffer addr = 0x%x", 1, p_dsp_info);
        #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */
    }
    else
    {
        /* this is not the first irq, we need to copy usb into share buffer */
        hal_gpt_get_duration_count(usb_stream_rx_handle[0].previous_gpt_count, gpt_count, &duration_count);
        if ((duration_count > 1500) || (duration_count < 500))
        {
            TRANSMITTER_LOG_E("[BLE Audio Dongle][USB_RX_DEBUG 0]usb_audio_rx_cb_ble_audio_dongle_0 duration = %d", 1, duration_count);
        }
        usb_stream_rx_handle[0].previous_gpt_count = gpt_count;

        /* get usb data size */
        available_data_size = USB_Audio_Get_Len_Received_Data(0);
        if (available_data_size > usb_stream_rx_handle[0].frame_size)
        {
            log_hal_msgid_info("[BLE Audio Dongle][USB_RX_DEBUG 0]Too much data in USB buffer %d\r\n", 1, available_data_size);
        }

        /* copy usb data into share buffer block one by one */
        while (available_data_size >= usb_stream_rx_handle[0].frame_size)
        {
            /* get share buffer block info */
            hal_audio_buf_mgm_get_free_buffer(p_dsp_info, &p_source_buf, &buf_size);
            if (buf_size < (usb_stream_rx_handle[0].frame_size + BLK_HEADER_SIZE))
            {
                TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]Not enough share buffer space", 0);
                // configASSERT(0);
                break;
            }

            /* update usb data header */
            usb_stream_rx_handle[0].usb_stream_header.sequence_number++;
            usb_stream_rx_handle[0].usb_stream_header.data_length = usb_stream_rx_handle[0].frame_size;

            /* write usb data into share buffer block */
            // memcpy(p_source_buf, &usb_stream_rx_handle[0].usb_stream_header, BLK_HEADER_SIZE);
            ((audio_transmitter_block_header_t *)p_source_buf)->sequence_number = usb_stream_rx_handle[0].usb_stream_header.sequence_number;
            ((audio_transmitter_block_header_t *)p_source_buf)->data_length     = usb_stream_rx_handle[0].usb_stream_header.data_length;
            USB_Audio_Read_Data(0, p_source_buf + BLK_HEADER_SIZE, usb_stream_rx_handle[0].frame_size);
            hal_audio_buf_mgm_get_write_data_done(p_dsp_info, usb_stream_rx_handle[0].frame_size + BLK_HEADER_SIZE);

            #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]r_offset = %u, w_offset = %u", 2, p_dsp_info->read_offset, p_dsp_info->write_offset);
            #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */

            //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, usb_stream_rx_handle[0].frame_size, 13);

            /* get residual usb data size */
            available_data_size = USB_Audio_Get_Len_Received_Data(0);

#if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            int16_t *start_address = NULL;
            uint32_t current_level = 0;
            uint32_t i;

            if (usb_stream_rx_handle[0].latency_debug_enable)
            {
                current_level = usb_stream_rx_handle[0].latency_debug_last_level;
                start_address = (int16_t *)(p_source_buf + BLK_HEADER_SIZE);
                if ((*(start_address + 2 * 0) > usb_stream_rx_handle[0].latency_debug_last_sample) &&
                    (*(start_address + 2 * 0) - usb_stream_rx_handle[0].latency_debug_last_sample > 40000))
                {
                    current_level = 1;
                }
                else if ((*(start_address + 2 * 0) < usb_stream_rx_handle[0].latency_debug_last_sample) &&
                         (usb_stream_rx_handle[0].latency_debug_last_sample - *(start_address + 2 * 0) > 40000))
                {
                    current_level = 0;
                }
                for (i = 0; i < usb_stream_rx_handle[0].frame_size / 4 - 1; i++)
                {
                    if ((*(start_address + 2 * i) > *(start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * i) - *(start_address + 2 * (i + 1)) > 40000))
                    {
                        current_level = 0;
                        break;
                    }
                    else if ((*(start_address + 2 * i) < *(start_address + 2 * (i + 1))) &&
                             (*(start_address + 2 * (i + 1)) - *(start_address + 2 * i) > 40000))
                    {
                        current_level = 1;
                        break;
                    }
                    else
                    {
                    }
                }
                usb_stream_rx_handle[0].latency_debug_last_sample = *(start_address + 2 * (usb_stream_rx_handle[0].frame_size / 4 - 1));
                if (current_level != usb_stream_rx_handle[0].latency_debug_last_level)
                {
                    hal_gpio_set_output(HAL_GPIO_13, current_level);
                    usb_stream_rx_handle[0].latency_debug_last_level = current_level;
                }
            }
#endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
        }
    }
}

static void usb_audio_rx_cb_ble_audio_dongle_1(void)
{
    uint32_t gpt_count, duration_count;
    uint32_t available_data_size;
    n9_dsp_share_info_t *p_dsp_info;
    uint32_t buf_size;
    uint8_t *p_source_buf;

    /* get current gpt count */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
    TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]usb_audio_rx_cb_ble_audio_dongle_1 callback = %u", 1, gpt_count);
    #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */

    /* get share buffer info */
    p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_1);

    if (usb_stream_rx_handle[1].usb_first_in_flag == 0)
    {
        /* this is the first irq, we need to drop all relict USB data */
        available_data_size = USB_Audio_Get_Len_Received_Data(1);
        USB_Audio_Rx_Buffer_Drop_Bytes(1, available_data_size);
        usb_stream_rx_handle[1].usb_first_in_flag = 1;
        usb_stream_rx_handle[1].previous_gpt_count = gpt_count;

        TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]usb_audio_rx_cb_ble_audio_dongle_1 callback first time = %u", 1, gpt_count);

        #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
        TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]share buffer addr = 0x%x", 1, p_dsp_info);
        #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */
    }
    else
    {
        /* this is not the first irq, we need to copy usb into share buffer */
        hal_gpt_get_duration_count(usb_stream_rx_handle[1].previous_gpt_count, gpt_count, &duration_count);
        if ((duration_count > 1500) || (duration_count < 500))
        {
            TRANSMITTER_LOG_E("[BLE Audio Dongle][USB_RX_DEBUG 1]usb_audio_rx_cb_ble_audio_dongle_1 duration = %d", 1, duration_count);
        }
        usb_stream_rx_handle[1].previous_gpt_count = gpt_count;

        /* get usb data size */
        available_data_size = USB_Audio_Get_Len_Received_Data(1);
        if (available_data_size > usb_stream_rx_handle[1].frame_size)
        {
            log_hal_msgid_info("[BLE Audio Dongle][USB_RX_DEBUG 1]Too much data in USB buffer %d\r\n", 1, available_data_size);
        }

        /* copy usb data into share buffer block one by one */
        while (available_data_size >= usb_stream_rx_handle[1].frame_size)
        {
            /* get share buffer block info */
            hal_audio_buf_mgm_get_free_buffer(p_dsp_info, &p_source_buf, &buf_size);
            if (buf_size < (usb_stream_rx_handle[1].frame_size + BLK_HEADER_SIZE))
            {
                TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]Not enough share buffer space", 0);
                // configASSERT(0);
                break;
            }

            /* update usb data header */
            usb_stream_rx_handle[1].usb_stream_header.sequence_number++;
            usb_stream_rx_handle[1].usb_stream_header.data_length = usb_stream_rx_handle[1].frame_size;

            /* write usb data into share buffer block */
            // memcpy(p_source_buf, &usb_stream_rx_handle[1].usb_stream_header, BLK_HEADER_SIZE);
            ((audio_transmitter_block_header_t *)p_source_buf)->sequence_number = usb_stream_rx_handle[1].usb_stream_header.sequence_number;
            ((audio_transmitter_block_header_t *)p_source_buf)->data_length     = usb_stream_rx_handle[1].usb_stream_header.data_length;
            USB_Audio_Read_Data(1, p_source_buf + BLK_HEADER_SIZE, usb_stream_rx_handle[1].frame_size);
            hal_audio_buf_mgm_get_write_data_done(p_dsp_info, usb_stream_rx_handle[1].frame_size + BLK_HEADER_SIZE);

            #if BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]r_offset = %u, w_offset = %u", 2, p_dsp_info->read_offset, p_dsp_info->write_offset);
            #endif /* BLE_AUDIO_DONGLE_MUSIC_PATH_DEBUG_LOG */

            //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, usb_stream_rx_handle[1].frame_size, 13);

            /* get residual usb data size */
            available_data_size = USB_Audio_Get_Len_Received_Data(1);

#if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            int16_t *start_address = NULL;
            uint32_t current_level = 0;
            uint32_t i;

            if (usb_stream_rx_handle[1].latency_debug_enable)
            {
                current_level = usb_stream_rx_handle[1].latency_debug_last_level;
                start_address = (int16_t *)(p_source_buf + BLK_HEADER_SIZE);
                if ((*(start_address + 2 * 0) > usb_stream_rx_handle[1].latency_debug_last_sample) &&
                    (*(start_address + 2 * 0) - usb_stream_rx_handle[1].latency_debug_last_sample > 40000))
                {
                    current_level = 1;
                }
                else if ((*(start_address + 2 * 0) < usb_stream_rx_handle[1].latency_debug_last_sample) &&
                         (usb_stream_rx_handle[1].latency_debug_last_sample - *(start_address + 2 * 0) > 40000))
                {
                    current_level = 0;
                }
                for (i = 0; i < usb_stream_rx_handle[1].frame_size / 4 - 1; i++)
                {
                    if ((*(start_address + 2 * i) > *(start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * i) - *(start_address + 2 * (i + 1)) > 40000))
                    {
                        current_level = 0;
                        break;
                    }
                    else if ((*(start_address + 2 * i) < *(start_address + 2 * (i + 1))) &&
                             (*(start_address + 2 * (i + 1)) - *(start_address + 2 * i) > 40000))
                    {
                        current_level = 1;
                        break;
                    }
                    else
                    {
                    }
                }
                usb_stream_rx_handle[1].latency_debug_last_sample = *(start_address + 2 * (usb_stream_rx_handle[1].frame_size / 4 - 1));
                if (current_level != usb_stream_rx_handle[1].latency_debug_last_level)
                {
                    hal_gpio_set_output(HAL_GPIO_13, current_level);
                    usb_stream_rx_handle[1].latency_debug_last_level = current_level;
                }
            }
#endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
        }
    }
}

static void usb_audio_tx_trigger_dsp_flow(void)
{
    hal_ccni_message_t ccni_msg = {{0}};
    if (hal_ccni_set_event(CCNI_CM4_TO_DSP0_EVENT3, &ccni_msg) != HAL_CCNI_STATUS_OK)
    {
        TRANSMITTER_LOG_E("[usb_audio_tx_trigger_dsp_flow]send ccni fail\r\n", 0);
        configASSERT(0);
    }

    #if BLE_AUDIO_DONGLE_VOICE_PATH_DEBUG_LOG
    TRANSMITTER_LOG_I("[usb_audio_tx_trigger_dsp_flow]send ccni", 0);
    #endif /* BLE_AUDIO_DONGLE_VOICE_PATH_DEBUG_LOG */
}

static void usb_audio_tx_cb_ble_audio_dongle_0(void)
{
    uint32_t gpt_count, duration_count;
    uint32_t data_size;
    n9_dsp_share_info_t *p_dsp_info;
    uint32_t blk_size;
    uint32_t data_size_total;
    uint8_t *p_source_buf;

    /* get current gpt count */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    #if BLE_AUDIO_DONGLE_VOICE_PATH_DEBUG_LOG
    TRANSMITTER_LOG_I("[BLE Audio Dongle]usb_audio_tx_cb_ble_audio_dongle_0 callback = %u", 1, gpt_count);
    #endif /* BLE_AUDIO_DONGLE_VOICE_PATH_DEBUG_LOG */

    if ((ul_stream_status & 0x1) == 0)
    {
        /* workaround: If the stream is not started, need to send zero data to usb to avoid usb host error */
        hal_usb_set_endpoint_tx_ready(1);
        hal_usb_write_endpoint_fifo(0x01, usb_stream_tx_handle[0].frame_size, (void*)all_zero_buffer);
        return;
    }

    /* get share buffer info */
    p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_SEND_TO_MCU);

    if (usb_stream_tx_handle[0].usb_first_in_flag == 0)
    {
        usb_stream_tx_handle[0].previous_gpt_count = gpt_count;
        usb_stream_tx_handle[0].usb_first_in_flag  = 1;

        TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_TX_DEBUG 0]usb_audio_tx_cb_ble_audio_dongle_0 callback first time = %u", 1, gpt_count);
    }
    else
    {
        hal_gpt_get_duration_count(usb_stream_tx_handle[0].previous_gpt_count, gpt_count, &duration_count);
        if ((duration_count > 1500) || (duration_count < 500))
        {
            TRANSMITTER_LOG_E("[BLE Audio Dongle][USB_TX_DEBUG 0]usb_audio_tx_cb_ble_audio_dongle_0 duration = %d", 1, duration_count);
        }
        usb_stream_tx_handle[0].previous_gpt_count = gpt_count;
    }

    /* get data info */
    hal_audio_buf_mgm_get_data_buffer(p_dsp_info, &p_source_buf, &data_size_total);
    if (data_size_total == 0)
    {
        data_size = 0;
        p_source_buf = NULL;
    }
    else
    {
        data_size = ((audio_transmitter_block_header_t *)p_source_buf)->data_length;
        /* check data size */
        if (data_size != usb_stream_tx_handle[0].frame_size)
        {
            TRANSMITTER_LOG_E("[BLE Audio Dongle]usb_audio_tx_cb_ble_audio_dongle_0 data_size is not right %u, %u", 2, data_size, usb_stream_tx_handle[0].frame_size);
            configASSERT(0);
        }
        p_source_buf += sizeof(audio_transmitter_block_header_t);
    }

    /* check if needs to send ccni to trigger dsp flow */
    if (data_size_total == (usb_stream_tx_handle[0].frame_size+sizeof(audio_transmitter_block_header_t))*DONGLE_USB_TX_SEND_CCNI_FRAMES)
    {
        /* send ccni to trigger dsp flow if there are only 3ms data in share buffer, so dsp has 2ms to process new data */
        usb_audio_tx_trigger_dsp_flow();
    }
    else if (data_size_total == 0)
    {
        /* send ccni to trigger dsp flow if there is no data in share buffer */
        usb_audio_tx_trigger_dsp_flow();
    }

    /* send usb data */
    hal_usb_set_endpoint_tx_ready(1);
    if (data_size == 0)
    {
        if (usb_stream_tx_handle[0].stream_is_started == 0)
        {
            /* the stream is not started, so send slience data */
            hal_usb_write_endpoint_fifo(0x01, usb_stream_tx_handle[0].frame_size, (void*)all_zero_buffer);
        }
        else
        {
            hal_usb_write_endpoint_fifo(0x01, usb_stream_tx_handle[0].frame_size, (void*)all_zero_buffer);
            TRANSMITTER_LOG_E("[BLE Audio Dongle]usb_audio_tx_cb_ble_audio_dongle_0 data is not enough", 0);
            // configASSERT(0);
        }
    }
    else
    {
        /* set data from share buffer into USB FIFO */
        hal_usb_write_endpoint_fifo(0x01, usb_stream_tx_handle[0].frame_size, (void*)p_source_buf);

        /* drop this packet */
        blk_size = p_dsp_info->sub_info.block_size;
        hal_audio_buf_mgm_get_read_data_done(p_dsp_info, blk_size);

        if (usb_stream_tx_handle[0].stream_is_started == 0)
        {
            /* set that the stream is started */
            usb_stream_tx_handle[0].stream_is_started = 1;
        }
    }

#if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
    int16_t *start_address = NULL;
    uint32_t current_level = 0;
    uint32_t i;
    int16_t sample_value = 0;
    uint16_t frame_samples;
    uint16_t channel_num;
    uint16_t resolution_size;

    if (usb_stream_tx_handle[0].latency_debug_enable)
    {
        if (data_size == 0)
        {
            start_address = all_zero_buffer;
        }
        else
        {
            start_address = p_source_buf;
        }
        if (usb_stream_tx_handle[0].usb_param.pcm.channel_mode == 1)
        {
            channel_num = 1;
        }
        else
        {
            channel_num = 2;
        }
        resolution_size = 2;
        frame_samples = usb_stream_tx_handle[0].frame_size / resolution_size / channel_num;
        for (i = 0; i < frame_samples; i++) {
            sample_value += (*(start_address + i*channel_num) / frame_samples);
        }
        if (sample_value >= 5000)
        {
            current_level = 1;
        }
        else
        {
            current_level = 0;
        }
        if (current_level != usb_stream_tx_handle[0].latency_debug_last_level) {
            hal_gpio_set_output(HAL_GPIO_13, current_level);
            usb_stream_tx_handle[0].latency_debug_last_level = current_level;
        }
    }
#endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
}

/* Public functions ----------------------------------------------------------*/
#if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
void ble_audio_dongle_rx_latency_debug_control(uint32_t port, bool enable)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (enable)
    {
        usb_stream_rx_handle[port].latency_debug_enable = 1;
        usb_stream_rx_handle[port].latency_debug_last_level = 0;
        usb_stream_rx_handle[port].latency_debug_last_sample = 0;
    }
    else
    {
        usb_stream_rx_handle[port].latency_debug_enable = 0;
        usb_stream_rx_handle[port].latency_debug_last_level = 0;
        usb_stream_rx_handle[port].latency_debug_last_sample = 0;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

void ble_audio_dongle_tx_latency_debug_control(uint32_t port, bool enable)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (enable)
    {
        usb_stream_tx_handle[port].latency_debug_enable = 1;
        usb_stream_tx_handle[port].latency_debug_last_level = 0;
        usb_stream_tx_handle[port].latency_debug_last_sample = 0;
    }
    else
    {
        usb_stream_tx_handle[port].latency_debug_enable = 0;
        usb_stream_tx_handle[port].latency_debug_last_level = 0;
        usb_stream_tx_handle[port].latency_debug_last_sample = 0;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}
#endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */

void ble_audio_dongle_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    uint32_t payload_size = 0;
    uint32_t gain;
    uint32_t gain_default;
    uint8_t volume_level;

    switch (config->scenario_sub_id)
    {
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT:
            /* stream source */
            /* get codec frame size */
            payload_size = ble_audio_codec_get_frame_size(  &(config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_type),
                                                            &(config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param));
            if ((payload_size == 0) || (payload_size != config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_size))
            {
                TRANSMITTER_LOG_E("[BLE Audio Dongle] error codec frame size %d, %d\r\n", 2, payload_size, config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_size);
                configASSERT(0);
            }
            open_param->param.stream_in = STREAM_IN_BT_COMMON;
            open_param->stream_in_param.bt_common.scenario_type = config->scenario_type;
            open_param->stream_in_param.bt_common.scenario_sub_id = config->scenario_sub_id;
            open_param->stream_in_param.bt_common.share_info_type = SHARE_BUFFER_INFO_TYPE;
            open_param->stream_in_param.bt_common.data_notification_frequency = 1;
            open_param->stream_in_param.bt_common.max_payload_size = payload_size+sizeof(LE_AUDIO_HEADER);
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.period                          = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.channel_enable                  = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_type                      = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_type;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.sample_rate     = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.sample_rate;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.channel_mode    = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.channel_mode;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.frame_interval  = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_interval;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.frame_size      = payload_size;
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.bit_rate        = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.bit_rate;
            /* upper layer will prepare channel 2's share memory */
            open_param->stream_in_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_1);
            // memset((((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->read_offset          = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->write_offset         = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_size  = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_num   = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_size);
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2 = (uint8_t *)open_param->stream_in_param.bt_common.p_share_info;
            /* upper layer will prepare channel 1's share memory */
            open_param->stream_in_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_0);
            // memset((((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->read_offset          = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->write_offset         = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_size  = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
            // ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_num   = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.p_share_info))->sub_info.block_size);
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1 = (uint8_t *)open_param->stream_in_param.bt_common.p_share_info;
            /* gain setting */
            gain = audio_get_gain_in_in_dB(0, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            gain_default = gain;
            volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_L;
            if(volume_level <= bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                gain = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_L = gain;
            gain = gain_default;
            volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_R;
            if(volume_level <= bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                gain = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_R = gain;
            if (config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.test_mode_enable)
            {
                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_L = 0;
                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_R = 0;
                TRANSMITTER_LOG_I("[BLE Audio Dongle][ul] enter test mode\r\n", 0);
            }
            TRANSMITTER_LOG_I("[BLE Audio Dongle][ul] codec setting: %u, %u, 0x%x, %u, %u, %u, %u, %u, %u, 0x%x, 0x%x, 0x%x, 0x%x\r\n", 13,
                                config->scenario_sub_id,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable,
                                payload_size,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_type,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.sample_rate,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.channel_mode,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_interval,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.bit_rate,
                                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1,
                                ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1))->start_addr,
                                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2,
                                ((n9_dsp_share_info_t *)(open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2))->start_addr);

            /* stream sink */
            /* get usb frame size */
            payload_size = usb_audio_get_frame_size(&(config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_type),
                                                    &(config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param));
            if (payload_size == 0)
            {
                TRANSMITTER_LOG_E("[BLE Audio Dongle] error usb frame size %d\r\n", 1, payload_size);
                configASSERT(0);
            }
            open_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
            open_param->stream_out_param.data_ul.scenario_type = config->scenario_type;
            open_param->stream_out_param.data_ul.scenario_sub_id = config->scenario_sub_id;
            open_param->stream_out_param.data_ul.data_notification_frequency = 0;
            open_param->stream_out_param.data_ul.max_payload_size = payload_size;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.period                       = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.channel_enable               = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.codec_type                   = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_type;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.codec_param.pcm.sample_rate  = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.sample_rate;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.codec_param.pcm.channel_mode = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.channel_mode;
            open_param->stream_out_param.data_ul.scenario_param.ble_audio_dongle_param.codec_param.pcm.format       = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.format;
            usb_stream_tx_handle[0].period                      = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period;
            usb_stream_tx_handle[0].frame_size                  = payload_size;
            usb_stream_tx_handle[0].usb_type                    = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_type;
            usb_stream_tx_handle[0].usb_param.pcm.sample_rate   = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.sample_rate;
            usb_stream_tx_handle[0].usb_param.pcm.channel_mode  = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.channel_mode;
            usb_stream_tx_handle[0].usb_param.pcm.format        = config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.format;
            open_param->stream_out_param.data_ul.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_SEND_TO_MCU);
            open_param->stream_out_param.data_ul.p_share_info->read_offset = 0;
            open_param->stream_out_param.data_ul.p_share_info->write_offset = 0;
            open_param->stream_out_param.data_ul.p_share_info->bBufferIsFull = false;
            audio_transmitter_modify_share_info_by_block(open_param->stream_out_param.data_ul.p_share_info, payload_size);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][ul] usb setting: %u, %u, 0x%x, %u, %u, %u, %u, %u, %d, %d, 0x%x\r\n", 11,
                                config->scenario_sub_id,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable,
                                payload_size,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_type,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.sample_rate,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.channel_mode,
                                config->scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.format,
                                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_L,
                                open_param->stream_in_param.bt_common.scenario_param.ble_audio_dongle_param.gain_default_R,
                                open_param->stream_out_param.data_ul.p_share_info);
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0:
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1:
            /* stream source */
            /* get usb frame size */
            payload_size = usb_audio_get_frame_size(&(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type),
                                                    &(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param));
            if (payload_size == 0)
            {
                TRANSMITTER_LOG_E("[BLE Audio Dongle] error usb frame size %d\r\n", 1, payload_size);
                configASSERT(0);
            }
            open_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
            open_param->stream_in_param.data_dl.scenario_type = config->scenario_type;
            open_param->stream_in_param.data_dl.scenario_sub_id = config->scenario_sub_id;
            open_param->stream_in_param.data_dl.data_notification_frequency = 0;
            open_param->stream_in_param.data_dl.max_payload_size = payload_size;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.period                        = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.channel_enable                = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.codec_type                    = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.codec_param.pcm.sample_rate   = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.sample_rate;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.codec_param.pcm.channel_mode  = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.channel_mode;
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.codec_param.pcm.format        = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.format;
            if (config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0)
            {
                usb_stream_rx_handle[0].period                      = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period;
                usb_stream_rx_handle[0].frame_size                  = payload_size;
                usb_stream_rx_handle[0].usb_type                    = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type;
                usb_stream_rx_handle[0].usb_param.pcm.sample_rate   = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.sample_rate;
                usb_stream_rx_handle[0].usb_param.pcm.channel_mode  = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.channel_mode;
                usb_stream_rx_handle[0].usb_param.pcm.format        = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.format;
                open_param->stream_in_param.data_dl.p_share_info    = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_0);
            }
            else
            {
                usb_stream_rx_handle[1].period                      = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period;
                usb_stream_rx_handle[1].frame_size                  = payload_size;
                usb_stream_rx_handle[1].usb_type                    = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type;
                usb_stream_rx_handle[1].usb_param.pcm.sample_rate   = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.sample_rate;
                usb_stream_rx_handle[1].usb_param.pcm.channel_mode  = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.channel_mode;
                usb_stream_rx_handle[1].usb_param.pcm.format        = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.format;
                open_param->stream_in_param.data_dl.p_share_info    = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_1);
            }
            open_param->stream_in_param.data_dl.p_share_info->read_offset = 0;
            open_param->stream_in_param.data_dl.p_share_info->write_offset = 0;
            open_param->stream_in_param.data_dl.p_share_info->bBufferIsFull = false;
            audio_transmitter_modify_share_info_by_block(open_param->stream_in_param.data_dl.p_share_info, payload_size);
            /* gain setting */
            gain = audio_get_gain_out_in_dB(0, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
            gain_default = gain;
            volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_L;
            if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
            }
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_L = gain;
            gain = gain_default;
            volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_R;
            if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
            }
            open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_R = gain;
            if (config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.test_mode_enable)
            {
                open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_L = 0;
                open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_R = 0;
                TRANSMITTER_LOG_I("[BLE Audio Dongle][dl] enter test mode\r\n", 0);
            }
            TRANSMITTER_LOG_I("[BLE Audio Dongle][dl] usb setting: %u, %u, 0x%x, %u, %u, %u, %u, %u, %d, %d, 0x%x\r\n", 11,
                                config->scenario_sub_id,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable,
                                payload_size,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.sample_rate,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.channel_mode,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.format,
                                open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_L,
                                open_param->stream_in_param.data_dl.scenario_param.ble_audio_dongle_param.gain_default_R,
                                open_param->stream_in_param.data_dl.p_share_info);

            /* stream sink */
            /* get codec frame size */
            payload_size = ble_audio_codec_get_frame_size(  &(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type),
                                                            &(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param));
            if ((payload_size == 0) || (payload_size != config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size))
            {
                TRANSMITTER_LOG_E("[BLE Audio Dongle] error codec frame size %d, %d\r\n", 2, payload_size, config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size);
                configASSERT(0);
            }
            open_param->param.stream_out = STREAM_OUT_BT_COMMON;
            open_param->stream_out_param.bt_common.scenario_type = config->scenario_type;
            open_param->stream_out_param.bt_common.scenario_sub_id = config->scenario_sub_id;
            open_param->stream_out_param.bt_common.share_info_type = SHARE_BUFFER_INFO_TYPE;
            open_param->stream_out_param.bt_common.data_notification_frequency = 1;
            open_param->stream_out_param.bt_common.max_payload_size = payload_size+sizeof(LE_AUDIO_HEADER);
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.period                         = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.channel_enable                 = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_type                     = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type;//AUDIO_DSP_CODEC_TYPE_LC3
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.sample_rate    = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.sample_rate;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.channel_mode   = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.channel_mode;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.frame_interval = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_interval;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.frame_size     = payload_size;
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3.bit_rate       = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.bit_rate;
            /* upper layer will prepare channel 2's share memory */
            open_param->stream_out_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1);
            // if (dl_stream_status == 0)
            // {
            //     memset((((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
            // }
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->read_offset         = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->write_offset        = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_num  = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size);
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2 = (uint8_t *)open_param->stream_out_param.bt_common.p_share_info;
            /* upper layer will prepare channel 1's share memory */
            open_param->stream_out_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0);
            // if (dl_stream_status == 0)
            // {
            //     memset((((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
            // }
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->read_offset         = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->write_offset        = 0;
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
            // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_num  = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size);
            open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1 = (uint8_t *)open_param->stream_out_param.bt_common.p_share_info;
            if (config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0)
            {
                dl_stream_status |= 0x1;
            }
            else
            {
                dl_stream_status |= 0x2;
            }
            TRANSMITTER_LOG_I("[BLE Audio Dongle][dl] codec setting: %u, %u, 0x%x, %u, %u, %u, %u, %u, %u, 0x%x, 0x%x, 0x%x, 0x%x\r\n", 13,
                                config->scenario_sub_id,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable,
                                payload_size,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.sample_rate,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.channel_mode,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_interval,
                                config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.bit_rate,
                                open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1,
                                ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1))->start_addr,
                                open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2,
                                ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2))->start_addr);
            break;

        default:
            TRANSMITTER_LOG_E("not in ble source dongle scenario sub id list\r\n", 0);
            configASSERT(0);
            break;
    }
}

void ble_audio_dongle_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    switch (config->scenario_sub_id)
    {
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT:
            start_param->param.stream_in = STREAM_IN_BT_COMMON;
            start_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0:
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1:
            start_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
            start_param->param.stream_out = STREAM_OUT_BT_COMMON;
            break;

        default:
            TRANSMITTER_LOG_E("not in ble source dongle scenario sub id list\r\n", 0);
            configASSERT(0);
            break;
    }
}

audio_transmitter_status_t ble_audio_dongle_set_runtime_config_playback(audio_transmitter_config_t *config, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config, mcu2dsp_audio_transmitter_runtime_config_param_t *runtime_config_param)
{
    audio_transmitter_status_t ret = AUDIO_TRANSMITTER_STATUS_FAIL;
    uint32_t operation = runtime_config_type;
    vol_gain_t gain;
    uint8_t volume_level;
    uint8_t volume_ratio = 0;

    if ((config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
    {
        switch (operation)
        {
            case BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_MIX:
                TRANSMITTER_LOG_I("scenario_sub_id =%d: MUSIC_MIX id %d.",2 ,config->scenario_sub_id, runtime_config->ble_audio_dongle_runtime_config.dl_mixer_id);
                runtime_config_param->config_operation = operation;
                dl_mixer_param_t dl_mixer;
                dl_mixer.scenario_type = g_audio_transmitter_control[runtime_config->ble_audio_dongle_runtime_config.dl_mixer_id].config.scenario_type;
                dl_mixer.scenario_sub_id = g_audio_transmitter_control[runtime_config->ble_audio_dongle_runtime_config.dl_mixer_id].config.scenario_sub_id;
                memcpy(runtime_config_param->config_param, &dl_mixer, sizeof(dl_mixer_param_t));
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_UNMIX:
                runtime_config_param->config_operation = operation;
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_L:
                volume_ratio = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_ratio;
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_1 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_R:
                volume_ratio = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_ratio;
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_2 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL:
                volume_ratio = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_ratio;
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_1 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_2 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            default:
                break;
        }
    }
    else if (config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
    {
        switch (operation)
        {
            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_L:
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_voice_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_R:
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_voice_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_DUL:
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_voice_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
                volume_level = runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_voice_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->ble_audio_dongle_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_SET_UL_CH1_INPUT_SOURCE:
            case BLE_AUDIO_DONGLE_CONFIG_OP_SET_UL_CH2_INPUT_SOURCE:
                runtime_config_param->config_operation = operation;
                memcpy(runtime_config_param->config_param, &runtime_config->ble_audio_dongle_runtime_config.channel_enable, sizeof(uint32_t));
                TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d channel_enable 0x%x.",
                                3 ,config->scenario_sub_id, operation, runtime_config->ble_audio_dongle_runtime_config.channel_enable);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            default:
                break;
        }
    }

    return ret;
}

audio_transmitter_status_t ble_audio_dongle_get_runtime_config(uint8_t scenario_type, uint8_t scenario_sub_id, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config)
{
    audio_transmitter_status_t ret = AUDIO_TRANSMITTER_STATUS_FAIL;
    uint32_t operation = runtime_config_type;

    if (scenario_type != AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
    {
        TRANSMITTER_LOG_I("[BLE Audio Dongle] error scenario_type %d.", 1 ,scenario_type);
        return ret;
    }

    if ((scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) || (scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
    {
        switch (operation)
        {
            case BLE_AUDIO_DONGLE_CONFIG_OP_GET_DL_CH1_SHARE_INFO:
                runtime_config->ble_audio_dongle_runtime_config.share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_GET_DL_CH2_SHARE_INFO:
                runtime_config->ble_audio_dongle_runtime_config.share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            default:
                break;
        }
    }
    else if (scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
    {
        switch (operation)
        {
            case BLE_AUDIO_DONGLE_CONFIG_OP_GET_UL_CH1_SHARE_INFO:
                runtime_config->ble_audio_dongle_runtime_config.share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_0);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            case BLE_AUDIO_DONGLE_CONFIG_OP_GET_UL_CH2_SHARE_INFO:
                runtime_config->ble_audio_dongle_runtime_config.share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_1);
                ret = AUDIO_TRANSMITTER_STATUS_SUCCESS;
                break;

            default:
                break;
        }
    }

    return ret;
}

void ble_audio_dongle_state_started_handler(uint8_t scenario_sub_id)
{
    uint32_t saved_mask;

    switch (scenario_sub_id)
    {
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT:
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            usb_stream_tx_handle[0].usb_first_in_flag = 0;
            usb_stream_tx_handle[0].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_tx_handle[0].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            ul_stream_status = ul_stream_status | 0x1;
            hal_nvic_restore_interrupt_mask(saved_mask);
            #ifdef MTK_USB_AUDIO_MICROPHONE
            USB_Audio_Register_Tx_Callback(0, usb_audio_tx_cb_ble_audio_dongle_0);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_TX_DEBUG 0]Register usb_audio_tx_cb 0", 0);
            #endif /* MTK_USB_AUDIO_MICROPHONE */
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0:
            usb_stream_rx_handle[0].usb_first_in_flag = 0;
            usb_stream_rx_handle[0].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_rx_handle[0].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            #if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            USB_Audio_Register_Rx_Callback(0, usb_audio_rx_cb_ble_audio_dongle_0);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]Register usb_audio_rx_cb 0", 0);
            #endif /* MTK_USB_DEMO_ENABLED */
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1:
            usb_stream_rx_handle[1].usb_first_in_flag = 0;
            usb_stream_rx_handle[1].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_rx_handle[1].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            #if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            USB_Audio_Register_Rx_Callback(1, usb_audio_rx_cb_ble_audio_dongle_1);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]Register usb_audio_rx_cb 1", 0);
            #endif /* MTK_USB_DEMO_ENABLED */
            break;

        default:
            TRANSMITTER_LOG_E("not in ble source dongle scenario sub id list\r\n", 0);
            configASSERT(0);
            break;
    }
}

void ble_audio_dongle_state_idle_handler(uint8_t scenario_sub_id)
{
    uint32_t saved_mask;

    switch (scenario_sub_id)
    {
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT:
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            usb_stream_tx_handle[0].usb_first_in_flag = 0;
            usb_stream_tx_handle[0].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_tx_handle[0].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            ul_stream_status = ul_stream_status & 0xfffffffe;
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0:
            #if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            USB_Audio_Register_Rx_Callback(0, NULL);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 0]Unregister usb_audio_rx_cb 0", 0);
            #endif /* MTK_USB_DEMO_ENABLED */
            usb_stream_rx_handle[0].usb_first_in_flag = 0;
            usb_stream_rx_handle[0].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_rx_handle[0].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            dl_stream_status = dl_stream_status & 0xfffffffe;
            break;

        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1:
            #if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            USB_Audio_Register_Rx_Callback(1, NULL);
            TRANSMITTER_LOG_I("[BLE Audio Dongle][USB_RX_DEBUG 1]Unregister usb_audio_rx_cb 1", 0);
            #endif /* MTK_USB_DEMO_ENABLED */
            usb_stream_rx_handle[1].usb_first_in_flag = 0;
            usb_stream_rx_handle[1].stream_is_started = 0;
            #if BLE_AUDIO_DONGLE_DEBUG_LANTENCY
            usb_stream_rx_handle[1].latency_debug_enable = 0;
            #endif /* BLE_AUDIO_DONGLE_DEBUG_LANTENCY */
            dl_stream_status = dl_stream_status & 0xfffffffd;
            break;

        default:
            TRANSMITTER_LOG_E("not in ble source dongle scenario sub id list\r\n", 0);
            configASSERT(0);
            break;
    }
}

#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
