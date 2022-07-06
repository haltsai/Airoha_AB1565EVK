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


#ifdef MTK_USB_DEMO_ENABLED

/* C library */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "syslog.h"

/* Middleware includes */
#include "usb_case.h"

/* Syslog create module for usb_case.c */
log_create_module_variant(USB_CASE, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

bool g_usb_atci_reg = false;
USB_ATCI_FUNC g_atci_callback;
USB_RACE_FUNC g_race_callback;
USB_LOGS_FUNC g_logs_callback;

void usb_case_register_atci_callback(USB_ATCI_FUNC atci_callback)
{
    LOG_MSGID_I(USB_CASE, "usb_case_register_atci_callback", 0);

    g_atci_callback = atci_callback;

    g_usb_atci_reg = true;
}

void usb_case_atci_call(bool usb_plug_in)
{
    LOG_MSGID_I(USB_CASE, "usb_case_atci_call[%d]", 1, usb_plug_in);

    if(g_usb_atci_reg == true) {
       g_atci_callback((void *) usb_plug_in);
    }
}

void usb_case_register_race_callback(USB_RACE_FUNC race_callback)
{
    LOG_MSGID_I(USB_CASE, "usb_case_register_race_callback", 0);

    g_race_callback = race_callback;
}

void usb_case_race_call(bool usb_plug_in)
{
    LOG_MSGID_I(USB_CASE, "usb_case_race_call[%d]", 1, usb_plug_in);

    if(g_race_callback != NULL) {
       g_race_callback((void *) usb_plug_in);
    }
}

void usb_case_register_logs_callback(USB_LOGS_FUNC logs_callback)
{
    LOG_MSGID_I(USB_CASE, "usb_case_register_logs_callback", 0);

    g_logs_callback = logs_callback;
}

void usb_case_logs_call(bool usb_plug_in)
{
    LOG_MSGID_I(USB_CASE, "usb_case_logs_call[%d]", 1, usb_plug_in);

    if(g_logs_callback != NULL) {
       g_logs_callback((void *) usb_plug_in);
    }
}

#endif
