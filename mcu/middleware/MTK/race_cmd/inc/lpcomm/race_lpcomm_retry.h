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


#ifndef __RACE_LPCOMM_RETRY_H__
#define __RACE_LPCOMM_RETRY_H__

#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_RETRY_ENABLE
#include "race_lpcomm_trans.h"


/* In some systems of the remote devices, when they are in the background mode, their timers may not work if the expiration time exceeds 9s. */
#define RACE_LPCOMM_RETRY_TIMEOUT_IN_MS              (3000)
#define RACE_LPCOMM_RETRY_MAX_TIME                   (2)


typedef struct race_lpcomm_retry_list
{
    struct race_lpcomm_retry_list *next;
    void *data;
    race_lpcomm_trans_method_enum method;
    uint8_t device_id;
    uint32_t curr_time_in_ms;
    bool is_used;
    uint8_t retry_count;
    uint8_t max_retry_time;
}race_lpcomm_retry_list_struct;



bool race_lpcomm_retry_init(void);

void race_lpcomm_retry_deinit(void);

race_lpcomm_retry_list_struct *race_lpcomm_retry_list_node_alloc(void);

void race_lpcomm_retry_list_node_free(void *node);

RACE_ERRCODE race_lpcomm_retry_list_insert(race_lpcomm_retry_list_struct *list_node);

RACE_ERRCODE race_lpcomm_retry_list_remove(race_lpcomm_retry_list_struct *list_node);

RACE_ERRCODE race_lpcomm_retry_cancel(uint8_t app_id, bool gen_fake_rsp);

race_lpcomm_retry_list_struct *race_lpcomm_retry_list_node_find_by_process_id(uint16_t process_id);

void race_lpcomm_retry_timer_expiration_process(bool *timer_in_use);

RACE_ERRCODE race_lpcomm_retry_drop_packet(race_lpcomm_retry_list_struct *list_node);

#endif /* RACE_LPCOMM_RETRY_ENABLE */

#endif /* __RACE_LPCOMM_RETRY_H__ */

