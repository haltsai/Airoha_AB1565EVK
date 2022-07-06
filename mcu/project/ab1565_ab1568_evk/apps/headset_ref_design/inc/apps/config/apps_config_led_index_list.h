/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#ifndef __APPS_CONFIG_APPS_CONFIG_LED_INDEX_LIST_H__
#define __APPS_CONFIG_APPS_CONFIG_LED_INDEX_LIST_H__

enum {
    LED_INDEX_IDLE                              = 0,
    LED_INDEX_CONNECTABLE                       = 1,
    LED_INDEX_HOLD_CALL                         = 2,
    LED_INDEX_INCOMING_CALL                     = 3,
    LED_INDEX_OUTGOING_CALL                     = 4,
    LED_INDEX_FIND_ME                           = 5,
    LED_INDEX_CALL_ACTIVE                       = 6,
    LED_INDEX_CHARGING                          = 7,
    LED_INDEX_CHARGING_FULL                     = 8,
    LED_INDEX_LOW_BATTERY                       = 9,
    LED_INDEX_CHARGING_ERROR                    = 10,
    LED_INDEX_DISCONNECTED                      = 11,
    //FG patterns
    LED_INDEX_FOTA_START                        = 12,
    LED_INDEX_FOTA_CANCELLED                    = 13,
    LED_INDEX_POWER_ON                          = 14,
    LED_INDEX_POWER_OFF                         = 15,
    LED_INDEX_AIR_PAIRING                       = 16,
    LED_INDEX_AIR_PAIRING_SUCCESS               = 17,
    LED_INDEX_AIR_PAIRING_FAIL                  = 18,
    LED_INDEX_TRIGGER_RHO                       = 19,
};

#endif /* __APPS_CONFIG_APPS_CONFIG_LED_INDEX_LIST_H__ */
