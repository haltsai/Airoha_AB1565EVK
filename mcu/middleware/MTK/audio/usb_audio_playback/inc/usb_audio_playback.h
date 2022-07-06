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

#ifndef __USB_AUDIO_PLAYBACK_H__
#define __USB_AUDIO_PLAYBACK_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <hal_platform.h>
#include "hal_audio.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    USB_AUDIO_EXECUTION_FAIL    = -1,
    USB_AUDIO_EXECUTION_SUCCESS =  0,
} usb_audio_result_t;

/** @brief usb_audio playback state. */
typedef enum {
    USB_AUDIO_STATE_ERROR = -1,     /**< An error occurred. */
    USB_AUDIO_STATE_IDLE,           /**< The usb_audio playback is inactive. */
    USB_AUDIO_STATE_READY,          /**< The usb_audio playback is ready to play the media. */
    USB_AUDIO_STATE_PLAY,           /**< The usb_audio playback is in playing state. */
    USB_AUDIO_STATE_STOP,           /**< The usb_audio playback has stopped. */
} usb_audio_playback_state_t;

usb_audio_result_t audio_usb_audio_playback_open(void);
usb_audio_result_t audio_usb_audio_playback_start(void);
usb_audio_result_t audio_usb_audio_playback_stop(void);
usb_audio_result_t audio_usb_audio_playback_close(void);


#ifdef __cplusplus
}
#endif

#endif  /*__USB_AUDIO_PLAYBACK_H__*/
