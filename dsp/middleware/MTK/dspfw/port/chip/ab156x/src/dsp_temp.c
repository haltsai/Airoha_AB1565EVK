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
#include "dsp_audio_ctrl.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_control.h"
#include "hal_audio.h"
#include "dsp_audio_msg_define.h"
#include "dsp_scenario.h"
#include "hal_ccni.h"
#include "dsp_gain_control.h"
#include "audio_afe_common.h"
#ifdef MTK_ANC_ENABLE
#include "anc_api.h"
#endif
#include "stream_n9sco.h"
#include "clk_skew.h"

#ifdef AIR_SILENCE_DETECTION_ENABLE
#include "silence_detection_interface.h"
#endif

log_create_module(dsp_mw, PRINT_LEVEL_INFO);


/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

void Audio_Setup(void)
{
    /*
        rx_ack_option
        bit[0] (1): NO ACK
        bit[1] (2): execute callback function before response ACK.
        others (0): execute callback function after response ACK.
       */

#ifdef MTK_SENSOR_SOURCE_ENABLE
    /*gsensor ccni msg callback func. register*/
    aud_msg_rx_register_callback(MSG_MCU2DSP_GSENSOR_DETECT_READ_RG, CB_GSENSOR_DETECT_READ_RG, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_GSENSOR_DETECT_WRITE_RG, CB_GSENSOR_DETECT_WRITE_RG, 1);
#endif

#ifdef MTK_BT_A2DP_ENABLE
    /* A2DP ccni msg callback func. register */
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_OPEN, CB_N9_A2DP_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_START, CB_N9_A2DP_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_STOP, CB_N9_A2DP_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_CLOSE, CB_N9_A2DP_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_SUSPEND, CB_N9_A2DP_SUSPEND, 0);//suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_RESUME, CB_N9_A2DP_RESUME, 0);//resume
    #ifdef MTK_GAMING_MODE_HEADSET
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_DL_PLAY_EN_FROM_BTCON, dsp_a2dp_set_dl_play_en, 1);//get play en
    #endif
#endif

#ifdef MTK_BT_HFP_ENABLE
    /* eSCO ccni msg callback func. register */
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_OPEN, CB_N9_SCO_UL_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_START, CB_N9_SCO_UL_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_STOP, CB_N9_SCO_UL_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_CLOSE, CB_N9_SCO_UL_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_PLAY_EN, CB_N9_SCO_UL_PLAY, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_SUSPEND, CB_N9_SCO_UL_SUSPEND, 0);
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_UL_RESUME, CB_N9_SCO_UL_RESUME, 0);

    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_OPEN, CB_N9_SCO_DL_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_START, CB_N9_SCO_DL_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_STOP, CB_N9_SCO_DL_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_CLOSE, CB_N9_SCO_DL_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_SUSPEND, CB_N9_SCO_DL_SUSPEND, 0);//suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_RESUME, CB_N9_SCO_DL_RESUME, 0);//resume
#endif

	// TODO: create new irq msg for BLE
#ifdef AIR_BT_CODEC_BLE_ENABLED
    aud_msg_rx_register_callback(MSG_N92DSP_DL_IRQ, CB_N9_BLE_DLIRQ, 1);
    aud_msg_rx_register_callback(MSG_N92DSP_UL_IRQ, CB_N9_BLE_ULIRQ, 1);

    /* BLE ccni msg callback func. register */
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_OPEN,    CB_N9_BLE_UL_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_START,   CB_N9_BLE_UL_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_STOP,    CB_N9_BLE_UL_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_CLOSE,   CB_N9_BLE_UL_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_PLAY_EN, CB_N9_BLE_UL_PLAY, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_SUSPEND, CB_N9_BLE_UL_SUSPEND, 0);
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_RESUME,  CB_N9_BLE_UL_RESUME, 0);

    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_OPEN,    CB_N9_BLE_DL_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_START,   CB_N9_BLE_DL_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_STOP,    CB_N9_BLE_DL_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_CLOSE,   CB_N9_BLE_DL_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_SUSPEND, CB_N9_BLE_DL_SUSPEND, 0);//suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_RESUME,  CB_N9_BLE_DL_RESUME, 0);//resume

    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_BUFFER_INFO, CB_N9_BLE_UL_INFO, 0);//
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_DL_BUFFER_INFO,  CB_N9_BLE_DL_INFO, 0);//
    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_UL_PLAYBACK_DATA_INFO,  CB_N9_BLE_UL_PLAYBACK_DATA_INFO, 0);//

    aud_msg_rx_register_callback(MSG_MCU2DSP_BLE_AUDIO_INIT_PLAY_INFO, CB_N9_BLE_INIT_PLAY_INFO, 0); /* Initial play info from BT controller */
#endif  //#AIR_BT_CODEC_BLE_ENABLED

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    /*Volume control callback function register*/
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, DSP_GC_SetOutputVolume, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, DSP_GC_SetInputVolume, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_MUTE_OUTPUT_DEVICE, DSP_GC_MuteOutput, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_MUTE_INPUT_DEVICE, DSP_GC_MuteInput, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_OUTPUT_VOLUME_PARAMETERS, DSP_GC_SetGainParameters, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_POWER_OFF_DAC_IMMEDIATELY, DSP_GC_POWER_OFF_DAC_IMMEDIATELY, 2);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    /*Device control callback function register*/
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE, DSP_DRV_AFE_SetOutputDevice, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE, DSP_DRV_AFE_SetInputDevice, 2);

    /*Channel number callback function register*/
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_CHANNEL, DSP_DRV_AFE_SetOutputChannel, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_CHANNEL, DSP_DRV_AFE_SetInputChannel, 2);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

    #ifdef MTK_CM4_PLAYBACK_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_OPEN, CB_CM4_PLAYBACK_OPEN, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_START, CB_CM4_PLAYBACK_START, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_STOP, CB_CM4_PLAYBACK_STOP, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_CLOSE, CB_CM4_PLAYBACK_CLOSE, 2);
    aud_msg_tx_ack_register_callback(MSG_DSP2MCU_PLAYBACK_DATA_REQUEST, CB_CM4_PLAYBACK_DATA_REQ_ACK);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_SUSPEND, CB_CM4_PLAYBACK_SUSPEND, 0);//suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_PLAYBACK_RESUME, CB_CM4_PLAYBACK_RESUME, 0);//resume
    #endif

    #ifdef MTK_AUDIO_PLC_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_PLC_CONTROL_TEMP, DSP_AUDIO_PLC_CONTROL, 1);//open
    #endif

    #ifdef AIR_BT_CLK_SKEW_ENABLE
	aud_msg_rx_register_callback(MSG_MCU2DSP_BT_AUDIO_CLK_SKEW_DEBUG_CONTROL_TEMP, DSP_CLK_SKEW_DEBUG_CONTROL, 2);
    #endif

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_HWGAIN_SET_FADE_TIME_GAIN, DSP_GC_SetHWGainWithFadeTime, 1);//open
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef MTK_LINEIN_PLAYBACK_ENABLE
    /* LINEIN_PLAYBACK ccni msg callback func. register */
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_OPEN, CB_CM4_LINEIN_PLAYBACK_OPEN, 2);//open
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_START, CB_CM4_LINEIN_PLAYBACK_START, 2);//play
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_STOP, CB_CM4_LINEIN_PLAYBACK_STOP, 1);//stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_CLOSE, CB_CM4_LINEIN_PLAYBACK_CLOSE, 2);//close
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_SUSPEND, CB_CM4_LINEIN_PLAYBACK_SUSPEND, 2);//suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, CB_CM4_LINEIN_PLAYBACK_RESUME, 2);//resume
    aud_msg_rx_register_callback(MSG_MCU2DSP_TRULY_LINEIN_PLAYBACK_OPEN, CB_CM4_TRULY_LINEIN_PLAYBACK_OPEN, 2);  //Truly LineIN open
    aud_msg_rx_register_callback(MSG_MCU2DSP_TRULY_LINEIN_PLAYBACK_CLOSE, CB_CM4_TRULY_LINEIN_PLAYBACK_CLOSE, 2);//Truly LineIN close
#endif

#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AUDIO_LOOPBACK_TEST, DSP_AUDIO_LOOPBACK_TEST_CONFIG,1);
#endif

    #ifdef MTK_CM4_RECORD_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_OPEN, CB_CM4_RECORD_OPEN, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_START, CB_CM4_RECORD_START, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_STOP, CB_CM4_RECORD_STOP, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_CLOSE, CB_CM4_RECORD_CLOSE, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_SUSPEND, CB_CM4_RECORD_SUSPEND, 0);
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_RESUME, CB_CM4_RECORD_RESUME, 0);
    #ifdef MTK_LEAKAGE_DETECTION_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_RECORD_LC_SET_PARAM, CB_CM4_RECORD_LC_SET_PARAM_ACK, 2);
    #endif
    #endif

    #ifdef AIR_SIDETONE_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SIDETONE_START, dsp_sidetone_start, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SIDETONE_STOP, dsp_sidetone_stop, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SIDETONE_SET_VOLUME, dsp_sidetone_set_volume, 1);
    #endif

    #ifdef MTK_ANC_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_ANC_START, dsp_anc_start, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_ANC_STOP, dsp_anc_stop, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_ANC_SET_VOLUME, dsp_set_anc_volume, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_ANC_SET_PARAM, dsp_set_anc_param, 2);
    #endif
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_DC_COMPENSATION_START, dsp_dc_compensation_start, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_DC_COMPENSATION_STOP, dsp_dc_compensation_stop, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, Ch_Select_Set_Param, 1);
    #ifdef MTK_BT_A2DP_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_ALC_SWITCH, dsp_alc_switch, 1);
    #endif
    #if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, dsp_peq_set_param, 2);
    #endif
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_ALGORITHM_PARAM, dsp_set_algorithm_param, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_SET, dsp_set_adaptive_anc, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_GET, dsp_get_adaptive_anc, 2);

    #ifdef HAL_AUDIO_DSP_SHUTDOWN_SPECIAL_CONTROL_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_DUMMY_DSP_SHUTDOWN, DSP_DUMMY_SHUTDOWN, 1);
    #endif

    #ifdef MTK_PROMPT_SOUND_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_OPEN, CB_CM4_VP_PLAYBACK_OPEN, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_START, CB_CM4_VP_PLAYBACK_START, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_STOP, CB_CM4_VP_PLAYBACK_STOP, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_CONFIG, CB_CM4_VP_PLAYBACK_CONFIG, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_CLOSE, CB_CM4_VP_PLAYBACK_CLOSE, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_AWS_SYNC_TRIGGER, CB_CM4_VP_PLAYBACK_TRIGGER, 1);
    aud_msg_tx_ack_register_callback(MSG_DSP2MCU_PROMPT_DATA_REQUEST, CB_CM4_VP_PLAYBACK_DATA_REQ_ACK);
    #endif

    #ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_OPEN, CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_OPEN, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_START, CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_START, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_STOP, CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_STOP, 1);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CLOSE, CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_CLOSE, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CHANGE_FEATURE, CB_CM4_VP_DUMMY_SOURCE_PLAYBACK_CHANGE_FEATURE, 0);
    #endif

    #ifdef AIR_BT_CLK_SKEW_ENABLE
    aud_msg_rx_register_callback(MSG_N92DSP_CLOCK_LEADS, CB_N9_CLK_SKEW_LEAD, 0);
    aud_msg_rx_register_callback(MSG_N92DSP_CLOCK_LAGS, CB_N9_CLK_SKEW_LAG, 0);
    #endif

    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_SYSRAM, dsp_set_sysram_addr, 0);
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_STREAM_DEINIT, dsp_streaming_deinit_all, 2);

#ifdef AIR_VOICE_NR_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_REQ_GET_REALTIME_REF_GAIN, dsp_get_reference_gain, 2);
    aud_msg_rx_register_callback(MSG_MCU2DSP_BT_VOICE_DL_AVC_PARA_SEND, CB_CM4_SCO_DL_AVC_VOL_UPDATE, 1);//avc_vol update
    #ifdef MTK_AIRDUMP_EN
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AIRDUMP_EN, CB_CM4_SCO_AIRDUMP_EN,1);
    #endif
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AEC_NR_EN, AEC_NR_Enable,1);
#endif

    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_REQ_GET_REALTIME_LIB_VERSION, dsp_query_lib_version, 2);

    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AUDIO_DUMP_MASK, CB_AUDIO_DUMP_INIT,1);

    #ifdef ENABLE_HWSRC_CLKSKEW
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_CLKSKEW_MODE_SEL, ClkSkewMode_Selection,1);
    #endif
	#ifdef MTK_SLT_AUDIO_HW
    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_AUDIO_SLT, AUDIO_SLT_START,0);
	#endif
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_AMP_FORCE_CLOSE, CB_CM4_AUDIO_AMP_FORCE_CLOSE, 2);

    aud_msg_rx_register_callback(MSG_MCU2DSP_COMMON_SET_DRIVER_PARAM, dsp_set_audio_device_parameters, 2);




#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_OPEN, audio_transmitter_open, 2);        //open
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_START, audio_transmitter_start, 2);      //start
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_STOP, audio_transmitter_stop, 1);        //stop
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_CLOSE, audio_transmitter_close, 2);      //close
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_CONFIG, audio_transmitter_config, 2);      //config
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_SUSPEND, audio_transmitter_suspend, 2);    //suspend
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_TRANSMITTER_RESUME, audio_transmitter_resume, 2);      //resume
    #ifdef MTK_GAMING_MODE_HEADSET
    aud_msg_rx_register_callback(MSG_MCU2DSP_GAME_HEADSET_UL_IRQ_EN, gaming_headset_uplink_enable_irq, 1);      //config
    #endif /* MTK_GAMING_MODE_HEADSET */
#endif

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    /* Audio Sync */
    aud_msg_rx_register_callback(MSG_MCU2DSP_AUDIO_SYNC_REQUEST, dsp_audio_request_sync_process, 2);
    dsp_audio_request_sync_initialization();
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

    aud_msg_init();
    #if A2DP_DBG_PORT
    hal_gpio_init(HAL_GPIO_16);
    hal_pinmux_set_function(HAL_GPIO_16, 0);
    hal_gpio_init(HAL_GPIO_17);
    hal_pinmux_set_function(HAL_GPIO_17, 0);
    hal_gpio_init(HAL_GPIO_30);
    hal_pinmux_set_function(HAL_GPIO_30, 0);
    hal_gpio_init(HAL_GPIO_19);
    hal_pinmux_set_function(HAL_GPIO_19, 0);
    hal_gpio_init(HAL_GPIO_32);
    hal_pinmux_set_function(HAL_GPIO_32, 0);
    hal_gpio_set_direction(HAL_GPIO_16, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_17, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_30, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_19, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_direction(HAL_GPIO_32, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_16, 0);
    hal_gpio_set_output(HAL_GPIO_17, 0);
    hal_gpio_set_output(HAL_GPIO_30, 0);
    hal_gpio_set_output(HAL_GPIO_19, 0);
    hal_gpio_set_output(HAL_GPIO_32, 0);
    #endif
    #if 0  /*1568 GPIO*/
    hal_gpio_init(HAL_GPIO_20);
    hal_pinmux_set_function(HAL_GPIO_20, 0);
    hal_gpio_set_direction(HAL_GPIO_20, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_20, 0);

    hal_gpio_init(HAL_GPIO_11);
    hal_pinmux_set_function(HAL_GPIO_11, 0);
    hal_gpio_set_direction(HAL_GPIO_11, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_11, 0);

    hal_gpio_init(HAL_GPIO_3);
    hal_pinmux_set_function(HAL_GPIO_3, 0);
    hal_gpio_set_direction(HAL_GPIO_3, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_3, 0);
    #endif

#ifdef AIR_AUDIO_HARDWARE_ENABLE
    afe_register_irq_ops();
#ifdef ENABLE_AMP_TIMER
    afe_register_amp_handler();
#endif

#ifdef ENABLE_SIDETONE_RAMP_TIMER
    fw_sidetone_ramp_init();
#endif
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

#ifdef AB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK
    hal_ccni_message_t msg;
    memset(msg.ccni_message,0,sizeof(msg.ccni_message));
    hal_ccni_message_t *ack = &msg;
    CB_CM4_TRULY_LINEIN_PLAYBACK_OPEN(msg,ack);
#endif

#ifdef AIR_SILENCE_DETECTION_ENABLE
    Sink_Audio_ExtAmpOff_Control_Init();
    Sink_Audio_SilenceDetection_Init(afe_send_silence_status_ccni,NULL,Sink_Audio_ExtAmpOff_Control_Callback);
#endif

    //dsp_vad_enable(true, false);

}
