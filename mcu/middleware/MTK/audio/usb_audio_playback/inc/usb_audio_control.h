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

#ifndef __USB_AUDIO_CONTROL_H__
#define __USB_AUDIO_CONTROL_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hal_platform.h>
#include "hal_audio.h"
#include "bt_sink_srv_ami.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_USB_AUDIO_EVENT_ERROR = -1,
    AUDIO_USB_AUDIO_EVENT_PLAYING = 0,
    AUDIO_USB_AUDIO_EVENT_STOPED = 1,
    AUDIO_USB_AUDIO_EVENT_SUSPENDED = 2,
    AUDIO_USB_AUDIO_EVENT_REJECTED = 3,
    AUDIO_USB_AUDIO_EVENT_EXCEPTION = 4
} audio_usb_audio_event_t;

typedef enum {
    AUDIO_USB_AUDIO_STATUS_ERROR = -1,
    AUDIO_USB_AUDIO_STATUS_SUCCESS = 0
} audio_usb_audio_status_t;

typedef void (*audio_usb_audio_callback_t)(audio_usb_audio_event_t event, void *user_data);

typedef struct {
    audio_usb_audio_callback_t callback;
    void *user_data;
} audio_usb_audio_config_t;

audio_usb_audio_status_t audio_usb_audio_control_init(audio_usb_audio_config_t *config);
audio_usb_audio_status_t audio_usb_audio_control_deinit(void);
audio_usb_audio_status_t audio_usb_audio_control_start(void);
audio_usb_audio_status_t audio_usb_audio_control_stop(void);
audio_usb_audio_status_t audio_usb_audio_control_set_volume(uint32_t volume);
//////////audio_usb_audio_status_t audio_usb_audio_control_set_sample_rate(uint32_t volume);
audio_usb_audio_status_t audio_usb_audio_control_mute(void);
audio_usb_audio_status_t audio_usb_audio_control_unmute(void);


#ifdef __cplusplus
}
#endif

#endif  /*__USB_AUDIO_CONTROL_H__*/
