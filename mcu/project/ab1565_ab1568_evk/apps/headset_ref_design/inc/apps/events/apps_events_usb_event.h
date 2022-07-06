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

/**
 * File: app_usb_audio_utils.h
 *
 * Description: This file defines the common structure and functions of usb audio app.
 */


#ifndef __APP_EVENTS_USB_EVENT_H__
#define __APP_EVENTS_USB_EVENT_H__

#ifdef MTK_USB_DEMO_ENABLED
#include <stdint.h>

typedef uint8_t app_usb_audio_port_t;
#define APP_USB_AUDIO_UNKNOWN_PORT                      0x00     /**< unknown port. */
#define APP_USB_AUDIO_SPK_PORT                          0x01     /**< usb speaker. */
#define APP_USB_AUDIO_MIC_PORT                          0x02     /**< streaming microphone. */

typedef uint8_t app_usb_audio_channel_t;
#define APP_USB_AUDIO_DUAL_CHANNEL                      0x00     /**< usb speaker. */
#define APP_USB_AUDIO_LEFT_CHANNEL                      0x01     /**< usb speaker. */
#define APP_USB_AUDIO_RIGHT_CHANNEL                     0x02     /**< streaming microphone. */

typedef uint8_t app_usb_audio_sample_rate_t;
#define APP_USB_AUDIO_SAMPLE_RATE_UNKNOWN                0x00     /**< Unknown. */
#define APP_USB_AUDIO_SAMPLE_RATE_16K                    0x01     /**< 16KHz. */
#define APP_USB_AUDIO_SAMPLE_RATE_32K                    0x02     /**< 32KHz. */
#define APP_USB_AUDIO_SAMPLE_RATE_44_1K                  0x03     /**< 44.1KHz. */
#define APP_USB_AUDIO_SAMPLE_RATE_48K                    0x04     /**< 48KHz. */

/**
 *  @brief This enumeration defines the action of USB audio.
 */
typedef enum {
    APPS_EVENTS_USB_AUDIO_UNPLUG,        /**<  Notify app usb device unplug. Parameter is NULL */
    APPS_EVENTS_USB_AUDIO_PLAY,          /**<  Notify app to play USB audio. Parameter please ref#app_events_usb_port_t */
    APPS_EVENTS_USB_AUDIO_STOP,          /**<  Notify app to stop USB audio. Parameter please ref#app_events_usb_port_t */
    APPS_EVENTS_USB_AUDIO_VOLUME,        /**<  Notify app to set the volume of USB audio. Parameter please ref#app_events_usb_volume_t */
    APPS_EVENTS_USB_AUDIO_MUTE,          /**<  Notify app to mute the USB audio. Parameter please ref#app_events_usb_mute_t */
    APPS_EVENTS_USB_AUDIO_SAMPLE_RATE,   /**<  Notify app the sample rate of the USB audio. Parameter please ref#app_events_usb_sample_rate_t */
    APPS_EVENTS_USB_AUDIO_SUSPEND,       /**<  Notify app the USB is suspend. */
    APPS_EVENTS_USB_AUDIO_RESUME,        /**<  Notify app the USB is resume. */
} app_usb_audio_event_t;

typedef struct {
    app_usb_audio_port_t port_type;         /* spk/mic */
    uint8_t port_num;                       /* range:0 ~ 1; 0:chat,1:gaming */
} app_events_usb_port_t;

typedef struct {
    app_usb_audio_port_t port_type;         /* spk/mic */
    uint8_t port_num;                       /* range:0 ~ 1; 0:chat,1:gaming */
    app_usb_audio_channel_t audio_channel;  /* left/right */
    uint8_t volume;                         /* 0 ~ 100 */
} app_events_usb_volume_t;

typedef struct {
    app_usb_audio_port_t port_type;         /* spk/mic */
    uint8_t port_num;                       /* range:0 ~ 1; 0:chat,1:gaming */
    uint8_t is_mute;                        /* 0:unmute; 1:mute */
} app_events_usb_mute_t;

typedef struct {
    app_usb_audio_port_t port_type;         /* spk/mic */
    uint8_t port_num;                       /* range:0 ~ 1; 0:chat,1:gaming */
    app_usb_audio_sample_rate_t rate;
} app_events_usb_sample_rate_t;

/**
 * @brief      Initialize the usb event, register callback.
 */
void apps_event_usb_event_init(void);

#endif /* MTK_USB_DEMO_ENABLED */

#endif /* __APP_USB_AUDIO_UTILS_H__ */
    