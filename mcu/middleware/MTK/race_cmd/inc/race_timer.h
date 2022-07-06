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


#ifndef __RACE_TIMER_H__
#define __RACE_TIMER_H__


#include "race_cmd_feature.h"
#include "race_cmd.h"


#define RACE_TIMER_TIMEOUT_DELTA_IN_MS               (50)


#define RACE_FOTA_SP_MAX_RETRY_TIME                   (3)
#define RACE_FOTA_SP_RETRY_INTERVAL                   (9000)
#define RACE_TIMER_ROLE_SWITCH_TIMEOUT_IN_MS         (9000)
#define RACE_TIMER_ROLE_SWITCH_AGENT_TIMEOUT_IN_MS   (RACE_TIMER_ROLE_SWITCH_TIMEOUT_IN_MS)
#define RACE_TIMER_ROLE_SWITCH_PARTNER_TIMEOUT_IN_MS (RACE_TIMER_ROLE_SWITCH_TIMEOUT_IN_MS)  /* (SPP/BLE max retry interval + RHO expected max time)s */

#if RACE_LPCOMM_RETRY_MAX_TIME
#define RACE_TIMER_FOTA_COMMIT_DELAY_IN_MS           (RACE_LPCOMM_RETRY_TIMEOUT_IN_MS + 1000)
#elif RACE_FOTA_SP_RETRY_INTERVAL
#define RACE_TIMER_FOTA_COMMIT_DELAY_IN_MS           (RACE_FOTA_SP_RETRY_INTERVAL + 1000)
#else
#define RACE_TIMER_FOTA_COMMIT_DELAY_IN_MS           (0)
#endif

#if RACE_LPCOMM_RETRY_MAX_TIME
#define RACE_TIMER_CFU_CLEAR_CACHED_INFO_DELAY_IN_MS    ((RACE_LPCOMM_RETRY_MAX_TIME + 1) * RACE_LPCOMM_RETRY_TIMEOUT_IN_MS + 1000)
#else
#define RACE_TIMER_CFU_CLEAR_CACHED_INFO_DELAY_IN_MS    (10000)
#endif

#define RACE_TIMER_INVALID_TIMER_ID (0)


typedef void (*race_timer_expiration_hdl)(uint8_t id, void *user_data);

typedef struct race_timer_list
{
    struct race_timer_list *next;
    bool is_used;
    uint32_t timestamp;
    uint32_t timeout;
    race_timer_expiration_hdl hdl;
    void *user_data;
}race_timer_list_struct;


RACE_ERRCODE race_timer_start(uint32_t delay_msec);

/* This kind of timer should be used within race task include the timer handler especially when user_data is used. */
RACE_ERRCODE race_timer_smart_start(uint8_t *id,
                                         uint32_t timeout,
                                         race_timer_expiration_hdl hdl,
                                         void *user_data);

RACE_ERRCODE race_timer_smart_reset(uint8_t id);

bool race_timer_smart_is_enabled(uint8_t id);

RACE_ERRCODE race_timer_smart_change_period(uint8_t id,
                                                      uint32_t timeout);

/* This function stops the timer by its timer id and return user_data set when use race_timer_smart_start() */
void race_timer_smart_stop(uint8_t id, void **user_data);

void race_timer_expiration_msg_process(void);

#endif /* __RACE_TIMER_H__ */

