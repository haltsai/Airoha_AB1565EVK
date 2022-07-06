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

#ifndef __APPS_CUSTOMER_CONFIG_H__
#define __APPS_CUSTOMER_CONFIG_H__

#include "multi_va_manager.h"
/***********************************************************************************************
********************** Defines the feature switch and parameters in APPs ***********************
***********************************************************************************************/
#define APPS_BATTERY_LOW_THRESHOLD                      (10) /* Low battery status if battery percentage < the value */
#define APPS_BATTERY_FULL_THRESHOLD                     (100) /* Battery full status if battery percentage >= the value */
#define APPS_DIFFERENCE_BATTERY_VALUE_FOR_RHO           (30) /* When the battery percentage of agent add the value < the battery value of partner, will trigger RHO */

#define APPS_AIR_PAIRING_INFO    { \
        0x41, 0x69, 0x72, 0x6f, 0x68, 0x61, \
        0x41, 0x42, 0x41, 0x45, 0x45, 0x78, \
        0x49, 0x4E, 0x46, 0x4F \
}   /* Earbuds air pairing faled if air pairing informatin is not matched, every manufacturer should define pairing information */

#define APPS_AIR_PAIRING_KEY     { \
        0x01, 0x05, 0x05, 0x00, 0x0A, 0x0B, 0x0C, 0x0D, \
        0x0E, 0x0F, 0x11, 0x22, 0xA2, 0xBC, 0x32, 0x49  \
}   /* Key for air pairing, every manufacturer should define a different key */
#define APPS_AIR_PAIRING_DURATION    (30)    /* The duration of do air pairing (seconds). */


#define APPS_AUTO_SET_BT_DISCOVERABLE        (0) /* When disconnected from smart phone or power on BT, enable BT discoverable automatically */


#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#define APPS_TIMEOUT_OF_SLEEP_AFTER_NO_CONNECTION   (5 * 60 * 1000)     /* The waiting time (ms) before sleep */
#define APPS_TIMEOUT_OF_SLEEP_OF_SILENCE_DETECT     (20 * 60 * 1000)    /* The waiting time (ms) before sleep after silence detected. */


#define APPS_POWER_SAVING_NONE                          (0)
#define APPS_POWER_SAVING_DISABLE_BT                    (1)
#define APPS_POWER_SAVING_SYSTEM_OFF                    (2)

/* The basic power saving mode */
#define APPS_POWER_SAVING_MODE                          (2) /* == APPS_POWER_SAVING_SYSTEM_OFF */
/* The POWER_SAVING_MODE if ANC/PassThrough is enabled. It must be <= APPS_POWER_SAVING_MODE */
#define APPS_POWER_SAVING_WHEN_ANC_PASSTHROUGH_ENABLED  (0) /* == APPS_POWER_SAVING_NONE */
#endif

#define APPS_DEFAULT_VA_TYPE                    MULTI_VA_TYPE_UNKNOWN

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_app_common.h"
/* The retry count of the ULL connection in single link mode. The value must be in the enum bt_app_common_ull_stream_retry_count_t */
#define APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK          (BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_4)
#endif
/************************************************************************************************/

#endif /* __APPS_CUSTOMER_CONFIG_H__ */
