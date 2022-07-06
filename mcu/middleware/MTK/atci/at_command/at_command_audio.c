/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

// For Register AT command handler
#include "at_command.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(MTK_LINEIN_PLAYBACK_ENABLE)
#include "linein_playback.h"
#endif

#include "hal_audio.h"

#ifdef MTK_SMT_AUDIO_TEST
    #include "at_command_audio_ata_test.h"
    #include "Audio_FFT.h"
    #include "hal_nvic.h"

    #define SMT_DROP_CNT 60
    #define SMT_SAVE 61
    #define SMT_UL_CNT_LIMIT 65

#endif


#ifdef AIR_ATA_TEST_ENABLE
#ifndef MTK_SMT_AUDIO_TEST
    #include "at_command_audio_ata_test.h"
    #include "Audio_FFT.h"
    #include "hal_nvic.h"

    #define SMT_DROP_CNT 60
    #define SMT_SAVE 61
    #define SMT_UL_CNT_LIMIT 65
#endif
    bool g_ata_running = false;
    hal_audio_analog_mdoe_t g_ata_amic_mode = HAL_AUDIO_ANALOG_MODE_DUMMY;
#endif


#if !defined(MTK_AUDIO_AT_COMMAND_DISABLE) && defined(HAL_AUDIO_MODULE_ENABLED)
    #include "hal_log.h"
#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
    #include "bt_sink_srv_ami.h"
    #include "bt_sink_srv_common.h"
    #if defined(MTK_PROMPT_SOUND_ENABLE)
      #include "prompt_control.h"
    #endif
    #ifdef MTK_ANC_ENABLE
    #ifdef MTK_ANC_V2
      #include "anc_control_api.h"
    #else
      #include "anc_control.h"
    #endif
    #endif
    #if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    #ifdef MTK_RACE_CMD_ENABLE
      #include "race_cmd_dsprealtime.h"
    #endif
    #endif
    #ifdef MTK_RECORD_ENABLE
      #include "record_control.h"
    #endif
    #if defined(MTK_LINEIN_PLAYBACK_ENABLE)
      #include "audio_sink_srv_line_in.h"

    #endif
    #ifdef MTK_LEAKAGE_DETECTION_ENABLE
      #include "leakage_detection_control.h"
    #endif
    #include "nvkey.h"
    #include "nvkey_dspfw.h"
    #include "hal_audio_internal.h"
    #include "hal_dvfs_internal.h"
    #include "hal_gpio.h"
    #include "hal_audio_cm4_dsp_message.h"
#endif

  #if !defined(MTK_AVM_DIRECT)
    #include "hal_audio_test.h"
    #include "hal_audio_enhancement.h"

    #ifdef MTK_EXTERNAL_DSP_ENABLE
    #include "external_dsp_application.h"
    #include "external_dsp_driver.h"

    #if defined (MTK_NDVC_ENABLE)
    extern uint16_t spe_ndvc_uplink_noise_index_map(uint16_t db);
    bool ndvc_at_test = false;
    #endif /*MTK_NDVC_ENABLE*/

    #endif /*MTK_EXTERNAL_DSP_ENABLE*/

    #if defined(MTK_BT_AWS_ENABLE)
    #include "hal_audio_internal_service.h"
    #endif  /* defined(MTK_BT_AWS_ENABLE)) */

  #if defined(MTK_BT_HFP_CODEC_DEBUG)
  #include "bt_hfp_codec_internal.h"
typedef enum {
    AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE = 0,
    AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT = 1,
} at_audio_hfp_save_or_print_method_t;
  #endif
  #endif /*!defined(MTK_AVM_DIRECT)*/

#ifdef AIR_LE_AUDIO_ENABLE
#include "ble_pacs.h"
#endif

#ifdef AIR_ANC_USER_UNAWARE_ENABLE
#include "audio_nvdm_common.h"
#define AT_CMD_CONTROL_ADAPTIVE_ANC_STREAM TRUE //for power testing
#if defined(AT_CMD_CONTROL_ADAPTIVE_ANC_STREAM)
#include "anc_monitor.h"
#endif
#endif

#define _UNUSED(x)  ((void)(x))
log_create_module(atcmd_aud, PRINT_LEVEL_INFO);

#define LOGMSGIDE(fmt,arg...)   LOG_MSGID_E(atcmd_aud, "ATCMD_AUD: "fmt,##arg)
#define LOGMSGIDW(fmt,arg...)   LOG_MSGID_W(atcmd_aud, "ATCMD_AUD: "fmt,##arg)
#define LOGMSGIDI(fmt,arg...)   LOG_MSGID_I(atcmd_aud ,"ATCMD_AUD: "fmt,##arg)


#if defined(MTK_PROMPT_SOUND_ENABLE)
bool g_app_voice_prompt_test_off = false;
#endif

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
static uint8_t g_peq_test_coef[] = { //500hz low pass
0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x0D, 0x00, 0x01, 0x00, 0x22, 0x00, 0x00, 0x4F, 0x22, 0x00,
0x00, 0x4F, 0x22, 0x00, 0x00, 0x4F, 0x8E, 0x82, 0x00, 0x8D, 0x86, 0x7B, 0x00, 0x37, 0x4D, 0x02,
0x00, 0xEF, 0x02, 0x00, 0x0D, 0x00, 0x01, 0x00, 0x1D, 0x00, 0x00, 0x01, 0x1D, 0x00, 0x00, 0x01,
0x1D, 0x00, 0x00, 0x01, 0x54, 0x82, 0x00, 0x31, 0xE1, 0x7B, 0x00, 0xAD, 0x4D, 0x02, 0x00, 0xEF, };
#endif


#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822)|| (PRODUCT_VERSION == 1565))
#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
extern au_afe_multi_input_instance_param_t audio_multi_instance_ctrl;
#endif
#endif
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
extern void ami_hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running);
#ifdef MTK_RECORD_ENABLE
bool record_flag = false;
bool g_record_inited = false;
int16_t g_record_Rdemo_data[128];
#define RECORD_OPUS_DATA_MAX_SIZE_U8    (sizeof(uint8_t) * 80)*4//ENCODER_BITRATE_32KBPS = 80 BYtes,record.frames_per_message = 4
#define RECORD_DATA_MAX_SIZE_U8         (sizeof(int16_t) * 128)
int8_t g_reocrd_id = 0;
extern uint16_t g_stream_in_sample_rate;
extern bool g_dump;
extern uint16_t g_stream_in_code_type;
extern uint16_t g_leakage_compensation_record_duration;

uint32_t record_times = 0;
#define RECORD_MAX_TIME  128*10//10 sec for 16K SPS record

#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)

#include "audio_transmitter_control.h"

#define TRANSMITTER_ID_MAX  4
typedef struct{
    audio_transmitter_id_t transmitter_id;
    audio_transmitter_scenario_type_t scenario_type;
    uint8_t scenario_sub_id;
}transmitter_user_t;
static transmitter_user_t g_transmitter_user[TRANSMITTER_ID_MAX] = {{-1,0,0},{-1,0,0},{-1,0,0},{-1,0,0}}; //need add more while TRANSMITTER_ID_MAX increase

static volatile bool g_transmitter_stop_ok = false;
static volatile bool g_transmitter_start_ok = false;


void user_audio_transmitter_callback(audio_transmitter_event_t event,void *data, void *user_data)
{
    uint32_t cb_data = 0;
    uint8_t *address;
    uint32_t length;
    transmitter_user_t *user = (transmitter_user_t *)user_data;

    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION:
             if(data != NULL){
                cb_data = *((uint32_t*)data);
            }
            audio_transmitter_get_read_information(user->transmitter_id, &address,&length);
            #if 0
            #include "audio_dump.h"
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, uplink data event, data = %x", 3, user->scenario_type, user->scenario_sub_id, cb_data);
            LOGMSGIDI("[audio_transmitter]: address = 0x%08x, length = 0x%08x", 2, address, length);
            if(length > 0){
                LOG_AUDIO_DUMP(address, length, VOICE_TX_MIC_3);
            }
            #endif
            audio_transmitter_read_done(user->transmitter_id, length);
            break;

        case AUDIO_TRANSMITTER_EVENT_DATA_DIRECT:
            if(data != NULL){
                cb_data = *((uint32_t*)data);
            }
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, uplink data direct event, data = %x", 3, user->scenario_type, user->scenario_sub_id, cb_data);
            break;

        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS:
            g_transmitter_start_ok = true;
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_START_SUCCESS", 2, user->scenario_type, user->scenario_sub_id);
            break;

        case AUDIO_TRANSMITTER_EVENT_START_FAIL:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_START_FAIL", 2, user->scenario_type, user->scenario_sub_id);
            break;

        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP:
            g_transmitter_start_ok = true;
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP", 2, user->scenario_type, user->scenario_sub_id);
            break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP", 2, user->scenario_type, user->scenario_sub_id);
            break;

        case AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_OUT:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_OUT", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_IN:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_IN", 2, user->scenario_type, user->scenario_sub_id);
        break;

        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS:
            g_transmitter_stop_ok = true;
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, AUDIO_TRANSMITTER_STOP_SUCCESS", 2, user->scenario_type, user->scenario_sub_id);
            break;

        default:
            LOGMSGIDI("[audio_transmitter]: scenario type %d, scenario sub id %d, unknown event %d", 3, user->scenario_type, user->scenario_sub_id, event);
            break;
    }
}
#endif

void record_control_CCNI_demo_callback(hal_audio_event_t event, void *data)
{
    uint32_t *user_data = data;
    switch(event){
        case HAL_AUDIO_EVENT_ERROR:
            LOGMSGIDI("[AT][RECORD][CALLBACK]HAL_AUDIO_EVENT_ERROR\r\n", 0);
            break;
        case HAL_AUDIO_EVENT_WWE_VERSION:
            LOGMSGIDI("[AT][RECORD][CALLBACK]HAL_AUDIO_EVENT_WWE_VERSION\r\n", 0);
            LOGMSGIDI("[AT][RECORD][CALLBACK]user_data = 0x%08x\r\n", 1, user_data);
            break;
        case HAL_AUDIO_EVENT_WWD_NOTIFICATION:
            LOGMSGIDI("[AT][RECORD][CALLBACK]HAL_AUDIO_EVENT_WWD_NOTIFICATION\r\n", 0);
            LOGMSGIDI("[AT][RECORD][CALLBACK]user_data = 0x%08x\r\n", 1, user_data);
            break;
        case HAL_AUDIO_EVENT_DATA_ABORT_NOTIFICATION:
            LOGMSGIDI("[AT][RECORD][CALLBACK]HAL_AUDIO_EVENT_DATA_ABORT_NOTIFICATION\r\n", 0);
            break;

        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
        if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_PCM){
            for(uint16_t i=0; i < 8 ; i++)
            {
                if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(&g_record_Rdemo_data, RECORD_DATA_MAX_SIZE_U8)){
                    //LOG_AUDIO_DUMP(g_record_Rdemo_data, sizeof(int16_t) * 128, VOICE_TX_MIC_3);
                    record_times++;
                    if(record_times == RECORD_MAX_TIME)
                    {
                        record_times = 0;
                        if (RECORD_CONTROL_EXECUTION_SUCCESS != audio_record_control_stop(g_reocrd_id)){
                            LOGMSGIDI("[AT][RECORD][CALLBACK]record stop fail\r\n", 0);
                            return;
                        }
                        if (RECORD_CONTROL_EXECUTION_SUCCESS != audio_record_control_deinit(g_reocrd_id)){
                            LOGMSGIDI("[AT][RECORD][CALLBACK]record deinit fail\r\n", 0);
                            return;
                        }
                        g_record_inited = false;
                        record_flag = !record_flag;
                    }
                }else {
                    LOGMSGIDI("[AT][RECORD][CALLBACK]read stream in failed\r\n", 0);
                }
            }
        }
        if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_OPUS){
            if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(&g_record_Rdemo_data, RECORD_OPUS_DATA_MAX_SIZE_U8)){
                    //LOG_AUDIO_DUMP(g_record_Rdemo_data, sizeof(int16_t) * 128, VOICE_TX_MIC_3);
                    LOGMSGIDI("[AT][RECORD][CALLBACK]read stream in opus\r\n", 0);
            }else {
                LOGMSGIDI("[AT][RECORD][CALLBACK]read stream in failed\r\n", 0);
            }
        }

        break;
    }

}

void record_control_AM_notify_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    LOGMSGIDI("[AT][RECORD]AM_CB, aud_id:%x, msg_id:%x, sub_msg:%x", 3, aud_id, msg_id, sub_msg);
    if (msg_id == AUD_SINK_OPEN_CODEC) {
        if(sub_msg == AUD_CMD_COMPLETE){
            //Start Record Success
            LOGMSGIDI("[AT][RECORD][AM CALLBACK]Start Record Success\r\n", 0);
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Fail
            LOGMSGIDI("[AT][RECORD][AM CALLBACK]Start Record Fail\r\n", 0);
        } else {
            //[error]
            LOGMSGIDI("[AT][RECORD][AM CALLBACK][error1]\r\n", 0);
        }
    }else if(msg_id == AUD_SELF_CMD_REQ){
        if(sub_msg == AUD_CMD_COMPLETE){
            //Stop Record Request Success
            LOGMSGIDI("[AT][RECORD][AM CALLBACK]Stop Record Request Success\r\n", 0);
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Request Fail because HFP exist. /*Reject Request.*/
            LOGMSGIDI("[AT][RECORD][AM CALLBACK]Start Record Request Fail because HFP exist.\r\n", 0);
        } else {
            //[error]
            LOGMSGIDI("[AT][RECORD][AM CALLBACK][error2]\r\n", 0);
        }
    }else if(msg_id == AUD_SUSPEND_BY_IND){
        if(sub_msg == AUD_SUSPEND_BY_HFP){
            //Suspend record because HFP interrupt.
            LOGMSGIDI("[AT][RECORD][AM CALLBACK]Suspend record because HFP interrupt\r\n", 0);
        } else {
            //[error]
            LOGMSGIDI("[AT][RECORD][AM CALLBACK][error3]\r\n", 0);
        }
    }else{
        //[error]
        LOGMSGIDI("[AT][RECORD][AM CALLBACK][error4]\r\n", 0);
    }
}

#endif

#if defined(MTK_USB_AUDIO_PLAYBACK_ENABLE) && defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))

#include "usb_audio_control.h"
#include "usbaudio_drv.h"

uint32_t g_user_data = 0xFFFFFFFF;

void audio_usb_audio_callback(audio_usb_audio_event_t event, void *user_data)
{
    LOGMSGIDI("[USB_PLAYBACK_CTL_CB]event = %d; user_data = %d.\r\n", 2, event, *((uint32_t *)user_data));
}

audio_usb_audio_config_t usb_audio_config = {
    audio_usb_audio_callback,
    &g_user_data
};

static void usb_audio_setinterface_cb(uint8_t bInterfaceNumber, uint8_t bAlternateSetting)
{
    LOGMSGIDI("[USB_PLAYBACK_CB]00_usb_audio_setinterface_cb() with bInterfaceNumber=%d & bAlternateSetting=%d", 2, bInterfaceNumber, bAlternateSetting);
    // For different conditions, we can start or stop relative audio device.

    if (bInterfaceNumber == 1 && bAlternateSetting == 0) {
        // bInterfaceNumber = 1 means speaker interface.
        // bAlternateSetting = 0 means USB audio stream is stopped.
        audio_usb_audio_control_stop();
        audio_usb_audio_control_deinit();

        //hal_gpt_delay_ms(100);
    } else if (bInterfaceNumber == 1 && bAlternateSetting == 1) {
        // bInterfaceNumber = 1 means speaker interface.
        // bAlternateSetting = 0 means USB audio stream is started.
        audio_usb_audio_control_init(&usb_audio_config);
        audio_usb_audio_control_start();
    }
#ifdef MTK_USB_AUDIO_MICROPHONE
    else if (bInterfaceNumber == 2 && bAlternateSetting == 0) {
        // bInterfaceNumber = 2 means microphone interface.
        // bAlternateSetting = 0 means USB audio stream is stopped.
    } else if (bInterfaceNumber == 2 && bAlternateSetting == 1) {
        // bInterfaceNumber = 2 means microphone interface.
        // bAlternateSetting = 0 means USB audio stream is started.
    }
#endif
}

static void usb_audio_setsamplingrate_cb(uint8_t ep_number, uint32_t sampling_rate)
{
    LOGMSGIDI("[USB_PLAYBACK_CB]11_usb_audio_setsamplingrate_cb() with sampling rate = %d.\r\n", 1, sampling_rate);

    if (ep_number == 0x01) {
        //set_audio_sampling_rate(sampling_rate);
    }
#ifdef MTK_USB_AUDIO_MICROPHONE
    else if (ep_number == 0x81) {
        //set_audio_microphone_sampling_rate(sampling_rate);
    }
#endif
}

static void usb_audio_volumechange_cb(uint8_t ep_number, uint8_t channel, uint32_t volume)
{
    LOGMSGIDI("[USB_PLAYBACK_CB]33_usb_audio_volumechange_cb() ep=0x%x channel=%d volume=0x%x.\r\n", 3, ep_number, channel, volume);

#if 0
    volume = volume / 6;
    uint32_t digital_gain = volume_gain_table[volume][0];
    uint32_t analog_gain = volume_gain_table[volume][1];

    hal_audio_set_stream_out_volume(digital_gain, analog_gain);
#else
    audio_usb_audio_control_set_volume(volume);
#endif
}

static void usb_audio_mute_cb(uint8_t ep_number, usb_audio_mute_t mute)
{
    LOGMSGIDI("[USB_PLAYBACK_CB]44_usb_audio_mute_cb() ep=0x%x mute=%d.\r\n", 2, ep_number, mute);

#if 0
    hal_audio_mute_stream_out(mute, HAL_AUDIO_STREAM_OUT1);

#else
    if (mute) {
        audio_usb_audio_control_mute();
    } else {
        audio_usb_audio_control_unmute();
    }

#endif
}


static void usb_audio_unplug_cb(void)
{
    LOGMSGIDI("[USB_PLAYBACK_CB]55_usb_audio_unplug_cb().\r\n", 0);

    audio_usb_audio_control_stop();
    audio_usb_audio_control_deinit();
}


void audio_usb_test()
{
    USB_Audio_Register_SetInterface_Callback(0, usb_audio_setinterface_cb);
    USB_Audio_Register_Unplug_Callback(0, usb_audio_unplug_cb);
    USB_Audio_Register_SetSamplingRate_Callback(0, usb_audio_setsamplingrate_cb);
    USB_Audio_Register_VolumeChange_Callback(0, usb_audio_volumechange_cb);
    USB_Audio_Register_Mute_Callback(0, usb_audio_mute_cb);
}

#endif

#if defined(MTK_USB_AUDIO_RECORD_ENABLE) && defined(MTK_RECORD_INTERLEAVE_ENABLE)
#include "usbaudio_drv.h"
#include "usb_audio_record.h"

static void usb_audio_setinterface_cb(uint8_t bInterfaceNumber, uint8_t bAlternateSetting)
{
    LOGMSGIDI("[USB_RECORD_CB]usb_audio_setinterface_cb() with bInterfaceNumber=%d & bAlternateSetting=%d", 2, bInterfaceNumber, bAlternateSetting);
    // For different conditions, we can start or stop relative audio device.

    if (bInterfaceNumber == 1 && bAlternateSetting == 0) {
        // bInterfaceNumber = 1 means speaker interface.
        // bAlternateSetting = 0 means USB audio stream is stopped.
    } else if (bInterfaceNumber == 1 && bAlternateSetting == 1) {
        // bInterfaceNumber = 1 means speaker interface.
        // bAlternateSetting = 0 means USB audio stream is started.
    }
#ifdef MTK_USB_AUDIO_RECORD_ENABLE
    else if (bInterfaceNumber == 2 && bAlternateSetting == 0) {
        // bInterfaceNumber = 2 means microphone interface.
        // bAlternateSetting = 0 means USB audio stream is stopped.
        UAC_Record_Stop();
        UAC_Record_Close();
    } else if (bInterfaceNumber == 2 && bAlternateSetting == 1) {
        // bInterfaceNumber = 2 means microphone interface.
        // bAlternateSetting = 0 means USB audio stream is started.
        UAC_Record_Open();
        UAC_Record_Start();
    }
#endif
}

static void usb_audio_unplug_cb(void)
{
    LOGMSGIDI("[USB_RECORD_CB]usb_audio_unplug_cb().\r\n", 0);
    UAC_Record_Stop();
    UAC_Record_Close();
}
#endif //MTK_USB_AUDIO_RECORD_ENABLE, MTK_RECORD_INTERLEAVE_ENABLE

#if defined(MTK_PROMPT_SOUND_ENABLE)
#if defined(MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE)
void at_voice_prompt_callback(prompt_control_event_t event_id)
{
    if (event_id == PROMPT_CONTROL_MEDIA_END) {
        LOGMSGIDI("At_command voice prompt stop callback.", 0);
    }
}
#endif
#endif

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#include "audio_nvdm_common.h"
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#include "user_trigger_adaptive_ff.h"
#include "apps_config_vp_manager.h"
#endif

#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
static void at_anc_user_trigger_ff_cb(int32_t Cal_status)
{
    LOGMSGIDI("[user_trigger_ff]at_anc_user_trigger_ff_cb, Cal_status:%d", 1, Cal_status);

    audio_anc_user_trigger_ff_stop();

    /*turn on ANC if needed*/
    audio_anc_user_trigger_ff_recover_anc(ANC_K_STATUS_KEEP_DEFAULT);

    g_dump = false;
}
#elif 0
static void at_user_trigger_adaptive_ff_vp_callback(void)
{
    LOGMSGIDI("[user_trigger_ff] user_trigger_adaptive_ff_vp_callback", 0);
    apps_config_set_voice(20, true, 200, 3, true, false, NULL);
}

#endif
#endif
#endif
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
void am_vendor_se_callback(vendor_se_event_t event, void *arg)
{
    LOGMSGIDI("[VENDOR_SE]am_vendor_se_callback: event = %d, arg = 0x%08x  \n", 2, (int32_t)event, (int32_t)arg);
}

void vendor_se_ut_test(void)
{
    static uint32_t vendor_se_arg = 0x12345678;
    int32_t vendor_se_id_0 = -1;
    int32_t vendor_se_id_1 = -1;
    int32_t vendor_se_id_2 = -1;
    vendor_se_id_0 = ami_get_vendor_se_id();
    ami_register_vendor_se(vendor_se_id_0, am_vendor_se_callback);
    ami_set_vendor_se(vendor_se_id_0, &vendor_se_arg);

    vendor_se_id_1 = ami_get_vendor_se_id();
    ami_register_vendor_se(vendor_se_id_1, am_vendor_se_callback);
    ami_set_vendor_se(vendor_se_id_1, &vendor_se_arg);

    vendor_se_id_2 = ami_get_vendor_se_id();
    ami_set_vendor_se(vendor_se_id_2, &vendor_se_arg);

    ami_set_vendor_se(vendor_se_id_1, &vendor_se_arg);
    ami_set_vendor_se(vendor_se_id_0, &vendor_se_arg);
}
#endif
// AT command handler
atci_status_t atci_cmd_hdlr_audio(atci_parse_cmd_param_t *parse_cmd)
{
#if defined(__GNUC__)
    atci_response_t *presponse = NULL;
    presponse = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGMSGIDE("memory malloced failed.\r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    LOGMSGIDI("atci_cmd_hdlr_audio \r\n", 0);

    memset(presponse, 0, sizeof(atci_response_t));
    presponse->response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:{    // rec: AT+ECMP=?
            char msg[] = "+EAUDIO =<op>[,<param>]\r\nOK\r\n";
            strncpy((char * restrict)presponse->response_buf, msg, sizeof(msg)-1);
            presponse->response_len = strlen((const char *)presponse->response_buf);
            atci_send_response(presponse);
            break;
        }
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EAUDIO=<op>  the handler need to parse the parameters
            if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DUMMY") != NULL) {
                LOGMSGIDI("This is a summy command in order to have \"if\" description for if loop\r\n", 0);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);

            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=CHANGE_HWIO_MIC_TYPE_SELECT") != NULL) {
                /*MIC Type: 0 and others, AMIC; 1,DMIC.*/
                /*WARNING: You need to confirm what mic hardware and settings exist in your system first. It would change SW HWIO behavior.(Would change NVDM Value.)*/
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                char* configMIC_s = NULL;
                uint16_t MIC_Select;

                configMIC_s = strchr((char *)parse_cmd->string_ptr, ',');
                configMIC_s++;
                sscanf(configMIC_s, "%hd", &MIC_Select);
                if(MIC_Select == 1){
                    LOGMSGIDI("Set HAL_AUDIO_DEVICE_DIGITAL_MIC_L.\r\n" ,0);
                    if(AUD_EXECUTION_SUCCESS != ami_set_audio_device( STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_DIGITAL_MIC_L, HAL_AUDIO_INTERFACE_1, REWRITE)){
                        LOGMSGIDE("Set HAL_AUDIO_DEVICE_DIGITAL_MIC_L error\r\n" ,0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                } else {
                    LOGMSGIDI("Set HAL_AUDIO_DEVICE_MAIN_MIC_L.\r\n" ,0);
                    if(AUD_EXECUTION_SUCCESS != ami_set_audio_device( STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_MAIN_MIC_L, HAL_AUDIO_INTERFACE_1, REWRITE)){
                        LOGMSGIDE("Set HAL_AUDIO_DEVICE_MAIN_MIC_L error\r\n" ,0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF_SET_PARAM") != NULL) {
                char* config_s = NULL;
                anc_user_trigger_adaptive_ff_param_nvdm_t adaptive_ff_parameters_nvdm;
                uint32_t length;
                sysram_status_t status;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adaptive_ff_parameters_nvdm.FF_Noise_thd);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adaptive_ff_parameters_nvdm.FB_Noise_thd);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adaptive_ff_parameters_nvdm.stepsize);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adaptive_ff_parameters_nvdm.Shaping_switch);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adaptive_ff_parameters_nvdm.time_end);

                length = sizeof(anc_user_trigger_adaptive_ff_param_nvdm_t);

                status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, length);
                if(status != NVDM_STATUS_NAT_OK) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, &length);

                sprintf((char *)presponse->response_buf,"read FF_Noise_thd:%d, FB_Noise_thd:%d, stepsize:%d, Shaping_switch:%d, time_end:%d\r\n", adaptive_ff_parameters_nvdm.FF_Noise_thd, adaptive_ff_parameters_nvdm.FB_Noise_thd, adaptive_ff_parameters_nvdm.stepsize, adaptive_ff_parameters_nvdm.Shaping_switch, adaptive_ff_parameters_nvdm.time_end);
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM") != NULL) {

                audio_anc_user_trigger_ff_read_filter_nvdm(FILTER_3);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF") != NULL) {
//                g_dump = true;
                audio_anc_user_trigger_ff_start(at_anc_user_trigger_ff_cb);
//                hal_gpt_delay_ms(6000);
//                audio_anc_user_trigger_ff_stop();
//                g_dump = false;

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#else
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF_PZ_SET_PARAM") != NULL) {

                user_trigger_adaptive_ff_param_nvdm_t adaptive_ff_parameters_nvdm;
                uint32_t length;
                sysram_status_t status;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                length = sizeof(user_trigger_adaptive_ff_param_nvdm_t);

                status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, &length);
                if (status != NVDM_STATUS_NAT_OK) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    LOGMSGIDE("[user_trigger_ff]Read param nvdm error\r\n" ,0);
                } else {
                    char* config_s = NULL;
                    config_s = strchr((char *)parse_cmd->string_ptr, ',');
                    config_s++;
                    sscanf(config_s, "%hhd", &adaptive_ff_parameters_nvdm.mode_setting);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.PZ_thd);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.PZ_VAD_control);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.PZ_cal_time);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.PZ_offset);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.PZ_FB_delay);

                    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, length);
                    if(status != NVDM_STATUS_NAT_OK) {
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }

                sprintf((char *)presponse->response_buf,"set PZ param: mode_setting:%d, PZ_thd:%d, PZ_VAD_control:%d, PZ_cal_time:%d, PZ_offset:%d, FB_delay:%d\r\n",
                adaptive_ff_parameters_nvdm.mode_setting,
                adaptive_ff_parameters_nvdm.PZ_thd,
                adaptive_ff_parameters_nvdm.PZ_VAD_control,
                adaptive_ff_parameters_nvdm.PZ_cal_time,
                adaptive_ff_parameters_nvdm.PZ_offset,
                adaptive_ff_parameters_nvdm.PZ_FB_delay);


                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF_SZ_SET_PARAM") != NULL) {

                char* config_s = NULL;
                user_trigger_adaptive_ff_param_nvdm_t adaptive_ff_parameters_nvdm;
                uint32_t length;
                sysram_status_t status;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                length = sizeof(user_trigger_adaptive_ff_param_nvdm_t);

                status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, &length);
                if (status != NVDM_STATUS_NAT_OK) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    LOGMSGIDE("[user_trigger_ff]Read param nvdm error\r\n" ,0);
                } else {
                    config_s = strchr((char *)parse_cmd->string_ptr, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.SZ_delay_i);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.SZ_thd);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.SZ_VAD_control);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.SZ_cal_time);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &adaptive_ff_parameters_nvdm.SZ_offset);

                    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF, (uint8_t *)&adaptive_ff_parameters_nvdm, length);
                    if(status != NVDM_STATUS_NAT_OK) {
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }

                sprintf((char *)presponse->response_buf,"set SZ param: SZ_delay_i:%d, SZ_thd:%d, SZ_VAD_control:%d, SZ_cal_time:%d, SZ_offset:%d\r\n",
                adaptive_ff_parameters_nvdm.SZ_delay_i,
                adaptive_ff_parameters_nvdm.SZ_thd,
                adaptive_ff_parameters_nvdm.SZ_VAD_control,
                adaptive_ff_parameters_nvdm.SZ_cal_time,
                adaptive_ff_parameters_nvdm.SZ_offset);


                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF_READ_FILTER_NVDM") != NULL) {

                typedef struct {
                    uint8_t rw_filter;
                    uint8_t *filter;
                } user_trigger_adaptive_ff_rw_anc_filter;
                user_trigger_adaptive_ff_rw_anc_filter rw_filter_cmd;

                rw_filter_cmd.rw_filter = USER_TRIGGER_FF_READ_ANC_COEF;
                rw_filter_cmd.filter = NULL;
                audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_UTFF, 0, &rw_filter_cmd);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_TRIGGER_FF") != NULL) {
                char* config_s = NULL;
                int mode;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');

                if(config_s == NULL){
                    LOGMSGIDE("[user_trigger_ff]error! need to assign mode.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                } else {
                    config_s++;
                    sscanf(config_s, "%d", &mode);

                    if ((mode < 0) || (mode > 2)) {
                        LOGMSGIDE("[user_trigger_ff]error mode!!.", 0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        presponse->response_len = strlen((const char *)presponse->response_buf);
                        atci_send_response(presponse);
                    } else {
                        //audio_user_trigger_adaptive_ff_register_vp_callback(at_user_trigger_adaptive_ff_vp_callback);

                        //audio_user_trigger_adaptive_ff_init();
                        //audio_user_trigger_adaptive_ff_start(NULL, mode);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        presponse->response_len = strlen((const char *)presponse->response_buf);
                        atci_send_response(presponse);
                    }
                }
            }
#endif
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LEAKAGE_COMPENSATION_SET_RECORD_DURATION") != NULL) {
                char* config_s = NULL;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &g_leakage_compensation_record_duration);
                if(g_leakage_compensation_record_duration > 30000){
                    g_leakage_compensation_record_duration = 30000;
                }

                sprintf((char *)presponse->response_buf,"SET_RECORD_DURATION %d ms,\r\n", g_leakage_compensation_record_duration);
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LEAKAGE_COMPENSATION_SET_PARAM") != NULL) {
                char* config_s = NULL;
                anc_leakage_compensation_parameters_nvdm_t leakage_compensation_parameters_nvdm;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int *)&leakage_compensation_parameters_nvdm.ld_thrd);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int *)&leakage_compensation_parameters_nvdm.RXIN_TXREF_DELAY);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int *)&leakage_compensation_parameters_nvdm.DIGITAL_GAIN);


                audio_anc_set_leakage_compensation_parameters_nvdm(&leakage_compensation_parameters_nvdm);
                audio_anc_read_leakage_compensation_parameters_nvdm();

                sprintf((char *)presponse->response_buf,"SET_PARAM ld_thrd:0x%x, RXIN_TXREF_DELAY:0x%x, DIGITAL_GAIN:0x%x\r\n", (int)leakage_compensation_parameters_nvdm.ld_thrd, (int)leakage_compensation_parameters_nvdm.RXIN_TXREF_DELAY, (int)leakage_compensation_parameters_nvdm.DIGITAL_GAIN);

                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
//#if defined(MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LEAKAGE_COMPENSATION") != NULL) {

                //temp VP
//                uint8_t *tone_buf = NULL;
//                uint32_t tone_size = 0;
//                static const uint8_t voice_prompt_mix_mp3_tone_connected[] = {
//                    #include "ANCenable.mp3.hex"
//                };
//                tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_connected;
//                tone_size = sizeof(voice_prompt_mix_mp3_tone_connected);

                LOGMSGIDI("[AT][RECORD_LC]Record start.", 0);

                audio_anc_leakage_compensation_set_status(1);

                audio_anc_leakage_compensation_start(NULL);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
//#endif
#endif
#ifdef MTK_RECORD_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=RECORD") != NULL) {
                LOGMSGIDI("[AT][RECORD]Record start.", 0);
                record_flag = !record_flag;
                LOGMSGIDI("[AT][RECORD]record_flag:%d\r\n", 1, record_flag);
                //if(g_reocrd_id == 0){
                if(g_record_inited == false){
                #ifdef MTK_RECORD_OPUS_ENABLE
                    record_encoder_cability_t encoder_capability;
                    encoder_capability.codec_type = AUDIO_DSP_CODEC_TYPE_OPUS;
                    encoder_capability.bit_rate = ENCODER_BITRATE_32KBPS;
                    encoder_capability.wwe_mode = WWE_MODE_NONE;
                    g_reocrd_id = audio_record_control_enabling_encoder_init(record_control_CCNI_demo_callback, NULL, record_control_AM_notify_callback,&encoder_capability);
                #else
                    g_reocrd_id = audio_record_control_init(record_control_CCNI_demo_callback, NULL, record_control_AM_notify_callback);
                #endif
                    g_record_inited = true;
                }
                LOGMSGIDI("[AT][RECORD]g_reocrd_id %x", 1, g_reocrd_id);

                record_control_result_t ami_ret;
                if(record_flag){
                    g_dump = true;
                    if(g_reocrd_id >= 0){
                        ami_ret = audio_record_control_start(g_reocrd_id);
                        LOGMSGIDI("[AT][RECORD]audio_record_control_start = %d \r\n", 1,ami_ret);
                    } else {
                        LOGMSGIDE("[AT][RECORD]Start: g_reocrd_id nagative error %x", 1, g_reocrd_id);
                    }
                }else{
                    if(g_reocrd_id >= 0){
                        ami_ret = audio_record_control_stop(g_reocrd_id);
                        LOGMSGIDI("[AT][RECORD]audio_record_control_stop = %d \r\n", 1,ami_ret);
                        audio_record_control_deinit(g_reocrd_id);
                        g_record_inited = false;
                    } else {
                        LOGMSGIDE("[AT][RECORD]Stop: g_reocrd_id nagative error %x", 1, g_reocrd_id);
                    }
                    g_dump = false;
                }

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=WWE") != NULL) {
                int wwe_mode = 0;
                char *config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&wwe_mode);
                LOGMSGIDI("[AT][RECORD]wwe_mode =  %d", 1, wwe_mode);

                if(wwe_mode != 0) {
                    LOGMSGIDI("[AT][RECORD]Record start.", 0);
                    record_flag = !record_flag;
                    LOGMSGIDI("[AT][RECORD]record_flag:%d\r\n", 1, record_flag);

                    //if(g_reocrd_id == 0){
                    if(g_record_inited == false){
                        record_encoder_cability_t encoder_capability;
                        encoder_capability.codec_type = AUDIO_DSP_CODEC_TYPE_PCM_WWE;
                        encoder_capability.bit_rate = ENCODER_BITRATE_16KBPS;
                        encoder_capability.wwe_mode = wwe_mode;
                        #ifdef MTK_VA_MODEL_MANAGER_ENABLE
                        #include "va_model_manager.h"
                        va_model_manager_model_info_t model_info;
                        //AMA LM
                        if(wwe_mode == 1) {
                            va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_AMA, &model_info);
                        } else {//GSOUND LM
                            va_model_manager_get_current_running_model(VA_MODEL_MANAGER_VA_TYPE_GVA, &model_info);
                        }
                        encoder_capability.wwe_language_mode_address = model_info.model_flash_address;
                        encoder_capability.wwe_language_mode_length = model_info.model_length;
                        #else
                        encoder_capability.wwe_language_mode_address = 0x1234;
                        encoder_capability.wwe_language_mode_length = 0x5678;
                        #endif
                        g_reocrd_id = audio_record_control_enabling_encoder_init(record_control_CCNI_demo_callback, NULL, record_control_AM_notify_callback,&encoder_capability);
                        g_record_inited = true;
                    }
                    LOGMSGIDI("[AT][RECORD]g_reocrd_id %x", 1, g_reocrd_id);

                    record_control_result_t ami_ret;
                    if(record_flag){
                        g_dump = true;
                        if(g_reocrd_id >= 0){
                            ami_ret = audio_record_control_start(g_reocrd_id);
                            LOGMSGIDI("[AT][RECORD]audio_record_control_start = %d \r\n", 1,ami_ret);
                        } else {
                            LOGMSGIDE("[AT][RECORD]Start: g_reocrd_id nagative error %x", 1, g_reocrd_id);
                        }
                    }else{
                        if(g_reocrd_id >= 0){
                            ami_ret = audio_record_control_stop(g_reocrd_id);
                            LOGMSGIDI("[AT][RECORD]audio_record_control_stop = %d \r\n", 1,ami_ret);
                            audio_record_control_deinit(g_reocrd_id);
                            g_record_inited = false;
                        } else {
                            LOGMSGIDE("[AT][RECORD]Stop: g_reocrd_id nagative error %x", 1, g_reocrd_id);
                        }
                        g_dump = false;
                    }
                } else {
                    g_dump = !g_dump;
                    if(g_dump) {
                        LOGMSGIDI("[AT][RECORD]Record dump is enabled.", 0);
                    } else {
                        LOGMSGIDI("[AT][RECORD]Record dump is disabled.", 0);
                    }
                }

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #if defined(MTK_USB_AUDIO_RECORD_ENABLE) && defined(MTK_RECORD_INTERLEAVE_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USB_AUDIO_RECORD") != NULL) {

                USB_Audio_Register_Mic_SetInterface_Callback(usb_audio_setinterface_cb);
                USB_Audio_Register_Mic_Unplug_Callback(usb_audio_unplug_cb);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif //MTK_USB_AUDIO_RECORD_ENABLE, MTK_RECORD_INTERLEAVE_ENABLE
            #if defined(MTK_USB_AUDIO_PLAYBACK_ENABLE) && defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USB_AUDIO") != NULL) {

                audio_usb_test();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
             else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=REC_SET_SR") != NULL) {
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    LOGMSGIDE("Set REC sample rate NULL error.", 0);
                } else {
                    chr++;
                    if(strstr((char *)chr, "16K") != NULL){
                        g_stream_in_sample_rate = 16000;
                        LOGMSGIDI("[AT][RECORD]Set REC sample rate: %d.", 1, g_stream_in_sample_rate);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    else if(strstr((char *)chr, "32K") != NULL){
                        g_stream_in_sample_rate = 32000;
                        LOGMSGIDI("[AT][RECORD]Set REC sample rate: %d.", 1, g_stream_in_sample_rate);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    else if(strstr((char *)chr, "48K") != NULL){
                        g_stream_in_sample_rate = 48000;
                        LOGMSGIDI("[AT][RECORD]Set REC sample rate: %d.", 1, g_stream_in_sample_rate);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    else {
                        LOGMSGIDE("[AT][RECORD]Set REC sample rate error.", 0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

#endif
#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=USER_UNAWARE_SET_PARA") != NULL) {
                typedef struct {
                    U8 ENABLE;                      /**< @Value   0x01 @Desc 1 */
                    U8 REVISION;                    /**< @Value   0x01 @Desc 1 */

                    S16 alpha_par;                /**< @Value      3278 @Desc 3278*/
                    S32 thd;                      /**< @Value      13631488 @Desc 13631488 */
                    S32 switch_par;

                } PACKED param_t;

                char* config_s = NULL;
                param_t UUpara;
                sysram_status_t status;
                uint32_t length = sizeof(param_t);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_USR_UNAWARE, (uint8_t *)&UUpara, &length);
                if (status != NVDM_STATUS_NAT_OK) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    LOGMSGIDE("[AT][User Unaware]Read param nvdm error\r\n" ,0);
                } else {
                    config_s = strchr((char *)parse_cmd->string_ptr, ',');
                    config_s++;
                    sscanf(config_s, "%hd", &UUpara.alpha_par);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%d", (int*)&UUpara.thd);

                    config_s = strchr(config_s, ',');
                    config_s++;
                    sscanf(config_s, "%d", (int*)&UUpara.switch_par);

                    LOGMSGIDI("[AT][User Unaware]Set PARA: alpha_par:%d, thd:%d, switch:%d", 3, UUpara.alpha_par, UUpara.thd, UUpara.switch_par);
                    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_USR_UNAWARE, (uint8_t *)&UUpara, length);
                    if(status != NVDM_STATUS_NAT_OK) {
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif

#if defined(AT_CMD_CONTROL_ADAPTIVE_ANC_STREAM)
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ADAPTIVE_ANC_STREAM") != NULL){
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                } else {
                    chr++;
                    if(strstr((char *)chr, "OPEN") != NULL){
                        LOGMSGIDI("[AT][Adaptive ANC] Open Adaptive ANC Stream.", 0);
                        audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_STREAM_CONTROL, TRUE);
                        audio_anc_monitor_anc_callback(AUDIO_ANC_CONTROL_EVENT_ON, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    else if(strstr((char *)chr, "CLOSE") != NULL){
                        LOGMSGIDI("[AT][Adaptive ANC] Close Adaptive ANC Stream.", 0);
                        audio_anc_monitor_set_info(AUDIO_ANC_MONITOR_STREAM_CONTROL, FALSE);
                        audio_anc_monitor_anc_callback(AUDIO_ANC_CONTROL_EVENT_OFF, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    else {
                        LOGMSGIDE("[AT][Adaptive ANC] Adaptive ANC AT cmd error.", 0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
#endif

#endif

#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=MULTIMIC_SEL") != NULL) {
                char* config_s = NULL;
                bool is_modified = FALSE;
                uint16_t mic_no,interface;
                uint32_t device;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hu", &mic_no);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%u", &device);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hu", &interface);

                switch (mic_no) {
                    case 1:
                        audio_multi_instance_ctrl.audio_device = device;
                        audio_multi_instance_ctrl.audio_interface = interface;
                        is_modified = TRUE;
                        break;

                    case 2:
                        audio_multi_instance_ctrl.audio_device1 = device;
                        audio_multi_instance_ctrl.audio_interface1 = interface;
                        is_modified = TRUE;
                        break;

                    case 3:
                        audio_multi_instance_ctrl.audio_device2 = device;
                        audio_multi_instance_ctrl.audio_interface2 = interface;
                        is_modified = TRUE;
                        break;

                    case 4:
                        audio_multi_instance_ctrl.audio_device3 = device;
                        audio_multi_instance_ctrl.audio_interface3 = interface;
                        is_modified = TRUE;
                        break;

                    case 99:
                        if ((device == 1) && (interface == 1)) {
                            audio_multi_instance_ctrl.echo_path_enabled = TRUE;
                        } else {
                            audio_multi_instance_ctrl.echo_path_enabled = FALSE;
                        }
                        is_modified = TRUE;
                        break;

                    default:
                        LOGMSGIDI("[MULTIMIC_SEL] Unsupported Input Instance \r\n", 0);
                        break;

                }

                if (is_modified) {
                    audio_multi_instance_ctrl.is_modified = TRUE;
                }

                if (mic_no == 99) {
                    LOGMSGIDI("[MULTIMIC_SEL] Echo Path Enabled:%d \r\n", 1, audio_multi_instance_ctrl.echo_path_enabled);
                } else {
                    LOGMSGIDI("[MULTIMIC_SEL] No:%d, Device:%d, Interface:%d \r\n", 3, mic_no, device, interface);
                }

                presponse->response_flag = (is_modified) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=MULTIMIC_RESET") != NULL) {

                audio_multi_instance_ctrl.is_modified = FALSE;
                LOGMSGIDI("[MULTIMIC_SEL] Reset Input Instance! \r\n", 0);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif // HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
#endif //((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
    #ifdef HAL_AUDIO_DSP_SHUTDOWN_SPECIAL_CONTROL_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_DSP_OFF") != NULL) {
                LOGMSGIDI("DSP OFF.", 0);
                hal_audio_deinit();
                LOGMSGIDI("HAL Deinit end.", 0);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_DSP_ON") != NULL) {
                LOGMSGIDI("DSP ON.", 0);
                hal_audio_init();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
    #endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_DEVICE_LEFT") != NULL) {
                LOGMSGIDI("Set device role: LEFT.", 0);
                if(AUD_EXECUTION_SUCCESS != ami_set_audio_channel(AUDIO_CHANNEL_NONE, AUDIO_CHANNEL_L, REWRITE)){
                    LOGMSGIDE("Set L_Channel error.", 0);
                }

#ifdef AIR_LE_AUDIO_ENABLE
                ble_pacs_set_audio_location(AUDIO_DIRECTION_SINK, AUDIO_LOCATION_FRONT_LEFT);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_DEVICE_RIGHT") != NULL) {
                LOGMSGIDI("Set device role: RIGHT.", 0);
                if(AUD_EXECUTION_SUCCESS != ami_set_audio_channel(AUDIO_CHANNEL_NONE, AUDIO_CHANNEL_R, REWRITE)){
                    LOGMSGIDE("Set R_Channel error.", 0);
                }
#ifdef AIR_LE_AUDIO_ENABLE
                ble_pacs_set_audio_location(AUDIO_DIRECTION_SINK, AUDIO_LOCATION_FRONT_RIGHT);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_OUTPUT_DEVICE,") != NULL) {
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    LOGMSGIDE("Set output device NULL error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                } else {
                    chr++;
                    if(strstr((char *)chr, "L") != NULL){
                        LOGMSGIDI("Set audio output device LEFT",0);
                        if((AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_AUDIO, HAL_AUDIO_DEVICE_DAC_L,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_VOICE, HAL_AUDIO_DEVICE_DAC_L,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_ANC, HAL_AUDIO_DEVICE_DAC_L,HAL_AUDIO_INTERFACE_1,REWRITE))){
                            LOGMSGIDE("Set audio output device LEFT error.", 0);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }else {
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        }
                    }else if (strstr((char *)chr, "R") != NULL) {
                        LOGMSGIDI("Set audio output device RIGHT",0);
                        if((AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_AUDIO, HAL_AUDIO_DEVICE_DAC_R,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_VOICE, HAL_AUDIO_DEVICE_DAC_R,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_ANC, HAL_AUDIO_DEVICE_DAC_R,HAL_AUDIO_INTERFACE_1,REWRITE))){
                            LOGMSGIDE("Set audio output device RIGHT error.", 0);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }else {
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        }
                    }else if (strstr((char *)chr, "Dual") != NULL) {
                        LOGMSGIDI("Set audio output device Dual",0);
                        if((AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_AUDIO, HAL_AUDIO_DEVICE_DAC_DUAL,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_VOICE, HAL_AUDIO_DEVICE_DAC_DUAL,HAL_AUDIO_INTERFACE_1,REWRITE))||
                           (AUD_EXECUTION_SUCCESS != ami_set_audio_device(STREAM_OUT, AU_DSP_ANC, HAL_AUDIO_DEVICE_DAC_DUAL,HAL_AUDIO_INTERFACE_1,REWRITE))){
                            LOGMSGIDE("Set audio output device Dual error.", 0);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }else {
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                        }
                    }
                    else {
                        LOGMSGIDE("Set audio output device error", 0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                }
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_MIC_CHANNEL,") != NULL) {
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    LOGMSGIDE("Set MIC_Channel NULL error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                } else {
                    chr++;
                    if(strstr((char *)chr, "L") != NULL){
                        LOGMSGIDI("Set MIC channel: LEFT.", 0);
                        if(AUD_EXECUTION_SUCCESS != ami_set_audio_channel(AUDIO_CHANNEL_NONE, AUDIO_CHANNEL_L, REWRITE)){
                            LOGMSGIDE("Set L_Channel error.", 0);
                        }
                        if(AUD_EXECUTION_SUCCESS != ami_set_audio_device( STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_MAIN_MIC_L, HAL_AUDIO_INTERFACE_1, REWRITE)){
                            LOGMSGIDE("Set MAIN_MIC_L error\r\n", 0);
                        }
                    } else if(strstr((char *)chr, "R") != NULL){
                        LOGMSGIDI("Set MIC channel: RIGHT.", 0);
                        if(AUD_EXECUTION_SUCCESS != ami_set_audio_channel(AUDIO_CHANNEL_SWAP, AUDIO_CHANNEL_R, REWRITE)){
                            LOGMSGIDE("Set R_Channel error.", 0);
                        }
                        if(AUD_EXECUTION_SUCCESS != ami_set_audio_device( STREAM_IN, AU_DSP_VOICE, HAL_AUDIO_DEVICE_MAIN_MIC_R, HAL_AUDIO_INTERFACE_1, REWRITE)){
                            LOGMSGIDE("Set MAIN_MIC_R error\r\n", 0);
                        }
                    } else {
                        LOGMSGIDE("SET_MIC_CHANNEL error", 0);
                    }
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                }
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SET_SIDETONE,") != NULL) {
                char *chr = NULL;
                uint32_t HFP_gain;
                nvdm_status_t result;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    LOGMSGIDE("Set SET_SIDETONE NULL error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                } else {
                    chr++;
                    if(strstr((char *)chr, "ENABLE") != NULL){
                        LOGMSGIDI("Set SET_SIDETONE: ENABLE.", 0);
                        chr= strchr(chr, ',');
                        chr++;
                        sscanf(chr, "%lu", &HFP_gain);
                        // HFP side tone Enable
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.Voice_Sidetone_EN = 0x01;
                            audio_nvdm_HW_config.Voice_Sidetone_Gain = (uint8_t)((int32_t)HFP_gain/100);
                        #else
                            audio_nvdm_HW_config.voice_scenario.Voice_Side_Tone_Enable = 0x01;
                            audio_nvdm_HW_config.sidetone_config.SideTone_Gain = (uint8_t)((int32_t)HFP_gain/100);
                        #endif
                        result = nvkey_write_data(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, (const uint8_t *)&audio_nvdm_HW_config, sizeof(audio_nvdm_HW_config));
                        if(result){
                            LOGMSGIDI("Audio Set SET_SIDETONE NVDM write error", 0);
                        }
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else if(strstr((char *)chr, "DISABLE") != NULL){
                        LOGMSGIDI("Set SET_SIDETONE: DISABLE.", 0);
                        // HFP side tone disable
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.Voice_Sidetone_EN = 0x00;
                        #else
                            audio_nvdm_HW_config.voice_scenario.Voice_Side_Tone_Enable = 0x00;
                        #endif
                        result = nvkey_write_data(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, (const uint8_t *)&audio_nvdm_HW_config, sizeof(audio_nvdm_HW_config));
                        if(result){
                            LOGMSGIDI("Audio Set SET_SIDETONE NVDM write error", 0);
                        }
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        LOGMSGIDE("SET_SIDETONE error", 0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }

                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_RG_DUMP") != NULL) {
                uint16_t i;
                uint32_t RG_INDEX = 0x70000000;
                for(i = 205;i>0;i--){
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX,
                        *(volatile uint32_t *)(RG_INDEX),      *(volatile uint32_t *)(RG_INDEX + 1) , *(volatile uint32_t *)(RG_INDEX + 2) , *(volatile uint32_t *)(RG_INDEX + 3) , *(volatile uint32_t *)(RG_INDEX + 4),
                        *(volatile uint32_t *)(RG_INDEX + 5) , *(volatile uint32_t *)(RG_INDEX + 6) , *(volatile uint32_t *)(RG_INDEX + 7) , *(volatile uint32_t *)(RG_INDEX + 8) , *(volatile uint32_t *)(RG_INDEX + 9));
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX + 10,
                        *(volatile uint32_t *)(RG_INDEX + 10), *(volatile uint32_t *)(RG_INDEX + 11), *(volatile uint32_t *)(RG_INDEX + 12), *(volatile uint32_t *)(RG_INDEX + 13), *(volatile uint32_t *)(RG_INDEX + 14),
                        *(volatile uint32_t *)(RG_INDEX + 15), *(volatile uint32_t *)(RG_INDEX + 16), *(volatile uint32_t *)(RG_INDEX + 17), *(volatile uint32_t *)(RG_INDEX + 18), *(volatile uint32_t *)(RG_INDEX + 19));
                    RG_INDEX += 20;
                    hal_gpt_delay_ms(2);
                }
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#if ((PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DIG_RG_DUMP") != NULL) {
                uint16_t i;
                uint32_t RG_INDEX = 0xC0000000;
                for(i = 205;i>0;i--){
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX,
                        *(volatile uint32_t *)(RG_INDEX),      *(volatile uint32_t *)(RG_INDEX + 1) , *(volatile uint32_t *)(RG_INDEX + 2) , *(volatile uint32_t *)(RG_INDEX + 3) , *(volatile uint32_t *)(RG_INDEX + 4),
                        *(volatile uint32_t *)(RG_INDEX + 5) , *(volatile uint32_t *)(RG_INDEX + 6) , *(volatile uint32_t *)(RG_INDEX + 7) , *(volatile uint32_t *)(RG_INDEX + 8) , *(volatile uint32_t *)(RG_INDEX + 9));
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX + 10,
                        *(volatile uint32_t *)(RG_INDEX + 10), *(volatile uint32_t *)(RG_INDEX + 11), *(volatile uint32_t *)(RG_INDEX + 12), *(volatile uint32_t *)(RG_INDEX + 13), *(volatile uint32_t *)(RG_INDEX + 14),
                        *(volatile uint32_t *)(RG_INDEX + 15), *(volatile uint32_t *)(RG_INDEX + 16), *(volatile uint32_t *)(RG_INDEX + 17), *(volatile uint32_t *)(RG_INDEX + 18), *(volatile uint32_t *)(RG_INDEX + 19));
                    RG_INDEX += 20;
                    hal_gpt_delay_ms(2);
                }
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_ANA_RG_DUMP") != NULL) {
                uint16_t i;
                uint32_t RG_INDEX = 0xA2070000;
                for(i = 205;i>0;i--){
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX,
                        *(volatile uint32_t *)(RG_INDEX),      *(volatile uint32_t *)(RG_INDEX + 1) , *(volatile uint32_t *)(RG_INDEX + 2) , *(volatile uint32_t *)(RG_INDEX + 3) , *(volatile uint32_t *)(RG_INDEX + 4),
                        *(volatile uint32_t *)(RG_INDEX + 5) , *(volatile uint32_t *)(RG_INDEX + 6) , *(volatile uint32_t *)(RG_INDEX + 7) , *(volatile uint32_t *)(RG_INDEX + 8) , *(volatile uint32_t *)(RG_INDEX + 9));
                    LOGMSGIDI("[RG dump index(0x%x)](0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x),(0x%x)", 11, RG_INDEX + 10,
                        *(volatile uint32_t *)(RG_INDEX + 10), *(volatile uint32_t *)(RG_INDEX + 11), *(volatile uint32_t *)(RG_INDEX + 12), *(volatile uint32_t *)(RG_INDEX + 13), *(volatile uint32_t *)(RG_INDEX + 14),
                        *(volatile uint32_t *)(RG_INDEX + 15), *(volatile uint32_t *)(RG_INDEX + 16), *(volatile uint32_t *)(RG_INDEX + 17), *(volatile uint32_t *)(RG_INDEX + 18), *(volatile uint32_t *)(RG_INDEX + 19));
                    RG_INDEX += 20;
                    hal_gpt_delay_ms(2);
                }
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_HWGAIN_SET_FADE_TIME_AND_GAIN,") != NULL) {
                char *config_s = strchr((char *)parse_cmd->string_ptr, ',');
                uint32_t fade_time,gain_index,data32,gain_select;
                config_s++;
                sscanf(config_s, "%d", (int*)&gain_select);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&fade_time);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&gain_index);

                LOGMSGIDI("AUD_HWGAIN_SET_FADE_TIME_AND_GAIN,gain_select = %d,fade_time = %d,gain_index = 0x%x", 3,gain_select,fade_time,gain_index);
                data32 = (fade_time <<16) | (gain_index & 0xFFFF);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_HWGAIN_SET_FADE_TIME_GAIN, gain_select, data32, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
    #if MTK_SMT_AUDIO_TEST
            else if (strstr((char*)parse_cmd->string_ptr, "AT+EAUDIO=SPEAKER")) {
                char *pch = NULL;
                char *param[3] = {NULL};
                pch = strtok(parse_cmd->string_ptr,"=");
                if (!pch) {
                    sprintf((char *)presponse->response_buf,"parameter error\r\n");
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    goto end ;
                }
                char **idx = param;
                uint32_t cnt = 0;
                for (;cnt<3;cnt++) {
                    pch = strtok(NULL,",");
                    *(idx++) = pch;
                }
                char *active = param[2];
                if (strstr(active,"Play_start")) {
                    switch(*param[1]) {
                    case 'L':
                        audio_smt_test_pure_on_off(true,SMT_CH_LEFT);
                        break;
                    case 'R':
                        audio_smt_test_pure_on_off(true,SMT_CH_RIGHT);
                        break;
                    default:
                        break;
                    }
                    hal_gpt_delay_ms(100);
                } else if(strstr(active,"Play_stop")) {
                    switch(*param[1]) {
                    case 'L':
                        audio_smt_test_pure_on_off(false,SMT_CH_LEFT);
                        break;
                    case 'R':
                        audio_smt_test_pure_on_off(false,SMT_CH_RIGHT);
                        break;
                    default:
                        break;
                    }
                    hal_gpt_delay_ms(100);
                } else {
                    LOGMSGIDE("AT+EAUDIO=SPEAKER,unvalid cmd\r\n", 0);
                }
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                end:
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

#ifdef AIR_ATA_TEST_ENABLE
            /*AT+EAUDIO=1KTone_check,DMIC,(0/1/2/3/4/5)\0d\0a
              AT+EAUDIO=1KTone_check,AMIC,(0/1/2/3/4/5),0\0d\0a    //ACC10k
              AT+EAUDIO=1KTone_check,AMIC,(0/1/2/3/4/5),1\0d\0a    //ACC20k
              AT+EAUDIO=1KTone_check,AMIC,(0/1/2/3/4/5),2\0d\0a    //DCC
              AT+EAUDIO=1KTone_check,help\0d\0a                    //print all support cm */
            else if((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=1KTone_check,") != NULL))
            {
                char* config_s = NULL;
                bool help_flag = false;
                uint32_t adc_id;
                uint32_t amic_mode; // ACC10k = 0, ACC20k = 1, DCC = 2,
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                fft_buf_t *fft_bufs = NULL;
                /*1. create and init fft_bufs*/
                if (fft_bufs == NULL) {
                    LOGMSGIDI("ATA test, create fft_bufs\r\n", 0);
                    fft_bufs = (fft_buf_t *)pvPortMalloc(sizeof(fft_buf_t));
                }
                //command help
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s ++;
                if(strstr(config_s, "help") != NULL){
                    help_flag = true;
                    goto ata_end;
                }

                /*2. setting mic device and adc*/
                if (fft_bufs != NULL) {
                    hal_audio_init_stream_buf(fft_bufs);
                    if(strstr(config_s, "AMIC,") != NULL) {
                        //parse atci cmd to get adc_id(0/1/2/3/4/5) and amic_mode(0/1/2)
                        sscanf(config_s, "%*[^,],%d,%d", &adc_id, &amic_mode);
                        if((adc_id > 5)||(adc_id < 0)||(amic_mode > 2)||(amic_mode < 0)){
                            LOGMSGIDI("ATA test, AMIC param error, adc_id %d, amic_mode %d\r\n", 2, adc_id, amic_mode);
                            help_flag = true;
                            goto ata_end;
                        }
                        switch(amic_mode){ //switch to hal definition @ hal_platform.h
                            case 0: //ACC10k
                                g_ata_amic_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
                                break;
                            case 1: //ACC20k
                                g_ata_amic_mode = HAL_AUDIO_ANALOG_INPUT_ACC20K;
                                break;
                            case 2: //DCC
                                g_ata_amic_mode = HAL_AUDIO_ANALOG_INPUT_DCC;
                                break;
                        }
                        ata_test_set_mic(HAL_AUDIO_DEVICE_MAIN_MIC_DUAL, adc_id);
                        LOGMSGIDI("ATA test, AMIC adc %d %d, mode: %d\r\n", 3, adc_id, adc_id+1, amic_mode);
                    }else if(strstr(config_s, "DMIC,") != NULL){
                        sscanf(config_s, "%*[^,],%d", &adc_id);
                        if((adc_id > 5)||(adc_id < 0)){
                            LOGMSGIDI("ATA test, DMIC param error, adc_id %d\r\n", 1, adc_id);
                            help_flag = true;
                            goto ata_end;
                        }
                        LOGMSGIDI("ATA test, DMIC adc %d %d\r\n", 2, adc_id, adc_id+1);
                        ata_test_set_mic(HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL, adc_id);
                    }else{
                        LOGMSGIDI("ATA test, MIC type error\r\n", 0);
                        help_flag = true;
                        goto ata_end;
                    }

                    /*3. Turn on DL(prompt) and UL(record)*/
                    #ifdef MTK_MP3_TASK_DEDICATE
                        if(prompt_control_query_state()){
                            prompt_control_stop_tone();
                            for(int vp_i = 0;;vp_i++){
                                if(prompt_control_query_state() != true){
                                    break;
                                }
                                vTaskDelay(2 / portTICK_RATE_MS);
                            }
                        }
                    #endif
                    LOGMSGIDI("ATA test, collecting data begin\r\n", 0);
                    ata_test_ktone_DL(true);
                    ata_test_start_stream_in(true);

                    /*4. read data from stream_in to fft_bufs*/
                    uint32_t i=0;
                    while (1) {
                        if (i < (SMT_DROP_CNT)) { //55
                            hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE); //drop
                        } else if(i < SMT_SAVE) { //56
                            fft_bufs->cpyIdx = fft_bufs->cpyIdx + (FFT_BUFFER_SIZE >> 1);
                        } else if(i < SMT_UL_CNT_LIMIT) { //60
                            if (HAL_AUDIO_STATUS_OK == hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE)){
                                break;
                            }
                        } else {
                            break;
                        }
                        hal_gpt_delay_ms(5);
                        i++;
                    }

                    /*5. Turn off UL(record) & DL(prompt)*/
                    LOGMSGIDI("ATA test, collecting data completed\r\n", 0);
                    ata_test_start_stream_in(false);
                    ata_test_ktone_DL(false);

                    /*6. cal FFT*/
                    LOGMSGIDI("ATA test, calculating...\r\n", 0);
                    ApplyFFT256(fft_bufs->bitstream_buf, 0, &fft_bufs->u4Freq_data, &fft_bufs->u4Mag_data, 16000);
                    sprintf((char *)presponse->response_buf,"Freq=%d, Mag=%d, ",fft_bufs->u4Freq_data,fft_bufs->u4Mag_data); //print result
                    LOGMSGIDI("ATA test result, Freq=%d, Mag=%d \r\n", 2, fft_bufs->u4Freq_data,fft_bufs->u4Mag_data);

                    /*7. Response*/
                    if (FreqCheck(1000, fft_bufs->u4Freq_data)) {
                       presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    if (fft_bufs->u4Mag_data < 5000000) {
                       presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                }
                ata_end:
                    if(help_flag){ //print support cmds
                        sprintf((char *)presponse->response_buf,"need some help, check log, \r\nATA test, support cmd: AT+EAUDIO=1KTone_check,$Mic_TYPE,$par1(adc_id),$par2(amic_mode)\\0d\\0a \r\n");
                        LOGMSGIDI("ATA test, support cmd: =1KTone_check,$Mic_TYPE,$par1(adc_id),$par2(amic_mode)\0d\0a",0);
                        LOGMSGIDI("ATA test, ex1: AT+EAUDIO=1KTone_check,DMIC,(0/1/2/3/4/5)\0d\0a",0);
                        LOGMSGIDI("ATA test, ex2: AT+EAUDIO=1KTone_check,AMIC,(0/1/2/3/4/5),(0/1/2)\0d\0a",0);
                        presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    vPortFree(fft_bufs);//Free FFT buffer
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
            }
#else //not define AIR_ATA_TEST_ENABLE
            else if((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC-ACC,L,1KTone_Ckeck") != NULL) ||
                    (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC-ACC,R,1KTone_Ckeck") != NULL) ||
                    (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC-DCC,L,1KTone_Ckeck") != NULL) ||
                    (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC-DCC,R,1KTone_Ckeck") != NULL) ||
                    (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DMIC,L,1KTone_Ckeck") != NULL) ||
                    (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DMIC,R,1KTone_Ckeck") != NULL))
            {
                char *pch = NULL;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                char *param[3] = {NULL};
                char **idx = param;
                int i = 0;
                uint32_t Atcmd_value = *((volatile uint32_t*)(0xA2120B04));
                static fft_buf_t *fft_bufs = NULL;

                if (fft_bufs == NULL) {
                    LOGMSGIDI("ATA loopback create fft_bufs\r\n", 0);
                    fft_bufs = (fft_buf_t *)pvPortMalloc(sizeof(fft_buf_t));
                }
                if (fft_bufs != NULL) {
                    hal_audio_init_stream_buf(fft_bufs);

                    if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC") != NULL) {
                        ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, HAL_AUDIO_DEVICE_MAIN_MIC_DUAL, HAL_AUDIO_INTERFACE_1, NOT_REWRITE);
                        if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AMIC-ACC") != NULL) {
                            *((volatile uint32_t*)(0xA2120B04)) |= 0x10;
                        }
                    }

                    if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DMIC") != NULL) {
                        ami_set_audio_device(STREAM_IN, AU_DSP_RECORD, HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL, HAL_AUDIO_INTERFACE_1, NOT_REWRITE);
                    }

                    pch = strtok(parse_cmd->string_ptr, "=");
                    while (pch != NULL) {
                        for (i = 0; i < 2; i++) {
                            pch = strtok(NULL,",");
                            *(idx++) = pch;
                        }
                    }

                    if (strchr(param[1], 'L')) {
                        ami_set_audio_channel(AUDIO_CHANNEL_L, AUDIO_CHANNEL_NONE, NOT_REWRITE);
                    } else {
                        ami_set_audio_channel(AUDIO_CHANNEL_R, AUDIO_CHANNEL_NONE, NOT_REWRITE);
                    }
        #ifdef MTK_MP3_TASK_DEDICATE
                    if(prompt_control_query_state()){
                        prompt_control_stop_tone();
                        for(int vp_i = 0;;vp_i++){
                            if(prompt_control_query_state() != true){
                                break;
                            }
                            vTaskDelay(2 / portTICK_RATE_MS);
                        }
                    }
        #endif
                    KTONE_DL_ON;
                    hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);
                    i=0;
                    while (1) {
                        if (i < (SMT_DROP_CNT)) {
                            hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE); //drop
                        } else if(i < SMT_SAVE) {
                            fft_bufs->cpyIdx = fft_bufs->cpyIdx + (FFT_BUFFER_SIZE >> 1);
                        } else if(i < SMT_UL_CNT_LIMIT) {
                            if (HAL_AUDIO_STATUS_OK == hal_audio_read_stream_in(fft_bufs->cpyIdx, FFT_BUFFER_SIZE)){
                                break;
                            }
                        } else {
                            break;
                        }
                        hal_gpt_delay_ms(5);
                        i++;
                    }

                    hal_audio_stop_stream_in();
                    KTONE_DL_OFF;

                    ApplyFFT256(fft_bufs->bitstream_buf, 0, &fft_bufs->u4Freq_data, &fft_bufs->u4Mag_data, 16000);
                    if (FreqCheck(1000, fft_bufs->u4Freq_data)) {
                       presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                    if (fft_bufs->u4Mag_data < 5000000) {
                       presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                    sprintf((char *)presponse->response_buf,"Freq=%d, Mag=%d, ",fft_bufs->u4Freq_data,fft_bufs->u4Mag_data);
                    *((volatile uint32_t*)(0xA2120B04)) = Atcmd_value;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif //end ifdef(AIR_ATA_TEST_ENABLE)

            else if((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PASS_THROUGH_TEST_MUTE") != NULL)) {
                PTT_u4Freq_Mag_data result = {0, 0};
                pass_through_test_mic_side mic_side = PTT_L;
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    LOGMSGIDE("Set REC sample rate NULL error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                } else {
                    chr++;
                    if(strstr((char *)chr, "L") != NULL){
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST_MUTE]L mic", 0);
                        mic_side = PTT_L;
                    }
                    else if(strstr((char *)chr, "R") != NULL){
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST_MUTE]R mic", 0);
                        mic_side = PTT_R;
                    }
                    else{
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST_MUTE]error setting, default mic L", 0);
                    }

                    if(pass_through_test(PTT_AMIC_DCC, mic_side, PTT_MUTE, &result) == PTT_SUCCESS) {
                        sprintf((char *)presponse->response_buf,"PASS_THROUGH_TEST_MUTE, Freq=%d, Mag=%d, db=%lf\r\n", result.freq_data, result.mag_data, result.db_data);
                        presponse->response_len = strlen((const char *)presponse->response_buf);
                        atci_send_response(presponse);
                    }
                }
            }

            else if((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PASS_THROUGH_TEST") != NULL)) {
                PTT_u4Freq_Mag_data result = {0, 0};
                uint32_t PTT_test_cnt = 5;
                uint32_t u4FreqArray[] = {PTT_500HZ, PTT_1000HZ, PTT_2000HZ, PTT_4000HZ, PTT_6000HZ};
                pass_through_test_mic_side mic_side = PTT_L;
                char *chr = NULL;
                chr= strchr(parse_cmd->string_ptr, ',');
                if(chr == NULL){
                    LOGMSGIDE("Set REC sample rate NULL error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                } else {
                    chr++;
                    if(strstr((char *)chr, "L") != NULL){
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST]L mic", 0);
                        mic_side = PTT_L;
                    }
                    else if(strstr((char *)chr, "R") != NULL){
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST]R mic", 0);
                        mic_side = PTT_R;
                    }
                    else{
                        LOGMSGIDI("[AT][PASS_THROUGH_TEST]error setting, default mic L", 0);
                    }

                    for(int i=0; i<PTT_test_cnt; i++){
                        if(pass_through_test(PTT_AMIC_DCC, mic_side, u4FreqArray[i], &result) == PTT_SUCCESS) {
                            sprintf((char *)presponse->response_buf,"PASS_THROUGH_TEST, Freq=%d, Mag=%d, db=%lf\r\n", result.freq_data, result.mag_data, result.db_data);
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
                        }
                    }
                }
            }

    #endif
            #if defined(MTK_BUILD_SMT_LOAD)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SMT_ON") != NULL) {
                LOGMSGIDI("here", 0);

                audio_smt_test(1);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_SMT_OFF") != NULL) {

                audio_smt_test(0);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #if defined(MTK_PROMPT_SOUND_ENABLE)
	 else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VP_TEST_OFF") != NULL){
	     g_app_voice_prompt_test_off = true;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
	 }
           else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VP_TEST_ON") != NULL){
	     g_app_voice_prompt_test_off = false;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
	 }
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DUMMY_SOURCE_OPEN") != NULL) {
                LOGMSGIDI("VP/DmySrc OPEN.", 0);
              #if 1
                prompt_control_dummy_source_param_t DmySrc_param;
                DmySrc_param.index = 8;
                DmySrc_param.mode  = DUMMY_SOURCE_MODE_LOOP;
                DmySrc_param.vol_lev  = 255;
                prompt_control_dummy_source_start(DmySrc_param);
              #else
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);
	            bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, false, STREAM_OUT_3);
                prompt_control_dummy_source_set_volume(255);

                /*Open, Start.*/
                dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_OPEN;
                dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_START;
                audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT;

                void *p_param_share;
                // Collect parameters
                mcu2dsp_open_param_t open_param;
                memset(&open_param,0,sizeof(open_param));
                open_param.param.stream_in  = STREAM_IN_VP_DUMMY_SOURCE;
                open_param.param.stream_out = STREAM_OUT_AFE;

                open_param.stream_in_param.playback.bit_type       = /*HAL_AUDIO_BITS_PER_SAMPLING_16;//*/HAL_AUDIO_BITS_PER_SAMPLING_24; /*DSP Source 32 resolution.*/
                open_param.stream_in_param.playback.sampling_rate  = HAL_AUDIO_SAMPLING_RATE_48KHZ;
                open_param.stream_in_param.playback.channel_number = /*HAL_AUDIO_STEREO;//*/HAL_AUDIO_MONO;
                open_param.stream_in_param.playback.codec_type = 0;  //not use
                open_param.stream_in_param.playback.dsp_local_data_index  = 0;
                open_param.stream_in_param.playback.dsp_local_stream_mode = 1;

                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);

                open_param.stream_out_param.afe.memory    = HAL_AUDIO_MEM3;
                open_param.stream_out_param.afe.format    = /*AFE_PCM_FORMAT_S16_LE;//*/AFE_PCM_FORMAT_S32_LE;    /*DSP Sink 32bit resolution.*/
                if(open_param.stream_out_param.afe.format == AFE_PCM_FORMAT_S32_LE){
                    open_param.stream_out_param.afe.irq_period   = 5;
                    open_param.stream_out_param.afe.frame_size   = 256;
                    open_param.stream_out_param.afe.frame_number = 4;
                }else{
                    open_param.stream_out_param.afe.irq_period   = 10;
                    open_param.stream_out_param.afe.frame_size   = 512;
                    open_param.stream_out_param.afe.frame_number = 4;
                    //512(frame size) / 48k(Sampling rate) = 10(irq_period)
                }

                open_param.stream_out_param.afe.stream_out_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;  //asrc in
                open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;             //asrc out
                open_param.stream_out_param.afe.hw_gain       = true;
                hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT, open_param.stream_out_param.afe.audio_device, hal_audio_sampling_rate_enum_to_value(open_param.stream_in_param.playback.sampling_rate), true);

                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);
                // Notify to do dynamic download. Use async wait.
                hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);

                // Register callback
                //hal_audio_service_hook_callback(msg_type, mp3_codec_pcm_out_isr_callback, handle);
                // Start playback
                mcu2dsp_start_param_t start_param;
                // Collect parameters
                start_param.param.stream_in  = STREAM_IN_VP_DUMMY_SOURCE;
                start_param.param.stream_out = STREAM_OUT_AFE;
                start_param.stream_out_param.afe.aws_flag         = false;
                start_param.stream_out_param.afe.aws_sync_request = false;
                start_param.stream_out_param.afe.aws_sync_time    = 0;
                p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);
                hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);
              #endif

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DUMMY_SOURCE_CLOSE") != NULL) {
                LOGMSGIDI("VP/DmySrc CLOSE.", 0);
              #if 1
                prompt_control_dummy_source_stop();
              #else
                dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_STOP;
                dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CLOSE;
                // Notify to stop
                hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);

                // Unregister callback
                //hal_audio_service_unhook_callback(msg_type);

                // Notify to release dynamic download
                hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);

                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);
              #endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DUMMY_SOURCE_TEST") != NULL) {
                LOGMSGIDI("VP/DmySrc test.", 0);
                #if 1
                prompt_control_dummy_source_param_t DmySrc_param;
                DmySrc_param.index = 127;
                DmySrc_param.mode  = DUMMY_SOURCE_MODE_ONE_SHOT;
                prompt_control_dummy_source_change_feature(DmySrc_param);
                #endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DUMMY_SOURCE_VOL_UP") != NULL) {
                LOGMSGIDI("VP/DmySrc vol up.", 0);
                uint8_t dummp_src_level = prompt_control_dummy_source_get_volume_level();
                if (dummp_src_level < 0xFF) {
                    dummp_src_level += 8;
                }
                prompt_control_dummy_source_set_volume(dummp_src_level);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_DUMMY_SOURCE_VOL_DOWN") != NULL) {
                LOGMSGIDI("VP/DmySrc vol down.", 0);
                uint8_t dummp_src_level = prompt_control_dummy_source_get_volume_level();
                if (dummp_src_level > 135) {
                    dummp_src_level -= 8;
                }
                prompt_control_dummy_source_set_volume(dummp_src_level);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
            #if defined(MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_VP_MP3") != NULL) {
                #if 0
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                #endif
                uint8_t *tone_buf = NULL;
                uint32_t tone_size = 0;
                static const uint8_t voice_prompt_mix_mp3_tone_48k[] = {
                    #include "48k.mp3.hex"
                };
                tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_48k;
                tone_size = sizeof(voice_prompt_mix_mp3_tone_48k);
                #ifndef MTK_MP3_TASK_DEDICATE
                prompt_control_play_tone(VPC_MP3, tone_buf, tone_size, at_voice_prompt_callback);
                #else
                prompt_control_play_sync_tone(VPC_MP3, tone_buf, tone_size, 0, at_voice_prompt_callback);
                #endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
                }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_VP_TRIGGER") != NULL) {
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PROMPT_AWS_SYNC_TRIGGER, 0, 0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_VP_CONNECTED_MP3") != NULL) {
                uint8_t *tone_buf = NULL;
                uint32_t tone_size = 0;
                static const uint8_t voice_prompt_mix_mp3_tone_connected[] = {
                    #include "connected_MADPCM.mp3.hex"
                };
                tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_connected;
                tone_size = sizeof(voice_prompt_mix_mp3_tone_connected);
                #ifndef MTK_MP3_TASK_DEDICATE
                prompt_control_play_tone(VPC_MP3, tone_buf, tone_size, at_voice_prompt_callback);
                #else
                prompt_control_play_sync_tone(VPC_MP3, tone_buf, tone_size, 0, at_voice_prompt_callback);
                #endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_VP_LONG_MP3") != NULL) {
                uint8_t *tone_buf = NULL;
                uint32_t tone_size = 0;
                static const uint8_t voice_prompt_mix_mp3_tone_long[] = {
                    #include "48k.mp3.long.hex"
                };
                tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_long;
                tone_size = sizeof(voice_prompt_mix_mp3_tone_long);
                #ifndef MTK_MP3_TASK_DEDICATE
                prompt_control_play_tone(VPC_MP3, tone_buf, tone_size, at_voice_prompt_callback);
                #else
                prompt_control_play_sync_tone(VPC_MP3, tone_buf, tone_size, 0, at_voice_prompt_callback);
                #endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUD_CHECK_GAIN") != NULL) {
                LOGMSGIDI("Gain Check: A(0x%x), G1(0x%x), G2(0x%x)", 3, *(volatile uint32_t *)0x70000f58, *(volatile uint32_t *)0x70000424, *(volatile uint32_t *)0x7000043C);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif /* MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE */
            #endif /* MTK_PROMPT_SOUND_ENABLE */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VOL_STREAM_OUT") != NULL) {
                char* config_s = NULL;
                unsigned int data32;
                unsigned int digital_volume_index, analog_volume_index;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &digital_volume_index);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &analog_volume_index);
                data32 = (analog_volume_index<<16) | (digital_volume_index & 0xFFFF);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, 0, data32, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VOL_STREAM_IN") != NULL) {
                char* config_s = NULL;
                unsigned int data32;
                unsigned int digital_volume_index, analog_volume_index;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &digital_volume_index);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &analog_volume_index);
                data32 = (analog_volume_index<<16) | (digital_volume_index & 0xFFFF);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, data32, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VOL_STREAM_2A2D") != NULL) {
                char* config_s = NULL;
                unsigned int a_gain_in_hex[INPUT_ANALOG_GAIN_NUM] = {0};
                unsigned int d_gain_in_hex[INPUT_DIGITAL_GAIN_NUM] = {0};

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_0]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_1]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_2]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_3]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &a_gain_in_hex[INPUT_ANALOG_GAIN_FOR_MIC_L]);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &a_gain_in_hex[INPUT_ANALOG_GAIN_FOR_MIC_R]);

                hal_audio_set_stream_in_volume_for_multiple_microphone(d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_0], d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_1], HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1);
                hal_audio_set_stream_in_volume_for_multiple_microphone(d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_2], d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_DEVICE_3], HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3);
                #ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                    hal_audio_set_stream_in_volume_for_multiple_microphone(d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH], d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH], HAL_AUDIO_INPUT_GAIN_SELECTION_D14);
                #else
                    hal_audio_set_stream_in_volume_for_multiple_microphone(d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH], d_gain_in_hex[INPUT_DIGITAL_GAIN_FOR_ECHO_PATH], HAL_AUDIO_INPUT_GAIN_SELECTION_D4);
                #endif
                hal_audio_set_stream_in_volume_for_multiple_microphone(a_gain_in_hex[INPUT_ANALOG_GAIN_FOR_MIC_L], a_gain_in_hex[INPUT_ANALOG_GAIN_FOR_MIC_R], HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VOL_STREAM_6A6D") != NULL) {
                char* config_s = NULL;
                unsigned int gain_select;
                unsigned int gain_index0;
                unsigned int gain_index1;
                uint32_t data32;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &gain_select);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &gain_index0);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", &gain_index1);

                data32 = (gain_index1 <<16) | (gain_index0 & 0xFFFF);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, gain_select, data32, false);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }

#endif
            #ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VENDOR_VOL") != NULL) {
                void vendor_volume_ut_test(void)
                {
                    const uint32_t a2dp_mode_1[4][2] =
                    {
                        {0x1, 0x11},
                        {0x2, 0x22},
                        {0x3, 0x33},
                        {0x4, 0x44}
                    };

                    const uint32_t a2dp_mode_2[4][2] =
                    {
                        {0x111, 0x1111},
                        {0x222, 0x2222},
                        {0x333, 0x3333},
                        {0x444, 0x4444}
                    };

                    const uint32_t vp_mode_1[4][2] =
                    {
                        {0x5, 0x55},
                        {0x6, 0x66},
                        {0x7, 0x77},
                        {0x8, 0x88}
                    };

                    const uint32_t vp_mode_2[4][2] =
                    {
                        {0x555, 0x5555},
                        {0x666, 0x6666},
                        {0x777, 0x7777},
                        {0x888, 0x8888}
                    };

                    uint32_t digital_gain, analog_gain;

                    ami_register_stream_out_volume_table(VOL_A2DP, 1, a2dp_mode_1, 4);
                    ami_register_stream_out_volume_table(VOL_A2DP, 2, a2dp_mode_2, 4);

                    ami_register_stream_out_volume_table(VOL_VP, 1, vp_mode_1, 4);
                    ami_register_stream_out_volume_table(VOL_VP, 2, vp_mode_2, 4);

                    LOGMSGIDI("[VENDOR_VOLUME]Select Mode 1  \n", 0);
                    ami_switch_stream_out_volume_table(VOL_A2DP, 1);
                    ami_switch_stream_out_volume_table(VOL_VP, 1);

                    for(uint32_t level = 0; level < 4; level ++)
                    {
                        ami_get_stream_out_volume(VOL_A2DP, level, &digital_gain, &analog_gain);

                        //audio_src_srv_report("[VENDOR_VOLUME][A2DP]digital_gain = 0x%08x, analog_gain = 0x%08x  \n", 2, digital_gain, analog_gain);

                        ami_get_stream_out_volume(VOL_VP, level, &digital_gain, &analog_gain);

                        //audio_src_srv_report("[VENDOR_VOLUME][VP]digital_gain = 0x%08x, analog_gain = 0x%08x  \n", 2, digital_gain, analog_gain);
                    }

                    LOGMSGIDI("[VENDOR_VOLUME]Select Mode 2  \n", 0);
                    ami_switch_stream_out_volume_table(VOL_A2DP, 2);
                    ami_switch_stream_out_volume_table(VOL_VP, 2);

                    for(uint32_t level = 0; level < 4; level ++)
                    {
                        ami_get_stream_out_volume(VOL_A2DP, level, &digital_gain, &analog_gain);

                        //audio_src_srv_report("[VENDOR_VOLUME][A2DP]digital_gain = 0x%08x, analog_gain = 0x%08x  \n", 2, digital_gain, analog_gain);

                        ami_get_stream_out_volume(VOL_VP, level, &digital_gain, &analog_gain);

                        //audio_src_srv_report("[VENDOR_VOLUME][VP]digital_gain = 0x%08x, analog_gain = 0x%08x  \n", 2, digital_gain, analog_gain);
                    }

                }
                vendor_volume_ut_test();

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VENDOR_SE") != NULL) {
                vendor_se_ut_test();

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SET_DETACHABLE_MIC") != NULL) {
                extern voice_mic_type_t current_voice_mic_type;
                char* config_s = NULL;
                int32_t target_mic;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&target_mic);
                LOGMSGIDI("[DETACHABLE_MIC]target_mic = %d  \n", 1, target_mic);

                if((target_mic != VOICE_MIC_TYPE_FIXED) && (target_mic != VOICE_MIC_TYPE_DETACHABLE)) {
                    LOGMSGIDI("[DETACHABLE_MIC]MIC setting error! = %d  \n", 0);
                }

                LOGMSGIDI("[DETACHABLE_MIC]Before setting: Current Mic = %d  \n", 1, current_voice_mic_type);

                ami_set_voice_mic_type(target_mic);

                LOGMSGIDI("[DETACHABLE_MIC]After setting: Current Mic = %d  \n", 1, current_voice_mic_type);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #if defined(MTK_LINEIN_PLAYBACK_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_OPEN") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_OPEN", 0);
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
                mcu2dsp_open_param_t open_param = {{0}};
                void *p_param_share;

                open_param.param.stream_in = STREAM_IN_AFE;
                open_param.param.stream_out = STREAM_OUT_AFE;

                open_param.stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
                open_param.stream_in_param.afe.stream_channel = HAL_AUDIO_DIRECT;
                open_param.stream_in_param.afe.memory = HAL_AUDIO_MEM1 ;
                open_param.stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_in_param.afe.sampling_rate = 48000;
                open_param.stream_in_param.afe.irq_period = 8;
                open_param.stream_in_param.afe.frame_size = 384;
                open_param.stream_in_param.afe.frame_number = 4;
                open_param.stream_in_param.afe.hw_gain = true;
                open_param.stream_in_param.afe.misc_parms = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;

                open_param.stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
                open_param.stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
                open_param.stream_out_param.afe.memory = HAL_AUDIO_MEM1;
                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_out_param.afe.stream_out_sampling_rate = 48000;
                open_param.stream_out_param.afe.sampling_rate = 48000;
                open_param.stream_out_param.afe.irq_period = 8;
                open_param.stream_out_param.afe.frame_size = 384;
                open_param.stream_out_param.afe.frame_number = 4;
                open_param.stream_out_param.afe.hw_gain = true;
                open_param.stream_out_param.afe.misc_parms = I2S_CLK_SOURCE_DCXO;

                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_LINEIN);

                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_OPEN, 0, (uint32_t)p_param_share, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_START") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_START", 0);
                mcu2dsp_start_param_t start_param = {{0}};
                void *p_param_share;

                // Collect parameters
                start_param.param.stream_in     = STREAM_IN_AFE;
                start_param.param.stream_out    = STREAM_OUT_AFE;
                start_param.stream_in_param.afe.aws_flag   =  false;
                start_param.stream_out_param.afe.aws_flag   =  false;
                p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_LINEIN);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_START, 0, (uint32_t)p_param_share, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_STOP") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_STOP", 0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_STOP, 0, 0, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_CLOSE") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_CLOSE", 0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_CLOSE, 0, 0, true);
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_SUSPEND") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_SUSPEND", 0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_SUSPEND, 0, 0, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEINPLAYBACK_RESUME") != NULL) {
                LOGMSGIDI("AT+EAUDIO=LINEINPLAYBACK_RESUME", 0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, 0, 0, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_OPEN") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_OPEN", 0);
                hal_audio_sampling_rate_t fs;
                hal_audio_device_t in_device;
                hal_audio_device_t out_device;

                char* config_s = NULL;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&fs);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&in_device);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&out_device);
#if 0
                audio_linein_playback_open(fs, in_device, out_device);
#else
                audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_IN, NULL);
                audio_sink_srv_am_line_in_codec_t Line_in_codec;
                Line_in_codec.codec_cap.in_audio_device    = in_device;
                Line_in_codec.codec_cap.out_audio_device   = out_device;
                Line_in_codec.codec_cap.linein_sample_rate = fs;
                audio_sink_srv_line_in_set_param(&Line_in_codec);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_START") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_START", 0);
#if 0
                audio_linein_playback_start();
#else
                audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_START, NULL);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_STOP") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_STOP", 0);
#if 0
                audio_linein_playback_stop();
#else
                audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_TRIGGER_STOP, NULL);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_CLOSE") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_CLOSE", 0);
#if 0
                audio_linein_playback_close();
#else
                audio_sink_srv_line_in_control_action_handler(AUDIO_SINK_SRV_LINE_IN_ACT_DEVICE_PLUG_OUT, NULL);
#endif
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_SET_VOLUME") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_LINEIN_PLAYBACK_SET_VOLUME", 0);
                linein_playback_gain_t gain;

                char* config_s = NULL;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%lu", &gain.in_digital_gain);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%lu", &gain.in_analog_gain);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%lu", &gain.out_digital_gain);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%lu", &gain.out_analog_gain);

                audio_linein_playback_set_volume(gain);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_PURE_LINEIN_PLAYBACK_OPEN") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_PURE_LINEIN_PLAYBACK_OPEN", 0);

                hal_audio_sampling_rate_t fs;
                hal_audio_device_t in_device;
                hal_audio_device_t out_device;
                char* config_s = NULL;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&fs);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&in_device);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&out_device);
                audio_pure_linein_playback_open(fs, in_device, out_device);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#ifdef LINE_IN_PURE_FOR_AMIC_CLASS_G_HQA
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_HQA_PURE_LINEIN_PLAYBACK_OPEN") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_HQA_PURE_LINEIN_PLAYBACK_OPEN", 0);
                hal_audio_sampling_rate_t fs;
                hal_audio_device_t in_device;
                hal_audio_device_t out_device;
                hal_audio_interface_t device_in_interface_HQA = HAL_AUDIO_INTERFACE_1;
                hal_audio_analog_mdoe_t adc_mode_HQA = HAL_AUDIO_ANALOG_INPUT_ACC10K;
                hal_audio_performance_mode_t mic_performance_HQA = AFE_PEROFRMANCE_NORMAL_MODE;
                hal_audio_performance_mode_t dac_performance_HQA = AFE_PEROFRMANCE_NORMAL_MODE;

                char* config_s = NULL;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&fs);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&in_device);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&device_in_interface_HQA);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&adc_mode_HQA);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&mic_performance_HQA);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&out_device);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&dac_performance_HQA);
                LOGMSGIDI("HQA fs %d in_device %d device_in_interface_HQA %d adc_mode_HQA %d mic_performance_HQA %d out_device %d dac_performance_HQA %d",7,fs,in_device,device_in_interface_HQA ,adc_mode_HQA, mic_performance_HQA ,out_device, dac_performance_HQA);
                audio_pure_linein_playback_open_HQA(fs,in_device,device_in_interface_HQA,adc_mode_HQA,mic_performance_HQA,out_device,dac_performance_HQA);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SET_GPIO") != NULL) {
                LOGMSGIDI("AT+EAUDIO=SET_GPIO", 0);
                hal_gpio_pin_t pin;
                uint8_t function_index;
                char* config_s = NULL;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&pin);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&function_index);
                hal_gpio_init(pin);
                hal_pinmux_set_function(pin,function_index);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_PURE_LINEIN_PLAYBACK_CLOSE") != NULL) {
                LOGMSGIDI("AT+EAUDIO=AUDIO_PURE_LINEIN_PLAYBACK_CLOSE", 0);
                audio_pure_linein_playback_close();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif /* MTK_LINEIN_PLAYBACK_ENABLE */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DL_NM") != NULL) {
                LOGMSGIDI("AT+EAUDIO=DL_NM", 0);
                *((volatile uint32_t*)(0xA2120B04)) &= 0xFFFFFEFF;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DL_HP") != NULL) {
                LOGMSGIDI("AT+EAUDIO=DL_HP", 0);
                *((volatile uint32_t*)(0xA2120B04)) |= 0x100;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=UL_NM") != NULL) {
                LOGMSGIDI("AT+EAUDIO=UL_NM", 0);
                *((volatile uint32_t*)(0xA2120B04)) &= 0xFFFFFDFF;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=UL_HP") != NULL) {
                LOGMSGIDI("AT+EAUDIO=UL_HP", 0);
                *((volatile uint32_t*)(0xA2120B04)) |= 0x200;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=REG_SET,") != NULL) {
                char *pch = strstr((char *)parse_cmd->string_ptr, "0x");
                if(pch == NULL){
                    LOGMSGIDE("[REG_SET]Input first parameter error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                    break;
                }

                unsigned int number = (unsigned int)strtoul(pch, NULL, 0);
                LOGMSGIDI("RG result is: %x \r\n", 1, number);

                pch = pch + 3;  // to skip the first one
                char *pch2 = strstr(pch, "0x");
                if(pch2 == NULL){
                    LOGMSGIDE("[REG_SET]Input second parameter error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                    break;
                }

                unsigned int number2 = (unsigned int)strtoul(pch2, NULL, 0);
                LOGMSGIDI("Value result is: %x \r\n", 1, number2);

                 *((volatile uint32_t*)(number)) = number2;

                LOGMSGIDI("Reg 0x%x is 0x%x", 2, number, *((volatile unsigned int*)(number)));
                LOGMSGIDI("Kevin here \r\n", 0);
                _UNUSED(number);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
           // "AT+EAUDIO=REG_GET,0x70000749"
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=REG_GET,") != NULL) {
                char *pch = strstr((char *)parse_cmd->string_ptr, "0x");
                if(pch == NULL){
                    LOGMSGIDE("[REG_GET]Input parameter error.", 0);
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    presponse->response_len = strlen((const char *)presponse->response_buf);
                    atci_send_response(presponse);
                    break;
                }
                unsigned int number = (unsigned int)strtoul(pch, NULL, 0);
                LOGMSGIDI("Reg 0x%x is 0x%x", 2, number, *((volatile unsigned int*)(number)));
                _UNUSED(number);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#if defined(HAL_AUDIO_SUPPORT_DEBUG_DUMP)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=REG_DEBUG_DUMP") != NULL) {
                LOGMSGIDI("AT+EAUDIO=REG_DEBUG_DUMP", 0);
                hal_audio_debug_dump();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
            #if defined(HAL_AUDIO_SUPPORT_APLL)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TurnOnAPLL") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                char* config_s = NULL;
                unsigned int samplerate;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &samplerate);

                LOGMSGIDI("AT+EAUDIO=TurnOnAPLL, FS:%d", 1, samplerate);
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AFE, true);
                if (HAL_AUDIO_STATUS_OK == hal_audio_apll_enable(true, samplerate)){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TurnOffAPLL") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                char* config_s = NULL;
                unsigned int samplerate;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &samplerate);

                LOGMSGIDI("AT+EAUDIO=TurnOffAPLL, FS:%d", 1, samplerate);
                if (HAL_AUDIO_STATUS_OK == hal_audio_apll_enable(false, samplerate)){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AFE, false);
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=QUERY_APLL") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                LOGMSGIDI("AT+EAUDIO=QUERY_APLL", 0);
                if (HAL_AUDIO_STATUS_OK == hal_audio_query_apll_status()){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TurnOnMCLK") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                char* config_s = NULL;
                uint8_t mclkoutpin, apll, divider;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&mclkoutpin);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&apll);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&divider);

                LOGMSGIDI("AT+EAUDIO=TurnOnMCLK, I2S:%d, APLL:%d, DIVIDER:%d", 3, mclkoutpin, apll, divider);
                if (HAL_AUDIO_STATUS_OK == hal_audio_mclk_enable(true, mclkoutpin, apll, divider)){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TurnOffMCLK") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                char* config_s = NULL;
                uint8_t mclkoutpin, apll, divider;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&mclkoutpin);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&apll);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%d", (int*)&divider);

                LOGMSGIDI("AT+EAUDIO=TurnOffMCLK, I2S:%d, APLL:%d, DIVIDER:%d", 3, mclkoutpin, apll, divider);
                if (HAL_AUDIO_STATUS_OK == hal_audio_mclk_enable(false, mclkoutpin, apll, divider)){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=QUERY_MCLK") != NULL) {
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                LOGMSGIDI("AT+EAUDIO=QUERY_MCLK", 0);
                if (HAL_AUDIO_STATUS_OK == hal_audio_query_mclk_status()){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DL_ANA_VOL,") != NULL) {
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                hal_audio_set_stream_out_volume(0x258,config);  // 600, config
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ALC_OFF") != NULL) {
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ALC_SWITCH, 0, 0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ALC_ON") != NULL) {
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ALC_SWITCH, 0, 1, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SW_SWAP_R_MIC") != NULL) {
                LOGMSGIDI("SW_SWAP_R_MIC",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, 3, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SW_SWAP_L_MIC") != NULL) {
                LOGMSGIDI("SW_SWAP_L_MIC",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, 2, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SW_SWAP_3rd_MIC") != NULL) {
                LOGMSGIDI("SW_SWAP_3rd_MIC",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 1, 4, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AEC_NR_EN") != NULL) {
                LOGMSGIDI("AEC_NR_EN",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 1, 1, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AEC_NR_DIS") != NULL) {
                LOGMSGIDI("AEC_NR_DIS",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 1, 0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SW_GAIN_Enable") != NULL) {
                LOGMSGIDI("SW_GAIN_Enable",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SW_GAIN_EN, 0, 1, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SW_GAIN_Disable") != NULL) {
                LOGMSGIDI("SW_GAIN_Disable",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SW_GAIN_EN, 0, 0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #ifdef ENABLE_HWSRC_CLKSKEW
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=CLKSKEW_MODE_V1") != NULL) {
                LOGMSGIDI("CLKSKEW_MODE_V1_Enable",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CLKSKEW_MODE_SEL, 0, 0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=CLKSKEW_MODE_V2") != NULL) {
                LOGMSGIDI("CLKSKEW_MODE_V2_Enable",0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CLKSKEW_MODE_SEL, 0, 1, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #ifdef MTK_ANC_ENABLE
            #ifdef MTK_ANC_V2
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_") != NULL) {
                audio_anc_control_result_t anc_ret;
                anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_ATCI, 0, (void *)parse_cmd->string_ptr);
                if(anc_ret != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS){
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }else{
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #else
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_ON,") != NULL) {
                char* config_s = NULL;
                unsigned int config = 0;
                unsigned int anc_mode;
                short runtime_gain = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                LOGMSGIDI("ANC on: %x",1,config);
                config_s = strchr(config_s, ',');
                if(config_s != NULL) {
                    config_s++;
                    sscanf(config_s, "%hd", &runtime_gain);
                    LOGMSGIDI("ANC on: %x %d",2,config, runtime_gain);
                    config_s = strchr(config_s, ',');
                    if(config_s != NULL) {
                        config_s++;
                        sscanf(config_s, "%d", &anc_mode);
                        if(anc_mode == 1) {
                            config |= ANC_FF_ONLY_BIT_MASK;
                        } else if(anc_mode == 2) {
                            config |= ANC_FB_ONLY_BIT_MASK;
                        }
                        LOGMSGIDI("ANC on: %x %d %d",3,config, runtime_gain, anc_mode);
                    }
                }
                audio_anc_enable(config, runtime_gain, at_anc_callback);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_OFF") != NULL) {
                audio_anc_disable(at_anc_callback);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_SET_VOL") != NULL) {
                char* config_s = NULL;
                anc_sw_gain_t anc_sw_gain;
                unsigned int to_role = 0;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &anc_sw_gain.gain_index_l);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &anc_sw_gain.gain_index_r);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%u", &to_role);

                audio_anc_set_volume(anc_sw_gain, (anc_to_role_t)to_role);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_SET_RUNTIME_VOL") != NULL) {
                char* config_s = NULL;
                int16_t anc_runtime_gain;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &anc_runtime_gain);

                audio_anc_set_runtime_volume(anc_runtime_gain);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_READ_VOL_NVDM") != NULL) {
                char* config_s = NULL;
                anc_sw_gain_t anc_sw_gain;
                uint16_t role = 0;
                uint32_t ret;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &role);

                ret = audio_anc_read_volume_nvdm(&anc_sw_gain, (uint8_t)role);
                if(role == 0) {
                    LOGMSGIDI("ANC read vol nvdm: %d %d",2,anc_sw_gain.gain_index_l,anc_sw_gain.gain_index_r);
                }

                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_WRITE_VOL_NVDM") != NULL) {
                char* config_s = NULL;
                anc_sw_gain_t anc_sw_gain;
                uint16_t role = 0;
                uint32_t ret;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &anc_sw_gain.gain_index_l);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &anc_sw_gain.gain_index_r);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &role);

                ret = audio_anc_write_volume_nvdm(&anc_sw_gain, (uint8_t)role);

                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_WRITE_F_VOL_NVDM") != NULL) {
                char* config_s = NULL;
                unsigned int filter_type;
                short filter_unique_gain;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%u", &filter_type);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &filter_unique_gain);
                anc_write_filter_volume_nvdm(filter_type, filter_unique_gain);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if ((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_CONFIG,") != NULL)
                   || (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_CONFIG_THRU,") != NULL)) {
                char* config_s = NULL;
                uint32_t arg[4];
                uint32_t index = 0;
                uint32_t nvkeyid = NVKEY_DSP_PARA_ANC;
                memset(arg, 0, sizeof(arg));
                config_s = (char *)parse_cmd->string_ptr;
                if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_CONFIG_THRU,") != NULL) {
                    nvkeyid = NVKEY_DSP_PARA_PASS_THRU;
                }
                while(((config_s = strchr(config_s, ',')) != NULL) && (index < 4))
                {
                    config_s++;
                    sscanf(config_s, "%d", (int *)&arg[index]);
                    index++;
                }
                anc_config(ANC_CONFIG_RAMP_DLY, &arg[0]);
                anc_config(ANC_CONFIG_ADC_SWAP, &arg[1]);
                anc_config(ANC_CONFIG_RAMP_STEP, &arg[2]);
                anc_config(ANC_CONFIG_RAMP_CONFIG_GAIN, &arg[3]);
                anc_config(ANC_CONFIG_RESTORE_NVDM_PARAM, &nvkeyid);
                LOGMSGIDI("nvkey 0x%x config: %d %d %d %d ",5,nvkeyid,arg[0],arg[1],arg[2],arg[3]);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if ((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_TUNE,") != NULL)
                   || (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_TUNE_THRU,") != NULL)) {
                char* config_s = NULL;
                void *p_param_share;
                uint32_t arg[11];
                uint32_t index = 0;
                uint32_t nvkeyid = NVKEY_DSP_PARA_ANC;
                memset(arg, 0, sizeof(arg));
                config_s = (char *)parse_cmd->string_ptr;
                if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_TUNE_THRU,") != NULL) {
                    nvkeyid = NVKEY_DSP_PARA_PASS_THRU;
                }
                while(((config_s = strchr(config_s, ',')) != NULL) && (index < 11))
                {
                    config_s++;
                    sscanf(config_s, "%d", (int *)&arg[index]);
                    index++;
                }
                anc_config(ANC_CONFIG_PWR_DET_ENABLE, &arg[0]);
                anc_config(ANC_CONFIG_SMOOTH, &arg[1]);
                anc_config(ANC_CONFIG_HIGH_THRESHOLD_IN_DBFS, &arg[2]);
                anc_config(ANC_CONFIG_LOWER_BOUND, &arg[3]);
                anc_config(ANC_CONFIG_RAMP_UP_DLY, &arg[4]);
                anc_config(ANC_CONFIG_RAMP_UP_STEP, &arg[5]);
                anc_config(ANC_CONFIG_RAMP_DOWN_DLY, &arg[6]);
                anc_config(ANC_CONFIG_RAMP_DOWN_STEP, &arg[7]);
                anc_config(ANC_CONFIG_OVF_CHECK_ENABLE, &arg[8]);
                anc_config(ANC_CONFIG_OVF_THRESHOLD, &arg[9]);
                anc_config(ANC_CONFIG_REDUCE_GAIN_STEP, &arg[10]);
                anc_config(ANC_CONFIG_RESTORE_NVDM_PARAM, &nvkeyid);

                LOGMSGIDI("nvkey 0x%x tune: %d %d %d %d %d %d %d %d %d %d %d",12,nvkeyid,arg[0],arg[1],arg[2],arg[3],arg[4],arg[5],arg[6],arg[7],arg[8],arg[9],arg[10]);
                p_param_share = hal_audio_dsp_controller_put_paramter( &arg, sizeof(arg), AUDIO_MESSAGE_TYPE_ANC);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_SET_PARAM, 0, (uint32_t)p_param_share, false);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_PT") != NULL) {
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_SET_PARAM, 0xFFFF, (uint32_t)config, false);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_READ_COEF") != NULL) {
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_SET_PARAM, 2, (uint32_t)0, false);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #ifdef MTK_DEQ_ENABLE
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_MUTE_STRM,") != NULL) {
                char* config_s = NULL;
                int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, 1, (uint32_t)config, true);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_DEQ,") != NULL) {
                char* config_s = NULL;
                unsigned int delay_samples, phase_inverse;
                short gain;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%u", &delay_samples);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &gain);
                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%u", &phase_inverse);
                anc_set_deq_param((unsigned char)delay_samples, gain, (unsigned char)phase_inverse);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ANC_DEQ_ENA,") != NULL) {
                char* config_s = NULL;
                unsigned int deq_enable;
                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%u", &deq_enable);
                anc_set_deq_enable((unsigned char)deq_enable);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DEQ_STEP1") != NULL) {
                uint32_t value = 0;
                uint32_t nvkeyid = NVKEY_DSP_PARA_ANC;
                anc_sw_gain_t anc_sw_gain;
                uint8_t anc_enable;

                //AT+EAUDIO=ANC_CONFIG,2,0,6,0\0d\0a
                anc_config(ANC_CONFIG_RAMP_CONFIG_GAIN, &value);
                anc_config(ANC_CONFIG_RESTORE_NVDM_PARAM, &nvkeyid);
                //AT+EAUDIO=ANC_ON,1,0,0\0d\0a
                audio_anc_enable(1, 0, at_anc_callback);
                //AT+EAUDIO=ANC_MUTE_STRM,2\0d\0a
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, 1, (uint32_t)2, true);
                //AT+EAUDIO=ANC_SET_VOL,-30000,0,0\0d\0a
                do {
                    anc_get_status(&anc_enable, NULL, NULL);
                } while (anc_enable == 0);
                anc_sw_gain.gain_index_l = -30000;
                anc_sw_gain.gain_index_r = 0;
                audio_anc_set_volume(anc_sw_gain, (anc_to_role_t)TO_BOTH);
                //AT+EAUDIO=REG_SET,0xA3030080,0x8060000\0d\0a
                nvkeyid = *((volatile unsigned int*)(0xA3030080));
                *((volatile unsigned int*)(0xA3030080)) = (nvkeyid & (~(1<<16)));
                LOGMSGIDI("0xA3030080 = 0x%x ", 1, *((volatile unsigned int*)(0xA3030080)));
                LOGMSGIDI("DEQ calibrate step 111 finish !", 0);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DEQ_STEP2") != NULL) {
                uint32_t value = 600;
                uint32_t nvkeyid = NVKEY_DSP_PARA_ANC;
                anc_sw_gain_t anc_sw_gain;
                uint8_t anc_enable;

                //AT+EAUDIO=ANC_MUTE_STRM,0\0d\0a
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, 1, (uint32_t)0, true);
                //AT+EAUDIO=ANC_CONFIG,2,0,6,600\0d\0a
                anc_config(ANC_CONFIG_RAMP_CONFIG_GAIN, &value);
                anc_config(ANC_CONFIG_RESTORE_NVDM_PARAM, &nvkeyid);
                //AT+EAUDIO=ANC_OFF\0d\0a
                audio_anc_disable(at_anc_callback);
                //AT+EAUDIO=ANC_ON,1,0,0\0d\0a
                audio_anc_enable(1, 0, at_anc_callback);
                //AT+EAUDIO=ANC_SET_VOL,-30000,0,0\0d\0a
                do {
                    anc_get_status(&anc_enable, NULL, NULL);
                } while (anc_enable == 0);
                anc_sw_gain.gain_index_l = -30000;
                anc_sw_gain.gain_index_r = 0;
                audio_anc_set_volume(anc_sw_gain, (anc_to_role_t)TO_BOTH);
                LOGMSGIDI("DEQ calibrate step 222 finish !", 0);

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #endif
            #endif
            #if defined(MTK_PEQ_ENABLE) && defined(MTK_RACE_CMD_ENABLE)
            else if ((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PEQ_MODE,") != NULL) ||
                     (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PEQ_SYNC,") != NULL))
            {
                char* config_s = NULL;
                unsigned int config = 0;
                unsigned int phase_id = 0;
                unsigned int  ret;
                uint8_t setting_mode = PEQ_DIRECT;
                bt_clock_t target_bt_clk = {0};
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if(role == BT_AWS_MCE_ROLE_AGENT) {
                    #ifdef MTK_AWS_MCE_ENABLE
                    bt_clock_t current_bt_clk = {0};
                    int diff = 0;
                    if(strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PEQ_SYNC,") != NULL)
                    {
                        setting_mode = PEQ_SYNC;
                        ret = bt_sink_srv_bt_clock_addition(&current_bt_clk, NULL, 0);
                        if(ret == BT_STATUS_FAIL) LOGMSGIDW("get current bt clock FAIL\n", 0);
                        ret = bt_sink_srv_bt_clock_addition(&target_bt_clk, NULL, PEQ_FW_LATENCY*1000);
                        if(ret == BT_STATUS_FAIL) LOGMSGIDW("get target bt clock FAIL with duration %d us\n", 1, PEQ_FW_LATENCY*1000);
                        diff = (((int32_t)target_bt_clk.nclk - (int32_t)current_bt_clk.nclk)*625/2 + ((int32_t)target_bt_clk.nclk_intra - (int32_t)current_bt_clk.nclk_intra));
                        if ((diff <= PEQ_FW_LATENCY*1000+10000) && (diff >= PEQ_FW_LATENCY*1000-10000)) {
                            LOGMSGIDI("get cur: %x.%x tar: %x.%x \n", 4, (unsigned int)current_bt_clk.nclk,(unsigned int)current_bt_clk.nclk_intra,(unsigned int)target_bt_clk.nclk,(unsigned int)target_bt_clk.nclk_intra);
                        } else {
                            LOGMSGIDW("cur: %x.%x tar: %x.%x  diff: %d xxxxxxxxxxxx\n",5,(unsigned int)current_bt_clk.nclk,(unsigned int)current_bt_clk.nclk_intra,(unsigned int)target_bt_clk.nclk,(unsigned int)target_bt_clk.nclk_intra,diff);
                        }
                        if(ret == BT_STATUS_FAIL) { // for agent only case
                            setting_mode = PEQ_DIRECT;
                            LOGMSGIDI("PEQ use direct mode for agent only case\n", 0);
                        }
                    }
                    #endif
                }
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                config_s = strchr(config_s, ',');
                if (config_s != NULL) {
                    config_s++;
                    sscanf(config_s, "%u", &phase_id);
                }
                if(config < 9) {
                    ret = race_dsprt_peq_change_mode_data((uint8_t)phase_id, setting_mode, target_bt_clk.nclk, (config!=0)?1:0, (uint8_t)config, AM_A2DP_PEQ);
                } else {
                    ret = race_dsprt_peq_realtime_data((uint8_t)phase_id, setting_mode, target_bt_clk.nclk, g_peq_test_coef, sizeof(g_peq_test_coef), AM_A2DP_PEQ);
                }
                if(ret != 0) {
                    LOGMSGIDE("PEQ_XXX,%d FAIL, role:0x%x, phase_id:%d", 3, config, role, phase_id);
                }
                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=CONFIG_DIS_PEQ") != NULL) {
                unsigned int  ret;
                ret = race_dsprt_peq_change_mode_data(0, PEQ_DIRECT, 0, 0, (uint8_t)PEQ_SOUND_MODE_FORCE_DRC, AM_A2DP_PEQ);
                if(ret != 0) {
                    LOGMSGIDE("CONFIG_DIS_PEQ FAIL, phase 0", 0);
                }
                ret = race_dsprt_peq_change_mode_data(1, PEQ_DIRECT, 0, 0, (uint8_t)PEQ_SOUND_MODE_FORCE_DRC, AM_A2DP_PEQ);
                if(ret != 0) {
                    LOGMSGIDE("CONFIG_DIS_PEQ FAIL, phase 1", 0);
                }
                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=CONFIG_ENA_PEQ") != NULL) {
                unsigned int  ret;
                #ifdef MTK_ANC_ENABLE
                #ifndef MTK_ANC_V2
                uint8_t anc_enable;
                uint8_t hybrid_enable;
                #endif
                #endif
                ret = race_dsprt_peq_change_mode_data(0, PEQ_DIRECT, 0, 1, PEQ_SOUND_MODE_FORCE_DRC, AM_A2DP_PEQ);
                if(ret != 0) {
                    LOGMSGIDE("CONFIG_ENA_PEQ FAIL, phase 0", 0);
                }
                #ifdef MTK_ANC_ENABLE
                #ifndef MTK_ANC_V2
                anc_get_status(&anc_enable, NULL, &hybrid_enable);
                if ((anc_enable > 0) && (hybrid_enable > 0)) {
                    ret = race_dsprt_peq_change_mode_data(1, PEQ_DIRECT, 0, 1, PEQ_SOUND_MODE_FORCE_DRC, AM_A2DP_PEQ);
                } else {
                    ret = race_dsprt_peq_change_mode_data(1, PEQ_DIRECT, 0, 0, PEQ_SOUND_MODE_UNASSIGNED, AM_A2DP_PEQ);
                }
                if(ret != 0) {
                    LOGMSGIDE("CONFIG_ENA_PEQ FAIL, phase 1", 0);
                }
                #endif
                #endif
                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif
            #ifdef MTK_LINEIN_PEQ_ENABLE
            else if ((strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEIN_PEQ_MODE,") != NULL) ||
                     (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=LINEIN_PEQ_SYNC,") != NULL))
            {
                char* config_s = NULL;
                unsigned int config = 0;
                unsigned int phase_id = 0;
                unsigned int  ret;
                uint8_t setting_mode = PEQ_DIRECT;
                bt_clock_t target_bt_clk = {0};
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if(role == BT_AWS_MCE_ROLE_AGENT) {
                    #ifdef MTK_AWS_MCE_ENABLE
                    bt_clock_t current_bt_clk = {0};
                    int diff = 0;
                    if(strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=PEQ_SYNC,") != NULL)
                    {
                        setting_mode = PEQ_SYNC;
                        ret = bt_sink_srv_bt_clock_addition(&current_bt_clk, NULL, 0);
                        if(ret == BT_STATUS_FAIL) LOGMSGIDW("get current bt clock FAIL\n", 0);
                        ret = bt_sink_srv_bt_clock_addition(&target_bt_clk, NULL, PEQ_FW_LATENCY*1000);
                        if(ret == BT_STATUS_FAIL) LOGMSGIDW("get target bt clock FAIL with duration %d us\n", 1, PEQ_FW_LATENCY*1000);
                        diff = (((int32_t)target_bt_clk.nclk - (int32_t)current_bt_clk.nclk)*625/2 + ((int32_t)target_bt_clk.nclk_intra - (int32_t)current_bt_clk.nclk_intra));
                        if ((diff <= PEQ_FW_LATENCY*1000+10000) && (diff >= PEQ_FW_LATENCY*1000-10000)) {
                            LOGMSGIDI("get cur: %x.%x tar: %x.%x \n", 4, (unsigned int)current_bt_clk.nclk,(unsigned int)current_bt_clk.nclk_intra,(unsigned int)target_bt_clk.nclk,(unsigned int)target_bt_clk.nclk_intra);
                        } else {
                            LOGMSGIDW("cur: %x.%x tar: %x.%x  diff: %d xxxxxxxxxxxx\n",5,(unsigned int)current_bt_clk.nclk,(unsigned int)current_bt_clk.nclk_intra,(unsigned int)target_bt_clk.nclk,(unsigned int)target_bt_clk.nclk_intra,diff);
                        }
                        if(ret == BT_STATUS_FAIL) { // for agent only case
                            setting_mode = PEQ_DIRECT;
                            LOGMSGIDI("PEQ use direct mode for agent only case\n", 0);
                        }
                    }
                    #endif
                }
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%d", &config);
                config_s = strchr(config_s, ',');
                if (config_s != NULL) {
                    config_s++;
                    sscanf(config_s, "%u", &phase_id);
                }
                if(config < 9) {
                    ret = race_dsprt_peq_change_mode_data((uint8_t)phase_id, setting_mode, target_bt_clk.nclk, (config!=0)?1:0, (uint8_t)config, AM_LINEIN_PEQ);
                } else {
                    ret = race_dsprt_peq_realtime_data((uint8_t)phase_id, setting_mode, target_bt_clk.nclk, g_peq_test_coef, sizeof(g_peq_test_coef), AM_LINEIN_PEQ);
                }
                if(ret != 0) {
                    LOGMSGIDE("PEQ_XXX,%d FAIL, role:0x%x, phase_id:%d", 3, config, role, phase_id);
                }
                presponse->response_flag = (ret == 0) ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            #endif

            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=VOLUME_PARAM_NVDM") != NULL) {
                char* config_s = NULL;
                ami_audio_volume_parameters_nvdm_t volume_param;

                config_s = strchr((char *)parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%hd", &volume_param.gain1_compensation);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hd", &volume_param.gain2_compensation);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hhu", &volume_param.gain1_sample_per_step);

                config_s = strchr(config_s, ',');
                config_s++;
                sscanf(config_s, "%hhu", &volume_param.gain2_sample_per_step);

                am_set_audio_output_volume_parameters_to_nvdm(&volume_param);
                am_load_audio_output_volume_parameters_from_nvdm();

                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif
#if defined(HAL_AUDIO_TEST_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_HEADSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(true);
                uint8_t result = audio_test_play_audio_1k_tone();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_HANDSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(true);
                uint8_t result = audio_test_play_audio_1k_tone();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Voice_Play_HEADSET") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_play_voice_1k_tone();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Play_Stop") != NULL) {
                uint8_t result = audio_test_stop_1k_tone();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Dump_SPE_COM_Param") != NULL) {
                spe_dump_common_param();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Dump_SPE_MOD_Param") != NULL) {
                spe_dump_mode_param();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            }
#if defined(HAL_AUDIO_SDFATFS_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_MMIC_ACC,") != NULL) {
                uint8_t result = 0;
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(0); //acc
                if(config == 0) {
                    result = audio_test_pcm2way_record();
                } else {
                    result = audio_test_pcm2way_wb_record();
                }
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_MMIC_DCC,") != NULL) {
                uint8_t result = 0;
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_audio_tone(false);
                audio_test_switch_mic_type(1); //dcc
                if(config == 0) {
                    result = audio_test_pcm2way_record();
                } else {
                    result = audio_test_pcm2way_wb_record();
                }
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_DMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_pcm2way_record();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Record_Test_Off") != NULL) {
                uint8_t result = audio_test_pcm2way_stop_1k_tone();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Play_SD") != NULL) {
                uint8_t result = audio_test_play_audio_sd();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Audio_Stop_SD") != NULL) {
                uint8_t result = audio_test_stop_audio_sd();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
#endif /* HAL_AUDIO_SDFATFS_ENABLE */
#ifdef MTK_BUILD_SMT_LOAD
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_HEADSET_HMIC") != NULL) {
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HEADSET);
                audio_test_set_input_device(HAL_AUDIO_DEVICE_HEADSET_MIC);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_SPK_MMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_MAIN_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=Ext_LPK_SPK_DMIC") != NULL) {
                audio_test_set_input_device(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC);
                audio_test_set_output_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
                audio_test_set_audio_tone(false);
                uint8_t result = audio_test_external_loopback_test();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SMT_RESULT") != NULL) {
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#ifdef HAL_ACCDET_MODULE_ENABLED
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ACCDET_TEST") != NULL) {
                uint8_t result = audio_test_detect_earphone();
                snprintf((char * restrict)presponse->response_buf, sizeof(presponse->response_buf), "+EAUDIO:%d\r\n", result);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=ACCDET_REG") != NULL) {
                //callback
                register_accdet_callback();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif /* HAL_ACCDET_MODULE_ENABLED */
#endif /* MTK_BUILD_SMT_LOAD */
#if defined(HAL_AUDIO_SLT_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SLT_START") != NULL) {
                uint8_t result = audio_slt_test();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=SLT_RESULT") != NULL) {
                audio_test_stop_1k_tone();
                uint8_t result = audio_test_detect_1k_tone_result();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
#endif
#ifdef MTK_EXTERNAL_DSP_ENABLE
#if defined (MTK_NDVC_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=NDVC,") != NULL) {
                char* noise_ptr = NULL;
                uint16_t noise_level =0, noise_idx=0;
                noise_ptr = strchr(parse_cmd->string_ptr, ',');
                noise_ptr++;
                noise_level = atoi(noise_ptr);
                ndvc_at_test = true;
                noise_idx = spe_ndvc_uplink_noise_index_map(noise_level);
                *DSP_SPH_SWNDVC_POWER_INDEX = noise_idx;
                LOGMSGIDI("\r\n[AT]NDVC Test noise_dB=%d, index=%d \r\n",2, noise_level, noise_idx);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=TESTATNDVCCOFF") != NULL) {
                ndvc_at_test = false;
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            }
#endif /*MTK_NDVC_ENABLE*/
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_PWR_ON_DL") != NULL) {
                uint8_t result = external_dsp_activate(true);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_PWR_OFF") != NULL) {
                uint8_t result = external_dsp_activate(false);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SET_MIPS,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ","); //s
                uint8_t cnt = 0;
                uint32_t mips = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "0x%x", (unsigned int *)&mips);
                        LOGMSGIDI("mips=%x(%d)\r\n", 2, mips, mips);
                    } else {
                        //printf("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                uint8_t result = external_dsp_set_clock_rate(mips);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_GET_MIPS") != NULL) {
                uint32_t mips = 0;
                uint8_t result = external_dsp_get_clock_rate(&mips);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SPE,") != NULL) {
                char* config_s = NULL;
                unsigned int config = 0;
                config_s = strchr(parse_cmd->string_ptr, ',');
                config_s++;
                sscanf(config_s, "%x", &config);
                LOGMSGIDI("config=%x\r\n",1, config);
                external_dsp_spi_init();
                external_dsp_set_output_source(config);
                external_dsp_configure_data_path(config);
                external_dsp_spi_deinit();
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            /* SMT item for MTK automotive product */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=UL2DL,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ","); //s
                uint8_t cnt = 0, result = 0;
                bool    mix_tone = false;
                uint32_t delay_ms = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        if (strncmp(token, "true", strlen("true")) == 0) {
                            mix_tone = true;
                        } else {
                            mix_tone = false;
                        }
                        LOGMSGIDI("mix_tone=%x\r\n", 1, mix_tone);
                    } else if(cnt == 2) {
                        sscanf(token, "%x", (unsigned int *)&delay_ms);
                        LOGMSGIDI("delay_ms=%x\r\n", 1, delay_ms);
                    } else {
                        //printf("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                result = audio_test_loopback_ul2dl_mixer(mix_tone, delay_ms);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_RD_DRAM,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ",");
                uint8_t cnt = 0, result = 0;
                uint32_t address = 0;
                uint16_t value = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "0x%x", (unsigned int *)&address);
                        LOGMSGIDI("address=%x\r\n", 1, address);
                    } else {
                        //printf("parse failed:%s!\r\n", token);
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                result = external_dsp_read_dram_word(address, &value);
                sprintf((char *)presponse->response_buf, "0x%x", value);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_WR_DRAM,") != NULL) {
                char *token = strtok(parse_cmd->string_ptr, ",");
                uint8_t cnt = 0, result = 0;
                uint32_t address = 0;
                uint32_t value = 0;
                while( token != NULL) {
                    if(cnt == 1) {
                        sscanf(token, "%x", (unsigned int *)&address);
                        LOGMSGIDI("address=%x\r\n", 1, address);
                    #if 1
                    } else if(cnt == 2) {
                        sscanf(token, "%x", (unsigned int *)&value);
                        LOGMSGIDI("value=%x\r\n", 1, value);
                    #endif
                    }
                    cnt ++;
                    token = strtok(NULL, ",");
                }
                LOGMSGIDI("address=%x, value=%x\r\n",2, address, value);
                result = external_dsp_write_dram_word(address, (uint16_t)value);
                sprintf((char *)presponse->response_buf, "0x%x", (unsigned int)value);
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#if defined(HAL_AUDIO_SLT_ENABLE) || defined(MTK_BUILD_SMT_LOAD)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_1") != NULL) {
                LOGMSGIDI("SMT Test 1. Dual dmic bypass test\r\n", 0);
                uint8_t result = audio_test_dual_dmic_bypass();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_2") != NULL) {
                LOGMSGIDI("SMT Test 2-1. Download external dsp firmware\r\n", 0);
                uint8_t result = audio_test_download_external_dsp();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SMT_3") != NULL) {
                LOGMSGIDI("SMT Test 2-2. I2S external loopback\r\n", 0);
                uint8_t result = audio_test_i2s_external_loopback();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DSP_SLT") != NULL) {
                LOGMSGIDI("[Audio Ext-DSP]SLT Test\r\n", 0);
                uint8_t result = audio_external_dsp_slt_test();
                if(result == 0) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
                else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                atci_send_response(presponse);
            }
#endif /* defined(HAL_AUDIO_SLT_ENABLE) || defined(MTK_BUILD_SMT_LOAD) */
#endif /* #ifdef MTK_EXTERNAL_DSP_ENABLE */

#if defined(MTK_BT_AWS_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AWS,") != NULL) {
                char *string_pointer = (char *)parse_cmd->string_ptr;
                int32_t result;
                if (strstr(string_pointer, "AWS,STATUS") != NULL) {
                    aws_clock_skew_status_t status;
                    status = audio_service_aws_get_clock_skew_status();
                    if (status == AWS_CLOCK_SKEW_STATUS_IDLE) {
                        LOGMSGIDI("[AWS AT CMD]AWS_CLOCK_SKEW_STATUS_IDLE \n", 0);
                        result = HAL_AUDIO_AWS_NORMAL;
                    } else if (status == AWS_CLOCK_SKEW_STATUS_BUSY) {
                        LOGMSGIDI("[AWS AT CMD]AWS_CLOCK_SKEW_STATUS_BUSY \n", 0);
                        result = HAL_AUDIO_AWS_NORMAL;
                    } else {
                        result = HAL_AUDIO_AWS_ERROR;
                    }
                } else if (strstr(string_pointer, "AWS,SKEW,") != NULL) {
                    char *p_string = strstr(string_pointer, "AWS,SKEW,");
                    int32_t val = 0;
                    p_string += strlen("AWS,SKEW,");
                    sscanf(p_string, "%d", (int *)&val);
                    result = audio_service_aws_set_clock_skew_compensation_value(val);
                } else if (strstr(string_pointer, "AWS,CNT") != NULL) {
                    uint32_t value;
                    hal_gpio_init(HAL_GPIO_12);
                    hal_pinmux_set_function(HAL_GPIO_12, 0);
                    hal_gpio_set_direction(HAL_GPIO_12, HAL_GPIO_DIRECTION_OUTPUT);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
                    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                    value = audio_service_aws_get_accumulated_sample_count();
                    LOGMSGIDI("[AWS AT CMD]AWS Accumulate count: %d \n",1, value);
                    result = HAL_AUDIO_AWS_NORMAL;
                } else if (strstr(string_pointer, "AWS,FILLSIL,AAC,") != NULL) {
                    char *p_string = strstr(string_pointer, "AWS,FILLSIL,AAC,");
                    uint32_t *sil_frm_cnt;
                    uint32_t temp = 100;
                    uint32_t *byte_cnt = &temp;
                    LOGMSGIDI("byte_cnt: %d",1, *byte_cnt);
                    uint32_t val;
                    p_string += strlen("AWS,FILLSIL,AAC,");
                    sscanf(p_string, "%d", (unsigned int*)&val);
                    sil_frm_cnt = &val;
                    LOGMSGIDI("sil_frame_count: %d",1, *sil_frm_cnt);
                    uint8_t temp_buffer[100];
                    memset(temp_buffer, 0, 100 * sizeof(uint8_t));
                    LOGMSGIDI("[AWS AT CMD]before fill silence: buffer space byte count: %d, sil frm user want to fill: %d",2, *byte_cnt, *sil_frm_cnt);
                    result = audio_service_aws_fill_silence_frame(temp_buffer, byte_cnt, AWS_CODEC_TYPE_AAC_FORMAT, sil_frm_cnt);
                    LOGMSGIDI("[AWS AT CMD]after fill silence: buffer space byte count: %d, sil frm user fill: %d",2, *byte_cnt, *sil_frm_cnt);
                    LOGMSGIDI("[AWS AT CMD]buffer print: ", 0);
                    for (int i = 0; i < 100; i++) {
                        LOGMSGIDI("0x%x ",1, temp_buffer[i]);
                    }
                } else {
                    LOGMSGIDE("[AWS AT CMD]Invalid AWS AT command\r\n", 0);
                    result = HAL_AUDIO_AWS_ERROR;
                }
                presponse->response_flag = result == HAL_AUDIO_AWS_NORMAL ? ATCI_RESPONSE_FLAG_APPEND_OK : ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif  /* defined(MTK_BT_AWS_ENABLE) */
#endif /* defined(HAL_AUDIO_TEST_ENABLE) */

#if defined(MTK_BT_HFP_CODEC_DEBUG)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=DEBUG,") != NULL) {   // for debug use
                char *string_pointer = (char *)parse_cmd->string_ptr;
                int32_t result;
                if (strstr(string_pointer, "DEBUG,HFP,") != NULL) { // debug hfp
                    if (strstr(string_pointer, "DEBUG,HFP,DL_ESCO,") != NULL) { // debug hfp DL_ESCO
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,DL_ESCO,");
                        p_string += strlen("DEBUG,HFP,DL_ESCO,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            LOGMSGIDI("DEBUG,HFP,DL_ESCO: invalid save_or_print_method(%d)\r\n",1, save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,DL_STREAM_OUT_PCM,") != NULL) {
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,DL_STREAM_OUT_PCM,");
                        p_string += strlen("DEBUG,HFP,DL_STREAM_OUT_PCM,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            LOGMSGIDI("DEBUG,HFP,DL_STREAM_OUT_PCM: invalid save_or_print_method(%d)\r\n",1, save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,VM,") != NULL) {
                        char *p_string = strstr(string_pointer, "DEBUG,HFP,VM,");
                        p_string += strlen("DEBUG,HFP,VM,");

                        uint32_t save_or_print_method = 0;
                        sscanf(p_string, "%d", (int *)&save_or_print_method);
                        if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_VM_LOG, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else if (save_or_print_method == AT_AUDIO_HFP_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT) {
                            bt_hfp_codec_debug_open(BT_HFP_CODEC_DEBUG_FLAG_VM_LOG, BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT);
                            result = ATCI_RESPONSE_FLAG_APPEND_OK;
                        } else {
                            LOGMSGIDI("DEBUG,HFP,VM: invalid save_or_print_method(%d)\r\n",1, save_or_print_method);
                            result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        }
                    } else if (strstr(string_pointer, "DEBUG,HFP,STOP") != NULL) {
                        bt_hfp_codec_debug_close();
                        result = ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        LOGMSGIDE("Invalid DEBUG,HFP AT command\r\n",0);
                        result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                } else {
                    LOGMSGIDE("Invalid DEBUG AT command : \r\n",0);
                    result = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_flag = result;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif  /* defined(BT_HFP_CODEC_DEBUG) */

#if defined(MTK_AUDIO_PLC_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO= Audio_PLC_ON") != NULL) {
                            LOGMSGIDI("AUDIO_PLC_ON",0);
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_PLC_CONTROL_TEMP, 0, (uint32_t)1, false);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO= Audio_PLC_OFF") != NULL) {
                            LOGMSGIDI("AUDIO_PLC_OFF",0);
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_PLC_CONTROL_TEMP, 0, (uint32_t)0, false);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
#endif  /* defined(MTK_AUDIO_PLC_ENABLE) */
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO= CLK_SKEW_Debug_ON") != NULL) {
                            LOGMSGIDI("Turn On CLK_SKEW Debug Log",0);
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_CLK_SKEW_DEBUG_CONTROL_TEMP, 0, (uint32_t)1, false);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO= CLK_SKEW_Debug_OFF") != NULL) {
                            LOGMSGIDI("Turn Off CLK_SKEW Debug Log",0);
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_CLK_SKEW_DEBUG_CONTROL_TEMP, 0, (uint32_t)0, false);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=AUDIO_TRANSMITTER") != NULL) {
                bool atci_ret_status = true;
                audio_transmitter_status_t status;
                audio_transmitter_config_t config;
                char *string_pointer = (char *)parse_cmd->string_ptr;
                uint8_t scenario_type = 0,scenario_id = 0;
                memset(&config, 0, sizeof(audio_transmitter_config_t));

                if (strstr(string_pointer, "AUDIO_TRANSMITTER,init") != NULL) {
                    if (strstr(string_pointer, "init,0") != NULL) {
                        /* For AUDIO_TRANSMITTER_A2DP_SOURCE */
                        atci_ret_status = false;
                    } else if (strstr(string_pointer, "init,1,") != NULL) {
                        /* For AUDIO_TRANSMITTER_GSENSOR */
#if defined (MTK_SENSOR_SOURCE_ENABLE)
                        char *para1;
                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get id*/
                        scenario_type = AUDIO_TRANSMITTER_GSENSOR;
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&scenario_id);
                        } else {
                            LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                            atci_ret_status = false;
                        }
                        if (atci_ret_status == true) {
                            config.scenario_type = scenario_type;
                            config.scenario_sub_id = scenario_id;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            LOGMSGIDI("[audio_transmitter]: gsensor source init", 0);
                        }
#else
                        atci_ret_status = false;
#endif
                    } else if (strstr(string_pointer, "init,2") != NULL) {
                        /* For AUDIO_TRANSMITTER_MULTI_MIC_STREAM */
#if defined (MTK_MULTI_MIC_STREAM_ENABLE)
                        char *para1, *para2, *para3, *para4, *para5;
                        uint32_t sample_rate, resolution, index, frame_size, frame_number;

                        LOGMSGIDI("[audio_transmitter]: multi mic init\r\n", 0);

                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get id*/
                        para2 = strtok(NULL, ","); /* get sample rate */
                        para3 = strtok(NULL, ","); /* get resolution */
                        para4 = strtok(NULL, ","); /* get frame samples */
                        para5 = strtok(NULL, ","); /* get frame number */
                        scenario_type = AUDIO_TRANSMITTER_MULTI_MIC_STREAM;
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&scenario_id);
                        } else {
                            LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                        }
                        if (para2 != NULL) {
                            sscanf(para2, "%d", (unsigned int *)&sample_rate);
                        } else {
                            LOGMSGIDE("[audio_transmitter]: sample rate is not found\r\n", 0);
                        }
                        if (para3 != NULL) {
                            sscanf(para3, "%d", (unsigned int *)&resolution);
                            if ((resolution != 8) && (resolution != 16) && (resolution != 24)) {
                                LOGMSGIDE("[audio_transmitter]: resolution is not valid\r\n", 0);
                                para3 = NULL;
                            }
                        } else {
                            LOGMSGIDE("[audio_transmitter]: resolution is not found\r\n", 0);
                        }
                        if (para4 != NULL) {
                            sscanf(para4, "%d", (unsigned int *)&frame_size);
                        } else {
                            LOGMSGIDE("[audio_transmitter]: frame sample is not found\r\n", 0);
                        }
                        if (para5 != NULL) {
                            sscanf(para5, "%d", (unsigned int *)&frame_number);
                        } else {
                            LOGMSGIDE("[audio_transmitter]: frame number is not found\r\n", 0);
                        }
                        if ((para1 == NULL) || (para2 == NULL) || (para3 == NULL) || (para4 == NULL) || (para5 == NULL)) {
                            atci_ret_status = false;
                        }

                        if (atci_ret_status == true) {
                            config.scenario_type = scenario_type;
                            config.scenario_sub_id = scenario_id;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            config.scenario_config.multi_mic_stream_config.echo_reference = audio_multi_instance_ctrl.echo_path_enabled;
                            config.scenario_config.multi_mic_stream_config.mic_configs[0].audio_device = audio_multi_instance_ctrl.audio_device;
                            config.scenario_config.multi_mic_stream_config.mic_configs[0].audio_interface = audio_multi_instance_ctrl.audio_interface;
                            config.scenario_config.multi_mic_stream_config.mic_configs[1].audio_device = audio_multi_instance_ctrl.audio_device1;
                            config.scenario_config.multi_mic_stream_config.mic_configs[1].audio_interface = audio_multi_instance_ctrl.audio_interface1;
                            config.scenario_config.multi_mic_stream_config.mic_configs[2].audio_device = audio_multi_instance_ctrl.audio_device2;
                            config.scenario_config.multi_mic_stream_config.mic_configs[2].audio_interface = audio_multi_instance_ctrl.audio_interface2;
                            config.scenario_config.multi_mic_stream_config.mic_configs[3].audio_device = audio_multi_instance_ctrl.audio_device3;
                            config.scenario_config.multi_mic_stream_config.mic_configs[3].audio_interface = audio_multi_instance_ctrl.audio_interface3;
                            config.scenario_config.multi_mic_stream_config.mic_para = NULL;
                            config.scenario_config.multi_mic_stream_config.sampling_rate = sample_rate;
                            config.scenario_config.multi_mic_stream_config.frame_size = (uint16_t)frame_size;
                            config.scenario_config.multi_mic_stream_config.frame_number = (uint16_t)frame_number;
                            if (resolution == 8) {
                                config.scenario_config.multi_mic_stream_config.format = AFE_PCM_FORMAT_S8;
                            } else if (resolution == 16) {
                                config.scenario_config.multi_mic_stream_config.format = AFE_PCM_FORMAT_S16_LE;
                            } else {
                                config.scenario_config.multi_mic_stream_config.format = AFE_PCM_FORMAT_S24_LE;
                            }
                            LOGMSGIDI("[audio_transmitter]: sample rate = %d, format = %d, frame samples = %d, frame number = %d\r\n", 4,
                                                config.scenario_config.multi_mic_stream_config.sampling_rate,
                                                config.scenario_config.multi_mic_stream_config.format,
                                                config.scenario_config.multi_mic_stream_config.frame_size,
                                                config.scenario_config.multi_mic_stream_config.frame_number);
                            for (index = 0; index < MAX_MULTI_MIC_STREAM_NUMBER; index++) {
                                LOGMSGIDI("[audio_transmitter]: channel %d (device %d - interface %d)\r\n", 3, index + 1,
                                                    config.scenario_config.multi_mic_stream_config.mic_configs[index].audio_device,
                                                    config.scenario_config.multi_mic_stream_config.mic_configs[index].audio_interface);
                            }
                            LOGMSGIDI("[audio_transmitter]: echo_reference %d", 1, config.scenario_config.multi_mic_stream_config.echo_reference);
                        }
#else
                        atci_ret_status = false;
#endif
                    } else if (strstr(string_pointer, "init,3") != NULL) {
                        /* For AUDIO_TRANSMITTER_GAMING_MODE */
                        atci_ret_status = false;

                    } else if (strstr(string_pointer, "init,5") != NULL) {
                        /* For AUDIO LOOPBACK TEST */
#if defined (MTK_AUDIO_LOOPBACK_TEST_ENABLE)
                        char *para1, *para2;
                        uint32_t device, interface;
                        LOGMSGIDE("[audio_transmitter][receive]: audio loopback test init\r\n", 0);

                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get device */
                        para2 = strtok(NULL, ","); /* get interface */
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&device);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: device is not found\r\n", 0);
                        }
                        if (para2 != NULL) {
                            sscanf(para2, "%d", (unsigned int *)&interface);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: interface is not found\r\n", 0);
                        }

                        if ((para1 == NULL) || (para2 == NULL)) {
                            atci_ret_status = false;
                        }

                        if (atci_ret_status == true) {
                            n9_dsp_share_info_t *share_info = hal_audio_query_bt_audio_dl_share_info();
                            memset(share_info, 0, sizeof(uint32_t) * 4);
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_LOOPBACK_TEST, 0, (uint32_t)share_info, false);

                            scenario_type = AUDIO_TRANSMITTER_TEST;
                            config.scenario_type = AUDIO_TRANSMITTER_TEST;
                            config.scenario_sub_id = AUDIO_TRANSMITTER_TEST_AUDIO_LOOPBACK;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_device = device;
                            config.scenario_config.audio_transmitter_test_config.audio_loopback_test_config.audio_interface = interface;
                        }
#else
                        atci_ret_status = false;
#endif
                    } else if (strstr(string_pointer, "init,6") != NULL) {
                        /* For TDM */
#if defined (AIR_AUDIO_I2S_SLAVE_TDM_ENABLE)
                        char *para1, *para2, *para3, *para4;
                        uint32_t in_memory, out_memory, in_interface, out_interface;

                        LOGMSGIDE("[audio_transmitter][receive]: tdm init\r\n", 0);

                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get in_mem*/
                        para2 = strtok(NULL, ","); /* get out_mem*/
                        para3 = strtok(NULL, ","); /* get in_interface*/
                        para4 = strtok(NULL, ","); /* get out_interface*/
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&in_memory);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: in_memory is not found\r\n", 0);
                        }
                        if (para2 != NULL) {
                            sscanf(para2, "%d", (unsigned int *)&out_memory);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: out_memory is not found\r\n", 0);
                        }
                        if (para3 != NULL) {
                            sscanf(para3, "%d", (unsigned int *)&in_interface);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: in_interface is not found\r\n", 0);
                        }
                        if (para4 != NULL) {
                            sscanf(para4, "%d", (unsigned int *)&out_interface);
                        } else {
                            LOGMSGIDE("[audio_transmitter][receive]: out_interface is not found\r\n", 0);
                        }

                        if ((para1 == NULL) || (para2 == NULL) || (para3 == NULL) || (para4 == NULL)) {
                            atci_ret_status = false;
                        }
                        if (atci_ret_status == true) {
                            scenario_type = AUDIO_TRANSMITTER_TDM;
                            config.scenario_type = scenario_type;
                            config.scenario_sub_id = 0;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            config.scenario_config.tdm_config.in_memory = in_memory;
                            config.scenario_config.tdm_config.out_memory = out_memory;
                            config.scenario_config.tdm_config.in_interface = in_interface;
                            config.scenario_config.tdm_config.out_interface = out_interface;
                        }
#else
                        atci_ret_status = false;
#endif
                        } else if (strstr(string_pointer, "init,7") != NULL) {
                        /* For AUDIO_TRANSMITTER_WIRED_AUDIO */
#if defined (AIR_WIRED_AUDIO_ENABLE)
                        char *para1;
                        LOGMSGIDE("[audio_transmitter]: wired audio init\r\n", 0);
                        scenario_type = AUDIO_TRANSMITTER_WIRED_AUDIO;
                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get id*/
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&scenario_id);
                        }
                        else {
                            LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                            atci_ret_status = false;
                        }

                        if (atci_ret_status == true) {
                            config.scenario_type = scenario_type;
                            config.scenario_sub_id = scenario_id;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            LOGMSGIDI("[audio_transmitter]: wired audio %d,%d init", 2,scenario_type,scenario_id);
                        }
#else
                        atci_ret_status = false;
#endif
                        } else if (strstr(string_pointer, "init,10") != NULL) {
#if defined (AIR_AUDIO_HW_LOOPBACK_ENABLE)
                        char *para1;
                        LOGMSGIDI("[audio_transmitter]: audio hw loopback init\r\n", 0);
                        scenario_type = AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK;
                        para1 = strtok(string_pointer, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ",");
                        para1 = strtok(NULL, ","); /* get id*/
                        if (para1 != NULL) {
                            sscanf(para1, "%d", (unsigned int *)&scenario_id);
                        }
                        else {
                            LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                            atci_ret_status = false;
                        }
                        if (atci_ret_status == true) {
                            config.scenario_type = scenario_type;
                            config.scenario_sub_id = scenario_id;
                            config.msg_handler = user_audio_transmitter_callback;
                            config.user_data = NULL;
                            LOGMSGIDI("[audio_transmitter]: audio hw loopback %d,%d init", 2,scenario_type,scenario_id);
                        }
#else
                        atci_ret_status = false;
#endif
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario not support", 0);
                        atci_ret_status = false;
                    }

                    if (atci_ret_status == true) {
                        int i;
                        for (i = 0; i < TRANSMITTER_ID_MAX; i++)
                        {
                            if(g_transmitter_user[i].transmitter_id < 0){
                                config.user_data = &g_transmitter_user[i];
                                audio_transmitter_id_t id = audio_transmitter_init(&config);
                                if (id < 0) {
                                    LOGMSGIDE("[audio_transmitter]: init false", 0);
                                    atci_ret_status = false;
                                }
                                else{
                                    g_transmitter_user[i].transmitter_id = id;
                                    g_transmitter_user[i].scenario_type = scenario_type;
                                    g_transmitter_user[i].scenario_sub_id = scenario_id;
                                    LOGMSGIDI("[audio_transmitter]: init: id = %d, scenario_type %d, scenario_id %d", 3, id, scenario_type, scenario_id);
                                    atci_ret_status = true;
                                }
                                break;
                            }
                        }
                        if(i == TRANSMITTER_ID_MAX){
                            LOGMSGIDE("[audio_transmitter]: init more than TRANSMITTER_ID_MAX", 0);
                            atci_ret_status = false;
                        }
                    }
                } else if (strstr(string_pointer, "AUDIO_TRANSMITTER,start") != NULL) {
                    char *string_pointer = (char *)parse_cmd->string_ptr;
                    uint8_t scenario_type, scenario_id;
                    char *para1, *para2;
                    para1 = strtok(string_pointer, ",");
                    para1 = strtok(NULL, ",");
                    para1 = strtok(NULL, ","); /* get type*/
                    para2 = strtok(NULL, ","); /* get id*/
                    if (para1 != NULL) {
                        sscanf(para1, "%d", (unsigned int *)&scenario_type);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario type is not found\r\n", 0);
                    }
                    if (para2 != NULL) {
                        sscanf(para2, "%d", (unsigned int *)&scenario_id);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                    }
                    if ((para1 == NULL) || (para2 == NULL)) {
                            atci_ret_status = false;
                    }
                    else{
                        int i;
                        for (i = 0; i < TRANSMITTER_ID_MAX; i++)
                        {
                            if((g_transmitter_user[i].scenario_type == scenario_type)
                                && (g_transmitter_user[i].scenario_sub_id == scenario_id)){
                                status = audio_transmitter_start(g_transmitter_user[i].transmitter_id);
                                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                                    atci_ret_status = false;
                                }
                                else{
                                    LOGMSGIDI("[audio_transmitter]: start type %d, id %d: status = %d", 3, scenario_type,scenario_id, status);
                                    while(g_transmitter_start_ok == false);
                                    g_transmitter_start_ok = false;
                                    atci_ret_status = true;
                                }
                                break;
                            }
                        }
                        if(i == TRANSMITTER_ID_MAX){
                            atci_ret_status = false;
                        }
                    }
                } else if (strstr(string_pointer, "AUDIO_TRANSMITTER,stop") != NULL) {
                    char *string_pointer = (char *)parse_cmd->string_ptr;
                    uint8_t scenario_type, scenario_id;
                    char *para1, *para2;
                    para1 = strtok(string_pointer, ",");
                    para1 = strtok(NULL, ",");
                    para1 = strtok(NULL, ","); /* get type*/
                    para2 = strtok(NULL, ","); /* get id*/
                    if (para1 != NULL) {
                        sscanf(para1, "%d", (unsigned int *)&scenario_type);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario type is not found\r\n", 0);
                    }
                    if (para2 != NULL) {
                        sscanf(para2, "%d", (unsigned int *)&scenario_id);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                    }
                    if ((para1 == NULL) || (para2 == NULL)) {
                            atci_ret_status = false;
                    }
                    else{
                        int i;
                        for (i = 0; i < TRANSMITTER_ID_MAX; i++)
                        {
                            if((g_transmitter_user[i].scenario_type == scenario_type)
                                && (g_transmitter_user[i].scenario_sub_id == scenario_id)){
                                status = audio_transmitter_stop(g_transmitter_user[i].transmitter_id);
                                LOGMSGIDI("[audio_transmitter]: stop type %d, id %d: status = %d", 3, scenario_type,scenario_id, status);
                                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                                    atci_ret_status = false;
                                }
                                else{
                                    atci_ret_status = true;
                                    while(g_transmitter_stop_ok == false);
                                    g_transmitter_stop_ok = false;
                                }
                                break;
                            }
                        }
                        if(i == TRANSMITTER_ID_MAX){
                            atci_ret_status = false;
                        }
                    }
                } else if (strstr(string_pointer, "AUDIO_TRANSMITTER,deinit") != NULL) {
                    char *string_pointer = (char *)parse_cmd->string_ptr;
                    uint8_t scenario_type, scenario_id;
                    char *para1, *para2;
                    para1 = strtok(string_pointer, ",");
                    para1 = strtok(NULL, ",");
                    para1 = strtok(NULL, ","); /* get type*/
                    para2 = strtok(NULL, ","); /* get id*/
                    if (para1 != NULL) {
                        sscanf(para1, "%d", (unsigned int *)&scenario_type);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario type is not found\r\n", 0);
                    }
                    if (para2 != NULL) {
                        sscanf(para2, "%d", (unsigned int *)&scenario_id);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                    }
                    if ((para1 == NULL) || (para2 == NULL)) {
                            atci_ret_status = false;
                    }
                    else{
                        int i;
                        for (i = 0; i < TRANSMITTER_ID_MAX; i++)
                        {
                            if((g_transmitter_user[i].scenario_type == scenario_type)
                                && (g_transmitter_user[i].scenario_sub_id == scenario_id)){
                                status = audio_transmitter_deinit(g_transmitter_user[i].transmitter_id);
                                LOGMSGIDI("[audio_transmitter]: deinit type %d, id %d: status = %d", 3, scenario_type,scenario_id, status);
                                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                                    atci_ret_status = false;
                                }
                                else{
                                    g_transmitter_user[i].transmitter_id = -1;
                                    atci_ret_status = true;
                                }
                                break;
                            }
                        }
                        if(i == TRANSMITTER_ID_MAX){
                            atci_ret_status = false;
                        }
                    }
                } else if (strstr(string_pointer, "AUDIO_TRANSMITTER,set_runtime_config") != NULL) {
                    char *string_pointer = (char *)parse_cmd->string_ptr;
                    uint8_t scenario_type, scenario_id, config_type, param=0;
                    char *para1, *para2, *para3, *para4;
                    para1 = strtok(string_pointer, ",");
                    para1 = strtok(NULL, ",");
                    para1 = strtok(NULL, ","); /* get type*/
                    para2 = strtok(NULL, ","); /* get id*/
                    para3 = strtok(NULL, ","); /* get config op*/
                    para4 = strtok(NULL, ","); /* get config param*/

                    _UNUSED(param);
                    _UNUSED(para4);
                    if (para1 != NULL) {
                        sscanf(para1, "%d", (unsigned int *)&scenario_type);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario type is not found\r\n", 0);
                    }
                    if (para2 != NULL) {
                        sscanf(para2, "%d", (unsigned int *)&scenario_id);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: scenario id is not found\r\n", 0);
                    }
                    if (para3 != NULL) {
                        sscanf(para3, "%d", (unsigned int *)&config_type);
                    } else {
                        LOGMSGIDE("[audio_transmitter]: config_type is not found\r\n", 0);
                    }
                    if ((para1 == NULL) || (para2 == NULL)|| (para3 == NULL)) {
                            atci_ret_status = false;
                    }
                    else{
                        int i;
                        for (i = 0; i < TRANSMITTER_ID_MAX; i++)
                        {
                            if((g_transmitter_user[i].scenario_type == scenario_type)
                                && (g_transmitter_user[i].scenario_sub_id == scenario_id)){

                                audio_transmitter_runtime_config_t runtime_config;
                                memset(&runtime_config, 0, sizeof(audio_transmitter_runtime_config_t));

#if defined(AIR_WIRED_AUDIO_ENABLE)
                                if (scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
                                {
                                    if (para4 != NULL) {
                                        sscanf(para4, "%d", (unsigned int *)&param);
                                    } else {
                                        LOGMSGIDE("[audio_transmitter]: param is not found\r\n", 0);
                                    }
                                    runtime_config.wired_audio_runtime_config.vol_level.vol_level_l = param;
                                }
#endif /*AIR_WIRED_AUDIO_ENABLE*/

                                status = audio_transmitter_set_runtime_config(g_transmitter_user[i].transmitter_id, config_type, &runtime_config);
                                LOGMSGIDI("[audio_transmitter]: set_runtime_config type %d, id %d: status = %d", 3, scenario_type,scenario_id, status);
                                if (status != AUDIO_TRANSMITTER_STATUS_SUCCESS) {
                                    atci_ret_status = false;
                                }
                                else{
                                    atci_ret_status = true;
                                }
                                break;
                            }
                        }
                        if(i == TRANSMITTER_ID_MAX){
                            atci_ret_status = false;
                        }
                    }
                } else {
                    LOGMSGIDE("[audio_transmitter]: Operation not support", 0);
                    atci_ret_status = false;
                }

                if (atci_ret_status == true) {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
#endif//(defined (MTK_AUDIO_TRANSMITTER_ENABLE))
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=GAME_ULL_LATENCY_ON_1") != NULL) {
                            LOGMSGIDI("GAME_ULL_LATENCY_ON_1",0);
                            extern uint16_t gaming_mode_latency_debug_1;
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            gaming_mode_latency_debug_1 = 1;
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=GAME_ULL_LATENCY_OFF_1") != NULL) {
                            LOGMSGIDI("GAME_ULL_LATENCY_OFF_1",0);
                            extern uint16_t gaming_mode_latency_debug_1;
                            gaming_mode_latency_debug_1 = 0;
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=GAME_ULL_LATENCY_ON") != NULL) {
                            LOGMSGIDI("GAME_ULL_LATENCY_ON",0);
                            extern uint16_t gaming_mode_latency_debug_0;
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            gaming_mode_latency_debug_0 = 1;
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=GAME_ULL_LATENCY_OFF") != NULL) {
                            LOGMSGIDI("GAME_ULL_LATENCY_OFF",0);
                            extern uint16_t gaming_mode_latency_debug_0;
                            gaming_mode_latency_debug_0 = 0;
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
#endif  /* defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE) */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_1") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_DL_LATENCY_ON_1",0);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            extern void ble_audio_dongle_rx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_rx_latency_debug_control(0, true);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_1") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_DL_LATENCY_OFF_1",0);
                            extern void ble_audio_dongle_rx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_rx_latency_debug_control(0, false);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_ON_2") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_DL_LATENCY_ON_2",0);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            extern void ble_audio_dongle_rx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_rx_latency_debug_control(1, true);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_DL_LATENCY_OFF_2") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_DL_LATENCY_OFF_2",0);
                            extern void ble_audio_dongle_rx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_rx_latency_debug_control(1, false);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_ON_1") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_UL_LATENCY_ON_1",0);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            extern void ble_audio_dongle_tx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_tx_latency_debug_control(0, true);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
            else if (strstr((char *)parse_cmd->string_ptr, "AT+EAUDIO=BLE_AUDIO_DONGLE_UL_LATENCY_OFF_1") != NULL) {
                            LOGMSGIDI("BLE_AUDIO_DONGLE_UL_LATENCY_OFF_1",0);
                            extern void ble_audio_dongle_tx_latency_debug_control(uint32_t port, bool enable);
                            ble_audio_dongle_tx_latency_debug_control(0, false);
                            hal_gpio_set_output(HAL_GPIO_13, HAL_GPIO_DATA_LOW);
                            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                            presponse->response_len = strlen((const char *)presponse->response_buf);
                            atci_send_response(presponse);
            }
#endif /* defined(AIR_BLE_AUDIO_DONGLE_ENABLE) */
            else
            {
                /* invalid AT command */
                LOGMSGIDI("atci_cmd_hdlr_audio: command not exist \r\n", 0);
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                presponse->response_len = strlen((const char *)presponse->response_buf);
                atci_send_response(presponse);
            }
            break;

        default :{
            /* others are invalid command format */
            LOGMSGIDI("atci_cmd_hdlr_audio: command mode not support. \r\n", 0);
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            presponse->response_len = strlen((const char *)presponse->response_buf);
            atci_send_response(presponse);
            break;
        }
    }

    vPortFree(presponse);
#endif /*defined(__GNUC__)*/
    return ATCI_STATUS_OK;
}

#endif /* !MTK_AUDIO_AT_COMMAND_DISABLE && (HAL_AUDIO_MODULE_ENABLED) */
