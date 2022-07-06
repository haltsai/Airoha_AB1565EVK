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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_ami.h"
#include "hal_audio_internal.h"
#include "bt_sink_srv_audio_setting.h"
#include "nvdm.h"
#include "FreeRTOS.h"
#ifdef __AM_DEBUG_INFO__
#include "bt_sink_srv_utils.h"
#endif

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#endif

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_sink_srv.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_xport.h"
#endif
#include "bt_sink_srv_aws_mce.h"
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_report_internal.h"
#endif

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
  #include "anc_control_api.h"
#else
  #include "anc_control.h"
#endif
#endif
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_dsprealtime.h"
#endif
#include "audio_log.h"
#include "nvkey_dspfw.h"
#include "nvkey.h"
#include "hal_nvic.h"
#include "hal_audio_cm4_dsp_message.h"
#include "audio_nvdm_common.h"
//#define ANALOG_VOL_MAX     10 /*depend on HAL*/
//#define DIGITAL_VOL_MAX    10 /*depend on HAL*/

#define _UNUSED(x)  ((void)(x))
#define AUDIO_SRC_SRV_AMI_SET_FLAG(MASK, FLAG) ((MASK) |= (FLAG))
#define AUDIO_SRC_SRV_AMI_RESET_FLAG(MASK, FLAG) ((MASK) &= ~(FLAG))

bt_sink_srv_am_background_t *g_prCurrent_player = NULL;
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
am_audio_app_callback_t  g_am_audio_app_callback = NULL;
static I2S_param_for_external_DSP_t g_param_for_external_dsp;
#endif
bt_sink_srv_am_id_t g_aud_id_num = 0;
bt_bd_addr_t g_int_dev_addr = {0};
#ifdef MTK_AVM_DIRECT
SemaphoreHandle_t g_ami_hal_semaphore_handle = NULL;
#endif

//==== Static variables ====
static bt_sink_srv_am_a2dp_sink_latency_t g_a2dp_sink_latency = 140000;
static bt_sink_srv_am_bt_audio_param_t    g_bt_inf_address    = 0x0;
static hal_audio_device_t                 g_afe_device        = HAL_AUDIO_DEVICE_DAC_DUAL;
#if defined(MTK_AVM_DIRECT)
extern HAL_AUDIO_CHANNEL_SELECT_t audio_Channel_Select;
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
extern HAL_AUDIO_DVFS_CLK_SELECT_t audio_nvdm_dvfs_config;
extern bt_sink_srv_am_amm_struct *ptr_callback_amm;
extern bt_sink_srv_am_amm_struct *ptr_isr_callback_amm;
extern void *pxCurrentTCB;
void*    AUDIO_SRC_SRV_AM_TASK = NULL;
#endif

#ifdef SUPPORT_PEQ_NVKEY_UPDATE
static ami_attach_nvdm_ctrl_t ami_attach_nvdm_ctrl = {
    .buffer = NULL,
    .buffer_offset = 0,
    .total_pkt= 0,
    .pre_pkt = 0,
};
#endif

#ifndef WIN32_UT
xSemaphoreHandle g_xSemaphore_ami = NULL;
void ami_mutex_lock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreTake(handle, portMAX_DELAY);
    }
}
void ami_mutex_unlock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreGive(handle);
    }
}
#else
bt_sink_srv_am_amm_struct *g_prAmm_current = NULL;
#define ami_mutex_lock()   { }
#define ami_mutex_unlock() { }
#endif

extern bool g_audio_nvdm_init_flg;

/*****************************************************************************
 * FUNCTION
 *  ami_register_get_id
 * DESCRIPTION
 *  Get the redistered ID
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
static bt_sink_srv_am_id_t ami_register_get_id(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_id_t bAud_id = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (g_rAm_aud_id[bAud_id].use != ID_CLOSE_STATE) {
        bAud_id++;
    }
    g_rAm_aud_id[bAud_id].use = ID_IDLE_STATE;
    //g_rAm_aud_id[bAud_id].contain_ptr = NULL;
    g_aud_id_num++;
    return bAud_id;
}

/*****************************************************************************
 * FUNCTION
 *  ami_register_check_id_exist
 * DESCRIPTION
 *  Check if the specified ID is valid
 * PARAMETERS
 *  aud_id          [IN]
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
 #if 0
static bt_sink_srv_am_id_t ami_register_check_id_exist(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use != ID_CLOSE_STATE)) {
        return TRUE;
    }
    return FALSE;
}
#endif


/*****************************************************************************
 * FUNCTION
 *  ami_register_delete_id
 * DESCRIPTION
 *  Delete redistered ID
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void ami_register_delete_id(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[aud_id].use != ID_CLOSE_STATE) {
        g_rAm_aud_id[aud_id].use = ID_CLOSE_STATE;
        g_aud_id_num--;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_current_scenario
 * DESCRIPTION
 *  Get current audio scenario
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_type_t
 *****************************************************************************/
bt_sink_srv_am_type_t bt_sink_srv_ami_get_current_scenario()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(g_prCurrent_player) {
        return g_prCurrent_player->type;
    } else {
        return NONE;
    }
}

#ifdef __GAIN_TABLE_NVDM_DIRECT__
extern uint8_t AUD_A2DP_VOL_OUT_MAX;
extern uint8_t AUD_A2DP_VOL_OUT_DEFAULT;
extern uint8_t AUD_VPRT_VOL_OUT_MAX;
extern uint8_t AUD_VPRT_VOL_OUT_DEFAULT;
extern uint8_t AUD_LINEIN_VOL_OUT_MAX;
extern uint8_t AUD_LINEIN_VOL_OUT_DEFAULT;

#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
extern uint8_t AUD_USB_AUDIO_SW_VOL_OUT_MAX;
extern uint8_t AUD_USB_AUDIO_SW_VOL_OUT_DEFAULT;
extern uint8_t AUD_USB_VOICE_SW_VOL_OUT_MAX;
extern uint8_t AUD_USB_VOICE_SW_VOL_OUT_DEFAULT;
#endif

#endif
/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_a2dp_max_volume_level
 * DESCRIPTION
 *  Get A2DP max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_a2dp_max_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if((AUD_A2DP_VOL_OUT_MAX == 0) || (AUD_A2DP_VOL_OUT_MAX > 100)){ //Gain table max level was 100
        audio_src_srv_report("[AudM]ami_get_a2dp_max_volume_level error. Max level:%d\n",1, AUD_A2DP_VOL_OUT_MAX);
        audio_src_srv_report("[AudM]A2DP max volume level error.", 0);
        configASSERT(0);
    }
    return AUD_A2DP_VOL_OUT_MAX;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_a2dp_default_volume_level
 * DESCRIPTION
 *  Get A2DP max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_a2dp_default_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if(AUD_A2DP_VOL_OUT_DEFAULT > AUD_A2DP_VOL_OUT_MAX){ //Gain table max level was AUD_A2DP_VOL_OUT_MAX
        audio_src_srv_report("[AudM]ami_get_a2dp_default_volume_level error. Default level:%d\n",1, AUD_A2DP_VOL_OUT_DEFAULT);
        audio_src_srv_report("[AudM]A2DP default volume level error.", 0);
        configASSERT(0);
    }
    return AUD_A2DP_VOL_OUT_DEFAULT;
#else
    return AUD_VOL_OUT_LEVEL6;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_vp_max_volume_level
 * DESCRIPTION
 *  Get VP max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_vp_max_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if((AUD_VPRT_VOL_OUT_MAX == 0) || (AUD_VPRT_VOL_OUT_MAX > 15)){ //Gain table max level was 15
        audio_src_srv_report("[AudM]ami_get_vp_max_volume_level error. Max level:%d\n",1, AUD_VPRT_VOL_OUT_MAX);
        //audio_src_srv_report("[AudM]VP max volume level error.", 0);
        //configASSERT(0);
    }
    return AUD_VPRT_VOL_OUT_MAX;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_vp_default_volume_level
 * DESCRIPTION
 *  Get VP max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_vp_default_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if(AUD_VPRT_VOL_OUT_DEFAULT > AUD_VPRT_VOL_OUT_MAX){ //Gain table max level was AUD_VPRT_VOL_OUT_MAX
        audio_src_srv_report("[AudM]ami_get_vp_default_volume_level error. Default level:%d\n",1, AUD_VPRT_VOL_OUT_DEFAULT);
        audio_src_srv_report("[AudM]VP default volume level error.", 0);
        configASSERT(0);
    }
    return AUD_VPRT_VOL_OUT_DEFAULT;
#else
    return AUD_VOL_OUT_LEVEL11;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_lineIN_max_volume_level
 * DESCRIPTION
 *  Get LineIN max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_lineIN_max_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if((AUD_LINEIN_VOL_OUT_MAX == 0) || (AUD_LINEIN_VOL_OUT_MAX > 100)){ //Gain table max level was 100
        audio_src_srv_report("[AudM]ami_get_lineIN_max_volume_level error. Max level:%d\n",1, AUD_LINEIN_VOL_OUT_MAX);
        audio_src_srv_report("[AudM]LineIN max volume level error.", 0);
        configASSERT(0);
    }
    return AUD_LINEIN_VOL_OUT_MAX;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_lineIN_default_volume_level
 * DESCRIPTION
 *  Get LineIN max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_lineIN_default_volume_level()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if(AUD_LINEIN_VOL_OUT_DEFAULT > AUD_LINEIN_VOL_OUT_MAX){ //Gain table max level was AUD_LINEIN_VOL_OUT_MAX
        audio_src_srv_report("[AudM]ami_get_lineIN_default_volume_level error. Default level:%d\n",1, AUD_LINEIN_VOL_OUT_DEFAULT);
        audio_src_srv_report("[AudM]LineIN default volume level error.", 0);
        configASSERT(0);
    }
    return AUD_LINEIN_VOL_OUT_DEFAULT;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}

#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
/*****************************************************************************
 * FUNCTION

 *  bt_sink_srv_ami_get_usb_voice_sw_default_volume_level
 * DESCRIPTION
 *  Get usb_voice_sw default volume level
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_voice_sw_default_volume_level()
{
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if(AUD_USB_VOICE_SW_VOL_OUT_DEFAULT > AUD_USB_VOICE_SW_VOL_OUT_MAX){ //Gain table max level was AUD_VPRT_VOL_OUT_MAX
        audio_src_srv_report("[AudM]ami_get_usb_voice_sw_default_volume_level error. Default level:%d\n",1, AUD_USB_VOICE_SW_VOL_OUT_DEFAULT);
        configASSERT(0);
    }
    return AUD_USB_VOICE_SW_VOL_OUT_DEFAULT;
#else
    return AUD_VOL_OUT_LEVEL11;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_usb_voice_sw_max_volume_level
 * DESCRIPTION
 *  Get usb_voice_sw max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()
{
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if((AUD_USB_VOICE_SW_VOL_OUT_MAX == 0) || (AUD_USB_VOICE_SW_VOL_OUT_MAX > 100)){ //Gain table max level was 100
        audio_src_srv_report("[AudM]ami_get_usb_voice_sw_max_volume_level error. Max level:%d\n",1, AUD_USB_VOICE_SW_VOL_OUT_MAX);
        configASSERT(0);
    }
    return AUD_USB_VOICE_SW_VOL_OUT_MAX;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}
/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_usb_music_sw_default_volume_level
 * DESCRIPTION
 *  Get usb_music_sw default volume level
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_music_sw_default_volume_level()
{
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if(AUD_USB_AUDIO_SW_VOL_OUT_DEFAULT > AUD_USB_AUDIO_SW_VOL_OUT_MAX){ //Gain table max level was AUD_VPRT_VOL_OUT_MAX
        audio_src_srv_report("[AudM]ami_get_usb_audio_sw_default_volume_level error. Default level:%d\n",1, AUD_USB_AUDIO_SW_VOL_OUT_DEFAULT);
        configASSERT(0);
    }
    return AUD_USB_AUDIO_SW_VOL_OUT_DEFAULT;
#else
    return AUD_VOL_OUT_LEVEL11;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_usb_music_sw_max_volume_level
 * DESCRIPTION
 *  Get usb_music_sw max volume level which can be changed by config tool
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_volume_level_t
 *****************************************************************************/
bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_music_sw_max_volume_level()
{
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    if((AUD_USB_AUDIO_SW_VOL_OUT_MAX == 0) || (AUD_USB_AUDIO_SW_VOL_OUT_MAX > 100)){ //Gain table max level was 100
        audio_src_srv_report("[AudM]ami_get_usb_audio_sw_max_volume_level error. Max level:%d\n",1, AUD_USB_AUDIO_SW_VOL_OUT_MAX);
        configASSERT(0);
    }
    return AUD_USB_AUDIO_SW_VOL_OUT_MAX;
#else
    return AUD_VOL_OUT_LEVEL15;
#endif
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_open
 * DESCRIPTION
 *  Use this function to open a new audio handler and get the audio ID.
 * PARAMETERS
 *  priority         [IN]
 *  handler          [IN]
 * RETURNS
 *  bt_sink_srv_am_id_t
 *****************************************************************************/
bt_sink_srv_am_id_t bt_sink_srv_ami_audio_open(bt_sink_srv_am_priority_t priority,
        bt_sink_srv_am_notify_callback handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_id_t bAud_id;
    int32_t pri = 0;
    //bt_sink_srv_am_hal_result_t result = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[AudM]open-pri: %d, num: %d, cb: 0x%x",3, priority, g_aud_id_num, (unsigned int)handler);
    //printf("[AudM]open-pri: %d, num: %d, cb: 0x%x\n", priority, g_aud_id_num, (unsigned int)handler);

    pri = (int32_t)priority;
    if ((g_aud_id_num < AM_REGISTER_ID_TOTAL) && (pri >= AUD_LOW) && (pri <= AUD_HIGH)) {
        ami_mutex_lock(g_xSemaphore_ami);
        bAud_id = ami_register_get_id();
        ami_mutex_unlock(g_xSemaphore_ami);
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Open func, ID: %d, pri: %d, num: %d",3,
            bAud_id, priority, g_aud_id_num);
#endif
        temp_background_t.aud_id = bAud_id;
        temp_background_t.notify_cb = handler;
        temp_background_t.priority = priority;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_OPEN_REQ, &temp_background_t,
                                 FALSE, NULL);
        return (bt_sink_srv_am_id_t)bAud_id;
    }
    return AUD_ID_INVALID;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_play
 * DESCRIPTION
 *  Start to play the specified audio handler.
 * PARAMETERS
 *  aud_id           [IN]
 *  capability_t     [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_play(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_audio_capability_t *capability_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        //(unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    audio_src_srv_report("[AudM]play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d",5,
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        (unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            ((g_rAm_aud_id[aud_id].use == ID_IDLE_STATE) || (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE) || (g_rAm_aud_id[aud_id].use == ID_STOPING_STATE))) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Play func, ID: %d",1, aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(true);
#endif

        if (capability_t->type == HFP) {
            audio_nvdm_update_status(AUDIO_NVDM_USER_HFP, AUDIO_NVDM_STATUS_PREV_CHANGE);
        }

        temp_background_t.aud_id = aud_id;
        temp_background_t.type = capability_t->type;
        temp_background_t.audio_path_type = capability_t->audio_path_type;
        memcpy(&(temp_background_t.local_context), &(capability_t->codec), sizeof(bt_sink_srv_am_codec_t));
        memcpy(&(temp_background_t.audio_stream_in), &(capability_t->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
        memcpy(&(temp_background_t.audio_stream_out), &(capability_t->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));
        memcpy(&g_int_dev_addr, &(capability_t->dev_addr), sizeof(bt_bd_addr_t));
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_PLAY_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_stop
 * DESCRIPTION
 *  Stop playing the specified audio handler.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_stop(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    audio_src_srv_report("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x",4,
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Stop func, ID: %d",1, aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(false);
#endif

        temp_background_t.aud_id = aud_id;
        g_rAm_aud_id[aud_id].use = ID_STOPING_STATE;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_STOP_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_close
 * DESCRIPTION
 *  Close the audio handler opened.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_close(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //audio_src_srv_report("[Sink][AMI] Pre-close func, ID: %d, use:%d", 2, aud_id, g_rAm_aud_id[aud_id].use);
    //audio_src_srv_report("[AudM]close-id: %d, num: %d, use: %d, ptr: 0x%x\n", 4,
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    audio_src_srv_report("[AudM]close-id: %d, num: %d, use: %d, ptr: 0x%x",4,
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            ((g_rAm_aud_id[aud_id].use == ID_IDLE_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_SUSPEND_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE)  ||
             (g_rAm_aud_id[aud_id].use == ID_STOPING_STATE))) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Close func, ID: %d",1, aud_id);
#endif
        temp_background_t.aud_id = aud_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_CLOSE_REQ, &temp_background_t,
                                 FALSE, NULL);
        ami_mutex_lock(g_xSemaphore_ami);
        ami_register_delete_id(aud_id);
        ami_mutex_unlock(g_xSemaphore_ami);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_volume
 * DESCRIPTION
 *  Set audio input/output volume.
 * PARAMETERS
 *  aud_id           [IN]
 *  volume_level     [IN]
 *  in_out           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_volume(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_volume_level_t volume_level,
        bt_sink_srv_am_stream_type_t in_out)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[Sink][AMI] set volume: aud_id:%d, volume_level:%d, in_out:%d",3, aud_id, volume_level, in_out);
    if ((aud_id < AM_REGISTER_ID_TOTAL)
        //&& (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)
        ) {

        temp_background_t.aud_id = aud_id;
        temp_background_t.in_out = in_out;
        if (in_out == STREAM_OUT) {
#ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            if (volume_level < AUD_VOL_OUT_MAX) {
#endif
                temp_background_t.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)volume_level;
#ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            } else {
#ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AMI] Vol-level error", 0);
#endif
                return AUD_EXECUTION_FAIL;
            }
#endif
        } else if (in_out == STREAM_IN) {
#ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            if (volume_level < AUD_VOL_IN_MAX) {
#endif
                temp_background_t.audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)volume_level;
#ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            } else {
#ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AMI] Vol-level error", 0);
#endif
                return AUD_EXECUTION_FAIL;
            }
#endif
        }
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_SET_VOLUME_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_mute
 * DESCRIPTION
 *  Mute audio input/output device.
 * PARAMETERS
 *  aud_id           [IN]
 *  mute             [IN]
 * in_out            [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mute(bt_sink_srv_am_id_t aud_id,
        bool mute,
        bt_sink_srv_am_stream_type_t in_out)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == FEATURE_NO_NEED_ID) {
        temp_background_t.aud_id = aud_id;
    } else if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
        temp_background_t.aud_id = aud_id;
    } else {
        temp_background_t.aud_id = FEATURE_NO_NEED_ID;
    }
    temp_background_t.in_out = in_out;
    switch (in_out) {
        case STREAM_IN:
            temp_background_t.audio_stream_in.audio_mute = mute;
            break;
        case STREAM_OUT:
        case STREAM_OUT_2:
        case STREAM_OUT_3:
            temp_background_t.audio_stream_out.audio_mute = mute;
            break;
        default:
            audio_src_srv_report("[Sink][AMI]Set mute error, in_out = %d", 1, in_out);
            return AUD_EXECUTION_FAIL;
            break;
    }
    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                MSG_ID_STREAM_MUTE_DEVICE_REQ, &temp_background_t,
                                FALSE, NULL);
    audio_src_srv_report("[Sink][AMI]Set mute mute [%d], in_out [%d]", 2, mute, in_out);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_device
 * DESCRIPTION
 *  Set audio input/output device.
 * PARAMETERS
 *  aud_id           [IN]
 *  device           [IN]
 *  in_out           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_device(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_device_set_t device)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_id_t bCount = 0;
    bt_sink_srv_am_device_set_t temp_device;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    temp_device = device;
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
        while (temp_device > 0) {
            if ((temp_device & 1) == 1) {
                bCount++;
            }
            temp_device >>= 1;
        }
        if (bCount > 1) {
            return AUD_EXECUTION_FAIL;
        }
        temp_background_t.aud_id = aud_id;
        if (device & DEVICE_OUT_LIST) {
            temp_background_t.in_out = STREAM_OUT;
            temp_background_t.audio_stream_out.audio_device = device;
        } else if (device & DEVICE_IN_LIST) {
            temp_background_t.in_out = STREAM_IN;
            temp_background_t.audio_stream_in.audio_device = device;
        }
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_CONFIG_DEVICE_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_continue_stream
 * DESCRIPTION
 *  Continuously write data to audio output for palyback / read data from audio input for record.
 * PARAMETERS
 *  aud_id           [IN]
 *  buffer           [IN/OUT]
 *  data_count       [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_continue_stream(bt_sink_srv_am_id_t aud_id,
        void *buffer,
        uint32_t data_count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            (g_rAm_aud_id[aud_id].contain_ptr->type == PCM) &&
            ((g_rAm_aud_id[aud_id].use == ID_PLAY_STATE) ||
             (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE))) {
        temp_background_t.aud_id = aud_id;
        temp_background_t.local_context.pcm_format.stream.size = data_count;
        temp_background_t.local_context.pcm_format.stream.buffer = buffer;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_READ_WRITE_DATA_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_get_stream_length
 * DESCRIPTION
 *  Query available input/output data length.
 * PARAMETERS
 *  aud_id           [IN]
 *  data_length      [OUT]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_get_stream_length(bt_sink_srv_am_id_t aud_id,
        uint32_t *data_length,
        bt_sink_srv_am_stream_type_t in_out)
{
    #if 0
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ami_register_check_id_exist(aud_id)) {
        temp_background_t.aud_id = aud_id;
        temp_background_t.in_out = in_out;
        temp_background_t.data_length_ptr = data_length;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_GET_LENGTH_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
    #endif
    bt_sink_srv_assert(data_length);
    if (STREAM_OUT == in_out) {
        hal_audio_get_stream_out_sample_count(data_length);
    } else if (STREAM_IN == in_out) {
        hal_audio_get_stream_in_sample_count(data_length);
    }

    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_set_a2dp_sink_latency
 * DESCRIPTION
 *  SET current a2dp sink latency
 * PARAMETERS
 *  local_a2dp_sink_latency    [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_set_a2dp_sink_latency(bt_sink_srv_am_a2dp_sink_latency_t a2dp_sink_latency)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(a2dp_sink_latency) {
        g_a2dp_sink_latency = a2dp_sink_latency;
        return AUD_EXECUTION_SUCCESS;
    } else {
        return AUD_EXECUTION_FAIL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_a2dp_sink_latency
 * DESCRIPTION
 *  GET current a2dp sink latency
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_a2dp_sink_latency_t
 *****************************************************************************/
bt_sink_srv_am_a2dp_sink_latency_t bt_sink_srv_ami_get_a2dp_sink_latency(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
        return g_a2dp_sink_latency;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_set_bt_inf_address
 * DESCRIPTION
 *  SET current bt information address
 * PARAMETERS
 *  local_bt_inf_address    [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_set_bt_inf_address(bt_sink_srv_am_bt_audio_param_t bt_inf_address)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(bt_inf_address) {
        g_bt_inf_address = bt_inf_address;
        return AUD_EXECUTION_SUCCESS;
    } else {
        return AUD_EXECUTION_FAIL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_bt_inf_address
 * DESCRIPTION
 *  GET current bt information address
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_bt_audio_param_t
 *****************************************************************************/
bt_sink_srv_am_bt_audio_param_t bt_sink_srv_ami_get_bt_inf_address(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
        return g_bt_inf_address;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_set_afe_device
 * DESCRIPTION
 *  SET current afe device
 * PARAMETERS
 *  afe_device    [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_set_afe_device(hal_audio_device_t afe_device)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(afe_device) {
        g_afe_device = afe_device;
        return AUD_EXECUTION_SUCCESS;
    } else {
        return AUD_EXECUTION_FAIL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_get_afe_device
 * DESCRIPTION
 *  GET current afe device
 * PARAMETERS
 *  void
 * RETURNS
 *  hal_audio_device_t
 *****************************************************************************/
hal_audio_device_t bt_sink_srv_ami_get_afe_device(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
        return g_afe_device;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_pause
 * DESCRIPTION
 *  Pause current audio
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_set_pause(void)
{
    bt_sink_srv_am_background_t temp_background_t = {0};

    if(g_prCurrent_player) {
        temp_background_t.aud_id = g_prCurrent_player->aud_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_SET_PAUSE, &temp_background_t,
                                 FALSE, NULL);
    }

    audio_src_srv_report("[AMI] am_audio_set_pause", 0);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_resume
 * DESCRIPTION
 *  Resume audio
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_set_resume(void)
{
    bt_sink_srv_am_background_t temp_background_t = {0};

    if(g_prCurrent_player) {
        temp_background_t.aud_id = g_prCurrent_player->aud_id;
        temp_background_t.type = g_prCurrent_player->type;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_SET_RESUME, &temp_background_t,
                                 FALSE, NULL);
    }
    else
    {
        temp_background_t.type = AUDIO_TRANSMITTER;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_SET_RESUME, &temp_background_t,
                                 FALSE, NULL);
    }

    audio_src_srv_report("[AMI] am_audio_set_resume", 0);
    return AUD_EXECUTION_SUCCESS;
}

/**
 * @brief The API is use to get the test mode config
 *
 * @param scenario
 * @return test_mode_config
 */
bool am_audio_get_test_mode_config(bt_sink_srv_am_test_mode_scenario_t scenario){
    bool result = FALSE;
    if (((audio_nvdm_HW_config.test_mode_config[scenario/8] >> (scenario%8)) & 0x1)== 1){
        result = TRUE;
    }
    return result;
}


#ifdef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
static uint8_t g_ami_sidetone_use_PT_enable_state = 0;
static int32_t g_ami_sidetone_use_PT_gain_HFP = 0;
static int32_t g_ami_sidetone_use_PT_gain_Reserve = 0;
#endif
extern void audio_side_tone_enable_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
/*****************************************************************************
 * FUNCTION
 *  am_audio_side_tone_enable
 * DESCRIPTION
 *  Enable side tone
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_side_tone_enable(void)
{
    bool is_am_task = false;
    uint32_t savedmask;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(AUDIO_SRC_SRV_AM_TASK == pxCurrentTCB){
        is_am_task = true;
    } else {
        is_am_task = false;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    if(is_am_task){
  #ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
        audio_side_tone_enable_hdlr(NULL);
  #else
        g_ami_sidetone_use_PT_enable_state = 1;
    #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
        audio_anc_control_result_t anc_ret;
        audio_anc_control_cap_t local_cap;
        memset(&local_cap, 0, sizeof(audio_anc_control_cap_t));
        local_cap.filter_cap.filter_id      = AUDIO_ANC_CONTROL_PASS_THRU_SIDETONE_FILTER_DEFAULT;
        local_cap.filter_cap.type           = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
        local_cap.filter_cap.filter_mask    = 0x3;
        local_cap.filter_cap.sram_bank_mask = 0x0;
        anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, AUDIO_ANC_CONTROL_EVENT_ON, &local_cap);
    #else
        #if 0//defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        ptr_race_pkt_t pLocalHeaderCmd = pvPortMalloc(sizeof(race_pkt_t) + 5);
        if(pLocalHeaderCmd != NULL){
            //pLocalHeaderCmd->hdr.pktId  = 0x05;
            pLocalHeaderCmd->hdr.type   = 0x5A;
            pLocalHeaderCmd->hdr.length = 7; //5+2
            pLocalHeaderCmd->hdr.id     = 0xE06;
            pLocalHeaderCmd->payload[0] = 0x0;
            pLocalHeaderCmd->payload[1] = 10;
            pLocalHeaderCmd->payload[2] = 11; //flash_no
            pLocalHeaderCmd->payload[3] = 4; //ancType
            pLocalHeaderCmd->payload[4] = 1;
            void* ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pLocalHeaderCmd, 0);
            vPortFree(pLocalHeaderCmd);
        }
        #else
        audio_src_srv_report("[AMI] am_audio_side_tone_enable fail, not support local device", 0);
        #endif
    #endif
  #endif
    } else {
  #ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_SIDE_TONE_ENABLE, NULL,
                             FALSE, NULL);
  #else
        g_ami_sidetone_use_PT_enable_state = 1;
    #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
        audio_anc_control_result_t         control_ret;
        audio_anc_control_filter_id_t      target_filter_id;
        audio_anc_control_type_t           target_anc_type;
        target_anc_type  = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
        target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_SIDETONE_FILTER_DEFAULT;
        control_ret = audio_anc_control_disable(NULL);
        control_ret = audio_anc_control_enable(target_filter_id, target_anc_type, NULL);
    #else
        #if 0//defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        ptr_race_pkt_t pLocalHeaderCmd = pvPortMalloc(sizeof(race_pkt_t) + 5);
        if(pLocalHeaderCmd != NULL){
            //pLocalHeaderCmd->hdr.pktId  = 0x05;
            pLocalHeaderCmd->hdr.type   = 0x5A;
            pLocalHeaderCmd->hdr.length = 7; //5+2
            pLocalHeaderCmd->hdr.id     = 0xE06;
            pLocalHeaderCmd->payload[0] = 0x0;
            pLocalHeaderCmd->payload[1] = 10;
            pLocalHeaderCmd->payload[2] = 11; //flash_no
            pLocalHeaderCmd->payload[3] = 4; //ancType
            pLocalHeaderCmd->payload[4] = 1;
            void* ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pLocalHeaderCmd, 0);
            vPortFree(pLocalHeaderCmd);
        }
        #else
        audio_src_srv_report("[AMI] am_audio_side_tone_enable fail, not support local device", 0);
        #endif
    #endif
  #endif
    }

    audio_src_srv_report("[AMI] am_audio_side_tone_enable", 0);
    return AUD_EXECUTION_SUCCESS;
}

/**
 * @brief The API is use to get the side tone gain value with db
 *
 * @param scenario
 * @return side tone gain with db
 */
int32_t g_side_tone_gain_common = SIDETONE_GAIN_MAGIC_NUM;
int32_t g_side_tone_gain_hfp = SIDETONE_GAIN_MAGIC_NUM;

int32_t am_audio_side_tone_get_volume_by_scenario(sidetone_scenario_t scenario) {
    int32_t local_sidetone_volume = 0;
#ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE

    if(scenario == SIDETONE_SCENARIO_HFP) {
        if(g_side_tone_gain_hfp == SIDETONE_GAIN_MAGIC_NUM) {
            if (audio_nvdm_HW_config.sidetone_config.SideTone_Gain >> 7) {
                g_side_tone_gain_hfp   = 0xFFFFFF00 | audio_nvdm_HW_config.sidetone_config.SideTone_Gain;
            } else {
                g_side_tone_gain_hfp   = audio_nvdm_HW_config.sidetone_config.SideTone_Gain;
            }
        }
        local_sidetone_volume = g_side_tone_gain_hfp;
    } else {
        if(g_side_tone_gain_common == SIDETONE_GAIN_MAGIC_NUM) {
            if(audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0] >> 7){
                g_side_tone_gain_common   = 0xFFFFFF00 | audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0];
            } else {
                g_side_tone_gain_common   = audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0];
            }
        }
        local_sidetone_volume = g_side_tone_gain_common;
    }
#else
  #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
    if(scenario == SIDETONE_SCENARIO_HFP){
        local_sidetone_volume = g_ami_sidetone_use_PT_gain_HFP;
    }else{
        local_sidetone_volume = g_ami_sidetone_use_PT_gain_Reserve;
    }
  #else
    audio_src_srv_report("[AMI] am_audio_side_tone_get_volume_by_scenario fail, not support local device", 0);
  #endif
#endif
    return local_sidetone_volume;
}

extern void audio_side_tone_set_volume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
/**
 * @brief The API is use to change the side tone gain value with db
 *
 * @param side_tone_gain
 * @param scenario
 * @return bt_sink_srv_am_result_t
 */

bt_sink_srv_am_result_t am_audio_side_tone_set_volume_by_scenario(sidetone_scenario_t scenario, int32_t side_tone_gain)
{
    bool is_am_task = false;
    uint32_t savedmask;
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_amm_struct am_amm;

    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(AUDIO_SRC_SRV_AM_TASK == pxCurrentTCB){
        is_am_task = true;
    } else {
        is_am_task = false;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
    if(is_am_task){
  #ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
        am_amm.background_info.local_feature.feature_param.sidetone_param.scenario = scenario;
        am_amm.background_info.local_feature.feature_param.sidetone_param.side_tone_gain = side_tone_gain;
        audio_side_tone_set_volume_hdlr(&am_amm);
  #else
        if(g_ami_sidetone_use_PT_enable_state){
    #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
        audio_anc_control_result_t anc_ret;
        audio_anc_control_cap_t local_cap;
        memset(&local_cap, 0, sizeof(audio_anc_control_cap_t));
        local_cap.target_device              = 0x1;
        local_cap.filter_cap.set_reg_mask    = AUDIO_ANC_CONTROL_SET_REG_RAMP_GAIN;
        local_cap.filter_cap.type            = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
        local_cap.filter_cap.filter_mask     = 0x3;
        local_cap.filter_cap.param.ramp_gain = (audio_anc_control_gain_t)side_tone_gain;
        anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, AUDIO_ANC_CONTROL_EVENT_SET_REG, &local_cap);
        if(scenario == SIDETONE_SCENARIO_HFP){
            g_ami_sidetone_use_PT_gain_HFP     = side_tone_gain;
        }else{
            g_ami_sidetone_use_PT_gain_Reserve = side_tone_gain;
        }
    #else
        #if 0//defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        ptr_race_pkt_t pLocalHeaderCmd = pvPortMalloc(sizeof(race_pkt_t) + 5);
        if(pLocalHeaderCmd != NULL){
            //pLocalHeaderCmd->hdr.pktId  = 0x05;
            pLocalHeaderCmd->hdr.type   = 0x5A;
            pLocalHeaderCmd->hdr.length = 6; //4+2
            pLocalHeaderCmd->hdr.id     = 0xE06;
            pLocalHeaderCmd->payload[0] = 0x0;
            pLocalHeaderCmd->payload[1] = 15;
            pLocalHeaderCmd->payload[2] = (int16_t) side_tone_gain;
            pLocalHeaderCmd->payload[3] = 1;
            void* ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pLocalHeaderCmd, 0);
            vPortFree(pLocalHeaderCmd);
        }
        #else
        audio_src_srv_report("[AMI] am_audio_side_tone_set_volume_by_scenario fail, not support local device", 0);
        #endif
    #endif
        }else{
            audio_src_srv_report("[AMI] am_audio_side_tone_set_volume fail, not enable sidetone", 0);
        }
  #endif
    } else {
  #ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
        temp_background_t.local_feature.feature_param.sidetone_param.scenario = scenario;
        temp_background_t.local_feature.feature_param.sidetone_param.side_tone_gain = side_tone_gain;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_SET_SIDE_TONE_VOLUME, &temp_background_t,
                                 FALSE, ptr_callback_amm);
  #else
        if(g_ami_sidetone_use_PT_enable_state){
    #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
        audio_anc_control_result_t         control_ret;
        control_ret = audio_anc_control_set_runtime_gain((audio_anc_control_gain_t) side_tone_gain, AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF);
        if(scenario == SIDETONE_SCENARIO_HFP){
            g_ami_sidetone_use_PT_gain_HFP     = side_tone_gain;
        }else{
            g_ami_sidetone_use_PT_gain_Reserve = side_tone_gain;
        }
    #else
        #if 0//defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        ptr_race_pkt_t pLocalHeaderCmd = pvPortMalloc(sizeof(race_pkt_t) + 4);
        if(pLocalHeaderCmd != NULL){
            //pLocalHeaderCmd->hdr.pktId  = 0x05;
            pLocalHeaderCmd->hdr.type   = 0x5A;
            pLocalHeaderCmd->hdr.length = 6; //4+2
            pLocalHeaderCmd->hdr.id     = 0xE06;
            pLocalHeaderCmd->payload[0] = 0x0;
            pLocalHeaderCmd->payload[1] = 15;
            pLocalHeaderCmd->payload[2] = (int16_t) side_tone_gain;
            pLocalHeaderCmd->payload[3] = 1;
            void* ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pLocalHeaderCmd, 0);
            vPortFree(pLocalHeaderCmd);
        }
        #else
        audio_src_srv_report("[AMI] am_audio_side_tone_set_volume_by_scenario fail, not support local device", 0);
        #endif
    #endif
        }else{
            audio_src_srv_report("[AMI] am_audio_side_tone_set_volume fail, not enable sidetone", 0);
        }
  #endif
    }

    audio_src_srv_report("[AMI] am_audio_side_tone_set_volume", 0);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_side_tone_disable
 * DESCRIPTION
 *  Disable side tone
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_side_tone_disable(void)
{
#ifndef AIR_SIDETONE_USE_PASSTHROUGH_ENABLE
    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_SIDE_TONE_DISABLE, NULL,
                             FALSE, NULL);
#else
    g_ami_sidetone_use_PT_enable_state = 0;
  #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_V2)
    audio_anc_control_result_t         control_ret;
    control_ret = audio_anc_control_disable(NULL);
  #else
    #if 0//defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    ptr_race_pkt_t pLocalHeaderCmd = pvPortMalloc(sizeof(race_pkt_t) + 6);
    if(pLocalHeaderCmd != NULL){
        //pLocalHeaderCmd->hdr.pktId  = 0x05;
        pLocalHeaderCmd->hdr.type   = 0x5A;
        pLocalHeaderCmd->hdr.length = 8; //6+2
        pLocalHeaderCmd->hdr.id     = 0xE06;
        pLocalHeaderCmd->payload[0] = 0x0;
        pLocalHeaderCmd->payload[1] = 11;
        pLocalHeaderCmd->payload[2] = 1;
        void* ptr = RACE_DSPREALTIME_ANC_PASSTHRU_COSYS_HDR(pLocalHeaderCmd, 0);
        vPortFree(pLocalHeaderCmd);
    }
    #else
    audio_src_srv_report("[AMI] am_audio_side_tone_disable fail, not support local device", 0);
    #endif
  #endif
#endif

    audio_src_srv_report("[AMI] am_audio_side_tone_disable", 0);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_dl_suspend
 * DESCRIPTION
 *  Suspend DL 1
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
volatile uint8_t g_audio_dl_suspend_by_user = false;
bt_sink_srv_am_result_t am_audio_dl_suspend(void)
{
    if(g_prCurrent_player && (g_prCurrent_player->type != NONE))
    {
        g_audio_dl_suspend_by_user = true;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_DL_SUSPEND, NULL,
                                 FALSE, NULL);

        audio_src_srv_report("[AMI] am_audio_dl_suspend: Scenario:%d",1, g_prCurrent_player->type);
        return AUD_EXECUTION_SUCCESS;
    }
    else
    {
        audio_src_srv_report("[AMI] am_audio_dl_suspend Fail", 0);
        return AUD_EXECUTION_FAIL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_dl_resume
 * DESCRIPTION
 *  Resume DL 1
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_dl_resume(void)
{
    g_audio_dl_suspend_by_user = false;
    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_DL_RESUME, NULL,
                             FALSE, NULL);

    audio_src_srv_report("[AMI] am_audio_dl_resume", 0);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_ul_suspend
 * DESCRIPTION
 *  Suspend UL 1
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_ul_suspend(void)
{
    if(g_prCurrent_player && (g_prCurrent_player->type != NONE))
    {
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_UL_SUSPEND, NULL,
                                 FALSE, NULL);

        audio_src_srv_report("[AMI] am_audio_ul_suspend: Scenario:%d",1, g_prCurrent_player->type);
        return AUD_EXECUTION_SUCCESS;
    }
    else
    {
        audio_src_srv_report("[AMI] am_audio_ul_suspend Fail", 0);
        return AUD_EXECUTION_FAIL;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_ul_resume
 * DESCRIPTION
 *  Resume UL 1
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_ul_resume(void)
{
    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_UL_RESUME, NULL,
                             FALSE, NULL);

    audio_src_srv_report("[AMI] am_audio_ul_resume", 0);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_feature
 * DESCRIPTION
 *  Set parameters to DSP or set feature control through AM task
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_set_feature(bt_sink_srv_am_id_t aud_id,
    bt_sink_srv_am_feature_t *feature_param)
{
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_feature_t *local_feature = &temp_background_t.local_feature;
    am_feature_type_t feature_type, feature_type_mask;
    uint8_t shift = 0;

    if(feature_param == NULL) {
        audio_src_srv_report("[AMI] am_audio_set_param error, invalid param.", 0);
        return AUD_EXECUTION_FAIL;
    } else if(feature_param->type_mask == 0) {
        audio_src_srv_report("[AMI] am_audio_set_param error, invalid param.", 0);
        return AUD_EXECUTION_FAIL;
    } else if( (g_prCurrent_player == NULL) && (aud_id != FEATURE_NO_NEED_ID) ) {
        audio_src_srv_report("[AMI] am_audio_set_param error, no current player.", 0);
        return AUD_EXECUTION_FAIL;
    }

    feature_type_mask = feature_param->type_mask;

    temp_background_t.aud_id = aud_id;
    if(g_prCurrent_player != NULL) {
        temp_background_t.type = g_prCurrent_player->type;
    }

    do {
        feature_type = feature_type_mask & (1 << shift++);
        feature_type_mask &= (~feature_type);
        if(feature_type == 0)
            continue;

        local_feature->type_mask |= feature_type;

        switch (feature_type)
        {
#if defined(MTK_AMP_DC_COMPENSATION_ENABLE)
            case DC_COMPENSATION:
                break;
#endif

#ifdef MTK_PEQ_ENABLE
            case AM_A2DP_PEQ:
            {
                memcpy(&local_feature->feature_param.peq_param, &feature_param->feature_param.peq_param, sizeof(bt_sink_srv_am_peq_param_t));
                break;
            }
#endif
#ifdef MTK_LINEIN_PEQ_ENABLE
            case AM_LINEIN_PEQ:
            {
                memcpy(&local_feature->feature_param.peq_param, &feature_param->feature_param.peq_param, sizeof(bt_sink_srv_am_peq_param_t));
                break;
            }
#endif
#ifdef MTK_ANC_ENABLE
            case AM_ANC:
            {
#ifdef MTK_ANC_V2
                local_feature->feature_param.anc_param.event = feature_param->feature_param.anc_param.event;
                memcpy(&local_feature->feature_param.anc_param.cap, &feature_param->feature_param.anc_param.cap, sizeof(audio_anc_control_cap_t));
#if 0 //for debug
                audio_src_srv_report("am_audio_set_feature before send filter(0x%x) value(0x%x).\r\n", 2, local_feature->feature_param.anc_param.cap.filter_cap.filter_mask, local_feature->feature_param.anc_param.cap.filter_cap.param.Dgain);
                audio_src_srv_report("am_audio_set_feature with event(%d) flash_id(%d) type(%d) filter_mask(0x%x) sram_bank(%d) sram_bank_mask(0x%x)\n", 6,
                       local_feature->feature_param.anc_param.event, local_feature->feature_param.anc_param.cap.filter_cap.flash_id,
                       local_feature->feature_param.anc_param.cap.filter_cap.type, local_feature->feature_param.anc_param.cap.filter_cap.filter_mask, local_feature->feature_param.anc_param.cap.filter_cap.sram_bank, local_feature->feature_param.anc_param.cap.filter_cap.sram_bank_mask);
#endif
                break;
#else
                memcpy(&local_feature->feature_param.anc_param, &feature_param->feature_param.anc_param, sizeof(bt_sink_srv_am_anc_param_t));
                break;
#endif
            }
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            case AM_AUDIO_COSYS:
            {
                memcpy(&local_feature->feature_param.cosys_param, &feature_param->feature_param.cosys_param, sizeof(bt_sink_srv_am_cosys_param_t));
                break;
            }
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
            case AM_VP:
            {
                memcpy(&local_feature->feature_param.vp_param, &feature_param->feature_param.vp_param, sizeof(bt_sink_srv_am_vp_param_t));
                break;
            }
#endif
            case AM_DYNAMIC_CHANGE_DSP_SETTING:
            {
                local_feature->feature_param.channel = feature_param->feature_param.channel;
                break;
            }
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            case AM_ADAPTIVE_FF:
            {
                memcpy(&local_feature->feature_param.adaptive_ff_param, &feature_param->feature_param.adaptive_ff_param, sizeof(bt_sink_srv_am_adaptive_ff_param_t));
                break;
            }
#endif
#endif
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
            case AM_ANC_MONITOR:
            {
                local_feature->feature_param.anc_monitor_param= feature_param->feature_param.anc_monitor_param;
                break;
            }
#endif

#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            case AM_AUDIO_BT_SET_PLAY_EN: {
                memcpy(&local_feature->feature_param.play_en_param, &feature_param->feature_param.play_en_param, sizeof(audio_dsp_a2dp_dl_play_en_param_t));
                break;
            }
#endif
            default:
                break;
        }
    } while(feature_type_mask != 0);

    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_SET_FEATURE, &temp_background_t,
                             FALSE, NULL);

    //audio_src_srv_report("[AMI] am_audio_set_param (type: 0x%x)", 1, feature_type_mask);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_feature_ISR
 * DESCRIPTION
 *  Set parameters to DSP or set feature control through AM task
 * PARAMETERS
 *
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_audio_set_feature_ISR(bt_sink_srv_am_id_t aud_id,
    bt_sink_srv_am_feature_t *feature_param)
{
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_feature_t *local_feature = &temp_background_t.local_feature;
    am_feature_type_t feature_type, feature_type_mask;
    uint8_t shift = 0;

    if(feature_param == NULL) {
        audio_src_srv_report("[AMI] am_audio_set_param error, invalid param.", 0);
        return AUD_EXECUTION_FAIL;
    } else if(feature_param->type_mask == 0) {
        audio_src_srv_report("[AMI] am_audio_set_param error, invalid param.", 0);
        return AUD_EXECUTION_FAIL;
    } else if( (g_prCurrent_player == NULL) && (aud_id != FEATURE_NO_NEED_ID) ) {
        audio_src_srv_report("[AMI] am_audio_set_param error, no current player.", 0);
        return AUD_EXECUTION_FAIL;
    }

    feature_type_mask = feature_param->type_mask;

    temp_background_t.aud_id = aud_id;
    if(g_prCurrent_player != NULL) {
        temp_background_t.type = g_prCurrent_player->type;
    }

    do {
        feature_type = feature_type_mask & (1 << shift++);
        feature_type_mask &= (~feature_type);
        if(feature_type == 0)
            continue;

        local_feature->type_mask |= feature_type;

        switch (feature_type)
        {

#ifdef MTK_AWS_MCE_ENABLE
            case AM_HFP_AVC:
            {
                local_feature->feature_param.avc_vol = feature_param->feature_param.avc_vol;
                break;
            }
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            case AM_BLE:
            {
                memcpy(&local_feature->feature_param.ble_param, &feature_param->feature_param.ble_param, sizeof(bt_sink_srv_am_ble_param_t));
                break;
            }
#endif
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            case AM_AUDIO_BT_SET_PLAY_EN: {
                memcpy(&local_feature->feature_param.play_en_param, &feature_param->feature_param.play_en_param, sizeof(audio_dsp_a2dp_dl_play_en_param_t));
                break;
            }
#endif
            default:
                break;
        }
    } while(feature_type_mask != 0);

    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                             MSG_ID_AUDIO_SET_FEATURE, &temp_background_t,
                             TRUE, ptr_callback_amm);

    //audio_src_srv_report("[AMI] am_audio_set_param (type: 0x%x)", 1, feature_type_mask);
    return AUD_EXECUTION_SUCCESS;
}

/*****************************************************************************
 * FUNCTION
 *
 *
 *
 *
 *****************************************************************************/
bt_sink_srv_am_priority_table_t g_pseudo_device_pri_table[AM_TYPE_TOTAL] = { {AUD_HIGH   , AUD_LOW  , AUD_LOW    , AUD_LOW},    //PCM
                                                                             {AUD_MIDDLE , AUD_LOW  , AUD_LOW    , AUD_LOW},    //A2DP
                                                                             {AUD_HIGH   , AUD_LOW  , AUD_HIGH   , AUD_LOW},    //HFP
                                                                             {AUD_LOW    , AUD_LOW  , AUD_LOW    , AUD_LOW},    //NONE
                                                                             {AUD_LOW    , AUD_LOW  , AUD_LOW    , AUD_LOW},    //FILES
                                                                             {AUD_MIDDLE , AUD_LOW  , AUD_LOW    , AUD_LOW},    //AWS
                                                                             {AUD_LOW    , AUD_LOW  , AUD_MIDDLE , AUD_LOW},    //RECORD
                                                                             {AUD_LOW    , AUD_HIGH , AUD_LOW    , AUD_LOW},    //VOICE_PROMPT
                                                                             {AUD_MIDDLE , AUD_LOW  , AUD_MIDDLE , AUD_LOW},    //LINE_IN
                                                                             };
static bt_sink_srv_am_priority_table_t ami_register_get_pri_table(bt_sink_srv_am_type_t type)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    return g_pseudo_device_pri_table[type];
}

bt_sink_srv_am_id_t ami_audio_open(bt_sink_srv_am_type_t pseudo_device_type,
                                        bt_sink_srv_am_notify_callback handler)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    bt_sink_srv_am_id_t bAud_id;
    bt_sink_srv_am_priority_table_t pri_table;
    //bt_sink_srv_am_hal_result_t result = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[AudM]open-type: %d, num: %d, cb: 0x%x",3, pseudo_device_type, g_aud_id_num, (unsigned int)handler);
    //printf("[AudM]open-pri: %d, num: %d, cb: 0x%x\n", priority, g_aud_id_num, (unsigned int)handler);

    pri_table = ami_register_get_pri_table(pseudo_device_type);
    if ((g_aud_id_num < AM_REGISTER_ID_TOTAL)) {
        ami_mutex_lock(g_xSemaphore_ami);
        bAud_id = ami_register_get_id();
        ami_mutex_unlock(g_xSemaphore_ami);
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Open func(edit), ID: %d, type: %d, num: %d",3,
            bAud_id, pseudo_device_type, g_aud_id_num);
#endif
        temp_background_t.aud_id    = bAud_id;
        temp_background_t.type      = pseudo_device_type;
        temp_background_t.notify_cb = handler;
        temp_background_t.priority  = AUD_LOW;
        temp_background_t.priority_table = pri_table;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_OPEN_REQ, &temp_background_t,
                                 FALSE, NULL);
        return (bt_sink_srv_am_id_t)bAud_id;
    }
    return AUD_ID_INVALID;
}

bt_sink_srv_am_result_t ami_audio_play(bt_sink_srv_am_id_t aud_id,
                                           bt_sink_srv_am_audio_capability_t *capability_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        //(unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    audio_src_srv_report("[AudM]Test_play-id: %d, num: %d, use: %d, ptr: 0x%x, type: %d",5,
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use,
        (unsigned int)g_rAm_aud_id[aud_id].contain_ptr, capability_t->type);
    if ((aud_id < AM_REGISTER_ID_TOTAL) &&
            ((g_rAm_aud_id[aud_id].use == ID_IDLE_STATE) || (g_rAm_aud_id[aud_id].use == ID_RESUME_STATE) || (g_rAm_aud_id[aud_id].use == ID_STOPING_STATE))) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Play func, ID: %d",1, aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(true);
#endif


        temp_background_t.aud_id = aud_id;
        temp_background_t.type = capability_t->type;
        temp_background_t.audio_path_type = capability_t->audio_path_type;
        memcpy(&(temp_background_t.local_context), &(capability_t->codec), sizeof(bt_sink_srv_am_codec_t));
        memcpy(&(temp_background_t.audio_stream_in), &(capability_t->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
        memcpy(&(temp_background_t.audio_stream_out), &(capability_t->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));
        memcpy(&g_int_dev_addr, &(capability_t->dev_addr), sizeof(bt_bd_addr_t));
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_PLAY_REQ_TEST, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

bt_sink_srv_am_result_t ami_audio_stop(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x\n",
        //aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    audio_src_srv_report("[AudM]stop-id: %d, num: %d, use: %d, ptr: 0x%x",4,
        aud_id, g_aud_id_num, g_rAm_aud_id[aud_id].use, (unsigned int)g_rAm_aud_id[aud_id].contain_ptr);
    if ((aud_id < AM_REGISTER_ID_TOTAL) && (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] Stop func, ID: %d",1, aud_id);
#endif

#ifdef MTK_AUTOMOTIVE_SUPPORT
        external_dsp_echo_ref_sw_config(false);
#endif

        temp_background_t.aud_id = aud_id;
        g_rAm_aud_id[aud_id].use = ID_STOPING_STATE;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_STREAM_STOP_REQ_TEST, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}

audio_channel_t ami_get_audio_channel(void)
{
    HAL_AUDIO_CHANNEL_SELECT_t channel_sel;
    nvdm_status_t nvdm_status = NVDM_STATUS_OK;
    uint32_t tableSize = 0;

    if (g_audio_nvdm_init_flg == false) {
        /*audio nvkey have not init done,need to read the audio channel nvkey*/
        nvdm_status = nvkey_data_item_length(NVKEYID_APP_AUDIO_CHANNEL, &tableSize);
        if(nvdm_status || !tableSize){
            audio_src_srv_err("[AM] NVDM Channel_selection_init Fail. Status:%u Len:%lu\n", 2, nvdm_status, tableSize);
            return AUDIO_CHANNEL_NONE;
        }
        if(tableSize != sizeof(channel_sel)){
            audio_src_srv_err("[AM] NVDM Channel_selection_init size not match. NVDM tableSize (%lu) != (%d)\n", 2, tableSize, sizeof(channel_sel));
            return AUDIO_CHANNEL_NONE;
        }
        nvdm_status = nvkey_read_data(NVKEYID_APP_AUDIO_CHANNEL, (uint8_t *)&channel_sel, &tableSize);
        if(nvdm_status){
            audio_src_srv_err("[AM] NVDM Channel_selection_init Fail 2. Status:%d pNvdmMp3Vol:0x%x\n", 2, nvdm_status, (unsigned int)&channel_sel);
            return AUDIO_CHANNEL_NONE;
        }
    }else {
        channel_sel = audio_Channel_Select;
    }
    /*Get Stream out channel.*/
    if(channel_sel.modeForAudioChannel){
        //HW mode
        hal_gpio_status_t status = HAL_GPIO_STATUS_INVALID_PARAMETER;
        hal_gpio_data_t channel_gpio_data = HAL_GPIO_DATA_LOW;

        status = hal_gpio_get_input((hal_gpio_pin_t)channel_sel.hwAudioChannel.gpioIndex, &channel_gpio_data);
        if (status == HAL_GPIO_STATUS_OK) {
            if (channel_gpio_data == HAL_GPIO_DATA_HIGH) {
                switch(channel_sel.hwAudioChannel.audioChannelGPIOH & 0x0F)
                {
                    case AU_DSP_CH_L:
                        return AUDIO_CHANNEL_L;
                    case AU_DSP_CH_R:
                        return AUDIO_CHANNEL_R;
                    default:
                        return AUDIO_CHANNEL_NONE;
                }
            } else {
                switch(channel_sel.hwAudioChannel.audioChannelGPIOL & 0x0F)
                {
                    case AU_DSP_CH_L:
                        return AUDIO_CHANNEL_L;
                    case AU_DSP_CH_R:
                        return AUDIO_CHANNEL_R;
                    default:
                        return AUDIO_CHANNEL_NONE;
                }
            }
        } else {
            audio_src_srv_report("ami_get_audio_channel() get channel setting false with HW_mode.", 0);
            return AUDIO_CHANNEL_NONE;
        }
    } else {
        if((channel_sel.audioChannel & 0x0F) == AU_DSP_CH_L){
            return AUDIO_CHANNEL_L;
        } else if ((channel_sel.audioChannel & 0x0F) == AU_DSP_CH_R){
            return AUDIO_CHANNEL_R;
        }
    }
    return AUDIO_CHANNEL_NONE;
}

bt_sink_srv_am_result_t ami_set_audio_channel(audio_channel_t set_in_channel, audio_channel_t set_out_channel, bool rewrite_nv)
{
    nvdm_status_t result;
    uint8_t channel_temp = 0x00;

    if(audio_Channel_Select.modeForAudioChannel){
        //HW mode
        //ToDO
        return AUD_EXECUTION_FAIL;
    } else {
        //Set stream IN channel
        if(set_in_channel == AUDIO_CHANNEL_SWAP){
            channel_temp &= 0x0F;
            channel_temp |= AU_DSP_CH_SWAP << 4;
        } else if(set_in_channel == AUDIO_CHANNEL_L){
            channel_temp &= 0x0F;
            channel_temp |= AU_DSP_CH_L << 4;
        } else if(set_in_channel == AUDIO_CHANNEL_R){
            channel_temp &= 0x0F;
            channel_temp |= AU_DSP_CH_R << 4;
        } else {
            channel_temp &= 0x0F;
            channel_temp |= AU_DSP_CH_LR << 4;
        }

        //Set stream OUT channel
        if (set_out_channel == AUDIO_CHANNEL_SWAP){
            channel_temp &= 0xF0;
            channel_temp |= AU_DSP_CH_SWAP;
        } else if(set_out_channel == AUDIO_CHANNEL_L){
            channel_temp &= 0xF0;
            channel_temp |= AU_DSP_CH_L;
        } else if(set_out_channel == AUDIO_CHANNEL_R){
            channel_temp &= 0xF0;
            channel_temp |= AU_DSP_CH_R;
        } else {
            channel_temp &= 0xF0;
            channel_temp |= AU_DSP_CH_LR;
        }

        //Update Audio In/Out Channel Table
        audio_Channel_Select.audioChannel = channel_temp;

        //Update Audio Channel NVDM.
        if(rewrite_nv){
            result = nvkey_write_data(NVKEYID_APP_AUDIO_CHANNEL, (const uint8_t *)&audio_Channel_Select, sizeof(audio_Channel_Select));
            if(result){
                audio_src_srv_report("Audio Channel NVDM write error", 0);
                return AUD_EXECUTION_FAIL;
            }
        }
    }
    return AUD_EXECUTION_SUCCESS;
}

static uint8_t get_channel_hw_mode_nvkey(void)
{
    uint8_t nvkey_val = 0;

    //HW mode
    hal_gpio_status_t status = HAL_GPIO_STATUS_INVALID_PARAMETER;
    hal_gpio_data_t channel_gpio_data = HAL_GPIO_DATA_LOW;

    status = hal_gpio_get_input((hal_gpio_pin_t)audio_Channel_Select.hwAudioChannel.gpioIndex, &channel_gpio_data);
    if (status == HAL_GPIO_STATUS_OK) {
        if (channel_gpio_data == HAL_GPIO_DATA_HIGH) {
            nvkey_val = audio_Channel_Select.hwAudioChannel.audioChannelGPIOH;
        } else {
            nvkey_val = audio_Channel_Select.hwAudioChannel.audioChannelGPIOL;
        }
    } else {
        audio_src_srv_report("get_channel_hw_mode_setting() get channel setting false with HW_mode.", 0);
        assert(0);
    }

    return nvkey_val;
}

bt_sink_srv_am_result_t ami_audio_setting_init(void)
{
    nvdm_status_t result;

    /*read all audio nvkey*/
    if (audio_nvdm_configure_init() == AUD_EXECUTION_FAIL) {
        return AUD_EXECUTION_FAIL;
    }

    /*if channel setting is HW mode,adapt audio channel setting in SW mode*/
     if(audio_Channel_Select.modeForAudioChannel){
        //HW mode
        audio_Channel_Select.audioChannel = get_channel_hw_mode_nvkey();
        audio_Channel_Select.modeForAudioChannel = 0x00; /*switch SW mode*/
        /*write nvkey update the setting in SW mode*/
        result = nvkey_write_data(NVKEYID_APP_AUDIO_CHANNEL, (const uint8_t *)&audio_Channel_Select, sizeof(audio_Channel_Select));
        if(result){
            audio_src_srv_report("Audio Channel NVDM write error", 0);
            return AUD_EXECUTION_FAIL;
        }
    }

    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_set_audio_device(bt_sink_srv_am_stream_type_t in_out, audio_scenario_sel_t Audio_or_Voice, hal_audio_device_t Device, hal_audio_interface_t i2s_interface, bool rewrite_nv)
{
    nvdm_status_t result;
    uint8_t device_temp = 0;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    uint8_t interface_temp = 0;
    uint8_t mic_channel = 0;
#endif

    if(in_out == STREAM_OUT){// in: 1, out:2
        if( (Device & HAL_AUDIO_DEVICE_DAC_DUAL) || (Device & HAL_AUDIO_DEVICE_EXT_CODEC)){
            //Update Audio Out Device Table
            switch(Device){
                case HAL_AUDIO_DEVICE_I2S_MASTER:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x30;
                    #else
                        device_temp = 0x03;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DAC_DUAL:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x20;
                    #else
                        device_temp = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DAC_R:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x10;
                    #else
                        device_temp = 0x01;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DAC_L:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        //device_temp = device_temp | 0x00;
                    #else
                        device_temp = 0x00;
                    #endif
                    break;
                }
                default:
                    audio_src_srv_report("Audio set out_device error", 0);
                    break;
            }
            switch(i2s_interface){
                case HAL_AUDIO_INTERFACE_1:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        //device_temp = device_temp | 0x00;
                    #else
                        interface_temp = 0x00;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_2:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x01;
                    #else
                        interface_temp = 0x01;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_3:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x02;
                    #else
                        interface_temp = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_4:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x03;
                    #else
                        interface_temp = 0x03;
                    #endif
                    break;
                }
                default:
                    audio_src_srv_report("Audio set out_device error", 0);
                    break;
            }
            if(Audio_or_Voice == AU_DSP_VOICE){
                #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    audio_nvdm_HW_config.Voice_OutputDev = device_temp;
                #else
                    audio_nvdm_HW_config.voice_scenario.Voice_Output_Path = device_temp;
                    audio_nvdm_HW_config.voice_scenario.Voice_Output_I2S_Interface = interface_temp;
                #endif
            } else if (Audio_or_Voice == AU_DSP_AUDIO){
                #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    audio_nvdm_HW_config.Audio_OutputDev = device_temp;
                #else
                    audio_nvdm_HW_config.audio_scenario.Audio_A2DP_Output_Path = device_temp;
                    audio_nvdm_HW_config.audio_scenario.Audio_A2DP_Output_I2S_Interface = interface_temp;
                #endif
            } else if (Audio_or_Voice == AU_DSP_ANC){
                #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    audio_nvdm_HW_config.ANC_OutputDev = device_temp;
                #else

                #endif
            }
            //Update Audio Out Device NVDM.
            if(rewrite_nv){
                result = nvkey_write_data(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, (const uint8_t *)&audio_nvdm_HW_config, sizeof(audio_nvdm_HW_config));
                if(result){
                    audio_src_srv_report("Audio out_device NVDM write error", 0);
                }
            }
        } else {
            audio_src_srv_report("Audio set out_device device error", 0);
            return AUD_EXECUTION_FAIL;
        }
    } else {
        if( (Device & 0x003f) || (Device & HAL_AUDIO_DEVICE_EXT_CODEC)){
            //Update Audio In Device Table
            switch(Device){
                case HAL_AUDIO_DEVICE_I2S_MASTER:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x80;
                    #else
                        device_temp = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x42;
                    #else
                        device_temp = 0x01;
                        mic_channel = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DIGITAL_MIC_R:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x41;
                    #else

                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_DIGITAL_MIC_L:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x40;
                    #else
                        device_temp = 0x01;
                        mic_channel = 0x01;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_MAIN_MIC_DUAL:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x08;
                    #else
                        device_temp = 0x00;
                        mic_channel = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_MAIN_MIC_R:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x04;
                    #else
                        device_temp = 0x00;
                        mic_channel = 0x01;
                    #endif
                    break;
                }
                case HAL_AUDIO_DEVICE_MAIN_MIC_L:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    //device_temp = device_temp | 0x00;
                    #else
                        device_temp = 0x00;
                        mic_channel = 0x00;
                    #endif
                    break;
                }
                default:
                    audio_src_srv_report("Audio set in_device enum(%d) error", 1, Device);
                    break;
            }
            switch(i2s_interface){
                case HAL_AUDIO_INTERFACE_1:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        //device_temp = device_temp | 0x00;
                    #else
                        interface_temp = 0x00;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_2:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x10;
                    #else
                        interface_temp = 0x01;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_3:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x20;
                    #else
                        interface_temp = 0x02;
                    #endif
                    break;
                }
                case HAL_AUDIO_INTERFACE_4:{
                    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                        device_temp = device_temp | 0x30;
                    #else
                        interface_temp = 0x03;
                    #endif
                    break;
                }
                default:
                    audio_src_srv_report("Audio set In_device error", 0);
                    break;
            }
            if(Audio_or_Voice == AU_DSP_VOICE){
                #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    audio_nvdm_HW_config.Voice_InputDev = device_temp;
                #else
                    audio_nvdm_HW_config.voice_scenario.Voice_Input_Path = device_temp;
                    if (device_temp == 0x01) {
                        //DMIC
                        audio_nvdm_HW_config.voice_scenario.Voice_Input_DMIC_channel = mic_channel;
                    }else if (device_temp == 0x00) {
                        //AMIC
                        audio_nvdm_HW_config.voice_scenario.Voice_Input_AMIC_channel = mic_channel;
                    }
                #endif
            } else if(Audio_or_Voice == AU_DSP_AUDIO){
                audio_src_srv_report("Audio set Audio In_device error", 0);
            } else if(Audio_or_Voice == AU_DSP_ANC){
                #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    audio_nvdm_HW_config.MIC_Select_ANC_FF = (Device == HAL_AUDIO_DEVICE_MAIN_MIC_R) ? 0x01 : 0x00; //temp: only support to choose main mic.
                #endif
            } else if(Audio_or_Voice == AU_DSP_RECORD){
                switch(Device){
                    case HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x02;
                            audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0x03;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = 0x08;
                            audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = 0x09;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_DIGITAL_MIC_R:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x03;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = 0x09;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_DIGITAL_MIC_L:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x02;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = 0x08;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_MAIN_MIC_DUAL:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x00;
                            audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0x01;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = 0x00;
                            audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select =  0x01;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_MAIN_MIC_R:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x01;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = 0x01;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_MAIN_MIC_L:{
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x00;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Ref_Input_Select = 0x00;
                        #endif
                        break;
                    }
                    case HAL_AUDIO_DEVICE_I2S_MASTER:
                    default:
                        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                            audio_nvdm_HW_config.MIC_Select_Record_Main = 0x00;
                        #else
                            audio_nvdm_HW_config.record_scenario.Record_Main_Input_Select = 0x00;
                        #endif
                        audio_src_srv_report("Record set in_device error", 0);
                        break;
                }
            }

            //Update Audio In Device NVDM.
            if(rewrite_nv){
                result = nvkey_write_data(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, (const uint8_t *)&audio_nvdm_HW_config, sizeof(audio_nvdm_HW_config));
                if(result){
                    audio_src_srv_report("Audio in_device NVDM write error", 0);
                }
            }
        } else {
            audio_src_srv_report("Audio set in_device device error", 0);
            return AUD_EXECUTION_FAIL;
        }
    }
    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_set_audio_device_extend(bt_sink_srv_am_stream_type_t in_out, audio_scenario_sel_t Audio_or_Voice, multic_mic_config_param_t *mic_config, bool rewrite_nv) // for multi mic
{
    if(in_out == STREAM_IN){// in: 1, out:2
        /*scenario AFE config part*/
        if (Audio_or_Voice == AU_DSP_VOICE) { //0:Audio, 1:Voice
                audio_src_srv_report("Open multic mic extend: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", 6, mic_config->mic0_nvkey,
                    mic_config->mic1_nvkey,
                    mic_config->mic2_nvkey,
                    mic_config->mic3_nvkey,
                    mic_config->mic4_nvkey,
                    mic_config->mic5_nvkey);
                audio_nvdm_HW_config.voice_scenario.Voice_Input_Path = 0x04; // Multic-mic
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select = mic_config->mic0_nvkey;
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Ref1_Input_Select = mic_config->mic1_nvkey;
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Ref2_Input_Select = mic_config->mic2_nvkey;
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Ref3_Input_Select = mic_config->mic3_nvkey;
            #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Ref4_Input_Select = mic_config->mic4_nvkey;
                audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Ref5_Input_Select = mic_config->mic5_nvkey;
            #endif
            // enable all the mic_bias
            audio_nvdm_HW_config.voice_scenario.Voice_MIC_Bias_Enable = 0xFF;
        }
    }
    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_get_audio_device(void)
{
#ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    audio_src_srv_report("Get Audio_in device(0x%x) Audio_out device(0x%x) Voice_in device(0x%x) Voice_out device(0x%x)", 4, audio_nvdm_HW_config.Audio_InputDev, audio_nvdm_HW_config.Audio_OutputDev, audio_nvdm_HW_config.Voice_InputDev, audio_nvdm_HW_config.Voice_OutputDev);
#endif
    return AUD_EXECUTION_SUCCESS;
}

extern volatile uint32_t g_am_task_mask;
void ami_set_audio_mask (uint32_t flag, bool isSet)
{
    uint32_t savedmask;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(isSet){
        AUDIO_SRC_SRV_AMI_SET_FLAG(g_am_task_mask, flag);
    } else {
        AUDIO_SRC_SRV_AMI_RESET_FLAG(g_am_task_mask, flag);
    }
    hal_nvic_restore_interrupt_mask(savedmask);
}

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
bt_sink_srv_am_result_t ami_set_app_notify_callback(am_audio_app_callback_t ami_app_callback)
{
    audio_src_srv_report("Set ami app callback(0x%x)", 1, ami_app_callback);
    g_am_audio_app_callback = ami_app_callback;
    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_set_afe_param (bt_sink_srv_am_stream_type_t stream_type, hal_audio_sampling_rate_t sampling_rate, bool enable)
{
    audio_src_srv_report("[Rdebug]ami_set_afe_param type(%d) enable(%d) spl_hz(%d)", 3, stream_type, enable, sampling_rate);
    uint32_t savedmask;
    audio_app_param_t app_param;
    app_param.I2S_param.Enable = enable;
    app_param.I2S_param.sampling_rate = sampling_rate;
    bool is_main_stream = false;
    bool main_stream_exist = false;
    bool DL2_exist = false;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    if(enable){
        if(stream_type != STREAM_OUT_2){
            is_main_stream = true;
            if(g_am_task_mask & AM_TASK_MASK_VP_HAPPENING){
               DL2_exist = true;
            }
        } else {
            is_main_stream = false;
            if((g_am_task_mask & AM_TASK_MASK_DL1_HAPPENING) ||
                (g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE) ||
                (g_am_task_mask & AM_TASK_MASK_UL1_HAPPENING)){
               main_stream_exist = true;
            }
        }
        if(is_main_stream){
            if(DL2_exist){
                hal_nvic_restore_interrupt_mask(savedmask);
                //**SUSPEND. No need to Notify app sampling rate.
            } else {
                hal_nvic_restore_interrupt_mask(savedmask);
                //**Notify app sampling rate.
                if(g_am_audio_app_callback != NULL){
                    if(g_param_for_external_dsp.Enable == false){
                        g_am_audio_app_callback(AUDIO_APPS_EVENTS_SAMPLE_RATE_CHANGE, app_param);
                    }
                    g_param_for_external_dsp.Enable        = app_param.I2S_param.Enable;
                    g_param_for_external_dsp.sampling_rate = app_param.I2S_param.sampling_rate;
                }
            }
        } else {
            if(main_stream_exist){
                hal_nvic_restore_interrupt_mask(savedmask);
                //**No need to Notify app sampling rate.
            } else {
                hal_nvic_restore_interrupt_mask(savedmask);
                //**Notify app sampling rate.
                if(g_am_audio_app_callback != NULL){
                    g_am_audio_app_callback(AUDIO_APPS_EVENTS_SAMPLE_RATE_CHANGE, app_param);
                    g_param_for_external_dsp.Enable        = app_param.I2S_param.Enable;
                    g_param_for_external_dsp.sampling_rate = app_param.I2S_param.sampling_rate;
                }
            }
        }
    } else {
        hal_nvic_restore_interrupt_mask(savedmask);
        if(false == ((g_am_task_mask & AM_TASK_MASK_VP_HAPPENING) ||
                      (g_am_task_mask & AM_TASK_MASK_DL1_HAPPENING) ||
                      (g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE) ||
                      (g_am_task_mask & AM_TASK_MASK_UL1_HAPPENING)))
        {
            //**Notify app disable.
            if(g_am_audio_app_callback != NULL){
                g_am_audio_app_callback(AUDIO_APPS_EVENTS_SAMPLE_RATE_CHANGE, app_param);
                g_param_for_external_dsp.Enable 	   = app_param.I2S_param.Enable;
                g_param_for_external_dsp.sampling_rate = app_param.I2S_param.sampling_rate;
            }
        }
    }
    return AUD_EXECUTION_SUCCESS;
}
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
/*****************************************************************************
 * FUNCTION
 *  am_prompt_control_play_tone
 * DESCRIPTION
 *  This function is prompt_sound_control ami interface which is used to lead VP request calling API
 *  or calling task event. (APP VP request >> AM Task event >> MP3 Task event)
 * PARAMETERS
 *  tone_type       [IN]
 *  *tone_buf       [IN]
 *  tone_size        [IN]
 *  sync_time       [IN]
 *  callback          [OUT]
 *SAMPLE CODE:
 *     @code
 *         static const uint8_t voice_prompt_data[];    //have to be static data.
 *         tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_long;
 *         tone_size = sizeof(voice_prompt_mix_mp3_tone_long);
 *         am_prompt_control_play_tone(VPC_MP3, tone_buf, tone_size, 0, voice_prompt_callback);
 *     @endcode
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_prompt_control_play_sync_tone(prompt_control_tone_type_t tone_type,
                                                            uint8_t *tone_buf,
                                                            uint32_t tone_size,
                                                            uint32_t sync_time,
                                                            prompt_control_callback_t callback)
{
#ifdef MTK_MP3_TASK_DEDICATE
    audio_src_srv_report("[AMI] am_audio_prompt_control_play_tone", 0);
    bt_sink_srv_am_feature_t feature_param;
    feature_param.type_mask = AM_VP;
    feature_param.feature_param.vp_param.event = PROMPT_CONTROL_MEDIA_PLAY;

    feature_param.feature_param.vp_param.tone_type       = tone_type;
    feature_param.feature_param.vp_param.tone_buf        = tone_buf;
    feature_param.feature_param.vp_param.tone_size       = tone_size;
    feature_param.feature_param.vp_param.sync_time       = sync_time;
    feature_param.feature_param.vp_param.vp_end_callback = callback;

    return am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
#else
    audio_src_srv_report("[AMI] prompt_control_play_tone [MP3 Task dynamic]", 0);
    bool ret = false;
    ret = prompt_control_play_tone_internal(tone_type, tone_buf, tone_size, sync_time, callback);
    if (ret == true){
        return AUD_EXECUTION_SUCCESS;
    } else {
        return AUD_EXECUTION_FAIL;
    }
#endif
}

/*****************************************************************************
 * FUNCTION
 *  am_prompt_control_stop_tone
 * DESCRIPTION
 *  This function is prompt_sound_control ami interface which is used to lead VP request calling API
 *  or calling task event. (APP VP request >> AM Task event >> MP3 Task event)
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t am_prompt_control_stop_tone(void)
{
#ifdef MTK_MP3_TASK_DEDICATE
    #if 1
    audio_src_srv_report("[AMI] am_audio_prompt_control_stop_tone", 0);
    bt_sink_srv_am_feature_t feature_param;
    feature_param.type_mask = AM_VP;
    feature_param.feature_param.vp_param.event = PROMPT_CONTROL_MEDIA_STOP;

    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
    #else
    audio_src_srv_report("[AMI] am_audio_prompt_control_stop_tone direct.", 0);
    prompt_control_stop_tone_internal();
    #endif
#else
    audio_src_srv_report("[AMI] prompt_control_stop_tone [MP3 Task dynamic]", 0);
    prompt_control_stop_tone_internal();
#endif
    return AUD_EXECUTION_SUCCESS;
}
#endif

bt_sink_srv_am_result_t am_dynamic_change_channel(audio_channel_selection_t change_channel)
{
    if(change_channel < AUDIO_CHANNEL_SELECTION_NUM){
        audio_src_srv_report("[AMI] am_dynamic_change (%d)channel", 1, change_channel);
    }else {
        audio_src_srv_report("[AMI] am_dynamic_change_channel_error", 0);
        return AUD_EXECUTION_FAIL;
    }
    bt_sink_srv_am_feature_t feature_param;
    feature_param.type_mask = AM_DYNAMIC_CHANGE_DSP_SETTING;
    feature_param.feature_param.channel = change_channel;

    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
    return AUD_EXECUTION_SUCCESS;
}

void am_hfp_ndvc_sent_avc_vol(bt_sink_avc_sync_statys_t sync_status)
{
    uint32_t avc_vol;
    if(sync_status == AVC_INIT_SYNC) {
        avc_vol = 8192;
    } else if (sync_status == AVC_UPDATE_SYNC) {
        avc_vol = hal_audio_dsp2mcu_data_get();
    } else {
		avc_vol = 8192;
	}
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_info_t info;
    info.module_id = BT_AWS_MCE_REPORT_MODULE_HFP_AVC;
    info.param_len = sizeof(uint32_t);
    info.param = &avc_vol;
    bt_aws_mce_report_send_event(&info);
    audio_src_srv_report("[NDVC] Agent sent IF pkt, avc_vol: %d", 1, avc_vol);
#endif

    // Sent avc_vol back to agent to update agent's volumne
    bt_sink_srv_am_feature_t feature_param;
    feature_param.type_mask = AM_HFP_AVC;
    feature_param.feature_param.avc_vol = avc_vol;
    am_audio_set_feature_ISR(FEATURE_NO_NEED_ID, &feature_param);
    //audio_src_srv_report("[NDVC] Agent sent IF pkt done", 0);
}

#ifdef MTK_AWS_MCE_ENABLE
void am_hfp_ndvc_callback(bt_aws_mce_report_info_t *para)
{
    if(para != NULL) {
        uint32_t* avc_vol = para->param;
        bt_sink_srv_am_feature_t feature_param;
        feature_param.type_mask = AM_HFP_AVC;
        feature_param.feature_param.avc_vol = *avc_vol;
        audio_src_srv_report("[NDVC] Partner receive IF pkt, avc_vol: %d", 1, avc_vol);
        am_audio_set_feature_ISR(FEATURE_NO_NEED_ID, &feature_param);
    } else {
        audio_src_srv_report("[am_hfp_ndvc_receive_callback para] if NULL", 0);
    }
}
#endif

bool get_audio_dump_info_from_nvdm(DSP_PARA_DATADUMP_STRU *p_DumpInfo)
{
    uint32_t tableSize;
    nvkey_status_t status;

    status = nvkey_data_item_length(NVKEYID_DSP_FW_PARA_DATADUMP, &tableSize);
    if(status || !tableSize || (tableSize != sizeof(DSP_PARA_DATADUMP_STRU))){
        audio_src_srv_report("[Sink][Setting]get_audio_dump_info_from_nvdm query length Fail. Status:%d Len:%d\n", 2, 2, status, tableSize);
        return false;
    }

    status = nvkey_read_data(NVKEYID_DSP_FW_PARA_DATADUMP, (uint8_t *)p_DumpInfo, &tableSize);
    if(status || !tableSize){
        audio_src_srv_report("[Sink][Setting]get_audio_dump_info_from_nvdm read nvdm Fail. Status:%d tableSize:%d\n", 2, 2, status, tableSize);
        return false;
    }

    return true;
}

#ifdef AIR_SILENCE_DETECTION_ENABLE
extern audio_dsp_silence_detection_param_t silence_detection_info;
void audio_silence_detection_nvdm_ccni(void)
{
    uint16_t nvkey_id = NVKEY_DSP_PARA_SILENCE_DETECTION;
    uint32_t nvkey_length = 0;
    void *malloc_ptr = NULL;
    sysram_status_t nvdm_status;

    nvdm_status =  flash_memory_query_nvdm_data_length(nvkey_id, &nvkey_length);
    if (nvdm_status || !nvkey_length) {
        AUD_LOG_E("[SD] Read Nvkey length Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
    }
    malloc_ptr = pvPortMallocNC(nvkey_length);
    if (malloc_ptr) {
        nvdm_status = flash_memory_read_nvdm_data(nvkey_id, (uint8_t *)malloc_ptr, &nvkey_length);
        if (nvdm_status || !nvkey_length) {
            AUD_LOG_E("[SD] Read Nvkey data Fail id:0x%x, status:%d ", 2, nvkey_id, nvdm_status);
        }
        memcpy(&(silence_detection_info.NvKey), malloc_ptr, sizeof(SD_NVKEY_STATE));
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_ALGORITHM_PARAM, nvkey_id, (uint32_t)malloc_ptr, true);
        vPortFreeNC(malloc_ptr);
    } else {
        AUD_LOG_E("[SD] malloc Fail", 0);
        assert(0);
    }
}

void audio_silence_detection_callback_register(silence_detection_callback_t FunPtr)
{
    silence_detection_info.callback = FunPtr;
}

U32 audio_silence_detection_get_detect_time_s(void)
{
    return silence_detection_info.NvKey.AutoPowerOff_Time;
}
#endif


void audio_driver_configure_set_hold_amp_gpio(uint32_t is_hold)
{
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_DRIVER_PARAM,
                                          AUDIO_DRIVER_SET_HOLD_AMP_GPIO,
                                          is_hold,
                                          true);
}

void am_set_audio_output_volume_parameters_to_nvdm(ami_audio_volume_parameters_nvdm_t *volume_param_ptr)
{
    sysram_status_t ret;
    ret = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_VOLUME_PARAMETERS, (uint8_t *)volume_param_ptr, sizeof(ami_audio_volume_parameters_nvdm_t));
    if(ret){
        audio_src_srv_report("[Sink][Setting]set_audio_output_volume_parameters_to_nvdm nvdm Fail. Ret:%d \n", 1, ret);
    }
}

#ifdef AIR_WIRED_AUDIO_ENABLE
void am_set_audio_output_volume_parameters_2_to_nvdm(ami_audio_volume_parameters_nvdm_t *volume_param_ptr)
{
    sysram_status_t ret;
    ret = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_VOLUME_PARAMETERS_2, (uint8_t *)volume_param_ptr, sizeof(ami_audio_volume_parameters_nvdm_t));
    if(ret){
        audio_src_srv_report("[Sink][Setting]set_audio_output_volume_parameters_2_to_nvdm nvdm Fail. Ret:%d \n", 1, ret);
    }
}
#endif

void am_load_audio_output_volume_parameters_from_nvdm(void)
{
    sysram_status_t ret;
    uint32_t mask[2];
    uint32_t length = sizeof(ami_audio_volume_parameters_nvdm_t);

    /*
    Ratio |    db    | Compensation
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

    ret = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VOLUME_PARAMETERS, (uint8_t *)mask, &length);
    if(ret){
        audio_src_srv_report("[Sink][Setting]load_audio_output_volume_parameters Fail. Ret:%d \n", 1, ret);
    }
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_VOLUME_PARAMETERS, (uint16_t)mask[1], mask[0], true);
    audio_src_srv_report("[Sink][Setting]load_audio_output_volume_parameters. Ret:%d , mask[1]=%x, mask[0]=%x\n", 3, ret, (uint16_t)mask[1], mask[0]);

#ifdef AIR_WIRED_AUDIO_ENABLE
    ret = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VOLUME_PARAMETERS_2, (uint8_t *)mask, &length);
    if(ret){
        audio_src_srv_report("[Sink][Setting]load_audio_output_volume_parameters_2 Fail. Ret:%d \n", 1, ret);
    }
    else{
        mask[0] &= 0xFFFF; // clear MSB 16bit
        mask[0] |= 0x5AA50000; // 0x5AA5
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_VOLUME_PARAMETERS, (uint16_t)mask[1], mask[0], true);
        audio_src_srv_report("[Sink][Setting]load_audio_output_volume_parameters_2. Ret:%d , mask[1]=%x, mask[0]=%x\n", 3, ret, (uint16_t)mask[1], mask[0]);
    }
#endif
}


bt_sink_srv_am_result_t ami_hal_set_mutex_lock(void)
{
    bt_sink_srv_am_result_t status = AUD_EXECUTION_SUCCESS;

    if ( g_ami_hal_semaphore_handle != NULL ) {
        audio_src_srv_report("[AMI] ami_hal_mutex_lock() +\r\n", 0);
        xSemaphoreTake(g_ami_hal_semaphore_handle, portMAX_DELAY);
    }
    else {
        status = AUD_EXECUTION_FAIL;
    }

    return status;
}
bt_sink_srv_am_result_t ami_hal_set_mutex_unlock(void)
{
    bt_sink_srv_am_result_t status = AUD_EXECUTION_SUCCESS;

    if ( g_ami_hal_semaphore_handle != NULL ) {
        xSemaphoreGive(g_ami_hal_semaphore_handle);
        audio_src_srv_report("[AMI] ami_hal_mutex_lock() -\r\n", 0);
    }
    else {
        status = AUD_EXECUTION_FAIL;
    }

    return status;
}

/*****************************************************************************
 * FUNCTION
 *  ami_hal_audio_status_set_running_flag
 * DESCRIPTION
 *  This function is used to protect hal_audio set running flag.
 * PARAMETERS
 *  type               [IN]
 *  is_running       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void ami_hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running)
{
    if ( g_ami_hal_semaphore_handle == NULL ) {
       g_ami_hal_semaphore_handle = xSemaphoreCreateMutex();
    }
    ami_hal_set_mutex_lock();
    hal_audio_status_set_running_flag(type, is_running);
    ami_hal_set_mutex_unlock();
}

bool ami_hal_audio_status_query_running_flag(audio_message_type_t type)
{
    bool query_status;
    if ( g_ami_hal_semaphore_handle == NULL ) {
       g_ami_hal_semaphore_handle = xSemaphoreCreateMutex();
    }
    ami_hal_set_mutex_lock();
    query_status = hal_audio_status_query_running_flag(type);
    ami_hal_set_mutex_unlock();
    return query_status;
}

bool ami_hal_audio_status_query_running_flag_by_type_list(audio_message_type_t *type_list, uint32_t type_list_num)
{
    bool query_status = false;
    if ( g_ami_hal_semaphore_handle == NULL ) {
       g_ami_hal_semaphore_handle = xSemaphoreCreateMutex();
    }
    ami_hal_set_mutex_lock();
    for(uint32_t i = 0; i < type_list_num; i++) {
        query_status = hal_audio_status_query_running_flag(type_list[i]);
        if(query_status == true) {
            break;
        }
    }
    ami_hal_set_mutex_unlock();
    return query_status;
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_send_amm
 * DESCRIPTION
 *  This function is used to send audio manager message.
 * PARAMETERS
 *  dest_id          [IN]
 *  src_id           [IN]
 *  cb_msg_id        [IN]
 *  msg_id           [IN]
 *  background_info  [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id,
                              bt_sink_srv_am_module_t src_id,
                              bt_sink_srv_am_cb_msg_class_t cb_msg_id,
                              bt_sink_srv_am_msg_id_t msg_id,
                              bt_sink_srv_am_background_t *background_info,
                              uint8_t fromISR,
                              bt_sink_srv_am_amm_struct *pr_Amm)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifndef WIN32_UT
    bt_sink_srv_am_amm_struct *g_prAmm_current = NULL;
#endif
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[AudM]msg_id = %d, fromISR = %d", 2, msg_id, fromISR);
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) {
#ifndef WIN32_UT
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
#else
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)malloc(sizeof(bt_sink_srv_am_amm_struct));
#endif

        if (g_prAmm_current == NULL) {
            audio_src_srv_report("[AudM]error. Allocate fail", 0);
            return;
        }

        g_prAmm_current->cb_msg_id = cb_msg_id;
        g_prAmm_current->src_mod_id = src_id;
        g_prAmm_current->dest_mod_id = dest_id;
        g_prAmm_current->msg_id = msg_id;
        if (background_info) {
            memcpy(&(g_prAmm_current->background_info), background_info, sizeof(bt_sink_srv_am_background_t));
        }

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            /*Do NOT AM Task send AM queue in this path.*/
            bool is_am_task = false;
            uint32_t savedmask;
            hal_nvic_save_and_set_interrupt_mask(&savedmask);
            if(AUDIO_SRC_SRV_AM_TASK == pxCurrentTCB){
                is_am_task = true;
            } else {
                is_am_task = false;
            }
            hal_nvic_restore_interrupt_mask(savedmask);
            if(is_am_task){
#ifdef AIR_AM_DIRECT_EXEC_ENABLE
                /* Execute directly when performing "Audio_transmitter operation" on AM task.*/
                if(msg_id == MSG_ID_STREAM_OPEN_REQ ||\
                    msg_id == MSG_ID_STREAM_PLAY_REQ_TEST ||\
                    msg_id == MSG_ID_STREAM_STOP_REQ_TEST ||\
                    msg_id == MSG_ID_STREAM_CLOSE_REQ) {
                    audio_src_srv_report("[AudM]Execute directly Audio_transmitter operation", 0);
                    am_direct_exec(g_prAmm_current);
                } else {
                    audio_src_srv_report("[AudM]warning. ============== AM send own queue has risk ==============", 0);
                    if(xQueueSend(g_xQueue_am, (void *) &g_prAmm_current, (TickType_t) 0) != pdPASS) {
                        audio_src_srv_report("[AudM]error. AM queue full when AM send own queue.", 0);
                        configASSERT(0);
                    }
                }
#else
                audio_src_srv_report("[AudM]warning. ============== AM send own queue has risk ==============", 0);
                if(xQueueSend(g_xQueue_am, (void *) &g_prAmm_current, (TickType_t) 0) != pdPASS) {
                    audio_src_srv_report("[AudM]error. AM queue full when AM send own queue.", 0);
                    configASSERT(0);
                }
#endif
            } else {
                xQueueSend(g_xQueue_am, (void *) &g_prAmm_current, portMAX_DELAY);
            }
        } else {
            audio_src_srv_report("AudM]error. AM send queue fail, AM queue not create yet. Dst_id:%d,Src_id:%d,CB_msg_id:%d,Msg_ID:%d.",
                4, dest_id, src_id, cb_msg_id, msg_id);
        }
#endif
    } else {
        pr_Amm->cb_msg_id = cb_msg_id;
        pr_Amm->src_mod_id = src_id;
        pr_Amm->dest_mod_id = dest_id;
        pr_Amm->msg_id = msg_id;
        if (background_info) {
            memcpy(&(pr_Amm->background_info), background_info, sizeof(bt_sink_srv_am_background_t));
        }

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            if(pdFALSE == xQueueIsQueueFullFromISR(g_xQueue_am)){
                xQueueSendFromISR(g_xQueue_am, (void *) &pr_Amm, &xHigherPriorityTaskWoken);
            }else{
                audio_src_srv_report("[AudM]Send queue error. Queue full (Drop:msg_id (%d))", 1, pr_Amm->msg_id);
            }
        } else {
            audio_src_srv_report("AudM]error. AM send queue from ISR fail, AM queue not create yet. Dst_id:%d,Src_id:%d,CB_msg_id:%d,Msg_ID:%d.",
                4, dest_id, src_id, cb_msg_id, msg_id);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_send_to_front_amm
 * DESCRIPTION
 *  This function is used to send audio manager message.
 * PARAMETERS
 *  dest_id          [IN]
 *  src_id           [IN]
 *  cb_msg_id        [IN]
 *  msg_id           [IN]
 *  background_info  [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_sink_srv_ami_send_to_front_amm(bt_sink_srv_am_module_t dest_id,
                              bt_sink_srv_am_module_t src_id,
                              bt_sink_srv_am_cb_msg_class_t cb_msg_id,
                              bt_sink_srv_am_msg_id_t msg_id,
                              bt_sink_srv_am_background_t *background_info,
                              uint8_t fromISR,
                              bt_sink_srv_am_amm_struct *pr_Amm)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
#ifndef WIN32_UT
    bt_sink_srv_am_amm_struct *g_prAmm_current = NULL;
#endif
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[AudM]front, msg_id = %d, fromISR = %d", 2, msg_id, fromISR);
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) {
#ifndef WIN32_UT
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
#else
        g_prAmm_current = (bt_sink_srv_am_amm_struct *)malloc(sizeof(bt_sink_srv_am_amm_struct));
#endif

        if (g_prAmm_current == NULL) {
            audio_src_srv_report("[AudM]front, error. Allocate fail", 0);
            return;
        }

        g_prAmm_current->cb_msg_id = cb_msg_id;
        g_prAmm_current->src_mod_id = src_id;
        g_prAmm_current->dest_mod_id = dest_id;
        g_prAmm_current->msg_id = msg_id;
        if (background_info) {
            memcpy(&(g_prAmm_current->background_info), background_info, sizeof(bt_sink_srv_am_background_t));
        }

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            /*Do NOT AM Task send AM queue in this path.*/
            bool is_am_task = false;
            uint32_t savedmask;
            hal_nvic_save_and_set_interrupt_mask(&savedmask);
            if(AUDIO_SRC_SRV_AM_TASK == pxCurrentTCB){
                is_am_task = true;
            } else {
                is_am_task = false;
            }
            hal_nvic_restore_interrupt_mask(savedmask);
            if(is_am_task){
                audio_src_srv_report("[AudM]front, warning. ============== AM send own queue has risk ==============", 0);
                if(xQueueSendToFront(g_xQueue_am, (void *) &g_prAmm_current, (TickType_t) 0) != pdPASS) {
                    audio_src_srv_report("[AudM]front, error. AM queue full when AM send own queue.", 0);
                    configASSERT(0);
                }
            } else {
                xQueueSendToFront(g_xQueue_am, (void *) &g_prAmm_current, portMAX_DELAY);
            }
        } else {
            audio_src_srv_report("AudM]front, error. AM send queue fail, AM queue not create yet. Dst_id:%d,Src_id:%d,CB_msg_id:%d,Msg_ID:%d.",
                4, dest_id, src_id, cb_msg_id, msg_id);
            vPortFree(g_prAmm_current);
            g_prAmm_current = NULL;
        }
#endif
    } else {
        pr_Amm->cb_msg_id = cb_msg_id;
        pr_Amm->src_mod_id = src_id;
        pr_Amm->dest_mod_id = dest_id;
        pr_Amm->msg_id = msg_id;
        if (background_info) {
            memcpy(&(pr_Amm->background_info), background_info, sizeof(bt_sink_srv_am_background_t));
        }

#ifndef WIN32_UT
        if (g_xQueue_am != 0) {
            if(pdFALSE == xQueueIsQueueFullFromISR(g_xQueue_am)){
                xQueueSendToFrontFromISR(g_xQueue_am, (void *) &pr_Amm, &xHigherPriorityTaskWoken);
            }else{
                audio_src_srv_report("[AudM]front, Send queue error. Queue full (Drop:msg_id (%d))", 1, pr_Amm->msg_id);
            }
        } else {
            audio_src_srv_report("AudM]front, error. AM send queue from ISR fail, AM queue not create yet. Dst_id:%d,Src_id:%d,CB_msg_id:%d,Msg_ID:%d.",
                4, dest_id, src_id, cb_msg_id, msg_id);
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
#endif
    }
}

#if STANDALONE_TEST
/* Hal API func */
hal_audio_status_t hal_audio_get_memory_size(uint32_t *memory_size)
{
    //*memory_size = 16<<10;
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_memory(uint16_t *memory)
{
    return HAL_AUDIO_STATUS_OK;
}
#endif

#ifdef MTK_AWS_MCE_ENABLE
static bt_aws_mce_report_module_id_t ami_event_to_report_module_id(ami_event_t ami_event)
{
    bt_aws_mce_report_module_id_t module_id;
    switch (ami_event) {
        case AMI_EVENT_PEQ_REALTIME:
        case AMI_EVENT_PEQ_CHANGE_MODE:
        case AMI_EVENT_ATTACH_INFO:
        case AMI_EVENT_ATTACH_NVDM_INFO:
            module_id = BT_AWS_MCE_REPORT_MODULE_PEQ;
            break;
        default:
            module_id = -1;
            break;
    }
    return module_id;
}

/* R-note: ANC */
static void aws_mce_ami_send_attch_packet(void)
{
#if defined(MTK_ANC_ENABLE) || defined(MTK_PEQ_ENABLE)
    AMI_AWS_MCE_ATTACH_PACKT_t attach_packet;
    memset(&attach_packet, 0, sizeof(AMI_AWS_MCE_ATTACH_PACKT_t));
#ifdef MTK_ANC_ENABLE
    uint8_t anc_enable, attach_enable;
#ifdef MTK_ANC_V2
    attach_enable = audio_anc_control_get_attach_enable();
#else
    anc_get_attach_enable(&attach_enable);
#endif
    if (attach_enable == 1) {
#ifdef MTK_ANC_V2
        audio_anc_control_filter_id_t filter_id;
        audio_anc_control_type_t      type;
        int16_t                       runtime_gain;
        audio_anc_control_get_status(&anc_enable, &filter_id, &type, &runtime_gain, NULL, NULL);
        attach_packet.anc_enable       = anc_enable;
        attach_packet.anc_filter_id    = filter_id;
        attach_packet.anc_filter_type  = type;
        attach_packet.anc_runtime_gain = runtime_gain;
#else
        uint32_t runtime_info = 0;
        anc_get_status(NULL, &runtime_info, NULL);
        anc_get_backup_status(&anc_enable);
        attach_packet.anc_enable = anc_enable;
        attach_packet.anc_runtime_gain = (int16_t)(runtime_info >> 16);
        attach_packet.anc_filter_type = (uint8_t)(runtime_info & 0xF);
#endif
    } else {
        attach_packet.anc_enable = 0xFF;
    }
#ifdef MTK_ANC_V2
    audio_src_srv_report("[mce_ami] send attach info: anc:%d %d %d %d\n", 4, attach_packet.anc_enable, attach_packet.anc_filter_id, attach_packet.anc_filter_type, attach_packet.anc_runtime_gain);
#else
    audio_src_srv_report("[mce_ami] send attach info: anc:%d %d %d\n", 3, attach_packet.anc_enable, attach_packet.anc_filter_type, attach_packet.anc_runtime_gain);
#endif
#endif
#ifdef MTK_PEQ_ENABLE
    if(aud_peq_get_sound_mode(A2DP, &attach_packet.a2dp_pre_peq_enable) != 0) {
        audio_src_srv_err("[mce_ami] send attach info: get peq sound mode error !!!\n", 0);
    }
    audio_src_srv_report("[mce_ami] send attach info: peq: %d %d\n", 4, attach_packet.a2dp_pre_peq_enable, attach_packet.a2dp_pre_peq_sound_mode, attach_packet.a2dp_post_peq_enable, attach_packet.a2dp_post_peq_sound_mode);
#endif
    bt_sink_srv_aws_mce_ami_data(AMI_EVENT_ATTACH_INFO, (uint8_t *)&attach_packet, sizeof(AMI_AWS_MCE_ATTACH_PACKT_t), false);
#endif
}

#ifdef SUPPORT_PEQ_NVKEY_UPDATE
static void aws_mce_ami_send_attch_nvdm_packet(void)
{
    uint32_t total_size;
    uint8_t *packet = NULL;
    aud_peq_get_changed_nvkey(&packet, &total_size);
    if ((packet != NULL) && (total_size > 0)) {
        bt_sink_srv_aws_mce_ami_data(AMI_EVENT_ATTACH_NVDM_INFO, (uint8_t *)packet, total_size, false);
    }
    if (packet != NULL) {
        vPortFree(packet);
    }
}
#endif


/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_aws_mce_ami_state_change_handler
 * DESCRIPTION
 *  For BT_SINK_SRV_MODULE_INTERNAL_AMI, to be notified when receives AWS MCE state change.
 * PARAMETERS
 *  state_ind     [IN]
 * RETURNS
 *****************************************************************************/
static void bt_sink_srv_aws_mce_ami_state_change_handler(bt_aws_mce_state_change_ind_t *state_ind)  /*R-note: Partner Later join. */
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
    uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(local_addr);

    audio_src_srv_report("[mce_ami]state_change(s)-aws_hd: 0x%x, state 0x%0x", 2, state_ind->handle, state_ind->state);

    if(state_ind->handle == aws_handle) {
        audio_src_srv_report("[mce_ami]It is same with special link handle. [Still send attach].", 0);
        //return;
    }

    if (role == BT_AWS_MCE_ROLE_AGENT) {
        if (state_ind->state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
#ifdef SUPPORT_PEQ_NVKEY_UPDATE
            aws_mce_ami_send_attch_nvdm_packet();
#endif
            aws_mce_ami_send_attch_packet();
            am_hfp_ndvc_sent_avc_vol(AVC_INIT_SYNC);
            audio_src_srv_report("[mce_ami] Partner is attached !", 0);
        } else if(state_ind->state == BT_AWS_MCE_AGENT_STATE_INACTIVE) { // disconnect
            audio_src_srv_report("[mce_ami] Agent is inactive", 0);
        } else {
            audio_src_srv_report("[mce_ami] Please handle deattach precedure", 0);
        }
    } else {
        audio_src_srv_report("It is not on agent", 0);
    }
}

static void aws_mce_ami_receive_attch_packet(AMI_AWS_MCE_ATTACH_PACKT_t *attach_packet)
{
#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
{
    audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
    audio_src_srv_report("[mce_ami] receive attach info: anc: %d %d %d %d\n", 4, attach_packet->anc_enable, attach_packet->anc_filter_id, attach_packet->anc_filter_type, attach_packet->anc_runtime_gain);
    if(attach_packet->anc_runtime_gain != AUDIO_ANC_CONTROL_UNASSIGNED_GAIN){
        anc_ret = audio_anc_control_set_runtime_gain(attach_packet->anc_runtime_gain, attach_packet->anc_filter_type);
        if(anc_ret != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS){
            audio_src_srv_report("[mce_ami] set_runtime_gain fail(%d)\n", 1, anc_ret);
        }
    }
    if(attach_packet->anc_enable == 1){
        anc_ret = audio_anc_control_enable(attach_packet->anc_filter_id, attach_packet->anc_filter_type, NULL);
        if(anc_ret != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS){
            audio_src_srv_report("[mce_ami] anc enable fail(%d)\n", 1, anc_ret);
        }
    } else {
        anc_ret = audio_anc_control_disable(NULL);
        if(anc_ret != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS){
            audio_src_srv_report("[mce_ami] anc disable fail(%d)\n", 1, anc_ret);
        }
    }
    _UNUSED(anc_ret);
}
#else
{
    uint32_t anc_filter_type;
    uint32_t anc_runtime_gain;
    audio_src_srv_report("[mce_ami] receive attach info: anc: %d %d %d\n", 3, attach_packet->anc_enable, attach_packet->anc_filter_type, attach_packet->anc_runtime_gain);

    if (audio_anc_backup_while_suspend(attach_packet->anc_enable, attach_packet->anc_filter_type, attach_packet->anc_runtime_gain) == ANC_CONTROL_EXECUTION_SUCCESS) {
        if (attach_packet->anc_enable == 1) {
            anc_filter_type = attach_packet->anc_filter_type;
            anc_runtime_gain = (uint32_t)attach_packet->anc_runtime_gain;
            audio_anc_control(ANC_CONTROL_EVENT_ON, (anc_filter_type<<16)|(anc_runtime_gain&0xFFFF), NULL);
        } else {
            anc_runtime_gain = (uint32_t)attach_packet->anc_runtime_gain;
            audio_anc_control(ANC_CONTROL_EVENT_OFF, 0, NULL);
            audio_anc_control(ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME, anc_runtime_gain, NULL);
        }
    }
}
#endif
#endif
#ifdef MTK_PEQ_ENABLE
{
    bt_sink_srv_am_feature_t am_feature;
    audio_src_srv_report("[mce_ami] receive attach info: peq: %d %d %d %d \n", 4, attach_packet->a2dp_pre_peq_enable, attach_packet->a2dp_pre_peq_sound_mode, attach_packet->a2dp_post_peq_enable, attach_packet->a2dp_post_peq_sound_mode);
    memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
    am_feature.type_mask                             = AM_A2DP_PEQ;
    am_feature.feature_param.peq_param.enable        = attach_packet->a2dp_pre_peq_enable;
    am_feature.feature_param.peq_param.sound_mode    = attach_packet->a2dp_pre_peq_sound_mode;
    am_feature.feature_param.peq_param.setting_mode  = PEQ_DIRECT;
    am_feature.feature_param.peq_param.not_clear_sysram = 1;
    am_feature.feature_param.peq_param.phase_id      = 0;
    am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
    am_feature.feature_param.peq_param.enable        = attach_packet->a2dp_post_peq_enable;
    am_feature.feature_param.peq_param.sound_mode    = attach_packet->a2dp_post_peq_sound_mode;
    am_feature.feature_param.peq_param.phase_id      = 1;
    am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
}
#endif
}

#ifdef SUPPORT_PEQ_NVKEY_UPDATE
static void aws_mce_ami_receive_attch_nvdm_packet(bt_aws_mce_report_info_t *info)
{
    AMI_AWS_MCE_PACKET_HDR_t *ami_packet_hdr = (AMI_AWS_MCE_PACKET_HDR_t *)info->param;
    AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *attach_nvdm_packet = (AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *)((uint8_t *)info->param + sizeof(AMI_AWS_MCE_PACKET_HDR_t));
    if (ami_packet_hdr->SubPktId == 0) {
        if (ami_attach_nvdm_ctrl.buffer != NULL) {
            vPortFree(ami_attach_nvdm_ctrl.buffer);
        }
        ami_attach_nvdm_ctrl.buffer = pvPortMalloc(ami_packet_hdr->numSubPkt * BT_AWS_MCE_MAX_DATA_LENGTH);
        ami_attach_nvdm_ctrl.buffer_offset = 0;
        ami_attach_nvdm_ctrl.total_pkt = ami_packet_hdr->numSubPkt;
        ami_attach_nvdm_ctrl.pre_pkt = -1;
    }

    if((ami_packet_hdr->SubPktId == (ami_attach_nvdm_ctrl.pre_pkt + 1)) && (ami_packet_hdr->numSubPkt == ami_attach_nvdm_ctrl.total_pkt) && (ami_attach_nvdm_ctrl.buffer != NULL))
    {
        configASSERT(info->param_len > sizeof(AMI_AWS_MCE_PACKET_HDR_t));
        uint16_t temp_size = info->param_len - sizeof(AMI_AWS_MCE_PACKET_HDR_t);
        memcpy(ami_attach_nvdm_ctrl.buffer + ami_attach_nvdm_ctrl.buffer_offset, (uint8_t *)attach_nvdm_packet, temp_size);
        ami_attach_nvdm_ctrl.buffer_offset += temp_size;
        ami_attach_nvdm_ctrl.pre_pkt = ami_packet_hdr->SubPktId;
        audio_src_srv_report("[mce_ami] receive attach nvdm, sub_pkt_id:%d/%d size:%d",3,ami_packet_hdr->SubPktId,ami_packet_hdr->numSubPkt,temp_size);
    }
    else
    {
        audio_src_srv_err("[mce_ami] packet header is wrong, (%d/%d), (%d/%d), buffer:0x%08x offset:%d\n",6, ami_packet_hdr->SubPktId, ami_packet_hdr->numSubPkt, ami_attach_nvdm_ctrl.pre_pkt, ami_attach_nvdm_ctrl.total_pkt, ami_attach_nvdm_ctrl.buffer, ami_attach_nvdm_ctrl.buffer_offset);
        return;
    }

    if(((ami_attach_nvdm_ctrl.pre_pkt + 1) == ami_attach_nvdm_ctrl.total_pkt) && (ami_attach_nvdm_ctrl.buffer != NULL))/*means last pkt*/
    {
        uint32_t total_size;
        uint8_t *packet;
        uint32_t have_changed;
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        attach_nvdm_packet = (AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *)ami_attach_nvdm_ctrl.buffer;
        have_changed = aud_peq_save_changed_nvkey(attach_nvdm_packet, (uint32_t)ami_attach_nvdm_ctrl.buffer_offset);
        vPortFree(ami_attach_nvdm_ctrl.buffer);
        ami_attach_nvdm_ctrl.buffer = NULL;
        ami_attach_nvdm_ctrl.buffer_offset = 0;
        ami_attach_nvdm_ctrl.total_pkt = 0;
        ami_attach_nvdm_ctrl.pre_pkt = 0;

        if (role == BT_AWS_MCE_ROLE_PARTNER) {
            aud_peq_get_changed_nvkey(&packet, &total_size);
            if ((packet != NULL) && (total_size > 0)) {
                bt_sink_srv_aws_mce_ami_data(AMI_EVENT_ATTACH_NVDM_INFO, (uint8_t *)packet, total_size, false);
            }
            if (packet != NULL) {
                vPortFree(packet);
            }
        } else if ((role == BT_AWS_MCE_ROLE_AGENT) && (have_changed == 1)) {
            bt_sink_srv_am_feature_t am_feature;
            uint8_t a2dp_peq_status[4];
            aud_peq_get_sound_mode(A2DP, &a2dp_peq_status[0]);
            memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
            am_feature.type_mask                             = AM_A2DP_PEQ;
            am_feature.feature_param.peq_param.enable        = a2dp_peq_status[0];
            am_feature.feature_param.peq_param.sound_mode    = a2dp_peq_status[1];
            am_feature.feature_param.peq_param.setting_mode  = PEQ_DIRECT;
            am_feature.feature_param.peq_param.not_clear_sysram = 1;
            am_feature.feature_param.peq_param.phase_id      = 0;
            am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
        }
    }
}
#endif

void bt_aws_mce_report_peq_callback(bt_aws_mce_report_info_t *info)  /*R-note: Partner peq. */
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    //RACE_LOG_MSGID_I("PEQ AWS MCE Report: 0x%x 0x%x 0x%x 0x%x 0x%x\n",5,info->module_id,info->is_sync,info->sync_time,info->param_len,info->param);
    if ((info->module_id == BT_AWS_MCE_REPORT_MODULE_PEQ) && (info->param != NULL)) {
        AMI_AWS_MCE_PACKET_HDR_t *ami_pkt_header = (AMI_AWS_MCE_PACKET_HDR_t*)info->param;
        if ((ami_pkt_header->ami_event == AMI_EVENT_PEQ_REALTIME) || (ami_pkt_header->ami_event == AMI_EVENT_PEQ_CHANGE_MODE)) {
            #ifdef MTK_PEQ_ENABLE
            if (role != BT_AWS_MCE_ROLE_AGENT) {
                race_dsprt_peq_collect_data(info);
            }
            #endif
        } else if (ami_pkt_header->ami_event == AMI_EVENT_ATTACH_INFO) {
            if (role != BT_AWS_MCE_ROLE_AGENT) {
                if(info->param_len == sizeof(AMI_AWS_MCE_PACKET_HDR_t) + sizeof(AMI_AWS_MCE_ATTACH_PACKT_t)) {
                    aws_mce_ami_receive_attch_packet((AMI_AWS_MCE_ATTACH_PACKT_t *)((uint8_t *)info->param + sizeof(AMI_AWS_MCE_PACKET_HDR_t)));
                }
            }
        } else if (ami_pkt_header->ami_event == AMI_EVENT_ATTACH_NVDM_INFO) {
            #ifdef SUPPORT_PEQ_NVKEY_UPDATE
            aws_mce_ami_receive_attch_nvdm_packet(info);
            #endif
        }
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_aws_mce_ami_data
 * DESCRIPTION
 *  For BT_SINK_SRV_MODULE_INTERNAL_AMI, to fragment data to multiple IF packet if data size is larger than 200 bytes, and send with bt_sink_srv_send_aws_mce_packet(.).
 * PARAMETERS
 *  ami_event     [IN]
 *  buffer           [IN]
 *  length          [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_aws_mce_ami_data(ami_event_t ami_event, uint8_t *buffer, uint32_t length, bool urgent) /* R-note: IF Patcket. */
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_info_t info;
    AMI_AWS_MCE_PACKET_HDR_t *ami_packet;
    bt_status_t aws_ret;
    uint32_t header_size = sizeof(bt_aws_mce_report_sync_payload_header_t) + sizeof(AMI_AWS_MCE_PACKET_HDR_t);
    uint32_t num_pkt = (length/(BT_AWS_MCE_MAX_DATA_LENGTH-header_size)) + 1;
    uint32_t cur_pkt;
    uint32_t size;

    if ((buffer == NULL) || (length == 0) || (bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_NONE))
    {
        audio_src_srv_report("bt_sink_srv_aws_mce_ami_data invalid param\n", 0);
        return AUD_EXECUTION_FAIL;
    }

    for(cur_pkt = 0; cur_pkt < num_pkt; cur_pkt++)
    {
        size = ((header_size+length) < BT_AWS_MCE_MAX_DATA_LENGTH) ? (header_size+length) : BT_AWS_MCE_MAX_DATA_LENGTH;
        size -= sizeof(bt_aws_mce_report_sync_payload_header_t);

        ami_packet = (AMI_AWS_MCE_PACKET_HDR_t *)pvPortMalloc(size);
        if(!ami_packet) {
            audio_src_srv_report("mce_ami malloc failed\n", 0);
            return AUD_EXECUTION_FAIL;
        }

        ami_packet->ami_event = ami_event;
        ami_packet->numSubPkt = num_pkt;
        ami_packet->SubPktId  = cur_pkt;

        size -= sizeof(AMI_AWS_MCE_PACKET_HDR_t);
        memcpy((uint8_t *)ami_packet + sizeof(AMI_AWS_MCE_PACKET_HDR_t), buffer, size);
        buffer += size;
        length -= size;

        info.module_id = ami_event_to_report_module_id(ami_event);
        info.param_len = size + sizeof(AMI_AWS_MCE_PACKET_HDR_t);
        info.param = (void *)ami_packet;

        if(urgent){
            aws_ret = bt_aws_mce_report_send_urgent_event(&info);
        }else{
            aws_ret = bt_aws_mce_report_send_event(&info);
        }

        if(aws_ret != BT_STATUS_SUCCESS) {
            audio_src_srv_report("Send AMI aws mce data FAIL \n", 0);
        } else {
            audio_src_srv_report("Send AMI aws mce data SUCCESS \n", 0);
        }
        race_mem_free(ami_packet);
    }
    return aws_ret;
#else
    return AUD_EXECUTION_SUCCESS;
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_event_ami_callback
 * DESCRIPTION
 *  To receive MODULE_MASK_AWS_MCE event.
 * PARAMETERS
 *  msg             [IN]
 *  status          [IN]
 *  buffer          [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_event_ami_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#ifdef MTK_AWS_MCE_ENABLE
    int32_t ret = BT_STATUS_SUCCESS;

    switch (msg) {
        case  BT_AWS_MCE_STATE_CHANGED_IND: {
            bt_aws_mce_state_change_ind_t *state_change = (bt_aws_mce_state_change_ind_t *)buffer;
            bt_sink_srv_aws_mce_ami_state_change_handler(state_change);
            break;
        }
        default:
            break;
    }

    return ret;
#else
    return 0;
#endif
}

bt_sink_srv_am_result_t ami_audio_power_off_flow(void)
{
    /* save status into nvdm */
    #ifdef MTK_ANC_ENABLE
    //anc_save_misc_param();
    #endif
    #if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    aud_peq_save_misc_param();
    #endif

    return AUD_EXECUTION_SUCCESS;
}

#ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE

#include "audio_src_srv.h"

#ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_MAX_MODE_NUM
#define MAX_MODE_NUM MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_MAX_MODE_NUM
#else
#define MAX_MODE_NUM  16
#endif

typedef struct {
    uint32_t (*p_table)[2];
    uint32_t table_num;
    bool is_mode_set;
} vendor_volume_table_t;

vendor_volume_table_t s_vendor_volume_table[VOL_TOTAL][MAX_MODE_NUM];
uint32_t s_vendor_volume_mode[VOL_TOTAL];

bt_sink_srv_am_result_t ami_register_stream_out_volume_table(vol_type_t type, int mode, const uint32_t table[][2], size_t table_num)
{
    if((type >= VOL_TOTAL) || (mode >= MAX_MODE_NUM)) {
        audio_src_srv_report("[VENDOR_VOLUME]error type or mode, type = %d, mode = %d  \n", 2, type, mode);
        return AUD_EXECUTION_FAIL;
    }
    audio_src_srv_report("[VENDOR_VOLUME]ami_register_stream_out_volume_table: type = %d, mode = %d, table = %x, table_num = %d  \n", 4, type, mode, *table, table_num);
    s_vendor_volume_table[type][mode].p_table = table;
    s_vendor_volume_table[type][mode].table_num = table_num;
    s_vendor_volume_table[type][mode].is_mode_set = true;

    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_switch_stream_out_volume_table(vol_type_t type, int mode)
{
    if((type >= VOL_TOTAL) || (mode >= MAX_MODE_NUM)) {
        audio_src_srv_report("[VENDOR_VOLUME]error type or mode, type = %d, mode = %d  \n", 2, type, mode);
        return AUD_EXECUTION_FAIL;
    }
    if(false == s_vendor_volume_table[type][mode].is_mode_set)
    {
        audio_src_srv_report("[VENDOR_VOLUME]mode not registerd, type = %d, mode = %d  \n", 2, type, mode);
        return AUD_EXECUTION_FAIL;
    }
    audio_src_srv_report("[VENDOR_VOLUME]ami_switch_stream_out_volume_table: type = %d, mode = %d  \n", 2, type, mode);
    s_vendor_volume_mode[type]  = mode;

    bt_sink_srv_am_type_t current_scenario = bt_sink_srv_ami_get_current_scenario();
    if((current_scenario == A2DP) || (current_scenario == HFP) || (current_scenario == LINE_IN))
    {
        audio_src_srv_report("[VENDOR_VOLUME]current_scenario is: %d", 1, current_scenario);
        //set volume directly if current running device is A2DP or HFP
        bt_sink_srv_am_id_t aud_id = g_prCurrent_player->aud_id;
        bt_sink_srv_am_volume_level_t volume_level_stream_out = g_prCurrent_player->audio_stream_out.audio_volume;
        bt_sink_srv_am_volume_level_t volume_level_stream_in = g_prCurrent_player->audio_stream_in.audio_volume;
        bt_sink_srv_ami_audio_set_volume(aud_id, volume_level_stream_out, STREAM_OUT);
        if (current_scenario == HFP) {
            bt_sink_srv_ami_audio_set_volume(aud_id, volume_level_stream_in, STREAM_IN);
        }
    }

    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_get_stream_out_volume(vol_type_t type, uint32_t level, uint32_t *digital_gain, uint32_t *analog_gain)
{
    if(type >= VOL_TOTAL) {
        audio_src_srv_report("[VENDOR_VOLUME]error type, type = %d  \n", 1, type);
        return AUD_EXECUTION_FAIL;
    }
    uint32_t (*p_table)[2] = s_vendor_volume_table[type][s_vendor_volume_mode[type]].p_table;

    if(p_table == NULL) {
        audio_src_srv_report("[VENDOR_VOLUME]no volume table is switched before use, type = %d  \n", 1, type);
        return AUD_EXECUTION_FAIL;
    }

    uint32_t total_level =s_vendor_volume_table[type][s_vendor_volume_mode[type]].table_num;

    if(level >= total_level) {
        audio_src_srv_report("[VENDOR_VOLUME]volume level exceed the total level, use the max level,  level = %d, max_level = %d  \n", 2, level, total_level - 1);
        level = total_level -1;
        //return AUD_EXECUTION_FAIL;
    }

    *digital_gain = ((uint32_t *)(p_table + level))[0];
    *analog_gain = ((uint32_t *)(p_table + level))[1];

    audio_src_srv_report("[VENDOR_VOLUME]ami_get_stream_out_volume: type = %d, level = %d, digital_gain = 0x%08x, analog_gain = 0x%08x  \n", 4, type, level, *digital_gain, *analog_gain);

    return AUD_EXECUTION_SUCCESS;
}

#endif

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#define VENDOR_SE_USER_MAX   5
vendor_se_context_t s_vendor_se_context[VENDOR_SE_USER_MAX];

am_vendor_se_id_t ami_get_vendor_se_id(void)
{
    uint32_t int_mask;
    hal_nvic_save_and_set_interrupt_mask(&int_mask);

    for(uint32_t i = 0; i < VENDOR_SE_USER_MAX; i++) {
        if(s_vendor_se_context[i].is_used == false) {
            s_vendor_se_context[i].is_used = true;
            for(uint32_t j = 0; j < AM_VENDOR_SE_MAX; j++) {
                s_vendor_se_context[i].is_vendor_se_set[j] = false;
            }
            hal_nvic_restore_interrupt_mask(int_mask);
            return (am_vendor_se_id_t)i;
        }
    }
    hal_nvic_restore_interrupt_mask(int_mask);
    audio_src_srv_report("[VENDOR_SE]ami_get_vendor_se_id: no extra id for use, maxmum user is %d  \n", 1, (int32_t)VENDOR_SE_USER_MAX);
    return (am_vendor_se_id_t)-1;
}

bt_sink_srv_am_result_t ami_register_vendor_se(am_vendor_se_id_t id, am_vendor_se_callback_t callback)
{
    if((id == -1) || (s_vendor_se_context[id].is_used == false)) {
        audio_src_srv_report("[VENDOR_SE]ami_register_vendor_se[%d]: wrong id, please call ami_get_vendor_se_id to get a valid id first   \n", 1, (int32_t)id);
        return AUD_EXECUTION_FAIL;
    }
    audio_src_srv_report("[VENDOR_SE]ami_register_vendor_se[%d]: callback = 0x%08x  \n", 2, (int32_t)id, (int32_t)callback);
    s_vendor_se_context[id].am_vendor_se_callback = callback;
    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_set_vendor_se(am_vendor_se_id_t id, void *arg)
{
    uint32_t i = 0;
    uint32_t int_mask;

    if((id == -1) || (s_vendor_se_context[id].is_used == false)) {
        audio_src_srv_report("[VENDOR_SE]ami_set_vendor_se[%d]: wrong id, please call ami_get_vendor_se_id to get a valid id first   \n", 1, (int32_t)id);
        return AUD_EXECUTION_FAIL;
    }
    if(arg == NULL) {
        audio_src_srv_report("[VENDOR_SE]ami_set_vendor_se[%d]: arg is NULL!  \n", 1, (int32_t)id);
        return AUD_EXECUTION_FAIL;
    }
    hal_nvic_save_and_set_interrupt_mask(&int_mask);
    for(i = 0; i < AM_VENDOR_SE_MAX; i++) {
        if(!s_vendor_se_context[id].is_vendor_se_set[i]) {
            s_vendor_se_context[id].vendor_se_arg[i] = arg;
            s_vendor_se_context[id].is_vendor_se_set[i] = true;
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(int_mask);
    if(i == AM_VENDOR_SE_MAX) {
        audio_src_srv_report("[VENDOR_SE]ami_set_vendor_se[%d]: no extra se \n", 1, (int32_t)id);
        return AUD_EXECUTION_FAIL;
    }

    audio_src_srv_report("[VENDOR_SE]ami_set_vendor_se[%d]: arg[%d] address = 0x%08x,  \n", 3, (int32_t)id, (int32_t)i, (int32_t)arg);
    if(arg != NULL) {
        audio_src_srv_report("[VENDOR_SE]ami_set_vendor_se[%d]: arg[%d] content = 0x%08x,  \n", 3, (int32_t)id, (int32_t)i, *(uint32_t *)arg);
    }

    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_AUDIO_SET_VENDOR_SE, NULL,
                                 FALSE, NULL);
    return AUD_EXECUTION_SUCCESS;
}

bt_sink_srv_am_result_t ami_execute_vendor_se(vendor_se_event_t event)
{
    audio_src_srv_report("[VENDOR_SE]ami_execute_vendor_se: event = %d  \n", 1, (int32_t)event);

    if(event == EVENT_SET_VENDOREFFECT) {
        for(uint32_t i = 0; i < VENDOR_SE_USER_MAX; i++) {
            if((s_vendor_se_context[i].is_used == true) && (s_vendor_se_context[i].am_vendor_se_callback != NULL)) {
                for(uint32_t j = 0; j < AM_VENDOR_SE_MAX; j++) {
                    if(s_vendor_se_context[i].is_vendor_se_set[j] == true) {
                        audio_src_srv_report("[VENDOR_SE]ami_execute_vendor_se: id = %d, se = %d \n", 2, (int32_t)i, (int32_t)j);
                        s_vendor_se_context[i].am_vendor_se_callback(EVENT_SET_VENDOREFFECT, s_vendor_se_context[i].vendor_se_arg[j]);
                        s_vendor_se_context[i].is_vendor_se_set[j] = false;
                        break;
                    }
                }
            }
        }
    } else {
        for(uint32_t i = 0; i < VENDOR_SE_USER_MAX; i++) {
            if((s_vendor_se_context[i].is_used == true) && (s_vendor_se_context[i].am_vendor_se_callback != NULL)) {
                audio_src_srv_report("[VENDOR_SE]ami_execute_vendor_se: id = %d  \n", 1, (int32_t)i);
                s_vendor_se_context[i].am_vendor_se_callback(event, NULL);
            }
        }
    }
    return AUD_EXECUTION_SUCCESS;
}

#endif

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
#ifndef MTK_LEAKAGE_DETECTION_ENABLE
typedef void (*anc_leakage_compensation_callback_t)(uint16_t leakage_status);
void anc_leakage_detection_racecmd_callback(uint16_t leakage_status)
{
}
void anc_leakage_detection_racecmd_response(uint16_t leakage_status, uint8_t fromPartner)
{
}
void audio_anc_leakage_compensation_set_status(bool status)
{
}
bool audio_anc_leakage_compensation_get_status(void)
{
return 0;
}
void audio_anc_leakage_compensation_terminate(void)
{
}
void audio_anc_leakage_compensation_start(anc_leakage_compensation_callback_t callback)
{
}
#endif

log_create_module(anc, PRINT_LEVEL_INFO);

#if !defined(MTK_DEBUG_LEVEL_NONE)
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_0[]  = LOG_DEBUG_PREFIX(anc) "anc_mutex_creat error\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_1[]  = LOG_DEBUG_PREFIX(anc) "anc_mutex_take error\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_2[]  = LOG_DEBUG_PREFIX(anc) "anc_mutex_give error\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_3[]  = LOG_DEBUG_PREFIX(anc) "[Convert feature]Unvalid filter type\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_4[]  = LOG_DEBUG_PREFIX(anc) "[Convert setting]Unvalid filter type\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_5[]  = LOG_DEBUG_PREFIX(anc) "Customized ANC callback - event:%d result:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_6[]  = LOG_DEBUG_PREFIX(anc) "anc_set_dvfs_control not pair, g_anc_debug:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_7[]  = LOG_DEBUG_PREFIX(anc) "anc_off and waiting async ack (anc_stop_delay_timer_callback)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_8[]  = LOG_DEBUG_PREFIX(anc) "[ANC Sync] enter anc timer callback event %d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_9[]  = LOG_DEBUG_PREFIX(anc) "[ANC Sync] sync time may wrong(>500ms): %d ms\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_10[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] start anc one-shot timer with duration %d ms\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_11[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] change timer period failed\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_12[] = LOG_DEBUG_PREFIX(anc) "[ANC sync] fail to send aws mce report\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_13[] = LOG_DEBUG_PREFIX(anc) "[ANC dirt] fail to send aws mce report\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_14[] = LOG_DEBUG_PREFIX(anc) "anc_sync_control ignore event:%d because HFP happening.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_15[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] is special link case, agent delay %d ms to apply and tell partner to apply directly\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_16[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] wait timer to be available : %d ms !\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_17[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] timer is unavailable too long !\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_18[] = LOG_DEBUG_PREFIX(anc) "AWS MCE Report: 0x%x 0x%x 0x%x\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_19[] = LOG_DEBUG_PREFIX(anc) "[ANC]Write ANC misc param error\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_20[] = LOG_DEBUG_PREFIX(anc) "[ANC]Backup ANC status 0x%x\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_21[] = LOG_DEBUG_PREFIX(anc) "[ANC]It's under MP calibration, don't backup ANC status\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_22[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_23[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_24[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_calibrate_gain Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_25[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_calibrate_gain size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_26[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_calibrate_gain is failed to read param - err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_27[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_set_calibrate_gain is failed to write param - err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_28[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_filter_coef Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_29[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_filter_coef size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_30[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_filter_coef (%d)(0x%x)(0x%x)(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_31[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_filter_coef failed to read 0x%x- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_32[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_param Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_33[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_param size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_34[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_param is failed to read 0x%x- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_35[] = LOG_DEBUG_PREFIX(anc) "frequency is risen to MAX V.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_36[] = LOG_DEBUG_PREFIX(anc) "frequency is set back %d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_37[] = LOG_DEBUG_PREFIX(anc) "[ANC Sync] Partner target_clk:0x%x, 0x%x, duration:%d us \n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_38[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_pwr_det_param Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_39[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_pwr_det_param size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_40[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_pwr_det_param is failed to read param- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_41[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_pwr_det_param (%d)(%d)(0x%x)(%d)(%d)(0x%x)(0x%x)(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_42[] = LOG_DEBUG_PREFIX(anc) "anc_callback_service %d cb:0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_43[] = LOG_DEBUG_PREFIX(anc) "[anc_copy_filter] pvPortMallocNC fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_44[] = LOG_DEBUG_PREFIX(anc) "[anc_copy_filter] is failed to anc_get_filter_coef - err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_45[] = LOG_DEBUG_PREFIX(anc) "[anc_copy_filter](0x%x) pvPortMallocNC fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_46[] = LOG_DEBUG_PREFIX(anc) "anc_copy_filter, result: %d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_47[] = LOG_DEBUG_PREFIX(anc) "[anc_realtime_update_param] set_reg_mask(%d) ch(%d) value(0x%x)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_48[] = LOG_DEBUG_PREFIX(anc) "anc_realtime_update_param reg mask(%d) error\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_49[] = LOG_DEBUG_PREFIX(anc) "[anc_realtime_update](0x%x) pvPortMallocNC fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_50[] = LOG_DEBUG_PREFIX(anc) "anc_realtime_update, result: %d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_51[] = LOG_DEBUG_PREFIX(anc) "anc on with flash_no: %d, filter type: %d, 0x%x, %d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_52[] = LOG_DEBUG_PREFIX(anc) "anc on with copy filter %d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_53[] = LOG_DEBUG_PREFIX(anc) "[anc_on] pvPortMallocNC fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_54[] = LOG_DEBUG_PREFIX(anc) "[anc_on] is failed to anc_get_filter_coef - err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_55[] = LOG_DEBUG_PREFIX(anc) "[anc_on] pANC_PASSTHRU_param pvPortMallocNC fail\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_56[] = LOG_DEBUG_PREFIX(anc) "[anc_on] anc_get_param error(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_57[] = LOG_DEBUG_PREFIX(anc) "anc_on, result: %d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_58[] = LOG_DEBUG_PREFIX(anc) "receive anc ack(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_59[] = LOG_DEBUG_PREFIX(anc) "anc_off and waiting async ack\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_60[] = LOG_DEBUG_PREFIX(anc) "[ANC] audio_anc_init() finish\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_61[] = LOG_DEBUG_PREFIX(anc) "[anc_set_param] set_reg_mask(0x%x)(0x%x) value(0x%x)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_62[] = LOG_DEBUG_PREFIX(anc) "[anc_set_param] set debug mode\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_63[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_set_param reg mask(%d) error\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_64[] = LOG_DEBUG_PREFIX(anc) "get_hybrid_capability %x %d %d \r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_65[] = LOG_DEBUG_PREFIX(anc) "ANC_SET_RUNTIME_VOL gain:%d, type:%d, cur_type:%d, cur_id:%d, EN(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_66[] = LOG_DEBUG_PREFIX(anc) "[ANC][Init]anc failed to get misc param.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_67[] = LOG_DEBUG_PREFIX(anc) "anc failed to register aws mce report callback\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_68[] = LOG_DEBUG_PREFIX(anc) "audio_anc_init: create timer failed\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_69[] = LOG_DEBUG_PREFIX(anc) "[ANC] create g_anc_stop_dly_timer failed\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_70[] = LOG_DEBUG_PREFIX(anc) "[ANC][API]audio_anc_enable_by_type type(%d) error.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_71[] = LOG_DEBUG_PREFIX(anc) "audio_anc_read_calibrate_gain_param ERROR!! Gain(0x%x) Filter(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_72[] = LOG_DEBUG_PREFIX(anc) "audio_anc_read_calibrate_gain_param Filter(0x%x) ERROR!!";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_73[] = LOG_DEBUG_PREFIX(anc) "audio_anc_write_calibrate_gain_param Filter(0x%x) ERROR!!";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_74[] = LOG_DEBUG_PREFIX(anc) "[ANC][API]audio_anc_enable type(%d) error.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_75[] = LOG_DEBUG_PREFIX(anc) "anc failed to reinit 0x%x param- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_76[] = LOG_DEBUG_PREFIX(anc) "audio_anc_control ignore event:%d because HFP happening.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_77[] = LOG_DEBUG_PREFIX(anc) "[ANC]audio_anc_control with event:%d flash_id(%d) type(%d)(0x%x)(%d)(0x%x)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_78[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]read nvkey mask[0]:0x%x, mask[1]:0x%x, mask[2]:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_79[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]read nvkey id 0x%x fail, status:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_80[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC][CALLBACK]HAL_AUDIO_EVENT_ERROR\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_81[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC][CALLBACK]read stream in failed\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_82[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]AM_CB, aud_id:%x, msg_id:%x, sub_msg:%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_83[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]AM_CB, AUD_SUSPEND_BY_HFP";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_84[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]AM_CB, AUD_SUSPEND_BY_IND";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_85[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]Leakage compensation start.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_86[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]g_reocrd_id %x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_87[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]after ccni, status:%d, w_set:%d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_88[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]TYPE_ENABLE input parameter error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_89[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]TYPE_ENABLE. cmd_syncMode(%d) cmd_ancType(%d), cmd_flash_no(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_90[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]ENABLE input parameter error(3)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_91[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]ENABLE. cmd_syncMode(%d) filter(0x%x) sram_bank_mask(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_92[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]DISABLE input parameter error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_93[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]DISABLE. cmd_syncMode(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_94[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER input parameter error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_95[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER cmd_syncMode(%d) at_copy_type(%d) filter(0x%x) sram_bank(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_96[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER cmd_flash_no none";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_97[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x1) input cmd_no. error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_98[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x1) input parameter. none";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_99[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x1)(%d) sscanf error count(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_100[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x2) input cmd_no. error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_101[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x2) input parameter. none";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_102[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]COPY_FILTER(0x2)(%d) sscanf error count(%d)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_103[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]SET_REG input parameter error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_104[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]SET_REG cmd_syncMode(%d) set_RG_type(%d) cmd_filter(0x%x) value_1(0x%x)";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_105[] = LOG_DEBUG_PREFIX(anc) "[ANC][ATCI]SET value_2,3 error";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_106[] = LOG_DEBUG_PREFIX(anc) "[ANC][AM]Enable User_define with flash_id(%d) type(%d) filter_mask(0x%x) sram_bank_mask(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_107[] = LOG_DEBUG_PREFIX(anc) "[ANC][AM]Enable with flash_id(%d) type(%d) filter_mask(0x%x) sram_bank_mask(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_108[] = LOG_DEBUG_PREFIX(anc) "[ANC][AM]Disable\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_109[] = LOG_DEBUG_PREFIX(anc) "[ANC][AM]Copy filter with  flash_id(%d) type(%d) filter_mask(0x%x) sram_bank(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_110[] = LOG_DEBUG_PREFIX(anc) "[ANC][RACE]RACE_ANC_ON cmd_length(%d) V2_on_param(%d) flash_no(%d) anc_type(%d) syncMode(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_111[] = LOG_DEBUG_PREFIX(anc) "[ANC][RACE]RACE_ANC_OFF cmd_length(%d) V2_off_param(%d) syncMode(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_112[] = LOG_DEBUG_PREFIX(anc) "[ANC][RACE]RACE_ANC_SET_RUNTIME_VOL cmd_length(%d) V2_runtime_vol_param(%d) runtime_vol(0x%x) syncMode(%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_113[] = LOG_DEBUG_PREFIX(anc) "[ANC][RACE]event id(%d) error.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_114[] = LOG_DEBUG_PREFIX(anc) "[ANC][Entry]source(%d) error.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_115[] = LOG_DEBUG_PREFIX(anc) "[ANC]Dummy anc disable, state:%d, enable:%d, filter_id:%d, type:%d suspend_flag:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_116[] = LOG_DEBUG_PREFIX(anc) "[ANC]Dummy anc enable, state:%d, enable:%d, filter_id:%d, type:%d, keep_disable:%d";

ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_117[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_118[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_119[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param is failed to read param- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_120[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_param %x-%x-%x-%x--%x %x-%x-%x-%x--%x";

ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_121[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_2_param Fail. Status:%u Len:%lu\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_122[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_2_param size not match. NVDM tableSize (%lu) != (%d)\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_123[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_2_param is failed to read param- err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_124[] = LOG_DEBUG_PREFIX(anc) "[ANC]anc_get_misc_2_param 0x%x";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_125[] = LOG_DEBUG_PREFIX(anc) "[ANC][Init]ANC Feature_1 enable at non-support enviorment.\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_126[] = LOG_DEBUG_PREFIX(anc) "ANC_RUNTIME_GAIN_CONVERT_RAMP_GAIN ch[%d] runtime_gain(%d) extend(%d)(%d)(%d)(%d)(%d)";

/*for Leakage detection*/
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_200[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC] audio_anc_control ignore event:%d because LD happening.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_201[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC] anc leakage detection start in audio_anc_control";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_202[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC] anc leakage detection stop in audio_anc_control, enabled:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_203[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC] anc leakage detection resume dl";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_204[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC] audio_anc_control ignore event:%d because leakage detection happening.";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_205[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC]audio_anc_control mute music:%d";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_206[] = LOG_DEBUG_PREFIX(anc) "[RECORD_LC][debug]callback:0x%x";

/*for User tirgger adaptive FF ANC*/
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_220[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]read filter_coef 0x%x from nvdm - stat(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_221[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]write filter_coef 0x%x from nvdm - err(%d)\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_222[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff] read/write filter, vPortFree\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_223[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]Wrong filter selection:%d\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_224[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]update keep filter, current_filter_no:%d, filter_select:%d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_225[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]Write Filter 0x%x result:%d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_226[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]restore to factory settings, result:%d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_227[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]Query Status, UT_done = %d, UT_filter_selection = %d\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_228[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]read 0x%x from nvdm fail - err\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_229[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]user_trigger_status_mutex create error\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_230[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]user_trigger_status_mutex take error\r\n";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_231[] = LOG_DEBUG_PREFIX(anc) "[user_trigger_ff]user_trigger_status_mutex give error\r\n";





static log_control_block_t *g_log_control_blocks[] = {
    &LOG_CONTROL_BLOCK_SYMBOL(anc),
};

void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_INFO, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_WARNING, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_ERROR, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(g_log_control_blocks[index], PRINT_LEVEL_DEBUG, message, arg_cnt, ap);
    va_end(ap);
}

void anc_port_log_MP(uint8_t index, uint8_t arg_cnt, uint8_t value1, uint8_t value2, uint8_t value3)
{
    switch(index){
        case 0x1:{
            LOG_W(MPLOG,"Pass-through: ON , cap: %d", value1);
            break;
        }
        case 0x2:{
            LOG_W(MPLOG,"ANC: ON , filter: %d type: %d cap: %d", value1, value2, value3);
            break;
        }
        case 0x3:{
            LOG_W(MPLOG,"ANC/Pass-thru: OFF");
            break;
        }
        default:
            break;
    }
}
#else
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_0[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_1[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_2[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_3[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_4[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_5[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_6[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_7[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_8[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_9[]  = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_10[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_11[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_12[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_13[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_14[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_15[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_16[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_17[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_18[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_19[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_20[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_21[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_22[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_23[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_24[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_25[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_26[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_27[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_28[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_29[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_30[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_31[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_32[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_33[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_34[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_35[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_36[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_37[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_38[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_39[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_40[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_41[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_42[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_43[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_44[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_45[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_46[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_47[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_48[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_49[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_50[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_51[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_52[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_53[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_54[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_55[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_56[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_57[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_58[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_59[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_60[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_61[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_62[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_63[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_64[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_65[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_66[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_67[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_68[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_69[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_70[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_71[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_72[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_73[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_74[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_75[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_76[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_77[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_78[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_79[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_80[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_81[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_82[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_83[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_84[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_85[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_86[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_87[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_88[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_89[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_90[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_91[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_92[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_93[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_94[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_95[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_96[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_97[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_98[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_99[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_100[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_101[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_102[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_103[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_104[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_105[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_106[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_107[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_108[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_109[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_110[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_111[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_112[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_113[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_114[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_115[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_116[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_117[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_118[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_119[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_120[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_121[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_122[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_123[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_124[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_125[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_126[] = "";

ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_200[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_201[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_202[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_203[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_204[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_205[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_206[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_220[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_221[] = "";
ATTR_LOG_STRING_LIB  g_ANC_msg_id_string_222[] = "";




void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...)
{
}
void anc_port_log_MP(uint8_t index, uint8_t arg_cnt, uint8_t value1, uint8_t value2, uint8_t value3)
{
}
#endif
#endif
#endif

#if defined(MTK_AUDIO_TRANSMITTER_ENABLE)
/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_volume
 * DESCRIPTION
 *  Set audio input/output volume.
 * PARAMETERS
 *  aud_id           [IN]
 *  volume_level     [IN]
 *  in_out           [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t ami_audio_set_audio_transmitter_config(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_audio_capability_t *capability_t)
{
/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[Sink][AMI] set audio transmitter config: aud_id:%d",1, aud_id);
    if ((aud_id < AM_REGISTER_ID_TOTAL)
        //&& (g_rAm_aud_id[aud_id].use == ID_PLAY_STATE)
        ) {

        temp_background_t.aud_id = aud_id;
        temp_background_t.type = g_rAm_aud_id[aud_id].contain_ptr->type;
        temp_background_t.local_context = g_rAm_aud_id[aud_id].contain_ptr->local_context;
        //temp_background_t.local_context.audio_transmitter_format.scenario_runtime_config = *config;
        temp_background_t.local_context.audio_transmitter_format.scenario_runtime_config_type = capability_t->codec.audio_transmitter_format.scenario_runtime_config_type;
        temp_background_t.local_context.audio_transmitter_format.scenario_runtime_config = capability_t->codec.audio_transmitter_format. scenario_runtime_config;
        temp_background_t.notify_cb = g_rAm_aud_id[aud_id].contain_ptr->notify_cb;

        bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_TRANS_SET_CONFIG_REQ, &temp_background_t,
                                 FALSE, NULL);
        return AUD_EXECUTION_SUCCESS;
    }
    return AUD_EXECUTION_FAIL;
}
#endif

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE

bt_sink_srv_am_result_t ami_set_voice_mic_type(voice_mic_type_t voice_mic_type) {

/*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if(voice_mic_type == current_voice_mic_type) {
        audio_src_srv_report("[DETACHABLE_MIC][Sink][AMI] ami_set_voice_mic_type: voice_mic_type(%d) unchanged",1, voice_mic_type);
        return AUD_EXECUTION_FAIL;
    }

    audio_src_srv_report("[DETACHABLE_MIC][Sink][AMI] ami_set_voice_mic_type: voice_mic_type(%d)", 1, voice_mic_type);
    current_voice_mic_type = voice_mic_type;
    temp_background_t.local_context.audio_detachable_mic_format.voice_mic_type = voice_mic_type;

    bt_sink_srv_ami_send_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                                 MSG_ID_VOICE_SET_MIC_TYPE_REQ, &temp_background_t,
                                 FALSE, NULL);
    return AUD_EXECUTION_SUCCESS;
}


#endif
#ifdef AIR_BT_AUDIO_SYNC_ENABLE
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_request_sync(bt_sink_srv_am_id_t aud_id,
                                                           bt_sink_srv_am_audio_sync_capability_t *sync_capability)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t temp_background_t = {0};
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    uint32_t current_cnt = 0;
    bool isr_flag = false;
    if (aud_id != FEATURE_NO_NEED_ID) {
        // check feature type
        switch (g_rAm_aud_id[aud_id].contain_ptr->type)
        {
            case A2DP:
                if (sync_capability->sync_scenario_type != MCU2DSP_SYNC_REQUEST_A2DP) {
                    audio_src_srv_report("[Sink][AMI] dsp sync request ami error: type error [%d] [%d]", 2, sync_capability->sync_scenario_type,
                                        g_rAm_aud_id[aud_id].contain_ptr->type);
                    return AUD_EXECUTION_FAIL;
                }
                break;
            case HFP:
                if (sync_capability->sync_scenario_type != MCU2DSP_SYNC_REQUEST_HFP) {
                    audio_src_srv_report("[Sink][AMI] dsp sync request ami error: type error [%d] [%d]", 2, sync_capability->sync_scenario_type,
                                        g_rAm_aud_id[aud_id].contain_ptr->type);
                    return AUD_EXECUTION_FAIL;
                }
                break;
            default:
                audio_src_srv_report("[Sink][AMI] dsp sync request ami error: type not support [%d]", 1, g_rAm_aud_id[aud_id].contain_ptr->type);
                return AUD_EXECUTION_FAIL;
                break;
        }
    }
    temp_background_t.aud_id = aud_id;
    memcpy(&temp_background_t.sync_parm, sync_capability, sizeof(bt_sink_srv_am_audio_sync_capability_t));
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_cnt);
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) {
        isr_flag = false;
    } else {
        isr_flag = true;
    }
#ifndef AMI_DSP_SYNC_DIRECT
    if (!isr_flag) {
        bt_sink_srv_ami_send_to_front_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                MSG_ID_AUDIO_REQUEST_SYNC, &temp_background_t, FALSE, NULL);
    } else {
        bt_sink_srv_ami_send_to_front_amm(MOD_AM, MOD_AMI, AUD_SELF_CMD_REQ,
                            MSG_ID_AUDIO_REQUEST_SYNC, &temp_background_t,
                            FALSE, ptr_callback_amm);
    }

#else
    bt_sink_srv_am_amm_struct amm_param = {0};
    amm_param.cb_msg_id = AUD_SELF_CMD_REQ;
    amm_param.src_mod_id = MOD_AMI;
    amm_param.dest_mod_id = MOD_AM;
    amm_param.msg_id = MSG_ID_AUDIO_REQUEST_SYNC;
    memcpy(&(amm_param.background_info), &temp_background_t, sizeof(bt_sink_srv_am_background_t));
    audio_request_sync_hdlr(&amm_param);
#endif
    audio_src_srv_report("[Sink][AMI] sync request ami info: c_cnt = [%u], aud_id = %d, from_isr = %d", 3, current_cnt, aud_id, isr_flag);
    return AUD_EXECUTION_SUCCESS;
}

void bt_sink_srv_ami_audio_service_hook_callback(audio_message_type_t type, hal_audio_callback_t callback, void *user_data)
{
    audio_src_srv_report("[Sink][AMI] hook audio sevice callback, type=[%d]", 1, type);
    hal_audio_service_hook_callback(type, callback, user_data);
}

void bt_sink_srv_ami_audio_service_unhook_callback(audio_message_type_t type)
{
    audio_src_srv_report("[Sink][AMI] unhook audio sevice callback, type=[%d]", 1, type);
    hal_audio_service_unhook_callback(type);
}

#endif /* AIR_BT_AUDIO_SYNC_ENABLE */


#ifdef AIR_AUDIO_MIXER_GAIN_ENABLE
/*****************************************************************************
 * FUNCTION
 *  bt_sink_srv_ami_audio_set_mixer_volume
 * DESCRIPTION
 *  Set audio mixer volume.
 * PARAMETERS
 *  volume_level     [IN]
 * RETURNS
 *  bt_sink_srv_am_result_t
 *****************************************************************************/
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mixer_volume(bt_sink_srv_am_volume_level_t volume_level)
{
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__

    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t digital_gain, analog_gain;
    bt_sink_srv_am_result_t am_result;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE
    am_result = ami_get_stream_out_volume(VOL_A2DP, volume_level, &digital_gain, &analog_gain);
#else
    bt_sink_srv_audio_setting_vol_t vol;
    bt_sink_srv_audio_setting_vol_info_t vol_info;
    memset(&vol, 0, sizeof(bt_sink_srv_audio_setting_vol_t));
    vol_info.type = VOL_A2DP;
    vol_info.vol_info.a2dp_vol_info.dev = HAL_AUDIO_DEVICE_HEADSET;
    vol_info.vol_info.a2dp_vol_info.lev = volume_level;
    am_result = (bt_sink_srv_am_result_t)bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
    digital_gain = vol.vol.a2dp_vol.vol.digital;
    analog_gain = vol.vol.a2dp_vol.vol.analog_L;
#endif

    if (am_result == AUD_EXECUTION_SUCCESS) {
        hal_audio_set_stream_out_dl3_volume(digital_gain, analog_gain);
        audio_src_srv_report("[Sink][AMI] set mixer volume volume_level:%d, Digital Gain:%d", 2, volume_level, digital_gain);

    } else {
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AMI] mixer_volume error", 0);
        #endif
        return AUD_EXECUTION_FAIL;
    }

    return AUD_EXECUTION_SUCCESS;
#else
    UNUSED(volume_level);
#endif

    return AUD_EXECUTION_FAIL;
}
#endif
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_power_off_dac_immediately(void)
{
    audio_src_srv_report("[Sink][AMI] POWER OFF DAC Immediately", 0);
    // Close DAC NOW!
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_POWER_OFF_DAC_IMMEDIATELY, 0, 0, true);
    return AUD_EXECUTION_SUCCESS;
}

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
const ull_misc_nvkey_t g_ull_misc_nvkey_default =
{
    .uplink_bitrate = 32000,
};

uint32_t audio_ull_get_uplink_bitrate(void)
{
    uint32_t size;
    nvkey_status_t status = NVKEY_STATUS_ERROR;
    ull_misc_nvkey_t ull_misc_nvkey;

    status = nvkey_data_item_length(NVKEYID_APP_ULL_MISC, &size);
    if ((status == NVKEY_STATUS_ITEM_NOT_FOUND) || (size != sizeof(ull_misc_nvkey_t)))
    {
        audio_src_srv_report("[ULL] can not found NVKEY, %d", 1, status);
        status = nvkey_write_data(NVKEYID_APP_ULL_MISC, (const uint8_t *)&g_ull_misc_nvkey_default, sizeof(ull_misc_nvkey_t));
        audio_src_srv_report("[ULL] write default NVKEY, %d", 1, status);
        ull_misc_nvkey.uplink_bitrate = g_ull_misc_nvkey_default.uplink_bitrate;
    }
    else if (status == NVKEY_STATUS_OK)
    {
        status = nvkey_read_data(NVKEYID_APP_ULL_MISC, (uint8_t *)&ull_misc_nvkey, &size);
        audio_src_srv_report("[ULL] read default NVKEY, %d, uplink bitrate = %u", 2, status, ull_misc_nvkey.uplink_bitrate);
    }
    else
    {
        audio_src_srv_report("[ULL] get uplink error, %d", 1, status);
        ull_misc_nvkey.uplink_bitrate = g_ull_misc_nvkey_default.uplink_bitrate;
    }

    return ull_misc_nvkey.uplink_bitrate;
}
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
