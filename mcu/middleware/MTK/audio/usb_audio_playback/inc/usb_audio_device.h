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

#ifndef __USB_AUDIO_DEVICE_H__
#define __USB_AUDIO_DEVICE_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hal_platform.h>
#include "hal_audio.h"
#include "audio_src_srv.h"
#include "usb_audio_control.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    audio_src_srv_handle_t *handle;
    uint32_t   audio_volume;
    bool       audio_mute;
    audio_sink_srv_am_usb_audio_codec_t codec;
} usb_audio_device_t;


typedef enum {
    USB_AUDIO_DEVICE_STATE_ERROR = -1,
    USB_AUDIO_DEVICE_STATE_IDLE,
    USB_AUDIO_DEVICE_STATE_READY,
    USB_AUDIO_DEVICE_STATE_PLAY,
    USB_AUDIO_DEVICE_STATE_STOP,
} usb_audio_device_state_t;

usb_audio_device_t *usb_audio_get_dev(void);

void usb_audio_device_play_handle(audio_src_srv_handle_t *handle);
void usb_audio_device_stop_handle(audio_src_srv_handle_t *handle);
void usb_audio_device_suspend_handle(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);
void usb_audio_device_reject_handle(audio_src_srv_handle_t *handle);
void usb_audio_device_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param);


#ifdef __cplusplus
}
#endif

#endif  /*__USB_AUDIO_DEVICE_H__*/
