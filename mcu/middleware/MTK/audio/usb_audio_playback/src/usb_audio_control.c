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
#include "usb_audio_device.h"
#include "usb_audio_control_internal.h"
#include "usb_audio_control.h"
#include "usb_audio_playback.h"
#include "usbaudio_drv.h"

extern uint8_t AUD_USB_AUDIO_VOL_OUT_MAX;
extern uint8_t AUD_USB_AUDIO_VOL_OUT_DEFAULT;

#define USB_AUDIO_VOLUME_SETTING_DELAY_MS  100

static uint32_t        s_new_volume = 0;
static TimerHandle_t   s_xUsbAudioOneShotTimer = NULL;
static usb_audio_context_t s_usb_audio_ctx;

usb_audio_context_t *usb_audio_get_ctx(void)
{
    return &s_usb_audio_ctx;
}

static usb_audio_device_state_t usb_audio_device_status = USB_AUDIO_DEVICE_STATE_IDLE;

void usb_audio_control_ami_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]ami callback, msg_id = %d, sub_msg = %d\r\n", 2, msg_id, sub_msg);
    usb_audio_device_t *dev = usb_audio_get_dev();
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    switch (msg_id) {
        case AUD_SELF_CMD_REQ: {
            if (AUD_CMD_FAILURE == sub_msg) {
                ctx->callback(AUDIO_USB_AUDIO_EVENT_ERROR, ctx->user_data);
            } else if (AUD_CMD_COMPLETE == sub_msg) {
                ctx->callback(AUDIO_USB_AUDIO_EVENT_STOPED, ctx->user_data);
            }
            break;
        }
        case AUD_SINK_OPEN_CODEC: {
            audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PLAYING);
            ctx->callback(AUDIO_USB_AUDIO_EVENT_PLAYING, ctx->user_data);
        }
    }
}

audio_usb_audio_status_t audio_usb_audio_control_init(audio_usb_audio_config_t *config)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]init\r\n", 0);

    if (usb_audio_device_status != USB_AUDIO_DEVICE_STATE_IDLE) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]init fail\r\n", 0);
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    usb_audio_device_t *dev = usb_audio_get_dev();

    memset(ctx, 0, sizeof(usb_audio_context_t));
    memset(dev, 0, sizeof(usb_audio_device_t));

    /* Construct handle and assign each member function */
    dev->handle = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_USB_AUDIO);
    if (NULL == dev->handle) {
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }
    dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_USB_AUDIO;
    dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_MIDDLE;
    dev->handle->dev_id = 0;
    dev->handle->play = usb_audio_device_play_handle;
    dev->handle->stop = usb_audio_device_stop_handle;
    dev->handle->suspend = usb_audio_device_suspend_handle;
    dev->handle->reject = usb_audio_device_reject_handle;
    dev->handle->exception_handle = usb_audio_device_exception_handle;
    dev->audio_volume = (s_new_volume == 0) ? AUD_USB_AUDIO_VOL_OUT_DEFAULT : s_new_volume;
    dev->audio_mute = false;
    dev->codec.codec_cap.usb_audio_sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
    dev->codec.codec_cap.in_audio_device = HAL_AUDIO_DEVICE_USBAUDIOPLAYBACK_DUAL;
    dev->codec.codec_cap.out_audio_device = HAL_AUDIO_DEVICE_DAC_DUAL;

    /* Allocate a AID */
    ctx->aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, usb_audio_control_ami_callback);
    if (AUD_ID_INVALID == ctx->aid) {
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }

    ctx->callback = config->callback;
    ctx->user_data = config->user_data;

    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);

    usb_audio_device_status = USB_AUDIO_DEVICE_STATE_READY;

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}

audio_usb_audio_status_t audio_usb_audio_control_deinit(void)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]deinit\r\n", 0);
    if (usb_audio_device_status != USB_AUDIO_DEVICE_STATE_STOP) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]deinit fail\r\n", 0);
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    usb_audio_device_t *dev = usb_audio_get_dev();

    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);

    audio_src_srv_destruct_handle(dev->handle);
    if (AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_close(ctx->aid)) {
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }

    usb_audio_device_status = USB_AUDIO_DEVICE_STATE_IDLE;

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}


audio_usb_audio_status_t audio_usb_audio_control_start(void)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]start\r\n", 0);

    hal_audio_set_dvfs_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_AUDIO_DVFS_LOCK);

    if (usb_audio_device_status != USB_AUDIO_DEVICE_STATE_READY) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]start fail\r\n", 0);
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }
    usb_audio_device_t *dev = usb_audio_get_dev();

    const audio_src_srv_handle_t *running_handle = audio_src_srv_get_runing_pseudo_device();

    if (running_handle != NULL) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]running_handle type = %d, state = 0x%x\r\n", 2, running_handle->type, running_handle->state);
    } else {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]no running handle", 0);
    }

    usb_audio_device_status = USB_AUDIO_DEVICE_STATE_PLAY;

    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}

audio_usb_audio_status_t audio_usb_audio_control_stop(void)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]stop\r\n", 0);
    if (usb_audio_device_status != USB_AUDIO_DEVICE_STATE_PLAY) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]stop fail\r\n", 0);
        return AUDIO_USB_AUDIO_STATUS_ERROR;
    }
    usb_audio_device_t *dev = usb_audio_get_dev();
    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
    usb_audio_device_status = USB_AUDIO_DEVICE_STATE_STOP;

    hal_audio_set_dvfs_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_AUDIO_DVFS_UNLOCK);

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}


static void prvUsbAudioOneShotTimerCallback(TimerHandle_t xTimer)
{
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    usb_audio_device_t *dev = usb_audio_get_dev();

    uint32_t volume_ratio = 100 / (AUD_USB_AUDIO_VOL_OUT_MAX + 1);

    s_new_volume /= volume_ratio;

    if (dev->audio_volume != s_new_volume) {
        dev->audio_volume = s_new_volume;
        bt_sink_srv_ami_audio_set_volume(ctx->aid, s_new_volume, STREAM_OUT);
    }

    if (xTimerDelete(s_xUsbAudioOneShotTimer, 0) != pdPASS) {
        audio_src_srv_report("[USB_PLAYBACK_AM_CTL]Timer Delete error.\r\n", 0);
    } else {
        s_xUsbAudioOneShotTimer = NULL;
    }
}

audio_usb_audio_status_t audio_usb_audio_control_set_volume(uint32_t volume)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]audio_usb_audio_control_set_volume: %d\r\n", 1, volume);
    if (s_xUsbAudioOneShotTimer == NULL) {
        s_xUsbAudioOneShotTimer = xTimerCreate("UsbAudioOneShot",
                                               USB_AUDIO_VOLUME_SETTING_DELAY_MS / portTICK_PERIOD_MS,
                                               pdFALSE,
                                               0,
                                               prvUsbAudioOneShotTimerCallback);
        if (s_xUsbAudioOneShotTimer == NULL) {
            audio_src_srv_report("[USB_PLAYBACK_AM_CTL]create one_shot Timer error.\n", 0);
            return AUDIO_USB_AUDIO_STATUS_ERROR;
        } else {
            if (xTimerStart(s_xUsbAudioOneShotTimer, 0) != pdPASS) {
                audio_src_srv_report("[USB_PLAYBACK_AM_CTL]Timer start error.\r\n", 0);
                return AUDIO_USB_AUDIO_STATUS_ERROR;
            }
        }
    } else {
        if (xTimerReset(s_xUsbAudioOneShotTimer, 0) != pdPASS) {
            audio_src_srv_report("[USB_PLAYBACK_AM_CTL]Timer reset error.\r\n", 0);
            return AUDIO_USB_AUDIO_STATUS_ERROR;
        }
    }

    s_new_volume = volume;

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}

audio_usb_audio_status_t audio_usb_audio_control_mute(void)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]audio_usb_audio_control_mute %d\r\n", 0);
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    bt_sink_srv_ami_audio_set_mute(ctx->aid, 1, STREAM_OUT);

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}

audio_usb_audio_status_t audio_usb_audio_control_unmute(void)
{
    audio_src_srv_report("[USB_PLAYBACK_AM_CTL]audio_usb_audio_control_unmute %d\r\n", 0);
    usb_audio_context_t *ctx = usb_audio_get_ctx();
    bt_sink_srv_ami_audio_set_mute(ctx->aid, 0, STREAM_OUT);

    return AUDIO_USB_AUDIO_STATUS_SUCCESS;
}
