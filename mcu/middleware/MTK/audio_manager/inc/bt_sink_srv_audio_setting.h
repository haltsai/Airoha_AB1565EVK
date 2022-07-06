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

#ifndef __BT_SINK_SRV_AUDIO_SETTING_H__
#define __BT_SINK_SRV_AUDIO_SETTING_H__


#if !defined(MTK_NO_PSRAM_ENABLE) || defined(MTK_AUDIO_GAIN_TABLE_ENABLE)
#define __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
#endif

#include "stdint.h"
#include "audio_coefficient.h"

#define __GAIN_TABLE_NVDM_DIRECT__

#ifdef __GAIN_TABLE_NVDM_DIRECT__
#define PACKED __attribute__((packed))
#endif


#define BT_SINK_SRV_AUDIO_SETTING_FALG_GEN_VOL_READY                    (0x0001)


/* EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE */
#if PRODUCT_VERSION == 2533
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV                   (16)
#else
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV                   (16)
#endif

/* EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE */
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV                    (1)

#ifdef __GAIN_TABLE_NVDM_DIRECT__
/*NVDM ID string*/
/*OUT*/
#define NVDM_ID_STRING_SIZE                     5
#define NVDM_ID_A2DP_PERCENTAGE_TABLE_STRING    "F23B"
#define NVDM_ID_HFP_PERCENTAGE_TABLE_STRING     "F23A"
#define NVDM_ID_MP3_PERCENTAGE_TABLE_STRING     "F23C"
#define NVDM_ID_VPRT_PERCENTAGE_TABLE_STRING    "F23E"
#define NVDM_ID_LINEIN_PERCENTAGE_TABLE_STRING  "F23F"
#define NVDM_ID_USB_AUDIO_PERCENTAGE_TABLE_STRING  "F240"
/*IN*/
#define NVDM_ID_SCO_MIC_PERCENTAGE_TABLE_STRING "F23D"
#endif

#define NVDM_ID_AUDIO_DUMP_TABLE_STRING         "E001"

typedef uint16_t device_t;

typedef enum {
    VOL_A2DP = 0,
    VOL_HFP,
    VOL_PCM,
    VOL_MP3,
    VOL_HFP_NB,
    VOL_VP,
    VOL_VC,
    VOL_LINE_IN,
    VOL_USB_AUDIO_IN,
    VOL_ANC,
    VOL_USB_AUDIO_SW_IN,
    VOL_USB_VOICE_SW_OUT,
    VOL_LINE_IN_DL3,

    VOL_DEF, /* stream out only */
    VOL_TOTAL
} vol_type_t;

typedef enum {
    GAIN_ANALOG,
    GAIN_DIGITAL,

    GAIN_TOTAL
} gain_type_t;

typedef struct {
    uint32_t digital;
    uint32_t analog_L;
    uint32_t analog_R;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    uint32_t analog_MIC2;
    uint32_t analog_MIC3;
    uint32_t analog_MIC4;
    uint32_t analog_MIC5;
#endif
} vol_t;

typedef struct {
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    //multi mic gain setting
    uint32_t digital_MIC0_L_Digital_Vol;
    uint32_t digital_MIC0_R_Digital_Vol;
    uint32_t digital_MIC1_L_Digital_Vol;
    uint32_t digital_MIC1_R_Digital_Vol;
    uint32_t digital_MIC2_L_Digital_Vol;
    uint32_t digital_MIC2_R_Digital_Vol;
    uint32_t digital_I2S0_L_Digital_Vol;
    uint32_t digital_I2S0_R_Digital_Vol;
    uint32_t digital_I2S1_L_Digital_Vol;
    uint32_t digital_I2S1_R_Digital_Vol;
    uint32_t digital_I2S2_L_Digital_Vol;
    uint32_t digital_I2S2_R_Digital_Vol;
    uint32_t digital_LINEIN_L_Digital_Vol;
    uint32_t digital_LINEIN_R_Digital_Vol;
    uint32_t digital_Echo_Reference_Vol;
    //mic function gain setting
    uint32_t digital_MIC0_L_Func_Digital_Vol;
    uint32_t digital_MIC0_R_Func_Digital_Vol;
    uint32_t digital_MIC1_L_Func_Digital_Vol;
    uint32_t digital_MIC1_R_Func_Digital_Vol;
    uint32_t digital_MIC2_L_Func_Digital_Vol;
    uint32_t digital_MIC2_R_Func_Digital_Vol;
    uint32_t digital_Echo_Func_Reference_Vol;
    uint32_t reserved;
#else
    uint32_t digital_Ref1;
    uint32_t digital_Ref2;
    uint32_t digital_RESERVE;
    uint32_t digital_Echo;
#endif
} vol_multiMIC_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} a2dp_vol_info_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} usb_audio_vol_info_t;

typedef struct {
    vol_t vol;
} a2dp_vol_t;

typedef struct {
    vol_t vol;
} usb_audio_vol_t;

typedef struct {
    uint8_t codec;
    device_t dev_in;
    uint8_t lev_in;
    device_t dev_out;
    uint8_t lev_out;
} hfp_vol_info_t;

typedef struct {
    vol_t          vol_in;
    vol_multiMIC_t vol_multiMIC_in;
    vol_t vol_out;
} hfp_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} pcm_vol_info_t;

typedef struct {
    vol_t vol;
} pcm_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} mp3_vol_info_t;

typedef struct {
    vol_t vol;
} mp3_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} def_vol_info_t;

typedef struct {
    vol_t vol;
} def_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} vp_vol_info_t;

typedef struct {
    vol_t vol;
} vp_vol_t;

typedef struct {
    device_t dev_in;
    uint8_t lev_in;
} vc_vol_info_t;

typedef struct {
    vol_t vol;
    vol_multiMIC_t vol_multiMIC;
} vc_vol_t;

typedef struct {
    device_t  dev_in;
    uint8_t   lev_in;
    device_t  dev_out;
    uint8_t   lev_out;
} lineIN_vol_info_t;

typedef struct {
    vol_t vol_in;
    vol_t vol_out;
} lineIN_vol_t;

typedef struct {
    vol_type_t type;
    union {
        a2dp_vol_info_t   a2dp_vol_info;
        hfp_vol_info_t    hfp_vol_info;
        pcm_vol_info_t    pcm_vol_info;
        mp3_vol_info_t    mp3_vol_info;
        def_vol_info_t    def_vol_info;
        vp_vol_info_t     vp_vol_info;
        vc_vol_info_t     vc_vol_info;
        lineIN_vol_info_t lineIN_vol_info;
        usb_audio_vol_info_t usb_audio_vol_info;
    } vol_info;
} bt_sink_srv_audio_setting_vol_info_t;

typedef struct {
    vol_type_t type;
    union {
        a2dp_vol_t   a2dp_vol;
        hfp_vol_t    hfp_vol;
        pcm_vol_t    pcm_vol;
        mp3_vol_t    mp3_vol;
        def_vol_t    def_vol;
        vp_vol_t     vp_vol;
        vc_vol_t     vc_vol;
        lineIN_vol_t lineIN_vol;
        usb_audio_vol_t usb_audio_vol;
    } vol;
} bt_sink_srv_audio_setting_vol_t;

typedef struct {
    uint32_t flag;
} bt_sink_srv_audio_setting_context_t;

#ifdef __GAIN_TABLE_NVDM_DIRECT__
/* sound level to percentage from 1530 */
typedef struct
{
    uint8_t baseSoundLevel;
    uint8_t decreaseSoundLevel;
} PACKED bt_sink_srv_audio_setting_decrease_vol;

typedef struct
{
    uint8_t totalSoundLevel;
    uint8_t defaultSoundLevel;
    bt_sink_srv_audio_setting_decrease_vol DecreaseSoundLevelCtl;
} PACKED bt_sink_srv_audio_setting_vol_para;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para scoVolPara;
    uint8_t scoSoundLevelToVgs[1]; //number of totalSoundLevel
    uint8_t scoSoundLevelToBeepTone[1]; //number of totalSoundLevel
    uint8_t scoSoundlevelToPercentage[1]; //number of totalSoundLevel
} PACKED bt_sink_srv_audio_setting_sco_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para a2dpVolPara;
    uint8_t a2dpSoundlevelToAvrcpVolume[1]; //number of totalSoundLevel
    uint8_t musicSoundLevelToBeepTone[1]; //number of totalSoundLevel
    uint8_t a2dpSoundlevelToPercentage[1]; //number of totalSoundLevel
} PACKED bt_sink_srv_audio_setting_a2dp_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para usbAudioVolPara;
    uint8_t usbAudioSoundlevelToAvrcpVolume[1]; //number of totalSoundLevel
    uint8_t usbAudioSoundLevelToBeepTone[1]; //number of totalSoundLevel
    uint8_t usbAudioSoundlevelToPercentage[1]; //number of totalSoundLevel
} PACKED bt_sink_srv_audio_setting_usb_audio_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para usbVolPara;
    uint8_t usbSoundlevelToAvrcpVolume[1]; //number of totalSoundLevel
    uint8_t usbSoundLevelToBeepTone[1]; //number of totalSoundLevel
    uint8_t usbSoundlevelToPercentage[1]; //number of totalSoundLevel
} PACKED bt_sink_srv_audio_setting_usb_sw_vol_para_t;
typedef struct
{
    bt_sink_srv_audio_setting_vol_para mp3VolPara;
    uint8_t mp3SoundlevelToPercentage[1];
} PACKED bt_sink_srv_audio_setting_mp3_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para scoMicVolPara;
    uint8_t scoMicSoundLevelToVgm[1];
    uint8_t scoMicSoundlevelToPercentage[1];
} PACKED bt_sink_srv_audio_setting_sco_mic_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para vprtVolPara;
    uint8_t vprtSoundlevelToPercentage[1];
} PACKED bt_sink_srv_audio_setting_vprt_vol_para_t;

typedef struct
{
    bt_sink_srv_audio_setting_vol_para lineInVolPara;
    uint8_t lineInSoundLevelToBeepTone[1];
    uint8_t lineinSoundlevelToPercentage[1];
} PACKED bt_sink_srv_audio_setting_lineIn_vol_para_t;

typedef struct
{
    uint16_t lineIn_MicAnalogVol_L;
    uint16_t lineIn_MicAnalogVol_R;
} PACKED bt_sink_srv_audio_setting_lineIn_mic_vol_para_t;

typedef struct
{
    uint16_t lineIn_MicAnalogVol_L;
    uint16_t lineIn_MicAnalogVol_R;
    uint16_t Mic_AnalogVol_L;
    uint16_t Mic_AnalogVol_R;
    uint16_t Anc_MicAnalogVol_L;
    uint16_t Anc_MicAnalogVol_R;
    uint16_t VC_MicAnalogVol_L;
    uint16_t VC_MicAnalogVol_R;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    uint16_t VC_MicAnalogVol_MIC2;
    uint16_t VC_MicAnalogVol_MIC3;
    uint16_t VC_MicAnalogVol_MIC4;
    uint16_t VC_MicAnalogVol_MIC5;
#endif
} PACKED bt_sink_srv_audio_setting_vc_mic_vol_para_t;

typedef struct
{
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    uint16_t VC_MIC0_L_Digital_Vol;
    uint16_t VC_MIC0_R_Digital_Vol;
    uint16_t VC_MIC1_L_Digital_Vol;
    uint16_t VC_MIC1_R_Digital_Vol;
    uint16_t VC_MIC2_L_Digital_Vol;
    uint16_t VC_MIC2_R_Digital_Vol;
    uint16_t VC_I2S0_L_Digital_Vol;
    uint16_t VC_I2S0_R_Digital_Vol;
    uint16_t VC_I2S1_L_Digital_Vol;
    uint16_t VC_I2S1_R_Digital_Vol;
    uint16_t VC_I2S2_L_Digital_Vol;
    uint16_t VC_I2S2_R_Digital_Vol;
    uint16_t VC_LINEIN_L_Digital_Vol;
    uint16_t VC_LINEIN_R_Digital_Vol;
    uint16_t VC_Echo_Reference_Vol;
    uint16_t reserved[9];
#else
    uint16_t VC_MainMic_Digital_Vol;
    uint16_t VC_RefMic_Digital_Vol;
#endif
} PACKED bt_sink_srv_audio_setting_vc_digital_mic_vol_para_t;

typedef struct
{
    uint16_t sco_Extended_MicAnalogVol_L;
    uint16_t sco_Extended_MicAnalogVol_R;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    uint16_t sco_Extended_MicAnalogVol_MIC2;
    uint16_t sco_Extended_MicAnalogVol_MIC3;
    uint16_t sco_Extended_MicAnalogVol_MIC4;
    uint16_t sco_Extended_MicAnalogVol_MIC5;
#endif
} PACKED bt_sink_srv_audio_setting_sco_extend_analog_mic_vol_para_t;

typedef struct
{
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
    uint16_t sco_Extended_MIC0_L_Digital_Vol;
    uint16_t sco_Extended_MIC0_R_Digital_Vol;
    uint16_t sco_Extended_MIC1_L_Digital_Vol;
    uint16_t sco_Extended_MIC1_R_Digital_Vol;
    uint16_t sco_Extended_MIC2_L_Digital_Vol;
    uint16_t sco_Extended_MIC2_R_Digital_Vol;
    uint16_t sco_Extended_I2S0_L_Digital_Vol;
    uint16_t sco_Extended_I2S0_R_Digital_Vol;
    uint16_t sco_Extended_I2S1_L_Digital_Vol;
    uint16_t sco_Extended_I2S1_R_Digital_Vol;
    uint16_t sco_Extended_I2S2_L_Digital_Vol;
    uint16_t sco_Extended_I2S2_R_Digital_Vol;
    uint16_t sco_Extended_LINEIN_L_Digital_Vol;
    uint16_t sco_Extended_LINEIN_R_Digital_Vol;
    uint16_t sco_Extended_Echo_Reference_Vol;
    uint16_t reserved[9];
#else
    uint16_t sco_Extended_MainMic_Digital_Vol;
    uint16_t sco_Extended_RefMic_Digital_Vol;
    uint16_t sco_Extended_Ref2Mic_Digital_Vol;
    uint16_t sco_Extended_Reserve_Digital_Vol;
    uint16_t sco_Extended_Echo_Reference_Vol;
#endif
} PACKED bt_sink_srv_audio_setting_sco_extend_digital_mic_vol_para_t;

#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
typedef struct
{
    uint16_t WWE_MIC0_L_Digital_Vol;
    uint16_t WWE_MIC0_R_Digital_Vol;
    uint16_t WWE_MIC1_L_Digital_Vol;
    uint16_t WWE_MIC1_R_Digital_Vol;
    uint16_t WWE_MIC2_L_Digital_Vol;
    uint16_t WWE_MIC2_R_Digital_Vol;
    uint16_t WWE_I2S0_L_Digital_Vol;
    uint16_t WWE_I2S0_R_Digital_Vol;
    uint16_t WWE_I2S1_L_Digital_Vol;
    uint16_t WWE_I2S1_R_Digital_Vol;
    uint16_t WWE_I2S2_L_Digital_Vol;
    uint16_t WWE_I2S2_R_Digital_Vol;
    uint16_t WWE_LINEIN_L_Digital_Vol;
    uint16_t WWE_LINEIN_R_Digital_Vol;
    uint16_t WWE_Echo_Reference_Vol;
    uint16_t reserved[9];
} PACKED bt_sink_srv_audio_setting_wwe_digital_mic_vol_para_t;

typedef struct
{
    uint16_t MIC_FUNC_MIC0_L_Digital_Vol;
    uint16_t MIC_FUNC_MIC0_R_Digital_Vol;
    uint16_t MIC_FUNC_MIC1_L_Digital_Vol;
    uint16_t MIC_FUNC_MIC1_R_Digital_Vol;
    uint16_t MIC_FUNC_MIC2_L_Digital_Vol;
    uint16_t MIC_FUNC_MIC2_R_Digital_Vol;
    uint16_t MIC_FUNC_Echo_Reference_Vol;
    uint16_t reserved[9];
} PACKED bt_sink_srv_audio_setting_mic_func_vol_para_t;

typedef struct {
    uint16_t Detach_MIC_Digital_Vol;
    uint16_t reserved[3];
} PACKED bt_sink_srv_audio_setting_detach_mic_digital_mic_vol_para_t;

typedef struct {
    uint16_t Detach_MIC_Analog_Vol_ACC10K;
    uint16_t Detach_MIC_Analog_Vol_ACC20K;
    uint16_t Detach_MIC_Analog_Vol_DCC;
    uint16_t reserved[5];
} PACKED bt_sink_srv_audio_setting_detach_mic_analog_mic_vol_para_t;

typedef struct {
    uint32_t digital_MIC0_L;
    uint32_t digital_MIC0_R;
    uint32_t digital_MIC1_L;
    uint32_t digital_MIC1_R;
    uint32_t digital_MIC2_L;
    uint32_t digital_MIC2_R;
    uint32_t digital_I2S0_L;
    uint32_t digital_I2S0_R;
    uint32_t digital_I2S1_L;
    uint32_t digital_I2S1_R;
    uint32_t digital_I2S2_L;
    uint32_t digital_I2S2_R;
    uint32_t digital_LINEIN_L;
    uint32_t digital_LINEIN_R;
    uint32_t digital_Echo;
    uint32_t digital_MIC0_L_func;
    uint32_t digital_MIC0_R_func;
    uint32_t digital_MIC1_L_func;
    uint32_t digital_MIC1_R_func;
    uint32_t digital_MIC2_L_func;
    uint32_t digital_MIC2_R_func;
    uint32_t digital_Echo_func;
    uint32_t analog_R;
    uint32_t analog_MIC2;
    uint32_t analog_MIC3;
    uint32_t analog_MIC4;
    uint32_t analog_MIC5;
} PACKED bt_sink_audio_setting_multi_vol_config_t;

#endif

#endif


void bt_sink_srv_audio_setting_init(void);

int32_t bt_sink_srv_audio_setting_get_vol(bt_sink_srv_audio_setting_vol_info_t *vol_info,
            bt_sink_srv_audio_setting_vol_t *vol);

void bt_sink_srv_audio_setting_update_voice_fillter_setting(bt_sink_srv_audio_setting_vol_info_t *vol_info,
        const audio_eaps_t *am_speech_eaps);

#ifdef __GAIN_TABLE_NVDM_DIRECT__
uint8_t audio_get_max_sound_level_out(vol_type_t volType);
uint8_t audio_get_max_sound_level_in(vol_type_t volType);
#endif

uint32_t audio_get_gain_in_in_dB(uint8_t level, gain_type_t gainType, vol_type_t volType);
uint32_t audio_get_gain_out_in_dB(uint8_t level, gain_type_t gainType, vol_type_t volType);
bool audio_get_analog_gain_out_offset_in_db(uint16_t *L_offset_gain,uint16_t *R_offset_gain);

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
void bt_sink_srv_audio_setting_detach_mic_gain_config(uint32_t *analog_gain, uint32_t *digital_gain, bt_sink_audio_setting_multi_vol_config_t *mult_vol_cfg);
#endif

#endif /* __BT_SINK_SRV_AUDIO_SETTING_H__ */

