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

#include "bt_sink_srv_ami.h"
#include "audio_src_srv.h"
#include "audio_log.h"
#include "usb_audio_control_internal.h"
#include "usb_audio_control.h"
#include "usb_audio_device.h"


static usb_audio_device_t s_usb_audio_dev;

usb_audio_device_t *usb_audio_get_dev(void)
{
    return &s_usb_audio_dev;
}

void usb_audio_device_play_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[USB_AUDIO_DEVICE]play handle", 0);

    usb_audio_context_t *ctx = usb_audio_get_ctx();
    usb_audio_device_t *dev = usb_audio_get_dev();
    ctx->codec_cap.type = USB_AUDIO_IN;
    memcpy(&(ctx->codec_cap.codec.usb_audio_format.usb_audio_codec), &(dev->codec), sizeof(audio_sink_srv_am_usb_audio_codec_t));
    ctx->codec_cap.audio_stream_in.audio_device = dev->codec.codec_cap.in_audio_device;
    ctx->codec_cap.audio_stream_in.audio_volume = AUD_VOL_IN_LEVEL0;
    ctx->codec_cap.audio_stream_out.audio_device = dev->codec.codec_cap.out_audio_device;
    ctx->codec_cap.audio_stream_out.audio_volume = dev->audio_volume;
    ctx->codec_cap.audio_stream_out.audio_mute = dev->audio_mute;
    bt_sink_srv_am_result_t am_ret =  bt_sink_srv_ami_audio_play(ctx->aid, &ctx->codec_cap);
    if (AUD_EXECUTION_SUCCESS != am_ret) {
        audio_src_srv_report("[USB_AUDIO_DEVICE]play(fail)", 0);
        audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    }
}

void usb_audio_device_stop_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[USB_AUDIO_DEVICE]stop handle", 0);
    usb_audio_context_t *ctx = usb_audio_get_ctx();

    bt_sink_srv_am_result_t am_ret = bt_sink_srv_ami_audio_stop(ctx->aid);

    if (AUD_EXECUTION_SUCCESS != am_ret) {
        audio_src_srv_report("[USB_AUDIO_DEVICE]stop(fail)", 0);
    }
}

void usb_audio_device_suspend_handle(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    audio_src_srv_report("[USB_AUDIO_DEVICE]suspend handle", 0);
    audio_src_srv_report("[USB_AUDIO_DEVICE]suspended by %d", 1, int_hd->type);
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    usb_audio_device_t *dev = usb_audio_get_dev();

    if (ctx->user_data != NULL) {
        *(uint32_t *)(ctx->user_data) = int_hd->type;
    }

    ctx->callback(AUDIO_USB_AUDIO_EVENT_SUSPENDED, ctx->user_data);

    bt_sink_srv_am_result_t am_ret = bt_sink_srv_ami_audio_stop(ctx->aid);

    if (AUD_EXECUTION_SUCCESS != am_ret) {
        audio_src_srv_report("[USB_AUDIO_DEVICE]suspend(fail)", 0);
    }

    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);

    audio_src_srv_add_waiting_list(handle);
}

void usb_audio_device_reject_handle(audio_src_srv_handle_t *handle)
{
    audio_src_srv_report("[USB_AUDIO_DEVICE]reject handle", 0);
    const audio_src_srv_handle_t *rej_handle = audio_src_srv_get_runing_pseudo_device();
    usb_audio_context_t *ctx = usb_audio_get_ctx();

    if (ctx->user_data != NULL) {
        *((uint32_t *)ctx->user_data) = rej_handle->type;
    }
    ctx->callback(AUDIO_USB_AUDIO_EVENT_REJECTED, ctx->user_data);
    audio_src_srv_report("[USB_AUDIO_DEVICE]rejected by %d", 1, rej_handle->type);
}

void usb_audio_device_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    audio_src_srv_report("[USB_AUDIO_DEVICE]exception handle", 0);

    ctx->callback(AUDIO_USB_AUDIO_EVENT_EXCEPTION, ctx->user_data);
}

