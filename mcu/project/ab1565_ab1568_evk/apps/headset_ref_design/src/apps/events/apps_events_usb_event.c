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
 * File: app_usb_audio_idle_utils.c
 *
 * Description:
 * This file define the common function of usb audio.
 */

#include "apps_events_usb_event.h"
#include "apps_events_event_group.h"
#include "usb.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_debug.h"

#define USB_EVENT_LOG_I(msg, ...)     APPS_LOG_MSGID_I("[USB_EVENT]"msg, ##__VA_ARGS__)
#define USB_EVENT_LOG_E(msg, ...)     APPS_LOG_MSGID_E("[USB_EVENT]"msg, ##__VA_ARGS__)
#define USB_EVENT_LOG_D(msg, ...)     APPS_LOG_MSGID_D("[USB_EVENT]"msg, ##__VA_ARGS__)



static void app_usb_suspend_callback(void)
{
    ui_shell_send_event(true, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_USB_AUDIO,
                        APPS_EVENTS_USB_AUDIO_SUSPEND,
                        NULL, 0, NULL, 0);
}

static void app_usb_resume_callback(void)
{
    ui_shell_send_event(true, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_USB_AUDIO,
                        APPS_EVENTS_USB_AUDIO_RESUME,
                        NULL, 0, NULL, 0);
}

void apps_event_usb_event_init()
{
    USB_Register_Suspend_Callback(app_usb_suspend_callback);
    USB_Register_Resume_Callback(app_usb_resume_callback);
    USB_EVENT_LOG_I("apps_event_usb_event_init done.", 0);
}

