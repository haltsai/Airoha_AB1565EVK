/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#include "bt_sink_srv_ami.h"
#include "audio_sink_srv_line_in.h"
#include "audio_sink_srv_line_in_internal.h"
#include "audio_src_srv.h"
#include "math.h"

audio_sink_srv_line_in_context_t g_audio_sink_srv_cntx;

void audio_sink_srv_line_in_control_init(void)
{
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();
    audio_sink_srv_line_in_device_t  *line_in_dev = NULL;
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        mcu2dsp_open_stream_in_param_t stream_in_param;
        mcu2dsp_open_stream_out_param_t stream_out_param;
    #endif

    if (ctx->line_in_aid > 0) {
        audio_src_srv_report("[sink][Line-in]control_init-error.", 0);
        return;
    }

    memset(ctx, 0x00, sizeof(audio_sink_srv_line_in_context_t));
    ctx->state = AUDIO_SRC_SRV_STATE_NONE;
    ctx->line_in_aid = AUDIO_SINK_SRV_INVALID_AID;
    ctx->vol_lev     = AUDIO_SINK_SRV_LINE_IN_VOL_DEF_LEV;

    /* Construct Line-IN pseudo handle */
    audio_sink_srv_line_in_create_handle();

    /******************************************************************************/
    /*Line-IN pseudo handle callback*/
    line_in_dev = audio_sink_srv_line_in_get_device();
    if(!line_in_dev) {
        audio_src_srv_report("[sink][Line-in]control_init-dev error.", 0);
    } else {
        #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            hal_audio_get_stream_in_setting_config(AU_DSP_LINEIN,&stream_in_param);
            hal_audio_get_stream_out_setting_config(AU_DSP_LINEIN,&stream_out_param);
        #endif
        line_in_dev->handle = (audio_src_srv_handle_t *)audio_sink_srv_line_in_alloc_pseudo_handle();
        line_in_dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN;
        line_in_dev->handle->dev_id = 0x0;
        line_in_dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_MIDDLE;
        #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            line_in_dev->codec.codec_cap.in_audio_device    = stream_in_param.afe.audio_device;
            line_in_dev->codec.codec_cap.out_audio_device   = stream_out_param.afe.audio_device;
        #else
            line_in_dev->codec.codec_cap.in_audio_device    = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
            line_in_dev->codec.codec_cap.out_audio_device   = HAL_AUDIO_DEVICE_DAC_DUAL;
        #endif
        line_in_dev->codec.codec_cap.linein_sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
    }

    bt_sink_srv_assert(line_in_dev &&"Error: Line-IN dev NULL");
    AUDIO_SINK_SRV_SET_FLAG(line_in_dev->flag, AUDIO_SINK_SRV_FLAG_LINE_IN_INIT);

    //audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_READY, NULL);
    /******************************************************************************/

    /* Construct Line-IN AMI handle */
    audio_sink_srv_line_in_init();

    //audio_src_srv_report("[sink][Line-in]init-EVT_START: 0x%x", 1, AUDIO_SINK_SRV_LINE_IN_EVT_START);
}

void audio_sink_srv_line_in_control_deinit(void)
{
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();
    audio_sink_srv_line_in_device_t  *line_in_dev = NULL;

    if (ctx->line_in_aid < 0) {
        audio_src_srv_report("[sink][Line-in]control_deinit-error.", 0);
        return;
    }

    /******************************************************************************/
    /*Line-IN pseudo handle callback*/
    line_in_dev = audio_sink_srv_line_in_get_device();
    if(!line_in_dev) {
        audio_src_srv_report("[sink][Line-in]control_deinit-dev error.", 0);
    } else {
        audio_sink_srv_line_in_state_machine_handle(line_in_dev, AUDIO_SINK_SRV_LINE_IN_EVT_UNAVAILABLE, NULL);
        audio_sink_srv_line_in_free_pseudo_handle(line_in_dev->handle);
    }
    /******************************************************************************/

    /* Dis-Construct Line-IN AMI handle */
    audio_sink_srv_line_in_deinit();

    /* Dis-Construct Line-IN pseudo handle */
    audio_sink_srv_line_in_destroy_handle();

    //audio_src_srv_report("[sink][Line-in]init-EVT_START: 0x%x", 1, AUDIO_SINK_SRV_LINE_IN_EVT_START);
}

audio_sink_srv_line_in_context_t *audio_sink_srv_line_in_get_context(void)
{
    return &g_audio_sink_srv_cntx;
}


audio_sink_srv_line_in_device_t *audio_sink_srv_line_in_get_device(void)
{
    audio_sink_srv_line_in_context_t *ctx = NULL;
    ctx = audio_sink_srv_line_in_get_context();

    return &ctx->sink_dev;
}

void audio_sink_srv_line_in_control_drv_play(void *param)
{
    audio_sink_srv_line_in_device_t *dev = (audio_sink_srv_line_in_device_t *)param;
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == dev->handle->type) {
        audio_sink_srv_line_in_drv_play(param);
    } else {
        audio_src_srv_report("[sink][Line-IN]drv_play(error)--type: %d", 1, dev->handle->type);
    }
}


void audio_sink_srv_line_in_control_drv_stop(void *param)
{
    audio_sink_srv_line_in_device_t *dev = (audio_sink_srv_line_in_device_t *)param;
    if (AUDIO_SRC_SRV_PSEUDO_DEVICE_LINE_IN == dev->handle->type) {
        audio_sink_srv_line_in_drv_stop(param);
    } else {
        audio_src_srv_report("[sink][Line-IN]drv_stop(error)--type: %d", 1, dev->handle->type);
    }
}

void audio_sink_srv_line_in_fill_audio_src_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_assert(handle);
    handle->play             = audio_sink_srv_line_in_play_handle;
    handle->stop             = audio_sink_srv_line_in_stop_handle;
    handle->suspend          = audio_sink_srv_line_in_suspend_handle;
    handle->reject           = audio_sink_srv_line_in_reject_handle;
    handle->exception_handle = audio_sink_srv_line_in_exception_handle;
}

void audio_sink_srv_line_in_fill_am_aud_param(bt_sink_srv_am_audio_capability_t  *aud_cap, audio_line_in_codec_capability_t *line_in_cap)
{
    audio_sink_srv_line_in_context_t *ctx = audio_sink_srv_line_in_get_context();

    memset(aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
    /************************************************************/
    if (NULL != aud_cap) {
        aud_cap->type = LINE_IN;
        memcpy(&(aud_cap->codec.line_in_format.line_in_codec.codec_cap), line_in_cap, sizeof(audio_line_in_codec_capability_t));
        aud_cap->audio_stream_in.audio_device  = AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE;
        aud_cap->audio_stream_in.audio_volume  = AUD_VOL_IN_LEVEL0;
        aud_cap->audio_stream_out.audio_device = AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE;
        aud_cap->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)(ctx->vol_lev);
        audio_src_srv_report("agent open codec init vol is %d", 1, aud_cap->audio_stream_out.audio_volume);
        aud_cap->audio_stream_out.audio_mute = false;
    }
    /************************************************************/
}

audio_sink_srv_line_in_status_t audio_sink_srv_line_in_set_param(audio_sink_srv_am_line_in_codec_t *line_in_cap)
{
    audio_sink_srv_line_in_device_t  *line_in_dev = NULL;
    line_in_dev = audio_sink_srv_line_in_get_device();
    if((line_in_dev != NULL) && (line_in_cap != NULL)){
        line_in_dev->codec.codec_cap.in_audio_device    = line_in_cap->codec_cap.in_audio_device;
        line_in_dev->codec.codec_cap.out_audio_device   = line_in_cap->codec_cap.out_audio_device;
        line_in_dev->codec.codec_cap.linein_sample_rate = line_in_cap->codec_cap.linein_sample_rate;
        return AUDIO_SINK_SRV_LINE_IN_STATUS_SUCCESS;
    } else {
        audio_src_srv_report("[sink][Line-IN]set_param(error)", 0);
        return AUDIO_SINK_SRV_LINE_IN_STATUS_FAIL;
    }
}

